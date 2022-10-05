
#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include "SDeviceLoginStarted.h"
#include "SPollToken.h"
#include "../config.h"

bool SDeviceLoginStarted::_timer = true;

SDeviceLoginStarted::SDeviceLoginStarted(std::shared_ptr<Document> doc) : _doc(doc),
                                                                          _retries(0) {
}

// timer event
void SDeviceLoginStarted::IntervalTimer(void) {
  _timer = true;
}

void SDeviceLoginStarted::entryAction(void) {
  log_d("entryAction");
}

void SDeviceLoginStarted::doActivity(void) {
  log_d("doActivity");
  bool success = _doc->startDevicelogin();  // detecting device-code event
  if (success) {
    _ticker.detach();
    exitAction();
  } else {
    _retries++;

    if (_retries > 12) {  // 3 minuits
      ESP.restart();
    }

    _ticker.once(DEFAULT_ERROR_RETRY_INTERVAL, IntervalTimer);
  }
}

void SDeviceLoginStarted::exitAction(void) {
  log_d("exitAction");
  this->_context->TransitionTo(new SPollToken(_doc));
}

void SDeviceLoginStarted::update(void) {
  if (_timer) {
    _timer = false;
    entryAction();
    doActivity();
  }
}
