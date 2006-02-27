import processing.gainer.*;

Gainer gainer;
void setup(){
  size(200,200);
  gainer = new Gainer(this,"COM3");
}

void draw(){
  background(0);
  rect(20,20,160,160);
}

//if you want get buttonEvent on callback()
void gainerButtonEvent(boolean bt){
  if(bt){
    fill(255,0,0);
  }else{
    fill(0,0,255);
  }
}
