//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_HASH_H
#define RTM_RBASE_HASH_H

#include <rbase/inc/platform.h>

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//-----------------------------------------------------------------------------

#if RTM_COMPILER_MSVC
	#define ROTL32(x,y)	_rotl(x,y)
#else
	inline static uint32_t rotl32( uint32_t x, int8_t r )
	{
	  return (x << r) | (x >> (32 - r));
	}
	#define	ROTL32(x,y)	rotl32(x,y)
#endif // !defined(_MSC_VER)

namespace rtm {

	//--------------------------------------------------------------------------
	/// Murmur3 hashing
	//--------------------------------------------------------------------------
	static inline uint32_t hashMurmur3(const void* _key, uint32_t _len, uint32_t _seed = 0)
	{
		const uint8_t* data = (const uint8_t*)_key;
		const int nblocks = _len / 4;

		uint32_t h1 = _seed;

		uint32_t c1 = 0xcc9e2d51;
		uint32_t c2 = 0x1b873593;

		const uint32_t* blocks = (const uint32_t *)(data + nblocks*4);

		for (int i=-nblocks; i; i++)
		{
			uint32_t k1 = blocks[i];

			k1 *= c1;
			k1 = ROTL32(k1,15);
			k1 *= c2;
    
			h1 ^= k1;
			h1 = ROTL32(h1,13); 
			h1 = h1*5+0xe6546b64;
		}

		const uint8_t* tail = (const uint8_t*)(data + nblocks*4);

		uint32_t k1 = 0;

		switch (_len & 3)
		{
		case 3: k1 ^= tail[2] << 16;
		        /* fall through */
		case 2: k1 ^= tail[1] << 8;
		        /* fall through */
		case 1: k1 ^= tail[0];
				k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
		};

		h1 ^= _len;
		h1 ^= h1 >> 16;
		h1 *= 0x85ebca6b;
		h1 ^= h1 >> 13;
		h1 *= 0xc2b2ae35;
		h1 ^= h1 >> 16;

		return h1;
	} 

	uint32_t hashCity32(const void* _key, uint32_t _len);
	uint64_t hashCity64(const void* _key, uint32_t _len);
	uint64_t hashCity64(const void* _key, uint32_t _len, uint64_t _seed);

	//--------------------------------------------------------------------------
	/// Calculate a string hash, suitable for short strings
	//--------------------------------------------------------------------------
	static inline uint32_t hashStr(const char* _string, uint32_t _maxChars = UINT32_MAX)
	{
	   uint32_t	h;
	   uint8_t*	p = (uint8_t*)_string;

	   h = 0;
	   while ((*p != '\0') && (_maxChars--))
	   {
		  h = 37 * h + *p;
		  p++;
	   }
	   return h;
	}

	void md5_calculate(const void* _data, uint32_t _data_size, uint8_t _out_digest[16]);
	void md5_to_string(uint8_t _digest[16], char _out_hash[33]);

	namespace hash_private {

		template <typename S> struct fnv_internal;
		template <typename S> struct fnv1;
		template <typename S> struct fnv1a;

		template <> struct fnv_internal<uint32_t>
		{
			constexpr static uint32_t FNV1_OFFSET_BASIS	= 0x811C9DC5;
			constexpr static uint32_t FNV1_PRIME		= 0x01000193;
		};

	} // namespace hash

	template <> struct hash_private::fnv1<uint32_t> : public hash_private::fnv_internal<uint32_t>
	{
		constexpr static inline uint32_t hash(char const* _str, const uint32_t _val = FNV1_OFFSET_BASIS)
		{
			return (_str[0] == '\0') ? _val : hash(&_str[1], (_val * FNV1_PRIME) ^ uint32_t(_str[0]));
		}
	};

	template <> struct hash_private::fnv1a<uint32_t> : public hash_private::fnv_internal<uint32_t>
	{
		constexpr static inline uint32_t hash(char const* _str, const uint32_t _val = FNV1_OFFSET_BASIS)
		{
			return (_str[0] == '\0') ? _val : hash(&_str[1], (_val ^ uint32_t(_str[0])) * FNV1_PRIME);
		}
	};

} // namespace rtm

/// compile time string hash
#define RTM_COMPILE_TIME_STRING_HASH(_string)	rtm::hash_private::fnv1<uint32_t>::hash(_string)

#endif // RTM_RBASE_HASH_H
