from typing import Any, Union

import pygame
from datetime import datetime

from slot import Slot, StatusManager
from lights import render_lights, LightOptions

import paho.mqtt.client as mqtt

FPS_LIMIT = 60
SLOT_COUNT = 4

# if mosquitto is running on the raspi then we can just use localhost
MQTT_HOST = 'localhost'

def interpolate(start: float, end: float, t: float) -> float:
    return start + (end - start) * t

def launch():
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2) # pyright: ignore[reportPrivateImportUsage]
    try:
        pygame.init()
        pygame.font.init()
        surface = pygame.display.set_mode((1280, 720), pygame.FULLSCREEN | pygame.SCALED)
        clock = pygame.time.Clock()
        running = True
        start_time = int(datetime.now().timestamp() * 1000)
        time_elapsed = lambda: int(datetime.now().timestamp() * 1000) - start_time
        
        something_changed = True
        light_opts = LightOptions(time_elapsed)
        
        def on_connect(client: mqtt.Client, userdata: Any, flags: mqtt.ConnectFlags, reason_code: mqtt.ReasonCode, props: Union[mqtt.Properties, None]): # pyright: ignore[reportPrivateImportUsage]
            print(f"connected with result code {reason_code}")
            client.subscribe('display/slots/+')
            client.subscribe('display/selected')
            client.subscribe('display/lights')
            client.subscribe('display/lights/+')
            
        def on_message(client: mqtt.Client, userdata: Any, message: mqtt.MQTTMessage):
            nonlocal something_changed
            if len(message.payload) == 0:
                return
            print(message.topic, message.payload)
            if message.topic == 'display/selected':
                for s in slots:
                    # 0 means no slot selected
                    # 1-4 means the respective slot is selected
                    s.set_selected(int(message.payload))
            elif message.topic.startswith('display/slots/'):
                for i in range(len(slots)):
                    # slot index starts from 1
                    # 0 means no shape (blank)
                    # 1-9 refer to the respective shape
                    if message.topic == f'display/slots/{i+1}':
                        slots[i].spin_to(int(message.payload))
            elif message.topic == 'display/lights':
                light_opts.active = int(message.payload) == 1
            elif message.topic.startswith('display/lights/'):
                topic = message.topic[15:]
                light_opts.set(topic, message.payload)
            something_changed = True
        
        mqttc.on_connect = on_connect
        mqttc.on_message = on_message
        
        mqttc.connect_async(MQTT_HOST, 1883, 60)
        mqttc.loop_start()
        
        connected = mqttc.is_connected()
        
        w = surface.width
        
        manager = StatusManager(SLOT_COUNT, mqttc)
        slots = [Slot(time_elapsed, surface, w // SLOT_COUNT, i + 1, w * i // SLOT_COUNT, manager) for i in range(SLOT_COUNT)]
        
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                    
            # only re render if there are changes:
            has_changes = False
            
            # 1. connection status changed
            #    update status indicator
            if mqttc.is_connected() != connected:
                connected = mqttc.is_connected()
                has_changes = True
            
            # 2. wheel(s) are active
            #    render the wheels spinning
            has_changes = has_changes or manager.status()
            
            # 3. something from MQTT changed
            has_changes = has_changes or something_changed
            
            # 4. rgb lights are active
            #    render the rgb lights or hide them with 1 extra render
            has_changes = has_changes or (light_opts.active and not light_opts.is_static())
                
            if has_changes:
                # mqtt is running on a separate thread (i think) so this needs to be updated early
                # in case mqtt sets it back to true while this render is happening
                something_changed = False
                    
                surface.fill((244, 244, 244))
                        
                for slot in slots:
                    slot.render()
                
                if light_opts.active:
                    lights = render_lights(w, surface.height // 10, (244, 244, 244), light_opts)
                    surface.blit(lights, (0,0))
                
                # connection indicator
                pygame.draw.aacircle(surface, (0,240,0) if connected else (100,0,0), (surface.width - 20, surface.height - 20), 8)
                
                pygame.display.update()
            
            clock.tick(FPS_LIMIT)
        
        mqttc.loop_stop()
        mqttc.disconnect()
        pygame.quit()
    except KeyboardInterrupt:
        mqttc.loop_stop()
        mqttc.disconnect()
        pygame.quit()    

if __name__ == '__main__':
    launch()