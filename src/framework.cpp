#include "framework.h"
#include <errno.h>
#include "BIF.h"
#include "VIF.h"
#include "TPAK.h"
#include "BPAK.h"
#include "VPAK.h"
#include "BCTL.h"
#include "tspTCTL.h"

msgstru struMsgs;
TPAK_VeichleInformation_UN mTPAK_Information;
VPAK_RemoteCtrlInformation_UN mVPAK_Information;
BPAK_VeichleInformation_UN mBPAK_Information;
QueueInfo_ST mFwQueueInfoRev;
QueueInfo_ST mFwQueueInfoSnd;

static Framework *Framework::m_instance = NULL;

BOOL Framework::Processing() {
  while (!m_bExit) {

#if QUEUE_TEST_MACRO
    ReceiveQueueTestTask();
    SendQueueTestTask();
    Sleep(200 * 1000);

#else
    Rcv_Queue_Mcu_To_FW();
    Rcv_Queue_Tsp_To_FW();
    Rcv_Queue_Ble_To_FW();
    Rcv_Queue_Ota_To_FW();
    Rcv_Queue_Pwr_To_FW();
    Rcv_Queue_Nvm_To_FW();
    Rcv_Queue_Log_To_FW();

#if VIVIAN_ADD_MONITOR
    fw_Thread_Monitor_Process();
#endif
    Sleep(50 * 1000);
#endif
  }

  return TRUE;
}

Framework::Framework() {
  ID_Queue_MCU_To_FW = -1;
  ID_Queue_BLE_To_FW = -1;
  ID_Queue_TSP_To_FW = -1;
  ID_Queue_OTA_To_FW = -1;
  ID_Queue_PWR_To_FW = -1;
  ID_Queue_NVM_To_FW = -1;
  ID_Queue_FW_To_MCU = -1;
  ID_Queue_FW_To_BLE = -1;
  ID_Queue_FW_To_TSP = -1;
  ID_Queue_FW_To_OTA = -1;
  ID_Queue_FW_To_PWR = -1;
  ID_Queue_FW_To_NVM = -1;
  Framework_InitSts = FALSE;
}

Framework *Framework::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new Framework;
  }

  return m_instance;
}

BOOL Framework::Init() {
#if VIVIAN_ADD_MONITOR
  fw_Thread_Monitor_Init();
#endif

  if (VIF_Init() == TRUE)
    printf("VIF init success!!!");
  else
    printf("VIF init fail!!!");

  if (BIF_Init() == TRUE)
    printf("BIF init success!!!");
  else
    printf("BIF init fail!!!");

  if (TPAK_Init() == TRUE)
    printf("TPAK init success!!!");
  else
    printf("TPAK init fail!!!");

  if (VPAK_Init() == TRUE)
    printf("VPAK init success!!!");
  else
    printf("VPAK init fail!!!");

  if (BPAK_Init() == TRUE)
    printf("BPAK init success!!!");
  else
    printf("BPAK init fail!!!");

  if (TCTL_Init() == TRUE)
    printf("TCTL_Init success!!!");
  else
    printf("TCTL_Init fail!!!");

  if (BCTL_Init() == TRUE)
    printf("BCTL_Init success!!!");
  else
    printf("BCTL_Init fail!!!");

  if (VUPG_Init() == TRUE)
    printf("VUPG_Init success!!!");
  else
    printf("VUPG_Init fail!!!");

  /*if(VCTL_Init() == TRUE)
      printf("TCTL_Init success!!!");
else
       printf("TCTL_Init fail!!!");*/

  ID_Queue_MCU_To_FW = msgget(ftok("/data/tbox", 1), IPC_CREAT | 0777);
  ID_Queue_BLE_To_FW = msgget(ftok("/data/tbox", 2), IPC_CREAT | 0777);
  ID_Queue_TSP_To_FW = msgget(ftok("/data/tbox", 3), IPC_CREAT | 0777);
  ID_Queue_OTA_To_FW = msgget(ftok("/data/tbox", 4), IPC_CREAT | 0777);
  ID_Queue_PWR_To_FW = msgget(ftok("/data/tbox", 5), IPC_CREAT | 0777);
  ID_Queue_NVM_To_FW = msgget(ftok("/data/tbox", 6), IPC_CREAT | 0777);
  ID_Queue_FW_To_MCU = msgget(ftok("/data/tbox", 7), IPC_CREAT | 0777);
  ID_Queue_FW_To_BLE = msgget(ftok("/data/tbox", 8), IPC_CREAT | 0777);
  ID_Queue_FW_To_TSP = msgget(ftok("/data/tbox", 9), IPC_CREAT | 0777);
  ID_Queue_FW_To_OTA = msgget(ftok("/data/tbox", 10), IPC_CREAT | 0777);
  ID_Queue_FW_To_PWR = msgget(ftok("/data/tbox", 11), IPC_CREAT | 0777);
  ID_Queue_FW_To_NVM = msgget(ftok("/data/tbox", 12), IPC_CREAT | 0777);

  if (ID_Queue_MCU_To_FW < 0 || ID_Queue_BLE_To_FW < 0 || ID_Queue_TSP_To_FW < 0 || ID_Queue_OTA_To_FW < 0 ||
      ID_Queue_PWR_To_FW < 0 || ID_Queue_NVM_To_FW < 0 || ID_Queue_FW_To_MCU < 0 || ID_Queue_FW_To_BLE < 0 ||
      ID_Queue_FW_To_TSP < 0 || ID_Queue_FW_To_OTA < 0 || ID_Queue_FW_To_PWR < 0 || ID_Queue_FW_To_NVM < 0) {
    printf("Create queue failed!!!\r\n");
  } else {
    printf("Create queue success!!!\r\n");
    Framework_InitSts = CreateThread();
  }

  return Framework_InitSts;
}
void Framework::SendQueueTestTask() {
  QueueInfo_ST QueueTest;
  int len = 0;
  static int count = 0;
#if 1
  QueueTest.mtype = 1;
  count++;
  if ((ID_Queue_FW_To_MCU > 0) && (count % 5 == 0)) {
    QueueTest.Msgs[0] = 0x01;
    QueueTest.Msgs[1] = 0x02;
    QueueTest.Msgs[2] = 0x03;
    QueueTest.Msgs[3] = 0x04;
    QueueTest.Msgs[4] = 0x05;

    QueueTest.Msgs[500] = 0x05;
    QueueTest.Msgs[501] = 0x04;
    QueueTest.Msgs[502] = 0x03;
    QueueTest.Msgs[503] = 0x02;
    QueueTest.Msgs[504] = 0x01;

    len = msgsnd(ID_Queue_FW_To_MCU, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework send to mcu error \r\n");
      printf("jason add Framework send to mcu error \r\n");
      printf("jason add Framework send to mcu error \r\n");
    } else {
      printf("jason add Framework send to mcu len =%d\r\n", len);
      printf("jason add Framework send to mcu len =%d\r\n", len);
      printf("jason add Framework send to mcu len =%d\r\n", len);
    }
  }
#endif

#if 1

  if ((ID_Queue_FW_To_BLE > 0) && (count % 5 == 0)) {
    QueueTest.Msgs[0] = 0x02;
    QueueTest.Msgs[1] = 0x03;
    QueueTest.Msgs[2] = 0x04;
    QueueTest.Msgs[3] = 0x05;
    QueueTest.Msgs[4] = 0x06;

    QueueTest.Msgs[500] = 0x06;
    QueueTest.Msgs[501] = 0x05;
    QueueTest.Msgs[502] = 0x04;
    QueueTest.Msgs[503] = 0x03;
    QueueTest.Msgs[504] = 0x02;

    len = msgsnd(ID_Queue_FW_To_BLE, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), 0);
    if (len == -1) {
      printf("jason add Framework send to ble error \r\n");
      printf("jason add Framework send to ble error \r\n");
      printf("jason add Framework send to ble error \r\n");
    } else {
      printf("jason add Framework send to ble len =%d\r\n", len);
      printf("jason add Framework send to ble len =%d\r\n", len);
      printf("jason add Framework send to ble len =%d\r\n", len);
    }
  }
