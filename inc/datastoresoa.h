//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_DATA_STORE_SOA_H
#define RTM_RBASE_DATA_STORE_SOA_H

#include <rbase/inc/datastore.h>

namespace rtm {

	template <	typename T1,
				typename T2,
				int NUM_ELEMENTS = (1<<16)>
	struct DataSOA2 : public DataBase<NUM_ELEMENTS, Storage::Dense>
	{
		typedef DataBase<NUM_ELEMENTS, Storage::Dense> Base;

		T1	m_dataT1[NUM_ELEMENTS];
		T2	m_dataT2[NUM_ELEMENTS];

		DataSOA2(bool _clearData = false)
		{
			if (_clearData)
			{
				memset(m_dataT1, 0, sizeof(T1) * NUM_ELEMENTS);
				memset(m_dataT2, 0, sizeof(T2) * NUM_ELEMENTS);
			}
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx, lastDataIdx;
			Base::Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (dataIdx != lastDataIdx)
			{
				m_dataT1[dataIdx] = m_dataT1[lastDataIdx];
				m_dataT2[dataIdx] = m_dataT2[lastDataIdx];
			}
		}
	};


	template <	typename T1,
				typename T2,
				typename T3,
				int NUM_ELEMENTS = (1<<16)>
	struct DataSOA3 : public DataBase<NUM_ELEMENTS, Storage::Dense>
	{
		typedef DataBase<NUM_ELEMENTS, Storage::Dense> Base;

		T1	m_dataT1[NUM_ELEMENTS];
		T2	m_dataT2[NUM_ELEMENTS];
		T3	m_dataT3[NUM_ELEMENTS];

		DataSOA3(bool _clearData = false)
		{
			if (_clearData)
			{
				memset(m_dataT1, 0, sizeof(T1) * NUM_ELEMENTS);
				memset(m_dataT2, 0, sizeof(T2) * NUM_ELEMENTS);
				memset(m_dataT3, 0, sizeof(T3) * NUM_ELEMENTS);
			}
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx;
			uint32_t lastDataIdx;
			Base::Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (dataIdx != lastDataIdx)
			{
				m_dataT1[dataIdx] = m_dataT1[lastDataIdx];
				m_dataT2[dataIdx] = m_dataT2[lastDataIdx];
				m_dataT3[dataIdx] = m_dataT3[lastDataIdx];
			}
		}
	};

	template <	typename T1,
				typename T2,
				typename T3,
				typename T4,
				int NUM_ELEMENTS = (1<<16)>
	struct DataSOA4 : public DataBase<NUM_ELEMENTS, Storage::Dense>
	{
		typedef DataBase<NUM_ELEMENTS, Storage::Dense> Base;

		T1	m_dataT1[NUM_ELEMENTS];
		T2	m_dataT2[NUM_ELEMENTS];
		T3	m_dataT3[NUM_ELEMENTS];
		T4	m_dataT4[NUM_ELEMENTS];

		DataSOA4(bool _clearData = false)
		{
			if (_clearData)
			{
				memset(m_dataT1, 0, sizeof(T1) * NUM_ELEMENTS);
				memset(m_dataT2, 0, sizeof(T2) * NUM_ELEMENTS);
				memset(m_dataT3, 0, sizeof(T3) * NUM_ELEMENTS);
				memset(m_dataT4, 0, sizeof(T4) * NUM_ELEMENTS);
			}
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx;
			uint32_t lastDataIdx;
			Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (dataIdx != lastDataIdx)
			{
				m_dataT1[dataIdx] = m_dataT1[lastDataIdx];
				m_dataT2[dataIdx] = m_dataT2[lastDataIdx];
				m_dataT3[dataIdx] = m_dataT3[lastDataIdx];
				m_dataT4[dataIdx] = m_dataT4[lastDataIdx];
			}
		}
	};

	template <	typename T1,
				typename T2,
				typename T3,
				typename T4,
				typename T5,
				int NUM_ELEMENTS = (1<<16)>
	struct DataSOA5 : public DataBase<NUM_ELEMENTS, Storage::Dense>
	{
		typedef DataBase<NUM_ELEMENTS, Storage::Dense> Base;

