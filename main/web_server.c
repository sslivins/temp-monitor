/**
 * @file web_server.c
 * @brief HTTP web server with REST API and embedded web portal
 */

#include "web_server.h"
#include "sensor_manager.h"
#include "ota_updater.h"
#include "nvs_storage.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include <string.h>

static const char *TAG = "web_server";
static httpd_handle_t s_server = NULL;

extern const char *APP_VERSION;

/* Embedded HTML page */
static const char INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Temperature Monitor</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { 
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
            color: #fff;
            min-height: 100vh;
            padding: 20px;
        }
        .container { max-width: 1200px; margin: 0 auto; }
        h1 { 
            text-align: center; 
            margin-bottom: 10px;
            font-size: 2em;
        }
        .version { 
            text-align: center; 
            color: #888; 
            margin-bottom: 30px;
            font-size: 0.9em;
        }
        .status-bar {
            display: flex;
            justify-content: center;
            gap: 20px;
            margin-bottom: 30px;
            flex-wrap: wrap;
        }
        .status-item {
            background: rgba(255,255,255,0.1);
            padding: 10px 20px;
            border-radius: 20px;
            font-size: 0.9em;
        }
        .status-online { color: #4ade80; }
        .status-offline { color: #f87171; }
        .sensors-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
            gap: 20px;
        }
        .sensor-card {
            background: rgba(255,255,255,0.05);
            border-radius: 15px;
            padding: 20px;
            border: 1px solid rgba(255,255,255,0.1);
            transition: transform 0.2s, box-shadow 0.2s;
        }
        .sensor-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        .sensor-temp {
            font-size: 3em;
            font-weight: 300;
            color: #60a5fa;
            margin: 10px 0;
        }
        .sensor-name {
            font-size: 1.2em;
            margin-bottom: 5px;
        }
        .sensor-address {
            font-size: 0.8em;
            color: #888;
            font-family: monospace;
        }
        .sensor-name-input {
            width: 100%;
            padding: 8px 12px;
            border: 1px solid rgba(255,255,255,0.2);
            border-radius: 8px;
            background: rgba(255,255,255,0.1);
            color: #fff;
            font-size: 1em;
            margin-top: 15px;
        }
        .sensor-name-input:focus {
            outline: none;
            border-color: #60a5fa;
        }
        .btn {
            padding: 8px 16px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-size: 0.9em;
            transition: background 0.2s;
            margin-top: 10px;
        }
        .btn-primary {
            background: #3b82f6;
            color: white;
        }
        .btn-primary:hover { background: #2563eb; }
        .btn-secondary {
            background: rgba(255,255,255,0.1);
            color: white;
        }
        .btn-secondary:hover { background: rgba(255,255,255,0.2); }
        .actions {
            text-align: center;
            margin-top: 30px;
        }
        .toast {
            position: fixed;
            bottom: 20px;
            right: 20px;
            background: #22c55e;
            color: white;
            padding: 12px 24px;
            border-radius: 8px;
            opacity: 0;
            transition: opacity 0.3s;
        }
        .toast.show { opacity: 1; }
        .toast.error { background: #ef4444; }
        .loading { opacity: 0.5; }
        @media (max-width: 600px) {
            .sensor-temp { font-size: 2.5em; }
            h1 { font-size: 1.5em; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌡️ ESP32 Temperature Monitor</h1>
        <div class="version" id="version">Version loading...</div>
        
        <div class="status-bar">
            <div class="status-item">
                <span id="sensor-count">0</span> Sensors
            </div>
            <div class="status-item">
                MQTT: <span id="mqtt-status" class="status-offline">Offline</span>
            </div>
            <div class="status-item">
                Last Update: <span id="last-update">-</span>
            </div>
        </div>

        <div class="sensors-grid" id="sensors-grid">
            <div class="sensor-card loading">Loading sensors...</div>
        </div>

        <div class="actions">
            <button class="btn btn-secondary" onclick="rescanSensors()">🔄 Rescan Sensors</button>
            <button class="btn btn-secondary" onclick="checkOTA()">📦 Check for Updates</button>
        </div>
    </div>

    <div class="toast" id="toast"></div>

    <script>
        let sensors = [];
        let updateInterval;

        async function fetchSensors() {
            try {
                const response = await fetch('/api/sensors');
                sensors = await response.json();
                renderSensors();
                document.getElementById('sensor-count').textContent = sensors.length;
                document.getElementById('last-update').textContent = new Date().toLocaleTimeString();
            } catch (err) {
                showToast('Failed to fetch sensors', true);
            }
        }

        async function fetchStatus() {
            try {
                const response = await fetch('/api/status');
                const status = await response.json();
                document.getElementById('version').textContent = 'Version ' + status.version;
                document.getElementById('mqtt-status').textContent = status.mqtt_connected ? 'Online' : 'Offline';
                document.getElementById('mqtt-status').className = status.mqtt_connected ? 'status-online' : 'status-offline';
            } catch (err) {
                console.error('Failed to fetch status');
            }
        }

        function renderSensors() {
            const grid = document.getElementById('sensors-grid');
            if (sensors.length === 0) {
                grid.innerHTML = '<div class="sensor-card">No sensors found. Click "Rescan Sensors" to detect connected sensors.</div>';
                return;
            }
            
            grid.innerHTML = sensors.map(sensor => `
                <div class="sensor-card" data-address="${sensor.address}">
                    <div class="sensor-name">${sensor.friendly_name || sensor.address}</div>
                    <div class="sensor-address">${sensor.address}</div>
                    <div class="sensor-temp">${sensor.valid ? sensor.temperature.toFixed(1) + '°C' : '--.-°C'}</div>
                    <input type="text" class="sensor-name-input" 
                           placeholder="Enter friendly name" 
                           value="${sensor.friendly_name || ''}"
                           onkeypress="if(event.key==='Enter') saveName('${sensor.address}', this.value)">
                    <button class="btn btn-primary" onclick="saveName('${sensor.address}', this.previousElementSibling.value)">
                        Save Name
                    </button>
                </div>
            `).join('');
        }

        async function saveName(address, name) {
            try {
                const response = await fetch('/api/sensors/' + address + '/name', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ friendly_name: name })
                });
                if (response.ok) {
                    showToast('Name saved successfully');
                    fetchSensors();
                } else {
                    showToast('Failed to save name', true);
                }
            } catch (err) {
                showToast('Error saving name', true);
            }
        }

        async function rescanSensors() {
            try {
                showToast('Scanning for sensors...');
                const response = await fetch('/api/sensors/rescan', { method: 'POST' });
                if (response.ok) {
                    showToast('Scan complete');
                    fetchSensors();
                } else {
                    showToast('Scan failed', true);
                }
            } catch (err) {
                showToast('Error during scan', true);
            }
        }

        async function checkOTA() {
            try {
                showToast('Checking for updates...');
                const response = await fetch('/api/ota/check', { method: 'POST' });
                const result = await response.json();
                if (result.update_available) {
                    if (confirm('Update available: ' + result.latest_version + '. Install now?')) {
                        fetch('/api/ota/update', { method: 'POST' });
                        showToast('Update started. Device will restart.');
                    }
                } else {
                    showToast('Already up to date: ' + result.current_version);
                }
            } catch (err) {
                showToast('Error checking for updates', true);
            }
        }

        function showToast(message, isError = false) {
            const toast = document.getElementById('toast');
            toast.textContent = message;
            toast.className = 'toast show' + (isError ? ' error' : '');
            setTimeout(() => toast.className = 'toast', 3000);
        }

        // Initial load
        fetchStatus();
        fetchSensors();
        
        // Auto-refresh every 5 seconds
        updateInterval = setInterval(fetchSensors, 5000);
    </script>
</body>
</html>
)rawliteral";

/**
 * @brief Handler for GET /
 */
static esp_err_t index_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, INDEX_HTML, strlen(INDEX_HTML));
    return ESP_OK;
}

