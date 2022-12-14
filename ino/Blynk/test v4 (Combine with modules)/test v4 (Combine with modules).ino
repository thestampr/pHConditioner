// main source code

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SimpleTimer.h>

#include "config.h"
#include "secret.h"
#include "utils.h"

int working = 0;

float update_timer = 0;
float process_percent = 0;
float ph_target = 7.0;
float ph;

SimpleTimer Timer;
OneWire Onewire(ONE_WIRE_BUS);
DallasTemperature Sensor(&Onewire);


BLYNK_CONNECTED() {
    Blynk.syncAll();
}

BLYNK_WRITE(PIN_WORKER) {
    digitalWrite(LED, param.asInt());
    working = param.asInt();
    if (param.asInt()) {
        debug("Starting, target=" + String(ph_target));
    } else {
        stop_working();
    }
}

BLYNK_WRITE(PIN_PH_TARGET) {
    ph_target = param.asFloat();
}

BLYNK_WRITE(PIN_PROCESS) {
    process_percent = param.asFloat();
}


float get_ph(void) {
    unsigned long int avgval;
    int buffer_arr[10], temp;

    for (int i=0; i<10; i++) { 
        buffer_arr[i] = analogRead(ANALOG_PH);
        delay(10);
    }
    for (int i=0; i<9; i++) {
        for ( int j=i+1; j<10; j++) {
            if ( buffer_arr[i] > buffer_arr[j]) {
                temp = buffer_arr[i];
                buffer_arr[i] = buffer_arr[j];
                buffer_arr[j] = temp;
            }
        }
    }
    avgval = 0;
    for (int i=2; i<8; i++)
        avgval += buffer_arr[i];
    float milli_volt = (float)avgval * 5.0 / 1024 / 6; 
    ph = (milli_volt * 3.5) + PH_CALIBRATION;
    return ph;
}

/**
 * Stop the machine from running
 * 
 */
void stop_working(void) {
    reset();
}


void reset(void) {
    working = 0;
    process_percent = 0;

    Blynk.virtualWrite(PIN_WORKER, 0);
    Blynk.virtualWrite(PIN_PROCESS, 0);

    digitalWrite(LED, working);
}

void update(void) {
    update_timer += DELAY_TIME/100;

    if (working) {
        process_percent += 0.1;
    } else {
        process_percent = 0;
    }

    if (update_timer >= DELAY_TIME * UPDATE_SECOND) {
        // Send data every UPDATE_SECOND

        debug("pH:" + String(ph));

        digitalWrite(BUILTIN_LED, LOW);
        Sensor.requestTemperatures();

        Blynk.virtualWrite(PIN_PH, ph);
        Blynk.virtualWrite(PIN_TEMP, Sensor.getTempCByIndex(0));
        Blynk.virtualWrite(PIN_PROCESS, process_percent);

        update_timer = 0;
    } else {
        digitalWrite(BUILTIN_LED, HIGH);
    }
}


void setup(void) {
    pinMode(LED, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(ANALOG_PH, INPUT);

    Serial.begin(115200);
    Sensor.begin();
    WiFi.begin(SSID, PASS);

    Blynk.begin(
        BLYNK_AUTH_TOKEN, 
        SSID, 
        PASS, 
        BLYNK_CLOUD, 
        80
    );

    Timer.setInterval(1000L, get_ph);
}

void loop(void) {
    if (Blynk.connected()) {
        Timer.run();
        Blynk.run();
        update();
        delay(DELAY_TIME);
    }
}