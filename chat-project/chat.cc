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

void chldfun(int);
void action(int);

int main(int argc , char * argv[])
{

	struct sockaddr_in listensock , connectsock;
	int port , listenfd , connectfd ;
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
	signal(SIGCHLD , chldfun);
	if(listen(listenfd,8)<0)
	{
		printf("listen error %d\n",errno);
	}
	while(1)
	{
		connectsock_size = sizeof(connectsock);
		if((connectfd = accept(listenfd,(struct sockaddr *)(&connectsock),&connectsock_size))<0)			
		{
			if(errno == EINTR)
			{
				continue;
			}else
			{
				printf("accept error %d\n",errno);
				return 0;
			}
		}
		printf("one user\n");
		if((childpid=fork())==0)
		{
			close(listenfd);
			action(connectfd);
			exit(0);	
		}
		close(connectfd);
		//close(listenfd);
	}

	
	close(listenfd);
	return 0;	
}


void chldfun(int sig)
{
	pid_t pid;
	int stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
	{
		printf("child pro close\n");
	}	
	return ;

}

void action(int fd)
{
	char buf[]="Hello user\n";
	printf("---buf size is %d---\n",sizeof(buf));
	write(fd,buf,sizeof(buf));	
	return ;
}
