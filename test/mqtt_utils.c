/**
 * @file mqtt_utils.c
 * @brief MQTT topic and payload utilities (host-testable)
 */

#include "mqtt_utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int mqtt_generate_state_topic(char *buf, size_t buf_len,
                              const char *base_topic, const char *sensor_id)
{
    if (buf == NULL || base_topic == NULL || sensor_id == NULL) {
        return -1;
    }
    
    int len = snprintf(buf, buf_len, "%s/sensor/%s/state", base_topic, sensor_id);
    
    if (len < 0 || (size_t)len >= buf_len) {
        return -1;  /* Truncated or error */
    }
    
    return len;
}

int mqtt_generate_status_topic(char *buf, size_t buf_len, const char *base_topic)
{
    if (buf == NULL || base_topic == NULL) {
        return -1;
    }
    
    int len = snprintf(buf, buf_len, "%s/status", base_topic);
    
    if (len < 0 || (size_t)len >= buf_len) {
        return -1;
    }
    
    return len;
}

int mqtt_generate_discovery_topic(char *buf, size_t buf_len,
                                  const char *discovery_prefix,
                                  const char *base_topic,
                                  const char *sensor_id)
{
    if (buf == NULL || discovery_prefix == NULL || 
        base_topic == NULL || sensor_id == NULL) {
        return -1;
    }
    
    int len = snprintf(buf, buf_len, "%s/sensor/%s_%s/config",
                       discovery_prefix, base_topic, sensor_id);
    
    if (len < 0 || (size_t)len >= buf_len) {
        return -1;
    }
    
    return len;
}

int mqtt_generate_unique_id(char *buf, size_t buf_len,
                            const char *base_topic, const char *sensor_id)
{
    if (buf == NULL || base_topic == NULL || sensor_id == NULL) {
        return -1;
    }
    
    int len = snprintf(buf, buf_len, "%s_%s", base_topic, sensor_id);
    
    if (len < 0 || (size_t)len >= buf_len) {
        return -1;
    }
    
    return len;
}

int mqtt_format_temperature(char *buf, size_t buf_len, float temperature)
{
    if (buf == NULL) {
        return -1;
    }
    
    int len = snprintf(buf, buf_len, "%.2f", temperature);
    
    if (len < 0 || (size_t)len >= buf_len) {
        return -1;
    }
    
    return len;
}

int mqtt_validate_sensor_id(const char *sensor_id)
{
    if (sensor_id == NULL) {
        return 0;
    }
    
    size_t len = strlen(sensor_id);
    
    /* DS18B20 address is 8 bytes = 16 hex chars */
    if (len != 16) {
        return 0;
    }
    
    /* Check all characters are hex digits */
    for (size_t i = 0; i < len; i++) {
        if (!isxdigit((unsigned char)sensor_id[i])) {
            return 0;
        }
    }
    
    return 1;
}
