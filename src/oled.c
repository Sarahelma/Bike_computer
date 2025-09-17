#include <stdint.h>
#include <stdio.h>
#include "ssd1306.h"
#include "pico/cyw43_arch.h"
#include "pico/cyw43_driver.h"
#include "lwipopts.h"
#include "stopwatch.h"
#include "hallsensor.h"
#include "states.h"
#include "mpu6050.h"


ssd1306_t disp;

void oled_init(void)
{
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8,12,2, "PLEASE");
    ssd1306_draw_string(&disp, 8,30,2, "WAIT ...");
    ssd1306_show(&disp);

};


void oled_home(void){
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8,12,2, "READY");
    ssd1306_draw_string(&disp, 8,30,2, "TO START");
    ssd1306_show(&disp);
}

// stopwatch display page
bool oled_time(repeating_timer_t *rt) {
    uint64_t secs = get_stopwatch_secs();
    uint64_t mins = secs / 60;

    char buf[32];
    snprintf(buf, sizeof(buf), "%02llu:%02llu", mins, secs % 60);

    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8, 12, 2, "TIME:");
    ssd1306_draw_string(&disp, 8, 30, 3, buf);
    ssd1306_show(&disp);
    return true; 
};

// speed display page 
bool oled_speed(repeating_timer_t *rt){
    float speed = get_speed(get_frequency());
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f ", speed);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8,12,2, "SPEED:");
    ssd1306_draw_string(&disp, 8, 30, 3, buf);
    ssd1306_draw_string(&disp, 80, 30, 2, "mph");
    ssd1306_show(&disp);
    return true;
};

// distance display page
bool oled_distance(repeating_timer_t *rt){
    float distance = get_distance();
    char buf[16];
    snprintf(buf, sizeof(buf), "%.3f", distance);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8,12,2, "DISTANCE:");
    ssd1306_draw_string(&disp, 8, 30, 3, buf);
    ssd1306_draw_string(&disp, 100, 30, 2, "km");
    ssd1306_show(&disp);
    return true;
};
// incline display page
bool oled_incline(repeating_timer_t *rt){
    char buf[16];
    float incline = get_roll_avg();
    snprintf(buf, sizeof(buf), "%.2f ° ", incline);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8,12,2, "INCLINE:");
    ssd1306_draw_string(&disp, 8, 30, 3, buf);
    ssd1306_show(&disp);
    return true;
};

// page after ride ends and before wifi is established
void oled_end(){
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8,12,2, "Connecting");
    ssd1306_draw_string(&disp, 8,30,2, "To WiFi...");
    ssd1306_show(&disp);
}
// page after wifi is established
void oled_stop(){
    char buf[16];
    uint32_t ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
    snprintf(buf, sizeof(buf), "%lu.%lu.%lu.%lu",
             ip_addr & 0xFF,
             (ip_addr >> 8) & 0xFF,
             (ip_addr >> 16) & 0xFF,
             (ip_addr >> 24) & 0xFF);

    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8,12,2, "View Stats:");
    ssd1306_draw_string(&disp, 8,45,1, buf);
    ssd1306_show(&disp);
};

