#include"header.h"
// SERVER socket() -> connect() -> read() -> wirte() -> close() 

int main(int argc,char **argv)
{
int i;
if(argc!=3)
{
	printf("Usage: ./c ipaddr portno\n");
	return;
}

char cmds[1024];
struct sockaddr_in v;
int sfd,len,ret;
////////////////////////////////////////////////////	SOCKET CREATION
/* 1 */	sfd =socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		perror("socket");
		return;
	}
	perror("socket");
	printf("sfd = %d \n",sfd);
////////////////////////////////////////////////////    CONNECT
/* 2 */
	v.sin_family = AF_INET;
        v.sin_port   = htons(atoi(argv[2]));
	v.sin_addr.s_addr = inet_addr(argv[1]);
	len= sizeof(v);

	ret = connect(sfd,(struct sockaddr *)&v,len);
	if(ret<0)
	{
		perror("connect");
		return;
	}
	perror("connect");
	printf("enter the data\n");
	for(i=0;i<10;i++)
	{
		scanf("%s",cmds);
		write(sfd,cmds,strlen(cmds));
	}
}
