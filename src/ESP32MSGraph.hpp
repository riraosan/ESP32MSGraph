#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <Document.h>
#include <Controller.hpp>

using WebServerClass = WebServer;

class ESP32MSGraph {
public:
  ESP32MSGraph(WebServerClass *server) : _server(server) {
  }

  ~ESP32MSGraph() {
  }

  void begin(void) {
    //_led.reset(new ViewLED);
    //_html.reset(new ViewHTML);
    _doc.reset(new Document(_server));

    //_doc.attachSubscriber(_led);
    //_jsondoc.attachSubscriber(_html);

    //コンテキストを作成する
    _context.reset(new Context);

    _context->TransitionTo(new SInitialize(std::move(_doc)));  // State start
  }

  void update(void) {
    _context->update();
    delay(1);
  }

private:
  WebServerClass *_server;

  std::unique_ptr<Context> _context;

  // std::unique_ptr<ViewLED>   _led;
  // std::unique_ptr<ViewHTML>  _html;
  std::shared_ptr<Document> _doc;
};
