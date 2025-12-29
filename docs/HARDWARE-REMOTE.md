# Hardware Remote Setup (M5StickC PLUS2)

This guide covers setting up the M5StickC PLUS2 as a wireless page turner remote.

## Hardware

### M5StickC PLUS2

- **Price**: ~$20 / £24
- **Where to buy**:
  - UK: [The Pi Hut](https://thepihut.com/products/m5stickc-plus2-esp32-mini-iot-development-kit)
  - Global: [M5Stack Store](https://shop.m5stack.com/products/m5stickc-plus2-esp32-mini-iot-development-kit)

### Features
- ESP32 microcontroller with WiFi
- Built-in 200mAh battery
- 1.14" LCD display
- 2 buttons (front + side)
- USB-C for charging and programming

## Software Setup

### Option 1: Arduino IDE (Recommended for Beginners)

1. **Install Arduino IDE**: https://www.arduino.cc/en/software

2. **Add M5Stack Board Support**:
   - Go to: File > Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://static-cdn.m5stack.com/resource/arduino/package_m5stack_index.json
     ```
   - Go to: Tools > Board > Boards Manager
   - Search for "M5Stack" and install

3. **Install M5StickCPlus2 Library**:
   - Go to: Tools > Manage Libraries
   - Search for "M5StickCPlus2" and install

4. **Select Board**:
   - Tools > Board > M5Stack > M5StickC PLUS2

5. **Select Port**:
   - Connect M5StickC PLUS2 via USB-C
   - Tools > Port > Select the COM port that appears

### Option 2: Arduino CLI (Command Line)

```bash
# Download arduino-cli from https://arduino.github.io/arduino-cli/

# Configure M5Stack board support
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://static-cdn.m5stack.com/resource/arduino/package_m5stack_index.json
arduino-cli core update-index
arduino-cli core install m5stack:esp32

# Install library
arduino-cli lib install M5StickCPlus2
```

## Configure the Firmware

1. Open `m5stick-plus2/KindlePageTurner/KindlePageTurner.ino`

2. Update these lines with your settings:

```cpp
const char* ssid = "YOUR_WIFI_SSID";       // Your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD"; // Your WiFi password
const char* kindleIP = "192.168.1.100";    // Your Kindle's IP address
```

3. Find your Kindle's IP:
   - Open KOReader on Kindle
   - Go to Tools > SSH server
   - The IP address is displayed

## Upload Firmware

### Arduino IDE
1. Click the Upload button (right arrow)
2. Wait for "Done uploading"

### Arduino CLI
```bash
# Find your port (e.g., COM3 on Windows, /dev/ttyUSB0 on Linux)
arduino-cli board list

# Compile and upload
arduino-cli compile --fqbn m5stack:esp32:m5stack_stickc_plus2 KindlePageTurner
arduino-cli upload -p COM3 --fqbn m5stack:esp32:m5stack_stickc_plus2 KindlePageTurner
```

## Usage

### Power On
Press the power button on the side briefly to turn on.

### Display States
- **"Connecting..."** - Connecting to WiFi
- **"Ready!"** - Connected and ready to use
- **"No WiFi!"** - Failed to connect (check credentials)
- **Green flash** - Page turn successful
- **Red flash** - Page turn failed (check Kindle server)

### Controls
- **Single tap** (front button) - Next page
- **Double tap** (front button, within 300ms) - Previous page

### Power Off
Hold the power button for 6 seconds.

### Charging
Connect USB-C cable. Red LED indicates charging, green when full.

## Troubleshooting

### "No WiFi!" on Display
- Check WiFi credentials in the code
- Ensure your WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Check WiFi signal strength

### Green Flash but No Page Turn
- Ensure Kindle server is running
- Check Kindle IP address is correct
- Test manually: `curl http://<KINDLE_IP>:8888/next`

### Red Flash
- Kindle server is not responding
- Check firewall: `iptables -I INPUT 1 -p tcp --dport 8888 -j ACCEPT`
- SSH into Kindle and restart server

### Device Not Recognized on PC
- Try a different USB-C cable (some are charge-only)
- Install CH340 or CP210x drivers if needed
- Try a different USB port

## Battery Life

With the 200mAh battery, expect:
- ~2-3 hours of continuous use
- Several days on standby
- Recharges in ~1 hour

## Customization

### Change Double-Tap Speed
Edit in the firmware:
```cpp
const unsigned long doubleTapWindow = 300; // milliseconds
```

### Use Both Buttons Instead
Modify the `loop()` function:
```cpp
if (M5.BtnA.wasPressed()) {
    sendPage("next");
}
if (M5.BtnB.wasPressed()) {
    sendPage("prev");
}
```
