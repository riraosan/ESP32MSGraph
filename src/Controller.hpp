
#pragma once

#include <Arduino.h>
#include <StateBase.h>
#include <Document.h>

class SWiFiConnecting;

// State
#define SMODEWIFICONNECTED        2   // Wifi connected
#define SMODEDEVICELOGINSTARTED   10  // Device login flow was started
#define SMODEDEVICELOGINFAILED    11  // Device login flow failed
#define SMODEAUTHREADY            20  // Authentication successful
#define SMODEREFRESHTOKEN         22  // Access token needs refresh
// TODO
#define SMODEPOLLPRESENCE         21  // Poll for presence
#define SMODEPRESENCEREQUESTERROR 23  // Access token needs refresh

class SInitialize : public State {
public:
  SInitialize(Document *doc) : _doc(doc) {}

  void action1() override {
    // TODO なにかの処理
  }

  void action2() override {
    this->_context->TransitionTo(new SWiFiConnecting(_doc));
  }

private:
  Document *_doc;
};

// SMODEWIFICONNECTING
//
class SWiFiConnecting : public State {
public:
  SWiFiConnecting(Document *doc) : _doc(doc) {
  }

  void action1() override {
    // WiFiの接続を開始する
    while (1) {
      if (/*WiFiクライアントがWiFiに接続するまで待つ*/) {
      } else {  // WiFiクライアントがWiFiに接続したら、次の状態へ遷移すること
                // TODO WiFiは接続したままとすること
                // TODO WiFiの接続はDocumentクラスで行うこと。
        this->_context->TransitionTo(new SWiFiConnected(_doc));
      }
      delay(1);
    }
  }

  void action2() override {
  }

private:
  Document *_doc;
};

// SMODEWIFICONNECTED
class SWiFiConnected : public State {
public:
  SWiFiConnected(Document *doc) : _doc(doc) {
  }

  void action1() override {
    // TODO _docのプロパティに従ってView表示を行う
  }

  void action2() override {
    this->_context->TransitionTo(new SDeviceLoginStarted(_doc));
  }

private:
  Document *_doc;
};

class SDeviceLoginStarted : public State {
public:
  SDeviceLoginStarted(Document *doc) : _doc(doc) {
  }

  void action1() override {
    // TODO _docのプロパティに従ってView表示を行う
  }

  void action2() override {
    this->_context->TransitionTo(new SAuthReady(_doc));
    this->_context->TransitionTo(new SDeviceLoginFailed(_doc));
    this->_context->TransitionTo(new SDeviceLoginStarted(_doc));
  }

private:
  Document *_doc;
};

class SDeviceLoginFailed : public State {
  void action2() override {
    this->_context->TransitionTo(new SDeviceLoginStarted(_doc));
  }

public:
  void Handle1() override {
  }
  void Handle2() override {
    this->_context->TransitionTo(new ConcreteStateA);
  }
};

class SAuthReady : public State {
public:
  void Handle1() override {
  }
  void Handle2() override {
    this->_context->TransitionTo(new ConcreteStateA);
  }
};

class SRefreshToken : public State {
public:
  void handler() override {
    refreshToken();
    this->_context->TransitionTo(new ConcreteStateA);
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
    this->_context->TransitionTo(new ConcreteStateA);
  }

private:
  StaticJsonDocument<200> _refleshtokenFilter;  //トークン再取得に使用
};

class SPollforPresence : public State {
public:
  void handler() override {
    pollPresence();
    this->_context->TransitionTo(new ConcreteStateA);
  }

private:
  StaticJsonDocument<200> _presenceFilter;  //在籍情報取得時に使用
};
