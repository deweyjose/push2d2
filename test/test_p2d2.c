//
// Created by Dewey Jose on 5/21/22.
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <calc.h>
#include <protocol.h>
#include <config.h>
#include <rotary.h>
#include <wiringPi.h>

#define log(format, ...) printf("TEST: " format "\n" , ##__VA_ARGS__)
#define EPSILON 0.00001

#define ASSERT_EQUALS(expected, actual) \
if (expected != actual) { \
    log("ERROR %s: %d != %d", __FUNCTION__ , expected, actual); \
    exit(1);                                    \
} \

#define ASSERT_EQUALS_LD(expected, actual) \
if (fabsl(expected - actual) > EPSILON) { \
    log("ERROR %s: %Lf != %Lf", __FUNCTION__ , expected, actual); \
    exit(1);                                    \
}                                         \

#define ASSERT_EQUALS_STR(expected, actual) \
if (strcmp(expected, actual) != 0) { \
    log("ERROR %s: %s != %s", __FUNCTION__ , expected, actual); \
    exit(1);                                    \
}                                         \

#define SUCCESS() log("SUCCESS %s", __FUNCTION__)

void test_to_dms() {
    struct dec_mins_secs out;
    to_dms(182.524167, &out);
    ASSERT_EQUALS(182, out.base);
    ASSERT_EQUALS(31, out.minutes);
    ASSERT_EQUALS(27, out.seconds);
    SUCCESS();
}

void test_from_dms() {
    struct dec_mins_secs input;
    input.base = 182;
    input.minutes = 31;
    input.seconds = 27;

    long double result = from_dms(&input);
    long double expected = 182.524167;

    ASSERT_EQUALS_LD(result, expected);
    SUCCESS();
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
    long double expected = 2459721.6478819;

    ASSERT_EQUALS_LD(result, expected);
    SUCCESS();
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

    ASSERT_EQUALS_LD(result, expected);
    SUCCESS();
}

void test_lst() {
    long double gst = 19.1634501;
    long double longitude = -71.20088889;
    long double result = lst(gst, longitude);
    long double expected = 14.41672417;

    ASSERT_EQUALS_LD(result, expected);
    SUCCESS();
}

void test_dec() {
    long double altitude = 51.90983333;
    long double azimuth = 218.7755556;
    long double latitude = 42.78842222;

    long double result = dec(altitude, azimuth, latitude);
    long double expected = 10.46797995;

    ASSERT_EQUALS_LD(result, expected);
    SUCCESS();
}

void test_ha() {
    long double altitude = 51.90983333;
    long double latitude = 42.78842222;
    long double dec = 10.46797995;

    long double result = ha(355, altitude, latitude, dec);
    long double expected = 23.1342075;

    ASSERT_EQUALS_LD(result, expected);

    result = ha(3, altitude, latitude, dec);
    expected = 336.865792;

    ASSERT_EQUALS_LD(result, expected);
    SUCCESS();
}

void test_ra() {
    long double lst = 14.41672417;
    long double ha = 1.5422805;
    long double result = ra(lst, ha);
    long double expected = 12.87444367;

    ASSERT_EQUALS_LD(result, expected);
    SUCCESS();
}

void test_deg_mins_secs() {
    struct dec_mins_secs out;
    memset(&out, 0, sizeof(struct dec_mins_secs));
    to_dms(71.20088889, &out);

    ASSERT_EQUALS(71, out.base);
    ASSERT_EQUALS(12, out.minutes);
    ASSERT_EQUALS(3, out.seconds);
    SUCCESS();
}

void test_response_ra() {
    char response[35];
    char *expected = "12:52:27#";
    response_ra(response, 12.87444367);
    ASSERT_EQUALS_STR(response, expected);
    SUCCESS();
}

void test_response_dec() {
    char response[35];
    char *expected = "+10*28#";
    response_dec(response, 10.46797995);
    ASSERT_EQUALS_STR(response, expected);
    SUCCESS();
}

void test_ra_command(struct coordinates *loc) {
    char *command = "#:GR#";
    char *expected = "03:23:26#";
    char response[35];
    protocol_handle_request(command, response, loc);
    ASSERT_EQUALS_STR(expected, response);
    SUCCESS();
}

