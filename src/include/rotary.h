//
// Created by Dewey Jose on 5/8/22.
//

#ifndef P2D2_ROTARY_H
#define P2D2_ROTARY_H

#include "config.h"

int rotary_initialize(const struct config * config);

long double rotary_get_azimuth();

long double rotary_get_altitude();

void rotary_set_azimuth(long double azimuth);

void rotary_set_altitude(long double altitude);

#endif //P2D2_ROTARY_H
