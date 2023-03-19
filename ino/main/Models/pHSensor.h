// PH sensor model

#pragma once

#include "..\utils\utils.h"
#include "..\utils\ph_dicts.h" // for ph and anlog mapping


class pHSensor {
    private:
        const int BITS = 1024;
        const float VOLTAGE = 5.0;
        const float CALIBRATION = 6.0;
        const float CALIBRATION_OFFSET = 0.0;

    public:
        int pin;
        float value;
        float start = 7.0;
        float target = 7.0;

        pHSensor(int pin_a): pin(pin_a) {
            pinMode(pin, INPUT);
        }
        
        float get_backup() {
            unsigned long int avgval = 0;
            int buffer_arr[10], temp;

            // Reading analog
            for (int i = 0; i < 10; i++) {
                buffer_arr[i] = analogRead(pin);

                if (RAW_ANALOG) {
                    debug(buffer_arr[i]);
                }
                
                delay(10);
            }

            // yo! wtf
            for (int i = 0; i < 9; i++) {
                for (int j = i + 1; j < 10; j++) {
                    if (buffer_arr[i] > buffer_arr[j]) {
                        temp = buffer_arr[i];
                        buffer_arr[i] = buffer_arr[j];
                        buffer_arr[j] = temp;
                    }
                }
            }

            // idk
            for (int i = 2; i < 8; i++) {
                avgval += buffer_arr[i];
            }

            float milli_volt = (float)avgval * VOLTAGE / BITS / CALIBRATION;
            value = (2.15 * milli_volt + CALIBRATION_OFFSET); 

            if (RAW_SENSOR) debug("Ph value : " + String(value));

            return value;
        }
        
        float get() {
            unsigned long int avgval = 0;
            int reading, temp;

            // Reading analog
            reading = analogRead(pin);

            if (RAW_ANALOG) {
                debug(reading);
            }

            if (reading < PH0) {
                value = 0.0;
            } else if (reading > PH14) {
                value = 14.0;
            } else {
                if (PH0 < reading && reading <= PH1) {
                    value = float_map(reading, PH0, PH1, 0.0, 1.0);
                } else if (PH1 < reading && reading <= PH2) {
                    value = float_map(reading, PH1, PH2, 1.0, 2.0);
                } else if (PH2 < reading && reading <= PH3) {
                    value = float_map(reading, PH2, PH3, 2.0, 3.0);
                } else if (PH3 < reading && reading <= PH4) {
                    value = float_map(reading, PH3, PH4, 3.0, 4.0);
                } else if (PH4 < reading && reading <= PH5) {
                    value = float_map(reading, PH4, PH5, 4.0, 5.0);
                } else if (PH5 < reading && reading <= PH6) {
                    value = float_map(reading, PH5, PH6, 5.0, 6.0);
                } else if (PH6 < reading && reading <= PH7) {
                    value = float_map(reading, PH6, PH7, 6.0, 7.0);
                } else if (PH7 < reading && reading <= PH8) {
                    value = float_map(reading, PH7, PH8, 7.0, 8.0);
                } else if (PH8 < reading && reading <= PH9) {
                    value = float_map(reading, PH8, PH9, 8.0, 9.0);
                } else if (PH9 < reading && reading <= PH10) {
                    value = float_map(reading, PH9, PH10, 9.0, 10.0);
                } else if (PH10 < reading && reading <= PH11) {
                    value = float_map(reading, PH10, PH11, 10.0, 11.0);
                } else if (PH11 < reading && reading <= PH12) {
                    value = float_map(reading, PH11, PH12, 11.0, 12.0);
                } else if (PH12 < reading && reading <= PH13) {
                    value = float_map(reading, PH12, PH13, 12.0, 13.0);
                } else if (PH13 < reading && reading <= PH14) {
                    value = float_map(reading, PH13, PH14, 13.0, 14.0);
                } 
            }

            return value;
        }

        float get_test(int denoise = 0) {
            int buffer = analogRead(pin);

            debug("Raw measuaring raw value = " + String(buffer));

            double milli_volt = VOLTAGE / BITS * buffer;
            debug("Voltage value = " + String(milli_volt));

            value = 7 + ((0.25 - milli_volt) / 0.18);
            debug("pH value = " + String(value));

            return value;
        }

        float percent(void) {
            float _percent = float_map(value, start, target, 0, 100);
            return _percent;
        }
};