cmake_minimum_required(VERSION 3.13)
project(EV3_API C)

set(CMAKE_C_STANDARD 11)

include_directories(firmware_headers/bluetooth)
include_directories(include)
include_directories(src)

add_library(ev3api STATIC
        firmware_headers/bluetooth/bluetooth.c
        src/ev3_inputs/ev3_input.c
        src/ev3_inputs/ev3_input_analog.c
        src/ev3_inputs/ev3_input_iic.c
        src/ev3_inputs/ev3_input_uart.c
        src/ev3_sensors/back_compatibility.c
        src/ev3_sensors/ev3_color.c
        src/ev3_sensors/ev3_gyro.c
        src/ev3_sensors/ev3_ir.c
        src/ev3_sensors/ev3_sensors.c
        src/ev3_sensors/ev3_touch.c
        src/ev3_sensors/ev3_ultrasonic.c
        src/ev3_sensors/ht_color.c
        src/ev3_sensors/ht_compass.c
        src/ev3_sensors/ht_ir.c
        src/ev3_sensors/nxt_sound.c
        src/ev3_sensors/nxt_temperature.c
        src/ev3_sensors/pixy_cam.c
        src/ev3.c
        src/ev3_bluetooth.c
        src/ev3_button.c
        src/ev3_lcd.c
        src/ev3_output.c
        src/ev3_sound.c
        src/ev3_timer.c
        src/ev3_wait.c
        )

