#include "../defines.h"
#include "wiiplatform.h"
#include "../exporter.h"
#include "../utils.h"
#include "../object/sprite.h"
#include "../log.h"

#include <string>
#include <sstream>

#define PLATFORM_WII_OUTPUT_BUTTON_EXT			".button"
#define PLATFORM_WII_OUTPUT_FONT_EXT			".font"
#define PLATFORM_WII_OUTPUT_SPRITE_EXT			".sprite"
#define PLATFORM_WII_OUTPUT_IMG_EXT				".tpl"
#define PLATFORM_WII_OUTPUT_TSF_IMG_EXT			".tsf"
#define PLATFORM_WII_OUTPUT_SOUNDRESOURCE_EXT	".adpcm"
#define PLATFORM_WII_OUTPUT_MUSICRESOURCE_EXT	".adpcm"
#define PLATFORM_WII_OUTPUT_SOUND_EXT			".sound"
#define PLATFORM_WII_OUTPUT_MUSIC_EXT			".music"
#define PLATFORM_WII_OUTPUT_MASK_EXT			".bitmask"
#define PLATFORM_WII_OUTPUT_MAP_EXT					".map"

#define PLATFORM_WII_INPUT_IMG_EXT			".tga"
#define PLATFORM_WII_INPUT_SOUND_EXT		".aif"
#define PLATFORM_WII_INPUT_MUSIC_EXT		".aif"

#ifdef WIN32
	//#define LZFTOOL								"lzftool.exe big"
	#define COMPRESSTOOL							"ntcompress.exe -lh -s"
	#define TEXCONV									"texconv.exe"
	#define DSPADPCM								"dspadpcm.exe"
	#define PLATFORM_WII_FONTGEN_COMMAND			"fontgen.exe"
#else
	#ifdef POSIX
		//#define LZFTOOL								"lzftool big"
		#define COMPRESSTOOL							"ntcompress -lh -s"
		#define TEXCONV									"texconv"
		#define DSPADPCM								"dspadpcm"
		#define PLATFORM_WII_FONTGEN_COMMAND			"fontgen"
	#endif
#endif


#define	TAG		"[WiiPlatform] "

void WiiPlatform::Compile(IResource *obj)
{
	switch (obj->GetType())
	{
		case RESOURCE_IMAGE:
			this->Compile(static_cast<const Image *>(obj));
		break;

		case RESOURCE_MUSIC:
			this->Compile(static_cast<const MusicResource *>(obj));
		break;

		case RESOURCE_SOUND:
			this->Compile(static_cast<const SoundResource *>(obj));
		break;

		case RESOURCE_MASK:
			this->Compile(static_cast<Mask *>(obj));
		break;

		default:
		break;
	}
}

