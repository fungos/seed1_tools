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
#include "rectpack.h"
#include <vector>

#include <iostream>
using namespace std;
//-----------------------------------------------------------------------------
/**
 * This class is needed for determine where is free space on form
 */
class IMatrix {
	typedef std::vector<char> IArrayChars;
	private:
		int get_max_width( IArrayChars::const_iterator & i, int count ) const;	///< recursion function to determine max number of free cells in matrix
		int get_min_width( IArrayChars::const_iterator & i, int count ) const;	///< recursion function to determine max number of free cells in matrix
	protected:

		IArrayChars *m_data;
		int m_width, m_height;
		int get_free_x( int y ) const;
		int get_free_y( int x, int start_y ) const;
		bool get(int x, int y) const;
	public:
		IMatrix( int w, int h, bool data = false );
		~IMatrix() { delete m_data; };
		inline bool operator()(int x, int y) const;
		void set_height( int height, bool data = false );
		inline int get_height(void) const;
		void fill(int x, int y, int w, int h, bool data = true );
		int get_free_top(void) const;	///< get lowest free y position
		IRect get_max_rect(void) const;	///< get free rect with maximal height
		void fill_small_rects( int max_width, bool data = true );
		int find_top_for_width( int min_width ) const;
};
//-----------------------------------------------------------------------------
bool IMatrix::operator()(int x, int y) const {
	return get(x,y);
}
//-----------------------------------------------------------------------------
int IMatrix::get_height(void) const {
	return m_height;
}
//-------------------------------------------------
IMatrix::IMatrix( int w, int h, bool data ) :
m_width( w ),
m_height( h ) {
	m_data = new IArrayChars( w * h, (IArrayChars::value_type)data );
}
//-------------------------------------------------
bool IMatrix::get(int x, int y) const {
	return (*m_data)[ y * m_width + x ];
}
//-------------------------------------------------
void IMatrix::set_height( int height, bool data ) {
	m_height = height;
	m_data->resize( height * m_width, (IArrayChars::value_type)data );
}
//-------------------------------------------------
void IMatrix::fill(int x, int y, int w, int h, bool data ) {
	IArrayChars::iterator a = m_data->begin() + y * m_width + x;
	IArrayChars::iterator b;
	for(int i = 0; i < h; i++, a += m_width ) {
		b = a;
		if ( a >= m_data->end() )
			throw 0;
		for( int j = 0; j < w; j++, b++ )
			*b = (IArrayChars::value_type)data;
	}
}
//-------------------------------------------------
int IMatrix::get_free_top(void) const {
	int x = m_width / 2;
	int py = 0, y = m_height - 1;

	if ( get(x, y) )	// in most cases this is false
		x = get_free_x( y );

	while( 1 ) {
		y = get_free_y( x, y ) - 1;
		if ( y < 0 )
			return 0;
		x = get_free_x( y );
		if ( x < 0 )
			return y + 1;
		//y--;
	}
	return py;
}
//-------------------------------------------------
int IMatrix::get_free_x( int y ) const {
	int x = 0;
	IArrayChars::const_iterator a = m_data->begin() + ( y * m_width );

	for( ; x < m_width; x++, a++ )
		if ( *a == (IArrayChars::value_type)false )
			break;

	return ( x >= m_width ) ? -1 : x;
}
//-------------------------------------------------
int IMatrix::get_free_y( int x, int start_y ) const {
	IArrayChars::const_iterator a = m_data->begin() + start_y * m_width + x;

	while( start_y >= 0 ) {
		if ( *a == (IArrayChars::value_type)true )
			break;
		start_y--;
		a -= m_width;
	}

	return start_y + 1;
}
//-------------------------------------------------
IRect IMatrix::get_max_rect(void) const {
	int y = get_free_top();
	if ( y >= m_height )
		return IRect(0,0,0,0);
	IArrayChars::const_iterator i, b = m_data->begin() + ( y * m_width );
	i = b;
	int w = get_max_width( i, m_width );
	int x = i - b;
	return IRect(x, y, w, m_height - y );
}
//-------------------------------------------------
int IMatrix::get_max_width( IArrayChars::const_iterator & i, int count ) const {
	int n;
	for( ; count > 0; count--, i++ )
		if ( *i == (IArrayChars::value_type)false )
			break;

	IArrayChars::const_iterator j = i;
	for( n = 0; count > 0; count--, j++, n++ )
		if ( *j == (IArrayChars::value_type)true )
			break;

	if ( count ) {
		count = get_max_width( j, count );
		if ( count > n ) {
			n = count;
			i = j;
		}
	}

	return n;
}
//-------------------------------------------------
int IMatrix::get_min_width( IArrayChars::const_iterator & i, int count ) const {
	int n;
	for( ; count > 0; count--, i++ )
		if ( *i == (IArrayChars::value_type)false )
			break;

	IArrayChars::const_iterator j = i;
	for( n = 0; count > 0; count--, j++, n++ )
		if ( *j == (IArrayChars::value_type)true )
			break;

	if ( count ) {
		count = get_min_width( j, count );
		if ( ( count < n ) && ( count > 0) ) {
			n = count;
			i = j;
		}
	}

	return n;
}
//-------------------------------------------------
void IMatrix::fill_small_rects( int max_width, bool data ) {
	while( 1 ) {
		int y = get_free_top();
		if ( y >= m_height )
			break;
		IArrayChars::const_iterator i, j = m_data->begin() + (y * m_width);
		i = j;
		int n = get_min_width( i, m_width );
		int x = i - j;
		if ( ( n > 0 ) && ( n < max_width ) ) {
			if ( x + n < m_width )
				j = i + n;
			if ( x > 0 )
				i--;
			int k;
			for(k = 0 ; y < m_height; k++, y++, j += m_width, i += m_width )
				if ( ( ( *i == (IArrayChars::value_type)false ) && ( x > 0 ) ) || ( ( x + n < m_width ) && ( *j == (IArrayChars::value_type)false ) ) )
					break;
			//if ( y <= m_height )
			fill(x, get_free_top(), n, k, data);
		} else
			break;
	}
}
//-----------------------------------------------------------------------------
int IMatrix::find_top_for_width( int min_width ) const {
	int y = get_free_top();
	IArrayChars::const_iterator p = m_data->begin() + y * m_width;
	for( ; y < m_height; y++, p += m_width ) {
		IArrayChars::const_iterator t = p;
		if ( get_max_width( t, m_width ) >= min_width )
			break;
	}
	return y;
}
//-----------------------------------------------------------------------------
int get_min_rect_width( const IArrayRects & rects ) {
	unsigned w = (unsigned)-1;
	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		if ( (unsigned)i->w < w )
			w = i->w;
	return w;
}
//-----------------------------------------------------------------------------
int get_max_rect_width( const IArrayRects & rects ) {
	int w = 0;
	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ ) {
		if ( i->w > w )
			w = i->w;
	}
	return w;
}
//-----------------------------------------------------------------------------
int get_min_rect_height( const IArrayRects & rects ) {
	unsigned h = (unsigned)-1;
	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		if ( (unsigned)i->h < h )
			h = i->h;
	return h;
}
//-----------------------------------------------------------------------------
int get_max_rect_height( const IArrayRects & rects ) {
	int h = 0;
	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		if ( i->h > h )
			h = i->h;
	return h;
}
//-----------------------------------------------------------------------------
IArrayRects find_rects_with_max_height( const IArrayRects & rects ) {
	int h = get_max_rect_height( rects );
	IArrayRects result;

	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		if ( i->h == h )
			result.push_back( *i );

	return result;
}
//-----------------------------------------------------------------------------
IArrayRects find_rects_with_max_width( const IArrayRects & rects ) {
	int w = get_max_rect_width( rects );
	IArrayRects result;

	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		if ( i->w == w )
			result.push_back( *i );

	return result;
}
//-----------------------------------------------------------------------------
IArrayRects find_rects_with_min_height( const IArrayRects & rects ) {
	int h = get_min_rect_height( rects );
	IArrayRects result;

	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		if ( i->h == h )
			result.push_back( *i );

	return result;
}
//-----------------------------------------------------------------------------
IArrayRects find_rects_with_min_width( const IArrayRects & rects ) {
	int w = get_min_rect_width( rects );
	IArrayRects result;

	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		if ( i->w == w )
			result.push_back( *i );

	return result;
}
//-----------------------------------------------------------------------------
IRect * get_best_rect_for( const IRect & rect, IArrayRects & rects, bool rotate = false) {
	IRect * x = 0;
	for( IArrayRects::iterator i = rects.begin(); i != rects.end(); ++i ) {

		/*cout << "w rect:" << rect.w << "image:" << i->w << endl;
		cout << "h rect:" << rect.h << "image:" << i->h << endl;*/

		if ( ( rect.h >= i->h ) && ( rect.w >= i->w ) ) {	//if both dimensions are less or equal to rect sizes
			if ( (!x) || ( i->w > x->w ) || ( ( i->w == x->w ) && ( i->h > x->h ) ) )	//if new width is greater than previous or equal but height is greater
				x = &*i;
		}
		if (rotate) {
			if ( ( rect.w >= i->h ) && ( rect.h >= i->w ) ) {	//if both dimensions are less or equal to rect sizes
				if ( (!x) || ( i->h > x->w ) || ( ( i->h == x->w ) && ( i->w > x->h ) ) ) {	//if new width is greater than previous or equal but height is greater
					i->rotate();
					x = &*i;
				}
			}
		}
	}
	return x;
}
//-----------------------------------------------------------------------------
unsigned long get_rects_square( const IArrayRects & rects ) {
	unsigned long s = 0;
	for( IArrayRects::const_iterator i = rects.begin(); i != rects.end(); i++ )
		s += i->w * i->h;
	return s;
}
//-----------------------------------------------------------------------------
void sortRectsByHeight( IArrayRects & rects, bool rotate )
{
	for( IArrayRects::iterator i = rects.begin(); i != --rects.end(); i++ ) {
		for( IArrayRects::iterator j = i; ++j != rects.end(); ) {
			if ( (j->h < j->w) && rotate)
				j->rotate();

			if ( ( i->h <= j->h ) && ( ( i->h < j->h ) || ( i->w < j->w ) ) ) {
				IRect t(*i);
				*i = *j;
				*j = t;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void removeRects(IArrayRects & from, const IArrayRects & what ) {
	for( IArrayRects::const_iterator i = what.begin(); i != what.end(); i++ ) {
		for( IArrayRects::iterator j = from.begin(); j != from.end(); j++ )
			if ( j->id == i->id ) {
				from.erase( j );
				break;
			}
	}
}
//-----------------------------------------------------------------------------
IArrayRects divideArray( IArrayRects & rects )
{
	IArrayRects result;
	sortRectsByHeight( rects );
	IArrayRects::iterator i = rects.begin();
	while( i != rects.end() ) {
		//cout<<rects.size()<<","<<i - rects.begin()<<endl;
		result.push_back( *i );
		rects.erase(i);
		if ( ++i != rects.end() )
			++i;
	}
	return result;
}
//-----------------------------------------------------------------------------
IArrayRects packRectangles( IRect & dest_rect, IArrayRects & source_rects, int max_height, bool rotate ) {
	IMatrix matrix( dest_rect.w, ( dest_rect.h > 0 ) ? dest_rect.h : 1 );
	IArrayRects ready;
	sortRectsByHeight( source_rects, rotate );
	while( ! source_rects.empty() ) {
		//printf("packing: %d\n", ready.size() );
		IRect s( matrix.get_max_rect() );
		IRect * d = get_best_rect_for( s, source_rects, rotate );
		if ( d ) {
			IArrayRects::iterator i;
			for( i = source_rects.begin(); i != source_rects.end(); i++ )
				if ( *i == *d )
					break;
			d->x = s.x;
			d->y = s.y;
			matrix.fill( d->x, d->y, d->w, d->h );
			ready.push_back(*d);
			source_rects.erase(i);
		} else {	//if all rects are lager than current best rect then resize matrix
			//int mh = get_max_rect_height( source_rects );
			IRect &s = source_rects.front();//find_rects_with_max_width( find_rects_with_max_height( source_rects ) ).front();
			int mh = s.h;
			int top = matrix.find_top_for_width( s.w );
			if ( matrix.get_height() - top < mh ) {
				int h = top + mh;
				if ( matrix.get_height() < max_height )
					h = ( h <= max_height ) ? h : max_height;
				else if ( matrix.get_height() >= max_height )
					break;
				matrix.set_height( h );
			}
			else
			{
				throw 0;	// it is impossible //matrix.set_height( matrix.get_height() + 1 );
			}
		}
		matrix.fill_small_rects( get_min_rect_width(source_rects) );
	}
	dest_rect.h = matrix.get_height();
	return ready;
}
//-----------------------------------------------------------------------------
