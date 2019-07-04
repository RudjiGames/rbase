//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_STRINGFN_H
#define RTM_RBASE_STRINGFN_H

#include <rbase/inc/platform.h>
#include <string.h> // memcpy

namespace rtm {

	typedef char (*fnChar)(char _ch);

	bool isInRange(char _ch, int _from, int _to);

	bool isSpace(char _ch);

	bool isAlpha(char _ch);

	bool isNumeric(char _ch);

	bool isAlphaNum(char _ch);

	bool isHexNum(char _ch);

	char toHexNum(char _ch);

	char fromHexNum(char _ch);

	char toNoop(char _ch);

	bool isUpper(char _ch);

	bool isLower(char _ch);

	char toLower(char _ch);

	char toUpper(char _ch);

	inline static uint32_t strLen(const char* _str, uint32_t _max = UINT32_MAX);

	inline static void strToUpper(char* _str, uint32_t _max = UINT32_MAX);

	inline static void strToLower(char* _str, uint32_t _max = UINT32_MAX);

	template<fnChar fn>
	inline static int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max = UINT32_MAX);

	template<fnChar fn>
	inline static const char* strStr(const char* _str, uint32_t _strMax, const char* _find, uint32_t _findMax = UINT32_MAX);

	inline static int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max = UINT32_MAX);

	inline static int32_t striCmp(const char* _lhs, const char* _rhs, uint32_t _max = UINT32_MAX);

	inline static uint32_t strlCpy(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num = UINT32_MAX);

	inline static uint32_t strlCat(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num = UINT32_MAX);

	inline static const char* strStr(const char* _str, const char* _find, uint32_t _max = UINT32_MAX);

	inline static const char* striStr(const char* _str, const char* _find, uint32_t _max = UINT32_MAX);

	//--------------------------------------------------------------------------

	inline bool isInRange(char _ch, int _from, int _to)
	{
		return unsigned(_ch - _from) <= unsigned(_to-_from);
	}

	inline bool isSpace(char _ch)
	{
		return	' '  == _ch ||
				'\t' == _ch ||
				'\n' == _ch ||
				'\v' == _ch ||
				'\f' == _ch ||
				'\r' == _ch;
	}

	inline bool isAlpha(char _ch)
	{
		return isLower(_ch) || isUpper(_ch);
	}

	inline bool isNumeric(char _ch)
	{
		return isInRange(_ch, '0', '9');
	}

	inline bool isAlphaNum(char _ch)
	{
		return isAlpha(_ch) || isNumeric(_ch);
	}

	inline bool isHexNum(char _ch)
	{
		return isInRange(toLower(_ch), 'a', 'f') || isNumeric(_ch);
	}

	inline char toHexNum(char _ch)
	{
		return "0123456789ABCDEF"[_ch & 0xf];
	}

	inline char fromHexNum(char _ch)
	{
		return _ch >= 'A' ? 10 + _ch - 'A' : _ch - '0';
	}
	
	inline char toNoop(char _ch)
	{
		return _ch;
	}

	inline bool isUpper(char _ch)
	{
		return isInRange(_ch, 'A', 'Z');
	}

	inline bool isLower(char _ch)
	{
		return isInRange(_ch, 'a', 'z');
	}

	inline char toLower(char _ch)
	{
		return _ch + (isUpper(_ch) ? 0x20 : 0);
	}

	inline char toUpper(char _ch)
	{
		return _ch - (isLower(_ch) ? 0x20 : 0);
	}

	inline static uint32_t strLen(const char* _str, uint32_t _max)
	{
		if (0 == _str)
			return 0;

		const char* ptr = _str;
		for (; 0 < _max && *ptr != '\0'; ++ptr, --_max) {};
		return uint32_t(ptr - _str);
	}

	inline static void strToUpper(char* _str, uint32_t _max)
	{
		while (*_str && _max--)
		{
			*_str = toUpper(*_str); ++_str;
		}
	}

	inline static void strToLower(char* _str, uint32_t _max)
	{
		while (*_str && _max--)
		{
			*_str = toLower(*_str); ++_str;
		}
	}

	template<fnChar fn>
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

	template<fnChar fn>
	inline static const char* strStr(const char* _str, uint32_t _strMax, const char* _find, uint32_t _findMax)
	{
		const char* ptr = _str;

		uint32_t       stringLen = strLen(_str,  _strMax);
		const uint32_t findLen   = strLen(_find, _findMax);

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

	template<fnChar fn>
	inline static const char* strChr(const char* _str, uint32_t _strMax, char _find)
	{
		uint32_t stringLen = strLen(_str, _strMax);
		for (uint32_t i=0; i<stringLen; ++i)
			if (fn(_str[i]) == _find)
				return &_str[i];
		return 0;
	}

	template<fnChar fn>
	inline static const char* strrChr(const char* _str, uint32_t _strMax, char _find)
	{
		int32_t stringLen = static_cast<int32_t>(strLen(_str, _strMax));
		for (int32_t i=stringLen-1; i>=0; --i)
			if (fn(_str[i]) == _find)
				return &_str[i];
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

	inline static uint32_t strlCpy(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num)
	{
		RTM_ASSERT(0 != _dst, "_dst can't be 0!");
		RTM_ASSERT(0 != _src, "_src can't be 0!");
		RTM_ASSERT(0 < _dstSize, "_dstSize can't be 0!");

		const uint32_t len = strLen(_src, _num);
		const uint32_t max = _dstSize-1;
		const uint32_t num = (len < max ? len : max);
		memcpy(_dst, _src, num);
		_dst[num] = '\0';

		return num;
	}

	inline static uint32_t strlCat(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num)
	{
		uint32_t len = strLen(_dst);
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

	inline static const char* strChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strChr<toNoop>(_str, _max, _find);
	}

	inline static const char* striChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strChr<toLower>(_str, _max, _find);
	}

	inline static const char* strrChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strrChr<toNoop>(_str, _max, _find);
	}

	inline static const char* strriChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strrChr<toLower>(_str, _max, _find);
	}

} // namespace rtm

#endif // RTM_RBASE_STRINGFN_H
