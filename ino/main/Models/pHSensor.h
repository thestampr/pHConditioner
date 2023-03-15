// PH sensor model

#pragma once

#include "..\utils\utils.h"

const float VOLTAGE = 3.0;
const float CALIBRATION = 6.0;
const float CALIBRATION_OFFSET = 21.0;
const int SAMPLINGS = 10;

const float BITS = 1024.0;

const float PH_MEAN = 7.6;

const float FILLTER = 0.95;


class pHSensor {
    private:
        int _counting;
        int _denoise_state;

        float _noise;
        float _denoise_ref;

        float _first_value_of_sampling;
        float _average_samplings;

        float smooth(float data) {
            float _smoothed = (data * (1 - FILLTER)) + (value  *  FILLTER);
            return _smoothed;
        }

    public:
        int pin;
        float value;
        float start = 7.0;
        float target = 7.0;

        pHSensor(int pin_a): pin(pin_a) {
            pinMode(pin, INPUT);
        }
        
        float get(int denoise = 0) {
            unsigned long int avgval = 0;
            int buffer_arr[10], temp;

            float last_value = value;

            // Reading analog
            for (int i = 0; i < 10; i++) {
                buffer_arr[i] = analogRead(pin);

                if (RAW_ANALOG) {
                    debug(buffer_arr[i]);
                }
                
                delay(10);
            }

            if (PH_CALIBRATING) {
                int sample = analogRead(pin);
                float voltage = sample * (VOLTAGE / (BITS-1));
                debug("Voltage : " + String(voltage));
            }

            // yo! wtf
            for (int i = 0; i < 10-1; i++) {
                for (int j = i + 1; j < 10; j++) {
                    if (buffer_arr[i] > buffer_arr[j]) {
                        temp = buffer_arr[i];
                        buffer_arr[i] = buffer_arr[j];
                        buffer_arr[j] = temp;
                    }
                }
            }

            // idk
            for (int i = 2; i < 10-2; i++) {
                avgval += buffer_arr[i];
            }

            float milli_volt = (float)avgval * VOLTAGE / BITS / CALIBRATION;
            // value = (milli_volt * 3.5);
            // value = (-5.70 * milli_volt + CALIBRATION_OFFSET) + (((-5.70 * milli_volt + CALIBRATION_OFFSET) - PH_MEAN) * 1.0); 
            value = (-5.70 * milli_volt + CALIBRATION_OFFSET); 

            if (RAW_SENSOR) debug("Ph value : " + String(value));

            if (USE_DENOISE && denoise) {
                if (!_denoise_state) {
                    // if denoise and denoise state was change

                    _denoise_state = 1;
                    _noise = last_value - value;
                    
                    debug("noise : " + String(_noise));
                }
                value -= _noise;
            } else {
                _denoise_state = 0;
            }

            // _average_samplings += value;

            // if (_counting == SAMPLINGS) {
            //     _average_samplings -= _first_value_of_sampling;
            //     _first_value_of_sampling = value;
            //     value = _average_samplings / SAMPLINGS;
            // } else {
            //     _counting += 1;
            //     _first_value_of_sampling = value;
            // }

            // value = (value + last_value) / 2;

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