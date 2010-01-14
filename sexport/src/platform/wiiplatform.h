#ifndef __WIIPLATFORM_H__
#define __WIIPLATFORM_H__

#include "../iplatform.h"

class WiiPlatform : public IPlatform
{
	private:

	public:
		virtual ~WiiPlatform() {}
		bfs::path ProcessFont(Font *obj, IResource *res, u32 totalChars);

		inline virtual u32 GetCode() const { return PLATFORM_CODE_WII; }
		inline virtual const char *GetName() const { return "wii"; }
		virtual bfs::path GetOutputPath(const IResource *res) const;
		virtual bfs::path GetInputPath(const IResource *res) const;
		virtual bfs::path GetOutputPath(const IObject *obj) const;
		virtual bfs::path GetInputPath(const IObject *obj) const;
		inline virtual u32 Swap32(u32 v) { return _Swap32(v); }
		inline virtual u16 Swap16(u16 v) { return _Swap16(v); }
		inline virtual f32 Swapf32(f32 v) { return _SwapF32(v); }

		virtual void Compile(Dictionary *obj);
		virtual void Compile(IObject *obj);
		virtual void Compile(IResource *obj);
		virtual void Compile(Sprite *obj);
		virtual void Compile(Animation *obj);
		virtual void Compile(Frame *obj);
		virtual void Compile(Image *obj);
		virtual void Compile(Sound *obj);
		virtual void Compile(Music *obj);
		virtual void Compile(SoundResource *obj);
		virtual void Compile(MusicResource *obj);
		virtual void Compile(Font *obj);
		virtual void Compile(Button *obj);
		virtual void Compile(Mask *obj);

		virtual void ConvertSound(bfs::path filename, SoundResource *obj);
};

#endif
