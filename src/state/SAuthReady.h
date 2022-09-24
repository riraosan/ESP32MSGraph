
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include "Ticker.h"

class SAuthReady : public State {
public:
  SAuthReady(std::shared_ptr<Document> doc);

  static void tokenRefreshTimer(void);

  void entryAction(void) override;
  void doActivity(void) override;
  void exitAction(void) override;
  void update(void) override;

private:
  std::shared_ptr<Document> _doc;

  Ticker      _ticker;
  static bool _timer;
  uint8_t     _retries;
};
