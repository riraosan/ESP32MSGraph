/*
ESP32MSGraph Library

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
#include <config.h>
#include <filter.h>

// basic
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <EEPROM.h>
#include <FS.h>
#include <SPIFFS.h>

// from github
#include <IotWebConf.h>
#include <ArduinoJson.h>
#include <WS2812FX.h>

// State
#define SMODEINITIAL              0   // Initial
#define SMODEWIFICONNECTING       1   // Wait for wifi connection
#define SMODEWIFICONNECTED        2   // Wifi connected
#define SMODEDEVICELOGINSTARTED   10  // Device login flow was started
#define SMODEDEVICELOGINFAILED    11  // Device login flow failed
#define SMODEAUTHREADY            20  // Authentication successful
#define SMODEREFRESHTOKEN         22  // Access token needs refresh
// TODO
#define SMODEPOLLPRESENCE         21  // Poll for presence
#define SMODEPRESENCEREQUESTERROR 23  // Access token needs refresh

class ESP32MSGraph {
public:
  // ESP32MSGraph(void) {}
  ESP32MSGraph(IotWebConf *config, WebServer *server) : _server(server),
                                                        _iotWebConf(config),
                                                        _state(SMODEINITIAL),
                                                        _laststate(SMODEINITIAL),
                                                        _tsPolling(0),
                                                        _expires(0),
                                                        interval(5),
                                                        _retries(0),
                                                        _lastIotWebConfState(0) {
    deserializeJson(_loginFilter, loginFilter);
    deserializeJson(_refleshtokenFilter, refleshtokenFilter);
    deserializeJson(_presenceFilter, presenceFilter);
  }

  ~ESP32MSGraph(void) {}

  void update(void) {
    statemachine();
  }

  void changeState(uint8_t state) {
    _state = state;
  }

  // user method
  void pollPresence(void);

  // LED(VIEW)
  // TODO del setAnimation
  void setAnimation(uint8_t segment, uint8_t mode = 0, uint32_t color = 0, uint16_t speed = 3000, bool reverse = false);
  void setPresenceAnimation(void);

  void handleRoot(void);
  void handleMinimalUpload(void);
  void startDevicelogin(void);
  void handleGetSettings(void);
  void handleClearSettings(void);
  void handleFileDelete(void);
  void handleFileList(void);
  void handleFileUpload(void);
  bool handleFileRead(String path);

  void setClientIdValue(String clientid) {
    _paramClientIdValue = clientid;
  }

  void setTenantValue(String tenant) {
    _paramTenantValue = tenant;
  }

  void setPollIntervalValue(String interval) {
    _paramPollIntervalValue = interval;
  }

  // TODO
  void setNumLedsValue(String lednum) {
    _paramNumLedsValue = lednum;
  }

protected:
  // controller
  // TODO
  void statemachine(void);

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

  int getTokenLifetime(void);

private:
  WebServer  *_server;
  IotWebConf *_iotWebConf;

  uint8_t       _state;
  uint8_t       _laststate;
  unsigned long _tsPolling;
  unsigned int  _expires;

  String _paramClientIdValue;
  String _paramTenantValue;
  String _paramPollIntervalValue;
  // TODO
  String _paramNumLedsValue;

  String access_token;
  String refresh_token;
  String id_token;

  String  user_code;
  String  device_code;
  uint8_t interval;

  // TODO
  String  availability;
  String  activity;
  uint8_t _retries;
  byte    _lastIotWebConfState;

  StaticJsonDocument<200> _loginFilter;         //初回ログインに使用
  StaticJsonDocument<200> _tokenFilter;         //トークン取得に使用
  StaticJsonDocument<200> _refleshtokenFilter;  //トークン再取得に使用
  StaticJsonDocument<200> _presenceFilter;      //在籍情報取得時に使用
};
