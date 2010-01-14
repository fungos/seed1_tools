#ifdef WIN32
#include <SDL/SDL_image.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#include "image.h"
#include "../exporter.h"
#include "../platform.h"
#include "../log.h"


#define TAG		"[Image] "


Image::Image(const char *name, const char *lang)
	: IResource(lang)
	, bHasAlpha(false)
	, iX(0)
	, iY(0)
	, iWidth(0)
	, iHeight(0)
	, tsfHeader()
{
	if (!name)
	{
		Error(ERROR_EXPORT_IMAGE_RESOURCE_MISSING_ATTRIB, TAG "You need an image name!");
	}

	IResource::SetName(name);
	bHasAlpha = TRUE;
}

Image::~Image()
{
}

void Image::SetFilename(const char *filename)
{
	IResource::SetFilename(filename);
	bfsOutputPath = pPlatform->GetOutputPath(this);
	bfsInputPath = pPlatform->GetInputPath(this);

	if (!bfs::exists(bfsInputPath))
	{
		Error(ERROR_FILE_NOT_FOUND, TAG "File '%s' not found.", bfsInputPath.string().c_str());
	}

#ifdef WIN32
	if (pPlatform->GetCode() == PLATFORM_CODE_WII)
	{
		SDL_Surface *surface = IMG_Load(bfsInputPath.string().c_str());
		if (!surface)
		{
			Error(ERROR_FILE_INVALID_FOR_TARGET_PLATFORM, TAG "File %s invalid (invalid image)!\n", bfsInputPath.string().c_str());
		}

		bHasAlpha = (surface->format->BitsPerPixel == 32);
		SDL_FreeSurface(surface);
	}
#endif
}

TSFHeader Image::GetTSFHeader() const
{
	return this->tsfHeader;
}

void Image::SetTSFHeader(const u32 width, const u32 height, const u32 frames, const u32 globalFrameTime, const bool loop, const bool animated, const u8 *frameTime, const u16 colorKey, u16 platform)
{
	this->tsfHeader.width = width;
	this->tsfHeader.height = height;
	this->tsfHeader.frames = frames;
	this->tsfHeader.globalFrameTime = globalFrameTime;
	this->tsfHeader.flags = NO_FLAGS;

	if (loop)
		this->tsfHeader.flags |= TSF_LOOP;
	if (animated)
		this->tsfHeader.flags |= TSF_ANIMATED;

	this->tsfHeader.colorKey = colorKey;
	this->tsfHeader.platform = pPlatform->GetCode();
}

void Image::SetTSFHeader(const TSFHeader header)
{
	this->tsfHeader = header;
	this->tsfHeader.platform = pPlatform->GetCode();
}

/*
void Image::Write(FILE *fp, Exporter *e)
{
	FileImageHeader tio;
	SDL_PixelFormat *fmt = surface->format;

	tio.platform = SWAP32(PLATFORM_CODE_WII);
	tio.height = SWAP32(this->surface->h);
	tio.width = SWAP32(this->surface->w);
	tio.colorkey = SWAP32(fmt->colorkey);
	tio.block.version = SWAP32(TIO_VERSION);
	tio.block.magic = SWAP32((u32)TIO_MAGIC);
	tio.padding01 = 0;
	tio.padding02 = 0;

	fwrite(&tio, sizeof(FileImageHeader), 1, fp);
	fwrite(this->surface->pixels, this->surface->w * this->surface->h * fmt->BytesPerPixel, 1, fp);
}
*/
