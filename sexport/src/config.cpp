#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "utils.h"

Config Config::instance;


void Config::SetAudioHeaderType(const char *value)
{
	if (!strcasecmp(value, "INT"))
		audioConfig.headerType = 0;
	else if (!strcasecmp(value, "STRING"))
		audioConfig.headerType = 1;
}


void Config::SetAudioBuildMethod(const char *value)
{
	if (!strcasecmp(value, "AX"))
		audioConfig.buildMethod = 0;
	else if (!strcasecmp(value, "DSPADPCM"))
		audioConfig.buildMethod = 1;
}


void Config::SetSpriteHeaderType(const char *value)
{
	if (!strcasecmp(value, "INT"))
		spriteConfig.headerType = 0;
	else if (!strcasecmp(value, "STRING"))
		spriteConfig.headerType = 1;
}


void Config::SetImageFormat(const char *value)
{
	spriteConfig.imageFormat = 0;

	if (!value)
		return;

	if (!strcasecmp(value, "COMPRESSED"))
		spriteConfig.imageFormat = 1;
	else if (!strcasecmp(value, "DEFAULT"))
		spriteConfig.imageFormat = 0;
}


const u8 Config::GetAudioHeaderType() const
{
	return audioConfig.headerType;
}


const u8 Config::GetAudioBuildMethod() const
{
	return audioConfig.buildMethod;
}


const u8 Config::GetSpriteHeaderType() const
{
	return spriteConfig.headerType;
}


const u8 Config::GetImageFormat() const
{
	return spriteConfig.imageFormat;
}

