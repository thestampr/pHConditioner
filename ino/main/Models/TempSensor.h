// Temperature sensor model

#pragma once

#include <DallasTemperature.h>
#include <OneWire.h>

#include "..\utils.h"


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
            // call this in `SimpleTimer.setInterval` to prevent from delay

            Sensor.requestTemperatures();
            value = Sensor.getTempCByIndex(0);
            return value;
        }
};