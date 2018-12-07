/*
 * test_client.c
 *
 *  Created on: May 22, 2018
 *      Author: yifeifan
 */

#include "unp.h"


/*
 * network.c
 *
 *  Created on: Aug 29, 2018
 *      Author: yifeifan
 */
//echo text
void str_echo_all(int sockfd)
{
	char recvbuff[MAXLINE];
	char buff[MAXLINE];
	struct sockaddr_in  peeraddr;
	socklen_t len;
	ssize_t n;

	bzero(&peeraddr,sizeof(peeraddr));
	len = sizeof(peeraddr);
	getpeername(sockfd,(SA *)&peeraddr,&len);
	printf("peeraddr: %s, port %d\n",\
			inet_ntop(AF_INET, &peeraddr.sin_addr, buff, sizeof(buff)),\
			ntohs(peeraddr.sin_port));
	//heartbeat_serv(sockfd,1,5);
again:
	while((n = read(sockfd, recvbuff, sizeof(recvbuff))) > 0)
		Writen(sockfd,recvbuff,n);
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		err_sys("str_echo : read error");

}
// echo text add
void echo_text_add(int sockfd)
{
	ssize_t n;
	long arg1, arg2;
	char buf[MAXLINE];

	for(;;)
	{
		if((n = Readline(sockfd, buf, MAXLINE)) == 0)
			return;
		if(sscanf(buf, "%ld%ld", &arg1, &arg2) == 2)
			snprintf(buf, sizeof(buf),"%ld\n",arg1 + arg2);
		else
			snprintf(buf, sizeof(buf),"%s","input error \n");
		n = strlen(buf);
		Writen(sockfd, buf, n);
	}
}

void echo_binary_add(int sockfd)
{
	ssize_t n;
	long args[2];
	long result;

	for(;;)
	{
		if((n = Readn(sockfd, &args,sizeof(args))) == 0)
			return;
		result = args[0] + args[1];
		Writen(sockfd, &result, sizeof(result));
	}
}

//echo binary


#if 0
 /* server fork new process*/
int main(int argc, char **argv)
{
	int listenfd, connfd,n;
	socklen_t clilen;
	struct sockaddr_in servaddr,cliaddr,listen_servaddr,conn_servaddr;
	pid_t childpid;
	char buff[MAXLINE];

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);
	int len = sizeof(servaddr);
	Bind(listenfd,(SA *)&servaddr, sizeof(servaddr));
	bzero(&listen_servaddr, sizeof(listen_servaddr));
	/*
	getsockname(listenfd,(SA *)&listen_servaddr,(socklen_t *)&len);
	printf("listen_servaddr: %s, port %d\n",\
			inet_ntop(AF_INET, &listen_servaddr.sin_addr, buff, sizeof(buff)),\
			ntohs(listen_servaddr.sin_port));
	*/
	Listen(listenfd, LISTENQ);
	Signal(SIGCHLD, sig_chld);

	for(;;)
	{
		clilen = sizeof(cliaddr);
		if((connfd = Accept(listenfd, (SA *)&cliaddr, &clilen)) < 0)
		{
			if(errno ==EINTR)
				continue;
			else
				err_sys("accept error");
		}
		bzero(&conn_servaddr,sizeof(conn_servaddr));
		getsockname(connfd,(SA *)&conn_servaddr,(socklen_t *)&len);
		printf("conn_servaddr: %s, port %d\n",\
				inet_ntop(AF_INET, &conn_servaddr.sin_addr, buff, sizeof(buff)),\
				ntohs(conn_servaddr.sin_port));

		if((childpid = Fork()) == 0)
		{
			Close(listenfd);
			str_echo_all(connfd);
			exit(0);
		}

		Close(connfd);
	}
}
#endif

//server select
#if 0
int main(int argc ,char **argv)
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buff[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	char buf[MAXLINE];

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(6666);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;
	maxi = -1;
	for(i = 0; i < FD_SETSIZE; i++)
	{
		client[i] = -1;    /* -1 indicates avaliable entry*/
	}
	FD_ZERO(&allset);


	for( ; ; )
	{
		FD_SET(listenfd,&allset);
		rset = allset;     /* structure assignment */
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);      /* if now client connection */
		if(FD_ISSET(listenfd, &rset))
		{
			clilen = sizeof(cliaddr);
			bzero(&cliaddr,sizeof(cliaddr));
			connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
			printf("new client: %s, port %d\n",\
					inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),\
					ntohs(cliaddr.sin_port));
			bzero(&cliaddr,sizeof(cliaddr));
			clilen = sizeof(cliaddr);
			getsockname(connfd,(SA *)&cliaddr,(socklen_t *)&clilen);
			printf("conn_servaddr: %s, port %d\n",\
					inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),\
					ntohs(cliaddr.sin_port));
			for(i = 0; i < FD_SETSIZE; i++)
			{
				if(client[i] < 0)
				{
					client[i] = connfd;                /* save descriptor */
					break;
				}
			}
			if(i == FD_SETSIZE)
				err_quit("too many clients");
			FD_SET(connfd,&allset);                    /* add new descriptor to set */
			if(connfd > maxfd)
				maxfd = connfd;                        /* for select */
			if(i > maxi)
				maxi = i;                              /* max index in client[] array */
			if(--nready <= 0)
				continue;                              /* no more readable descriptor */
		}
		for (i = 0; i <= maxi; i++)                    /* check all clients for data */
		{
			if((sockfd = client[i]) < 0)
				continue;
			if(FD_ISSET(sockfd, &rset))
			{
				if((n = Read(sockfd, buf, MAXLINE)) == 0)
				{
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
					Writen(sockfd, buf, n);
				if(--nready <= 0)
					break;                             /* no more readable descriptors */
			}
		}

	}
}
#endif

