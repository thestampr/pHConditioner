// Utils fuunction


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
    // Calculate the mapped value using the formula:
    // (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