#endif
#if 1
  if ((ID_Queue_FW_To_TSP > 0) && (count % 5 == 0)) {
    QueueTest.Msgs[0] = 0x03;
    QueueTest.Msgs[1] = 0x04;
    QueueTest.Msgs[2] = 0x05;
    QueueTest.Msgs[3] = 0x06;
    QueueTest.Msgs[4] = 0x07;

    QueueTest.Msgs[500] = 0x07;
    QueueTest.Msgs[501] = 0x06;
    QueueTest.Msgs[502] = 0x05;
    QueueTest.Msgs[503] = 0x04;
    QueueTest.Msgs[504] = 0x03;

    len = msgsnd(ID_Queue_FW_To_TSP, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework send to tsp error \r\n");
    } else {
      printf("jason add Framework send to tsp success\r\n");
    }
  }
#endif
#if 1
  if ((ID_Queue_FW_To_OTA > 0) && (count % 5 == 0)) {
    QueueTest.Msgs[0] = 0x04;
    QueueTest.Msgs[1] = 0x05;
    QueueTest.Msgs[2] = 0x06;
    QueueTest.Msgs[3] = 0x07;
    QueueTest.Msgs[4] = 0x08;

    QueueTest.Msgs[500] = 0x08;
    QueueTest.Msgs[501] = 0x07;
    QueueTest.Msgs[502] = 0x06;
    QueueTest.Msgs[503] = 0x05;
    QueueTest.Msgs[504] = 0x04;

    len = msgsnd(ID_Queue_FW_To_OTA, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework send to ota error \r\n");

    } else {
      printf("jason add Framework send to ota \r\n");

    }
  }
#endif
#if 1
  if ((ID_Queue_FW_To_PWR > 0) && (count % 5 == 0)) {
    QueueTest.Msgs[0] = 0x05;
    QueueTest.Msgs[1] = 0x06;
    QueueTest.Msgs[2] = 0x07;
    QueueTest.Msgs[3] = 0x08;
    QueueTest.Msgs[4] = 0x09;

    QueueTest.Msgs[500] = 0x09;
    QueueTest.Msgs[501] = 0x08;
    QueueTest.Msgs[502] = 0x07;
    QueueTest.Msgs[503] = 0x06;
    QueueTest.Msgs[504] = 0x05;

    len = msgsnd(ID_Queue_FW_To_PWR, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework send to tsp error \r\n");
      printf("jason add Framework send to tsp error \r\n");
      printf("jason add Framework send to tsp error \r\n");
    } else {
      printf("jason add Framework send to tsp len =%d\r\n", len);
      printf("jason add Framework send to tsp len =%d\r\n", len);
      printf("jason add Framework send to tsp len =%d\r\n", len);
    }
  }
#endif
#if 1
  if ((ID_Queue_FW_To_NVM > 0) && (count % 5 == 0)) {
    QueueTest.Msgs[0] = 0x06;
    QueueTest.Msgs[1] = 0x07;
    QueueTest.Msgs[2] = 0x08;
    QueueTest.Msgs[3] = 0x09;
    QueueTest.Msgs[4] = 0x0a;

    QueueTest.Msgs[500] = 0x0a;
    QueueTest.Msgs[501] = 0x09;
    QueueTest.Msgs[502] = 0x08;
    QueueTest.Msgs[503] = 0x07;
    QueueTest.Msgs[504] = 0x06;

    len = msgsnd(ID_Queue_FW_To_NVM, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework send to nvm error \r\n");

    } else {
      printf("jason add Framework send to nvm \r\n");

    }
  }
