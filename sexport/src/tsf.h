#ifndef __TSF_H__
#define __TSF_H__

struct TSFHeader
{
	u32 fileId;				// 4
	u32 width;				// 4
	u32 height;				// 4
	u32 frames;				// 4
	u32 globalFrameTime;	// 4 -- 0 = use frame own time
	u32 flags;				// 4
	u32 colorKey;			// 2
	u16 platform:8;			// 00 -> DS
							// 01 -> Wii
	u16 padding:8;
};

enum Flags
{
	NO_FLAGS = 0x00,
	TSF_LOOP = 0x01,
	TSF_ANIMATED = 0x02
};

#endif
