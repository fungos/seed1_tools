#include <sstream>
#include <math.h>

#include "xml/tinyxml.h"

#include "platform.h"
#include "exporter.h"
#include "utils.h"
#include "log.h"
#include "defines.h"
#include "filesystem.h"
#include "dictionary.h"
#include "cache.h"
#include "strcache.h"

#define TAG	"[Exporter] "

/*#define OUTPUT_ID		"output"
#define INPUT_IMAGE_ID	"input_image"
#define INPUT_SOUND_ID	"input_sound"
#define INPUT_MUSIC_ID	"input_music"*/

struct Finder
{
	const char *str;
	Finder(const char *s) : str(s) {}
	bool operator()(IObject *obj) { return !strcmp(obj->GetName(), str); }
};

char *ReplaceVariable(const char *input, const char *invar)
{
	int size = 1024;

	char xmlVar[64];
	snprintf(xmlVar, 63, "$(%s)", invar);
	
	char *opath = NULL;
	const char *var = strstr(input, xmlVar);
	if (var)
	{
	    opath = (char *)malloc(sizeof(char) * size);
		assert(opath != NULL);
		memset(opath, '\0', size);

		const char *envVar = getenv(invar);
		strncpy(opath, input, input - var);
		strncat(opath, envVar, size - 1 - strlen(opath));
		strncat(opath, &var[strlen(xmlVar)], size - 1 - strlen(opath));
	}

	return opath;
}


Exporter Exporter::instance;


Exporter::Exporter()
	: bInitialized(false)
	, bRebuild(false)
	, bPackages(false)
	, bCompression(false)
	, bPackageResources(false)
	, iAlignment(0)
	, mapResourcesLang()
	, mapDictionaries()
	, vecObjects()
	, vecStringName()
	, vecPackages()
	, mapInputPath()
	, bfsOutputPath()
	, pGroupManager(NULL)
	, bfsExeName()
{
}

Exporter::~Exporter()
{
	this->Shutdown();
}

void Exporter::Initialize()
{
	this->bInitialized = true;
}

void Exporter::Shutdown()
{
	mapInputPath.clear();
	InputPathMap().swap(mapInputPath);

	this->bInitialized = false;
}

bool Exporter::Setup(TiXmlDocument *doc, const char *platform)
{
	TiXmlNode *platformNode = NULL;

	if (!strcasecmp(platform, "wii"))
		platformNode = &(*doc)("sexport")("wii");
	else if (!strcasecmp(platform, "nds"))
		platformNode = &(*doc)("sexport")("nds");
	else if (!strcasecmp(platform, "iph"))
		platformNode = &(*doc)("sexport")("iph");
	else if (!strcasecmp(platform, "sdl"))
		platformNode = &(*doc)("sexport")("sdl");

	const char *outputPath = (*platformNode)("output")["path"];
	if (!outputPath)
	{
		Log(TAG "Output path not set.");
		return false;
	}

	char *opath = NULL;
	const char *path = NULL;

	opath = ReplaceVariable(outputPath, "SEEDSDK");
	path = (opath) ? opath : outputPath;
	this->bfsOutputPath = path;
	free(opath);
	if (!bfs::is_directory(bfsOutputPath))
	{
		Log(TAG "Invalid Output path: %s.", bfsOutputPath.string().c_str());
		return false;
	}

	const char *inputSpecificPath = (*platformNode)("image")["input"];
	if (!inputSpecificPath)
	{
		Log(TAG "Image Input path not set.");
		return false;
	}

	const char *imageScreenWidth = (*platformNode)("image")["res_width"];
	if (!imageScreenWidth)
	{
		Log(TAG "Default Image Screen Resolution Width not set.");
		return false;
	}
	pPlatform->SetWidth(atoi(imageScreenWidth));

	const char *imageScreenHeight = (*platformNode)("image")["res_height"];
	if (!imageScreenHeight)
	{
		Log(TAG "Default Image Screen Resolution Height not set.");
		return false;
	}
	pPlatform->SetHeight(atoi(imageScreenHeight));

	char *ipath = NULL;
	ipath = ReplaceVariable(inputSpecificPath, "SEEDSDK");
	path = (ipath) ? ipath : inputSpecificPath;
	this->mapInputPath[RESOURCE_IMAGE] = bfs::path(path);
	free(ipath);
	if (!bfs::is_directory(mapInputPath[RESOURCE_IMAGE]))
	{
		Log(TAG "Invalid Image Input path %s.", mapInputPath[RESOURCE_IMAGE].string().c_str());
		return false;
	}

	inputSpecificPath = (*platformNode)("sound")["input"];
	if (!inputSpecificPath)
	{
		Log(TAG "Sound Input path not set.");
		return false;
	}

	char *spath = NULL;
	spath = ReplaceVariable(inputSpecificPath, "SEEDSDK");
	path = (spath) ? spath : inputSpecificPath;
	this->mapInputPath[RESOURCE_SOUND] = bfs::path(path);
	free(spath);
	if (!bfs::is_directory(mapInputPath[RESOURCE_SOUND]))
	{
		Log(TAG "Invalid Sound Input path: %s.", mapInputPath[RESOURCE_SOUND].string().c_str());
		return false;
	}

	inputSpecificPath = (*platformNode)("music")["input"];
	if (!inputSpecificPath)
	{
		Log(TAG "Music Input path not set.");
		return false;
	}

	char *mpath = NULL;
	mpath = ReplaceVariable(inputSpecificPath, "SEEDSDK");
	path = (mpath) ? mpath : inputSpecificPath;
	this->mapInputPath[RESOURCE_MUSIC] = bfs::path(path);
	free(mpath);
	if (!bfs::is_directory(mapInputPath[RESOURCE_MUSIC]))
	{
		Log(TAG "Invalid Music Input path: %s.", mapInputPath[RESOURCE_MUSIC].string().c_str());
		return false;
	}

/*
	const char *headerType = (*platformNode)("image")["header-type"];
	if (!headerType)
	{
		Log(TAG "Wrong config value: object->sprite->header-type.");
		return false;
	}
	cfg->SetSpriteHeaderType(headerType);

	const char *imageFormat = (*platformNode)("image")["image-format"];
	if (!imageFormat)
	{
		cfg->SetImageFormat("default");
	}
	cfg->SetImageFormat(imageFormat);
*/
/*
	const char *tmpAudioBuildMethod = (*platformNode)("sound")["build-method"];
	if (!tmpAudioBuildMethod)
	{
		Log(TAG "Wrong config value: sound/music build-method.");
		return false;
	}
	cfg->SetAudioBuildMethod(tmpAudioBuildMethod);
*/
/*
	headerType = (*platformNode)("sound")["header-type"];
	if (!headerType)
	{
		Log(TAG "Wrong config value: sound/music header-type.");
		return false;
	}
	cfg->SetAudioHeaderType(headerType);
*/
	return true;
}

