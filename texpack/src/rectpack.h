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
#ifndef _RECTPACK_H
#define _RECTPACK_H

#include <list>
#include <cstring>

/**
 * \brief This class defining a rectangle with some properties
 */
class IRect {
	public:
		int x, y;	///< result coordinates
		int w, h;	///< size of rectangle
		void * id;	///< optional identifier of rectangle
		bool rotated;

		IRect(int X, int Y, int W, int H, void * ID = 0 ) : x(X), y(Y), w(W), h(H), id(ID), rotated(false){}
		IRect(int W, int H, void * ID = 0 ) : x(0), y(0), w(W), h(H), id(ID), rotated(false) {}

		inline bool operator==(const IRect &r) {
			return ! memcmp( this, &r, sizeof(r) );
		}
		inline bool operator!=(const IRect &r) {
			return (bool)memcmp( this, &r, sizeof(r) );
		}
		inline const IRect & rotate(void) {
			rotated = !rotated;
			int t = w;
			w = h;
			h = t;
			return *this;
		}
};

/**
 * \brief Array of rectangles
*/
typedef std::list<IRect> IArrayRects;
//-----------------------------------------------------------------------------
int get_min_rect_width( const IArrayRects & rects );
int get_max_rect_width( const IArrayRects & rects );
int get_min_rect_height( const IArrayRects & rects );
int get_max_rect_height( const IArrayRects & rects );
//-----------------------------------------------------------------------------
/**
 * Находит все прямоугольники с максимальной высотой
 * @param rects - набор прямоугольников
 * @return набор прямоугольников
 */
IArrayRects find_rects_with_max_height( const IArrayRects & rects );
//-----------------------------------------------------------------------------
/**
 * Находит все прямоугольники с максимальной шириной
 * @param rects - набор прямоугольников
 * @return набор прямоугольников
 */
IArrayRects find_rects_with_max_width( const IArrayRects & rects );
//-----------------------------------------------------------------------------
/**
 * Находит все прямоугольники с минимальной высотой
 * @param rects - набор прямоугольников
 * @return набор прямоугольников
 */
IArrayRects find_rects_with_min_height( const IArrayRects & rects );
//-----------------------------------------------------------------------------
/**
 * Находит все прямоугольники с минимальной шириной
 * @param rects - набор прямоугольников
 * @return набор прямоугольников
 */
IArrayRects find_rects_with_min_width( const IArrayRects & rects );
//-----------------------------------------------------------------------------
unsigned long get_rects_square( const IArrayRects & rects );
//-----------------------------------------------------------------------------
void sortRectsByHeight( IArrayRects & rects, bool rotate = false );
//-----------------------------------------------------------------------------
void removeRects(IArrayRects & from, const IArrayRects & what );
//-----------------------------------------------------------------------------
IArrayRects divideArray( IArrayRects & rects );
//-----------------------------------------------------------------------------
IArrayRects packRectangles( IRect & dest_rect, IArrayRects & source_rects, int max_height = 0, bool rotate = false );
//-----------------------------------------------------------------------------
#endif //_RECTPACK_H
