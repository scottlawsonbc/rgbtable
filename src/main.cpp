#include <WS2812FX.h>
#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <fauxmoESP.h>

#define LED_PIN    3
#define LED_COUNT 96

const char* ssid     = "Shady";
const char* password = "6043254234";
const char* name = "table";

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(LED_COUNT);

boolean state = false;
fauxmoESP fauxmo;

void display(void) {
    if(strip.CanShow()) {
        memcpy(strip.Pixels(), ws2812fx.getPixels(), strip.PixelsSize());
        strip.Dirty();
        strip.Show();
    }
}

void initSmartDevice() {
    fauxmo.addDevice(name);
    fauxmo.setPort(80);
    fauxmo.enable(true);
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool newState, unsigned char value) {
        if (String(device_name).equals(String(name))) {
            state = newState;
        }
    });
}

void initWiFi() {
    WiFi.begin(ssid, password);
    ws2812fx.setMode(FX_MODE_COMET);
    while (WiFi.status() != WL_CONNECTED) {
        ws2812fx.service();
    }
}

void initStrip() {
    ws2812fx.init();
    strip.Begin();
    strip.Show();
    ws2812fx.setCustomShow(display);
    ws2812fx.setBrightness(255);
    ws2812fx.setSpeed(8000);
    ws2812fx.start();
}

void setup() {
    Serial.begin(115200);
    initStrip();
    initWiFi();
    ws2812fx.setMode(FX_MODE_COLOR_WIPE_RANDOM);
    initSmartDevice();
}

void loop() {
    ws2812fx.service();
    fauxmo.handle();
    if (state) {
        ws2812fx.setBrightness(255);
    } else {
        ws2812fx.setBrightness(0);
    }
}
