/*
 (One time) peeping to digitalinput when mousePressed.
 
*/
import processing.gainer.*;

Gainer gainer;
PFont myFont;

void setup() {
  size(400, 255);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);

  noLoop();
  gainer = new Gainer(this); 
  gainer.beginDigitalInput();
}

void draw() { 

}
  
void mousePressed()
{
  gainer.endDigitalInput();
}

void mouseReleased()
{
  gainer.beginDigitalInput();
}
  
void gainerDigitalInputUpdated(boolean[] din)
{
  background(0);
    
  text("digitalInput[0]: " +din[0],10,80); 
  text("digitalInput[1]: " +din[1],10,110);
  text("digitalInput[2]: " +din[2],10,140);
  text("digitalInput[3]: " +din[3],10,170);
  
  redraw();
}
