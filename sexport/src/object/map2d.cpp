#include "../defines.h"
#include "../log.h"
#include "map2d.h"
#include "../exporter.h"
#include "../platform.h"
#include "../strcache.h"

#include <string>
#include <sstream>

#define TAG "[Sprite] "

IMapLayer::IMapLayer(const char *name)
{
	if (!name)
	{
		Error(ERROR_CREATING_OBJECT, TAG "Map Layer without name.\n");
	}

	pStringCache->AddString(name);
	pName = strdup(name);
}

IMapLayer::~IMapLayer()
{

}

MapLayerObject::MapLayerObject(const char *name)
	: IMapLayer(name)
{

}

MapLayerObject::~MapLayerObject()
{

}

MapLayerTiled::MapLayerTiled(const char *name)
	: IMapLayer(name)
{

}

MapLayerTiled::~MapLayerTiled()
{

}

Map2D::Map2D(const char *name)
	: layers()
	, iTileWidth(32)
	, iTileHeight(32)
	, iWidth(100)
	, iHeight(100)
{
	iType = OBJECT_MAP;

	if (!name)
	{
		Error(ERROR_CREATING_OBJECT, TAG "Map2D without name.\n");
	}

	pName = strdup(name);
	bfsOutputPath = pPlatform->GetOutputPath(this);
}

Map2D::~Map2D()
{
	if (pName)
		free(pName);
	pName = NULL;

	LayerIterator it = layers.begin();
	LayerIterator end = layers.end();
	for (; it != end; ++it)
	{
		IMapLayer *layer = (*it);
		delete layer;
	}

	layers.clear();
}

void Map2D::Add(IMapLayer *layer)
{
	layers.push_back(layer);
}

void Map2D::Write(FILE *fp, Exporter *e)
{
}

