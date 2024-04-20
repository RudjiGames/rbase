//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_FLAT_OBJECT_H
#define RTM_FLAT_OBJECT_H
#include <windows.h>
#include <rbase/inc/platform.h>

namespace rtm {

	class FlatObjectAllocator;

	//--------------------------------------------------------------------------
	// 'Pointer' which is a relative offset coupled to FlatObjectAllocator
	//--------------------------------------------------------------------------
	template <typename T>
	class FlatPtr
	{
		int32_t		m_offset;

	public:
		explicit FlatPtr(int32_t _offset)
			: m_offset(_offset) {}

		FlatPtr(FlatPtr& _other)
		{
			m_offset = _other.m_offset;
		}

		template <typename Y>
		explicit FlatPtr(const FlatPtr<Y>& _other)
		{
			m_offset = _other.getOffset();
		}

		inline T* get(FlatObjectAllocator& _alloc)
		{
			return (T*)(_alloc.getMemory() + m_offset);
		}

		inline uint32_t getOffset() const
		{
			return m_offset;
		}

		inline void advance(uint32_t _numObjects)
		{
			m_offset += _numObjects * sizeof(T);
		}

		inline uint32_t elementsFrom(const FlatPtr<T> _other)
		{
			RTM_ASSERT(m_offset > _other.m_offset, "Provided ptr must be greater than comparand!");
			uint32_t offset = m_offset - _other.m_offset;
			return offset / sizeof(T);
		}
	};

	//--------------------------------------------------------------------------
	// Linear allocator using offsets to allocate
	//--------------------------------------------------------------------------
	class FlatObjectAllocator
	{
		uint8_t*	m_memory;
		uint32_t	m_size;
		uint32_t	m_capacity;
		bool		m_isStatic;

		static const int INITIAL_SIZE = 1024;

	public:
		FlatObjectAllocator(void* _memory, uint32_t _size)
		{
			m_memory	= (uint8_t*)_memory;
			m_size		= 0;
			m_capacity	= _size;
			m_isStatic	= true;
		}

		FlatObjectAllocator()
		{
			m_memory	= (uint8_t*)malloc(INITIAL_SIZE);
			m_size		= 0;
			m_capacity	= INITIAL_SIZE;
			m_isStatic	= false;
		}

		~FlatObjectAllocator()
		{
			if (!m_isStatic)
				free(m_memory);
		}

		inline uint32_t getSize() const
		{
			return m_size;
		}

		inline uint8_t* getMemory() const
		{
			return m_memory;
		}

		inline FlatPtr<void> allocateMemory(uint32_t _sizeInBytes)
		{
			if (_sizeInBytes + m_size > m_capacity)
			{
				uint32_t newCapacity = _sizeInBytes + m_capacity * 3 / 2;
				m_memory = (uint8_t*)realloc(m_memory, newCapacity);
				m_capacity = newCapacity;
			}

			uint32_t oldSize = m_size;
			m_size += _sizeInBytes;
			return FlatPtr<void>(oldSize);
		}

		template <typename T>
		inline FlatPtr<T> allocateObjectsSized(uint32_t _sizeInBytes)
		{
			FlatPtr<void> memoryOffset = allocateMemory(_sizeInBytes);
			FlatPtr<T> ptr(memoryOffset);
			return ptr;
		}

		template <typename T>
		inline FlatPtr<T> allocateObjects(uint32_t _numObjects)
		{
			uint32_t memsize = _numObjects * sizeof(T);
			return allocateObjectsSized<T>(memsize);
		}

		template <typename T>
		inline FlatPtr<T> allocateObject()
		{
			return allocateObjects<T>(1);
		}
	};

} // namespace rtm

#endif // RTM_FLAT_OBJECT_H
