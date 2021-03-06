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
##
##      It should be started using the accompanying startfakebad.sh script. 
##
##  .Notes
##      NAME: fakebad.cpp
##      AUTHOR: Jennifer Gates
##      VERSION: 1.8
##      LASTEDIT: 23 AUG 2019
##      GIT REPO: https://github.com/jennifergates/Misc/tree/master/fakebad
##      CHANGELOG:
##      1.0 (10 AUG 2019) - Initial Draft (in progress)
##      1.5 (19 AUG 2019) - TCP network listener added
##      1.8 (23 AUG 2019) - TCP and UDP network listeners combined
##
########### */

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <time.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <vector>
#include <dirent.h>
#include <map>
#include <errno.h> 
#include <arpa/inet.h>
#include <thread>

using namespace std;
#define TRUE 1 
#define FALSE 0 

//Functions Declared
int AddCleanup(string, string);
string GetLogFilename();
string StartFakeLog(string);
int GetActions();
int StartNETListeners(int, int, string);
int GetPort();
struct timespec sleeptimerinseconds, nonanosec;


string GetLogFilename()
{
   // choose path for log file from list of obfuscated locations
   vector <string> rotlocations = {",bq`,fkfq+a", ",bq`,`olk+a", ",bq`,fkfq", ",rpo,p_fk", ",rpo,_fk"}; 
   srand (time(NULL));
   int range = rotlocations.size(); 
   int choice = rand() % range;   

   // unobfuscate location
   string location;
   for (char rotletter : rotlocations[choice])
   {
      location.push_back(rotletter + 3);
   }

   // choose name of log file from running processes
   // read all numeric pid dirs from /proc for comm file contents (processname) into rpids vector
   DIR *proc_dp = NULL;
   struct dirent *proc_dptr = NULL;
   vector <string> rpids;

   proc_dp = opendir("/proc/");
   while (NULL != (proc_dptr = readdir(proc_dp) ))
   {
      if ( proc_dptr->d_name[0] > 48 && proc_dptr->d_name[0] < 58)
      {
         string piddir = "/proc/" + (string)proc_dptr->d_name + "/comm";
         ifstream pidcomm;
         pidcomm.open(piddir);
         string pname;
         while (pidcomm >> pname)
         {
            rpids.push_back(pname);
         }
         pidcomm.close();
      }
   }
   closedir(proc_dp);

   // choose random process to copy name as a disguise
   srand (time(NULL));
   int prange = rpids.size();
   int pchoice = rand() % prange;
   //cout << rpids[pchoice] << "\n";

   // # remove any unwanted characters [:()]
   string filename = rpids[pchoice];
   if (filename.find("[") < filename.size()) { filename.replace(filename.find("["), 1, "");}
   if (filename.find("]") < filename.size()) { filename.replace(filename.find("]"), 1, "");}
   if (filename.find(":") < filename.size()) { filename.replace(filename.find(":"), 1, "");}
   if (filename.find("(") < filename.size()) { filename.replace(filename.find("("), 1, "");}
   if (filename.find(")") < filename.size()) { filename.replace(filename.find(")"), 1, "");}

   
   //put full path together
   string logfilename = location + "/" + filename + ".log";
   return logfilename;

}

int AddCleanup(string message, string cleanuplog)
{
   // write given message to cleanup log
   //cout << message;
   ofstream cleanup;
   cleanup.open(cleanuplog, ofstream::app);
   cleanup << message;
   cleanup.close();

   return 0;

}

string StartFakeLog(string cleanuplog)
{
   // create a fake log file to keep open and log to
   string logfile;
   ifstream f;
   int filetest;
   do
   {
      logfile = GetLogFilename();
      ifstream f(logfile);
      filetest = f.good();
      f.close();
   } while (filetest == 1); //if file exists, get a different name

   // add log file name to cleanup log
   AddCleanup("Fakebad process's fake log file: "+logfile+"\n", cleanuplog);
   return logfile;
}

int GetActions()
{
   srand (time(NULL));
   int range = 4 - 0 + 1; //0-4 inclusive
   int actions = rand() % range;
   return actions;
}

int GetPort()
{
   srand (time(NULL));
   int prange = 65535;
   int port = rand() % prange;
   return port; 
}

