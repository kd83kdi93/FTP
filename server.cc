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
#include "struct.h"


#define BUFFSIZE 1024

char res_welcome[]="200 linux ftp server\n";
char res_user[]="331 please insert password\n";
char res_pass[]="230 logined\n";
char res_user_err[]="430 Invalid username or password\n";
char res_notlogin[]="530 Not logged in\n";
char res_ready[]="220 service is ready\n";

void chldfun(int);
void action(int);
void prase_command(char * str , char * command , char * args);
void do_command(int fd , char * command , char * args);	
void command_user(int fd , char * args);
void command_pass(int fd , char * args);

COM com_com[]={
	{"USER",command_user},
	{"PASS",command_pass},
	{" ",NULL}
};


int main(int argc , char * argv[])
{

	struct sockaddr_in listensock , connectsock;
	int port , listenfd , connectfd ;
	socklen_t connectsock_size; 
	pid_t childpid;
	if(argc<2)
	{
		printf("usage server  <port>\n");
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
	char lp[5] , rp[BUFFSIZE];
	char command[BUFFSIZE];
	write(fd,res_welcome,strlen(res_welcome));	
	while(read(fd,command,BUFFSIZE))
	{
		prase_command(command , lp , rp);
		printf("\n");
		printf("command=%s lp=%d rp=%d\n",lp,strlen(lp),strlen(rp));
		do_command(fd,lp,rp);	
	}


	return ;

}



void prase_command(char * str , char * command , char * args)
{
	sscanf(str,"%s %s",command,args);	
	return ;
}



void do_command(int fd , char * command , char * args)
{
	int i=0;
	while(com_com[i].funp!=NULL)
	{
		if(strcmp(com_com[i].command,command)==0)
		{
			printf("prase %s command\n",command);
			com_com[i].funp(fd , args);
			break;
		}
		i++;
	}
	return; 
}


void command_user(int fd , char * args)
{
	printf("command_user function\n");
	if(!strcmp(args,"bikli"))
	{
		write(fd , res_user , strlen(res_user));		
	}
	else
	{
		write(fd , res_user_err , strlen(res_user_err));
		write(fd , res_ready , strlen(res_ready));
	}		
	return ;
}

void command_pass(int fd , char * args)
{
	printf("command_pass function\n");
	if(!strcmp(args,"123"))
	{
		write(fd , res_pass , strlen(res_pass));		
		write(fd , res_welcome , strlen(res_welcome));
	}
	else
	{
		write(fd , res_user_err , strlen(res_user_err));
		write(fd , res_ready , strlen(res_ready));
		//write(fd , res_notlogin , strlen(res_notlogin));
	}		
	return ;

}