bool Exporter::Process(const char *configfile, const char *xmlfile, const char *platformString, const bool rebuild, const bool packages, const u8 alignment, const bool compression, const bool add_resources)
{
	this->bRebuild 		= rebuild;
	this->bPackages 	= packages;
	this->bCompression 	= compression;
	this->iAlignment 	= alignment;
	this->bPackageResources  = add_resources;

	if (!packages)
		this->bPackageResources = false;

	bfs::path configPath(configfile);
	if (!bfs::exists(configPath) || !bfs::is_regular_file(configPath))
	{
		Error(ERROR_EXPORT_CONFIG_NOT_FOUND, TAG "Error opening config file: config.xml.");
	}

	TiXmlDocument config(configPath.string().c_str());
	config.LoadFile();
	if (config.Error())
	{
		Error(ERROR_EXPORT_CONFIG_OPENING_ERROR, TAG "TinyXML: Error opening config file: config.xml.");
	}

	if (!this->Setup(&config, platformString))
	{
		Error(ERROR_EXPORT_CONFIG_PARAM, TAG "Error reading configuration file - check your parameters.");
	}

	fprintf(stdout, "Input: ");
	InputPathMapIterator it = mapInputPath.begin();
	InputPathMapIterator end = mapInputPath.end();
	for (; it != end; ++it)
		fprintf(stdout, "\t%s\n", ((*it).second).directory_string().c_str());
	this->mapInputPath[RESOURCE_MASK] = this->mapInputPath[RESOURCE_IMAGE]; // So it wont be show at startup

	fprintf(stdout, "Output: %s\n", bfsOutputPath.directory_string().c_str());

	TiXmlDocument doc(xmlfile);
	doc.LoadFile();
	if (doc.Error())
	{
		Error(ERROR_EXPORT_PROJECT_OPENING_ERROR, TAG "Error opening input file %s: %s.", xmlfile, doc.ErrorDesc());
	}

	pGroupManager = new GroupManager();

	pCache->SetRebuild(rebuild);
	pStringCache->SetRebuild(rebuild);
	bfs::path dictPath(e->GetOutputPath());
	dictPath /= "strings.dict";

	pCache->AddFilename(dictPath.string().c_str());
	pCache->Load();
	fprintf(stdout, "LOAD CACHE!");
	pStringCache->Load();
	Log(TAG "Creating assets.");
	this->CreateStrings(&doc);
	this->CreateResources(&doc);
	this->CreateObjects(&doc);
	//this->CreateOutputPath(); // fazer isso em outro lugar, no createresource ou createobjects de forma discreta (create on access).

	this->CompileStrings();
	this->CompileResources();
	Log(TAG "Compiling objects... wait.");
	this->CompileObjects();

	// Create Output files
	this->WriteStrings();
	this->WriteObjects();
	this->WriteHeaderFile(xmlfile);

	if (bPackages)
	{
		this->CreatePackages();
		this->WritePackages();
	}

	this->WriteFileList();
	this->WriteStringList();

	this->RemovePackagedFiles();

	// Dump caches
	pCache->Dump();
	pStringCache->Dump();

	Log(TAG "Done!");
	return TRUE;
}

/**
UTILS
**/

eObjectType Exporter::GetObjectType(const char *str)
{
	if (strcasecmp(str, "sprite") == 0)
		return OBJECT_SPRITE;
	//else if (strcasecmp(str, "image") == 0)
	//	return OBJECT_IMAGE;
	else if (strcasecmp(str, "sound") == 0)
		return OBJECT_SOUND;
	else if (strcasecmp(str, "music") == 0)
		return OBJECT_MUSIC;
	else if (strcasecmp(str, "font") == 0)
		return OBJECT_FONT;
	else if (strcasecmp(str, "button") == 0)
		return OBJECT_BUTTON;

	return OBJECT_NONE;
}

eResourceType Exporter::GetResourceType(const char *str)
{
	if (strcasecmp(str, "image") == 0)
		return RESOURCE_IMAGE;
	else if (strcasecmp(str, "sound") == 0)
		return RESOURCE_SOUND;
	else if (strcasecmp(str, "music") == 0)
		return RESOURCE_MUSIC;
	else if (strcasecmp(str, "mask") == 0)
		return RESOURCE_MASK;

	return RESOURCE_NONE;
}

void Exporter::AddResource(IResource *res, const char *lang)
{
	ResourceLangMapIterator itl = mapResourcesLang.find(lang);
	if (itl != mapResourcesLang.end())
	{
		ResourceMap &mapResources = (*itl).second;

		ResourceMapIterator it = mapResources.find(res->GetName());
		if (it == mapResources.end())
		{
			mapResources[res->GetName()] = res;
		}
		else
		{
			Error(ERROR_EXPORT_RESOURCE_DUPLICATED, TAG "Resource '%s' (language='%s') is duplicated!", res->GetName(), lang);
		}
	}
	else
	{
		ResourceMap &mapResource = mapResourcesLang[lang];
		mapResource[res->GetName()] = res;
		//mapResourcesLang[lang] = mapResource;
	}
}

BOOL Exporter::ResourceHasLocalization(const char *name)
{
	ResourceLangMapIterator itl = mapResourcesLang.begin();
	ResourceLangMapIterator endl = mapResourcesLang.end();

	for (; itl != endl; ++itl)
	{
		const char *lang = (*itl).first;
		if (strcasecmp(lang, DEFAULT_LANG) != 0)
		{
			if (this->GetResource(name, lang))
				return TRUE;
		}
	}

	return FALSE;
}

IResource *Exporter::GetResource(const char *name)
{
	return this->GetResource(name, DEFAULT_LANG);
}

IResource *Exporter::GetResource(const char *name, const char *lang)
{
	IResource *res = NULL;

	ResourceLangMapIterator itl = mapResourcesLang.find(lang);
	if (itl != mapResourcesLang.end())
	{
		ResourceMap mapResources = (*itl).second;

		ResourceMapIterator it = mapResources.find(name);
		if (it != mapResources.end())
		{
			res = mapResources[name];
		}
		else
		{
			Log(TAG "Resource '%s' (lang='%s') not found in <resources></resources>", name, lang);
		}
	}
	else
	{
		Log(TAG "Resource table for language '%s' not found.", lang);
	}

	return res;
}

