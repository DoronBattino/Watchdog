/****************************************************************
*	Author: 	Doron 					*
*	Date: 		15/5					*
*	Title: 		UID					*
*	Description: 	Source file for UID WS			*
*	Group:		HRD30					*
*	Reviewer:	****					*
*****************************************************************/

#include <unistd.h>	/* pid_t  */
#include <time.h>	/* time_t */
#include <stddef.h>  	/* size_t */
#include <sys/types.h>	/* time_t */
#include <string.h>	/* strcmp, strcpy, strcat, strtoul */
#include <assert.h>	/* assert */
#include <arpa/inet.h>	/* AF_INET */
#include <sys/socket.h>	/* sockaddr */
#include <ifaddrs.h>	/* getifaddrs */
#include <stdio.h>	/* printf */

#include "uid.h"

const uid_ty BAD_UID = {0, 0, 0, 0};
static size_t counter;

uid_ty UIDCreate(void)
{
	uid_ty uid = {0, 0, 0, 0};
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *addr;

	uid.pid = getpid();
	uid.time = time(NULL);
	uid.counter = ++counter;

	if (0 != getifaddrs (&ifap))
	{
		return (uid);
	}
	
	for (ifa = ifap; ifa; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET &&
			strcmp(ifa->ifa_name, "lo"))
		{
			sa = (struct sockaddr_in *) ifa->ifa_addr;
		   	addr = inet_ntoa(sa->sin_addr);
		   	break;
		}
	}
	
	if (NULL == addr)
	{
		return (uid);
	}
	uid.ip = inet_addr(addr);

	freeifaddrs(ifap);
	
	uid.pid = getpid();
	uid.time = time(NULL);
	uid.counter = ++counter;	

	return (uid);
}
