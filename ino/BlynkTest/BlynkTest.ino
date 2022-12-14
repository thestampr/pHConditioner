#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

#include "secret.h"
#include "config.h"

#define SERIAL_BUFFER_SIZE 64
#define BLYNK_PRINT Serial
#define ESP8266_BAUD 9600

#define LED 13


SoftwareSerial EspSerial(2, 3); // RX, TX
// WidgetLCD lcd(V0);

ESP8266 wifi(&EspSerial);


// Executes when the value of virtual pin 0 changes
BLYNK_WRITE(V0) {
    if(param.asInt() == 1) {
        // execute this code if the switch widget is now ON
        digitalWrite(LED, HIGH);  // Set digital pin 2 HIGH
    }
    else {
        // execute this code if the switch widget is now OFF
        digitalWrite(LED, LOW);  // Set digital pin 2 LOW    
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Blynk test");

    EspSerial.begin(ESP8266_BAUD);
    Serial.println("Setup EspSerial done");
    Blynk.begin(BLYNK_AUTH_TOKEN, wifi, SSID, PASS);
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