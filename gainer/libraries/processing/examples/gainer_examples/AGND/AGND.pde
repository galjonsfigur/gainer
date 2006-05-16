import processing.gainer.*;

Gainer gainer;
PFont myFont;

int dataBuffer = 256;
int inX[] = new int[dataBuffer];
int inY[] = new int[dataBuffer];

int count=0;

void setup() {
  size(300, 255);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);

  stroke(255);
  fill(255);
  
  gainer = new Gainer(this);

  gainer.beginAnalogInput(); 
}

void draw() { 
  background(0);
  
  text("analogInput[0]: " +gainer.analogInput[0],10,80); 
  text("analogInput[1]: " +gainer.analogInput[1],10,110);

  inX[count] = gainer.analogInput[0];
  inY[count] = gainer.analogInput[1];
  
  for(int i=0;i<dataBuffer;i++){
    stroke(0,255,0);
    point(i+10,inX[i]);
    stroke(0,0,255);
    point(i+10 ,inY[i]+20); 
  }

  

count++;
count%=256;

}
  
void mousePressed()
{
 gainer.endAnalogInput();
}

void mouseReleased()
{
 gainer.beginAnalogInput();  
}

