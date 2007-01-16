/*
 (One time) peeping to analogInput when mousePressed.
*/

import processing.gainer.*;

Gainer gainer;
PFont myFont;

void setup() {
  size(300, 255);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);

  gainer = new Gainer(this);
  
}

void draw() { 
  background(0);
 
  text("analogInput[0]: " +gainer.analogInput[0],10,80); 
  text("analogInput[1]: " +gainer.analogInput[1],10,110);
  text("analogInput[2]: " +gainer.analogInput[2],10,140);
  text("analogInput[3]: " +gainer.analogInput[3],10,170);

}
  
void mousePressed()
{
  gainer.peekAnalogInput();
}
  
