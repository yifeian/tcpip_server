/*
 * udp_server.c
 *
 *  Created on: Oct 26, 2018
 *      Author: yifeifan
 */


#include "unp.h"

static void recvfrom_int(int);
static int count;

void dg_echo_count(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	socklen_t len;
	int socklen,len_t;
	char mesg[MAXLINE];
	Signal(SIGINT,recvfrom_int);
	len_t = sizeof(socklen);
	if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,(void*)&socklen, &len_t) == -1)
	{
		err_ret("getsockopt error");
	}
	printf("\r SO_RCVBUF is %d",socklen);

	for( ; ; )
	{
		len  = clilen;
		Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		//printf("recv from %s \n",Sock_ntop(pcliaddr,len));
		count++;
	}
}

void recvfrom_int(int signo)
{
	printf("\nreceived %d datagrams\n", count);
	exit(0);
}

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];
	char printbuf[MAXLINE];
	for(;;)
	{
		len = clilen;
		n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		printf("recv from %s \n",Sock_ntop(pcliaddr,len));
		snprintf(printbuf,strlen(mesg),"%s\r\n",mesg);
		printf("recv :%s\r\n",printbuf);
		Sendto(sockfd, mesg, n, 0, pcliaddr,len);
	}
}
#if 0
int main(int argc, char **argv)
{
	int sockfd;

	struct sockaddr_in servaddr, cliaddr;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);


	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13440);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));
	dg_echo_count(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

}


//server for tcp and udp

int main(int argc ,char **argv)
{
	int listenfd, connfd, udpfd, nready, maxfdp1;
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;

	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	void sig_chld(int);
	/*create listening TCP socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(12345);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
	Listen(listenfd, (SA*) &servaddr, sizeof(servaddr));

	/* create udp socket */
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(12345);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(udpfd, (SA *)&servaddr, sizeof(servaddr));
	Signal(SIGCHLD, sig_chld);
	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd) + 1;
	for(;;)
	{
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
				err_sys("select error ");
		}

		if(FD_ISSET(listenfd, &rset))
		{
			len = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *)&cliaddr, &len);
			if((childpid = Fork()) == 0)
			{
				Close(listenfd);
				str_echo(connfd);
				exit(0);
			}
			Close(connfd);
		}
		if(FD_ISSET(udpfd, &rset))
		{
			len = sizeof(cliaddr);
			n = recvfrom(udpfd, mesg, MAXLINE, 0,(SA *)&cliaddr, &len);
			printf("recv from %s \n",Sock_ntop(cliaddr,len));
			sendto(udpfd, mesg, n, 0,(SA *)&cliaddr, len);
		}
	}

}
#endif
