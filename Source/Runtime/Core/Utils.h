// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>

#include <string>
#include <vector>

namespace core
{
	namespace string
	{
		inline RUNTIME_API void Trim(std::string& str, const std::string& whitespace = " \t\n", bool left = true, bool right = true);
		inline RUNTIME_API std::string ToLower(std::string& str);
		inline RUNTIME_API std::string ToUpper(std::string& str);

		inline RUNTIME_API int ToInt(std::string& str);
		inline RUNTIME_API float ToFloat(std::string& str);

		inline RUNTIME_API std::string FromInt(unsigned int i);
		inline RUNTIME_API std::string FromInt(int i);
		inline RUNTIME_API std::string FromFloat(float f);

		inline RUNTIME_API void ReplaceFirst(std::string& str, const std::string& src, const std::string& dest);

		inline RUNTIME_API void ReplaceChar(std::string& str, const char src, const char dest);

		inline RUNTIME_API uint32_t Hash(const std::string& str);
	}

	namespace math
	{
		inline RUNTIME_API bool IsPowerOfTwo(unsigned int num);

		//! Convert N bit color channel value to P bits. It fills P bits with the bit pattern repeated.
		inline RUNTIME_API unsigned int FixedToFixed(unsigned int value, unsigned int n, unsigned int p);
		//! Convert floating point color channel value between 0.0 and 1.0 (otherwise clamped) to integer of a certain number of bits. Works for any value of bits between 0 and 31.
		inline RUNTIME_API unsigned int FloatToFixed(const float value, const unsigned int bits);
		//! Fixed point to float.
		inline RUNTIME_API float FixedToFloat(unsigned int value, unsigned int bits);
		//! Convert a float32 to a float16 (NV_half_float).Courtesy of OpenEXR.
		inline RUNTIME_API unsigned short int FloatToHalf(float i);
		//! Convert a float16 (NV_half_float) to a float32.Courtesy of OpenEXR.
		inline RUNTIME_API float HalfToFloat(unsigned short int y);
	}

	namespace system
	{
		inline RUNTIME_API std::string GetEnv(const std::string& var);
	}
} // end namespace core