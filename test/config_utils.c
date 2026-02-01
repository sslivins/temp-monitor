/**
 * @file config_utils.c
 * @brief Configuration validation utilities (host-testable)
 */

#include "config_utils.h"
#include <string.h>
#include <ctype.h>

config_error_t config_validate_read_interval(uint32_t interval_ms, uint32_t *clamped_ms)
{
    config_error_t result = CONFIG_VALID;
    uint32_t clamped = interval_ms;
    
    if (interval_ms < CONFIG_READ_INTERVAL_MIN_MS) {
        clamped = CONFIG_READ_INTERVAL_MIN_MS;
        result = CONFIG_ERR_TOO_LOW;
    } else if (interval_ms > CONFIG_READ_INTERVAL_MAX_MS) {
        clamped = CONFIG_READ_INTERVAL_MAX_MS;
        result = CONFIG_ERR_TOO_HIGH;
    }
    
    if (clamped_ms != NULL) {
        *clamped_ms = clamped;
    }
    
    return result;
}

config_error_t config_validate_publish_interval(uint32_t interval_ms, uint32_t *clamped_ms)
{
    config_error_t result = CONFIG_VALID;
    uint32_t clamped = interval_ms;
    
    if (interval_ms < CONFIG_PUBLISH_INTERVAL_MIN_MS) {
        clamped = CONFIG_PUBLISH_INTERVAL_MIN_MS;
        result = CONFIG_ERR_TOO_LOW;
    } else if (interval_ms > CONFIG_PUBLISH_INTERVAL_MAX_MS) {
        clamped = CONFIG_PUBLISH_INTERVAL_MAX_MS;
        result = CONFIG_ERR_TOO_HIGH;
    }
    
    if (clamped_ms != NULL) {
        *clamped_ms = clamped;
    }
    
    return result;
}

config_error_t config_validate_resolution(uint8_t resolution)
{
    if (resolution < CONFIG_RESOLUTION_MIN) {
        return CONFIG_ERR_TOO_LOW;
    }
    if (resolution > CONFIG_RESOLUTION_MAX) {
        return CONFIG_ERR_TOO_HIGH;
    }
    return CONFIG_VALID;
}

config_error_t config_validate_friendly_name(const char *name)
{
    if (name == NULL) {
        return CONFIG_ERR_NULL_INPUT;
    }
    
    size_t len = strlen(name);
    
    if (len < CONFIG_FRIENDLY_NAME_MIN_LEN) {
        return CONFIG_ERR_TOO_SHORT;
    }
    
    if (len > CONFIG_FRIENDLY_NAME_MAX_LEN) {
        return CONFIG_ERR_TOO_LONG;
    }
    
    /* Check for printable characters */
    if (!config_is_printable_ascii(name)) {
        return CONFIG_ERR_INVALID_CHARS;
    }
    
    return CONFIG_VALID;
}

config_error_t config_validate_mqtt_uri(const char *uri)
{
    if (uri == NULL) {
        return CONFIG_ERR_NULL_INPUT;
    }
    
    size_t len = strlen(uri);
    
    if (len == 0) {
        return CONFIG_ERR_TOO_SHORT;
    }
    
    if (len > CONFIG_MQTT_URI_MAX_LEN) {
        return CONFIG_ERR_TOO_LONG;
    }
    
    /* Check for valid MQTT URI prefix */
    if (strncmp(uri, "mqtt://", 7) != 0 && 
        strncmp(uri, "mqtts://", 8) != 0 &&
        strncmp(uri, "ws://", 5) != 0 &&
        strncmp(uri, "wss://", 6) != 0) {
        return CONFIG_ERR_INVALID_FORMAT;
    }
    
    return CONFIG_VALID;
}

config_error_t config_validate_wifi_ssid(const char *ssid)
{
    if (ssid == NULL) {
        return CONFIG_ERR_NULL_INPUT;
    }
    
    size_t len = strlen(ssid);
    
    if (len == 0) {
        return CONFIG_ERR_TOO_SHORT;
    }
    
    if (len > CONFIG_WIFI_SSID_MAX_LEN) {
        return CONFIG_ERR_TOO_LONG;
    }
    
    return CONFIG_VALID;
}

int config_is_printable_ascii(const char *str)
{
    if (str == NULL) {
        return 0;
    }
    
    for (const char *p = str; *p != '\0'; p++) {
        /* Allow printable ASCII (32-126) */
        if (*p < 32 || *p > 126) {
            return 0;
        }
    }
    
    return 1;
}

const char *config_error_str(config_error_t error)
{
    switch (error) {
        case CONFIG_VALID:           return "Valid";
        case CONFIG_ERR_NULL_INPUT:  return "Null input";
        case CONFIG_ERR_TOO_LOW:     return "Value too low";
        case CONFIG_ERR_TOO_HIGH:    return "Value too high";
        case CONFIG_ERR_TOO_SHORT:   return "String too short";
        case CONFIG_ERR_TOO_LONG:    return "String too long";
        case CONFIG_ERR_INVALID_FORMAT: return "Invalid format";
        case CONFIG_ERR_INVALID_CHARS:  return "Invalid characters";
        default:                     return "Unknown error";
    }
}
