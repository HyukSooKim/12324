#include <Arduino.h>

#define LED_OFF 0
#define LED_ON 1

// 클래스 선언부 , 정의는 cpp 파일에 있음 
class Led{
  private:
    int pin;
    byte state;

  public:
    Led(int pin);
    void init();
    void on();
    void off();
    byte getState();
};
