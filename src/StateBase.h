
#pragma once

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

  virtual void action1(void) = 0;
  virtual void action2(void) = 0;

protected:
  /**
   * @var Context
   */
  Context *_context;
};

class Context {
public:
  Context(State *state) : _state(nullptr) {
    this->TransitionTo(state);
  }

  ~Context() {
    delete _state;
  }

  /**
   * The Context allows changing the State object at runtime.
   */
  void TransitionTo(State *state) {
    log_i("Context: Transition to %s", typeid(*state).name());
    if (this->_state != nullptr) {
      delete this->_state;
    }

    this->_state = state;
    this->_state->set_context(this);
  }

  /**
   * The Context delegates part of its behavior to the current State object.
   */
  void request1(void) {
    this->_state->action1();
  }

  void request2(void) {
    this->_state->action2();
  }

private:
  /**
   * @var State A reference to the current state of the Context.
   */
  State *_state;
};
