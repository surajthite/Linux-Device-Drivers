/************************
filename : aesd-scokets.c
author: suraj thite 
date 2/24/2020
References :  https://beej.us/guide/bgnet/html/
*************************/
/**********************************************************************
Include Files 
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>
//#include <sys/queue.h>
#include "queue.h"
#include <time.h>

/**********************************************************************
#defines 
**********************************************************************/
#define USE_AESD_CHAR_DEVICE 1	//switch to suppot character driver

#define PORT "9000"  // the port users will be connecting to

#define MAXLENGTH 4096

#define BACKLOG 10   // how many pending connections queue will hold

#define DELAY 10




/*Global Variables*/
int sockfd;
int new_fd,fd;
bool term_flag  = false;
timer_t timer_id;
int n=0;
int m=0;
int sum=0;
bool daemon_flag=false;

/*Mutex for locking operation*/

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*Linked List Structure*/

typedef struct _connection
{
	SLIST_ENTRY (_connection) pointers;
	int sock_data;
	int thread_complete;
	pthread_t thread_id;

}connection_t;

SLIST_HEAD(connection_list,_connection) connection;

/*Function prototyping*/

void *get_in_addr(struct sockaddr*);

static void SIGINT_Handler(int x);

void* process(void*);

int timer_specs(void);

int start_timer(int);

void time_handler(union sigval val);


/**********************************************************************
Function name : static void SIGINT_Handler(int x)
Parameters : int x (signo value)
return type : void
Description : Singal Handler function to gracefully terminate the process
**********************************************************************/  	

static void SIGINT_Handler(int x)
{
	
	shutdown(sockfd, SHUT_RDWR);

	term_flag =true;

}

/**********************************************************************
Function name : void timer_handler (union sigval val)
Parameters : union sigval val
return type : void
Description : Timer handler function for sigval
**********************************************************************/
void time_handler(union sigval val)
{
    puts("\n10 seconds passed!");
    char outstr[200];
    char time_buff[200];
    time_t t;
    struct tm *tmp;
    t = time(NULL);
    tmp=localtime(&t);
    strftime(outstr, sizeof(outstr), "%a, %d %b %Y %T %z", tmp);   
    printf("timestamp:%s\n", outstr);
    snprintf(time_buff,sizeof(time_buff),"timestamp:%s\n", outstr);
    pthread_mutex_lock(&lock);
    write(fd,time_buff,strlen(time_buff)); //Write the data from buffer to the file
    pthread_mutex_unlock(&lock);

}

/**********************************************************************
Function name : int setup_timer()
Parameters : void
return type : 0
Description : Timer Setup function 
**********************************************************************/

int setup_timer()
{
    struct  sigevent sev;
    sev.sigev_notify = SIGEV_THREAD; 
    sev.sigev_notify_function = &time_handler; 
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &timer_id;

    if(timer_create(CLOCK_MONOTONIC, &sev, &timer_id) != 0)
    {
        printf("Error on creating timer\n");
    }  

    return 0;
}


int timer_start(int interval_s)
{
   struct itimerspec in;

    in.it_value.tv_sec = interval_s;
    in.it_value.tv_nsec = 0; 
    in.it_interval.tv_sec =interval_s;
    in.it_interval.tv_nsec = 0;
    
    //issue the periodic timer request here.
    if(timer_settime(timer_id, 0, &in, NULL) !=0)
    {
        printf("Error on settime function\n");
    }
    return 0;
}

/**********************************************************************
Function name : int stop_timer()
Parameters : null
return type : void
Description : This function deletes the timer associated with timer_id
**********************************************************************/
int stop_timer()
{
    timer_delete(timer_id);

    return 0;
}

/**********************************************************************
Function name : void *get_in_addr(struct sockaddr *sa)
Parameters : struct sockaddr *sa
return type : void
Description : This function returns the socket address 
**********************************************************************/

void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) 
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/**********************************************************************
Function name : main
Parameters : int argc, char* argv[]
return type :int
Description : This is the main function i.e. entry point of program.
**********************************************************************/ 

