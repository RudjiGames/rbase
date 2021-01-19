//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_STRINGVIEW_H
#define RTM_RBASE_STRINGVIEW_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringfn.h>
#include <string.h> // memcpy

#ifndef RBASE_NAMESPACE
	#define RTM_STRING_ALLOC	malloc
	#define RTM_STRING_REALLOC	realloc
	#define RTM_STRING_FREE		free
#else
	#define RTM_STRING_ALLOC	::RBASE_NAMESPACE::rtm_alloc
	#define RTM_STRING_REALLOC	::RBASE_NAMESPACE::rtm_realloc
	#define RTM_STRING_FREE		::RBASE_NAMESPACE::rtm_free
#endif

namespace rtm {

	// fwd declare

	class StringView;
	class String;
	template <uint32_t S>
	class StringTemp;

	// string view, doesn't own data

	class StringView
	{
		const char*	m_str;
		uint32_t	m_len;

	public:
		StringView();
		StringView(const StringView& _other);
		StringView(const String& _other);
		template <uint32_t S>
		StringView(const StringTemp<S>& _other);
		StringView(const char* _str, uint32_t _len = UINT32_MAX);
		StringView(const char* _start, const char* _end);

		StringView& operator = (const StringView& _other);
		StringView& operator = (const String& _other);
		template <uint32_t S>
		StringView& operator = (const StringTemp<S>& _other);
		StringView& operator = (const char* _str);

		void		clear();
		void		set(const char* _str, uint32_t _len = UINT32_MAX);
		void		set(const char* _start, const char* _end);
		bool		isNull() const;
		const char*	data() const;
		uint32_t	length() const;

		operator const char* ();
		char operator[](uint32_t _index) const;
	};

	static uint32_t strLen(const StringView& _view);
	template<fnChar fn>
	static int32_t strCmp(const StringView& _view, const char* _cmp);
	template<fnChar fn>
	static const char* strStr(const StringView& _view, uint32_t _strMax, const char* _find, uint32_t _findMax = UINT32_MAX);
	static int32_t strCmp(const StringView& _view, const char* _rhs, uint32_t _max = UINT32_MAX);
	static int32_t striCmp(const StringView& _view, const char* _rhs, uint32_t _max = UINT32_MAX);
	static const char* strStr(const StringView& _view, const char* _find, uint32_t _max = UINT32_MAX);
	static const char* striStr(const StringView& _view, const char* _find, uint32_t _max = UINT32_MAX);
	static const char* strChr(const StringView& _view, char _find, uint32_t _max = UINT32_MAX);
	static const char* striChr(const StringView& _view, char _find, uint32_t _max = UINT32_MAX);

	// dynamically allocated string

	class String
	{
		const char*	m_str;
		uint32_t	m_len;

	public:
		String();
		String(const StringView& _view);
		String(const String& _other);
		template <uint32_t S>
		String(const StringTemp<S>& _temp);
		String(const char* _str, uint32_t _len = UINT32_MAX);
		String(const char* _start, const char* _end);
		~String();

		String& operator = (const StringView& _other);
		String& operator = (const String& _other);
		template <uint32_t S>
		String& operator = (const StringTemp<S>& _other);
		String& operator = (const char* _str);

		void		clear();
		void		set(const char* _str, uint32_t _len = UINT32_MAX);
		void		set(const char* _start, const char* _end);
		void		append(StringView& _view);
		void		append(String& _string);
		template <uint32_t S>
		void		append(StringTemp<S>& _view);
		void		append(const char* _str, uint32_t _len = UINT32_MAX);
		void		append(const char* _start, const char* _end);
		bool		isNull() const;
		const char*	data() const;
		uint32_t	length() const;
		operator const char* ();
		operator StringView ();
		char operator[](uint32_t _index) const;
	};

	// dynamically allocated string for temporary use with on stack data up to 1024 chars

	template <uint32_t S_ON_STACK_SIZE = 1024>
	class StringTemp
	{
		uint32_t	m_len;
		uint32_t	m_capacity;
		char*		m_str;
		char		m_strData[S_ON_STACK_SIZE];

