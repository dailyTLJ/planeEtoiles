/**
 * oscP5parsing by andreas schlegel
 * example shows how to parse incoming osc messages "by hand".
 * it is recommended to take a look at oscP5plug for an
 * alternative and more convenient way to parse messages.
 * oscP5 website at http://www.sojamo.de/oscP5
 */

import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;

void setup() {
  size(400,400);
  frameRate(25);
  /* start oscP5, listening for incoming messages at port 12000 */
  oscP5 = new OscP5(this,9000);
  
  /* myRemoteLocation is a NetAddress. a NetAddress takes 2 parameters,
   * an ip address and a port number. myRemoteLocation is used as parameter in
   * oscP5.send() when sending osc packets to another computer, device, 
   * application. usage see below. for testing purposes the listening port
   * and the port of the remote location address are the same, hence you will
   * send messages back to this sketch.
   */
  myRemoteLocation = new NetAddress("127.0.0.1",12000);
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


void oscEvent(OscMessage theOscMessage) {
  /* check if theOscMessage has the address pattern we are looking for. */
  
  if(theOscMessage.checkAddrPattern("/blobserver/hog")==true) {
    /* check if the typetag is the right one. */
    if(theOscMessage.checkTypetag("iiffiii")) {
      /* parse theOscMessage and extract the values from the osc message arguments. */
      int v1 = theOscMessage.get(0).intValue();  
      int v2 = theOscMessage.get(1).intValue();  
      float v3 = theOscMessage.get(2).floatValue();
      float v4 = theOscMessage.get(3).floatValue();   
      int v5 = theOscMessage.get(4).intValue();
      int v6 = theOscMessage.get(5).intValue();
      int v7 = theOscMessage.get(6).intValue();
      print("### received an osc message /hog with typetag iiffiii.");
      println(" values: "+v1+", "+v2+", "+v3+", "+v4+", "+v5+", "+v6+", "+v7);
      return;
    }  
  } 
  println("### received an osc message. with address pattern "+theOscMessage.addrPattern());
}
