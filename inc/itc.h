//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_ITC_H
#define RTM_RBASE_ITC_H

#include <rbase/inc/thread.h>
#include <rbase/inc/semaphore.h>
#include <string.h> // memcpy
#include <math.h>
#include <atomic>

namespace rtm {

	template <class T = uintptr_t>
	class SPSCBuffer
	{
		T*					m_buffer;
		std::atomic<int>	m_read;
		std::atomic<int>	m_write;
		int					m_capacity;
		int					m_size;
		int					m_size_mask;

	public:
		SPSCBuffer(int _capacity)
			: m_read(0)
			, m_write(0)
			, m_capacity(_capacity)
		{
			int power = floor(log(_capacity)/log(2)) + 1;
			m_size		= 1 << power;
			m_size_mask	= m_size - 1;
			m_buffer	= new T[m_size];
		}

		~SPSCBuffer()
		{
			delete[] m_buffer;
		}

		bool write(const T& _item);
		bool read(T* _item);

		template <typename U>
		bool write(U const& data)
		{
			RTM_ASSERT(sizeof(U) <= sizeof(T), "");
			union { T val; U arg; } u;
			u.val = 0;
			u.arg = data;
			return write(u.val);
		}

		template <typename U>
		bool read(U* data)
		{
			RTM_ASSERT(sizeof(U) <= sizeof(T), "");
			union { T val; U arg; } u;
			if (read(&u.val))
			{
				*data = u.arg;
				return true;
			}
			return false;
		}
	};

	template <class T>
	bool SPSCBuffer<T>::write(const T& _item)
	{
		const int w = m_write.load(std::memory_order_relaxed);
		const int r = m_read.load(std::memory_order_acquire);
		const int nitems = w - r;
		if (nitems == m_capacity || nitems + m_size == m_capacity)
			return false;

		m_buffer[w] = _item;
		m_write.store((w+1) & m_size_mask, std::memory_order_release);
		return true;
	}

	template <class T>
	bool SPSCBuffer<T>::read(T* _item)
	{
		const int r = m_read.load(std::memory_order_relaxed);
		const int w = m_write.load(std::memory_order_acquire);
		if (r == w)
			return false;

		*_item = m_buffer[r];
		m_read.store((r+1) & m_size_mask, std::memory_order_release);
		return true;
	}

	class RWBuffer
	{
		uint8_t*	m_buffer;
		uint32_t	m_pos;
		uint32_t	m_size;
		uint32_t	m_capacity;

		RTM_CLASS_NO_COPY(RWBuffer)

	public:
		RWBuffer()
			: m_buffer(0)
			, m_pos(0)
			, m_size(0)
			, m_capacity(0)
		{
		}

		~RWBuffer()
		{
			delete[] m_buffer;
		}

		void setCapacity(uint32_t _capacity)
		{
			m_buffer	= (uint8_t*)new uint8_t[_capacity];
			m_capacity	= _capacity;
		}
		
		void start()
		{
			m_pos	= 0;
			m_size	= m_capacity;
		}

		void finish()
		{
			m_size	= m_pos;
			m_pos	= 0;
		}

		bool spaceAvailable() const
		{
			return m_pos < m_size;
		}

		void write(const void* _data, uint32_t _size)
		{
			RTM_ASSERT(m_size == m_capacity, "Write called outside of start/finish!");
			RTM_ASSERT(m_pos + _size <= m_size, "Command buffer is full!");
			memcpy(&m_buffer[m_pos], _data, _size);
			m_pos += _size;
		}

		template <typename T>
		void write(const T& _value)
		{
			write((const char*)(&_value), sizeof(T));
		}

		bool dataAvailable() const
		{
			return m_pos < m_size;
		}

		void read(void* _data, uint32_t _size)
		{
			RTM_ASSERT(m_pos + _size <= m_size, "Trying to read beyond command buffer end!");
			memcpy(_data, &m_buffer[m_pos], _size);
			m_pos += _size;
		}

		template <typename T>
		void read(T& _value)
		{
			read((char*)&_value, sizeof(T));
		}

		const uint8_t* skip(uint32_t _size)
		{
			RTM_ASSERT(m_pos + _size <= m_size, "Trying to read/write beyond command buffer end!");
			const uint8_t* result = &m_buffer[m_pos];
			m_pos += _size;
			return result;
		}

		template <typename T>
		void skip()
		{
			skip(sizeof(T));
		}
	};

	class CommandBuffer
	{
		RWBuffer		m_commandBuffers[2];
		RWBuffer*		m_cmdBufferProduce;
		RWBuffer*		m_cmdBufferConsume;
		Semaphore		m_semConsume;
		Semaphore		m_semProduce;
		Thread			m_appThread;
		bool			m_runThread;
		ThreadEntry		m_consumerFunc;

	public:
		CommandBuffer(uint32_t _bufferSize = 32*1024);
		~CommandBuffer();

		void init(rtm::ThreadEntry _consumerFunc);
		void frame();
		void shutDown();

		template <typename T>
		void write(const T& _value)
		{
			m_cmdBufferProduce->write<T>(_value);
		}

		template <typename T>
		void read(T& _value)
		{
			m_cmdBufferConsume->read<T>(_value);
		}

		bool dataAvailable() const
		{
			return m_cmdBufferConsume->dataAvailable();
		}

		bool spacevailable() const
		{
			return m_cmdBufferProduce->dataAvailable();
		}

	private:
		static int32_t consumerThread(void* _userData);
	};

} // namespace rtm

#endif // RTM_RBASE_ITC_H
