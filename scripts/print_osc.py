# print out all OSC data coming from blobserver

from OSC import OSCServer
import sys
from time import sleep
import time


server = OSCServer( ("192.168.2.120", 9000) )
# server = OSCServer( ("localhost", 9000) )
blob_flow_id = 2
server.timeout = 0
run = True


# this method of reporting timeouts only works by convention
# that before calling handle_request() field .timed_out is 
# set to False
def handle_timeout(self):
    self.timed_out = True


# funny python's way to add a method to an instance of a class
import types
server.handle_timeout = types.MethodType(handle_timeout, server)




def startMessage(path, tags, args, source):
    print "\n- - - - - - - - - - - - "

def stopMessage(path, tags, args, source):
    pass

def ignore(path, tags, args, source):
    pass


# tags = iiffiii  << hog
# args = loc_x, loc_y, vel_x, vel_y, id, age, lost
# 
def printBlob(path, tags, args, source):
    print "\n ", 'id', args[4], "\tloc", args[0], "|", args[1], "\tvel", int(args[2]), "|", int(args[3]), "\tage", args[5], "\tlost", args[6]

def printMsg(path, tags, args, source):
    text = ''.join(path.split("/"))
    print "OSC msg: "
    print "\tPATH \t\t", path
    print "\tTAGS \t\t", tags
    print "\tARGS \t\t", args
    print "\tSOURCE \t\t", source



server.addMsgHandler( "/blobserver/startFrame", startMessage )
# server.addMsgHandler( "/blobserver/startFrame", printMsg )
server.addMsgHandler( "/blobserver/endFrame", stopMessage )
# server.addMsgHandler( "/blobserver/endFrame", printMsg )
server.addMsgHandler( "/blobserver/bgsubtractor", printBlob )
# server.addMsgHandler( "/blobserver/bgsubtractor", printMsg )
server.addMsgHandler( "/blobserver/stitch", ignore )
#server.addMsgHandler( "/blobserver/stitch", printMsg )
server.addMsgHandler( "/blobserver/hog", printBlob )
server.addMsgHandler( "/blobserver/nop", ignore )




# user script that's called by the game engine every frame
def each_frame():
    # clear timed_out flag
    server.timed_out = False
    # handle all pending requests then return
    while not server.timed_out:
        server.handle_request()



# simulate a "game engine"
while run:
    print '.'
    # do the game stuff:
    sleep(1)
    # call user script
    each_frame()

server.close()
