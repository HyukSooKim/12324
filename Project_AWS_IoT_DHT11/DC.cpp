#include "DC.h"

DC::DC(int in_a, int in_b, int en_a){
  this->in_a = in_a;
  this->in_b = in_b;
  this->en_a = en_a;
  init();
}

void DC::init(){
  pinMode(in_a, OUTPUT);
  pinMode(in_b, OUTPUT);
  off();
  state = DC_OFF;
}

void DC::on(){
     digitalWrite(in_a, HIGH);
     digitalWrite(in_b, LOW);
     analogWrite(en_a, 100);
     state = DC_ON;
}

void DC::off(){
     digitalWrite(in_a, LOW);
     digitalWrite(in_b, LOW);
     analogWrite(en_a, 0);
     state = DC_OFF;
}

byte DC::getState(){
  return state;
}