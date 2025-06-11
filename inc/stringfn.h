//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_STRINGFN_H
#define RTM_RBASE_STRINGFN_H

#include <rbase/inc/platform.h>

namespace rtm {

	/// Sets all bytes in a buffer to the given value
	///
	/// @param[in] _dst: Destination buffer
	/// @param[in] _val: Value to set
	/// @param[in] _numBytes: Numer of bytes to set
	static inline void memSet(void* _dst, uint8_t _val, int64_t _numBytes);

	/// Copies a memory buffer
	///
	/// @param[in] _dst: Destination buffer
	/// @param[in] _dstSize: Size of the destination buffer
	/// @param[in] _src: Source buffer
	/// @param[in] _numBytes: Size of the source buffer
	static inline void memCopy(void* _dst, uint32_t _dstSize, const void* _src, int64_t _numBytes);

	/// Moves a memory buffer
	///
	/// @param[in] _dst: Destination address
	/// @param[in] _src: Source address
	/// @param[in] _size: Size, in bytes, to move
	static inline void memMove(void* _dst, const void* _src, int64_t _size);

	/// Checks if character is in a given range, including endpoints
	///
	/// @param[in] _ch: Character to check
	/// @param[in] _from: Range start
	/// @param[in] _to: Range end
	///
	/// @returns true if character is in range
	static inline int32_t memCompare(void* _dst, const void* _src, int64_t _numBytes);

	/// Checks if character is in a given range, including endpoints
	///
	/// @param[in] _ch: Character to check
	/// @param[in] _from: Range start
	/// @param[in] _to: Range end
	///
	/// @returns true if character is in range
	static inline bool charIsInRange(char _ch, int _from, int _to);

	/// Checks if character is a space (any of \' tnvfr' chars)
	///
	/// @param[in] _ch: Character to check
	///
	/// @returns true if character is space
	static inline bool charIsSpace(char _ch);

	/// Checks if character is an uppercase or lowercase letter
	///
	/// @param[in] _ch: Character to check
	///
	/// @returns true if character is a letter
	static inline bool charIsAlpha(char _ch);

	/// Checks if character is any between '0' and '9', including them
	///
	/// @param[in] _ch: Character to check
	///
	/// @returns true if character is a number
	static inline bool charIsNumeric(char _ch);

	/// Checks if character is an uppercase or a lowercase letter or a number
	///
	/// @param[in] _ch: Character to check
	///
	/// @returns true if character is a letter or a number
	static inline bool charIsAlphaNumeric(char _ch);

	/// Checks if character is a hex digit (0-9 or a-f)
	///
	/// @param[in] _ch: Character to check
	///
	/// @returns true if character is a hex digit
	static inline bool charIsHexNum(char _ch);

	/// Converts a number between 0 and 15, including, to a hex digit
	///
	/// @param[in] _ch: Character to convert
	///
	/// @returns resulting hex digit
	static inline char charToHexNum(char _ch);

	/// Converts from a hex digit to a number between 0 and 15, including
	///
	/// @param[in] _ch: Hex digit to convert
	///
	/// @returns resulting number
	static inline char charFromHexNum(char _ch);

	/// Noop
	///
	/// @param[in] _ch: Character to not process
	///
	/// @returns original character
	static inline char charNoop(char _ch);

	/// Checks if character is upper case
	///
	/// @param[in] _ch: Character to check
	///
	/// @returns true if character is upper case
	static inline bool charIsUpper(char _ch);

	/// Checks if character is lower case
	///
	/// @param[in] _ch: Character to check
	///
	/// @returns true if character is lower case
	static inline bool charIsLower(char _ch);

	/// Converts a character to upper case
	///
	/// @param[in] _ch: Character to convert
	///
	/// @returns uppercase character
	static inline char charToUpper(char _ch);

	/// Converts a character to lowercase case
	///
	/// @param[in] _ch: Character to convert
	///
	/// @returns lowercase character
	static inline char charToLower(char _ch);

