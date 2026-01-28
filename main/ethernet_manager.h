/**
 * @file ethernet_manager.h
 * @brief Ethernet/POE connection management for ESP32-POE-ISO
 */

#ifndef ETHERNET_MANAGER_H
#define ETHERNET_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Initialize Ethernet hardware for ESP32-POE-ISO
 * 
 * Configures the LAN8720 PHY used on ESP32-POE-ISO boards
 */
esp_err_t ethernet_manager_init(void);

/**
 * @brief Start Ethernet connection
 */
esp_err_t ethernet_manager_start(void);

/**
 * @brief Stop Ethernet connection
 */
esp_err_t ethernet_manager_stop(void);

/**
 * @brief Check if Ethernet is connected
 */
bool ethernet_manager_is_connected(void);

/**
 * @brief Get Ethernet IP address as string
 */
const char* ethernet_manager_get_ip(void);

#endif /* ETHERNET_MANAGER_H */
