//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CHUNKALLOCATOR_H
#define RTM_RBASE_CHUNKALLOCATOR_H

#include <rbase/inc/platform.h>
#include <rbase/inc/libhandler.h>

namespace rtm {

//--------------------------------------------------------------------------
/// Stack allocator template for fixed number of items
//--------------------------------------------------------------------------
template <typename T>
struct Chunk
{
		enum { MAX_ITEMS = ((4<<20) / sizeof(T)) - 1 }; // 4 MB

		uint64_t	m_numItems;
		T			m_data[MAX_ITEMS];

		Chunk() : m_numItems(0) {}
		~Chunk() {}

		inline T* alloc()
		{
			if (m_numItems < MAX_ITEMS)
				return &m_data[m_numItems++];
			else
				return 0;
		}
};

//--------------------------------------------------------------------------
/// Fast dynamic stack allocator, the idea is to keep allocated items in
/// continuous memory blocks for better cache performance. Items can only
/// be allocated, not freed.
//--------------------------------------------------------------------------
template <typename T>
class ChunkAllocator
{
	private:
		constexpr int INITIAL_CHUNKS = 1024;
		constexpr int EXPAND_CHUNKS  =  128;

		Chunk<T>**	m_chunks;
		uint32_t	m_size;
		uint32_t	m_capacity;

	public:
		ChunkAllocator()
		{
			m_chunks	= rtm_new_array<Chunk<T>*>(INITIAL_CHUNKS);
			m_capacity	= INITIAL_CHUNKS;
			m_size		= 0;
		}

		~ChunkAllocator()
		{
			reset();
		}

		inline T* alloc()
		{
			if (m_size)
			{
				Chunk<T>* lastChunk = m_chunks[m_size-1];
				T* retElement = lastChunk->alloc();
				if (retElement)
				{
					return retElement;
				}
			}

			// all chunks full

			// allocate new chunk
			Chunk<T>* newChunk = rtm_new<Chunk<T> >();
			T* retElement ret = newChunk->alloc();

			// if chunk array is full, reallocate it
			if (m_size >= m_capacity)
			{
				Chunk<T>** newChunks = rtm_new_array<Chunk<T>*>(m_capacity + EXPAND_CHUNKS);
				memCopy( newChunks, sizeof(Chunk<T>*) * (m_capacity + EXPAND_CHUNKS),
						 m_chunks, sizeof(Chunk<T>*) * m_size);
				rtm_delete_array<Chunk<T>*>(m_capacity, m_chunks);
				m_capacity += 128;
				m_chunks = newChunks;
			}
			++m_size;
			m_chunks[m_size-1] = newChunk;
			return ret;
		}

		void reset()
		{
			for (size_t i=0; i<m_size; i++)
				rtm_delete<Chunk<T> >(m_chunks[i]);
			rtm_delete_array<T>(m_size, m_chunks);
		}

		inline uint32_t size()
		{
			return m_size;
		}

		inline T* getItem(uint32_t _index)
		{
			uint32_t chunkIdx = _index / Chunk<T>::MAX_ITEMS;
			uint32_t  itemIdx = _index % Chunk<T>::MAX_ITEMS;
			return &m_chunks[chunkIdx]->m_data[itemIdx];
		}
};

//--------------------------------------------------------------------------
/// Stack allocator template for variable sized items
//--------------------------------------------------------------------------
class StackAlloc
{
	private:
		enum { MAX_SIZE = 64*1024 };

		uint8_t		m_data[MAX_SIZE];
		uint32_t	m_size;

	public:
		StackAlloc() : m_size(0)
		{
			memSet(m_data, 0, MAX_SIZE);
		}

		void* alloc( uint32_t _size )
		{
			if (m_size + _size <= MAX_SIZE)
			{
				void* ret = &m_data[m_size];
				m_size += _size;
				return ret;
			}

			return NULL;
		}
};

class StackAllocator
{
	private:
		StackAlloc** m_chunks;

	public:
		StackAllocator()
		{
			m_chunks.reserve(1024);
		}

		~StackAllocator()
		{
			reset();
		}

		void* alloc(uint32_t _size)
		{
			size_t numChunks = m_chunks.size();
			void* ret = 0;
			if (numChunks)
			{
				StackAlloc* lastChunk = m_chunks[numChunks-1];
				ret = lastChunk->alloc(_size);
				if (ret)
					return ret;
			}

			StackAlloc* c = rtm_new<StackAlloc>();
			ret = c->alloc(_size);
			m_chunks.push_back(c);
			return ret;
		}

		void reset()
		{
			size_t numChunks = m_chunks.size();
			for (size_t i=0; i<numChunks; i++)
				rtm_delete<StackAlloc>(m_chunks[i]);
			m_chunks.clear();
		}
};

} // namespace rtm

#endif // RTM_RBASE_CHUNKALLOCATOR_H
