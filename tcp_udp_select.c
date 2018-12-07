/*
 * tcp_udp_select.c
 *
 *  Created on: Oct 29, 2018
 *      Author: yifeifan
 */

#include "unp.h"

#define SERV_PORT 13000

void str_echo(int sockfd)
{
	char recvbuff[MAXLINE];
	ssize_t n,len;
	char buff[MAXLINE];
	struct sockaddr_in cliaddr;
	len = sizeof(cliaddr);
	getpeername(sockfd,(SA *)&cliaddr, &len);
	printf("connection from %s, port %d\n",\
	    	inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),\
	    	ntohs(cliaddr.sin_port));

again:
	while((n = read(sockfd, recvbuff, sizeof(recvbuff))) > 0)
		Writen(sockfd,recvbuff,n);
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		err_sys("str_echo : read error");

}
#if 0

int main(int argc, char **argv)
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

    /*create listening tcp socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*缺省条件下，一个套接口不能与一个已在使用中的本地地址捆绑（参见bind()）。
	 * 但有时会需要“重用”地址。因为每一个连接都由本地地址和远端地址的组合唯一确定
	 * 所以只要远端地址不同，两个套接口与一个地址捆绑并无大碍.为了通知套接口实现不
	 * 要因为一个地址已被一个套接口使用就不让它与另一个套接口捆绑应用程序可在bind()
	 * 调用前先设置SO_REUSEADDR选项。请注意仅在bind()调用时该选项才被解释；
	 * 故此无需（但也无害）将一个不会共用地址的套接口设置该选项，
	 * 或者在bind()对这个或其他套接口无影响情况下设置或清除这一选项。
	 */
	setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
	/*listen函数使用主动连接套接字变为被连接套接口，使得一个进程可以接受其它进程的请求，从而成为一个服务器进程*/
	Listen(listenfd, LISTENQ);

	/*create a UDP socket */
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(udpfd, (SA *)&servaddr, sizeof(servaddr));
    /*在一个进程终止或者停止时，将SIGCHLD信号发送给其父进程，按系统默认将忽略此信号，
     * 如果父进程希望被告知其子系统的这种状态，则应捕捉此信号
     */
    Signal(SIGCHLD, sig_chld);
    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;
    for(; ;)
    {
    	FD_SET(listenfd, &rset);
    	FD_SET(udpfd, &rset);
    	/*返回处于就绪状态并且已经包含在fd_set结构中的描述字总数
    	 * 当返回为-1时，所有描述符集清0。
    	 * 当返回为0时，表示超时。
    	 * 当返回为正数时，表示已经准备好的描述符数。
    	 * */

    	if((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
    	{
    		if(errno == EINTR)
    			continue;
    		else
    			err_sys("select error");
    	}

    	if(FD_ISSET(listenfd,&rset))
    	{
    		len = sizeof(cliaddr);
    		connfd = Accept(listenfd,(SA *)&cliaddr, &len);

    		if((childpid = Fork()) == 0) /* child process */
    		{
    			Close(listenfd);        /* close listening socket */
    			str_echo(connfd);		/*process the request */
    			exit(0);
    		}
    		Close(connfd);             /* parent close connected socket */

    	}
    	if(FD_ISSET(udpfd, &rset))
    	{
    		len = sizeof(cliaddr);
    		n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);
    		//Sendto(udpfd, mesg, n, 0, (SA *)&cliaddr, len);
    		//printf("recv from %s \n",Sock_ntop(&cliaddr,len));
    	}
    }

}
#endif
