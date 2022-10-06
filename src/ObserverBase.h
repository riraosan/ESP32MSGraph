
#pragma once

// https://refactoring.guru/en/design-patterns/observer/cpp/example

class ISubscriver {
public:
  virtual ~ISubscriver(){};
  virtual void Update(void) = 0;
};

class IPublisher {
public:
  virtual ~IPublisher(){};
  virtual void Attach(ISubscriver *subscriver) = 0;
  virtual void Detach(ISubscriver *subscriver) = 0;
  virtual void Notify()                        = 0;
};
