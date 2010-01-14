#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "exporter.h"
#include "cmdlineparser.h"
#include "platform/wiiplatform.h"
#include "platform/iphplatform.h"
#include "platform/sdlplatform.h"

IPlatform 	*pPlatform = NULL;
bool 		gVerbose = false;
bool 		gQuiet = false;

void SetXmlFile(const char *value, void *data);
void SetPlatformString(const char *value, void *data);
void Rebuild(const char *value, void *data);
void EnablePackages(const char *value, void *data);
void SetAlignment(const char *value, void *data);
void EnableCompression(const char *value, void *data);
void EnablePackageResources(const char *value, void *data);
void RebuildDirectoryTree(const char *value, void *data);
void EnableVerboseMode(const char *value, void *data);
void EnableQuietMode(const char *value, void *data);

int main(int argc, const char **argv)
{
	char xmlfile[256];
	char platform[128];
	bool rebuild = false;
	bool packages = false;
	bool compression = false;
	bool resources = false;
	bool dirs = false;
	u8	 alignment = ALIGN_32;

	CmdLineParser *parser = new CmdLineParser(10);
	parser->Add("i", "input", false, false, true, SetXmlFile, xmlfile);
	parser->Add("p", "platform", false, false, true, SetPlatformString, platform);
	parser->Add("r", "rebuild", true, false, false, Rebuild, &rebuild);
	parser->Add("k", "packages", true, false, false, EnablePackages, &packages);
	parser->Add("a", "alignment", true, false, true, SetAlignment, &alignment);
	parser->Add("c", "compress", true, false, false, EnableCompression, &compression);
	parser->Add("d", "add_resources", true, false, false, EnablePackageResources, &resources);
	parser->Add("t", "tree", true, false, true, RebuildDirectoryTree, &dirs);
	parser->Add("v", "verbose", true, false, false, EnableVerboseMode, &gVerbose);
	parser->Add("q", "quiet", true, false, false, EnableQuietMode, &gQuiet);

	fprintf(stdout, "Seed Exporter (c) Danny Angelo Carminati Grein 2008\n");
	if (!parser->Parse(argc, argv, true))
	{
		fprintf(stdout, "\nusage: %s -i [input xml] -p [platform] [params]\n\n", argv[0]);
		fprintf(stdout, "PARAMS\n");
		fprintf(stdout, "\t\t-r, --rebuild\tRebuild all files.\n");
		fprintf(stdout, "\t\t-k, --packages\tCreate group packages.\n");
		fprintf(stdout, "\t\t-a [value], --alignment [value]\tOutput data alignment.\n");
		fprintf(stdout, "\t\t-c, --compress\tEnable output data compression.\n");
		fprintf(stdout, "\t\t-t,--tree\tBuild directory structure only (not implemented!)\n");
		fprintf(stdout, "\t\t-v,--verbose\tVerbose mode.\n");
		fprintf(stdout, "\t\t-q,--quiet\tQuiet mode.\n");
		fprintf(stdout, "\n");

		return EXIT_FAILURE;
	}

	e->bfsExeName = bfs::path(argv[0]);
	e->Process(xmlfile, platform, rebuild, packages, alignment, compression, resources);

	return EXIT_SUCCESS;
}

void SetXmlFile(const char *value, void *data)
{
	if (!value)
		return;

	char *xmlfile = static_cast<char*>(data);
	strncpy(xmlfile, value, strlen(value));
	xmlfile[strlen(value)] = '\0';
}

void SetPlatformString(const char *value, void *data)
{
	if (!value)
		return;

	char *p = static_cast<char*>(data);
	strncpy(p, value, strlen(value));
	p[strlen(value)] = '\0';

	if (!strcasecmp(value, "wii"))
	{
		pPlatform = new WiiPlatform();
	}
	else if (!strcasecmp(value, "iph"))
	{
		pPlatform = new IphPlatform();
	}
	else if (!strcasecmp(value, "sdl"))
	{
		pPlatform = new SdlPlatform();
	}
	else
	{
		fprintf(stderr, "Platform %s not supported.\n", value);
	}
}

void Rebuild(const char *value, void *data)
{
	*static_cast<bool*>(data) = true;
}

void EnablePackages(const char *value, void *data)
{
	*static_cast<bool*>(data) = true;
}

void EnablePackageResources(const char *value, void *data)
{
	*static_cast<bool*>(data) = true;
}

void RebuildDirectoryTree(const char *value, void *data)
{
	*static_cast<bool*>(data) = true;
}

void EnableQuietMode(const char *value, void *data)
{
	*static_cast<bool*>(data) = true;
}

void EnableVerboseMode(const char *value, void *data)
{
	*static_cast<bool*>(data) = true;
}

void SetAlignment(const char *value, void *data)
{
	if (!strcmp(value, "32"))
		*static_cast<u8*>(data) = ALIGN_32;
	else if (!strcmp(value, "16"))
		*static_cast<u8*>(data) = ALIGN_16;
	else if (!strcmp(value, "8"))
		*static_cast<u8*>(data) = ALIGN_8;
}

void EnableCompression(const char *value, void *data)
{
	*static_cast<bool*>(data) = true;
}
