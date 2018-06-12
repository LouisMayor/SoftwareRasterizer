#pragma once

/*
	Acknowledgement: https://www.joshbeam.com/articles/simple_line_drawing/ /\ https://www.joshbeam.com/articles/triangle_rasterization/
	for original code.
*/

#include "Vector2.h"
#include "ResourceClasses.h"
#include <fstream>

//#define NDEBUG /* disables assert() */
#include <cassert>

class Rasterizer {
public:
	Rasterizer( ) = default;

	Rasterizer( const Rasterizer & ) = delete;
	Rasterizer operator=( const Rasterizer & ) = delete;

	~Rasterizer( ) {
		Release( );
	}

	void AllocateFrameBuffer( const uint32 _width, const uint32 _height ) {
		Release( );

		std::printf( "Allocating frame buffer\n" );

		frameWidth = _width;
		frameHeight = _height;
		frameBuffer = new Colour[frameWidth * frameHeight];

		ClearBuffer( Colour( .0f, .0f, .0f, 1.f ) );
	}

	void DeallocateFrameBuffer( ) {
		Release( );
	}

	void SetFrameBuffer( Colour* const _inputBuffer, const uint32 _width, const uint32 _height ) {
		Release( );

		frameBuffer = _inputBuffer;
		frameWidth = _width;
		frameHeight = _height;
	}

	void ClearBuffer( ) {
		assert( frameBuffer != nullptr );
		std::printf( "clearing buffer\n" );
		FillBlack( );
	}

	void ClearBuffer( const Colour& _pixelColour ) {
		assert( frameBuffer != nullptr );
		std::printf( "clearing buffer\n" );
		FillColour( _pixelColour );
	}

	void DrawSpan( const Span &span, const uint32 y ) {
		assert( frameBuffer != nullptr );
		std::printf( "drawing span\n" );

		const int32 x_vector = span.x1 - span.x0;
		if( x_vector == 0 ) {
			std::printf( "Warning: spans x on same point\n" );
			return;
		}

		const Colour col_vector = span.col1 - span.col0;
		float32 factor = 0.0f;
		float32 fStep = 1.0f / static_cast< float32 >(x_vector);

		/* finally drawing */
		for( uint32 x = span.x0; x < span.x1; ++x ) {
			SetPixel( x, y, span.col0 + (col_vector * factor) );
			factor += fStep;
		}
	}

	void DrawSpawBetweenEdges( const Edge &e0, const Edge &e1 ) {
		assert( frameBuffer != nullptr );	

		/* compute difference on Y plane. if no differnce, i.e. 0. return */
		float32 edge0_y_vector;
		float32 edge1_y_vector;

		if( e0.y1 > e0.y0 ) {
			edge0_y_vector = static_cast< float32 > (e0.y1 - e0.y0);
			edge0_y_vector = -edge0_y_vector;
		} else {
			edge0_y_vector = e0.y0 - e0.y1;
		}

		if( e1.y1 > e1.y0 ) {
			edge1_y_vector = static_cast< float32 > (e1.y1 - e1.y0);
			edge1_y_vector = -edge1_y_vector;
		} else {
			edge1_y_vector = e1.y0 - e1.y1;
		}

		/* questionable choice to cast to float and then do 0 cmp */
		if( edge0_y_vector == 0.f || edge1_y_vector == 0.f ) {
			std::printf( "Warning: edges y on same point\n" );
			return;
		}

		float32 edge0_x_vector;
		float32 edge1_x_vector;

		if( e0.x1 > e0.x0 ) {
			edge0_x_vector = static_cast< float32 > (e0.x1 - e0.x0);
			edge0_x_vector = -edge0_x_vector;
		} else {
			edge0_x_vector = e0.x0 - e0.x1;
		}

		if( e1.x1 > e1.x0 ) {
			edge1_x_vector = static_cast< float32 > (e1.x1 - e1.x0);
			edge1_x_vector = -edge1_x_vector;
		} else {
			edge1_x_vector = e1.x0 - e1.x1;
		}

		std::printf( "drawing span between edges\n" );

		Colour  edge0_colour_vector = (e0.col1 - e0.col0);
		Colour  edge1_colour_vector = (e1.col1 - e1.col0);

		/* compute values for interpolation */
		float32 factor0 = static_cast< float32 >(e1.y0 - e0.y0) / edge0_y_vector;
		float32 factor1 = 0.0f;
		const float32 fStep0 = (1.0f / edge0_y_vector);
		const float32 fStep1 = (1.0f / edge1_y_vector);

		/* lets get drawing */
		for( uint32 y = e1.y0; y < e1.y1; ++y ) {

			//auto col0 = e0.col0 + (edge0_colour_vector * factor0);
			//auto col1 = e1.col0 + (edge1_colour_vector * factor1);

			auto col0 = e0.col0;
			auto col1 = e1.col0;

			const Span tmp( col0, e0.x0 + static_cast< int32 >(edge0_x_vector * factor0),
							col1, e1.x0 + static_cast< int32 >(edge1_x_vector * factor1) );

			DrawSpan( tmp, y );

			factor0 += fStep0;
			factor1 += fStep1;
		}
	}

