#pragma once

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
// but slower than using dedicated SSE instructions
inline float fast_inverse_sqrt_quake(float number)
{
	union {
		float f;
		uint32_t i;
	} conv;

	float x2;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	conv.f = number;
	conv.i = 0x5f3759df - (conv.i >> 1);
	conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
	return conv.f;
}

// this might be even faster if the value to be normalizes is close to one 
// http://allenchou.net/2014/02/game-math-fast-re-normalization-of-unit-vectors/
inline float fast_inverse_sqrt_around_one(float x)
{
	const float a0 = 15.0f / 8.0f;
	const float a1 = -5.0f / 4.0f;
	const float a2 = 3.0f / 8.0f;

	return a0 + a1 * x + a2 * x * x;
}

#ifdef _WIN32
#include <immintrin.h>
// this is the fastest way, using SSE instructions !
// this incorporates also the necessary multiplication for vector normalization 
inline void fast_inverse_sqrt_sse(float*  pOut, float* pIn)
{
	__m128 in = _mm_load_ss(pIn);
	_mm_store_ss(pOut, _mm_rsqrt_ss(in));
}
#endif