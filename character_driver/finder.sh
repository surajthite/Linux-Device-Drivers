#!/bin/sh

#Finder script for Assignment 1 
#Aurthor : Suraj Thite 
 
set -o errexit  # stop execution on error
if [ $# -lt 2 ]
then
	echo "ERROR:Parameter not Specified, Please specify both the parameters"
	exit 1
fi

FILESDIR=$1 # retrieve input parameter and populate a variable

SEARCHSTR=$2 #text string to be searched within these files

#echo $FILESDIR

#echo $SEARCHSTR

if [ ! -d "$FILESDIR" ]
then
  echo " Directory doesnt exist" 
  exit 1
fi

FILE_COUNT=`find $FILESDIR -type f -print | wc -l`  #find the number of files within specified subdirectory and store tha value in temp variable

#echo "There are $FILE_COUNT files in the specified directory."

#grep -r -c $SEARCHSTR $FILESDIR

WORD_COUNT=`grep -r -o -i   $SEARCHSTR $FILESDIR | wc -l`  #count the number of occurences of string in the directory specified as argument and store it in temp variable

echo "The number of files are  $FILE_COUNT and the number of matching lines are $WORD_COUNT."


