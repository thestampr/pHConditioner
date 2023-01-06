// PH sensor model

class pHSensor {
    public:
        int pin;
        float value;

        pHSensor(int pin_): pin(pin_) {
            pinMode(pin, INPUT);
        }

        float get() {
            unsigned long int avgval;
            int buffer_arr[10], temp;

            for (int i = 0; i < 10; i++) {
                buffer_arr[i] = analogRead(ANALOG_PH);
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

            float milli_volt = (float)avgval * 5.0 / 1024 / 6;
            value = (milli_volt * 3.5) + PH_CALIBRATION;
            return value;
        }
};
