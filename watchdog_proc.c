/********************************************************************
*	Author:			Doron Battino									*
* 	Reviewer:		 												*
* 	Date: 			30/07/2023										*
* 	Title: 			watchdog										*
* 	Description:	Watchdog process 								*
* 	Group: 			HRD30											*
*********************************************************************/

#define _POSIX_C_SOURCE 200809L /* setenv, strdup */

#include <stdio.h>				/* puts */
#include <signal.h>				/* signals */
#include <pthread.h>			/* pthread functions */
#include <unistd.h>				/* execvp */
#include <string.h>				/* strdup, strlen */

#include "wd_proc.h"
#include "watchdog.h"
#include "scheduler.h"
#include "utils.h"

size_t g_counter;
int g_is_ready;
pid_t g_other_pid;

typedef struct wd_info
{
	int argc;
	char **args;
	size_t max_mis;
	size_t t_interval;
	
} wd_info_ty;

extern wd_info_ty wd_info;

enum
{
	BUFF_SIZE = 20
};

/******************************* Declarations *********************************/

static void SetArgsAsWD(const char **args_);
extern void SetSigHandlers(void);
extern void *WatchDogMakeMeImmIMP(void *arg_struct);
extern void DestroyArgsIMP(char **args);

/********************************* Main ***************************************/

int main(int argc, char *argv[])
{
	/* update global struct */
	wd_info.argc = argc;
	g_counter = 0;
	
	/* MakeMeWD(argv) */
	if (0 != MakeMeWD(argc, (const char **)argv))
	{
		perror("MakeMeWD error");
		return (ERROR);
	}
	
	return (SUCCESS);
}

/**************************** Implementations *********************************/

int MakeMeWD(int argc, const char **argv_)
{
	char *pid;
	UNUSED_VAR(argc);
	puts("watchdog started");
	
	g_is_ready = 1;
	
	pid = getenv("WD_PID");
	ExitIfBad((NULL != pid), "Fail to get env var", ERROR);
	g_other_pid = atoi(pid);
	
	/* SetArgsAsWD(argv_); */
	SetArgsAsWD(argv_);
	/* status = SchedNComunicate(); */
	WatchDogMakeMeImmIMP(wd_info.args);
	
	return (SUCCESS);
}

static void SetArgsAsWD(const char **args_)
{
	wd_info.max_mis = atoi((args_[1]));
	wd_info.t_interval = atoi((args_[2]));
		
	wd_info.args = (char **)(&(args_[3]));
}







































































