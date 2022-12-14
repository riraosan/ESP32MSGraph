#include <unity.h>

#include <Arduino.h>
#include <WebServer.h>
#include <ESP32MSGraph.h>
using WebServerClass = WebServer;

WebServerClass server;
ESP32MSGraph   graph(&server);

void wifi_test_001(void) {
}

void wifi_test_002(void) {
}

void setup() {
  delay(2000);
  UNITY_BEGIN();

  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  graph.begin();
}

void loop() {
  graph.update();
}
