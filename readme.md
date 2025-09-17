A DIY bicycle datalogger made using a raspberry pi pico w (c/c++ sdk.) I thought this would be a nice way familiarise myself with the pico w's wireless features for future IoT projects using sensors/modules leftover from previous projects and learn how to use fusion 360. The deliverable will be a handbar mounted device that tracks variables such as the journey time, distance travelled, speed, incline and gps location, which will be accessable via a locally hosted webserver.

### Features
- [x] Wheel speed sensor  
- [x] Tilt Sensor  
- [x] Oled display  
- [x] Battery  
- [ ] Datalogging  
- [x] Webpage
- [ ] GPS route tracking  
- [ ] 3d printed enclosure

## Setup

## Webpage
The webpage server is based on this [example] (https://github.com/krzmaz/pico-w-webserver-example/tree/main) which has a helpful [blog](https://krzmaz.com/2022-08-15-creating-a-web-server-on-raspberry-pi-pico-w-using-pico-sdk-and-lwip/) to go with it. 

It uses lwIP to host a basic shtml webpage with variables as tags, which is implemented in ssi.cpp. These are updated with measured values when the red stop button is pressed. The journey datasets can also be downloaded from the webpage.

## OLED pages
On startup wait for the device to calbrate the IMU for a few seconds. 
It then moves on the start page, to start the journey press the GREEN button.
That will take you to the stopwatch page, using the BLACK button you can scroll through the speed, distance, incline and stopwatch pages.
Once your journey is complete, press the RED button. This will launch a web server and display the IP address to view the journey's summary and download datasets.

## Wheel speed 

To calculate speed a hall effect sensor is mounted on the front fork with a magnet mounted on the spokes. This creates a pulse on rotation, which triggers an interrupt. The time difference between pulses is used to calculate the revolutions per second and using the wheel diameter the speed in meters per second. A counter tracks the number of rotations to calculate the overall distance.


## Tilt Sensor

An MPU6050 Six-Axis (Gyro + Accelerometer) is used to determine the incline of the slope. The implementation I've used is based this arduino [library](https://github.com/rfetick/MPU6050_light/tree/master).

In MPU6050.c to calibrate the IMU at rest gyro readings are sampled and averaged. The bias is then deducted from future readings. Calibrating the accelerometer gave poor results so I've left that out for now.


For my purposes I only need the roll angle to find the uphill downhill incline. To calculate this a complementary filter is used using both gyro and accel data. First the accel data is normalised for +/- 2g. The tilt is computed from accel using : 

Then the gyro angle is calculated using:

And then a combinataion of the two is used in the complementery filter:





This is called in the main loop where is sampled at 500Hz.

It doesn't work as well as I hoped it would with an inaccuracy of several degrees in readings. In the future better calibration and a kalman filter can be implemented, or maybe just the mpu6050 dm.

## Logging

## Power
The device is powered using a 3.7v lithium polymer battery and is charged using a USBC tp4056 charging board. All peripherals are powered from the Pico's regulated 3v3 supply. 

## 3D assembly
The enclosure was designed in fusion 360 and 3D printed...