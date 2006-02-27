import processing.gainer.*;

Gainer gainer;


int idx[] = new int[4];
int vec[] = new int[4];

int ltb[] = {0, 0,1, 1,1, 1,1, 1,2, 2,2, 2,3, 3,3,
             3,4, 4,5, 5,6, 6,7, 7,8, 8,9, 9,10,
             10,11, 11,12, 13,14, 15,17, 18,20,
             21,23, 24,25, 26,28, 29,31, 32,34,
             39,45, 51,57, 62,68, 74,80, 83,86,
             89,93, 101,109, 117,126, 149,173,
             196,220, 224,228, 232,237, 241,246,250,255 };


void setup(){
  size(250,250);
  gainer = new Gainer(this,"COM3");
  stroke(255);
  
  idx[0] = 15;
  idx[1] = 30;
  idx[2] = 45;
  idx[3] = 70;

  for(int i=0;i<4;i++){
    vec[i] = 1;
  }
 
}


void draw(){
  background(0);

  for(int i=0;i<4;i++){
    idx[i] += vec[i];
    if(idx[i] >= ltb.length-1 || idx[i] <=0){
      vec[i]*=-1;
    }
    
  }
  
  int values[] = {ltb[idx[0]],ltb[idx[1]],ltb[idx[2]],ltb[idx[3]]};
  gainer.analogOutput(values);
  
/*
  print("  " + ltb[idx[0]]);
  print("  " + ltb[idx[1]]);
  print("  " + ltb[idx[2]]);
  println("  " + ltb[idx[3]]);
*/
}
