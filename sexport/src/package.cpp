#include "package.h"
#include <sstream>
#include "exporter.h"
#include "platform.h"
#include "cache.h"


/*
Package::Package(const Package &p)
	: pGroup(NULL)
	, vPackageEntry()
	, bfsPath()
{
}

const Package &Package::operator=(const Package &p) const
{
	return *this;
}
*/

Package::Package(Group *g)
	: pGroup(NULL)
	, vPackageEntry()
	, bfsPath()
{
	if (!g)
	{
		fprintf(stderr, "Package created without a valid group.\n");
		exit(-1);
	}
	pGroup = g;

	std::string pkgStr(g->GetName());
	pkgStr = pkgStr + ".package";

	bfs::path tmp(pkgStr);

	bfs::path foo = e->GetOutputPath();
	this->bfsPath = foo / tmp;

	pCache->AddFilename(bfsPath.string().c_str());
}

Package::~Package()
{
}

void Package::Write()
{
	FILE *fp = fopen(bfsPath.string().c_str(), "wb+");
	if (!fp)
	{
		fprintf(stderr, "\t[PACKAGE] Unable to open package file in write-mode: %s.\n", bfsPath.filename().c_str());
		return;
	}

	this->PopulatePackageEntries();

	PackageHeader ph;
	ph.block.platform = pPlatform->Swap32(pPlatform->GetCode());
	ph.block.magic = pPlatform->Swap32(PKG_MAGIC);
	ph.block.version = pPlatform->Swap32(PKG_VERSION);
	ph.filesAmount = pPlatform->Swap32(vPackageEntry.size());
	fwrite(&ph, sizeof(PackageHeader), 1, fp);

	this->WritePackageEntriesAndUpdateOffsetAddr(fp);
	this->WriteObjectsAndUpdateOffset(fp);
	this->WriteResourcesAndUpdateOffset(fp);
	this->WriteOffset(fp);

	fclose(fp);
	printf("\tPackage %s written succesfully.\n", pGroup->GetName());
}

void Package::WriteOffset(FILE *fpkg)
{
	for (u32 i = 0; i < vPackageEntry.size(); i++)
	{
		rewind(fpkg);
		fseek(fpkg, vPackageEntry[i]->iOffsetAddr, SEEK_SET);
		u32 swpOffset = pPlatform->Swap32(vPackageEntry[i]->iOffset);
		fwrite(&swpOffset, sizeof(swpOffset), 1, fpkg);
	}
}

void Package::WriteResourcesAndUpdateOffset(FILE *fpkg)
{
	u8 align = e->GetAlignment();
	for (u32 i = 0; i < vPackageEntry.size(); i++)
	{
		if (vPackageEntry[i]->bIsObject)
			continue;

		FILE *fpTmp = fopen(vPackageEntry[i]->bfsPath.string().c_str(), "rb+");
		if (!fpTmp)
		{
			fprintf(stderr, "\t[PACKAGE] Unable to open resource file %s.\n", vPackageEntry[i]->bfsPath.string().c_str());
			exit(EXIT_FAILURE);
		}

		size_t pos = ftell(fpkg);
		if (pos % align)
		{
			pos += align - (pos % align);
			fseek(fpkg, pos - ftell(fpkg), SEEK_CUR);
		}

		vPackageEntry[i]->iOffset = pos;

		u8 data;
		while(!feof(fpTmp))
		{
			fread(&data, sizeof(u8), 1, fpTmp);
			fwrite(&data, sizeof(u8), 1, fpkg);
		}

		fclose(fpTmp);

		//bfs::remove(vPackageEntry[i]->bfsPath); // FIXME
	}
}

void Package::WriteObjectsAndUpdateOffset(FILE *fpkg)
{
	u8 align = e->GetAlignment();
	for (u32 i = 0; i < vPackageEntry.size(); i++)
	{
		if (!vPackageEntry[i]->bIsObject)
			break;

		vPackageEntry[i]->iOffset = ftell(fpkg);

		FILE *fpTmp = fopen(vPackageEntry[i]->bfsPath.string().c_str(), "rb+");
		if (!fpTmp)
		{
			fprintf(stderr, "\t[PACKAGE] Unable to open object file %s.\n", vPackageEntry[i]->bfsPath.string().c_str());
			exit(EXIT_FAILURE);
		}

		size_t pos = ftell(fpkg);
		if (pos % align)
		{
			pos += align - (pos % align);
			fseek(fpkg, pos - ftell(fpkg), SEEK_CUR);
		}

		vPackageEntry[i]->iOffset = ftell(fpkg);

		u8 data;
		while (!feof(fpTmp))
		{
			fread(&data, sizeof(u8), 1, fpTmp);
			fwrite(&data, sizeof(u8), 1, fpkg);
		}

		fclose(fpTmp);

		//bfs::remove(vPackageEntry[i]->bfsPath); // FIXME
	}
}

