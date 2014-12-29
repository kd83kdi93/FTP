#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main(int argc , char * argv[])
{
	struct sockaddr_in clientaddr;
	int clientfd,port;
	socklen_t client_len;	
	char buf[255];
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
	printf("connecting\n");
	if(connect(clientfd,(struct sockaddr *)&clientaddr,client_len)<0)
	{
		printf("connect error %d\n",errno);
		return 0;
	}
	read(clientfd,buf,255);
	printf("%s\n",buf);
	close(clientfd);
	return 0;
}
