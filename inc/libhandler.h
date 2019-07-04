//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_LIBHANDLER_H
#define RTM_RBASE_LIBHANDLER_H

#ifndef RBASE_NAMESPACE
#error "Must define RBASE_NAMESPACE!"
#endif

#include <rbase/inc/platform.h>

#include <stdio.h>	// vsprintf
#include <string.h>	// memcpy
#include <stdlib.h>
#include <stdarg.h> // va_start...

#define RTM_DEFAULT_ALIGNMENT 8

#ifndef RTM_LIBHANDLER_DECLARE
#define RTM_LIBHANDLER_DECLARE

namespace RBASE_NAMESPACE {

	extern rtm::MemoryManager*	g_allocator;
	extern rtm::ErrorHandler*	g_errorHandler;

	extern void* rtm_alloc(size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT);
	extern void* rtm_realloc(void* _ptr, size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT);
	extern void  rtm_free(void* _ptr);

} // namespace RBASE_NAMESPACE

#endif // RTM_LIBHANDLER_DECLARE

#ifdef RTM_LIBHANDLER_DEFINE

namespace RBASE_NAMESPACE {

	rtm::MemoryManager*		g_allocator 		= 0;
	rtm::ErrorHandler*		g_errorHandler		= 0;

	void ErrorHandler_fatal(const char* _file, int _line, const char* _format, ...)
	{
		if (g_errorHandler)
		{
			char message[8192];
			va_list args;
			va_start(args, _format);
			vsprintf(message, _format, args);
			g_errorHandler->fatal(_file, _line, message);
			va_end(args);
			return;
		}
	}

	void ErrorHandler_warning(const char* _file, int _line, const char* _format, ...)
	{
		if (g_errorHandler)
		{
			char message[8192];
			va_list args;
			va_start(args, _format);
			vsprintf(message, _format, args);
			g_errorHandler->warning(_file, _line, message);
			va_end(args);
			return;
		}
	}

	void ErrorHandler_log(const char* _file, int _line, const char* _format, ...)
	{
		if (g_errorHandler)
		{
			char message[8192];
			va_list args;
			va_start(args, _format);
			vsprintf(message, _format, args);
			g_errorHandler->debug(_file, _line, message);
			va_end(args);
			return;
		}
	}

	void* rtm_alloc(size_t _size, size_t _alignment)
	{
		void* ptr = 0;
		if (g_allocator)
			ptr = g_allocator->alloc(_size, _alignment);
		else
			ptr = malloc(_size);
		RTM_ASSERT(ptr!=0, "Failed to allocate memory!");
		return ptr;
	}

	void* rtm_realloc(void* _ptr, size_t _size, size_t _alignment)
	{
		void* ptr = 0;
		if (g_allocator)
			ptr = g_allocator->realloc(_ptr, _size, _alignment);
		else
			ptr = realloc(_ptr, _size);
		RTM_ASSERT(ptr!=0, "Failed to allocate memory!");
		return ptr;
	}

	void rtm_free(void* _ptr)
	{
		if (g_allocator)
		{
			g_allocator->free(_ptr);
			return;
		}
		free(_ptr);
	}

} // namespace RBASE_NAMESPACE

#endif // RTM_LIBHANDLER_DEFINE

#ifndef RTM_RBASE_LIBHANDLER_MEMORY_H
#define RTM_RBASE_LIBHANDLER_MEMORY_H

	struct rtmAllocTag { enum Enum { Tag }; };
	inline void* operator new (size_t, void* _mem, rtmAllocTag::Enum) { return _mem; }
	inline void operator delete (void*, void*, rtmAllocTag::Enum) { }
	
	template <typename T>
	T* rtm_new()
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		return new(mem, rtmAllocTag::Tag) T();
	}

	template <typename T, typename Arg1>
	T* rtm_new(Arg1 _arg1)
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		return new(mem, rtmAllocTag::Tag) T(_arg1);
	}

	template <typename T, typename Arg1, typename Arg2>
	T* rtm_new(Arg1 _arg1, Arg2 _arg2)
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		return new(mem, rtmAllocTag::Tag) T(_arg1, _arg2);
	}

	template <typename T, typename Arg1, typename Arg2, typename Arg3>
	T* rtm_new(Arg1 _arg1, Arg2 _arg2, Arg3 _arg3)
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		return new(mem, rtmAllocTag::Tag) T(_arg1, _arg2, _arg3);
	}

	template <typename T>
	T* rtm_new_array(size_t _numItems)
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T) * _numItems);
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		T* p = (T*)mem;
		while (_numItems--)
		{
			new(p, rtmAllocTag::Tag) T();
			++p;
		}
		return (T*)mem;
	}

	template <typename T, typename Arg1>
	T* rtm_new_array(size_t _numItems, Arg1 _arg1)
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T) * _numItems);
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		T* p = (T*)mem;
		while (_numItems--)
		{
			new(p, rtmAllocTag::Tag) T(_arg1);
			++p;
		}
		return (T*)mem;
	}

	template <typename T, typename Arg1, typename Arg2>
	T* rtm_new_array(size_t _numItems, Arg1 _arg1, Arg2 _arg2)
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T) * _numItems);
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		T* p = (T*)mem;
		while (_numItems--)
		{
			new(p, rtmAllocTag::Tag) T(_arg1, _arg2);
			++p;
		}
		return (T*)mem;
	}

	template <typename T, typename Arg1, typename Arg2, typename Arg3>
	T* rtm_new_array(size_t _numItems, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3)
	{
		void* mem = RBASE_NAMESPACE::rtm_alloc(sizeof(T) * _numItems);
		RTM_ASSERT(mem != 0, "Failed to allocate memory!");
		T* p = (T*)mem;
		while (_numItems--)
		{
			new(p, rtmAllocTag::Tag) T(_arg1, _arg2, _arg3);
			++p;
		}
		return (T*)mem;
	}

	template <typename T>
	void rtm_delete(T* _ptr)
	{
		_ptr->~T();
		RBASE_NAMESPACE::rtm_free(_ptr);
	}

	template <typename T>
	void rtm_delete_array(size_t _numItems, T* _ptr)
	{
		T* it = _ptr;
		while (_numItems--)
		{
			it->~T();
			++it;
		}
		RBASE_NAMESPACE::rtm_free(_ptr);
	}

	// STL allocator

	template<typename T>
	struct rtm_pointer_traits
	{
		using reference			= T&;
		using const_reference	= const T&;
		using difference_type	= intptr_t;
	};

	template<> struct rtm_pointer_traits<void>	{};

	template<typename T = void>
	struct rtm_allocator : public rtm_pointer_traits<T>
	{
		using value_type		= T;
		using size_type			= size_t;
		using pointer			= T*;
		using const_pointer		= const T*;
		using difference_type	= typename rtm_pointer_traits<pointer>::difference_type;

		rtm_allocator() = default;
		~rtm_allocator() = default;
		template<typename U> rtm_allocator(const rtm_allocator<U> &) {}
		template<typename U> struct rebind { typedef rtm_allocator<U> other; };

		size_type max_size() const { return (size_type)INT32_MAX; }
		T * allocate(size_t _numBlocks, const void* = 0) { return (value_type*)RBASE_NAMESPACE::rtm_alloc(sizeof(T) * _numBlocks); }
		void deallocate(T* _ptr, size_t) { RBASE_NAMESPACE::rtm_free(_ptr); }
	};

	template <class T, class U>	/*constexpr*/ bool operator==(const rtm_allocator<T>&, const rtm_allocator<U>&) { return true; }
	template <class T, class U>	/*constexpr*/ bool operator!=(const rtm_allocator<T>& _a1, const rtm_allocator<U>& _a2) { return !(_a1 == _a2); }

