/*
 Dual Axis sensor.
 ADXL202
*/

import processing.gainer.*;

Gainer gainer;
PFont myFont;
float rSize;  // rectangle size


void setup() {
  size(300, 300, P3D);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);
  
  rSize = width/4;  
  noStroke();
  fill(204, 204);
  frameRate(30);
  
  gainer = new Gainer(this);
}

void draw() { 
  background(0);
  
  gainer.peekAnalogInput();
  float ax = gainer.analogInput[0];
  float ay = gainer.analogInput[1];
  
  
  float pitch = asin((ax-128)/128);
  float roll  = asin((ay-128)/128);
  
  text("pitch: " + pitch ,10,140); 
  text("roll: " + roll ,10,170);
  //float dt = TWO_PI/255; 

    
  translate(width/2, height/2);

  rotateX(pitch);
  rotateY(roll);
  
  //rotateX(dt*mx);
  //rotateY(dt*my);
  rect(-rSize, -rSize, rSize*2, rSize*2);


}



