//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_RANDOM_H
#define RTM_RBASE_RANDOM_H

#include <rbase/inc/platform.h>

namespace rtm {

	/// http://www.cse.yorku.ca/~oz/marsaglia-rng.html
	class Random
	{
		uint32_t m_z;
		uint32_t m_w;

	public:
		Random()
			: m_z(12345)
			, m_w(65435)
		{
		}

		void seed(uint32_t _z = 12345, uint32_t _w = 65435)
		{
			m_z = _z;
			m_w = _w;
		}

		uint32_t gen()
		{
			m_z = 36969*(m_z&0xffff)+(m_z>>16);
			m_w = 18000*(m_w&0xffff)+(m_w>>16);
			return (m_z<<16)+m_w;
		}

		float genf()
		{
			uint32_t rnd = gen() & 0xffff;
			return float(rnd) * (1.0f/float(0xffff));
		}
	};

} // namespace rtm

#endif // RTM_RBASE_RANDOM_H
