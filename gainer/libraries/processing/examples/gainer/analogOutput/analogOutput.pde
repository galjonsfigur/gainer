import processing.gainer.*;

Gainer gainer;
void setup(){
  size(250,250);
  gainer = new Gainer(this,"COM3");
  stroke(255);
}

void draw(){
  background(0);

  line(0,height/2,width,height/2);
  line(width/2,0,width/2,height);
}

void mouseMoved()
{
  float rx = (float)mouseX / width;
  float ry = (float)mouseY / height;
  
  int v0 = int(255*ry);
  int v1 = int(255*rx);
  
  gainer.analogOutput(0,v0);
  gainer.analogOutput(1,v1); 
  
}

void mouseDragged()
{
  float rx = (float)mouseX / width;
  float ry = (float)mouseY / height;
  
  int v0 = int(255*ry);
  int v1 = int(255*rx);
  
  int values[] = {v0,v1,v0,v1};
  gainer.analogOutput(values);

}





