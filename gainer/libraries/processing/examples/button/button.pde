import processing.gainer.*;

Gainer gainer;
void setup(){
  gainer = new Gainer(this,"COM3");
}

void draw(){
  background(0);
  if(gainer.buttonPressed){
    background(255);
  }
}




