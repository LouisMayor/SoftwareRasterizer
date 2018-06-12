#pragma once

#include "types.h"

/* 8bit RGBA colour */
class Colour {
public:
	Colour( ) = default;

	Colour( const float32 _r, const float32 _g, const float32 _b ) {
		r = _r;
		g = _g;
		b = _b;
		a = 1.0f;
	}

	Colour( const float32 _r, const float32 _g, const float32 _b, const float32 _a ) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Colour( const Colour& _col ) {
		r = _col.r;
		g = _col.g;
		b = _col.b;
		a = _col.a;
	}

	Colour operator=( const Colour& _col ) {
		this->r = _col.r;
		this->g = _col.g;
		this->b = _col.b;
		this->a = _col.a;
		return (*this);
	}

	Colour operator+( const Colour& _col ) const {
		return Colour( r + _col.r, g + _col.g, b + _col.b, a + _col.a );
	}

	Colour operator-( const Colour& _col ) const {
		return Colour( std::fmaxf( r - _col.r, 0.0f ), std::fmaxf( g - _col.g, 0.0f ), std::fmaxf( b - _col.b, 0.0f ), std::fmaxf( a - _col.a, 0.0f ) );
	}

	Colour operator*( const Colour& _col ) const {
		return Colour( r * _col.r, g * _col.g, b * _col.b, a * _col.a );
	}

	Colour operator*( const float32& _col ) const {
		return Colour( r * _col, g * _col, b * _col, a * _col );
	}

	uint32 to_uint32( ) const {
		uint32 _r = ( uint32 ) (r * 255.0f);
		uint32 _g = ( uint32 ) (g * 255.0f);
		uint32 _b = ( uint32 ) (b * 255.0f);
		uint32 _a = ( uint32 ) (a * 255.0f);

		return (_a << 24) | (_r << 16) | (_g << 8) | _b;
	}

	float32 r, g, b, a;
};

class Edge {
public:
	Edge( ) = default;

	Edge( const Colour &_colour0, int32 _x0, int32 _y0,
		  const Colour &_colour1, int32 _x1, int32 _y1 ) {
		if( _y0 < _y1 ) {
			col0 = _colour0;
			x0 = _x0;
			y0 = _y0;
			col1 = _colour1;
			x1 = _x1;
			y1 = _y1;
		} else {
			col0 = _colour1;
			x0 = _x1;
			y0 = _y1;
			col1 = _colour0;
			x1 = _x0;
			y1 = _y0;
		}
	}

	Colour col0, col1;
	int32 x0, y0, x1, y1;
};

class Span {
public:
	Span( ) = default;

	Span( const Colour& _colour0, const int32 _x0,
		  const Colour& _colour1, const int32 _x1 ) {
		if( _x0 < _x1 ) {
			col0 = _colour0;
			x0 = _x0;
			col1 = _colour1;
			x1 = _x1;
		} else {
			col0 = _colour1;
			x0 = _x1;
			col1 = _colour0;
			x1 = _x0;
		}
	}

	Colour col0, col1;
	int32 x0, x1;
};