/*
configuration3
Analog in 0
Analog out 4
Digital in 8
Digital out 4
*/
import processing.gainer.*;

Gainer gainer;
void setup(){
  gainer = new Gainer(this,"COM3",2);
}

void draw(){
  background(0);
}

void mousePressed(){
  
  gainer.digitalOutput(0xFFFF);
}

void mouseReleased(){
  gainer.digitalOutput(0xA);
}

