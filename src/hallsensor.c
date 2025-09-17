#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stopwatch.h"
#include "states.h"
#include "mpu6050.h"

static volatile uint32_t current_tick = 0;
static volatile uint32_t last_tick = 0;
static volatile uint32_t tick_counter = 0;
static volatile float max_speed = 0.0;

// triggered on hall interrupt
void hall_callback(uint gpio, uint32_t __event) {
    last_tick = current_tick; // stores last tick time
    current_tick = get_absolute_time(); // records current tick time
    tick_counter++;
    // on each hall interrupt we log time, tick count and incline
    log_hall(current_tick, tick_counter, get_roll_avg());
}

// calculates frequency in Hz
float get_frequency(void) {
    float freq = 1.0 / (absolute_time_diff_us(last_tick, current_tick) / 1000000.0);
    return freq;
}
// calculates speed in mph
float get_speed(float freq) {
    float mph = (freq * Wheel * 3600 * PI) / 1609.344;
    if (100 > mph && mph > max_speed) {
        max_speed = mph;
    }
    return mph;
}
// calculates distance in km
float get_distance(void) {
    float distance = tick_counter * Wheel * PI / 1000.0;
    return distance;
}
float get_max_speed(void) {
    return max_speed; 
}
void reset_hall(void) {
    tick_counter = 0;
    current_tick = 0;
    last_tick = 0;
}