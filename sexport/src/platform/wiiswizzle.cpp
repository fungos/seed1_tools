#include <string.h>

#include "wiiswizzle.h"

static u8 __tmp_swizzle_cache[8192];

int SwizzleTexture(PIXEL *buff, u32 width, u32 height)
{
	u8 *texels = (u8 *)buff;
	u8 *cache = (u8 *)&__tmp_swizzle_cache;
	u32 blockWd, blockX, blockHt, blockY, rowStride, row, col, lineStride, blockStride, shiftW, shiftH;
	u8 *dest, *src;

	shiftW = 2;                            // 4 pixels per block
	shiftH = 2;                            // 4 pixels per block
	blockStride = 16;                    // block stride in bytes
	lineStride = width * 4;                // line stride in bytes

	blockWd = width >> shiftW;            // blocks in width
	blockHt = height >> shiftH;            // blocks in height
	rowStride = lineStride << shiftH;    // size of a cached line
	dest = texels;

	for (blockY = 0; blockY < blockHt; blockY++)
	{
		memcpy(cache, dest, rowStride);
		for (blockX = 0; blockX < blockWd; blockX++)
		{
			src = &cache[blockX * blockStride];
			for (row = 0; row < (u32)(1 << shiftH); row++)
			{
				for (col = 0; col < blockStride; col += 4)
				{
					// fill in two blocks at once here AR -> B1, GB -> B2
					dest[0] = src[col];                 // alpha
					dest[1] = src[col + 1];             // red
					dest[32] = src[col + 2];            // green
					dest[32 + 1] = src[col + 3];        // blue
					dest += 2;
				}
				src += lineStride;
			}
			dest += 32;                        // skip over next complete block
		}
	}

	return TRUE;
}
