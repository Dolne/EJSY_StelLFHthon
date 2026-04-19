import os
from typing import Any, Union
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "hide"

import pygame
from datetime import datetime

from slot import Slot, StatusManager

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
        
        selected_changed = True
        
        def on_connect(client: mqtt.Client, userdata: Any, flags: mqtt.ConnectFlags, reason_code: mqtt.ReasonCode, props: Union[mqtt.Properties, None]): # pyright: ignore[reportPrivateImportUsage]
            print(f"connected with result code {reason_code}")
            client.subscribe('display/slots/+')
            client.subscribe('display/selected')
            
        def on_message(client: mqtt.Client, userdata: Any, message: mqtt.MQTTMessage):
            if not message.payload.isdigit():
                print(message.topic, message.payload)
                return
            print(message.topic, int(message.payload))
            if message.topic == 'display/selected':
                nonlocal selected_changed
                selected_changed = True
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
        
        mqttc.on_connect = on_connect
        mqttc.on_message = on_message
        
        mqttc.connect_async(MQTT_HOST, 1883, 60)
        mqttc.loop_start()
        
        connected = mqttc.is_connected()
        
        w = surface.get_width()
        
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
            
            # 3. selection changed
            #    update the selection arrow
            has_changes = has_changes or selected_changed
                
            if has_changes:
                    
                surface.fill((244, 244, 244))
                        
                for slot in slots:
                    slot.render()
                selected_changed = False
                
                # connection indicator
                pygame.draw.circle(surface, (0,240,0) if connected else (100,0,0), (20,20), 10)
                
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