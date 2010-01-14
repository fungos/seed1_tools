#ifndef __IRESOURCE_H__
#define __IRESOURCE_H__

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "cache.h"

enum eResourceType
{
	RESOURCE_NONE = 0,
	RESOURCE_IMAGE,
	RESOURCE_SOUND,
	RESOURCE_MUSIC,
	RESOURCE_MASK
};

enum eFormat
{
	DEFAULT = 0,
	COMPRESSED,
	TSFCONV
};

/*typedef std::vector<char *>			FileNameVector;
typedef FileNameVector::iterator	FileNameVectorIterator;*/
typedef std::vector<bfs::path>		FilePathVector;
typedef FilePathVector::iterator	FilePathVectorIterator;

class Exporter;

class IResource
{
	private:
		IResource(const IResource &);
		IResource &operator=(const IResource &);

	protected:
		eResourceType 	iType;
		char			*pName;
		char			*pFilename;
		bfs::path 		bfsOutputPath;
		bfs::path 		bfsInputPath;
		u32		 		iFileSize;
		FilePathVector	vFilePath;

		eFormat			iFormat;
		bool			bRebuild;
		const char		*pLang;

	public:
		IResource(const char *lang)
			: iType(RESOURCE_IMAGE)
			, pName(NULL)
			, pFilename(NULL)
			, bfsOutputPath()
			, bfsInputPath()
			, iFileSize(0)
			, vFilePath()
			, iFormat(DEFAULT)
			, bRebuild(FALSE)
			, pLang(lang)
		{}

		virtual ~IResource()
		{
			if (this->pName)
				free(this->pName);

			if (this->pFilename)
				free(this->pFilename);

			this->pName = NULL;
			this->pFilename = NULL;
			this->pLang = NULL;
		}

		inline const char *GetLanguage() const
		{
			return this->pLang;
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

		inline void SetFileSize(const u32 v)
		{
			iFileSize = v;
		}

		inline u32 GetFileSize() const
		{
			return iFileSize;
		}

		virtual eResourceType GetType() const
		{
			return iType;
		}

		virtual void SetName(const char *name)
		{
			if (!name)
			{
				fprintf(stderr, "Resource must have a name!\n");
				return;
			}

			if (pName)
				free(pName);

			pName = strdup(name);
		}

		virtual const char *GetName() const
		{
			return this->pName;
		}

		virtual void SetFilename(const char *filename)
		{
			if (!filename)
			{
				fprintf(stderr, "Resource ('%s') must have a filename!\n", this->pName);
				return;
			}

			if (pFilename)
				free(pFilename);

			pFilename = strdup(filename);
		}

		virtual const char *GetFilename() const
		{
			return this->pFilename;
		}

		inline bfs::path GetOutputPath() const
		{
			return this->bfsOutputPath;
		}

		inline bfs::path GetInputPath() const
		{
			return this->bfsInputPath;
		}

		virtual void SetFormat(const eFormat e)
		{
			this->iFormat = e;
		}

		inline const eFormat GetFormat() const
		{
			return this->iFormat;
		}

		virtual void SetRebuild(const bool value)
		{
			this->bRebuild = value;
		}

		virtual bool NeedRebuild() const
		{
			return this->bRebuild;
		}

		//virtual void Write(FILE *fp, Exporter *e) = 0;
};

#endif
