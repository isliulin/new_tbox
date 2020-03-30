/*
	lte.cpp
	Description:
*/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/reboot.h>
#include "common.h"
#include "Pwr.h"

static CPwr* CPwr::m_instance = NULL;

CPwr::CPwr()
{
	m_binit = false;

}

CPwr::~CPwr()
{

}

static CPwr* CPwr::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new CPwr;
	}
	return m_instance;
}

 static void CPwr::FreeInstance()
{

    if(m_instance != NULL)
    {
        delete m_instance;
    }
	
    m_instance = NULL;

}
 
BOOL CPwr::Init()
{
	m_binit = CreateThread(); /*param is default time*/
	
	return m_binit;
}

void CPwr::Deinit()
{
    KillThread();
}

void CPwr::Run()
{

    InformThread();

}
BOOL CPwr::Processing()
{
    while(!m_bExit)
    {
#if QUEUE_TEST_MACRO
		SendQueueTestTask();
		ReceiveQueueTestTask();
		Sleep(200*1000); /*sleep 500ms*/

 #else
	 printf("lte processing \r\n");
        Sleep(5000000); /*sleep 500ms*/
#endif
    }

}

BOOL CPwr::TimeoutProcessing()
{
	printf("jason add lte timeout processing \r\n");
	return true;
}
void CPwr::ReceiveQueueTestTask()
{
	int len = 0;
	QueueInfo_ST  QueueTest ;

	   if(Framework::GetInstance()->ID_Queue_FW_To_PWR  > 0)
	   {
			len =msgrcv(Framework::GetInstance()->ID_Queue_FW_To_PWR,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);
			if(len == -1)
			{
				printf("jason add PWR receive from  fw no data \r\n");
				printf("jason add PWR receive from  fw no data \r\n");
				printf("jason add PWR receive from  fw no data \r\n");
			}
			else
			{
				printf("jason add PWR receive from  fw len = %d \r\n",len);
				printf("jason add PWR receive from  fw QueueTest.Msgs[0] = %x \r\n",QueueTest.Msgs[0]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[1] = %x \r\n",QueueTest.Msgs[1]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[2] = %x \r\n",QueueTest.Msgs[2]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[3] = %x \r\n",QueueTest.Msgs[3]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[4] = %x \r\n",QueueTest.Msgs[4]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[500] = %x \r\n",QueueTest.Msgs[500]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[501] = %x \r\n",QueueTest.Msgs[501]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[502] = %x \r\n",QueueTest.Msgs[502]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[503]);
				printf("jason add PWR receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[504]);
			}
	   }

}

 void CPwr::SendQueueTestTask()
{
	QueueInfo_ST  QueueTest;
		QueueTest.mtype =1;

	int len = 0;
	static int count = 0;
	   if( (Framework::GetInstance()->ID_Queue_PWR_To_FW  > 0) && (count++ %5 ==0))
	   {
	   	QueueTest.Msgs[0] = 0x55;
		QueueTest.Msgs[1] = 0x66;
	   	QueueTest.Msgs[2] = 0x77;
	   	QueueTest.Msgs[3] = 0x88;
	   	QueueTest.Msgs[4] = 0x99;
		
	   	QueueTest.Msgs[500] = 0x99;
		QueueTest.Msgs[501] = 0x88;
	   	QueueTest.Msgs[502] = 0x77;
	   	QueueTest.Msgs[503] = 0x66;
	   	QueueTest.Msgs[504] = 0x55;

		 len = msgsnd(Framework::GetInstance()->ID_Queue_PWR_To_FW,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),IPC_NOWAIT);
		if(len == -1)
		{
			printf("jason add PWR send to Framework error \r\n");
			printf("jason add PWR send to Framework error \r\n");
			printf("jason add PWR send to Framework error \r\n");
		}
		else
		{
			printf("jason add PWR send to mcu Framework =%d\r\n",len);
			printf("jason add PWR send to mcu Framework =%d\r\n",len);
			printf("jason add PWR send to mcu Framework =%d\r\n",len);
		}
	   }

 }
