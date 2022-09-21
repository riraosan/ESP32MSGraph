
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>
#include "SPollforPresence.h"

//遷移先
#include "SPollToken.h"

class SAuthReady : public State {
public:
  SAuthReady(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");
    //_doc->saveContext();
    this->_context->TransitionTo(new SPollforPresence(std::move(_doc)));
  }

  void exitAction(void) override {
    log_d("exitAction");
  }

private:
  std::shared_ptr<Document> _doc;
};
