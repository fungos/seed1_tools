#include <stdlib.h>

#include "args.h"

Args::Args(const char *shortChar, const char *longStr, const bool nullAllow, const bool exclusive, const bool dependsOnValue, CallbackFunc func, void *data) :
	shrt(shortChar),
	lng(longStr),
	exclusive(exclusive),
	depends(dependsOnValue),
	nullAllow(nullAllow),
	callback(func),
	data(data)
{
}

Args::~Args()
{
    /*
	if (shrt)
		free(shrt);
	if (lng)
		free(lng);
    */
}
