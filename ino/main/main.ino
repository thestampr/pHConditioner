// main source code

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <EEPROM.h>

#include "config.h"
#include "secret.h"
#include "utils.h"

#include "Models\pHSensor.h"
#include "Models\TempSensor.h"


int working = 0;
int update_timer = 1;

float update_timer_counter = 0;
float ph_target = 7.0;

SimpleTimer Timer;

TempSensor Temp(ONE_WIRE_BUS);
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

        debug("pH:" + String(Ph.value) + ", Temp:" + String(Temp.value));

        digitalWrite(BUILTIN_LED, LOW);

        Blynk.virtualWrite(PIN_PH, Ph.value);
        Blynk.virtualWrite(PIN_TEMP, Temp.value);
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

    Serial.begin(115200);
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
    Timer.setInterval(1000L, [&]() { Temp.get(); });
}

void loop(void) {
    Blynk.run();
    Timer.run();

    if (Blynk.connected()) {
        update();
    }

    delay(DELAY_TIME);
}