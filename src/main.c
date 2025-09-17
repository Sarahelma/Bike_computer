#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "pico/cyw43_driver.h"
#include "lwipopts.h"
#include "ssi.h"
#include "oled.h"
#include "stopwatch.h"
#include "states.h"
#include "hallsensor.h"
#include "mpu6050.h"


void run_server() {
    httpd_init();
    ssi_init();
    printf("Http server initialized.\n");
    // infinite loop for now
    for (;;) {}
}

int main() {
    // init buttons and hall sensor
    gpio_init(red);
    gpio_init(green);
    gpio_init(black);
    gpio_init(hall);

    gpio_set_dir(red, GPIO_IN);
    gpio_set_dir(green, GPIO_IN);
    gpio_set_dir(black, GPIO_IN);
    gpio_set_dir(hall, GPIO_IN);

    gpio_pull_down(green);
    gpio_pull_down(black);
    gpio_pull_down(red);
    gpio_pull_down(hall);

    stdio_init_all();
    oled_init();
    mpu6050_init();
    oled_init();
    
    static repeating_timer_t oled_timer;

    //used for debouncing button presses
    bool prev_red = false;
    bool prev_green = false;
    bool prev_black = false;
    
    absolute_time_t last_button_press = get_absolute_time();
    const uint32_t debounce_delay_us = 500000; // 500ms in microseconds

    states state = HOME;

    // flags for state timers
    bool timer_flag = false;
    bool speed_flag = false;
    bool distance_flag = false;
    bool incline_flag = false;
    static absolute_time_t lasttime = {0};

    // hall sensor interrupt (normally high)
    gpio_set_irq_enabled_with_callback(hall, GPIO_IRQ_EDGE_FALL, true, &hall_callback);

    while(1){
    absolute_time_t now = get_absolute_time();

    // reads mpu6050 every 2ms
    if (absolute_time_diff_us(lasttime, now) >= 2000) {
        lasttime = now;
        mpu_timer_update();
    }
    // reads buttons
    bool cur_red = gpio_get(red);
    bool cur_green = gpio_get(green);
    bool cur_black = gpio_get(black);
    
    bool red_pressed = !prev_red && cur_red && absolute_time_diff_us(last_button_press, now) > debounce_delay_us;
    bool green_pressed = !prev_green && cur_green && absolute_time_diff_us(last_button_press, now) > debounce_delay_us;
    bool black_pressed = !prev_black && cur_black && absolute_time_diff_us(last_button_press, now) > debounce_delay_us;
    
    if (red_pressed || green_pressed || black_pressed) {
        last_button_press = now;
    }
    
    prev_red = cur_red;
    prev_green = cur_green;
    prev_black = cur_black;
    

    // state machine depending on which button is pressed
    switch (state) {
        case HOME:
            oled_home();
            sleep_ms(500);
            if (green_pressed) {
                state = STOPWATCH;
                start_stopwatch();
                reset_hall();
            }
            break;
        case STOPWATCH:
            if (!timer_flag) {
                // updates oled every 100ms and sets flag, flag is cleared when leaving state
                // same applies for other pages
                add_repeating_timer_ms(100, oled_time, NULL, &oled_timer);
                timer_flag = true;
            }
            if (red_pressed) {
                stop_stopwatch();
                cancel_repeating_timer(&oled_timer);
                timer_flag = false;
                state = STOP;
            }
            if (black_pressed) {
                cancel_repeating_timer(&oled_timer);
                timer_flag = false;
                state = SPEED;
            }
            break;
        case SPEED:
            if (!speed_flag) {
                add_repeating_timer_ms(100, oled_speed, NULL, &oled_timer);
                speed_flag = true;
            }
            if (red_pressed) {
                stop_stopwatch();
                cancel_repeating_timer(&oled_timer);
                timer_flag = false;
                state = STOP;
            }
            if (black_pressed) {
                cancel_repeating_timer(&oled_timer);
                speed_flag = false;
                state = DISTANCE;
            }
            break;
        case DISTANCE:
            if (!distance_flag) {
                add_repeating_timer_ms(100, oled_distance, NULL, &oled_timer);
                distance_flag = true;
            }
            if (red_pressed) {
                cancel_repeating_timer(&oled_timer);
                distance_flag = false;
                state = STOP;
            }
            if (black_pressed) {
                cancel_repeating_timer(&oled_timer);
                distance_flag = false;
                state = INCLINE;
            }
            break;
        case INCLINE:
            if (!incline_flag) {
                add_repeating_timer_ms(100, oled_incline, NULL, &oled_timer);
                incline_flag = true;
            }
            if (red_pressed) {
                cancel_repeating_timer(&oled_timer);
                incline_flag = false;
                state = STOP;
            }
            if (black_pressed) {
                cancel_repeating_timer(&oled_timer);
                incline_flag = false;
                state = STOPWATCH;
            }
            break;
        case STOP:
        // stops stopwatch and hall sensor
            oled_end();
            gpio_set_irq_enabled(hall, GPIO_IRQ_EDGE_FALL, false);
            // connects to wifi and runs server
            if (cyw43_arch_init()) {
                printf("failed to initialise\n");
                return 1;
            }

            cyw43_arch_enable_sta_mode();
            cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_NO_POWERSAVE_MODE, 20, 1, 1, 1));
            printf("Connecting to WiFi...\n");
            printf("Connecting to WiFi...\n");
            if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
            printf("failed to connect.\n");
            return 1;
            } else {
            printf("Connected.\n");
            extern cyw43_t cyw43_state;
            uint32_t ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
            printf("IP Address: %lu.%lu.%lu.%lu\n", ip_addr & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr >> 16) & 0xFF, ip_addr >> 24);
            }
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            oled_stop();
            run_server();

            break;
        default:
            printf("No valid option selected.\n");
            return 1;
    }
}
}