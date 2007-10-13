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

  noLoop();
  
  gainer = new Gainer(this);
  
}

void draw() { 

}
  
void mousePressed()
{
  gainer.peekAnalogInput();
}
  
void gainerAnalogInputUpdated(int[] ain)
{
    background(0);
 
  text("analogInput[0]: " +ain[0],10,80); 
  text("analogInput[1]: " +ain[1],10,110);
  text("analogInput[2]: " +ain[2],10,140);
  text("analogInput[3]: " +ain[3],10,170);

  redraw();
}
