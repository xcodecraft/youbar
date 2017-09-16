#pragma once
#ifndef MD5_H
#define MD5_H

#include <fstream>


/* Type define */
typedef unsigned char byte;
typedef unsigned long ulong;
namespace yyou
{

	using std::string;
	using std::ifstream;

	/* MD5 declaration. */
	class  md5 
    {
	public:
		md5();
		md5(const void *input, size_t length);
		md5(const string &str);
		md5(ifstream &in);
		void update(const void *input, size_t length);
		void update(const std::string &str);
		void update(ifstream &in);
		const byte* digest();
		string to_string();
		void reset();
	private:
		void update(const byte *input, size_t length);
		void final();
		void transform(const byte block[64]);
		void encode(const ulong *input, byte *output, size_t length);
		void decode(const byte *input, ulong *output, size_t length);
		std::string bytesToHexString(const byte *input, size_t length);

		/* class uncopyable */
		md5(const md5&);
		md5& operator=(const md5&);
	private:
		ulong _state[4]; /* state (ABCD) */
		ulong _count[2]; /* number of bits, modulo 2^64 (low-order word first) */
		byte _buffer[64]; /* input buffer */
		byte _digest[16]; /* message digest */
		bool _finished;  /* calculate finished ? */

		static const byte PADDING[64]; /* padding for calculate */
		static const char HEX[16];
		static const size_t BUFFER_SIZE;
	};

}
#endif/*MD5_H*/

