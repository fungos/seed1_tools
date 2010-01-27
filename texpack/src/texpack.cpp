/***************************************************************************
 *   Copyright (C) 2006 by Sergey Belyashov                                *
 *   b-s-a at narod dot ru                                                 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <getopt.h>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>
#include <errno.h>

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <list>
#include <fstream>
#include <string>

#include <IL/il.h>
#include <IL/ilu.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "iimage.hpp"
#include "texpack.h"
#include "rectpack.h"
#include "ierror.hpp"

//-----------------------------------------------------------------------------
static const struct option prg_options[] = {
	{ "source-dir", required_argument, 0, 's' },
	{ "output", required_argument, 0, 'o' },
	{ "width", required_argument, 0, 'w' },
	//{ "height", required_argument, 0, 'h' },
	{ "max-width", required_argument, 0, 'W' },
	{ "max-height", required_argument, 0, 'H' },
	{ "help", 0, 0, 'u' },
	{ "version", 0, 0, 'v' },
	{ "list", required_argument, 0, 'l' },
	{ "relative", 0, 0, 'r' },
	{ "texnameCutAt", required_argument, 0, 'd'},
	{ "holdernameCutAt", required_argument, 0, 'y'},
	{ "file", required_argument, 0, 'f' },
	{ "format", required_argument, 0, 'F' },
	{ "border", required_argument, 0, 'b' },
	{ "native-size", 0, 0, 'n' },
	{ "separate-by-alpha", 0, 0, 'a' },
	{ "use-rotations", 0, 0, 'R' },
	{ "quiet", 0, 0, 'q' },
	{ 0,0,0,0 }
};
static const char * formatA = "%1$s\t%2$s\t%3$u\t%4$u\t%5$u\t%6$u\t%11$s\t%7$f%8$f%9$f%10$f";	//absolute values format
static const char * formatR = "%1$s\t%2$s\t%7$f\t%8$f\t%9$f\t%10$f\t%11$s\t%3$u%4$u%5$u%6$u";	//relative values format (texture coordinates)
const char * format = formatA;
static int max_width = 2048;
static int max_height = 2048;
static int border = 0;
static bool convert_size = true;
static bool alpha_separate = false;
static bool use_rotations = false;
static bool quiet = false;
static bool plain_width_height = false; // DANNY
static int texnameSizeCut = 0;
static int holdernameSizeCut = 0;


std::string version(void);
//-----------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
	std::string src_dir_name;
	std::string dst_file_name;
	std::string result_filename;
	//std::string base_dir_name;
	int bwidth = 0;
	std::string fileslist;
	FILE * res_out = stdout;
	while( 1 ) {
		int c = getopt_long( argc, argv, "s:o:w:W:H:h:auvl:rd:y:f:F:b:nRq", prg_options, 0);

		if ( c == -1 )
			break;

		switch( c ) {
			case '?':
				break;
			case 'F':
				format = optarg;
				break;
			case 'H':
				max_height = atoi(optarg);
				break;
			case 'R':
				use_rotations = true;
				break;
			case 'W':
				max_width = atoi(optarg);
				break;
			case 'a':
				alpha_separate = true;
				break;
			case 'b':
				border = atoi(optarg);
				if ( border < 0 ) {
					cerr << "Border must be a non negative value" << endl;
					exit(1);
				}
				break;
			case 'f':
				result_filename = optarg;
				break;
			/*case 'h':
				break;*/
			case 'l':
				fileslist = optarg;
				break;
			case 'n':
				convert_size = false;
				break;
			case 'o':
				dst_file_name = optarg;
				break;
			case 'q':
				quiet = true;
				break;
			case 'r':
				format = formatR;
				break;
			case 'd': // DANNY
				plain_width_height = true;
				//base_dir_name = optarg;
				texnameSizeCut = atoi(optarg);
				break;
			case 'y':
				plain_width_height = true;
				holdernameSizeCut = atoi(optarg);
				break;
			case 's':
				src_dir_name = optarg;
				break;
			case 'u':
				help();
				break;
			case 'v':
				cout << version() << endl;
				exit( EXIT_SUCCESS );
			case 'w':
				bwidth = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Invalid option. Try --help.\n");
				exit(1);
		}
	}
	if ( ( ( src_dir_name == "" ) && ( fileslist == "" ) ) || ( dst_file_name == "" ) ) {
		cout<<"You must define both source directory (or files list) and destination file"<<endl;
		exit( 2 );
	}

	if ( ! max_width ) {
		max_width = bwidth;
		if ( ! max_width )
			max_width = 2048;
	}

	std::vector<std::string> files_list;
	if ( src_dir_name != "" )
		files_list = listDirectory( src_dir_name );
	else {
		ifstream file;
		file.open( fileslist.c_str(), ios::in );
		while( ! file.eof() ) {
			std::string s;
			getline(file, s);
			if ( s != "" )
				files_list.push_back( s );
		}
		file.close();
	}

	ilInit();
	iluInit();
	ilEnable( IL_ORIGIN_SET );
	ilOriginFunc( IL_ORIGIN_UPPER_LEFT );

