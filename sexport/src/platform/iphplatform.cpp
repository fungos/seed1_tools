#include "../defines.h"
#include "iphplatform.h"
#include "../exporter.h"
#include "../object/sprite.h"
#include "../log.h"
#include "../string.h"
#include "../cache.h"
#include "../utils.h"

#include <string>
#include <sstream>

#define PLATFORM_IPH_OUTPUT_FONT_EXT			".font"
#define PLATFORM_IPH_OUTPUT_SPRITE_EXT			".sprite"
#define PLATFORM_IPH_OUTPUT_IMG_EXT				".png"
#define PLATFORM_IPH_OUTPUT_IMG_EXT_COMPRESSED 	".pvrtc"
#define PLATFORM_IPH_OUTPUT_TSF_IMG_EXT			".tsf"
#define PLATFORM_IPH_OUTPUT_SOUNDRESOURCE_EXT	".caf"
#define PLATFORM_IPH_OUTPUT_MUSICRESOURCE_EXT	".m4a"
#define PLATFORM_IPH_OUTPUT_SOUND_EXT			".sound"
#define PLATFORM_IPH_OUTPUT_MUSIC_EXT			".music"
#define PLATFORM_IPH_OUTPUT_BUTTON_EXT			".button"
#define PLATFORM_IPH_OUTPUT_MASK_EXT			".bitmask"
#define PLATFORM_IPH_OUTPUT_MAP_EXT					".map"

#define PLATFORM_IPH_INPUT_FONT_EXT			".tga"
#define PLATFORM_IPH_INPUT_IMG_EXT			".png"
#define PLATFORM_IPH_INPUT_IMG_EXT_COMPRESSED ".pvrtc"
#define PLATFORM_IPH_INPUT_SOUND_EXT		".wav" // ".aif"
#define PLATFORM_IPH_INPUT_MUSIC_EXT		".wav"

#define TAG "[Compiler] "

#ifdef WIN32
	#define PLATFORM_IPH_COPY_COMMAND			"copy"
	#define PLATFORM_IPH_COPY_OVERWRITE_PARAM	"/Y"
	#define PLATFORM_IPH_FONTGEN_COMMAND		"fontgen.exe"
	#define PLATFORM_IPH_TMPDIR			"iph\\"
#else
	#ifdef POSIX
		#define PLATFORM_IPH_COPY_COMMAND			"cp -f "
		#define PLATFORM_IPH_COPY_OVERWRITE_PARAM	""
		#define PLATFORM_IPH_FONTGEN_COMMAND		"fontgen"
		#define PLATFORM_IPH_TMPDIR			"tmp/"
	#endif
#endif

void IphPlatform::Compile(IResource *obj)
{
	switch (obj->GetType())
	{
		case RESOURCE_IMAGE:
			this->Compile(static_cast<Image *>(obj));
		break;

		case RESOURCE_MUSIC:
			this->Compile(static_cast<MusicResource *>(obj));
		break;

		case RESOURCE_SOUND:
			this->Compile(static_cast<SoundResource *>(obj));
		break;

		case RESOURCE_MASK:
			this->Compile(static_cast<Mask *>(obj));
		break;

		default:
		break;
	}
}

void IphPlatform::Compile(IObject *obj)
{
	switch (obj->GetType())
	{
		case OBJECT_SPRITE:
			this->Compile(static_cast<Sprite *>(obj));
		break;
/*
		case OBJECT_IMAGE:
			this->Compile(static_cast<const Image *>(obj));
		break;
*/
		case OBJECT_ANIMATION:
			this->Compile(static_cast<Sprite *>(obj));
		break;

		case OBJECT_FRAME:
			this->Compile(static_cast<Frame *>(obj));
		break;

		case OBJECT_MUSIC:
			this->Compile(static_cast<const Music *>(obj));
		break;

		case OBJECT_SOUND:
			this->Compile(static_cast<const Sound *>(obj));
		break;

		case OBJECT_FONT:
			this->Compile(static_cast<const Font *>(obj));
		break;

		case OBJECT_BUTTON:
			this->Compile(static_cast<const Button *>(obj));
		break;

		default:
		break;
	}
}

struct LowerThanStringComparator
{
	BOOL operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

void IphPlatform::Compile(Dictionary *obj)
{
}

void IphPlatform::Compile(Sprite *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
}

void IphPlatform::Compile(Animation *obj)
{
}

void IphPlatform::Compile(Image *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());

