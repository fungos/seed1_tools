#ifndef __IOBJECT_H__
#define __IOBJECT_H__

#include <stdlib.h>
#include <stdio.h>
#include <map>

#include "iresource.h"
#include "cache.h"


enum eObjectType
{
	OBJECT_NONE 		= 0x00,
	OBJECT_SPRITE 		= 0x01,
	//OBJECT_IMAGE = 0x02,
	//OBJECT_AUDIO = 0x04,

	OBJECT_ANIMATION 	= 0x08,
	OBJECT_FRAME 		= 0x10,
	OBJECT_MUSIC 		= 0x20,
	OBJECT_SOUND 		= 0x40,
	OBJECT_FONT 		= 0x80,
	OBJECT_BUTTON 		= 0x100,

	OBJECT_MAP			= 0x1000,
};


typedef std::map<const char *, IResource *, StringComparator> ResourceMap;
typedef ResourceMap::iterator	ResourceMapIterator;


class Exporter;

class IObject
{
	private:
		IObject(const IObject &);
		bool operator=(const IObject &);

	protected:
		eObjectType 	iType;
		bool 			bRebuild;
		char			*pName;
		bfs::path 		bfsOutputPath;
		bfs::path 		bfsInputPath;
		FilePathVector	vFilePath;
		ResourceMap 	mapResources;
		u32 			iFileSize;

	public:
		IObject() : iType(OBJECT_NONE), bRebuild(FALSE), pName(NULL), bfsOutputPath(), bfsInputPath(), vFilePath(), mapResources(), iFileSize(0) {}
		virtual ~IObject() {}

		virtual eObjectType GetType() const
		{
			return this->iType;
		}

		inline u32 GetFilesAmount() const
		{
			return this->vFilePath.size();
		}

		inline void AddFilePath(bfs::path p)
		{
			pCache->AddFilename(p.string().c_str());
			this->vFilePath.push_back(p);
		}

		inline bfs::path GetFileVectorPath(const u32 idx) const
		{
			return this->vFilePath.at(idx);
		}

		inline FilePathVectorIterator GetFirstFileName()
		{
			return this->vFilePath.begin();
		}

		inline FilePathVectorIterator GetLastFile()
		{
			return this->vFilePath.end();
		}

		inline void SetRebuild(const bool value)
		{
			this->bRebuild = value;
		}

		inline bool NeedRebuild() const
		{
			return this->bRebuild;
		}

		inline const char *GetName() const
		{
			return this->pName;
		}

		inline bfs::path GetOutputPath()
		{
			return this->bfsOutputPath;
		}

		inline bfs::path GetInputPath()
		{
			return this->bfsInputPath;
		}

		inline void AddResource(IResource *res)
		{
			mapResources[res->GetFilename()] = res;
		}

		inline IResource *GetResource(const char *fileName)
		{
			IResource *ret = NULL;
			if (fileName)
			{
				if (mapResources[fileName])
					ret = mapResources[fileName];
			}
			return ret;
		}

		inline ResourceMapIterator GetFirstResource()
		{
			return mapResources.begin();
		}

		inline ResourceMapIterator GetLastResource()
		{
			return mapResources.end();
		}

		virtual void Write(FILE *fp, Exporter *e) = 0;
};

#endif
