#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include "GravityTDS.h"
GravityTDS Tds;

//------------ LCD ---------------------------
LiquidCrystal_I2C lcd(0x27,20,4);// PCF8574A =  0x3F
//----------------------------------------------
float TDS = 0;

void setup()
{    
    lcd.begin();
    lcd.setCursor(0,0);lcd.print("   < TDS  Meter >   ");  
    lcd.setCursor(0,1);lcd.print("                    "); 
    lcd.setCursor(0,2);lcd.print("                    "); 
    lcd.setCursor(0,3);lcd.print("TDS    =         ppm");    
    Tds.setPin(A0);
    Tds.setAref(5.0);  
    Tds.setAdcRange(1024);  
    Tds.begin();     
}
void loop()
{
    Tds.setTemperature(25);  
    Tds.update();  
    TDS = Tds.getTdsValue()/1.6;  
    lcd.setCursor(12,3); lcd.print("    ");
    lcd.setCursor(12,3); lcd.print(TDS,0);    
    delay(1000);
}

