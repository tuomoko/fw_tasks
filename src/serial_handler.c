#include <stdio.h>
#include <string.h>
#include "serial_handler.h"

void init_serial_port_handler(SerialPortHandler *handler) {
    handler->buffer_index = 0;
    handler->escape_flag = 0;
    handler->started = 0;
    memset(handler->buffer, 0, BUFFER_SIZE);
}

void handle_command(const char *command) {
    // Implement your command handling logic here
    printf("Received command: %s\n", command);
}

void serial_receive_char(SerialPortHandler *handler, __uint8_t c) {
    if (!handler->started) {
        if (c == START_CHAR) {
            handler->started = 1;
        }
        return;
    }

    if (handler->escape_flag) {
        handler->escape_flag = 0;
        if (handler->buffer_index < BUFFER_SIZE - 1) {
            handler->buffer[handler->buffer_index++] = c;
        }
    } else if (c == ESCAPE_CHAR) {
        handler->escape_flag = 1;
    } else if (c == END_CHAR) {
        handler->buffer[handler->buffer_index] = '\0';
        handle_command(handler->buffer);
        handler->buffer_index = 0;
        handler->started = 0;
    } else {
        if (handler->buffer_index < BUFFER_SIZE - 1) {
            handler->buffer[handler->buffer_index++] = c;
        }
    }
}
