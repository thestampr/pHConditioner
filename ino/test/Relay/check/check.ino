#define check 13

void setup(void) {
    pinMode(check, OUTPUT);
}

void loop(void) {
    digitalWrite(check, HIGH);
    delay(500);
    digitalWrite(check, LOW);
    delay(500);
}