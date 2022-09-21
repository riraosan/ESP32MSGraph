
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

//遷移先
#include "SAuthReady.h"

class SDeviceLoginStarted : public State {
public:
  SDeviceLoginStarted(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
  }

  void exitAction(void) override {
    log_d("exitAction");
    delay(1000);
    this->_context->TransitionTo(new SDeviceLoginStarted(std::move(_doc)));
  }

private:
  std::shared_ptr<Document> _doc;
};
