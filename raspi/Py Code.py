import paho.mqtt.client as mqtt
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.image as img
#import numpy as np
import time

#fix UserWarning: Starting a Matplotlib GUI outside of the main thread will likely fail.
#matplotlib.use('QtAgg')

#create 3 subplots under one plot
fig, (ax1, ax2, ax3) = plt.subplots(1,3)
ax1.axis('off')
ax2.axis('off')
ax3.axis('off')

#load images
circle = img.imread("SmallBlueCircle.jpg")
triangle = img.imread("BigOrangeTriangle.jpg")

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe("$SYS/#")
    client.subscribe("display")

plt.ion()


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    if int(msg.payload) == 1:
        ax1.imshow(triangle)
        ax2.imshow(circle)
        ax3.imshow(circle)
    if int(msg.payload) == 2:
        ax1.imshow(circle)
        ax2.imshow(triangle)
        ax3.imshow(circle)
    if int(msg.payload) == 3:
        ax1.imshow(circle)
        ax2.imshow(circle)
        ax3.imshow(triangle)
    
    plt.show()
    plt.pause(2)

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect("192.168.1.101", 1883, 60)

#mqttc.loop_start()

#while True:  
#while True:
    #time.sleep(100)

#mqttc.loop_stop()

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
mqttc.loop_forever()
#plt.show(block = False)
