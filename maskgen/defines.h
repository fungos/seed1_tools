#ifndef __DEFINES__
#define __DEFINES__

#include <string.h>

#ifdef WIN32
	#define _CRT_SECURE_NO_WARNINGS
	#ifdef MSVC
		#pragma warning(disable:4996)
		#pragma warning(disable:4351)

		#ifndef snprintf
			#define snprintf _snprintf
		#endif
	#endif

	#undef BOOL
	#define PLATFORM_PATH_SEPARATOR	'\\'
	#define PLATFORM_PATH_SEPARATOR_STR	"\\"
	#define INVALID_PATH_SEPARATOR	'/'
#else
	#define PLATFORM_PATH_SEPARATOR '/'
	#define PLATFORM_PATH_SEPARATOR_STR "/"
	#define INVALID_PATH_SEPARATOR	'\\'
#endif

#define GAME_PATH_SEPARATOR 	'/'
#define GAME_PATH_SEPARATOR_STR "/"


#define VALIDATEPATH(path)	{\
								char *ptr = NULL; \
								while ((ptr = strchr(path, INVALID_PATH_SEPARATOR)) != FALSE) \
									ptr[0] = PLATFORM_PATH_SEPARATOR; \
							}\

#define VALIDATE_GAMEPATH(path) {\
									char *ptr = NULL; \
									while ((ptr = strchr(path, PLATFORM_PATH_SEPARATOR)) != FALSE) \
										ptr[0] = GAME_PATH_SEPARATOR; \
									while ((ptr = strchr(path, INVALID_PATH_SEPARATOR)) != FALSE) \
										ptr[0] = GAME_PATH_SEPARATOR; \
								}\

#define CHECKPTR(ptr)	{\
							if (!ptr)\
								fprintf(stderr, "%s:%d: error.\n", __FILE__, __LINE__);\
						}\

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

#define PIXEL	u32
#define BOOL	int
#define TRUE	1
#define FALSE	0

#define ALIGN_32	32
#define ALIGN_16	16
#define ALIGN_8		8

#ifndef _Swap16
static inline u16 _Swap16(u16 D)
{
	return ((D<<8)|(D>>8));
}
#endif

#ifndef _Swap32
static inline u32 _Swap32(u32 D)
{
	return ((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}
#endif

#define HALT do {} while(1);

#define ASSERT_NULL(x)	if (!((void *)x)) { fprintf(stderr, "Failed assertion " #x); HALT; }

#define DESC_INPUT	0
#define DESC_OUTPUT	1

enum ePlatformCode
{
	PLATFORM_CODE_NDS = 0,
	PLATFORM_CODE_WII,
	PLATFORM_CODE_IPH,
	PLATFORM_CODE_SDL,
	PLATFORM_CODE_WIN,
	PLATFORM_CODE_MAC,
	PLATFORM_CODE_NIX,
	PLATFORM_CODE_PSP,
};

struct ObjectHeader
{
	u32 magic;
	u32 version;
	u32 platform;
};


#endif

