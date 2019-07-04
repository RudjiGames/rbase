//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_DATA_STORE_H
#define RTM_RBASE_DATA_STORE_H

#include <rbase/inc/platform.h>
#include <rbase/inc/handlepool.h>

namespace rtm {

	template <int NUM_ELEMENTS = (1<<16)>
	class IndexRemap
	{
		uint32_t	m_structIdx[NUM_ELEMENTS];
		uint32_t	m_handleIdx[NUM_ELEMENTS];

	public:
		IndexRemap()
		{
			reset();
		}

		uint32_t getStructIndex(uint32_t _handleIndex) const
		{
			return m_structIdx[_handleIndex];
		}

		uint32_t getHandleIndex(uint32_t _structIndex) const
		{
			return m_handleIdx[_structIndex];
		}

		void map(uint32_t _structIndex, uint32_t _handleIndex)
		{
			m_structIdx[_handleIndex] = _structIndex;
			m_handleIdx[_structIndex] = _handleIndex;
		}

		void reset()
		{
			for (uint32_t i=0; i<NUM_ELEMENTS; ++i)
			{
				m_structIdx[i] = i;
				m_handleIdx[i] = i;
			}
		}
	};

	template <int NUM_ELEMENTS = (1<<16)>
	struct HandlePolicyBase
	{
		HandlePool<NUM_ELEMENTS>	m_handles;

		uint32_t dataIndexToHandle(uint32_t _index) const
		{
			const uint32_t gen = m_handles.generationFromIndex(_index);
			Handle<> handle(_index, gen);
			return handle;
		}

		uint32_t size() const
		{
			return m_handles.size();
		}

		bool isValid(uint32_t _handle) const
		{
			return m_handles.isValid(_handle);
		}
	};

	template <int NUM_ELEMENTS = (1<<16)>
	struct HandlePolicySparse : public HandlePolicyBase<NUM_ELEMENTS>
	{
		typedef HandlePolicyBase<NUM_ELEMENTS> Base;

		uint32_t allocate()
		{
			return Base::m_handles.alloc();
		}

		void free(uint32_t _handle, uint32_t& _dataIdx, uint32_t& _lastDataIdx)
		{
			_dataIdx		= 0;
			_lastDataIdx	= 0;
			Base::m_handles.free(_handle);
		}

		uint32_t getDataIndex(uint32_t _handle) const
		{
			RTM_ASSERT(Base::m_handles.isValid(_handle), "Invalid handle passed!");
			const Handle<> h(_handle);
			return h.index();
		}

		bool isDense() const
		{
			return false;
		}
	};
	
	template <int NUM_ELEMENTS = (1<<16)>
	struct HandlePolicyDense : public HandlePolicyBase<NUM_ELEMENTS>
	{
		typedef HandlePolicyBase<NUM_ELEMENTS> Base;

		IndexRemap<NUM_ELEMENTS>	m_remap;

		uint32_t allocate()
		{
			const uint32_t dataIdx = Base::m_handles.size();
			Handle<> handle(Base::m_handles.alloc());
			m_remap.map(dataIdx, handle.index());
			return handle;
		}

		void free(uint32_t _handle, uint32_t& _dataIdx, uint32_t& _lastDataIdx)
		{
			_lastDataIdx = Base::m_handles.size() - 1;
			const uint32_t lastHandleIndex = m_remap.getHandleIndex(_lastDataIdx);
			
			const Handle<> h(_handle);
			const uint32_t idx = h.index();
			_dataIdx = m_remap.getStructIndex(idx);

			if (_dataIdx != _lastDataIdx)
				m_remap.map(_dataIdx, lastHandleIndex);

			Base::m_handles.free(_handle);
		}

		uint32_t getDataIndex(uint32_t _handle) const
		{
			RTM_ASSERT(Base::m_handles.isValid(_handle), "Invalid handle passed!");
			const Handle<> h(_handle);
			const uint32_t idx = h.index();
			return m_remap.getStructIndex(idx);
		}

