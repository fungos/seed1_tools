#ifndef __DEFINES__
#define __DEFINES__

#include <string.h>
#include "utils.h"


#define DEFAULT_LANG	"en_US"

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

#ifndef _SwapF32
static inline f32 _SwapF32(f32 f)
{
	union
	{
		float f;
		unsigned char b[4];
	} dat1, dat2;

	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
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

struct StringComparator
{
	bool operator()(const char *s1, const char *s2) const
	{
		return strcasecmp(s1, s2) < 0;
	}
};

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

#ifdef POSIX
	#define COMMAND_QUIET		<< (gQuiet ? " >> /dev/null " : " ")
#else
	#define COMMAND_QUIET		<< (gQuiet ? " >> quiet.txt " : " ")
#endif

#define RUN_COMMAND(x)		{ \
								x COMMAND_QUIET; \
								DebugInfo("CMD: %s", x.str().c_str()); \
								u32 cmdRetCode = system(x.str().c_str()); \
								if (cmdRetCode) \
									Error(ERROR_COMMAND_FAIL, "Command '%s' didn't execute properly. Return code: %d.", x.str().c_str(), cmdRetCode); \
							}

enum ExitCodes
{
	ERROR_NONE									= 0,
	// TinyXML Related
	ERROR_EXPORT_CONFIG_NOT_FOUND				= 1,
	ERROR_EXPORT_CONFIG_OPENING_ERROR			= 2,
	ERROR_EXPORT_PROJECT_OPENING_ERROR			= 3,
	ERROR_EXPORT_CONFIG_PARAM					= 4,
	// Image Resource Related
	ERROR_EXPORT_IMAGE_RESOURCE_MISSING_ATTRIB	= 40,
	ERROR_EXPORT_IMAGE_RESOURCE_DUPLICATED		= 41,
	// Sound Resource Related
	ERROR_EXPORT_SOUND_RESOURCE_MISSING_ATTRIB	= 60,
	// Music Resource Related
	ERROR_EXPORT_MUSIC_RESOURCE_ATTRIB_ERROR	= 70,
	// Mask Resource Related
	ERROR_EXPORT_MASK_RESOURCE_MISSING_ATTRIB	= 80,
	ERROR_EXPORT_MASK_RESOURCE_DUPLICATED		= 81,
	// Generic Resources
	ERROR_EXPORT_RESOURCE_NOT_FOUND				= 90,
	ERROR_EXPORT_RESOURCE_MISSING_TYPE			= 91,
	ERROR_EXPORT_RESOURCE_INVALID_TYPE			= 92,
	ERROR_EXPORT_RESOURCE_DUPLICATED			= 93,
	ERROR_EXPORT_RESOURCE_LANG_ERROR			= 94,
	// Sprite Object Related
	ERROR_EXPORT_SPRITE_MISSING_ATTRIB			= 100,
	// Animation Object Related
	ERROR_EXPORT_ANIMATION_MISSING_ATTRIB		= 110,
	// Frame Object Related
	ERROR_EXPORT_FRAME_RESOURCE_NOT_FOUND		= 120,
	ERROR_EXPORT_FRAME_MISSING_ATTRIB			= 121,
	// Font Object Related
	ERROR_EXPORT_FONT_MISSING_ATTRIB			= 130,
	// Sound Object Related
	ERROR_EXPORT_SOUND_MISSING_ATTRIB			= 140,
	// Music Object Related
	ERROR_EXPORT_MUSIC_MISSING_ATTRIB			= 150,
	// Audio Object Related
	ERROR_EXPORT_AUDIO_MISSING_ATTRIB			= 160,
	// Button Object Related
	ERROR_EXPORT_BUTTON_MISSING_ATTRIB			= 170,
	ERROR_EXPORT_BUTTON_SPRITE_NOT_FOUND		= 171,
	ERROR_EXPORT_STRING_MISSING_ATTRIB			= 172,
	// Generic Objects
	ERROR_EXPORT_OBJECT_NOT_FOUND				= 200,
	ERROR_EXPORT_OBJECT_MISSING_TYPE			= 201,
	ERROR_EXPORT_OBJECT_INVALID_TYPE			= 202,
	// Instancing / constructor
	ERROR_CREATING_OBJECT						= 500,
	// File related
	ERROR_FILE_NOT_FOUND						= 900,
	ERROR_FILE_COULD_NOT_OPEN					= 901,
	ERROR_FILE_COULD_NOT_OPEN_WRITE				= 902,
	ERROR_FILE_COULD_NOT_OPEN_CREATE			= 903,
	// Dictionary + Strings
	ERROR_STRING_ALREADY_EXISTS					= 1000,
	ERROR_STRING_MISSING_ATTRIBUTE				= 1001,
	ERROR_STRING_MISSING						= 1002,
	ERRROR_LANGUAGE_ABBREV_UNKNOWN				= 1003,
	// Platform related error
	ERROR_FILE_INVALID_FOR_TARGET_PLATFORM		= 2000,
	// Other
	ERROR_COMMAND_FAIL							= 3000,
	ERROR_INVALID_XML_ATTRIBUTE					= 3001,
	ERROR_MISSING_XML_ATTRIBUTE					= 3002,
	ERROR_UNKNOWN								= -1
};

#endif

