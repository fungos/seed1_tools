#include "strcache.h"

#include <stdio.h>

StringCache StringCache::instance;


StringCache::StringCache()
	: vecString()
	, bRebuild(FALSE)
{
}

StringCache::~StringCache()
{
	vecString.clear();
}


s32 StringCache::AddString(std::string str)
{
	s32 id = this->GetStringId(str);
	if (id < 0)
	{
		vecString.push_back(str);
		id = vecString.size() - 1;
	}
	return id;
}

s32 StringCache::GetStringId(std::string str)
{
	s32 s = (s32)vecString.size();
	for (s32 i = 0; i < s; i++)
	{
		if (vecString[i] == str)
			return i;
	}
	return -1;
}

const char *StringCache::GetStringById(u32 id)
{
	return vecString[id].c_str();
}

void StringCache::Dump()
{
	FILE *fp = fopen("stringcache.txt", "wt+");
	s32 s = (s32)pStringCache->Size();
	for (s32 i = 0; i < s; i++)
	{
		const char *name = pStringCache->GetStringById(i);
		fprintf(fp, "%s\n", name);
	}
	fclose(fp);
}

void StringCache::Load()
{
	if (!this->bRebuild)
	{
		char entry[1024];
		FILE *fp = fopen("stringcache.txt", "rt");
		if (fp)
		{
			while (!feof(fp))
			{
				fgets(entry, 1024, fp);
				if (entry[strlen(entry) - 1] == '\n')
					entry[strlen(entry) - 1] = '\0';
				this->AddString(entry);
			}
			fclose(fp);
		}
	}
}
