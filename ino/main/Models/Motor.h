// MotorPump model

#pragma once

#define RUN_TIME 2000 // millisecs
#define STOP_TIME 5000 // millisecs


class MotorPump {
    private:
        int state = 0;
        int change_state = 0;
        unsigned long state_runtime;
        unsigned long state_endtime;

    public:
        int pin;
        bool do_delay = false;

        MotorPump(int pin_d): pin(pin_d), do_delay(true) {
            pinMode(pin, OUTPUT);
        }

        MotorPump(int pin_d, bool do_delay): pin(pin_d), do_delay(do_delay) {
            pinMode(pin, OUTPUT);
        }

        void run(void) {
            state_runtime = millis();
            if (do_delay) {
                if (state) {
                    if (change_state) {
                        state_endtime = state_runtime + RUN_TIME;
                        change_state = 0;
                    }
                    digitalWrite(pin, HIGH);
                    if (state_runtime > state_endtime) {
                        state = 0;
                        change_state = 1;
                    }
                } else {
                    if (change_state) {
                        state_endtime = state_runtime + STOP_TIME;
                        change_state = 0;
                    }
                    digitalWrite(pin, LOW);
                    if (state_runtime > state_endtime) {
                        state = 1;
                        change_state = 1;
                    }
                }
            } else digitalWrite(pin, HIGH);
        }

        void stop(void) {
            digitalWrite(pin, LOW);
            state = 0;
        }
};