	public:
		StringTemp();
		StringTemp(const StringView& _other);
		StringTemp(const String& _other);
		StringTemp(const StringTemp& _other);
		StringTemp(const char* _str, uint32_t _len = UINT32_MAX);
		StringTemp(const char* _start, const char* _end);
		~StringTemp();

		StringTemp& operator = (const StringView& _view);
		StringTemp& operator = (const String& _other);
		StringTemp& operator = (const StringTemp& _str);
		StringTemp& operator = (const char* _str);

		StringTemp& operator += (const StringView& _view);
		StringTemp& operator += (const String& _other);
		StringTemp& operator += (const StringTemp& _str);
		StringTemp& operator += (const char* _str);
		StringTemp& operator += (char _c);

		void		clear();
		void		set(const char* _str, uint32_t _len = UINT32_MAX);
		void		set(const char* _start, const char* _end);
		void		append(StringView& _view);
		void		append(String& _string);
		void		append(StringTemp& _view);
		void		append(char _c);
		void		append(const char* _str, uint32_t _len = UINT32_MAX);
		void		append(const char* _start, const char* _end);
		bool		isNull() const;
		const char*	data() const;
		char*		data();
		uint32_t	length() const;
		void		setLength(uint32_t _len);
		uint32_t	capacity() const;
		operator const char* () const;
		char operator[](uint32_t _index) const;
		char& operator[](uint32_t _index);
	private:
		bool		isOnStack() const;
		void		expandCapacity(uint32_t _minCapacity);
	};

	//--------------------------------------------------------------------------

	inline StringView::StringView()
	{
		clear();
	}

	inline StringView::StringView(const StringView& _other)
	{
		set(_other.data(), _other.length());
	}

	inline StringView::StringView(const String& _other)
	{
		set(_other.data(), _other.length());
	}

	template <uint32_t S>
	inline StringView::StringView(const StringTemp<S>& _other)
	{
		set(_other.data(), _other.length());
	}

	inline StringView::StringView(const char* _str, uint32_t _len)
	{
		set(_str, _len);
	}

	inline StringView::StringView(const char* _start, const char* _end)
	{
		set(_start, _end);
	}

	inline StringView& StringView::operator = (const StringView& _other)
	{
		m_str = _other.m_str;
		m_len = _other.m_len;
		return *this;
	}

	inline StringView& StringView::operator = (const String& _other)
	{
		m_str = _other.data();
		m_len = _other.length();
		return *this;
	}

	template <uint32_t S>
	inline StringView& StringView::operator = (const StringTemp<S>& _other)
	{
		m_str = _other.data();
		m_len = _other.length();
		return *this;
	}

	inline StringView& StringView::operator = (const char* _str)
	{
		m_str = _str;
		m_len = strLen(_str);
		return *this;
	}
		
	inline void StringView::clear()
	{
		m_str = 0;
		m_len = 0;
	}

	inline void StringView::set(const char* _str, uint32_t _len)
	{
		m_str = _str;
		m_len = _len == UINT32_MAX ? strLen(_str) : _len;
	}

	inline void StringView::set(const char* _start, const char* _end)
	{
		RTM_ASSERT(_end > _start, "");
		m_str = _start;
		m_len = (uint32_t)(_end - _start);
	}

	inline bool StringView::isNull() const
	{
		return m_len == 0;
	}

	inline const char* StringView::data() const
	{
		return m_str;
	}

	inline uint32_t StringView::length() const
	{
		return m_len;
	}

	inline StringView::operator const char* ()
	{
		return data();
	}

	inline char StringView::operator[](uint32_t _index) const 
	{
		RTM_ASSERT(_index < m_len, "");
		return m_str[_index];
	}

	//--------------------------------------------------------------------------

	inline static uint32_t strLen(const StringView& _view)
	{
		return _view.length();
	}

	template<fnChar fn>
	inline static int32_t strCmp(const StringView& _view, const char* _cmp)
	{
		return strCmp<fn>(_view.data(), _cmp, _view.length());;
	}

