//Example By ArduinoAll

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 //กำหนดว่าขาของเซนเซอร์ 18B20 ต่อกับขา 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void) {
    Serial.begin(9600);
    Serial.println("ArduinoAll Test Temperature 18B20");
    sensors.begin();
}

void loop(void) {
    sensors.requestTemperatures(); //สั่งอ่านค่าอุณหภูมิ
    Serial.print(sensors.getTempCByIndex(0)); // แสดงค่าอุณหภูมิ
    Serial.println(" *C");
    delay(2000);
}