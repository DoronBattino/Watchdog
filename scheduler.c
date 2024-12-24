/****************************************************************
*	Author: 	Doron 											*
*	Date: 		20/5											*
*	Title: 		Scheduler.										*
*	Description: 	Source file for scheduler WS				*
*	Group:		HRD30											*
*	Reviewer:	Michael											*
*****************************************************************/

#include <stdlib.h>	/* malloc, free, NULL, size_t */

#include "scheduler.h"
#include "pqueue.h"
#include "task.h"
#include "uid.h"
#include "utils.h"

struct sched
{
	pq_ty *pq;
	task_ty *curr;
	size_t clock;
	int is_run;
	DEBUG_ONLY(int was_stopped;)
};

static int MoreUrgent(const task_ty *tsk1, const task_ty *tsk2);
/*****************************************************************/


sched_ty *SchedCreate(void)
{
	sched_ty *schd = (sched_ty *)(malloc(sizeof(sched_ty)));
	if (NULL == schd)
	{
		return (NULL);
	}
	
	schd->pq = PQCreate((int(*)(const void *, const void *))(&MoreUrgent));
	if (NULL == schd->pq)
	{
		free(schd);
		return (NULL);
	}
	
	schd->curr = NULL;
	schd->clock = 0;
	schd->is_run = 0;
	DEBUG_ONLY(schd->was_stopped = 0;)

	return (schd);
}
 
void SchedDestroy(sched_ty *schedule_)
{
	void *to_delete = NULL;

	assert(NULL != schedule_); 

	while (!PQIsEmpty(schedule_->pq))
	{
		to_delete = PQErase(schedule_->pq, 
			     (int(*)(const void *, const void *))(&TaskCompare),
			     PQPeek(schedule_->pq));
		TaskDestroy((task_ty *)(to_delete));
	}
	
	PQDestroy(schedule_->pq);
	free(schedule_);
}

uid_ty SchedAdd(sched_ty *schedule_, size_t interval_seconds_,
				int (* action_) (void *param_), void *param_)
{
	task_ty *tsk = NULL;

	assert(NULL != schedule_); 
	assert(NULL != action_); 
	assert(NULL != param_); 
	
	tsk = TaskCreate(interval_seconds_, action_, param_);
	if(tsk == NULL)
	{
		return (BAD_UID);
	}
	
	if (0 != schedule_->is_run)
	{
		TaskDestroy(tsk);
		return (BAD_UID);
	}
	if (0 != PQEnqueue(schedule_->pq, tsk))
	{
		TaskDestroy(tsk);
		return (BAD_UID);
	}
	
	schedule_->curr = (task_ty *)(PQPeek(schedule_->pq));
	return (TaskGetID(tsk));
}

void SchedRemove(sched_ty *schedule_, uid_ty task_)
{
	void *to_remove = NULL;

	assert(NULL != schedule_); 
	
	if (TaskIsMatchID(schedule_->curr, &task_))
	{
		to_remove = PQErase(schedule_->pq,
			(int(*)(const void *, const void *))(&TaskCompare),
		 	&task_);
		TaskDestroy((task_ty *)(to_remove));
		schedule_->curr = (task_ty *)(PQPeek(schedule_->pq));
		return;
	} 
	
	to_remove = PQErase(schedule_->pq,
			(int(*)(const void *, const void *))(&TaskCompare),
		 	&task_);
	TaskDestroy((task_ty *)(to_remove));
}

int SchedRun(sched_ty *schedule_)
{
	assert(NULL != schedule_); 
	assert(0 == schedule_->was_stopped);
	
	schedule_->is_run = 1;
	schedule_->clock = 0;
	
	while (!PQIsEmpty(schedule_->pq) && 0 != schedule_->is_run)
	{
		schedule_->curr = (task_ty *)(PQPeek(schedule_->pq));
		
		while ((schedule_->clock) < (TaskWhen(schedule_->curr)))
		{
			if (0 == (sleep(TaskWhen(schedule_->curr) - 
				schedule_->clock)))
			{
				schedule_->clock = TaskWhen(schedule_->curr);
			}
		}
		
		PQDequeue(schedule_->pq);
		if (0 == TaskDoNUpdate(schedule_->curr))
		{
			TaskDestroy(schedule_->curr);
		}
		else
		{
			PQEnqueue(schedule_->pq, schedule_->curr);
		}
	}
	
	/* DEBUG(debug_was_stopped=1) */
	DEBUG_ONLY(schedule_->was_stopped = 1;)
	/* return IsEmpty() */
	return (SchedIsEmpty(schedule_));	

}
 
void SchedStop(sched_ty *schedule_)
{
	assert(NULL != schedule_); 
	assert(1 == schedule_->is_run);  
	
	schedule_->is_run = 0;
}

size_t SchedSize(const sched_ty *schedule_)
{
	assert(NULL != schedule_);

	return (PQSize(schedule_->pq)); 
}

int SchedIsEmpty(const sched_ty *schedule_)
{
	assert(NULL != schedule_); 

	return (PQIsEmpty(schedule_->pq)); 
}

void SchedClear(sched_ty *schedule_)
{
	task_ty *to_delete = NULL;
	
	assert(NULL != schedule_); 
	
	while (!PQIsEmpty(schedule_->pq))
	{
		to_delete = PQErase(schedule_->pq,
			    (int(*)(const void *, const void *))(&TaskCompare),
		 	    (task_ty *)(PQPeek(schedule_->pq)));
		TaskDestroy(to_delete);
	}
	schedule_->curr = NULL;
	schedule_->is_run = 0;
}

int MoreUrgent(const task_ty *tsk1, const task_ty *tsk2)
{
	assert(NULL != tsk1);
	assert(NULL != tsk2);
	
	return (TaskWhen(tsk1) > TaskWhen(tsk2));
}























