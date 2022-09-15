#include <Arduino.h>
#include <unity.h>

#include <Connect.hpp>

#if defined(BUILD_TEST)

#if 0
//OK 2022-09-11
#define SSID     "Buffalo-C130"
#define PASSWORD "nnkxnpshmhai6"
#else
#define SSID     ""
#define PASSWORD ""
#endif

Connect wifi;

void wifi_test_001(void) {
}

void wifi_test_002(void) {
}

void setup() {
  delay(2000);
  UNITY_BEGIN();

  // wifi.begin(SSID, PASSWORD);
  wifi.begin();
}

void loop() {
  wifi.update();
}
#endif
