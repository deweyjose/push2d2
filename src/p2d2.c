//
// Created by Dewey Jose on 5/7/22.
//

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <string.h>
#include <p2d2.h>
#include <display.h>
#include <rotary.h>
#include <config.h>
#include <calc.h>
#include <serial.h>
#include <protocol.h>
#include <stdlib.h>

#define log(format, ...) printf("P2D2: " format "\n" , ##__VA_ARGS__)

int main(int argc, char *argv[]) {
    log("Push2D2 version %d.%d", P2D2_VERSION_MAJOR, P2D2_MINOR);

    struct config config;
    memset(&config, 0, sizeof(struct config));

    if (!config_load(&config, argv[1])) {
        return ERROR_INITIALIZE_CONFIG;
    }

    config_dump(&config);

    wiringPiSetup(); // he just returns zero anyway...

    piHiPri(99);

    if (!display_initialize(&config)) {
        return ERROR_LOAD_CONFIG;
    }

    display_text("init encoders", 0);

    if (!rotary_initialize(&config)) {
        return ERROR_INITIALIZE_ROTARY;
    }

    if (!serial_initialize(&config.serial)) {
        return ERROR_INITIALIZE_SERIAL;
    }

    if (!protocol_initialize()) {
        log("failed to initialize protocol layer");
        return ERROR_INITIALIZE_PROTOCOL;
    }

    display_text("move the scope!", 0);

    fflush(stdout);

    char sb_buffer[32];
    struct serial_buffer sb;
    sb.current_position = 0;
    sb.stop_char = '#';
    sb.length = 32;
    sb.buffer = sb_buffer;

    usleep(2000000);

    display_clear();
    while (1) {
        if (serial_read_command(&sb)) {
            char response[32];
            protocol_dispatch(sb.buffer, response, &config.coordinates);
            serial_write_response(response);
            sb.current_position = 0;
        }

        char buffer[16];
        sprintf(buffer, " A %9.5f", rotary_get_altitude());
        display_text(buffer, 0);
        sprintf(buffer, "AZ %9.5f", rotary_get_azimuth());
        display_text(buffer, 1);
        fflush(stdout);

        usleep(50000);
    }
}
