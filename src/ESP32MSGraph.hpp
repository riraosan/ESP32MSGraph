#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <Document.h>
#include "./state/SInitialize.h"

using WebServerClass = WebServer;

class ESP32MSGraph {
public:
  ESP32MSGraph(WebServerClass *server) : _server(server) {
    _doc.reset(new Document(_server));
    _context.reset(new Context);
  }

  ~ESP32MSGraph() {
  }

  void begin(void) {
    //_led.reset(new ViewLED);
    //_html.reset(new ViewHTML);

    //_doc.attachSubscriber(_led);
    //_jsondoc.attachSubscriber(_html);

    log_d("change starting");
    _context->TransitionTo(new SInitialize(std::move(_doc)));  // State start
  }

  void update(void) {
#if 1
    _context->printStateName();
#endif
    _context->update();
    delay(1);
  }

private:
  WebServerClass           *_server;
  std::unique_ptr<Context>  _context;
  std::shared_ptr<Document> _doc;
  // std::unique_ptr<ViewLED>   _led;
  // std::unique_ptr<ViewHTML>  _html;
};
