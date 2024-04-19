//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_FLAT_OBJECT_H
#define RTM_FLAT_OBJECT_H

#include <rbase/inc/platform.h>

namespace rtm {

	template <typename T>
	class FlatPtr
	{
		int32_t		m_offset;

	public:
		FlatPtr(int32_t _offset = 0)
			: m_offset(_offset) {}

		inline T* operator -> ()
		{
			return (T*)((intptr_t)this + (intptr_t)m_offset);
		}

		inline void set(void* _ptr)
		{
			m_offset = (int32_t)((intptr_t)_ptr - (intptr_t)this);
		}

		inline uint32_t get() const
		{
			return m_offset;
		}

		inline void advance(uint32_t _numObjects)
		{
			m_offset += _numObjects * sizeof(T);
		}

		inline FlatPtr<T> operator - (const FlatPtr<T> _other)
		{
			RTM_ASSERT(m_offset > _other.m_offset, "Provided ptr must be greater than comparand!");
			uint32_t offset = m_offset - _other.m_offset;
		}

		inline uint32_t elementsFrom(const FlatPtr<T> _other)
		{
			RTM_ASSERT(m_offset > _other.m_offset, "Provided ptr must be greater than comparand!");
			uint32_t offset = m_offset - _other.m_offset;
			return offset / sizeof(T);
		}
	};

	template <typename T>
	class FlatObjectAllocator
	{
		uint8_t*	m_memory;
		uint32_t	m_size;
		uint32_t	m_capacity;

	public:
		inline FlatObjectAllocator()
		{
			m_memory	= (uint8_t*)malloc(sizeof(T) * 2);
			m_size		= sizeof(T);
			m_capacity	= sizeof(T) * 2;
		}

		inline ~FlatObjectAllocator()
		{
			free(m_memory);
		}

		inline T* getObject()
		{
			return (T*)m_memory;
		}

		inline uint32_t getSize() const
		{
			return m_size;
		}

		inline void* getMemory(uint32_t _offset = 0)
		{
			return (m_memory + _offset);
		}

		inline uint32_t getOffset(void* _memory)
		{
			return (uint32_t)((uintptr_t)_memory - (uintptr_t)m_memory);
		}

		inline uint32_t allocateMemory(uint32_t _sizeInBytes)
		{
			if (_sizeInBytes + m_size > m_capacity)
			{
				uint32_t newCapacity = _sizeInBytes + m_capacity * 3 / 2;
				m_memory = (uint8_t*)realloc(m_memory, newCapacity);
				m_capacity = newCapacity;
			}

			uint32_t oldSize = m_size;
			m_size += _sizeInBytes;
			return oldSize;
		}

		template <typename Y>
		inline FlatPtr<Y> allocateObjects(uint32_t _numObjects)
		{
			uint32_t memsize = _numObjects * sizeof(Y);
			return FlatPtr<Y>(allocateMemory(memsize));
		}

		template <typename Y>
		inline FlatPtr<Y> allocateObject()
		{
			return allocateObjects(1);
		}
	};

} // namespace rtm

#endif // RTM_FLAT_OBJECT_H
