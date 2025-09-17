#ifndef STOPWATCH_H
#define STOPWATCH_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void start_stopwatch(void);
void stop_stopwatch(void);
uint32_t get_stopwatch_secs(void);
uint64_t get_ride_time_secs(void);


#ifdef __cplusplus
}
#endif

#endif // STOPWATCH_H