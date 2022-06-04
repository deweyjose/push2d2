//
// Created by Dewey Jose on 5/16/22.
//

#ifndef PUSH2D2_CALC_H
#define PUSH2D2_CALC_H

#include <time.h>

#define TRUNC(x) ((int)(x))

// Julian Datetime

#define JD_CONSTANT 1720994.5

long double jd();
long double jd_from_time_t(struct tm * tm_ptr);

// Greenwich Sidereal Time
#define GST_JD_EPOCH 2451545.5
#define GST_TC0 36525
#define GST_TC1 6.697374558
#define GST_TC2 2400.051336
#define GST_TC3 0.000025862
#define GST_UTC1 1.002737909

// Greenwich Sidereal Time

long double gst();

long double gst_from_jd_tm(long double jd, struct tm *tm_ptr);

// Local Sidereal time

long double lst(long double gst, long double longitude);

// Declination

long double dec(long double altitude, long double azimuth, long double latitude);

// Hour Angle

long double ha(long double azimuth, long double altitude, long double latitude, long double dec);

// Right Ascension

long double ra(long double lst, long double ha);

// decimal base minutes seconds

struct dec_mins_secs {
    short base;
    short minutes;
    short seconds;
};

struct dec_mins_secs * to_dms(long double deg, struct dec_mins_secs * out);

long double from_dms(struct dec_mins_secs *input);

#endif //PUSH2D2_CALC_H
