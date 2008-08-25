/*
 MODE7 MatrixLED sample 
 masato at bird.dip.jp
*/

import processing.gainer.*;

Gainer gainer = null;

int pos = 10;
int x = 12;
String text = "Gainer 8x8 Matrix LED Library for Proce55ing";
int len = text.length() * 15;
int sw = 0;

void setup()
{
  if (gainer == null) 
    gainer = new Gainer(this, "COM3", Gainer.MODE7, false);
  size(200, 200);
  PFont fontA = loadFont("Albany-8.vlw");
  textFont(fontA, 32);
  noLoop();
}

void draw()
{
  background(0);
  fill(255, 0, 0);
  x -= 4;
  if (x <= -len) x = 0;
  text(text, x, 60);
  if (x < 0) 
    text(text, len + x, 60);
  
  PImage p = get(8, 36, 32, 32);
  p.filter(GRAY);
  p.filter(THRESHOLD, .3);

  set(60, 90, p);

  PImage pp = new PImage(8,8);
  int[] px = new int[64];
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      int x, y;
      x = i * 16;
      y = j * 4;
      px[i * 8 + j] = ((p.pixels[x * 8 + y] & 0xffffff) == 0)? 0: 15;
      pp.pixels[i * 8 + j] = p.pixels[x * 8 + y];
    }
  }
  set(60, 140, pp);
  gainer.scanMatrix(px); 
   delay(50);
}

void mousePressed()
{
  if (sw == 0) {
    loop();
  } else {
    noLoop();
  }
  sw = (sw == 0)? 1: 0;
}
