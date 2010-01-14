#ifndef __CONFIG_H__
#define __CONFIG_H__

//#include "exporter.h"
#include "defines.h"

struct sSpriteConfig
{
	u8 headerType; // 0 = numeric; 1 = fileName;
	u8 imageFormat; // 0 = default; 1 = compressed texture
};

struct sAudioConfig
{
	u8 buildMethod; // 0 = AX; 1 = DSPADPCM;
	u8 headerType;	// 0 = numeric; 1 = filename;
};

class Config
{
	public:
		enum AudioHeaderType
		{
			AudioHeaderNumeric	= 0,
			AudioHeaderString	= 1
		};

		enum SpriteHeaderType
		{
			SpriteHeaderDefault = 0,
			SpriteHeaderCompressedTexture = 1
		};

	public:
		void SetAudioHeaderType(const char *value);
		void SetAudioBuildMethod(const char *value);
		void SetSpriteHeaderType(const char *value);
		void SetImageFormat(const char *value);

		const u8 GetAudioHeaderType() const;
		const u8 GetAudioBuildMethod() const;
		const u8 GetSpriteHeaderType() const;
		const u8 GetImageFormat() const;

	public:
		static Config instance;


	private:
		Config() : audioConfig(), spriteConfig() {}
		~Config() {}

	private:
		sAudioConfig audioConfig;
		sSpriteConfig spriteConfig;
};

Config *const cfg = &Config::instance;

#endif
