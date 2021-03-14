cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Test_hd44780_wo_arduino)
spiffs_create_partition_image(spiffs data)