#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "defines.h"
#include <map>

class FileSystem
{
	typedef std::map<const char *, bfs::path *, StringComparator>	SystemPathMap;
	typedef SystemPathMap::iterator									SystemPathMapIterator;


	public:
		FileSystem();
		~FileSystem();

		void Initialize();
		void Shutdown();

		void AddSystemPath(const char *identifier, const char *path);
		bfs::path *GetSystemPath(const char *identifier);
		void AddPath(const char *systemPathIdentifier, const char *filename);
		void GetFullPath(const char *systemPathIdentifier, const char *filename);
		bfs::path GetRelativePath(const bfs::path fullPath, const bfs::path systemPath);
		const char *GetRelativePathC(const char *full, const char *system) const;

		bool IsValidPath(const char *path);
		bool IsDirectory(const char *path);

	public:
		static FileSystem instance;

	private:
		FileSystem(const FileSystem &);
		FileSystem &operator=(const FileSystem &);

		void Release();


	private:
		bool bInitialized;

		SystemPathMap mapSystemPath;
};

FileSystem *const pFileSystem = &FileSystem::instance;


#endif // __FILESYSTEM_H__
