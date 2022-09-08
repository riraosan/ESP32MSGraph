#pragma once

#include <Arduino.h>

class ESP32MSGraph {
public:
  ESP32MSGraph(void) {
  }

  ~ESP32MSGraph() {
  }

  void begin(void) {
    _led.reset(new ViewLED);
    //_html.reset(new ViewHTML);
    _doc.reset(new Document);

    _doc.attachSubscriber(_led);
    //_jsondoc.attachSubscriber(_html);

    //コンテキストを作成する
    _context.reset(new Context);

    _context.attachState(new StateInitialize(_doc));  // State start
    //_context.attachState(new StateWiFiConnecting(_doc));  // Wait for wifi connection
    //_context.attachState(new StateWiFiConnection(_doc)); // Wifi connected SMODEWIFICONNECTED
    //_context.attachState(new StateWiFiConnection(_doc)); // Device login flow was started SMODEDEVICELOGINSTARTED
    //_context.attachState(new StateWiFiConnection(_doc)); // Device login flow failed SMODEDEVICELOGINFAILED
    //_context.attachState(new StateWiFiConnection(_doc)); // Authentication successful SMODEAUTHREADY
    //_context.attachState(new StateWiFiConnection(_doc)); // Access token needs refresh SMODEREFRESHTOKEN
  }

private:
  std::unique_ptr<Context> _context;

  std::unique_ptr<ViewLED>   _led;
  std::unique_ptr<ViewHTML>  _html;
  std::unique_ptr<Doccument> _doc;
};
