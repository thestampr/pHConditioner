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
    // If the time until calling the function again is less than DELAY_TIME, delay for the remaining time.

    blocking_code_runtime = millis();
    if (blocking_code_endtime > blocking_code_runtime) {
        delay(blocking_code_endtime - millis());
    }
    blocking_code_endtime = blocking_code_runtime + DELAY_TIME;
}

void get_sensor(void) {
    // Get value of sensors

    Ph.get();
    Temp.get();
}


BLYNK_CONNECTED() {
    // Sync priority
    Blynk.syncVirtual(PIN_PH_TARGET);
    Blynk.syncAll();

    Blynk.virtualWrite(PIN_VERSION, VERSION);

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
    if (working) {
        if (do_sync) {
            Ph.start = Ph.value;
        } else {
            Ph.start = Ph.get();
        }
        logger("Working, pHtarget=" + String(Ph.target));
    } else {
        stop_process();
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

BLYNK_WRITE(PIN_GOOD_RANGE) {
    good_range = param.asFloat();
}


void wm_setup(void) {
    // Setup WifiManager

    WifiMgr.setDarkMode(WM_DARKMODE);
    WifiMgr.setDebugOutput(WM_DEBUG);
    WifiMgr.setSaveConfigCallback(wm_savecallback);
    WifiMgr.setConfigPortalBlocking(!WM_NONBLOCKING);
}

void wm_reset(void) {
    // Reset wifi

    stop();

    while (reset_checker && Blynk.connected()) {
        Blynk.run();
        digitalWrite(BUILTIN_LED, LOW);
        delay(10);
    }
    digitalWrite(BUILTIN_LED, HIGH);

    logger("Resetting...");
    WifiMgr.resetSettings();
    ESP.restart();
}

void wm_savecallback(void) {
    // WifiManager saved callback

    restart();
}


void checks(void) {
    // Check if any button-hold is held for DANGER_HOLD_SEC seconds is being executed

    reset_check();
    restart_check();
}

void reset_check(void) {
    // Check if wifi-reset button is held for DANGER_HOLD_SEC seconds

    if (reset_checker && (millis() - last_reset_hold >= DANGER_HOLD_SEC * 1000)) {
        wm_reset();
    }
}

void restart_check(void) {
    // Check if restart button is held for DANGER_HOLD_SEC seconds

    if (restart_checker && (millis() - last_restart_hold >= DANGER_HOLD_SEC * 1000)) {
        restart();
    }
}


void sync(void) {
    // Sync data to blynk

    if (do_sync) {
        unsigned long now = millis();
        if (now - last_sync >= sync_clock * 1000) {
            // Sync every sync_clock minuite

            last_sync = now;

            digitalWrite(BUILTIN_LED, LOW);
            Blynk.virtualWrite(PIN_PH, Ph.value);
            Blynk.virtualWrite(PIN_TEMP, Temp.value);
            digitalWrite(BUILTIN_LED, HIGH);

            Blynk.virtualWrite(PIN_PROCESS, percent);
        } else {
            digitalWrite(BUILTIN_LED, HIGH);
        }
    }
}

void run_process(void) {
    // Main working process

    get_sensor();

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
            stop_process();
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

    digitalWrite(LED, working);
}

void stop_process(void) {
    // Stop working process

    reset();
}


void restart(void) {
    // Restart hardware

    stop();

    while (restart_checker && Blynk.connected()) {
        Blynk.run();
        digitalWrite(BUILTIN_LED, LOW);
        delay(10);
    }
    digitalWrite(BUILTIN_LED, HIGH);

    logger("Restarting...");
    ESP.restart();
}

void reset(void) {
    // Reset process

    working = 0;
    percent = 0;

    Blynk.virtualWrite(PIN_WORKER, 0);
}

void stop(void) {
    // Stop hardware, must call before hardware is being shutting down

    do_sync = false;
    
    BasePump.stop();
    AcidPump.stop();
    FlowPump.stop();
    digitalWrite(LED, LOW);
}


void setup(void) {
    // Hardware setup

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

    get_sensor();
    last_sync = millis();
    debug("Setup completed");
}

void loop(void) {
    // Main loop

    dynamic_delay();

    Blynk.run();
    run_process();

    if (Blynk.connected()) {
        sync();
    } else {
        digitalWrite(BUILTIN_LED, LOW);
    }
    if (WM_NONBLOCKING && !DEBUG) WifiMgr.process();

    checks();
}