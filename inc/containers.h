//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_FREE_LIST_H
#define RTM_RBASE_FREE_LIST_H

#include <rbase/inc/platform.h>

namespace rtm {

	template <int32_t BLOCK_SIZE, int32_t BLOCK_COUNT>
	struct FreeList
	{
	private:
		struct Block
		{
			union
			{
				int32_t m_nextFree;
				int8_t	m_memory[BLOCK_SIZE];
			};
		};

		Block	m_blocks[BLOCK_COUNT];
		int32_t m_nextFreeBlock;

	public:
		FreeList()
		{
			m_nextFreeBlock = 0;

			for (int i = 0; i<BLOCK_COUNT - 1; i++)
				m_blocks[i].m_nextFree = i + 1;

			m_blocks[BLOCK_COUNT - 1].m_nextFree = -1;
		}
	
		inline bool	isInPool(void* _ptr) const
		{
			return (int32_t)(reinterpret_cast<Block*>(_ptr) - m_blocks) < BLOCK_COUNT;
		}

		inline void* alloc()
		{
			if (m_nextFreeBlock == -1)
				return 0;

			Block* b = &m_blocks[m_nextFreeBlock];
			m_nextFreeBlock = b->m_nextFree;
			return b;
		}

		inline void free(void* _ptr)
		{
			Block* toFree = reinterpret_cast<Block*>(_ptr);
			toFree->m_nextFree = m_nextFreeBlock;
			m_nextFreeBlock = toFree - m_blocks;
		}
	};

	template <typename T, int NUM_ELEMENTS>
	struct FixedArray
	{
		T			m_data[NUM_ELEMENTS];
		uint32_t	m_size;

		FixedArray()
			: m_size(0)
		{}

		void push_back(T _e)
		{
			RTM_ASSERT(m_size < NUM_ELEMENTS, "Array is full!");
			m_data[m_size++] = _e;
		}

		T pop_back()
		{
			RTM_ASSERT(m_size > 0, "Array is empty!");
			return m_data[m_size--];
		}

		const T& operator[] (uint32_t _idx) const
		{
			RTM_ASSERT(_idx < m_size, "Out of bounds access!");
			return m_data[_idx];
		}
		
		T& operator[] (uint32_t _idx)
		{
			RTM_ASSERT(_idx < m_size, "Out of bounds access!");
			return m_data[_idx];
		}

		void clear()
		{
			m_size = 0;
		}

		uint32_t size() const
		{
			return m_size;
		}

		bool isEmpty() const
		{
			return m_size == 0;
		}
		
		bool isFull() const
		{
			return m_size == NUM_ELEMENTS;
		}	
	};

	template <typename T, int NUM_ELEMENTS>
	struct FixedFifo
	{
		T		m_data[NUM_ELEMENTS];
		int32_t	m_front;
		int32_t	m_size;

		FixedFifo()
			: m_front(0)
			, m_size(0)
		{
			RTM_ASSERT(((NUM_ELEMENTS & (NUM_ELEMENTS-1)) == 0) &&
						(NUM_ELEMENTS != 0), "NUM_ELEMENTS must be a power of two!");
		}

		void push_back(T _e)
		{
			RTM_ASSERT(m_size < NUM_ELEMENTS, "Queue is full!");
			const int16_t idx = (m_front + m_size) & (NUM_ELEMENTS - 1);
			m_data[idx] = _e;
			++m_size;
		}

		T pop_front()
		{
			RTM_ASSERT(m_size > 0, "Queue is empty!");
			m_front &= NUM_ELEMENTS - 1;
			--m_size;
			return m_data[m_front++];
		}

		uint32_t size() const
		{
			return m_size;
		}

		bool isEmpty() const
		{
			return m_size == 0;
		}
		
		bool isFull() const
		{
			return m_size == NUM_ELEMENTS;
		}
	};	

} // namespace rtm

#endif // RTM_RBASE_FREE_LIST_H

