
# ESP Phishing Demo

**Developed by 0x0806**

## ⚠️ Disclaimer

This project is for **educational and security research purposes only**. It demonstrates potential WiFi security vulnerabilities and should only be used in controlled environments with proper authorization. Unauthorized use of this software is illegal and unethical.

## 📋 Overview

ESP Phishing Demo is a proof-of-concept WiFi security testing tool built for ESP8266 microcontrollers. It demonstrates common attack vectors including:

- **WiFi Network Scanning** - Discovers nearby wireless networks
- **Deauthentication Attacks** - Forces clients to disconnect from target networks
- **Evil Twin Access Points** - Creates fake hotspots mimicking legitimate networks
- **Credential Harvesting** - Captures entered passwords through captive portals

## 🔧 Hardware Requirements

- ESP8266 microcontroller (NodeMCU, Wemos D1 Mini, etc.)
- USB cable for programming
- Computer with Arduino IDE

## 📦 Software Dependencies

- Arduino IDE
- ESP8266 Arduino Core
- Required Libraries:
  - `ESP8266WiFi`
  - `DNSServer`
  - `ESP8266WebServer`
  - `ESP8266HTTPClient`

## 🚀 Installation & Setup

1. **Install Arduino IDE** if not already installed
2. **Add ESP8266 Board Manager**:
   - Go to `File` → `Preferences`
   - Add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to Additional Board Manager URLs
3. **Install ESP8266 Core**:
   - Go to `Tools` → `Board` → `Board Manager`
   - Search for "ESP8266" and install
4. **Upload the Code**:
   - Connect your ESP8266 to your computer
   - Select the correct board and port in Arduino IDE
   - Upload `esp-phish-demo.ino`

## 🎯 Usage

### Initial Setup
1. Power on the ESP8266
2. Connect to the WiFi network `ESP Phish` (password: `0x0806`)
3. Open a web browser and navigate to `192.168.4.1`

### Admin Interface
- Access the admin panel at `192.168.4.1/admin`
- View discovered networks and their details
- Select target networks for attacks

### Attack Modes

#### 1. Network Scanning
- Automatically scans for nearby WiFi networks every 15 seconds
- Displays SSID, BSSID, and channel information

#### 2. Deauthentication Attack
- Select a target network from the list
- Click "Start Deauth" to begin disconnecting clients
- Monitor attack progress in serial output

#### 3. Evil Twin Attack
- Choose a target network to clone
- Click "Start EvilTwin" to create a fake access point
- Victims connecting will see a fake "firmware update" portal
- Captured credentials are displayed in the admin interface

## 🎨 Features

- **Modern Web Interface** - Clean, responsive design with Bootstrap-inspired styling
- **Real-time Updates** - Dynamic network scanning and status updates
- **Mobile Friendly** - Works on smartphones and tablets
- **Serial Monitoring** - Debug output for development and monitoring
- **Credential Logging** - Captured passwords are stored and displayed

## 🔒 Security Features

- Admin interface protection
- Input validation for password attempts
- Rate limiting for connection attempts
- Automatic cleanup of resources

## 📊 Web Interface Screenshots

The interface includes:
- Network discovery table with real-time updates
- Attack control buttons with status indicators
- Credential capture display
- Mobile-responsive design

## 🛠️ Technical Details

### Network Architecture
- Creates AP on `192.168.4.1`
- DNS server redirects all requests to captive portal
- Web server handles HTTP requests and form submissions

### Attack Vectors
- **802.11 Deauthentication** - Sends deauth frames to disconnect clients
- **DNS Hijacking** - Redirects all DNS queries to the captive portal
- **Social Engineering** - Uses convincing "firmware update" messaging

### Code Structure
- `setup()` - Initializes WiFi, DNS, and web server
- `loop()` - Handles requests and executes attacks
- `performScan()` - Discovers nearby networks
- `handleIndex()` - Main web interface logic
- `handleAdmin()` - Admin panel functionality

## 🔧 Configuration

### Default Settings
- **AP SSID**: `ESP Phish`
- **AP Password**: `0x0806`
- **IP Address**: `192.168.4.1`
- **DNS Port**: `53`
- **Web Port**: `80`

### Customization Options
- Modify SSID and password in `setup()`
- Change web interface styling in CSS sections
- Update phishing page content in `TITLE`, `SUBTITLE`, and `BODY` constants

## 📝 Legal Notice

This software is intended for:
- ✅ Educational purposes
- ✅ Authorized penetration testing
- ✅ Security research in controlled environments
- ✅ Network administrator training

**DO NOT USE FOR:**
- ❌ Unauthorized access to networks
- ❌ Stealing credentials
- ❌ Malicious activities
- ❌ Violating privacy or laws

## 🤝 Contributing

Contributions are welcome! Please ensure all contributions maintain the educational focus and include appropriate warnings about ethical usage.

## 📜 License

This project is released under the MIT License. See the LICENSE file for details.

## 👨‍💻 Author

**0x0806** - Security Researcher & Developer

---

*Remember: With great power comes great responsibility. Use this knowledge to protect, not to harm.*
