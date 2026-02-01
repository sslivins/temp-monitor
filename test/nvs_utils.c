/**
 * @file nvs_utils.c
 * @brief NVS key generation utilities (host-testable)
 */

#include "nvs_utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int nvs_generate_sensor_key(const uint8_t *address, char *key, size_t key_len)
{
    if (address == NULL || key == NULL) {
        return -1;
    }
    
    /* Need at least 11 chars: "s_" + 8 hex digits + null */
    if (key_len < 11) {
        return -1;
    }
    
    /* Use last 4 bytes of address (bytes 4-7) for uniqueness */
    /* This keeps key under 15 char NVS limit */
    int len = snprintf(key, key_len, "s_%02x%02x%02x%02x",
                       address[4], address[5], address[6], address[7]);
    
    if (len < 0 || (size_t)len >= key_len) {
        return -1;
    }
    
    return 0;
}

int nvs_validate_key(const char *key)
{
    if (key == NULL) {
        return 0;
    }
    
    size_t len = strlen(key);
    
    /* NVS keys must be 1-15 characters */
    if (len == 0 || len > NVS_KEY_MAX_LEN) {
        return 0;
    }
    
    /* Check characters: alphanumeric and underscore only */
    for (size_t i = 0; i < len; i++) {
        char c = key[i];
        if (!isalnum((unsigned char)c) && c != '_') {
            return 0;
        }
    }
    
    return 1;
}

size_t nvs_get_max_value_len(const char *key)
{
    if (key == NULL || !nvs_validate_key(key)) {
        return 0;
    }
    
    /* Sensor names: 32 chars */
    if (strncmp(key, NVS_KEY_PREFIX_SENSOR, strlen(NVS_KEY_PREFIX_SENSOR)) == 0) {
        return 32;
    }
    
    /* MQTT config: 128 chars for URIs, 64 for credentials */
    if (strncmp(key, NVS_KEY_PREFIX_MQTT, strlen(NVS_KEY_PREFIX_MQTT)) == 0) {
        if (strstr(key, "uri") != NULL) {
            return 128;
        }
        return 64;  /* username, password */
    }
    
    /* WiFi config: 32 for SSID, 64 for password */
    if (strncmp(key, NVS_KEY_PREFIX_WIFI, strlen(NVS_KEY_PREFIX_WIFI)) == 0) {
        if (strstr(key, "ssid") != NULL) {
            return 32;
        }
        return 64;  /* password */
    }
    
    /* Default max for unknown keys */
    return 256;
}

int nvs_is_sensor_key(const char *key)
{
    if (key == NULL) {
        return 0;
    }
    
    /* Sensor keys start with "s_" and are exactly 10 chars (s_ + 8 hex) */
    if (strlen(key) != 10) {
        return 0;
    }
    
    if (strncmp(key, NVS_KEY_PREFIX_SENSOR, strlen(NVS_KEY_PREFIX_SENSOR)) != 0) {
        return 0;
    }
    
    /* Verify remaining chars are hex digits */
    for (int i = 2; i < 10; i++) {
        if (!isxdigit((unsigned char)key[i])) {
            return 0;
        }
    }
    
    return 1;
}

int nvs_parse_sensor_key(const char *key, uint8_t *partial_addr)
{
    if (key == NULL || partial_addr == NULL) {
        return -1;
    }
    
    if (!nvs_is_sensor_key(key)) {
        return -1;
    }
    
    /* Parse hex bytes from key (skip "s_" prefix) */
    for (int i = 0; i < 4; i++) {
        unsigned int byte;
        if (sscanf(key + 2 + i * 2, "%02x", &byte) != 1) {
            return -1;
        }
        partial_addr[i] = (uint8_t)byte;
    }
    
    return 0;
}
