
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include "SAuthReady.h"

class SRefreshToken : public State {
public:
  SRefreshToken(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  static void IntervalTimer(void) {
    _timer = true;
  }

  void entryAction(void) override {
    log_d("entryAction");
    //無し
  }

  void doActivity(void) override {
    log_d("doActivity");

    // TODO トークン有効時間がすぎたら以下を実行する（タイマーを実行すればいい？）
    bool success = _doc->refreshToken();
    if (success) {
      // TODO トークンを保存できること
      exitAction();
    } else {
      _ticker.once(10, IntervalTimer);
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

private:
  static bool _timer;
  Ticker      _ticker;

  std::shared_ptr<Document> _doc;
};

bool SRefreshToken::_timer = false;
