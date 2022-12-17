// Source from https://github.com/marofES/How-to-connect-blynk-in-proteus/
#define BLYNK_PRINT Serial

#include <BlynkSimpleStream.h>

#include "secret.h"

#define LED 13


void setup() {
    pinMode(LED, OUTPUT);
    // Blynk will work through Serial
    // Do not read or write this serial manually in your sketch
    Serial.begin(9600);
    Blynk.begin(Serial, BLYNK_AUTH_TOKEN);
}

void loop() {
    Blynk.run();
}