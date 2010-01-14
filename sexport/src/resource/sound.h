#ifndef __SOUND_H__
#define __SOUND_H__

#include "../defines.h"
#include "../iobject.h"
#include "../iresource.h"


#define SFX_MAGIC	0x00584653
#define SFX_VERSION 0x00000001

#define SOUND_LOOP	0x01


class Exporter;


struct SoundObjectHeader
{
	ObjectHeader block;
	u32			volume;
	u32			flags;
	//char 		*filename;
};

class SoundResource : public IResource
{
	private:
		SoundResource(const SoundResource &);
		SoundResource &operator=(const SoundResource &);

	public:
		SoundResource(const char *name, const char *lang);
		virtual ~SoundResource() {}

		virtual void SetFilename(const char *fileName);
};

class Sound : public IObject
{
	private:
		Sound(const Sound &);
		Sound &operator=(const Sound &);

		const SoundResource *pResource;
		u32		iVolume;
		bool 	bLoop;

	public:
		Sound(const char *name, const SoundResource *res);
		virtual ~Sound();

		void SetVolume(u32 vol) { this->iVolume = vol; }
		void SetLoop(bool b) { this->bLoop = b; }

		void Write(FILE *fp, Exporter *e);
		//inline int GetType() const { return OBJECT_SOUND; }
};


#endif
