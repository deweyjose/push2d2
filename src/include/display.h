//
// Created by Dewey Jose on 5/7/22.
//

#ifndef P2D2_DISPLAY_H
#define P2D2_DISPLAY_H

#include <config.h>

int display_initialize(const struct config *s);

void display_clear();

void display_text(char * text, int line);

#endif //P2D2_DISPLAY_H
