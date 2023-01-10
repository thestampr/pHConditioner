// main source code

#define BLYNK_PRINT Serial

#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

#include "utils\config.h"
#include "utils\secret.h"
#include "utils\utils.h"
#include "utils\enums.h"

#include "Models\pHSensor.h"
#include "Models\TempSensor.h"


int working;
int rss_checker; // reset
int rst_checker; // restart
int sync_clock = 1;

float ph_target = 7.0;
float rss_hold_counter;
float rst_hold_counter;
float sync_clock_counter;

bool do_sync = false;

SimpleTimer Timer;
WiFiManager WifiMgr;

TempSensor Temp(ONE_WIRE_BUS);
pHSensor Ph(ANALOG_PH);


BLYNK_CONNECTED() {
    debug("Blynk connectd");
    Blynk.syncAll();
    Blynk.virtualWrite(PIN_VERSION, VERSION);
    Blynk.virtualWrite(PIN_STATUS, State.connected);
    do_sync = true;
}

BLYNK_DISCONNECTED() {
    debug("Blynk disconnectd");
    do_sync = false;
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

BLYNK_WRITE(PIN_RESET) {
    rss_checker = param.asInt();
}

BLYNK_WRITE(PIN_RESTART) {
    rst_checker = param.asInt();
}

BLYNK_WRITE(PIN_PH_TARGET) {
    ph_target = param.asFloat();
}

BLYNK_WRITE(PIN_SYNC_CLOCK) {
    sync_clock = param.asInt();
}


void wm_setup(void) {
    WifiMgr.setDarkMode(WM_DARKMODE);
    WifiMgr.setDebugOutput(WM_DEBUG);
    WifiMgr.setSaveConfigCallback(wm_savecallback);
    WifiMgr.setConfigPortalBlocking(!WM_NONBLOCKING);
}

void wm_reset(void) {
    debug("Resetting...");
    Blynk.virtualWrite(PIN_STATUS, State.resetting);
    WifiMgr.resetSettings();
    ESP.restart();
}

void wm_savecallback(void) {
    restart();
}


void rsscheck(void) {
    if (rss_checker) {
        rss_hold_counter += DELAY_TIME/100;
    } else {
        rss_hold_counter = 0;
    }
    if (rss_checker && (rss_hold_counter >= DELAY_TIME * DANGER_HOLD_SEC)) {
        wm_reset();
    }
}

void rstcheck(void) {
    if (rst_checker) {
        rst_hold_counter += DELAY_TIME/100;
    } else {
        rst_hold_counter = 0;
    }
    if (rst_checker && (rst_hold_counter >= DELAY_TIME * DANGER_HOLD_SEC)) {
        restart();
    }
}


void restart(void) {
    debug("Restarting...");
    do_sync = false;
    if (Blynk.connected()) Blynk.virtualWrite(PIN_STATUS, State.restarting);
    ESP.restart();
}

void reset(void) {
    working = 0;

    Blynk.virtualWrite(PIN_WORKER, 0);
    Blynk.virtualWrite(PIN_PROCESS, 0);
    Blynk.virtualWrite(PIN_STATUS, State.idle);
}

void stop(void) {
    reset();
}

void sync(void) {
    if (do_sync) {
        sync_clock_counter += DELAY_TIME/100;

        if (sync_clock_counter >= DELAY_TIME * sync_clock) {
            // Send data every sync_clock

            // debug("pH:" + String(Ph.value) + ", Temp:" + String(Temp.value));

            digitalWrite(BUILTIN_LED, LOW);

            Blynk.virtualWrite(PIN_PH, Ph.value);
            Blynk.virtualWrite(PIN_TEMP, Temp.value);

            if (working) Blynk.virtualWrite(PIN_STATUS, State.working);
            else Blynk.virtualWrite(PIN_STATUS, State.idle);

            sync_clock_counter = 0;
        } else {
            digitalWrite(BUILTIN_LED, HIGH);
        }
    }
}


void setup(void) {
    pinMode(LED, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    wm_setup();

    if (WifiMgr.autoConnect(WM_SSID)) {
        Blynk.begin(
            BLYNK_AUTH_TOKEN, 
            WifiMgr.getWiFiSSID().c_str(), 
            WifiMgr.getWiFiPass().c_str(), 
            BLYNK_CLOUD, 
            80
        );
        debug("Connected");
    } else {
        debug("Connect failed");
        Serial.println("Connect to " + String(WM_SSID) + " for wifi configuration.");
    }

    Timer.setInterval(1000L, [&]() { Ph.get(); });
    Timer.setInterval(1000L, [&]() { Temp.get(); });
}

void loop(void) {
    if (WM_NONBLOCKING) WifiMgr.process();

    Blynk.run();
    Timer.run();

    if (Blynk.connected()) {
        sync();
    } else {
        digitalWrite(BUILTIN_LED, LOW);
    }

    rstcheck();
    rsscheck();
    delay(DELAY_TIME);
}