#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_BUFFER	1024

#define MULTI_CLIENT
//#define SINGLE_CLIENT

// Global Varialbla

int socket_creation(int port)
{
	int master_sfd;
	struct sockaddr_in server_add;

	/* Socket Creation  */
	master_sfd = socket(AF_INET,SOCK_STREAM,0);

	/* Binding Socket with Local Address */
	server_add.sin_family = AF_INET;						//bind
	server_add.sin_port   = htons(port);
	server_add.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(master_sfd,(struct sockaddr*)&server_add,sizeof(server_add));

	/* Listioning to socket */
	listen(master_sfd,1);
	return master_sfd;
}

#ifdef SINGLE_CLIENT
void *ServerThread(void *ptr)
{
	char client_data[MAX_BUFFER];
	int server_nsfd, master_sfd, client_len;
	struct sockaddr_in client_add;
	ssize_t ret;
	
	master_sfd = socket_creation(atoi(ptr));

	/* Accepting Clinet Connection */
	server_nsfd = accept( master_sfd , (struct sockaddr*)&client_add, &client_len);
	/* Reading data from socket and Writing to Rx-Fifo */
	while(1)
	{
		ret = recv(server_nsfd, client_data, MAX_BUFFER, MSG_DONTWAIT);
		if(ret > 0){
			printf("client data: %s\n",client_data);
			memset(client_data,0,sizeof(client_data));
		}
		usleep(1);
	}				
}
#endif

#ifdef MULTI_CLIENT
void *MultiClientServerThread(void *ptr)
{
	int opt = 1; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 		
	char buffer[1025]; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds; 
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
	//get a master socket 
	master_socket =  socket_creation(atoi(ptr));
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
	while(1) 
	{ 
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
				
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				pthread_exit(NULL); 
			} 
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n", 
				new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
						
					break; 
				} 
			} 
		} 
			
		//else its some IO operation on some other socket 
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message 
				if ((valread = read( sd , buffer, 1024)) == 0) 
				{ 
					//Somebody disconnected , get his details and print 
					getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n" , 
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
						
					//Close the socket and mark as 0 in list for reuse 
					close( sd ); 
					client_socket[i] = 0; 
				} 
					
				//Echo back the message that came in 
				else
				{ 
					//set the string terminating NULL byte on the end 
					//of the data read 
					buffer[valread] = '\0'; 
					//send(sd , buffer , strlen(buffer) , 0 ); 
					printf("%s\n",buffer);
				} 
			} 
		} 
	} 
}
#endif

int main(int argc, char *argv[])
{
	if(argc !=2)
	{
		printf("USAGE: ./server_bin PORT_NO\n");
		return -1;
	}

#ifdef SINGLE_CLIENT
	pthread_t ServerThreadID;
	pthread_create(&ServerThreadID, NULL, ServerThread, argv[1]);
	pthread_join(ServerThreadID,NULL);
#endif

#ifdef MULTI_CLIENT
	pthread_t MultiClientServerThreadID;
	pthread_create(&MultiClientServerThreadID, NULL, MultiClientServerThread, argv[1]);
	pthread_join(MultiClientServerThreadID,NULL);
#endif
	return 0;
}
