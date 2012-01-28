#ifndef __MAP2D_H__
#define __MAP2D_H__

#include "../defines.h"
#include "../iobject.h"

class IMapLayer;
class MapObject;
class MapSpriteEntity;

typedef u32 Tile;

VECTOR_POD_DEF(Tile);
VECTOR_PTR_DEF(IMapLayer);
VECTOR_PTR_DEF(MapObject);
VECTOR_PTR_DEF(MapSpriteEntity);

#define MAP_MAGIC		0x0050414d
#define MAP_VERSION		0x00000001

#define LAY_MAGIC		0x0059414c
#define LAY_VERSION		0x00000001

enum eMapType
{
	MapType2D,
	MapTypeMax
};

enum eLayerType
{
	LayerTypeTiled,
	LayerTypeObject,
	LayerTypeMosaic,
	LayerTypeMax
};

enum eBlendType
{
	BlendNone,
	BlendMultiply
};

struct MapHeader
{
	ObjectHeader block;
	u32 iType;
	u32 iMapWidth;
	u32 iMapHeight;
	u32 iTileWidth;
	u32 iTileHeight;
	u32 iLayerCount;
};

struct LayerHeader
{
	u32 iType;
	u32 iNameId;
	u32 iVisible;
	f32 fOpacity;
	u32 iDataIndex;
	// layerdata
		// series of tilesId for tiled layer;
		// u32 for amount of object, then n objects for metadata layer;
		// u32 for amount of sprites, then n objects for sprite entities layer;
};

struct LayerObjectHeader
{
	u32 iNameId;
	u32 iTypeId;
	u32 iPropertiesId;
	f32 fPosX;
	f32 fPosY;
	f32 fWidth;
	f32 fHeight;
};

struct LayerSpriteEntityHeader
{
	u32 iNameId;
	f32 fPosX;
	f32 fPosY;
	f32 fAngle;
	f32 fScale;
	u32 iBlendType;
	u32 iColor;
	u32 iCollidable;
	u32	iSpriteFileId;
};

class Exporter;

class MapSpriteEntity : public IObject
{
	private:
		MapSpriteEntity(const MapSpriteEntity &);
		bool operator=(const MapSpriteEntity &);

		f32 fX;
		f32 fY;
		f32 fAngle;
		f32 fScale;
		eBlendType nBlendType;
		PIXEL pxColor;
		BOOL bCollidable;
		const char *pSprite;

	public:
		MapSpriteEntity(const char *name);
		virtual ~MapSpriteEntity();

		inline void SetSprite(const char *f)
		{
			this->pSprite = f;
		}

		void SetAngle(f32 angle);
		void SetPosition(f32 x, f32 y);
		void SetScale(f32 scale);
		void SetBlendType(eBlendType type);
		void SetPixelColor(u8 r, u8 g, u8 b, u8 a);
		void SetCollidable(BOOL b);

		virtual void Write(FILE *fp, Exporter *e);
};

class MapObject : public IObject
{
	private:
		MapObject(const MapObject &);
		bool operator=(const MapObject &);

		char *pType;
		char *pProps;
		f32 fX;
		f32 fY;
		f32 fW;
		f32 fH;

	public:
		MapObject(const char *name);
		virtual ~MapObject();

		void SetType(const char *type);
		void SetProperties(const char *props);
		void SetPosition(f32 x, f32 y);
		void SetSize(f32 w, f32 h);

		virtual void Write(FILE *fp, Exporter *e);
};

class IMapLayer : public IObject
{
	private:
		IMapLayer(const IMapLayer &);
		bool operator=(const IMapLayer &);

	protected:
		eLayerType nType;
		bool bVisibility;
		f32 fOpacity;
		u32 iDataIndex;

	public:
		IMapLayer(const char *name);
		virtual ~IMapLayer();

		virtual void Write(FILE *fp, Exporter *e);
		virtual void WriteData(FILE *fp, Exporter *e) = 0;

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

		void SetDataIndex(u32 idx)
		{
			iDataIndex = idx;
		}

		u32 GetDataIndex() const
		{
			return iDataIndex;
		}
};

class MapLayerObject : public IMapLayer
{
	private:
		MapLayerObject(const MapLayerObject &);
		bool operator=(const MapLayerObject &);

		MapObjectVector vObjects;

	public:
		MapLayerObject(const char *name);
		virtual ~MapLayerObject();

		void Add(MapObject *obj);

		virtual void WriteData(FILE *fp, Exporter *e);
};

class MapLayerTiled : public IMapLayer
{
	private:
		MapLayerTiled(const MapLayerTiled &);
		bool operator=(const MapLayerTiled &);

		TileVector vTiles;

	public:
		MapLayerTiled(const char *name);
		virtual ~MapLayerTiled();

		void CreateTiles(const char *text);

		virtual void WriteData(FILE *fp, Exporter *e);
};

class MapLayerMosaic : public IMapLayer
{
	private:
		MapLayerMosaic(const MapLayerMosaic &);
		bool operator=(const MapLayerMosaic &);

		MapSpriteEntityVector vSprites;

	public:
		MapLayerMosaic(const char *name);
		virtual ~MapLayerMosaic();

		void Add(MapSpriteEntity *obj);

		virtual void WriteData(FILE *fp, Exporter *e);
};

class Map2D : public IObject
{
	private:
		Map2D(const Map2D &);
		bool operator=(const Map2D &);

		IMapLayerVector vLayers;
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
