#ifndef __STRING_CACHE_H__
#define __STRING_CACHE_H__

#include <vector>
#include "defines.h"

typedef std::vector<std::string> CacheVector;
typedef CacheVector::iterator CacheIterator;

class StringCache
{
	public:
		StringCache();
		~StringCache();

		s32 AddString(std::string str);
		s32 GetStringId(std::string str);
		const char *GetStringById(u32 id);
		u32 Size() const { return vecString.size(); }
		void SetRebuild(BOOL b) { this->bRebuild = b; }

		void Dump();
		void Load();

	private:
		StringCache(const StringCache &);
		StringCache &operator=(const StringCache &);

		CacheVector vecString;
		BOOL bRebuild;

	public:
		static StringCache instance;
};

StringCache *const pStringCache = &StringCache::instance;

#endif // __STRING_CACHE_H__
