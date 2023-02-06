#include <Arduino.h>

//#define PH A0;

float resolution;
int measurings;
float voltage;
float pHvalue;
float b = 0.00;
float m = 0.167;

void setup(){
    Serial.begin(9600);
    resolution = 1024.0;
}

void loop(){
    measurings = 0;
    for (int i = 0; i < 10; i++){
        measurings = measurings + analogRead(A0);
        delay(10);
    }
    voltage = ((5 / resolution) * (measurings/10));
    
    pHvalue = ((7 + ((2.5 - voltage) / m)))+ b;
    
    Serial.print("pH= ");
    Serial.println(pHvalue);
    
    delay(1000);
}
