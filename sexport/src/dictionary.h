#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

/*
http://www.loc.gov/standards/iso639-2/php/code_list.php
http://www.iso.org/iso/iso-3166-1_decoding_table
*/


enum
{
	en_US,
	pt_BR,
	es_ES,
	de_DE,
	ja_JA,
	cn_CN,
	fr_FR,
	jp_JP,
	MaximumLanguage
};

#define DIC_MAGIC		0x00434944
#define DIC_VERSION		0x00000001
#define MASKBITS		0x3F
#define MASKBYTE		0x80
#define MASK2BYTES		0xC0
#define MASK3BYTES		0xE0
#define MASK4BYTES		0xF0
#define MASK5BYTES		0xF8
#define MASK6BYTES		0xFC

#define TABLE_SIZE		0xFFFF

#include <map>
#include "defines.h"
#include "log.h"
#include "string.h"
#include "exporter.h"


struct CharInfo
{
	u32				iPosition;
	u32             iCount;
	wchar_t			UTF8Decoded;
	const char		*UTF8Encoded;
};

typedef std::map<const char *, String *, StringComparator> StringMap;
typedef StringMap::iterator StringMapIterator;

typedef std::map<wchar_t, CharInfo *> CharMap;
typedef CharMap::iterator CharMapIterator;


struct DictionaryObjectHeader
{
	struct ObjectHeader block;
	u32 language;
	//u32 width;
	u32 glyphs;
	u32 strings;
	// glyphs table
	// string table
	//   string null-terminated
	//   string null-terminated
	//   ...
};


struct StringEntry
{
	u32 length;
	u8 *str;
};


class Dictionary
{
	public:
		Dictionary(const char *language);
		~Dictionary();

		void Process();
		void Write(FILE *fp, Exporter *e);
		void WriteU16Strings(FILE *fp);
		void WriteU16Glyphs(FILE *fp);
		u32 GetUTF8Char(const char *p, u32 *skip);
		void WriteUTF8Char(const char *p, FILE *fp);

		u32 iGetLanguage() const
		{
			return iLang;
		}

		const char *pcGetLanguage() const
		{
			return pLang;
		}

		bool AddString(const char *name, const char *text)
		{
			bool ret = true;
			if (!mapStrings[name])
			{
				String *str = new String(name, text);
				mapStrings[name] = str;
			}
			else
			{
				Log("WARNING: The string '%s' is duplicated in dictionary '%s'", name, pLang);
				ret = false;
			}

			return ret;
		}

		const char *GetStringByName(const char *name)
		{
			const String *str = mapStrings[name];
			return str->GetText();
		}

		const StringMap &GetStrings() const
		{
			return mapStrings;
		}

		const bfs::path &GetOutputPath() const
		{
			return cOutputPath;
		}

		u32 GetExtensionTableSize() const
		{
			return mapChars.size();
		}

	private:
		Dictionary(const Dictionary &);
		const Dictionary &operator =(const Dictionary &);

		u32 		iLang;
		const char *pLang;
		StringMap	mapStrings;
		CharMap		mapChars;

		//u32 		iWidth;
		u32 		iLargeUTF8;
		u32 		iLastChar;
		u32 		iCounter;

		bfs::path	cOutputPath;
};


#endif // __DICTIONARY_H__
