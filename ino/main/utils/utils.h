// utils.h

#pragma once

#include "config.h"


/**
 * Prints a debug message to the serial port.
 *
 * @param message The debug message or data to be printed.
 */
template <typename Printable>
void debug(Printable message) {
    if (DEBUG) Serial.println("DEBUG: " + String(message));
}

/**
 * Prints a logger message to the serial port and blynk if connected.
 *
 * @param message The debug message or data to be printed.
 */
template <typename Printable>
void logger(Printable message) {
    Serial.println("LOG: " + String(message));
    // if (Blynk.connected()) Blynk.virtualWrite(PIN_TERMINAL, String(message));
}

/**
 * Maps a float value from one range to another.
 * 
 * @param x The value to map.
 * @param in_min The minimum of the input range.
 * @param in_max The maximum of the input range.
 * @param out_min The minimum of the output range.
 * @param out_max The maximum of the output range.
 * @return The mapped value.
 */
float float_map(float x, float in_min, float in_max, float out_min, float out_max) {
    float out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    
    if (out < out_min) out = out_min;
    else if (out > out_max) out = out_max;

    return out;
}

/**
 * Converts a given number to a character array and removes leading zeros.
 * 
 * @param num The number to be converted.
 * @param buffer The character array to store the converted number.
 * @param digit The number of digits in the converted number.
 */
void num_to_char(unsigned long num, char *buffer, unsigned char digit) {
    char i;
    // Convert the number to a character array
    for (i=(digit-1); i>=0; i--) {
        buffer[i] = (num % 10) + '0';  // 234 , 23 , 2
        num = num / 10;
    }  
    // Remove leading zeros
    for (i=0; i<(digit-1); i++) {
        if (buffer[i] == '0') {
            buffer[i] =  ' ';
        } else {
            i = 100;
        }
    }   
}