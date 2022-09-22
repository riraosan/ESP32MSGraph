
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

//遷移先
#include "SDeviceLoginStarted.h"

class SDeviceLoginFailed : public State {
public:
  SDeviceLoginFailed(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
    exitAction();
  }

  void exitAction(void) override {
    log_d("exitAction");
    this->_context->TransitionTo(new SDeviceLoginStarted(std::move(_doc)));
  }

  void update(void) override {
    log_d("update");
    entryAction();
    doActivity();
  }

private:
  std::shared_ptr<Document> _doc;
};
