/**
 * @file web_server.h
 * @brief HTTP web server with REST API and web portal
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_err.h"

/**
 * @brief Start the web server
 */
esp_err_t web_server_start(void);

/**
 * @brief Stop the web server
 */
esp_err_t web_server_stop(void);

#endif /* WEB_SERVER_H */
