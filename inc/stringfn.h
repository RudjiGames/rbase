//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_STRINGFN_H
#define RTM_RBASE_STRINGFN_H

#include <rbase/inc/platform.h>
#include <string.h>

namespace rtm {

	typedef char (*CharFn)(char _ch);

	char toNoop(char _ch);
	bool isUpper(char _ch);
	bool isLower(char _ch);
	char toLower(char _ch);
	char toUpper(char _ch);

	inline static int32_t strnlen(const char* _str, uint32_t _max = -1)
	{
		if (NULL == _str)
		{
			return 0;
		}

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
	inline static int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max = -1)
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
	inline static const char* strStr(const char* _str, uint32_t _strMax, const char* _find, uint32_t _findMax = -1)
	{
		const char* ptr = _str;

		int32_t       stringLen = strnlen(_str,  _strMax);
		const int32_t findLen   = strnlen(_find, _findMax);

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
					return NULL;
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

		return NULL;
	}


	inline static int32_t strncmp(const char* _lhs, const char* _rhs, uint32_t _max = -1)
	{
		return strCmp<toNoop>(_lhs, _rhs, _max);
	}

	inline static int32_t strincmp(const char* _lhs, const char* _rhs, uint32_t _max = -1)
	{
		return strCmp<toLower>(_lhs, _rhs, _max);
	}

	inline static int32_t strlncpy(char* _dst, int32_t _dstSize, const char* _src, uint32_t _num = -1)
	{
		RTM_ASSERT(NULL != _dst, "_dst can't be NULL!");
		RTM_ASSERT(NULL != _src, "_src can't be NULL!");
		RTM_ASSERT(0 < _dstSize, "_dstSize can't be 0!");

		const int32_t len = strnlen(_src, _num);
		const int32_t max = _dstSize-1;
		const int32_t num = (len < max ? len : max);
		memcpy(_dst, _src, num);
		_dst[num] = '\0';

		return num;
	}

	inline static int32_t strlncat(char* _dst, int32_t _dstSize, const char* _src, uint32_t _num = -1)
	{
		int32_t len = strnlen(_dst, _num);
		_dstSize -= len;
		_dst += len;
		return strlncpy(_dst, _dstSize, _src, _num);
	}

	inline static const char* strnstr(const char* _str, const char* _find, uint32_t _max = -1)
	{
		return strStr<toNoop>(_str, _max, _find, INT32_MAX);
	}

	inline static const char* stristr(const char* _str, const char* _find, uint32_t _max = -1)
	{
		return strStr<toLower>(_str, _max, _find, INT32_MAX);
	}

	char *strdup(const char* _str);

} // namespace rtm

#endif // RTM_RBASE_STRINGFN_H

