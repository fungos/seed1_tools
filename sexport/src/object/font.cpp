#ifdef WIN32
#include <SDL/SDL_image.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#include "font.h"
#include "../exporter.h"
#include "../platform.h"
#include "../filesystem.h"
#include "../cache.h"

Font::Font(const char *name, IResource *res)
	: space(3)
	, spacing(0.0f)
	, tracking(0.0f)
	, glyphWidth(0.0f)
	, glyphHeight(0.0f)
	, characters(90)
	/*, language(en_US)*/
	, usingAtlas(false)
	, pResource(NULL)
	, cSprite()
	, iExtId(0xFFFFFFFF)
{
	this->iType = OBJECT_FONT;

	if (!name)
	{
		fprintf(stderr, "You need a font name!\n");
		exit(EXIT_FAILURE);
	}

	if (!res)
	{
		fprintf(stderr, "You need a font resource!\n");
		exit(EXIT_FAILURE);
	}

	this->pName = strdup(name);
	this->bfsInputPath = pPlatform->GetInputPath(this);
	this->bfsOutputPath = pPlatform->GetOutputPath(this);
	this->pResource = res;
}

Font::~Font()
{
	if (this->pName)
		free(this->pName);
	this->pName = NULL;

	this->vFilePath.clear();
	FilePathVector().swap(vFilePath);
}

void Font::SetSprite(bfs::path file)
{
	this->cSprite = file;
}

void Font::Write(FILE *fp, Exporter *e)
{
	const char *file = this->cSprite.string().c_str();
	u32 fid = pCache->GetFilenameId(file);

	FontObjectHeader fo;

	fo.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	fo.block.magic = pPlatform->Swap32(FNT_MAGIC);
	fo.block.version = pPlatform->Swap32(FNT_VERSION);
	fo.space = pPlatform->Swapf32(space);
	fo.spacing = pPlatform->Swapf32(spacing);
	fo.tracking = pPlatform->Swapf32(tracking);
	fo.spriteId = pPlatform->Swap32(fid);
	fo.extId = pPlatform->Swap32(this->iExtId);
	//fo.characters = pPlatform->Swap32(characters);
	//fo.language = pPlatform->Swap32(language);

	fwrite(&fo, sizeof(FontObjectHeader), 1, fp);
	//fprintf(stdout, "ID: %d File: %s\n", cache->GetFilenameId(this->cSprite.string().c_str()), this->cSprite.string().c_str());

	// by filename
	//fprintf(fp, "%s", pFileSystem->GetRelativePath(this->cSprite, e->GetOutputPath()).string().c_str());
	//fputc(0, fp);

	//bfs::path ext = this->cSprite.replace_extension("");
	//fprintf(fp, "%s", pFileSystem->GetRelativePath(ext, e->GetOutputPath()).string().c_str());
	//fprintf(fp, "_ext.sprite");
	//fputc(0, fp);
}

