//Ph
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
unsigned int  PH;

//Temperature
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2 //กำหนดว่าขาของเซนเซอร์ 18B20 ต่อกับขา 2



OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);



void setup(void){
    Serial.begin(9600);
    Serial.println("ArduinoAll Test Temperature 18B20");
    sensors.begin();
}

void loop(void){
    //Temperature Display
    sensors.requestTemperatures();
    Serial.print("Tmp : ");
    Serial.print(sensors.getTempCByIndex(0));
    Serial.print(" *C");
    Serial.println();

    //PH Display
    //Read_PH2();
    //Serial.print("PH  : ");
    //Serial.print(PH/10);
    //Serial.print(".");
    //Serial.print(PH%10);
    //Serial.println();
    //Serial.println();
    
    delay(1000);
}

void Read_PH2(){
    unsigned char i;
    int buf[10],temp;
    unsigned long int avgValue; 
    float PHD;
    for(int i=0;i<10;i++){
        buf[i]=analogRead(0);
        delay(10);
    }
    for(int i=0;i<9;i++){
        for(int j=i+1;j<10;j++){
            if(buf[i]>buf[j]){
                temp=buf[i];
                buf[i]=buf[j];
                buf[j]=temp;
            }
        }
    }
    avgValue=0;
    for(int i=2;i<8;i++){
        avgValue+=buf[i];
        float pHVol=(float)avgValue*5.0/1024/6;
        PHD = -5.70 * pHVol + 21.34;
    }
    PH = PHD *10;
}