s32 Exporter::GetStringId(const char *name)
{
	s32 s = (s32)vecStringName.size();
	for (s32 i = 0; i < s; i++)
	{
		if (!strcasecmp(vecStringName[i], name))
			return i;
	}
	return -1;
}

/**
CREATE
**/
IResource *Exporter::CreateResourceImage(TiXmlNode *object)
{
	bool build = bRebuild;

	const char *lang = (*object)["language"];
	if (!lang)
		lang = DEFAULT_LANG;

	eFormat conv = DEFAULT;
	const char *sConv = (*object)["format"];
	if (sConv)
	{
		if (!strcasecmp(sConv, "TSF"))
			conv = TSFCONV;
		else if (!strcasecmp(sConv, "COMPRESSED"))
			conv = COMPRESSED;
	}

	const char *name = (*object)["name"];
	if (!name)
	{
		Error(ERROR_EXPORT_IMAGE_RESOURCE_MISSING_ATTRIB, TAG "Found image resource without NAME!");
	}

	const char *fname = (*object)["filename"];
	if (!fname)
	{
		Error(ERROR_EXPORT_IMAGE_RESOURCE_MISSING_ATTRIB, TAG "Image resource '%s' without FILENAME!", name);
	}

	Log(TAG "Resource %s from language %s", fname, lang);

	/*const char *outname = (*object)["output"];

	Image *img = new Image((*object)["name"]);

	if (outname)
		img->SetOutputName(outname);*/

	Image *img = new Image(name, lang);
	img->SetFilename(fname);
	img->SetFormat(conv);

	const char *reb = (*object)["rebuild"];
	if (!reb)
	{
		if (this->IsModified(object, RESOURCE_IMAGE, img->GetOutputPath().string().c_str()))
			build = true;
	}
	else
	{
		if (!strcasecmp("false", reb))
			build = false;
		else if (!strcasecmp("true", reb))
			build = true;
	}
	img->SetRebuild(build);

	const char *tmp = NULL;
	u32 x = 0;
	u32 y = 0;
	u32 w = 0;
	u32 h = 0;
	u32 rw = pPlatform->GetWidth();
	u32 rh = pPlatform->GetHeight();
	tmp = (*object)["x"];
	if (tmp)
		x = atoi(tmp);

	tmp = (*object)["y"];
	if (tmp)
		y = atoi(tmp);

	tmp = (*object)["width"];
	if (tmp)
		w = atoi(tmp);

	tmp = (*object)["height"];
	if (tmp)
		h = atoi(tmp);

	tmp = (*object)["res_width"];
	if (tmp)
		rh = atoi(tmp);

	tmp = (*object)["res_height"];
	if (tmp)
		rw = atoi(tmp);

	img->SetX(x);
	img->SetY(y);
	img->SetWidth(w);
	img->SetHeight(h);
	img->SetScreenWidth(rw);
	img->SetScreenHeight(rh);

	if (conv == TSFCONV)
	{
		TSFHeader tsfHeader;
		tsfHeader.frames = 1;
		tsfHeader.globalFrameTime = 1;
		tsfHeader.flags = NO_FLAGS;
		tsfHeader.colorKey = 0xFF00FF;

		const char *value = (*object)["width"];
		if (value)
			tsfHeader.width = atoi(value);

		value = (*object)["height"];
		if (value)
			tsfHeader.height = atoi(value);

		value = (*object)["frames"];
		if (value)
			tsfHeader.frames = atoi(value);

		value = (*object)["globalFrameTime"];
		if (value)
			tsfHeader.globalFrameTime = atoi(value);

		value = (*object)["loop"];
		if (value && atoi(value))
			tsfHeader.flags |= TSF_LOOP;

		value = (*object)["animated"];
		if (value && atoi(value))
			tsfHeader.flags |= TSF_ANIMATED;

		img->SetTSFHeader(tsfHeader);
	}

	this->AddResource(img, lang);

	return img;
}

IResource *Exporter::CreateResourceSound(TiXmlNode *object)
{
	const char *lang = (*object)["language"];
	if (!lang)
		lang = DEFAULT_LANG;

	const char *name = (*object)["name"];
	if (!name)
	{
		Error(ERROR_EXPORT_SOUND_RESOURCE_MISSING_ATTRIB, TAG "Found sound resource without NAME!");
	}

	SoundResource *res = new SoundResource(name, lang);

	const char *fname = (*object)["filename"];
	if (!fname)
	{
		Error(ERROR_EXPORT_SOUND_RESOURCE_MISSING_ATTRIB, TAG "Sound resource '%s' without FILENAME!", name);
	}

	res->SetFilename(fname);

	bool build = bRebuild;
	const char *reb = (*object)["rebuild"];
	if (!reb)
	{
		if (this->IsModified(object, RESOURCE_SOUND, res->GetOutputPath().string().c_str()))
			build = true;
	}
	else
	{
		if (!strcasecmp("false", reb))
			build = false;
		else if (!strcasecmp("true", reb))
			build = true;
	}
	res->SetRebuild(build);

	this->AddResource(res, lang);

	return res;
}

IResource *Exporter::CreateResourceMusic(TiXmlNode *object)
{
	const char *lang = (*object)["language"];
	if (!lang)
		lang = DEFAULT_LANG;

	const char *name = (*object)["name"];
	if (!name)
	{
		Error(ERROR_EXPORT_MUSIC_RESOURCE_ATTRIB_ERROR, TAG "Found music resource without NAME!");
	}

	MusicResource *res = new MusicResource(name, lang);

	const char *fname = (*object)["filename"];
	if (!fname)
	{
		Error(ERROR_EXPORT_MUSIC_RESOURCE_ATTRIB_ERROR, TAG "Music resource '%s' without FILENAME!", name);
	}

	res->SetFilename(fname);

	bool build = bRebuild;
	const char *reb = (*object)["rebuild"];
	if (!reb)
	{
		if (this->IsModified(object, RESOURCE_MUSIC, res->GetOutputPath().string().c_str()))
			build = true;
	}
	else
	{
		if (!strcasecmp("false", reb))
			build = false;
		else if (!strcasecmp("true", reb))
			build = true;
	}
	res->SetRebuild(build);

	this->AddResource(res, lang);

	return res;
}

