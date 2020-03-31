#ifndef _HMAC_MD5_H_
#define _HMAC_MD5_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
 

#define MAX_FILE 1024*1024*2
 

typedef struct{
	unsigned long int state[4]; 
	unsigned long int count[2]; 
	unsigned char PADDING[64]; 
	unsigned char buffer[64]; 
}MD5_State;


#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))


#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))



#define FF(a, b, c, d, x, s, ac) { \
(a) += F ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
(a) += G ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
(a) += H ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
(a) += I ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}

class HMAC_MD5
{
	public:
	HMAC_MD5() {};
	~HMAC_MD5() {};
	void MD5_memcpy( unsigned char *output, unsigned char *input, unsigned int len );
	void MD5_memset( unsigned char *output, int value, unsigned int len );
	void Encode( unsigned char *output, unsigned long int *input, unsigned int len );
	void Decode( unsigned long int *output, unsigned char *input, unsigned int len );
	void MD5_Init( MD5_State *s );
	void MD5_Transform( unsigned long int state[4], unsigned char block[64] );
	void MD5_Update( MD5_State *s, unsigned char *input, unsigned int inputLen );
	void MD5_Final( MD5_State *s, unsigned char digest[16] );
	int FileIn( const char *strFile, char *&inBuff );
	void Usage( const char *appname );
	bool CheckParse( int argc, char** argv );
	void md5_digest( void const *strContent, unsigned int iLength, unsigned char output[16] );
	int md5test(const char *testfile, unsigned char *output);
private:
	char iii;
};

#endif