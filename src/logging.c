#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "logging.h"
#include "hallsensor.h"
#include "mpu6050.h"

struct data_point {
    absolute_time_t timestamp;
    uint32_t tick_count;
    float roll_avg;
};
struct data_point log[MAX_POINTS]; 
static size_t log_index = 0;
void log_hall(absolute_time_t timestamp, uint32_t tick_count, float roll_avg) {
    if (log_index < MAX_POINTS) {
        log[log_index].timestamp = timestamp;
        log[log_index].tick_count = tick_count;
        log[log_index].roll_avg = roll_avg;
        log_index++;
    }
}



