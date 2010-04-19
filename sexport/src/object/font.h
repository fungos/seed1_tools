#ifndef __FONT_H__
#define __FONT_H__

#include "../defines.h"
#include "../iobject.h"
#include "../iresource.h"
#include "../tsf.h"
#include "../dictionary.h"

#define FNT_MAGIC		0x00544E46
#define FNT_VERSION		0x00000001


struct FontObjectHeader
{
	ObjectHeader block;
	f32 tracking;
	f32 spacing;
	f32 space;
	u32 spriteId;
	u32 extId;
	// const char *pSprite;
	// const char *pSpriteExt;
};

class Exporter;

class Font : public IObject
{
	private:
		f32 space;
		f32 spacing;
		f32 tracking;
		f32 glyphWidth;
		f32 glyphHeight;
		u32 characters;
		//u32 language;
		bool usingAtlas;
		IResource *pResource;
		bfs::path cSprite;
		u32 iExtId;

		Font(const Font &);
		Font &operator=(const Font &);

	public:
		Font(const char *name, IResource *res);
		virtual ~Font();

		inline const char *GetFilename() const
		{
			return pName;
		}

		inline const u32 GetCharacters() const
		{
			return this->characters;
		}

		/*inline const u32 *GetLanguage() const
		{
			return &this->language;
		}*/

		inline const bool IsUsingAtlas() const
		{
			return this->usingAtlas;
		}

		inline void SetTracking(f32 t)
		{
			this->tracking = t;
		}

		inline void SetSpacing(f32 t)
		{
			this->spacing = t;
		}

		inline void SetSpace(f32 t)
		{
			this->space = t;
		}

		inline void SetCharacters(u32 t)
		{
			this->characters = t;
		}

		inline void SetGlyphWidth(u32 t)
		{
			this->glyphWidth = t;
		}

		inline void SetGlyphHeight(u32 t)
		{
			this->glyphHeight = t;
		}

		inline u32 GetGlyphWidth() const
		{
			return glyphWidth;
		}

		inline u32 GetGlyphHeight() const
		{
			return glyphHeight;
		}

		inline IResource *GetResource() const
		{
			return this->pResource;
		}

		inline void SetExtId(u32 id)
		{
			this->iExtId = id;
		}

		void SetSprite(bfs::path file);
/*
		inline void SetLanguage(const char *aux)
		{
			if (!strcasecmp(aux, "en_US"))
				this->language = en_US;
			else if(!strcasecmp(aux, "de_DE"))
				this->language = de_DE;
			else if(!strcasecmp(aux, "pt_BR"))
				this->language = pt_BR;
		}
*/
		inline void SetUsingAtlas(bool b)
		{
			this->usingAtlas = b;
		}

		void Write(FILE *fp, Exporter *e);

		//inline int GetType() const { return OBJECT_FONT; }
};

#endif
