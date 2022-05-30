//
// Created by Dewey Jose on 5/8/22.
//

#ifndef P2D2_ROTARY_H
#define P2D2_ROTARY_H

#include "config.h"

int rotary_initialize(const struct config * config);

long double rotary_get_azimuth();

long double rotary_get_altitude();

#endif //P2D2_ROTARY_H
