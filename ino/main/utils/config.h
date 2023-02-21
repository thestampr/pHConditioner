// config.h

#pragma once

#define VERSION "2.2.5"

// Debug
#define DEBUG 1
#define WM_DEBUG 0
#define RAW_SENSOR 0
#define TEST 0

// Pins
#define LED D0
#define ONE_WIRE_BUS D5
#define ANALOG_PH A0

#define BASE_PIN D1
#define ACID_PIN D2
#define FLOW_PIN D3

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
#define PIN_GOOD_RANGE V9 // V11 for default
// #define PIN_STATUS V9
// #define PIN_TERMINAL V10
// Limit to 10 pins for free plan

#define BLYNK_CLOUD "blynk.cloud"

#define WM_SSID "PH-CON"
#define WM_NONBLOCKING 1
#define WM_DARKMODE 1

#define DANGER_HOLD_SEC 3 // seconds

#define DELAY_TIME 350 // milliseconds