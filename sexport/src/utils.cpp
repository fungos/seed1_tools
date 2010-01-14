#include <string.h>

#include "utils.h"
#include "defines.h"


char *to_upper(const char *str)
{
	char *strUp = strdup(str);

	for (u32 i = 0; i < strlen(str); i++)
		if (str[i] < 0x7B && str[i] > 0x60)
			strUp[i] = str[i] & (0x41 + (str[i] - 0x61));

	return strUp;
}

const char *get_filename(const char *str)
{
	char *tmp = const_cast<char*>(str);
	char *ext = const_cast<char*>(str);
	while ((tmp = const_cast<char*>(strchr(tmp, '/'))))
		ext = ++tmp;

	return ext;
}

int create_directory(const char *path)
{
	return 0;
}

int file_is_newer_than(const char *src, const char *dst)
{
	return 0;
}
