/*
configuration3
Analog in 4
Analog out 0
Digital in 4
Digital out 8
*/


import processing.gainer.*;

Gainer gainer;
void setup(){
  gainer = new Gainer(this,"COM3",3);
}

void draw(){
  background(0);
}

void mousePressed(){
  int[] out = {155,255,40,150,70,200,50,0};
  gainer.analogOutput(out);
}

void mouseReleased(){
  int[] out = {0,0,0,0,0,0,0,0};
  gainer.analogOutput(out);
}

