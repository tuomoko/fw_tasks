#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 32
#define START_CHAR 0xFF
#define END_CHAR 0xFE
#define ESCAPE_CHAR 0xFD

typedef struct {
    char buffer[BUFFER_SIZE];
    int buffer_index;
    int escape_flag;
    int started;
} SerialPortHandler;

void init_serial_port_handler(SerialPortHandler *handler);
void handle_command(const char *command);
void serial_receive_char(SerialPortHandler *handler, __uint8_t c);

#endif // SERIAL_HANDLER_H