	void DrawTriangle( const Colour &colour0, float32 x0, float32 y0,
					   const Colour &colour1, float32 x1, float32 y1,
					   const Colour &colour2, float32 x2, float32 y2 ) {
		assert( frameBuffer != nullptr );
		std::printf( "drawing triangle\n" );

		/* convert NDC to pixel */
		float32 pixelx0, pixely0;
		ndc_to_pixel( x0, y0, pixelx0, pixely0 );

		float32 pixelx1, pixely1;
		ndc_to_pixel( x1, y1, pixelx1, pixely1 );

		float32 pixelx2, pixely2;
		ndc_to_pixel( x2, y2, pixelx2, pixely2 );

		/*	create "edge" list
			ostensibly the same as our NDC for the line version
		*/
		Edge edges[3] = {
			Edge( colour0, pixelx0, pixely0, colour1, pixelx1, pixely1 ),
			Edge( colour1, pixelx1, pixely1, colour2, pixelx2, pixely2 ),
			Edge( colour2, pixelx2, pixely2, colour0, pixelx0, pixely0 )
		};

		/* find c2 (pythag a2 + b2 = c2) out of 3 edges */
		uint32 maxLength = 0;
		uint32 longestEdgeIndex = 0;

		for( uint32 i = 0; i < 3; ++i ) {
			uint32 tmpLength = (edges[i].y1 - edges[i].y0);
			if( tmpLength > maxLength ) {
				maxLength = tmpLength;
				longestEdgeIndex = i;
			}
		}

		uint32 shortEdge0 = ( longestEdgeIndex + 1 ) % 3;
		uint32 shortEdge1 = ( longestEdgeIndex + 2 ) % 3;

		DrawSpawBetweenEdges( edges[longestEdgeIndex], edges[shortEdge0] );
		DrawSpawBetweenEdges( edges[longestEdgeIndex], edges[shortEdge1] );
	}

	float32 EdgeFunction( const math::Vector2& a,
						  const math::Vector2& b,
						  const math::Vector2& c ) {
		assert( frameBuffer != nullptr );
		return((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x));
	}

	/* http://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage */
	void Triangle( const Colour &colour0, math::Vector2 _v0,
				   const Colour &colour1, math::Vector2 _v1,
				   const Colour &colour2, math::Vector2 _v2 ) {
		assert( frameBuffer != nullptr );

		/* convert NDC to pixel */
		math::Vector2 v0;
		ndc_to_pixel( _v0.x, _v0.y, v0.x, v0.y );

		math::Vector2 v1;
		ndc_to_pixel( _v1.x, _v1.y, v1.x, v1.y );

		math::Vector2 v2;
		ndc_to_pixel( _v2.x, _v2.y, v2.x, v2.y );

		/*	order inputs
			closest point to origin -> furthest point
			(v0 -> v2)
		*/

		if( fabsf(v0.x + v0.y) > fabsf(v1.x + v1.y)) {
			std::swap( v0.x, v1.x );
			std::swap( v0.y, v1.y );
		}

		if( fabsf( v1.x + v1.y ) > fabsf( v2.x + v2.y ) ) {
			std::swap( v1.x, v2.x );
			std::swap( v1.y, v2.y );
		}

		if( fabsf( v0.x + v0.y ) > fabsf( v1.x + v1.y ) ) {
			std::swap( v0.x, v1.x );
			std::swap( v0.y, v1.y );
		}

		float32 area = EdgeFunction( v0, v1, v2 );
		std::printf( "Buffer resolution: %ux%u\n", frameWidth, frameHeight );

		uint32 n = 0;
		for( uint32_t y = 0; y < frameHeight; ++y ) {
			for( uint32_t x = 0; x < frameWidth; ++x ) {				
				std::printf( "\rpixel: %u/%u", n+1, frameWidth*frameHeight );
				n++;
				math::Vector2 p = math::Vector2( x + 0.5f, y + 0.5f );
				float32 w0 = EdgeFunction( v1, v2, p );
				float32 w1 = EdgeFunction( v2, v0, p );
				float32 w2 = EdgeFunction( v0, v1, p );
				if( w0 >= 0 && w1 >= 0 && w2 >= 0 || w0 <= 0 && w1 <= 0 && w2 <= 0 ) {
					w0 /= area;
					w1 /= area;
					w2 /= area;				
					SetPixel( x, y, colour1 );
				}
			}
		}
		std::printf( "\n" );
	}

