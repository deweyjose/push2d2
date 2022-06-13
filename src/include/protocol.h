//
// Created by Dewey Jose on 5/30/22.
//

#ifndef P2D2_PROTOCOL_H
#define P2D2_PROTOCOL_H

#include <config.h>

int protocol_initialize();

char * protocol_dispatch(char * command, char * response, coordinates_config_ptr location);

char * response_ra(char * buffer, long double ra);

char * response_dec(char *buffer, long double dec);

#endif //P2D2_PROTOCOL_H
