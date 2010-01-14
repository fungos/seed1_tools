/***************************************************************************
 *   Copyright (C) 2006 by Sergey Belyashov                                *
 *   b-s-a at narod dot ru                                                *
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
#ifndef _IMAGEGLUE_H
#define _IMAGEGLUE_H

#define VERSION	"Techfront"

#include <string>
#include <rectpack.h>
#include <vector>
#include <list>
#include <IL/il.h>
#include <cstdio>

typedef std::list<IImage*> IImageList;

//-----------------------------------------------------------------------------
void mirror(void);
unsigned convertSize(unsigned n);
std::string get_base_name( const std::string & path );
std::string get_extention( const std::string & filename );
std::string remove_extention( const std::string & file_name );
std::vector<std::string> listDirectory( const std::string & path );
void putImagesToForm(ILuint form, const IArrayRects  & rects );
void putRectsToResultFile(FILE * res_file, int width, int height, const IArrayRects  & rects, const std::string & holder);//, const std::string & base_dir_name );
ILuint genForm( int width, int height, bool alpha );
void saveForm( ILuint img, std::string filename );
IArrayRects getRects( const IImageList & images );
IImageList loadImages( const std::vector<std::string> & files_list );
void deleteImages( const IImageList & images );
ILuint makeTexture( int width, int max_height, IArrayRects & src_rects, IArrayRects & ready );
std::string num2string(int n, const char * format = "%d");
void help(void);
//-----------------------------------------------------------------------------
#endif //_IMAGEGLUE_H
