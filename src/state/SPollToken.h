
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include "SAuthReady.h"

class SPollToken : public State {
public:
  SPollToken(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  static void IntervalTimer(void) {
    _timer = true;
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
    // TOFOトークンが取れた場合SAuthReadyへ遷移すること
    bool success = _doc->pollForToken();
    if (success) {
      // TODO トークンを保存する
      exitAction();
    } else {
      _ticker.once(30, IntervalTimer);
    }
  }

  void exitAction(void) override {
    log_d("exitAction");
    this->_context->TransitionTo(new SAuthReady(std::move(_doc)));
  }

  void update(void) override {
    if (_timer) {
      entryAction();
      doActivity();
      _timer = false;
    }
  }

  String getDeviceCode(void) {
    return _doc->getDeviceCode();
  }

  String getUserCode(void) {
    return _doc->getUserCode();
  }

private:
  Ticker      _ticker;
  static bool _timer;

  std::shared_ptr<Document> _doc;
};

bool SPollToken::_timer = true;
