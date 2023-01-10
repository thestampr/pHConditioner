// Temperature sensor model

#pragma once

#include <DallasTemperature.h>
#include <OneWire.h>


class TempSensor {
    public:
        int pin;
        float value;

        OneWire Onewire;
        DallasTemperature Sensor;

        TempSensor(int pin_d): pin(pin_d), Onewire(pin_d), Sensor(&Onewire) {
            Sensor.begin();
        }

        float get(void) {
            Sensor.requestTemperatures();
            value = Sensor.getTempCByIndex(0);
            return value;
        }
};