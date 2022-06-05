//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <serial.h>
#include <wiringSerial.h>
#include <string.h>

#define log(format, ...) printf("SERIAL: " format "\n" , ##__VA_ARGS__)

int handle = 0;
int buffer_position = 0;
char buffer[64];

int serial_initialize(struct serial *config) {
    handle = serialOpen(config->device, config->baud_rate);
    if (handle == -1) {
        log("failed to initialize serial device %s, baud rate %d", config->device, config->baud_rate);
        return 0;
    }
    return 1;
}

void serial_write_response(char *data) {
    serialPuts(handle, data);

}

int serial_read_command(char *output) {
    int available = serialDataAvail(handle);
    if (available > 0) {
        char last_char = 0;
        for (int i = 0; i < available; ++i) {
            last_char = (char) serialGetchar(handle);
            buffer[buffer_position++] = last_char;
        }

        if (last_char == '#') {
            buffer[buffer_position] = '\0';
            strcpy(output, buffer);
            buffer_position = 0;
            return 1;
        }
    }
    return 0;
}

