
#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include "SAuthReady.h"
#include "../config.h"

bool SAuthReady::_timer = false;

SAuthReady::SAuthReady(std::shared_ptr<Document> doc) : _doc(doc),
                                                        _retries(0) {
  // Set timer to start within token validity time
  _ticker.once(60 * TOKEN_REFRESH_TIMEOUT, tokenRefreshTimer);
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
    _doc->saveContext();
    _ticker.detach();
    _ticker.once(60 * TOKEN_REFRESH_TIMEOUT, tokenRefreshTimer);
  } else {
    _retries++;
    if (_retries > 5) {
      ESP.restart();
    }
    _ticker.detach();
    _ticker.once(DEFAULT_ERROR_RETRY_INTERVAL, tokenRefreshTimer);
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
