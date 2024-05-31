//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_LIBHANDLER_H
#define RTM_RBASE_LIBHANDLER_H

#ifndef RBASE_NAMESPACE
#error "Must define RBASE_NAMESPACE!"
#endif

#include <rbase/inc/platform.h>
#include <rbase/inc/stringfn.h>

#include <stdio.h>	// vsprintf
#include <stdarg.h> // va_start...

#ifndef RTM_LIBHANDLER_DECLARE
#define RTM_LIBHANDLER_DECLARE

namespace RBASE_NAMESPACE {

	extern rtm::MemoryManager*	g_allocator;
	extern rtm::ErrorHandler*	g_errorHandler;

	extern void* rtm_alloc(size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT);
	extern void* rtm_realloc(void* _ptr, size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT);
	extern void  rtm_free(void* _ptr, size_t _alignment = RTM_DEFAULT_ALIGNMENT);

} // namespace RBASE_NAMESPACE

#endif // RTM_LIBHANDLER_DECLARE

#ifdef RTM_LIBHANDLER_DEFINE

namespace RBASE_NAMESPACE {

	constexpr int MSG_BUFF_SIZE = 8192;

	rtm::MemoryManager*		g_allocator 		= 0;
	rtm::ErrorHandler*		g_errorHandler		= 0;

	void ErrorHandler_fatal(const char* _file, int _line, const char* _format, ...)
	{
		if (g_errorHandler)
		{
			char message[MSG_BUFF_SIZE];
			va_list args;
			va_start(args, _format);
			vsnprintf(message, MSG_BUFF_SIZE, _format, args);
			g_errorHandler->fatal(_file, _line, message);
			va_end(args);
			return;
		}
	}

	void ErrorHandler_warning(const char* _file, int _line, const char* _format, ...)
	{
		if (g_errorHandler)
		{
			char message[MSG_BUFF_SIZE];
			va_list args;
			va_start(args, _format);
			vsnprintf(message, MSG_BUFF_SIZE, _format, args);
			g_errorHandler->warning(_file, _line, message);
			va_end(args);
			return;
		}
	}

	void ErrorHandler_log(const char* _file, int _line, const char* _format, ...)
	{
		if (g_errorHandler)
		{
			char message[MSG_BUFF_SIZE];
			va_list args;
			va_start(args, _format);
			vsnprintf(message, MSG_BUFF_SIZE, _format, args);
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

	void rtm_free(void* _ptr, size_t _alignment)
	{
		if (g_allocator)
		{
			g_allocator->free(_ptr, _alignment);
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
			rtm::memCopy(mem.m_data, _ptr, _size);
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
				RBASE_NAMESPACE::g_allocator->free(_memory.m_data, RTM_DEFAULT_ALIGNMENT);
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