//	ilEnable( IL_FORMAT_SET );
//	ilSetInteger( IL_FORMAT_MODE, IL_RGBA );
	ilEnable( IL_TYPE_SET );
	ilSetInteger( IL_TYPE_MODE, IL_UNSIGNED_BYTE );

	//Loading images
	IImageList images( loadImages( files_list ) );
	IImageList images_alpha;

	if (alpha_separate) {
		for(IImageList::iterator i = images.begin(); i != images.end(); ++i)
			if ((*i)->alpha) {
				images_alpha.push_back(*i);
				IImageList::iterator x = i;
				--x;
				images.erase(i);			//XXX is it work?
				i = x;
			}
	} else
		for(IImageList::iterator i = images.begin(); i != images.end(); ++i)
			(*i)->alpha = true;

	IArrayRects src_rects( getRects( images ) );

	if ( result_filename != "" ) {
		res_out = fopen( const_cast<char*>( result_filename.c_str() ), "w" );
		if ( ! res_out ) {
			fprintf(stderr, "Unable to create result file (\"%s\")", strerror( errno ) );
			exit(2);
		}
	}
	int counter = 0;
	int width;
	IArrayRects current_rects(src_rects);
	bool alpha_used = !alpha_separate;
	if ( src_rects.empty() ) {
		alpha_used = true;
		current_rects = getRects(images_alpha);
	}
	do {
		int n = get_max_rect_width( current_rects );
		if (use_rotations) {
			int h = get_max_rect_height( current_rects );
			if (h > n)
				n = h;
		}
		unsigned long square = get_rects_square( current_rects );

		if ( ! bwidth ) {
			width = (int)( sqrt( square ) + 0.5f );
			if ( width < n )
				width = n;
		} else
			width = bwidth;
		if ( width > max_width )
			width = max_width;

		if (convert_size)
			width = convertSize( width );

		IArrayRects result;
		ILuint img = makeTexture( width, max_height, current_rects, result );
		square = get_rects_square( result );

		if ( img ) {
			ilBindImage( img );

			int width = ilGetInteger( IL_IMAGE_WIDTH );
			int height = ilGetInteger( IL_IMAGE_HEIGHT );

			std::string texname;
			if (!plain_width_height)
			{
				texname = get_base_name(dst_file_name) + num2string(counter) + std::string(".png");
			}
			else
			{
				texname = dst_file_name + num2string(counter);
			}

			if ( !quiet )
				fprintf(stderr, "%s: %dx%d %.2F%%\n",
					   texname.c_str(),
					   width,
					   height,
			(double)square * 100/(width * height) );

			putRectsToResultFile( res_out, width, height, result, texname);//, base_dir_name );

			saveForm( img, dst_file_name + num2string(counter) + ".png" );

			counter++;
		} else
			throw IError("Unable to create texture");

		if ( current_rects.empty() && (!alpha_used) ) {
			current_rects = getRects(images_alpha);
			alpha_used = true;
		}

	} while( ! current_rects.empty() );

	if ( res_out != stdout )
		fclose(res_out);
	deleteImages( images );
	deleteImages( images_alpha );
	ilShutDown();
	return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
