#ifndef __CMDLINEPARSER_H__
#define __CMDLINEPARSER_H__

#include "args.h"

class CmdLineParser
{
	private:
		Args **argv;
		int argc;
		int idx;
		int argcExpected;

		int Find(const char *parm);

		CmdLineParser(const CmdLineParser &);
		bool operator=(const CmdLineParser &);

	public:
		CmdLineParser(const int argsSize);
		~CmdLineParser();

		bool Add(const char *shortVersion, const char *longVersion, const bool nullAllow, const bool exclusive, const bool dependsOnValue, CallbackFunc func, void *data);
		bool Parse(const int sysArgc, const char **sysArgv, const bool verbose);

};

#endif
