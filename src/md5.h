//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//
/// File: MD5.h                                                            ///
/// Desc:                                                                  ///
/// RSA Data Security, Inc.<br>                                            ///
/// MD5 Message-Digest Algorithm.<br>                                      ///
/// Derived from the RSA Data Security, Inc.<br>                           ///
/// MD5 Message-Digest Algorithm.<br>                                      ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_MD5_H
#define RTM_RBASE_MD5_H

namespace rtm {

class MD5
{
	enum { BUFFER_SIZE = 64 };

	private:
		uint32_t	m_state[4];
		uint32_t	m_count[2];
		uint8_t		m_buffer[BUFFER_SIZE];
		uint8_t		m_digest[16];

		void		init();
		void		transform(uint8_t* _buffer);
		static void	encode(uint8_t* _dest, uint32_t* _src, uint32_t _length);
		static void	decode(uint32_t* _dest, uint8_t* _src, uint32_t _length);

		static inline uint32_t	rotateLeft(uint32_t _x, uint32_t _n);
		static inline uint32_t	F(uint32_t _x, uint32_t _y, uint32_t _z);
		static inline uint32_t	G(uint32_t _x, uint32_t _y, uint32_t _z);
		static inline uint32_t	H(uint32_t _x, uint32_t _y, uint32_t _z);
		static inline uint32_t	I(uint32_t _x, uint32_t _y, uint32_t _z);
		static inline void		FF(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t _s, uint32_t _ac);
		static inline void		GG(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t _s, uint32_t _ac);
		static inline void		HH(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t _s, uint32_t _ac);
		static inline void		II(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t _s, uint32_t _ac);

	public:
		MD5();

		void process(void* _input, uint32_t _inputLength);
		void processString(const char* _input);
		void finalize();
		void getHash(uint8_t outDigest[16]);
		void getHex(char HexRepresentation[33]);

		template <typename T>
		MD5& operator << (const T& _value)
		{
			process( (void*)&_value, sizeof(_value) );
			return *this;
		}
};

inline uint32_t MD5::rotateLeft(uint32_t _x, uint32_t _n)
{
	return (_x << _n) | (_x >> (32-_n));
}

inline uint32_t MD5::F(uint32_t _x, uint32_t _y, uint32_t _z)
{
	return (_x & _y) | (~_x & _z);
}

inline uint32_t MD5::G(uint32_t _x, uint32_t _y, uint32_t _z)
{
	return (_x & _z) | (_y & ~_z);
}

inline uint32_t MD5::H(uint32_t _x, uint32_t _y, uint32_t _z)
{
	return _x ^ _y ^ _z;
}

inline uint32_t MD5::I(uint32_t _x, uint32_t _y, uint32_t _z)
{
	return _y ^ (_x | ~_z);
}

inline void MD5::FF(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t  _s, uint32_t _ac)
{
	_a += F(_b, _c, _d) + _x + _ac;
	_a = rotateLeft (_a, _s) +_b;
}

inline void MD5::GG(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t _s, uint32_t _ac)
{
	_a += G(_b, _c, _d) + _x + _ac;
	_a = rotateLeft (_a, _s) +_b;
}

inline void MD5::HH(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t _s, uint32_t _ac)
{
	_a += H(_b, _c, _d) + _x + _ac;
	_a = rotateLeft (_a, _s) +_b;
}

inline void MD5::II(uint32_t& _a, uint32_t _b, uint32_t _c, uint32_t _d, uint32_t _x, uint32_t _s, uint32_t _ac)
{
	_a += I(_b, _c, _d) + _x + _ac;
	_a = rotateLeft (_a, _s) +_b;
}

} // namespace rtm

#endif  //  RTM_RBASE_MD5_H
