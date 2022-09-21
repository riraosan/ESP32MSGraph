
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>

class SInitialize : public State {
public:
  SInitialize(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("entryAction");
  }

  void doActivity(void) override {
    log_d("doActivity");

  }

  void exitAction(void) override {
    log_d("exitAction");
  }

private:
  std::shared_ptr<Document> _doc;
};
