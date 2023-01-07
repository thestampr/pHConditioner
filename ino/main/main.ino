// main source code

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SimpleTimer.h>
#include <EEPROM.h>

#include "config.h"
#include "secret.h"
#include "utils.h"

#include "Models\pHSensor.h"

int working = 0;
int update_timer = 1;

float update_timer_counter = 0;
float ph_target = 7.0;

SimpleTimer Timer;
OneWire Onewire(ONE_WIRE_BUS);
DallasTemperature Sensor(&Onewire);

pHSensor Ph(ANALOG_PH);


String read_eeprom(int address, String default_value) {
    String result = "";
    char c;
    int i;
    while (true) {
        c = EEPROM.read(address + i);
        if (c == 0 || c == 255 || i >= EEPROM_MAX_LENGTH) {  // stop reading if null character or empty cell or max length reached
            if (result == "") {  // if nothing was read
                result = default_value;  // use the default value
            }
            break;
        }
        result += c;
        i++;
    }
    return result;
}


BLYNK_CONNECTED() {
    Blynk.syncAll();
}

BLYNK_WRITE(PIN_WORKER) {
    digitalWrite(LED, param.asInt());
    working = param.asInt();
    if (param.asInt()) {
        debug("Starting, target=" + String(ph_target));
    } else {
        stop();
    }
}

BLYNK_WRITE(PIN_PH_TARGET) {
    ph_target = param.asFloat();
}

BLYNK_WRITE(PIN_UPDATE_TIMER) {
    update_timer = param.asInt();
}


void update(void) {
    update_timer_counter += DELAY_TIME/100;

    if (update_timer_counter >= DELAY_TIME * update_timer) {
        // Send data every update_timer

        debug("pH:" + String(Ph.value));

        digitalWrite(BUILTIN_LED, LOW);
        Sensor.requestTemperatures();

        Blynk.virtualWrite(PIN_PH, Ph.value);
        Blynk.virtualWrite(PIN_TEMP, Sensor.getTempCByIndex(0));
        update_timer_counter = 0;
    } else {
        digitalWrite(BUILTIN_LED, HIGH);
    }
}

void reset(void) {
    working = 0;

    Blynk.virtualWrite(PIN_WORKER, 0);
    Blynk.virtualWrite(PIN_PROCESS, 0);
}

void stop(void) {
    
}


void setup(void) {
    pinMode(LED, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    // pinMode(ANALOG_PH, INPUT);

    Serial.begin(115200);
    Sensor.begin();
    WiFi.begin(SSID, PASS);
    EEPROM.begin(512);

    Blynk.begin(
        BLYNK_AUTH_TOKEN, 
        SSID, 
        PASS, 
        BLYNK_CLOUD, 
        80
    );

    Timer.setInterval(1000L, [&]() { Ph.get(); });
}

void loop(void) {
    if (Blynk.connected()) {
        Timer.run();
        Blynk.run();
        update();
        delay(DELAY_TIME);
    }
}