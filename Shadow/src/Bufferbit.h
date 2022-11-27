#pragma once

enum class BUFFER_BIT {
	COLOR_BUFFER_BIT = 0 << 1,
	DEPTH_BUFFER_BIT = 1 << 1,
	STENCIL_BUFFE_BIT = 2 << 1,
};

inline BUFFER_BIT operator|(BUFFER_BIT a, BUFFER_BIT b)
{
	return static_cast<BUFFER_BIT>(static_cast<int>(a) | static_cast<int>(b));
}