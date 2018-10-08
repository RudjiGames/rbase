//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_STRINGVIEW_H
#define RTM_RBASE_STRINGVIEW_H

#include <rbase/inc/platform.h>
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

	class StringView
	{
		const char*	m_str;
		uint32_t	m_len;

	public:
		StringView();
		StringView(const char* _str, uint32_t _len = -1);
		StringView(const char* _start, const char* _end);

		StringView& operator = (const char* _str);
		StringView& operator = (const StringView& _other);

		void		clear();
		void		set(const char* _str, uint32_t _len = -1);
		void		set(const char* _start, const char* _end);
		bool		isNull() const;
		const char*	data() const;
		uint32_t	length() const;

		operator const char* ();
		char operator[](uint32_t _index);
	};

	class String
	{
		const char*	m_str;
		uint32_t	m_len;

	public:
		String();
		String(StringView& _view);
		String(const char* _str, uint32_t _len = -1);
		String(const char* _start, const char* _end);
		~String();

		String& operator = (const char* _str);		String& operator = (const String& _other);		String& operator = (const StringView& _other);
		void		clear();
		void		set(const char* _str, uint32_t _len = -1);
		void		set(const char* _start, const char* _end);
		void		set(String& _string);
		void		set(StringView& _view);
		void		append(const char* _str, uint32_t _len = -1);
		void		append(const char* _start, const char* _end);
		void		append(String& _string);
		void		append(StringView& _view);
		bool		isNull() const;
		const char*	data() const;
		uint32_t	length() const;
		operator const char* ();
		char operator[](uint32_t _index);
	};

	//--------------------------------------------------------------------------

	inline StringView::StringView()
	{
		clear();
	}

	inline StringView::StringView(const char* _str, uint32_t _len)
	{
		set(_str, _len);
	}

	inline StringView::StringView(const char* _start, const char* _end)
	{
		set(_start, _end);
	}

	inline StringView& StringView::operator = (const char* _str)
	{
		m_str = _str;
		m_len = strLen(_str);
		return *this;
	}

	inline StringView& StringView::operator = (const StringView& _other)
	{
		m_str = _other.m_str;
		m_len = _other.m_len;
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
		m_len = _len == -1 ? strLen(_str) : _len;
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

	inline char StringView::operator[](uint32_t _index)
	{
		RTM_ASSERT(_index < m_len, "");
		return m_str[_index];
	}

	//--------------------------------------------------------------------------

	String::String()
		: m_str(0)
		, m_len(0)
	{
	}

	String::String(StringView& _view)
	{
		set(_view.data(), _view.length());
	}

	String::String(const char* _str, uint32_t _len)
	{
		set(_str, _len);
	}

	String::String(const char* _start, const char* _end)
	{
		set(_start, _end);
	}

	String::~String()
	{
		if (m_str)
		{
			RTM_STRING_FREE((void*)m_str);
		}
	}

	String& String::operator = (const char* _str)
	{
		if (!_str)
		{
			clear();
			return *this;
		}

		set(_str);
		return *this;
	}

	String& String::operator = (const String& _other)
	{
		set(_other.data(), _other.length());
		return *this;
	}

	String& String::operator = (const StringView& _other)
	{
		set(_other.data(), _other.length());
		return *this;
	}

	void String::clear()
	{
		if (m_len != 0)
		{
			RTM_STRING_FREE((void*)m_str);
		}
		m_str = 0;
		m_len = 0;
	}

	void String::set(const char* _str, uint32_t _len)
	{
		clear();
		if (_str == 0)
			return;

		m_len = _len == -1 ? strLen(_str) : _len;
		char* str = (char*)RTM_STRING_ALLOC(sizeof(char) * m_len);
		strlCpy(str, m_len, _str);
		m_str = str;
	}

	void String::set(const char* _start, const char* _end)
	{
		RTM_ASSERT(_end > _start, "");
		set(_start, _end - _start);
	}

	void String::set(String& _string)
	{
		set(_string.data(), _string.length());
	}

	void String::set(StringView& _view)
	{
		set(_view.data(), _view.length());
	}

	void String::append(const char* _str, uint32_t _len)
	{
		uint32_t aLen = _len == -1 ? strLen(_str) : _len;
		m_str = (const char*)RTM_STRING_REALLOC((void*)m_str, sizeof(char) * (m_len + aLen));
		strlCpy((char*)&m_str[m_len], aLen, _str, aLen);
		m_len += aLen;
	}

	void String::append(const char* _start, const char* _end)
	{
		append(_start, _end - _start);
	}

	void String::append(String& _string)
	{
		append(_string.data(), _string.length());
	}

	void String::append(StringView& _view)
	{
		append(_view.data(), _view.length());
	}

	bool String::isNull() const
	{
		return m_len == 0;
	}

	const char*	String::data() const
	{
		return m_str;
	}

	uint32_t String::length() const
	{
		return m_len;
	}

	String::operator const char* ()
	{
		return data();
	}

	char String::operator[](uint32_t _index)
	{
		RTM_ASSERT(_index < m_len, "");
		return m_str[_index];
	}

} // namespace rtm

#endif // RTM_RBASE_STRINGVIEW_H
