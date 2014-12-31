#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


int main(int argc , char * argv[])
{
	struct sockaddr_in clientaddr;
	int clientfd,port;
	socklen_t client_len;	
	char buf[255];
	char command[255];
	ssize_t len;
	if(argc<3)
	{
		printf("usage client <ip> <port>\n");
		return 0;
	}
	port = atoi(argv[2]);
	printf("port is %d\n",port);
	if((clientfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("create socket error %d\n",errno);
	}
	bzero(&clientaddr,sizeof(clientaddr));
	clientaddr.sin_family=AF_INET;
	clientaddr.sin_port=htons(port);
	inet_pton(AF_INET,argv[1],&clientaddr.sin_addr);
	client_len = sizeof(clientaddr);
	if(connect(clientfd,(struct sockaddr *)&clientaddr,client_len)<0)
	{
		printf("connect error %d\n",errno);
		return 0;
	}
	printf("connected\n");
	while(1)
	{
		/*	
		while(len=read(clientfd,buf,255))
		{
			printf("%s len is %d errno is %d\n",buf,len,errno);
			if(len<255)
			{
				break;
			}
		}
		 */
		while(len=read(clientfd,buf,255))
		{
			printf("%s len is %d errno is %d\n",buf,len,errno);
			if(len<0)
			{
				break;
			}
		}
		printf("insert you command:\n");
		fgets(command,255,stdin);
		write(clientfd,command,sizeof(command));
		if(command[sizeof(command)-1]=='\n')
		{
			command[sizeof(command)-1]='\0';
		}
		if(!strcmp(command,"remoteexit"))
		{
			
			close(clientfd);
			return 0;
		}
	}
	return 0;
}



