#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <vector>

#include "../defines.h"
#include "../resource/image.h"
#include "../iobject.h"

#define TSO_MAGIC	0x004F5354
#define TSO_VERSION 0x00000003

#define TAO_MAGIC	0x004F4154
#define TAO_VERSION 0x00000002

#define TFO_MAGIC	0x004F4654
#define TFO_VERSION 0x00000003

#define AnimationIterator std::vector<Animation *>::iterator
#define ImageIterator std::vector<Image *>::iterator
#define FrameIterator std::vector<Frame *>::iterator

// sprite flags
#define SPRITE_NOFLAG	0
#define SPRITE_ATLAS	0x01

struct SpriteObjectHeader
{
	ObjectHeader block;
	u32 animation_amount;
	u32 flags;

	SpriteObjectHeader() : block(), animation_amount(0), flags(SPRITE_NOFLAG) {}
};

struct AnimationObjectHeader
{
	//ObjectHeader block;
	u32 index;
	u32 frame_amount;
	u32 flags;
	u32 animId; //animation name
	//frames

	AnimationObjectHeader() : index(0), frame_amount(0), flags(0), animId(0) {}
};

struct FrameObjectHeader
{
	//ObjectHeader block;
	u32 index;
	u32 time;
	u32 x, y;
	u32 w, h;
	u32 resW, resH;
	u32	packId; // WII ONLY - multiple texture by tpl - must be aways zero
	u32 frameId; // frame name
	u32 fileId; // filename
};

class Exporter;

class Frame : public IObject
{
	private:
		Frame(const Frame &);
		bool operator=(const Frame &);

		const Image *image;
		char *name;
		u32 time;
		u32 x, y;
		u32 w, h;
		u32 index;

	public:
		Frame(const char *name, const Image *, u32 time, u32 index);
		virtual ~Frame();

		inline BOOL IsUsingAtlas()
		{
			BOOL ret = FALSE;

			const Image *img = this->GetImage();
			if (bfs::file_size(img->GetInputPath()) / (1024 * 1024) >= 4)
				ret = TRUE;

			return ret;
		}

		inline const Image *GetImage()
		{
			return const_cast<const Image*>(image);
		}

		inline const u32 GetTime() const
		{
			return time;
		}

		inline void SetTime(u32 time)
		{
			this->time = time;
		}

		inline void SetWidth(const u32 w)
		{
			this->w = w;
		}

		inline void SetHeight(const u32 h)
		{
			this->h = h;
		}

		inline void SetX(const u32 x)
		{
			this->x = x;
		}

		inline void SetY(const u32 y)
		{
			this->y = y;
		}

		inline const char *GetFilename() const
		{
			return this->image->GetFilename();
		}

		inline const bool HasAlpha() const
		{
			return this->image->HasAlpha();
		}

		void Write(FILE *fp, Exporter *e);

		//inline int GetType() const { return OBJECT_FRAME; }
};


class Animation : public IObject
{
	private:
		Animation(const Animation &);
		bool operator=(const Animation &);

		AnimationObjectHeader anim;
		std::vector<Frame *> frames;
		char *name;
		u32 time;
		u32 flags;
		u32 w, h;
		u32 x, y;
		u32 index;

	public:
		enum eFlags
		{
			NONE 	 = 0x00,
			LOOP	 = 0x01,
			ANIMATED = 0x02
		};

		Animation(const char *name, u32 time, bool loop, bool animated, u32 index);
		virtual ~Animation();

		void Add(Frame *);
		inline const u32 GetTime() const { return this->time; }
		inline const u32 GetSize() const { return this->frames.size(); }
		inline const Frame *GetFrame(u32 i) const { return this->frames[i]; }
		inline BOOL GetAtlasFlag()
		{
			BOOL ret = FALSE;

			FrameIterator it = this->frames.begin();
			FrameIterator end = this->frames.end();
			for (; it != end; ++it)
			{
				Frame *frame = (*it);
				if (frame->IsUsingAtlas())
				{
					ret = TRUE;
					break;
				}
			}

			return ret;
		}

		inline void SetX(const u32 value)
		{
			this->x = value;
		}

		inline void SetY(const u32 value)
		{
			this->y = value;
		}

		inline void SetWidth(const u32 value)
		{
			this->w = value;
		}

		inline void SetHeight(const u32 value)
		{
			this->h = value;
		}

		inline const u32 GetX() const
		{
			return this->x;
		}

		inline const u32 GetY() const
		{
			return this->y;
		}

		inline const u32 GetWidth() const
		{
			return this->w;
		}

		inline const u32 GetHeight() const
		{
			return this->h;
		}

		void Write(FILE *fp, Exporter *e);
		void WriteFrames(FILE *fp, Exporter *e);
		//inline int GetType() const { return OBJECT_ANIMATION; }
};


class Sprite : public IObject
{
	private:
		Sprite(const Sprite &);
		bool operator=(const Sprite &);

		std::vector<Animation *> animation;
		u32 x, y;
		u32 w, h;
		//char *name;

	public:
		Sprite(const char *name);
		virtual ~Sprite();

		void Add(Animation *anim);
		void Write(FILE *fp, Exporter *e);

		inline void SetX(const u32 value)
		{
			this->x = value;
		}

		inline void SetY(const u32 value)
		{
			this->y = value;
		}

		inline void SetWidth(const u32 value)
		{
			this->w = value;
		}

		inline void SetHeight(const u32 value)
		{
			this->h = value;
		}

		inline u32 GetX() const
		{
			return this->x;
		}

		inline u32 GetY() const
		{
			return this->y;
		}

		inline u32 GetWidth() const
		{
			return this->w;
		}

		inline u32 GetHeight() const
		{
			return this->h;
		}

		inline u32 GetSize() const
		{
			return animation.size();
		}

		inline Animation *GetAnimation(u32 index) const
		{
			return animation[index];
		}

		//inline int GetType() const { return OBJECT_SPRITE; }
};

#endif
