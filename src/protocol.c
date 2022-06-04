//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <memory.h>
#include <calc.h>
#include <protocol.h>
#include <rotary.h>
#include <regex.h>
#include <math.h>

#define log(format, ...) printf("PROTOCOL: " format "\n" , ##__VA_ARGS__)

#define COMMAND_RA "#:GR#"
#define COMMAND_DEC "#:GD#"
#define COMMAND_SYNC_RA_REGEX "\\#:Q\\#:Sr[0-9]+:[0-9]+:[0-9]+\\#"
#define COMMAND_SYNC_DEC_REGEX ":Sd[\\+\\-]+[0-9]+[^\\x00-\\x7F]*[0-9]+:[0-9]+\\#"
#define COMMAND_CM ":CM#"
#define SYNC_RA_FORMAT "#:Q#:Sr%02hu:%hu:%hu#"
#define SYNC_DEC_FORMAT ":Sd%c%02hi%c%c%c%02hi:%02hi#"

regex_t regex_sync_ra;
regex_t regex_sync_dec;

long double last_ra_sync = 0;
long double last_dec_sync = 0;

/**
 * Initialize regex
 * @param regex
 * @param pattern
 * @return
 */
int initialize_regex(regex_t *regex, const char *pattern) {
    int result = regcomp(regex, pattern, REG_EXTENDED);
    if (result) {
        char buffer[35];
        regerror(result, &regex_sync_ra, buffer, 35);
        log("regex comp error: %s with %s", buffer, pattern);
        return 0;
    }
    return 1;
}

int protocol_initialize() {
    if (!initialize_regex(&regex_sync_ra, COMMAND_SYNC_RA_REGEX)) {
        return 0;
    }

    if (!initialize_regex(&regex_sync_dec, COMMAND_SYNC_DEC_REGEX)) {
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
    sprintf(buffer, "+%02d*%02d#", out.base, out.minutes);
    return buffer;
}

/**
 * Process LX200 Meade Serial Command Set (some of them :)...)
 * @param input LX200 Meade Serial Command
 * @param output
 * @param location
 * @return
 */
char *protocol_handle_request(char *input, char *output, struct coordinates *location) {
    if (strcmp(COMMAND_DEC, input) == 0) {
        long double altitude = rotary_get_altitude();
        long double azimuth = rotary_get_azimuth();
        return response_dec(output, dec(altitude, azimuth, location->latitude));
    } else if (strcmp(COMMAND_RA, input) == 0) {
        long double altitude = rotary_get_altitude();
        long double azimuth = rotary_get_azimuth();
        long double greenwich_sidereal_time = gst();
        long double local_sidereal_time = lst(greenwich_sidereal_time, location->longitude);
        long double declination = dec(altitude, azimuth, location->latitude);
        long double hour_angle = ha(azimuth, altitude, location->latitude, declination);
        hour_angle /= (long double) 15;
        return response_ra(output, ra(local_sidereal_time, hour_angle));
    } else if (!regexec(&regex_sync_ra, input, 0, NULL, 0)) {
        short hours, minutes, seconds = 0;
        sscanf(input, SYNC_RA_FORMAT, &hours, &minutes, &seconds);

        // get RA in hour decimal
        struct dec_mins_secs ra_dms;
        ra_dms.base = hours;
        ra_dms.minutes = minutes;
        ra_dms.seconds = seconds;
        last_ra_sync = from_dms(&ra_dms);

        log("accept RA %Lf", last_ra_sync);

        sprintf(output, "1");
        return output;
    } else if (!regexec(&regex_sync_dec, input, 0, NULL, 0)) {
        char sign, c1, c2, c3 = 0;
        short degrees, minutes, seconds = 0;
        sscanf(input, SYNC_DEC_FORMAT, &sign, &degrees, &c1, &c2, &c3, &minutes, &seconds);

        struct dec_mins_secs dec_dms;
        dec_dms.base = degrees;
        dec_dms.minutes = minutes;
        dec_dms.seconds = seconds;
        last_dec_sync = from_dms(&dec_dms);

        log("accept DEC %Lf", last_dec_sync);

        sprintf(output, "1");

        return output;
    } else if (strcmp(COMMAND_CM, input) == 0) {
        // altitude
        // get LST
        long double greenwich_sidereal_time = gst();
        long double local_sidereal_time = lst(greenwich_sidereal_time, location->longitude);

        long double hour_angle = local_sidereal_time - last_ra_sync;
        if (hour_angle < 0) {
            hour_angle += 24;
        }

        // get hour angle in degrees
        hour_angle *= 15;

        long double sin_lat = sinl(location->latitude);
        long double sin_dec = sinl(last_dec_sync);
        long double cos_lat = cosl(location->latitude);
        long double sin_a = (sin_dec * sin_lat)
                            + (cosl(last_dec_sync) * cos_lat * cosl(hour_angle));

        long double altitude = asinl(sin_a);

        // azimuth
        long double cos_az = (sin_dec - (sin_lat * sin_a)) / (cos_lat * cosl(altitude));

        long double azimuth = acosl(cos_az);

        long double sin_ha = sinl(hour_angle);

        if (sin_ha > 0) {
            azimuth = 360.0 - azimuth;
        }

        rotary_set_altitude(altitude);
        rotary_set_azimuth(azimuth);

        log("commit a: %Lf, A: %Lf", altitude, azimuth);

        sprintf(output, "Polaris #");

        return output;
    } else {
        sprintf(output, "%c", 21);
        return output;
    }
}

