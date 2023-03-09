// PH sensor model

#pragma once

#include "..\utils\utils.h"

const float VOLTAGE = 3.0;
const float CALIBRATION = 6.0;
const float CALIBRATION_OFFSET = 20.0;
const int SAMPLINGS = 10; // don't touch it

const float BITS = 1024.0;

const float PH_MEAN = 7.6;


class pHSensor {
    public:
        int pin;
        float value;
        float start = 7.0;
        float target = 7.0;

        pHSensor(int pin_a): pin(pin_a) {
            pinMode(pin, INPUT);
        }
        
        float get(void) {
            unsigned long int avgval = 0;
            int buffer_arr[SAMPLINGS], temp;

            // Reading analog
            for (int i = 0; i < SAMPLINGS; i++) {
                buffer_arr[i] = analogRead(pin);
                // debug(buffer_arr[i]);
                delay(10);
            }

            if (PH_CALIBRATING) {
                int sample = analogRead(pin);
                float voltage = sample * (VOLTAGE / (BITS-1));
                debug("Voltage : " + String(voltage));
            }

            // yo! wtf
            for (int i = 0; i < SAMPLINGS-1; i++) {
                for (int j = i + 1; j < SAMPLINGS; j++) {
                    if (buffer_arr[i] > buffer_arr[j]) {
                        temp = buffer_arr[i];
                        buffer_arr[i] = buffer_arr[j];
                        buffer_arr[j] = temp;
                    }
                }
            }

            // idk
            for (int i = 2; i < SAMPLINGS-2; i++)
                avgval += buffer_arr[i];

            float milli_volt = (float)avgval * VOLTAGE / BITS / CALIBRATION;
            // value = (milli_volt * 3.5);
            // value = (-5.70 * milli_volt + CALIBRATION_OFFSET) + (((-5.70 * milli_volt + CALIBRATION_OFFSET) - PH_MEAN) * 1.0); 
            value = (-5.70 * milli_volt + CALIBRATION_OFFSET); 
            if (RAW_SENSOR) debug("Ph value : " + String(value));
            return value;
        }

        float percent(void) {
            float _percent = float_map(value, start, target, 0, 100);
            return _percent;
        }
};