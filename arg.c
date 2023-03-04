#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "codes.h"

/*
 * argtol take a simple cli arg and attempts to convert it
 * into a long. If the arg is not convertable, we exit with INVALID_ARG.
 */
long argtol(char *arg)
{
	char *end;

	errno = 0;
	long out = strtol(arg, &end, 10);
	if (errno != 0 || *end != '\0')
	{
		printf("Invalid argument %s\n", arg);
		exit(INVALID_ARG);
	}

	return out;
}
