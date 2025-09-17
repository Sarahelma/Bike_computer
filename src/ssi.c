#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "stopwatch.h"
#include "hallsensor.h"
#include "mpu6050.h"

const char * __not_in_flash("httpd") bike_tags[] = {
    "distance", // 0
    "time", // 1
    "speed", // 2
    "incline" // 3
};

u16_t __time_critical_func(ssi_handler)(int iIndex, char *pcInsert, int iInsertLen)
{
    size_t printed;
    
    switch (iIndex) {
        case 0: 
        // distance
            printed = snprintf(pcInsert, iInsertLen, "%f", get_distance());
            break;
            
        case 1: 
            {
                uint32_t total_seconds = get_ride_time_secs();
                uint32_t hours = total_seconds / 3600;
                uint32_t minutes = (total_seconds % 3600) / 60;
                uint32_t seconds = total_seconds % 60;
                printed = snprintf(pcInsert, iInsertLen, "%02lu:%02lu:%02lu", 
                                 hours, minutes, seconds);
            }
            break;
            
        case 2: 
            printed = snprintf(pcInsert, iInsertLen, "%3f", get_max_speed());
            break;
            
        case 3: 
            printed = snprintf(pcInsert, iInsertLen, "+%3f%%", get_max_incline());
            break;
            
        default: /* unknown tag */
            printed = 0;
            break;
    }
    LWIP_ASSERT("sane length", printed <= 0xFFFF);
    return (u16_t)printed;
};

void ssi_init()
{

    for (size_t i = 0; i < LWIP_ARRAYSIZE(bike_tags); i++) {
        LWIP_ASSERT("tag too long for LWIP_HTTPD_MAX_TAG_NAME_LEN",
                    strlen(bike_tags[i]) <= LWIP_HTTPD_MAX_TAG_NAME_LEN);
    }

      http_set_ssi_handler(ssi_handler,
                           bike_tags, LWIP_ARRAYSIZE(bike_tags)
      );
};
