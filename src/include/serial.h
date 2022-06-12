//
// Created by Dewey Jose on 5/30/22.
//

#ifndef P2D2_SERIAL_H
#define P2D2_SERIAL_H

#include <config.h>

struct serial_buffer {
    int length;
    int current_position;
    char * buffer;
};
typedef struct serial_buffer * sb_ptr;

int serial_initialize(struct serial * config);

int serial_read_command(sb_ptr buffer);

void serial_write_response(char *data);

#endif //P2D2_SERIAL_H

