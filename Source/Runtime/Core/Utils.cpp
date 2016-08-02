/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Core/Utils.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <regex>
#include <stdlib.h>

namespace core
{
	namespace string
	{
		RUNTIME_API void Trim(std::string& str, const std::string& whitespace, bool left, bool right)
		{
			if (right)
				str.erase(str.find_last_not_of(whitespace) + 1); // trim right
			if (left)
				str.erase(0, str.find_first_not_of(whitespace)); // trim left
		}

		RUNTIME_API std::string ToLower(std::string& str)
		{
			std::string ret = str;
			std::transform(ret.begin(), ret.end(), ret.begin(), tolower);
			return ret;
		}

		RUNTIME_API std::string ToUpper(std::string& str)
		{
			std::string ret = str;
			std::transform(ret.begin(), ret.end(), ret.begin(), toupper);
			return ret;
		}

		RUNTIME_API int ToInt(std::string& str)
		{
			int ret;

			std::stringstream ss(str);
			ss >> ret;

			return ret;
		}

		RUNTIME_API float ToFloat(std::string& str)
		{
			float ret;

			std::stringstream ss(str);
			ss >> ret;

			return ret;
		}

		RUNTIME_API std::string FromInt(unsigned int i)
		{
			std::stringstream ss;
			ss << i;

			std::string ret = ss.str();

			return ret;
		}

		RUNTIME_API std::string FromInt(int i)
		{
			std::stringstream ss;
			ss << i;

			std::string ret = ss.str();

			return ret;
		}

		RUNTIME_API std::string FromFloat(float f)
		{
			std::stringstream ss;
			ss << f;

			std::string ret = ss.str();

			return ret;
		}

		RUNTIME_API void ReplaceFirst(std::string& str, const std::string& src, const std::string& dest)
		{
			std::size_t found = str.find(src);
			if (found != std::string::npos)
			{
				str.replace(found, src.length(), dest);
			}
		}

		RUNTIME_API void ReplaceChar(std::string& str, const char src, const char dest)
		{
			for (unsigned int i = 0; i < str.length(); i++)
			{
				if (str[i] == src)	str[i] = dest;
			}
		}

		RUNTIME_API uint32_t Hash(const std::string& str)
		{
			uint32_t hash, i;
			for (hash = i = 0; i < str.length(); ++i)
			{
				hash += str[i];
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}
			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);

			return hash;
		}
	}

	namespace math
	{
		RUNTIME_API bool IsPowerOfTwo(unsigned int num)
		{
			return ((num & (num - 1)) == 0);
		}

		RUNTIME_API unsigned int FixedToFixed(unsigned int value, unsigned int n, unsigned int p)
		{
			if (n > p)
			{
				// Less bits required than available; this is easy
				value >>= n - p;
			}
			else if (n < p)
			{
				// More bits required than are there, do the fill
				// Use old fashioned division, probably better than a loop
				if (value == 0)
					value = 0;
				else if (value == (((unsigned int)(1)) << n) - 1)
					value = (1 << p) - 1;
				else
					value = value*(1 << p) / ((1 << n) - 1);
			}
			return value;
		}

		RUNTIME_API unsigned int FloatToFixed(const float value, const unsigned int bits)
		{
			if (value <= 0.0f)
				return 0;
			else if (value >= 1.0f)
				return (1 << bits) - 1;
			else
				return (unsigned int)(value * (1 << bits));
		}

		RUNTIME_API float FixedToFloat(unsigned int value, unsigned int bits)
		{
			return (float)value / (float)((1 << bits) - 1);
		}

		unsigned short int FloatToHalf(float i)
		{
			union
			{
				float f;
				unsigned int i;
			}v;

			v.f = i;

			register int s = (v.i >> 16) & 0x00008000;
			register int e = ((v.i >> 23) & 0x000000ff) - (127 - 15);
			register int m = v.i & 0x007fffff;

			if (e <= 0)
			{
				if (e < -10)
				{
					return 0;
				}
				m = (m | 0x00800000) >> (1 - e);

				return s | (m >> 13);
			}
			else if (e == 0xff - (127 - 15))
			{
				if (m == 0) // Inf
				{
					return s | 0x7c00;
				}
				else // NAN
				{
					m >>= 13;
					return s | 0x7c00 | m | (m == 0);
				}
			}
			else
			{
				if (e > 30) // Overflow
				{
					return s | 0x7c00;
				}

				return s | (e << 10) | (m >> 13);
			}
		}

		RUNTIME_API float HalfToFloat(unsigned short int y)
		{
			union
			{
				float f;
				unsigned int i;
			}v;

			register int s = (y >> 15) & 0x00000001;
			register int e = (y >> 10) & 0x0000001f;
			register int m = y & 0x000003ff;

			if (e == 0)
			{
				if (m == 0) // Plus or minus zero
				{
					v.i = s << 31;
					return v.f;
				}
				else // Denormalized number -- renormalize it
				{
					while (!(m & 0x00000400))
					{
						m <<= 1;
						e -= 1;
					}

					e += 1;
					m &= ~0x00000400;
				}
			}
			else if (e == 31)
			{
				if (m == 0) // Inf
				{
					v.i = (s << 31) | 0x7f800000;
					return v.f;
				}
				else // NaN
				{
					v.i = (s << 31) | 0x7f800000 | (m << 13);
					return v.f;
				}
			}

			e = e + (127 - 15);
			m = m << 13;

			v.i = (s << 31) | (e << 23) | m;
			return v.f;
		}
	}

	namespace system
	{
		RUNTIME_API std::string GetEnv(const std::string& var)
		{
			char* path;
			size_t size;

			getenv_s(&size, nullptr, 0, var.c_str());
			if (size == 0)
				return std::string();

			path = (char*)malloc(size * sizeof(char));

			if (path == nullptr)
				return std::string();

			getenv_s(&size, path, size, var.c_str());

			std::string ret = path;

			free(path);

			return ret;
		}
	}
} // end namespace core
