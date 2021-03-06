#include "../defines.h"
#include "../log.h"
#include "map2d.h"
#include "../exporter.h"
#include "../platform.h"
#include "../strcache.h"

#include <string>
#include <sstream>

#define TAG "[Sprite] "

MapSpriteEntity::MapSpriteEntity(const char *name)
	: fX(0.0f)
	, fY(0.0f)
	, fAngle(0.0f)
	, fScale(0.0f)
	, nBlendType(BlendNone)
	, pxColor(0)
	, bCollidable(FALSE)
	, pSprite(NULL)
{
	if (!name)
	{
		Error(ERROR_CREATING_OBJECT, TAG "Map Sprite Entity without name.\n");
	}

	pStringCache->AddString(name);
	pName = strdup(name);
}

MapSpriteEntity::~MapSpriteEntity()
{
	if (pName)
		free(pName);

	pName = NULL;
}

void MapSpriteEntity::Write(FILE *fp, Exporter *e)
{
	LayerSpriteEntityHeader hdr;
	hdr.iNameId = pPlatform->Swap32(pStringCache->GetStringId(pName));
	hdr.fPosX = pPlatform->Swapf32(fX);
	hdr.fPosY = pPlatform->Swapf32(fY);
	hdr.fAngle = pPlatform->Swapf32(fAngle);
	hdr.fScale = pPlatform->Swapf32(fScale);
	hdr.iBlendType = pPlatform->Swap32(nBlendType);
	hdr.iColor = pPlatform->Swap32(pxColor);
	hdr.iCollidable = pPlatform->Swap32(bCollidable);
	hdr.iSpriteFileId = pPlatform->Swap32(0xffffffff);

	if (pSprite)
	{
		std::string sfile(e->GetOutputPath().string());
		sfile += "/";
		sfile += pSprite;
		sfile += ".sprite";

		const char *file = sfile.c_str();
		u32 fid = pCache->GetFilenameId(file);

		//fwrite(sprite, strlen(sprite), 1, fp);
		//fwrite(".sprite", strlen(".sprite")+1, 1, fp);
		hdr.iSpriteFileId = pPlatform->Swap32(fid);
	}

	fwrite(&hdr, sizeof(LayerSpriteEntityHeader), 1, fp);
}

void MapSpriteEntity::SetPosition(f32 x, f32 y)
{
	fX = x;
	fY = y;
}

void MapSpriteEntity::SetScale(f32 scale)
{
	fScale = scale;
}

void MapSpriteEntity::SetAngle(f32 ang)
{
	fAngle = ang;
}

void MapSpriteEntity::SetBlendType(eBlendType type)
{
	nBlendType = type;
}

void MapSpriteEntity::SetPixelColor(u8 r, u8 g, u8 b, u8 a)
{
	pxColor = (r << 24) | (g << 16) | (b << 8) | (a);
}

void MapSpriteEntity::SetCollidable(BOOL b)
{
	bCollidable = b;
}

MapObject::MapObject(const char *name)
	: pType(NULL)
	, pProps(NULL)
	, fX(0.0f)
	, fY(0.0f)
	, fW(0.0f)
	, fH(0.0f)
{
	if (!name)
	{
		Error(ERROR_CREATING_OBJECT, TAG "Map Object without name.\n");
	}

	pStringCache->AddString(name);
	pName = strdup(name);
}

MapObject::~MapObject()
{
	if (pName)
		free(pName);

	pName = NULL;

	if (pType)
		free(pType);

	pType = NULL;

	if (pProps)
		free(pProps);

	pProps = NULL;
}

void MapObject::Write(FILE *fp, Exporter *e)
{
	LayerObjectHeader hdr;
	hdr.iNameId = pPlatform->Swap32(pStringCache->GetStringId(pName));
	hdr.iTypeId = pPlatform->Swap32(pStringCache->GetStringId(pType));
	if (pProps)
		hdr.iPropertiesId = pPlatform->Swap32(pStringCache->GetStringId(pProps));
	else
		hdr.iPropertiesId = 0xffffffff;
	hdr.fPosX = pPlatform->Swapf32(fX);
	hdr.fPosY = pPlatform->Swapf32(fY);
	hdr.fWidth = pPlatform->Swapf32(fW);
	hdr.fHeight = pPlatform->Swapf32(fH);
	fwrite(&hdr, sizeof(LayerObjectHeader), 1, fp);
}

void MapObject::SetType(const char *type)
{
	if (type)
	{
		pStringCache->AddString(type);
		pType = strdup(type);
	}
}

