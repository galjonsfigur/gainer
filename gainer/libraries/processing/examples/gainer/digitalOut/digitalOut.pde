import processing.gainer.*;

PFont myFont;
Gainer gainer;

void setup(){
  size(400,200);
  myFont = loadFont("CourierNewPSMT-24.vlw");
  textFont(myFont, 24);
  
  gainer = new Gainer(this,"COM3");
  
}

void draw(){
  background(0);
  text("Please press key ",20,20);
  text(" '1' and 'q' ",20,60);
  text(" '2' and 'w' ",20,100);
  text(" '3' and 'e' ",20,140);
  text(" '4' and 'r' ",20,180);
}

void mousePressed()
{
  //this is fast
  if(mouseButton==LEFT)
    gainer.digitalOutput(0x00);
  if(mouseButton==RIGHT)
     gainer.digitalOutput(0xff);
}

void keyPressed()
{
  if(key == '1'){
    gainer.setHigh(1);
  }
  if(key == 'q'){
    gainer.setLow(1);    
  }
  if(key == '2'){
    //this is late
    gainer.setHigh(0);
    gainer.setHigh(1);
    gainer.setHigh(2);
    gainer.setHigh(3);
  }
  if(key == 'w'){
    //this is late
   gainer.setLow(0);
   gainer.setLow(1);
   gainer.setLow(2);
   gainer.setLow(3);
  }
  if(key == '3'){
    //this is faster than setHigh(int) and setLow(int) 
     boolean b[] = {true,false,true,false};
     gainer.digitalOutput(b);
     
  }
  if(key == 'e'){
    //this is faster than setHigh(int) and setLow(int) 
     boolean b[] = {false,true,false,true};
     gainer.digitalOutput(b);
     
  }
  if(key == '4'){
      //this is faster than setHigh(int) and setLow(int) 
      int p[] = {0,1,3};
      gainer.setHigh(p);
  }
  if(key == 'r'){
    //this is faster than setHigh(int) and setLow(int) 
    int p[] = {0,1,3};
     gainer.setLow(p);
  }
    

}



