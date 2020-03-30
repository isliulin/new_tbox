/*

ble.cpp
Description

*/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/reboot.h>
#include "common.h"
#include "Nvm.h"


QueueInfo_ST nvmQueueInfoRev;
QueueInfo_ST nvmQueueInfoSnd;


static CNvm* CNvm::m_instance = NULL;

CNvm::CNvm()
{
	m_binit = false;

}

CNvm::~CNvm()
{

}

static CNvm* CNvm::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new CNvm;
	}
	return m_instance;
}

 static void CNvm::FreeInstance()
{

    if(m_instance != NULL)
    {
        delete m_instance;
    }
	
    m_instance = NULL;

}
 
BOOL CNvm::Init()
{
	m_binit = CreateThread(); /*param is default time*/
	
	return m_binit;
}

void CNvm::Deinit()
{
    KillThread();
}

void CNvm::Run()
{

    InformThread();

}
BOOL CNvm::Processing()
{
	int ret_len;
	mFramework = Framework::GetInstance();

    while(!m_bExit)
    {
#if QUEUE_TEST_MACRO
		 ReceiveQueueTestTask();
 		 SendQueueTestTask();
		 Sleep(200 * 1000);
#else
	 printf("ble processing \r\n");
        Sleep(500 * 1000); /*sleep 500ms*/
#endif
    }

}

BOOL CNvm::TimeoutProcessing()
{
	printf("jason add ble timeout processing \r\n");
	return true;
}

void CNvm::ReceiveQueueTestTask()
{
	int len = 0;
	QueueInfo_ST  QueueTest;

	   if(Framework::GetInstance()->ID_Queue_FW_To_NVM> 0)
	   {
			len =msgrcv(Framework::GetInstance()->ID_Queue_FW_To_NVM,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);
			if(len == -1)
			{
				printf("jason add nvm receive from  fw no data \r\n");
				printf("jason add nvm receive from  fw no data \r\n");
				printf("jason add nvm receive from  fw no data \r\n");
			}
			else
			{
				printf("jason add nvm receive from  fw len = %d \r\n",len);
				printf("jason add nvm receive from  fw QueueTest.Msgs[0] = %x \r\n",QueueTest.Msgs[0]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[1] = %x \r\n",QueueTest.Msgs[1]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[2] = %x \r\n",QueueTest.Msgs[2]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[3] = %x \r\n",QueueTest.Msgs[3]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[4] = %x \r\n",QueueTest.Msgs[4]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[500] = %x \r\n",QueueTest.Msgs[500]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[501] = %x \r\n",QueueTest.Msgs[501]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[502] = %x \r\n",QueueTest.Msgs[502]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[503]);
				printf("jason add nvm receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[504]);
			}
	   }

}

 void CNvm::SendQueueTestTask()
{
	QueueInfo_ST  QueueTest;
	int len = 0;
	static int count = 0;
	QueueTest.mtype = 1;

	   if( (Framework::GetInstance()->ID_Queue_NVM_To_FW  > 0) && (count++ %5 ==0))
	   {
	   	QueueTest.Msgs[0] = 0x66;
		QueueTest.Msgs[1] = 0x77;
	   	QueueTest.Msgs[2] = 0x88;
	   	QueueTest.Msgs[3] = 0x99;
	   	QueueTest.Msgs[4] = 0xaa;
		
	   	QueueTest.Msgs[500] = 0xaa;
		QueueTest.Msgs[501] = 0x99;
	   	QueueTest.Msgs[502] = 0x88;
	   	QueueTest.Msgs[503] = 0x77;
	   	QueueTest.Msgs[504] = 0x66;

		 len = msgsnd(Framework::GetInstance()->ID_Queue_NVM_To_FW,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),IPC_NOWAIT);
		if(len == -1)
		{
			printf("jason add nvm send to Framework error \r\n");

		}
		else
		{
			printf("jason add nvm send to mcu Framework success\r\n");

		}
	   }

 }
