//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CHUNKALLOCATOR_H
#define RTM_RBASE_CHUNKALLOCATOR_H

#include <rbase/inc/platform.h>
#include <rbase/inc/libhandler.h>
#include <vector>

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

		inline T*	alloc()
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
		std::vector<Chunk<T>*>	m_chunks;
		uint32_t				m_size;

	public:
		ChunkAllocator()
		{
			m_chunks.reserve(1024);
			m_size = 0;
		}

		~ChunkAllocator()
		{
			reset();
		}

		inline T* alloc()
		{
			m_size++;
			size_t numChunks = m_chunks.size();
			T* ret = 0;
			if (numChunks)
			{
				Chunk<T>* lastChunk = m_chunks[numChunks-1];
				ret = lastChunk->alloc();
				if (ret)
					return ret;
			}

			Chunk<T>* c = rtm_new<Chunk<T> >();
			ret = c->alloc();
			m_chunks.push_back(c);
			return ret;
		}

		void reset()
		{
			size_t numChunks = m_chunks.size();
			for (size_t i=0; i<numChunks; i++)
				rtm_delete<Chunk<T> >(m_chunks[i]);
			m_chunks.clear();
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
		std::vector<StackAlloc*>	m_chunks;

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
