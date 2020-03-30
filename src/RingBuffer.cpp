#include "RingBuffer.h"


RingBuffer::RingBuffer()
{
	PBuffFirst = (uint8_t *)malloc(BUFFERSIZE*(sizeof(char)));
	if(NULL == PBuffFirst){
		BUFFERLOG("PBuffFirst malloc failed!\n");
	}
	memset(PBuffFirst, 0, BUFFERSIZE);
	BufferSize = BUFFERSIZE;
	UsingDataLen = 0;
	PRead = PWrite = PBuffFirst;
	PBuffLast = PBuffFirst + BUFFERSIZE - 1;
}


RingBuffer::RingBuffer(int Size)
{
	PBuffFirst = (uint8_t *)malloc(Size*(sizeof(char)));
	if(NULL == PBuffFirst){
		BUFFERLOG("PBuffFirst malloc failed!\n");
	}
	memset(PBuffFirst, 0, Size);
	BufferSize = Size;
	UsingDataLen = 0;
	PRead = PWrite = PBuffFirst;
	PBuffLast = PBuffFirst + BufferSize - 1;
}

void RingBuffer::Init_RingBuffer()
{
	PRead = PWrite = PBuffFirst;
	UsingDataLen = 0;
}


RingBuffer::~RingBuffer()
{
	if(NULL != PBuffFirst)
		free(PBuffFirst);
	UsingDataLen = 0;
	BufferSize = 0;
	PRead = NULL;
	PWrite = NULL;
	PBuffFirst = NULL;
	PBuffLast = NULL;
}


int RingBuffer::WriteData(uint8_t *Buff, int Len)
{
	int TailDataLen = 0;
	if(NULL == Buff || Len <= 0)
	{
		BUFFERLOG("Input Buff failed!\n");
		return -1;
	}
	
	if(GetResidueDataLen() < Len)
	{
		BUFFERLOG("Buffer memory is full!\n");
		return -1;
	}

	if((PWrite + Len - 1) <= PBuffLast)
	{
		memcpy(PWrite, Buff, Len);
		if((PWrite + Len - 1) == PBuffLast)
			PWrite = PBuffFirst;
		else
			PWrite += Len;
	}
	else	//(PRead > PWrite)
	{
		TailDataLen = PBuffLast - PWrite + 1;
		memcpy(PWrite, Buff, TailDataLen);
		memcpy(PBuffFirst, Buff + TailDataLen, Len - TailDataLen);
		PWrite = PBuffFirst + (Len - TailDataLen);
	}
	
	UsingDataLen += Len;
	return Len;
}


int RingBuffer::ReadData(uint8_t *Buff, int Len)
{
	int TailDataLen = 0;
	if(NULL == Buff || Len <= 0)
		return -1;

	if(Len > GetUsingDataLen())
	{
		Len = GetUsingDataLen();
	}
	
	if(PRead + Len - 1 <= PBuffLast)
	{
		memcpy(Buff, PRead, Len);
		if(PRead + Len - 1 == PBuffLast)
			PRead = PBuffFirst;
		else
			PRead += Len;
	}
	else
	{
		TailDataLen = PBuffLast - PRead + 1;
		memcpy(Buff, PRead, TailDataLen);
		memcpy(Buff + TailDataLen, PBuffFirst, Len - TailDataLen);
		PRead = PBuffFirst + Len - TailDataLen;
	}
	UsingDataLen -= Len;
	return Len;
}

uint8_t *RingBuffer::Get_PReadAddr()
{
	return PRead;
}

int RingBuffer::GetResidueDataLen()
{
	return (BufferSize - UsingDataLen);
}

int RingBuffer::GetUsingDataLen()
{
	return UsingDataLen;
}

uint16_t RingBuffer::Get_DataLen(uint16_t FixLen)
{
	uint16_t TempLen = 0;
	if(PRead + FixLen  <= PBuffLast)	//长度未超过BUFF尾
	{
		return (uint16_t)((PRead[FixLen-1] << 8) | PRead[FixLen]);
	}
	else
	{
		if(PRead + FixLen - 1 == PBuffLast)	//一个字节在尾部一个字节在头部
		{
			return (uint16_t)((*PBuffLast << 8) | *PBuffFirst);
		}
		else	//两个字节全部在头部
		{
			TempLen = FixLen - (PBuffLast - PRead + 1);
			return (uint16_t)((PBuffFirst[TempLen - 1] << 8) | PBuffFirst[TempLen]);
		}
	}
}


