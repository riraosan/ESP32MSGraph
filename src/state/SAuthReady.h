
//#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

class SAuthReady : public State {
public:
  SAuthReady(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
    //TODO タイマーを設定する（トークン有効時間）
  }

  void doActivity(void) override {
    log_d("doActivity");
    //TODO　トークン有効時間内に一回だけトークンをリフレッシュすること
  }

  void exitAction(void) override {
    log_d("exitAction");
  }

  void update(void){

  }

private:
  std::shared_ptr<Document> _doc;
};
