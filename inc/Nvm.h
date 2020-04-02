#ifndef __NVM_H__
#define  __NVM_H__

#include "BaseThread.h"
#include "framework.h"
#include "QueueCommon.h"
#include <string>

#define NVM_DEBUG  1

#if NVM_DEBUG
#define NVMLOG(format, ...) printf("== NVM == FILE: %s, FUN: %s, LINE: %d " format "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define NVM_NO(format, ...) printf(format,##__VA_ARGS__)
#else
#define NVMLOG(format,...)
#define NVM_NO(format,...)
#endif

#define DEFAULT_VIN                     "1111222233334444"
#define DEFAULT_IMEI                    "2222"
#define DEFAULT_CMEI                    "3333"
#define DEFAULT_ICCID                   "4444"

#define DEFAULT_OLD_OSID                "0"
#define DEFAULT_OLD_MCU_VERSION         "0"
#define DEFAULT_OLD_BLE_VERSION         "0"
#define DEFAULT_OLD_OS_VERSION          "0"
#define DEFAULT_NEW_OSID                "0"
#define DEFAULT_NEW_MCU_VERSION         "0"
#define DEFAULT_NEW_BLE_VERSION         "0"
#define DEFAULT_NEW_OS_VERSION          "0"

#define DEFAULT_HOST                  "192.168.1.18"
#define DEFAULT_PORTS                 3333
#define DEFAULT_UREPORTFREQ           99
#define DEFAULT_PSKID                 "1234567890"
#define DEFAULT_CARTYPE               "RX-78-01"
#define DEFAULT_HEXPSK                "123456"
#define DEFAULT_STRPSK                "654321"
#define DEFAULT_ALARM                 24
#define DEFAULT_REPORTFREQ            88
#define DEFAULT_LOGSWITCH             true
#define DEFAULT_PASSWORD              "12345678"
#define DEFAULT_CLIENT_ID             "ABCDEFGH"
#define DEFAULT_USERNAME              "xyz"
#define DEFAULT_PORT                  1080
#define DEFAULT_KEEPALIVE             1
#define DEFAULT_MSGMAXSISZ            512
#define DEFAULT_TOPICMAXSIZE          1024
#define DEFAULT_TESTORNORMAL          1

#define C_TRUE 0
#define C_FALSE -1

#define SSSS_LEN   4
#define SSS_LEN    8
#define SS_LEN     16
#define S_LEN      32
#define M_LEN      64
#define L_LEN      128
#define XL_LEN     256

#define BUFSIZE 1024
#define INFOSIZE 768
#define PARTSIZE 256

#define CFG_PART_SIZE 1024
#define SYS_PART_SIZE 256
#define FAU_PART_SIZE 256
#define CRC_PART_SIZE 4

#define SAVE_FILE_PATH  "/data/nvm"
#define BACKUP_FILE_PATH  "/cache/nvm"

#define NVM_THREAD_PERIOD    (200*1000)//MS
#define CNT_SYNC_5S        ((5*1000*1000)/NVM_THREAD_PERIOD)

#pragma pack(1)

typedef enum NVM_WorkMode_Tag {
  NVMWORK_INIT,
  NVMWORK_NORMAL,
  NVMWORK_BUSY,
  NVMWORK_NOTREADY,
  NVMWORK_ALL
} NVM_WorkMode_ENUM;

typedef struct NVM_Firmware_Parameter_Tag {
  uint8_t VIN[32];
  uint8_t IMEI[16];
  uint8_t CMEI[16];
  uint8_t ICCID[32];
} NVM_Firmware_Parameter_ST;

typedef struct NVM_Upgrade_Parameter_Tag {
  uint8_t OSID[16];
  uint8_t McuVer[16];
  uint8_t BleVer[16];
  uint8_t OSVer[16];
} NVM_Upgrade_Parameter_ST;

