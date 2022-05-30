//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <memory.h>
#include <calc.h>

#define COMMAND_RA ":GR#"
#define COMMAND_DEC ":GD#"

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
