#include "dictionary.h"
#include "platform.h"


#define TAG		"[Dictionary] "


Dictionary::Dictionary(const char *language)
	: iLang(en_US)
	, pLang(language)
	, mapStrings()
	, mapChars()
	//, iWidth(0)
	, iLargeUTF8(1)
	, iLastChar(0)
	, iCounter(0)
	, cOutputPath()
{
	if (!strcasecmp(language, "de_DE"))
	{
		iLang = de_DE;
	}
	else if (!strcasecmp(language, "pt_BR"))
	{
		iLang = pt_BR;
	}
	else if (!strcasecmp(language, "ja_JA"))
	{
		iLang = ja_JA;
	}
	else if (!strcasecmp(language, "cn_CN"))
	{
		iLang = cn_CN;
	}
	else if (!strcasecmp(language, "es_ES"))
	{
		iLang = es_ES;
	}

	bfs::path output(e->GetOutputPath());
	output /= "l10n";
	output /= language;
	output /= "strings.dict";

	cOutputPath = output;

	//bfs::create_directories(output.parent_path());
}

Dictionary::~Dictionary()
{
}

// http://www.bsdua.org/libbsdua.html#utf8
// wchar_t size
// http://0xcc.net/jsescape/
void Dictionary::Process()
{
	//wchar_t tempbuff[1024 * 4];

	StringVector vstr = e->GetStringVector();
	u32 size = vstr.size();

	if (mapStrings.size() != size)
	{
		Error(ERROR_EXPORT_PROJECT_OPENING_ERROR, TAG "Some language has missing some string, good luck finding it.");
	}

	for (u32 i = 0; i < size; i++)
	{
		String *s = mapStrings[vstr[i]];
		const char *str = s->GetText();
		//size_t total = mbstowcs(tempbuff, str, 1024 * 4);

		u32 length = 0;

		while (1)
		//for (u32 j = 0; j < total; j++)
		{
			u32 skip = 0;
			//wchar_t glyph = tempbuff[j];
			wchar_t glyph = (this->GetUTF8Char(str, &skip) & 0xFFFF);
			if (glyph == 0)
				break;

			if (glyph > '~')
			{
				if (mapChars[glyph])
				{
					CharInfo *ci = mapChars[glyph];
					ci->iCount++;
				}
				else
				{
					CharInfo *ci = new CharInfo();
					ci->iPosition = 0;
					ci->iCount = 1;
					ci->UTF8Decoded = glyph;
					ci->UTF8Encoded = str;
					mapChars[glyph] = ci;
				}
			}

			if (glyph > iLastChar)
				iLastChar = glyph;

			iCounter++;
			length++;
			str += skip;
		}

		s->SetLength(length);
	}

/**/
	std::string n(pLang);
	n += ".txt";

	bfs::path table("tmp");
	table /= n;

	bfs::create_directories(table.parent_path());
	FILE *fp = fopen(table.string().c_str(), "wt");

	CharMapIterator it = mapChars.begin();
	CharMapIterator end = mapChars.end();
	for (; it != end; ++it)
	{
		CharInfo *ci = (*it).second;
		this->WriteUTF8Char(ci->UTF8Encoded, fp);
	}
	//fputc(0, fp);
	fclose(fp);
/**/

	//Log(TAG "[%s] Unique glyphs: %d, Total in text: %d, Large glyph was: %d, Longest was: %d bytes, Total phrases: %d\n", this->pLang, mapChars.size(), iCounter, iLastChar, iLargeUTF8, vstr.size());
	Log(TAG "[%s] Extended table size: %d", this->pLang, mapChars.size());
}

void Dictionary::Write(FILE *fp, Exporter *e)
{
	StringVector vstr = e->GetStringVector();
	u32 total = vstr.size();
/*
	if (iCounter < 0xFF)
	{
		iWidth = 1;
	}
	else if (iCounter < 0xFFFF)
	{
		iWidth = 2;
	}
	else
	{
		iWidth = 4;
	}
*/
	struct DictionaryObjectHeader o;

	o.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	o.block.magic = pPlatform->Swap32(DIC_MAGIC);
	o.block.version = pPlatform->Swap32(DIC_VERSION);
	o.language = pPlatform->Swap32(iLang);
	//o.width = pPlatform->Swap32(iWidth);
	o.glyphs = pPlatform->Swap32(mapChars.size());
	o.strings = pPlatform->Swap32(total);

	fwrite(&o, sizeof(struct DictionaryObjectHeader), 1, fp);
	this->WriteU16Glyphs(fp);
	this->WriteU16Strings(fp);
}

