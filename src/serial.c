//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <serial.h>
#include <wiringSerial.h>

#define log(format, ...) printf("SERIAL: " format "\n" , ##__VA_ARGS__)

int handle = 0;

/**
 * Initialize the Serial library with settings from config;
 * @param config
 * @return 0|1
 */
int serial_initialize(serial_config_ptr config) {
    handle = serialOpen(config->device, config->baud_rate);
    if (handle == -1) {
        log("failed to initialize serial_config device %s, baud rate %d", config->device, config->baud_rate);
        return 0;
    }
    return 1;
}

/**
 * Write data to the serial_config interface.
 * @param data
 */
void serial_write_response(char *data) {
    serialPuts(handle, data);

}

/**
 * Read data from the serial_config interface into the buffer.
 * This needs to be cleaned up further. The protocol
 * layer should be the one to analyze the buffer for
 * command control characters.
 * @param buffer_ptr
 * @return 0|1 if there is a command
 */
int serial_read_command(sb_ptr buffer_ptr) {
    int available = serialDataAvail(handle);
    if (available > 0) {
        char last_char = 0;
        for (int i = 0; i < available; ++i) {
            last_char = (char) serialGetchar(handle);
            buffer_ptr->buffer[buffer_ptr->current_position++] = last_char;
        }

        if (last_char == '#') {
            buffer_ptr->buffer[buffer_ptr->current_position] = '\0';
            buffer_ptr->current_position = 0;
            return 1;
        }
    }
    return 0;
}

