//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------// 
/// File: MD5.cpp                                                          ///
/// Desc:                                                                  ///
/// RSA Data Security, Inc.<br>                                            ///
/// MD5 Message-Digest Algorithm.<br>                                      ///
/// Derived from the RSA Data Security, Inc.<br>                           ///
/// MD5 Message-Digest Algorithm.<br>                                      ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/src/md5.h>

#include <string.h>

namespace rtm {

static char chrToHex(const char _char)
{
	if (_char<(char)10)
		return (char)'0' + _char;
	else
		return ((char)'A' - (char)10) + _char;
}

void md5_calculate(void* _data, uint32_t _dataSize, uint8_t _digest[16])
{
	MD5 md5;
	md5.process(_data, _dataSize);
	md5.finalize();
	md5.getHash(_digest);
}

void md5_toString(uint8_t _digest[16], char _hash[33])
{

	for (uint32_t h=0; h<16; h++)
	{
		_hash[h*2+0] = chrToHex((char)(_digest[h] >> 4));
		_hash[h*2+1] = chrToHex((char)(_digest[h] & 0xf));
	}
	_hash[32] = '\0';
}

MD5::MD5()
{
	init();
}

void MD5::process(void* _input, uint32_t _inputLength)
{
	uint8_t* input = (uint8_t*)_input;

	uint32_t input_index, buffer_index;
	uint32_t buffer_space;
	  
	buffer_index = (uint32_t)((m_count[0] >> 3) & 0x3F);

	if ((m_count[0] += ((uint32_t) _inputLength << 3))<((uint32_t) _inputLength << 3))
		m_count[1]++;

	m_count[1] += ((uint32_t)_inputLength >> 29);

	buffer_space = BUFFER_SIZE - buffer_index;

	if (_inputLength >= buffer_space)
	{	
		memcpy( m_buffer+buffer_index, input, buffer_space );
		transform(m_buffer);

		for (input_index = buffer_space; input_index + 63 < _inputLength; input_index += 64)
			transform(input+input_index);

		buffer_index = 0;
	}
	else
		input_index = 0;

	memcpy( m_buffer+buffer_index, input+input_index, _inputLength-input_index );
}

void MD5::processString(const char* _input)
{
	uint32_t len = (uint32_t)strlen(_input);
	process((void*)_input, len * sizeof(char));
}

void MD5::finalize()
{
	uint8_t	bits[8];
	uint32_t index, padLen;
	static uint8_t PADDING[64] =
	{
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	encode(bits, m_count, 8);

	index = (uint32_t)((m_count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	process(PADDING, padLen);
	
	process(bits, 8);
	encode(m_digest, m_state, 16);
	memset(m_buffer, 0, sizeof(*m_buffer));
}

void MD5::getHash(uint8_t _digest[16])
{
	memcpy( _digest, m_digest, 16 );
}

void MD5::getHex(char _hexRepresentation[33])
{
	for (int i=0; i<32; i+=2)
	{
		uint8_t val = m_digest[i>>1];
		_hexRepresentation[i]	= chrToHex((val >> 4));
		_hexRepresentation[i+1]	= chrToHex((val & 0xf));
	}

	_hexRepresentation[32] = '\0';
}

void MD5::init()
{
	m_count[0] = 0;
	m_count[1] = 0;

	m_state[0] = 0x67452301;
	m_state[1] = 0xefcdab89;
	m_state[2] = 0x98badcfe;
	m_state[3] = 0x10325476;
}

enum eMD5Transform
{
	S11_	= 7,
	S12_	= 12,
	S13_	= 17,
	S14_	= 22,
	S21_	= 5,
	S22_	= 9,
	S23_	= 14,
	S24_	= 20,
	S31_	= 4,
	S32_	= 11,
	S33_	= 16,
	S34_	= 23,
	S41_	= 6,
	S42_	= 10,
	S43_	= 15,
	S44_	= 21
};

void MD5::transform(uint8_t _block[64])
{
	uint32_t a = m_state[0];
	uint32_t b = m_state[1];
	uint32_t c = m_state[2];
	uint32_t d = m_state[3];
	uint32_t x[16];

	decode(x, _block, 64);

	// Round 1 
	FF (a, b, c, d, x[ 0], S11_, 0xd76aa478);
	FF (d, a, b, c, x[ 1], S12_, 0xe8c7b756);
	FF (c, d, a, b, x[ 2], S13_, 0x242070db);
	FF (b, c, d, a, x[ 3], S14_, 0xc1bdceee);
	FF (a, b, c, d, x[ 4], S11_, 0xf57c0faf);
	FF (d, a, b, c, x[ 5], S12_, 0x4787c62a);
	FF (c, d, a, b, x[ 6], S13_, 0xa8304613);
	FF (b, c, d, a, x[ 7], S14_, 0xfd469501);
	FF (a, b, c, d, x[ 8], S11_, 0x698098d8);
	FF (d, a, b, c, x[ 9], S12_, 0x8b44f7af);
	FF (c, d, a, b, x[10], S13_, 0xffff5bb1);
	FF (b, c, d, a, x[11], S14_, 0x895cd7be);
	FF (a, b, c, d, x[12], S11_, 0x6b901122);
	FF (d, a, b, c, x[13], S12_, 0xfd987193);
	FF (c, d, a, b, x[14], S13_, 0xa679438e);
	FF (b, c, d, a, x[15], S14_, 0x49b40821);

	// Round 2 
	GG (a, b, c, d, x[ 1], S21_, 0xf61e2562);
	GG (d, a, b, c, x[ 6], S22_, 0xc040b340);
	GG (c, d, a, b, x[11], S23_, 0x265e5a51);
	GG (b, c, d, a, x[ 0], S24_, 0xe9b6c7aa);
	GG (a, b, c, d, x[ 5], S21_, 0xd62f105d);
	GG (d, a, b, c, x[10], S22_,  0x2441453);
	GG (c, d, a, b, x[15], S23_, 0xd8a1e681);
	GG (b, c, d, a, x[ 4], S24_, 0xe7d3fbc8);
	GG (a, b, c, d, x[ 9], S21_, 0x21e1cde6);
	GG (d, a, b, c, x[14], S22_, 0xc33707d6);
	GG (c, d, a, b, x[ 3], S23_, 0xf4d50d87);
	GG (b, c, d, a, x[ 8], S24_, 0x455a14ed);
	GG (a, b, c, d, x[13], S21_, 0xa9e3e905);
	GG (d, a, b, c, x[ 2], S22_, 0xfcefa3f8);
	GG (c, d, a, b, x[ 7], S23_, 0x676f02d9);
	GG (b, c, d, a, x[12], S24_, 0x8d2a4c8a);

	// Round 3 
	HH (a, b, c, d, x[ 5], S31_, 0xfffa3942);
	HH (d, a, b, c, x[ 8], S32_, 0x8771f681);
	HH (c, d, a, b, x[11], S33_, 0x6d9d6122);
	HH (b, c, d, a, x[14], S34_, 0xfde5380c);
	HH (a, b, c, d, x[ 1], S31_, 0xa4beea44);
	HH (d, a, b, c, x[ 4], S32_, 0x4bdecfa9);
	HH (c, d, a, b, x[ 7], S33_, 0xf6bb4b60);
	HH (b, c, d, a, x[10], S34_, 0xbebfbc70);
	HH (a, b, c, d, x[13], S31_, 0x289b7ec6);
	HH (d, a, b, c, x[ 0], S32_, 0xeaa127fa);
	HH (c, d, a, b, x[ 3], S33_, 0xd4ef3085);
	HH (b, c, d, a, x[ 6], S34_,  0x4881d05);
	HH (a, b, c, d, x[ 9], S31_, 0xd9d4d039);
	HH (d, a, b, c, x[12], S32_, 0xe6db99e5);
	HH (c, d, a, b, x[15], S33_, 0x1fa27cf8);
	HH (b, c, d, a, x[ 2], S34_, 0xc4ac5665);

	// Round 4 
	II (a, b, c, d, x[ 0], S41_, 0xf4292244);
	II (d, a, b, c, x[ 7], S42_, 0x432aff97);
	II (c, d, a, b, x[14], S43_, 0xab9423a7);
	II (b, c, d, a, x[ 5], S44_, 0xfc93a039);
	II (a, b, c, d, x[12], S41_, 0x655b59c3);
	II (d, a, b, c, x[ 3], S42_, 0x8f0ccc92);
	II (c, d, a, b, x[10], S43_, 0xffeff47d);
	II (b, c, d, a, x[ 1], S44_, 0x85845dd1);
	II (a, b, c, d, x[ 8], S41_, 0x6fa87e4f);
	II (d, a, b, c, x[15], S42_, 0xfe2ce6e0);
	II (c, d, a, b, x[ 6], S43_, 0xa3014314);
	II (b, c, d, a, x[13], S44_, 0x4e0811a1);
	II (a, b, c, d, x[ 4], S41_, 0xf7537e82);
	II (d, a, b, c, x[11], S42_, 0xbd3af235);
	II (c, d, a, b, x[ 2], S43_, 0x2ad7d2bb);
	II (b, c, d, a, x[ 9], S44_, 0xeb86d391);

	m_state[0] += a;
	m_state[1] += b;
	m_state[2] += c;
	m_state[3] += d;

	memset((uint8_t*)x, 0, sizeof(x));
}

void MD5::encode(uint8_t* _output, uint32_t* _input, uint32_t _length)
{
	uint32_t i, j;
	uint32_t len = _length;
	for ( i=0, j=0; j<len; i++, j+=4)
	{
		_output[j]   = (uint8_t)( _input[i] & 0xff );
		_output[j+1] = (uint8_t)((_input[i] >> 8) & 0xff );
		_output[j+2] = (uint8_t)((_input[i] >> 16) & 0xff );
		_output[j+3] = (uint8_t)((_input[i] >> 24) & 0xff );
	}
}

void MD5::decode( uint32_t *_output, uint8_t* _input, uint32_t _length)
{
	uint32_t i, j;
	uint32_t len = _length;
	for ( i=0, j=0; j<len; i++, j+=4)
		_output[i] = ((uint32_t)_input[j]) | (((uint32_t)_input[j+1]) << 8) | (((uint32_t)_input[j+2]) << 16) | (((uint32_t)_input[j+3]) << 24);
}

} // namespace rtm
