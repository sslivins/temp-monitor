/**
 * @file nvs_utils.h
 * @brief NVS key generation utilities (host-testable)
 */

#ifndef NVS_UTILS_H
#define NVS_UTILS_H

#include <stdint.h>
#include <stddef.h>

/* NVS key limits - NVS keys must be ≤15 chars (excluding null) */
#define NVS_KEY_MAX_LEN 15

/* Key prefixes for different data types */
#define NVS_KEY_PREFIX_SENSOR   "s_"      /* Sensor friendly names */
#define NVS_KEY_PREFIX_MQTT     "mqtt_"   /* MQTT config */
#define NVS_KEY_PREFIX_WIFI     "wifi_"   /* WiFi config */
#define NVS_KEY_PREFIX_SENSOR_CFG "scfg_" /* Sensor settings */

/**
 * @brief Generate NVS key from sensor address
 * 
 * Uses last 4 bytes of address to create unique key within 15 char limit.
 * Format: "s_XXXXXXXX" (10 chars)
 * 
 * @param address 8-byte sensor ROM address
 * @param key Output buffer (must be at least 16 bytes)
 * @param key_len Size of key buffer
 * @return 0 on success, -1 on error
 */
int nvs_generate_sensor_key(const uint8_t *address, char *key, size_t key_len);

/**
 * @brief Validate NVS key format
 * 
 * Checks that key is within length limits and contains valid characters
 * (alphanumeric plus underscore)
 * 
 * @param key Key string to validate
 * @return 1 if valid, 0 if invalid
 */
int nvs_validate_key(const char *key);

/**
 * @brief Get the maximum value length for a given key
 * 
 * Different key types have different max value lengths
 * 
 * @param key Key string
 * @return Maximum value length, or 0 if key is invalid
 */
size_t nvs_get_max_value_len(const char *key);

/**
 * @brief Check if a key is a sensor name key
 * 
 * @param key Key string
 * @return 1 if sensor key, 0 otherwise
 */
int nvs_is_sensor_key(const char *key);

/**
 * @brief Parse sensor address from key
 * 
 * Extracts the partial address (last 4 bytes) from a sensor key
 * 
 * @param key Sensor key string (format: "s_XXXXXXXX")
 * @param partial_addr Output buffer for 4 bytes
 * @return 0 on success, -1 on error
 */
int nvs_parse_sensor_key(const char *key, uint8_t *partial_addr);

#endif /* NVS_UTILS_H */
