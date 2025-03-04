/*
 * Copyright (c) 2025 Tuomo Kohtamäki
 * 
 * This file contains unit tests for the Serial Handler library.
 */

#include "unity.h"
#include "serial_handler.h"

void setUp(void) {
    // This function is run before each test
}

void tearDown(void) {
    // This function is run after each test
}

void test_serial_receive_char_should_start_on_start_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL);

    serial_receive_char(&handler, START_CHAR);
    TEST_ASSERT_TRUE(handler.started);
}

void test_serial_receive_char_should_store_data(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, 'A');
    TEST_ASSERT_EQUAL('A', handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
}

void test_serial_receive_char_should_handle_escape_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    serial_receive_char(&handler, 'B');
    TEST_ASSERT_EQUAL('B', handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
}

void test_serial_receive_char_should_handle_escaped_end_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    serial_receive_char(&handler, END_CHAR);
    TEST_ASSERT_EQUAL(END_CHAR, handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
    TEST_ASSERT_TRUE(handler.started);
}

void test_serial_receive_char_should_handle_two_escape_chars(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    serial_receive_char(&handler, ESCAPE_CHAR);
    TEST_ASSERT_EQUAL(ESCAPE_CHAR, handler.buffer[0]);
    TEST_ASSERT_EQUAL(1, handler.buffer_index);
}

void test_serial_receive_char_should_handle_end_char(void) {
    SerialPortHandler handler;
    init_serial_port_handler(&handler, NULL);

    serial_receive_char(&handler, START_CHAR);
    serial_receive_char(&handler, 'C');
    serial_receive_char(&handler, END_CHAR);
    TEST_ASSERT_EQUAL('C', handler.buffer[0]);
    TEST_ASSERT_EQUAL(END_CHAR, handler.buffer[1]);
    TEST_ASSERT_EQUAL(0, handler.buffer_index);
    TEST_ASSERT_FALSE(handler.started);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_serial_receive_char_should_start_on_start_char);
    RUN_TEST(test_serial_receive_char_should_store_data);
    RUN_TEST(test_serial_receive_char_should_handle_escape_char);
    RUN_TEST(test_serial_receive_char_should_handle_escaped_end_char);
    RUN_TEST(test_serial_receive_char_should_handle_two_escape_chars);
    RUN_TEST(test_serial_receive_char_should_handle_end_char);
    return UNITY_END();
}