	/// Returns length of the string
	///
	/// @param[in] _str: String to find length of
	/// @param[in] _max: Maximum characters to check
	///
	/// @returns length of the string
	static inline uint32_t strLen(const char* _str, uint32_t _max = UINT32_MAX);

	/// Converts string to uppercase
	///
	/// @param[in] _str: String to convert
	/// @param[in] _max: Maximum characters to convert
	static inline void strToUpper(char* _str, uint32_t _max = UINT32_MAX);

	/// Converts string to lowercase
	///
	/// @param[in] _str: String to convert
	/// @param[in] _max: Maximum characters to convert
	static inline void strToLower(char* _str, uint32_t _max = UINT32_MAX);

	/// 
	typedef char (*fnChar)(char _ch);

	/// Compares two strings
	///
	/// @param[in] _lhs: Left hand string
	/// @param[in] _rhs: Right hand string
	/// @param[in] _max: Maximum characters to compare
	///
	/// @returns string compare result, delta between first different characters
	template<fnChar fn>
	static inline int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max = UINT32_MAX);

	/// Searches for a string within a string
	///
	/// @param[in] _str: String to search in
	/// @param[in] _strMax: Maximum characters to search in
	/// @param[in] _find: String to search for
	/// @param[in] _findMax: Maximum characters to search for
	///
	/// @returns pointer to found occurence or nullptr if not found
	template<fnChar fn>
	static inline const char* strStr(const char* _str, uint32_t _strMax, const char* _find, uint32_t _findMax = UINT32_MAX);

	/// Compares two strings, case sensitive
	///
	/// @param[in] _lhs: Left hand string
	/// @param[in] _rhs: Right hand string
	/// @param[in] _max: Maximum characters to compare
	///
	/// @returns string compare result, delta between first different characters
	static inline int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max = UINT32_MAX);

	/// Compares two strings, case insensitive
	///
	/// @param[in] _lhs: Left hand string
	/// @param[in] _rhs: Right hand string
	/// @param[in] _max: Maximum characters to compare
	///
	/// @returns string compare result, delta between first different characters
	static inline int32_t striCmp(const char* _lhs, const char* _rhs, uint32_t _max = UINT32_MAX);

	/// Copies a string
	///
	/// @param[in] _dst: Destination string buffer
	/// @param[in] _dstSize: Destination string buffer size
	/// @param[in] _src: Source string
	/// @param[in] _num: Number of characters to copy
	///
	/// @returns the number of copied characters
	static inline uint32_t strlCpy(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num = UINT32_MAX);

	/// Concatenates a string
	///
	/// @param[in] _dst: Destination string buffer
	/// @param[in] _dstSize: Destination string buffer size
	/// @param[in] _src: Source string
	/// @param[in] _num: Number of characters to concatenate
	///
	/// @returns the number of characters that were concatenated
	static inline uint32_t strlCat(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num = UINT32_MAX);

	/// Searches for a string within a string, case sensitive
	///
	/// @param[in] _str: String to search in
	/// @param[in] _strMax: Maximum characters to search in
	/// @param[in] _find: String to search for
	/// @param[in] _findMax: Maximum characters to search for
	///
	/// @returns pointer to found occurence or nullptr if not found
	static inline const char* strStr(const char* _str, const char* _find, uint32_t _max = UINT32_MAX);

	/// Searches for a string within a string, case insensitive
	///
	/// @param[in] _str: String to search in
	/// @param[in] _strMax: Maximum characters to search in
	/// @param[in] _find: String to search for
	/// @param[in] _findMax: Maximum characters to search for
	///
	/// @returns pointer to found occurence or nullptr if not found
	static inline const char* striStr(const char* _str, const char* _find, uint32_t _max = UINT32_MAX);

	/// Replaces all string occurences within a string with another string
	///
	/// @param[in] _srcBuffer: Source string
	/// @param[in] _dstBuffer: Destination buffer
	/// @param[in] _dstSize: Destination buffer size
	/// @param[in] _token: Token to replace
	/// @param[in] _newToken: Token to replace with
	static inline void strReplace(const char* _srcBuffer, char* _dstBuffer, uint32_t _dstSize, const char* _token, const char* _newToken);

} // namespace rtm

