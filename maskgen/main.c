#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_image.h"
#include "defines.h"

#define MAX_FILENAME 256


#define MSK_MAGIC		0x004B534D
#define MSK_VERSION		0x00000001

u32 gPlatform = PLATFORM_CODE_SDL;


struct MaskObjectHeader
{
	struct ObjectHeader foh;
	u16 w;
	u16 h;
	//u8 *bitmask;
};


inline u32 swap32(u32 v)
{
	u32 r = v;
	switch (gPlatform)
	{
		case PLATFORM_CODE_WII:
			r = _Swap32(v);
		break;

		default:
		break;
	}

	return r;
}


int main(int argc, char *argv[])
{
	Uint32 initflags = SDL_INIT_TIMER;

	if (argc != 5)
	{
		fprintf(stderr, "%s <filename.TGA!> <output.MASK> -p <platform>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!strcasecmp(argv[4], "iph"))
		gPlatform = PLATFORM_CODE_IPH;
	else if (!strcasecmp(argv[4], "wii"))
		gPlatform = PLATFORM_CODE_WII;
	else if (!strcasecmp(argv[4], "sdl"))
		gPlatform = PLATFORM_CODE_SDL;
	else
	{
		fprintf(stderr, "Invalid platform name: %s\n", argv[4]);
		return EXIT_FAILURE;
	}

	if (SDL_Init(initflags) < 0)
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Surface *img = IMG_Load(argv[1]);
	if (!img)
	{
		fprintf(stderr, "Image %s not found or invalid format.\n", argv[1]);
		return EXIT_FAILURE;
	}

	if (img->format->BitsPerPixel != 32)
	{
		fprintf(stderr, "Image %s must be 32bits (/lazy programing mode on).\n", argv[1]);
		return EXIT_FAILURE;
	}

/*
	const char *tganame = argv[1];
	char tmp[MAX_FILENAME - 5];
	memset(tmp, '\0', MAX_FILENAME - 5);
	int extPos = (int)(strchr(tganame, '.') - tganame);
	int total = extPos < MAX_FILENAME - 5 ? extPos : MAX_FILENAME - 5;

	strncpy(tmp, tganame, total);
	strncat(tmp, ".mask", MAX_FILENAME);
*/
	FILE *fp = fopen(argv[2], "wb+");


	struct MaskObjectHeader hdr;
	hdr.foh.magic = swap32(MSK_MAGIC);
	hdr.foh.version = swap32(MSK_VERSION);
	hdr.foh.platform = swap32(gPlatform);
	hdr.w = (img->w / 8) + ((img->w % 8) != 0 ? 1 : 0);
	hdr.h = img->h;

	fwrite(&hdr, sizeof(struct MaskObjectHeader), 1, fp);

	u8 byte = 0;
	int y = 0;
	for (; y < img->h; y++)
	{
		int x = 0;
		for (; x < img->w; x++)
		{
			Uint32 *buffer = (Uint32 *)img->pixels;
			Uint32 px = buffer[y * img->w + x];

			u32 bitpos = (7 - (x % 8));
			if (px == 0xffff00ff || px == 0x00ff00ff)
			{
				byte |= 1 << bitpos;
			}

			if (bitpos == 0 || x == img->w-1)
			{
				fputc(byte, fp);
				byte = 0;
			}
		}
	}

	fclose(fp);
	SDL_FreeSurface(img);
	SDL_Quit();

	return EXIT_SUCCESS;
}
