import processing.gainer.*;

Gainer gainer;
void setup(){
  gainer = new Gainer(this,"COM4",Gainer.MODE6);
//  gainer = new Gainer(this,"COM4",6);
  
  
}

void draw(){
  background(0);
}

void scanline(int l){
  
}


void mousePressed(){
  gainer.digitalOutput(0x00EE);
}

void mouseReleased(){
  gainer.digitalOutput(0xFFFF);
}

