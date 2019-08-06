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
echo -e "This script executes the fakebad binary. It chooses from a few methods of execution in an attempt to blend in."
echo -e "The fakebad.py script should be used to create the binary. An easy way to do this is with pyinstaller. "
echo -e "     EX:   # pyinstaller --onefile fakebad.py \n"
echo -e "If you haven't compiled the fakebad.py into a binary on this system, quit the script now.\n Enter q to quit or c to continue [q|c]:"
read quitscript

if ! [[ -z $quitscript ]] && [[ $quitscript == "q" ]]; then
	echo -e "Quitting. Come back when you've got the binary!"
	exit
fi

# Configure some variables
echo -e "Enter path to binary: "
read binary
binarypath=$(dirname $binary)
binaryname=$(basename $binary)
cleanuplog="/var/log/fakebad.log"
# obfuscated locations for binary to run out of so you can read the script and not ruin the exercise 
rotlocations=(".rgp.vavg/q" ".rgp.peba/q" ".rgp.vavg" ".hfe.fova" ".hfe.ova")


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
if [ -f $cleanuplog ]; then

	# check if binary is still running from previous run
	echo -e "[] Checking if binary is already running... please wait"
	if [ $(grep -c "pid" $cleanuplog) > 0 ]; then
		pid=$(cat $cleanuplog | grep -P "is running with pid: " | cut -d":" -f2 )
		running=$(ps -o cmd= $pid)
		name=$(cat $cleanuplog | grep -P "is running with name: " | cut -d":" -f2)

		if [ $running == $name ]; then
			echo -e "     $running is running now with the same pid and name from last run. \n     Kill process $running with pid $pid [y|n]? "
			read killold
			if [ $killold == "y" ]; then
				echo -e "     Killing $running $pid"
				kill $pid
			fi
		fi
	fi

	# clean up old copies of binary if copied on last run
	echo -e "[] Cleaning up any previous copies of binary file... please wait"
	binaries=$(grep -c "name:" $cleanuplog)
	if [ $binaries -ne "0" ]; then
		copied=$(cat $cleanuplog | grep -P "is running with name: " | cut -d':' -f2 )
		if [ -f $copied ]; then
			rm $copied
		fi
	fi

	# clean up old fake log files if re-running
	echo -e "[] Cleaning up any previous fake log files... please wait"
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

echo -e "[] Creating a disguise name and starting process"
# get a disguise - random process name to use as the link name or the exec -a rename
processes=($(ps -eo comm | sort -u ))
randomnum=$(echo $(( $RANDOM % ${#processes[@]} )))
disguise=${processes[$randomnum]}

# NEED to remove chars from name like /:[]
disguise=${disguise//:/}
disguise=${disguise//[/}
disguise=${disguise//]/}
disguise=${disguise////}

# pick a way to execute the binary, (0)hard link, (1)different hame, or (2)different location?
method=$(( $RANDOM % 5))
#method=4
case $method in
    0) 
		# create a hard link to the binary named the disguised name.
		cd /tmp
        ln $binary $disguise
		# run it with current dir as PATH so just process name in ps list
		env PATH=. $disguise &
		# remove the binary leaving it running in memory only
		sleep 10
        rm $disguise
		;;
    1) 
		# create a hard link to the binary named the disguised name, change the owner to 
		# a different user, run it in the background.
		cd /tmp
        ln $binary $disguise
		# run it with current dir as PATH so just process name in ps list
		baduser=$(cat /etc/passwd | grep -e "/bin/*sh" | tail -n1 | cut -d":" -f1)
		chown $baduser:$baduser $disguise
		env PATH=. $disguise &
		;;
	2) 
		# run the binary in the background with a disguised name in the process list
		exec -a $disguise $binary &
		# change the timestamp on the binary to the time of the local ls command binary
		touch $binary -r $(which ls)
		;;
    3) 
		# copy the file to a different location and run it from there
		# unobfuscate possible locations
        locations=$(echo $rotlocations | tr '[A-Za-z/.]' '[N-ZA-Mn-za-m./]')
        # choose a location
		randomloc=$(echo $(( $RANDOM % ${#locations[@]} )))
		location=${locations[$randomloc]}
		newloc="$location/$disguise"
		cp $binary $newloc
		# execute the file
		$newloc &
		touch $newloc -r $(which rm)
		;;
	4) 		
		# copy the file to a different location, run it from there, then delete so it is just in memory
		# unobfuscate possible locations
        locations=$(echo $rotlocations | tr '[A-Za-z/.]' '[N-ZA-Mn-za-m./]')
        # choose a location
		randomloc=$(echo $(( $RANDOM % ${#locations[@]} )))
		location=${locations[$randomloc]}
		newloc="$location/$disguise"
		cp $binary $newloc
		# execute the file
		$newloc &
		# remove the binary leaving it running in memory only
		sleep 10
		rm $newloc
		;;
esac


echo -e "\nFake bad process is now running. Find it! \n"

