# type: ignore

import paho.mqtt.publish as publish
import paho.mqtt.subscribe as subscribe

import time

MQTT_HOST = 'localhost'

def set_slots(slots):
    if len(slots) != 4:
        raise ValueError()
    
    msgs = [
        { 'topic': f'display/slots/{i+1}', 'payload': slots[i] }
        for i in range(4)
    ]
    publish.multiple(msgs, hostname=MQTT_HOST)
    
    while True:
        msg = subscribe.simple('display/status')
        if msg.payload.isdigit() and int(msg.payload) == 0:
            return
        
def set_selected(slot):
    publish.single('display/selected', slot)
    
print('running demo...')

set_selected(0)
set_slots([1,1,7,1])

set_selected(1)
time.sleep(1)
set_selected(2)
time.sleep(1)
set_selected(3)
time.sleep(1)
set_selected(4)
time.sleep(1)
set_selected(0)

set_slots([2,2,6,0])

set_selected(1)
time.sleep(1)
set_selected(2)
time.sleep(1)
set_selected(3)
time.sleep(1)
set_selected(0)

while True:
    print('enter slot values (eg 2 2 8 or 3 4 3 3) or "exit"')
    str = input('> ')
    if str == 'exit':
        break
    inputs = str.split()
    inputs = list(filter(lambda s: len(s) > 0, inputs))
    if len(inputs) > 4:
        continue
    slots = []
    ok = True
    for s in inputs:
        if s.isdigit():
            slots.append(int(s))
        else:
            ok = False
            break
    if not ok:
        continue

    for i in range(4 - len(slots)):
        slots.append(0)
        
    set_slots(slots)
    for i in range(len(slots)):
        if slots[i] != 0:
            set_selected(i + 1)
            time.sleep(1)
            
    set_selected(0)