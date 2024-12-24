/********************************************************************
*	Author:			Doron Battino									*
* 	Reviewer:		Noam											*
* 	Date: 			30/07/2023										*
* 	Title: 			watchdog										*
* 	Description:	Source file										*
* 	Group: 			HRD30											*
*********************************************************************/

#define _POSIX_C_SOURCE 200809L     /* sigset_t, setenv, strdup */

#include <signal.h>				    /* signals */
#include <pthread.h>			    /* pthread functions */
#include <unistd.h>				    /* execvp */
#include <stdlib.h>				    /* getenv, setenv */
#include <sys/wait.h>			    /* waitpid */
#include <string.h>				    /* strcpy, strlen */

#include "watchdog.h"
#include "scheduler.h"
#include "uid.h"
#include "utils.h"

enum
{
	BUFF_SIZE = 20,
	NUM_ARGS = 3
};

volatile size_t g_counter = 0;
volatile int g_is_dnr = 0;
volatile int g_is_ready = 0;
pid_t g_other_pid = 0;
pthread_mutex_t lock;
pthread_cond_t sched_started;
pthread_t wd_thrd;
sigset_t g_oldset;
sigset_t g_newset;
char g_max_mis[BUFF_SIZE];
char g_t_interval[BUFF_SIZE];
static const char *g_wd_path = "/home/doron/git/system_programming/wd";

struct sigaction sa_def;
struct sigaction sa_1;
struct sigaction sa_2;

typedef struct wd_info
{
	int argc;
	char **args;
	size_t max_mis;
	size_t t_interval;
	
} wd_info_ty;

wd_info_ty wd_info = {0, NULL, 0, 0};


/***************************** Declarations ***********************************/

int SchedNComunicate(void *arg_struct);
int CheckCounterIMP(void *param_);
int SendSignalIMP(void *pid_);
int ReleaseCondIMP(void *param_);
int Resasitate(void);
void SigHandler1(int signum_);
void SigHandler2(int signum_);
void *WatchDogMakeMeImmIMP(void *args);
int Cleanup(void);
int SetAgrsAsClient(int argc, char **argv, size_t max_mis_, size_t t_interval_);
int SetSigHandlers(void);

/***************************** Implementations ********************************/

int WatchDogMakeMeImm(int argc, char **argv_, size_t max_mis_, size_t t_interval_)
{
	puts("Client started");
	
	if (NULL != getenv("WD_PID"))
	{
		g_other_pid = atoi(getenv("WD_PID"));
		g_is_ready = 1;
	}
	
	g_counter = 0; 
	
	/* store - and update handlers mask */
	ReturnIfBad(sigemptyset(&g_oldset), "sigemptyset failed", ERROR);
	ReturnIfBad(sigemptyset(&g_newset), "sigemptyset failed", ERROR);
	ReturnIfBad(sigaddset(&g_newset, SIGUSR1), "sigaddset failed", ERROR);
	ReturnIfBad(sigaddset(&g_newset, SIGUSR2), "sigaddset failed", ERROR);
	ReturnIfBad(sigprocmask(SIG_BLOCK, &g_newset, NULL), "sigprocmask failed", ERROR);
	
	/* args = SetAgrsAsClient(argc, argv, max_mis, t_interval); */
	ReturnIfBad(SetAgrsAsClient(argc, argv_, max_mis_, t_interval_),
				"set client args failed", ERROR);
	
	ReturnIfBad(pthread_mutex_init(&lock, NULL), "mutex init failed", ERROR);
	ReturnIfBad(pthread_cond_init(&sched_started, NULL), "cond init failed", ERROR);
	
	/*create a new thread with MakeImmortalImp()*/
	ReturnIfBad
	(
		pthread_create(&wd_thrd, NULL, WatchDogMakeMeImmIMP, NULL),
		"pthread_create failed",
		ERROR
	);
	
	/* waits until wd sched starts running */
	ReturnIfBad(pthread_cond_wait(&sched_started, &lock), "cond wait failed", ERROR);
	
	ReturnIfBad(pthread_cond_destroy(&sched_started), "cond destroy failed", ERROR);
	ReturnIfBad(pthread_mutex_destroy(&lock), "mutex destroy failed", ERROR);

	return (SUCCESS);
}






int WatchDogDNR(void)
{
	/*dnr flag = 1*/
	g_is_dnr = 1;
	
	puts("DNR Request");
	
	/*kill SIGUSER2*/
	if(0 > kill(g_other_pid, SIGUSR2))
	{
		perror("kill failed");
	}
	
	/* Cleanup */
	if (0 != Cleanup())
	{
		return (ERROR);
	}
	
	/*return 0*/
	return (SUCCESS);
}



/***************************** Static functions *******************************/





int SetAgrsAsClient(int argc, char **argv, size_t max_mis_, size_t t_interval_)
{
	size_t new_size = argc + NUM_ARGS;
	size_t i = NUM_ARGS;
	
	wd_info.args = (char **)malloc(sizeof(char *) * new_size);
	if (NULL == wd_info.args)
	{
		return (ERROR);
	}
	
	sprintf(g_max_mis, "%ld", max_mis_);
	sprintf(g_t_interval, "%ld", t_interval_);
	
	wd_info.args[0] = (char *)g_wd_path;
	wd_info.args[1] = g_max_mis;
	wd_info.args[2] = g_t_interval;
	wd_info.max_mis = max_mis_;
	wd_info.t_interval = t_interval_;
	
	for ( ; i < new_size ; ++i)
	{
		wd_info.args[i] = (char *)(argv[i - NUM_ARGS]);
	}
	
	return (SUCCESS);
}






