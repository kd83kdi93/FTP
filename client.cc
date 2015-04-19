#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
int main(int argc , char * argv[])
{
	struct sockaddr_in clientaddr;
	struct epoll_event env , events[4];
	int clientfd,port,epfd;
	int stdno=fileno(stdin);
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
	epfd = epoll_create(5);
	env.data.fd = clientfd;
	env.events = EPOLLIN;
	epoll_ctl(epfd , EPOLL_CTL_ADD , clientfd , &env);
	env.data.fd = stdno;
	env.events = EPOLLIN;
	epoll_ctl(epfd , EPOLL_CTL_ADD , stdno , &env);
	while(true)
	{
		int ready = epoll_wait(epfd ,(struct epoll_event *)&events , 4 ,-1);
		for(int i =0 ;i<ready ; i++)
		{
			if(events[i].data.fd == stdno)
			{
				printf("you: ");
				scanf("%s",buf);
				write(clientfd , buf ,255);
				bzero(buf,255);
			}
			else if((events[i].data.fd == clientfd)&&(events[i].events & EPOLLIN))
			{
				int tmp = read(clientfd,buf,255);
				printf("others: %s\n",buf);
				bzero(buf,255);
			}
		}
	}
	env.data.fd = clientfd;
	epoll_ctl(epfd , EPOLL_CTL_DEL , clientfd , &env);
	env.data.fd = stdno;
	epoll_ctl(epfd , EPOLL_CTL_DEL , stdno , &env);
	close(epfd);
	close(clientfd);
	return 0;
}
