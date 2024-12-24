/****************************************************************
*	Author: 	Doron 					*
*	Date: 		15/5					*
*	Title: 		UID					*
*	Description: 	Header file for UID WS			*
*	Group:		HRD30					*
*	Reviewer:	****					*
*****************************************************************/

#ifndef __ILRD_UID_H
#define __ILRD_UID_H

#include <unistd.h>  	/* pid_t  */
#include <time.h>    	/* time_t */
#include <stddef.h>  	/* size_t */
#include <sys/types.h>	/* time_t */
#include <assert.h>		/* assert */

#define IP_LEN 50

typedef struct uid uid_ty;
/* For ensuring with UidIsSame creation succeded */
extern const uid_ty BAD_UID; 

/*
 *	Error: 		Returns NULL.
 *	Complexity:	O(1). 
 */
 
uid_ty UIDCreate(void);

/*
 *	Complexity:	O(1). 
 */
 
static int UIDIsSame(const uid_ty *, const uid_ty *)__attribute__((unused));


/**********************************************************************/
/* Nothing to see here, move along please!
 * Anything below this line may change without notice 
 */

struct uid
{
	pid_t pid;
	time_t time;
	size_t counter;
	unsigned long ip;
};

static int UIDIsSame(const uid_ty* lhs, const uid_ty* rhs)
{
	assert(lhs);
	assert(rhs);
	
	return 
	(
		lhs->pid == rhs->pid &&
		lhs->time == rhs->time &&
		lhs->counter == rhs->counter &&
		lhs->ip == rhs->ip
	);
}


#endif
