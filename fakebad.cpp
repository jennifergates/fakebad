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
#include <random>
#include <ctime>

using namespace std;

int addCleanup(string, string);
//int getLogfilename();
//int logger();
int getActions();
//int startnetlistener();

int addCleanup(string message, string cleanuplog)
{
   
   //cout << message;
   ofstream cleanup;
   cleanup.open(cleanuplog, ofstream::app);
   cleanup << message;
   cleanup.close();

   return 0;

}

int getActions()
{
   int range = 4 - 0 + 1; //0-4 inclusive
   int actions = rand() % range;
   return actions;
}

int main() 
{
   //variables
   string message;
   string cleanuplog = "/var/log/fakebadcpp.log";

   //randomly select process's actions
   int actions = getActions();

   // add timestamp to cleanup log file
   // Current date/time based on current system
   time_t now = time(0);
   // Convert now to tm struct for local timezone
   tm* localtm = localtime(&now);
   string timestamp =  asctime(localtm);
   //cout << timestamp;
   addCleanup(timestamp, cleanuplog);

   // add pid to cleanup log file
   /*addcleanup("\n"+str(timestamp))
   processpid = str(getpid())
   processname = sys.argv[0]
   addcleanup("\nProcess is running with pid: "+processpid +"\nProcess is running with name: "+processname+"\n")*/



   message = "Testing\n";
   addCleanup(message, cleanuplog);

   return 0;
}