
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>
//遷移先
#include "SPollToken.h"

class SDeviceLoginStarted : public State {
public:
  SDeviceLoginStarted(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  // timer event
  static void IntervalTimer(void) {
    _timer = true;
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
    bool success = _doc->startDevicelogin();  // detecting device-code event
    if (success) {
      exitAction();
    } else {
      _ticker.once(30, IntervalTimer);
    }
  }

  void exitAction(void) override {
    log_d("exitAction");
    this->_context->TransitionTo(new SPollToken(std::move(_doc)));
  }

  void update(void) override {
    if (_timer) {
      entryAction();
      doActivity();
      _timer = false;
    }
  }

private:
  Ticker      _ticker;
  static bool _timer;

  std::shared_ptr<Document> _doc;
};

bool SDeviceLoginStarted::_timer = true;