#ifdef RTM_DEFINE_STL_TYPES
	#ifndef RTM_DEFINE_STL_STRING
	#define RTM_DEFINE_STL_STRING
	#endif

	#ifndef RTM_DEFINE_STL_VECTOR
	#define RTM_DEFINE_STL_VECTOR
	#endif

	#ifndef RTM_DEFINE_STL_UNORDERED_MAP
	#define RTM_DEFINE_STL_UNORDERED_MAP
	#endif
#endif // RTM_DEFINE_STL_TYPES

#ifdef RTM_DEFINE_STL_STRING
	#include <string>
	typedef std::basic_string<char, std::char_traits<char>, rtm_allocator<char> >			rtm_string;
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, rtm_allocator<wchar_t> >	rtm_wstring;
#endif // RTM_DEFINE_STL_STRING

#ifdef RTM_DEFINE_STL_VECTOR
	#include <vector>
	template <typename T>	using rtm_vector = std::vector<T, rtm_allocator<T> >;
#endif // RTM_DEFINE_STL_STRING

#ifdef RTM_DEFINE_STL_UNORDERED_MAP
	#include <unordered_map>
	template <class K, class T,	class H = std::hash<K>, class Keq = std::equal_to<K> >
	using rtm_unordered_map = std::unordered_map<K, T, H, Keq, rtm_allocator<std::pair<const K, T> > >;
#endif // RTM_DEFINE_STL_STRING

	struct Memory
	{
		enum Enum
		{
			Internal,
			External
		};

		void*			m_data;
		size_t			m_size;
		Memory::Enum	m_type;

		Memory() :
			m_data(0),
			m_size(0),
			m_type(External)
		{}

		bool isNULL() const
		{
			return m_data == 0;
		}

		static const Memory reserve(size_t _size)
		{
			Memory mem;
			mem.m_size = _size;
			mem.m_type = Memory::Internal;
	
			void* ptr = 0;
			if (RBASE_NAMESPACE::g_allocator)
				ptr = RBASE_NAMESPACE::rtm_alloc(_size, RTM_DEFAULT_ALIGNMENT);
			else
				ptr = new uint8_t[_size];
			RTM_ASSERT(ptr != 0, "Failed to allocate memory block!");
			mem.m_data = ptr;
			return mem;
		}

		static const Memory clone(const void* _ptr, size_t _size)
		{
			Memory mem;
			mem.m_size = _size;
			mem.m_type = Memory::Internal;
			mem.m_data = RBASE_NAMESPACE::rtm_alloc(_size, RTM_DEFAULT_ALIGNMENT);
			memcpy(mem.m_data, _ptr, _size);
			return mem;
		}

		static const Memory clone(Memory& _memory)
		{
			return clone(_memory.m_data, _memory.m_size);
		}

		static const Memory external(const void* _ptr, uint32_t _size)
		{
			Memory mem;
			mem.m_size = _size;
			mem.m_type = Memory::External;
			mem.m_data = const_cast<void*>(_ptr);
			return mem;
		}

		static void release(const Memory& _memory)
		{
			if (_memory.m_type == Memory::External)
				return;

			RTM_ASSERT(_memory.m_data != 0, "Invalid pointer!");
			if (RBASE_NAMESPACE::g_allocator)
				RBASE_NAMESPACE::g_allocator->free(_memory.m_data);
			else
				delete[] (uint8_t*)_memory.m_data;
		}
	};
#endif // RTM_RBASE_LIBHANDLER_MEMORY_H

#endif // RTM_RBASE_LIBHANDLER_H

#ifndef	RTM_LIBHANDLER_DEFINE
	#undef RTM_RBASE_LIBHANDLER_H	// handle PCH
#endif // RTM_LIBHANDLER_DEFINE

#include <rbase/inc/libassert.h>
