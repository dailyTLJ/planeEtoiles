

import oscP5.*;
import netP5.*;
import controlP5.*;

ControlP5 controlP5;

OscP5 oscP5;
NetAddress myRemoteLocation; 

int hog_mergeDistance = 45; 

void setup() {
  size(300,600);
  frameRate(25);
  
  controlP5 = new ControlP5(this);
  controlP5.addSlider("mergeDistance",0,100,45,50,100,100,10);
  
  /* listening */
  oscP5 = new OscP5(this,9000);
  
  /* broadcasting */
  myRemoteLocation = new NetAddress("127.0.0.1",9002);
}

void draw() {
  background(0);
}

void mousePressed() {
   /* create a new osc message object */
  OscMessage myMessage = new OscMessage("/test");
  
  myMessage.add(123); /* add an int to the osc message */
  myMessage.add(12.34); /* add a float to the osc message */
  myMessage.add("some text"); /* add a string to the osc message */

  /* send the message */
  oscP5.send(myMessage, myRemoteLocation); 
}


void keyPressed() {
  OscMessage m;
  switch(key) {
    case('c'):
      m = new OscMessage("/blobserver/signIn", new Object[] {"127.0.0.1", 9000} );
      oscP5.send(m,myRemoteLocation);  
      break;
    case('d'):
      m = new OscMessage("/blobserver/signOut", new Object[] {"127.0.0.1"} );
      oscP5.send(m,myRemoteLocation);  
      break;

  }  
}


void oscEvent(OscMessage theOscMessage) {
  println("### received an osc message. with address pattern "+
          theOscMessage.addrPattern()+" typetag "+ theOscMessage.typetag());
}


void mergeDistance(int v) {
  println("a slider event: " + v);
}
