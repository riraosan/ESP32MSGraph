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
String receivedDateTime;
String bodyPreview;
String pre("2022-10-08T00:00:00Z");

Button2 button;
// Filter by number of emails, receivedDateTime , and subject.
constexpr char mailFilter[] = R"(
{
  "@odata.context"  : false,
  "@odata.count"    : true,
  "@odata.nextLink" : false,
  "value"           : [
    {
     "receivedDateTime" : true,
     "subject"          : true,
     "bodyPreview"      : true,
    }
  ]
}
)";

StaticJsonDocument<512> _mailFilter;  // the purpose of saving RAM.

// Display only 10 e-mails whose subject begins with "Graph" and which are after the specified time. The order of display is ascending.
// Important: Fill in the spaces in the URL with "%20" or "+".
String baseAPI(R"(https://graph.microsoft.com/v1.0/me/messages?$filter=startswith(subject,'Graph')+and+receivedDateTime+gt+{receivedDateTime}&$top=10&$count=true&$select=subject,receivedDateTime,bodyPreview)");
String emailAPI(baseAPI);

bool pollMail(String api) {
  const size_t        capacity(JSON_OBJECT_SIZE(4) + 2048);
  DynamicJsonDocument responseDoc(capacity);

  bool res = msgraph.requestGraphAPI(responseDoc,
                                     DeserializationOption::Filter(_mailFilter),
                                     api,
                                     "",
                                     "GET",
                                     true);

  if (res) {
    count = responseDoc["@odata.count"].as<String>();
    if (count.toInt() > 0) {
      int index        = count.toInt() - 1;
      receivedDateTime = responseDoc["value"][index]["receivedDateTime"].as<String>();
      subject          = responseDoc["value"][index]["subject"].as<String>();
      bodyPreview      = responseDoc["value"][index]["bodyPreview"].as<String>();
    }

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
  button.begin(39);  // ATOM:G39 StickCPlus:G37

  deserializeJson(_mailFilter, mailFilter);
  _timer.attach(60, _interval);

  display.setCursor(10, 10);
  display.println("Connecting...");

  msgraph.setScope("offline_access%20openid%20email%20profile%20Presence.Read%20Mail.Read%20Mail.ReadBasic%20Mail.ReadWrite");
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
  display.println(" " + bodyPreview);
  display.display();
}

void loop() {
  if (flag && msgraph.getAuthReady()) {
    flag     = false;
    emailAPI = baseAPI;
    emailAPI.replace("{receivedDateTime}", pre);
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
