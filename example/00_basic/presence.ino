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

Ticker _timer;
bool   flag = false;

void _interval(void) {
  flag = true;
}

WebServerClass server;
ESP32MSGraph   msgraph(&server);

StaticJsonDocument<200> _presenceFilter;

String _availability;

bool pollPresence(void) {
  log_d("pollPresence()");
  // See: https://github.com/microsoftgraph/microsoft-graph-docs/blob/ananya/api-reference/beta/resources/presence.md
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
    _availability = responseDoc["availability"].as<String>();
    // String activity     = responseDoc["activity"].as<String>();
    log_i("success to get Presence: %s", _availability.c_str());
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

void setup() {
  Serial.begin(115200);
  // ATOM Lite WiFi不具合対策
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  deserializeJson(_presenceFilter, presenceFilter);
  _timer.attach(30, _interval);

  msgraph.begin();
}

void loop() {
  if (flag && msgraph.getAuthReady()) {
    flag = false;
    pollPresence();
  }
  msgraph.update();
}
