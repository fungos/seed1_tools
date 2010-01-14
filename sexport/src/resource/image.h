#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "../defines.h"
#include "../iresource.h"
#include "../tsf.h"

#define TIO_MAGIC		0x004F4954
#define TIO_VERSION		0x00000001

/*
We need this padding because the image inside the file must reside in an address 32bytes aligned.
*/
struct ImageObjectHeader
{
	ObjectHeader block;
	u32 width;
	u32 height;
	u32 colorkey;
	u32 padding01;
	u32 padding02;
};

class Exporter;

class Image : public IResource
{
	private:
		bool bHasAlpha;
		u32 iX;
		u32 iY;
		u32 iWidth;
		u32 iHeight;
		u32 iScreenWidth;
		u32 iScreenHeight;
		TSFHeader tsfHeader;

	public:
		Image(const char *name, const char *lang);
		virtual ~Image();

		virtual void SetFilename(const char *filename);

		void SetX(u32 x) { this->iX = x; }
		void SetY(u32 y) { this->iY = y; }
		void SetWidth(u32 w) { this->iWidth = w; }
		void SetHeight(u32 h) { this->iHeight = h; }
		void SetScreenWidth(u32 w) { this->iScreenWidth = w; }
		void SetScreenHeight(u32 h) { this->iScreenHeight = h; }

		u32 GetX() const { return this->iX; }
		u32 GetY() const { return this->iY; }
		u32 GetWidth() const { return this->iWidth; }
		u32 GetHeight() const { return this->iHeight; }
		u32 GetScreenWidth() const { return this->iScreenWidth; }
		u32 GetScreenHeight() const { return this->iScreenHeight; }
		const bool HasAlpha() const { return this->bHasAlpha; }

		void SetTSFHeader(const u32 width, const u32 height, const u32 frames, const u32 globalFrameTime, const bool loop, const bool animated, const u8 *frameTime, const u16 colorKey, u16 platform);
		void SetTSFHeader(const TSFHeader header);
		TSFHeader GetTSFHeader() const;

		//void Write(FILE *fp, Exporter *e);

		eResourceType GetType() const { return RESOURCE_IMAGE; }
};

#endif
