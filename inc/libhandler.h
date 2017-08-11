//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_LIBHANDLER_H__
#define __RTM_RBASE_LIBHANDLER_H__

#ifndef RTM_LIBHANDLE_NAMESPACE
#error "Must define RTM_LIBHANDLE_NAMESPACE!"
#endif

#include <stdio.h>	// vsprintf
#include <string.h>	// memcpy
#include <stdlib.h>
#include <stdarg.h> // va_start...

#define RTM_DEFAULT_ALIGNMENT 8

#ifdef RTM_LIBHANDLER_DEFINE

namespace RTM_LIBHANDLE_NAMESPACE {

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

	void* rtm_alloc(size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT)
	{
		void* ptr = nullptr;
		if (g_allocator)
			ptr = g_allocator->alloc(_size, _alignment);
		else
			ptr = malloc(_size);
		RTM_ASSERT(ptr!=nullptr, "Failed to allocate memory!");
		return ptr;
	}

	void* rtm_realloc(void* _ptr, size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT)
	{
		void* ptr = nullptr;
		if (g_allocator)
			ptr = g_allocator->realloc(_ptr, _size, _alignment);
		else
			ptr = realloc(_ptr, _size);
		RTM_ASSERT(ptr!=nullptr, "Failed to allocate memory!");
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

} // namespace RTM_LIBHANDLE_NAMESPACE

#else // RTM_LIBHANDLER_DEFINE

namespace RTM_LIBHANDLE_NAMESPACE {

	extern rtm::MemoryManager*	g_allocator;
	extern rtm::ErrorHandler*	g_errorHandler;

	extern void* rtm_alloc(size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT);
	extern void* rtm_realloc(void* _ptr, size_t _size, size_t _alignment = RTM_DEFAULT_ALIGNMENT);
	extern void  rtm_free(void* _ptr);

} // namespace RTM_LIBHANDLE_NAMESPACE
	
#endif // RTM_LIBHANDLER_DEFINE

	/// Avoid include <new>
	struct rmemPlacementNew {};
	inline void* operator new(size_t, rmemPlacementNew, void* ptr) { return ptr; }
	inline void operator delete(void*, rmemPlacementNew, void*) {}
	#define RTM_PLACEMENT_NEW(_PTR)  new(ImPlacementNewDummy(), _PTR)

	template <typename T>
	T* rtm_new()
	{
		void* mem = RTM_LIBHANDLE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != nullptr, "Failed to allocate memory!");
		return RTM_PLACEMENT_NEW(mem) T();
	}

	template <typename T, typename Arg1>
	T* rtm_new(Arg1 _arg1)
	{
		void* mem = RTM_LIBHANDLE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != nullptr, "Failed to allocate memory!");
		return RTM_PLACEMENT_NEW(mem) T(_arg1);
	}

	template <typename T, typename Arg1, typename Arg2>
	T* rtm_new(Arg1 _arg1, Arg2 _arg2)
	{
		void* mem = RTM_LIBHANDLE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != nullptr, "Failed to allocate memory!");
		return RTM_PLACEMENT_NEW(mem) T(_arg1, _arg2);
	}

	template <typename T, typename Arg1, typename Arg2, typename Arg3>
	T* rtm_new(Arg1 _arg1, Arg2 _arg2, Arg3 _arg3)
	{
		void* mem = RTM_LIBHANDLE_NAMESPACE::rtm_alloc(sizeof(T));
		RTM_ASSERT(mem != nullptr, "Failed to allocate memory!");
		return RTM_PLACEMENT_NEW(mem) T(_arg1, _arg2, _arg3);
	}

	template <typename T>
	void rtm_delete(T* _ptr)
	{
		_ptr->~T();
		RTM_LIBHANDLE_NAMESPACE::rtm_free(_ptr);
	}

	template <class T>
	struct rtm_allocator
	{
	  typedef T value_type;
	  rtm_allocator() {}
	  template <class U> rtm_allocator(const rtm_allocator<U>& other);
	  T* allocate(std::size_t _n) { return (T*)rtm_malloc(sizeof(T) * _n); }
	  void deallocate(T* _p, std::size_t /*_n*/) { rtm_free(_p); }
	};
	template <class T, class U>	bool operator==(const rtm_allocator<T>&, const rtm_allocator<U>&) { return false; }
	template <class T, class U>	bool operator!=(const rtm_allocator<T>&, const rtm_allocator<U>&) { return false; }


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
			if (RTM_LIBHANDLE_NAMESPACE::g_allocator)
				ptr = RTM_LIBHANDLE_NAMESPACE::rtm_alloc(_size, 16);
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
			mem.m_data = RTM_LIBHANDLE_NAMESPACE::rtm_alloc(_size);
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
			if (RTM_LIBHANDLE_NAMESPACE::g_allocator)
				RTM_LIBHANDLE_NAMESPACE::g_allocator->free(_memory.m_data);
			else
				delete[] (uint8_t*)_memory.m_data;
		}
	};

#endif // __RTM_RBASE_LIBHANDLER_H__
