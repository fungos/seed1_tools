#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "../defines.h"
#include "../iobject.h"
#include "../iresource.h"

#define BTN_MAGIC		0x004E5442
#define BTN_VERSION		0x00000001


struct ButtonObjectHeader
{
	ObjectHeader block;
	u32		id;
	u32		priority;
	f32		x;
	f32		y;
	u32		masktype; // 0 = rect, 1 = mask, 2 = pixel
	u32		labelid;
	u32		spriteFileId; //const char *sprite;
	u32		maskFileId; //const char *mask;
};

class Exporter;

class Button : public IObject
{
	private:
		u32 id;
		u32 priority;
		u32 labelid;
		u32 masktype;
		f32 x;
		f32 y;
		const char *sprite;
		IResource *mask;

		Button(const Button &);
		Button &operator=(const Button &);

	public:
		Button(const char *name, u32 myid);
		virtual ~Button();

		inline const char *GetFilename() const
		{
			return pName;
		}

		inline void SetPriority(u32 v)
		{
			this->priority = v;
		}

		inline void SetLabel(u32 v)
		{
			this->labelid = v;
		}

		inline void SetMaskType(u32 v)
		{
			this->masktype = v;
		}

		inline void SetX(f32 x)
		{
			this->x = x;
		}

		inline void SetY(f32 y)
		{
			this->y = y;
		}

		inline void SetSprite(const char *f)
		{
			this->sprite = f;
		}

		inline void SetMask(IResource *res)
		{
			this->mask = res;
		}

		inline IResource *GetMask() const
		{
			return this->mask;
		}

		inline u32 GetId() const
		{
			return this->id;
		}

		void Write(FILE *fp, Exporter *e);
};

#endif