	template<fnChar fn>
	inline static const char* strStr(const StringView& _view, uint32_t _strMax, const char* _find, uint32_t _findMax)
	{
		_strMax = _strMax > _view.length() ? _view.length() : _strMax;
		return strStr<fn>(_view.data(), _strMax, _find, _findMax);
	}

	inline static int32_t strCmp(const StringView& _view, const char* _rhs, uint32_t _max)
	{
		if (!_view.length() && _max) return -1;
		_max = _max > _view.length() ? _view.length() : _max;
		return strCmp(_view.data(), _rhs, _max);
	}

	inline static int32_t striCmp(const StringView& _view, const char* _rhs, uint32_t _max)
	{
		if (!_view.length() && _max) return -1;
		_max = _max > _view.length() ? _view.length() : _max;
		return striCmp(_view.data(), _rhs, _max);
	}

	inline static const char* strStr(const StringView& _view, const char* _find, uint32_t _max)
	{
		if (!_view.length() && _max) return 0;
		_max = _max > _view.length() ? _view.length() : _max;
		return strStr(_view.data(), _find, _max);
	}

	inline static const char* striStr(const StringView& _view, const char* _find, uint32_t _max)
	{
		if (!_view.length() && _max) return 0;
		_max = _max > _view.length() ? _view.length() : _max;
		return striStr(_view.data(), _find, _max);
	}

	inline static const char* strChr(const StringView& _view, char _find, uint32_t _max)
	{
		if (!_view.length() && _max) return 0;
		_max = _max > _view.length() ? _view.length() : _max;
		return strChr(_view.data(), _find, _max);
	}

	inline static const char* striChr(const StringView& _view, char _find, uint32_t _max)
	{
		if (!_view.length() && _max) return 0;
		_max = _max > _view.length() ? _view.length() : _max;
		return striChr(_view.data(), _find, _max);
	}

	//--------------------------------------------------------------------------

	inline String::String()
		: m_str(0)
		, m_len(0)
	{
	}

	inline String::String(const StringView& _view)
		: m_str(0) , m_len(0)
	{
		set(_view.data(), _view.length());
	}

	inline String::String(const String& _other)
		: m_str(0) , m_len(0)
	{
		set(_other.data(), _other.length());
	}

	template <uint32_t S>
	inline String::String(const StringTemp<S>& _temp)
		: m_str(0) , m_len(0)
	{
		set(_temp.data(), _temp.length());
	}

	inline String::String(const char* _str, uint32_t _len)
		: m_str(0) , m_len(0)
	{
		set(_str, _len);
	}

	inline String::String(const char* _start, const char* _end)
		: m_str(0) , m_len(0)
	{
		set(_start, _end);
	}

	inline String::~String()
	{
		clear();
	}

	inline String& String::operator = (const String& _other)
	{
		set(_other.data(), _other.length());
		return *this;
	}

	template <uint32_t S>
	inline String& String::operator = (const StringTemp<S>& _temp)
	{
		set(_temp.data(), _temp.length());
		return *this;
	}

	inline String& String::operator = (const StringView& _view)
	{
		set(_view.data(), _view.length());
		return *this;
	}

	inline String& String::operator = (const char* _str)
	{
		if (!_str)
		{
			clear();
			return *this;
		}

		set(_str);
		return *this;
	}

	inline void String::clear()
	{
		if (m_len != 0)
		{
			RTM_STRING_FREE((void*)m_str);
		}
		m_str = 0;
		m_len = 0;
	}

	inline void String::set(const char* _str, uint32_t _len)
	{
		clear();
		if (_str == 0)
			return;

		m_len = _len == UINT32_MAX ? strLen(_str) : _len;
		char* str = (char*)RTM_STRING_ALLOC(sizeof(char) * (m_len+1));
		strlCpy(str, m_len+1, _str, m_len);
		str[m_len] = 0;
		m_str = str;
	}

	inline void String::set(const char* _start, const char* _end)
	{
		RTM_ASSERT(_end > _start, "");
		set(_start, (uint32_t)(_end - _start));
	}

