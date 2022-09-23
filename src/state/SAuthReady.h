
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

class SAuthReady : public State {
public:
  SAuthReady(std::shared_ptr<Document> doc) : _doc(doc) {
    //トークン有効時間のタイマーを設定する
    _ticker.once(60 * 10, tokenRefreshTimer);
  }

  static void tokenRefreshTimer(void) {
    _timer = true;
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");

    // TODO トークン有効時間がすぎたら以下を実行する（タイマーを実行すればいい？）
    bool success = _doc->refreshToken();
    if (success) {
      // TODO トークンを保存できること
    } else if (/) {
    } else {
      // TODO リトライ回数を設定して回数を越えた場合の処理を考えること
      // TODO リトライ回数を越えた場合、何らかの理由で接続できなくなったと判定して機器をリセットしてWiFi再接続からやり直すこと
      // TODO 再認証の際に保存したデバイスIDで再び認証できること
      _ticker.once(10, IntervtokenRefreshTimeralTimer);
    }
  }

  void exitAction(void) override {
    log_d("exitAction");
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

bool SAuthReady::_timer = false;
