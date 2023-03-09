#define PIN A0

int pH_Value; 
float Voltage;

void setup() { 
    Serial.begin(9600);
    pinMode(PIN, INPUT); 
} 
 
void loop() { 
    pH_Value = analogRead(PIN); 
    Voltage = pH_Value * (5.0 / 1023.0); 
    Serial.println(Voltage); 
    delay(500); 
}