//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_HANDLE_LIST_H
#define RTM_RBASE_HANDLE_LIST_H

#include <rbase/inc/platform.h>
#include <rbase/inc/containers.h>
#include <rbase/inc/uint32_t.h>

namespace rtm {

	template <int IDX_BITS = 23, int GEN_BITS = 8>
	struct Handle
	{
		uint32_t	m_idx : IDX_BITS;
		uint32_t	m_gen : GEN_BITS;

		Handle(uint32_t _index, uint32_t _gen)
			: m_idx(_index)
			, m_gen(_gen)
		{}

		Handle(uint32_t _handle)
			: m_idx(_handle & ((1<<IDX_BITS) - 1))
			, m_gen((_handle >> IDX_BITS) & ((1<<GEN_BITS) - 1))
		{}

		uint32_t index() const
		{
			return m_idx;
		}

		uint32_t generation() const
		{
			return m_gen;
		}

		operator uint32_t() const
		{
			return (m_gen << IDX_BITS) | m_idx;
		}

		static uint32_t getIndex(uint32_t _handle)
		{
			Handle h(_handle);
			return h.index();
		}
	};

	template <uint32_t MAX_ELEMENTS>
	struct MinFreeIndices
	{
		enum { Val = MAX_ELEMENTS > 1024 ? 1024 : MAX_ELEMENTS / 2 };
	};

	template <
		uint32_t MAX_ELEMENTS,
		uint32_t INVALID_HANDLE = 0xffffffff,
		uint32_t MIN_FREE_INDICES = MinFreeIndices<MAX_ELEMENTS>::Val,
		uint32_t IDX_BITS = 23,
		uint32_t GEN_BITS = 8 
	>
	struct HandlePool
	{
		typedef Handle<IDX_BITS, GEN_BITS>	HandleType;

		FixedArray<uint8_t, MAX_ELEMENTS>	m_generation;
		FixedFifo<uint32_t, MAX_ELEMENTS>	m_freeIndices;
		uint32_t							m_size;

	public:
		HandlePool()
			: m_size(0)
		{}

		~HandlePool() 
		{
			RTM_WARN(m_size == 0, "Trying to destroy a non-empty handle allocator!");
		}

		uint32_t alloc()
		{
			RTM_ASSERT(!isFull(), "No free handles left!");

			uint32_t idx;
			if (m_freeIndices.size() > MIN_FREE_INDICES)
			{
				idx = m_freeIndices.pop_front();
			}
			else
			{
				if (m_generation.size() == MAX_ELEMENTS)
				{
					if (m_freeIndices.size() > 0)
						idx = m_freeIndices.pop_front();
					else
						return INVALID_HANDLE;
				}
				else
				{
					m_generation.push_back(0);
					idx = m_generation.size() - 1;
				}
				RTM_ASSERT(idx < (1 << IDX_BITS), "Index out of range!");
			}

			++m_size;
			return HandleType(idx, m_generation[idx]);
		}

		bool isValid(uint32_t _handle) const
		{
			if (_handle == INVALID_HANDLE)
				return false;

			const HandleType h(_handle);
			return m_generation[h.index()] == h.generation();
		}

		void free(uint32_t _handle)
		{
			RTM_ASSERT(m_size > 0, "Freeing a handle while no handles were allocated!");
			RTM_ASSERT(isValid(_handle), "Trying to free an invalid handle!");

			const HandleType h(_handle);
			const uint32_t idx = h.index();
			++m_generation[idx];
			m_freeIndices.push_back(idx);
			--m_size;
		}

		uint32_t generationFromIndex(uint32_t _index) const
		{
			return m_generation[_index];
		}

		uint32_t size() const
		{
			return m_size;
		}

		bool isFull() const
		{
			return m_size == MAX_ELEMENTS;
		}
	};

} // namespace rtm

#endif // RTM_RBASE_HANDLE_LIST_H
