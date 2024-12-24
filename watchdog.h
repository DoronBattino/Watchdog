/********************************************************************
*	Author:			Doron Battino									*
* 	Reviewer:		 												*
* 	Date: 			30/07/2023										*
* 	Title: 			watchdog										*
* 	Description:	Header file										*
* 	Group: 			HRD30											*
*********************************************************************/

#ifndef __ILRD_WATCHDOG_H
#define __ILRD_WATCHDOG_H

/*
*	Description:	This function attaches watchdog process to your program,
*					if your program fails, wd get it restarted including input
*					parameters.
*
*	Input:			max_times - number of times your program not responded to wd.
*					t_interval - interval between checks for program status.
*
*	Returns:		0 on succes.
*					1 on failure.
*
*	Note:			Cannot use more than on wd if processes related in inheritance
*					relashionship.		
*/
int WatchDogMakeMeImm(int argc, char **argv, size_t max_times, size_t interval_size);


/*
*	Description:	This function signals wd to stop watching your program
*/
int WatchDogDNR(void);

#endif /* __ILRD_WATCHDOG_H */
