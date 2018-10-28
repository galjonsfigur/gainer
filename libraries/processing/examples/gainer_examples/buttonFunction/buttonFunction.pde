/*
  when buttonPressed on Gainer.
  (callback Function)
*/
import processing.gainer.*;

Gainer gainer;
void setup(){
  size(200,200);
  gainer = new Gainer(this);
}

void draw(){
  background(0);
  rect(20,20,160,160);
}

//if you want get buttonEvent on handler
void gainerButtonEvent(boolean bt){
  if(bt){
    fill(255,0,0);
  }else{
    fill(0,0,255);
  }
}
