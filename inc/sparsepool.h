//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_SPARSEPOOL_H
#define RTM_RBASE_SPARSEPOOL_H

#include <rbase/inc/platform.h>
#include <malloc.h> // memalign
#include <memory.h> // memset, memcpy

namespace rtm {

	class SparsePool
	{
	protected:
		struct Chunk
		{
			uint8_t*	m_data;
			uint32_t	m_size;

			Chunk() : 
				m_data(0), 
				m_size(0)
			{}
		};

		Chunk*				m_chunks;
		uint32_t			m_numChunks;
		uint32_t			m_elementSize;
		uint32_t			m_alignment;
		uint32_t			m_elementsInChunk;
		uint32_t			m_elementsTotal;
		MemoryManager*		m_memoryManager;

	public:
		SparsePool()
		{
			memset(this, 0, sizeof(SparsePool));
		}

		~SparsePool()
		{
			releaseChunks();
		}

		uint32_t size() const
		{
			return m_elementsTotal;
		}

		void init(uint32_t _elementSize, uint32_t _alignment = 0, uint32_t _elementsInChunk = 2048, MemoryManager* _allocator = 0)
		{
			RTM_ASSERT((_elementsInChunk & (_elementsInChunk-1)) == 0, "Number of elements in a chunk must be power of two!");
			RTM_ASSERT(_elementsInChunk != 0, "Number of elements in chunk must greater than zero!");

			m_chunks			= 0;
			m_numChunks			= 0;
			m_elementSize		= _elementSize; 
			m_alignment			= _alignment;
			m_elementsInChunk	= _elementsInChunk;
			m_elementsTotal		= 0;
			m_memoryManager		= _allocator;

			if (m_alignment == 0)
				m_alignment = m_elementSize;

			m_elementSize = RTM_ALIGNTO(m_elementSize, m_alignment);
		}

		inline void* alloc(uint32_t _idx)
		{
			RTM_ASSERT(m_elementSize != 0, "Sparse pool not initialized!");

			const uint32_t chunkIdx		= _idx / m_elementsInChunk;
			const uint32_t elementIdx	= _idx & (m_elementsInChunk-1);
			
			if (chunkIdx >= m_numChunks)
			{
				const uint32_t oldChunksSize = m_numChunks * m_elementsInChunk * m_elementSize;
				const uint32_t newChunksSize = (chunkIdx+1) * m_elementsInChunk * m_elementSize;
				m_chunks = (Chunk*)reallocateHeap(m_chunks, oldChunksSize, newChunksSize);
				m_numChunks = chunkIdx;
			}

			Chunk& chunk = m_chunks[chunkIdx];
			if (chunk.m_data == 0)
				allocateChunk(chunk);

			RTM_ASSERT(chunk.m_size < m_elementsInChunk, "Too many elements in a chunk, something's wrong!");
			++chunk.m_size;
			++m_elementsTotal;
			return chunk.m_data + (elementIdx * m_elementSize);
		}

		inline void free(uint32_t _idx)
		{
			RTM_ASSERT(m_elementSize != 0, "Sparse pool not initialized!");

			const uint32_t chunkIdx		= _idx / m_elementsInChunk;
#if RTM_DEBUG
			const uint32_t elementIdx	= _idx & (m_elementsInChunk-1);
#endif

			RTM_ASSERT(chunkIdx < m_numChunks, "Out of bounds access!");

			Chunk& chunk = m_chunks[chunkIdx];
			RTM_ASSERT(chunk.m_size > 0, "Trying to free a non-allocated element!");
			--chunk.m_size;
			--m_elementsTotal;

#if RTM_DEBUG
			memset(chunk.m_data + (elementIdx * m_elementSize), 0xcd, m_elementSize);
#endif
			if (chunk.m_size == 0)
				freeChunk(chunk);
		}

		inline void* get(uint32_t _idx)
		{
			RTM_ASSERT(m_elementSize != 0, "Sparse pool not initialized!");

			const uint32_t chunkIdx		= _idx / m_elementsInChunk;
			const uint32_t elementIdx	= _idx & (m_elementsInChunk-1);

			RTM_ASSERT(chunkIdx < m_numChunks, "Out of bounds access!");
			Chunk& chunk = m_chunks[chunkIdx];
			RTM_ASSERT(chunk.m_data != 0, "Accessing a null chunk!");
			return chunk.m_data + (elementIdx * m_elementSize);
		}

		void releaseChunks()
		{
			for (uint32_t i=0; i<m_numChunks; ++i)
			{
				Chunk& c = m_chunks[i];
				if (c.m_data)
					freeChunk(c);
			}
			reallocateHeap(m_chunks, 0, 0);
		}

	private:
		void* reallocateHeap(void* _ptr, uint32_t _oldSize, uint32_t _newSize)
		{
			uint8_t* ptr = 0;
			if (m_memoryManager)
				ptr = (uint8_t*)m_memoryManager->realloc(_ptr, _newSize, m_alignment);
			else
			{
#if RTM_COMPILER_MSVC
				ptr = (uint8_t*)_aligned_realloc(_ptr, _newSize, m_alignment);
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
				ptr = (uint8_t*)memalign(m_alignment, _newSize);
				memcpy(ptr, _ptr, _oldSize);
#else
	#error "Unsupported compiler!"
#endif
			}

			if (_newSize > _oldSize)
				memset(ptr + _oldSize, 0, _newSize - _oldSize);
			return ptr;
		}

		void allocateChunk(Chunk& _c)
		{
			RTM_ASSERT(_c.m_data == 0, "Chunk must be null to be allocated!");
			RTM_ASSERT(_c.m_size == 0, "Chunk must be null to be allocated!");
			uint32_t block_size = RTM_ALIGNTO(m_elementSize * m_elementsInChunk, m_alignment);
			if (m_memoryManager)
				_c.m_data = (uint8_t*)m_memoryManager->alloc(block_size, m_alignment);
			else
			{
#if RTM_COMPILER_MSVC
				_c.m_data = (uint8_t*)_aligned_malloc(block_size, m_alignment);
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
				_c.m_data = (uint8_t*)memalign(m_alignment, block_size);
#else
	#error "Unsupported compiler!"
#endif
				
			}
#if RTM_DEBUG
			memset(_c.m_data, 0xcd, block_size);
#endif
		}

		void freeChunk(Chunk& _c)
		{
			RTM_ASSERT(_c.m_data != 0, "Chunk must be null to be freed!");
			if (m_memoryManager)
				m_memoryManager->free(_c.m_data);
			else
			{
#if RTM_COMPILER_MSVC
				_aligned_free(_c.m_data);
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
				::free(_c.m_data);
#else
	#error "Unsupported compiler!"
#endif
			}
			memset(&_c, 0, sizeof(Chunk));
		}
	};

} // namespace rtm

#endif // RTM_RBASE_SPARSEPOOL_H
