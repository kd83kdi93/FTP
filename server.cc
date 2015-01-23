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
void prase_command(char * str , char * command , char * args);


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
	char buf[]="200 Hello user\n";
	char res_user[]="331 please insert password\n";
	char res_pass[]="230 logined\n";
	char lp[10] , rp[64];
	char command[255];
	write(fd,buf,strlen(buf));	
	read(fd,command,255);
	prase_command(command , lp , rp);
	//printf("len is %d ",strlen(command));
	printf("lp=%d rp=%d\n",strlen(lp),strlen(rp));

	write(fd,res_user,strlen(res_user));
	read(fd,command,255);
	prase_command(command , lp , rp);
	//printf("len is %d ",strlen(command));
	printf("lp=%d rp=%d\n",strlen(lp),strlen(rp));

	write(fd,res_pass,strlen(res_pass));
	read(fd,command,255);
	prase_command(command , lp , rp);
	//printf("len is %d ",strlen(command));
	printf("lp=%d rp=%d\n",strlen(lp),strlen(rp));

	write(fd,res_pass,strlen(res_pass));
	read(fd,command,255);
	prase_command(command , lp , rp);
	//printf("len is %d ",strlen(command));
	printf("lp=%d rp=%d\n",strlen(lp),strlen(rp));

	read(fd,command,255);
	prase_command(command , lp , rp);
	//printf("len is %d ",strlen(command));
	printf("lp=%d rp=%d\n",strlen(lp),strlen(rp));
	if(!strcmp(lp,"PWD"))
	{
		write(fd,"/root",5);
	}
	return ;

}


void prase_command(char * str , char * command , char * args)
{
	int i=0,len=0;
	len = strlen(str);
	len -= 2;
	while(i<len)
	{
		i++;
		if(str[i]==' ')
		{
			strncpy(command , str , i);
			strncpy(args , &str[i+1],len-i-1);
			break;
		}
	}
	return ;
}
