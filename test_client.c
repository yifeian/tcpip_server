/*
 * test_client.c
 *
 *  Created on: May 22, 2018
 *      Author: yifeifan
 */

#include "unp.h"

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];
	while(fgets(sendline,MAXLINE,fp) != NULL)
	{
		write(sockfd, sendline,strlen(sendline));
		if(readline(sockfd,recvline,MAXLINE) == 0)
			printf("str_cli : server terminated prematurely");
		fputs(recvline,stdout);
	}
}

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	if(argc != 2)
	{
		printf("usage: tcpcli < ipaddress");
		return -1;
	}
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1],&servaddr.sin_addr);
	connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
	str_cli(stdin,sockfd);
	return 0;
}
