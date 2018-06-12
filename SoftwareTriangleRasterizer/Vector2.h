#pragma once

#include "types.h"
#include "MathStuff.h"

#include <tuple>
#include <utility>

namespace math {
	class Vector2 {
	public:
		/* Constructors/Destructor */
		Vector2( ) = default;

		Vector2( float _x, float _y );

		Vector2( const Vector2& _input );

		/* Operator overloaders */
		Vector2 operator= ( const Vector2& _input );

		Vector2 operator+ ( const Vector2& _input );

		Vector2 operator- ( const Vector2& _input );

		Vector2 operator* ( const Vector2& _input );

		Vector2 operator/ ( const Vector2& _input );

		/* Member Functions */
		using int32Pair = std::tuple<int32, int32>;
		int32Pair to_int32( ) const {
			return (int32Pair( std::make_tuple( static_cast< int32 >(x), static_cast< int32 >(y) ) ));
		}
		
		/* Magnitude/Length squared function */
		inline float LengthSquared( )	const;

		/* Magnitude/Length function |v| */
		inline float Length( )			const;

		inline bool  IsZero( )			const;

		// puts vector between 0 -> 1 (unit vector)
		void Normalise( );

		// finds the angle between two given vectors (v,w)
		float Dot( const Vector2& _w ) const;

		// compute a new orthogonal vector, ( perpendicular ) to two given vectors (v,w)
		// Example usage: face normals, or camera's up vector

		/*	http://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
			while in 2D, this area is given by the cross-product itself 
			(which as mentioned before can also be interpreted as the determinant of a 2x2 matrix):
			Area= A.x ∗ B.y − A.y ∗ B.x.
		*/
		Vector2 Cross( const Vector2& _w ) const;

		Vector2 TripleProduct( const Vector2& _w ) const;

		/*	Example usage: if ScalarTripleProduct(v,w,u) > 0
			'u' must be left of v cross w, and soo on. i.e.
			tells us if an object is left/right of a given position
			--------------------------------------------------------
			there is an alternative method (which is cheaper)
			compute vector from target to source and do
			dot (axis X, target difference vector)
		*/
		float ScalarTripleProduct( const Vector2& _w, const Vector2& _u ) const;

		float FindAngleBetween( Vector2& _w ) const;

		/* Non-member Functions */
		static float	LengthSquared			( const Vector2& _v );
		static float	Length					( const Vector2& _v );
		static bool		IsZero					( const Vector2& _v ) ;
		static bool		IsZero					( const float&	 _input );
		static Vector2	Cross					( const Vector2& _v, const Vector2& _w	);
		static float	Dot						( const Vector2& _v, const Vector2& _w	);
		static Vector2	Normalise				( const Vector2& _v						);
		static Vector2	TripleProduct			( const Vector2& _v, const Vector2& _w	);
		static float	ScalarTripleProduct		( const Vector2& _v, const Vector2& _w, const Vector2& _u );
		static float	FindAngleBetween		( Vector2& _v, Vector2& _w, Vector2& _u );
		static bool 	FindDirectionBetween	( Vector2& _v, Vector2& _w, Vector2& _u, float* out_value = nullptr );

		/* Member variables */
		float x, y;

		/* Non-member variables */
		static const Vector2 Zero;
		static const Vector2 One;
		static const Vector2 Right;
		static const Vector2 Forward;
		static const Vector2 Up;
	};
}