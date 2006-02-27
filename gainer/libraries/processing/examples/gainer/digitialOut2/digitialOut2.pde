import processing.gainer.*;

Gainer gainer;

int state = 0;
int count=0;

boolean bvals[] = {false,false,false,false};

void setup()
{
  gainer = new Gainer(this,"COM3");

  framerate(1);
}


void draw()
{
  count++;

  switch(state){
  case 0:
   count%=32;
   gainer.digitalOutput(count);
  break;
  case 1:
  {
   count%=4;
   int ports[] = {count};
   gainer.setHigh(ports);
  }
  break;
  case 2:
  {
   count%=4;
   int ports[] = {count};
   gainer.setLow(ports);
  }
  break;
  case 3:
  count%=4;
  bvals[count] = !bvals[count];
  gainer.digitalOutput(bvals);
  
  break;
  }
}

void gainerButtonEvent(boolean bt){
  if(bt){
   state++;
   state%=4; 
   
   println("now state" +state);
  }
}
