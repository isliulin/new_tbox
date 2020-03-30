/*
log.cpp:
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

#include "Log.h"

static CLog *CLog::m_instance = NULL;

CLog::CLog() {
  m_binit = false;

}

CLog::~CLog() {

}

static CLog *CLog::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new CLog;
  }
  return m_instance;
}

static void CLog::FreeInstance() {

  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;

}

BOOL CLog::Init() {
  m_binit = CreateThread(); /*param is default time*/

  return m_binit;
}

void CLog::Deinit() {
  KillThread();
}

void CLog::Run() {

  InformThread();

}
BOOL CLog::Processing() {
  while (!m_bExit) {
    printf("log processing \r\n");
    Sleep(5000000); /*sleep 500ms*/
  }

}

BOOL CLog::TimeoutProcessing() {
  printf("jason add log timeout processing \r\n");
  return true;
}

void CLog::ReceiveQueueTask() {
  int len = 0;
  QueueInfo_ST QueueTest;

  if (Framework::GetInstance()->ID_Queue_FW_To_LOG > 0) {
    len = msgrcv(Framework::GetInstance()->ID_Queue_FW_To_TSP,
                 &QueueTest,
                 sizeof(QueueInfo_ST) - sizeof(long),
                 1,
                 IPC_NOWAIT);
    if (len == -1) {
      printf("jason add TSP receive from  log  no data \r\n");
      printf("jason add TSP receive from  log no data \r\n");
      printf("jason add TSP receive from  log no data \r\n");
    } else {
      printf("jason add TSP receive from  log QueueTest.Msgs[0] = %x \r\n", QueueTest.Msgs[0]);
      printf("jason add TSP receive from  log QueueTest.Msgs[1] = %x \r\n", QueueTest.Msgs[1]);

    }
  }

}

void CLog::SendQueueTask() {
  QueueInfo_ST QueueTest;
  int len = 0;
  static int count = 0;
  QueueTest.mtype = 1;
  QueueTest.head.Gr = ID_THREAD_LOG;

  if ((Framework::GetInstance()->ID_Queue_LOG_To_FW > 0) && (count++ % 5 == 0)) {
    QueueTest.head.Gr = ID_THREAD_LOG;
    QueueTest.head.Id = ID_LOG_2_FW_SYNC;
    len = msgsnd(Framework::GetInstance()->ID_Queue_TSP_To_FW, &QueueTest, sizeof(ParaInfoHead_ST), IPC_NOWAIT);
    if (len == -1) {
      printf("jason add tsp send to Framework error \r\n");

    } else {
      printf("jason add tsp send to mcu Framework success\r\n");

    }
  }

}
 
