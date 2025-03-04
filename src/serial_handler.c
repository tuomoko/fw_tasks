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
void init_serial_port_handler(SerialPortHandler *handler, CommandCallback pwm_callback, UARTSendCallback send_callback) {
    handler->pwm_callback = pwm_callback;
    handler->send_callback = send_callback;
    handler->buffer_index = 0;
    handler->escape_flag = 0;
    handler->started = 0;
    memset(handler->buffer, 0, BUFFER_SIZE);
    handler->r = 0;
    handler->g = 0;
    handler->b = 0;
}

/**
 * @brief Handles a received command.
 * 
 * Decodes the bits and acts accordingly.
 * 
 * @todo This function is called from ISR. Ideally the commands should be handled in a separate task.
 * 
 * @param command Pointer to the command string to handle.
 */
void handle_command(const unsigned char *command, SerialPortHandler *handler) {
    const __uint8_t msg_type = command[0];
    __uint8_t response[3];
    if (msg_type == MSG_TYPE_REQUEST) {
        const __uint8_t opcode = command[1];
        if (opcode == OPCODE_SET_LED_COLOR) {
            handler->r = command[2];
            handler->g = command[3];
            handler->b = command[4];
            handler->pwm_callback(handler->r, handler->g, handler->b);
            response[0] = 1;
            send_serial_response(handler, response, 1);
        }
        else if (opcode == OPCODE_GET_LED_COLOR) {
            response[0] = handler->r;
            response[1] = handler->g;
            response[2] = handler->b;
            send_serial_response(handler, response, 3);
        }
    }
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
        handle_command(handler->buffer, handler);
        handler->buffer_index = 0;
        handler->started = 0;
    } else {
        if (handler->buffer_index < BUFFER_SIZE - 1) {
            handler->buffer[handler->buffer_index++] = c;
        }
    }
}

/**
 * @brief Sends a response over the serial port.
 * 
 * This function sends the response data using the UART send callback.
 * 
 * @param handler Pointer to the SerialPortHandler structure.
 * @param response Pointer to the response data to send.
 * @param length Length of the response data.
 */
void send_serial_response(SerialPortHandler *handler, const __uint8_t *response, size_t length) {
    if (handler->send_callback == NULL) {
        return;
    }
    handler->send_callback(START_CHAR);
    handler->send_callback(MSG_TYPE_RESPONSE);
    for (size_t i = 0; i < length; ++i) {
        handler->send_callback(response[i]);
    }
    handler->send_callback(END_CHAR);
}