unsigned convertSize(unsigned n) {
	unsigned i = 1;
	while( i < n )
		i <<= 1;
	return i;
}
//-----------------------------------------------------------------------------
std::string get_extention( const std::string & filename ) {
	for( std::string::const_iterator i = --filename.end(); i != filename.begin(); i-- )
		if ( *i == '.' )
			return filename.substr( ++i - filename.begin(), filename.end() - i - 1);
	return std::string();
}
//-----------------------------------------------------------------------------
std::string get_base_name( const std::string & path ) {
	for( std::string::const_iterator i = --path.end(); i != path.begin(); i-- )
		if ( *i == '/' || *i == '\\')
			return path.substr( ++i - path.begin(), path.end() - i - 1);
	return path.substr( 0, path.length() );
}
//-----------------------------------------------------------------------------
std::string remove_extension( const std::string & file_name ) {
	for( std::string::const_iterator i = --file_name.end(); i != file_name.begin(); i-- )
		if ( *i == '.' )
			return file_name.substr( 0, i - file_name.begin());
	return file_name.substr( 0, file_name.length() );
}

std::string separator_cleanup(const std::string &str) {
	std::string ret("");
	for(std::string::const_iterator i = str.begin(); i != str.end(); ++i )
		if ( *i == '\\' )
			ret += '/';
		else
			ret += *i;

	return ret;
}
//-----------------------------------------------------------------------------
#ifndef WIN32
std::vector<std::string> listDirectory( const std::string & path ) {
	struct dirent **list;
	int n;
	std::vector<std::string> files_list;
	struct stat st;
	std::string src_dir_name = ( path[ path.length() - 1 ] != '/' ) ? path + "/" : path;

	n = scandir( src_dir_name.c_str(), &list, 0, alphasort );
	if ( n < 1 ) {
		cout<<"No such directory ("<<src_dir_name<<")"<<endl;
		exit(2);
	}

	for( --n; n >= 0; n-- ) {
		files_list.push_back( src_dir_name + list[n]->d_name );
		free( list[n] );
		stat( files_list.back().c_str(), &st );
		if ( S_ISDIR(st.st_mode) )
			files_list.pop_back();
	}
	free( list );
	return files_list;
}
#else
std::vector<std::string> listDirectory( const std::string & path )
{
	return std::vector<std::string>();
}
#endif
//-----------------------------------------------------------------------------
void putRectsToResultFile(FILE * res_file, int width, int height, const IArrayRects  & rects, const std::string & holder) { //, const std::string &base_dir_name ) {
	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ ) {
		int x_border = ( i->w < max_width ) ? border : 0;
		int y_border = ( i->h < max_height ) ? border : 0;

		const IRect rect( i->x + x_border, i->y + y_border,
			i->w - 2 * x_border, i->h - 2 * y_border, i->id );

		IImage * const &image = (IImage*)rect.id;
		std::string texname = remove_extension(get_base_name(image->filename));
		std::string holdername = remove_extension(get_base_name(holder));

		unsigned left = rect.x;
		unsigned top = rect.y;
		unsigned right = rect.w;
		unsigned bottom = rect.h;

		// DANNY
		if (!plain_width_height)
		{
			right += rect.x;
			bottom += rect.y;
		}
		else
		{
			//texname = separator_cleanup(remove_extension(image->filename).substr(base_dir_name.size()));
			//holdername = separator_cleanup(holder.substr(18)); // dracula - base_dir_name.size()));
			texname = separator_cleanup(remove_extension(image->filename).substr(texnameSizeCut));
			holdername = separator_cleanup(holder.substr(holdernameSizeCut)); // dracula - base_dir_name.size()));
		}

		fprintf( res_file, format,
				 holdername.c_str(),	//name of holder
				 texname.c_str(),	//name of texture
				 left,
				 top,
				 right,
				 bottom);/*,
				 (double)left/width,
				 (double)top/height,
				 (double)right/width,
				 (double)bottom/height,
				 i->rotated ? "rotated" : "",
				 (unsigned)rect.w,
				 (unsigned)rect.h,
				 (double)rect.w/width,
				 (double)rect.h/height);*/
		fputc('\n', res_file);
	}
}
//-----------------------------------------------------------------------------
void rotate_image(void)
{
	const ILuint w = ilGetInteger(IL_IMAGE_WIDTH);
	const ILuint h = ilGetInteger(IL_IMAGE_HEIGHT);
	const ILuint szPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	const ILuint format = ilGetInteger( IL_IMAGE_FORMAT );
	const ILuint type = ilGetInteger( IL_IMAGE_TYPE );
	const ILuint szRow = szPixel*w;
	const ILuint szCol = szPixel*h;

	const char * src_data = (char*)ilGetData();
	char * dst_data = (char*)calloc(w*h, szPixel);
	ILuint x, y, yb, xb;
	const char *s;
	char *d;
	for(y = 0, yb = 0, xb = 0; y < h; ++y, yb += szRow, xb += szPixel ) {
		s = src_data + yb;
		d = dst_data + xb;
		for(x = 0; x < w; ++x) {
			memcpy(d, s, szPixel);
			d += szCol;
			s += szPixel;
		}
	}
	ilTexImage(h, w, 1, ( (format == IL_RGBA) || (format == IL_BGRA) ) ? 4 : 3, format, type, dst_data);
	free(dst_data);
	iluFlipImage();
}
//-----------------------------------------------------------------------------
void putImagesToForm(ILuint form, const IArrayRects & rects ) {
	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ ) {
		const IRect &rect = *i;
		IImage * image = (IImage*)rect.id;
		ilBindImage( image->img );
		if (rect.rotated)
			rotate_image();
		void * buf = ilGetData();
		ILuint format = ilGetInteger(IL_IMAGE_FORMAT);
		ILuint type = ilGetInteger(IL_IMAGE_TYPE);
		ilBindImage( form );
		int x_border = border;
		int y_border = border;
		if ( i->w + 2*border >= max_width )
			x_border = 0;
		if ( i->h + 2*border >= max_height )
			y_border = 0;

		ilSetPixels( rect.x + x_border, rect.y + y_border, 0,
			rect.w - 2 * x_border, rect.h - 2 * y_border, 1, format, type, buf );
	}
}
//-----------------------------------------------------------------------------
ILuint genForm( int width, int height, bool alpha ) {
	ILuint result;
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilGenImages( 1, &result );
	ilBindImage( result );
	ilTexImage( width, height, (alpha ? 32 : 24), (alpha ? 4 : 3), (alpha ? IL_RGBA : IL_RGB), IL_UNSIGNED_BYTE, 0 );
	memset( ilGetData(), 0, width * height * ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) );
	return result;
}
//-----------------------------------------------------------------------------
void saveForm( ILuint img, std::string filename ) {
	ilBindImage( img );
	iluFlipImage();
	ilEnable( IL_FILE_OVERWRITE );
	//ilSaveImage(const_cast<char*>( filename.c_str() ) ); // DANNY
	ilSave(IL_PNG, const_cast<char*>( filename.c_str() ));
	ilDeleteImages( 1, &img );
}
//-----------------------------------------------------------------------------
IImageList loadImages( const std::vector<std::string> & files_list ) {
	IImageList images;

	for( std::vector<std::string>::const_iterator i = files_list.begin(); i != files_list.end(); ++i ) {
		try {
			images.push_back(new IImage(*i));
		} catch(...) {
			std::cerr << "WARNING: Unable to open file \"" << *i << "\"" << std::endl;
		}
	}

	return images;
}
//-----------------------------------------------------------------------------
IArrayRects getRects( const IImageList & images ) {
	IArrayRects rects;

	for( IImageList::const_iterator i = images.begin(); i != images.end(); ++i ) {
		const IImageList::value_type &img = *i;

		int w = (int)img->w + 2*border <= max_width ? img->w + 2*border : img->w/*max_width*/;
		int h = (int)img->h + 2*border <= max_height ? img->h + 2*border : img->h/*max_height*/;

		rects.push_back( IRect( 0, 0, w, h, img ) );
	}

	return rects;
}
//-----------------------------------------------------------------------------
void deleteImages( const IImageList & images ) {
	for( IImageList::const_iterator i = images.begin(); i != images.end(); ++i )
		delete *i;
}
//-----------------------------------------------------------------------------
ILuint makeTexture( int width, int max_height, IArrayRects & src_rects, IArrayRects & ready ) {
	ILuint result = 0;
	int min_h;
	if (use_rotations) {
		min_h = get_max_rect_height( src_rects );
		int t = get_max_rect_width( src_rects );
		if (min_h > t)
			min_h = t;
	} else
		min_h = get_max_rect_height( src_rects );
	IRect dst( 0, 0, width, width); //min_h ); // Define o tamanho final da imagem

	ready = packRectangles( dst, src_rects, max_height, use_rotations );

	int height = convert_size ? convertSize( dst.h ) : dst.h;
	if ( height <= max_height ) {
		result = genForm( width, height, ((IImage*)ready.front().id)->alpha );
		putImagesToForm( result, ready );
	}
	return result;
}
//-----------------------------------------------------------------------------
std::string num2string(int n, const char * format) {
	char *t = (char*)malloc(100);
	snprintf(t, 99, format, n );
	std::string s( t );
	free(t);
	return s;
}
//-----------------------------------------------------------------------------
void help(void) {
	cout<<"Usage:\n";
	cout<<"\ttexpack --list=<file name> --output=texture [options]\n";
	cout<<"\ttexpack --source-dir=<dir name> --output=texture [options]\n";
	cout<<"Parameters:\n";
	cout<<" -l\t--list=FILE NAME      \tfile with list of source images (each line - full file name to image)\n";
	cout<<" -o\t--output=TEMPLATE     \tatlases name template\n";
	cout<<" -s\t--source-dir=DIR NAME \tdirectory with source images\n" << endl;
	cout<<"Options:\n";
	cout<<" -r\t--relative            \tresult info in float format (x/atlas_width, y/atlas_height)\n";
	cout<<" -d\t--danny				  \tDanny mode on!\n";
	cout<<" -f\t--file=FILE NAME      \twrite result to text file instead stdout\n";
	cout<<" -w\t--width=WIDTH         \twidth of atlases (default: not set)\n";
	//cout<<" -h\t--height=HEIGHT       \theight of atlases (default: not set)\n";
	cout<<" -W\t--max-width=WIDTH     \tmaximal width of atlases (default " << max_width << ")\n";
	cout<<" -H\t--max-height=HEIGHT   \tmaximal height of atlases (default " << max_height << ")\n";
	cout<<" -F\t--format=FORMAT       \tuser defined format of output\n";
	cout<<" -b\t--border=NUMBER	      \tborder size around each source images\n";
	cout<<" -n\t--native-size         \tdo not use discrete sizes (2, 4, 8,.. 256, 512...)\n";
	cout<<" -a\t--separate-by-alpha   \tseparate textures by alpha channel (image with data in alpha channel are packing separately)\n";
	cout<<" -R\t--use-rotations       \tuse rotations of source images when packing\n";
	cout<<" -v\t--version             \tversion info\n";
	cout<<"   \t--help                \tthis help\n" << endl;
	//cout<<"All source images must be 32 bits per pixel."<<endl;
	cout<<"All atlases are in PNG RGB (24 bpp) or RGBA (32 bpp) format.\n" << endl;
	cout<<"FORMAT must contain any characters and ALL special printf-like macros:\n";
	cout<<" %1$s - atlas name\n";
	cout<<" %2$s - source image name\n";
	cout<<" %3$u - source image left absolute coordinate in atlas\n";
	cout<<" %4$u - source image top absolute coordinate in atlas\n";
	cout<<" %5$u - source image right absolute coordinate in atlas\n";
	cout<<" %6$u - source image bottom absolute coordinate in atlas\n";
	cout<<" %7$f - source image left relative coordinate in atlas\n";
	cout<<" %8$f - source image top relative coordinate in atlas\n";
	cout<<" %9$f - source image right relative coordinate in atlas\n";
	cout<<"%10$f - source image bottom relative coordinate in atlas\n";
	cout<<"%11$s - image rotation indicator (\"rotated\" or \"\")\n";
	cout<<"%12$u - source image absolute width\n";
	cout<<"%13$u - source image absolute height\n";
	cout<<"%14$f - source image relative width\n";
	cout<<"%15$f - source image relative height\n";
	cout<<"Note: right - left = width and bottom - top = height\n";
	cout<<"By default using next FORMAT (delimeters are TABs):\n";
	cout<<formatA << "\n";
	cout<<"With option --relative using next format:\n";
	cout<<formatR << endl;
	cout<<"Examples:\n";
	cout<<" texpack -s /home/user/pictures -o textures -H 1024 -w 1024\n";
	cout<<" texpack -l /home/user/filelist.txt -o textures\n\n";
	cout<<"Both examples produce few big images (textures0.png, textures1.png...)\n";
	cout<<"in current directory.\n";
	cout<<"First example limits size of images to 1024x1024.\n";
	cout<<"Second example limits images to 2048x2048.\n";
	cout<<"Size of last produced image may be less than limits.\n" << endl;
	exit( EXIT_SUCCESS );
}
//-----------------------------------------------------------------------------
std::string version(void)
{
	return std::string(VERSION);
}
//-----------------------------------------------------------------------------
