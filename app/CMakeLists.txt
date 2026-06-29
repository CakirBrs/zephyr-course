cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED)
project(my_zephyr_app)

target_sources(app PRIVATE src/main.cpp src/sensor_shell.c drivers/led_sensor/led_sensor.c)
target_include_directories(app PRIVATE drivers/led_sensor)
