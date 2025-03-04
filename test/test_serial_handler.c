/*
 * Copyright (c) 2025 Tuomo Kohtamäki
 * 
 * This file contains unit tests for the Serial Handler library.
 */

#include "unity.h"
#include "serial_handler.h"

static __uint8_t mock_r, mock_g, mock_b;
static unsigned char mock_response[6];
static size_t mock_response_length;

void mock_pwm_callback(__uint8_t r, __uint8_t g, __uint8_t b) {
    mock_r = r;
    mock_g = g;
    mock_b = b;
}

void mock_send_callback(unsigned char c) {
    mock_response[mock_response_length++] = c;
}

void setUp(void) {
    // This function is run before each test
    mock_r = 0;
    mock_g = 0;
    mock_b = 0;
    mock_response_length = 0;
}

void tearDown(void) {
    // This function is run after each test
}

void test_serial_receive_char_should_start_on_start_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL, NULL);

    serial_receive_char(&handler, START_CHAR);
    TEST_ASSERT_TRUE(handler.started);
}

void test_serial_receive_char_should_store_data(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, 'A');
    TEST_ASSERT_EQUAL('A', handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
}

void test_serial_receive_char_should_handle_escape_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    serial_receive_char(&handler, 'B');
    TEST_ASSERT_EQUAL('B', handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
}

void test_serial_receive_char_should_handle_escaped_end_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    serial_receive_char(&handler, END_CHAR);
    TEST_ASSERT_EQUAL(END_CHAR, handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
    TEST_ASSERT_TRUE(handler.started);
}

void test_serial_receive_char_should_handle_two_escape_chars(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    TEST_ASSERT_EQUAL(ESCAPE_CHAR, handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
}

void test_serial_receive_char_should_handle_end_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, 'C');
    serial_receive_char(&handler, END_CHAR);
    TEST_ASSERT_EQUAL('C', handler.buffer[0]);
    TEST_ASSERT_EQUAL(END_CHAR, handler.buffer[1]);
    TEST_ASSERT_EQUAL(0, handler.buffer_index);
    TEST_ASSERT_FALSE(handler.started);
}

void test_handle_command_should_set_led_color(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, mock_pwm_callback, mock_send_callback);

    unsigned char command[] = {MSG_TYPE_REQUEST, OPCODE_SET_LED_COLOR, 10, 20, 30};
    handle_command(command, &handler);

    TEST_ASSERT_EQUAL(10, mock_r);
    TEST_ASSERT_EQUAL(20, mock_g);
    TEST_ASSERT_EQUAL(30, mock_b);
    TEST_ASSERT_EQUAL(4, mock_response_length);
    TEST_ASSERT_EQUAL(START_CHAR, mock_response[0]);
    TEST_ASSERT_EQUAL(MSG_TYPE_RESPONSE, mock_response[1]);
    TEST_ASSERT_EQUAL(1, mock_response[2]);
    TEST_ASSERT_EQUAL(END_CHAR, mock_response[3]);
}

void test_handle_command_should_get_led_color(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, mock_pwm_callback, mock_send_callback);

    handler.r = 10;
    handler.g = 20;
    handler.b = 30;

    unsigned char command[] = {MSG_TYPE_REQUEST, OPCODE_GET_LED_COLOR};
    handle_command(command, &handler);

    TEST_ASSERT_EQUAL(6, mock_response_length);
    TEST_ASSERT_EQUAL(START_CHAR, mock_response[0]);
    TEST_ASSERT_EQUAL(MSG_TYPE_RESPONSE, mock_response[1]);
    TEST_ASSERT_EQUAL(10, mock_response[2]);
    TEST_ASSERT_EQUAL(20, mock_response[3]);
    TEST_ASSERT_EQUAL(30, mock_response[4]);
    TEST_ASSERT_EQUAL(END_CHAR, mock_response[5]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_serial_receive_char_should_start_on_start_char);
    RUN_TEST(test_serial_receive_char_should_store_data);
    RUN_TEST(test_serial_receive_char_should_handle_escape_char);
    RUN_TEST(test_serial_receive_char_should_handle_escaped_end_char);
    RUN_TEST(test_serial_receive_char_should_handle_two_escape_chars);
    RUN_TEST(test_serial_receive_char_should_handle_end_char);
    RUN_TEST(test_handle_command_should_set_led_color);
    RUN_TEST(test_handle_command_should_get_led_color);
    return UNITY_END();
}