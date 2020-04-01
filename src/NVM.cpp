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
#include <Nvm.h>
#include "common.h"
#include "Nvm.h"

QueueInfo_ST nvmQueueInfoRev;
QueueInfo_ST nvmQueueInfoSnd;

static uint32_t gl_u32CrcTable[XL_LEN];
static uint32_t gl_crc = 0xffffffff;

CNvm *CNvm::m_instance = NULL;

CNvm::CNvm() {

}

CNvm::~CNvm() {

}

CNvm *CNvm::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new CNvm;
  }
  return m_instance;
}

void CNvm::FreeInstance() {

  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;

}

BOOL CNvm::Init() {

  NVMLOG("INIT \n");

  m_binit = false;

  nvm_Init();

  m_binit = CreateThread(); /*param is default time*/

  return m_binit;
}

void CNvm::Deinit() {
  KillThread();
}

void CNvm::Run() {

  InformThread();

}
BOOL CNvm::Processing() {
  int ret_len;
  mFramework = Framework::GetInstance();

  while (!m_bExit) {
#if QUEUE_TEST_MACRO
    ReceiveQueueTestTask();
     SendQueueTestTask();
    Sleep(200 * 1000);
#else
    printf("nvm processing \r\n");

//    SendQueueTestTask();
//    printf("VIN %s \n ", &m_rFileFormat.m_stConfig.BYTES.m_data_parameter.m_stFirmwareParameter.VIN);

    QuId_NVM_2_FW = mFramework->ID_Queue_NVM_To_FW;
    QuId_FW_2_NVM = mFramework->ID_Queue_FW_To_NVM;

    printf("** %d ** %d ** \n", QuId_NVM_2_FW, QuId_NVM_2_FW);

    if (QuId_NVM_2_FW != C_FALSE && QuId_FW_2_NVM != C_FALSE) {
      m_u8WorkMode = NVMWORK_NORMAL;
    } else {
      m_u8WorkMode = NVMWORK_NOTREADY;
      NVMLOG("[ERROR] nvm not ready!!\n");
    }

    switch (m_u8WorkMode) {
      case NVMWORK_NOTREADY:NVMLOG("[NVM_PROCESS]NVMWORK_NOTREADY!!\n");
        break;

      case NVMWORK_INIT:NVMLOG("[NVM_PROCESS]NVMWORK_INIT!!\n");
        break;

      case NVMWORK_NORMAL:NVMLOG("[NVM_PROCESS]NVMWORK_NORMAL!!\n");
        nvm_SyncProcess();
        nvm_RecvQueue();
        nvm_WriteProcess();
//        nvm_SendProcess();

        break;
      default:break;
    }

    Sleep(NVM_THREAD_PERIOD);
#endif
  }

}

BOOL CNvm::TimeoutProcessing() {
  printf("jason add ble timeout processing \r\n");
  return true;
}

