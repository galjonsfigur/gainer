import processing.gainer.*;

Gainer gainer;
PFont myFont;

void setup() {
  size(400, 255);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);

  gainer = new Gainer(this);

  gainer.beginDigitalInput(); 
}

void draw() { 
  background(0);

  text("digitalInput[0]: " +gainer.digitalInput[0],10,80); 
  text("digitalInput[1]: " +gainer.digitalInput[1],10,110);
  text("digitalInput[2]: " +gainer.digitalInput[2],10,140);
  text("digitalInput[3]: " +gainer.digitalInput[3],10,170);

}
  
void mousePressed()
{
//you can't put gainer's anather function between beginDigitalInput() and endDigitalInput()
//gainer.onLED(); //bad
 gainer.endDigitalInput();
 gainer.turnOnLED();//ok
}

void mouseReleased()
{
 gainer.turnOffLED();//ok
 gainer.beginDigitalInput();  
// gainer.offLED();//bad
}

