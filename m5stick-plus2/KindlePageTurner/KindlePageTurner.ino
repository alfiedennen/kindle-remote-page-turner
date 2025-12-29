/*
 * Kindle Page Turner Remote
 * For M5StickC PLUS2
 *
 * Controls:
 *   - Single tap (front button): Next page
 *   - Double tap (front button): Previous page
 *
 * Setup:
 *   1. Update WiFi credentials below
 *   2. Update Kindle IP address
 *   3. Upload to M5StickC PLUS2
 */

#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ============================================
// CONFIGURATION - Update these values!
// ============================================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* kindleIP = "192.168.1.100";  // Your Kindle's IP address
// ============================================

// Double-tap detection
unsigned long lastTapTime = 0;
const unsigned long doubleTapWindow = 300; // ms window for double-tap
bool waitingForSecondTap = false;

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.println("Connecting...");

    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        attempts++;
    }

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 30);
    if (WiFi.status() == WL_CONNECTED) {
        M5.Lcd.println("Ready!");
        M5.Lcd.setCursor(10, 60);
        M5.Lcd.setTextSize(1);
        M5.Lcd.println(WiFi.localIP().toString());
    } else {
        M5.Lcd.println("No WiFi!");
    }
}

void sendPage(const char* direction) {
    if (WiFi.status() != WL_CONNECTED) {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(10, 30);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("Reconnecting...");
        WiFi.reconnect();
        delay(2000);
        if (WiFi.status() != WL_CONNECTED) {
            M5.Lcd.fillScreen(RED);
            M5.Lcd.setCursor(10, 30);
            M5.Lcd.println("No WiFi!");
            delay(1000);
            return;
        }
    }

    HTTPClient http;
    String url = String("http://") + kindleIP + ":8888/" + direction;
    http.begin(url);
    http.setTimeout(2000);
    int code = http.GET();
    http.end();

    // Silent operation - no screen flash
}

void loop() {
    M5.update();
    unsigned long now = millis();

    // Front button (A) - single tap = next, double tap = prev
    if (M5.BtnA.wasPressed()) {
        if (waitingForSecondTap && (now - lastTapTime < doubleTapWindow)) {
            // Double tap detected - go back
            waitingForSecondTap = false;
            sendPage("prev");
        } else {
            // First tap - wait to see if there's a second
            waitingForSecondTap = true;
            lastTapTime = now;
        }
    }

    // If waiting for second tap and window expired, send next
    if (waitingForSecondTap && (now - lastTapTime >= doubleTapWindow)) {
        waitingForSecondTap = false;
        sendPage("next");
    }

    delay(10);
}
