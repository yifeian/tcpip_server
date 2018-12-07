/*
 * tcp_recv_out_of_data.c
 *
 *  Created on: Nov 19, 2018
 *      Author: yifeifan
 */
#include "unp.h"
#include <unistd.h>
#include <fcntl.h>

#define SIGURG_WAY
#if 0
int listenfd, connfd;

void sig_urg(int);


int main(int argc ,char **argv)
{
	int n,justreadoob = 0;
	fd_set rset, xset;
	char buff[100];
	if(argc == 2)
		listenfd = Tcp_listen(NULL,argv[1],NULL);
	else if(argc == 3)
		listenfd = Tcp_listen(argv[1],argv[2],NULL);
	else
		err_quit("usage : [<host>]<port>");
	connfd = Accept(listenfd,NULL,NULL);
#ifdef SIGURG_WAY
	Signal(SIGURG,sig_urg);
	Fcntl(connfd, F_SETOWN, getpid());
#else
	FD_ZERO(&rset);
	FD_ZERO(&xset);
#endif
	for(;;)
	{
#ifdef SIGURG_WAY
		if((n = Read(connfd, buff, sizeof(buff - 1))) == 0)
		{
			printf("reccived EOF \n");
			exit(0);
		}
		buff[n] = 0;
		printf("read %d bytes :%s\n",n,buff);
	}
#else
	FD_SET(connfd,&rset);
	if(justreadoob == 0)
		FD_SET(connfd,&xset);

		Select(connfd+1, &rset,NULL,&xset,NULL);
		if(FD_ISSET(connfd, &xset))
		{
			n = Recv(connfd, buff, sizeof(buff)-1,MSG_OOB);
			buff[n] = 0;
			printf("read %d OBB byte :%s\n",n,buff);
			justreadoob = 1;
		}
		if(FD_ISSET(connfd, &rset))
		{
			if((n = Read(connfd, buff, sizeof(buff)-1)) == 0)
			{
				printf("reccived EOF\n");
				exit(0);
			}
			buff[n] = 0;
			printf("read %d bytes : %s\n",n,buff);
			justreadoob = 0;
		}
	}
#endif
}
void sig_urg(int signo)
{
	int n;

	char buff[2048];
	printf("SIGURG received\n");
	n = Recv(connfd, buff, sizeof(buff)-1,MSG_OOB);
	buff[n] = 0;
	printf("read %d OOB byte : %s\n",n,buff);
}

#endif
#if 0
/* use sockatmark function  */

int sockatmark(int fd)
{
	int flag;
	if(ioctl(fd, SIOCATMARK, &flag) < 0)
		return(-1);
	return(flag != 0);
}


/*
 * 1. 外带标记总是指向普通数据最后一字字节紧后的问题，如果外带数据在线接收，
 那么如果下一个待读入的字节使用MSG_OOB标志发送，那么sockatmark就返回真
 如果SO_OOBINLINE选项没有开启，那么下一个待读入的字节是跟在外带数据后发送的第一个字节，
   2. 读操作总是停在外带标记上。
 * */
void main(int argc, char **argv)
{
	int n,listenfd,connfd;
	const int on = 1;
	char buff[100];
	argc = 2;
	if(argc == 2)
		listenfd = Tcp_listen(NULL,argv[1],NULL);
	else if(argc == 3)
		listenfd = Tcp_listen(argv[1],argv[2],NULL);
	else
		err_quit("usage : [<host>]<port>");

	setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on ,sizeof(on));
	connfd = Accept(listenfd, NULL,NULL);
	/*
	 * sleep一段时间来接收来自发送进程的所有数据，这么做是为了
		展示read停在带外标记上，不受缓冲区的额外数据影响
	*/
	sleep(5);
	for(;;)
	{
		if(sockatmark(connfd))
			printf("at OOB mark\n");
		if((n = Read(connfd, buff, sizeof(buff)-1)) == 0)
		{
			printf("received EOF \n");
			exit(0);
		}
		buff[n] = 0;
		printf("read %d bytes :%s\n",n,buff);
	}
}



/*
 * 1. 即便因为流量控制而停止发送数据了，tcp仍然发送外带数据的通知
   2. 在外带数据到达前，接收进程可呢个被通知说发送进程以后发送了外带数据，
   （SIGURG或者select），如果接收进场接着指定MSG_OOB调用recv，外带数据却尚未到达
    recv将返回EWOULDBLOCK错误。
 * */
int main(int argc, char **argv)
{
	int size;
	if(argc == 2)
		listenfd = Tcp_listen(NULL,argv[1],NULL);
	else if(argc == 3)
		listenfd = Tcp_listen(argv[1],argv[2],NULL);
	else
		err_quit("usage : [<host>]<port>");
	size = 4096;
	Setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	connfd = Accept(listenfd, NULL, NULL);
	Signal(SIGURG,sig_urg);
	Fcntl(connfd, F_SETOWN, getpid());
	for(;;)
		pause();
}
#endif