/// ---------------------------------------------------------------------- ///
///  Implementation                                                        ///
/// ---------------------------------------------------------------------- ///

namespace rtm {

	static inline void memSet(void* _dst, uint8_t _val, int64_t _numBytes)
	{
		uint8_t* dst = (uint8_t*)_dst;
		while (_numBytes--)
		{
			*dst++ = _val;
		}
	}

	static inline void memCopy(void* _dst, uint32_t _dstSize, const void* _src, int64_t _numBytes)
	{
		RTM_ASSERT(_dstSize >= _numBytes, "");
		_numBytes = _numBytes > _dstSize ? _dstSize : _numBytes;
		uint8_t* dst = (uint8_t*)_dst;
		const uint8_t* end = dst + _numBytes;
		const uint8_t* src = (const uint8_t*)_src;
		while (dst != end)
		{
			*dst++ = *src++;
		}
	}

	static inline void memMove(void* _dst, const void* _src, int64_t _size)
	{
		memCopy(_dst, (uint32_t)_size, _src, _size);
	}

	static inline int32_t memCompare(const void* _tgt, const void* _src, int64_t _numBytes)
	{
		uint8_t* tgt = (uint8_t*)_tgt;
		uint8_t* src = (uint8_t*)_src;
		while (*tgt == *src)
		{
			++tgt;
			++src;
			--_numBytes;
		}
		return _numBytes == 0 ? 0 : *tgt - *src;
	}

	static inline bool charIsInRange(char _ch, int _from, int _to)
	{
		return unsigned(_ch - _from) <= unsigned(_to-_from);
	}

	static inline bool charIsSpace(char _ch)
	{
		return	' '  == _ch ||
				'\t' == _ch ||
				'\n' == _ch ||
				'\v' == _ch ||
				'\f' == _ch ||
				'\r' == _ch;
	}

	static inline bool charIsAlpha(char _ch)
	{
		return charIsLower(_ch) || charIsUpper(_ch);
	}

	static inline bool charIsNumeric(char _ch)
	{
		return charIsInRange(_ch, '0', '9');
	}

	static inline bool charIsAlphaNumeric(char _ch)
	{
		return charIsAlpha(_ch) || charIsNumeric(_ch);
	}

	static inline bool charIsHexNum(char _ch)
	{
		return charIsInRange(charToLower(_ch), 'a', 'f') || charIsNumeric(_ch);
	}

	static inline char charToHexNum(char _ch)
	{
		return "0123456789ABCDEF"[_ch & 0xf];
	}

	static inline char charFromHexNum(char _ch)
	{
		return _ch >= 'A' ? 10 + _ch - 'A' : _ch - '0';
	}
	
	static inline char charNoop(char _ch)
	{
		return _ch;
	}

	static inline bool charIsUpper(char _ch)
	{
		return charIsInRange(_ch, 'A', 'Z');
	}

	static inline bool charIsLower(char _ch)
	{
		return charIsInRange(_ch, 'a', 'z');
	}

	static inline char charToUpper(char _ch)
	{
		return _ch - (charIsLower(_ch) ? 0x20 : 0);
	}

	static inline char charToLower(char _ch)
	{
		return _ch + (charIsUpper(_ch) ? 0x20 : 0);
	}

	static inline uint32_t strLen(const char* _str, uint32_t _max)
	{
		if (0 == _str)
			return 0;

		const char* ptr = _str;
		for (; 0 < _max && *ptr != '\0'; ++ptr, --_max) {};
		return uint32_t(ptr - _str);
	}

