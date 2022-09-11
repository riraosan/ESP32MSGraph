
#pragma once

#if defined(ARDUINO_ARCH_ESP32)
#include <functional>
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AutoConnect.h>
using WiFiWebServer = WebServer;
#else
#error Only for ESP32
#endif

// TODO コードを受け取るHTMLをはりつけること

class Connect {
public:
  Connect() : _portal(_server),
              _loginconfig("/loginSettings", "Device Login Settings"),
              _devicelogin("/lauchdevicelogin", "Device Login", false),
              _header("Device Login Settings"),
                  _isDetect(false),
              _isConnect(false),
              _hostName(F("graph")),
              _apName(F("ATOM-G-AP")),
              _httpPort(80) {
    _content = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
設定 __AC_LINK__
</body>
</html>)";
  }

  void begin(void) {
    begin("", "");
  }

  bool getDetect(void) {
    return _isDetect;
  }

  void setDetect(bool isDetect) {
    _isDetect = isDetect;
  }

  bool getConnect(void) {
    return _isConnect;
  }

  void setConnect(bool isConnect) {
    _isConnect = isConnect;
  }

  void createAuxPage(void) {
    // login settings
    // ACText(header, "Device Login Settings");
    // ACText(caption, "次の項目を入力して[Get Device Code]ボタンを押してください");
    // ACInput(userID, "Client ID", "", "Client or Generic ID", "e.g. 3837bbf0-30fb-47ad-bce8-f460ba9880c3");
    // ACInput(tenantID, "Tenant Hostname or ID", "", "Tenant Hostname or ID", "e.g. contoso.onmicrosoft.com");
    // ACSubmit(devicecode, "Get Device Code", "/getdevicecode");

    _loginconfig.add({_header});

    // show device code
    // ACText(header2, "Device Login");
    // ACText(caption2, "Device Codeをコピーして[Start device login]ボタンを押してください");
    // ACInput(devicecode2, "Device Code", "");  //ここに読み取り専用で表示
    // ACSubmit(startlogin, "Start device login", "https://microsoft.com/devicelogin");

    _devicelogin.add({_header});
    _portal.join({_loginconfig, _devicelogin});
  }

  void begin(const char* SSID, const char* PASSWORD) {
    createAuxPage();

    // Responder of root page and apply page handled directly from WebServer class.
    _server.on("/", [&]() {
      _content.replace("__AC_LINK__", String(AUTOCONNECT_LINK(COG_16)));
      _server.send(200, "text/html", _content);
    });

    // memo これがないと落ちる。
    _portal.on("/loginSettings", [&](AutoConnectAux& aux, PageArgument& args) -> String {
      return String();
    });

    // memo これがないと落ちる。
    _portal.on("/getdevicecode", [&](AutoConnectAux& aux, PageArgument& args) -> String {
      return String();
    });

    _portal.on("/lauchdevicelogin", [&](AutoConnectAux& aux, PageArgument& args) -> String {
      return String();
    });

    // //ユーザーコードを取得する
    // _server.on("/startlogin", [&]() {
    //   _usercodeHtml.replace("__CLIENTID__", _clientIdValue);
    //   _usercodeHtml.replace("__TENANTVALUE__", _tenantValue);
    //   _usercodeHtml.replace("__POLLVALUE__", _pollIntervalValue);

    //   _usercodeHtml.replace("__VERSION__", VERSION);
    //   _usercodeHtml.replace("__SKETCHSIZE__", String(ESP.getSketchSize()));
    //   _usercodeHtml.replace("__FREESKETCHSPACE__", String(ESP.getFreeSketchSpace()));
    //   _usercodeHtml.replace("__FREEHEAP__", String(ESP.getFreeHeap()));
    //   _usercodeHtml.replace("__USEDHEAP__", String(327680 - ESP.getFreeHeap()));

    //   _server.send(200, "text/html", _usercodeHtml);
    // });

    _config.autoReconnect     = true;
    _config.reconnectInterval = 1;

    _config.ota  = AC_OTA_BUILTIN;
    _config.apid = _apName;
    _portal.config(_config);

    _portal.onDetect([&](IPAddress& ip) -> bool {
      log_d("onDetect");
      setDetect(true);
      return true;
    });

    _portal.onConnect([&](IPAddress& ip) {
      log_d("onConnect");
      setConnect(true);
    });

    _portal.onNotFound([&]() {
      log_d("onNotFound");
    });

    bool result = false;

    if (String(SSID).isEmpty() || String(PASSWORD).isEmpty()) {
      result = _portal.begin();
    } else {
      result = _portal.begin(SSID, PASSWORD);
    }

    if (result) {
      log_i("WiFi connected: %s", WiFi.localIP().toString().c_str());

      if (MDNS.begin(_hostName.c_str())) {
        MDNS.addService("http", "tcp", _httpPort);
        log_i("HTTP Server ready! Open http://%s.local/ in your browser\n", _hostName.c_str());
      } else
        log_e("Error setting up MDNS responder");
    } else {
      log_e("ESP32 can't connect to AP.");
      ESP.restart();
    }
  }

  void update(void) {
    _portal.handleClient();
  }

private:
  WiFiWebServer     _server;
  AutoConnectConfig _config;
  AutoConnect       _portal;

  AutoConnectAux _loginconfig;
  AutoConnectAux _devicelogin;
  AutoConnectText _header;


  bool _isDetect;
  bool _isConnect;

  String   _content;
  String   _hostName;
  String   _apName;
  uint16_t _httpPort;

  String _clientIdValue;
  String _tenantValue;
  String _pollIntervalValue;
};
