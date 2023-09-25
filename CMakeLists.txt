cmake_minimum_required(VERSION 3.24)
project(apriltag-dectection)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)

add_executable(calib src/calibration.cpp)
add_executable(main src/main.cpp)

target_link_libraries(calib ${OpenCV_LIBS})