	/* https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm /|\ https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B */
	void BresenhamLineFunction( const Colour& _lineColour0, const math::Vector2& line0,
								const Colour& _lineColour1, const math::Vector2& line1 ) {
		assert( frameBuffer != nullptr );
		std::printf( "drawing line\n" );

		/* convert NDC to pixel */
		math::Vector2 v1;
		ndc_to_pixel( line0.x, line0.y, v1.x, v1.y );

		math::Vector2 v2;
		ndc_to_pixel( line1.x, line1.y, v2.x, v2.y );

		const bool steepGradient = fabs( v2.y - v1.y ) > fabs( v2.x - v1.x );

		if( steepGradient ) {
			std::swap( v1.x, v1.y );
			std::swap( v2.x, v2.y );
		}

		if( v1.x > v2.x ) {
			std::swap( v1.x, v2.x );
			std::swap( v1.y, v2.y );
		}

		float32 dx = v2.x - v1.x;
		float32 dy = fabs( v2.y - v1.y );
		float32 d = dx / 2.0f;
		const int32 yStep = (v1.y < v2.y) ? 1 : -1;

		auto tmp1 = v1.to_int32( );
		int32 y = std::get<1>( tmp1 );

		const int32 maxX = v2.x;

		for( int32 x = std::get<0>( tmp1 ); x < maxX; x++ ) {
			if( steepGradient ) {
				SetPixel( y, x, _lineColour0 );
			} else {
				SetPixel( x, y, _lineColour0 );
			}

			d -= dy;
			if( d < 0 ) {
				y += yStep;
				d += dx;
			}
		}
	}

	void DrawLine( Colour& _lineColour0, const float32 _linePoint0X, const float32 _linePoint0Y,
				   Colour& _lineColour1, const float32 _linePoint1X, const float32 _linePoint1Y ) {
		assert( frameBuffer != nullptr );
		std::printf( "drawing line\n" );

		float32 x_vector;
		float32 y_vector;
		
		/* convert NDC to pixel */
		float32 pixelx0, pixely0;
		ndc_to_pixel( _linePoint0X, _linePoint0Y, pixelx0, pixely0 );

		float32 pixelx1, pixely1;
		ndc_to_pixel( _linePoint1X, _linePoint1Y, pixelx1, pixely1 );

		/*	slight deviation from source code.
			always negate larger -> smaller.

			in the case 0 - 1
			flip x/y value.

			otherwise, the x/y vector gets erroneous
		
			+ 0 ( top left x and y )
			is redundant in this case, except when your coordinate space is not topleft orientated
		*/

		if( pixely1 > pixely0 ) {
			y_vector = pixely1 - pixely0;
			y_vector = -y_vector;
		} else {
			y_vector = pixely0 - pixely1;
		}

		if( pixelx1 > pixelx0 ) {
			x_vector = pixelx1 - pixelx0;
			x_vector = -x_vector;
		} else {
			x_vector = pixelx0 - pixelx1;
		}

		/* line points exists on the same pixel, draw colour of the first point */
		if( x_vector == 0.f && y_vector == 0.f ) {
			SetPixel( pixelx0, pixely0, _lineColour0 );
			return;
		}

		if( fabsf( x_vector ) > fabsf( y_vector ) ) {
			float32 x_min, x_max;
			if( pixelx0 < pixelx1 ) {
				x_min = pixelx0;
				x_max = pixelx1;
			} else {
				x_min = pixelx1;
				x_max = pixelx0;
			}

			float32 slope = y_vector / x_vector;
			for( float32 x = x_min; x <= x_max; x += 1.f ) {
				const float32 y = pixely0 + ((x - pixelx0) * slope);
				const Colour col = _lineColour0 + ((_lineColour1 - _lineColour0) * ((x - pixelx0) / x_vector));
				SetPixel( x, y, col );
			}
		} else {
			float32 y_min, y_max;
			if( pixely0 < pixely1 ) {
				y_min = pixely0;
				y_max = pixely1;
			} else {
				y_min = pixely1;
				y_max = pixely0;
			}

			float32 slope = x_vector / y_vector;
			for( float32 y = y_min; y <= y_max; y += 1.f ) {
				const float32 x = pixelx0 + ((y - pixely0) * slope);
				const Colour col = _lineColour0 + ((_lineColour1 - _lineColour0) * ((y - pixely0) / y_vector));
				SetPixel( x, y, col );
			}
		}
	}

