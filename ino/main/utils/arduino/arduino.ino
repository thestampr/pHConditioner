#define pin_in 12
#define pin_out 13

void setup() {
    pinMode(pin_in, INPUT);
    pinMode(pin_out, OUTPUT);
}

void loop() {
    if (digitalRead(pin_in)) {
        digitalWrite(pin_out, HIGH);
    } else {
        digitalWrite(pin_out, LOW);
    }

    delay(10);
}