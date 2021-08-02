#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN       D1
#define NUMPIXELS 20
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);

  pixels.begin();
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(0, 150, 0));
  pixels.show();
}

void loop() {
}