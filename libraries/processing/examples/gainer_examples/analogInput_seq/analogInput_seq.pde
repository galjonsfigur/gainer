/*
  (sequens) Read to analogInput.
  and Stop to reading when mousePressed. 
*/
import processing.gainer.*;

Gainer gainer;
PFont myFont;

void setup() {
  size(300, 255);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);

  //Gainer.DEBUG = true;
  gainer = new Gainer(this);
  
  gainer.beginAnalogInput(); 
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
//you can't use gainer's function between beginAnalogInput() and endAnalogInput()
 //gainer.onLED(); //bad
 gainer.endAnalogInput();
 //gainer.turnOnLED();



}

void mouseReleased()
{
 //gainer.turnOffLED();
 gainer.beginAnalogInput();  
// gainer.offLED();//bad
}

