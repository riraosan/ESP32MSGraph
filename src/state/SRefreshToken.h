
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

//自己遷移

class SRefreshToken : public State {
public:
  SRefreshToken(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");

    //TODO トークン有効時間がすぎたら以下を実行する（タイマーを実行すればいい？）
    bool success = refreshToken();
    if (success) {
      saveContext();
      this->_context->TransitionTo(new SPollforPresence(std::move(_doc)));
    }
  }

  void exitAction(void) override {
    log_d("exitAction");
  }

private:
  std::shared_ptr<Document> _doc;
  StaticJsonDocument<200>   _refleshtokenFilter;  //トークン再取得に使用
};
