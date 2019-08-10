/*##########
##  .Description
##      This script mimics a suspicious and possibly malicious process.  
##      It writes to a cleanup log (default /var/log/fakebad.log) with information 
##      about its actions.
##
##      The binary will randomly select to exhibit one the following profiles:
##          0 - no actions, just a running process
##          1 - running process that opens a file and writes to it every 5 seconds
##          2 - running process that opens 1 to 3 network listeners (randomly decides 
##              if tcp, udp, or raw) on random ports
##          3 - running process that opens file and network listener
##
##      It should be compiled into a binary using a tool such as pyinstaller on 
##      the system which it will be run. 
##           Example: pyinstaller --onefile fakebad.py
##
##      It should be started using the accompanying startfakebad.sh script. 
##
##  .Notes
##      NAME: fakebad.cpp
##      AUTHOR: Jennifer Gates
##      VERSION: 1.0
##      LASTEDIT: 10 AUG 2019
##      GIT REPO: https://github.com/jennifergates/Misc/tree/master/fakebad
##      CHANGELOG:
##      1.0 (10 AUG 2019) - Initial Draft (in progress)
##
########### */

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int addCleanup(string, string);
//int getLogfilename();
//int logger();
//int getactions();
//int startnetlistener();

int addCleanup(string message, string cleanuplog)
{
   
   cout << message;
   ofstream cleanup;
   cleanup.open(cleanuplog);
   cleanup << message;
   cleanup.close();

   return 0;

}


int main() 
{

   string message;
   string cleanuplog = "/var/log/fakebadcpp.log";

   message = "Testing\n";
   addCleanup(message, cleanuplog);

   return 0;
}