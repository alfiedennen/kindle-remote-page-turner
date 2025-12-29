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
    // Check WiFi and reconnect if needed
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 10) {
            delay(200);
            retries++;
        }
        if (WiFi.status() != WL_CONNECTED) {
            return; // Give up silently
        }
    }

    // Try up to 3 times
    for (int attempt = 0; attempt < 3; attempt++) {
        HTTPClient http;
        String url = String("http://") + kindleIP + ":8888/" + direction;
        http.begin(url);
        http.setTimeout(3000);
        http.setConnectTimeout(2000);
        int code = http.GET();
        http.end();

        if (code == 200) {
            return; // Success
        }
        delay(100); // Brief pause before retry
    }
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
