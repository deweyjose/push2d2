//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <memory.h>
#include <calc.h>
#include <protocol.h>
#include <rotary.h>
#include <regex.h>

#define log(format, ...) printf("PROTOCOL: " format "\n" , ##__VA_ARGS__)
#define COMMAND_RA "#:GR#"
#define COMMAND_DEC "#:GD#"
#define COMMAND_SYNC_RA "\\#:Q\\#:Sr[0-9]+:[0-9]+:[0-9]+\\#"
#define COMMAND_SYNC_DEC ":Sd[\\+\\-]+[0-9]+[^\\x00-\\x7F]*[0-9]+:[0-9]+\\#"
#define COMMAND_CM ":CM#"
#define SYNC_RA_SSCANF "#:Q#:Sr%02d:%d:%d#"
#define SYNC_DEC_SSCANF ":Sd%c%02d%c%c%c%02d:%02d#"

regex_t regex_sync_ra;
regex_t regex_sync_dec;

int initialize_regex(regex_t *regex, const char *pattern) {
    int result = regcomp(regex, pattern, REG_EXTENDED);
    if (result) {
        char buffer[35];
        regerror(result, &regex_sync_ra, buffer, 35);
        log("regcomp error: %s with %s", buffer, pattern);
        return 0;
    }
    return 1;
}

int protocol_initialize() {
    if (!initialize_regex(&regex_sync_ra, COMMAND_SYNC_RA)) {
        return 0;
    }

    if (!initialize_regex(&regex_sync_dec, COMMAND_SYNC_DEC)) {
        return 0;
    }

    return 1;
}

/**
 * Get Telescope RA.
 * @param buffer
 * @return HH:MM:SS
 */
char *response_ra(char *buffer, long double ra) {
    struct dec_mins_secs out;
    memset(&out, 0, sizeof(struct dec_mins_secs));
    to_dms(ra, &out);
    sprintf(buffer, "%02d:%02d:%02d#", out.base, out.minutes, out.seconds);
    return buffer;
}

/**
 * Get Telescope Declination.
 * @param buffer
 * @return sDD*MM’SS
 */
char *response_dec(char *buffer, long double dec) {
    struct dec_mins_secs out;
    memset(&out, 0, sizeof(struct dec_mins_secs));
    to_dms(dec, &out);
    sprintf(buffer, "+%02d*%02d#", out.base, out.minutes, out.seconds);
    return buffer;
}

char *protocol_handle_request(char *input, char *output, struct coordinates *location) {
    long double altitude = rotary_get_altitude();
    long double azimuth = rotary_get_azimuth();
    if (strcmp(COMMAND_DEC, input) == 0) {
        return response_dec(output, dec(altitude, azimuth, location->latitude));
    } else if (strcmp(COMMAND_RA, input) == 0) {
        long double greenwich_sidereal_time = gst();
        long double local_sidereal_time = lst(greenwich_sidereal_time, location->longitude);
        long double declination = dec(altitude, azimuth, location->latitude);
        long double hour_angle = ha(azimuth, altitude, location->latitude, declination);
        hour_angle /= (long double) 15;
        return response_ra(output, ra(local_sidereal_time, hour_angle));
    } else if (!regexec(&regex_sync_ra, input, 0, NULL, 0)) {
        int hours, minutes, seconds = 0;

        sscanf(input, SYNC_RA_SSCANF, &hours, &minutes, &seconds);
        // set azimuth
        sprintf(output, "1");
    } else if (!regexec(&regex_sync_dec, input, 0, NULL, 0)) {
        char sign, c1, c2, c3 = 0;
        int degrees, minutes, seconds = 0;
        sscanf(input, SYNC_DEC_SSCANF, &sign, &degrees, &c1, &c2, &c3, &minutes, &seconds);
        sprintf(output, "1");
    } else if (strcmp(COMMAND_CM, input) == 0) {
        sprintf(output, "Polaris #");
    } else {
        sprintf(output, "\0x15");
        return output;
    }
}

