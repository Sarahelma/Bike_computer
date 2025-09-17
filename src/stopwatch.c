#include <stdint.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/time.h>

static uint64_t global_start_time;
static uint64_t global_stop_time;

void start_stopwatch(){
    global_start_time = get_absolute_time();
}
void stop_stopwatch(){
    global_stop_time = get_absolute_time();
}

uint32_t get_stopwatch_secs()
{
    return absolute_time_diff_us(global_start_time, get_absolute_time()) / 1000000;

}

uint64_t get_ride_time_secs()
{
    return ((global_stop_time - global_start_time) / 1000000);
}


