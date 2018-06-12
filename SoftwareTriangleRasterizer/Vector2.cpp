#include "Vector2.h"

namespace math {
	Vector2::Vector2( float _x, float _y )
		: x( _x ), y( _y ){
	};

	Vector2::Vector2( const Vector2& _input )
		: x( _input.x ), y( _input.y ) {
	}

	Vector2 Vector2::operator= ( const Vector2& _input ) {
		if( this != &_input ) {
			x = _input.x;
			y = _input.y;
		}
		return (*this);
	}

	Vector2 Vector2::operator+ ( const Vector2& _input ) {
		x += _input.x;
		y += _input.y;
		return (*this);
	}

	Vector2 Vector2::operator- ( const Vector2& _input ) {
		x -= _input.x;
		y -= _input.y;
		return (*this);
	}

	Vector2 Vector2::operator* ( const Vector2& _input ) {
		x *= _input.x;
		y *= _input.y;
		return (*this);
	}

	Vector2 Vector2::operator/ ( const Vector2& _input ) {
		x /= _input.x;
		y /= _input.y;
		return (*this);
	}

	inline float Vector2::LengthSquared( ) const {
		return (x * x + y * y);
	}

	inline float Vector2::Length( ) const {
		return sqrtf( x * x + y * y );
	}

	inline bool Vector2::IsZero( ) const {
		return fabs( x * x + y * y ) < _epsilon;
	}

	void Vector2::Normalise( ) {
		if( IsZero( ) ) {
			x = 0.0f;
			y = 0.0f;
			return;
		}
		/*	 1
			---
			|v|
		*/
		float length = 1.0f / sqrtf( LengthSquared( ) );
		x *= length;
		y *= length;
	}

	float Vector2::Dot( const Vector2& _w ) const {
		return (x * _w.x + y * _w.y);
	}

	Vector2 Vector2::Cross( const Vector2& _w ) const {
		return Vector2( y * _w.x - _w.y * x,
						x * _w.y - _w.x * y );
	}

	Vector2 Vector2::TripleProduct( const Vector2& _w ) const {
		return Vector2( Cross( _w, this->Cross( _w ) ) );
	}

	float Vector2::ScalarTripleProduct( const Vector2& _w, const Vector2& _u ) const {
		return Dot( _u, this->Cross( _w ) );
	}

	// finds the angle between two given vectors (v,w)
	float Vector2::Dot( const Vector2& _v, const Vector2& _w ) {
		return (_v.x * _w.x + _v.y * _w.y);
	}

	// compute a new orthogonal vector, ( perpendicular ) to two given vectors (v,w)
	Vector2 Vector2::Cross( const Vector2& _v, const Vector2& _w ) {
		return Vector2( _v.y , -_v.x );
		//return Vector2( _v.x * _w.y - _w.y * _v.x,
		//				_v.y * _w.x - _w.x * _v.y );
	}

	// puts vector between 0 -> 1 (unit vector)
	Vector2 Vector2::Normalise( const Vector2& _v ) {
		if( _v.IsZero( ) ) {
			return Vector2( 0.0f, 0.0f );
		}
		/*	 1
			---
			|v|
		*/
		float length = 1.0f / sqrtf( _v.LengthSquared( ) );
		return Vector2( _v.x * length, _v.y * length );
	}

	Vector2 Vector2::TripleProduct( const Vector2& _v, const Vector2& _w ) {
		return Vector2( Cross( _w, Cross( _v, _w ) ) );
	}

	float Vector2::ScalarTripleProduct( const Vector2& _v, const Vector2& _w, const Vector2& _u ) {
		return Dot( _u, Cross( _v, _w ) );
	}

	/* returns in degrees */
	float Vector2::FindAngleBetween( Vector2& _v, Vector2& _w, Vector2& _u ) {
		Vector2 _target_source = Vector2( _w - _v );
		_u.Normalise( );
		_target_source.Normalise( );
		return radians_to_degrees( acos( Dot( _u, _target_source ) ) );
	}

	/* returns boolean. false if left, true if right */
	bool Vector2::FindDirectionBetween( Vector2 & _v, Vector2 & _w, Vector2 & _u, float* out_value ) {
		Vector2 _target_source = Vector2( _w - _v );
		_u.Normalise( );
		_target_source.Normalise( );
		float _direction = Dot( _u, _target_source );
		if( out_value ) {
			*out_value = radians_to_degrees( _direction );
		}
		return (_direction > 0.0f);
	}

	float Vector2::LengthSquared( const Vector2& _v ) {
		return (_v.x * _v.x + _v.y * _v.y );
	}

	float Vector2::Length( const Vector2& _v ) {
		return sqrtf( _v.x * _v.x + _v.y * _v.y );
	}

	bool  Vector2::IsZero( const Vector2& _v ) {
		return fabs( _v.x * _v.x + _v.y * _v.y ) < _epsilon;
	}

	bool  Vector2::IsZero( const float& _input ) {
		return _input < _epsilon;
	}

	const Vector2 Vector2::Zero( 0.0f, 0.0f );
	const Vector2 Vector2::One( 1.0f, 1.0f );
	const Vector2 Vector2::Right( -1.0f, 0.0f );
	const Vector2 Vector2::Forward( 0.0f, 0.0f );
	const Vector2 Vector2::Up( 0.0f, 1.0f );
}