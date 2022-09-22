
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

//遷移先
#include "SPollforPresence.h"

// Please ass User class
// for Presence API
class SPollforPresence : public State {
public:
  SPollforPresence(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
    //TODO　プレゼンスを取得すること
    //トークン有効時間が過ぎたら、トークンリフレッシュへ遷移すること
  }

  void exitAction(void) override {
    log_d("exitAction");
  }

  void update(void) override{
    log_d("update");
  }

private:
  std::shared_ptr<Document> _doc;
  StaticJsonDocument<200>   _presenceFilter;  //在籍情報取得時に使用
};
