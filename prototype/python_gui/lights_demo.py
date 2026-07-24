# type: ignore

import paho.mqtt.publish as publish

import time

MQTT_HOST = 'localhost'

def enable_lights():
    publish.single('display/lights', '1')

def set_lights(colours: str, travel: float, period: int, segments: int):
    publish.multiple([
        {'topic': 'display/lights/colours', 'payload': colours},
        {'topic': 'display/lights/travel', 'payload': travel},
        {'topic': 'display/lights/period', 'payload': period},
        {'topic': 'display/lights/segments', 'payload': segments}
    ])

enable_lights()

# default rgb
set_lights('0% 0, 100% 360', 0.5, 1200, 16)
time.sleep(4)

# solid green
set_lights('0% 120', 0, 1200, 1)
time.sleep(2)

# wee woo wee woo
set_lights('0% 0, 50% 0, 50% 240, 100% 240', 2, 1000, 4)
time.sleep(4)

# low health
set_lights('0% 0, 50% 0 100 100, 100% 0', 0, 2000, 1)
time.sleep(4)

# rgb but rtl
set_lights('0% 0, 100% 360', -0.5, 1200, 16)