/**
 * @file mqtt_utils.h
 * @brief MQTT topic and payload utilities (host-testable)
 */

#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

#include <stddef.h>

/**
 * @brief Generate MQTT state topic for a sensor
 * 
 * Format: {base_topic}/sensor/{sensor_id}/state
 * 
 * @param buf Output buffer
 * @param buf_len Buffer length
 * @param base_topic Base MQTT topic (e.g., "esp32-poe-temp")
 * @param sensor_id Sensor ID (address string)
 * @return Length written (excluding null), or -1 on error
 */
int mqtt_generate_state_topic(char *buf, size_t buf_len, 
                              const char *base_topic, const char *sensor_id);

/**
 * @brief Generate MQTT status/availability topic
 * 
 * Format: {base_topic}/status
 * 
 * @param buf Output buffer
 * @param buf_len Buffer length
 * @param base_topic Base MQTT topic
 * @return Length written (excluding null), or -1 on error
 */
int mqtt_generate_status_topic(char *buf, size_t buf_len, const char *base_topic);

/**
 * @brief Generate Home Assistant discovery topic
 * 
 * Format: {discovery_prefix}/sensor/{base_topic}_{sensor_id}/config
 * 
 * @param buf Output buffer
 * @param buf_len Buffer length
 * @param discovery_prefix HA discovery prefix (usually "homeassistant")
 * @param base_topic Base MQTT topic
 * @param sensor_id Sensor ID (address string)
 * @return Length written (excluding null), or -1 on error
 */
int mqtt_generate_discovery_topic(char *buf, size_t buf_len,
                                  const char *discovery_prefix,
                                  const char *base_topic, 
                                  const char *sensor_id);

/**
 * @brief Generate unique ID for Home Assistant entity
 * 
 * Format: {base_topic}_{sensor_id}
 * 
 * @param buf Output buffer
 * @param buf_len Buffer length
 * @param base_topic Base MQTT topic
 * @param sensor_id Sensor ID (address string)
 * @return Length written (excluding null), or -1 on error
 */
int mqtt_generate_unique_id(char *buf, size_t buf_len,
                            const char *base_topic, const char *sensor_id);

/**
 * @brief Format temperature value as string
 * 
 * @param buf Output buffer
 * @param buf_len Buffer length
 * @param temperature Temperature in Celsius
 * @return Length written (excluding null), or -1 on error
 */
int mqtt_format_temperature(char *buf, size_t buf_len, float temperature);

/**
 * @brief Validate sensor ID (should be hex address string)
 * 
 * @param sensor_id Sensor ID to validate
 * @return 1 if valid, 0 if invalid
 */
int mqtt_validate_sensor_id(const char *sensor_id);

#endif /* MQTT_UTILS_H */
