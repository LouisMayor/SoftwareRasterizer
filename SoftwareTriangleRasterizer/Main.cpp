#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "TemplateRAIIManager.h"
#include "Rasterizer.h"

/*	based on https://www.joshbeam.com/articles/triangle_rasterization/ & http://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage

	PPM functions from http://www.scratchapixel.com/code.php?id=51&origin=/lessons/digital-imaging/simple-image-manipulations

	*ignore* just use normalised device coordinates instead
	FYI for placing vertices. Using D3D coordinate space.
	i.e. 0 coords at the top left.

	opposed to OpenGL.
	i.e. 0 coords at the bottom left
*/

void DrawEquilateralTriangle( Rasterizer* const ras, const uint32& frameWidth, const uint32& frameHeight ) {
	Colour color0( 1.0f, 0.0f, 0.0f );
	math::Vector2 v0 = math::Vector2( -1.f, -1.f );

	Colour color1( 0.0f, 1.0f, 0.0f );
	math::Vector2 v1 = math::Vector2( .0f, 1.f );

	Colour color2( 0.0f, 0.0f, 1.0f );
	math::Vector2 v2 = math::Vector2( 1.f, -1.f );

	ras->Triangle( color0, v0, color1, v1, color2, v2 );
	//ras->DrawTriangle( color0, x0, y0, color1, x1, y1, color0, x2, y2 );

	ras->WriteToPPM( "EquilateralTriangle" );
}

void DrawRightAngleTriangle( Rasterizer* const ras, const uint32& frameWidth, const uint32& frameHeight ) {
	Colour color0( 1.0f, 0.0f, 0.0f );
	math::Vector2 v0 = math::Vector2( -.5f, -1.f );

	Colour color1( 0.0f, 1.0f, 0.0f );
	math::Vector2 v1 = math::Vector2( -.5f, 1.f );

	Colour color2( 0.0f, 0.0f, 1.0f );
	math::Vector2 v2 = math::Vector2( 1.f, -1.f );

	ras->Triangle( color0, v0, color1, v1, color2, v2 );
	//ras->DrawTriangle( color0, x0, y0, color1, x1, y1, color0, x2, y2 );

	ras->WriteToPPM( "RightAngleTriangle" );
}

void DrawQuad( Rasterizer* const ras, const uint32& frameWidth, const uint32& frameHeight ) {
	Colour color0( 1.0f, 0.0f, 0.0f );
	math::Vector2 v0 = math::Vector2( -.5f, -.5f );

	Colour color1( 0.0f, 1.0f, 0.0f );
	math::Vector2 v1 = math::Vector2( -.5f, .5f );

	Colour color2( 0.0f, 0.0f, 1.0f );
	math::Vector2 v2 = math::Vector2( .5f, -.5f );

	Colour color3( 0.0f, 0.0f, 1.0f );
	math::Vector2 v3 = math::Vector2( .5f,  .5f );

	ras->Triangle( color0, v0, color1, v1, color2, v2 );
	ras->Triangle( color0, v2, color1, v3, color2, v1 );

	//ras->DrawTriangle( color0, x0, y0, color0, x1, y1, color0, x2, y2 );
	//ras->DrawTriangle( color0, x2, y2, color0, x3, y3, color0, x1, y1 );
	ras->WriteToPPM( "Quad" );
}

void ObtuseScaleneTriangle( Rasterizer* const ras, const uint32& frameWidth, const uint32& frameHeight ) {
	Colour color0( 1.0f, 0.0f, 0.0f );
	math::Vector2 v0 = math::Vector2( 1.f, -1.f );

	Colour color1( 0.0f, 1.0f, 0.0f );
	math::Vector2 v1 = math::Vector2( .0f, -.6f );

	Colour color2( 0.0f, 0.0f, 1.0f );
	math::Vector2 v2 = math::Vector2( -.8f, 1.f );

	ras->Triangle( color0, v0, color1, v1, color2, v2 );

	ras->WriteToPPM( "ObtuseScaleneTriangle" );
}

int main( ) {
	raii::RAIIManager<Rasterizer> rasterizer = raii::make_unique_ptr<Rasterizer>( );

	Colour clearCol( 0.0f, 0.0f, 0.0f );
	uint32 width =  800;
	uint32 height = 800;

	rasterizer->AllocateFrameBuffer( width, height );

	rasterizer->ClearBuffer( );
	DrawEquilateralTriangle( rasterizer.get( ), width, height );

	rasterizer->ClearBuffer( );
	DrawQuad( rasterizer.get( ), width, height );

	rasterizer->ClearBuffer( );
	ObtuseScaleneTriangle( rasterizer.get( ), width, height );

	rasterizer->ClearBuffer( );
	DrawRightAngleTriangle( rasterizer.get( ), width, height );

	std::cin.get( );

	return 0;
}