IResource *Exporter::CreateResourceMask(TiXmlNode *object)
{
	bool build = bRebuild;

	const char *lang = (*object)["language"];
	if (!lang)
		lang = DEFAULT_LANG;

	const char *name = (*object)["name"];
	if (!name)
	{
		Error(ERROR_EXPORT_MASK_RESOURCE_MISSING_ATTRIB, TAG "Found mask resource without NAME!");
	}

	const char *fname = (*object)["filename"];
	if (!fname)
	{
		Error(ERROR_EXPORT_MASK_RESOURCE_MISSING_ATTRIB, TAG "Mask resource '%s' without FILENAME!", name);
	}

	Mask *msk = new Mask(name, lang);
	msk->SetFilename(fname);

	const char *reb = (*object)["rebuild"];
	if (!reb)
	{
		if (this->IsModified(object, RESOURCE_MASK, msk->GetOutputPath().string().c_str()))
			build = true;
	}
	else
	{
		if (!strcasecmp("false", reb))
			build = false;
		else if (!strcasecmp("true", reb))
			build = true;
	}
	msk->SetRebuild(build);

	this->AddResource(msk, lang);

	return msk;
}

/* RESOURCES */
void Exporter::CreateResources(TiXmlDocument *doc)
{
	// Create Resources
	BEGIN_ITERATE_XML_NODES2(node, (*doc)("root")("resources")("resource"))
		IResource *obj = NULL;

		const char *type = (*node)["type"];
		if (!type)
		{
			Error(ERROR_EXPORT_RESOURCE_MISSING_TYPE, "There is a resource without a type!");
		}

		switch (this->GetResourceType(type))
		{
			case RESOURCE_IMAGE:
			{
				obj = this->CreateResourceImage(node);
			}
			break;

			case RESOURCE_SOUND:
			{
				obj = this->CreateResourceSound(node);
			}
			break;

			case RESOURCE_MUSIC:
			{
				obj = this->CreateResourceMusic(node);
			}
			break;

			case RESOURCE_MASK:
			{
				obj = this->CreateResourceMask(node);
			}
			break;

			default:
				Error(ERROR_EXPORT_RESOURCE_INVALID_TYPE, "A invalid resource type was found: %s", type);
			break;
		}
	END_ITERATE_XML_NODES2(node, (*doc)("root")("resources")("resource"))
}

/* STRINGS */
void Exporter::CreateStrings(TiXmlDocument *doc)
{
	bool ok = true;
	bool first = true;
	// Create Dictionaries

/*	if (&(*doc)("root")("strings") == NULL)
	{
		Log(TAG "No <strings></strings> node found, skiping.");
		return;
	}
*/
	BEGIN_ITERATE_XML_NODES2(node, (*doc)("root")("strings")("dictionary"))
		Dictionary *obj = NULL;

		const char *lang = (*node)["language"];
		if (!lang)
		{
			Error(ERROR_MISSING_XML_ATTRIBUTE, "There is a dictionary without a 'language' attribute!");
		}

		obj = new Dictionary(lang);

		const char *last = "<first_string>";
		BEGIN_ITERATE_XML_NODES2(string, (*node)("string"))

			const char *name = (*string)["name"];
			const char *text = (*string)["text"];

			if (!name)
				Error(ERROR_STRING_MISSING_ATTRIBUTE, "A string from dictionary '%s' has no NAME. Last one: '%s'.", lang, last);
			if (!text)
				Log("A string from dictionary '%s' has no TEXT, using empty string. Last one: '%s'.", lang, last);

			last = name;

			if (!first && GetStringId(name) == -1)
				Error(ERROR_STRING_MISSING, "String '%s' from '%s' was not created in the first dictionary.", name, lang);

			ok = ok & obj->AddString(name, text);; // if its a dupe, we will exit here
			if (first)
				vecStringName.push_back(name); // never we will get a dupe

		END_ITERATE_XML_NODES2(string, (*node)("string"))

		first = false;
		mapDictionaries[lang] = obj;

		if (ok)
			obj->Process();

	END_ITERATE_XML_NODES2(node, (*doc)("root")("strings")("dictionary"))

	if (!ok)
		Error(ERROR_STRING_ALREADY_EXISTS, TAG "There are duplicated strings in your dictionary.");
}

/* OBJECTS */
void Exporter::CreateFrames(Animation *anim, TiXmlNode *node)
{
	u32 frameidx = 0;
	BEGIN_ITERATE_XML_NODES2(object, (*node)("frame"))
		u32 frame_count = anim->GetTime();
		u32 x = 0, y = 0;
		u32 w = 0, h = 0;

		const char *tmp = (*object)["frame_count"];
		if (tmp)
			frame_count = atoi(tmp);
		tmp = (*object)["x"];

		if (tmp)
			x = atoi(tmp);
		else
			x = anim->GetX();

		tmp = (*object)["y"];
		if (tmp)
			y = atoi(tmp);
		else
			y = anim->GetY();

		tmp = (*object)["width"];
		if (tmp)
			w = atoi(tmp);
		else
			w = anim->GetWidth();

		tmp = (*object)["height"];
		if (tmp)
			h = atoi(tmp);
		else
			h = anim->GetHeight();

		const char *name = (*object)["name"];
		const char *resourceName = (*object)["resource"];
		if (!resourceName)
		{
			if (name)
				Error(ERROR_EXPORT_FRAME_MISSING_ATTRIB, TAG "Object frame '%s' lacking 'resource' field.", name);
			else
				Error(ERROR_EXPORT_FRAME_MISSING_ATTRIB, TAG "Anonymous object frame lacking 'resource' field.");
		}

		IResource *res = this->GetResource(resourceName);
		if (res)
		{
			anim->AddResource(res);

			Frame *frame = new Frame(name, static_cast<Image *>(res), frame_count, frameidx++);
			frame->SetWidth(w);
			frame->SetHeight(h);
			frame->SetX(x);
			frame->SetY(y);

			anim->Add(frame);
		}
		else
		{
			if (name)
				Error(ERROR_EXPORT_FRAME_RESOURCE_NOT_FOUND, TAG "Frame '%s' has no valid resource.", name);
			else
				Error(ERROR_EXPORT_FRAME_RESOURCE_NOT_FOUND, TAG "An anonymous frame has no valid resource.");
		}
	END_ITERATE_XML_NODES2(object, (*node)("frame"))
}

