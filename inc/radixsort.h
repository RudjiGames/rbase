//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//
/// Taken from bx library: https://github.com/bkaradzic/bx                 ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_RADIXSORT_H
#define RTM_RBASE_RADIXSORT_H

#include <rbase/inc/platform.h>

namespace rtm {

	constexpr uint32_t RTM_RADIXSORT_BITS			= 11;
	constexpr uint32_t RTM_RADIXSORT_HISTOGRAM_SIZE = (1 << RTM_RADIXSORT_BITS);
	constexpr uint32_t RTM_RADIXSORT_BIT_MASK		= (RTM_RADIXSORT_HISTOGRAM_SIZE - 1);

	/// Sorts an array using radix sort
	///
	/// @param[in,out] _keys	: Keys for sorting
	/// @param[in] _tempKeys	: Temporary buffer for key sorting
	/// @param[in] _size		: Number of values to sort
	void sortRadix(uint32_t* _keys, uint32_t* _tempKeys, uint32_t _size);

	/// Sorts an array using radix sort
	///
	/// @param[in,out] _keys	: Keys for sorting
	/// @param[in] _tempKeys	: Temporary buffer for key sorting
	/// @param[in] _size		: Number of values to sort
	void sortRadix64(uint64_t* _keys, uint64_t* _tempKeys, uint32_t _size);

	/// Sorts a value array using radix sort
	///
	/// @param[in,out] _keys	: Keys for sorting
	/// @param[in] _tempKeys	: Temporary buffer for key sorting
	/// @param[in,out] _values	: Values to sort
	/// @param[in] _tempValues	: Temporary buffer for values
	/// @param[in] _size		: Number of values to sort
	template <typename Ty>
	static inline void sortRadix(uint32_t* _keys, uint32_t* _tempKeys, Ty* _values, Ty* _tempValues, uint32_t _size)
	{
		uint16_t histogram[RTM_RADIXSORT_HISTOGRAM_SIZE];
		uint16_t shift = 0;
		for (uint32_t pass=0; pass<3; ++pass)
		{
			memSet(histogram, 0, sizeof(uint16_t)*RTM_RADIXSORT_HISTOGRAM_SIZE);
			for (uint32_t i=0; i<_size; ++i)
			{
				uint32_t key = _keys[i];
				uint16_t index = (key >> shift) & RTM_RADIXSORT_BIT_MASK;
				++histogram[index];
			}

			uint16_t offset = 0;
			for (uint32_t i=0; i<RTM_RADIXSORT_HISTOGRAM_SIZE; ++i)
			{
				uint16_t count = histogram[i];
				histogram[i] = offset;
				offset += count;
			}

			for (uint32_t i=0; i<_size; ++i)
			{
				uint32_t key = _keys[i];
				uint16_t index = (key>>shift)&RTM_RADIXSORT_BIT_MASK;
				uint16_t dest = histogram[index]++;
				_tempKeys[dest] = key;
				_tempValues[dest] = _values[i];
			}

			uint32_t* swapKeys = _tempKeys;
			_tempKeys = _keys;
			_keys = swapKeys;

			Ty* swapValues = _tempValues;
			_tempValues = _values;
			_values = swapValues;

			shift += RTM_RADIXSORT_BITS;
		}
	}

	/// Sorts a value array using radix sort
	///
	/// @param[in,out] _keys	: Keys for sorting
	/// @param[out] _tempKeys	: Temporary buffer for key sorting
	/// @param[in] _values		: Values to sort
	/// @param[in] _tempValues	: Temporary buffer for values
	/// @param[in] _size		: Number of values to sort
	template <typename Ty>
	static inline void sortRadix64(uint64_t* _keys, uint64_t* _tempKeys, Ty* _values, Ty* _tempValues, uint32_t _size)
	{
		uint16_t histogram[RTM_RADIXSORT_HISTOGRAM_SIZE];
		uint16_t shift = 0;
		for (uint32_t pass=0; pass<6; ++pass)
		{
			memSet(histogram, 0, sizeof(uint16_t)*RTM_RADIXSORT_HISTOGRAM_SIZE);
			for (uint32_t i=0; i<_size; ++i)
			{
				uint64_t key = _keys[i];
				uint16_t index = (key>>shift)&RTM_RADIXSORT_BIT_MASK;
				++histogram[index];
			}

			uint16_t offset = 0;
			for (uint32_t i=0; i<RTM_RADIXSORT_HISTOGRAM_SIZE; ++i)
			{
				uint16_t count = histogram[i];
				histogram[i] = offset;
				offset += count;
			}

			for (uint32_t i=0; i<_size; ++i)
			{
				uint64_t key = _keys[i];
				uint16_t index = (key >> shift) & RTM_RADIXSORT_BIT_MASK;
				uint16_t dest = histogram[index]++;
				_tempKeys[dest] = key;
				_tempValues[dest] = _values[i];
			}

			uint64_t* swapKeys = _tempKeys;
			_tempKeys = _keys;
			_keys = swapKeys;

			Ty* swapValues = _tempValues;
			_tempValues = _values;
			_values = swapValues;

			shift += RTM_RADIXSORT_BITS;
		}
	}

} // namespace rtm

#endif // RTM_RBASE_RADIXSORT_H
