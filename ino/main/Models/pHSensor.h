// PH sensor model

#pragma once

#include "..\utils\utils.h"

const float VOLTAGE = 3.0;
const float CALIBRATION = 8.0;


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
            unsigned long int avgval;
            int buffer_arr[10], temp;

            for (int i = 0; i < 10; i++) {
                buffer_arr[i] = analogRead(pin);
                if (RAW_SENSOR) Serial.println(buffer_arr[i]);
                delay(10);
            }
            for (int i = 0; i < 9; i++) {
                for (int j = i + 1; j < 10; j++) {
                    if (buffer_arr[i] > buffer_arr[j]) {
                        temp = buffer_arr[i];
                        buffer_arr[i] = buffer_arr[j];
                        buffer_arr[j] = temp;
                    }
                }
            }
            avgval = 0;
            for (int i = 2; i < 8; i++)
                avgval += buffer_arr[i];

            float milli_volt = (float)avgval * VOLTAGE / 1024 / CALIBRATION;
            value = (milli_volt * 3.5);
            return value;
        }
        
        float get_backup(void) {
            unsigned long int avgval;
            int buffer_arr[10], temp;
            float read_buffer = 0.0;

            for (int i = 0; i < 10; i++) {
                buffer_arr[i] = analogRead(pin);
                read_buffer += buffer_arr[i];
                if (RAW_SENSOR) Serial.println(buffer_arr[i]);
                delay(10);
            }
            int buffer_lenght = sizeof(buffer_arr) / sizeof(float);
            avgval = read_buffer / buffer_lenght;
            // float milli_volt = (float)avgval * VOLTAGE / 1024 / CALIBRATION;
            value = float_map(avgval, 0.0, 1024.0, 1.0, 14.0);
            return value;
        }
};