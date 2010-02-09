#include "cache.h"
#include "platform.h"

Cache Cache::instance;


Cache::Cache()
	: vecFilename()
	, bRebuild(FALSE)
	, pcPlatform(NULL)
{
}

Cache::~Cache()
{
	/*
	for (s32 i = 0, s = vecFilename.size(); i < s; i++)
	{
		free(vecFilename[i]);
		vecFilename[i] = NULL;
	}
	*/

	this->Reset();
}


void Cache::Reset()
{
	vecFilename.clear();
}


s32 Cache::AddFilename(const char *filename)
{
	if (strstr(filename, "l10n"))
		return -1;

	s32 id = this->GetFilenameId(filename);
	if (id < 0)
	{
		bfs::path pname(filename);
		vecFilename.push_back(pname.string());
		id = vecFilename.size() - 1;
	}
	return id;
}

s32 Cache::GetFilenameId(const char *filename)
{
	bfs::path pname(filename);
	std::string str = pname.string();

	s32 s = (s32)vecFilename.size();
	for (s32 i = 0; i < s; i++)
	{
		//if (!strcasecmp(vecFilename[i], filename))
		//	return i;
		if (vecFilename[i] == str)
			return i;
	}
	return -1;
}

const char *Cache::GetFilenameById(u32 id)
{
	if (vecFilename.size() <= id)
		return NULL;

	return vecFilename[id].c_str();
}

void Cache::Dump()
{
	std::string fname("cache.");
	fname += pPlatform->GetName();
	fname += ".txt";

	FILE *fp = fopen(fname.c_str(), "wt+");
	s32 s = (s32)pCache->Size();
	for (s32 i = 0; i < s; i++)
	{
		const char *name = pCache->GetFilenameById(i);
		fprintf(fp, "%s\n", name);
	}
	fclose(fp);
}

void Cache::Load()
{
	if (!this->bRebuild)
	{
		char entry[1024];
		std::string fname("cache.");
		fname += pPlatform->GetName();
		fname += ".txt";

		FILE *fp = fopen(fname.c_str(), "rt");
		if (fp)
		{
			while (!feof(fp))
			{
				fgets(entry, 1024, fp);
				if (entry[strlen(entry) - 1] == '\n')
					entry[strlen(entry) - 1] = '\0';
				this->AddFilename(entry);
			}
			fclose(fp);
		}
	}
}