void Dictionary::WriteU16Glyphs(FILE *fp)
{
	CharMapIterator it = mapChars.begin();
	CharMapIterator end = mapChars.end();
	for (; it != end; ++it)
	{
		CharInfo *ci = (*it).second;
		fwrite(&ci->UTF8Decoded, sizeof(ci->UTF8Decoded), 1, fp);
	}
}

void Dictionary::WriteU16Strings(FILE *fp)
{
	StringVector vstr = e->GetStringVector();
	for (u32 i = 0; i < vstr.size(); i++)
	{
		String *s = mapStrings[vstr[i]];
		const char *str = s->GetText();

		u32 len = pPlatform->Swap32(s->GetLength());
		fwrite(&len, sizeof(u32), 1, fp);

		while (1)
		{
			u32 skip = 0;
			wchar_t glyph = (this->GetUTF8Char(str, &skip) & 0xFFFF);
			if (glyph == 0)
			{
				fwrite(&glyph, sizeof(u16), 1, fp);
				break;
			}

			u16 ch = pPlatform->Swap16((u16)glyph);
			fwrite(&ch, sizeof(u16), 1, fp);
			str += skip;
		}
	}
}

u32 Dictionary::GetUTF8Char(const char *p, u32 *skip)
{
	u32 size = 1;
	u32 ch = 0;
	u8 b = p[0];

	*skip = size;
	if (b == 0)
		return 0; // bleh

	if ((b & MASK6BYTES) == MASK6BYTES)
	{
		size = 6;
		ch = ((b & 0x01) << 30) | ((p[1] & MASKBITS) << 24) | ((p[2] & MASKBITS) << 18) | ((p[3] & MASKBITS) << 12) | ((p[4] & MASKBITS) << 6) | (p[5] & MASKBITS);
	}
	// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	else if ((b & MASK5BYTES) == MASK5BYTES)
	{
		size = 5;
		ch = ((b & 0x03) << 24) | ((p[1] & MASKBITS) << 18) | ((p[2] & MASKBITS) << 12) | ((p[3] & MASKBITS) << 6) | (p[4] & MASKBITS);
	}
	// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	else if ((b & MASK4BYTES) == MASK4BYTES)
	{
		size = 4;
		ch = ((b & 0x07) << 18) | ((p[1] & MASKBITS) << 12) | ((p[2] & MASKBITS) << 6) | (p[3] & MASKBITS);
	}
	// 1110xxxx 10xxxxxx 10xxxxxx
	else if ((b & MASK3BYTES) == MASK3BYTES)
	{
		size = 3;
		ch = ((b & 0x0F) << 12) | ((p[1] & MASKBITS) << 6) | (p[2] & MASKBITS);
	}
	// 110xxxxx 10xxxxxx
	else if ((b & MASK2BYTES) == MASK2BYTES)
	{
		size = 2;
		ch = ((b & 0x1F) << 6) | (p[1] & MASKBITS);
	}
	// 0xxxxxxx
	else if (b < MASKBYTE)
	{
		ch = b;
	}

	if (size > iLargeUTF8)
		iLargeUTF8 = size;

	*skip = size;

	return ch;
}

void Dictionary::WriteUTF8Char(const char *p, FILE *fp)
{
	u8 b = p[0];

	if (b == 0)
		return;

	if ((b & MASK6BYTES) == MASK6BYTES)
	{
		fputc(p[0], fp);
		fputc(p[1], fp);
		fputc(p[2], fp);
		fputc(p[3], fp);
		fputc(p[4], fp);
		fputc(p[5], fp);
	}
	else if ((b & MASK5BYTES) == MASK5BYTES)
	{
		fputc(p[0], fp);
		fputc(p[1], fp);
		fputc(p[2], fp);
		fputc(p[3], fp);
		fputc(p[4], fp);
	}
	else if ((b & MASK4BYTES) == MASK4BYTES)
	{
		fputc(p[0], fp);
		fputc(p[1], fp);
		fputc(p[2], fp);
		fputc(p[3], fp);
	}
	else if ((b & MASK3BYTES) == MASK3BYTES)
	{
		fputc(p[0], fp);
		fputc(p[1], fp);
		fputc(p[2], fp);
	}
	else if ((b & MASK2BYTES) == MASK2BYTES)
	{
		fputc(p[0], fp);
		fputc(p[1], fp);
	}
	else if (b < MASKBYTE)
	{
		fputc(p[0], fp);
	}
}
