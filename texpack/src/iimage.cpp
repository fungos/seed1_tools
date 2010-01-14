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
#include "iimage.hpp"
#include "ierror.hpp"
#include <cstdio>

//-----------------------------------------------------------------------------
IImage::IImage( const std::string & file_name )
	: filename( file_name ) {

	//try to load image
	ilGenImages( 1, & img );
	ilBindImage( img );
	if ( ilLoadImage( const_cast<char *>( filename.c_str() ) ) != IL_TRUE )
		throw IError("Unable to load image");

	//determine image size
	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	ILuint format = ilGetInteger(IL_IMAGE_FORMAT);
	alpha = false;
	if ( (format == IL_RGBA) || (format == IL_BGRA) ) {	//have alpha channel?
		const unsigned char * data = (const unsigned char *)ilGetData() + 3;	//only alpha channel is interesting
		const long size = w * h;
		for(long i = 0;(i < size) && (!alpha); ++i, data += 4)
			alpha = (*data != 255);//partially transparent pixel found alpha channel used
	}
}
//-----------------------------------------------------------------------------
IImage::~IImage() {
	ilDeleteImages( 1, &img );
}
//-----------------------------------------------------------------------------