int StartNETListeners(int TCPport, int UDPport, string cleanuplog)
{ 
   //code based on GeeksforGeeks.org tutorial "Socket Programming"
   int opt = TRUE; 
   int listenSocket, addrlen, newSocket, connectingSocket[30], maxConnections = 30, activity, i, totalInput, sd; 
   int max_sd; 
   int sockfd;
   char buffer[1024]; //udp date input
   struct sockaddr_in address, servaddr, cliaddr; 

      
   char dataInput[9]; //tcp data dataInput of 8 bytes plus string terminator \0      
   //set of socket descriptors 
   fd_set readfds; 
      
   //Command and Control Messages for connections
   const char *C2message1 = "#574 Enter Node Number: \r\n"; 
   const char *C2message2 = ", wait for further commands. \r\n"; 
   const char *C2message3 = ", proceed with your mission. \r\n"; 
   const char *C2message4 = "U3VwZXIgU2VjcmV0IGJhZCBzdHVmZi4= \r\n"; 

   //initialize all in connectingSocket[] to 0 so not checked 
   for (i = 0; i < maxConnections; i++) 
   { 
      connectingSocket[i] = 0; 
   } 
    
   ////////// TCP Socket Setup   
   //create a listening tcp socket 
   if( (listenSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
   { 
      perror("Failed to create socket. Exiting."); 
      exit(EXIT_FAILURE); 
   } 
   
   //set tcp listening socket to allow multiple connections , 
   if( setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
      sizeof(opt)) < 0 ) 
   { 
      perror("Failed to setsockopt for multiple TCP connections. Exiting."); 
      exit(EXIT_FAILURE); 
   } 
   
   //type of tcp socket created 
   address.sin_family = AF_INET; 
   address.sin_addr.s_addr = INADDR_ANY; 
   address.sin_port = htons( TCPport ); 
      
   //bind the socket to localhost port  
   if (bind(listenSocket, (struct sockaddr *)&address, sizeof(address))<0) 
   while (bind(listenSocket, (struct sockaddr *)&address, sizeof(address))<0) 
   { 
      perror("Failed to bind to localhost. Exiting."); 
      address.sin_port = htons(GetPort());
      //exit(EXIT_FAILURE); 
   } 
      
   //try to specify maximum of 3 pending connections for the master socket 
   if (listen(listenSocket, 3) < 0) 
   { 
      perror("Failed to start listener. Exiting."); 
      exit(EXIT_FAILURE); 
   } 
      
   //accept the incoming connection 
   addrlen = sizeof(address); 


   ////////// UPP Socket Setup   
   // Creating UDP socket file descriptor
   if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   {
      perror("socket creation failed");
      exit(EXIT_FAILURE);
   }
   if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
   {
      perror("Failed to setsockopt for multiple IDP connections. Exiting.");
      exit(EXIT_FAILURE);
   }

   memset(&servaddr, 0, sizeof(servaddr));
   memset(&cliaddr, 0, sizeof(cliaddr));

   //filling UDP server information
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = INADDR_ANY;
   servaddr.sin_port = htons(UDPport);

   //bind the socket with the server address
   if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
   {
      perror("bind failed");
      exit(EXIT_FAILURE);
   }
   
   ////////// Update Cleanup log   
   // Log network connections in clean up log
   string portString = "Fakebad process's network connection: TCP " + to_string(TCPport) + "\nFakebad process's network connection: UDP " + to_string(UDPport) + "\n";
   AddCleanup(portString, cleanuplog);

   
   ////////// Use Select to monitor network connections
   int n;
   socklen_t len;

   while(TRUE) 
   { 

      //clear the socket set 
      FD_ZERO(&readfds); 
   
      //add master socket to set 
      FD_SET(listenSocket, &readfds); 
      max_sd = listenSocket; 

      FD_SET(sockfd, &readfds);
      if (sockfd > listenSocket)
      {
         max_sd = sockfd;
      }
         
      //add child sockets to set 
      for ( i = 0 ; i < maxConnections ; i++) 
      { 
         //socket descriptor 
         sd = connectingSocket[i]; 
            
         //if valid socket descriptor then add to read list 
         if(sd > 0) 
            FD_SET( sd , &readfds); 
            
         //highest file descriptor number, need it for the select function 
         if(sd > max_sd) 
            max_sd = sd; 
      } 
   
      //wait indefinitely for connection on one of the sockets 
      activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
   
      if ((activity < 0) && (errno!=EINTR)) 
      { 
         printf("Error selecting socket descriptor."); 
      } 
         
      //check master socket for incoming TCP connection 
      if (FD_ISSET(listenSocket, &readfds)) 
      { 
         if ((newSocket = accept(listenSocket, 
               (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
         { 
            perror("Failed to accept incoming connection. Exiting."); 
            exit(EXIT_FAILURE); 
         } 
         
         //send request for node number. 
         if( send(newSocket, C2message1, strlen(C2message1), 0) != strlen(C2message1) ) 
         { 
            perror("Failed to send. Exiting."); 
         } 
            
         //add new socket to array of sockets 
         for (i = 0; i < maxConnections; i++) 
         { 
            //if position is empty 
            if( connectingSocket[i] == 0 ) 
            { 
               connectingSocket[i] = newSocket; 
               break; 
            } 
         } 
      } 

      // check master socket for incoming UDP packet and respond if data starts with 42 
      if (FD_ISSET(sockfd, &readfds))
      {
         n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
         buffer[n] = '\0';
         if (buffer[0] == '4' and buffer[1] == '2')
         {
            sendto(sockfd, (const char *)C2message4, strlen(C2message4), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
         }

      }  


      //handle additional input on other socket 
      for (i = 0; i < maxConnections; i++) 
      { 
         sd = connectingSocket[i]; 
            
         if (FD_ISSET( sd , &readfds)) 
         { 
            //Check if connection was closed and read the incoming message 
            if ((totalInput = read( sd , dataInput, 8)) == 0) 
            { 
               //get peer info
               //getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen); 
                  
               //Close the socket and mark as 0 in list for reuse 
               close( sd ); 
               connectingSocket[i] = 0; 
            } 
               
            //respond with C2 instructions, different message if node entered is 42. 
            else
            { 
               //set the string terminating NULL byte on the end of the data read 
               dataInput[totalInput] = '\0';
               send(sd , dataInput, strlen(dataInput)-1, 0 );
               if ( dataInput[1] == '2' and dataInput[0] == '4')
               {
                  send(sd, C2message3, strlen(C2message3) , 0 );
               } 
               else
               {
                  send(sd , C2message2 , strlen(C2message2) , 0 ); 
               }
               //Close the socket and mark as 0 in list for reuse 
               close( sd ); 
               connectingSocket[i] = 0; 
            } 
         } 
      } 
   } 
      
}


int main() 
{
   //variables
   string cleanuplog = "/var/log/fakebad.log";

   //randomly select process's actions
   //int actions = GetActions();
   // used TESTING specific actions only, uncomment above for final version
   int actions = 3;

   // add timestamp to cleanup log file
   // Current date/time based on current system
   time_t now = time(0);
   // Convert now to tm struct for local timezone
   tm* localtm = localtime(&now);
   string timestamp =  asctime(localtm);
   AddCleanup(timestamp, cleanuplog);

   // add pid and process name to cleanup log file
   int processpid = getpid();
   ifstream comm("/proc/self/comm");
   string processname;
   getline(comm, processname);

   if ( processname == "fakebad")
   {
      ifstream cmdline("/proc/self/cmdline");
      getline(cmdline, processname);
      processname.pop_back();
   }

   string processinfo = "Process is running with pid: " + to_string(processpid) + "\nProcess is running with name: " + processname + "\n";
   AddCleanup(processinfo, cleanuplog);

   //If no actions selected (0), just sleep so process is in ps list
   if ( actions == 0)
   {
      while (1 == 1)
      {
         time(0);
      }
   }

   //If log action selected (1), periodically write to a file so process is in ps list and handle to file is open
   if ( actions == 1)
   {
      string logfile = StartFakeLog(cleanuplog);
      const char * logfilechar = logfile.c_str();
      FILE * pfile;
      pfile = fopen(logfilechar, "a");

      while (1 == 1)
      {
         fputs("Evil log file entry.\n", pfile);
         fflush(pfile);
         sleeptimerinseconds.tv_sec = 60;
         sleeptimerinseconds.tv_nsec = 0;
         nanosleep(&sleeptimerinseconds, &nonanosec);
      }
   }

   //If network action selected (2), open network connections. if logging also selected (3) also log 
   if ( actions == 2 || actions == 3)
   {


      int TCPlistenPort = GetPort();
      sleeptimerinseconds.tv_sec = 1;
      sleeptimerinseconds.tv_nsec = 0;
      nanosleep(&sleeptimerinseconds, &nonanosec);
      int UDPlistenPort = GetPort();
      thread thread_object(StartNETListeners, TCPlistenPort, UDPlistenPort, cleanuplog);
      thread_object.detach();

      // if also logging, open log file
      if (actions == 3)
      {
         string logfile = StartFakeLog(cleanuplog);
         const char * logfilechar = logfile.c_str();
         FILE * pfile;
         pfile = fopen(logfilechar, "a");

         while (1 == 1)
         {
            fputs("Evil log file entry.\n", pfile);
            fflush(pfile);
            sleeptimerinseconds.tv_sec = 60;
            sleeptimerinseconds.tv_nsec = 0;
            nanosleep(&sleeptimerinseconds, &nonanosec);
         }
      }

   }

   return 0;
}
