#pragma once

#include <Arduino.h>

class ESP32MSGraph : public Documents {
public:
  ESP32MSGraph(void) {
  }

  ~ESP32MSGraph() {
  }

  void begin(void) {
    //ここにパブリッシャー（発行者：Document）とサブスクライバー（購読者：View）を生成する（Document-View and State)
    pub.begin(&subscriber);
  }

  void update(void) {
    //ここにパブリッシャーのupdateメソッドをコールする
    pub.update();
  }

private:
  State     *_state;
  WebServer *_server;
  View      *subscriber;  //観察
};




ESP32MSGraph graph;//ステートマシンと意味づけすること

std::unique_ptr<ViewLED>   _led;
std::unique_ptr<ViewHTML>  _html;
std::unique_ptr<Doccument> _jsondoc;

void setup(void) {
  _led.reset(new ViewLED);
  _html.reset(new ViewHTML);

  _jsondoc.attachSubscriber(_led);
  _jsondoc.attachSubscriber(_html);

  _state.reset(new State);

  _state.attachPublisher(_jsondoc);
  _state.attachWiFi(_wifi);
}

void loop(void){
  _state.update()//状態遷移する
}

*/
