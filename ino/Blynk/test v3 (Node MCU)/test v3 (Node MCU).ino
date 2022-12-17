#define BLYNK_PRINT Serial // Comment this out to hide Blynk serial debugging

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "config.h"
#include "secret.h"
#include "utils.h"

#define LED D0


float ph_target = 7.0;

void reset(void) {
    Blynk.virtualWrite(PIN_WORKER, 0);
    // Blynk.virtualWrite(PIN_TEMP, 0);
    // Blynk.virtualWrite(PIN_PH, 0);
    Blynk.virtualWrite(PIN_PH_TARGET, ph_target);
    Blynk.virtualWrite(PIN_PROCESS, 0);
}

// on worker change
BLYNK_WRITE(PIN_WORKER) {
    if (param.asInt() == 1) {
        digitalWrite(LED, HIGH);
        Serial.print("Starting, target=");
        Serial.println(ph_target);
    }
    else {
        digitalWrite(LED, LOW);
    }
}

// on ph target change
BLYNK_WRITE(PIN_PH_TARGET) {
    ph_target = param.asFloat();
}

// on connect
BLYNK_CONNECTED(){
    Blynk.virtualWrite(V0, 0);
    reset();
}

void setup() {
    // Setup pins
    pinMode(LED, OUTPUT);

    Serial.begin(115200); // Start the Serial communication to send messages to the computer
    delay(10);
    WiFi.begin(SSID, PASS); // Connect to the network

    Blynk.begin(
        BLYNK_AUTH_TOKEN, 
        SSID, 
        PASS, 
        BLYNK_CLOUD, 
        80
    );
}

void loop() {
    Blynk.run();
}