#endif

}
void Framework::ReceiveQueueTestTask() {
  int len = 0;
  QueueInfo_ST QueueTest;
#if 1
  if (ID_Queue_MCU_To_FW > 0) {
    len = msgrcv(ID_Queue_MCU_To_FW, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework receive from  mcu no data \r\n");
      printf("jason add Framework receive from  mcu no data \r\n");
      printf("jason add Framework receive from  mcu no data \r\n");
    } else {
      printf("jason add Framework receive from  mcu len = %d \r\n", len);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[0] = %x \r\n", QueueTest.Msgs[0]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[1] = %x \r\n", QueueTest.Msgs[1]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[2] = %x \r\n", QueueTest.Msgs[2]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[3] = %x \r\n", QueueTest.Msgs[3]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[4] = %x \r\n", QueueTest.Msgs[4]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[500] = %x \r\n", QueueTest.Msgs[500]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[501] = %x \r\n", QueueTest.Msgs[501]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[502] = %x \r\n", QueueTest.Msgs[502]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[503]);
      printf("jason add Framework receive from  mcu QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[504]);
    }
  }
#endif
#if 1
  if (ID_Queue_BLE_To_FW > 0) {
    len = msgrcv(ID_Queue_BLE_To_FW, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);

    if (len == -1) {
      printf("jason add Framework receive from  ble no data \r\n");
      printf("jason add Framework receive from  ble no data \r\n");
      printf("jason add Framework receive from  ble no data \r\n");
    } else {
      printf("jason add Framework receive from  ble len = %d \r\n", len);
      printf("jason add Framework receive from  ble QueueTest.Msgs[0] = %x \r\n", QueueTest.Msgs[0]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[1] = %x \r\n", QueueTest.Msgs[1]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[2] = %x \r\n", QueueTest.Msgs[2]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[3] = %x \r\n", QueueTest.Msgs[3]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[4] = %x \r\n", QueueTest.Msgs[4]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[500] = %x \r\n", QueueTest.Msgs[500]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[501] = %x \r\n", QueueTest.Msgs[501]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[502] = %x \r\n", QueueTest.Msgs[502]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[503]);
      printf("jason add Framework receive from  ble QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[504]);
    }

  }
#endif
#if 1
  if (ID_Queue_TSP_To_FW > 0) {
    len = msgrcv(ID_Queue_TSP_To_FW, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework receive from  tsp no data \r\n");

    } else {
      printf("jason add Framework receive from  tsp len = %d \r\n", len);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[0] = %x \r\n", QueueTest.Msgs[0]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[1] = %x \r\n", QueueTest.Msgs[1]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[2] = %x \r\n", QueueTest.Msgs[2]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[3] = %x \r\n", QueueTest.Msgs[3]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[4] = %x \r\n", QueueTest.Msgs[4]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[500] = %x \r\n", QueueTest.Msgs[500]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[501] = %x \r\n", QueueTest.Msgs[501]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[502] = %x \r\n", QueueTest.Msgs[502]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[503]);
      printf("jason add Framework receive from  tsp QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[504]);
    }

  }
#endif
#if 1

  if (ID_Queue_OTA_To_FW > 0) {
    len = msgrcv(ID_Queue_OTA_To_FW, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework receive from  ota no data \r\n");
      printf("jason add Framework receive from  ota no data \r\n");
      printf("jason add Framework receive from  ota no data \r\n");
    } else {
      printf("jason add Framework receive from  ota len = %d \r\n", len);
      printf("jason add Framework receive from  ota QueueTest.Msgs[0] = %d \r\n", QueueTest.Msgs[0]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[1] = %d \r\n", QueueTest.Msgs[1]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[2] = %d \r\n", QueueTest.Msgs[2]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[3] = %d \r\n", QueueTest.Msgs[3]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[4] = %d \r\n", QueueTest.Msgs[4]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[500] = %d \r\n", QueueTest.Msgs[500]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[501] = %d \r\n", QueueTest.Msgs[501]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[502] = %d \r\n", QueueTest.Msgs[502]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[503] = %d \r\n", QueueTest.Msgs[503]);
      printf("jason add Framework receive from  ota QueueTest.Msgs[503] = %d \r\n", QueueTest.Msgs[504]);
    }

  }
#endif
#if 1
  if (ID_Queue_PWR_To_FW > 0) {
    len = msgrcv(ID_Queue_PWR_To_FW, &QueueTest, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework receive from  pwr no data \r\n");
      printf("jason add Framework receive from  pwr no data \r\n");
      printf("jason add Framework receive from  pwr no data \r\n");
    } else {
      printf("jason add Framework receive from  pwr len = %d \r\n", len);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[0] = %x \r\n", QueueTest.Msgs[0]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[1] = %x \r\n", QueueTest.Msgs[1]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[2] = %x \r\n", QueueTest.Msgs[2]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[3] = %x \r\n", QueueTest.Msgs[3]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[4] = %x \r\n", QueueTest.Msgs[4]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[500] = %x \r\n", QueueTest.Msgs[500]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[501] = %x \r\n", QueueTest.Msgs[501]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[502] = %x \r\n", QueueTest.Msgs[502]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[503]);
      printf("jason add Framework receive from  pwr QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[504]);
    }

  }
#endif
#if 1
  if (ID_Queue_NVM_To_FW > 0) {
    len = msgrcv(ID_Queue_NVM_To_FW, &QueueTest, sizeof(QueueInfo_ST), 1, IPC_NOWAIT);
    if (len == -1) {
      printf("jason add Framework receive from  nvm no data \r\n");
      printf("jason add Framework receive from  nvm no data \r\n");
      printf("jason add Framework receive from  nvm no data \r\n");
    } else {
      printf("jason add Framework receive from  nvm len = %d \r\n", len);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[0] = %d \r\n", QueueTest.Msgs[0]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[1] = %d \r\n", QueueTest.Msgs[1]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[2] = %d \r\n", QueueTest.Msgs[2]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[3] = %d \r\n", QueueTest.Msgs[3]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[4] = %d \r\n", QueueTest.Msgs[4]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[500] = %d \r\n", QueueTest.Msgs[500]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[501] = %d \r\n", QueueTest.Msgs[501]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[502] = %d \r\n", QueueTest.Msgs[502]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[503] = %d \r\n", QueueTest.Msgs[503]);
      printf("jason add Framework receive from  nvm QueueTest.Msgs[503] = %d \r\n", QueueTest.Msgs[504]);
    }
  }
#endif

}

BOOL Framework::Run() {
  InformThread();

  return TRUE;
}

void Framework::Rcv_Queue_Mcu_To_FW() {
  int ret_len;

  ret_len = msgrcv(ID_Queue_MCU_To_FW, &mFwQueueInfoRev, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
  if (ret_len > 0) {
    //printf("[**framework.cpp->Rcv_Queue_Mcu_To_FW**]%s(%d): receive datalen : %d --- type : %d--%d\n", __func__, __LINE__, (int)ret_len,mFwQueueInfoRev.head.Gr,mFwQueueInfoRev.head.Id);
    Information_Parse(mFwQueueInfoRev.head.Gr, mFwQueueInfoRev.head.Id, ret_len);
  } else {
    //printf("[**framework.cpp->Rcv_Queue_Mcu_To_FW**]  no receive data !!!,errno=%d[%s]\n",errno,strerror(errno));
  }
#if 0
  if(mFwQueueInfoRev.head.Id == ID_MCU_2_OTA_PTC_UPG_DATA_REQ)
  {
    uint32_t offset ;
    uint16_t len ;


    fw_SendQueue(ID_FW_2_OTA_UPG_MCU_REQ);

  }
#endif

#if VIVIAN_ADD_MONITOR
  switch (mFwQueueInfoRev.head.Id) {
    case ID_MCU_2_FW_PTC_SYNC_REQ:m_fw_syncFlg[ID_THREAD_MCU] = 1;
      break;
  }
#endif

}

void Framework::Rcv_Queue_Ble_To_FW() {
  int ret_len;

  ret_len = msgrcv(ID_Queue_BLE_To_FW, &mFwQueueInfoRev, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
  if (ret_len > 0) {
    printf("[**framework.cpp->Rcv_Queue_Ble_To_FW**]%s(%d): receive datalen : %d --- type : %d\n",
           __func__,
           __LINE__,
           (int) ret_len,
           mFwQueueInfoRev.head.Gr);
  }
  //else
  //printf("[**framework.cpp->Rcv_Queue_Ble_To_FW**]%s(%d): no receive data !!!\n", __func__, __LINE__);
#if VIVIAN_ADD_MONITOR
  switch (mFwQueueInfoRev.head.Id) {
    case ID_BLE_2_FW_SYNC:m_fw_syncFlg[ID_THREAD_BLE] = 1;
      break;
  }
#endif
}

void Framework::Rcv_Queue_Tsp_To_FW() {
  int ret_len;

  mFwQueueInfoRev.head.Gr = 0;
  mFwQueueInfoRev.head.Id = 0;
  memset(mFwQueueInfoRev.Msgs, 0, sizeof(mFwQueueInfoRev.Msgs));
  fw_rev_msg_len = msgrcv(ID_Queue_TSP_To_FW, &mFwQueueInfoRev, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
  if (fw_rev_msg_len > 0) {
    printf("[**framework.cpp->Rcv_Queue_Tsp_To_FW**]%s(%d): receive datalen : %d --- type : %d--%d\n",
           __func__,
           __LINE__,
           (int) fw_rev_msg_len,
           mFwQueueInfoRev.head.Gr,
           mFwQueueInfoRev.head.Id);
    Information_Parse(mFwQueueInfoRev.head.Gr, mFwQueueInfoRev.head.Id, fw_rev_msg_len);
  }
  //else
  //printf("[**GR_THD_SOURCE_FW Tsp**]%s(%d): no receive data !!!\n", __func__, __LINE__);

#if VIVIAN_ADD_MONITOR
  switch (mFwQueueInfoRev.head.Id) {
    case ID_TSP_2_FW_THD_SYNC:m_fw_syncFlg[ID_THREAD_TSP] = 1;
      break;
  }
#endif

}

void Framework::Rcv_Queue_Ota_To_FW() {
  int ret_len;

  ret_len = msgrcv(ID_Queue_OTA_To_FW, &mFwQueueInfoRev, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
  if (ret_len > 0) {
    // printf("[**framework.cpp->Rcv_Queue_Ota_To_FW Ota**]%s(%d): receive datalen : %d --- type : %d\n", __func__, __LINE__, (int)ret_len,mFwQueueInfoRev.head.Gr);
    Information_Parse(mFwQueueInfoRev.head.Gr, mFwQueueInfoRev.head.Id, ret_len);
  }

#if VIVIAN_ADD_MONITOR
  switch (mFwQueueInfoRev.head.Id) {
    case ID_OTA_2_FW_SYNC:m_fw_syncFlg[ID_THREAD_OTA] = 1;
      break;
  }
#endif

}

void Framework::Rcv_Queue_Pwr_To_FW() {
  int ret_len;

  ret_len = msgrcv(ID_Queue_PWR_To_FW, &mFwQueueInfoRev, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
  if (ret_len > 0) {
    printf("[**framework.cpp->Rcv_Queue_Pwr_To_FW pwr**]%s(%d): receive datalen : %d --- type : %d\n",
           __func__,
           __LINE__,
           (int) ret_len,
           mFwQueueInfoRev.head.Gr);
    Information_Parse(mFwQueueInfoRev.head.Gr, mFwQueueInfoRev.head.Id, ret_len);
  }

#if VIVIAN_ADD_MONITOR
  switch (mFwQueueInfoRev.head.Id) {
    case ID_PM_2_FW_SYNC:m_fw_syncFlg[ID_THREAD_PWR] = 1;
      break;
  }
#endif
}

void Framework::Rcv_Queue_Nvm_To_FW() {
  int ret_len;

//  printf("fw %s \n", __FUNCTION__);
  ret_len = msgrcv(ID_Queue_NVM_To_FW, &mFwQueueInfoRev, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
  if (ret_len > 0) {
    printf("[*framework.cpp->Rcv_Queue_Nvm_To_FW  nvm**]%s(%d): receive datalen : %d --- type : %d\n",
           __func__,
           __LINE__,
           (int) ret_len,
           mFwQueueInfoRev.head.Gr);
  } else {
//     printf("[**GR_THD_SOURCE_FW Nvm**]  no receive data !!!,errno=%d[%s]\n",errno,strerror(errno));
  }

#if VIVIAN_ADD_MONITOR
  switch (mFwQueueInfoRev.head.Id) {
    case ID_NVM_2_FW_SYNC:m_fw_syncFlg[ID_THREAD_NVM] = 1;
      break;
  }
#endif
}
void Framework::Rcv_Queue_Log_To_FW() {
  int ret_len;

  ret_len = msgrcv(ID_Queue_NVM_To_FW, &mFwQueueInfoRev, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);
  if (ret_len > 0) {
    //   printf("[*framework.cpp->Rcv_Queue_Nvm_To_FW  nvm**]%s(%d): receive datalen : %d --- type : %d\n", __func__, __LINE__, (int)ret_len,mFwQueueInfoRev.head.Gr);
  } else {
    // printf("[**GR_THD_SOURCE_FW Nvm**]  no receive data !!!,errno=%d[%s]\n",errno,strerror(errno));
  }

#if VIVIAN_ADD_MONITOR
  switch (mFwQueueInfoRev.head.Id) {
    case ID_LOG_2_FW_SYNC:m_fw_syncFlg[ID_THREAD_LOG] = 1;
      break;
  }
#endif
}
BOOL Framework::fw_ParseMcu_VecihleReprot(void) {
  fw_SendQueue(ID_FW_2_MCU_PTC_VEICHLE_ACK);
  fw_SendQueue(ID_FW_2_TSP_PTC_VEICHLE_INFO_REPORT);
  return TRUE;
}

BOOL Framework::fw_ParseMcu_SyncAck(void) {
  fw_SendQueue(ID_FW_2_MCU_PTC_SYNC_ACK);
  return TRUE;
}

BOOL Framework::fw_ParseMcu_CommAck(void) {

  return TRUE;
}

BOOL Framework::fw_ParseMcu_Remoet_Ctl_Ack(void) {
  fw_SendQueue(ID_FW_2_TSP_PTC_REMOTE_CTL_ACK);
  return TRUE;
}

BOOL Framework::fw_ParseMcu_Upg_Begain(void) {

  OTA_UpgReqData_ST data;
  uint8_t len;
  len = MCU_PTC_HEAD_LEN + mFwQueueInfoRev.Msgs[8] + 1;
  memcpy(&data, mFwQueueInfoRev.Msgs + len, sizeof(OTA_UpgReqData_ST));
  VUPG_UpdateSlaveReqData(&data);

  fw_SendQueue(ID_FW_2_OTA_UPG_MCU_REQ);
  FWLOG("[printf]ID_MCU_2_OTA_PTC_UPG_DATA_REQ\n");

  return TRUE;
}

BOOL Framework::fw_ParseMcu(uint8_t Id) {
  switch (Id) {
    case ID_MCU_2_FW_PTC_SYNC_REQ: fw_ParseMcu_SyncAck();
      break;
    case ID_MCU_2_FW_PTC_COMMON_ACK:break;
    case ID_MCU_2_TSP_PTC_REMOTE_CTL_ACK:fw_ParseMcu_Remoet_Ctl_Ack();
      break;
    case ID_MCU_2_OTA_PTC_UPG_DATA_REQ: fw_ParseMcu_Upg_Begain();
      break;
    case ID_MCU_2_OTA_PTC_UPG_END:break;
    case ID_MCU_2_TSP_PTC_VEICHLE_INFO_REPORT:fw_ParseMcu_VecihleReprot();
      break;
    default:break;
  }
  return TRUE;
}

BOOL Framework::fw_ParseTsp(uint8_t Id) {
  switch (Id) {
    case ID_TSP_2_MCU_PTC_REMOTE_CTL: fw_ParseTsp_RemoetCtl();
      break;
    case ID_TSP_2_BLE_PTC_TOKEN: fw_ParseTsp_Token();
      break;
    default:break;
  }

  return TRUE;
}

BOOL Framework::fw_ParseTsp_RemoetCtl(void) {
  fw_SendQueue(ID_FW_2_MCU_PTC_REMOTE_CTL);
  return TRUE;
}

BOOL Framework::fw_ParseTsp_Token(void) {
  fw_SendQueue(ID_FW_2_BLE_PTC_TOKEN);
  fw_SendQueue(ID_FW_2_TSP_PTC_TOKEN_ACK);
  return TRUE;
}

BOOL Framework::fw_ParseBle(uint8_t Id) {
  switch (Id) {
    case ID_BLE_2_TSP_REQUEST_TOKEN: fw_ParseBle_Token_Req();
      break;
    case ID_BLE_2_MCU_PTC_CTLCMD:fw_ParseBle_Ctl_Cmd();
      break;
    default:break;
  }

  return TRUE;
}

BOOL Framework::fw_ParseBle_Token_Req(void) {
  fw_SendQueue(ID_FW_2_TSP_PTC_TOKEN_REQ);
  return TRUE;

}

BOOL Framework::fw_ParseBle_Ctl_Cmd(void) {
  fw_SendQueue(ID_FW_2_MCU_PTC_REMOTE_CTL);
  return TRUE;

}

BOOL Framework::fw_ParseOta(uint8_t Id, uint16_t len) {
  int msg_len;
  switch (Id) {
    case ID_OTA_2_FW_SYNC: FWLOG("[SUCCESS] ID_OTA_2_FW_SYNC!!\r\n");
      m_fw_syncCnt[ID_THREAD_OTA] = MCU_SYNC_CNT;
      break;
    case ID_OTA_2_MCU_PTC_UPD_BEGIN: FWLOG("[SUCCESS] ID_OTA_2_MCU_PTC_UPD_BEGIN!!\r\n");
      OTA_SocSendInfo_ST Info;
      memcpy(&Info, mFwQueueInfoRev.Msgs, sizeof(OTA_SocSendInfo_ST));
      VUPG_UpdateUpgHostReq(&Info);
      {
        fw_SendQueue(ID_FW_2_MCU_PTC_UPG_BEGAIN);
      }

      FWLOG("[SUCCESS] len:%d\r\n", Info.m_FileLen);

      FWLOG("[SUCCESS] crc:%x\r\n", Info.m_nCRC);

      break;
    case ID_OTA_2_MCU_PTC_UPD_TRANSFER: OTA_SendMcuData_ST Data;
      memcpy(&Data, mFwQueueInfoRev.Msgs, sizeof(OTA_SendMcuData_ST));
      VUPG_UpdateUpgHostData(&Data);

      FWLOG("[SUCCESS] before ID_OTA_2_MCU_PTC_UPD_TRANSFER!!\r\n");
      fw_SendQueue(ID_FW_2_MCU_PTC_UPG_DATA);
      FWLOG("[SUCCESS] after ID_OTA_2_MCU_PTC_UPD_TRANSFER!!\r\n");

      break;
    case ID_OTA_2_MCU_PTC_UPD_ACK: FWLOG("[SUCCESS] ID_OTA_2_MCU_PTC_UPD_ACK!!\r\n");
      fw_SendQueue(ID_FW_2_MCU_EVT5);
      break;
    case ID_OTA_2_MCU_PTC_UPD_END: FWLOG("[SUCCESS] ID_OTA_2_MCU_PTC_UPD_END!!\r\n");
      fw_SendQueue(ID_FW_2_MCU_EVT5);
      break;

    case ID_OTA_2_BLE_PTC_UPD_ACK: FWLOG("[SUCCESS] ID_OTA_2_BLE_PTC_UPD_ACK!!\r\n");
      fw_SendQueue(ID_FW_2_BLE_PTC_UPG_ACK);
      break;
    case ID_OTA_2_BLE_PTC_UPD_BEGAIN: FWLOG("[SUCCESS] ID_OTA_2_BLE_PTC_UPD_BEGAIN!!\r\n");
      fw_SendQueue(ID_FW_2_BLE_PTC_UPG_BEGAIN);
      break;
    case ID_OTA_2_BLE_PTC_UPD_END: FWLOG("[SUCCESS] ID_OTA_2_BLE_PTC_UPD_END!!\r\n");
      fw_SendQueue(ID_FW_2_BLE_PTC_UPG_END);
      break;
    case ID_OTA_2_BLE_PTC_UPD_TRANSFER: FWLOG("[SUCCESS] ID_OTA_2_BLE_PTC_UPD_TRANSFER!!\r\n");
      fw_SendQueue(ID_FW_2_BLE_PTC_UPG_TRANSFER);
      break;
    case ID_OTA_2_TSP_PTC_TBOXSEND_VERSION_QUERY: FWLOG("[SUCCESS] ID_OTA_2_TSP_PTC_TBOXSEND_VERSION_QUERY!!\r\n");
      fw_SendQueue(ID_FW_2_TSP_PTC_REMOTE_UPGRADE);
      break;
    case ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK: FWLOG("[SUCCESS] ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK!!\r\n");
      fw_SendQueue(ID_FW_2_TSP_EVT7);
      break;
      //case ID_OTA_2_TSP_THD_RESET_LTE:
      //	break;
    case ID_OTA_2_FW_UPG_STS: FWLOG("[SUCCESS] ID_OTA_2_FW_UPG_STS!!\r\n");
      break;
    case ID_OTA_2_NVM_SAVE_OSID: FWLOG("[SUCCESS] ID_OTA_2_NVM_SAVE_OSID!!\r\n");
      fw_SendQueue(ID_FW_2_NVM_EVT0);
      break;
  }

  return TRUE;
}

BOOL Framework::fw_ParsePwr(uint8_t Id) {

  return TRUE;
}

BOOL Framework::Information_Parse(char Gr, char id, uint16_t len) {
  switch (Gr) {
    case GR_THD_SOURCE_OTA:fw_ParseOta(id, len);
      break;
    case GR_THD_SOURCE_MCU:fw_ParseMcu(id);
      break;
    case GR_THD_SOURCE_TSP: fw_ParseTsp(id);
      break;
    case GR_THD_SOURCE_PM:break;
    case GR_THD_SOURCE_NVM:break;
    case GR_THD_SOURCE_BLE:fw_ParseBle(id);
      break;
    default:break;
  }

  return TRUE;
}

#if VIVIAN_ADD_MONITOR
void Framework::fw_Thread_Monitor_Init(void) {
  uint8_t i;

  for (i = ID_THREAD_OTA; i < ID_THREAD_ALL; i++) {
    m_fw_syncFlg[i] = TRUE;

    m_fw_syncCnt[i] = MCU_SYNC_CNT;
  }
}

void Framework::fw_Thread_Monitor_Process(void) {

  uint8_t i;

  uint8_t flag = 0;

  for (i = ID_THREAD_OTA; i < ID_THREAD_ALL; i++) {
    m_fw_syncCnt[i]--;

    if (m_fw_syncCnt[i] == 0) {
      //VPAK_Sync();//send sync PTC TO MCU-->8090
      fw_SendQueue(ID_MCU_2_FW_PTC_SYNC_REQ);

      break;
    }
  }
}
#endif

int Framework::fw_SendQueue(uint8_t Id) {
  uint8_t Dest_Group = ((Id & 0xF0) >> 4);

  switch (Dest_Group) {

    case GR_THD_SOURCE_TSP: fw_TspSendQueue(Id);
      break;

    case GR_THD_SOURCE_MCU: fw_McuSendQueue(Id);
      break;

    case GR_THD_SOURCE_BLE: fw_BleSendQueue(Id);
      break;

    case GR_THD_SOURCE_OTA: fw_OtaSendQueue(Id);
      break;

    default:break;

  }
}

int Framework::fw_McuSendQueue(uint8_t Id) {
  int ret_len;
  uint16_t msg_len = 0;
  fwTCTL_RemoteCtrl_UN m_tempRemoteCtlData;

  switch (Id) {
    case ID_FW_2_MCU_PTC_SYNC_ACK: {
      if (VPAK_PackageData(&msg_len, VPAK_HostACK_CMDID, (uint8_t *) mVPAK_Information.Buf) == RET_OK) {
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_MCU_PTC_SYNC_ACK;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mVPAK_Information.Buf, msg_len);
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_MCU,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
        if (ret_len == -1) {
          //printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_SYNC_ACK msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
        } else {
          //printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_SYNC_ACK msgsnd() write msg ok\n");

        }
      } else {
        //printf("framework.cpp->fw_McuSendQueue VPAK_PackageData sync ack fail!!\n");
      }
    }
      break;
    case ID_FW_2_MCU_PTC_VEICHLE_ACK: {
      if (VPAK_PackageData(&msg_len, VPAK_HostStatusReport_CMDID, (uint8_t *) mVPAK_Information.Buf) == RET_OK) {
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_MCU_PTC_VEICHLE_ACK;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mVPAK_Information.Buf, msg_len);
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_MCU,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
        if (ret_len < 0) {
          //printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_VEICHLE_ACK msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
        } else {
          //printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_VEICHLE_ACK msgsnd() write msg ok\n");
        }
      } else {
        printf("framework.cpp->fw_McuSendQueue VPAK_PackageData veichle ack fail!!\n");
      }
    }
      break;
    case ID_FW_2_MCU_PTC_REMOTE_CTL: {
      if (VPAK_PackageData(&msg_len, VPAK_REMOTECTRL_CMDID, (uint8_t *) mVPAK_Information.Buf) == RET_OK) {
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_MCU_PTC_REMOTE_CTL;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mVPAK_Information.Buf, msg_len);
        printf("framework.cpp  fw_McuSendQueue from tsp remote ctl to mcu uqeue Data:");
        for (int i = 0; i < msg_len; i++) {
          printf("%02x", mFwQueueInfoSnd.Msgs[i]);
        }
        printf("\r\n");
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_MCU,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
#if 0
        if (ret_len < 0)
                printf("framework.cpp->fw_McuSendQueue ID_TSP_2_MCU_PTC_REMOTE_CTL msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
        else
          printf("framework.cpp->fw_McuSendQueue ID_TSP_2_MCU_PTC_REMOTE_CTL msgsnd() write msg success!!!\n");
#endif
      } else {
        printf("framework.cpp  fw_McuSendQueue VPAK_PackageData failled \r\n");

      }
    }
      break;

    case ID_FW_2_MCU_PTC_UPG_BEGAIN: FWLOG("ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");

      if (VPAK_PackageData(&msg_len, VPAK_SendUpgrade_CMDID, (uint8_t *) mVPAK_Information.Buf) == RET_OK) {

        FWLOG("ID_FW_2_MCU_PTC_UPG_BEGAIN send to MCU\r\n");

        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_MCU_PTC_UPG_BEGAIN;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mVPAK_Information.Buf, msg_len);

        ret_len = msgsnd(m_instance->ID_Queue_FW_To_MCU,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
        if (ret_len < 0) {
          //printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_VEICHLE_ACK msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
        } else {
          //printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_VEICHLE_ACK msgsnd() write msg ok\n");
        }
      } else {

        FWLOG("[ERROR]VPAK_PackageData VPAK_SendUpgrade_CMDID ERROR!!\r\n");
      }

      break;
    case ID_FW_2_MCU_PTC_UPG_DATA: printf("fw_MCUsendQueue ID_FW_2_MCU_PTC_UPG_DATAstart\r\n");
      //memset(mFwQueueInfoSnd.Msgs,0,sizeof(mFwQueueInfoSnd.Msgs));

      if (VPAK_PackageData(&msg_len, VPAK_UpgradeDateAACK_CMDID, (uint8_t *) mFwQueueInfoSnd.Msgs) == RET_OK) {
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_MCU_PTC_UPG_DATA;
#if 0
        printf("VPAK_UpgradeDateAACK_CMDID start\r\n");
        for(int i =0 ; i < msg_len; i ++)
        {
          printf("%02x ",mFwQueueInfoSnd.Msgs[i]);
        }
        printf("VPAK_UpgradeDateAACK_CMDID end\r\n");
#endif
        //memset(mFwQueueInfoSnd.Msgs,0,sizeof(mFwQueueInfoSnd.Msgs));
        //memcpy(mFwQueueInfoSnd.Msgs, mVPAK_Information.Buf, msg_len);

        ret_len = msgsnd(m_instance->ID_Queue_FW_To_MCU,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
        if (ret_len < 0) {
          printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_VEICHLE_ACK msgsnd() write msg failed,errno=%d[%s]\n",
                 errno,
                 strerror(errno));
        } else {
          printf("framework.cpp->fw_McuSendQueue ID_FW_2_MCU_PTC_VEICHLE_ACK msgsnd() write msg ok\n");
        }
      } else {

        printf("framework.cpp->VPAK_UpgradeDateAACK_CMDID  FAILE!!!!\n");
      }
      break;
    default:break;
  }

  return TRUE;

  /*QueueInfo_ST st_SndMsg;
  uint8_t sendLen = 0;
  int res = 0;

  //---------------------------
  //STEP1:package data
  //--------------------------
  switch(Id)
  {
  case ID_FW_2_MCU_PTC_SYNC_ACK:
      st_SndMsg.head.Gr = GR_THD_SOURCE_FW;
      st_SndMsg.head.Id = ID_FW_2_MCU_PTC_SYNC_ACK;
      sendLen = 2;
      break;

  default:
      break;
  }

  //---------------------------
  //STEP2:SEND data By Len
  //---------------------------
  res = msgsnd(ID_Queue_FW_To_MCU,&st_SndMsg,sendLen,IPC_NOWAIT);
  if(res <= 0 )
  {
      FWLOG("[ERROR]: send %d,res:%d\n",sendLen,res);
      return FALSE;
  }
  else
  {
      return TRUE;
  }*/
}

int Framework::fw_PwrSendQueue(uint8_t Id) {

}

int Framework::fw_OtaSendQueue(uint8_t Id) {

  int ret_len;
  uint16_t len = 0;
  //uint8_t *pos = NULL;

  mFwQueueInfoSnd.head.Gr = GR_THD_SOURCE_FW;
  mFwQueueInfoSnd.head.Id = Id;
  mFwQueueInfoSnd.mtype = 1;

  switch (Id) {
    case ID_FW_2_OTA_UPG_MCU_REQ:

      FWLOG("[SUCCESS] FW REQ MCU UPG\r\n");
      OTA_UpgReqData_ST *pInfo;
      pInfo = VPUG_GetSlaveReqData();

#if 0
    len = 6;
    uint8_t  index;
    printf("##########OTA####FW #######\r\n");
    for(int i =0; i < 64; i ++)
    {
      printf("%02x ",mFwQueueInfoRev.Msgs[i]);
    }
    printf("\r\n##########OTA#####FW ######\r\n");

    pos = mFwQueueInfoRev.Msgs + 8;
    index = *pos;
    pos ++;
    pos += index;
    index = mFwQueueInfoRev.Msgs[0];
    memcpy(mFwQueueInfoSnd.Msgs,pos,4);
    pos += 4;
    memcpy(mFwQueueInfoSnd.Msgs+4,pos,2);

    printf("##########OTA####send to ota #######\r\n");
    for(int i =0; i < 6;i ++)
    {
      printf("%02x ",mFwQueueInfoSnd.Msgs[i]);
    }
    printf("\r\n##########OTA###send to ota ######\r\n");
#else
      memcpy(mFwQueueInfoSnd.Msgs, pInfo, sizeof(OTA_UpgReqData_ST));

#if 0
    printf("##########FW SEND2OTA####send to ota #######\r\n");
   for(int i =0; i < 6;i ++)
   {
     printf("%02x ",mFwQueueInfoSnd.Msgs[i]);
   }
    printf("\r\n##########FW SEND2OTA###send to ota ######\r\n");
#endif

#endif
      break;
    case ID_FW_2_OTA_UPG_MCU_RESULT: FWLOG("[SUCCESS] FW RESULT MCU UPG\r\n");
      break;
  }

  ret_len = msgsnd(m_instance->ID_Queue_FW_To_OTA,
                   (void *) &mFwQueueInfoSnd,
                   sizeof(QueueInfo_ST) - sizeof(long),
                   IPC_NOWAIT);
  if (ret_len < 0) {
    FWLOG("[ERROR] FW 2 OTA QUEUE SEND !\r\n");
    //printf("framework.cpp->fw_ParseMcu_VecihleReprot msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
  } else {
    //printf("framework.cpp->fw_ParseMcu_VecihleReprot fw send to tsp write msg ok\n");
  }

}

int Framework::fw_NvmSendQueue(uint8_t Id) {
  int retLen;
  uint16_t msgLen =1024;

  mFwQueueInfoSnd.mtype = 1;
  mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_NVM;
  mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_NVM_FWINFO;
  memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
  memcpy(mFwQueueInfoSnd.Msgs, "9", msgLen);

  retLen = msgsnd(m_instance->ID_Queue_FW_To_NVM,
                  (void *) &mFwQueueInfoSnd,
                  msgLen + sizeof(ParaInfoHead_ST),
                  IPC_NOWAIT);
}

int Framework::fw_TspSendQueue(uint8_t Id) {
  int ret_len;
  uint16_t msg_len;

  switch (Id) {
    case ID_FW_2_TSP_PTC_VEICHLE_INFO_REPORT: {
      if (TPAK_PackageData(&msg_len, TPAK_VEICHLEINFO_APPID, (uint8_t *) mTPAK_Information.Buf) == RET_OK) {
        //TPAK_Printf();
        //send to tsp
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_TSP_PTC_VEICHLE_INFO_REPORT;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mTPAK_Information.Buf, msg_len);
        //printf("framework.cpp->fw_TspSendQueue:%d -- len:%d\r\n",mFwQueueInfoSnd.head.Gr,msg_len);
        /*printf("mFwQueueInfoSnd send  to tsp uqeue Data:");
        for(int i=0;i<msg_len;i++)
        {
          printf("%2x",mFwQueueInfoSnd.Msgs[i]);
        }
        printf("\r\n");*/
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_TSP,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
#if 0
        if (ret_len < 0)
          printf("framework.cpp->fw_ParseMcu_VecihleReprot msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
        else
          printf("framework.cpp->fw_ParseMcu_VecihleReprot fw send to tsp write msg ok\n");
#endif
      } else {
        printf("framework mcu TPAK fail!!\n");
      }

      //send to ble
      if (TPAK_PackageData(&msg_len, TPAK_VEICHLEINFO_APPID, (uint8_t *) mTPAK_Information.Buf) == RET_OK) {
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_BLE_PTC_VEICHLE_INFO_REPORT;
#if 0
        printf("mFwQueueInfoSnd send  to ble uqeue Data:");
  for(int i=0;i<msg_len;i++)
  {
    printf("%02x ",mFwQueueInfoSnd.Msgs[i]);
  }
  printf("\r\n");
#endif
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_BLE,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);

        //printf("framework.cpp->fw_ParseMcu_VecihleReprot:send Gr:%d -- Id:%d\r\n",mFwQueueInfoSnd.head.Gr,mFwQueueInfoSnd.head.Id);
        if (ret_len < 0) {
          //printf("framework.cpp->fw_ParseMcu_VecihleReprot msgsnd() write msg send to ble failed,errno=%d[%s]\n",errno,strerror(errno));
        } else {
          //printf("framework.cpp->fw_ParseMcu_VecihleReprot fw send to ble write msg ok\n");
        }
      }

    }
      break;
    case ID_FW_2_TSP_PTC_REMOTE_CTL_ACK: {
      if (TPAK_PackageData(&msg_len, TPAK_RemoteCtrlACK_APPID, (uint8_t *) mTPAK_Information.Buf) == RET_OK) {
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_TSP_PTC_REMOTE_CTL_ACK;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mTPAK_Information.Buf, msg_len);

#if  0
        printf("framework.cpp->fw_TspSendQueue send remote ctl ack to tsp Gr:%d -- len:%d\r\n",mFwQueueInfoSnd.head.Gr,msg_len);
        printf("mFwQueueInfoSnd send remote ctl ack to tsp uqeue Data:");
        for(int i=0;i<msg_len;i++)
        {
          printf("%02x ",mFwQueueInfoSnd.Msgs[i]);
        }
        printf("\r\n");
#endif
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_TSP,
                         (void *) &mFwQueueInfoSnd,
                         msg_len + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
        if (ret_len < 0) {
          //printf("framework.cpp->send remote ctl ack to tsp msgsnd() write msg send to tsp failed,errno=%d[%s]\n",errno,strerror(errno));
        } else {
          //printf("framework.cpp->send remote ctl ack to tsp fw send to tsp write msg ok\n");
        }
      } else {
        printf("framework mcu TPAK fail!!\n");
      }
    }
      break;
    case ID_FW_2_TSP_PTC_TOKEN_REQ: {
      if (BPAK_PackageData(BPAK_REPROT_VECIHLE_INFORMATION, (uint8_t *) mTPAK_Information.Buf) == RET_OK) {
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_TSP_PTC_TOKEN_REQ;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mTPAK_Information.Buf, 112);

#if 0
        printf("framework.cpp->fw_TspSendQueue send token req to tsp Gr:%d -- len:%d\r\n",mFwQueueInfoSnd.head.Gr,112);
        printf("mFwQueueInfoSnd send token req to tsp uqeue Data:");
        for(int i=0;i<112;i++)
        {
          printf("%02x ",mFwQueueInfoSnd.Msgs[i]);
        }
        printf("\r\n");
#endif
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_TSP,
                         (void *) &mFwQueueInfoSnd,
                         112 + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);

        if (ret_len < 0) {
          //printf("framework.cpp->send token req to tsp msgsnd() write token msg send to tsp failed,errno=%d[%s]\n",errno,strerror(errno));
        } else {
          //printf("framework.cpp->send token req to tsp  write token msg ok\n");
        }
      } else {
        printf("framework ble BPAK token fail!!\n");
      }
    }
      break;
    case ID_FW_2_TSP_PTC_TOKEN_ACK:break;
    default:break;
  }

  return TRUE;
}

int Framework::fw_BleSendQueue(uint8_t Id) {
  int ret_len;

  switch (Id) {
    case ID_FW_2_BLE_PTC_TOKEN: {
      if (BPAK_PackageData(BPAK_REPROT_VECIHLE_INFORMATION, (uint8_t *) mTPAK_Information.Buf) == RET_OK) {
        mFwQueueInfoSnd.mtype = 1;
        mFwQueueInfoSnd.head.Gr = (uint8_t) GR_THD_SOURCE_FW;
        mFwQueueInfoSnd.head.Id = (uint8_t) ID_FW_2_BLE_PTC_TOKEN;
        memset(mFwQueueInfoSnd.Msgs, 0, sizeof(mFwQueueInfoSnd.Msgs));
        memcpy(mFwQueueInfoSnd.Msgs, mTPAK_Information.Buf, 112);
#if 0
        printf("framework.cpp->fw_TspSendQueue send token value to ble Gr:%d -- len:%d\r\n",mFwQueueInfoSnd.head.Gr,112);
        printf("mFwQueueInfoSnd send token value to ble uqeue Data:");
        for(int i=0;i<112;i++)
        {
          printf("%02x ",mFwQueueInfoSnd.Msgs[i]);
        }
        printf("\r\n");
#endif
        ret_len = msgsnd(m_instance->ID_Queue_FW_To_TSP,
                         (void *) &mFwQueueInfoSnd,
                         112 + sizeof(ParaInfoHead_ST),
                         IPC_NOWAIT);
        if (ret_len < 0) {
          //printf("framework.cpp->send token value to ble msgsnd() write token msg send to tsp failed,errno=%d[%s]\n",errno,strerror(errno));
        } else {
          //printf("framework.cpp->send token value to ble  write token msg ok\n");
        }
      } else {
        printf("framework ble BPAK token fail!!\n");
      }
    }
      break;
    default:break;
  }

}

int Framework::fw_LogSendQueue(uint8_t Id) {

}

