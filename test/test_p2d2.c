//
// Created by Dewey Jose on 5/21/22.
//
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <calc.h>

#define log(format, ...) printf("TEST: " format "\n" , ##__VA_ARGS__)
#define EPSILON 0.00001

short assertEquals(long double l, long double r) {
    return fabs(l - r) < EPSILON;
}

void test_jd() {
    struct tm test_tm;
    memset(&test_tm, 0, sizeof(struct tm));
    test_tm.tm_year = 2022 - 1900;
    test_tm.tm_mon = 4;
    test_tm.tm_mday = 22;
    test_tm.tm_hour = 3;
    test_tm.tm_min = 32;
    test_tm.tm_sec = 57;

    long double result = jd_from_time_t(&test_tm);
    long double expected = 2459721.647882;
    if (!assertEquals(result, expected)) {
        log("ERROR test_jd: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_jd: %Lf == %Lf", result, expected);
}

void test_gst() {
    struct tm test_tm;
    memset(&test_tm, 0, sizeof(struct tm));
    test_tm.tm_year = 2022 - 1900;
    test_tm.tm_mon = 4;
    test_tm.tm_mday = 22;
    test_tm.tm_hour = 3;
    test_tm.tm_min = 32;
    test_tm.tm_sec = 57;

    long double jd = jd_from_time_t(&test_tm);
    long double result = gst(jd, &test_tm);
    long double expected = 19.509501;

    if (!assertEquals(result, expected)) {
        log("ERROR test_gst: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_gst: %Lf == %Lf", result, expected);
}

void test_dec() {
    long double altitude = 51.90983333;
    long double azimuth = 218.7755556;
    long double latitude = 42.78842222;

    long double result = dec(altitude, azimuth, latitude);
    long double expected = 10.46797995;

    if (!assertEquals(result, expected)) {
        log("ERROR test_dec: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_dec: %Lf == %Lf", result, expected);
}

int main(int argc, char **argv) {
    test_jd();
    test_gst();
    test_dec();
}