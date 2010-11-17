#ifndef __MAP2D_H__
#define __MAP2D_H__

#include "../defines.h"
#include "../iobject.h"

#define LayerVector std::vector<IMapLayer *>
#define LayerIterator LayerVector::iterator
#define LayerObjectVector std::vector<LayerObject *>
#define LayerObjectIterator LayerObjectVector::iterator

#define MAP_MAGIC		0x0050414d
#define MAP_VERSION		0x00000001

#define LAY_MAGIC		0x0059414c
#define LAY_VERSION		0x00000001

#define LTD_MAGIC		0x0044544c
#define LTD_VERSION		0x00000001

#define LBJ_MAGIC		0x004a424c
#define LBJ_VERSION		0x00000001

enum eMapType
{
	MapType2D,
	MapTypeMax
};

enum eLayerType
{
	LayerTypeTiled,
	LayerTypeObject,
	LayerTypeTileless,
	LayerTypeMax,
};

struct MapObjectHeader
{
	ObjectHeader block;
	u32 iType;
	u32 iMapWidth;
	u32 iMapHeight;
	u32 iTileWidth;
	u32 iTileHeight;
	u32 iLayerCount;
};

struct LayerObjectHeader
{
	ObjectHeader block;
	u32 iType;
	u32 iNameId;
	u32 iVisible;
	f32 fOpacity;
	u32 iDataIndex;
};

struct LayerTiledObjectHeader
{
	ObjectHeader block;
	// tiles id = map.width * map.height * sizeof(u32)
};

struct LayerObjectObjectHeader
{
	ObjectHeader block;
	u32 iNameId;
	u32 iTypeId;
	u32 iPosX;
	u32 iPosY;
	u32 iWidth;
	u32 iHeight;
};

class Exporter;

class IMapLayer : public IObject
{
	private:
		IMapLayer(const IMapLayer &);
		bool operator=(const IMapLayer &);

	public:
		IMapLayer(const char *name);
		virtual ~IMapLayer();
};

class MapLayerObject : public IMapLayer
{
	private:
		MapLayerObject(const MapLayerObject &);
		bool operator=(const MapLayerObject &);

	public:
		MapLayerObject(const char *name);
		virtual ~MapLayerObject();
};

class MapLayerTiled : public IMapLayer
{
	private:
		MapLayerTiled(const MapLayerTiled &);
		bool operator=(const MapLayerTiled &);

	public:
		MapLayerTiled(const char *name);
		virtual ~MapLayerTiled();
};

class Map2D : public IObject
{
	private:
		Map2D(const Map2D &);
		bool operator=(const Map2D &);

		LayerVector layers;
		u32 iTileWidth;
		u32 iTileHeight;
		u32 iWidth;
		u32 iHeight;

	public:
		Map2D(const char *name);
		virtual ~Map2D();

		void Add(IMapLayer *layer);
		void Write(FILE *fp, Exporter *e);

		void SetTileSize(u32 width, u32 height)
		{
			iTileWidth = width;
			iTileHeight = height;
		}

		void SetSize(u32 width, u32 height)
		{
			iWidth = width;
			iHeight = height;
		}
};

#endif // __MAP2D_H__
