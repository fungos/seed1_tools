#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sstream>
#include <string>

#include "music.h"
#include "../exporter.h"
#include "../config.h"
#include "../utils.h"
#include "../platform.h"
#include "../log.h"


#define TAG		"[Music] "


MusicResource::MusicResource(const char *name, const char *lang)
	: IResource(lang)
{
	iType = RESOURCE_MUSIC;
	if (name)
		this->SetName(name);
	else
	{
		Error(ERROR_EXPORT_SOUND_RESOURCE_MISSING_ATTRIB, TAG "A music resource requires a name attribute.");
	}
}

void MusicResource::SetFilename(const char *fileName)
{
	IResource::SetFilename(fileName);
	bfsOutputPath = pPlatform->GetOutputPath(this);
	bfsInputPath = pPlatform->GetInputPath(this);
}

Music::Music(const char *name, const MusicResource *res)
	: pResource(res)
	, iVolume(0)
{
	this->iType = OBJECT_MUSIC;

	if (!res)
	{
		Error(ERROR_EXPORT_SOUND_RESOURCE_MISSING_ATTRIB, TAG "You need a music audio resource.\n");
	}

	this->pName = strdup(name); //strdup(res->GetName());
	this->bfsOutputPath = pPlatform->GetOutputPath(this);
	//this->pResource = res;
}

Music::~Music()
{
	if (this->pName)
		free(this->pName);
	this->pName = NULL;
}

void Music::Write(FILE *fp, Exporter *e)
{
	std::ostringstream line;

	MusicObjectHeader foh;
	foh.block.magic = pPlatform->Swap32(BGM_MAGIC);
	foh.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	foh.block.version = pPlatform->Swap32(BGM_VERSION);

	foh.volume = pPlatform->Swap32(this->iVolume);

	//const char *full = pPlatform->GetOutputPath(pResource).string().c_str();
	const char *base = e->GetOutputPath().string().c_str();
	u32 files = this->pResource->GetFilesAmount();

	fwrite(&foh, sizeof(MusicObjectHeader), 1, fp);

	if (!files)
		Log(TAG "No music resources added to music object.");

	for (u32 i = 0; i < files; i++)
	{
		const char *file = this->pResource->GetFileVectorPath(i).string().c_str();
		const char *path = &file[strlen(base)];

		while (path[0] == '\\' || path[0] == '/')
				path = &path[1];

		char *foo = strdup(path);
		char *ptr = NULL;
		while ((ptr = strchr(foo, '\\')) != FALSE)
			ptr[0] = '/';
		line << foo;

		fwrite(foo, strlen(foo) + 1, 1, fp);

		free (foo);
	}
}
