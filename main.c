/*
 * daemon to copy file from local to remote
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <syslog.h>

#define SHELLSCRIPT "\
#! /bin/bash \n\
rsync -azh /my_daemon_test -e 'ssh -p 2222' user@domain.com:~ \n\
"


void main(int *argc, char *argv[])
{
	pid_t pid, sid;
	int flag = 0, sys_ret;

	pid = fork();
	
	/* Error in fork */
	if (pid < 0) {
		perror("Error - Fork :");
		exit(EXIT_FAILURE);
	}

	/* Parent Process */
	if (pid > 0) {
//		printf("Parent Process..!! \n");
		exit(EXIT_SUCCESS);
	}

	/* Child Process */
//	printf("Child Process..!! \n");

	umask(0);

	/* Open Syslog */
	openlog(argv[0], LOG_NOWAIT|LOG_PID, LOG_USER);
	
	/* Try to create our own process group */
	sid = setsid();
	if (sid < 0) {
		syslog(LOG_ERR, "Could not create Group ID\n");
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		syslog(LOG_ERR, "Failed to change Directory\n");
		exit (EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	syslog(LOG_NOTICE, "Successfully started daemon\n");

	while(1) {
		syslog(LOG_NOTICE, "daemon in while loop : %d\n", flag);
		sys_ret = system(SHELLSCRIPT);
		syslog(LOG_NOTICE, "RSYNC Executed %d\n", sys_ret);
		sleep(10);
		flag++;
	}

	syslog(LOG_NOTICE, "Terminating Daemon");
	closelog();
}

