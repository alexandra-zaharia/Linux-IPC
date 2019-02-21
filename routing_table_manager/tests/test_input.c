//
// Created by Alexandra Zaharia on 21/02/19.
//


#include "unity.h"
#include "input.h"


void setUp(void)
{
    // set stuff up here
}

void tearDown()
{
    // clean stuff up here
}


void test_read_ip_address_from_buffer_accepts_correct_ip_addresses()
{
    TEST_ASSERT_EQUAL_INT(0, read_ip_address_from_buffer("1.2.3.4"));
    TEST_ASSERT_EQUAL_INT(0, read_ip_address_from_buffer("0.0.0.0"));
    TEST_ASSERT_EQUAL_INT(0, read_ip_address_from_buffer("255.255.255.255"));
}

void test_read_ip_address_from_buffer_rejects_incorrect_ip_addresses()
{
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2.3.-1"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2.3.256"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2.3."));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2.3"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2."));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1."));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.."));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("..."));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2..3.4"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2.3.4x"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2.3.4/"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("1.2.3.4/5"));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer(""));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer(" "));
    TEST_ASSERT_EQUAL_INT(-1, read_ip_address_from_buffer("x"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_read_ip_address_from_buffer_accepts_correct_ip_addresses);
    RUN_TEST(test_read_ip_address_from_buffer_rejects_incorrect_ip_addresses);
    return UNITY_END();
}