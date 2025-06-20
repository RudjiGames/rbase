//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CHUNKALLOCATOR_H
#define RTM_RBASE_CHUNKALLOCATOR_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringfn.h>
#include <rbase/inc/libhandler.h>

namespace rtm {

	// Count trailing zeroes
	static constexpr int chunkCTZ(const int _v)
	{
		unsigned int v = _v; // 32-bit word input to count zero bits on right
		unsigned int c = 32; // c will be the number of zero bits on the right
		v &= -signed(v);
		if (v) c--;
		if (v & 0x0000FFFF) c -= 16;
		if (v & 0x00FF00FF) c -=  8;
		if (v & 0x0F0F0F0F) c -=  4;
		if (v & 0x33333333) c -=  2;
		if (v & 0x55555555) c -=  1;
		return c;
	}

	//--------------------------------------------------------------------------
	/// Fast dynamic stack allocator, the idea is to keep allocated items in
	/// continuous memory blocks for better cache performance. Items can only
	/// be allocated, not freed.
	//--------------------------------------------------------------------------
	template <typename T, int CHUNK_ITEMS = 1 << 16>
	class ChunkAllocator
	{
	private:
		struct RTM_ALIGN(16) Chunk
		{
			T		m_data[CHUNK_ITEMS];
		};

		enum
		{
			CHUNK_SHIFT			= chunkCTZ(CHUNK_ITEMS),
			CHUNK_MASK			= CHUNK_ITEMS - 1,
			CHUNK_ARRAY_INITIAL = 128,
			CHUNK_ARRAY_GROW_BY = 128
		};

		Chunk**		m_chunks;
		uint32_t	m_numItems;
		uint32_t	m_numChunks;
		uint32_t	m_maxChunks;

	public:
		inline ChunkAllocator()
			: m_chunks(nullptr)
			, m_numItems(0)
			, m_numChunks(0)
			, m_maxChunks(CHUNK_ARRAY_INITIAL)
		{
			static_assert((CHUNK_ITEMS & CHUNK_MASK) == 0);
			m_chunks = rtm_new_array<Chunk*>(CHUNK_ARRAY_INITIAL);
			addNewChunk(); // add initial chunk so there is always a valid last chunk (for alloc call)
		}

		inline ~ChunkAllocator()
		{
			reset();
		}

		inline void allocOptionalAddChunk()
		{
			// check if current chunk is full:
			// current item index is last one in chunk
			if ((m_numItems & CHUNK_MASK) == CHUNK_MASK)
			{
				addNewChunk();
			}
		}

		inline uint32_t allocHandle()
		{
			allocOptionalAddChunk();
			return m_numItems++;
		}

		inline uint32_t allocHandle(T*& _optionalPtr)
		{
			allocOptionalAddChunk();

			const uint32_t allocIndex        = m_numItems++;
			const uint32_t allocIndexInChunk = allocIndex & CHUNK_MASK;

			if (_optionalPtr)
			{
				_optionalPtr = &m_chunks[m_numChunks - 1].m_data[allocIndexInChunk];
			}

			return allocIndex;
		}

		inline T* alloc()
		{
			const uint32_t itemHandle = allocHandle();
			return getItem(itemHandle - 1);
		}

		inline void reset()
		{
			const uint32_t numChunks = m_numChunks;
			for (size_t i=0; i<numChunks; ++i)
			{
				rtm_delete<Chunk>(m_chunks[i]);
			}
			rtm_delete_array<Chunk*>(m_maxChunks, m_chunks);

			m_chunks	= nullptr;
			m_numItems	= 0;
			m_numChunks	= 0;
			m_maxChunks	= 0;
		}

		inline uint32_t size() const
		{
			return m_numItems;
		}

		inline T* getItem(uint32_t _index)
		{
			uint32_t chunkIdx = _index >> CHUNK_SHIFT;
			uint32_t  itemIdx = _index  & CHUNK_MASK;
			return &m_chunks[chunkIdx]->m_data[itemIdx];
		}