/**
 * @brief Handler for GET /api/status
 */
static esp_err_t api_status_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "version", APP_VERSION);
    cJSON_AddNumberToObject(root, "sensor_count", sensor_manager_get_count());
    cJSON_AddNumberToObject(root, "uptime_seconds", esp_log_timestamp() / 1000);
    cJSON_AddNumberToObject(root, "free_heap", esp_get_free_heap_size());
    
    extern bool mqtt_ha_is_connected(void);
    cJSON_AddBoolToObject(root, "mqtt_connected", mqtt_ha_is_connected());

    char *json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    free(json);
    
    return ESP_OK;
}

/**
 * @brief Handler for GET /api/sensors
 */
static esp_err_t api_sensors_get_handler(httpd_req_t *req)
{
    int count;
    const managed_sensor_t *sensors = sensor_manager_get_sensors(&count);

    cJSON *root = cJSON_CreateArray();
    
    for (int i = 0; i < count; i++) {
        cJSON *sensor = cJSON_CreateObject();
        cJSON_AddStringToObject(sensor, "address", sensors[i].address_str);
        cJSON_AddNumberToObject(sensor, "temperature", sensors[i].hw_sensor.temperature);
        cJSON_AddBoolToObject(sensor, "valid", sensors[i].hw_sensor.valid);
        
        if (sensors[i].has_friendly_name) {
            cJSON_AddStringToObject(sensor, "friendly_name", sensors[i].friendly_name);
        } else {
            cJSON_AddNullToObject(sensor, "friendly_name");
        }
        
        cJSON_AddItemToArray(root, sensor);
    }

    char *json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    free(json);
    
    return ESP_OK;
}

/**
 * @brief Handler for POST /api/sensors/rescan
 */
static esp_err_t api_sensors_rescan_handler(httpd_req_t *req)
{
    esp_err_t err = sensor_manager_rescan();
    
    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "success", err == ESP_OK);
    cJSON_AddNumberToObject(root, "sensor_count", sensor_manager_get_count());

    char *json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    free(json);
    
    return ESP_OK;
}

/**
 * @brief Handler for POST /api/sensors/:address/name
 */
