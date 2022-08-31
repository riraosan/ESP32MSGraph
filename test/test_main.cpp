#include <Arduino.h>
#include <unity.h>

//#define BUILD_TEST

#if defined(BUILD_TEST)
void setup() {
}

void loop() {
}
#else
#include "./00_basic/presence.ino"
#endif
