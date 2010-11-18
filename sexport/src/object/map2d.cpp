#include "../defines.h"
#include "../log.h"
#include "map2d.h"
#include "../exporter.h"
#include "../platform.h"
#include "../strcache.h"

#include <string>
#include <sstream>

#define TAG "[Sprite] "

MapObject::MapObject(const char *name)
	: pType(NULL)
	, fX(0.0f)
	, fY(0.0f)
	, fW(0.0f)
	, fH(0.0f)
{

}

MapObject::~MapObject()
{
	if (pType)
		free(pType);

	pType = NULL;
}

void MapObject::Write(FILE *fp, Exporter *e)
{
}

void MapObject::SetType(const char *type)
{
	if (type)
	{
		pStringCache->AddString(type);
		pType = strdup(type);
	}
}

void MapObject::SetPosition(f32 x, f32 y)
{
	fX = x;
	fY = y;
}

void MapObject::SetSize(f32 w, f32 h)
{
	fW = w;
	fH = h;
}

IMapLayer::IMapLayer(const char *name)
	: bVisibility(true)
	, fOpacity(1.0f)
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
	, vObjects()
{

}

MapLayerObject::~MapLayerObject()
{
	vObjects.clear();
}

void MapLayerObject::Add(MapObject *obj)
{
	vObjects.push_back(obj);
}

void MapLayerObject::Write(FILE *fp, Exporter *e)
{
}

MapLayerTiled::MapLayerTiled(const char *name)
	: IMapLayer(name)
	, vTiles()
{

}

MapLayerTiled::~MapLayerTiled()
{
	vTiles.clear();
}

void MapLayerTiled::CreateTiles(const char *text)
{
	std::stringstream ss(text);
	int i = 0;

	while (ss >> i)
	{
		vTiles.push_back(i);
		if (ss.peek() == ',')
			ss.ignore();
	}
}

void MapLayerTiled::Write(FILE *fp, Exporter *e)
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

	pStringCache->AddString(name);
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

