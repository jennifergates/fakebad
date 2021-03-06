#!/bin/bash
##########
##  .Description
##		This script is used to start the fakebad binary created from fakebad.py. It does some 
##  	cleanup from any previous runs and then starts the binary in ways similar to a malicious process.
##  .Example
##		sudo startfakebad.sh 
##  .Notes
##      NAME: startfakebad.sh
##      AUTHOR: Jennifer Gates
##      VERSION: 1.0
##      LASTEDIT: 6 Aug 2019
##      GIT REPO: https://github.com/jennifergates/Misc/tree/master/fakebad
##      CHANGELOG:
##      1.0 (27 July 2019) - Initial Draft (in progress)
##		2.0 (6 Aug 2019) - grouped cleanup if statements together, added more execution methods
##
##########

# make sure this script is run as root
if (( $EUID != 0 )); then
    echo -e "Please run this script as root."
    exit
fi

# make sure binary has been compiled
echo -e "\n\n\nThis script executes the fakebad binary. It chooses from a few methods of execution in an attempt to blend in."
echo -e "The fakebad script should be used to create the binary. "
echo -e "  If using the python script, an easy way to do this is with pyinstaller. "
echo -e "     EX:   # pyinstaller --onefile fakebad.py \n"
echo -e "  If using the c++ script, an easy way to do this is with g++."
echo -e "     EX:   # g++ fakebad.cpp -pthread -lpthread -o fakebad\n"
echo -e "If you haven't compiled the fakebad script into a binary on this system, quit the script now.\n Enter q to quit or enter to continue:"
read quitscript

if ! [[ -z $quitscript ]] && [[ $quitscript == "q" ]]; then
	echo -e "Quitting. Come back when you've got the binary!"
	exit
fi

# Configure some variables
echo -e "Enter the FULL PATH to binary: "
read binary
if ! [[ -f $binary ]]; then
	echo -e "Hey! $binary does not exist. Quitting!"
	exit
fi
binarypath=$(dirname $binary)
binaryname=$(basename $binary)
cleanuplog="/var/log/fakebad.log"
# obfuscated locations for binary to run out of so you can read the script and not ruin the exercise 
rotlocations=(".rgp.vavg/q" ".rgp.peba/q" ".rgp.vavg" ".hfe.fova" ".hfe.ova")
rotarguments=(" --frffvba" " -a -h" " --freivpr" " --pbas=.rgp.arg/pbas" " -b .gzc./FTSVLFOho3DtnTIlMFR=" " -p" " -h" " --hfre" " -yn" " --ybt" " -kh" " -4" " --pbas .rgp.bcg.ren/pbas")


# if more than one hard link to the binary already exists, delete it 
echo -e "\n[] Checking for existing links to that binary.... this might take a minute..."
linkedfiles=($(find / -samefile $binary 2> /dev/null))

