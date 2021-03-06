//
// Created by Dewey Jose on 5/8/22.
//

#include <stdio.h>
#include <wiringPi.h>
#include <memory.h>
#include <rotary.h>

#define DEGREES 360.00

#define log(format, ...) printf("ROTARY: " format "\n" , ##__VA_ARGS__)

#define setup_pin(axis, p) \
if (wiringPiISR(axis ## _encoder.config->phase_##p ## _pin, INT_EDGE_BOTH, &rotary_##axis ## _callback) != 0) { \
    log(#axis" pin " #p " ISR setup failed");\
    return 0; \
} \

struct rotary_info {
    rotary_config_ptr config;
    volatile long encoder_count;
    volatile int last_encoded;
    volatile double degrees_per_encoded;
    volatile int max_encoded;
    int max_reset;
    int min_reset;
};

struct rotary_info azimuth_encoder;
struct rotary_info altitude_encoder;

void rotary_read_pins(struct rotary_info *info) {
    int encoded = (digitalRead(info->config->phase_a_pin) << 1) | digitalRead(info->config->phase_b_pin);
    int sum = (info->last_encoded << 2) | encoded;
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        // increasing
        //   azimuth wraps to 0
        //   altitude stops as the scope can't go any further
        if (info->encoder_count >= info->max_encoded) {
            info->encoder_count = info->max_reset;
        } else {
            ++(info->encoder_count);
        }
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        // decreasing
        //  azimuth wraps to max
        //  altitude stops as the scope can't go any further
        if (info->encoder_count <= 0) {
            info->encoder_count = info->min_reset;
        } else {
            --(info->encoder_count);
        }
    }
    info->last_encoded = encoded;
}

__attribute__((unused))
void rotary_azimuth_callback() {
    rotary_read_pins(&azimuth_encoder);
}

__attribute__((unused)) void rotary_altitude_callback() {
    rotary_read_pins(&altitude_encoder);
}

int rotary_initialize(config_ptr config) {
    int size = sizeof(struct rotary_info);
    memset(&azimuth_encoder, 0, size);
    memset(&altitude_encoder, 0, size);

    azimuth_encoder.config = &config->azimuth_config;
    altitude_encoder.config = &config->altitude_config;

    setup_pin(azimuth, a)
    setup_pin(azimuth, b)
    setup_pin(altitude, a)
    setup_pin(altitude, b)

    azimuth_encoder.max_encoded = azimuth_encoder.config->cpr * azimuth_encoder.config->gear_ratio;
    azimuth_encoder.degrees_per_encoded = DEGREES / azimuth_encoder.max_encoded;
    --azimuth_encoder.max_encoded;
    azimuth_encoder.max_reset = 0;
    azimuth_encoder.min_reset = azimuth_encoder.max_encoded;
    azimuth_encoder.encoder_count = 1;

    altitude_encoder.max_encoded= (altitude_encoder.config->cpr * altitude_encoder.config->gear_ratio);
    altitude_encoder.degrees_per_encoded = DEGREES / altitude_encoder.max_encoded;
    --altitude_encoder.max_encoded;
    altitude_encoder.max_reset = altitude_encoder.max_encoded;
    altitude_encoder.min_reset = 0;
    altitude_encoder.encoder_count = 1;

    return 1;
}

long double rotary_get_azimuth() {
    return azimuth_encoder.encoder_count * azimuth_encoder.degrees_per_encoded;
}

long double rotary_get_altitude() {
    return altitude_encoder.encoder_count * altitude_encoder.degrees_per_encoded;;
}

void rotary_set_azimuth(long double azimuth) {
    azimuth_encoder.encoder_count = (long)(azimuth / azimuth_encoder.degrees_per_encoded);
}

void rotary_set_altitude(long double altitude) {
    altitude_encoder.encoder_count = (long)(altitude / altitude_encoder.degrees_per_encoded);
}