	void WriteToPPM( const char* const _filename ) {
		assert( frameBuffer != nullptr );
		std::printf( "Writing image\n" );

		if( frameWidth == 0 || frameHeight == 0 ) { fprintf( stderr, "Can't save an empty image\n" ); return; }
		std::ofstream ofs;
		try {
			/* need to spec. binary mode for Windows users */
			ofs.open( _filename, std::ios::binary );
			if( ofs.fail( ) ) throw("Can't open output file");
			ofs << "P6\n" << frameWidth << " " << frameHeight << "\n255\n";
			uint8 r, g, b;
			/* loop over each pixel in the image, clamp and convert to byte format */
			for( int i = 0; i < frameWidth * frameHeight; ++i ) {
				r = static_cast< uint8 >(std::fmin( 1.f, frameBuffer[i].r ) * 255);
				g = static_cast< uint8 >(std::fmin( 1.f, frameBuffer[i].g ) * 255);
				b = static_cast< uint8 >(std::fmin( 1.f, frameBuffer[i].b ) * 255);
				ofs << r << g << b;
			}
			ofs.close( );
		}
		catch( const char* const err ) {
			fprintf( stderr, "%s\n", err );
			ofs.close( );
		}
		std::printf( "\"%s.ppm\" can be found in solution dir\n\n", _filename );
	}

	static void debug_convert_pixel_to_ndc( const float32 _x, const float32 _y,
											const float32 _frameWidth, const float32 _frameHeight ) {
		const float32 ndc_x = _x / (_frameWidth / 2.f) - 1.f;
		const float32 ndc_y = 1.f - _y / (_frameHeight / 2.f);

		std::printf( "x: %f y: %f", ndc_x, ndc_y );
	}

	void debug_convert_pixel_to_ndc( const float32 _x, const float32 _y ) {
		const float32 ndc_x = _x / (frameWidth / 2.f) - 1.f;
		const float32 ndc_y = 1.f - _y / (frameHeight / 2.f);

		std::printf( "x: %f y: %f", ndc_x, ndc_y );
	}

	void pixel_to_ndc( const float32 _x, const float32 _y, float32& ndc_x, float32& ndc_y ) {
		ndc_x = _x / (frameWidth / 2.f) - 1.f;
		ndc_y = 1.f - _y / (frameHeight / 2.f);
	}

	void ndc_to_pixel( const float32 _x0, const float32 _y0, float32& _x1, float32& _y1 ) {
		_x1 = (_x0 + 1.f) * frameWidth  * .5f + 0.f;
		_y1 = (1.f - _y0) * frameHeight * .5f + 0.f;
	}

private:
	void SetPixel( const float32 _x, const float32 _y, const Colour& _pixelColour ) {
		assert( frameBuffer != nullptr );

		/* check pixel bounds */
		if( _x >= frameWidth || _y >= frameHeight ) {
			return;
		}
		const uint32 x = static_cast< uint32 >(_x);
		const uint32 y = static_cast< uint32 >(_y);
		frameBuffer[y * frameWidth + x] = _pixelColour;
	}

	void Release( ) {
		if( frameBuffer ) {
			delete[] frameBuffer;
		}
		frameBuffer = nullptr;
		frameWidth = 0;
		frameHeight = 0;
	}

	void FillWhite( ) {
		assert( frameBuffer != nullptr );
		for( uint32 x = 0; x < frameWidth; ++x ) {
			for( uint32 y = 0; y < frameHeight; ++y ) {
				SetPixel( x, y, Colour( 1.f, 1.f, 1.f, 1.f ) );
			}
		}
	}

	void FillBlack( ) {
		assert( frameBuffer != nullptr );
		for( uint32 x = 0; x < frameWidth; ++x ) {
			for( uint32 y = 0; y < frameHeight; ++y ) {
				SetPixel( x, y, Colour( .0f, .0f, .0f, .0f ) );
			}
		}
	}

	void FillColour( const Colour& _col ) {
		assert( frameBuffer != nullptr );
		for( uint32 x = 0; x < frameWidth; ++x ) {
			for( uint32 y = 0; y < frameHeight; ++y ) {
				SetPixel( x, y, Colour( _col ) );
			}
		}
	}

	/* each element in the frame buffer, i.e. pixel will contain a Colour */
	Colour* frameBuffer = nullptr;
	uint32 frameWidth, frameHeight;
};