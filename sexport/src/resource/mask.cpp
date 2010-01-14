#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sstream>
#include <string>

#include "mask.h"
#include "../exporter.h"
#include "../config.h"
#include "../utils.h"
#include "../platform.h"
#include "../log.h"


#define TAG		"[Mask] "


Mask::Mask(const char *name, const char *lang)
	: IResource(lang)
{
	iType = RESOURCE_MASK;
	if (name)
		this->SetName(name);
	else
	{
		Error(ERROR_EXPORT_MASK_RESOURCE_MISSING_ATTRIB, TAG "A mask resource requires a name attribute.");
	}
}

void Mask::SetFilename(const char *fileName)
{
	IResource::SetFilename(fileName);
	bfsOutputPath = pPlatform->GetOutputPath(this);
	bfsInputPath = pPlatform->GetInputPath(this);

	if (!bfs::exists(bfsInputPath))
	{
		Error(ERROR_FILE_NOT_FOUND, TAG "Mask file '%s' not found.", bfsInputPath.string().c_str());
	}
}
