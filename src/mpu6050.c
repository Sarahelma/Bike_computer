#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"

static int16_t gyro_offset[3] = {0};
//static int16_t accel_offset[3] = {0};
static double roll = 0.0;  
static absolute_time_t last_time;
static double roll_buffer[AVG_SAMPLES];
static int roll_index = 0;
static double roll_sum = 0;
static double roll_avg = 0;
static double max_incline = 0.0;


// setup I2C and calibrate
void mpu6050_init(void) {
    i2c_init(i2c0, 100000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    uint8_t buf[2];

    // reset 
    buf[0] = 0x6B; buf[1] = 0x80;
    i2c_write_blocking(i2c0, MPU6050_ADDR, buf, 2, false);
    sleep_ms(100);

    // wake up
    buf[1] = 0x00;
    i2c_write_blocking(i2c0, MPU6050_ADDR, buf, 2, false);
    sleep_ms(100);

    for(int i = 0; i < AVG_SAMPLES; i++) roll_buffer[i] = 0.0;
    roll_sum = 0.0;


    calibrate_gyro();
    last_time = get_absolute_time();
}


void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3]) {
    // Reads 14-byte message
    uint8_t buf[14];
    uint8_t reg = 0x3B; // ACCEL_XOUT_H
    i2c_write_blocking(i2c0, MPU6050_ADDR, &reg, 1, false);
    i2c_read_blocking(i2c0, MPU6050_ADDR, buf, 14, false);

    // accel 
    accel[0] = (int16_t)(buf[0] << 8 | buf[1]);
    accel[1] = (int16_t)(buf[2] << 8 | buf[3]);
    accel[2] = (int16_t)(buf[4] << 8 | buf[5]);
    // don't care about temp bytes
    // gyro
    gyro[0] = (int16_t)(buf[8] << 8 | buf[9]);
    gyro[1] = (int16_t)(buf[10] << 8 | buf[11]);
    gyro[2] = (int16_t)(buf[12] << 8 | buf[13]);
}

void calibrate_gyro(void) {
    int32_t sum[3] = {0};
    int16_t accel[3], gyro[3];

    // takes 500 samples to set gyro offset
    for(int i=0;i<500;i++) {
        mpu6050_read_raw(accel, gyro);
        for(int j=0;j<3;j++) sum[j]+=gyro[j];
        sleep_ms(10);
    }
    for(int j=0;j<3;j++) gyro_offset[j] = sum[j]/500;
}

/* this didn't work well
void calibrate_accel(void) {
    int32_t sum[3] = {0};
    int16_t accel[3], gyro[3];

    for(int i=0; i<100; i++) {
        mpu6050_read_raw(accel, gyro);
        for(int j=0; j<3; j++) sum[j] += accel[j];
        sleep_ms(10);
    }
    for(int j=0; j<3; j++) accel_offset[j] = sum[j] / 100;
}
*/


// calculateroll with complementary filter
float get_incline(int16_t accel[3], int16_t gyro[3]){
    absolute_time_t now = get_absolute_time();
    float dt = (float)absolute_time_diff_us(last_time, now) / 1000000.0;
    last_time = now;

    float ay = (accel[1]) / twoG;
    float az = (accel[2]) / twoG;

    float roll_acc = atan2(ay, az) * 180.0 / M_PI;

    float gx = ((float)gyro[0] - gyro_offset[0]) / 131.0;

    roll = 0.98 * (roll + gx*dt) + 0.02 * roll_acc;
    return roll;
}


// we call this every 20ms to update roll average
void mpu_timer_update(void) {
    static int16_t accel[3], gyro[3];

    mpu6050_read_raw(accel, gyro);
    float roll_now = get_incline(accel, gyro);

    roll_sum -= roll_buffer[roll_index];
    roll_buffer[roll_index] = roll_now;
    roll_sum += roll_now;

    roll_index = (roll_index + 1) % AVG_SAMPLES;
    roll_avg = roll_sum / AVG_SAMPLES;
    if (roll_avg > max_incline) {
        max_incline = roll_avg;
    }
}

// to display average roll on oled
float get_roll_avg(void) {
    return roll_avg;
}

float get_max_incline(void) {
    return max_incline;
}