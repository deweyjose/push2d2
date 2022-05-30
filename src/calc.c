//
// Created by Dewey Jose on 5/16/22.
// Most of these calculations are based on
// Practical Astronomy with your Calculator or Spreadsheet
// By Peter Duffett-Smith, Jonathan Zwart
//

#include <math.h>
#include <calc.h>

#define log(format, ...) printf("CALC: " format "\n" , ##__VA_ARGS__)

long double RAD_HELPER = M_PI / 180;
long double DEG_HELPER = 180 / M_PI;

long double rad(long double deg) {
    return deg * RAD_HELPER;
}

long double deg(long double rad) {
    return rad * DEG_HELPER;
}

long double decimal_hours(struct tm *tm_ptr) {
    long double ld_sec = (long double) tm_ptr->tm_sec;
    long double ld_min = (long double) tm_ptr->tm_min;
    long double ld_hour = (long double) tm_ptr->tm_hour;
    return (((ld_sec / 60) + ld_min) / 60) + ld_hour;
}

/**
 * Helper function. This is called by core p2d2.
 * @return
 */
long double jd() {
    time_t t = time(NULL);
    struct tm *tm_ptr = localtime(&t);
    return jd_from_time_t(tm_ptr);
}

/**
 * Calculate julian date based on a tm.
 * Refactored to make unit tests easier.
 * @param tm_ptr
 * @return
 */
long double jd_from_time_t(struct tm *tm_ptr) {
    struct tm tm = *tm_ptr;
    int year = tm.tm_year += 1900;
    long double day_fraction = decimal_hours(tm_ptr) / 24;
    long double day_decimal = (long double) tm.tm_mday + day_fraction;
    year = (tm.tm_mon < 3 ? year - 1 : year);
    int month = tm.tm_mon + 1; // 0 index based ... :|
    month = (month < 3 ? month + 12 : month);
    long double A = year / 100;
    long double B = 2 - A + TRUNC(A / 4);
    long C = (year < 0 ? TRUNC((365.25 * year) - 0.75) : TRUNC(365.25 * year));
    long D = TRUNC(30.6001 * (month + 1));
    long double JD = B + C + D + day_decimal + JD_CONSTANT;
    return JD;
}

/**
 * Compute Greenwich Sidereal Time in decimal hours.
 * @return
 */
long double gst(long double jd, struct tm *tm_ptr) {
    long double S = jd - GST_JD_EPOCH;
    long double T = S / GST_TC0;
    long double T0 = GST_TC1 + (GST_TC2 * T) + (GST_TC3 * T * T);
    long double T0P = T0 - (24 * TRUNC(T0 / 24));
    long double A = GST_UTC1 * decimal_hours(tm_ptr);
    long double GST = A + T0P;
    return (GST - (24 * TRUNC(GST / 24)));
}

/**
 * Computes Declination
 * @param altitude
 * @param azimuth
 * @param latitude
 * @return
 */
extern long double dec(long double altitude, long double azimuth, long double latitude) {
    long double rAltitude = rad(altitude);
    long double rAzimuth = rad(azimuth);
    long double rLatitude = rad(latitude);
    long double sinDec = (sinl(rLatitude) * sinl(rAltitude)) + (cosl(rLatitude) * cosl(rAltitude) * cosl(rAzimuth));
    long double asinDec = asinl(sinDec);
    return deg(asinDec);
}