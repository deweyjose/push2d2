cmake_minimum_required(VERSION 3.10)
project(P2D2 VERSION 0.1)
configure_file(p2d2.h p2d2.h)
add_executable(p2d2 lcd.c)