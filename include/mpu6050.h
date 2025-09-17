#ifndef MPU6050_H
#define MPU6050_H

#define MPU6050_ADDR 0x68
#define AVG_SAMPLES 50
#define SAMPLES 200
#define twoG 16384.0

#ifdef __cplusplus
extern "C" {
#endif
void mpu6050_init(void);
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3]);
float get_incline(int16_t accel[3], int16_t gyro[3]);
void calibrate_gyro(void);
void mpu_timer_update(void);
float get_roll_avg(void);
void calibrate_accel(void);
float get_max_incline(void);
#ifdef __cplusplus
}
#endif

#endif