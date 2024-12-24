/****************************************************************
*	Author: 	Doron 					*
*	Date: 		20/5					*
*	Title: 		Scheduler.				*
*	Description: 	Header file for scheduler WS		*
*	Group:		HRD30					*
*	Reviewer:	****					*
*****************************************************************/
#ifndef __ILRD_SCHEDULER_H
#define __ILRD_SCHEDULER_H

#include <stddef.h>

#include "pqueue.h"
#include "uid.h"
#include "task.h"


/* FUNCTION DECLERATIONS */

typedef struct sched sched_ty;

sched_ty *SchedCreate(void);
 
void SchedDestroy(sched_ty *schedule_);

/*	Desc:	Adds a new task to scheduler
*
*	Input:	schedule_ -	schedulr to work on.
*		interval -	time for the task
*		function ptr -	for callback function that gets parameter and
*				return 1 if task needs to be back at sched,
*				0 if finished.
*		param - 	for callback function.
*
*	Complex:O(size of sched).
*/
uid_ty SchedAdd(sched_ty *schedule_, size_t interval_seconds_,
				int (* action_) (void *param_), void *param_);

/*
*	Complex: O(size of scheduler).	
*/
void SchedRemove(sched_ty *schedule_, uid_ty task_);

/* 	
*	Compelx: O((size of scheduler) * (action function))
*/
int SchedRun(sched_ty *schedule_);

/* 	
*	Desc:	stop the scheduler run.
*	Complex: O(1). 
*/ 
void SchedStop(sched_ty *schedule_);

size_t SchedSize(const sched_ty *schedule_);

int SchedIsEmpty(const sched_ty *schedule_);

void SchedClear(sched_ty *schedule_);


#endif	/* __ILRD_SCHEDULER_H */
