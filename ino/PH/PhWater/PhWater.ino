// code ซื้อ

#include <Wire.h>

float PH;

void setup(void){
    Serial.begin(9600);
}

//-------------------------------------------------------------

void loop(void){ 
    char Key;
    Read_PH2();
    
    Serial.print("pH= ");
    Serial.println(PH/10);
    delay(10000);
}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------

void Read_PH2(){
    unsigned char i;
    int buf[10], temp;
    unsigned long int avgValue; 
    float PHD;
    
    for(int i=0; i<10; i++){
       buf[i] = analogRead(0);
       delay(10);
    }
    for(int i=0; i<9; i++){
        for(int j=i+1; j<10; j++){
           if(buf[i]>buf[j]){
               temp = buf[i];
               buf[i] = buf[j];
               buf[j] = temp;
           }
       }
    }
    avgValue = 0;
    for(int i=2; i<8; i++)
        avgValue += buf[i];
    float pHVol = (float)avgValue*5.0/1024/6;
    PHD = -5.70 * pHVol + 21.34; 
    PH = PHD *10;
}

//------------------------------------------------------------------

void NumToChar(unsigned long Num, char *Buffer, unsigned char Digit){
    char i;
    for(i=(Digit-1); i>= 0; i--){
        Buffer[i] = (Num % 10) + '0';  // 234 , 23 , 2
        Num = Num / 10;
    }  
    for(i=0; i<(Digit-1); i++){
        if(Buffer[i] == '0'){
            Buffer[i] =  ' ';
        }else{
            i =100;
        }
    }   
}