void *WatchDogMakeMeImmIMP(void *arg_struct)
{	
	SetSigHandlers();
	
	if(0 > pthread_sigmask(SIG_UNBLOCK, &g_newset, NULL))
	{
	    return (NULL);
	}	
	
	/* SchedNComunicate(); */
	if (0 != SchedNComunicate(arg_struct))
	{
		perror("SchedNComunicate failed");
	}	
	
	return (NULL);	
}







int SetSigHandlers(void)
{
	sa_1.sa_flags = SA_SIGINFO;
	sa_2.sa_flags = SA_SIGINFO;
	sa_1.sa_handler = SigHandler1;
	sa_2.sa_handler = SigHandler2;
	
	/*define signal handler for SIGUSER1*/
	ReturnIfBad(sigaction(SIGUSR1, &sa_1, NULL), "sigaction1 failed", ERROR);
	
	/*define signal handler for SIGUSER2*/
	ReturnIfBad(sigaction(SIGUSR2, &sa_2, NULL), "sigaction2 failed", ERROR);
	
	return (SUCCESS);
}






int SchedNComunicate(void *arg_struct)
{
	uid_ty uid = {0,0,0,0};
	uid_ty BAD_UID = {0,0,0,0};
	
	/*sched create*/
	sched_ty *sched = SchedCreate();
	/*handle failure*/
	if (NULL == sched)
	{
		perror("scheduler failed");
		return (ERROR);
	}
	
	/*add task - send signal*/
	uid = SchedAdd(sched, wd_info.t_interval, SendSignalIMP, (void *)sched);
	if (UIDIsSame(&BAD_UID, &uid))
	{
		perror("task watch failed");
		SchedDestroy(sched);
		return (ERROR);
	}

	/* add task check counter */
	uid = SchedAdd(sched, wd_info.t_interval, CheckCounterIMP, (void *)sched);
	if (UIDIsSame(&BAD_UID, &uid))
	{
		perror("task ready failed");
		SchedDestroy(sched);
		return (ERROR);
	}
	
	/*run sched*/
	SchedRun(sched);
	
	/*destroy sched*/
	SchedDestroy(sched);
	
	/*CleanUp*/
	if (0 != Cleanup())
	{
		return (ERROR);
	}
	
	UNUSED_VAR(arg_struct);
	return (SUCCESS);
}





/*task - check counter*/
int CheckCounterIMP(void *param_)
{
	int wstatus = 0;

	/*assert*/
	assert(param_);

	/*if dnr flag == true*/
	if (1 == g_is_dnr)
	{
		/*SchedStop()*/
		SchedStop((sched_ty *)param_);
		return (0);
	}
	
	/*if counter < max_m*/
	if (g_counter < wd_info.max_mis)
	{
		g_counter = __sync_add_and_fetch(&g_counter, 1);
	}
	
	/*else counter >= max_m*/
	else
	{
		/*ready for signal = 0*/
		g_is_ready = 0;
		
		if (g_other_pid != 0)
		{
			/*kill other side*/
			if (0 > kill(g_other_pid, SIGTERM))
			{
				/*handle faliure*/
				perror("kill failed");
			}
			waitpid(g_other_pid, &wstatus, 0);
		}
		
		else
		{
		    /* Allowing client to continue */
			ReturnIfBad
			(
			    pthread_cond_broadcast(&sched_started),
			    "cond_broadcast failed",
			    ERROR
			);
		}
		
		g_counter = 0;
		
		/*resasitate()*/
		if (0 != Resasitate())
		{
			SchedStop((sched_ty *)param_);
			return (0);
		}
		
	}
	return (1);
}




/*task - send signal*/
int SendSignalIMP(void *param_)
{
	/*assert*/
	assert(NULL != param_);
	
	/*if dnr flag == true*/
	if (1 == g_is_dnr)
	{
		/*SchedStop()*/
		SchedStop((sched_ty *)param_);
		return (0);
	}
		
	/*if ready for signal == 1*/
	if (1 == g_is_ready)
	{
		/*kill SIGURS1*/
		if (0 > kill(g_other_pid, SIGUSR1))
		{
			perror("kill failed");
			return (1);
		}
	}
	g_counter = __sync_add_and_fetch(&g_counter, 1);
	
	return (1);
}




void SigHandler1(int signum_)
{
	/*unused signum*/
	UNUSED_VAR(signum_);
	
	/*set g_counter to 0*/
	g_counter = 0;
	
	g_is_ready = 1;
}





void SigHandler2(int signum_)
{
	/*unused signum*/
	UNUSED_VAR(signum_);
	
	/*set dnr flag to 1*/
	g_is_dnr = 1;
}





int Resasitate(void)
{
	char buff[BUFF_SIZE];
	pid_t pid = 0;	
		
	sprintf(buff, "%d", getpid()); /* ERR CHECK */
	setenv("WD_PID", buff, 1);	
	
	/*fork*/
	pid = fork();
	
	/*if error*/
	if (pid < 0 )
	{
		perror("fork failed");
		return (ERROR);
	}
	
	/*if child*/
	else if (0 == pid)
	{
		/*exec*/
		if (0 > execv(wd_info.args[0], wd_info.args))
		{
			perror("execv failed");
			if (0 != Cleanup())
			{
				perror("cleanup failed");
			}
			return (ERROR);
		}
	}
	
	/* if parent */
	g_other_pid = pid;

	return (SUCCESS);
}




int Cleanup(void)
{
	/* free resources */
	free(wd_info.args);
	
	/* update env var */
	if
	(
		0 != unsetenv("WD_PID") ||
		0 != sigprocmask(SIG_SETMASK, &g_oldset, NULL)
	)
	{
		return (ERROR);
	}
	
	return (SUCCESS);
}







































