//
// Created by Dewey Jose on 5/30/22.
//

#ifndef P2D2_SERIAL_H
#define P2D2_SERIAL_H

#include <config.h>

int serial_initialize(struct serial * config);

int serial_read(char *output);

void serial_write(char *data);

#endif //P2D2_SERIAL_H

