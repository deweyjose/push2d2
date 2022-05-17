cmake_minimum_required(VERSION 3.7)

project(P2D2 VERSION 0.1)

configure_file(p2d2.h.in p2d2.h)

# main application
add_executable(p2d2 p2d2.c display.c config.c rotary.c)

# add libraries to link
target_link_libraries(p2d2 PUBLIC ${EXTRA_LIBS} wiringPi wiringPiDev confuse)

# include folders
target_include_directories(p2d2 PUBLIC "${PROJECT_BINARY_DIR}")

