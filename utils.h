#ifndef __ILRD_UTILS_H
#define __ILRD_UTILS_H

#include <stdlib.h>	/* exit */
#include <stdio.h>	/* fprintf */

#ifdef __GNUC__
#define LIKELY(X)        __builtin_expect((X),1)
#define UNLIKELY(X)    __builtin_expect((X),0)

#define DEAD_MEM(T) ((T*)(0xbadc0ffee0ddf00d))

#define MAX(X, Y) (X) > (Y) ? (X) : (Y)
#define MIN(X, Y) (X) > (Y) ? (Y) : (X)

#else
#define LIKELY(X)        (X)
#define UNLIKELY(X)    (X)
#endif 	/* ifdef __GNUC__ */

#ifdef _DEBUG
#define DEBUG_ONLY(X)	 X
#else
#define DEBUG_ONLY(X)	 0
#endif	/* _DEBUG */

#define RETURN_IF_BAD(condition, return_value)  \
        if (!(condition))  return (return_value);					

#define UNUSED_VAR(X)	 (void)(X)
#define UNUSED_FUNC	 __attribute__((unused))

UNUSED_FUNC static void ExitIfBad(int is_good, const char* what, int exit_val)
{
	if (0 > is_good) 
    {
		fprintf(stderr, "Error: %s\n", what);
		exit(exit_val);
	}
}

UNUSED_FUNC static int ReturnIfBad(int is_good, const char* what, int return_val)
{
	if (0 > is_good) 
    {
		fprintf(stderr, "Error: %s\n", what);
		return(return_val);
	}
	return (is_good);
}

enum
{
    SUCCESS = 0,
    ERROR = -1
};

#endif	/* __ILRD_UTILS_H */
