#define R 12 // In1 (1 on, 3 off)
#define L 13 // In2 (4 on, 6 off)

char key; // สร้างตัวแปรสำหรับ Input

void setup() {
    /* เรียกใช้เมื่อเริ่มต้นโปรแกรม ทำงานครั้งเดียว */
    
    // default
    Serial.begin(9600);
    
    // กำหนดโหมดเป็น Output
    pinMode(R, OUTPUT);
    pinMode(L, OUTPUT);
}


void loop() {
    /* เรียกใช้หลังจาก setup ทำงานแบบ loop */
    
    // Input
    if (Serial.available()) {
        key = Serial.read(); // Keyboard input

        // In1 statement
        if (key == '1' || key == '3') {
            if (key == '1') {
                digitalWrite(R, HIGH);
            }
            else if (key == '3') {
                digitalWrite(R, LOW);
            }
        }
        
        // In2 statement
        else if (key == '4' || key == '6') {
            if (key == '4') {
                digitalWrite(L, HIGH);
            }
            else if (key == '6') {
                digitalWrite(L, LOW);
            }
        }
        
        // control In1, In2
        else if (key == '7' || key == '9') {
            if (key == '7') {
                digitalWrite(R, HIGH);
                digitalWrite(L, HIGH);
            }
            else if (key == '9') {
                digitalWrite(R, LOW);
                digitalWrite(L, LOW);
            }
        }
    }
    delay(10);
}
