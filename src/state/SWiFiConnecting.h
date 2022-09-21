
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

//遷移先
#include "SWiFiConnected.h"

class SWiFiConnecting : public State {
public:
  SWiFiConnecting(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
    _doc->beginWiFi();
  }

  void exitAction(void) override {
    log_d("exitAction");
    this->_context->TransitionTo(new SWiFiConnected(std::move(_doc)));
  }

private:
  std::shared_ptr<Document> _doc;
};