	inline void String::append(String& _string)
	{
		append(_string.data(), _string.length());
	}

	template <uint32_t S>
	inline void String::append(StringTemp<S>& _temp)
	{
		append(_temp.data(), _temp.length());
	}

	inline void String::append(StringView& _view)
	{
		append(_view.data(), _view.length());
	}

	inline void String::append(const char* _str, uint32_t _len)
	{
		uint32_t aLen = _len == UINT32_MAX ? strLen(_str) : _len;
		m_str = (const char*)RTM_STRING_REALLOC((void*)m_str, sizeof(char) * (m_len + aLen));
		strlCpy((char*)&m_str[m_len], aLen, _str, aLen);
		m_len += aLen;
	}

	inline void String::append(const char* _start, const char* _end)
	{
		append(_start, (uint32_t)(_end - _start));
	}

	inline bool String::isNull() const
	{
		return m_len == 0;
	}

	inline const char*	String::data() const
	{
		return m_str;
	}

	inline uint32_t String::length() const
	{
		return m_len;
	}

	inline String::operator const char* ()
	{
		return data();
	}

	inline String::operator StringView ()
	{
		return StringView(data(), length());
	}

	inline char String::operator[](uint32_t _index) const
	{
		RTM_ASSERT(_index < m_len, "");
		return m_str[_index];
	}

	//--------------------------------------------------------------------------

	template <uint32_t S>
	inline StringTemp<S>::StringTemp()
		: m_len(0), m_capacity(S), m_str(0)
	{
	}

	template <uint32_t S>
	inline StringTemp<S>::StringTemp(const StringView& _other)
		: m_len(0), m_capacity(S), m_str(0)
	{
		set(_other.data(), _other.length());
	}

	template <uint32_t S>
	inline StringTemp<S>::StringTemp(const String& _other)
		: m_len(0), m_capacity(S), m_str(0)
	{
		set(_other.data(), _other.length());
	}

	template <uint32_t S>
	inline StringTemp<S>::StringTemp(const StringTemp& _other)
		: m_len(0), m_capacity(S), m_str(0)
	{
		set(_other.data(), _other.length());
	}

	template <uint32_t S>
	inline StringTemp<S>::StringTemp(const char* _str, uint32_t _len)
		: m_len(0), m_capacity(S), m_str(0)
	{
		set(_str, _len);
	}

	template <uint32_t S>
	inline StringTemp<S>::StringTemp(const char* _start, const char* _end)
		: m_len(0), m_capacity(S), m_str(0)
	{
		set(_start, uint32_t(_end - _start));
	}