typedef struct NVM_MQTT_Config_Tag {
  uint8_t m_ServerHost[64];    //MQTT Server Host
  uint16_t m_ServerPorts;
  uint16_t m_NormalReportFreq;
  uint8_t m_PskId[64];
  uint8_t m_CarType[16];
  uint8_t m_HexPSK[64];
  uint8_t m_StrPSK[64];
  uint16_t m_Alarm;
  uint16_t m_SleepReportFreq;
  bool m_LogSwitch;
  uint8_t m_Password[64];
  uint8_t m_ClientId[64];
  uint8_t m_Username[64];
  uint16_t m_Port;
  uint16_t m_KeepAlive;
  uint16_t m_MsgMaxSize;
  uint16_t m_TopicMaxSize;
  int m_TestOrNormal;
} NVM_MQTT_Config_ST;

typedef struct NVM_System_Upgrade_Parameter_Tag {

  NVM_Upgrade_Parameter_ST m_VerOld;
  NVM_Upgrade_Parameter_ST m_VerNew;
} NVM_System_Upgrade_Parameter_ST;

//typedef struct NVM_DataParameter_Tag {
//  NVM_FirmwareParameter_ST m_stFirmwareParameter;
//  NVM_UpgradeParameter_ST m_stUpgradeParameter;
//  NVM_MQTT_Config_ST m_MQTTConfig;
//} NVM_DataParameter_ST;

typedef union NVM_Config_Tag {
  uint8_t Buff[1024];
  struct {
//    NVM_Firmware_Parameter_ST m_CfgFirmWare;
    NVM_MQTT_Config_ST m_CfgMQTT;
  } BYTES;
} NVM_Config_UN;
typedef union NVM_System_Tag {
  uint8_t Buff[256];
  struct {
    NVM_System_Upgrade_Parameter_ST m_SysUpgrade;
  } BYTES;
} NVM_System_UN;
typedef union NVM_Fault_Tag {
  uint8_t Buff[256];
  struct {

  } BYTES;
} NVM_Fault_UN;

typedef struct NVM_FileFormat_Tag {
  NVM_Config_UN m_stConfig;
  NVM_System_UN m_stSystem;
  NVM_Fault_UN m_stFault;
  uint32_t m_Crc32;
} NVM_FileFormat_ST;

#pragma pack()

class CNvm : public TiotThread {
 public:
  static CNvm *GetInstance();
  static void FreeInstance();
  CNvm();
  virtual ~CNvm();
  BOOL Init();
  void Deinit();
  void Run();
  Framework *mFramework;

 protected:
  BOOL Processing();
  BOOL TimeoutProcessing();
  static CNvm *m_instance;
  BOOL m_binit;
  void ReceiveQueueTestTask();
  void SendQueueTestTask();

 public:
  //init crc list
  static void initCrcTable();
  //gen crc32
  static uint32_t getCRC32(uint32_t crc, uint8_t *buffer, uint32_t size);
  static int calcImgCRC(uint8_t *buffer, uint32_t *incrc);

 private:
  bool nvm_SyncProcess();

  int nvm_SendQueue(uint8_t Gr, uint8_t Id);
  int nvm_RecvQueue();

  int nvm_WriteFile();
  int nvm_ReadFile();

  int GetInitSts();

  int nvm_Init();
  int nvm_SendProcess();
  int nvm_RecvProcess();
  int nvm_WriteProcess();
  int nvm_DefaultRestore();
  int nvm_BackupProcess();

 private:
  int QuId_NVM_2_FW;
  int QuId_FW_2_NVM;
//  NVM_DataParameter_ST m_stDataParameter;

  NVM_WorkMode_ENUM m_u8WorkMode;
  QueueInfo_ST m_stRevMsg;
  QueueInfo_ST m_stSndMsg;

  NVM_FileFormat_ST m_wFileFormat;
//  NVM_FileFormat_ST m_rFileFormat;

  bool m_bDataChangeFlag;
};

#endif
