

import oscP5.*;
import netP5.*;
import controlP5.*;

ControlP5 controlP5;

OscP5 oscP5;
NetAddress myRemoteLocation; 

String client = "localhost";
String actuator = "Actuator";
int blobport = 9002;
int port = 5555;
int flow = 1;

void setup() {
  size(250,520);
  frameRate(25);
  
  controlP5 = new ControlP5(this);
  
  int x = 20;
  int y = 10;
  int h = 15;
  int dy = 30;
  controlP5.addButton("connect",0,x,y+=dy,80,19);
  y+=dy;
  controlP5.addSlider("mergeDistance",0,100,45,x,y+=dy,100,h);
  controlP5.addSlider("maxTrackDistance",0,200,100,x,y+=dy,100,h);
  controlP5.addSlider("occlusionDistance",0,100,32,x,y+=dy,100,h);
  controlP5.addSlider("lifetime",0,30,10,x,y+=dy,100,h);
  controlP5.addSlider("filterSize",0,5,1,x,y+=dy,100,h);
  controlP5.addSlider("processNoiseCov",0,10,7,x,y+=dy,100,h);
  controlP5.addSlider("measurementNoiseCov",0,10,4,x,y+=dy,100,h);
  controlP5.addSlider("margin",-1,1,0,x,y+=dy,100,h);
  controlP5.addSlider("maxTimePerFrame",0,10,5,x,y+=dy,100,h);
  controlP5.addSlider("maxThreads",0,10,8,x,y+=dy,100,h);
  controlP5.addToggle("saveSamples",false,x,y+=dy,20,20);
  y+=10;
  controlP5.addSlider("saveSamplesAge",10,200,120,x,y+=dy,100,h);
  
  
  /* listening */
  oscP5 = new OscP5(this,port);
  
  /* broadcasting */
  myRemoteLocation = new NetAddress("127.0.0.1",blobport);
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
}

void connect(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/signIn", new Object[] {client, port} );
  oscP5.send(m,myRemoteLocation);  
  println("connect");
}


void oscEvent(OscMessage theOscMessage) {
  println("### received an osc message. with address pattern "+
          theOscMessage.addrPattern()+" typetag "+ theOscMessage.typetag());
  println("### " +theOscMessage.toString());
  println("### " +theOscMessage.get(0));
}

void filterSize(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "filterSize", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("filterSize: " + v);
}

void mergeDistance(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "mergeDistance", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("mergeDistance: " + v);
}

void lifetime(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "lifetime", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("lifetime: " + v);
}

void maxTrackDistance(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "maxTrackDistance", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("maxTrackDistance: " + v);
}

void occlusionDistance(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "occlusionDistance", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("occlusionDistance: " + v);
}

void saveSamplesAge(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "saveSamplesAge", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("saveSamplesAge: " + v);
}

void margin(float v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "margin", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("margin: " + v);
}

void maxThreads(int v) {
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "maxThreads", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("maxThreads: " + v);
}

void processNoiseCov(int v) {
  float logv = pow(10, -v);
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "processNoiseCov", new Float(logv) } );
  oscP5.send(m,myRemoteLocation);  
  println("processNoiseCov: " + logv);
}

void measurementNoiseCov(int v) {
  float logv = pow(10, -v);
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "measurementNoiseCov", new Float(logv) } );
  oscP5.send(m,myRemoteLocation);  
  println("measurementNoiseCov: " + logv);
}

void maxTimePerFrame(int v) {
  float logv = 2 * pow(10, v);
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "maxTimePerFrame", new Float(logv) } );
  oscP5.send(m,myRemoteLocation);  
  println("maxTimePerFrame: " + logv);
}

void saveSamples(boolean theFlag) {
  int v = 0;
  if(theFlag==true) {
    v = 1;
  }
  OscMessage m;
  m = new OscMessage("/blobserver/setParameter", new Object[] {client, flow, actuator, "saveSamples", new Float(v) } );
  oscP5.send(m,myRemoteLocation);  
  println("saveSamples: " + v);
}
