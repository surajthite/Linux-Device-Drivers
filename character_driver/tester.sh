#!/bin/sh
# Tester script for assignment 1 and assignment 2
# Author: Siddhant Jajoo

set -e
set -u

NUMFILES=10
WRITESTR=AESD_IS_AWESOME
WRITEDIR=/tmp/aesd-data

if [ $# -lt 2 ]
then
	echo "Using default value ${WRITESTR} for string to write"
	if [ $# -lt 1 ]
	then
		echo "Using default value ${NUMFILES} for number of files to write"
	else
		NUMFILES=$1
	fi	
else
	NUMFILES=$1
	WRITESTR=$2
fi

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

echo "Writing ${NUMFILES} files containing string ${WRITESTR} to ${WRITEDIR}"

rm -rf "${WRITEDIR}"
mkdir -p "$WRITEDIR"

#The WRITEDIR is in quotes because if the directory path consists of spaces, then variable substitution will consider it as multiple argument.
#The quotes signify that the entire string in WRITEDIR is a single string.
#This issue can also be resolved by using double square brackets i.e [[ ]] instead of using quotes.
if [ -d "$WRITEDIR" ]
then
	echo "$WRITEDIR created"
else
	exit 1
fi

# Please do not modify this line, for compatibility with upcoming assignments
# ----------------------------------------------------
# YOUR CODE BELOW: the example implementation only writes one file
# You need to modify it to write the number of files passed as the NUMFILES parameter
# And customize the written filename with numbers 1-10
for i in $(seq 1 $NUMFILES)
do
writer "$WRITEDIR/$WRITESTR${i}" "$WRITESTR"
done
# ----------------------------------------------------
# End of your code modifications.  Please don't modify this line
# For compatibility with upcoming assignments

# Invoke the finder shell script and save the output in OUTPUTSTRING
OUTPUTSTRING=$(finder.sh "$WRITEDIR" "$WRITESTR")
echo $OUTPUTSTRING > ~/assignment-4-result.txt 	#Save Output of Finder command
set +e
echo ${OUTPUTSTRING} | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
	echo "success"
	exit 0
else
	echo "failed: expected  ${MATCHSTR} in ${OUTPUTSTRING} but instead found"
	exit 1
fi
