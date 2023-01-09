// Project constant

#pragma once

#define VERSION "2.0.0"

// Debug
#define DEBUG 1
#define WM_DEBUG 0

// Pins
#define LED D0
#define ONE_WIRE_BUS D5
#define ANALOG_PH A0

// Virtual pins
#define PIN_VERSION V0
#define PIN_WORKER V1
#define PIN_TEMP V2
#define PIN_PH V3
#define PIN_PH_TARGET V4
#define PIN_PROCESS V5
#define PIN_UPDATE_TIMER V6

#define BLYNK_CLOUD "blynk.cloud"

#define WM_SSID "PH-CON"
#define WM_NONBLOCKING 1
#define WM_DARKMODE 1

const float DELAY_TIME = 10;

const int EEPROM_MAX_LENGTH = 32;