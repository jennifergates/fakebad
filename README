# README FAKEBAD 

fakebad was created to accompany a training module on techniques to locate a malicious or possibly malicious binary running on a linux system. 

It is comprised of two scripts, one python and one shell. The python script should be converted to a binary file with something like PyInstaller.  The shell script is used to launch the binary.

The binary will randomly select to exhibit one the following profiles:
 - no actions, just a running process
 - running process that opens a file and writes to it every 5 seconds
 - running process that opens a network listener (randomly decides if tcp, udp, or raw) on random port
 - running process that opens file and network listener

For clean up purposes, the scripts will write information about their actions to a log file at /var/log/fakebad.log. **This file contains, in effect, the answers and should not be referenced during the exercise.**

The shell script (startfakebad.sh) should be run with root privileges.  When run it will ask for the location of the binary file. It then does a bit of clean up if previously run. 

The shell script choses a random process name from the processes currently running on the system to use as a disguise for the binary process.

Finally, the shell script executes the binary.
