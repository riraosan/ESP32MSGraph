
#pragma once

#include <typeinfo>
#include <Arduino.h>
#include <Document.h>

class Context;

class State {
public:
  virtual ~State() {
  }

  void setContext(Context *context) {
    this->_context = context;
  }

  virtual void entryAction(void) = 0;
  virtual void doActivity(void)  = 0;
  virtual void exitAction(void)  = 0;
  virtual void update(void)      = 0;

protected:
  Context *_context;
};

class Context {
public:
  Context(State *state) : _state(nullptr) {
    this->TransitionTo(state);
  }

  Context(void) : _state(nullptr) {
  }

  ~Context() {
    delete _state;
  }

  /**
   * The Context allows changing the State object at runtime.
   */
  void TransitionTo(State *state) {
    // log_i("Context: Transition to %s", typeid(*State).name());
    if (this->_state != nullptr) {
      delete this->_state;
    }

    this->_state = state;
    this->_state->setContext(this);
  }

  /**
   * The Context delegates part of its behavior to the current State object.
   */
  void entryRequest(void) {
    if (_state) {
      this->_state->entryAction();
    }
  }

  void doRequest(void) {
    if (_state) {
      this->_state->doActivity();
    }
  }

  void exitRequest(void) {
    if (_state) {
      this->_state->exitAction();
    }
  }

  void update(void){
    if(_state){
      this->_state->update();
    }
  }

  void printStateName(void) {
    if (_state) {
      // log_i("--- Context: State is %s", typeid(*_state).name());
    }
  }

private:
  /**
   * @var State A reference to the current state of the Context.
   */
  State *_state;
};
