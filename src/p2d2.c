//
// Created by Dewey Jose on 5/7/22.
//

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <string.h>
#include "p2d2.h"
#include "include/display.h"
#include "include/rotary.h"
#include "include/config.h"
#include "include/calc.h"

#define log(format, ...) printf("P2D2: " format "\n" , ##__VA_ARGS__)

int main(int argc, char *argv[]) {
    log("Push2D2 version %d.%d", P2D2_VERSION_MAJOR, P2D2_MINOR);
    for (int i = 0; i < 10; ++i) {
        log("Julian Date %Lf", jd());
        fflush(stdout);
        usleep(1000000);
    }


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

    display_text("move the scope", 0);
    fflush(stdout);

    while (1) {

        sleep(1);
        display_clear();
        char buffer[16];
        sprintf(buffer, "Az %*.4Lf", 3, rotary_get_azimuth());
        display_text(buffer, 0);
        sprintf(buffer, "A  %*.4Lf", 3, rotary_get_altitude());
        display_text(buffer, 1);
        fflush(stdout);
    }
}
