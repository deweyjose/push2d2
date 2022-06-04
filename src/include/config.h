//
// Created by Dewey Jose on 5/12/22.
//

#ifndef P2D2_CONFIG_H
#define P2D2_CONFIG_H

struct serial {
    char * device;
    short baud_rate;
};

struct coordinates {
    long double latitude;
    long double longitude;
};

struct display_config {
    short rows;
    short columns;
    short bit_mode;
    short register_select_pin;
    short enable_pin;
    short d0_pin;
    short d1_pin;
    short d2_pin;
    short d3_pin;
    short d4_pin;
    short d5_pin;
    short d6_pin;
    short d7_pin;
};

struct rotary_config {
    short phase_a_pin;
    short phase_b_pin;
    short cpr;
    short gear_ratio;
};

struct config {
    struct serial serial;
    struct coordinates coordinates;
    struct display_config display_config;
    struct rotary_config azimuth_config;
    struct rotary_config altitude_config;
};

int config_load(struct config *config, char * filename);
void config_dump(struct config *config);

#endif //P2D2_CONFIG_H
