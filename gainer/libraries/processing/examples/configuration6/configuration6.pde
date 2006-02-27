import processing.gainer.*;

Gainer gainer;

int[] map = {
0,0,0,1,1,0,0,0,
0,0,1,1,1,1,0,0,
0,1,1,1,1,1,1,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0
};

void setup(){
  //gainer.DEBUG();
  gainer = new Gainer(this,"COM3",Gainer.MODE6);
//  gainer = new Gainer(this,"COM3",6);

gainer.digitalOutput(0xFFFF);

}

void draw(){
  background(0);
  
  if(mousePressed)
  {
    for(int i=0;i<8;i++){
      scanLine(i,getLine(i));
      //delay(3);
    }
  }else{
    gainer.digitalOutput(0xFFE7);
  }
}


void scanLine(int col,int data){
  int vc = 0xFF ^ 0x1<<col;
  int s = data<<8 | vc;
  gainer.digitalOutput(s);
}

int getLine(int col){
  int ret=0;
  for(int i=0;i<8;i++){
    ret |= map[col*8+i]<<i;
  }
  
  return ret;
}
