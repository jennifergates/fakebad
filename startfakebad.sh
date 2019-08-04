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
##      LASTEDIT: 27 July 2019
##      GIT REPO: https://github.com/jennifergates/Misc/tree/master/fakebad
##      CHANGELOG:
##      1.0 (27 July 2019) - Initial Draft (in progress)
##
##########

# make sure this script is run as root
if (( $EUID != 0 )); then
    echo -e "Please run this script as root."
    exit
fi

echo -e "This script runs the fakebad binary in random ways in order to disguise the process similiar to malicious executables.\n\n"
echo -e "The fakebad.py script should be used to create the binary. An easy way to do this is with pyinstaller. \n\n"
echo -e "     EX:   # pyinstaller --onefile fakebad.py \n\n"

echo -e "Enter path to binary: "
read binary
binarypath=$(dirname $binary)
binaryname=$(basename $binary)
cleanuplog="/var/log/fakebad.log"
rotlocations=(".rgp.vavg/q" ".rgp.peba/q" ".rgp.vavg" ".hfe.fova" ".hfe.ova")


# check if binary is still running from previous run
echo -e "\n[] Checking if binary is already running... please wait"
if [ -f $cleanuplog ]; then
	if [ $(grep -c "pid" $cleanuplog) > 0 ]; then
		pid=$(cat $cleanuplog | grep -P "is running with pid: " | cut -d":" -f2 )
		running=$(ps -o cmd= $pid)
		name=$(cat $cleanuplog | grep -P "is running with name: " | cut -d":" -f2)

		if [ $running == $name ]; then
			echo -e "     $running is running now with the same pid and name from last run. \n     Kill $running $pid [y|n]? "
			read killold
			if [ $killold == "y" ]; then
				echo -e "     Killing $running $pid"
				kill $pid
			fi
		fi
	fi
fi

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

# clean up old copies of binary if copied on last run
echo -e "\n[] Cleaning up any previous copies of binary file... please wait"
if [ -f $cleanuplog ]; then
	binaries=$(grep -c "name:" $cleanuplog)
	if [ $binaries -ne "0" ]; then
		copied=$(cat $cleanuplog | grep -P "is running with name: " | cut -d':' -f2 )
		if [ -f $copied ]; then
			rm $copied
		fi
	fi
fi

# clean up old log files if re-running
echo -e "\n[] Cleaning up any previous fake log files... please wait"
if [ -f $cleanuplog ]; then
	logs=$(grep -c "log file" $cleanuplog )

	if [ $logs -ne "0" ]; then
		fakelog=$(cat $cleanuplog | grep -P "Fakebad process's fake log file: " | cut -d':' -f2)
		if [ -f $fakelog ]; then
			rm $fakelog
		fi
	fi
fi

if [ -f $cleanuplog ]; then
	echo -e "\n[] Removing previous $cleanuplog file."
	rm $cleanuplog
fi

echo -e "\n[] Creating a disguise name and starting process"
# get a disguise - random process name to use as the link name or the exec -a rename
#ps -eo comm
processes=($(ps -eo comm | sort -u ))
randomnum=$(echo $(( $RANDOM % ${#processes[@]} )))
disguise=${processes[$randomnum]}

# NEED to remove chars from name like /:[]
disguise=${disguise//:/}
disguise=${disguise//[/}
disguise=${disguise//]/}
disguise=${disguise////}

# pick a way to execute the binary, (0)hard link, (1)different hame, or (2)different location?
method=$(( $RANDOM % 3))

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
	# run the binary in the background with a disguised name in the process list
	exec -a $disguise $binary &
	# change the timestamp on the binary to the time of the local ls command binary
	touch $binary -r $(which ls)
	;;
    2) 
	# copy the file to a different location and run it from there
	# unobfuscate possible locations
        locations=$(echo $rotlocations | tr '[A-Za-z/.]' '[N-ZA-Mn-za-m./]')

	randomloc=$(echo $(( $RANDOM % ${#locations[@]} )))
	location=${locations[$randomloc]}
	newloc="$location/$disguise"
	cp $binary $newloc
	$newloc &
	touch $newloc -r $(which rm)
	;;
esac


echo -e "\n[] Fake bad process is now running. Find it."

