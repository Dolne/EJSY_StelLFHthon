#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 48
Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);

int blink_time = 500; // milliseconds
bool led_state = false;
int last_switch = 0; // tracks time that LED was last switched

void setup(){
    Serial.begin(115200);
    led.begin();
    led.setBrightness(50);
    led.show();
}

void loop(){
    Serial.println("hello");
}
