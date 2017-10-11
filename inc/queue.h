//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_QUEUE_H
#define RTM_RBASE_QUEUE_H

#ifndef RBASE_NAMESPACE
#define RBASE_NAMESPACE rtm
#endif // RBASE_NAMESPACE

#include <rbase/inc/libhandler.h>
#include <atomic>

namespace rtm {

// Taken from http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue

template <typename T>
class MpMcQueue
{
private:
	struct cell_t
	{
		std::atomic<size_t>   m_sequence;
		T                     m_data;
	};

	static size_t const     cacheline_size = 64;
	typedef char            cacheline_pad_t[cacheline_size];

	cacheline_pad_t         m_pad0;
	cell_t* const           m_buffer;
	size_t const            m_buffer_mask;
	cacheline_pad_t         m_pad1;
	std::atomic<size_t>     m_enqueue_pos;
	cacheline_pad_t         m_pad2;
	std::atomic<size_t>     m_dequeue_pos;
	cacheline_pad_t         m_pad3;
	size_t					m_bufferSize;

	RTM_CLASS_NO_COPY(MpMcQueue);

public:
	MpMcQueue(size_t _bufferSize)
		: m_buffer(rtm_new_array<cell_t>(_bufferSize))
		, m_buffer_mask(_bufferSize - 1)
		, m_bufferSize(_bufferSize)
	{
		RTM_ASSERT((m_bufferSize >= 2) && ((m_bufferSize & (m_bufferSize - 1)) == 0), "");

		for (size_t i=0; i!=m_bufferSize; ++i)
			m_buffer[i].m_sequence.store(i, std::memory_order_relaxed);

		m_enqueue_pos.store(0, std::memory_order_relaxed);
		m_dequeue_pos.store(0, std::memory_order_relaxed);
	}

	~MpMcQueue()
	{
		rtm_delete_array<cell_t>(m_bufferSize, m_buffer);
	}

	bool enqueue(T const& data)
	{
		cell_t* cell;
		size_t pos = m_enqueue_pos.load(std::memory_order_relaxed);
		for (;;)
		{
			cell = &m_buffer[pos & m_buffer_mask];
			size_t seq = cell->m_sequence.load(std::memory_order_acquire);
			intptr_t dif = (intptr_t)seq - (intptr_t)pos;
			if (dif == 0)
			{
				if (m_enqueue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
					break;
			}
			else
				if (dif < 0)
					return false;
				else
					pos = m_enqueue_pos.load(std::memory_order_relaxed);
		}
		cell->m_data = data;
		cell->m_sequence.store(pos + 1, std::memory_order_release);
		return true;
	}

	bool dequeue(T& data)
	{
		cell_t* cell;
		size_t pos = m_dequeue_pos.load(std::memory_order_relaxed);
		for (;;)
		{
			cell = &m_buffer[pos & m_buffer_mask];
			size_t seq = cell->m_sequence.load(std::memory_order_acquire);
			intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);
			if (dif == 0)
			{
				if (m_dequeue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
					break;
			}
			else
				if (dif < 0)
					return false;
				else
					pos = m_dequeue_pos.load(std::memory_order_relaxed);
		}
		data = cell->m_data;
		cell->m_sequence.store(pos + m_buffer_mask + 1, std::memory_order_release);
		return true;
	}

	template <typename U>
	bool enqueue(U const& data)
	{
		union {
			T val;
			U arg;
		} u;
		u.arg = data;
		return enqueue(u.val);
	}

	template <typename U>
	bool dequeue(U& data)
	{
		union {
			T val;
			U arg;
		} u;
		if (dequeue(u.val))
		{
			data = u.arg;
			return true;
		}
		return false;
	}
};

} // namespace rtm

#endif // RTM_RBASE_QUEUE_H
