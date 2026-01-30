/**
 * @file test_mqtt_utils.c
 * @brief Unit tests for MQTT utilities
 */

#include "unity.h"
#include "mqtt_utils.h"
#include <string.h>
#include <math.h>

/* ===== State Topic Tests ===== */

void test_mqtt_state_topic_basic(void)
{
    char buf[128];
    int len = mqtt_generate_state_topic(buf, sizeof(buf), 
                                        "esp32-poe-temp", "28FF123456789ABC");
    
    TEST_ASSERT_EQUAL_STRING("esp32-poe-temp/sensor/28FF123456789ABC/state", buf);
    TEST_ASSERT_EQUAL_INT(44, len);  /* strlen of the topic */
}

void test_mqtt_state_topic_custom_base(void)
{
    char buf[128];
    mqtt_generate_state_topic(buf, sizeof(buf), "my-device", "AABBCCDD11223344");
    
    TEST_ASSERT_EQUAL_STRING("my-device/sensor/AABBCCDD11223344/state", buf);
}

void test_mqtt_state_topic_buffer_too_small(void)
{
    char buf[20];  /* Too small */
    int len = mqtt_generate_state_topic(buf, sizeof(buf), 
                                        "esp32-poe-temp", "28FF123456789ABC");
    
    TEST_ASSERT_EQUAL_INT(-1, len);  /* Should indicate error */
}

void test_mqtt_state_topic_null_inputs(void)
{
    char buf[128];
    
    TEST_ASSERT_EQUAL_INT(-1, mqtt_generate_state_topic(NULL, 128, "base", "id"));
    TEST_ASSERT_EQUAL_INT(-1, mqtt_generate_state_topic(buf, 128, NULL, "id"));
    TEST_ASSERT_EQUAL_INT(-1, mqtt_generate_state_topic(buf, 128, "base", NULL));
}

/* ===== Status Topic Tests ===== */

void test_mqtt_status_topic_basic(void)
{
    char buf[64];
    int len = mqtt_generate_status_topic(buf, sizeof(buf), "esp32-poe-temp");
    
    TEST_ASSERT_EQUAL_STRING("esp32-poe-temp/status", buf);
    TEST_ASSERT_EQUAL_INT(21, len);
}

void test_mqtt_status_topic_null(void)
{
    char buf[64];
    TEST_ASSERT_EQUAL_INT(-1, mqtt_generate_status_topic(buf, sizeof(buf), NULL));
}

/* ===== Discovery Topic Tests ===== */

void test_mqtt_discovery_topic_basic(void)
{
    char buf[256];
    int len = mqtt_generate_discovery_topic(buf, sizeof(buf),
                                            "homeassistant",
                                            "esp32-poe-temp",
                                            "28FF123456789ABC");
    
    TEST_ASSERT_EQUAL_STRING(
        "homeassistant/sensor/esp32-poe-temp_28FF123456789ABC/config", buf);
    TEST_ASSERT_GREATER_THAN(0, len);
}

void test_mqtt_discovery_topic_custom_prefix(void)
{
    char buf[256];
    mqtt_generate_discovery_topic(buf, sizeof(buf),
                                  "ha", "device", "SENSOR01");
    
    TEST_ASSERT_EQUAL_STRING("ha/sensor/device_SENSOR01/config", buf);
}

/* ===== Unique ID Tests ===== */

void test_mqtt_unique_id_basic(void)
{
    char buf[64];
    int len = mqtt_generate_unique_id(buf, sizeof(buf),
                                      "esp32-poe-temp", "28FF123456789ABC");
    
    TEST_ASSERT_EQUAL_STRING("esp32-poe-temp_28FF123456789ABC", buf);
    TEST_ASSERT_EQUAL_INT(31, len);
}

void test_mqtt_unique_id_short(void)
{
    char buf[64];
    mqtt_generate_unique_id(buf, sizeof(buf), "dev", "S1");
    
    TEST_ASSERT_EQUAL_STRING("dev_S1", buf);
}

/* ===== Temperature Formatting Tests ===== */

void test_mqtt_format_temperature_positive(void)
{
    char buf[32];
    mqtt_format_temperature(buf, sizeof(buf), 23.45f);
    
    TEST_ASSERT_EQUAL_STRING("23.45", buf);
}

void test_mqtt_format_temperature_negative(void)
{
    char buf[32];
    mqtt_format_temperature(buf, sizeof(buf), -5.50f);
    
    TEST_ASSERT_EQUAL_STRING("-5.50", buf);
}

