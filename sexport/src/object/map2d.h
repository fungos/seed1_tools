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

class MapObject : public IObject
{
	private:
		MapObject(const MapObject &);
		bool operator=(const MapObject &);

		char *pType;
		f32 fX;
		f32 fY;
		f32 fW;
		f32 fH;

	public:
		MapObject(const char *name);
		virtual ~MapObject();

		void SetType(const char *type);
		void SetPosition(f32 x, f32 y);
		void SetSize(f32 w, f32 h);

		virtual void Write(FILE *fp, Exporter *e);
};

class IMapLayer : public IObject
{
	private:
		IMapLayer(const IMapLayer &);
		bool operator=(const IMapLayer &);

		bool bVisibility;
		f32 fOpacity;

	public:
		IMapLayer(const char *name);
		virtual ~IMapLayer();

		void SetVisibility(bool b)
		{
			bVisibility = b;
		}

		bool GetVisibility() const
		{
			return bVisibility;
		}

		void SetOpacity(f32 opacity)
		{
			fOpacity = opacity;
		}

		f32 GetOpacity() const
		{
			return fOpacity;
		}
};

class MapLayerObject : public IMapLayer
{
	private:
		MapLayerObject(const MapLayerObject &);
		bool operator=(const MapLayerObject &);

		typedef std::vector<MapObject *> VectorObjects;
		typedef VectorObjects::iterator VectorObjectsIterator;
		VectorObjects vObjects;

	public:
		MapLayerObject(const char *name);
		virtual ~MapLayerObject();

		void Add(MapObject *obj);

		virtual void Write(FILE *fp, Exporter *e);
};

class MapLayerTiled : public IMapLayer
{
	private:
		MapLayerTiled(const MapLayerTiled &);
		bool operator=(const MapLayerTiled &);

		std::vector<int> vTiles;

	public:
		MapLayerTiled(const char *name);
		virtual ~MapLayerTiled();

		void CreateTiles(const char *text);

		virtual void Write(FILE *fp, Exporter *e);
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
