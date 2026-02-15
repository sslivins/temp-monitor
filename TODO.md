# TODO

## Live Testing Infrastructure

### Hardware Setup
- [ ] Acquire Raspberry Pi Zero 2 W
- [ ] Install GitHub Actions runner on Raspberry Pi Zero 2 W
- [ ] Connect Raspberry Pi to ESP32-POE via serial port
  - Serial connection will enable communication for testing and monitoring
  - Document serial port configuration (baud rate, pins, etc.)
- [ ] Set up physical test environment with ESP32-POE device

### Test Coverage

The live tests will augment existing unit tests:
- Current unit tests: address_utils, config_utils, mqtt_utils, nvs_utils, version_compare
- Live tests will provide end-to-end validation on actual hardware

#### Web Interface Tests
- [ ] Test main dashboard (/) 
  - Verify temperature sensor readings display correctly
  - Test live updates and refresh functionality
  - Validate sensor identification and change detection highlighting
- [ ] Test configuration page (/config)
  - MQTT settings (broker, port, credentials)
  - WiFi settings (SSID, password)
  - Interval settings (read interval, publish interval)
  - OTA URL configuration
  - Security settings (enable/disable auth, username, password)
  - Log level control
  - Log buffer view, clear, and download
- [ ] Test OTA page (/ota)
  - Manual firmware upload functionality
  - Automatic update check from GitHub releases
  - Progress display during updates
- [ ] Test authentication flows
  - Login page (/login)
  - Session management
  - Protected page redirects when not authenticated
  - Logout functionality
- [ ] Test mDNS resolution (thermux.local access)
- [ ] Test service discovery (_thermux._tcp, _http._tcp)

#### API Tests
- [ ] **Sensor Endpoints**
  - GET /api/sensors - Retrieve sensor readings
  - Validate JSON response format
  - Test with multiple sensors (up to 20)
- [ ] **Status Endpoints**
  - GET /api/status - Device status and network info
  - Verify uptime, connectivity, firmware version
- [ ] **Configuration Endpoints**
  - GET/POST /api/config - Configuration retrieval and updates
  - Test MQTT, WiFi, interval settings via API
- [ ] **Authentication Endpoints**
  - POST /api/auth/login - Login with credentials
  - POST /api/auth/logout - Session termination
  - GET /api/auth/status - Check authentication status
  - GET /api/config/auth - Get auth config including API key
  - POST /api/config/auth/regenerate-key - Generate new API key
- [ ] **OTA Endpoints**
  - POST /api/ota/update - Trigger OTA update
  - Test automatic download from GitHub releases
- [ ] **API Key Authentication**
  - Test X-API-Key header authentication
  - Verify stateless API access
  - Test API key regeneration
- [ ] **Session Authentication**
  - Test cookie-based sessions
  - Verify 401 responses with login_required flag
  - Test session expiration (7 day timeout)

#### Hardware Integration Tests
- [ ] Test actual DS18B20 sensor communication
  - Verify 1-Wire bus operations
  - Test with multiple sensors on bus
  - Validate parallel read optimization (skip ROM command)
  - Test different sensor resolutions (9-bit to 12-bit)
- [ ] Test bus error tracking
  - Monitor CRC error rates
  - Verify error reporting in web UI and Home Assistant
- [ ] Test Ethernet connectivity (primary)
- [ ] Test WiFi fallback functionality
- [ ] Test PoE power delivery

#### MQTT Integration Tests
- [ ] Test MQTT broker connection
- [ ] Verify Home Assistant auto-discovery messages
- [ ] Test sensor entity registration
- [ ] Test diagnostic entity publication (network status, bus errors)
- [ ] Verify MQTT publish intervals

#### Performance Tests
- [ ] Measure read times with various sensor counts (1, 5, 10, 20)
- [ ] Verify parallel read performance vs sequential
- [ ] Test system under continuous load
- [ ] Monitor memory usage and stability

### CI/CD Integration
- [ ] Create GitHub Actions workflow for live tests
- [ ] Configure workflow to run on Raspberry Pi self-hosted runner
- [ ] Set up test reporting and artifacts
- [ ] Define test success/failure criteria
- [ ] Schedule regular test runs (nightly/on-demand)

### Documentation
- [ ] Document live test setup procedure
- [ ] Create troubleshooting guide for test failures
- [ ] Document serial port connection details
- [ ] Add live test results to PR checks (if applicable)