void Exporter::CreateAnimations(Sprite *spt, TiXmlNode *node)
{
	u32 animidx = 0;
	BEGIN_ITERATE_XML_NODES2(object, (*node)("animation"))

		const char *name = (*object)["name"];
		if (!name)
		{
			Error(ERROR_EXPORT_ANIMATION_MISSING_ATTRIB, TAG "An animation has no NAME.");
		}

		u32 frame = 0;
		u32 x = 0, y = 0;
		u32 w = 0, h = 0;
		bool animated = false;
		bool loop = false;

		const char *tmp = (*object)["frame_count"];
		if (tmp)
			frame = atoi(tmp);

		x = spt->GetX();
		y = spt->GetY();
		w = spt->GetWidth();
		h = spt->GetHeight();

		tmp = (*object)["loop"];
		if (tmp)
			loop = (strcasecmp(tmp, "true") == 0);

		tmp = (*object)["animated"];
		if (tmp)
			animated = (strcasecmp(tmp, "true") == 0);

		bool modified = false;
		BEGIN_ITERATE_XML_NODES2(child, (*object)("frame"))
			if ((modified = IsModified(child, RESOURCE_IMAGE, spt->GetName())))
				break;
		END_ITERATE_XML_NODES2(child, (*object)("frame"))

		bool build = false;
		if (bRebuild || modified)
			build = true;

		Animation *anim = new Animation(name, frame, loop, animated, animidx++);
		anim->SetX(x);
		anim->SetY(y);
		anim->SetWidth(w);
		anim->SetHeight(h);

		spt->SetRebuild(build);
		spt->Add(anim);
		this->CreateFrames(anim, object);

		ResourceMapIterator it = anim->GetFirstResource();
		for (; it != anim->GetLastResource(); ++it)
		{
			IResource *res = ((*it).second);
			spt->AddResource(res);
		}
	END_ITERATE_XML_NODES2(object, (*node)("animation"))
}

IObject *Exporter::CreateObjectSprite(TiXmlNode *object)
{
	const char *name = (*object)["name"];
	if (!name)
	{
		Error(ERROR_EXPORT_SPRITE_MISSING_ATTRIB, TAG "Sprite has no NAME.");
	}

	Sprite *spt = new Sprite(name);

	u32 x = 0;
	u32 y = 0;
	u32 w = 0;
	u32 h = 0;

	const char *tmp = (*object)["x"];
	if (tmp)
		x = atoi(tmp);

	tmp = (*object)["y"];
	if (tmp)
		y = atoi(tmp);

	tmp = (*object)["width"];
	if (tmp)
		w = atoi(tmp);

	tmp = (*object)["height"];
	if (tmp)
		h = atoi(tmp);

	tmp = (*object)["animated"];
	if (tmp)
		Error(ERROR_INVALID_XML_ATTRIBUTE, "Sprite doesn't have 'animated' attribute. It must be set in the animation node.");

	tmp = (*object)["loop"];
	if (tmp)
		Error(ERROR_INVALID_XML_ATTRIBUTE, "Sprite doesn't have 'loop' attribute. It must be set in the animation node.");

	tmp = (*object)["frame_count"];
	if (tmp)
		Error(ERROR_INVALID_XML_ATTRIBUTE, "Sprite doesn't have 'frame_count' attribute. It must be set in the animation or frame node.");

	spt->SetX(x);
	spt->SetY(y);
	spt->SetWidth(w);
	spt->SetHeight(h);

	this->CreateAnimations(spt, object);
	vecObjects.push_back(spt);

	return spt;
}

IObject *Exporter::CreateObjectFont(TiXmlNode *object)
{
	bool build = bRebuild;

	const char *name = (*object)["name"];
	if (!name)
	{
		Error(ERROR_EXPORT_FONT_MISSING_ATTRIB, TAG "Font has no NAME.");
	}

	if (this->IsModified(object, RESOURCE_IMAGE, name))
		build = true;

	const char *resourceName = (*object)["resource"];
	if (!resourceName)
	{
		Error(ERROR_EXPORT_FONT_MISSING_ATTRIB, TAG "Font '%s' lacking 'resource' field.", name);
	}

	Font *fnt = new Font(name, this->GetResource(resourceName));
	fnt->SetRebuild(build);

	f32 tracking = 0.0f;
	f32 spacing = 0.0f;
	f32 space = 0.03f;
	u32 glyph_width = 0.0f;
	u32 glyph_height = 0.0f;
	u32 characters = 90; // FIXME: Avisar os artistas!!!
	const char *language = "en_US";
	bool atlas = false;

	const char *value = (*object)["tracking"];
	if (value)
		tracking = atof(value);

	value = (*object)["spacing"];
	if (value)
		spacing = atof(value);

	value = (*object)["space"];
	if (value)
		space = atof(value);

	value = (*object)["characters"];
	if (value)
		characters = atoi(value);

	value = (*object)["language"];
	if (value)
		language = value;

	value = (*object)["atlas"];
	if (value)
		atlas = (strcasecmp(value, "true") == 0);

	value = (*object)["glyph_width"];
	if (value)
		glyph_width = atoi(value);

	value = (*object)["glyph_height"];
	if (value)
		glyph_height = atoi(value);

	if (glyph_width == 0.0f || glyph_height == 0.0f)
	{
		Error(ERROR_EXPORT_FONT_MISSING_ATTRIB, TAG "Font GLYPH_WIDTH and/or GLYPH_HEIGHT is empty or has invalid value.");
	}

	fnt->SetTracking(tracking);
	fnt->SetSpacing(spacing);
	fnt->SetSpace(space);
	fnt->SetCharacters(characters);
	//fnt->SetLanguage(language);
	fnt->SetUsingAtlas(atlas);
	fnt->SetGlyphWidth(glyph_width);
	fnt->SetGlyphHeight(glyph_height);
	fnt->AddResource(const_cast<IResource *>(fnt->GetResource()));

	vecObjects.push_back(fnt);

	return fnt;
}