static esp_err_t api_sensor_name_handler(httpd_req_t *req)
{
    /* Extract address from URI */
    char address[20] = {0};
    const char *uri = req->uri;
    
    /* URI format: /api/sensors/XXXX/name */
    const char *start = strstr(uri, "/api/sensors/");
    if (start) {
        start += strlen("/api/sensors/");
        const char *end = strstr(start, "/name");
        if (end && (end - start) < sizeof(address)) {
            strncpy(address, start, end - start);
        }
    }

    if (strlen(address) == 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid address");
        return ESP_FAIL;
    }

    /* Read request body */
    char content[128];
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No body");
        return ESP_FAIL;
    }
    content[ret] = '\0';

    /* Parse JSON */
    cJSON *root = cJSON_Parse(content);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    cJSON *name_item = cJSON_GetObjectItem(root, "friendly_name");
    if (!cJSON_IsString(name_item)) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing friendly_name");
        return ESP_FAIL;
    }

    esp_err_t err = sensor_manager_set_friendly_name(address, name_item->valuestring);
    cJSON_Delete(root);

    cJSON *response = cJSON_CreateObject();
    cJSON_AddBoolToObject(response, "success", err == ESP_OK);

    char *json = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    free(json);
    
    return ESP_OK;
}

/**
 * @brief Handler for POST /api/ota/check
 */
static esp_err_t api_ota_check_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    
#if CONFIG_OTA_ENABLED
    bool update_available = false;
    char latest_version[32] = {0};
    
    esp_err_t err = ota_check_for_update();
    update_available = ota_is_update_available();
    ota_get_latest_version(latest_version, sizeof(latest_version));
    
    cJSON_AddBoolToObject(root, "update_available", update_available);
    cJSON_AddStringToObject(root, "current_version", APP_VERSION);
    cJSON_AddStringToObject(root, "latest_version", latest_version);
#else
    cJSON_AddBoolToObject(root, "update_available", false);
    cJSON_AddStringToObject(root, "current_version", APP_VERSION);
    cJSON_AddStringToObject(root, "error", "OTA disabled");
#endif

    char *json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    free(json);
    
    return ESP_OK;
}

/**
 * @brief Handler for POST /api/ota/update
 */
static esp_err_t api_ota_update_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    
#if CONFIG_OTA_ENABLED
    if (ota_is_update_available()) {
        cJSON_AddBoolToObject(root, "started", true);
        cJSON_AddStringToObject(root, "message", "Update starting, device will restart");
        
        char *json = cJSON_PrintUnformatted(root);
        cJSON_Delete(root);
        
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json, strlen(json));
        free(json);
        
        /* Start OTA in background */
        ota_start_update();
    } else {
        cJSON_AddBoolToObject(root, "started", false);
        cJSON_AddStringToObject(root, "message", "No update available");
        
        char *json = cJSON_PrintUnformatted(root);
        cJSON_Delete(root);
        
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json, strlen(json));
        free(json);
    }
#else
    cJSON_AddBoolToObject(root, "started", false);
    cJSON_AddStringToObject(root, "error", "OTA disabled");
    
    char *json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    free(json);
#endif
    
    return ESP_OK;
}

esp_err_t web_server_start(void)
{
    ESP_LOGI(TAG, "Starting web server on port %d", CONFIG_WEB_SERVER_PORT);

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = CONFIG_WEB_SERVER_PORT;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 10;

    esp_err_t err = httpd_start(&s_server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start web server");
        return err;
    }

    /* Register URI handlers */
    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_get_handler,
    };
    httpd_register_uri_handler(s_server, &index_uri);

    httpd_uri_t status_uri = {
        .uri = "/api/status",
        .method = HTTP_GET,
        .handler = api_status_handler,
    };
    httpd_register_uri_handler(s_server, &status_uri);

    httpd_uri_t sensors_uri = {
        .uri = "/api/sensors",
        .method = HTTP_GET,
        .handler = api_sensors_get_handler,
    };
    httpd_register_uri_handler(s_server, &sensors_uri);

    httpd_uri_t rescan_uri = {
        .uri = "/api/sensors/rescan",
        .method = HTTP_POST,
        .handler = api_sensors_rescan_handler,
    };
    httpd_register_uri_handler(s_server, &rescan_uri);

    httpd_uri_t sensor_name_uri = {
        .uri = "/api/sensors/*/name",
        .method = HTTP_POST,
        .handler = api_sensor_name_handler,
    };
    httpd_register_uri_handler(s_server, &sensor_name_uri);

    httpd_uri_t ota_check_uri = {
        .uri = "/api/ota/check",
        .method = HTTP_POST,
        .handler = api_ota_check_handler,
    };
    httpd_register_uri_handler(s_server, &ota_check_uri);

    httpd_uri_t ota_update_uri = {
        .uri = "/api/ota/update",
        .method = HTTP_POST,
        .handler = api_ota_update_handler,
    };
    httpd_register_uri_handler(s_server, &ota_update_uri);

    ESP_LOGI(TAG, "Web server started");
    return ESP_OK;
}

esp_err_t web_server_stop(void)
{
    if (s_server) {
        httpd_stop(s_server);
        s_server = NULL;
    }
    return ESP_OK;
}