void WiiPlatform::Compile(IObject *obj)
{
	switch (obj->GetType())
	{
		case OBJECT_SPRITE:
			this->Compile(static_cast<Sprite *>(obj));
		break;
/*
		case OBJECT_IMAGE:
			this->Compile(static_cast<Image *>(obj));
		break;
*/
/*
		case OBJECT_AUDIO:
			this->Compile(static_cast<Audio *>(obj));
		break;
*/
		case OBJECT_ANIMATION:
			this->Compile(static_cast<Sprite *>(obj));
		break;

		case OBJECT_FRAME:
			this->Compile(static_cast<Frame *>(obj));
		break;
/*
		case OBJECT_MUSIC:
			this->Compile(static_cast<Audio *>(obj));
		break;

		case OBJECT_SOUND:
			this->Compile(static_cast<Audio *>(obj));
		break;
*/
		case OBJECT_FONT:
			this->Compile(static_cast<Font *>(obj));
		break;

		case OBJECT_BUTTON:
			this->Compile(static_cast<const Button *>(obj));
		break;

		default:
			// TODO: hmm?
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

void WiiPlatform::Compile(Dictionary *obj)
{
}

void WiiPlatform::Compile(Sprite *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());

	std::ostringstream files;
	std::ostringstream alphas;
	std::map<const char*, int, LowerThanStringComparator> frames;

	int id = 0;
	int as = obj->GetSize();
	for (int a = 0; a < as; a++)
	{
		Animation *anim = obj->GetAnimation(a);
		int fs = anim->GetSize();
		for (int f = 0; f < fs; f++)
		{
			const Frame *frame = anim->GetFrame(f);

			if (frames.find(frame->GetFilename()) == frames.end())
			{
				frames[frame->GetFilename()] = 1;
				files << "file " << id << " = " << frame->GetFilename() << ".tga\n";

				if (!frame->HasAlpha())
					alphas << "image " << id << " = " << id << ", x, RGBA8\n";
				else
					alphas << "image " << id << " = " << id << ", " << id << ", RGBA8\n";

				id++;
			}
		}
	}

	for (int i = 0; i < id; i++)
	{
		char tex[32];
		snprintf(tex, 32, "texture %d = %d, x\n", id, id);
	}

	std::ostringstream fileTmp;
	fileTmp << "tmp" << PLATFORM_PATH_SEPARATOR_STR << "tmp.tcs";

	FILE *fp = fopen(fileTmp.str().c_str(), "w+");
	fprintf(fp, "path = %s\\\n", e->GetInputPath(RESOURCE_IMAGE).string().c_str());
	fprintf(fp, "%s", files.str().c_str());
	fprintf(fp, "%s", alphas.str().c_str());
	for (int i = 0; i < id; i++)
		fprintf(fp, "texture %d = %d, x\n", i, i);
	fclose(fp);

	// xunxo mode on
	/*std::ostringstream tplOut;
	tplOut << e->GetOutputPath() << PLATFORM_PATH_SEPARATOR_STR << obj->GetName() << PLATFORM_WII_OUTPUT_IMG_EXT;
	pCache->AddFilename(tplOut.str().c_str());*/

	std::ostringstream cmd;
	cmd << this->GetName() << PLATFORM_PATH_SEPARATOR_STR << "texconv tmp" << PLATFORM_PATH_SEPARATOR_STR << "tmp.tcs ";
	cmd << e->GetOutputPath() << PLATFORM_PATH_SEPARATOR_STR << obj->GetName() << PLATFORM_WII_OUTPUT_IMG_EXT;

	BOOL valid = TRUE;
	for (u32 i = 0; i < obj->GetSize(); i++)
	{
		Animation *anim = obj->GetAnimation(i);
		if (!anim)
			continue;

		for (u32 j = 0; j < anim->GetSize(); j++)
		{
			Frame *frame = const_cast<Frame*>(anim->GetFrame(j));
			if (!frame)
				continue;

			// a regular 1024x1024x32 TGA texture has a size of 4MB
			const Image *img = frame->GetImage();
			if (bfs::file_size(img->GetInputPath()) / (1024 * 1024) >= 4)
			{
				valid = FALSE;
				break;
			}
		}

		if (!valid)
			break;
	}

	if (valid)
	{
		std::ostringstream tplOut;
		tplOut << e->GetOutputPath() << PLATFORM_PATH_SEPARATOR_STR << obj->GetName() << PLATFORM_WII_OUTPUT_IMG_EXT;
		pCache->AddFilename(tplOut.str().c_str());

		RUN_COMMAND(cmd);
	}

	// xm off


	if (e->IsCompressionEnabled() && valid)
	{
		bfs::path wiiToolsPath(this->GetName());
		bfs::path pathNewExtension = obj->GetOutputPath();
		pathNewExtension.replace_extension(".lh");

		std::ostringstream foo;
		foo << e->GetOutputPath() << PLATFORM_PATH_SEPARATOR_STR << obj->GetName() << PLATFORM_WII_OUTPUT_IMG_EXT;
		bfs::path inputTPL(foo.str());

		cmd.str("");
		//cmd << (wiiToolsPath / LZFTOOL).file_string() << " \"";
		cmd << (wiiToolsPath / COMPRESSTOOL).file_string() << " \"";
		cmd << foo.str();
		cmd << "\" -o \"" << pathNewExtension << "\"";

		RUN_COMMAND(cmd);

		bfs::remove(inputTPL);
		bfs::rename(pathNewExtension, inputTPL);
	}
}

void WiiPlatform::Compile(Animation *obj)
{
}

void WiiPlatform::Compile(Frame *obj)
{
}

void WiiPlatform::Compile(Image *obj)
{
	/*if (obj->GetFormat() == TSFCONV)
	{
		if (e->IsCompressionEnabled())
		{
			fprintf(stderr, "TSF compression not iPLATFORM_WII_OUTPUT_IMG_EXTmplemented yet...\n");
			HALT;
		}
		fprintf(stdout, "[TSFCONV] image: %s\n", ((obj->GetName() && strlen(obj->GetName())) ? obj->GetName() : "noname"));

		TSFHeader tsfHeader = obj->GetTSFHeader();

		bfs::path tsfconv(this->GetName());
		tsfconv /= "tsfconv";

		bfs::path input(e->GetInputPath(RESOURCE_IMAGE));
		input /= obj->GetFilename();

		bfs::path output(e->GetOutputPath());
		std::string outStr(obj->GetFilename());
		outStr = outStr + ".tsf";PLATFORM_WII_OUTPUT_IMG_EXT

		output /= outStr;

		std::ostringstream cmd;
		cmd << tsfconv.file_string() << " -i " << "\"" << input << "\"";
		cmd << " -o " << "\"" << output << "\"";
		cmd << " -w " << tsfHeader.width << " -h " << tsfHeader.height << " -f " << tsfHeader.frames;
		cmd << " -g " << tsfHeader.globalFrameTime;

		if (tsfHeader.flags & TSF_LOOP)
			cmd << " -l";

		if (tsfHeader.flags & TSF_ANIMATED)
			cmd << " -a";

		cmd << " -c " << tsfHeader.colorKey;
		cmd << " -p 1";

		DebugInfo("CMD: %s\n", cmd.str().c_str());
		system(cmd.str().c_str());
	}
	else
	{
		DebugInfo("[TEXCONV] image: %s\n", ((obj->GetFilename() && strlen(obj->GetFilename())) ? obj->GetFilename() : "noname"));

		bfs::path wiiToolsPath(this->GetName());

		bfs::path tmp("tmp");
		tmp /= "tmp.tcs";

		std::ostringstream cmd;
		cmd << (wiiToolsPath / TEXCONV).file_string() << " " << tmp << " ";
		cmd << "\"" << obj->GetOutputPath() << "\"";

		char script[256];
		snprintf(script, 256, "path = %s\\\nfile 0 = %s.tga\nimage 0 = 0, %c, RGBA8\ntexture 0 = 0, x", e->GetInputPath(RESOURCE_IMAGE).string().c_str(), obj->GetFilename(), (obj->HasAlpha() ? '0' : 'x'));

		FILE *fp = fopen(tmp.string().c_str(), "w+");
		fprintf(fp, script);
		fclose(fp);

		DebugInfo("CMD: %s\n", cmd.str().c_str());
		u32 cmdRetCode = 0;
		cmdRetCode = system(cmd.str().c_str());
		if (cmdRetCode)
		{
			fprintf(stderr, "%s didn't execute properly. Return code: %d.\n", TSFCONV, cmdRetCode);
			exit(EXIT_FAILURE);
		}

		if (e->IsCompressionEnabled())
		{
			bfs::path pathNewExtension = obj->GetOutputPath();
			pathNewExtension.replace_extension(".lzf");

			cmd.str("");
			cmd << (wiiToolsPath / LZFTOOL).file_string() << " \"" << obj->GetOutputPath() << "\" \"" << pathNewExtension << "\"";

			DebugInfo("CMD: %s\n", cmd.str().c_str());
			cmdRetCode = system(cmd.str().c_str());
			if (cmdRetCode)
			{
				fprintf(stderr, "\n%s didn't execute properly. Return code: %d.\n", LZFTOOL, cmdRetCode);
				//exit(EXIT_FAILURE);
			}

			bfs::remove(obj->GetOutputPath());
			bfs::rename(pathNewExtension, obj->GetOutputPath());
		}
	}*/
}

void WiiPlatform::Compile(Sound *obj)
{
}

void WiiPlatform::Compile(Music *obj)
{
}

void WiiPlatform::Compile(MusicResource *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
	/*bfs::path dspadpcm(this->GetName());
	dspadpcm /= "dspadpcm";*/
	bfs::path wiiToolPath(this->GetName());

	bfs::path musicInpPath(e->GetInputPath(RESOURCE_MUSIC));

	// L
	bfs::path musicLOutPath;
	{
		std::string musicL(obj->GetFilename());
		musicL = musicL + "_L";
		bfs::path musicLInpPath = musicInpPath / (musicL + std::string(".aif"));

		if (!bfs::exists(musicLInpPath))
		{
			Log(TAG "File %s not found, falling back to .wav and converting it to platform specific format.", musicLInpPath.string().c_str());
			bfs::path tmp = musicInpPath / (musicL + ".wav");

			if (!bfs::exists(tmp))
			{
				Error(ERROR_FILE_NOT_FOUND, TAG "Music audio file '%s' not found.", tmp.string().c_str());
			}
			else
			{
				std::ostringstream cmd;
				cmd << this->GetName() << PLATFORM_PATH_SEPARATOR_STR << "sox ";
				cmd << tmp.string().c_str() << " -c 1 -r 44100 -b 16 ";
				cmd << musicLInpPath.string().c_str();

				RUN_COMMAND(cmd);
			}
		}

		musicLOutPath = e->GetOutputPath() / (musicL + std::string(".adpcm"));

		std::ostringstream cmd;
		//cmd << dspadpcm.file_string() << " -e -v ";
		cmd << (wiiToolPath / DSPADPCM).file_string() << " -e -v ";
		cmd << "\"" << musicLInpPath << "\"" << " ";
		cmd << musicLOutPath;

		RUN_COMMAND(cmd);
	}

	// R
	bfs::path musicROutPath;
	{
		std::string musicR(obj->GetFilename());
		musicR = musicR + "_R";

		bfs::path musicRInpPath(e->GetInputPath(RESOURCE_MUSIC));
		musicRInpPath /= (musicR + std::string(".aif"));

		if (!bfs::exists(musicRInpPath))
		{
			Log(TAG "File %s not found, falling back to .wav and converting it to platform specific format.", musicRInpPath.string().c_str());
			bfs::path tmp = musicInpPath / (musicR + ".wav");

			if (!bfs::exists(tmp))
			{
				Error(ERROR_FILE_NOT_FOUND, TAG "Music audio file '%s' not found.", tmp.string().c_str());
			}
			else
			{
				std::ostringstream cmd;
				cmd << this->GetName() << PLATFORM_PATH_SEPARATOR_STR << "sox ";
				cmd << tmp.string().c_str() << " -c 1 -r 44100 -b 16 ";
				cmd << musicRInpPath.string().c_str();

				RUN_COMMAND(cmd);
			}
		}

		musicROutPath = e->GetOutputPath() / (musicR + std::string(".adpcm"));

		std::ostringstream cmd;
		//cmd << dspadpcm.file_string() << " -e -v ";
		cmd << (wiiToolPath / DSPADPCM).file_string() << " -e -v ";
		cmd << "\"" << musicRInpPath << "\"" << " ";
		cmd << musicROutPath;

		RUN_COMMAND(cmd);
	}

	/*if (e->IsCompressionEnabled())
	{
		bfs::path pathNewExtension = musicLOutPath;
		pathNewExtension.replace_extension(".lzf");
		cmd.str("");
		cmd << (wiiToolPath / LZFTOOL).file_string() << " \"" << musicLOutPath << "\" \"" << pathNewExtension << "\"";

		DebugInfo("CMD: %s\n", cmd.str().c_str());
		cmdRetCode = system(cmd.str().c_str());
		if (cmdRetCode)
		{
			fprintf(stderr, "%s didn't execute properly. Return code: %d.\n", LZFTOOL, cmdRetCode);
			//exit(EXIT_FAILURE);
		}

		pathNewExtension = musicROutPath;
		pathNewExtension.replace_extension(".lzf");
		cmd.str("");
		cmd << (wiiToolPath / LZFTOOL).file_string() << " \"" << musicROutPath << "\" \"" << pathNewExtension << "\"";

		DebugInfo("CMD: %s\n", cmd.str().c_str());
		cmdRetCode = system(cmd.str().c_str());
		if (cmdRetCode)
		{
			fprintf(stderr, "%s didn't execute properly. Return code: %d.\n", LZFTOOL, cmdRetCode);
			//exit(EXIT_FAILURE);
		}
	}*/

	obj->AddFilePath(musicLOutPath);
	obj->AddFilePath(musicROutPath);
}

void WiiPlatform::Compile(SoundResource *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
	/*bfs::path dspadpcm(this->GetName());
	dspadpcm /= "dspadpcm";*/
	bfs::path wiiToolPath(this->GetName());

	std::ostringstream cmd;
	//cmd << dspadpcm.file_string() << " -e -v " << "\"" << obj->GetInputPath() << "\"";
	cmd << (wiiToolPath / DSPADPCM).file_string() << " -e -v " << "\"" << obj->GetInputPath() << "\"";
	cmd << " " << "\"" << obj->GetOutputPath() << "\"";
	RUN_COMMAND(cmd);

	/*if (e->IsCompressionEnabled())
	{
		bfs::path pathNewExtension = obj->GetOutputPath();
		pathNewExtension.replace_extension(".lzf");
		cmd.str("");
		cmd << (wiiToolPath / LZFTOOL).file_string() << " \"" << obj->GetOutputPath() << "\" \"" << pathNewExtension << "\"";

		DebugInfo("CMD: %s\n", cmd.str().c_str());
		cmdRetCode = system(cmd.str().c_str());
		if (cmdRetCode)
		{
			fprintf(stderr, "%s didn't execute properly. Return code: %d.\n", LZFTOOL, cmdRetCode);
			//exit(EXIT_FAILURE);
		}
	}*/
}

void WiiPlatform::Compile(Font *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
	IResource *res = const_cast<IResource *>(obj->GetResource());
	Image *img = static_cast<Image *>(res);

	std::ostringstream cmd;
	bfs::path toolPath(this->GetName());
	toolPath /= PLATFORM_WII_FONTGEN_COMMAND;

	cmd << toolPath.file_string() << " \"";
	cmd << obj->GetInputPath() << "\"";
	cmd << " \"" << obj->GetName() << "\" " << obj->GetCharacters();

	if (obj->IsUsingAtlas())
	{
		cmd << " \"" << res->GetFilename() << "\" " << img->GetX() << " " << img->GetY();
	}
	RUN_COMMAND(cmd);

	// current cache dump
	pCache->Dump();

	bfs::path fontXML("tmp/font.xml");

	cmd.str("");
	cmd << e->bfsExeName.string() << " -i " << fontXML << " -p " << this->GetName();
	RUN_COMMAND(cmd);

	// updated cache reload
	pCache->Reset();
	pCache->Load();

	bfs::path fontSprite = obj->GetOutputPath();
	fontSprite.replace_extension(".sprite");
	bfs::remove(fontXML);

	obj->SetSprite(fontSprite);
	obj->AddFilePath(fontSprite);

	/*if (e->IsCompressionEnabled())
	{
		// .font
		bfs::path pathNewExtension = obj->GetOutputPath();
		pathNewExtension.replace_extension(".lzf");
		cmd.str("");
		cmd << (wiiToolPath / LZFTOOL).file_string() << " \"" << obj->GetOutputPath() << "\" \"" << pathNewExtension << "\"";

		DebugInfo("CMD: %s\n", cmd.str().c_str());
		cmdRetCode = system(cmd.str().c_str());
		if (cmdRetCode)
		{
			fprintf(stderr, "%s didn't execute properly. Return code: %d.\n", LZFTOOL, cmdRetCode);
			//exit(EXIT_FAILURE);
		}

		bfs::remove(obj->GetOutputPath());
		bfs::rename(pathNewExtension, obj->GetOutputPath());

		// .sprite
		pathNewExtension = fontSprite;
		pathNewExtension.replace_extension(".lzf");

		cmd.str("");
		cmd << (wiiToolPath / LZFTOOL).file_string() << " \"" << fontSprite << "\" \"" << pathNewExtension << "\"";

		DebugInfo("CMD: %s\n", cmd.str().c_str());
		cmdRetCode = system(cmd.str().c_str());
		if (cmdRetCode)
		{
			fprintf(stderr, "%s didn't execute properly. Return code: %d.\n", LZFTOOL, cmdRetCode);
			//exit(EXIT_FAILURE);
		}

		bfs::remove(fontSprite);
		bfs::rename(pathNewExtension, fontSprite);
	}*/

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
			//IResource *ext = e->GetResource(extname.str().c_str());

			std::string extname(res->GetName());
			extname += "_ext";
			IResource *ext = e->GetResource(extname.c_str(), dict->pcGetLanguage());

			//IResource *ext = e->GetResource(res->GetName(), dict->pcGetLanguage());
			if (ext)
			{
				std::string x(res->GetFilename());
				x += "_ext.tpl";
				bfs::path xx(e->GetOutputPath());
				xx /= x;
				pCache->AddFilename(xx.string().c_str());
				obj->SetExtId(pCache->GetFilenameId(xx.string().c_str()));

				x = res->GetFilename();
				x += "_ext.sprite";
				xx = e->GetOutputPath();
				xx /= x;
				pCache->AddFilename(xx.string().c_str());
				obj->SetExtId(pCache->GetFilenameId(xx.string().c_str()));


				bfs::path sprite = this->ProcessFont(obj, ext, size);
				obj->AddFilePath(sprite);
			}
			else
			{
				//Error(ERROR_UNKNOWN, TAG "Needed language '%s' resource extension table %s for font %s not found.", dict->pcGetLanguage(), extname.c_str(), obj->GetName());
				Error(ERROR_UNKNOWN, TAG "Needed language '%s' specific resource for font %s not found.", dict->pcGetLanguage(), obj->GetName());
			}
		}
	}
}

