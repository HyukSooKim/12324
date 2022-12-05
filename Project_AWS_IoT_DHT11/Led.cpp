#include "Led.h"

Led::Led(int pin){
  this->pin = pin;
  init();
}

void Led::init(){
  pinMode(pin, OUTPUT);
  off();
  state = LED_OFF;
}

void Led::on(){
  digitalWrite(pin, HIGH);
  state = LED_ON;
}

void Led::off(){
  digitalWrite(pin, LOW);
  state = LED_OFF;
}

byte Led::getState(){
  return state;
}
