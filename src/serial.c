//
// Created by Dewey Jose on 5/30/22.
//

#include <stdio.h>
#include <serial.h>
#include <wiringSerial.h>

#define log(format, ...) printf("SERIAL: " format "\n" , ##__VA_ARGS__)

int serial = 0;

int serial_initialize(struct serial *config) {
    serial = serialOpen(config->device, config->baud_rate);
    if (serial == -1) {
        log("failed to initialize serial device %s, baud rate %d", config->device, config->baud_rate);
        return 0;
    }
    return 1;
}

void serial_write(char *data) {
    //log("serial write: %s", data);
    serialPuts(serial, data);

}

int serial_read(char * output) {
    int available = serialDataAvail(serial);
    if (available > 0) {
        for (int i = 0; i < available; ++i) {
            output[i] = (char) serialGetchar(serial);
        }
        output[available] = '\0';
    }
    return available;
}

