#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sstream>
#include <string>

#include "sound.h"
#include "../exporter.h"
#include "../config.h"
#include "../utils.h"
#include "../platform.h"
#include "../log.h"


#define TAG		"[Sound] "


SoundResource::SoundResource(const char *name, const char *lang)
	: IResource(lang)
{
	iType = RESOURCE_SOUND;
	if (name)
		this->SetName(name);
	else
	{
		Error(ERROR_EXPORT_SOUND_RESOURCE_MISSING_ATTRIB, TAG "A sound resource requires a name attribute.");
	}
}

void SoundResource::SetFilename(const char *fileName)
{
	IResource::SetFilename(fileName);
	bfsOutputPath = pPlatform->GetOutputPath(this);
	bfsInputPath = pPlatform->GetInputPath(this);

	if (!bfs::exists(bfsInputPath))
	{
		Log(TAG "File %s not found, falling back to .wav and converting it to platform specific format.", bfsInputPath.string().c_str());
		std::string inp(fileName);
		bfs::path tmp(e->GetInputPath(this->GetType()));
		tmp /= (inp + ".wav");

		if (!bfs::exists(tmp))
		{
			Error(ERROR_FILE_NOT_FOUND, TAG "Sound audio file '%s' not found.", tmp.string().c_str());
		}
		else
		{
			pPlatform->ConvertSound(tmp, this);
		}
	}
}

Sound::Sound(const char *name, const SoundResource *res)
	: pResource(res)
	, iVolume(0)
	, bLoop(false)
{
	this->iType = OBJECT_SOUND;

	if (!res)
	{
		Error(ERROR_EXPORT_SOUND_RESOURCE_MISSING_ATTRIB, TAG "You need a sound audio resource.\n");
	}

	this->pName = strdup(name); //strdup(res->GetName());
	this->bfsOutputPath = pPlatform->GetOutputPath(this);
	//this->pResource = res;
}

Sound::~Sound()
{
	if (this->pName)
		free(this->pName);
	this->pName = NULL;
}

void Sound::Write(FILE *fp, Exporter *e)
{
	SoundObjectHeader foh;
	foh.block.magic = pPlatform->Swap32(SFX_MAGIC);
	foh.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	foh.block.version = pPlatform->Swap32(SFX_VERSION);

	foh.volume 	= pPlatform->Swap32(this->iVolume);
	u32 flags = 0;
	if (this->bLoop)
		flags	|= SOUND_LOOP;
	foh.flags	= pPlatform->Swap32(flags);

	std::ostringstream line;

	const char *full = pPlatform->GetOutputPath(pResource).string().c_str();
	const char *base = e->GetOutputPath().string().c_str();

	const char *path = &full[strlen(base)];

	while (path[0] == '\\' || path[0] == '/')
		path = &path[1];

	char *foo = strdup(path);
	char *ptr = NULL;
	while ((ptr = strchr(foo, '\\')) != FALSE)
		ptr[0] = '/';
	line << foo;
	free (foo);

	fwrite(&foh, sizeof(SoundObjectHeader), 1, fp);
	fwrite(line.str().c_str(), line.str().size() + 1, 1, fp);
}

