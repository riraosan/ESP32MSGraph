
#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include "SRefreshToken.h"
#include "SAuthReady.h"
#include "SDeviceLoginStarted.h"
#include "../config.h"

bool SRefreshToken::_timer = true;

SRefreshToken::SRefreshToken(std::shared_ptr<Document> doc) : _doc(doc),
                                                              _retries(0) {
}

void SRefreshToken::IntervalTimer(void) {
  _timer = true;
}

void SRefreshToken::entryAction(void) {
  log_d("entryAction");
}

void SRefreshToken::doActivity(void) {
  log_d("doActivity");
  bool success = _doc->refreshToken();
  if (success) {
    _doc->saveContext();
    _ticker.detach();
    exitAction();
  } else {
    _retries++;
    if (_retries > 3) {
      this->_context->TransitionTo(new SDeviceLoginStarted(_doc));
    } else {
      _ticker.once(DEFAULT_ERROR_RETRY_INTERVAL, IntervalTimer);
    }
  }
}

void SRefreshToken::exitAction(void) {
  log_d("exitAction");
  this->_context->TransitionTo(new SAuthReady(_doc));
}

void SRefreshToken::update(void) {
  if (_timer) {
    _timer = false;
    entryAction();
    doActivity();
  }
}
