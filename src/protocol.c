//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <memory.h>
#include <calc.h>
#include <protocol.h>
#include <rotary.h>

#define log(format, ...) printf("PROTOCOL: " format "\n" , ##__VA_ARGS__)

#define COMMAND_RA ":GR#"
#define COMMAND_DEC ":GD#"

char * process_request(char * input, char * output, struct coordinates * location) {
    long double altitude = rotary_get_altitude();
    long double azimuth = rotary_get_azimuth();
    if (strcmp(COMMAND_DEC, input) == 0) {
        return response_dec(output, dec(altitude, azimuth, location->latitude));
    } else if (strcmp(COMMAND_RA, input) == 0) {
        long double greenwich_sidereal_time = gst();
        long double local_sidereal_time = lst(greenwich_sidereal_time, location->longitude);
        long double declination = dec(altitude, azimuth, location->latitude);
        long double hour_angle = ha(altitude, location->latitude, declination);
        return response_ra(output, ra(local_sidereal_time, hour_angle));
    } else {
        sprintf(output, "\0x15");
        return output;
    }
}

/**
 * Get Telescope RA.
 * @param buffer
 * @return HH:MM:SS
 */
char * response_ra(char * buffer, long double ra) {
    struct dec_mins_secs out;
    memset(&out, 0, sizeof(struct dec_mins_secs));
    ddms(ra, &out);
    sprintf(buffer, "%02d:%02d:%02d#", out.base, out.minutes, out.seconds);
    return buffer;
}

/**
 * Get Telescope Declination.
 * @param buffer
 * @return sDD*MM’SS
 */
char * response_dec(char *buffer, long double dec) {
    struct dec_mins_secs out;
    memset(&out, 0, sizeof(struct dec_mins_secs));
    ddms(dec, &out);
    sprintf(buffer, "+%02d*%02d'%02d#", out.base, out.minutes, out.seconds);
    return buffer;
}
