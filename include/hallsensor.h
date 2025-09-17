#ifndef HALLSENSOR_H
#define HALLSENSOR_H

#include "pico/stdlib.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void hall_callback(uint gpio, uint32_t __event);
float get_frequency(void);
float get_speed(float freq);
float get_distance(void);
void reset_hall(void);
float get_max_speed(void);

#ifdef __cplusplus
}
#endif


#endif // HALLSENSOR_H