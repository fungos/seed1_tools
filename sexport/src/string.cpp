#include "string.h"
#include "log.h"

String::String(const char *name, const char *ptr)
	: pName(name)
	, pPtr(ptr)
	, iLength(0)
	, iUTF8Length(strlen(ptr))
{
	if (!name)
	{
		Error(ERROR_EXPORT_STRING_MISSING_ATTRIB, "You need a string 'name' attribute!\n");
	}

	if (!ptr)
	{
		Error(ERROR_EXPORT_STRING_MISSING_ATTRIB, "You need a string 'text' attribute!\n");
	}
/*
	this->pName = name;
	this->pPtr = ptr;
	this->iUTF8Length = strlen(ptr);
	this->iLength = 0; // dictionary will set the length
*/
}

String::~String()
{
}
