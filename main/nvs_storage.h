/**
 * @file nvs_storage.h
 * @brief NVS storage interface for persistent configuration
 */

#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_FRIENDLY_NAME_LEN 32
#define SENSOR_ADDRESS_LEN 8

/**
 * @brief Initialize NVS storage subsystem
 */
esp_err_t nvs_storage_init(void);

/**
 * @brief Save a sensor's friendly name
 * @param sensor_address 8-byte sensor ROM address
 * @param friendly_name Friendly name string (max 32 chars)
 */
esp_err_t nvs_storage_save_sensor_name(const uint8_t *sensor_address, const char *friendly_name);

/**
 * @brief Load a sensor's friendly name
 * @param sensor_address 8-byte sensor ROM address
 * @param friendly_name Buffer to store friendly name
 * @param max_len Maximum length of buffer
 * @return ESP_OK if found, ESP_ERR_NOT_FOUND if no name saved
 */
esp_err_t nvs_storage_load_sensor_name(const uint8_t *sensor_address, char *friendly_name, size_t max_len);

/**
 * @brief Delete a sensor's friendly name
 * @param sensor_address 8-byte sensor ROM address
 */
esp_err_t nvs_storage_delete_sensor_name(const uint8_t *sensor_address);

/**
 * @brief Save MQTT configuration
 */
esp_err_t nvs_storage_save_mqtt_config(const char *broker_uri, const char *username, const char *password);

/**
 * @brief Load MQTT configuration
 */
esp_err_t nvs_storage_load_mqtt_config(char *broker_uri, size_t uri_len,
                                        char *username, size_t user_len,
                                        char *password, size_t pass_len);

/**
 * @brief Save WiFi credentials
 */
esp_err_t nvs_storage_save_wifi_config(const char *ssid, const char *password);

/**
 * @brief Load WiFi credentials
 */
esp_err_t nvs_storage_load_wifi_config(char *ssid, size_t ssid_len,
                                        char *password, size_t pass_len);

/**
 * @brief Factory reset - erase all stored configuration
 */
esp_err_t nvs_storage_factory_reset(void);

#endif /* NVS_STORAGE_H */
