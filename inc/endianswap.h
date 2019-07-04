//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_ENDIAN_H
#define RTM_RBASE_ENDIAN_H

#include <rbase/inc/platform.h>

namespace rtm {

	//--------------------------------------------------------------------------
	/// Static methods for manipulating endianess.
	//--------------------------------------------------------------------------
	struct Endian
	{
		/// swaps byte order in little endian encoded value on big endian architectures.
		template <typename T>
		inline static T swapLE(T _value)
		{
#if RTM_LITTLE_ENDIAN
			return _value;
#else
			return Endian::swap(_value);
#endif
		}

		/// swaps byte order in big endian encoded value on little endian architectures.
		template <typename T>
		inline static T swapBE(T _value)
		{
#if RTM_LITTLE_ENDIAN
			return Endian::swap(_value);
#else
			return _value;
#endif
		}

		inline static uint8_t swap(uint8_t _value)
		{
			return _value;
		}

		inline static uint16_t swap(uint16_t _value)
		{
			return  (_value>>8) | 
					((_value & 0xff)<<8);
		}

		inline static uint32_t swap(uint32_t _value)
		{
			return  (_value>>24) | 
					((_value<<8) & 0x00FF0000) |
					((_value>>8) & 0x0000FF00) |
					(_value<<24);
		}

		inline static uint64_t swap(uint64_t _value)
		{
			return  (_value>>56) | 
					((_value<<40) & 0x00FF000000000000LL) |
					((_value<<24) & 0x0000FF0000000000LL) |
					((_value<<8)  & 0x000000FF00000000LL) |
					((_value>>8)  & 0x00000000FF000000LL) |
					((_value>>24) & 0x0000000000FF0000LL) |
					((_value>>40) & 0x000000000000FF00LL) |
					(_value<<56);
		}

		inline static float swap(float _value)
		{
			float ret;
			char* src = (char*)&_value;
			char* dst = (char*)&ret;
			dst[0] = src[3];
			dst[1] = src[2];
			dst[2] = src[1];
			dst[3] = src[0];
			return ret;
		}
	};

} // namespace rtm

#endif  // RTM_RBASE_ENDIAN_H