IObject *Exporter::CreateObjectButton(TiXmlNode *object)
{
	const char *name = (*object)["name"];
	if (!name)
	{
		Error(ERROR_EXPORT_BUTTON_MISSING_ATTRIB, TAG "Button has no NAME.");
	}

	const char *id = (*object)["id"];
	if (!id)
	{
		Error(ERROR_EXPORT_BUTTON_MISSING_ATTRIB, TAG "Button '%s' lacking 'id' field.", name);
	}

	Button *btn = new Button(name, atoi(id));
	btn->SetRebuild(true);

	f32 x = 0.0f;
	f32 y = 0.0f;
	u32 prio = 0;
	u32 labelid = 0;
	u32 masktype = 0; // rectangle
	const char *collision = NULL;
	const char *label = NULL;
	const char *sprite = NULL;
	const char *mask = NULL;

	const char *value = (*object)["x"];
	if (value)
		x = atof(value);

	value = (*object)["y"];
	if (value)
		y = atof(value);

	value = (*object)["priority"];
	if (value)
		prio = atoi(value);

	value = (*object)["collision"];
	if (value)
		collision = value;

	value = (*object)["label"];
	if (value)
		label = value;

	value = (*object)["object_sprite"];
	if (value)
		sprite = value;

	value = (*object)["resource_mask"];
	if (value)
		mask = value;

	if (!strcmp(mask, ""))
		mask = NULL;

	if (!strcmp(sprite, ""))
		sprite = NULL;

	if (!strcmp(label, ""))
		label = NULL;

	if (!strcmp(collision, ""))
		collision = NULL;

	if (!strcasecmp(collision, "mask"))
		masktype = 1;

	if (!strcasecmp(collision, "pixel"))
		masktype = 2;

	if (masktype == 1 && !mask)
	{
		Error(ERROR_EXPORT_BUTTON_MISSING_ATTRIB, "Button using mask collision but no resource_mask file was set.");
	}

	// glipse into the future
	//lableid = gDict->GetTextId(label);

	btn->SetX(x);
	btn->SetY(y);
	btn->SetPriority(prio);
	btn->SetMaskType(masktype);
	btn->SetLabel(labelid);

	if (sprite && !this->ObjectExists(sprite))
	{
		Error(ERROR_EXPORT_BUTTON_SPRITE_NOT_FOUND, "Button using a sprite object that could not be found.");
	}

	btn->SetSprite(sprite);
	btn->SetMask(this->GetResource(mask));

	vecObjects.push_back(btn);

	return btn;
}

IObject *Exporter::CreateObjectSound(TiXmlNode *object)
{
	const char *soundName = (*object)["name"];
	if (!soundName)
	{
		Error(ERROR_EXPORT_SOUND_MISSING_ATTRIB, TAG "A sound has no NAME");
	}

	const char *resourceName = (*object)["resource"];
	if (!resourceName)
	{
		Error(ERROR_EXPORT_SOUND_MISSING_ATTRIB, TAG "Sound '%s' lacking 'resource' field.", soundName);
	}

	bool build = false;
	if (bRebuild || this->IsModified(object, RESOURCE_SOUND, soundName))
		build = true;

	bool loop = false;
	u32 volume = 100;

	const char *value = (*object)["loop"];
	if (value)
		loop = (strcasecmp(value, "true") == 0);

	value = (*object)["volume"];
	if (value)
		volume = atoi(value);

	const SoundResource *res = static_cast<const SoundResource *>(this->GetResource(resourceName));
	Sound *sound = new Sound(soundName, res);
	sound->SetRebuild(build);
	sound->SetVolume(volume);
	sound->SetLoop(loop);

	vecObjects.push_back(sound);

	return sound;
}

IObject *Exporter::CreateObjectMusic(TiXmlNode *object)
{
	const char *musicName = (*object)["name"];
	if (!musicName)
	{
		Error(ERROR_EXPORT_SOUND_MISSING_ATTRIB, TAG "A sound has no NAME");
	}

	const char *resourceName = (*object)["resource"];
	if (!resourceName)
	{
		Error(ERROR_EXPORT_SOUND_MISSING_ATTRIB, TAG "Sound '%s' lacking 'resource' field.", musicName);
	}

	bool build = false;
	if (bRebuild || this->IsModified(object, RESOURCE_MUSIC, musicName))
		build = true;

	u32 volume = 100;

	const char *value = (*object)["volume"];
	if (value)
		volume = atoi(value);

	const MusicResource *res = static_cast<const MusicResource *>(this->GetResource(resourceName));
	Music *music = new Music(musicName, res);
	music->SetRebuild(build);
	music->SetVolume(volume);

	vecObjects.push_back(music);

	return music;
}

void Exporter::CreateObjects(TiXmlDocument *doc)
{
	// Create Objects
	BEGIN_ITERATE_XML_NODES2(object, (*doc)("root")("objects")("object"))
		IObject *obj = NULL;
		const char *type = (*object)["type"];
		if (!type)
		{
			Error(ERROR_EXPORT_OBJECT_MISSING_TYPE, TAG "Object has no TYPE!");
		}

		switch (this->GetObjectType(type))
		{
			case OBJECT_SPRITE:
			{
				obj = this->CreateObjectSprite(object);
			}
			break;
/*
			case OBJECT_IMAGE:
			{
				obj = this->CreateObjectImage(object);
			}
			break;
*/
			case OBJECT_FONT:
			{
				obj = this->CreateObjectFont(object);
			}
			break;

			case OBJECT_BUTTON:
			{
				obj = this->CreateObjectButton(object);
			}
			break;

			case OBJECT_MUSIC:
			{
				obj = this->CreateObjectMusic(object);
			}
			break;

			case OBJECT_SOUND:
			{
				obj = this->CreateObjectSound(object);
			}
			break;

			default:
				Error(ERROR_EXPORT_OBJECT_INVALID_TYPE, TAG "Invalid object type '%s'", type);
			break;
		}

		// multiple groups here

		const char *groupName = (*object)["group"];
		if (groupName)
		{
			Group *g = pGroupManager->Get(groupName);
			if (!g)
			{
				g = new Group(groupName);
				pGroupManager->Add(g);
			}
			g->AddObject(obj);

			// packing .sprite and .audio only
//#if COMPILE_RESOURCE == 1
			if (e->IsPackageResourcesEnabled())
			{
				ResourceMapIterator it = obj->GetFirstResource();
				for (; it != obj->GetLastResource(); ++it)
				{
					IResource *res = ((*it).second);
					g->AddResource(res);
				}
			}
//#endif
		}

	END_ITERATE_XML_NODES2(object, (*doc)("root")("objects")("object"))
}

/* PACKAGES */
void Exporter::CreatePackages()
{
	for (GroupMapIterator it = pGroupManager->GetFirst(); it != pGroupManager->GetLast(); ++it)
	{
		Group *g = ((*it).second);
		Package *p = new Package(g);
		vecPackages.push_back(p);
	}
}

/**
COMPILE
**/
void Exporter::CompileStrings()
{
	DictionaryMapIterator it = mapDictionaries.begin();
	DictionaryMapIterator end = mapDictionaries.end();
	for (; it != end; ++it)
	{
		Dictionary *dic = ((*it).second);
		pPlatform->Compile(dic);
	}
}

