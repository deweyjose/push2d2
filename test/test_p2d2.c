//
// Created by Dewey Jose on 5/21/22.
//
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <calc.h>
#include <protocol.h>
#include <config.h>
#include <rotary.h>
#include <wiringPi.h>

#define log(format, ...) printf("TEST: " format "\n" , ##__VA_ARGS__)
#define EPSILON 0.00001

short assertEqualsl(long double l, long double r) {
    return fabs(l - r) < EPSILON;
}

short assertEquals(long l, long r) {
    return l == r;
}

short assertEqualsStr(char *l, char *r) {
    return strcmp(l, r) == 0;
}

void test_to_dms() {
    struct dec_mins_secs out;
    to_dms(182.524167, &out);
    if (!assertEquals(182, out.base)) {
        log("ERROR test_to_dms: %d != %d", 182, out.base);
        exit(1);
    }
    if (!assertEquals(31, out.minutes)) {
        log("ERROR test_to_dms: %d != %d", 31, out.minutes);
        exit(1);
    }

    if (!assertEquals(27, out.seconds)) {
        log("ERROR test_to_dms: %d != %d", 27, out.seconds);
        exit(1);
    }
    log("SUCCESS test_to_dms");
}

void test_from_dms() {
    struct dec_mins_secs input;
    input.base = 182;
    input.minutes = 31;
    input.seconds = 27;

    long double result = from_dms(&input);
    long double expected = 182.524167;

    if (!assertEqualsl(result, expected)) {
        log("ERROR test_from_dms: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_from_dms");
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
    long double result = gst_from_jd_tm(jd, &test_tm);
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

    long double result = ha(355, altitude, latitude, dec);
    long double expected = 23.1342075;

    if (!assertEqualsl(result, expected)) {
        log("ERROR test_ha: %Lf != %Lf", result, expected);
        exit(1);
    }
    log("SUCCESS test_ha: %Lf == %Lf", result, expected);

    result = ha(3, altitude, latitude, dec);
    expected = 336.865792;

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
    to_dms(71.20088889, &out);

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
    char *expected = "12:52:27#";
    response_ra(response, 12.87444367);
    if (!assertEqualsStr(response, expected)) {
        log("ERROR test_response_ra: %s != %s", response, expected);
        exit(1);
    }
    log("SUCCESS test_response_ra: %s == %s", response, expected);
}

void test_response_dec() {
    char response[35];
    char *expected = "+10*28#";
    response_dec(response, 10.46797995);
    if (!assertEqualsStr(response, expected)) {
        log("ERROR test_response_dec: %s != %s", response, expected);
        exit(1);
    }
    log("SUCCESS test_response_dec: %s == %s", response, expected);
}

void test_process_request() {

    wiringPiSetup();

    struct config c;
    c.azimuth_config.gear_ratio=20;
    c.azimuth_config.cpr=2400;
    c.azimuth_config.phase_b_pin=6;
    c.azimuth_config.phase_a_pin=7;
    c.altitude_config.gear_ratio=20;
    c.altitude_config.cpr=2400;
    c.altitude_config.phase_a_pin=8;
    c.altitude_config.phase_b_pin=9;

    rotary_initialize(&c);
    rotary_set_altitude(45);
    rotary_set_azimuth(1);

    protocol_initialize();

    struct coordinates loc;
    memset(&loc, 0, sizeof(struct coordinates));
    loc.latitude = 42.78842222;
    loc.longitude = 71.20088889;

    char *request_ra = "#:GR#";
    char *expected_ra_response = "12:52:27#";

    char response_buffer[35];
    memset(response_buffer, 0, 32);
    protocol_handle_request(request_ra, response_buffer, &loc);
    if (!assertEqualsStr(response_buffer, expected_ra_response)) {
        log("ERROR test_process_request ra: %s != %s", response_buffer, expected_ra_response);
        //exit(1);
    } else {
        log("SUCCESS test_process_request ra: %s == %s", response_buffer, expected_ra_response);
    }

    memset(response_buffer, 0, 32);
    char *request_dec = "#:GD#";
    char *expected_dec_response = "+10*28#";
    protocol_handle_request(request_dec, response_buffer, &loc);
    if (!assertEqualsStr(response_buffer, expected_dec_response)) {
        log("ERROR test_process_request dec: %s != %s", response_buffer, expected_dec_response);
        //exit(1);
    } else {
        log("SUCCESS test_process_request dec: %s == %s", response_buffer, expected_dec_response);
    }

    memset(response_buffer, 0, 32);
    char *request_bla = ":BLA#";
    char *expected_bla_response = "\0x15";
    protocol_handle_request(request_bla, response_buffer, &loc);
    if (!assertEqualsStr(response_buffer, expected_bla_response)) {
        log("ERROR test_process_request bla: %s != %s", response_buffer, expected_bla_response);
        //exit(1);
    } else {
        log("SUCCESS test_process_request bla: %s == %s", response_buffer, expected_bla_response);
    }

    memset(response_buffer, 0, 32);
    char *request_sync = "#:Q#:Sr01:37:26#";
    char *expected_sync_response = "1";
    protocol_handle_request(request_sync, response_buffer, &loc);
    if (!assertEqualsStr(response_buffer, expected_sync_response)) {
        log("ERROR test_process_request sync ra: %s != %s", response_buffer, expected_sync_response);
        //exit(1);
    } else {
        log("SUCCESS test_process_request sync ra: %s == %s", response_buffer, expected_sync_response);
    }

    memset(response_buffer, 0, 32);
    char *request_sync_dec = ":Sd+72�06:40#";
    protocol_handle_request(request_sync_dec, response_buffer, &loc);
    if (!assertEqualsStr(response_buffer, expected_sync_response)) {
        log("ERROR test_process_request sync dec: %s != %s", response_buffer, expected_sync_response);
        //exit(1);
    } else {
        log("SUCCESS test_process_request sync dec: %s == %s", response_buffer, expected_sync_response);
    }
}

int main(int argc, char **argv) {
    test_to_dms();
    test_from_dms();
    test_jd();
    test_gst();
    test_lst();
    test_dec();
    test_ha();
    test_ra();
    test_deg_mins_secs();
    test_response_ra();
    test_response_dec();
    test_process_request();
}