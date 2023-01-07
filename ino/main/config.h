// Project constant

#pragma once

// Debug
#define DEBUG 0

// Pins
#define LED D0
#define ONE_WIRE_BUS D5
#define ANALOG_PH A0

// Virtual pins
#define PIN_WORKER V0
#define PIN_TEMP V1
#define PIN_PH V2
#define PIN_PH_TARGET V3
#define PIN_PROCESS V4
#define PIN_UPDATE_TIMER V5

#define BLYNK_CLOUD "blynk.cloud"

const float DELAY_TIME = 10;

const int EEPROM_MAX_LENGTH = 32;