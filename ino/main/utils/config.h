// config.h

#pragma once

#define VERSION "2.3.0"

// Debug
#define DEBUG 1
#define WM_DEBUG 0
#define RAW_ANALOG 1
#define RAW_SENSOR 0
#define TEST 0

#define CLEANUP 0

// worker
#define NEW_WORKER 0

// Pins
#define LED D0
#define ONE_WIRE_BUS D5
#define ANALOG_PH A0

#define ACID_PIN D1
#define BASE_PIN D2
#define FLOW_PIN D3
#define MGND_PIN D6 // To prevent from blinking on startup, in needed ?

// Virtual pins
#define PIN_VERSION V0
#define PIN_WORKER V1
#define PIN_TEMP V2
#define PIN_PH V3
#define PIN_PH_TARGET V4
#define PIN_PROCESS V5
#define PIN_SYNC_CLOCK V6
#define PIN_RESET V7
#define PIN_RESTART V8
#define PIN_GOOD_RANGE V9

#define BLYNK_CLOUD "blynk.cloud"

#define WM_SSID "PH-CON"
#define WM_NONBLOCKING 1
#define WM_DARKMODE 1

#define DANGER_HOLD_SEC 3 // seconds

#define DELAY_TIME 250 // milliseconds