void CNvm::ReceiveQueueTestTask() {
  int len = 0;
  QueueInfo_ST QueueTest;

  if (Framework::GetInstance()->ID_Queue_FW_To_NVM > 0) {
    len = msgrcv(Framework::GetInstance()->ID_Queue_FW_To_NVM,
                 &QueueTest,
                 sizeof(QueueInfo_ST) - sizeof(long),
                 1,
                 IPC_NOWAIT);
    if (len == -1) {
      printf("jason add nvm receive from  fw no data \r\n");
      printf("jason add nvm receive from  fw no data \r\n");
      printf("jason add nvm receive from  fw no data \r\n");
    } else {
      printf("jason add nvm receive from  fw len = %d \r\n", len);
      printf("jason add nvm receive from  fw QueueTest.Msgs[0] = %x \r\n", QueueTest.Msgs[0]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[1] = %x \r\n", QueueTest.Msgs[1]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[2] = %x \r\n", QueueTest.Msgs[2]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[3] = %x \r\n", QueueTest.Msgs[3]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[4] = %x \r\n", QueueTest.Msgs[4]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[500] = %x \r\n", QueueTest.Msgs[500]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[501] = %x \r\n", QueueTest.Msgs[501]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[502] = %x \r\n", QueueTest.Msgs[502]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[503]);
      printf("jason add nvm receive from  fw QueueTest.Msgs[503] = %x \r\n", QueueTest.Msgs[504]);
    }
  }

}

void CNvm::SendQueueTestTask() {
  QueueInfo_ST QueueTest;
  int len = 0;
  static int count = 0;
  QueueTest.mtype = 1;

  Framework::GetInstance()->fw_NvmSendQueue(ID_FW_2_NVM_SET_CONFIG);

#if 0
  if ((Framework::GetInstance()->ID_Queue_NVM_To_FW > 0) && (count++ % 5 == 0)) {
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

    len = msgsnd(Framework::GetInstance()->ID_Queue_NVM_To_FW,
                 &QueueTest,
                 sizeof(QueueInfo_ST) - sizeof(long),
                 IPC_NOWAIT);
    if (len == -1) {
      printf("jason add nvm send to Framework error \r\n");

    } else {
      printf("jason add nvm send to mcu Framework success\r\n");

    }
  }
#endif

}

bool CNvm::nvm_SyncProcess() {
  static uint8_t nvmSyncCnt = 0;
  /*only need send SYNC ID*/

  nvmSyncCnt++;

  if (nvmSyncCnt >= CNT_SYNC_5S) {
    nvmSyncCnt = 0;

    nvm_SendQueue(GR_THD_SOURCE_NVM, ID_NVM_2_FW_SYNC);
  }

}
int CNvm::nvm_SendQueue(uint8_t Gr, uint8_t Id) {
  m_stSndMsg.head.Gr = Gr;
  m_stSndMsg.head.Id = Id;
  m_stSndMsg.mtype = 1;
  uint16_t Cmdlen = 0;
  int resLen = 0;

  switch (Id) {

    case ID_NVM_2_FW_SYNC:NVMLOG("ID_NVM_2_FW_SYNC\n");
      m_stSndMsg.Msgs[0] = 0x63;
      break;
    case ID_NVM_2_FW_GET_CONFIG:NVMLOG("ID_NVM_2_FW_GET_SYSTEM\n");
      memcpy(&m_stSndMsg, &m_wFileFormat.m_stConfig, CFG_PART_SIZE);
      break;
    case ID_NVM_2_FW_GET_SYSTEM:NVMLOG("ID_NVM_2_FW_GET_SYSTEM\n");
      memcpy(&m_stSndMsg, &m_wFileFormat.m_stSystem, SYS_PART_SIZE);
      break;
    case ID_NVM_2_FW_GET_FAULT:NVMLOG("ID_NVM_2_FW_GET_FAULT\n");
      memcpy(&m_stSndMsg, &m_wFileFormat.m_stFault, FAU_PART_SIZE);
      break;
    case ID_NVM_2_FW_SET_CONFIG:NVMLOG("ID_NVM_2_FW_SET_SYSTEM\n");
      memcpy(&m_stSndMsg, &m_wFileFormat.m_stConfig, CFG_PART_SIZE);
      break;
    case ID_NVM_2_FW_SET_SYSTEM:NVMLOG("ID_NVM_2_FW_SET_SYSTEM\n");
      memcpy(&m_stSndMsg, &m_wFileFormat.m_stSystem, SYS_PART_SIZE);
      break;
    case ID_NVM_2_FW_SET_FAULT:NVMLOG("ID_NVM_2_FW_SET_FAULT\n");
      memcpy(&m_stSndMsg, &m_wFileFormat.m_stFault, FAU_PART_SIZE);
      break;
    default:break;

  }

  resLen = msgsnd(QuId_NVM_2_FW, (void *) &m_stSndMsg, sizeof(QueueInfo_ST) - sizeof(long), IPC_NOWAIT);

  if (resLen == C_FALSE) {
    NVMLOG("[ERROR]: LEN MISMATCH :cmdlen %d,reslen:%d\n", Cmdlen, resLen);
    return C_FALSE;
  } else {
    return C_TRUE;
  }

}

int CNvm::nvm_RecvQueue() {

  int readLen = 0;

  readLen = msgrcv(QuId_FW_2_NVM, (void *) &m_stRevMsg, sizeof(QueueInfo_ST) - sizeof(long), 1, IPC_NOWAIT);

  if (readLen > 0) {
    NVMLOG("[SUCCESS]RevData len %d \n", readLen);
    NVMLOG("[SUCCESS]RevData : 0x%x,%s \n", m_stRevMsg.head.Id, (char *) m_stRevMsg.Msgs);
  } else {
    NVMLOG("[WARN] no receive data !!!\n");
    return FALSE;
  }

  switch (m_stRevMsg.head.Gr) {

    case GR_THD_SOURCE_NVM:
      switch (m_stRevMsg.head.Id) {

        case ID_FW_2_NVM_SET_CONFIG:NVMLOG("ID_FW_2_NVM_SET_CONFIG\n");
          memset(&m_wFileFormat.m_stConfig, 0, sizeof(m_wFileFormat.m_stConfig));
          memcpy(&m_wFileFormat.m_stConfig, m_stRevMsg.Msgs, CFG_PART_SIZE);
          m_bDataChangeFlag = true;
          break;
        case ID_FW_2_NVM_SET_SYSTEM:NVMLOG("ID_FW_2_NVM_SET_SYSTEM\n");
          memset(&m_wFileFormat.m_stSystem, 0, sizeof(m_wFileFormat.m_stSystem));
          memcpy(&m_wFileFormat.m_stSystem, m_stRevMsg.Msgs, SYS_PART_SIZE);
          m_bDataChangeFlag = true;
          break;
        case ID_FW_2_NVM_SET_FAULT:NVMLOG("ID_FW_2_NVM_SET_FAULT\n");
          memset(&m_wFileFormat.m_stFault, 0, sizeof(m_wFileFormat.m_stFault));
          memcpy(&m_wFileFormat.m_stFault, m_stRevMsg.Msgs, FAU_PART_SIZE);
          m_bDataChangeFlag = true;
          break;
        case ID_FW_2_NVM_GET_CONFIG:NVMLOG("ID_FW_2_NVM_GET_CONFIG\n");
          nvm_SendQueue(GR_THD_SOURCE_NVM, ID_NVM_2_FW_GET_CONFIG);
          break;
        case ID_FW_2_NVM_GET_SYSTEM:NVMLOG("ID_FW_2_NVM_GET_SYSTEM\n");
          nvm_SendQueue(GR_THD_SOURCE_NVM, ID_NVM_2_FW_GET_SYSTEM);
          break;
        case ID_FW_2_NVM_GET_FAULT:NVMLOG("ID_FW_2_NVM_GET_FAULT\n");
          nvm_SendQueue(GR_THD_SOURCE_NVM, ID_NVM_2_FW_GET_FAULT);
          break;
        default:NVMLOG("default \n");
          break;

      }
      break;
    default:break;
  }
}
int CNvm::nvm_ReadFile() {
  NVMLOG("nvm_ReadFile \n");
  //TODO:read file
  FILE *fp = NULL;

  fp = fopen(SAVE_FILE_PATH, "rb+");
  if (fp == NULL) {
    NVMLOG("open file fail\n");
    return C_FALSE;
  }

  fread(&m_wFileFormat, sizeof(uint8_t), sizeof(m_wFileFormat), fp);
  return C_TRUE;
}
int CNvm::nvm_WriteFile() {

  NVMLOG("nvm_WriteFile \n");

  FILE *fp = NULL;

//  memset(&m_wFileFormat, 0, sizeof(m_wFileFormat));

  fp = fopen(SAVE_FILE_PATH, "wb+");
  if (fp == NULL) {
    NVMLOG("open file fail\n");
    return C_FALSE;
  }

  printf("m_wFileFormat size %d \n", sizeof(m_wFileFormat));
  printf("m_stConfig size %d \n", sizeof(m_wFileFormat.m_stConfig));
  printf("m_stSystem size %d \n", sizeof(m_wFileFormat.m_stSystem));
  printf("m_stFault size %d \n", sizeof(m_wFileFormat.m_stFault));
  printf("m_Crc32 size %d \n", sizeof(m_wFileFormat.m_Crc32));

  fwrite(&m_wFileFormat, sizeof(m_wFileFormat), sizeof(uint8_t), fp);

  fclose(fp);
  NVMLOG("nvm_WriteFile finish \n");

  return C_TRUE;
}

void CNvm::initCrcTable() {
  NVMLOG("initCrcTable \n");
  uint32_t c;
  uint32_t i;
  uint32_t j;

  for (i = 0; i < 256; i++) {
    c = (uint32_t) i;
    for (j = 0; j < 8; j++) {
      if (c & 1)
        c = 0xedb88320L ^ (c >> 1);
      else
        c = c >> 1;
    }
    gl_u32CrcTable[i] = c;
  }
}
uint32_t CNvm::getCRC32(uint32_t crc, uint8_t *buffer, uint32_t size) {
  uint32_t i;
  for (i = 0; i < size; i++) {
    crc = gl_u32CrcTable[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
  }
  return crc;
}
int CNvm::calcImgCRC(uint8_t *buffer, uint32_t *incrc) {
  int fd;
  int nread;
  int ret;
  uint8_t buf[BUFSIZE];

  NVMLOG("calcImgCRC \n");
  NVMLOG("calcImgCRC %d \n", buffer);
  NVMLOG("calcImgCRC %d \n", incrc);

  while ((nread = read(fd, buf, BUFSIZE)) > 0) {
    printf("+++ %s", buf);
//    incrc = getCRC32(incrc, buf, nread);
  }
//  *incrc = incrc;

  printf("--- \n");

  close(fd);

  if (nread < 0) {
    printf("%d:read %s.\n", __LINE__, strerror(errno));
    return C_FALSE;
  }

  NVMLOG("calcImgCRC finish \n");

  return C_TRUE;
}

int CNvm::GetInitSts() {
  return m_u8WorkMode;
}
int CNvm::nvm_Init() {
  NVMLOG("nvm_Init \n");
  uint8_t Buff[INFOSIZE] = {0};
//  uint32_t crc = 0xffffffff;
  uint32_t crc32 = NULL;

  m_u8WorkMode = NVMWORK_NOTREADY;
  QuId_NVM_2_FW = C_FALSE;
  QuId_FW_2_NVM = C_FALSE;
  m_bDataChangeFlag = false;
  initCrcTable();

  nvm_BackupProcess();
  nvm_ReadFile();

  memset(&Buff, 0, sizeof(Buff));
  memcpy(&Buff, &m_wFileFormat, sizeof(Buff));

//  m_rFileFormat.m_Crc32 = 0x5654ad40;
//
//  for (int i = 0; i < sizeof(Buff); ++i) {
//    printf("0x%x  \n", Buff[i]);
//  }

  crc32 = getCRC32(gl_crc, Buff, sizeof(Buff));

  NVMLOG("crc32 after \n");
  printf("crc32 == %x \n", crc32);
  printf("ffcrc32 == %x \n", m_wFileFormat.m_Crc32);

  if (crc32 == m_wFileFormat.m_Crc32) {
    printf("crc ok \n");
  } else {
    printf("crc fail \n");
    nvm_DefaultRestore();
//    nvm_WriteFile();
    m_bDataChangeFlag = true;
    nvm_WriteProcess();
  }

  return 0;
}
int CNvm::nvm_WriteProcess() {
  int ret;

  m_u8WorkMode = NVMWORK_BUSY;

  if (m_bDataChangeFlag) {
    for (int i = 0; i < 3; ++i) {
      ret = nvm_WriteFile();
      if (ret == C_FALSE && i < 2) {
        continue;
      } else {
        break;
      }
    }
  }
  NVMLOG("write ret == %d \n", ret);

  if (ret == C_TRUE) {
    m_bDataChangeFlag = false;
  }

  m_u8WorkMode = NVMWORK_NORMAL;
  return ret;
}
int CNvm::nvm_DefaultRestore() {

  NVMLOG("nvm_DefaultRestore \n");

  uint8_t Buff[INFOSIZE] = {0};

  memset(&m_wFileFormat, 0, sizeof(m_wFileFormat));

#if 0
  memcpy(m_stDataParameter.m_stFirmwareParameter.VIN,
         DEFAULT_VIN,
         sizeof(m_stDataParameter.m_stFirmwareParameter.VIN));

  memcpy(m_stDataParameter.m_stFirmwareParameter.IMEI,
         DEFAULT_IMEI,
         sizeof(m_stDataParameter.m_stFirmwareParameter.IMEI));

  memcpy(m_stDataParameter.m_stFirmwareParameter.CMEI,
         DEFAULT_CMEI,
         sizeof(m_stDataParameter.m_stFirmwareParameter.CMEI));

  memcpy(m_stDataParameter.m_stFirmwareParameter.ICCID,
         DEFAULT_ICCID,
         sizeof(m_stDataParameter.m_stFirmwareParameter.ICCID));
#endif

  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgFirmWare.VIN,
         DEFAULT_VIN,
         sizeof(DEFAULT_VIN));

  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgFirmWare.IMEI,
         DEFAULT_IMEI,
         sizeof(DEFAULT_IMEI));

  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgFirmWare.CMEI,
         DEFAULT_CMEI,
         sizeof(DEFAULT_CMEI));

  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgFirmWare.ICCID,
         DEFAULT_ICCID,
         sizeof(DEFAULT_CMEI));

  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Host, DEFAULT_HOST, sizeof(DEFAULT_HOST));

