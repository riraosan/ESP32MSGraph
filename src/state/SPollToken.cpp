
#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include "SPollToken.h"
#include "SAuthReady.h"
#include "SDeviceLoginStarted.h"

bool SPollToken::_timer = true;

SPollToken::SPollToken(std::shared_ptr<Document> doc) : _doc(doc),
                                                        _retries(0) {
}

void SPollToken::IntervalTimer(void) {
  _timer = true;
}

void SPollToken::entryAction(void) {
  log_d("entryAction");
}

void SPollToken::doActivity(void) {
  log_d("doActivity");
  bool success = _doc->pollForToken();
  if (success) {
    //timerを解放できること
    _ticker.detach();
    // TODO トークンが保存できること
    exitAction();
  } else {
    _retries++;
    if (_retries > 3) {
      this->_context->TransitionTo(new SDeviceLoginStarted(std::move(_doc)));
    } else {
      _ticker.once(60, IntervalTimer);
    }
  }
}

void SPollToken::exitAction(void) {
  log_d("exitAction");
  this->_context->TransitionTo(new SAuthReady(std::move(_doc)));
}

void SPollToken::update(void) {
  if (_timer) {
    _timer = false;
    entryAction();
    doActivity();
  }
}

String SPollToken::getDeviceCode(void) {
  return _doc->getDeviceCode();
}

String SPollToken::getUserCode(void) {
  return _doc->getUserCode();
}
