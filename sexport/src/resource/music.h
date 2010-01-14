#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "../defines.h"
#include "../iobject.h"
#include "../iresource.h"


#define BGM_MAGIC	0x004D4742
#define BGM_VERSION 0x00000001


class Exporter;


struct MusicObjectHeader
{
	ObjectHeader block;
	u32			volume;
	//char 		*filename;
};


class MusicResource : public IResource
{
	private:
		MusicResource(const MusicResource &);
		MusicResource &operator=(const MusicResource &);

	public:
		MusicResource(const char *name, const char *lang);
		virtual ~MusicResource() {}

		virtual void SetFilename(const char *fileName);
};


class Music : public IObject
{
	private:
		Music(const Music &);
		Music &operator=(const Music &);

		const MusicResource *pResource;
		u32		iVolume;

	public:
		Music(const char *name, const MusicResource *res);
		virtual ~Music();

		void SetVolume(u32 vol) { this->iVolume = vol; };

		void Write(FILE *fp, Exporter *e);
		//inline int GetType() const { return OBJECT_MUSIC; }
};

#endif
