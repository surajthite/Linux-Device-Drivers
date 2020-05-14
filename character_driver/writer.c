/* AESD ASSIGNMENT 2
AUTHOR : SURAJ THITE
*/

//Header files for inclusion
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

//Main Function
int main(int agrc, char* argv[])
{
	openlog(NULL,0,LOG_USER);						//Open Logs for Logging purposes. Logs saved in \var\log\syslog file
	if (agrc < 3)								//Check if arguments are less than 2
	{
	  printf("Not all parameters passes, Please pass all the parameters \n");	//Write a print statement to the shell
	  syslog(LOG_ERR,"Invalid no of parameters passed");			//Log the error using syslog utility
	  return 1;								//Return Status
	}

	int fd;									//Variable to store the file desciptor
	const char *buf1 = "\ngithub username: surajthite\n";			//Constant string buffer to store github user name
	int nr;									//Varibale to store return status of write command

	time_t gettime;
	struct tm *temp;
	time( &gettime );
	temp = localtime( &gettime );
	const char *buf2 = asctime(temp);					//Pointer to the string returned by asctime function

	fd = open (argv[1], O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);	//Open a file in APPEND Mode and create it if not present in the directory
	if (fd < 0)
	{
      printf("ERROR \n");	//Print error if File cannot be opened
	  syslog(LOG_ERR,"File cannot be opened!");	//Log the error using syslog utility
	}

	syslog(LOG_DEBUG,"writing %s to %s file location",argv[2],argv[1]);	//Log the file writing action in LOG_DEBUG mode
	nr = write (fd,argv[2],strlen(argv[2]));				//Write the passed argument to the File
	if(nr!=strlen (argv[2]))                                //Check for error if return value is different than no of bytes to write
		syslog(LOG_ERR,"RETURN STATUS FOR WRITE1 %d \n",nr);	
	//syslog(LOG_DEBUG,"writing %s to %s file location",buf1,argv[1]);	//Log the file writing action in LOG_DEBUG mode
	nr = write (fd,buf1,strlen(buf1));				        //Write the github user name to the File
	if(nr!=strlen(buf1)) 							//Check for error if return value is different than no of bytes to write
		syslog(LOG_ERR,"RETURN STATUS FOR WRITE2 %d \n",nr);	
	//syslog(LOG_DEBUG,"writing %s to %s file location",buf2,argv[1]);	//Log the file writing action in LOG_DEBUG mode
	nr = write (fd,buf2,strlen(buf2));					//Write the date and time to the File
	if(nr!=strlen(buf2))							//Check for error if return value is different than no of bytes to write
		syslog(LOG_ERR,"RETURN STATUS FOR WRITE3 %d \n",nr);
	close(fd);								//Close the file
	closelog();								//Close the log	
	return 0;
}


