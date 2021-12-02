
/*****************************************************************************/
/*** tcpserver.c                                                           ***/
/***                                                                       ***/
/*** Demonstrate an TCP server.                                            ***/
/*****************************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <pthread.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

void panic(char *msg);
#define panic(m)	{perror(m); abort();}

/*The set of possible STATES*/
#define AFK				0
#define ONLINE			1

/*Time to check the state of the client*/
#define fiveseconds       		30

#define MAX_CLIENT_NUM			10

typedef struct client_socket_into client_socket_info_t;

struct client_socket_into
{
	int socket;
	int state;
	int index;
	char client_name[32];
}; 

typedef struct client_messages client_messages_t;

struct client_messages
{
	char message[256];
	char client_name[32];
}; 

client_socket_info_t socket_table[MAX_CLIENT_NUM];


static void sig_handler(int signal)
{
	client_messages_t messages;

	int i=0;

	if(signal ==SIGALRM)
	{
			strcpy(messages.client_name,"st");
			strcpy(messages.message, "state");
			
			for(i=0; i<MAX_CLIENT_NUM; i++)
			{
					if(socket_table[i].state)
						send(socket_table[i].socket,&messages,sizeof(messages),0);
			}

	}
}


/*******************************************************************************/
/*** The tv_sec member represents the elapsed time, in whole seconds.        ***/
/*** The tv_usec member captures rest of the elapsed time, represented as    ***/
/*** the number of microseconds.                                             ***/
/*** it_interval specifies the period between successive timer expirations.  ***/
/*** if it is zero the alarm will fire only once.							 ***/
/*** if it_value is not zero it indicates the time left to the next timer    ***/
/*** expiration and if it is zero it implies that timer is disabled.		 ***/
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


/*****************************************************************************/
/*** This program creates a simple echo server: whatever you send it, it   ***/
/*** echoes the message back.                                              ***/
/*****************************************************************************/
void *threadfuntion(void *arg)                    
{	
	client_messages_t messages;

	int i=0;

	client_socket_info_t *info = (client_socket_info_t *)arg;   /* get & convert the socket */
	
	while(1)
	{
		if( recv(info->socket,&messages.message,sizeof(messages.message),0))
		{
			strcpy(messages.client_name,socket_table[info->index].client_name);
			
			printf("%s said: %s\n", messages.client_name, messages.message);

			for(i=0; i<MAX_CLIENT_NUM; i++)
			{
					if(socket_table[i].state)
						send(socket_table[i].socket,&messages,sizeof(messages),0);
			}
		}	

	}

	shutdown(info->socket,SHUT_RD);
	shutdown(info->socket,SHUT_WR);
	shutdown(info->socket,SHUT_RDWR);

	//socket_table[info->index].state = 0;
	
	return 0;                           /* terminate the thread */
}

int main(int count, char *args[])
{	struct sockaddr_in addr;
	int listen_sd, port;
	int i=0;

	if ( count != 2 )
	{
		printf("usage: %s <protocol or portnum>\n", args[0]);
		exit(0);
	}

	/*---Get server's IP and standard service connection--*/
	if ( !isdigit(args[1][0]) )
	{
		struct servent *srv = getservbyname(args[1], "tcp");
		if ( srv == NULL )
			panic(args[1]);
		printf("%s: port=%d\n", srv->s_name, ntohs(srv->s_port));
		port = srv->s_port;
	}
	else
		port = htons(atoi(args[1]));

	/*--- create socket ---*/
	listen_sd = socket(PF_INET, SOCK_STREAM, 0);
	if ( listen_sd < 0 )
		panic("socket");

	/*--- bind port/address to socket ---*/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = INADDR_ANY;                   /* any interface */
	if ( bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
		panic("bind");

	/*--- make into listener with 10 slots ---*/
	if ( listen(listen_sd, MAX_CLIENT_NUM) != 0 )
		panic("listen")

	/*--- begin waiting for connections ---*/
	else
	{

		 /* process all incoming clients */

		while (1)                        
		{
						
			int n = sizeof(addr);

			int sd = accept(listen_sd, (struct sockaddr*)&addr, &n);     /* accept connection */

			if(sd!=-1 && i<10)
			{
				recv(sd,socket_table[i].client_name,sizeof(socket_table[i].client_name),0);

				pthread_t child;
				socket_table[i].socket=sd;
				socket_table[i].state=1;		/*means connection opened*/
				socket_table[i].index=i;
	
				printf("New connection\n");
				pthread_create(&child, 0, threadfuntion, &socket_table[i]);       /* start thread */
				i++;
				pthread_detach(child);                      /* don't track it */
			}
		}
	}
}
