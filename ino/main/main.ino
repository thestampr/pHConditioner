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

// ph range checker
int inrange_times;
float run_pump = PUMP_RUN_TIME;
float wait_pump = PUMP_WAIT_TIME;
float good_range = 0.1;

float percent;

bool do_sync = false;

unsigned long blocking_code_runtime;
unsigned long blocking_code_endtime;
unsigned long last_sync;
unsigned long last_reset_hold;
unsigned long last_restart_hold;

// if NEW_WORKER is set
int reading_state = 0;
int reading_runtime = 10000;
int reading_stoptime = 2000;
int change_worker_state;
unsigned long worker_state_runtime;
unsigned long worker_state_endtime;


WiFiManager WifiMgr;
TempSensor Temp(ONE_WIRE_BUS);
pHSensor Ph(ANALOG_PH);

// create motor object inside a code
MotorPump AcidPump(ACID_PIN); 
MotorPump BasePump(BASE_PIN);
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

bool is_inrange(void) {
    // check if ph value is in good range

    // if (((Ph.target - good_range) < Ph.value) && (Ph.value < (Ph.target + good_range))) {
    if (abs(Ph.target - Ph.value) <= good_range) {
        return true;
    }
    return false;
}


BLYNK_CONNECTED() {
    // connected event

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
    // disconncted event

    debug("Disconnected");
    do_sync = false;
}

BLYNK_WRITE(PIN_WORKER) {
    // worker pin event

    working = param.asInt();
    if (working && !is_inrange()) {
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

BLYNK_WRITE(PIN_PH_TARGET) {
    // ph target event

    Ph.target = param.asFloat();
}


// settings
BLYNK_WRITE(PIN_RESET) {
    // reset setting event

    reset_checker = param.asInt();
    if (reset_checker) last_reset_hold = millis();
}

BLYNK_WRITE(PIN_RESTART) {
    // restart setting event

    restart_checker = param.asInt();
    if (restart_checker) last_restart_hold = millis();
}

BLYNK_WRITE(PIN_SYNC_CLOCK) {
    // sync clock setting event

    sync_clock = param.asInt();
}

BLYNK_WRITE(PIN_GOOD_RANGE) {
    // good range setting event

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


int sync(void) {
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

            return 1;
        } else {
            digitalWrite(BUILTIN_LED, HIGH);
        }
    } 
    return 0;
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

        if (is_inrange()) {
            inrange_times++;
        } 

        if (inrange_times == CHECK_TIMES) {
            stop_process();
            logger("done.");
        } else {
            if (inrange_times) {
                run_pump = 500;
                wait_pump = 10000;
            }

            float _percent = Ph.percent();

            if (_percent > percent) {
                percent = _percent;
            }

            if (Ph.value < Ph.target) {
                BasePump.run(PUMP_RUN_TIME, wait_pump);
                AcidPump.stop();
            } else {
                AcidPump.run(PUMP_RUN_TIME, wait_pump);
                BasePump.stop();
            }
            FlowPump.run();
        }
    } else {
        /**
         * @brief then, stop all motors
         * 
         */

        AcidPump.stop();
        BasePump.stop();
        FlowPump.stop(10000);
    }

    digitalWrite(LED, working);
}

void run_process_v2(void) {
    // Main working process (rewrite V2)

    /**
     * @brief rewrite V2, to prevent from dc moter noise
     * then; dc mortor can't be working with reading sensor at the same time
     * 
     * if not working: read sensor by normal, like v1
     * else: switch between reading and motor worker
     * 
     */

    worker_state_runtime = millis();
    Temp.get();

    if (working) {
        if (worker_state_runtime > worker_state_endtime) {
            // state manager
            if (reading_state) {
                reading_state = 0;
                worker_state_endtime = worker_state_runtime + reading_stoptime;

                // getting the last value
                Ph.get();
                debug("Reading...");
            } else {
                reading_state = 1;
                worker_state_endtime = worker_state_runtime + reading_runtime;
            }
        }

        if (reading_state) {
            AcidPump.stop();
            BasePump.stop();
            FlowPump.stop();
            
            // if (worker_state_endtime != (worker_state_runtime + reading_runtime)) {
            //     // ignore first value after state was changed

            //     get_sensor();
            //     debug("Reading...");
            // }
        }

        else {
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

            if (abs(Ph.target - Ph.value) <= good_range) {
                stop_process();
                logger("done.");
            } else {
                percent = Ph.percent();

                if (Ph.value < Ph.target) {
                    BasePump.run(reading_stoptime);
                    AcidPump.stop();
                } else {
                    AcidPump.run();
                    BasePump.stop();
                }
                FlowPump.run();
                debug("Working...");
            }
        }
    } else {
        /**
         * @brief then, stop all motors
         * read senssor after all moter has stopped
         * 
         */

        if (reading_state) {
            // set new worker_state_endtime if all motors already stopped

            worker_state_endtime = worker_state_runtime + reading_stoptime;
            reading_state = 0;
        }
        
        if ((!AcidPump.is_running) && (!BasePump.is_running) && (!FlowPump.is_running)) {
            if (worker_state_runtime > worker_state_endtime) {
                // reading after worker_state_endtime

                Ph.get();
            }
        } else {
            // set new worker_state_endtime after all motor has stopped

            worker_state_endtime = worker_state_runtime + reading_stoptime;
        }

        AcidPump.stop();
        BasePump.stop();
        FlowPump.stop();
    }

    digitalWrite(LED, working);
}

void cleanup(void) {
    // cleanup tank

    if (working) {
        AcidPump.run(0);
        BasePump.run(0);
    } else {
        AcidPump.stop();
        BasePump.stop();
    }
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

    inrange_times = 0;
    run_pump = PUMP_RUN_TIME;
    wait_pump = PUMP_WAIT_TIME;

    Blynk.virtualWrite(PIN_WORKER, 0);
}

void stop(void) {
    // Stop hardware, must call before hardware is being shutting down

    do_sync = false;
    
    AcidPump.stop();
    BasePump.stop();
    FlowPump.stop();
    digitalWrite(LED, LOW);
}


void setup(void) {
    // Hardware setup

    pinMode(MGND_PIN, OUTPUT);
    digitalWrite(MGND_PIN, HIGH); // Open pump circuit to prevent blinking on startup.

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
    digitalWrite(MGND_PIN, LOW); // Then close circuit on ready.
    
    debug("Setup completed");
}

void loop(void) {
    // Main loop

    dynamic_delay();

    Blynk.run();

    if (CLEANUP) {
        cleanup();
    } else {
        if (NEW_WORKER == 0) {
            run_process();
        }
        else if(NEW_WORKER == 1) {
            run_process_v2();
        }
    }

    if (Blynk.connected()) {
        sync();
    } else {
        digitalWrite(BUILTIN_LED, LOW);
    }
    if (WM_NONBLOCKING && !DEBUG) WifiMgr.process();

    checks();
}