void test_mqtt_format_temperature_zero(void)
{
    char buf[32];
    mqtt_format_temperature(buf, sizeof(buf), 0.0f);
    
    TEST_ASSERT_EQUAL_STRING("0.00", buf);
}

void test_mqtt_format_temperature_rounding(void)
{
    char buf[32];
    mqtt_format_temperature(buf, sizeof(buf), 23.456f);
    
    TEST_ASSERT_EQUAL_STRING("23.46", buf);  /* Rounds to 2 decimal places */
}

void test_mqtt_format_temperature_large(void)
{
    char buf[32];
    mqtt_format_temperature(buf, sizeof(buf), 125.00f);  /* DS18B20 max */
    
    TEST_ASSERT_EQUAL_STRING("125.00", buf);
}

void test_mqtt_format_temperature_small_negative(void)
{
    char buf[32];
    mqtt_format_temperature(buf, sizeof(buf), -55.00f);  /* DS18B20 min */
    
    TEST_ASSERT_EQUAL_STRING("-55.00", buf);
}

/* ===== Sensor ID Validation Tests ===== */

void test_mqtt_validate_sensor_id_valid(void)
{
    TEST_ASSERT_TRUE(mqtt_validate_sensor_id("28FF123456789ABC"));
    TEST_ASSERT_TRUE(mqtt_validate_sensor_id("28ff123456789abc"));  /* lowercase */
    TEST_ASSERT_TRUE(mqtt_validate_sensor_id("0000000000000000"));
    TEST_ASSERT_TRUE(mqtt_validate_sensor_id("FFFFFFFFFFFFFFFF"));
}

void test_mqtt_validate_sensor_id_invalid_length(void)
{
    TEST_ASSERT_FALSE(mqtt_validate_sensor_id("28FF"));          /* Too short */
    TEST_ASSERT_FALSE(mqtt_validate_sensor_id("28FF123456789ABCDE"));  /* Too long */
    TEST_ASSERT_FALSE(mqtt_validate_sensor_id(""));              /* Empty */
}

void test_mqtt_validate_sensor_id_invalid_chars(void)
{
    TEST_ASSERT_FALSE(mqtt_validate_sensor_id("28FF12345678GHIJ"));  /* Non-hex */
    TEST_ASSERT_FALSE(mqtt_validate_sensor_id("28FF-1234-5678-9A"));  /* Dashes */
    TEST_ASSERT_FALSE(mqtt_validate_sensor_id("28FF 123456789AB"));   /* Space */
}

void test_mqtt_validate_sensor_id_null(void)
{
    TEST_ASSERT_FALSE(mqtt_validate_sensor_id(NULL));
}

/* ===== Test Runner ===== */

void run_mqtt_tests(void)
{
    /* State topic tests */
    RUN_TEST(test_mqtt_state_topic_basic);
    RUN_TEST(test_mqtt_state_topic_custom_base);
    RUN_TEST(test_mqtt_state_topic_buffer_too_small);
    RUN_TEST(test_mqtt_state_topic_null_inputs);
    
    /* Status topic tests */
    RUN_TEST(test_mqtt_status_topic_basic);
    RUN_TEST(test_mqtt_status_topic_null);
    
    /* Discovery topic tests */
    RUN_TEST(test_mqtt_discovery_topic_basic);
    RUN_TEST(test_mqtt_discovery_topic_custom_prefix);
    
    /* Unique ID tests */
    RUN_TEST(test_mqtt_unique_id_basic);
    RUN_TEST(test_mqtt_unique_id_short);
    
    /* Temperature formatting tests */
    RUN_TEST(test_mqtt_format_temperature_positive);
    RUN_TEST(test_mqtt_format_temperature_negative);
    RUN_TEST(test_mqtt_format_temperature_zero);
    RUN_TEST(test_mqtt_format_temperature_rounding);
    RUN_TEST(test_mqtt_format_temperature_large);
    RUN_TEST(test_mqtt_format_temperature_small_negative);
    
    /* Sensor ID validation tests */
    RUN_TEST(test_mqtt_validate_sensor_id_valid);
    RUN_TEST(test_mqtt_validate_sensor_id_invalid_length);
    RUN_TEST(test_mqtt_validate_sensor_id_invalid_chars);
    RUN_TEST(test_mqtt_validate_sensor_id_null);
}
