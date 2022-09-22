
#pragma once

#if defined(ARDUINO_ARCH_ESP32)
#include <memory>
#include <functional>
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AutoConnect.h>
#include <StateBase.h>
#include "./state/SDeviceLoginStarted.h"
#include "./state/SInitialize.h"
using WebServerClass = WebServer;
#else
#error Only for ESP32
#endif

class ESP32MSGraph {
public:
  ESP32MSGraph(WebServerClass* server) : _server(server),
                                         _portal(*_server),
                                         /*setting page*/
                                         _loginconfig("/loginSettings", "Device Login Settings"),
                                         _header("header", "Device Login Settings"),
                                         _caption("caption", "次の項目を入力して[Get Device Code]ボタンを押してください", "", "", AC_Tag_P),
                                         _clientID("clientID", "", "Client ID", "Client or Generic ID", "e.g. 3837bbf0-30fb-47ad-bce8-f460ba9880c3"),
                                         _tenantID("tenantID", "", "Tenant Hostname or ID", "Tenant Hostname or ID", "e.g. contoso.onmicrosoft.com"),
                                         _getdevicecode("getdevicecode", "Get Device Code", "/getdevicecode"),
                                         /*start login page*/
                                         _devicelogin("/lauchdevicelogin", "Dvice Login", false),  // hidden
                                         _header2("header2", "Device Login"),
                                         _caption2("caption2", "Device Codeをクリップボードにコピーして[Start device login]ボタンを押してください", "", "", AC_Tag_P),
                                         _devicecode("devicecode", "", "Device Code"),
                                         _startlogin("startlogin", "Start device login", "https://microsoft.com/devicelogin"),
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

    _pleasewait = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
</head>
<script>

window.setTimeout(function(){
  window.location.href = '/lauchdevicelogin'
}, 3000);

</script>
<body>
...We could get Device Code.
We will go to next page...
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
    _loginconfig.add({_header, _caption, _clientID, _tenantID, _getdevicecode});

    // show device code
    _devicelogin.add({_header2, _caption2, _devicecode, _startlogin});

    _portal.join({_loginconfig, _devicelogin});
  }

  void begin(const char* SSID, const char* PASSWORD) {
    _doc.reset(new Document(_server));
    _context.reset(new Context);

    createAuxPage();

    // Responder of root page and apply page handled directly from WebServer class.
    _server->on("/", [&]() {
      _content.replace("__AC_LINK__", String(AUTOCONNECT_LINK(COG_16)));
      _server->send(200, "text/html", _content);
    });

    _server->on("/getdevicecode", [&]() {
      _loginconfig.fetchElement();

      _clientIdValue = _loginconfig["clientID"].value;
      _tenantValue   = _loginconfig["tenantID"].value;

      log_d("%s, %s", _clientIdValue.c_str(), _tenantValue.c_str());
      _doc->setTenantValue(_tenantValue);
      _doc->setClientIdValue(_clientIdValue);

      log_d("Starting state transitions");
      _context->TransitionTo(new SDeviceLoginStarted(std::move(_doc)));

      _server->send(200, "text/html", _pleasewait);  //次の画面へ遷移
    });

    // memo これがないと落ちる。
    _portal.on("/loginSettings", [&](AutoConnectAux& aux, PageArgument& args) -> String {
      aux["clientID"].as<AutoConnectInput>().value = "ex)3837bbf0-30fb-47ad-bce8-f460ba9880c3";
      aux["tenantID"].as<AutoConnectInput>().value = "ex)contoso.onmicrosoft.com";
      return String();
    });

    _portal.on("/lauchdevicelogin", [&](AutoConnectAux& aux, PageArgument& args) -> String {
      aux["devicecode"].as<AutoConnectInput>().value = _deviceCode;
      return String();
    });

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
      //_context->TransitionTo(new SInitialize(std::move(_doc)));
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
      } else {
        log_e("Error setting up MDNS responder");
      }
    } else {
      log_e("ESP32 can't connect to AP.");
      ESP.restart();
    }
  }

  void update(void) {
    _portal.handleClient();

    _context->update();

    delay(1);
  }

protected:
  std::unique_ptr<Context>  _context;
  std::shared_ptr<Document> _doc;

  WebServerClass*   _server;
  AutoConnectConfig _config;
  AutoConnect       _portal;

  Ticker _IntervalTimer;

private:
  AutoConnectAux    _loginconfig;
  AutoConnectText   _header;
  AutoConnectText   _caption;
  AutoConnectInput  _clientID;
  AutoConnectInput  _tenantID;
  AutoConnectSubmit _getdevicecode;

  AutoConnectAux    _devicelogin;
  AutoConnectText   _header2;
  AutoConnectText   _caption2;
  AutoConnectInput  _devicecode;
  AutoConnectSubmit _startlogin;

  bool _isDetect;
  bool _isConnect;

  String   _content;
  String   _hostName;
  String   _apName;
  uint16_t _httpPort;
  String   _pleasewait;

  String _clientIdValue;
  String _tenantValue;
  String _deviceCode;
};
