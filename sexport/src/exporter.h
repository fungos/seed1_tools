#ifndef __EXPORTER_H__
#define __EXPORTER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <map>

#include "config.h"
#include "xml/tinyxml.h"
#include "string.h"
#include "iobject.h"
#include "iresource.h"
#include "object/sprite.h"
#include "object/font.h"
#include "object/button.h"
#include "resource/image.h"
#include "resource/music.h"
#include "resource/sound.h"
#include "resource/mask.h"
#include "groupmanager.h"
#include "package.h"

class Dictionary;

typedef std::map<const char *, IResource *, StringComparator> ResourceMap;
typedef ResourceMap::iterator ResourceMapIterator;

typedef std::map<const char *, ResourceMap, StringComparator> ResourceLangMap;
typedef ResourceLangMap::iterator ResourceLangMapIterator;

typedef std::map<const char *, Dictionary *, StringComparator> DictionaryMap;
typedef DictionaryMap::iterator DictionaryMapIterator;

typedef std::vector<IResource *> ResourceVector;
typedef ResourceVector::iterator ResourceVectorIterator;

typedef std::vector<IObject *> ObjectVector;
typedef ObjectVector::iterator ObjectVectorIterator;

typedef std::vector<const char *> StringVector;
typedef StringVector::iterator StringIterator;

typedef std::vector<Package *>	PackageVector;
typedef PackageVector::iterator	PackageVectorIterator;

typedef std::map<int, bfs::path> InputPathMap;
typedef InputPathMap::iterator	InputPathMapIterator;

class Exporter
{
	friend class Audio;
	friend class Sprite;
	friend class Image;
	friend class Font;

	private:
		Exporter(const Exporter &);
		Exporter &operator=(const Exporter &);

		Exporter();
		~Exporter();

		bool Setup(TiXmlDocument doc, const char *platform);

	private:
		bool bInitialized;
		bool bRebuild;
		bool bPackages;
		bool bCompression;
		bool bPackageResources;

		u8	iAlignment;

		ResourceLangMap		mapResourcesLang;
		DictionaryMap		mapDictionaries;
		ObjectVector		vecObjects;
		StringVector		vecStringName;
		PackageVector		vecPackages;

		InputPathMap		mapInputPath;
		bfs::path			bfsOutputPath;

		GroupManager		*pGroupManager;

	public:
		//const char *pExeName;
		bfs::path bfsExeName;

		void Initialize();
		void Shutdown();

		void CreateFrames(Animation *anim, TiXmlNode *node);
		void CreateAnimations(Sprite *spt, TiXmlNode *node);

		s32 GetStringId(const char *str);
		const StringVector &GetStringVector() const
		{
			return vecStringName;
		}

		/* resources */
		eResourceType GetResourceType(const char *str);
		void AddResource(IResource *res, const char *lang);
		IResource *GetResource(const char *name, const char *lang);
		IResource *GetResource(const char *name);
		IResource *CreateResourceImage(TiXmlNode *node);
		IResource *CreateResourceSound(TiXmlNode *node);
		IResource *CreateResourceMusic(TiXmlNode *node);
		IResource *CreateResourceMask(TiXmlNode *node);
		void CreateResources(TiXmlDocument *doc);
		void CompileResources();
		BOOL ResourceHasLocalization(const char *name);

		/* objects */
		eObjectType GetObjectType(const char *str);
		IObject *CreateObjectSprite(TiXmlNode *node);
		IObject *CreateObjectFont(TiXmlNode *node);
		IObject *CreateObjectButton(TiXmlNode *node);
		IObject *CreateObjectMusic(TiXmlNode *node);
		IObject *CreateObjectSound(TiXmlNode *node);
		void CreateObjects(TiXmlDocument *doc);
		void CompileObjects();
		void WriteObjects();

		/* strings */
		void CreateStrings(TiXmlDocument *doc);
		void CompileStrings();
		void WriteStrings();
		DictionaryMap &GetDictionaries()
		{
			return mapDictionaries;
		}

		/* file table */
		s32 AddFilename(const char *filename);
		s32 GetFilenameId(const char *filename);
		const char *GetFilenameById(u32 id);

		/* packages */
		void CreatePackages();
		void WritePackages();
		void RemovePackagedFiles();

		void WriteHeaderFile(const char *f);
		void WriteFileList();
		void WriteStringList();

		bool Process(const char *configfile, const char *xmlfile, const char *platformString, const bool rebuild, const bool packages, const u8 alignment, const bool compression, const bool add_resources);

		bool CreateOutputPath();
		bool IsModified(TiXmlNode *node, eResourceType type, const char *outputName);
		bool ObjectExists(const char *name);

		inline bool IsRebuild() const
		{
			return bRebuild;
		}

		inline bfs::path GetInputPath(eResourceType type)
		{
			return mapInputPath[(int)type];
		}

		inline bfs::path GetOutputPath() const
		{
			return bfsOutputPath;
		}

		inline u8 GetAlignment() const
		{
			return iAlignment;
		}

		inline bool IsCompressionEnabled() const
		{
			return bCompression;
		}

		inline bool IsPackageResourcesEnabled() const
		{
			return bPackageResources;
		}

		static Exporter instance;
};

Exporter *const e = &Exporter::instance;

#endif

