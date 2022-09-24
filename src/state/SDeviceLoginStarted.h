
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>

class SDeviceLoginStarted : public State {
public:
  SDeviceLoginStarted(std::shared_ptr<Document> doc);
  // timer event
  static void IntervalTimer(void);

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
