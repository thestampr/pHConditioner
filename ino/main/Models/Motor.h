// MotorPump model

#pragma once

#define RUN_TIME 1000 // millisecs
#define STOP_TIME 5000 // millisecs


class MotorPump {
    private:
        int state;
        int change_state;
        int after_is_set;
        int stop_after;
        unsigned long state_runtime;
        unsigned long state_endtime;

    public:
        int pin;
        int is_running;
        bool do_delay = false;

        MotorPump(int pin_d): pin(pin_d), do_delay(true) {
            pinMode(pin, OUTPUT);
        }

        MotorPump(int pin_d, bool do_delay): pin(pin_d), do_delay(do_delay) {
            pinMode(pin, OUTPUT);
        }

        void run(int run_time = RUN_TIME, int stop_time = STOP_TIME) {
            state_runtime = millis();
            if (do_delay && run_time) {
                if (state) {
                    if (change_state) {
                        state_endtime = state_runtime + run_time;
                        change_state = 0;
                    }
                    digitalWrite(pin, HIGH);
                    if (state_runtime > state_endtime) {
                        state = 0;
                        change_state = 1;
                        digitalWrite(pin, LOW);
                    }
                } else {
                    if (change_state) {
                        state_endtime = state_runtime + stop_time;
                        change_state = 0;
                    }
                    digitalWrite(pin, LOW);
                    if (state_runtime > state_endtime) {
                        state = 1;
                        change_state = 1;
                        digitalWrite(pin, HIGH);
                    }
                }
            } else {
                digitalWrite(pin, HIGH);
                state = 1;
            }
            is_running = 1;
            stop_after = 0;
        }

        void stop(int after = 0) {
            if (is_running) {    
                if (!after) {
                    digitalWrite(pin, LOW);
                    state = 0;
                    is_running = 0;
                    stop_after = 0;
                } else {
                    unsigned long now = millis();
                    if (!stop_after) {
                        stop_after = now + after;
                    }
                    if (now > stop_after) {
                        digitalWrite(pin, LOW);
                        state = 0;
                        is_running = 0;
                        stop_after = 0;
                    }
                }
            }
        }
};