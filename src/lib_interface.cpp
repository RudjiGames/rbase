//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/console.h>
#include <rbase/inc/uint32_t.h>

namespace rtm {

template<typename Ty>
inline Ty alignUp(Ty _a, int32_t _align)
{
	const Ty mask = Ty(_align - 1);
	return Ty( (_a + mask) & ~mask);
}

inline void* alignPtr(void* _ptr, size_t _extra, size_t _align)
{
	union { void* ptr; uintptr_t addr; } un;
	un.ptr = _ptr;
	uintptr_t unaligned = un.addr + _extra; // space for header
	uintptr_t aligned = alignUp(unaligned, int32_t(_align));
	un.addr = aligned;
	return un.ptr;
}

inline void* alignedAlloc(MemoryManager* _allocator, size_t _size, size_t _align)
{
	const size_t align = _align < sizeof(uint32_t) ? sizeof(uint32_t) : _align;
	const size_t total = _size + align;
	uint8_t* ptr = (uint8_t*)_allocator->alloc(total, RTM_DEFAULT_ALIGNMENT);
	uint8_t* aligned = (uint8_t*)alignPtr(ptr, sizeof(uint32_t), align);
	uint32_t* header = (uint32_t*)aligned - 1;
	*header = uint32_t(aligned - ptr);
	return aligned;
}

inline void alignedFree(MemoryManager* _allocator, void* _ptr, size_t _align)
{
	RTM_UNUSED(_align);
	uint8_t* aligned = (uint8_t*)_ptr;
	uint32_t* header = (uint32_t*)aligned - 1;
	uint8_t* ptr = aligned - *header;
	_allocator->free(ptr, RTM_DEFAULT_ALIGNMENT);
}

inline void* alignedRealloc(MemoryManager* _allocator, void* _ptr, size_t _size, size_t _align)
{
	if (NULL == _ptr)
		return alignedAlloc(_allocator, _size, _align);

	uint8_t* aligned = (uint8_t*)_ptr;
	uint32_t offset = *( (uint32_t*)aligned - 1);
	uint8_t* ptr = aligned - offset;

	const size_t align = _align < sizeof(uint32_t) ? sizeof(uint32_t) : _align;
	const size_t total = _size + align;
	ptr = (uint8_t*)_allocator->realloc(ptr, total, RTM_DEFAULT_ALIGNMENT);
	uint8_t* newAligned = (uint8_t*)alignPtr(ptr, sizeof(uint32_t), align);

	if (newAligned == aligned)
	{
		return aligned;
	}

	aligned = ptr + offset;
	memmove(newAligned, aligned, _size);
	uint32_t* header = (uint32_t*)newAligned - 1;
	*header = uint32_t(newAligned - ptr);
	return newAligned;
}

struct MemoryManagerCrt : public MemoryManager
{
	virtual void* alloc(size_t _size, size_t _alignment) override
	{
		if (_alignment <= RTM_DEFAULT_ALIGNMENT)
			return ::malloc(_size);

#if RTM_COMPILER_MSVC
		return _aligned_malloc(_size, _alignment);
#else
		return alignedAlloc(this, _size, _alignment);
#endif
	}

	virtual void* realloc(void* _ptr, size_t _size, size_t _alignment) override
	{
		if (_alignment <= RTM_DEFAULT_ALIGNMENT)
			return ::realloc(_ptr, _size);

#if RTM_COMPILER_MSVC
		return _aligned_realloc(_ptr, _size, _alignment);
#else
		return alignedRealloc(this, _ptr, _size, _alignment);
#endif
	}

	virtual void free(void* _ptr, size_t _alignment) override
	{
		if (!_ptr)
			return;

		if (_alignment <= RTM_DEFAULT_ALIGNMENT)
		{
			::free(_ptr);
			return;
		}

#if RTM_COMPILER_MSVC
		_aligned_free(_ptr);
#else
		alignedFree(this, _ptr, _alignment);
#endif // RTM_COMPILER_MSVC
	}

} MemoryManagerCrtInstance;


struct ErrorHandlerStd : public ErrorHandler
{
	virtual void fatal(const char* _file, int _line, const char* _message) override
	{
		RTM_UNUSED_2(_file, _line);
		rtm::Console::error(_message);
	}

	virtual void warning(const char* _file, int _line, const char* _message) override
	{
		RTM_UNUSED_2(_file, _line);
		rtm::Console::warning(_message);
	}

	virtual void debug(const char* _file, int _line, const char* _message) override
	{
		RTM_UNUSED_2(_file, _line);
		rtm::Console::print(_message);
	}

} ErrorHandlerStdInstance;

struct MemoryManager* rbaseGetMemoryManager()
{
	return &MemoryManagerCrtInstance;
}

struct ErrorHandler* rbaseGetErrorHandler()
{
	return &ErrorHandlerStdInstance;
}

} // namespace rtm
