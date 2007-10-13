/*
 you can control in two volumes on anaglogInput
*/

import processing.gainer.*;


float court_x = 0;
float court_y = 50;
float court_width = 560;
float court_height = 310;

float wall = 20;
float bar_pos = 60;

float court_centerx = court_width/2;
float court_centery = court_y+wall+court_height/2;

color colBK = #9c4a58;
color colRD = #ce906b;
color colGN = #4dc854;
color colWE = #fcfcfc;

Ball b;

Bar bar1,bar2;
class Bar{
  color c;
  float px;
  float py;

  
  Bar(color c,float x,float y){
    px = x;
    py = y;
    this.c = c;
  }
  /*
  void center(){
    py = court_centery;
  }
  
  void update(float vy){
    py +=vy;
    if(py-15 < court_y+wall){
      py = court_y+wall +15;
    }else if(py+15 > court_y+court_height+wall){
      py = court_y+court_height+wall -15;
    }
  }*/
  
  void update(int y){
    float ty = (float)y/255;
    
    py = ty*court_height + court_y;
    if(py-15 < court_y+wall){
      py = court_y+wall +15;
    }else if(py+15 > court_y+court_height+wall){
      py = court_y+court_height+wall -15;
    }   
  }
  
  void display(){
    pushMatrix();
    translate(px,py);
    fill(c);
    rect(0,0,16,30);
    popMatrix(); 
  }
}

class Ball{
  float px;
  float py;
  float vx;
  float vy;
  
  Ball(float x,float y){
    px = x;
    py = y;
  }
  
  void update(float x,float y,float vx,float vy){
    px = x;
    py = y;
    this.vx = vx;
    this.vy = vy;
  }
  
  void update(){
    px+=vx;
    py+=vy;
    
   
    if(py < court_y+wall || py> court_y+court_height+wall){
      vy *= -1;
    }
    
    if(px < bar1.px+8 && px > bar1.px-4 && py < bar1.py+16 && py > bar1.py-16){
      vx *= -1;
    }
    
    
    if(px > bar2.px-8 && px < bar2.px+4 && py < bar2.py+16 && py > bar2.py-16){
      vx *= -1;
    }
    
    
    if(px-4 < 0 || px+4 > court_width){
      reset();
    }
    
    
  }
  
  void reset(){
      float tx = random(-2.0,2.0);
      float ty = random(-2.0,2.0);
      tx = tx>0 ? tx+2.5 : tx-2.5;
      update(court_centerx,court_centery,tx,ty );
  }
  
  void display(){
    pushMatrix();
    translate(px,py);
    fill(colWE);
    rect(0,0,8,8);
    popMatrix();
  }
  
  
};

Gainer gainer;

void setup()
{
  size(560,400);
  background(colBK);
  noStroke();
 
  b = new Ball(100,100); 
  b.reset();
  
  bar1 = new Bar(colRD,bar_pos,court_centery);
  bar2 = new Bar(colGN,court_width-bar_pos,court_centery);
  
  rectMode(CENTER);
  
  gainer = new Gainer(this);
  gainer.beginAnalogInput();
  
  frameRate(30);
}


void draw()
{
  background(colBK);
  fill(colWE);
  rectMode(CORNER);
  rect(court_x,court_y,court_width,20);
  rect(court_x,court_y+court_height+20,court_width,20); 
  rectMode(CENTER);
  b.update();
  b.display();
  
  ///////
  /*
  if(mouseX<court_centerx){
    bar1.update(float(mouseY-pmouseY));
  }else{
    bar2.update(float(mouseY-pmouseY));
  }
  */
  ///////
    bar1.update(gainer.analogInput[2]);
    bar2.update(gainer.analogInput[3]);
  
  
  ///////
  bar1.display();
  bar2.display();
  
}