bfs::path WiiPlatform::ProcessFont(Font *obj, IResource *res, u32 totalChars)
{
	bfs::create_directories(res->GetOutputPath().parent_path());
	Image *img = static_cast<Image *>(res);

	/* build basic font */
	std::ostringstream cmd;
	bfs::path toolPath(this->GetName());
	toolPath /= PLATFORM_WII_FONTGEN_COMMAND;

	cmd << toolPath.file_string() << " \"";
	cmd << res->GetInputPath() << "\"";

	const char *lang = res->GetLanguage();
	cmd << " \"l10n/" << lang << "/" << obj->GetName() << "_ext\" " << totalChars;
	//cmd << " \"" << res->GetName() << "\" " << totalChars;
	//cmd << " \"" << name << "\" " << totalChars;

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
	fontSprite = res->GetOutputPath();
	fontSprite.replace_extension(".sprite");
	//bfs::remove(fontXML);

	return fontSprite;
}

void WiiPlatform::ConvertSound(bfs::path filename, SoundResource *obj)
{
	std::ostringstream cmd;

	cmd << this->GetName() << PLATFORM_PATH_SEPARATOR_STR << "sox ";
	cmd << filename.string().c_str() << " -b 16 ";
	cmd << obj->GetInputPath().string().c_str();

	RUN_COMMAND(cmd);
}

