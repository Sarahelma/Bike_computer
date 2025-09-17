#ifndef OLED_H
#define OLED_H

#ifdef __cplusplus
extern "C" {
#endif
void oled_init(void);
void oled_home(void);
bool oled_time(repeating_timer_t *rt);
bool oled_speed(repeating_timer_t *rt);
bool oled_distance(repeating_timer_t *rt);
bool oled_incline(repeating_timer_t *rt);
void oled_stop(void);
void oled_end(void);

#ifdef __cplusplus
}
#endif

#endif // OLED_H

