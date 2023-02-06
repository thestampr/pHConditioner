// Forget about running this code.
// It's useless code, Isn't it?


#define I 0 // input 
#define O 1 // output 
#define V 3 // value 

#define MOTOR_COUNT 5 // motors

int motors[MOTOR_COUNT][3] = {
    {3, 8}, 
    {4, 9}, 
    {5, 10}, 
    {6, 11}, 
    {7, 12}
};

void setup() {
    Serial.begin(9600);

    for (int i = 0; i < MOTOR_COUNT; i++) {
        pinMode(motors[i][I], INPUT);
        pinMode(motors[i][O], OUTPUT);
        digitalWrite(motors[i][O], 0);
    }
}

void loop() {
    for (int i = 0; i < MOTOR_COUNT; i++) {
        digitalWrite(motors[i][O], digitalRead(motors[i][I]));
    }

    delay(10);
}