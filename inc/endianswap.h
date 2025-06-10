//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_ENDIAN_H
#define RTM_RBASE_ENDIAN_H

#include <rbase/inc/platform.h>

namespace rtm {

	/// Endian swaps a value
	/// 
	/// @param[in] _value: Value to endian swap
	/// 
	/// @returns the resulting endian swapped value.
	static inline uint8_t endianSwap(uint8_t _value);

	/// Endian swaps a value
	/// 
	/// @param[in] _value: Value to endian swap
	/// 
	/// @returns the resulting endian swapped value.
	static inline uint16_t endianSwap(uint16_t _value);

	/// Endian swaps a value
	/// 
	/// @param[in] _value: Value to endian swap
	/// 
	/// @returns the resulting endian swapped value.
	static inline uint32_t endianSwap(uint32_t _value);

	/// Endian swaps a value
	/// 
	/// @param[in] _value: Value to endian swap
	/// 
	/// @returns the resulting endian swapped value.
	static inline uint64_t endianSwap(uint64_t _value);

	/// Endian swaps a value
	/// 
	/// @param[in] _value: Value to endian swap
	/// 
	/// @returns the resulting endian swapped value.
	static inline float endianSwap(float _value);

} // namespace rtm

/// ---------------------------------------------------------------------- ///
///  Implementation                                                        ///
/// ---------------------------------------------------------------------- ///

namespace rtm {

	//--------------------------------------------------------------------------
	/// Static methods for manipulating endianess.
	//--------------------------------------------------------------------------
	struct Endian
	{
		/// swaps byte order in little endian encoded value on big endian architectures.
		template <typename T>
		static inline T swapLE(T _value)
		{
#if RTM_LITTLE_ENDIAN
			return _value;
#else
			return rtm::endianSwap(_value);
#endif
		}

		/// swaps byte order in big endian encoded value on little endian architectures.
		template <typename T>
		static inline T swapBE(T _value)
		{
#if RTM_LITTLE_ENDIAN
			return rtm::endianSwap(_value);
#else
			return _value;
#endif
		}
	};

	static inline uint8_t endianSwap(uint8_t _value)
	{
		return _value;
	}

	static inline uint16_t endianSwap(uint16_t _value)
	{
		return (_value >> 8) | ((_value & 0xff) << 8);
	}

	static inline uint32_t endianSwap(uint32_t _value)
	{
		return  (_value >> 24)				 |
				((_value << 8) & 0x00FF0000) |
				((_value >> 8) & 0x0000FF00) |
				(_value << 24);
	}

	static inline uint64_t endianSwap(uint64_t _value)
	{
		return  (_value >> 56) |
			((_value << 40) & 0x00FF000000000000LL) |
			((_value << 24) & 0x0000FF0000000000LL) |
			((_value <<  8) & 0x000000FF00000000LL) |
			((_value >>  8) & 0x00000000FF000000LL) |
			((_value >> 24) & 0x0000000000FF0000LL) |
			((_value >> 40) & 0x000000000000FF00LL) |
			 (_value << 56);
	}

	static inline float endianSwap(float _value)
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

} // namespace rtm

#endif  // RTM_RBASE_ENDIAN_H
