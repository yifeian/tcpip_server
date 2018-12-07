/*
 * server_thread.c
 *
 *  Created on: Dec 7, 2018
 *      Author: yifeifan
 */

#include "unp.h"

void *doit(void *);


void str_echo_thread(int sockfd)
{
	char recvbuff[MAXLINE];
	char buff[MAXLINE];
	socklen_t len;
	ssize_t n;
again:
	while((n = read(sockfd, recvbuff, sizeof(recvbuff))) > 0)
		Writen(sockfd,recvbuff,n);
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		err_sys("str_echo : read error");

}

int main(int argc, char **argv)
{
	int listenfd, connfd, rc, *iptr;
	pthread_t tid;
	socklen_t addrlen, len;
	struct sockaddr_in cliaddr;
	char buff[MAXLINE];
	if(argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if(argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage : [<host>] <service or port>");

	Signal(SIGCHLD, sig_chld);
	for(;;)
	{
		len = addrlen;
		iptr = malloc(sizeof(int));
		*iptr = Accept(listenfd, (SA *)&cliaddr, &len);
		printf("new client: %s, port %d\n",\
				inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),\
				ntohs(cliaddr.sin_port));

		if((rc = pthread_create(&tid, NULL, &doit, iptr)) != 0)
			err_sys("create thread faild ");
	}
}

void *doit(void *arg)
{
	int connfd;
	connfd = *((int *)arg);
	free(arg);
	pthread_detach(pthread_self());
	str_echo_thread(connfd);
	close(connfd);
	return(NULL);
}
