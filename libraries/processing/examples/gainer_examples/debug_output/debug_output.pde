/*
 debug string output
*/
import processing.gainer.*;

Gainer gainer;
PFont myFont;

void setup() {
  size(300, 255);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);

  //show the gainer serial code
  Gainer.DEBUG = true;
  gainer = new Gainer(this);

}

void draw() { 
  background(0);
 
  text("press mouse button " ,10,80); 
}
  
void mousePressed()
{
 //send code "h*"
 gainer.turnOnLED();

}

void mouseReleased()
{
  //send code "l*"
 gainer.turnOffLED();

}