void Exporter::CompileResources()
{
	ResourceLangMapIterator it = mapResourcesLang.begin();
	ResourceLangMapIterator end = mapResourcesLang.end();

	for (; it != end; ++it)
	{
		Log(TAG "Compiling resources for language: %s. Wait...", (*it).first);
		ResourceMap mapResources = (*it).second;

		ResourceMapIterator itr = mapResources.begin();
		ResourceMapIterator endr = mapResources.end();
		for (; itr != endr; ++itr)
		{
			IResource *res = ((*itr).second);
			if (res->NeedRebuild())
				pPlatform->Compile(res);
		}
	}
}

void Exporter::CompileObjects()
{
	ObjectVectorIterator it = vecObjects.begin();
	ObjectVectorIterator end = vecObjects.end();
	for (; it != end; ++it)
	{
		IObject *obj = (*it);
		if (obj->NeedRebuild())
			pPlatform->Compile(obj);
	}
}

/**
WRITE
**/
void Exporter::WriteStrings()
{
	DictionaryMapIterator it = mapDictionaries.begin();
	Log(TAG "Generating dictionaries...");
	for (; it != mapDictionaries.end(); ++it)
	{
		Dictionary *dic = (*it).second;
		bfs::create_directories(dic->GetOutputPath().parent_path());
		FILE *fp = fopen(dic->GetOutputPath().string().c_str(), "wb+");
		if (!fp)
		{
			Error(ERROR_UNKNOWN, TAG "Could not create dictionary file: %s", dic->GetOutputPath().string().c_str());
		}

		dic->Write(fp, this);
		fclose(fp);
	}
	Log(TAG "Finished writing dictionaries...");
}

void Exporter::WritePackages()
{
	PackageVectorIterator it = vecPackages.begin();
	Log(TAG "Generating packages...");
	for (; it != vecPackages.end(); ++it)
	{
		Package *p = *it;
		p->Write();
	}
	Log(TAG "Finished writing packages...");

	/*Log(TAG "Removing already packaged files.");
	it = vecPackages.begin();
	for (; it != vecPackages.end(); ++it)
	{
		Package *p = *it;
		p->RemovePackagedFiles();
	}
	Log(TAG "Finished removing packaged files...");*/
}


void Exporter::RemovePackagedFiles()
{
	Log(TAG "Removing already packaged files.");
	PackageVectorIterator it = vecPackages.begin();
	for (; it != vecPackages.end(); ++it)
	{
		Package *p = *it;
		p->RemovePackagedFiles();
	}
	Log(TAG "Finished removing packaged files...");
}


void Exporter::WriteObjects()
{
	ObjectVectorIterator it = vecObjects.begin();
	ObjectVectorIterator end = vecObjects.end();
	int objectsBuilt = false;
	for (; it != end; ++it)
	{
		IObject *obj = *it;
		if (obj->NeedRebuild())
		{
			objectsBuilt++;

			Log(TAG "Building object '%s'.", obj->GetName());

			bfs::create_directories(obj->GetOutputPath().parent_path());
			FILE *fp = fopen(obj->GetOutputPath().string().c_str(), "wb+");
			if (!fp)
			{
				Error(ERROR_FILE_COULD_NOT_OPEN_WRITE, TAG "Failed to open %s in write-mode.", obj->GetOutputPath().string().c_str());
			}
			obj->Write(fp, this);
			fclose(fp);
		}
	}

	if (!objectsBuilt)
		Log(TAG "Nothing to be done.");
	else
		Log(TAG "%d object(s) to be processed.", objectsBuilt);
}

void Exporter::WriteHeaderFile(const char *xmlfile)
{
	std::ostringstream fileTmp;
	u32 i = 0;
	while (xmlfile[i] != '.')
		fileTmp << xmlfile[i++];

	std::string name(fileTmp.str());
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	fileTmp << ".h";

	name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
	name.erase(std::remove(name.begin(), name.end(), ':'), name.end());
	name.erase(std::remove(name.begin(), name.end(), '/'), name.end());
	name.erase(std::remove(name.begin(), name.end(), '\\'), name.end());
	name.erase(std::remove(name.begin(), name.end(), '?'), name.end());

	FILE *fp = fopen(fileTmp.str().c_str(), "wt+");
	if (!fp)
	{
		Error(ERROR_FILE_COULD_NOT_OPEN_CREATE, TAG "Failed to open/create %s file.", fileTmp.str().c_str());
	}

	Log(TAG " * Generating header file...");

	u32 sptId = 0;
	u32 fntId = 0;
	u32 sfxId = 0;
	u32 bgmId = 0;

	fprintf(fp, "#ifndef __%s_H__\n", name.c_str());
	fprintf(fp, "#define __%s_H__\n\n\n", name.c_str());

	for (u32 i = 0, size = vecStringName.size(); i < size; i++)
	{
		char *s = to_upper(vecStringName.at(i));
		fprintf(fp, "#define DIC_%s\t\t_(%d)\n", s, i);
		fprintf(fp, "#define DIC_%s_ID\t\t%d\n", s, i);
		free(s);
	}

	for (u32 i = 0, size = vecObjects.size(); i < size; i++)
	{
		IObject *obj = vecObjects.at(i);

		u32 fileId = pCache->GetFilenameId(obj->GetOutputPath().string().c_str());
		switch (obj->GetType())
		{
			case OBJECT_MUSIC:
			{
				char *s = to_upper(get_filename(obj->GetName()));

				//if (cfg->GetAudioHeaderType() == 0)
				//{
					fprintf(fp, "#define BGM_%s\t\t_F(%d)\n", s, fileId);// bgmId);
					fprintf(fp, "#define BGM_%s_ID\t\t%d\n", s, fileId);// bgmId);
				/*}
				else
				{
					bfs::path relativePath = pFileSystem->GetRelativePath(obj->GetOutputPath(), e->GetOutputPath());
					fprintf(fp, "#define BGM_%s\t\t\"%s\"\n", s, relativePath.string().c_str());
				}*/

				bgmId++;
				free(s);
			}
			break;

			case OBJECT_SOUND:
			{
				char *s = to_upper(get_filename(obj->GetName()));

				//if (cfg->GetAudioHeaderType() == 0)
				fprintf(fp, "#define SFX_%s\t\t_F(%d)\n", s, fileId);//sfxId);
				fprintf(fp, "#define SFX_%s_ID\t\t%d\n", s, fileId);//sfxId);
				/*else
				{
					bfs::path relativePath = pFileSystem->GetRelativePath(obj->GetOutputPath(), e->GetOutputPath());
					fprintf(fp, "#define SFX_%s\t\t\"%s\"\n", s, relativePath.string().c_str());
				}*/

				sfxId++;
				free(s);
			}
			break;

			case OBJECT_SPRITE:
			{
				char *s = to_upper(get_filename(obj->GetName()));

				//if (cfg->GetSpriteHeaderType() == Config::SpriteHeaderDefault)
					fprintf(fp, "#define SPT_%s\t\t_F(%d)\n", s, fileId);// sptId);
				fprintf(fp, "#define SPT_%s_ID\t\t%d\n", s, fileId);// sptId);
				/*else
				{
					bfs::path relativePath = pFileSystem->GetRelativePath(obj->GetOutputPath(), e->GetOutputPath());
					fprintf(fp, "#define SPT_%s\t\t\"%s\"\n", s, relativePath.string().c_str());
				}*/

				sptId++;
				free(s);
			}
			break;

			case OBJECT_FONT:
			{
				char *s = to_upper(get_filename(obj->GetName()));

				//if (cfg->GetSpriteHeaderType() == Config::SpriteHeaderDefault)
				fprintf(fp, "#define FNT_%s\t\t_F(%d)\n", s, fileId); //fntId);
				fprintf(fp, "#define FNT_%s_ID\t\t%d\n", s, fileId); //fntId);
				/*else
				{
					// FIXME: .font is not implemented, we use .sprite for now.
					bfs::path relativePath = pFileSystem->GetRelativePath(obj->GetOutputPath(), e->GetOutputPath());
					relativePath.replace_extension(".sprite");
					fprintf(fp, "#define FNT_%s\t\t\"%s\"\n", s, relativePath.string().c_str());
				}*/

				fntId++;
				free(s);
			}
			break;

			case OBJECT_BUTTON:
			{
				Button *btn = static_cast<Button *>(obj);
				char *s = to_upper(get_filename(obj->GetName()));

				bfs::path relativePath = pFileSystem->GetRelativePath(obj->GetOutputPath(), e->GetOutputPath());
				//fprintf(fp, "#define BTN_%s\t\t\"%s\"\n", s, relativePath.string().c_str());
				fprintf(fp, "#define BTN_%s\t\t_F(%d)\n", s, fileId);
				fprintf(fp, "#define BTN_%s_ID\t\t%d\n", s, fileId);
				fprintf(fp, "#define BTN_ID_%s\t\t%d\n", s, btn->GetId());

				free(s);
			}
			break;

			default:
				fprintf(fp, "// %s\n", obj->GetName());
			break;
		}
	}

	fprintf(fp, "\n\n#endif // __%s_H__\n", name.c_str());
	fclose(fp);
}

