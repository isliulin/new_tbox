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
#include "Fcty.h"


QueueInfo_ST FctyQueueInfoRev;
QueueInfo_ST FctyQueueInfoSnd;


static CFcty* CFcty::m_instance = NULL;

CFcty::CFcty()
{
	m_binit = false;

	memset(&FctyQueueInfoRev,0,sizeof(QueueInfo_ST));
	memset(&FctyQueueInfoRev,0,sizeof(QueueInfo_ST));

}

CFcty::~CFcty()
{

}

static CFcty* CFcty::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new CFcty;
	}
	return m_instance;
}

 static void CFcty::FreeInstance()
{

    if(m_instance != NULL)
    {
        delete m_instance;
    }
	
    m_instance = NULL;

}
 
BOOL CFcty::Init()
{
	memset(&FctyQueueInfoRev,0,sizeof(QueueInfo_ST));
	memset(&FctyQueueInfoRev,0,sizeof(QueueInfo_ST));
	
	m_binit = CreateThread(); /*param is default time*/
	
	return m_binit;
}


BOOL CFcty::GetInitSts(void)
{
	memset(&FctyQueueInfoRev,0,sizeof(QueueInfo_ST));
	memset(&FctyQueueInfoRev,0,sizeof(QueueInfo_ST));
	
	m_binit = CreateThread(); /*param is default time*/
	
	return m_binit;
}


void CFcty::Deinit()
{
    KillThread();
}

void CFcty::Run()
{

    InformThread();

}
BOOL CFcty::Processing()
{
	int ret_len;
	mFramework = Framework::GetInstance();

    while(!m_bExit)
    {
	 	FCTYLOG("FCTY processing \r\n");
        Sleep(FCTY_THREAD_PERIOD); /*sleep 500ms*/
    }

}

BOOL CFcty::TimeoutProcessing()
{
	FCTYLOG("jason add ble timeout processing \r\n");
	return true;
}

