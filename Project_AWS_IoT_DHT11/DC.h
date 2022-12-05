#include <Arduino.h>

#define DC_OFF 0
#define DC_ON 1

class DC{
  private:
    int in_a;
    int in_b;
    int en_a;
    byte state;

  public:
    DC(int in_a, int in_b, int en_a);
    void init();
    void on();
    void off();
    byte getState();
};