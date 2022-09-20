
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

//自己遷移

class SPollToken : public State {
public:
  SPollToken(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
  }

  void exitAction(void) override {
    log_d("exitAction");
    this->_context->TransitionTo(new SPollToken(std::move(_doc)));
  }

private:
  std::shared_ptr<Document> _doc;
  StaticJsonDocument<200>   _refleshtokenFilter;  //トークン再取得に使用
};