		T1	m_dataT1[NUM_ELEMENTS];
		T2	m_dataT2[NUM_ELEMENTS];
		T3	m_dataT3[NUM_ELEMENTS];
		T4	m_dataT4[NUM_ELEMENTS];
		T5	m_dataT5[NUM_ELEMENTS];

		DataSOA5(bool _clearData = false)
		{
			if (_clearData)
			{
				memset(m_dataT1, 0, sizeof(T1) * NUM_ELEMENTS);
				memset(m_dataT2, 0, sizeof(T2) * NUM_ELEMENTS);
				memset(m_dataT3, 0, sizeof(T3) * NUM_ELEMENTS);
				memset(m_dataT4, 0, sizeof(T4) * NUM_ELEMENTS);
				memset(m_dataT5, 0, sizeof(T5) * NUM_ELEMENTS);
			}
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx;
			uint32_t lastDataIdx;
			Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (dataIdx != lastDataIdx)
			{
				m_dataT1[dataIdx] = m_dataT1[lastDataIdx];
				m_dataT2[dataIdx] = m_dataT2[lastDataIdx];
				m_dataT3[dataIdx] = m_dataT3[lastDataIdx];
				m_dataT4[dataIdx] = m_dataT4[lastDataIdx];
				m_dataT5[dataIdx] = m_dataT5[lastDataIdx];
			}
		}
	};

	template <	typename T1,
				typename T2,
				typename T3,
				typename T4,
				typename T5,
				typename T6,
				int NUM_ELEMENTS = (1<<16)>
	struct DataSOA6 : public DataBase<NUM_ELEMENTS, Storage::Dense>
	{
		typedef DataBase<NUM_ELEMENTS, Storage::Dense> Base;

		T1	m_dataT1[NUM_ELEMENTS];
		T2	m_dataT2[NUM_ELEMENTS];
		T3	m_dataT3[NUM_ELEMENTS];
		T4	m_dataT4[NUM_ELEMENTS];
		T5	m_dataT5[NUM_ELEMENTS];
		T6	m_dataT6[NUM_ELEMENTS];

		DataSOA6(bool _clearData = false)
		{
			if (_clearData)
			{
				memset(m_dataT1, 0, sizeof(T1) * NUM_ELEMENTS);
				memset(m_dataT2, 0, sizeof(T2) * NUM_ELEMENTS);
				memset(m_dataT3, 0, sizeof(T3) * NUM_ELEMENTS);
				memset(m_dataT4, 0, sizeof(T4) * NUM_ELEMENTS);
				memset(m_dataT5, 0, sizeof(T5) * NUM_ELEMENTS);
				memset(m_dataT6, 0, sizeof(T6) * NUM_ELEMENTS);
			}
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx;
			uint32_t lastDataIdx;
			Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (dataIdx != lastDataIdx)
			{
				m_dataT1[dataIdx] = m_dataT1[lastDataIdx];
				m_dataT2[dataIdx] = m_dataT2[lastDataIdx];
				m_dataT3[dataIdx] = m_dataT3[lastDataIdx];
				m_dataT4[dataIdx] = m_dataT4[lastDataIdx];
				m_dataT5[dataIdx] = m_dataT5[lastDataIdx];
				m_dataT6[dataIdx] = m_dataT6[lastDataIdx];
			}
		}
	};

	template <	typename T1,
				typename T2,
				typename T3,
				typename T4,
				typename T5,
				typename T6,
				typename T7,
				int NUM_ELEMENTS = (1<<16)>
	struct DataSOA7 : public DataBase<NUM_ELEMENTS, Storage::Dense>
	{
		typedef DataBase<NUM_ELEMENTS, Storage::Dense> Base;

		T1	m_dataT1[NUM_ELEMENTS];
		T2	m_dataT2[NUM_ELEMENTS];
		T3	m_dataT3[NUM_ELEMENTS];
		T4	m_dataT4[NUM_ELEMENTS];
		T5	m_dataT5[NUM_ELEMENTS];
		T6	m_dataT6[NUM_ELEMENTS];
		T7	m_dataT7[NUM_ELEMENTS];

