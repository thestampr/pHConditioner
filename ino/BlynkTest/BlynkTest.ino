#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

#include "secret.h"
#include "config.h"

#define BUAD_RATE 9600
#define LED 13


SoftwareSerial EspSerial(2, 3); // RX, TX
// WidgetLCD lcd(V0);

ESP8266 wifi(&EspSerial);


// Executes when the value of virtual pin 0 changes
BLYNK_WRITE(V0) {
    if (param.asInt() == 1) {
        digitalWrite(LED, HIGH);
    }
    else {
        digitalWrite(LED, LOW);
    }
}

void setup() {
    Serial.begin(BUAD_RATE);
    Serial.println("Blynk test");

    EspSerial.begin(BUAD_RATE);
    Serial.println("Setup EspSerial done");

    Blynk.begin(BLYNK_AUTH_TOKEN, wifi, SSID, PASS); // Stuck
    Serial.println("Setup Blynk done");

    pinMode(LED, OUTPUT);

    // lcd.clear();
    // lcd.print(1, 1, "IoT");
    Serial.println("All done");
}

void loop() {
    Blynk.run();
    // listen for communication from the ESP8266 and then write it to the serial monitor
    if ( EspSerial.available() )   {  Serial.write( EspSerial.read() );  }
 
    // listen for user input and send it to the ESP8266
    if ( Serial.available() )       {  EspSerial.write( Serial.read() );  }
}