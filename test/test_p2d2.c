//
// Created by Dewey Jose on 5/21/22.
//
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <calc.h>
#include <protocol.h>

#define log(format, ...) printf("TEST: " format "\n" , ##__VA_ARGS__)
#define EPSILON 0.00001

short assertEqualsl(long double l, long double r) {
    return fabs(l - r) < EPSILON;
}

short assertEquals(long l, long r) {
    return l == r;
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
    if (!assertEqualsl(result, expected)) {
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

    if (!assertEqualsl(result, expected)) {
        log("ERROR test_gst: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_gst: %Lf == %Lf", result, expected);
}

void test_lst() {
    long double gst = 19.1634501;
    long double longitude = -71.20088889;
    long double result = lst(gst, longitude);
    long double expected = 14.41672417;
    if (!assertEqualsl(result, expected)) {
        log("ERROR test_lst: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_lst: %Lf == %Lf", result, expected);
}

void test_dec() {
    long double altitude = 51.90983333;
    long double azimuth = 218.7755556;
    long double latitude = 42.78842222;

    long double result = dec(altitude, azimuth, latitude);
    long double expected = 10.46797995;

    if (!assertEqualsl(result, expected)) {
        log("ERROR test_dec: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_dec: %Lf == %Lf", result, expected);
}

void test_ha() {
    long double altitude = 51.90983333;
    long double latitude = 42.78842222;
    long double dec = 10.46797995;

    long double result = ha(altitude, latitude, dec);
    long double expected = 23.1342075;

    if (!assertEqualsl(result, expected)) {
        log("ERROR test_ha: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_ha: %Lf == %Lf", result, expected);
}

void test_ra() {
    long double lst = 14.41672417;
    long double ha = 1.5422805;
    long double result = ra(lst, ha);
    long double expected = 12.87444367;

    if (!assertEqualsl(result, expected)) {
        log("ERROR test_ra: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_ra: %Lf == %Lf", result, expected);
}

void test_deg_mins_secs() {
    struct dec_mins_secs out;
    memset(&out, 0, sizeof(struct dec_mins_secs));
    ddms(71.20088889, &out);

    if (!assertEquals(71, out.base)) {
        log("ERROR test_dec_deg_mins_secs: degs %d != %d", 71, out.base);
        exit(1);
    }
    if (!assertEquals(12, out.minutes)) {
        log("ERROR test_dec_deg_mins_secs: mins %d != %d", 12, out.minutes);
        exit(1);
    }
    if (!assertEquals(3, out.seconds)) {
        log("ERROR test_dec_deg_mins_secs: secs %d != %d", 3, out.seconds);
        exit(1);
    }
    log("SUCCESS test_dec_deg_mins_secs");
}

void test_response_ra() {
    char response[35];
    response_ra(response, 12.87444367);
    log("ra %s", response);
}

void test_response_dec() {
    char response[35];
    response_dec(response, 10.46797995);
    log("dec %s", response);
}

int main(int argc, char **argv) {
    test_jd();
    test_gst();
    test_lst();
    test_dec();
    test_ha();
    test_ra();
    test_deg_mins_secs();
    test_response_ra();
    test_response_dec();
}