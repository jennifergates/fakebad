#!/usr/bin/python

# script to create a suspicious binary that behaves like an unknown and possible malicious process. 
# Create with pyinstaller --onefile fakebad.py.  Gets called from startfakebad.sh.
# always writes to cleanuplog (default /var/log/fakebad.log) with information for cleanup.

# actions: 
# no actions, just stays running
# 1  open a file and write to it every 5 seconds
# 2  open 1-3 network listeners (udp, tcp or raw) 
# 3  open a file and network listeners

from random import choice,randrange 
from psutil import pids,Process
from os import path,getpid
from re import sub
from time import sleep
import socket
import threading
import sys

cleanuplog = "/var/log/fakebad.log"

def getLogfilename():
    # choose path for log file from list
    locations = ["/tmp","/root","/var/log","/etc","/etc/cron.d","/opt","/usr/lib"]
    randomlocation = choice(locations)

    # choose name of log file from running processes
    rpids = pids()
    randompid = choice(rpids)
    processcommand = Process(randompid)
    processname = processcommand.name()
    processname = path.basename(processname)
    # remove any unwanted characters
    processname = sub('[:()]', '', processname)
    logfilename = randomlocation+"/"+processname

    return logfilename

def addcleanup(message):
    # create a log file for cleanup purposes
    cleanup = open(cleanuplog, 'a')
    cleanup.write(message)
    cleanup.close()


def logger():
    # create a fake log file to keep open and log to
    logfile = getLogfilename()
    while path.exists(logfile):
        # if file already exists, get a new name
        logfile = getLogfilename()

    # add log file name to cleanup log 
    addcleanup("Fakebad process's fake log file: "+logfile+"\n")
    logfileh = open(logfile, 'a', 0)
    return logfileh

def getactions():
    # randomly determine what actions the process will conduct
    actionnum = randrange(0,4)
    return actionnum
    
def startnetlistener():
    # start listening randomly on a random tcp, a random udp port, or a raw listener
    proto = choice(['tcp','udp','raw'])
    port = randrange(7,65535)

    # add network info for cleanup purposes
    addcleanup("Fakebad process's network connection: "+proto+" "+str(port)+"\n")

    if proto == "tcp":
        # create an INET, STREAMing socket
        serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
        # bind the socket to a public host and a port
        serversocket.bind(('', port))

        # become a server socket for up to 5 connections
        serversocket.listen(5)
    
        # accept connections from outside
        (clientsocket, address) = serversocket.accept()

    if proto == "udp":
        # Create a TCP/IP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Bind the socket to the port
        sock.bind(('', port))
        data, address = sock.recv(4096)

    if proto == "raw":
        # Create raw socket
        rawSocket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)

        # Bind the socket 
        rawSocket.bind(('',0))
        receivedPacket = rawSocket.recv(2048)


def main():
    # randomly select process's actions
    actions = getactions()
    
    # add pid to cleanup log file
    processpid = str(getpid())
    processname = sys.argv[0]
    addcleanup("\nProcess is running with pid: "+processpid +"\nProcess is running with name: "+processname+"\n")
   
    # If no actions selected (0), just sleep so process is in ps list
    if actions == 0 :
        while True:
            sleep(20)

    # If log action selected (1), create a random file and start logging to it
    if actions == 1 :
        h = logger()
        while True:
            h.write("logging\n\n")
            sleep(20)
            
    # If listen action selected (2 or 3), start listener(s)
    if actions == 2 or actions == 3 :

        #open 1-3 network connections of various types each in its own thread
        rangeend = randrange(1,4)
        for n in range(0,rangeend):
            # create a new thread to leave the listener open and still write to the log file
            thread = threading.Thread(target=startnetlistener, args=())
            thread.start()

        #if log and network selected (3), also create random file and start logging to it
        if actions == 3:
            h = logger()
        while True:
            if actions == 3:
                h.write("logging\n\n")
            sleep(20) 


if __name__ == '__main__':
    main()

