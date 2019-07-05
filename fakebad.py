#!/usr/bin/python

# script to behave like an unknown and possible malicious process. Gets called from startfakebad.sh.
# always writes to log /var/log/fakebad.log with file locations for cleanup 
# to clean up, run: 
#     cat /var/log/fakebad.log | xargs rm
#     rm /var/log/fakebad.log

# actions: 
# no actions, just stays running
# 1  open a file and write to it every 5 seconds
# 2  open a network (udp, tcp or raw) listener on a random port
# 3  both opens file and network port

from random import choice,randrange 
from psutil import pids,Process
from os import path,getpid
from re import sub
from time import sleep
import socket

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
    # print logfilename
  
    # create a log file for cleanup purposes
    cleanup = open("/var/log/fakebad.log", 'a')
    cleanup.write("Fakebad process's fake log file:\n\n")
    cleanup.write(logfilename+"\n\n")
    cleanup.close()

    return logfilename


def logger():
    logfile = getLogfilename()
    while path.exists(logfile):
        # print "exists"
        logfile = getLogfilename()
    logfileh = open(logfile, 'a', 0)
    return logfileh

def getactions():
    actionnum = randrange(0,4)
    return actionnum

def main():
    actions = getactions()
    # print actions

    # create a log file for cleanup purposes
    processpid = str(getpid())
    cleanup = open("/var/log/fakebad.log", 'a')
    cleanup.write("Process is running with pid "+processpid +".\n\n")
    cleanup.close()
   
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
            
    # If listen action selected (2 or 3), start listening on a random port
    # MAKE THE listening process a function and then call it random 1 to 4 times instead of just once.
    if actions == 2 or actions == 3:
        proto = choice(['tcp','udp','raw'])
        port = randrange(7,65535)
        # print proto +" "+ str(port)
        # add network info for cleanup purposes
        cleanup = open("/var/log/fakebad.log", 'a')
        cleanup.write("Fakebad process's network connection:\n\n")
        cleanup.write(proto+" "+str(port)+"\n\n")
        cleanup.close()
 
        if actions == 3:
            h = logger()

        if proto == "tcp":
            # create an INET, STREAMing socket
            serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            
            # bind the socket to a public host and a port
            serversocket.bind(('', port))

            # become a server socket for up to 5 connections
            serversocket.listen(5)

            while True:
                if actions == 3:
                    h.write("logging\n\n")
                    sleep(20) 
                #accept connections from outside
                (clientsocket, address) = serversocket.accept()

        if proto == "udp":
            # Create a TCP/IP socket
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

            # Bind the socket to the port
            sock.bind(('', port))

            while True:
                if actions == 3:
                    h.write("logging\n\n")
                    sleep(20) 
                data, address = sock.recv(4096)

        if proto == "raw":
            # Create raw socket
            rawSocket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)

            # Bind the socket 
            rawSocket.bind(('',0))

        while True:
                if actions == 3:
                    h.write("logging\n\n")
                    sleep(20) 
                receivedPacket = rawSocket.recv(2048)


if __name__ == '__main__':
    main()
