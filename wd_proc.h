/********************************************************************
*	Author:			Doron Battino									*
* 	Reviewer:		 												*
* 	Date: 			30/07/2023										*
* 	Title: 			watchdog process								*
* 	Description:	Header file										*
* 	Group: 			HRD30											*
*********************************************************************/

#ifndef __ILRD_WATCHDOG_PROC_H
#define __ILRD_WATCHDOG_PROC_H


/*
*	Description:	This function is wd internal api, making wd process to
*					check if client program is still running.
*
*	Returns:		0 on success, 1 on failure.
*/
int MakeMeWD(int argc, const char **argv_);

#endif /* __ILRD_WATCHDOG_PROC_H */
