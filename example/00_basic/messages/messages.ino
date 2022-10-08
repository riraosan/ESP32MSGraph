/*
ESP32MSGraph Library Mail Sample

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
#include <Button2.h>
//#include <M5StickCPlus.h>

Ticker _timer;
bool   flag = false;

void _interval(void) {
  flag = true;
}

WebServerClass server;
ESP32MSGraph   msgraph(&server);

Button2 button;
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
// String emailAPI(R"(https://graph.microsoft.com/v1.0/me/messages?$count=true)");
String emailAPI(R"(https://graph.microsoft.com/v1.0/me/messages?$filter=((from/emailAddress/address)%20eq%20'{from mail Address}')&$count=true)");

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

void handler(Button2& btn) {
  switch (btn.getType()) {
    case clickType::single_click:
      Serial.print("single ");
      break;
    case clickType::double_click:
      Serial.print("double ");
      break;
    case clickType::triple_click:
      Serial.print("triple ");
      break;
    case clickType::long_click:
      Serial.print("long ");
      msgraph.deleteRefreshToken();
      ESP.restart();
      break;
    case clickType::empty:
      break;
    default:
      break;
  }

  Serial.print("click");
  Serial.print(" (");
  Serial.print(btn.getNumberOfClicks());
  Serial.println(")");
}

void initM5(void) {
  M5.begin();             // Initialize M5StickC Plus.  初始化 M5StickC PLus
  M5.Lcd.setTextSize(3);  // Set font size.  设置字体大小
  M5.Lcd.setRotation(3);  // Rotate the screen. 将屏幕旋转
  // LCD display.  Lcd显示
  M5.Lcd.print("Hello World");
}

void setup() {
  // initM5();

  Serial.begin(115200);
  // ATOM Lite WiFi不具合対策
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  // button
  // button.setClickHandler(handler);
  // button.setDoubleClickHandler(handler);
  // button.setTripleClickHandler(handler);
  button.setLongClickHandler(handler);
  button.begin(37);  // G37

  deserializeJson(_mailFilter, mailFilter);
  _timer.attach(30, _interval);

  emailAPI.replace("{from mail Address}", "your@email.com");

  msgraph.setScope("offline_access%20openid%20email%20profile%20Presence.Read%20Mail.Read%20Mail.ReadBasic%20Mail.ReadWrite");
  msgraph.begin();
}

void loop() {
  if (flag && msgraph.getAuthReady()) {
    flag = false;
    pollMail(emailAPI);
  }
  msgraph.update();
  button.loop();
}
