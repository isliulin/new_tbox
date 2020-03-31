#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#ifndef BUFFER_DEBUG
	#define BUFFER_DEBUG 1
#endif



#if BUFFER_DEBUG 	
	#define BUFFERLOG(format,...) printf("== RingBuffer == FILE: %s, FUN: %s, LINE: %d "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define BUFFER_NO(format,...) printf(format,##__VA_ARGS__)
#else	
	#define BUFFERLOG(format,...)
	#define BUFFER_NO(format,...)
#endif

#define BUFFERSIZE		(4096)

class RingBuffer
{
	public:
		RingBuffer();
		~RingBuffer();
		RingBuffer(int Size);
		int GetUsingDataLen();
		int GetResidueDataLen();

		int WriteData(uint8_t *Buff, int Len);
		int ReadData(uint8_t *Buff, int Len);
		uint16_t Get_DataLen(uint16_t FixLen);

		uint8_t *Get_PReadAddr();
		void Init_RingBuffer();

		
	private:
		uint8_t *PRead;			//Flag Buffer Head pointer
		uint8_t *PWrite;		//Flag Buffer Tail pointer
		uint8_t *PBuffFirst;	//Flag input data Head pointer
		uint8_t *PBuffLast;		//Flag input data Tail pointer
		int BufferSize;			//RingBuffer size
		int UsingDataLen;		//The used RingBuffer size
};

#endif
