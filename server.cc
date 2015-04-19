#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/epoll.h>



int main(int argc , char * argv[])
{

	struct sockaddr_in listensock , connectsock;
	struct epoll_event env , events[20];
	int port , listenfd , connectfd , epfd , ready_read;
	char chat[256];
	socklen_t connectsock_size; 
	pid_t childpid;
	if(argc<2)
	{
		printf("usage chat <port>\n");
		return 0;
	}
	port = atoi(argv[argc-1]);
	printf("server port is %d\n",port);
	if((listenfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("listen socket errno %d\n",errno);
		return 0;

	}
	bzero(&listensock,sizeof(listensock));
	listensock.sin_family = AF_INET;
	listensock.sin_addr.s_addr=htonl(INADDR_ANY);
	listensock.sin_port=htons(port);
	if(bind(listenfd,(struct sockaddr *)&listensock,sizeof(listensock))<0)
	{
		printf("bind errno %d\n",errno);
	}
	if(listen(listenfd,8)<0)
	{
		printf("listen error %d\n",errno);
	}
	epfd = epoll_create(256);
	env.data.fd = listenfd;
	env.events = EPOLLIN;
	epoll_ctl(epfd , EPOLL_CTL_ADD , listenfd , &env);
	while(1)
	{
		
		ready_read  = epoll_wait(epfd , (epoll_event *)&events , 20 , -1);
		for(int i=0 ; i<ready_read ; i++)
		{
			if(events[i].data.fd == listenfd)
			{
				connectsock_size = sizeof(connectsock);
				if((connectfd = accept(listenfd,(struct sockaddr *)(&connectsock),&connectsock_size))<0)			
				{
					if(errno == EINTR)
					{
						break;
					}else
					{
						printf("accept error %d\n",errno);
						return 0;
					}
				}
				printf("one user\n");
				env.data.fd = connectfd;
				env.events = EPOLLIN | EPOLLOUT;
				epoll_ctl(epfd , EPOLL_CTL_ADD , connectfd , &env);

			}else if(events[i].events & EPOLLIN)
			{
				int tmp = read(events[i].data.fd , chat , 256);		
				if(tmp <= 0 )
				{
					env.data.fd = events[i].data.fd;
					epoll_ctl(epfd , EPOLL_CTL_DEL , env.data.fd , &env);
					close(env.data.fd);
					continue;
				}
				for(int j =0 ; j<ready_read ; j++)
				{
					if(events[j].events & EPOLLOUT)
					{
						write(events[j].data.fd , chat , tmp);
					}
				}
				bzero(chat , 256);
			}
	
		}

	}
	env.data.fd = listenfd;
	epoll_ctl(epfd , EPOLL_CTL_DEL , env.data.fd , &env);
	close(epfd);
	close(listenfd);
	return 0;	
}



