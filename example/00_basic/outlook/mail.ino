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

// メールの件数を取得します。
constexpr char mailFilter[] = R"(
{
  "@odata.context"  : false,
  "@odata.count"    : true,
  "@odata.nextLink" : false,
  "value"           : false
}
)";

StaticJsonDocument<200> _mailFilter;

// from Addressから受信したメールを取得する
String emailAPI(R"(https://graph.microsoft.com/v1.0/me/messages?$filter=(from/emailAddress/address) eq '{from mail Address}'&$count=true)");

bool pollMail(String api) {
  // See:
  const size_t        capacity = JSON_OBJECT_SIZE(4) + 500;
  DynamicJsonDocument responseDoc(capacity);

  bool res = msgraph.requestGraphAPI(responseDoc,
                                     DeserializationOption::Filter(_mailFilter),
                                     api,
                                     "",
                                     "GET",
                                     true);

  if (res) {
    int count = responseDoc["@odata.count"].as<int>();
    log_i("success to get mail counts: %d", count);
  } else if (responseDoc.containsKey("error")) {
    const char* _error_code = responseDoc["error"]["code"];
    if (strcmp(_error_code, "InvalidAuthenticationToken")) {
      log_e("Refresh needed");
    } else {
      log_e("Error: %s", _error_code);
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

  deserializeJson(_mailFilter, mailFilter);
  _timer.attach(30, _interval);

  emailAPI.replace("{from mail Address}", "your@email.addr");

  msgraph.begin();
}

void loop() {
  if (flag && msgraph.getAuthReady()) {
    flag = false;
    pollMail(emailAPI);
  }
  msgraph.update();
}