void WiiPlatform::Compile(Button *obj)
{
	bfs::create_directories(obj->GetOutputPath().parent_path());
}

void WiiPlatform::Compile(Mask *obj)
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

bfs::path WiiPlatform::GetOutputPath(const IResource *res) const
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
			out = out + PLATFORM_WII_OUTPUT_IMG_EXT;
		}
		break;

		case RESOURCE_MASK:
		{
			out = out + PLATFORM_WII_OUTPUT_MASK_EXT;
		}
		break;

		case RESOURCE_MUSIC:
		{
			out = out + PLATFORM_WII_OUTPUT_MUSICRESOURCE_EXT;
		}
		break;

		case RESOURCE_SOUND:
		{
			out = out + PLATFORM_WII_OUTPUT_SOUNDRESOURCE_EXT;
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

bfs::path WiiPlatform::GetInputPath(const IResource *res) const
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
			bfs::path f = tmp / (inp + ".tga");
			if (bfs::exists(f))
			{
				inp = inp + ".tga";
			}
			else
			{
				Error(ERROR_FILE_NOT_FOUND, "Input: File %s.tga not found at %s", res->GetFilename(), res->GetFilename(), tmp.string().c_str());
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
			inp = inp + PLATFORM_WII_INPUT_MUSIC_EXT;
		}
		break;

		case RESOURCE_SOUND:
		{
			inp = inp + PLATFORM_WII_INPUT_SOUND_EXT;
		}
		break;

		default:
			Error(ERROR_EXPORT_RESOURCE_INVALID_TYPE, "WARNING: Resource type not known for resource %s.", res->GetName());
		break;
	}

	tmp /= inp;

	return tmp;
}

