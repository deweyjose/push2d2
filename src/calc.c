//
// Created by Dewey Jose on 5/16/22.
// Most of these calculations are based on
// Practical Astronomy with your Calculator or Spreadsheet
// By Peter Duffett-Smith, Jonathan Zwart
//

#include <stdio.h>
#include <math.h>
#include <calc.h>

#define log(format, ...) printf("CALC: " format "\n" , ##__VA_ARGS__)

// Do this once up, save a few ticks
long double RAD_HELPER = M_PI / 180;
long double DEG_HELPER = 180 / M_PI;

/**
 * C trigonometry functions require Radians.
 * Convert Degree => Radian
 * @param degree
 * @return radian
 */
long double rad(long double deg) {
    return deg * RAD_HELPER;
}

/**
 * C trigonometry functions require Radians.
 * Convert Radian => Degree
 * @param radian
 * @return degree
 */
long double deg(long double rad) {
    return rad * DEG_HELPER;
}

/**
 * Compute decimal hours for some Base (degrees or time).
 * @param base
 * @param minutes
 * @param seconds
 * @return decimal hours
 */
long double decimal_hours(long double base, long double minutes, long double seconds) {
    return (((seconds / 60) + minutes) / 60) + base;
}

/**
 * Many of the calculations require decimal hours.
 * This function transforms a tm into decimal hours.
 * @param tm_ptr
 * @return decimal hours
 */
long double decimal_hours_from_tm(struct tm *tm_ptr) {
    return decimal_hours(
            (long double) tm_ptr->tm_hour,
            (long double) tm_ptr->tm_min,
            (long double) tm_ptr->tm_sec
    );
}

/**
 * Decompose decimal hours to degrees, minutes and seconds.
 * This function is used for both Degrees and Time.
 * @param deg
 * @param ddms
 * @return
 */
dms_ptr dms_to_decimal_hours(long double deg, dms_ptr out) {
    out->degrees = TRUNC(deg);
    long double minutes = (deg - out->degrees) * 60;
    out->minutes = TRUNC(minutes);
    out->seconds = (short) ((minutes - (long double) out->minutes) * 60);
    return out;
}

/**
 * Compute decimal hours.
 * This function is used for both Degrees and Time.
 * @param input
 * @return
 */
long double decimal_hours_from_dms(dms_ptr input) {
    return decimal_hours(
            (long double)input->degrees,
            (long double)input->minutes,
            (long double)input->seconds
    );
}

/**
 * Calculate julian date based on a tm.
 * Refactored to make unit tests easier.
 * @param tm_ptr
 * @return
 */
long double jd_from_tm(struct tm *tm_ptr) {
    struct tm tm = *tm_ptr;
    int year = tm.tm_year + 1900;
    long double day_fraction = decimal_hours_from_tm(tm_ptr) / 24;
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

//
// this should get #ifdef'ed out...
time_t test_time = 0;

void set_test_time(time_t time) {
    test_time = time;
}
//

time_t get_time() {
    return test_time ? test_time : time(NULL);
}

long double gst() {
    time_t t = get_time();
    struct tm *tm_ptr = localtime(&t);
    return gst_from_jd_tm(jd_from_tm(tm_ptr), tm_ptr);
}

/**
 * Compute Greenwich Sidereal Time in decimal hours.
 * @return
 */
long double gst_from_jd_tm(long double jd, struct tm *tm_ptr) {
    long double S = jd - GST_JD_EPOCH;
    long double T = S / GST_TC0;
    long double T0 = GST_TC1 + (GST_TC2 * T) + (GST_TC3 * T * T);
    long double T0P = T0 - (24 * TRUNC(T0 / 24));
    long double A = GST_UTC1 * decimal_hours_from_tm(tm_ptr);
    long double gst = A + T0P;
    return (gst - (24 * TRUNC(gst / 24)));
}

/**
 * Compute Local Sidereal Time.
 * @param gst
 * @param longitude
 * @return
 */
long double lst(long double gst, long double longitude) {
    long double longHours = longitude / 15;
    long double lst = gst + longHours;
    return lst - (24 * TRUNC(lst / 24));
}

/**
 * Computes Declination
 * @param altitude
 * @param azimuth
 * @param latitude
 * @return
 */
long double dec(long double altitude, long double azimuth, long double latitude) {
    long double rAltitude = rad(altitude);
    long double rAzimuth = rad(azimuth);
    long double rLatitude = rad(latitude);
    long double sinDec = (sinl(rLatitude) * sinl(rAltitude)) + (cosl(rLatitude) * cosl(rAltitude) * cosl(rAzimuth));
    long double asinDec = asinl(sinDec);
    return deg(asinDec);
}

/**
 * Computes Hour Angle
 * @param altitude
 * @param latitude
 * @param dec
 * @return
 */
long double ha(long double azimuth, long double altitude, long double latitude, long double dec) {
    long double rAltitude = rad(altitude);
    long double rLatitude = rad(latitude);
    long double rDec = rad(dec);
    long double cosHA = (sinl(rAltitude) - (sinl(rLatitude) * sinl(rDec))) / (cosl(rLatitude) * cosl(rDec));
    long double acosHA = acosl(cosHA);
    long double HA = deg(acosHA);
    if (sinl(rad(azimuth)) < 0) {
        return HA;
    } else {
        return 360.0 - HA;
    }
}

/**
 * Compute Right Ascension.
 * @param lst
 * @param ha
 * @return
 */
long double ra(long double lst, long double ha) {
    long double ra = lst - ha;
    return ra > 0 ? ra : ra + 24;
}

azimuth_altitude_ptr compute_az_and_alt(
        long double ra,
        long double dec,
        coordinates_ptr location,
        azimuth_altitude_ptr out
) {
    long double greenwich_sidereal_time = gst();
    long double local_sidereal_time = lst(greenwich_sidereal_time, location->longitude);

    long double hour_angle = local_sidereal_time - ra;

    if (hour_angle < 0) {
        hour_angle += 24;
    }

    if (hour_angle < 0) {
        log("ERROR hour angle < 0 %Lf", hour_angle);
    }

    hour_angle *= 15;

    long double rad_ha = rad(hour_angle);
    long double sin_lat = sinl(rad(location->latitude));
    long double sin_dec = sinl(rad(dec));
    long double cos_lat = cosl(rad(location->latitude));
    long double cos_dec = cosl(rad(dec));
    long double cos_ha = cosl(rad_ha);
    long double sin_a = (sin_dec * sin_lat) + (cos_dec * cos_lat * cos_ha);

    out->altitude = deg(asinl(sin_a));

    long double cos_a = cosl(rad(out->altitude));
    long double cos_az = (sin_dec - (sin_lat * sin_a)) / (cos_lat * cos_a);
    long double azimuth = deg(acosl(cos_az));

    long double sin_ha = sinl(rad_ha);
    if (sin_ha > 0) {
        azimuth = 360.0 - azimuth;
    }

    out->azimuth = azimuth;

    return out;
}

