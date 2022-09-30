
#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include "SAuthReady.h"

bool SAuthReady::_timer = false;

SAuthReady::SAuthReady(std::shared_ptr<Document> doc) : _doc(doc),
                                                        _retries(0) {
  //トークン有効時間内に発火するタイマーを設定
  _ticker.once(60 * 50, tokenRefreshTimer);
}

void SAuthReady::tokenRefreshTimer(void) {
  _timer = true;
}

void SAuthReady::entryAction(void) {
  log_d("entryAction");
}

void SAuthReady::doActivity(void) {
  log_d("doActivity");

  bool success = _doc->refreshToken();
  if (success) {
    // TODO トークンを保存できること
    _doc->saveContext();
    _ticker.detach();
    _ticker.once(60 * 50, tokenRefreshTimer);
  } else {
    _retries++;
    if (_retries > 5) {
      ESP.restart();
      // TODO 再認証の際に保存したデバイスIDで再び認証できること
    }
    _ticker.detach();
    _ticker.once(30, tokenRefreshTimer);
  }
}

void SAuthReady::exitAction(void) {
  log_d("exitAction");
}

void SAuthReady::update(void) {
  if (_timer) {
    _timer = false;
    entryAction();
    doActivity();
  }
}
