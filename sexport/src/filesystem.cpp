#include "filesystem.h"
#include "log.h"

#include <stdio.h>

#define TAG	"[FileSystem] "

FileSystem FileSystem::instance;

FileSystem::FileSystem() :
	bInitialized(false),
	mapSystemPath()
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::Release()
{
	SystemPathMapIterator it = mapSystemPath.begin();
	SystemPathMapIterator end = mapSystemPath.end();

	for (; it != end; ++it)
	{
		bfs::path *p = (*it).second;
		delete p;
	}

	mapSystemPath.clear();
	if (!mapSystemPath.size())
	{
		SystemPathMap().swap(mapSystemPath);
	}
	else
	{
		Log(TAG "Failed to release system paths memory.");
		HALT;
	}
}

void FileSystem::Initialize()
{
	this->bInitialized = true;
}

void FileSystem::Shutdown()
{
	this->Release();
	this->bInitialized = false;
}

void FileSystem::AddSystemPath(const char *identifier, const char *path)
{
	bfs::path *p = new bfs::path(path);
	if (mapSystemPath.find(identifier) == mapSystemPath.end())
		mapSystemPath[identifier] = p;
	else
		Log(TAG "System path %s already exists.\n", identifier);
}

inline bool FileSystem::IsValidPath(const char *path)
{
	ASSERT_NULL(path);

	bool ret = FALSE;
	bfs::path p(path);
	if (bfs::exists(p))
		ret = TRUE;
	return ret;
}

inline bool FileSystem::IsDirectory(const char *path)
{
	bool ret = false;
	if (this->IsValidPath(path))
	{
		bfs::path p(path);
		if (bfs::is_directory(p))
			ret = true;
	}

	return ret;
}

inline bfs::path *FileSystem::GetSystemPath(const char *identifier)
{
	ASSERT_NULL(identifier);

	bfs::path *ret = NULL;
	if (mapSystemPath.find(identifier) != mapSystemPath.end())
		ret = mapSystemPath[identifier];

	return ret;
}

bfs::path FileSystem::GetRelativePath(const bfs::path fullPath, const bfs::path systemPath)
{
	return bfs::path(this->GetRelativePathC(fullPath.string().c_str(), systemPath.string().c_str()));
	/*
	std::string str1(fullPath.string());
	std::string str2(systemPath.string());

	std::string str3;

	str3 = str1.substr(str2.size() + 1, str1.size());
	return bfs::path(str3);
	*/
}

const char *FileSystem::GetRelativePathC(const char *full, const char *system) const
{
	const char *path = &full[strlen(system)];

	while (path[0] == '\\' || path[0] == '/')
			path = &path[1];

	return path;
}