//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Ports, DEFAULT_PORTS, sizeof(DEFAULT_PORTS));
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Ports = DEFAULT_PORTS;
//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_uReportFreq, DEFAULT_UREPORTFREQ, sizeof(DEFAULT_UREPORTFREQ));
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_uReportFreq = DEFAULT_UREPORTFREQ;

  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_PskId, DEFAULT_PSKID, sizeof(DEFAULT_PSKID));
  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_CarType, DEFAULT_CARTYPE, sizeof(DEFAULT_CARTYPE));
  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_HexPSK, DEFAULT_HEXPSK, sizeof(DEFAULT_HEXPSK));
  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_StrPSK, DEFAULT_STRPSK, sizeof(DEFAULT_STRPSK));

//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Alarm, DEFAULT_ALARM, sizeof(DEFAULT_ALARM));
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Alarm = DEFAULT_ALARM;
//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_ReportFreq, DEFAULT_REPORTFREQ, sizeof(DEFAULT_REPORTFREQ));
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_ReportFreq = DEFAULT_REPORTFREQ;
//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_LogSwitch, DEFAULT_LOGSWITCH, sizeof(DEFAULT_LOGSWITCH));
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_LogSwitch = DEFAULT_LOGSWITCH;

  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Password, DEFAULT_PASSWORD, sizeof(DEFAULT_PASSWORD));
  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_ClientId, DEFAULT_CLIENT_ID, sizeof(DEFAULT_CLIENT_ID));
  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Username, DEFAULT_USERNAME, sizeof(DEFAULT_USERNAME));