	private:
		inline void addNewChunk()
		{
			Chunk* newChunk = rtm_new<Chunk>();
			if (m_numChunks == m_maxChunks) // check if chunk array is full
			{
				Chunk** newArray = rtm_new_array<Chunk*>(m_maxChunks + CHUNK_ARRAY_GROW_BY);
				const uint64_t sizeToCopy = sizeof(Chunk*) * m_numChunks;
				memCopy(newArray, sizeToCopy, m_chunks, sizeToCopy);
				rtm_delete_array<Chunk*>(m_maxChunks, m_chunks);
				m_maxChunks += CHUNK_ARRAY_GROW_BY;
				m_chunks     = newArray;
			}
			m_chunks[m_numChunks++] = newChunk;
		}
	};

	//--------------------------------------------------------------------------
	/// Stack allocator template for variable sized items
	//--------------------------------------------------------------------------
	class StackAllocator
	{
		enum
		{
			// Bigger chunk == higher performance
			// 1 Mb seems to be a sweet spot and matches large page size
			CHUNK_SIZE			= 1024 * 1024, // 1 Mb
			CHUNK_ARRAY_INITIAL	= 128,
			CHUNK_ARRAY_GROW_BY	= 128
		};

		struct Chunk
		{
			uint8_t	m_data[CHUNK_SIZE];
		};

		Chunk**		m_chunks;
		uint64_t	m_curChunkSize;
		uint32_t	m_numChunks;
		uint32_t	m_maxChunks;

	public:
		inline StackAllocator()
			: m_chunks(nullptr)
			, m_curChunkSize(0)
			, m_numChunks(0)
			, m_maxChunks(0)
		{
			m_chunks	= rtm_new_array<Chunk*>(CHUNK_ARRAY_INITIAL);
			m_maxChunks	= CHUNK_ARRAY_INITIAL;
			addNewChunk(); // add initial chunk so there is always a valid last chunk (for alloc call)
		}

		inline ~StackAllocator()
		{
			reset();
		}

		inline void* alloc(uint32_t _size)
		{
			RTM_ASSERT(_size <= CHUNK_SIZE, "");
			// check if current chunk is full:
			// current item index is last one in chunk
			if ((m_curChunkSize + _size) > CHUNK_SIZE)
			{
				addNewChunk();
			}

			Chunk* lastChunk = m_chunks[m_numChunks - 1];
			void* retPtr     = &lastChunk->m_data[m_curChunkSize];
			m_curChunkSize  += _size;
			return retPtr;
		}

		inline uint32_t totalMemorySize() const
		{
			return m_numChunks * sizeof(Chunk) + sizeof(StackAllocator);
		}

		void reset()
		{
			const uint32_t numChunks = m_numChunks;
			for (size_t i = 0; i < numChunks; ++i)
			{
				rtm_delete<Chunk>(m_chunks[i]);
			}

			rtm_delete_array<Chunk*>(m_maxChunks, m_chunks);
			m_chunks		= nullptr;
			m_curChunkSize	= 0;
			m_numChunks		= 0;
			m_maxChunks		= 0;
		}

	private:
		void addNewChunk()
		{
			Chunk* newChunk = rtm_new<Chunk>();
			if (m_numChunks == m_maxChunks) // check if chunk array is full
			{
				Chunk** newArray = rtm_new_array<Chunk*>(m_maxChunks + CHUNK_ARRAY_GROW_BY);
				const uint64_t sizeToCopy = sizeof(Chunk*) * m_numChunks;
				memCopy(newArray, sizeToCopy, m_chunks, sizeToCopy);
				rtm_delete_array<Chunk*>(m_maxChunks, m_chunks);
				m_maxChunks += CHUNK_ARRAY_GROW_BY;
				m_chunks = newArray;
			}
			m_chunks[m_numChunks++]	= newChunk;
			m_curChunkSize			= 0;
		}
	};

} // namespace rtm

#endif // RTM_RBASE_CHUNKALLOCATOR_H
