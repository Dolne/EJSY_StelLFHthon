#include <Arduino.h>
#include <FastLED.h>

CRGB feedbackLeds[30];
CRGB scanningLeds[4];

void setup()
{
    Serial.begin(9600);
    FastLED.addLeds<WS2812B, 25, GRB>(feedbackLeds, 30);
    FastLED.addLeds<WS2812B, 4, GRB>(scanningLeds, 4);
}

void loop()
{
    static int p = 0;

    fill_rainbow(feedbackLeds, 30, p, 255 / 30);
    fill_solid(scanningLeds, 4, CRGB::Black);
    scanningLeds[(p % 400) / 100] = CRGB::Red;

    FastLED.show();

    p++;
    delay(10);
}