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
using WebServerClass = WebServer;

class Document {
public:
  // Document(void) {}
  Document(WebServerClass *server) : _server(server),
                                     _tsPolling(0),
                                     _expires(0),
                                     _interval(5),
                                     _retries(0),
                                     _authReady(false) {
    deserializeJson(_loginFilter, loginFilter);
    deserializeJson(_refleshtokenFilter, refleshtokenFilter);
  }

  ~Document(void) {
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

  // for WebUI
  // API request handler
  bool   requestGraphAPI(JsonDocument &doc, ARDUINOJSON_NAMESPACE::Filter filter, String url, String payload = "", String type = "POST", bool sendAuth = false);
  String getContentType(String filename);
  bool   exists(String path);

  // preference
  bool loadContext(void);
  void saveContext(void);

  // for Azure AD login
  bool pollForToken(void);
  bool refreshToken(void);
  bool startDevicelogin(void);
  int  getTokenLifetime(void);

  String getDeviceCode(void);
  String getUserCode(void);

  bool getAuthReady(void);

private:
  WebServerClass *_server;

  unsigned long _tsPolling;

  // from user
  String _paramClientIdValue;
  String _paramTenantValue;
  String _paramPollIntervalValue;

  // from Graph API
  String   _access_token;
  String   _refresh_token;
  String   _id_token;
  String   _user_code;
  String   _device_code;
  String   _verification_uri;
  uint32_t _expires;
  uint8_t  _interval;
  String   _message;

  // retry count
  uint8_t _retries;

  bool _authReady;

  StaticJsonDocument<200> _loginFilter;
  StaticJsonDocument<200> _refleshtokenFilter;
};