	std::ostringstream cmd;
	cmd << PLATFORM_IPH_COPY_COMMAND << " \"" << obj->GetInputPath().directory_string() << "\"";
	cmd << " \"" << obj->GetOutputPath().directory_string() << "\"";
	cmd << " " << PLATFORM_IPH_COPY_OVERWRITE_PARAM;
	RUN_COMMAND(cmd);

	DebugInfo("IMAGE: %s\n", cmd.str().c_str());
}

void IphPlatform::Compile(Frame *obj)
{
}

void IphPlatform::Compile(Font *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
	IResource *res = const_cast<IResource *>(obj->GetResource());
	Image *img = static_cast<Image *>(res);

	std::ostringstream cmd;
	bfs::path toolPath(this->GetName());
	toolPath /= PLATFORM_IPH_FONTGEN_COMMAND;

	cmd << toolPath.file_string() << " \"";
	cmd << obj->GetInputPath() << "\"";
	cmd << " \"" << obj->GetName() << "\" " << obj->GetCharacters() << " ";
	cmd << obj->GetGlyphWidth() << " " << obj->GetGlyphHeight();

	if (obj->IsUsingAtlas())
	{
		cmd << " \"" << res->GetFilename() << "\" " << img->GetX() << " " << img->GetY();
	}
	RUN_COMMAND(cmd);

	// current cache dump
	pCache->Dump();

	bfs::path fontXML("tmp/font.xml");

	cmd.str("");
	cmd << e->bfsExeName.string() << " -i " << fontXML << " -p " << this->GetName() << " -v ";
	RUN_COMMAND(cmd);

	// updated cache reload
	pCache->Reset();
	pCache->Load();

	bfs::path fontSprite = obj->GetOutputPath();
	fontSprite.replace_extension(".sprite");
	//bfs::remove(fontXML);

	obj->SetSprite(fontSprite);
	obj->AddFilePath(fontSprite);

	/* build extension tables for languages */
	DictionaryMap dict = e->GetDictionaries();
	DictionaryMapIterator it = dict.begin();
	DictionaryMapIterator end = dict.end();
	for (; it != end; ++it)
	{
		Dictionary *dict = (*it).second;
		u32 size = dict->GetExtensionTableSize();
		if (size)
		{
			//std::ostringstream extname;
			//extname << "l10n/" << dict->pcGetLanguage() << "/" << obj->GetFilename() << "_ext";
			//extname << res->GetName() << "_ext";

			std::string extname(res->GetName());
			extname += "_ext";
			IResource *ext = e->GetResource(extname.c_str(), dict->pcGetLanguage());

			//IResource *ext = e->GetResource(res->GetName(), dict->pcGetLanguage());
			if (ext)
			{
				//std::string x(res->GetFilename());
				std::ostringstream x;
				//x << "l10n/" << dict->pcGetLanguage() << "/" << res->GetName() << "_ext.tga";
				x << res->GetName() << "_ext.tga";
				bfs::path xx(e->GetOutputPath());
				xx /= x.str();
				//pCache->AddFilename(xx.string().c_str());
				obj->SetExtId(pCache->GetFilenameId(xx.string().c_str()));

				std::ostringstream x2;
				//x2 << "l10n/" << dict->pcGetLanguage() << "/" << res->GetName() << "_ext.sprite";
				x2 << res->GetName() << "_ext.sprite";
				xx = e->GetOutputPath();
				xx /= x2.str();
				pCache->AddFilename(xx.string().c_str());
				obj->SetExtId(pCache->GetFilenameId(xx.string().c_str()));


				//bfs::path sprite = this->ProcessFont(obj, ext, size);
				//obj->AddFilePath(sprite);
				this->ProcessFont(obj, ext, size);
				std::ostringstream x3;
				x3 << "l10n/" << dict->pcGetLanguage() << "/" << res->GetName() << "_ext.sprite";
				xx = e->GetOutputPath();
				xx /= x3.str();
				obj->AddFilePath(xx.string().c_str());
			}
			else
			{
				//Error(ERROR_UNKNOWN, TAG "Needed language '%s' resource extension table %s for font %s not found.", dict->pcGetLanguage(), extname.c_str(), obj->GetName());
				//Error(ERROR_UNKNOWN, TAG "Needed language '%s' specific resource for font %s not found.", dict->pcGetLanguage(), obj->GetName());
			}
		}
	}
}

