/*
Document Library

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

#pragma once

#include <Arduino.h>
#include <config.h>
#include <filter.h>

// basic
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <EEPROM.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Connect.hpp>
using WebServerClass = WebServer;

class Document : public Connect {
public:
  // Document(void) {}
  Document(WebServerClass *server) : Connect(server),
                                     _tsPolling(0),
                                     _expires(0),
                                     _interval(5),
                                     _retries(0) {
    deserializeJson(_loginFilter, loginFilter);
    deserializeJson(_refleshtokenFilter, refleshtokenFilter);
    deserializeJson(_presenceFilter, presenceFilter);
  }

  ~Document(void) {
  }

  void update(void) {
  }

  // user method
  void pollPresence(void);
  void handleRoot(void);
  void handleGetSettings(void);
  void handleClearSettings(void);
  void handleFileDelete(void);
  void handleFileList(void);
  bool handleFileRead(String path);

  void setClientIdValue(String clientid) {
    _paramClientIdValue = clientid;
  }

  void setTenantValue(String tenant) {
    _paramTenantValue = tenant;
  }

protected:
  // for WebUI
  // API request handler
  bool   requestJsonApi(JsonDocument &doc, ARDUINOJSON_NAMESPACE::Filter filter, String url, String payload = "", String type = "POST", bool sendAuth = false);
  String getContentType(String filename);
  bool   exists(String path);

  // preference
  bool loadContext(void);
  void saveContext(void);
  void removeContext(void);

  // for Azure AD login
  void pollForToken(void);
  bool refreshToken(void);
  void startDevicelogin(void);

  int getTokenLifetime(void);

private:
  unsigned long _tsPolling;
  unsigned int  _expires;

  String _paramClientIdValue;
  String _paramTenantValue;
  String _paramPollIntervalValue;

  String _access_token;
  String _refresh_token;
  String _id_token;

  String  _user_code;
  String  _device_code;
  uint8_t _interval;

  uint8_t _retries;

  StaticJsonDocument<200> _loginFilter;         //初回ログインに使用
  StaticJsonDocument<200> _tokenFilter;         //トークン取得に使用
  StaticJsonDocument<200> _refleshtokenFilter;  //トークン再取得に使用
  StaticJsonDocument<200> _presenceFilter;      //在籍情報取得時に使用
};
