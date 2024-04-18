//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_FLAT_OBJECT_H
#define RTM_FLAT_OBJECT_H

#include <rbase/inc/platform.h>

namespace rtm {

	template <typename T>
	class FixupPtr
	{
		int32_t		m_offset;

	public:
		FixupPtr(int32_t _offset = 0)
			: m_offset(_offset) {}

		inline T* operator -> ()
		{
			return (T*)((intptr_t)this + (intptr_t)m_offset);
		}

		inline void set(void* _ptr)
		{
			m_offset = (int32_t)((intptr_t)_ptr - (intptr_t)this);
		}
	};

} // namespace rtm

#endif // RTM_FLAT_OBJECT_H