//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_KeepAlive, DEFAULT_KEEPALIVE, sizeof(DEFAULT_KEEPALIVE));
//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Port, DEFAULT_PORT, sizeof(DEFAULT_PORT));
//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_MsgMaxSisz, DEFAULT_MSGMAXSISZ, sizeof(DEFAULT_MSGMAXSISZ));
//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_TopicMaxSize, DEFAULT_TOPICMAXSIZE, sizeof(DEFAULT_TOPICMAXSIZE));
//  memcpy(m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_TestOrNormal, DEFAULT_TESTORNORMAL, sizeof(DEFAULT_TESTORNORMAL));

  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_KeepAlive = DEFAULT_KEEPALIVE;
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_Port = DEFAULT_PORT;
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_MsgMaxSisz = DEFAULT_MSGMAXSISZ;
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_TopicMaxSize = DEFAULT_TOPICMAXSIZE;
  m_wFileFormat.m_stConfig.BYTES.m_CfgMQTT.m_TestOrNormal = DEFAULT_TESTORNORMAL;

  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerOld.OSID,
         DEFAULT_OLD_OSID,
         sizeof(DEFAULT_OLD_OSID));
  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerOld.McuVer,
         DEFAULT_OLD_MCU_VERSION,
         sizeof(DEFAULT_OLD_MCU_VERSION));
  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerOld.BleVer,
         DEFAULT_OLD_BLE_VERSION,
         sizeof(DEFAULT_OLD_BLE_VERSION));
  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerOld.OSVer,
         DEFAULT_OLD_OS_VERSION,
         sizeof(DEFAULT_OLD_OS_VERSION));

  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerNew.OSID,
         DEFAULT_NEW_OSID,
         sizeof(DEFAULT_NEW_OSID));
  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerNew.McuVer,
         DEFAULT_NEW_MCU_VERSION,
         sizeof(DEFAULT_NEW_MCU_VERSION));
  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerNew.BleVer,
         DEFAULT_NEW_BLE_VERSION,
         sizeof(DEFAULT_NEW_BLE_VERSION));
  memcpy(m_wFileFormat.m_stSystem.BYTES.m_SysUpgrade.m_VerNew.OSVer,
         DEFAULT_NEW_OS_VERSION,
         sizeof(DEFAULT_NEW_OS_VERSION));



//  memcpy(&m_wFileFormat.m_stConfig,
//         &m_stDataParameter,
//         sizeof(m_stDataParameter));

  memset(&Buff, 0, sizeof(Buff));
  memcpy(&Buff, &m_wFileFormat, sizeof(Buff));
  uint32_t crc32 = getCRC32(gl_crc, Buff, sizeof(Buff));

  m_wFileFormat.m_Crc32 = crc32;

  return 0;
}
int CNvm::nvm_SendProcess() {

  return 0;
}
int CNvm::nvm_BackupProcess() {
  char cmd[M_LEN] = {0};
  sprintf(cmd, "cp %s %s", BACKUP_FILE_PATH, SAVE_FILE_PATH);
  printf("cmd %s \n", cmd);

  if (!access(SAVE_FILE_PATH, F_OK)) {
    printf("OK \n");
  } else if (!access(BACKUP_FILE_PATH, F_OK)) {
    system(cmd);
  } else {
    NVMLOG("NO NVM FILE !!!!!! \n");
  }

  return 0;
}

