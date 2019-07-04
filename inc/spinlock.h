//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_SPINLOCK_H
#define RTM_RBASE_SPINLOCK_H

#include <rbase/inc/platform.h>
#include <rbase/inc/atomic.h>
#include <rbase/inc/thread.h>

namespace rtm {

	class SpinLock
	{
		RTM_CLASS_NO_COPY(SpinLock)
	
		volatile int32_t m_Lock;

	public:
		SpinLock() : m_Lock(0) {}

		inline void	lock()
		{
			while (interlockedCAS(&m_Lock, 1, 0) != 0)
			{
				Thread::yield();
			}
		}

		inline void	unlock()
		{
			interlockedSet(&m_Lock, 0);
		}
	};

	class SpinLockScope
	{
		RTM_CLASS_NO_DEFAULT_CONSTRUCTOR(SpinLockScope)
		RTM_CLASS_NO_COPY(SpinLockScope)

		SpinLock& m_Lock;

	public:
		SpinLockScope(SpinLock& _lock) : m_Lock(_lock)
		{
			m_Lock.lock();
		}

		~SpinLockScope()
		{
			m_Lock.unlock();
		}
	};

} // namespace rtm

#endif // RTM_RBASE_SPINLOCK_H
