/*
 * Copyright (c) 2025 Tuomo Kohtamäki
 * 
 * This file is part of the Serial Handler library.
 */

#include <stdio.h>
#include <string.h>
#include "serial_handler.h"

/**
 * @brief Initializes the serial port handler.
 * 
 * This function sets the initial values for the buffer index, escape flag,
 * and started flag. It also clears the buffer.
 * 
 * @param handler Pointer to the SerialPortHandler structure to initialize.
 */
void init_serial_port_handler(SerialPortHandler *handler) {
    handler->buffer_index = 0;
    handler->escape_flag = 0;
    handler->started = 0;
    memset(handler->buffer, 0, BUFFER_SIZE);
}

/**
 * @brief Handles a received command.
 * 
 * Decodes the bits and acts accordingly.
 * 
 * @todo This function is called from ISR. The commands should be handled in a separate task.
 * 
 * @param command Pointer to the command string to handle.
 */
void handle_command(const unsigned char *command) {
    // Implement your command handling logic here
}

/**
 * @brief Receives a character from the serial port.
 * 
 * This function processes a received character and updates the state of the
 * serial port handler. It handles start, escape, and end characters, and
 * stores received data in the buffer.
 * 
 * @param handler Pointer to the SerialPortHandler structure.
 * @param c The received character.
 */
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
        handler->buffer[handler->buffer_index] = c;
        handle_command(handler->buffer);
        handler->buffer_index = 0;
        handler->started = 0;
    } else {
        if (handler->buffer_index < BUFFER_SIZE - 1) {
            handler->buffer[handler->buffer_index++] = c;
        }
    }
}
