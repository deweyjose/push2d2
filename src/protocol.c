//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <calc.h>
#include <protocol.h>
#include <rotary.h>
#include <regex.h>

#define log(format, ...) printf("PROTOCOL: " format "\n" , ##__VA_ARGS__)

#define COMMAND_RA "#:GR#"
#define COMMAND_DEC "#:GD#"
#define COMMAND_SYNC_RA_REGEX "\\#:Q\\#:Sr[0-9]+:[0-9]+:[0-9]+\\#"
#define COMMAND_SYNC_DEC_REGEX ":Sd[\\+\\-]+[0-9]+[^\\x00-\\x7F]*[0-9]+:[0-9]+\\#"
#define COMMAND_CM ":CM#"
#define SYNC_RA_FORMAT "#:Q#:Sr%02hu:%hu:%hu#"
#define SYNC_DEC_FORMAT ":Sd%c%02hi%c%02hi:%02hi#"

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

/**
 * Public initialization API. Ensure the regex's are valid.
 * At some point move this to configuration.
 * Command Matchers => Command Responders.
 * @return
 */
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
    struct degrees_mins_secs out;
    memset(&out, 0, sizeof(struct degrees_mins_secs));
    dms_to_decimal_hours(ra, &out);
    sprintf(buffer, "%02d:%02d:%02d#", out.degrees, out.minutes, out.seconds);
    return buffer;
}

/**
 * Get Telescope Declination.
 * @param buffer
 * @return sDD*MM’SS
 */
char *response_dec(char *buffer, long double dec) {
    struct degrees_mins_secs out;
    memset(&out, 0, sizeof(struct degrees_mins_secs));
    dms_to_decimal_hours(dec, &out);

    char decSign = dec < 0 ? '-' : '+';
    sprintf(buffer, "%c%02d%c%02d:%02d#",
            decSign,
            abs(out.degrees),
            223,
            abs(out.minutes),
            abs(out.seconds));
    return buffer;

}

/**
 * Process LX200 Meade Serial Command Set (some of them :)...)
 * @param command LX200 Meade Serial Command
 * @param response
 * @param location
 * @return
 */
char *protocol_dispatch(char *command, char *response, coordinates_config_ptr location) {
    if (strcmp(COMMAND_DEC, command) == 0) {
        long double altitude = rotary_get_altitude();
        long double azimuth = rotary_get_azimuth();
        return response_dec(response, dec(altitude, azimuth, location->latitude));
    } else if (strcmp(COMMAND_RA, command) == 0) {
        long double altitude = rotary_get_altitude();
        long double azimuth = rotary_get_azimuth();
        return response_ra(response, ra(altitude, azimuth, location));
    } else if (!regexec(&regex_sync_ra, command, 0, NULL, 0)) {
        short hours, minutes, seconds = 0;
        sscanf(command, SYNC_RA_FORMAT, &hours, &minutes, &seconds);

        // get RA in hour decimal
        struct degrees_mins_secs ra_dms;
        ra_dms.degrees = hours;
        ra_dms.minutes = minutes;
        ra_dms.seconds = seconds;
        last_ra_sync = decimal_hours_from_dms(&ra_dms);

        log("accept sync RA %Lf", last_ra_sync);

        sprintf(response, "1");
        return response;
    } else if (!regexec(&regex_sync_dec, command, 0, NULL, 0)) {
        char sign, c1 = 0;
        short degrees, minutes, seconds = 0;
        sscanf(command, SYNC_DEC_FORMAT, &sign, &degrees, &c1, &minutes, &seconds);

        struct degrees_mins_secs dec_dms;
        dec_dms.degrees = degrees;
        dec_dms.minutes = minutes;
        dec_dms.seconds = seconds;
        last_dec_sync = decimal_hours_from_dms(&dec_dms);

        log("accept sync DEC %Lf", last_dec_sync);

        sprintf(response, "1");

        return response;
    } else if (strcmp(COMMAND_CM, command) == 0) {

        struct azimuth_altitude out;
        compute_az_and_alt(last_ra_sync, last_dec_sync, location, &out);

        rotary_set_altitude(out.altitude);
        rotary_set_azimuth(out.azimuth);

        log("commit a: %Lf, A: %Lf", out.altitude, out.azimuth);

        sprintf(response, "Polaris #");

        return response;
    } else {
        sprintf(response, "%c", 21);
        return response;
    }
}

