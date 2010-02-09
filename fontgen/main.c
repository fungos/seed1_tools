#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_image.h"


#ifdef WIN32
#define TMP_FILE	"tmp/font.xml"
#else
#define TMP_FILE	"tmp/font.xml"
#endif

#define DEFAULT_FONT_NUM_CHARS 		90
//#define FONT_COLUMNS 				16

#define ERROR_RTCODE_INVALID_ARGS	1
#define ERROR_RTCODE_SDLFAILURE		2
#define ERROR_RTCODE_SDLIMAGE		3
#define ERROR_RTCODE_TMPFILE		4


typedef struct _rect
{
	int x;
	int y;
	int w;
	int h;
} rect;


int main(int argc, char *argv[])
{
	Uint32 initflags = SDL_INIT_TIMER;

	if (argc < 2)
	{
		fprintf(stderr, "%s <filename.TGA!> <sprite object name> [<glyphs amount>=90] <glyph_width> <glyph_height> [<atlas resource name> <atlas x> <atlas y>]\n", argv[0]);
		exit(ERROR_RTCODE_INVALID_ARGS);
	}
	/*else if (argc > 4)
	{
		fprintf(stderr, "Wrong parameter count/sequence. If using atlas, you MUST pass glyphs amount, atlas name, x and y in the correct order.\n");
	}*/

	if ( SDL_Init(initflags) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(ERROR_RTCODE_SDLFAILURE);
	}

	SDL_Surface *img = IMG_Load(argv[1]);
	if (!img)
	{
		fprintf(stderr, "Image %s not found or invalid format.\n", argv[1]);
		exit(ERROR_RTCODE_SDLIMAGE);
	}

	int c = 0;
	while (argv[1][c] != '.') c++;
	argv[1][c] = '\0';

	char *atlasName = argv[2];
	int atlasX = 0;
	int atlasY = 0;

	int glyph_width = atoi(argv[4]);
	int glyph_height = atoi(argv[5]);
	int font_columns = img->w / glyph_width;

	int fontNumChars = DEFAULT_FONT_NUM_CHARS;
	int amountOfLineChars = 6; // DEFAULT_FONT_NUM_CHARS
	if (argc > 2)
	{
		fontNumChars = atoi(argv[3]);
		//amountOfLineChars = (fontNumChars / FONT_COLUMNS) + ((fontNumChars % FONT_COLUMNS) ? 1 : 0);
		amountOfLineChars = img->h / glyph_height;
	}

	if (argc == 9)
	{
		atlasName = argv[6];
		atlasX = atoi(argv[7]);
		atlasY = atoi(argv[8]);
	}

	fprintf(stdout, "* Generating font xml usign resource %s (%s.tga, sprite %s) with %d glyphs\n", atlasName, argv[1], argv[2], fontNumChars);

	rect chars[fontNumChars];
	memset(chars, '\0', fontNumChars * sizeof(rect));

	//int glyphMaxHeight = img->h / amountOfLineChars;
	//int glyphMaxWidth = img->w / FONT_COLUMNS;

	int i = 0;
	for (i = 0; i < fontNumChars; i++)
	{
		//int row = i / FONT_COLUMNS;
		int row = i / font_columns;
		//int col = i % FONT_COLUMNS;
		int col = i % font_columns;

		//chars[i].h = glyphMaxHeight;
		chars[i].h = glyph_height;
		//chars[i].x = (col * glyphMaxWidth);// / (float)width;
		chars[i].x = (col * glyph_width);// / (float)width;
		//chars[i].y = (row * glyphMaxHeight);// / (float)height;
		chars[i].y = (row * glyph_height);// / (float)height;

		int found = 0;

		int j = 0;
		//for (j = glyphMaxWidth-1; j >= 0 && !found; j--)
		for (j = glyph_width-1; j >= 0 && !found; j--)
		{
			//Uint32 x = (col * glyphMaxWidth) + j;
			Uint32 x = (col * glyph_width) + j;

			int k = 0;
			//for (k = glyphMaxHeight-1; k >= 0; k--)
			for (k = glyph_height-1; k >= 0; k--)
			{
				//Uint32 y = (row * glyphMaxHeight) + k;
				Uint32 y = (row * glyph_height) + k;
				Uint32 *buffer = (Uint32 *)img->pixels;
				Uint32 px = buffer[y * img->w + x];

				if (px & 0xff000000 && (j+1 > chars[i].w))
				{
					chars[i].w = j+1;
					//fprintf(stdout, "Font Cell %i (%c) Width %d\n", i, i + 33, charW[i]);

					found = 1;
					//break;
				}
			}
		}
	}

	FILE *fp = fopen(TMP_FILE, "wt");
	if (!fp)
	{
		fprintf(stderr, "[FONTGEN] Unable to open temporary file: " TMP_FILE ".\n");
		exit(ERROR_RTCODE_TMPFILE);
	}

	const char *tganame = argv[1];
	int x = strlen(tganame) - 1;
	while (tganame[x] != '\\' && tganame[x] != '/') x--;
	tganame = &tganame[x+1];

	fprintf(fp, "<root>\n\t<strings></strings>\n\t<resources>\n\t\t<resource type='image' filename='%s' name='font' />\n\t</resources>\n\t<objects>\n\t\t<object type='sprite' name='%s' resource='font'>\n\t\t\t<animation name='font'>\n", atlasName, argv[2]);

	for (i = 0; i < fontNumChars; i++)
	{
		fprintf(fp, "\t\t\t\t<frame resource='font' x='%d' y='%d' width='%d' height='%d' />\n", chars[i].x + atlasX, chars[i].y + atlasY, chars[i].w, chars[i].h);
	}

	fprintf(fp, "\t\t\t</animation>\n\t\t</object>\n\t</objects>\n</root>\n");

	fclose(fp);

	SDL_FreeSurface(img);

	SDL_Quit();
	return EXIT_SUCCESS;
}