bfs::path IphPlatform::ProcessFont(Font *obj, IResource *res, u32 totalChars)
{
	bfs::create_directories(res->GetOutputPath().parent_path());
	Image *img = static_cast<Image *>(res);

	/* build basic font */
	std::ostringstream cmd;
	bfs::path toolPath(this->GetName());
	toolPath /= PLATFORM_IPH_FONTGEN_COMMAND;

	cmd << toolPath.file_string() << " \"";
	//cmd << res->GetInputPath() << "\"";
	const char *lang = res->GetLanguage();
	cmd << e->GetInputPath(RESOURCE_IMAGE).string() << "/l10n/" << lang << "/" << obj->GetName() << "_ext.tga\" ";

	cmd << " \"l10n/" << lang << "/" << obj->GetName() << "_ext\" " << totalChars << " ";
	//cmd << " \"" << res->GetName() << "\" " << totalChars;
	//cmd << " \"" << name << "\" " << totalChars;
	cmd << obj->GetGlyphWidth() << " " << obj->GetGlyphHeight();
	if (obj->IsUsingAtlas())
	{
		cmd << " \"" << res->GetFilename() << "\" " << img->GetX() << " " << img->GetY();
	}
	RUN_COMMAND(cmd);


	bfs::path fontXML("tmp/font.xml");

	cmd.str("");
	cmd << e->bfsExeName.string() << " -i " << fontXML << " -p " << this->GetName();
	RUN_COMMAND(cmd);

	bfs::path fontSprite;
	//fontSprite = res->GetOutputPath();
	fontSprite = res->GetInputPath();
	fontSprite.replace_extension(".sprite");
	//bfs::remove(fontXML);

	return fontSprite;
}

void IphPlatform::Compile(Sound *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
}

void IphPlatform::Compile(Music *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
}

void IphPlatform::Compile(MusicResource *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());

	std::ostringstream cmd;

	//cmd << "afconvert -f caff -d LEI16 -c 1 ";
	cmd << "afconvert -f mp4f -d aac -b 128000 ";
	cmd << obj->GetInputPath().string().c_str() << " ";
	cmd << obj->GetOutputPath().string().c_str();

	RUN_COMMAND(cmd);

	obj->AddFilePath(obj->GetOutputPath().string().c_str());
}

void IphPlatform::Compile(SoundResource *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());

	std::ostringstream cmd;

	cmd << "afconvert -f caff -d LEI16 -c 1 ";
	cmd << obj->GetInputPath().string().c_str() << " ";
	cmd << obj->GetOutputPath().string().c_str();

	RUN_COMMAND(cmd);

	obj->AddFilePath(obj->GetOutputPath().string().c_str());
}

void IphPlatform::Compile(Button *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
}

void IphPlatform::Compile(Mask *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());

	std::ostringstream cmd;

	cmd << this->GetName() << PLATFORM_PATH_SEPARATOR_STR << "maskgen ";
	cmd << obj->GetInputPath().string().c_str();
	cmd << " ";
	cmd << obj->GetOutputPath().string().c_str();
	cmd << " -p " << this->GetName();

	RUN_COMMAND(cmd);

	obj->AddFilePath(obj->GetOutputPath().string().c_str());
}

bfs::path IphPlatform::GetOutputPath(const IResource *res) const
{
	std::string out(res->GetFilename());
	bfs::path tmp = bfs::path(e->GetOutputPath().string().c_str());
	std::string lang(res->GetLanguage());
	if (lang != DEFAULT_LANG)
	{
		tmp /= bfs::path("l10n") / lang;
	}

	switch (res->GetType())
	{
		case RESOURCE_IMAGE:
		{
			bfs::path inp(e->GetInputPath(res->GetType()));
			if (lang != DEFAULT_LANG)
			{
				inp /= bfs::path("l10n") / lang;
			}

			bfs::path f = inp / (out + PLATFORM_IPH_OUTPUT_IMG_EXT);
			if (bfs::exists(f))
			{
				out = out + PLATFORM_IPH_OUTPUT_IMG_EXT;
			}
			else
			{
				Error(ERROR_FILE_NOT_FOUND, "Output: File %s.png or %s.tga not found at %s", res->GetFilename(), res->GetFilename(), inp.string().c_str());
			}
		}
		break;

		case RESOURCE_MASK:
		{
			out = out + PLATFORM_IPH_OUTPUT_MASK_EXT;
		}
		break;

		case RESOURCE_MUSIC:
		{
			out = out + PLATFORM_IPH_OUTPUT_MUSICRESOURCE_EXT;
		}
		break;

		case RESOURCE_SOUND:
		{
			out = out + PLATFORM_IPH_OUTPUT_SOUNDRESOURCE_EXT;
		}
		break;

		default:
			Error(ERROR_EXPORT_RESOURCE_INVALID_TYPE, "WARNING: Resource type not known for resource %s.", res->GetName());
		break;
	}

	tmp /= out;
	pCache->AddFilename(tmp.string().c_str());

	return tmp;
}

