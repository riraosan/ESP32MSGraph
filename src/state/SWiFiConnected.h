
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

//遷移先
#include "SDeviceLoginStarted.h"

class SWiFiConnected : public State {
public:
  SWiFiConnected(std::shared_ptr<Document> doc) : _doc(std::move(_doc)) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
  }

  void exitAction(void) override {
    log_d("exitAction");
    this->_context->TransitionTo(new SDeviceLoginStarted(std::move(_doc)));
  }

private:
  std::shared_ptr<Document> _doc;
};