if [ ${#linkedfiles[@]} > 1 ]; then
	for i in "${linkedfiles[@]}"
	do
		if [ $i != $binary ]; then 
			echo -e "Deleting additional hard link from previous run: $i" 
			rm -i $i
		fi
	done
fi

# if the cleanuplog exists, do some cleanup from previous runs
if [[ -f $cleanuplog ]]; then

	# check if binary is still running from previous run
	echo -e "[] Checking if binary is already running... please wait"
	if [ $(grep -c "pid" $cleanuplog) > 0 ]; then
		pid=$(cat $cleanuplog | grep -P "is running with pid: " | cut -d":" -f2 )
		running=$(ps -o cmd= $pid)
		if ! [[ -z $running ]]; then
			name=$(cat $cleanuplog | grep -P "is running with name: " | cut -d":" -f2)
			runbase=$(basename $running)
			if [ $runbase == $name ]; then
				echo -e "     $running is running now with the same pid and name from last run. \n     Kill process $running with pid $pid [y|n]? "
				read killold
				if [ $killold == "y" ]; then
					echo -e "     Killing $running $pid"
					kill $pid
				fi
			fi
		fi
	fi

	# clean up old copies of binary if copied on last run
	echo -e "[] Cleaning up any previous copies of binary file... "
	binaries=$(grep -c "name:" $cleanuplog)
	if [ $binaries -ne "0" ]; then
		copied=$(cat $cleanuplog | grep -P "is running with name: " | cut -d':' -f2 )
		if [ -f $copied ]; then
			rm $copied
		fi
	fi

	# clean up old fake log files if re-running
	echo -e "[] Cleaning up any previous fake log files... "
	logs=$(grep -c "log file" $cleanuplog )

	if [ $logs -ne "0" ]; then
		fakelog=$(cat $cleanuplog | grep -P "Fakebad process's fake log file: " | cut -d':' -f2)
		if [ -f $fakelog ]; then
			rm $fakelog
		fi
	fi

	echo -e "[] Removing previous $cleanuplog file."
	rm $cleanuplog
fi

echo -e "[] Creating a disguise name and starting process... please wait"
# get a disguise - random process name to use as the link name or the exec -a rename
processes=($(ps -eo comm | sort -u ))
randomnum=$(echo $(( $RANDOM % ${#processes[@]} )))
disguise=${processes[$randomnum]}

# NEED to remove chars from name like ()/:[]
disguise=${disguise//:/}
disguise=${disguise//[/}
disguise=${disguise//]/}
disguise=${disguise//(/}
disguise=${disguise//)/}
disguise=${disguise////}

# randomly pick which way to execute the binary
method=$(( $RANDOM % 6))
#method=4

# set last assigned pid to a lower number so pid assigned to process is not always as bottom of ps
randlastpid=$(( $RANDOM % 10240 ))
echo $randlastpid > /proc/sys/kernel/ns_last_pid; 

case $method in
    0) 
		# create a hard link to the binary named the disguised name.
		cd /tmp
        ln $binary $disguise
		# run it in its own session with tmp dir as PATH so just process name in ps list
		setsid bash -c "env PATH=/tmp $disguise &"
		# remove the binary leaving it running in memory only
		sleep 10
        rm $disguise
		;;
    1) 
		# create a hard link to the binary named the disguised name, change the owner to 
		# a different user, run it in the background.
        ln $binary /usr/bin/$disguise
		# run it with current dir as PATH so just process name in ps list
		baduser=$(cat /etc/passwd | grep -e "/bin/*sh" | tail -n1 | cut -d":" -f1)
		chown $baduser:$baduser /usr/bin/$disguise
		setsid bash -c "env PATH=/usr/bin $disguise &"
		;;
	2) 
		# run the binary in the background with a disguised name in the process list
		setsid bash -c "exec -a $disguise $binary &"
		# change the timestamp on the binary to the time of the local ls command binary
		touch $binary -r $(which ls)
		;;
    3) 
		# copy the file to a different location and run it from there
        # choose a location and unobfuscate it
		randomloc=$(echo $(( $RANDOM % ${#rotlocations[@]} )))   		
		rotlocation=${rotlocations[$randomloc]}
        location=$(echo $rotlocation | tr '[A-Za-z/.]' '[N-ZA-Mn-za-m./]')

		newloc="$location/$disguise"
		cp $binary $newloc
		# execute the file
		setsid bash -c "$newloc &"
		# change the timestamp on the binary to the time of the local rm command binary
		touch $newloc -r $(which rm)
		;;
	4) 		
		# copy the file to a different location, run it from there, then delete so it is just in memory
        # choose a location and unobfuscate it
		randomloc=$(echo $(( $RANDOM % ${#rotlocations[@]} )))   		
		rotlocation=${rotlocations[$randomloc]}
        location=$(echo $rotlocation | tr '[A-Za-z/.]' '[N-ZA-Mn-za-m./]')

		newloc="$location/$disguise"
		cp $binary $newloc
		# execute the file
		setsid bash -c "$newloc &"
		# remove the binary leaving it running in memory only
		sleep 10
		rm $newloc
		;;
	5) 		
		## copy the file to a different location, run it from there with fake arguments
		# unobfuscate possible fake arguments
        # choose fake arguments
		randomarg=$(echo $(( $RANDOM % ${#rotarguments[@]} )))
		rotargument=${rotarguments[$randomarg]}
        argument=$(echo $rotargument | tr '[A-Za-z/.]' '[N-ZA-Mn-za-m./]')

        # choose a location and unobfuscate it
		randomloc=$(echo $(( $RANDOM % ${#rotlocations[@]} )))   		
		rotlocation=${rotlocations[$randomloc]}
        location=$(echo $rotlocation | tr '[A-Za-z/.]' '[N-ZA-Mn-za-m./]')

		newloc="$location/$disguise"
		cp $binary $newloc
		# execute the file
		setsid bash -c "$newloc $argument &"
		# change the timestamp on the binary to the time of the local mv command binary
		touch $newloc -r $(which mv)
		;;
esac

# wait for binary to start up then reset the ns_last_pid to the highest pid in ps
sleep 10
randlastpid=$( ps -eo pid | tail -n1 )
echo $randlastpid > /proc/sys/kernel/ns_last_pid; sleep 10

echo -e "\nFake bad process is now running. Find it! \n"
