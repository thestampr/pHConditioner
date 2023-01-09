// main source code

#define BLYNK_PRINT Serial

#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

#include "config.h"
#include "secret.h"
#include "utils.h"

#include "Models\pHSensor.h"
#include "Models\TempSensor.h"


int working;
int rst_checker;
int update_timer = 1;

float ph_target = 7.0;
float rst_hold_counter;
float update_timer_counter;

SimpleTimer Timer;
WiFiManager WifiMgr;

TempSensor Temp(ONE_WIRE_BUS);
pHSensor Ph(ANALOG_PH);


BLYNK_CONNECTED() {
    Blynk.syncAll();
    Blynk.virtualWrite(PIN_VERSION, VERSION);
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
    rst_checker = param.asInt();
}

BLYNK_WRITE(PIN_PH_TARGET) {
    ph_target = param.asFloat();
}

BLYNK_WRITE(PIN_UPDATE_TIMER) {
    update_timer = param.asInt();
}


void wm_setup(void) {
    WifiMgr.setDarkMode(WM_DARKMODE);
    WifiMgr.setDebugOutput(WM_DEBUG);
    WifiMgr.setSaveConfigCallback(wm_savecallback);
    WifiMgr.setConfigPortalBlocking(!WM_NONBLOCKING);
}

void wm_reset(void) {
    debug("Resetting...");

    WifiMgr.resetSettings();
    ESP.restart();
}

void wm_savecallback(void) {
    debug("Restarting...");
    ESP.restart();
}


void rstcheck(void) {
    if (rst_checker) {
        rst_hold_counter += DELAY_TIME/100;
    } else {
        rst_hold_counter = 0;
    }
    if (rst_checker && (rst_hold_counter >= DELAY_TIME * RST_HOLDSECOND)) {
        wm_reset();
    }
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
    reset();
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
        update();
    } else {
        digitalWrite(BUILTIN_LED, LOW);
    }

    rstcheck();
    delay(DELAY_TIME);
}