#define FST_MAGIC	0x00545346
#define FST_VERSION 0x00000001
void Exporter::WriteFileList()
{
	struct FileListObjectHeader
	{
		ObjectHeader block;
		u32 langcount;
		u32 filecount;
		// language code
		// filename list
	};

	u32 pos = strlen(e->GetOutputPath().string().c_str());

	DictionaryMap dict = e->GetDictionaries();
	if (dict.size() == 0)
	{
		dict[DEFAULT_LANG] = new Dictionary(DEFAULT_LANG);
	}

	FileListObjectHeader foh;
	foh.block.magic = pPlatform->Swap32(FST_MAGIC);
	foh.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	foh.block.version = pPlatform->Swap32(FST_VERSION);
	foh.langcount = pPlatform->Swap32(dict.size());
	foh.filecount = pPlatform->Swap32(pCache->Size());

	bfs::path out = e->GetOutputPath();
	out /= "filelist.data";
	FILE *fp = fopen(out.string().c_str(), "wb+");
	fwrite(&foh, sizeof(FileListObjectHeader), 1, fp);

	DictionaryMapIterator it = dict.begin();
	DictionaryMapIterator end = dict.end();
	for (; it != end; ++it)
	{
		Dictionary *dict = (*it).second;
		u32 code = pPlatform->Swap32(dict->iGetLanguage());
		fwrite(&code, sizeof(u32), 1, fp);

		//std::string flist("filelist.");
		//flist += std::string(dict->pcGetLanguage());
		//flist += std::string(".txt");
		//FILE *fp = fopen(flist.c_str(), "wt+");
		s32 s = (s32)pCache->Size();
		for (s32 i = 0; i < s; i++)
		{
			const char *name = pCache->GetFilenameById(i);

			std::string l10n("l10n/");
			l10n += dict->pcGetLanguage();
			l10n += "/";

			bfs::path o = e->GetOutputPath();
			bfs::path p = &name[pos+1];
			bfs::path l(l10n);

			o /= l;
			o /= p;

			if (bfs::exists(o))
				fprintf(fp, "%s", &(o.string().c_str())[pos+1]);
			else
				fprintf(fp, "%s", &name[pos+1]);
			fputc(0, fp);
		}
		//fclose(fp);
	}
	fclose(fp);
}

#define SST_MAGIC	0x00535346
#define SST_VERSION 0x00000001
void Exporter::WriteStringList()
{
	struct StringListObjectHeader
	{
		ObjectHeader block;
		u32 strcount;
		// str list
	};

	StringListObjectHeader soh;
	soh.block.magic = pPlatform->Swap32(SST_MAGIC);
	soh.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	soh.block.version = pPlatform->Swap32(SST_VERSION);
	soh.strcount = pPlatform->Swap32(pStringCache->Size());

	bfs::path out = e->GetOutputPath();
	out /= "strlist.data";
	FILE *fp = fopen(out.string().c_str(), "wb+");
	if (!fp)
		Error(-1, "Could not open/create strlist.data!");
	fwrite(&soh, sizeof(StringListObjectHeader), 1, fp);

	s32 s = (s32)pStringCache->Size();
	for (s32 i = 0; i < s; i++)
	{
		const char *name = pStringCache->GetStringById(i);
		fprintf(fp, "%s", name);
		fputc(0, fp);
	}
	fclose(fp);
}


bool Exporter::CreateOutputPath()
{
	return true;
}

bool Exporter::IsModified(TiXmlNode *node, eResourceType type, const char *outputName)
{
	return TRUE;
}

bool Exporter::ObjectExists(const char *name)
{
	ObjectVectorIterator it = std::find_if(vecObjects.begin(), vecObjects.end(), Finder(name));

	return (*it != NULL && it != vecObjects.end());
}
