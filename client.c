
/*****************************************************************************/
/*** tcpclient.c                                                           ***/
/***                                                                       ***/
/*** Demonstrate an TCP client.                                            ***/
/*****************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <resolv.h>
#include <signal.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

/*The set of possible STATES*/
#define AFK				0
#define ONLINE			1

/*Time to check if it has not sent any new message for more than a minute*/
#define oneminut       10


typedef struct client_messages client_messages_t;

struct client_messages
{
	char message[256];
	char client_name[32];
}; 


typedef struct client_data client_data_t;

struct client_data
{
	int state;
	int socket;
}; 

client_data_t client;

char name[32];

//for the device driver
	char ledOn = '1', ledOff = '0';
	int fd0;

void panic(char *msg);
#define panic(m)	{perror(m); abort();}

static void sig_handler(int signal)
{
		if(signal == SIGALRM)
		{
			client.state = AFK;
				
		}
}

/*******************************************************************************/
/*** The tv_sec member represents the elapsed time, in whole seconds.        ***/
/*** The tv_usec member captures rest of the elapsed time, represented as    ***/
/*** the number of microseconds.                                             ***/
/*** it_interval specifies the period between successive timer expirations.  ***/
/*** if it is zero the alarm will fire only once.				***/
/*** if it_value is not zero it indicates the time left to the next timer    ***/
/*** expiration and if it is zero it implies that timer is disabled.		***/
/*******************************************************************************/

void timer(int sec, int usec)
{
		struct itimerval itv;

		signal(SIGALRM,sig_handler); //sets sig_handler to handle the signal

		itv.it_interval.tv_sec = sec;
		itv.it_interval.tv_usec = usec;

		itv.it_value.tv_sec = sec;
		itv.it_value.tv_usec = usec;

		setitimer (ITIMER_REAL, &itv, NULL);

}

void *threadreceive(void *arg)                    
{	
	int sd = *(int*)arg;  

	client_messages_t messages;

	while(1)
	{
		if (recv(sd,&messages,sizeof(messages),0))
			printf("%s said: %s\n",messages.client_name, messages.message);
			
	}
	
	shutdown(sd,SHUT_RD);
	shutdown(sd,SHUT_WR);
	shutdown(sd,SHUT_RDWR);
	
	return 0;                           /* terminate the thread */
}

void *threadsend(void *arg)                    
{	
	int sd = *(int*)arg;

	client_messages_t messages;

	while(1)
	{
		scanf("%s",messages.message);
		
		if(strcmp(messages.message,"close")==0)
		{
			write(fd0, &ledOff, 1);
			sleep(5);
			close(fd0);
			system("rmmod led");
			shutdown(sd,SHUT_RD);
			shutdown(sd,SHUT_WR);
			shutdown(sd,SHUT_RDWR);
		
		}
		
		else
		{
			send(sd,&messages.message,sizeof(messages.message),0);
		
			//timer(oneminut,0);
		
		}

	}
	
	shutdown(sd,SHUT_RD);
	shutdown(sd,SHUT_WR);
	shutdown(sd,SHUT_RDWR);

	return 0;                           /* terminate the thread */
}

/****************************************************************************/
/*** This program opens a connection to a server using either a port or a ***/
/*** service.  Once open, it sends the message from the command line.     ***/
/*** some protocols (like HTTP) require a couple newlines at the end of   ***/
/*** the message.                                                         ***/
/*** Compile and try 'tcpclient lwn.net http "GET / HTTP/1.0" '.          ***/
/****************************************************************************/
int main(int count, char *args[])
{	struct hostent* host;
	struct sockaddr_in addr;
	int sd, port;
	

	if ( count != 3 )
	{
		printf("usage: %s <servername> <protocol or portnum>\n", args[0]);
		exit(0);
	}

	/*---Get server's IP and standard service connection--*/
	host = gethostbyname(args[1]);
	//printf("Server %s has IP address = %s\n", args[1],inet_ntoa(*(long*)host->h_addr_list[0]));
	if ( !isdigit(args[2][0]) )
	{
		struct servent *srv = getservbyname(args[2], "tcp");
		if ( srv == NULL )
			panic(args[2]);
		printf("%s: port=%d\n", srv->s_name, ntohs(srv->s_port));
		port = srv->s_port;
	}
	else
		port = htons(atoi(args[2]));

	/*---Create socket and connect to server---*/
	client.socket = socket(PF_INET, SOCK_STREAM, 0);        /* create socket */
	if ( sd < 0 )
		panic("socket");
	memset(&addr, 0, sizeof(addr));       /* create & zero struct */
	addr.sin_family = AF_INET;        /* select internet protocol */
	addr.sin_port = port;                       /* set the port # */
	addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);  /* set the addr */

	/*---If connection successful, send the message and read results---*/
	if ( connect(client.socket, (struct sockaddr*)&addr, sizeof(addr)) == 0)
	{
		//device driver
		printf("\n\nInserting Device Driver...\n");
		
   		system("insmod led.ko");
   		
   		printf("\nCheck devicer driver:\n");
   		
  		system("lsmod");
  		
  		printf("\nIs the device driver in /dev:\n");
  		
    		system("ls -l /dev/led0");
    		
    		fd0 = open("/dev/led0", O_WRONLY);
    		
   		system("echo none >/sys/class/leds/led0/trigger");
   		
   		write(fd0, &ledOn, 1);
   		
   		
   		//for the TCP/IP
   		
		printf("Write your name: ");

		scanf("%s",name);

		send(client.socket,name,sizeof(name),0);
		
		client.state = ONLINE;

		pthread_t send ,receive;

		   	pthread_create(&send, 0, threadsend, &client.socket);    /* start thread 1*/

			pthread_create(&receive, 0, threadreceive, &client.socket); 
			
			//timer(oneminut,0);

			pthread_join(receive,NULL); 
	}
		

	else
		panic("connect");
}
