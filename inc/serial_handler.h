/*
 * Copyright (c) 2025 Tuomo Kohtamäki
 * 
 * This file is part of the Serial Handler library.
 */

#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 32

// Special characters
#define START_CHAR 0xFF
#define END_CHAR 0xFE
#define ESCAPE_CHAR 0xFD

// Supported message types
#define MSG_TYPE_REQUEST 0x00
#define MSG_TYPE_RESPONSE 0x01

// Supported opcodes
#define OPCODE_SET_LED_COLOR 0x00
#define OPCODE_GET_LED_COLOR 0x01

typedef void (*CommandCallback)(__uint8_t r, __uint8_t g, __uint8_t b);
typedef void (*UARTSendCallback)(unsigned char c);

typedef struct {
    unsigned char buffer[BUFFER_SIZE];
    int buffer_index;
    int escape_flag;
    int started;
    CommandCallback pwm_callback;
    UARTSendCallback send_callback;
    __uint8_t r;
    __uint8_t g;
    __uint8_t b;
} SerialPortHandler;

void init_serial_port_handler(SerialPortHandler *handler, CommandCallback pwm_callback, UARTSendCallback send_callback);
void handle_command(const unsigned char *command, SerialPortHandler *handler);
void serial_receive_char(SerialPortHandler *handler, __uint8_t c);
void send_serial_response(SerialPortHandler *handler, const __uint8_t *response, size_t length);


#endif // SERIAL_HANDLER_H