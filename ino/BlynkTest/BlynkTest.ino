#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

#include "secret.h"
#include "config.h"

#define SERIAL_BUFFER_SIZE 64
#define BLYNK_PRINT Serial
#define ESP8266_BAUD 9600


SoftwareSerial EspSerial(2, 3); // RX, TX
WidgetLCD lcd(V0);

ESP8266 wifi(&EspSerial);


void setup() {
    Serial.begin(9600);
    EspSerial.begin(ESP8266_BAUD);
    Blynk.begin(BLYNK_AUTH_TOKEN, wifi, SSID, PASS);
    lcd.clear();
    lcd.print(1, 1, "IoT");
}

void loop() {
    Blynk.run();
}