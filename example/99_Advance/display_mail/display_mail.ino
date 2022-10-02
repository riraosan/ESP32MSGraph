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
#include <M5GFX.h>
#include <ESP32_8BIT_CVBS.h>

Ticker _timer;
bool   flag = false;

void _interval(void) {
  flag = true;
}

WebServerClass server;
ESP32MSGraph   msgraph(&server);

ESP32_8BIT_CVBS display;

String count;
String subject;
String receivedDateTime("2022-10-02T00:00:00Z");

String pre;

Button2 button;
// メールの件数を取得します。
constexpr char mailFilter[] = R"(
{
  "@odata.context"  : false,
  "@odata.count"    : true,
  "@odata.nextLink" : false,
  "value"           : true
}
)";

StaticJsonDocument<200> _mailFilter;

// from Addressから受信したメールを取得する
// 重要： URL中のスペースは「%20」か「+」で埋めること
String emailAPI(R"(https://graph.microsoft.com/v1.0/me/messages?$filter=startswith(subject,'Graph')&$select=subject,receivedDateTime&$top=1&$orderby=subject+desc)");

bool pollMail(String api) {
  // See:
  const size_t        capacity = JSON_OBJECT_SIZE(4) + 512;
  DynamicJsonDocument responseDoc(capacity);

  bool res = msgraph.requestGraphAPI(responseDoc,
                                     DeserializationOption::Filter(_mailFilter),
                                     api,
                                     "",
                                     "GET",
                                     true);

  if (res) {
    // count            = responseDoc["@odata.count"].as<String>();
    receivedDateTime = responseDoc["value"][0]["receivedDateTime"].as<String>();
    subject          = responseDoc["value"][0]["subject"].as<String>();

    log_i("success to get mail counts: %s", count.c_str());
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

void setupDisplay(void) {
  display.setColorDepth(8);
  display.setRotation(0);
  display.begin();
  display.startWrite();

  display.setFont(&fonts::lgfxJapanMincho_16);
  display.setTextSize(1);
  display.setCursor(10, 10);
  display.setTextColor(TFT_WHITE);
}

void setup() {
  Serial.begin(115200);
  // ATOM Lite WiFi不具合対策
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  setupDisplay();

  // button
  button.setClickHandler(handler);
  button.setDoubleClickHandler(handler);
  button.setTripleClickHandler(handler);
  button.setLongClickHandler(handler);
  button.begin(37);  // G37

  deserializeJson(_mailFilter, mailFilter);
  _timer.attach(60, _interval);

  display.setCursor(10, 10);
  display.println("Connecting...");

  msgraph.begin();

  display.fillScreen(TFT_BLACK);
  display.setCursor(10, 10);
  display.println("Microsoft Graph API TEST.");
}

bool checkDeploy(void) {
  bool result;

  if (receivedDateTime != pre) {
    result = true;
  } else {
    result = false;
  }

  pre = receivedDateTime;
  return result;
}

void displayText(void) {
  display.setCursor(10, 10);
  display.println("Microsoft Graph API TEST.");
  display.println(" " + count);
  display.println(" " + receivedDateTime);
  display.println(" " + subject);
  display.display();
}

void loop() {
  if (flag && msgraph.getAuthReady()) {
    flag = false;
    pollMail(emailAPI);

    if (checkDeploy()) {
      display.fillScreen(TFT_GREEN);
      display.setTextColor(TFT_BLACK);
      displayText();
    } else {
      display.fillScreen(TFT_BLACK);
      display.setTextColor(TFT_WHITE);
      displayText();
    }
  }

  msgraph.update();
  button.loop();
}