	static inline void strToUpper(char* _str, uint32_t _max)
	{
		while (*_str && _max--)
		{
			*_str = charToUpper(*_str); ++_str;
		}
	}

	static inline void strToLower(char* _str, uint32_t _max)
	{
		while (*_str && _max--)
		{
			*_str = charToLower(*_str); ++_str;
		}
	}

	template<fnChar fn>
	static inline int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max)
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
	static inline const char* strStr(const char* _str, uint32_t _strMax, const char* _find, uint32_t _findMax)
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
	static inline const char* strChr(const char* _str, uint32_t _strMax, char _find)
	{
		uint32_t stringLen = strLen(_str, _strMax);
		for (uint32_t i=0; i<stringLen; ++i)
		{
			if (fn(_str[i]) == _find)
			{
				return &_str[i];
			}
		}
		return 0;
	}

	template<fnChar fn>
	static inline const char* strrChr(const char* _str, uint32_t _strMax, char _find)
	{
		int32_t stringLen = static_cast<int32_t>(strLen(_str, _strMax));
		for (int32_t i=stringLen - 1; i >= 0; --i)
		{
			if (fn(_str[i]) == _find)
			{
				return &_str[i];
			}
		}
		return 0;
	}

	static inline int32_t strCmp(const char* _lhs, const char* _rhs, uint32_t _max)
	{
		return strCmp<charNoop>(_lhs, _rhs, _max);
	}

	static inline int32_t striCmp(const char* _lhs, const char* _rhs, uint32_t _max)
	{
		return strCmp<charToLower>(_lhs, _rhs, _max);
	}

	static inline uint32_t strlCpy(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num)
	{
		RTM_ASSERT(0 != _dst, "_dst can't be 0!");
		RTM_ASSERT(0 != _src, "_src can't be 0!");
		RTM_ASSERT(0 < _dstSize, "_dstSize can't be 0!");

		const uint32_t len = strLen(_src, _num);
		const uint32_t max = _dstSize-1;
		const uint32_t num = (len < max ? len : max);
		memCopy(_dst, _dstSize, _src, num);
		_dst[num] = '\0';

		return num;
	}

	static inline uint32_t strlCat(char* _dst, uint32_t _dstSize, const char* _src, uint32_t _num)
	{
		uint32_t len = strLen(_dst);
		_dstSize -= len;
		_dst += len;
		return strlCpy(_dst, _dstSize, _src, _num);
	}

	static inline const char* strStr(const char* _str, const char* _find, uint32_t _max)
	{
		return strStr<charNoop>(_str, _max, _find, INT32_MAX);
	}

	static inline const char* striStr(const char* _str, const char* _find, uint32_t _max)
	{
		return strStr<charToLower>(_str, _max, _find, INT32_MAX);
	}

	static inline void strReplace(const char* _srcBuffer, char* _dstBuffer, uint32_t _dstSize, const char* _token, const char* _newToken)
	{
		const char* srcToken = strStr(_srcBuffer, _token);
		if (!srcToken)
		{
			strlCpy(_dstBuffer, _dstSize, _srcBuffer);
			return;
		}
		strlCpy(_dstBuffer, _dstSize, _srcBuffer, (uint32_t)((uintptr_t)srcToken - (uintptr_t)_srcBuffer));
		strlCat(_dstBuffer, _dstSize, _newToken);
		strlCat(_dstBuffer, _dstSize, srcToken + strLen(_token));
	}

	static inline const char* strChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strChr<charNoop>(_str, _max, _find);
	}

	static inline const char* striChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strChr<charToLower>(_str, _max, _find);
	}

	static inline const char* strrChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strrChr<charNoop>(_str, _max, _find);
	}

	static inline const char* strriChr(const char* _str, char _find, uint32_t _max = UINT32_MAX)
	{
		return strrChr<charToLower>(_str, _max, _find);
	}

} // namespace rtm

#endif // RTM_RBASE_STRINGFN_H