void MapObject::SetProperties(const char *props)
{
	if (props)
	{
		pStringCache->AddString(props);
		pProps = strdup(props);
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
	: nType(LayerTypeTiled)
	, bVisibility(true)
	, fOpacity(1.0f)
	, iDataIndex(0xFFFFFFFF)
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

void IMapLayer::Write(FILE *fp, Exporter *e)
{
	LayerHeader hdr;
	hdr.iType = pPlatform->Swap32(nType);
	hdr.iNameId = pPlatform->Swap32(pStringCache->GetStringId(pName));
	hdr.iVisible = pPlatform->Swap32(bVisibility);
	hdr.fOpacity = pPlatform->Swapf32(fOpacity);
	hdr.iDataIndex = pPlatform->Swap32(iDataIndex);
	fwrite(&hdr, sizeof(LayerHeader), 1, fp);
}


MapLayerObject::MapLayerObject(const char *name)
	: IMapLayer(name)
	, vObjects()
{
	nType = LayerTypeObject;
}

MapLayerObject::~MapLayerObject()
{
	vObjects.clear();
}

void MapLayerObject::Add(MapObject *obj)
{
	vObjects.push_back(obj);
}

void MapLayerObject::WriteData(FILE *fp, Exporter *e)
{
	MapObjectIterator it;
	MapObjectIterator end;

	u32 amount = pPlatform->Swap32(vObjects.size());
	fwrite(&amount, sizeof(amount), 1, fp);

	// Write Objects
	it = vObjects.begin();
	end = vObjects.end();
	for (; it != end; ++it)
	{
		MapObject *obj = (*it);
		obj->Write(fp, e);
	}
}


MapLayerTiled::MapLayerTiled(const char *name)
	: IMapLayer(name)
	, vTiles()
{
	nType = LayerTypeTiled;
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

void MapLayerTiled::WriteData(FILE *fp, Exporter *e)
{
	TileIterator it;
	TileIterator end;

	// Write Tiles
	it = vTiles.begin();
	end = vTiles.end();
	for (; it != end; ++it)
	{
		Tile tile = (*it);

		u32 data = pPlatform->Swap32(tile);
		fwrite(&data, sizeof(u32), 1, fp);
	}
}


MapLayerMosaic::MapLayerMosaic(const char *name)
	: IMapLayer(name)
	, vSprites()
{
	nType = LayerTypeMosaic;
}

MapLayerMosaic::~MapLayerMosaic()
{
	vSprites.clear();
}

void MapLayerMosaic::Add(MapSpriteEntity *obj)
{
	vSprites.push_back(obj);
}

void MapLayerMosaic::WriteData(FILE *fp, Exporter *e)
{
	MapSpriteEntityIterator it;
	MapSpriteEntityIterator end;

	u32 amount = pPlatform->Swap32(vSprites.size());
	fwrite(&amount, sizeof(amount), 1, fp);

	// Write Objects
	it = vSprites.begin();
	end = vSprites.end();
	for (; it != end; ++it)
	{
		MapSpriteEntity *obj = (*it);
		obj->Write(fp, e);
	}
}


Map2D::Map2D(const char *name)
	: vLayers()
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

	IMapLayerIterator it = vLayers.begin();
	IMapLayerIterator end = vLayers.end();
	for (; it != end; ++it)
	{
		IMapLayer *layer = (*it);
		delete layer;
	}

	vLayers.clear();
}

void Map2D::Add(IMapLayer *layer)
{
	vLayers.push_back(layer);
}

void Map2D::Write(FILE *fp, Exporter *e)
{
	MapHeader hdr;

	hdr.block.magic = pPlatform->Swap32((u32)MAP_MAGIC);
	hdr.block.version = pPlatform->Swap32(MAP_VERSION);
	hdr.block.platform = pPlatform->Swap32(pPlatform->GetCode());

	hdr.iType = pPlatform->Swap32(MapType2D);
	hdr.iMapWidth = pPlatform->Swap32(iWidth);
	hdr.iMapHeight = pPlatform->Swap32(iHeight);
	hdr.iTileWidth = pPlatform->Swap32(iTileWidth);
	hdr.iTileHeight = pPlatform->Swap32(iTileHeight);
	hdr.iLayerCount = pPlatform->Swap32(vLayers.size());
	fwrite(&hdr, sizeof(MapHeader), 1, fp);

	IMapLayerIterator it;
	IMapLayerIterator end;

	size_t pos = 0;

	size_t layers = ftell(fp); // the start of layers block
	// Write Layers
	it = vLayers.begin();
	end = vLayers.end();
	for (; it != end; ++it)
	{
		IMapLayer *l = (*it);
		l->Write(fp, e);
	}

	size_t data = ftell(fp); // the start of data block
	// Write Layer Custom Data
	it = vLayers.begin();
	end = vLayers.end();
	for (; it != end; ++it)
	{
		IMapLayer *l = (*it);

		pos = ftell(fp);
		l->WriteData(fp, e);
		l->SetDataIndex(pos - data);
	}

	// Fix Layer Index to Point to the actual Data
	// Write Layers
	fseek(fp, layers, SEEK_SET);
	it = vLayers.begin();
	end = vLayers.end();
	for (; it != end; ++it)
	{
		IMapLayer *l = (*it);
		l->Write(fp, e);
	}
}