int main(int argc, char* argv[])
{
		
	printf("PID of parent Process:%d\n", (int)getpid()); 
#if (USE_AESD_CHAR_DEVICE ==0)
	setup_timer();
	timer_start(DELAY);
#endif

	SLIST_INIT(&connection);

#if (USE_AESD_CHAR_DEVICE ==0)
	remove("/var/tmp/aesdsocketdata"); 
#endif


if(signal(SIGINT, SIGINT_Handler) == SIG_ERR)     
{
    fprintf(stderr,"Cannot handle SIGINT!\n");
    return -1;
}
	if (signal(SIGTERM,SIGINT_Handler) == SIG_ERR)    
{
    fprintf(stderr,"Cannot handle SIGTERM!\n");
    exit (EXIT_FAILURE);
}

if(argc ==2)                                        
{
   if(strcmp(argv[1],"-d") == 0)
    {
        printf("DAeMoN Mod3!");
        daemon_flag =true;   
     }                      
}

	struct addrinfo hints, *servinfo, *p;

	struct sockaddr_storage their_addr; 
	int new_fd=0;
	
	int yes=1;

	char s[INET6_ADDRSTRLEN];
	
	socklen_t sin_size;

	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; 

	openlog(NULL, 0, LOG_USER);

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	
	for(p = servinfo; p != NULL; p = p->ai_next) {
	if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
	{
		perror("server: socket");
	return -1;
	}

	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
	{
	perror("setsockopt");
	exit(1);
	}

	
	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
	{
	close(sockfd);
	perror("server: bind");
	continue;
	}

	break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL) 
	{
	fprintf(stderr, "server: failed to bind\n");
	exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
	perror("listen");
	exit(1);
	}

	printf("server: waiting for connections...\n");


	while(1) 
	{  
		
		if(daemon_flag==1)
		{	
			daemon_flag=0;
			
			pid_t pid;

			/* create new process */
			pid = fork ();
			if (pid == -1)
			return -1;
			else if (pid != 0)	
			exit (EXIT_SUCCESS);
	
			/* create new session and process group */
			if (setsid () == -1)
			return -1;
			/* set the working directory to the root directory */
			if (chdir ("/") == -1)
			return -1;

			#if (USE_AESD_CHAR_DEVICE == 0)
				timer_specs();
				start_timer(10);
			#endif
		}

		if(term_flag ==true)
		{
			term_flag =false;
			break;
		}

		sin_size = sizeof their_addr;

		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		
		if (new_fd == -1) {
		perror("accept");
		}

		/*Get the IP address of Client*/
		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),
		s, sizeof s);
		printf("Accepted connection from %s\n", s);
		syslog(LOG_DEBUG,"Accepted connection from %s\n",s);


		connection_t* client_ptr = (connection_t*)malloc(sizeof(connection_t));

		connection_t* con;

		client_ptr->sock_data = new_fd;

		client_ptr->thread_complete = 0;

		SLIST_INSERT_HEAD(&connection, client_ptr, pointers);

		/*Creating Threads*/

		pthread_t thread_id;

		pthread_create(&thread_id, NULL, process, (void*)client_ptr);
		
		printf("New thread id=%ld\n",thread_id);

		client_ptr->thread_id=thread_id;

		connection_t *temp;

		int error;

		/*Traversing linked list to remove completed thread*/

		SLIST_FOREACH_SAFE(con, &connection, pointers,temp)
		{	

			if(con->thread_complete==true)
			{
				 printf("\n Node is removed");

				error=pthread_join(con->thread_id, NULL);

				if(error!=0)
				{
					printf("Error in joining pthread= %d\n", error);
				}
				else
				{
					printf("success in joining pthread.\n");
				}
				
				SLIST_REMOVE(&connection,con,_connection,pointers);
	
			}
		}

		
	}

	syslog(LOG_DEBUG,"Caught signal,Exiting"); //Print on syslog on catching the signal

	close(sockfd);  //Close the socket on termination

	printf("\nRemoving aesdsocketdata\n"); 

#if USE_AESD_CHAR_DEVICE
	remove("/dev/aesdchar");
#else
	remove("/var/tmp/aesdsocketdata");
	stop_timer();
	free(timer);
#endif	

	closelog();

	return 0;

}

void* process(void * pointer)
{	

	printf("Thread id = %ld\n", pthread_self()); //get current thread id
	int nr1=0,nr2=0,nr3=0;
	
	connection_t* conn=NULL;

	conn=(connection_t *)pointer;
	
	char* recv_buff  = NULL;

	recv_buff =(char*)malloc(MAXLENGTH*sizeof(char));

	memset(recv_buff ,0,MAXLENGTH);

	n = recv(conn->sock_data, recv_buff , MAXLENGTH,0);

	sum += n;

	printf("total number of bytes received = %d\n", sum);

	char *nwline_char =strchr(recv_buff ,'\n');
	
	if(!nwline_char)
	{
		printf("No. of bytes received = %d\n", n);	

		recv_buff  = (char*)realloc(recv_buff , 75*MAXLENGTH*sizeof(char));
		
		m=recv(conn->sock_data, recv_buff +n,75*MAXLENGTH,0);
	}

	size_t count = strlen(recv_buff );
	

#if USE_AESD_CHAR_DEVICE

	fd = open("/dev/aesdchar", O_RDWR | O_CREAT | O_APPEND, 0755);

#else

	fd = open("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_APPEND, 0755);

#endif


	/*Writing "String" parameter to the created file*/
#if USE_AESD_CHAR_DEVICE

	nr1=write(fd,recv_buff,count);

#else

	pthread_mutex_lock(&mutex_lock);
	nr1=write(fd, recv_buff,count);
	pthread_mutex_unlock(&mutex_lock);

#endif	

	if(nr1 == -1)
	{
  		syslog(LOG_ERR,"\nError occured while writing file\n");

  		return NULL;
	}

#if USE_AESD_CHAR_DEVICE
	
int off_set;
    do
    {
        char tx_buff;
        off_set = read(fd,&tx_buff,1);
        send(conn->sock_data, &tx_buff, off_set, 0);
             
    } while (off_set != 0);
	

#else
	
	off_t file_size = lseek(fd, 0l, SEEK_END);

	char* tx_buff=NULL;

	tx_buff = (char*)malloc(file_size);

	lseek(fd,0l,SEEK_SET);

	nr2=read(fd,tx_buff,file_size);

	send(conn->sock_data, &tx_buff, file_size, 0);

#endif	


	if(nr2 == -1)
	{
  		syslog(LOG_ERR,"\nError while reading file\n");

  		return NULL;
	}


	if(nr3 == -1)
	{
  		syslog(LOG_ERR,"\nError while sending\n");
  		printf("error in sending back\n");
  		return NULL;
	}

		if (n<=0)
	{
    	syslog(LOG_DEBUG,"Closed connection from\n");
    	perror("send");
    	exit(0);
	}

		printf("thread exit\n");

		free(recv_buff );

		close(conn->sock_data);

		conn->thread_complete = true;

		return 0;
}
