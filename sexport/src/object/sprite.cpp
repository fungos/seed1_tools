#include "../defines.h"
#include "../log.h"
#include "sprite.h"
#include "../exporter.h"
#include "../platform.h"
#include "../strcache.h"

#include <string>
#include <sstream>

#define TAG "[Sprite] "

const char empty[] = "";

Frame::Frame(const char *name, const Image *image, u32 time, u32 index) : image(NULL), name(NULL), time(time), x(0), y(0), w(0), h(0), index(index)
{
	this->iType = OBJECT_FRAME;

	if (!image)
	{
		fprintf(stderr, "FRAME: (%s) Resource not found.", name);
		exit(EXIT_FAILURE);
	}

	//this->pName = strdup(image->GetName());
	this->image = image;

	if (name)
	{
		pStringCache->AddString(name);
		this->name = strdup(name);
		//this->bfsOutputPath = pPlatform->GetOutputPath(this);
		//this->bfsInputPath = pPlatform->GetInputPath(this);
	}

	//this->time = time;
}

Frame::~Frame()
{
	if (this->name)
		free(this->name);
	this->name = NULL;

	//if (this->pName)
	//	free(this->pName);
	//this->pName = NULL;

}

void Frame::Write(FILE *fp, Exporter *e)
{
	//fprintf(stdout, "\t\tFrame: %s, Time: %d\n", ((name&&strlen(name)) ? name : "noname"), time);
	//const Image *m = image;

	FrameObjectHeader foh;
	//foh.block.magic = pPlatform->Swap32(TFO_MAGIC);
	//foh.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	//foh.block.version = pPlatform->Swap32(TFO_VERSION);

	foh.index = pPlatform->Swap32(this->index);
	foh.packId = 0;
	foh.time = pPlatform->Swap32(this->time);
	foh.x = pPlatform->Swap32(image->GetX() + this->x);
	foh.y = pPlatform->Swap32(image->GetY() + this->y);
	foh.w = pPlatform->Swap32(image->GetWidth() + this->w);
	foh.h = pPlatform->Swap32(image->GetHeight() + this->h);
	foh.resH = pPlatform->Swap32(image->GetScreenHeight());
	foh.resW = pPlatform->Swap32(image->GetScreenWidth());
	foh.frameId = pPlatform->Swap32(0xffffffff);
	if (this->name)
	{
		u32 sid = pStringCache->GetStringId(this->name);
		foh.frameId = pPlatform->Swap32(sid);
	}

	std::string fout(image->GetOutputPath().string());
	const char *file = fout.c_str();
	string::size_type loc = fout.find("l10n");
	if (loc != string::npos)
	{
		fout = e->GetOutputPath().string() + fout.substr(loc + 10);
		file = fout.c_str();
	}

	u32 fid = pCache->GetFilenameId(file);
	foh.fileId = pPlatform->Swap32(fid);

	//int x = SWAP32(this->time);
	//fwrite(&x, sizeof(u32), 1, fp);

	fwrite(&foh, sizeof(FrameObjectHeader), 1, fp);
/*
	if (this->name)
		fwrite(this->name, strlen(this->name)+1, 1, fp);
	else
		fwrite(empty, strlen(empty)+1, 1, fp);

	fwrite(img->GetFilename(), strlen(img->GetFilename()), 1, fp);
	//fwrite(".tpl\0", 5, 1, fp);

	const char *str = img->GetOutputPath().string().c_str();
	str = &str[strlen(e->GetOutputPath().string().c_str()) + strlen(img->GetFilename()) + 1];
	fwrite(str, strlen(str)+1, 1, fp);
*/
}


Animation::Animation(const char *name, u32 time, bool loop, bool animated, u32 index) : anim(), frames(), name(NULL), time(time), flags(0), w(0), h(0), x(0), y(0), index(index)
{
	this->iType = OBJECT_ANIMATION;

	if (!name)
	{
		Error(ERROR_CREATING_OBJECT, TAG "Animation without name.\n");
	}

	pStringCache->AddString(name);
	this->name = strdup(name);

	//this->flags = 0;
	if (loop)
		this->flags |= LOOP;
	if (animated)
		this->flags |= ANIMATED;

	//this->time = time;
}

