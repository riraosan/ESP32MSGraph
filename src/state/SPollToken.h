
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include <Ticker.h>

class SPollToken : public State {
public:
  SPollToken(std::shared_ptr<Document> doc);

  static void IntervalTimer(void);
  void        entryAction(void) override;
  void        doActivity(void) override;
  void        exitAction(void) override;
  void        update(void) override;
  String      getDeviceCode(void);
  String      getUserCode(void);

private:
  std::shared_ptr<Document> _doc;

  Ticker      _ticker;
  static bool _timer;
  uint8_t     _retries;
};
