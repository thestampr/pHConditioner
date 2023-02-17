// main source code

#define BLYNK_PRINT Serial

#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

#include "utils\config.h"
#include "utils\secret.h"
#include "utils\utils.h"
#include "utils\enums.h"

#include "models\Motor.h"
#include "models\pHSensor.h"
#include "models\TempSensor.h"


int working;
int reset_checker;
int restart_checker;
int sync_clock = 1;
float good_range = 0.2;

float percent;

bool do_sync = false;

unsigned long blocking_code_runtime;
unsigned long blocking_code_endtime;
unsigned long last_sync;
unsigned long last_reset_hold;
unsigned long last_restart_hold;

WiFiManager WifiMgr;
TempSensor Temp(ONE_WIRE_BUS);
pHSensor Ph(ANALOG_PH);

MotorPump BasePump(BASE_PIN);
MotorPump AcidPump(ACID_PIN);
MotorPump FlowPump(FLOW_PIN, false);


void dynamic_delay(void) {
    blocking_code_runtime = millis();
    if (blocking_code_endtime > blocking_code_runtime) {
        delay(blocking_code_endtime - blocking_code_runtime);
    }
    blocking_code_endtime = blocking_code_runtime + DELAY_TIME;
}


BLYNK_CONNECTED() {
    Blynk.syncAll();
    Blynk.virtualWrite(PIN_VERSION, VERSION);
    Blynk.virtualWrite(PIN_STATUS, State.connected);

    if (DEBUG) {
        logger("Connected to " + String(SSID));
    } else {
        logger("Connected to " + String(WifiMgr.getWiFiSSID().c_str()));
    }

    do_sync = true;
}

BLYNK_DISCONNECTED() {
    debug("Disconnected");
    do_sync = false;
}

BLYNK_WRITE(PIN_WORKER) {
    working = param.asInt();
    if (param.asInt()) {
        Ph.start = Ph.value;
        logger("Working, pHtarget=" + String(Ph.target));
    } else {
        stop();
    }
}

BLYNK_WRITE(PIN_RESET) {
    reset_checker = param.asInt();
    if (reset_checker) last_reset_hold = millis();
}

BLYNK_WRITE(PIN_RESTART) {
    restart_checker = param.asInt();
    if (restart_checker) last_restart_hold = millis();
}

BLYNK_WRITE(PIN_PH_TARGET) {
    Ph.target = param.asFloat();
}

BLYNK_WRITE(PIN_SYNC_CLOCK) {
    sync_clock = param.asInt();
}

BLYNK_WRITE(PIN_GOODRANGE) {
    good_range = param.asFloat();
}


void wm_setup(void) {
    WifiMgr.setDarkMode(WM_DARKMODE);
    WifiMgr.setDebugOutput(WM_DEBUG);
    WifiMgr.setSaveConfigCallback(wm_savecallback);
    WifiMgr.setConfigPortalBlocking(!WM_NONBLOCKING);
}

void wm_reset(void) {
    logger("Resetting...");
    Blynk.virtualWrite(PIN_STATUS, State.resetting);
    WifiMgr.resetSettings();
    ESP.restart();
}

void wm_savecallback(void) {
    restart();
}


void checks(void) {
    reset_check();
    restart_check();
}

void reset_check(void) {
    if (reset_checker && (millis() - last_reset_hold >= DANGER_HOLD_SEC * 1000)) {
        wm_reset();
    }
}

void restart_check(void) {
    if (restart_checker && (millis() - last_restart_hold >= DANGER_HOLD_SEC * 1000)) {
        restart();
    }
}


void restart(void) {
    do_sync = false;

    while (restart_checker && Blynk.connected()) {
        Blynk.run();
        digitalWrite(BUILTIN_LED, LOW);
        delay(10);
    }
    digitalWrite(BUILTIN_LED, HIGH);
    if (Blynk.connected()) Blynk.virtualWrite(PIN_STATUS, State.restarting);

    logger("Restarting...");
    ESP.restart();
}

void reset(void) {
    working = 0;
    percent = 0;

    Blynk.virtualWrite(PIN_WORKER, 0);
    Blynk.virtualWrite(PIN_PROCESS, 0);
    Blynk.virtualWrite(PIN_STATUS, State.idle);
}

void stop(void) {
    reset();
}

void sync(void) {
    if (do_sync) {
        if (millis() - last_sync >= sync_clock * 1000) {
            // Send data every sync_clock

            // debug("pH:" + String(Ph.value) + ", Temp:" + String(Temp.value));

            last_sync = millis();
            digitalWrite(BUILTIN_LED, LOW);

            Blynk.virtualWrite(PIN_PH, Ph.value);
            Blynk.virtualWrite(PIN_TEMP, Temp.value);

            digitalWrite(BUILTIN_LED, HIGH);

            if (working) {
                Blynk.virtualWrite(PIN_STATUS, State.working);
                Blynk.virtualWrite(PIN_PROCESS, percent);
            }
            else Blynk.virtualWrite(PIN_STATUS, State.idle);
        } else {
            digitalWrite(BUILTIN_LED, HIGH);
        }
    }
}

void get_sensor(void) {
    Ph.get();
    Temp.get();
}

void run_process(void) {
    get_sensor();

    digitalWrite(LED, working);

    if (working) {
        /**
         * @brief first, compare ph value
         * 
         * if Ph value in +- good range
         *      done()
         * else 
         *      if Ph.value < Ph.target
         *          acid()
         *      else // value > target
         *          base()
         */

        // BUG

        if (((Ph.target - good_range) < Ph.value) && (Ph.value < (Ph.target + good_range))) {
            stop();
            logger("done.");
        } else {
            percent = Ph.percent();

            if (Ph.value < Ph.target) {
                AcidPump.run();
                BasePump.stop();
            } else {
                BasePump.run();
                AcidPump.stop();
            }
            FlowPump.run();
        }
    } else {
        /**
         * @brief then, stop all motors
         * 
         */

        BasePump.stop();
        AcidPump.stop();
        FlowPump.stop(5000);
    }
}

void run_tester(void) {
    get_sensor();

    if (working) {
        /**
         * @brief run motor with "run_time", "end_time" parameters
         */

        BasePump.run(RUN_TIME, 1000);
    } else {
        /**
         * @brief stop motor with "after" parameter
         * 
         */

        BasePump.stop(3000);
    }
}


void setup(void) {
    pinMode(LED, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

    if (DEBUG) {
        Serial.println("Debug mode, auto connecting...");

        Blynk.begin(
            BLYNK_AUTH_TOKEN, 
            SSID, 
            PASS, 
            BLYNK_CLOUD, 
            80
        );
    } else {
        wm_setup();

        if (WifiMgr.autoConnect(WM_SSID)) {
            Blynk.begin(
                BLYNK_AUTH_TOKEN, 
                WifiMgr.getWiFiSSID().c_str(), 
                WifiMgr.getWiFiPass().c_str(), 
                BLYNK_CLOUD, 
                80
            );
        } else {
            debug("Connect failed");
            Serial.println("Connect to " + String(WM_SSID) + " for wifi configuration.");
        }
    }

    last_sync = millis();
    get_sensor();
    debug("Setup completed");
}

void loop(void) {
    dynamic_delay();

    if (TEST) run_tester();
    else run_process();

    Blynk.run();

    if (Blynk.connected()) {
        sync();
    } else {
        digitalWrite(BUILTIN_LED, LOW);
    }
    if (WM_NONBLOCKING) WifiMgr.process();

    checks();
}