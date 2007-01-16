/*
  operate LED on gainer.
*/
import processing.gainer.*;

Gainer gainer;
void setup(){
  gainer = new Gainer(this);
}

void draw(){
  background(0);
}

void mousePressed(){
  gainer.turnOnLED();
}

void mouseReleased(){
  gainer.turnOffLED();
}

