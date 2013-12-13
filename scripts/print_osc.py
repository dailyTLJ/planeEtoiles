# print out all OSC data coming from blobserver

from OSC import OSCServer
import sys
from time import sleep
import time


# server = OSCServer( ("10.4.40.28", 9000) )
# server = OSCServer( ("localhost", 9000) )
server = OSCServer( ("10.10.44.21", 9000) )
blob_flow_id = 1
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
    if args[1] == blob_flow_id:
        print "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", args[0]

def stopMessage(path, tags, args, source):
    pass

def ignore(path, tags, args, source):
    pass


# tags = iiiffiii  << hog
# args = id x y vx vy age lost occluded
# 
def printBlob(path, tags, args, source):
    print 'id', str(args[0]).zfill(10), "\tloc", args[1], "|", args[2], "\tvel", int(args[3]), "|", int(args[4]), "\tage", str(args[5]).zfill(5), "\tlost", str(args[6]).zfill(3), "\toccl", args[7]

def printMsg(path, tags, args, source):
    text = ''.join(path.split("/"))
    print "OSC msg: "
    print "\tPATH \t\t", path
    print "\tTAGS \t\t", tags
    print "\tARGS \t\t", args
    print "\tSOURCE \t\t", source



server.addMsgHandler( "/blobserver/startFrame", startMessage )
# server.addMsgHandler( "/blobserver/startFrame", ignore )
server.addMsgHandler( "/blobserver/endFrame", stopMessage )
# server.addMsgHandler( "/blobserver/endFrame", ignore )
server.addMsgHandler( "/blobserver/bgsubtractor", printBlob )
# server.addMsgHandler( "/blobserver/bgsubtractor", ignore )
server.addMsgHandler( "/blobserver/stitch", ignore )
#server.addMsgHandler( "/blobserver/stitch", printMsg )
server.addMsgHandler( "/blobserver/hog", printBlob )
server.addMsgHandler( "/blobserver/nop", ignore )
server.addMsgHandler( "/blobserver/broadcast", printMsg )



# user script that's called by the game engine every frame
def each_frame():
    # clear timed_out flag
    server.timed_out = False
    # handle all pending requests then return
    while not server.timed_out:
        server.handle_request()



# simulate a "game engine"
while run:
    # print '.'
    # do the game stuff:
    sleep(0.01)
    # call user script
    each_frame()

server.close()