bfs::path IphPlatform::GetInputPath(const IResource *res) const
{
	std::string inp(res->GetFilename());
	bfs::path tmp = e->GetInputPath(res->GetType());

	std::string lang(res->GetLanguage());
	if (lang != DEFAULT_LANG)
	{
		tmp /= bfs::path("l10n") / lang;
	}

	switch (res->GetType())
	{
		case RESOURCE_IMAGE:
		{
			bfs::path f = tmp / (inp + ".png");
			if (bfs::exists(f))
			{
				inp = inp + ".png";
			}
			else
			{
				Error(ERROR_FILE_NOT_FOUND, "Input: File %s.png not found at %s", res->GetFilename(), tmp.string().c_str());
			}
		}
		break;

		case RESOURCE_MASK:
		{
			bfs::path f = tmp / (inp + ".tga");
			if (bfs::exists(f))
			{
				inp = inp + ".tga";
			}
			else
			{
				Error(ERROR_FILE_NOT_FOUND, "Input: Mask file %s.tga not found at %s", tmp.string().c_str(), res->GetFilename());
			}
		}
		break;

		case RESOURCE_MUSIC:
		{
			inp = inp + PLATFORM_IPH_INPUT_MUSIC_EXT;
		}
		break;

		case RESOURCE_SOUND:
		{
			inp = inp + PLATFORM_IPH_INPUT_SOUND_EXT;
		}
		break;

		default:
			Error(ERROR_EXPORT_RESOURCE_INVALID_TYPE, "WARNING: Resource type not known for resource %s.", res->GetName());
		break;
	}

	tmp /= inp;

	return tmp;
}

bfs::path IphPlatform::GetOutputPath(const IObject *obj) const
{
	std::string out(obj->GetName());
	bfs::path tmp = e->GetOutputPath();

	switch (obj->GetType())
	{
		case OBJECT_FONT:
		{
			out = out + PLATFORM_IPH_OUTPUT_FONT_EXT;
		}
		break;

		case OBJECT_SOUND:
		{
			out = out + PLATFORM_IPH_OUTPUT_SOUND_EXT;
		}
		break;

		case OBJECT_MUSIC:
		{
			out = out + PLATFORM_IPH_OUTPUT_MUSIC_EXT;
		}
		break;

		case OBJECT_SPRITE:
		{
			out = out + PLATFORM_IPH_OUTPUT_SPRITE_EXT;
		}
		break;

		case OBJECT_BUTTON:
		{
			out = out + PLATFORM_IPH_OUTPUT_BUTTON_EXT;
		}
		break;

		case OBJECT_MAP:
		{
			out = out + PLATFORM_IPH_OUTPUT_MAP_EXT;
		}
		break;

		default:
			Error(ERROR_EXPORT_OBJECT_INVALID_TYPE, "WARNING: Object type %d not known for object %s.", obj->GetType(), out.c_str());
		break;
	}

	tmp /= out;
	pCache->AddFilename(tmp.string().c_str());

	return tmp;
}

bfs::path IphPlatform::GetInputPath(const IObject *obj) const
{
	std::string inp(obj->GetName());
	bfs::path tmp;

	switch (obj->GetType())
	{
		case OBJECT_FONT:
		{
			tmp = e->GetInputPath(RESOURCE_IMAGE);
			inp = inp + PLATFORM_IPH_INPUT_FONT_EXT;
		}
		break;

		case OBJECT_BUTTON:
		{
		}
		break;

		default:
			Error(ERROR_EXPORT_OBJECT_INVALID_TYPE, "WARNING: Object type not known for object %s.", obj->GetName());
		break;
	}

	tmp /= inp;

	return tmp;
}
