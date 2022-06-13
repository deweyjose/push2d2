//
// Created by Dewey Jose on 5/16/22.
//

#ifndef PUSH2D2_CALC_H
#define PUSH2D2_CALC_H

#include <time.h>
#include <config.h>

#define TRUNC(x) ((int)(x))

#define JD_CONSTANT 1720994.5

#define GST_JD_EPOCH 2451545.5
#define GST_TC0 36525
#define GST_TC1 6.697374558
#define GST_TC2 2400.051336
#define GST_TC3 0.000025862
#define GST_UTC1 1.002737909

struct azimuth_altitude {
    long double azimuth;
    long double altitude;
};
typedef struct azimuth_altitude *az_alt_ptr;

struct degrees_mins_secs {
    short degrees;
    short minutes;
    short seconds;
};
typedef struct degrees_mins_secs * dms_ptr;

long double jd_from_tm(struct tm *tm_ptr);

long double gst();

long double gst_from_jd_tm(long double jd, struct tm *tm_ptr);

long double lst(long double gst, long double longitude);

long double dec(long double altitude, long double azimuth, long double latitude);

long double ha(long double azimuth, long double altitude, long double latitude, long double dec);

long double ra(long double lst, long double ha);

az_alt_ptr compute_az_and_alt(
        long double ra,
        long double dec,
        coordinates_config_ptr location,
        az_alt_ptr out
);

dms_ptr dms_to_decimal_hours(long double deg, dms_ptr out);

long double decimal_hours_from_dms(dms_ptr input);

void set_test_time(time_t time);

#endif //PUSH2D2_CALC_H