void test_dec_command(struct coordinates *loc) {
    char *command = "#:GD#";
    char *expected = "+87*40#";
    char response[35];
    protocol_handle_request(command, response, loc);
    ASSERT_EQUALS_STR(expected, response);
    SUCCESS();
}

void test_bad_command(struct coordinates *loc) {
    char *command = ":BLA#";
    char expected[5];
    sprintf(expected, "%c", 21);
    char response[35];
    protocol_handle_request(command, response, loc);
    ASSERT_EQUALS_STR(expected, response);
    SUCCESS();
}

void test_sync_ra_command(struct coordinates *loc) {
    char *command = "#:Q#:Sr01:37:26#";
    char *expected = "1";
    char response[35];
    protocol_handle_request(command, response, loc);
    ASSERT_EQUALS_STR(expected, response);
    SUCCESS();
}

void test_sync_dec_command(struct coordinates *loc) {
    char *command = ":Sd+72�06:40#";
    char *expected = "1";
    char response[35];
    protocol_handle_request(command, response, loc);
    ASSERT_EQUALS_STR(expected, response);
    SUCCESS();
}

void test_sync_commit_command(struct coordinates *loc) {
    char *command = ":CM#";
    char *expected = "Polaris #";
    char response[35];
    protocol_handle_request(command, response, loc);
    ASSERT_EQUALS_STR(expected, response);
    SUCCESS();
}

void test_az_alt_conversion(struct coordinates *loc) {
    set_test_time(1654451483);

    char * ra = "#:Q#:Sr12:55:02#";
    char * dec = ":Sd+55*50:37#";
    char response[35];
    protocol_handle_request(ra, response, loc);
    protocol_handle_request(dec, response, loc);
    protocol_handle_request(":CM#", response, loc);

    ASSERT_EQUALS_LD(38.377500, rotary_get_azimuth()); // align on our boundary, slightly lossy
    ASSERT_EQUALS_LD(28.117500, rotary_get_altitude());
    SUCCESS();
}


void test_process_request() {

    wiringPiSetup();

    struct config c;
    c.azimuth_config.gear_ratio = 20;
    c.azimuth_config.cpr = 2400;
    c.azimuth_config.phase_b_pin = 6;
    c.azimuth_config.phase_a_pin = 7;
    c.altitude_config.gear_ratio = 20;
    c.altitude_config.cpr = 2400;
    c.altitude_config.phase_a_pin = 8;
    c.altitude_config.phase_b_pin = 9;

    struct coordinates loc;
    loc.latitude = 42.78842222;
    loc.longitude = -71.20088889;

    set_test_time(1654351516);
    rotary_initialize(&c);
    rotary_set_altitude(45);
    rotary_set_azimuth(1);

    protocol_initialize();

    test_ra_command(&loc);
    test_dec_command(&loc);
    test_bad_command(&loc);
    test_sync_ra_command(&loc);
    test_sync_dec_command(&loc);
    test_sync_commit_command(&loc);
    test_az_alt_conversion(&loc);
    SUCCESS();
}

void test_compute_az_and_alt() {
    struct azimuth_altitude out;

    struct dec_mins_secs ra;
    ra.base = 12; ra.minutes = 55; ra.seconds = 2;
    long double ra_dec = from_dms(&ra);

    struct dec_mins_secs dec;
    dec.base= 55; dec.minutes = 50; dec.seconds = 37;
    long double dec_dec = from_dms(&dec);

    struct coordinates loc;
    loc.latitude = 42.78842222;
    loc.longitude = -71.20088889;

    set_test_time(1654451483);
    compute_az_and_alt(ra_dec, dec_dec, &loc, &out);

    ASSERT_EQUALS_LD(38.384030, out.azimuth)
    ASSERT_EQUALS_LD(28.118535, out.altitude);
    SUCCESS();
}

int main() {

    test_to_dms();
    test_from_dms();
    test_jd();
    test_gst();
    test_lst();
    test_dec();
    test_ha();
    test_ra();
    test_deg_mins_secs();
    test_compute_az_and_alt();
    test_response_ra();
    test_response_dec();
    test_process_request();
}