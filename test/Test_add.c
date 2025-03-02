#include "unity.h"

#include <stdint.h>
#include <stdbool.h>
#include "add.h"


void setUp(void) {}   // Runs before each test
void tearDown(void) {} // Runs after each test

void test_add_should_return_sum_of_two_integers(void) {
    TEST_ASSERT_EQUAL(5, add(2, 3));
    TEST_ASSERT_EQUAL(0, add(-1, 1));
    TEST_ASSERT_EQUAL(-3, add(-1, -2));
    TEST_ASSERT_EQUAL(100, add(50, 50));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_should_return_sum_of_two_integers);
    return UNITY_END();
}
