#ifndef __CACHE_H__
#define __CACHE_H__

#include <vector>
#include "defines.h"

typedef std::vector<std::string> FilenameVector;
typedef FilenameVector::iterator FilenameIterator;

class Cache
{
	public:
		Cache();
		virtual ~Cache();

		s32 AddFilename(const char *filename);
		s32 GetFilenameId(const char *filename);
		const char *GetFilenameById(u32 id);
		u32 Size() const { return vecFilename.size(); }
		void SetRebuild(BOOL b) { this->bRebuild = b; }

		void Dump();
		void Load();
		void Reset();

	private:
		Cache(const Cache &);
		Cache &operator=(const Cache &);

		FilenameVector vecFilename;
		BOOL bRebuild;

		const char *pcPlatform;

	public:
		static Cache instance;
};

Cache *const pCache = &Cache::instance;

#endif // __CACHE_H__