		bool isDense() const
		{
			return true;
		}
	};

	struct Storage
	{
		enum Enum
		{
			Sparse,
			Dense
		};
	};

	template <int NUM_ELEMENTS, Storage::Enum STORE_POLICY = Storage::Sparse>
	struct HandlePolicySelector
	{
		typedef rtm::HandlePolicySparse<NUM_ELEMENTS>	HandlePolicy;
	};

	template <int NUM_ELEMENTS>
	struct HandlePolicySelector<NUM_ELEMENTS, Storage::Dense>
	{
		typedef rtm::HandlePolicyDense<NUM_ELEMENTS>	HandlePolicy;
	};

	template <int NUM_ELEMENTS = (1<<16), Storage::Enum STORE_POLICY = Storage::Sparse>
	class DataBase
	{
	protected:
		typedef typename HandlePolicySelector<NUM_ELEMENTS, STORE_POLICY>::HandlePolicy	HandleAlloc;

		HandleAlloc	m_allocator;

	public:
		uint32_t allocate()
		{
			return m_allocator.allocate();
		}

		uint32_t size() const
		{
			return m_allocator.size();
		}

		bool isValid(uint32_t _handle) const
		{
			return m_allocator.isValid(_handle);
		}

		uint32_t dataIndexToHandle(uint32_t _index) const
		{
			return m_allocator.dataIndexToHandle(_index);
		}
	};

	template <typename T, int NUM_ELEMENTS = (1<<16), Storage::Enum STORE_POLICY = Storage::Sparse>
	class Data : public DataBase<NUM_ELEMENTS, STORE_POLICY>
	{
		typedef DataBase<NUM_ELEMENTS, STORE_POLICY> Base;

		T	m_data[NUM_ELEMENTS];

	public:
		Data(bool _clearData = false)
		{
			if (_clearData)
				memset(m_data, 0, sizeof(T) * NUM_ELEMENTS);
		}

		uint32_t allocate()
		{
			return DataBase<NUM_ELEMENTS, STORE_POLICY>::allocate();
		}

		uint32_t allocate(T*& _storedData)
		{
			_storedData = 0;
			uint32_t index = Base::m_allocator.allocate();
			if (Base::isValid(index))
			{
				_storedData = &m_data[index];
			}
			return index;
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx;
			uint32_t lastDataIdx;
			Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (Base::m_allocator.isDense()) // compile time opt?
			if (dataIdx != lastDataIdx)
			{
				m_data[dataIdx] = m_data[lastDataIdx];
			}
		}

		T getData(uint32_t _handle)
		{
			const uint32_t dataIdx = Base::m_allocator.getDataIndex(_handle);
			return m_data[dataIdx];
		}

		void setData(uint32_t _handle, T _data)
		{
			const uint32_t dataIdx = Base::m_allocator.getDataIndex(_handle);
			m_data[dataIdx] = _data;
		}

		T* getDataPtr(uint32_t _handle)
		{
			const uint32_t dataIdx = Base::m_allocator.getDataIndex(_handle);
			return &m_data[dataIdx];
		}

		T* getDataPtrSafe(uint32_t _handle)
		{
			if (!Base::isValid(_handle))
				return 0;

			const uint32_t dataIdx = Base::m_allocator.getDataIndex(_handle);
			return &m_data[dataIdx];
		}

		T getDataIndexed(uint32_t _index)
		{
			RTM_ASSERT(_index < Base::size(), "Out of bounds access!");
			RTM_ASSERT(Base::m_allocator.isDense(), "Operator based access allowed only on dense data store!");
			return m_data[_index];
		}

		T* getDataIndexedPtr(uint32_t _index)
		{
			RTM_ASSERT(_index < Base::size(), "Out of bounds access!");
			RTM_ASSERT(Base::m_allocator.isDense(), "Operator based access allowed only on dense data store!");
			return &m_data[_index];
		}
	};

} // namespace rtm

#endif // RTM_RBASE_DATA_STORE_H
