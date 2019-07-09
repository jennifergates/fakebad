#!/bin/bash

echo "Enter path to binary: "
read binary
binarypath=$(dirname $binary)
binaryname=$(basename $binary)


# if a link exists, delete it 
echo "Checking for existing links to that binary.... this might take a minute..."
linkedfiles=($(find / -samefile $binary 2> /dev/null))

echo $linkedfiles
echo ${#linkedfiles[@]} 

if [ ${#linkedfiles[@]} > 1 ]; then
	for i in "${linkedfiles[@]}"
	do
		if [ $i != $binary ]; then 
			echo "Deleting old link: $i" 
			rm -i $i
		fi
	done
fi

# clean up old log files if re-running
if [ -f /var/log/fakebad.log ]; then
	echo "Cleaning up any previous fake log files... please wait"
	cat /var/log/fakebad.log | xargs rm
	rm /var/log/fakebad.log
fi

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
echo "Suspect binary name: $disguise"

# pick a way to execute the binary, (0)link, (1)exec -a, or (2)fork?
#method=$(( $RANDOM % 3))
method=0

case $method in
    0) 
	# create a hard link to the binary named the disguised name.
	cd /tmp
        ln $binary $disguise
	# run it with current dir as PATH so just process name in ps list
	env PATH=$PATH:. $disguise &
	# remove the binary leaving it running in memory only
        rm $disguise
	;;
    1) 
	# run the binary in the background with a disguised name in the process list
	exec -a $disguise $binary &
	# change the timestamp on the binary to the time of the local ls command binary
	touch $binary -r $(which ls)
	;;
    2) 
	#fork
	;;
esac


echo "running"