bfs::path WiiPlatform::GetOutputPath(const IObject *obj) const
{
	std::string out(obj->GetName());
	bfs::path tmp = e->GetOutputPath();

	switch (obj->GetType())
	{
		case OBJECT_FONT:
		{
			out = out + PLATFORM_WII_OUTPUT_FONT_EXT;
		}
		break;

		case OBJECT_SOUND:
		{
			out = out + PLATFORM_WII_OUTPUT_SOUND_EXT;
		}
		break;

		case OBJECT_MUSIC:
		{
			out = out + PLATFORM_WII_OUTPUT_MUSIC_EXT;
		}
		break;

		case OBJECT_SPRITE:
		{
			out = out + PLATFORM_WII_OUTPUT_SPRITE_EXT;
		}
		break;

		case OBJECT_BUTTON:
		{
			out = out + PLATFORM_WII_OUTPUT_BUTTON_EXT;
		}
		break;

		case OBJECT_MAP:
		{
			out = out + PLATFORM_WII_OUTPUT_MAP_EXT;
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

bfs::path WiiPlatform::GetInputPath(const IObject *obj) const
{
	std::string inp(obj->GetName());
	bfs::path tmp;

	switch (obj->GetType())
	{
		case OBJECT_FONT:
		{
			tmp = e->GetInputPath(RESOURCE_IMAGE);
			inp = inp + PLATFORM_WII_INPUT_IMG_EXT; // PLATFORM_WII_INPUT_FONT_EXT ??????
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