	template <uint32_t S>
	inline StringTemp<S>::~StringTemp()
	{
		clear();
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator = (const StringView& _view)
	{
		set(_view.data(), _view.length());
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator = (const String& _str)
	{
		set(_str.data(), _str.length());
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator = (const StringTemp& _other)
	{
		set(_other.data(), _other.length());
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator = (const char* _str)
	{
		set(_str, strLen(_str));
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator += (const StringView& _view)
	{
		append(_view);
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator += (const String& _other)
	{
		append(_other);
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator += (const StringTemp& _str)
	{
		append(_str);
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator += (const char* _str)
	{
		append(_str);
		return *this;
	}

	template <uint32_t S>
	inline StringTemp<S>& StringTemp<S>::operator += (char _c)
	{
		append(_c);
		return *this;
	}

	template <uint32_t S>
	inline void StringTemp<S>::clear()
	{
		if (!isOnStack())
		{
			RTM_STRING_FREE((void*)m_str);
		}
		m_str		= &m_strData[0];
		m_len		= 0;
		m_capacity	= S;
	}

	template <uint32_t S>
	inline void StringTemp<S>::set(const char* _str, uint32_t _len)
	{
		uint32_t length = _len == UINT32_MAX ? rtm::strLen(_str) : _len;
		set(_str, _str + length);
	}

	template <uint32_t S>
	inline void StringTemp<S>::set(const char* _start, const char* _end)
	{
		uint32_t len = (uint32_t)(_end - _start);

		clear();

		if (len < S)
			m_str = &m_strData[0];
		else
		{
			if (len + 1 > m_capacity)
			{
				m_str		= (char*)RTM_STRING_ALLOC(sizeof(char) * (len + 1));
				m_capacity	= len + 1;
			}
		}

		rtm::strlCpy(m_str, len+1, _start, len);
		m_str[len]	= 0;
		m_len		= len;
	}

	template <uint32_t S>
	inline void StringTemp<S>::append(StringView& _view)
	{
		append(_view.data(), _view.length());
	}

	template <uint32_t S>
	inline void StringTemp<S>::append(String& _string)
	{
		append(_string.data(), _string.length());
	}

	template <uint32_t S>
	inline void StringTemp<S>::append(StringTemp& _view)
	{
		append(_view.data(), _view.length());
	}

	template <uint32_t S>
	inline void StringTemp<S>::append(char _c)
	{
		append(&_c, 1);
	}

	template <uint32_t S>
	inline void StringTemp<S>::append(const char* _str, uint32_t _len)
	{
		_len = _len == UINT32_MAX ? strLen(_str) : _len;
		uint32_t newLen = m_len + _len;

		if (isOnStack() && (newLen < S))
		{
			rtm::strlCpy(&m_str[m_len], S, _str, _len);
			m_len = newLen;
			return;
		}

		if (isOnStack())
		{
			// allocate, copy, append
			m_capacity = newLen + 1;
			char* newStr = (char*)RTM_STRING_ALLOC(m_capacity);
			if (m_len)
				rtm::strlCpy(newStr, m_len, m_str);
			rtm::strlCpy(&newStr[m_len], m_capacity, _str, _len);
			m_str		= newStr;
		}
		else
		{
			// reallocate, append
			if (newLen + 1 > m_capacity)
				expandCapacity(newLen + 1);
			rtm::strlCpy(&m_str[m_len], m_capacity - m_len, _str, _len);
		}

		m_len			= newLen;
		m_str[m_len]	= 0;
	}

	template <uint32_t S>
	inline void StringTemp<S>::append(const char* _start, const char* _end)
	{
		append(_start, (uint32_t)(_end - _start));
	}

	template <uint32_t S>
	inline bool StringTemp<S>::isNull() const
	{
		return m_len == 0;
	}

	template <uint32_t S>
	inline const char* StringTemp<S>::data() const
	{
		return m_str;
	}

	template <uint32_t S>
	inline char* StringTemp<S>::data()
	{
		return m_str;
	}

	template <uint32_t S>
	inline uint32_t	StringTemp<S>::length() const
	{
		return m_len;
	}

	template <uint32_t S>
	inline void	StringTemp<S>::setLength(uint32_t _len)
	{
		m_len = _len;
	}

	template <uint32_t S>
	inline uint32_t	StringTemp<S>::capacity() const
	{
		return m_capacity;
	}

	template <uint32_t S>
	inline StringTemp<S>::operator const char* () const
	{
		return m_str;
	}

	template <uint32_t S>
	inline char StringTemp<S>::operator[](uint32_t _index) const
	{
		RTM_ASSERT(_index < m_len, "");
		return m_str[_index];
	}

	template <uint32_t S>
	inline char& StringTemp<S>::operator[](uint32_t _index)
	{
		RTM_ASSERT(_index < m_len, "");
		return m_str[_index];
	}

	template <uint32_t S>
	inline bool StringTemp<S>::isOnStack() const
	{
		return m_str == &m_strData[0];
	}

	template <uint32_t S>
	inline void StringTemp<S>::expandCapacity(uint32_t _minCapacity)
	{
		RTM_ASSERT(!isOnStack(), "");
		m_capacity = m_capacity * 2;
		if (_minCapacity > m_capacity)
			m_capacity = _minCapacity;
		m_str = (char*)RTM_STRING_REALLOC(m_str, m_capacity);
	}

} // namespace rtm

#endif // RTM_RBASE_STRINGVIEW_H
