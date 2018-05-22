/*
 * tcp_echo_server.c
 *
 *  Created on: May 21, 2018
 *      Author: yifeifan
 */
#include "unp.h"


void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
again:
	while((n = read(sockfd,buf,MAXLINE)) > 0)
		write(sockfd,buf,n);
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		printf("str_echo: read error");
}

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	char buff[MAXLINE];
	time_t ticks;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	for(;;)
	{
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (SA*)&cliaddr, &clilen);
		printf("connection form %s,port %d\n",inet_ntop(AF_INET,&cliaddr.sin_addr,buff,sizeof(buff)),ntohs(cliaddr.sin_port));
		ticks = time(NULL);
		//snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));
		if((childpid = fork()) == 0)
		{
			printf("fork fail \n");
			close(listenfd);
			str_echo(connfd);
			return 0;
		}
		close(connfd);
	}
}
