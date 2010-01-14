#include "log.h"

#include "utils.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void Log(const char *message, ...)
{
	if (gQuiet)
		return;

	char t[2048];
	va_list ap;

	va_start(ap, message);
		vsnprintf(t, 2048, message, ap);
	va_end(ap);

	fprintf(stdout, "%s\n", t);
}

void Error(int returnCode, const char *message, ...)
{
	char t[2048];
	va_list ap;

	va_start(ap, message);
		vsnprintf(t, 2048, message, ap);
	va_end(ap);

	fprintf(stderr, "%s\n", t);
	exit(returnCode);
}

void DebugInfo(const char *message, ...)
{
	if (gQuiet || !gVerbose)
		return;

	char t[2048];
	va_list ap;

	va_start(ap, message);
		vsnprintf(t, 2048, message, ap);
	va_end(ap);

	fprintf(stdout, "%s\n", t);
}
