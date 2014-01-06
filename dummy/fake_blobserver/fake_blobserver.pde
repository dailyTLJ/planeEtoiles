/**
 * fake blobserver OSC output
 */
 
 
import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;
String client = "127.0.0.1";
int port = 9000;
ArrayList blobs;


int flowID = 1;
float maxVel = 1;
int idCount = 0;

void setup() {
  size(420,140);
  frameRate(25);
  
  /* listening */
  oscP5 = new OscP5(this,12000);
  
  myRemoteLocation = new NetAddress(client,port);
  
  blobs = new ArrayList();
  for (int i=0; i<3; i++) {
    Blob b = new Blob();
    blobs.add(b);
  }
}


void draw() {
  background(0);  
  for (int i=0; i<blobs.size(); i++) {
    Blob b = (Blob) blobs.get(i);
    b.update();
    b.draw();
  }
  
  // send OSC messages
  
  
  OscMessage m;
  m = new OscMessage("/blobserver/startFrame", new Object[] {0, flowID} );
  oscP5.send(m,myRemoteLocation);  
  
  for (int i=0; i<blobs.size(); i++) {
    Blob b = (Blob) blobs.get(i);
    // iiiffiii: id x y vx vy age lost occluded
    m = new OscMessage("/blobserver/hog", new Object[] { b.id, (int) b.x, (int) b.y, b.vx, b.vy, b.age, b.lost, b.occluded  } );
    oscP5.send(m,myRemoteLocation);  
  }
  
  m = new OscMessage("/blobserver/endFrame", new Object[] {0, flowID} );
  oscP5.send(m,myRemoteLocation);  
}


class Blob {
  float x;
  float y;
  float vx;
  float vy;
  int id;
  int age;
  int lost;
  int occluded;
  
  
  
  Blob() {
    id = idCount++;
    x = random(width);
    y = random(height);
    vx = 0;
    vy = 0;
    occluded = 0;
    lost = 0;
    age = 0;
  }
  
  void draw() {
    noStroke();
    fill(255);
    ellipse(x,y, 20, 20);
  }
  
  void update() {
    age++;
    vx += random(0.2) - 0.1;
    vy += random(0.2) - 0.1;
    if (abs(vx) > maxVel) vx *= 0.9;
    if (abs(vy) > maxVel) vy *= 0.9;
    x += vx;
    y += vy;
    
    if (x>width) {
      x = 0;
      id = idCount++;
    }
    if (x<0) {
      x = width;
      id = idCount++; 
    }
    if (y>height) {
      y = 0;
      id = idCount++;
    }
    if (y<0) {
      y = height;
      id = idCount++;
    }
  }
  
}
