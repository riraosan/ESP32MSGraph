
#pragma once

#include <memory>
#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>
#include <ArduinoJson.h>

// user class
// for Presence API
class SPollforPresence : public State {
public:
  SPollforPresence(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    //this->_context->TransitionTo(new ConcreteStateA);
  }

private:
  std::shared_ptr<Document> _doc;
  StaticJsonDocument<200>   _presenceFilter;  //在籍情報取得時に使用
};

class SRefreshToken : public State {
public:
  SRefreshToken(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    // this->_context->TransitionTo(new ConcreteStateA);
  }

private:
  std::shared_ptr<Document> _doc;
  StaticJsonDocument<200>   _refleshtokenFilter;  //トークン再取得に使用
};

class SPollToken : public State {
public:
  SPollToken(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    // this->_context->TransitionTo(new ConcreteStateA);
  }

private:
  std::shared_ptr<Document> _doc;
  StaticJsonDocument<200>   _refleshtokenFilter;  //トークン再取得に使用
};

class SAuthReady : public State {
public:
  SAuthReady(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    // this->_context->TransitionTo(new ConcreteStateA);
  }

private:
  std::shared_ptr<Document> _doc;
};

class SDeviceLoginStarted : public State {
public:
  SDeviceLoginStarted(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    // this->_context->TransitionTo(new ConcreteStateA);
  }

private:
  std::shared_ptr<Document> _doc;
};

class SDeviceLoginFailed : public State {
public:
  SDeviceLoginFailed(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    this->_context->TransitionTo(new SDeviceLoginStarted(_doc));
  }

private:
  std::shared_ptr<Document> _doc;
};

class SWiFiConnected : public State {
public:
  SWiFiConnected(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    this->_context->TransitionTo(new SDeviceLoginStarted(_doc));
  }

private:
  std::shared_ptr<Document> _doc;
};
class SWiFiConnecting : public State {
public:
  SWiFiConnecting(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    this->_context->TransitionTo(new SWiFiConnected(_doc));
  }

private:
  std::shared_ptr<Document> _doc;
};

class SInitialize : public State {
public:
  SInitialize(std::shared_ptr<Document> doc) : _doc(doc) {
  }

  void entryAction(void) override {
    log_d("SInitialize::action1");
  }

  void doActivity(void) override {
  }

  void exitAction(void) override {
    log_d("Sinitialize::action1");
    // this->_context->TransitionTo(new SWiFiConnecting(std::move(_doc)));
  }

private:
  std::shared_ptr<Document> _doc;
};
