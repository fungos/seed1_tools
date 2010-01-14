#ifndef __IPLATFORM_H__
#define __IPLATFORM_H__

#include "defines.h"

class Sprite;
class Animation;
class Frame;
class Image;
class MusicResource;
class SoundResource;
class Font;
class IResource;
class IObject;
class Sound;
class Music;
class Button;
class Mask;
class Dictionary;

class IPlatform
{
	private:
		u32 iW, iH;

	public:
		IPlatform() : iW(0), iH(0) {}
		virtual ~IPlatform() {}

		virtual u32 GetCode() const = 0;
		virtual const char *GetName() const = 0;
		virtual bfs::path GetOutputPath(const IResource *res) const = 0;
		virtual bfs::path GetInputPath(const IResource *res) const = 0;
		virtual bfs::path GetOutputPath(const IObject *obj) const = 0;
		virtual bfs::path GetInputPath(const IObject *obj) const = 0;
		virtual u32 Swap32(u32 v) = 0;
		virtual u16 Swap16(u16 v) = 0;
		virtual f32 Swapf32(f32 v) = 0;

		inline const bool CreateOneDataFileByResource() const { return false; }
		inline const bool CreateOneDataFileByObject() const { return !this->CreateOneDataFileByResource(); }

		virtual void Compile(Dictionary *obj) = 0;
		virtual void Compile(IObject *obj) = 0;
		virtual void Compile(IResource *obj) = 0;
		virtual void Compile(Sprite *obj) = 0;
		virtual void Compile(Animation *obj) = 0;
		virtual void Compile(Frame *obj) = 0;
		virtual void Compile(Image *obj) = 0;
		virtual void Compile(Sound *obj) = 0;
		virtual void Compile(Music *obj) = 0;
		virtual void Compile(SoundResource *obj) = 0;
		virtual void Compile(MusicResource *obj) = 0;
		virtual void Compile(Font *obj)= 0;
		virtual void Compile(Button *obj) = 0;
		virtual void Compile(Mask *obj) = 0;

		virtual void ConvertSound(bfs::path filename, SoundResource *obj) {}
		virtual void ConvertMusic(bfs::path filename, MusicResource *obj) {}

		virtual void SetWidth(u32 width) { iW = width; }
		virtual void SetHeight(u32 height) { iH = height; }
		virtual u32 GetWidth() const { return iW; }
		virtual u32 GetHeight() const { return iH; }
};

#endif
