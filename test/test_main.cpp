#include <Arduino.h>
#include <unity.h>

//#define BUILD_TEST

#if defined(BUILD_TEST)
#include <ESP32MSGraph.h>
void setup() {
}

void loop(){
}
#else
#include "00_basic/presence.ino"
#endif
