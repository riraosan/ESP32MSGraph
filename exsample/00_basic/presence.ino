/*
ESP32MSGraph Library Presence Sample

Original Source:
https://github.com/toblum/ESPTeamsPresence

Licence:
[MIT](https://github.com/riraosan/ESP32MSGraph/blob/master/LICENSE)

Author:
refactored by
[riraosan_0901](https://twitter.com/riraosan_0901)
[riraosan](https://github.com/riraosan)

Contributors:
[toblum](https://github.com/toblum)
*/

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <EEPROM.h>
#include <FS.h>
#include <SPIFFS.h>
#include <esp32-hal-log.h>

#include <IotWebConf.h>
#include <ArduinoJson.h>
#include <WS2812FX.h>

#include <ESP32_RMT_Driver.h>
#include <ESP32MSGraph.h>
#include <config.h>
#include <filter.h>

constexpr char thingName[]             = "ESP32MSGraph";  // SSID
constexpr char wifiInitialApPassword[] = "12345678";      // password

#define STRING_LEN  64
#define INTEGER_LEN 16
char _paramClientIdValue[STRING_LEN];
char _paramTenantValue[STRING_LEN];
char _paramPollIntervalValue[INTEGER_LEN];
char _paramNumLedsValue[INTEGER_LEN];

IotWebConfSeparator separator;
IotWebConfParameter paramClientId;
IotWebConfParameter paramTenant;
IotWebConfParameter paramPollInterval;
IotWebConfParameter paramNumLeds;

DNSServer        dnsServer;
WebServer        server(80);
IotWebConf       iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);
ESP32MSGraph     graph(&iotWebConf, &server);
ESP32_RMT_Driver blinker;
HTTPUpdateServer httpUpdater;

// WS2812FX
WS2812FX ws2812fx(NUMLEDS, DATAPIN, NEO_GRB + NEO_KHZ800);
int      numberLeds;

// LED Task
TaskHandle_t TaskNeopixel;

/**
 * Multicore
 */
void neopixelTask(void* parameter) {
  for (;;) {
    ws2812fx.service();
    delay(10);
  }
}

void onWifiConnected() {
  graph.changeState(SMODEWIFICONNECTED);
}

// Config was saved
void onConfigSaved() {
  log_i("Configuration was updated.");
  ws2812fx.setLength(atoi(_paramNumLedsValue));

  graph.setClientIdValue(String(_paramClientIdValue));
  graph.setTenantValue(String(_paramTenantValue));
  graph.setPollIntervalValue(String(_paramPollIntervalValue));
  graph.setNumLedsValue(String(atoi(_paramNumLedsValue)));
}

void initAPI(void) {
  // HTTP server - Set up required URL handlers on the web server.
  server.on("/", HTTP_GET, [] { graph.handleRoot(); });
  server.on("/config", HTTP_GET, [] { iotWebConf.handleConfig(); });
  server.on("/config", HTTP_POST, [] { iotWebConf.handleConfig(); });
  server.on("/upload", HTTP_GET, [] { graph.handleMinimalUpload(); });
  server.on("/api/startDevicelogin", HTTP_GET, [] { graph.startDevicelogin(); });
  server.on("/api/settings", HTTP_GET, [] { graph.handleGetSettings(); });
  server.on("/api/clearSettings", HTTP_GET, [] { graph.handleClearSettings(); });

  server.onNotFound([]() {
    iotWebConf.handleNotFound();
    // if (!handleFileRead(server.uri())) {
    //   server.send(404, "text/plain", "FileNotFound");
    // }
  });
}

// WebUI - Initializing the configuration.
void initWebUI(void) {
  // Add parameter
  separator         = IotWebConfSeparator();
  paramClientId     = IotWebConfParameter("Client-ID (Generic ID: 3837bbf0-30fb-47ad-bce8-f460ba9880c3)", "clientId", _paramClientIdValue, STRING_LEN, "text", "e.g. 3837bbf0-30fb-47ad-bce8-f460ba9880c3", "3837bbf0-30fb-47ad-bce8-f460ba9880c3");
  paramTenant       = IotWebConfParameter("Tenant hostname / ID", "tenantId", _paramTenantValue, STRING_LEN, "text", "e.g. contoso.onmicrosoft.com");
  paramPollInterval = IotWebConfParameter("Presence polling interval (sec) (default: 30)", "pollInterval", _paramPollIntervalValue, INTEGER_LEN, "number", "10..300", DEFAULT_POLLING_PRESENCE_INTERVAL, "min='10' max='300' step='5'");
  paramNumLeds      = IotWebConfParameter("Number of LEDs (default: 16)", "numLeds", _paramNumLedsValue, INTEGER_LEN, "number", "1..500", "16", "min='1' max='500' step='1'");

#ifdef DISABLECERTCHECK
  log_d("WARNING: Checking of HTTPS certificates disabled.");
#endif
#ifdef LED_BUILTIN
  iotWebConf.setStatusPin(LED_BUILTIN);
#endif
  iotWebConf.setWifiConnectionTimeoutMs(5000);
  iotWebConf.addParameter(&separator);
  iotWebConf.addParameter(&paramClientId);
  iotWebConf.addParameter(&paramTenant);
  iotWebConf.addParameter(&paramPollInterval);
  iotWebConf.addParameter(&paramNumLeds);
  iotWebConf.setWifiConnectionCallback(&onWifiConnected);
  iotWebConf.setConfigSavedCallback(&onConfigSaved);
  iotWebConf.setupUpdateServer(&httpUpdater);
  iotWebConf.skipApStartup();
  iotWebConf.init();

  onConfigSaved();

  initAPI();
}

void customShow(void) {
  uint8_t* pixels = ws2812fx.getPixels();
  // numBytes is one more then the size of the ws2812fx's *pixels array.
  // the extra byte is used by the driver to insert the LED reset pulse at the end.
  uint16_t numBytes = ws2812fx.getNumBytes() + 1;
  rmt_write_sample(RMT_CHANNEL_0, pixels, numBytes, false);  // channel 0
}

void initLED(void) {
  ws2812fx.init();
  blinker.begin(RMT_CHANNEL_0, ws2812fx.getPin());
  ws2812fx.start();

  numberLeds = atoi(_paramNumLedsValue);
  if (numberLeds < 1) {
    log_d("Number of LEDs not given, using 16.");
    numberLeds = NUMLEDS;
  }
  ws2812fx.setLength(numberLeds);
  ws2812fx.setCustomShow(customShow);

  // Pin neopixel logic to core 0
  xTaskCreatePinnedToCore(neopixelTask, "Neopixels", 1000, NULL, 1, &TaskNeopixel, 0);
}

void setup() {
  // WiFi不具合対策
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  Serial.begin(115200);
  log_d("setup() Starting up...");

  if (!SPIFFS.begin(true)) {
    log_d("SPIFFS Mount Failed");
    return;
  }

  initWebUI();
  initLED();

  log_d("setup() ready...");
}

void loop() {
  // View
  iotWebConf.doLoop();
  // Document
  graph.update();
}
