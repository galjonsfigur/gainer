/*
when buttonPressed on Gainer.
*/
import processing.gainer.*;

Gainer gainer;
void setup(){
  size(200,200);
  Gainer.DEBUG = true;
  gainer = new Gainer(this);
}

void draw(){
  background(0);
  if(gainer.buttonPressed){
    background(255);
  }
}