		DataSOA7(bool _clearData = false)
		{
			if (_clearData)
			{
				memset(m_dataT1, 0, sizeof(T1) * NUM_ELEMENTS);
				memset(m_dataT2, 0, sizeof(T2) * NUM_ELEMENTS);
				memset(m_dataT3, 0, sizeof(T3) * NUM_ELEMENTS);
				memset(m_dataT4, 0, sizeof(T4) * NUM_ELEMENTS);
				memset(m_dataT5, 0, sizeof(T5) * NUM_ELEMENTS);
				memset(m_dataT6, 0, sizeof(T6) * NUM_ELEMENTS);
				memset(m_dataT7, 0, sizeof(T7) * NUM_ELEMENTS);
			}
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx;
			uint32_t lastDataIdx;
			Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (dataIdx != lastDataIdx)
			{
				m_dataT1[dataIdx] = m_dataT1[lastDataIdx];
				m_dataT2[dataIdx] = m_dataT2[lastDataIdx];
				m_dataT3[dataIdx] = m_dataT3[lastDataIdx];
				m_dataT4[dataIdx] = m_dataT4[lastDataIdx];
				m_dataT5[dataIdx] = m_dataT5[lastDataIdx];
				m_dataT6[dataIdx] = m_dataT6[lastDataIdx];
				m_dataT7[dataIdx] = m_dataT7[lastDataIdx];
			}
		}
	};

	template <	typename T1,
				typename T2,
				typename T3,
				typename T4,
				typename T5,
				typename T6,
				typename T7,
				typename T8,
				int NUM_ELEMENTS = (1<<16)>
	struct DataSOA8 : public DataBase<NUM_ELEMENTS, Storage::Dense>
	{
		typedef DataBase<NUM_ELEMENTS, Storage::Dense> Base;

		T1	m_dataT1[NUM_ELEMENTS];
		T2	m_dataT2[NUM_ELEMENTS];
		T3	m_dataT3[NUM_ELEMENTS];
		T4	m_dataT4[NUM_ELEMENTS];
		T5	m_dataT5[NUM_ELEMENTS];
		T6	m_dataT6[NUM_ELEMENTS];
		T7	m_dataT7[NUM_ELEMENTS];
		T8	m_dataT8[NUM_ELEMENTS];

		DataSOA8(bool _clearData = false)
		{
			if (_clearData)
			{
				memset(m_dataT1, 0, sizeof(T1) * NUM_ELEMENTS);
				memset(m_dataT2, 0, sizeof(T2) * NUM_ELEMENTS);
				memset(m_dataT3, 0, sizeof(T3) * NUM_ELEMENTS);
				memset(m_dataT4, 0, sizeof(T4) * NUM_ELEMENTS);
				memset(m_dataT5, 0, sizeof(T5) * NUM_ELEMENTS);
				memset(m_dataT6, 0, sizeof(T6) * NUM_ELEMENTS);
				memset(m_dataT7, 0, sizeof(T7) * NUM_ELEMENTS);
				memset(m_dataT8, 0, sizeof(T8) * NUM_ELEMENTS);
			}
		}

		void free(uint32_t _handle)
		{
			uint32_t dataIdx;
			uint32_t lastDataIdx;
			Base::m_allocator.free(_handle, dataIdx, lastDataIdx);

			if (dataIdx != lastDataIdx)
			{
				m_dataT1[dataIdx] = m_dataT1[lastDataIdx];
				m_dataT2[dataIdx] = m_dataT2[lastDataIdx];
				m_dataT3[dataIdx] = m_dataT3[lastDataIdx];
				m_dataT4[dataIdx] = m_dataT4[lastDataIdx];
				m_dataT5[dataIdx] = m_dataT5[lastDataIdx];
				m_dataT6[dataIdx] = m_dataT6[lastDataIdx];
				m_dataT7[dataIdx] = m_dataT7[lastDataIdx];
				m_dataT8[dataIdx] = m_dataT8[lastDataIdx];
			}
		}
	};

} // namespace rtm

#endif // RTM_RBASE_DATA_STORE_SOA_H
