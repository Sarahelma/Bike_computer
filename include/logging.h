#ifndef LOGGING_H
#define LOGGING_H

#include "pico/time.h"
#define MAX_POINTS 10000
void log_hall(absolute_time_t timestamp, uint32_t tick_count, float roll_avg);

#endif // LOGGING_H