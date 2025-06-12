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
	/// Typed stack allocator chunk. Total size of 4MB.
	//--------------------------------------------------------------------------
	template <typename T, size_t MaxMemory = 4 << 20>
	struct Chunk
	{
		enum
		{
			CHUNK_MEMORY = MaxMemory - (sizeof(T) > sizeof(uint32_t) ? sizeof(T) : sizeof(uint32_t)),
			CHUNK_ITEMS  = CHUNK_MEMORY / sizeof(T)
		};

		uint32_t	m_numItems;
		T			m_data[CHUNK_ITEMS];

		inline Chunk() : m_numItems(0) {}

		inline T* alloc()
		{
			if (m_numItems < CHUNK_ITEMS)
			{
				return &m_data[m_numItems++];
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
		enum
		{
			CHUNKS_INITIAL = 128,
			CHUNKS_GROW_BY = 128
		};

		Chunk<T>**	m_chunks;
		uint32_t	m_size;
		uint32_t	m_capacity;

	public:
		inline ChunkAllocator()
		{
			m_chunks	= rtm_new_array<Chunk<T>*>(CHUNKS_INITIAL);
			m_capacity	= CHUNKS_INITIAL;
			m_size		= 0;
		}

		inline ~ChunkAllocator()
		{
			reset();
		}

		inline T* alloc()
		{
			if (m_size)
			{
				if (T* retElement = getLastChunk()->alloc())
				{
					return retElement;
				}
			}

			// allocate new chunk
			Chunk<T>* newChunk = rtm_new<Chunk<T>>();
			T* ret = newChunk->alloc();
			addNewChunk(newChunk);
			return ret;
		}

		inline void reset()
		{
			for (size_t i=0; i<m_size; ++i)
			{
				rtm_delete<Chunk<T>>(m_chunks[i]);
			}
			rtm_delete_array<Chunk<T>*>(m_size, m_chunks);
		}

		inline uint32_t size() const
		{
			return m_size;
		}

		inline T* getItem(uint32_t _index)
		{
			uint32_t chunkIdx = _index / Chunk<T>::MAX_ITEMS;
			uint32_t  itemIdx = _index % Chunk<T>::MAX_ITEMS;
			return &m_chunks[chunkIdx]->m_data[itemIdx];
		}

		void addNewChunk(Chunk<T>* _newChunk)
		{
			Chunk<T>** newArray = rtm_new_array<Chunk<T>*>(m_capacity + CHUNKS_GROW_BY);
			const uint64_t sizeToCopy = sizeof(Chunk<T>*) * m_capacity;
			memCopy(newArray, sizeToCopy, m_chunks, sizeToCopy);
			rtm_delete_array<Chunk<T>*>(m_capacity, m_chunks);
			m_capacity += CHUNKS_GROW_BY;
			m_chunks = newArray;
			m_chunks[m_size++] = _newChunk;
		}

		inline Chunk<T>* getLastChunk()
		{
			RTM_ASSERT(m_size != 0, "");
			return m_chunks[m_size - 1];
		}
	};

	//--------------------------------------------------------------------------
	/// Stack allocator template for variable sized items
	//--------------------------------------------------------------------------
	class StackAllocatorChunk
	{
		enum { CHUNK_SIZE = 64*1024 - sizeof(uint32_t) }; // 64Kb total

		uint32_t	m_size;
		uint8_t		m_data[CHUNK_SIZE];

	public:
		inline StackAllocatorChunk() : m_size(0)
		{
			memSet(m_data, 0, CHUNK_SIZE);
		}

		inline void* alloc( uint32_t _size )
		{
			RTM_ASSERT(_size < CHUNK_SIZE, "");

			if (m_size + _size <= CHUNK_SIZE)
			{
				void* ret = &m_data[m_size];
				m_size += _size;
				return ret;
			}

			return nullptr;
		}
	};

	class StackAllocator
	{
		enum
		{
			CHUNKS_INITIAL = 128,
			CHUNKS_GROW_BY = 128
		};

		StackAllocatorChunk**	m_chunks;
		uint32_t				m_size;
		uint32_t				m_capacity;

	public:
		inline StackAllocator()
		{
			m_chunks	= rtm_new_array<StackAllocatorChunk*>(CHUNKS_INITIAL);
			m_capacity	= CHUNKS_INITIAL;
			m_size		= 0;
		}

		inline ~StackAllocator()
		{
			reset();
		}

		inline void* alloc(uint32_t _size)
		{
			const uint32_t numChunks = m_size;
			if (numChunks)
			{
				if (void* ret = getLastChunk()->alloc(_size))
				{
					return ret;
				}
			}
			
			StackAllocatorChunk* newChunk = rtm_new<StackAllocatorChunk>();
			void* ret = newChunk->alloc(_size);
			addNewChunk(newChunk);
			return ret;
		}

		inline uint32_t totalMemorySize() const
		{
			return m_size * sizeof(StackAllocatorChunk) + m_capacity * sizeof(StackAllocatorChunk*);
		}

		void reset()
		{
			const uint32_t numChunks = m_size;
			for (uint32_t i=0; i<numChunks; ++i)
			{
				rtm_delete<StackAllocatorChunk>(m_chunks[i]);
			}

			rtm_delete_array<StackAllocatorChunk*>(m_capacity, m_chunks);
			m_chunks = nullptr;
		}

	private:

		void addNewChunk(StackAllocatorChunk* _newChunk)
		{
			StackAllocatorChunk** newArray = rtm_new_array<StackAllocatorChunk*>(m_capacity + CHUNKS_GROW_BY);
			const uint64_t sizeToCopy = sizeof(StackAllocatorChunk*) * m_capacity;
			memCopy(newArray, sizeToCopy, m_chunks, sizeToCopy);
			rtm_delete_array<StackAllocatorChunk*>(m_capacity, m_chunks);
			m_capacity += CHUNKS_GROW_BY;
			m_chunks = newArray;
			m_chunks[m_size++] = _newChunk;
		}

		inline StackAllocatorChunk* getLastChunk()
		{
			RTM_ASSERT(m_size != 0, "");
			return m_chunks[m_size - 1];
		}
	};

} // namespace rtm

#endif // RTM_RBASE_CHUNKALLOCATOR_H