void Package::WritePackageEntriesAndUpdateOffsetAddr(FILE *fpkg)
{
	for (u32 i = 0; i < vPackageEntry.size(); i++)
	{
		struct
		{
			u32 iOffset;
			u32 iFileSize;
		} tmp;

		tmp.iOffset = pPlatform->Swap32(vPackageEntry[i]->iOffset);
		if (!bfs::exists(vPackageEntry[i]->bfsPath))
		{
			fprintf(stderr, "[PACKAGE] File '%s' doesn't exist.", vPackageEntry[i]->bfsName.string().c_str());
			exit(EXIT_FAILURE);
		}

		printf("[PACKAGE] file: %s", (vPackageEntry[i]->bfsPath.string().c_str()));
		printf(" - size: %d\n", static_cast<int>(bfs::file_size(vPackageEntry[i]->bfsPath)));
		tmp.iFileSize = pPlatform->Swap32(bfs::file_size(vPackageEntry[i]->bfsPath));

		vPackageEntry[i]->iOffsetAddr = ftell(fpkg);

		fwrite(&tmp, sizeof(tmp), 1, fpkg);
		fwrite(vPackageEntry[i]->bfsName.string().c_str(), strlen(vPackageEntry[i]->bfsName.string().c_str()) + 1, 1, fpkg);
	}
}

void Package::PopulatePackageEntries()
{
	ObjectMapIterator oit = pGroup->GetFirstObject();
	for (; oit != pGroup->GetLastObject(); ++oit)
	{
		IObject *o = ((*oit).second);

		if (this->CanBeAdded(o->GetOutputPath().string()))
		{
			PackageEntry *pe = new PackageEntry();
			pe->bIsObject = TRUE;
			pe->bfsPath = o->GetOutputPath();
			pe->bfsName = this->SubtractPath(pe->bfsPath, e->GetOutputPath());

			if (!strcmp(pe->bfsName.extension().c_str(), ".tpl") || !strcmp(pe->bfsName.extension().c_str(), ".adpcm") || !strcmp(pe->bfsName.extension().c_str(), ".ogg"))
				//int a = 1;
			{
				fprintf(stderr, "\nNAO DEVERIA ESTAR EM PACKAGE: %s\n", pe->bfsName.string().c_str());
				exit(EXIT_FAILURE);
			}
			pe->iOffset = 0;
			pe->iOffsetAddr = 0;

			vPackageEntry.push_back(pe);

			for (u32 j = 0; j < o->GetFilesAmount(); j++)
			{
				if (this->CanBeAdded(o->GetFileVectorPath(j).string()))
				{
					PackageEntry *pex = new PackageEntry();
					pex->bIsObject = TRUE;
					pex->bfsPath = o->GetFileVectorPath(j);
					pex->bfsName = this->SubtractPath(pex->bfsPath, e->GetOutputPath());
					if (!strcmp(pex->bfsName.extension().c_str(), ".tpl") || !strcmp(pex->bfsName.extension().c_str(), ".adpcm") || !strcmp(pex->bfsName.extension().c_str(), ".ogg"))
					//	int a = 1;
					{
						fprintf(stderr, "\nNAO DEVERIA ESTAR EM PACKAGE: %s\n", pex->bfsName.string().c_str());
						exit(EXIT_FAILURE);
					}
					pex->iOffset = 0;
					pex->iOffsetAddr = 0;

					vPackageEntry.push_back(pex);
				}
			}
		}
	}

	ResourceMapIterator rit = pGroup->GetFirstResource();
	for (; rit != pGroup->GetLastResource(); ++rit)
	{
		IResource *r = ((*rit).second);

		if (r->GetType() != RESOURCE_MUSIC)
		{
			if (this->CanBeAdded(r->GetOutputPath().string()))
			{
				PackageEntry *pe = new PackageEntry();
				pe->bIsObject = FALSE;
				pe->bfsPath = r->GetOutputPath();
				//pe->bfsName = bfs::path(r->GetFilename());
				pe->bfsName = this->SubtractPath(pe->bfsPath, e->GetOutputPath());

				pe->iOffset = 0;
				pe->iOffsetAddr = 0;

				vPackageEntry.push_back(pe);
			}
		}

		for (u32 j = 0; j < r->GetFilesAmount(); j++)
		{
			if (this->CanBeAdded(r->GetFileVectorPath(j).string()))
			{
				PackageEntry *pe = new PackageEntry();
				pe->bIsObject = FALSE;
				pe->pData = NULL;
				pe->bfsPath = r->GetFileVectorPath(j);
				pe->bfsName = this->SubtractPath(pe->bfsPath, e->GetOutputPath());
				pe->iOffset = 0;
				pe->iOffsetAddr = 0;

				vPackageEntry.push_back(pe);
			}
		}
	}
}

bfs::path Package::SubtractPath(bfs::path p1, bfs::path p2)
{
	std::string str1(p1.string());
	std::string str2(p2.string());

	std::string str3;

	str3 = str1.substr(str2.size() + 1, str1.size());
	return bfs::path(str3);
}

void Package::RemovePackagedFiles()
{
	for (u32 i = 0; i < vPackageEntry.size(); i++)
	{
		bfs::remove(vPackageEntry[i]->bfsPath);
		if (bfs::is_directory(vPackageEntry[i]->bfsPath.branch_path()) && bfs::is_empty(vPackageEntry[i]->bfsPath.branch_path()))
		{
			bfs::remove(vPackageEntry[i]->bfsPath.branch_path());
		}
	}
}

bool Package::CanBeAdded(std::string filename)
{
	s32 s = (s32)vPackageFile.size();
	for (s32 i = 0; i < s; i++)
	{
		//if (!strcasecmp(vecFilename[i], filename))
		//	return i;
		if (vPackageFile[i] == filename)
			return false;
	}

	vPackageFile.push_back(filename);
	return true;
}
