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
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <vector>

using namespace std;

int addCleanup(string, string);
string getLogfilename();
//int logger();
int getActions();
//int startnetlistener();

string getLogfilename()
{
   // choose path for log file from list of obfuscated locations
   vector <string> rotlocations = {",bq`,fkfq+a", ",bq`,`olk+a", ",bq`,fkfq", ",rpo,p_fk", ",rpo,_fk"}; 
   int range = rotlocations.size() - 0 + 1; //inclusive
   int choice = rand() % range;   

   // unobfuscate location
   //vector <string> locations;
   //for (string rotlocation : rotlocations)
   //{
      string location;
      for (char rotletter : rotlocations[choice])
      {
         location.push_back(rotletter + 3);
      }
      //locations.push_back(location);
      cout << location;
   //}
   return location;
    //locations = [ x.decode('base64') for x in rotlocations ]
    //randomlocation = choice(locations)

    // # choose name of log file from running processes
    // rpids = pids()
    // randompid = choice(rpids)
    // processcommand = Process(randompid)
    // processname = processcommand.name()
    // processname = path.basename(processname)
    // # remove any unwanted characters
    // processname = sub('[:()]', '', processname)
    // logfilename = randomlocation+"/"+processname
}

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
   //int actions = getActions();


   // TESTING only
   int actions = 0;
   string logfilename = getLogfilename();

   // add timestamp to cleanup log file
   // Current date/time based on current system
   time_t now = time(0);
   // Convert now to tm struct for local timezone
   tm* localtm = localtime(&now);
   string timestamp =  asctime(localtm);
   addCleanup(timestamp, cleanuplog);

   // add pid and process name to cleanup log file
   int processpid = getpid();
   ifstream comm("/proc/self/comm");
   string processname;
   getline(comm, processname);
   cout << processname << "\n";
   string processinfo = "\nProcess is running with pid: " + to_string(processpid) + "\nProcess is running with name: " + processname + "\n";
   addCleanup(processinfo, cleanuplog);

   //If no actions selected (0), just sleep so process is in ps list
   if ( actions == 0)
   {
      while (1 == 1)
      {
         time(0);
      }
   }

   message = "Testing\n";
   addCleanup(message, cleanuplog);

   return 0;
}