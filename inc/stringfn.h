//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_STRINGFN_H
#define RTM_RBASE_STRINGFN_H

#include <rbase/inc/platform.h>
#include <string.h> // memcpy

namespace rtm {

	typedef char (*CharFn)(char _ch);

	char toNoop(char _ch);

	bool isUpper(char _ch);

	bool isLower(char _ch);

	char toLower(char _ch);

	char toUpper(char _ch);

	inline static int32_t strLen(const char* _str, uint32_t _max = -1);

	inline static void strToUpper(char* _str);

	inline static void strToLower(char* _str);

	template<CharFn fn>
	inline static int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max = -1);

	template<CharFn fn>
	inline static const char* strStr(const char* _str, uint32_t _strMax, const char* _find, uint32_t _findMax = -1);

	inline static int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max = -1);

	inline static int32_t striCmp(const char* _lhs, const char* _rhs, uint32_t _max = -1);

	inline static int32_t strlCpy(char* _dst, int32_t _dstSize, const char* _src, uint32_t _num = -1);

	inline static int32_t strlCat(char* _dst, int32_t _dstSize, const char* _src, uint32_t _num = -1);

	inline static const char* strStr(const char* _str, const char* _find, uint32_t _max = -1);

	inline static const char* striStr(const char* _str, const char* _find, uint32_t _max = -1);

	inline static int32_t strLen(const char* _str, uint32_t _max)
	{
		if (0 == _str)
			return 0;

		const char* ptr = _str;
		for (; 0 < _max && *ptr != '\0'; ++ptr, --_max) {};
		return int32_t(ptr - _str);
	}

	inline static void strToUpper(char* _str)
	{
		while (*_str) { *_str = toUpper(*_str); ++_str; }
	}

	inline static void strToLower(char* _str)
	{
		while (*_str) { *_str = toLower(*_str); ++_str; }
	}

	template<CharFn fn>
	inline static int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max)
	{
		for (
			; 0 < _max && fn(*_lhs) == fn(*_rhs)
			; ++_lhs, ++_rhs, --_max
			)
		{
			if (*_lhs == '\0'
			||  *_rhs == '\0')
			{
				break;
			}
		}

		return 0 == _max ? 0 : fn(*_lhs) - fn(*_rhs);
	}

	template<CharFn fn>
	inline static const char* strStr(const char* _str, uint32_t _strMax, const char* _find, uint32_t _findMax)
	{
		const char* ptr = _str;

		int32_t       stringLen = strLen(_str,  _strMax);
		const int32_t findLen   = strLen(_find, _findMax);

		for (; stringLen >= findLen; ++ptr, --stringLen)
		{
			// Find start of the string.
			while (fn(*ptr) != fn(*_find) )
			{
				++ptr;
				--stringLen;

				// Search pattern lenght can't be longer than the string.
				if (findLen > stringLen)
				{
					return 0;
				}
			}

			// Set pointers.
			const char* string = ptr;
			const char* search = _find;

			// Start comparing.
			while (fn(*string++) == fn(*search++) )
			{
				// If end of the 'search' string is reached, all characters match.
				if ('\0' == *search)
				{
					return ptr;
				}
			}
		}

		return 0;
	}

	inline static int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max)
	{
		return strCmp<toNoop>(_lhs, _rhs, _max);
	}

	inline static int32_t striCmp(const char* _lhs, const char* _rhs, uint32_t _max)
	{
		return strCmp<toLower>(_lhs, _rhs, _max);
	}

	inline static int32_t strlCpy(char* _dst, int32_t _dstSize, const char* _src, uint32_t _num)
	{
		RTM_ASSERT(0 != _dst, "_dst can't be 0!");
		RTM_ASSERT(0 != _src, "_src can't be 0!");
		RTM_ASSERT(0 < _dstSize, "_dstSize can't be 0!");

		const int32_t len = strLen(_src, _num);
		const int32_t max = _dstSize-1;
		const int32_t num = (len < max ? len : max);
		memcpy(_dst, _src, num);
		_dst[num] = '\0';

		return num;
	}

	inline static int32_t strlCat(char* _dst, int32_t _dstSize, const char* _src, uint32_t _num)
	{
		int32_t len = strLen(_dst, _num);
		_dstSize -= len;
		_dst += len;
		return strlCpy(_dst, _dstSize, _src, _num);
	}

	inline static const char* strStr(const char* _str, const char* _find, uint32_t _max)
	{
		return strStr<toNoop>(_str, _max, _find, INT32_MAX);
	}

	inline static const char* striStr(const char* _str, const char* _find, uint32_t _max)
	{
		return strStr<toLower>(_str, _max, _find, INT32_MAX);
	}

} // namespace rtm

#endif // RTM_RBASE_STRINGFN_H
