set(CMAKE_VERBOSE_MAKEFILE ON)

configure_file(${PROJECT_SOURCE_DIR}/src/include/p2d2.h.in ${PROJECT_SOURCE_DIR}/src/include/p2d2.h)

add_library(p2d2_lib STATIC display.c config.c rotary.c calc.c protocol.c serial.c)

add_executable(p2d2 p2d2.c)

target_link_libraries(p2d2 PUBLIC p2d2_lib wiringPi wiringPiDev confuse m)



