#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#include "defines.h"
#include "group.h"

#define PKG_MAGIC	0x00504B47
#define PKG_VERSION	0x00000001

class Package
{
	private:
		struct PackageEntry
		{
			BOOL		bIsObject;
			void		*pData;
			bfs::path  	bfsPath;
			bfs::path   bfsName;
			u32 		iOffsetAddr;
			u32 		iOffset;
			// filesize
			// filename
			PackageEntry() :  bIsObject(false), pData(NULL), bfsPath(), bfsName(), iOffsetAddr(0), iOffset(0) {}
			PackageEntry(const PackageEntry &);
			const PackageEntry &operator=(const PackageEntry &);
		};

		struct PackageHeader
		{
			ObjectHeader block;
			u32 filesAmount;
			// packages FS table
		};

		typedef std::vector<PackageEntry *> 	PackageEntryVector;
		typedef PackageEntryVector::iterator	PackageEntryVectorIterator;

		typedef std::vector<std::string> PackagedFilesVector;
		typedef PackagedFilesVector::iterator PackagedFilesIterator;

	private:
		Group 				*pGroup;
		PackageEntryVector 	vPackageEntry;
		PackagedFilesVector	vPackageFile;
		bfs::path			bfsPath;

	private:
		Package(const Package &p);
		const Package &operator=(const Package &p) const;

		u32 GetFilesAmount();
		void PopulatePackageEntries();
		void WritePackageEntriesAndUpdateOffsetAddr(FILE *fpkg);
		void WriteObjectsAndUpdateOffset(FILE *fpkg);
		void WriteResourcesAndUpdateOffset(FILE *fpkg);
		void WriteOffset(FILE *fpkg);

		bfs::path SubtractPath(bfs::path p1, bfs::path p2);

	public:
		Package(Group *g);
		~Package();

		void Write();
		void RemovePackagedFiles();
		bool CanBeAdded(std::string filename);
};

#endif
