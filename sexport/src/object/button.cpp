#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#include "button.h"
#include "../filesystem.h"
#include "../exporter.h"
#include "../platform.h"
#include "../log.h"

Button::Button(const char *name, u32 myid)
	: id(myid)
	, priority(0)
	, labelid(0)
	, masktype(0)
	, x(0.0f)
	, y(0.0f)
	, sprite(NULL)
	, mask(NULL)
{
	this->iType = OBJECT_BUTTON;

	if (!name)
	{
		Error(ERROR_EXPORT_BUTTON_MISSING_ATTRIB, "You need a button name!\n");
	}

	if (!myid)
	{
		Error(ERROR_EXPORT_BUTTON_MISSING_ATTRIB, "You need a button id!");
	}

	this->pName = strdup(name);
	this->bfsInputPath = pPlatform->GetInputPath(this);
	this->bfsOutputPath = pPlatform->GetOutputPath(this);
}

Button::~Button()
{
	if (this->pName)
		free(this->pName);
	this->pName = NULL;

	this->vFilePath.clear();
	FilePathVector().swap(vFilePath);
}

void Button::Write(FILE *fp, Exporter *e)
{
	ButtonObjectHeader o;

	o.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	o.block.magic = pPlatform->Swap32(BTN_MAGIC);
	o.block.version = pPlatform->Swap32(BTN_VERSION);
	o.id = pPlatform->Swap32(id);
	o.priority = pPlatform->Swap32(priority);
	o.x = pPlatform->Swapf32(x);
	o.y = pPlatform->Swapf32(y);
	//o.x = x;
	//o.y = y;
	o.masktype = pPlatform->Swap32(masktype);
	o.labelid = pPlatform->Swap32(labelid);
	o.maskFileId = pPlatform->Swap32(0xffffffff); // to remember swap
	o.spriteFileId = pPlatform->Swap32(0xffffffff);

	if (sprite)
	{
		std::string sfile(e->GetOutputPath().string());
		sfile += "/";
		sfile += sprite;
		sfile += ".sprite";

		const char *file = sfile.c_str();
		u32 fid = pCache->GetFilenameId(file);

		//fwrite(sprite, strlen(sprite), 1, fp);
		//fwrite(".sprite", strlen(".sprite")+1, 1, fp);
		o.spriteFileId = pPlatform->Swap32(fid);
	}
	//else fputc(0, fp);

	if (mask)
	{
		//const char *p = (pFileSystem->GetRelativePath(mask->GetOutputPath(), e->GetOutputPath())).string().c_str();
		//fwrite(p, strlen(p) + 1, 1, fp);
		const char *file = mask->GetOutputPath().string().c_str();
		u32 fid = pCache->GetFilenameId(file);
		o.maskFileId = pPlatform->Swap32(fid);
	}
	//else fputc(0, fp);

	fwrite(&o, sizeof(ButtonObjectHeader), 1, fp);
}
