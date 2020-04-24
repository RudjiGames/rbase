//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
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
class Chunk
{
	private:
		enum { MAX_ITEMS = ((1<<14) - 52) / sizeof(T) }; // 16Kb - book keeping

		uint32_t	m_NumItems;
		T*			m_Data;

	public:
		Chunk() : m_NumItems(0)
		{
			m_Data = rtm_new_array<T>(MAX_ITEMS);
			memset(m_Data, 0, sizeof(T)*MAX_ITEMS);
		}

		~Chunk()
		{
			rtm_delete_array<T>(MAX_ITEMS, m_Data);
		}

		T*	alloc()
		{
			if (m_NumItems < MAX_ITEMS)
			{
				T* ret = &m_Data[m_NumItems];
				++m_NumItems;
				return ret;
			}
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
		std::vector<Chunk<T>*>	m_Chunks;

	public:
		ChunkAllocator()
		{
			m_Chunks.reserve(1024);
		}

		~ChunkAllocator()
		{
			reset();
		}

		T* alloc()
		{
			size_t numChunks = m_Chunks.size();
			T* ret = 0;
			if (numChunks)
			{
				Chunk<T>* lastChunk = m_Chunks[numChunks-1];
				ret = lastChunk->alloc();
				if (ret)
					return ret;
			}

			Chunk<T>* c = rtm_new<Chunk<T> >();
			ret = c->alloc();
			m_Chunks.push_back(c);
			return ret;
		}

		void reset()
		{
			size_t numChunks = m_Chunks.size();
			for (size_t i=0; i<numChunks; i++)
				rtm_delete<Chunk<T> >(m_Chunks[i]);
			m_Chunks.clear();
		}
};


//--------------------------------------------------------------------------
/// Stack allocator template for variable sized items
//--------------------------------------------------------------------------
class StackAlloc
{
	private:
		enum { MAX_SIZE = 64*1024 };

		uint8_t		m_Data[MAX_SIZE];
		uint32_t	m_size;

	public:
		StackAlloc() : m_size(0)
		{
			memset(m_Data, 0, MAX_SIZE);
		}

		void* alloc( uint32_t _size )
		{
			if (m_size + _size <= MAX_SIZE)
			{
				void* ret = &m_Data[m_size];
				m_size += _size;
				return ret;
			}

			return NULL;
		}
};

class StackAllocator
{
	private:
		rtm_vector<StackAlloc*>	m_Chunks;

	public:
		StackAllocator()
		{
			m_Chunks.reserve(1024);
		}

		~StackAllocator()
		{
			reset();
		}

		void* alloc(uint32_t _size)
		{
			size_t numChunks = m_Chunks.size();
			void* ret = 0;
			if (numChunks)
			{
				StackAlloc* lastChunk = m_Chunks[numChunks-1];
				ret = lastChunk->alloc(_size);
				if (ret)
					return ret;
			}

			StackAlloc* c = rtm_new<StackAlloc>();
			ret = c->alloc(_size);
			m_Chunks.push_back(c);
			return ret;
		}

		void reset()
		{
			size_t numChunks = m_Chunks.size();
			for (size_t i=0; i<numChunks; i++)
				rtm_delete<StackAlloc>(m_Chunks[i]);
			m_Chunks.clear();
		}
};

} // namespace rtm

#endif // RTM_RBASE_CHUNKALLOCATOR_H
