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
#include <WebServer.h>
#include <ESP32MSGraph.h>
using WebServerClass = WebServer;
#include <Ticker.h>
#include <WS2812FX.h>

WebServerClass server;
ESP32MSGraph   msgraph(&server);

WS2812FX     ws2812fx(25, 27, NEO_GRB + NEO_KHZ800);
TaskHandle_t TaskNeopixel;
String       _availability;

Ticker _timer;
bool   flag = false;

// Please add User filter.
constexpr char presenceFilter[] = R"(
{
  "id"           : false,
  "availability" : true,
  "activity"     : true"
}
)";

StaticJsonDocument<200> _presenceFilter;

void _interval(void) {
  flag = true;
}

void setAnimation(uint8_t brightness, uint16_t speed, uint32_t color, uint8_t mode) {
  ws2812fx.setBrightness(brightness);
  ws2812fx.setSpeed(speed);
  ws2812fx.setColor(color);
  ws2812fx.setMode(mode);
}

void setPresenceAnimation(String activity) {
  if (activity.equals("Available")) {
    setAnimation(255, 1000, GREEN, FX_MODE_BREATH);
  }
  if (activity.equals("Away")) {
    setAnimation(255, 1000, YELLOW, FX_MODE_BREATH);
  }
  if (activity.equals("BeRightBack")) {
    setAnimation(255, 1000, ORANGE, FX_MODE_BREATH);
  }
  if (activity.equals("Busy")) {
    setAnimation(255, 1000, PURPLE, FX_MODE_BREATH);
  }
  if (activity.equals("DoNotDisturb") || activity.equals("UrgentInterruptionsOnly")) {
    setAnimation(255, 1000, PINK, FX_MODE_BREATH);
  }
  if (activity.equals("InACall")) {
    setAnimation(255, 1000, RED, FX_MODE_BREATH);
  }
  if (activity.equals("InAConferenceCall")) {
    setAnimation(255, 9000, RED, FX_MODE_BREATH);
  }
  if (activity.equals("Inactive")) {
    setAnimation(255, 1000, WHITE, FX_MODE_BREATH);
  }
  if (activity.equals("InAMeeting")) {
    setAnimation(255, 1000, RED, FX_MODE_SCAN);
  }
  if (activity.equals("Offline") || activity.equals("OffWork") || activity.equals("OutOfOffice") || activity.equals("PresenceUnknown")) {
    setAnimation(255, 1000, BLACK, FX_MODE_STATIC);
  }
  if (activity.equals("Presenting")) {
    setAnimation(255, 1000, RED, FX_MODE_COLOR_WIPE);
  }
}

bool pollPresence(void) {
  log_d("pollPresence()");

  const size_t        capacity = JSON_OBJECT_SIZE(4) + 500;
  DynamicJsonDocument responseDoc(capacity);

  bool res = msgraph.requestGraphAPI(responseDoc,
                                     DeserializationOption::Filter(_presenceFilter),
                                     "https://graph.microsoft.com/v1.0/me/presence",
                                     "",
                                     "GET",
                                     true);

  if (res) {
    // Store presence info
    //_availability = responseDoc["availability"].as<String>();
    String activity = responseDoc["activity"].as<String>();
    setPresenceAnimation(activity);
    log_i("success to get Presence: %s", activity.c_str());
  } else if (responseDoc.containsKey("error")) {
    const char* _error_code = responseDoc["error"]["code"];
    if (strcmp(_error_code, "InvalidAuthenticationToken")) {
      log_e("pollPresence() - Refresh needed");
    } else {
      log_e("pollPresence() - Error: %s\n", _error_code);
    }
  } else {
    log_e("unknown");
  }

  return res;
}

void neopixelTask(void* parameter) {
  for (;;) {
    ws2812fx.service();
    delay(1);
  }
}

void setup() {
  Serial.begin(115200);

  // ATOM Lite WiFi不具合対策
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  deserializeJson(_presenceFilter, presenceFilter);
  _timer.attach(30, _interval);

  ws2812fx.init();
  ws2812fx.setBrightness(128);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(WHITE);
  ws2812fx.setMode(FX_MODE_BLINK);
  ws2812fx.start();

  // Pin neopixel logic to core 0
  xTaskCreatePinnedToCore(neopixelTask, "Neopixels", 1024, NULL, 1, &TaskNeopixel, 1);

  msgraph.begin();
}

void loop() {
  if (flag && msgraph.getAuthReady()) {
    flag = false;
    pollPresence();
  }

  msgraph.update();
}
