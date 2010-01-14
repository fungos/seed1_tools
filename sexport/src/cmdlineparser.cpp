#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cmdlineparser.h"
#include "defines.h"

CmdLineParser::CmdLineParser(const int argsSize) : argv(NULL), argc(0), idx(0), argcExpected(0)
{
	argc = argsSize;
	argv = (Args**)malloc(sizeof(Args*) * argc);

	for (int i = 0; i < argsSize; i++)
	{
		argv[i] = (Args*)malloc(sizeof(Args));
	}
}

CmdLineParser::~CmdLineParser()
{
	if (argv)
	{
		for (int i = 0; i < argc; i++)
		{
			if (argv[i])
			{
				free(argv[i]);
			}
		}
	}
}

bool CmdLineParser::Add(const char *shortVersion, const char *longVersion, const bool nullAllow, const bool exclusive, const bool dependsOnValue, CallbackFunc func, void *data)
{
	if (idx == argc)
	{
		fprintf(stderr, "[CMDLINEPARSER] Trying to add more arguments than allocated.\n");
		ASSERT_NULL(0);
	}

	Args *arg = new Args(shortVersion, longVersion, nullAllow, exclusive, dependsOnValue, func, data);
	argv[idx] = arg;

	if (nullAllow)
		argcExpected--;
	else if (dependsOnValue)
		argcExpected++;

	idx++;

	return true;
}

int CmdLineParser::Find(const char *parm)
{
	char *it = const_cast<char*>(parm);
	if (it[0] == '-')
	{
		it++;
		if (it[0] == '-')
			it++;
	}

	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i]->shrt, it) || !strcmp(argv[i]->lng, it))
			return i;
	}

	return -1;
}

bool CmdLineParser::Parse(const int sysArgc, const char **sysArgv, const bool verbose)
{
	if (sysArgc - 1 < (argc + argcExpected))
	{
		if (verbose)
			fprintf(stderr, "Invalid number of arguments.\n");

		return false;
	}

	for (int i = 1; i < sysArgc; i++)
	{
		const char *arg = sysArgv[i];
		int ret = Find(arg);
		if (ret == -1)
		{
			if (verbose)
				fprintf(stderr, "Invalid argument: %s\n", arg);

			return false;
		}

		Args *found = argv[ret];
		if (found->depends)
		{
			const char *value = sysArgv[i + 1];
			if (value[0] == '-')
			{
				if (verbose)
					fprintf(stderr, "Invalid argument: %s %s.\n", sysArgv[i], value);

				return false;
			}
			i++;
			found->callback(value, found->data);
		}
		else
			found->callback(NULL, found->data);
	}

	return true;
}
