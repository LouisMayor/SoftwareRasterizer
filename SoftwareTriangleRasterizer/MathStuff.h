#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cfloat>

namespace math {
	static constexpr double _pi = M_PI;
	static constexpr float  _epsilon = FLT_EPSILON;

	inline float radians_to_degrees( const float& _rads ) {
		return (_rads * 180.0f) / _pi;
	}

	inline float degrees_to_radians( const float& _degrees ) {
		return (_degrees * _pi) / 180.0f;
	}
}