Animation::~Animation()
{
	if (this->name)
		free(this->name);
	this->name = NULL;
	this->flags = 0;

	FrameIterator it = this->frames.begin();
	FrameIterator end = this->frames.end();
	for (; it != end; ++it)
	{
		Frame *frame = (*it);
		delete frame;
	}

	this->frames.clear();
}

void Animation::Add(Frame *frame)
{
	if (frame->GetTime() == 0)
		frame->SetTime(this->time);

	this->frames.push_back(frame);
}

void Animation::Write(FILE *fp, Exporter *e)
{
	//fprintf(stdout, "\tAnimation: %s, Time: %d\n", name, time);
	AnimationObjectHeader tao;

	//tao.block.magic = pPlatform->Swap32((u32)TAO_MAGIC);
	//tao.block.version = pPlatform->Swap32(TAO_VERSION);
	//tao.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	tao.index = pPlatform->Swap32(this->index);
	tao.frame_amount = pPlatform->Swap32(frames.size());
	tao.flags = pPlatform->Swap32(this->flags);

	u32 sid = pStringCache->GetStringId(this->name);
	tao.animId = pPlatform->Swap32(sid);

	fwrite(&tao, sizeof(AnimationObjectHeader), 1, fp);

//	fwrite(this->name, strlen(this->name)+1, 1, fp);
	// this->WriteFrames();
}

void Animation::WriteFrames(FILE *fp, Exporter *e)
{
	FrameIterator it = this->frames.begin();
	FrameIterator end = this->frames.end();
	for (; it != end; ++it)
	{
		Frame *frame = (*it);
		frame->Write(fp, e);
	}
}

Sprite::Sprite(const char *name) : animation(), x(0), y(0), w(0), h(0)
{
	this->iType = OBJECT_SPRITE;

	if (!name)
	{
		Error(ERROR_CREATING_OBJECT, TAG "Sprite without name.\n");
	}

	this->pName = strdup(name);
	this->bfsOutputPath = pPlatform->GetOutputPath(this);
}

Sprite::~Sprite()
{
	if (this->pName)
		free(this->pName);
	this->pName = NULL;

	AnimationIterator it = this->animation.begin();
	AnimationIterator end = this->animation.end();
	for (; it != end; ++it)
	{
		Animation *anim = (*it);
		delete anim;
	}

	this->animation.clear();
}

void Sprite::Add(Animation *anim)
{
	this->animation.push_back(anim);
}

void Sprite::Write(FILE *fp, Exporter *e)
{
	SpriteObjectHeader tso;

	tso.block.magic = pPlatform->Swap32((u32)TSO_MAGIC);
	tso.block.version = pPlatform->Swap32(TSO_VERSION);
	tso.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	tso.animation_amount = pPlatform->Swap32(animation.size());

	tso.flags = pPlatform->Swap32(SPRITE_NOFLAG);
	AnimationIterator it = this->animation.begin();
	AnimationIterator end = this->animation.end();
	it = this->animation.begin();
	end = this->animation.end();
	for (; it != end; ++it)
	{
		Animation *anim = (*it);
		if (anim->GetAtlasFlag())
			tso.flags = pPlatform->Swap32(SPRITE_ATLAS);
	}

	fwrite(&tso, sizeof(SpriteObjectHeader), 1, fp);

	// Animation sequence
	it = this->animation.begin();
	end = this->animation.end();
	for (; it != end; ++it)
	{
		Animation *anim = (*it);
		anim->Write(fp, e);
	}

	// Frames sequence
	it = this->animation.begin();
	end = this->animation.end();
	for (; it != end; ++it)
	{
		Animation *anim = (*it);
		anim->WriteFrames(fp, e);
	}
}

