/*
 * sigchldwait.c
 *
 *  Created on: May 22, 2018
 *      Author: yifeifan
 */
#include "unp.h"

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	/*
	pid = wait(&stat);
	*/
	while((pid = waitpid(-1,&stat,WNOHANG)) > 0)
		printf("child %d terminated\n",pid);

	return;
}
