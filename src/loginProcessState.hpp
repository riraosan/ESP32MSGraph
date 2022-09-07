
#pragma once

#include <Arduino.h>
#include "StateBase.h"

class SInitialize : public State {
public:
  void handler() override {
    // TODO WiFi接続
    this->context_->TransitionTo(new ConcreteStateA);
  }

private:
  void _startWiFi(void) {
  }
};

class SDeviceLoginStarted : public State {
  // TODO モデルのオブジェクトを生成すること。コンストラクタ？
public:
  // request
  void handler() override {
    _pollForToken();
    this->context_->TransitionTo(new ConcreteStateA);
  }

private:
  StaticJsonDocument<200> _loginFilter;  //初回ログインに使用
};

class SDeviceLoginFailed : public State {
public:
  void Handle1() override {
  }
  void Handle2() override {
    this->context_->TransitionTo(new ConcreteStateA);
  }
};

class SAuthReady : public State {
public:
  void Handle1() override {
  }
  void Handle2() override {
    this->context_->TransitionTo(new ConcreteStateA);
  }
};

class SRefreshToken : public State {
public:
  void handler() override {
    refreshToken();
    this->context_->TransitionTo(new ConcreteStateA);
  }

private:
  StaticJsonDocument<200> _refleshtokenFilter;  //トークン再取得に使用

  // Refresh the access token
  bool
};

class SPollToken : public State {
public:
  void handler() override {
    pollForToken();
    this->context_->TransitionTo(new ConcreteStateA);
  }

private:
  StaticJsonDocument<200> _refleshtokenFilter;  //トークン再取得に使用
};

class SPollforPresence : public State {
public:
  void handler() override {
    pollPresence();
    this->context_->TransitionTo(new ConcreteStateA);
  }

private:
  StaticJsonDocument<200> _presenceFilter;  //在籍情報取得時に使用

};
