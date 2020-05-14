#!/bin/sh

#Writer Script for Assignment 1 
#Aurthor : Suraj Thite 

set -o errexit  # stop execution on error

if [ $# -lt 2 ]
then
	echo "ERROR:Parameter not Specified, Please specify both the parameters"
	exit 1
fi

WRITEFILE=$1 #full path to a file passed as argument on the terminal. 

WRITESTR=$2 #text string to be written at specified file. 

if ! touch $WRITEFILE    #Create  a new file in the directory specified in the command line  and throw error if file cannot be created and exit with status 1
then 
	echo "Failed to Create a File!"
	exit 1
fi
echo "$WRITESTR" > $WRITEFILE 	#clear the file and write the input string at line 1 of the file 

echo "Git username : surajthite" >> $WRITEFILE #Append the git user name to the next line of the file 	

DATE=$(date +"%m-%d-%Y")	#Store output returned by date utility in a variable in MM DD YY FORMAT

echo $DATE >> $WRITEFILE        #Append the date to the next line of the file  
