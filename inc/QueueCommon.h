#ifndef _QUEUE_COMMON_H_
#define _QUEUE_COMMON_H_

#include "common.h"

#define MSG_MAX_LEN             1024

#define MCU_PTC_HEAD_LEN    8
#define MCU_PTC_CRC_LEN     2

#define MCU_PTC_TAIL_LEN    3

#pragma pack(1)

typedef struct ParaInfoHead_Tag {
  uint8_t Gr;
  uint8_t Id;
} ParaInfoHead_ST;

typedef struct {
  long mtype = 1;
  ParaInfoHead_ST head;
  unsigned char Msgs[MSG_MAX_LEN];

} QueueInfo_ST;

#if  1
typedef struct OTA_SocSendInfo_Tag {
  unsigned int m_nCRC;
  uint32_t m_FileLen;

} OTA_SocSendInfo_ST;

#endif

#pragma pack()

/*
GROUP DEFINE BEGAIN
*/
/*=============================
GROUP vivian define 
=============================*/
#define GR_THD_SOURCE_OTA  0x01
#define GR_THD_SOURCE_MCU  0x02
#define GR_THD_SOURCE_TSP  0x03
#define GR_THD_SOURCE_PM  0x04
#define GR_THD_SOURCE_NVM  0x05
#define GR_THD_SOURCE_BLE  0x06
#define GR_THD_SOURCE_FW  0x07


/*
ID DEFINE BEGAIN
*/
/*=============================
ID_OTA vivian define 
=============================*/
#define ID_OTA_2_MCU_PTC_UPD_ACK      0x20
#define ID_OTA_2_MCU_PTC_UPD_BEGIN    0x21
#define ID_OTA_2_MCU_PTC_UPD_TRANSFER   0x22
#define ID_OTA_2_MCU_PTC_UPD_END      0x23
#define ID_OTA_2_MCU_EVT4        0x24
#define ID_OTA_2_MCU_EVT5        0x25
#define ID_OTA_2_MCU_EVT6        0x26
#define ID_OTA_2_MCU_EVT7        0x27
#define ID_OTA_2_MCU_EVT8        0x28
#define ID_OTA_2_MCU_EVT9        0x29
#define ID_OTA_2_MCU_EVTA        0x2A
#define ID_OTA_2_MCU_EVTB        0x2B
#define ID_OTA_2_MCU_EVTC        0x2C
#define ID_OTA_2_MCU_EVTD        0x2D
#define ID_OTA_2_MCU_EVTE        0x2E
#define ID_OTA_2_MCU_EVTF        0x2F

#define ID_OTA_2_TSP_THD_RESET_LTE                0x30
#define ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK            0x31
#define ID_OTA_2_TSP_PTC_TBOXSEND_VERSION_QUERY        0x32

#define ID_OTA_2_TSP_EVT3                  0x33
#define ID_OTA_2_TSP_EVT4                  0x34
#define ID_OTA_2_TSP_EVT5                  0x35
#define ID_OTA_2_TSP_EVT6                  0x36
#define ID_OTA_2_TSP_EVT7                  0x37
#define ID_OTA_2_TSP_EVT8                  0x38
#define ID_OTA_2_TSP_EVT9                  0x39
#define ID_OTA_2_TSP_EVTA                  0x3A
#define ID_OTA_2_TSP_EVTB                  0x3B
#define ID_OTA_2_TSP_EVTC                  0x3C
#define ID_OTA_2_TSP_EVTD                  0x3D
#define ID_OTA_2_TSP_EVTE                  0x3E
#define ID_OTA_2_TSP_EVTF                  0x3F

#define ID_OTA_2_PM_RESET   0x40
#define ID_OTA_2_PM_EVT1   0x41
#define ID_OTA_2_PM_EVT2   0x42
#define ID_OTA_2_PM_EVT3   0x43
#define ID_OTA_2_PM_EVT4   0x44
#define ID_OTA_2_PM_EVT5   0x45
#define ID_OTA_2_PM_EVT6   0x46
#define ID_OTA_2_PM_EVT7   0x47
#define ID_OTA_2_PM_EVT8   0x48
#define ID_OTA_2_PM_EVT9   0x49
#define ID_OTA_2_PM_EVTA   0x4A
#define ID_OTA_2_PM_EVTB   0x4B
#define ID_OTA_2_PM_EVTC   0x4C
#define ID_OTA_2_PM_EVTD   0x4D
#define ID_OTA_2_PM_EVTE   0x4E
#define ID_OTA_2_PM_EVTF   0x4F

#define ID_OTA_2_NVM_SAVE_OSID      0x50
#define ID_OTA_2_NVM_EVT1        0x51
#define ID_OTA_2_NVM_EVT2        0x52
#define ID_OTA_2_NVM_EVT3        0x53
#define ID_OTA_2_NVM_EVT4        0x54
#define ID_OTA_2_NVM_EVT5        0x55
#define ID_OTA_2_NVM_EVT6        0x56
#define ID_OTA_2_NVM_EVT7        0x57
#define ID_OTA_2_NVM_EVT8        0x58
#define ID_OTA_2_NVM_EVT9        0x59
#define ID_OTA_2_NVM_EVTA        0x5A
#define ID_OTA_2_NVM_EVTB        0x5B
#define ID_OTA_2_NVM_EVTC        0x5C
#define ID_OTA_2_NVM_EVTD        0x5D
#define ID_OTA_2_NVM_EVTE        0x5E
#define ID_OTA_2_NVM_EVTF        0x5F

#define ID_OTA_2_BLE_PTC_UPD_ACK      0x60
#define ID_OTA_2_BLE_PTC_UPD_BEGAIN    0x61
#define ID_OTA_2_BLE_PTC_UPD_TRANSFER   0x62
#define ID_OTA_2_BLE_PTC_UPD_END      0x63
#define ID_OTA_2_BLE_EVT4   0x64
#define ID_OTA_2_BLE_EVT5   0x65
#define ID_OTA_2_BLE_EVT6   0x66
#define ID_OTA_2_BLE_EVT7   0x67
#define ID_OTA_2_BLE_EVT8   0x68
#define ID_OTA_2_BLE_EVT9   0x69
#define ID_OTA_2_BLE_EVTA   0x6A
#define ID_OTA_2_BLE_EVTB   0x6B
#define ID_OTA_2_BLE_EVTC   0x6C
#define ID_OTA_2_BLE_EVTD   0x6D
#define ID_OTA_2_BLE_EVTE   0x6E
#define ID_OTA_2_BLE_EVTF   0x6F

#define ID_OTA_2_FW_SYNC      0x70
#define ID_OTA_2_FW_UPG_STS    0x71
#define ID_OTA_2_FW_EVT2   0x72
#define ID_OTA_2_FW_EVT3   0x73
#define ID_OTA_2_FW_EVT4   0x74
#define ID_OTA_2_FW_EVT5   0x75
#define ID_OTA_2_FW_EVT6   0x76
#define ID_OTA_2_FW_EVT7   0x77
#define ID_OTA_2_FW_EVT8   0x78
#define ID_OTA_2_FW_EVT9   0x79
#define ID_OTA_2_FW_EVTA   0x7A
#define ID_OTA_2_FW_EVTB   0x7B
#define ID_OTA_2_FW_EVTC   0x7C
#define ID_OTA_2_FW_EVTD   0x7D
#define ID_OTA_2_FW_EVTE   0x7E
#define ID_OTA_2_FW_EVTF   0x7F


/*ID MCU VIVIAN define*/
#define ID_MCU_2_OTA_EVT0          0x10
#define ID_MCU_2_OTA_PTC_UPG_DATA_REQ    0x11
#define ID_MCU_2_OTA_PTC_UPG_END      0x12
#define ID_MCU_2_OTA_EVT3  0x13
#define ID_MCU_2_OTA_EVT4  0x14
#define ID_MCU_2_OTA_EVT5  0x15
#define ID_MCU_2_OTA_EVT6  0x16
#define ID_MCU_2_OTA_EVT7  0x17
#define ID_MCU_2_OTA_EVT8  0x18
#define ID_MCU_2_OTA_EVT9  0x19
#define ID_MCU_2_OTA_EVTA  0x1A
#define ID_MCU_2_OTA_EVTB  0x1B
#define ID_MCU_2_OTA_EVTC  0x1C
#define ID_MCU_2_OTA_EVTD  0x1D
#define ID_MCU_2_OTA_EVTE  0x1E
#define ID_MCU_2_OTA_EVTF  0x1F

#define ID_MCU_2_TSP_PTC_SYNC              0x30
#define ID_MCU_2_TSP_PTC_COMMON_ACK           0x31
#define ID_MCU_2_TSP_HEART_ACK                0x32
#define ID_MCU_2_TSP_PTC_REMOTE_CTL_ACK       0x33
#define ID_MCU_2_TSP_PTC_VEICHLE_INFO_REPORT  0x34
#define ID_MCU_2_TSP_EVT5  0x35
#define ID_MCU_2_TSP_EVT6  0x36
#define ID_MCU_2_TSP_EVT7   0x37
#define ID_MCU_2_TSP_EVT8   0x38
#define ID_MCU_2_TSP_EVT9   0x39
#define ID_MCU_2_TSP_EVTA   0x3A
#define ID_MCU_2_TSP_EVTB   0x3B
#define ID_MCU_2_TSP_EVTC   0x3C
#define ID_MCU_2_TSP_EVTD   0x3D
#define ID_MCU_2_TSP_EVTE   0x3E
#define ID_MCU_2_TSP_EVTF   0x3F

#define ID_MCU_2_PM_EVT0    0x40
#define ID_MCU_2_PM_RESET   0x41
#define ID_MCU_2_PM_EVT2    0x42
#define ID_MCU_2_PM_EVT3    0x43
#define ID_MCU_2_PM_EVT4    0x44
#define ID_MCU_2_PM_EVT5    0x45
#define ID_MCU_2_PM_EVT6    0x46
#define ID_MCU_2_PM_EVT7    0x47
#define ID_MCU_2_PM_EVT8    0x48
#define ID_MCU_2_PM_EVT9    0x49
#define ID_MCU_2_PM_EVTA    0x4A
#define ID_MCU_2_PM_EVTB    0x4B
#define ID_MCU_2_PM_EVTC    0x4C
#define ID_MCU_2_PM_EVTD    0x4D
#define ID_MCU_2_PM_EVTE    0x4E
#define ID_MCU_2_PM_EVTF    0x4F

#define ID_MCU_2_NVM_EVT0   0x50
#define ID_MCU_2_NVM_EVT1   0x51
#define ID_MCU_2_NVM_EVT2   0x52
#define ID_MCU_2_NVM_EVT3   0x53
#define ID_MCU_2_NVM_EVT4   0x54
#define ID_MCU_2_NVM_EVT5   0x55
#define ID_MCU_2_NVM_EVT6   0x56
#define ID_MCU_2_NVM_EVT7   0x57
#define ID_MCU_2_NVM_EVT8   0x58
#define ID_MCU_2_NVM_EVT9   0x59
#define ID_MCU_2_NVM_EVTA   0x5A
#define ID_MCU_2_NVM_EVTB   0x5B
#define ID_MCU_2_NVM_EVTC   0x5C
#define ID_MCU_2_NVM_EVTD   0x5D
#define ID_MCU_2_NVM_EVTE   0x5E
#define ID_MCU_2_NVM_EVTF   0x5F

#define ID_MCU_2_BLE_EVT0   0x60
#define ID_MCU_2_BLE_EVT1   0x61
#define ID_MCU_2_BLE_EVT2   0x62
#define ID_MCU_2_BLE_EVT3   0x63
#define ID_MCU_2_BLE_EVT4   0x64
#define ID_MCU_2_BLE_EVT5   0x65
#define ID_MCU_2_BLE_EVT6   0x66
#define ID_MCU_2_BLE_EVT7   0x67
#define ID_MCU_2_BLE_EVT8   0x68
#define ID_MCU_2_BLE_EVT9   0x69
#define ID_MCU_2_BLE_EVTA   0x6A
#define ID_MCU_2_BLE_EVTB   0x6B
#define ID_MCU_2_BLE_EVTC   0x6C
#define ID_MCU_2_BLE_EVTD   0x6D
#define ID_MCU_2_BLE_EVTE   0x6E
#define ID_MCU_2_BLE_EVTF   0x6F

#define ID_MCU_2_FW_PTC_SYNC_REQ    0x70
#define ID_MCU_2_FW_PTC_COMMON_ACK  0x71
#define ID_MCU_2_FW_EVT2          0x72
#define ID_MCU_2_FW_EVT3        0x73
#define ID_MCU_2_FW_EVT4   0x74
#define ID_MCU_2_FW_EVT5   0x75
#define ID_MCU_2_FW_EVT6   0x76
#define ID_MCU_2_FW_EVT7   0x77
#define ID_MCU_2_FW_EVT8   0x78
#define ID_MCU_2_FW_EVT9   0x79
#define ID_MCU_2_FW_EVTA   0x7A
#define ID_MCU_2_FW_EVTB   0x7B
#define ID_MCU_2_FW_EVTC   0x7C
#define ID_MCU_2_FW_EVTD   0x7D
#define ID_MCU_2_FW_EVTE   0x7E
#define ID_MCU_2_FW_EVTF   0x7F


/*ID TSP vivian define*/
#define ID_TSP_2_OTA_EVT0          0x10
#define ID_TSP_2_OTA_VERSION_QUERY    0x11
#define ID_TSP_2_OTA_EVT2          0x12
#define ID_TSP_2_OTA_EVT3          0x13
#define ID_TSP_2_OTA_EVT4          0x14
#define ID_TSP_2_OTA_EVT5          0x15
#define ID_TSP_2_OTA_EVT6          0x16
#define ID_TSP_2_OTA_EVT7          0x17
#define ID_TSP_2_OTA_EVT8          0x18
#define ID_TSP_2_OTA_EVT9          0x19
#define ID_TSP_2_OTA_EVTA          0x1A
#define ID_TSP_2_OTA_EVTB          0x1B
#define ID_TSP_2_OTA_EVTC          0x1C
#define ID_TSP_2_OTA_EVTD          0x1D
#define ID_TSP_2_OTA_EVTE          0x1E
#define ID_TSP_2_OTA_EVTF          0x1F

#define ID_TSP_2_MCU_EVT0        0x20
#define ID_TSP_2_MCU_PTC_REMOTE_CTL    0x21
#define ID_TSP_2_MCU_PTC_SET_PARM      0x22
#define ID_TSP_2_MCU_EVT3        0x23
#define ID_TSP_2_MCU_EVT4        0x24
#define ID_TSP_2_MCU_EVT5        0x25
#define ID_TSP_2_MCU_EVT6        0x26
#define ID_TSP_2_MCU_EVT7            0x27
#define ID_TSP_2_MCU_EVT8        0x28
#define ID_TSP_2_MCU_EVT9        0x29
#define ID_TSP_2_MCU_EVTA        0x2A
#define ID_TSP_2_MCU_EVTB        0x2B
#define ID_TSP_2_MCU_EVTC        0x2C
#define ID_TSP_2_MCU_EVTD        0x2D
#define ID_TSP_2_MCU_EVTE        0x2E
#define ID_TSP_2_MCU_EVTF        0x2F

#define ID_TSP_2_PM_EVT0   0x40
#define ID_TSP_2_PM_EVT1   0x41
#define ID_TSP_2_PM_EVT2   0x42
#define ID_TSP_2_PM_EVT3   0x43
#define ID_TSP_2_PM_EVT4   0x44
#define ID_TSP_2_PM_EVT5   0x45
#define ID_TSP_2_PM_EVT6   0x46
#define ID_TSP_2_PM_EVT7   0x47
#define ID_TSP_2_PM_EVT8   0x48
#define ID_TSP_2_PM_EVT9   0x49
#define ID_TSP_2_PM_EVTA   0x4A
#define ID_TSP_2_PM_EVTB   0x4B
#define ID_TSP_2_PM_EVTC   0x4C
#define ID_TSP_2_PM_EVTD   0x4D
#define ID_TSP_2_PM_EVTE   0x4E
#define ID_TSP_2_PM_EVTF   0x4F

#define ID_TSP_2_NVM_EVT0   0x50
#define ID_TSP_2_NVM_EVT1   0x51
#define ID_TSP_2_NVM_EVT2   0x52
#define ID_TSP_2_NVM_EVT3   0x53
#define ID_TSP_2_NVM_EVT4   0x54
#define ID_TSP_2_NVM_EVT5   0x55
#define ID_TSP_2_NVM_EVT6   0x56
#define ID_TSP_2_NVM_EVT7   0x57
#define ID_TSP_2_NVM_EVT8   0x58
#define ID_TSP_2_NVM_EVT9   0x59
#define ID_TSP_2_NVM_EVTA   0x5A
#define ID_TSP_2_NVM_EVTB   0x5B
#define ID_TSP_2_NVM_EVTC   0x5C
#define ID_TSP_2_NVM_EVTD   0x5D
#define ID_TSP_2_NVM_EVTE   0x5E
#define ID_TSP_2_NVM_EVTF   0x5F

#define ID_TSP_2_BLE_EVT0      0x60
#define ID_TSP_2_BLE_TOKEN_ACK    0x61
#define ID_TSP_2_BLE_PTC_TOKEN    0x62
#define ID_TSP_2_BLE_EVT3        0x63
#define ID_TSP_2_BLE_EVT4      0x64
#define ID_TSP_2_BLE_EVT5      0x65
#define ID_TSP_2_BLE_EVT6      0x66
#define ID_TSP_2_BLE_EVT7      0x67
#define ID_TSP_2_BLE_EVT8      0x68
#define ID_TSP_2_BLE_EVT9      0x69
#define ID_TSP_2_BLE_EVTA      0x6A
#define ID_TSP_2_BLE_EVTB      0x6B
#define ID_TSP_2_BLE_EVTC      0x6C
#define ID_TSP_2_BLE_EVTD      0x6D
#define ID_TSP_2_BLE_EVTE      0x6E
#define ID_TSP_2_BLE_EVTF      0x6F

#define ID_TSP_2_FW_THD_SYNC          0x70
#define ID_TSP_2_FW_TIME_SYNC        0x71
#define ID_TSP_2_FW_STATUS_REPORT_REQ      0x72

#define ID_TSP_2_FW_EVT3        0x73
#define ID_TSP_2_FW_EVT4        0x74
#define ID_TSP_2_FW_EVT5        0x75
#define ID_TSP_2_FW_EVT6        0x76
#define ID_TSP_2_FW_EVT7        0x77
#define ID_TSP_2_FW_EVT8        0x78
#define ID_TSP_2_FW_EVT9        0x79
#define ID_TSP_2_FW_EVTA        0x7A
#define ID_TSP_2_FW_EVTB        0x7B
#define ID_TSP_2_FW_EVTC        0x7C
#define ID_TSP_2_FW_EVTD        0x7D
#define ID_TSP_2_FW_EVTE        0x7E
#define ID_TSP_2_FW_EVTF        0x7F



/*ID PM vivian define*/
#define ID_PM_2_OTA_EVT0  0x10
#define ID_PM_2_OTA_EVT1  0x11
#define ID_PM_2_OTA_EVT2  0x12
#define ID_PM_2_OTA_EVT3  0x13
#define ID_PM_2_OTA_EVT4  0x14
#define ID_PM_2_OTA_EVT5  0x15
#define ID_PM_2_OTA_EVT6  0x16
#define ID_PM_2_OTA_EVT7  0x17
#define ID_PM_2_OTA_EVT8  0x18
#define ID_PM_2_OTA_EVT9  0x19
#define ID_PM_2_OTA_EVTA  0x1A
#define ID_PM_2_OTA_EVTB  0x1B
#define ID_PM_2_OTA_EVTC  0x1C
#define ID_PM_2_OTA_EVTD  0x1D
#define ID_PM_2_OTA_EVTE  0x1E
#define ID_PM_2_OTA_EVTF  0x1F

#define ID_PM_2_MCU_EVT0   0x20
#define ID_PWR_2_MCU_PTC_GOTOSLEEP   0x21
#define ID_PM_2_MCU_EVT2   0x22
#define ID_PM_2_MCU_EVT3   0x23
#define ID_PM_2_MCU_EVT4   0x24
#define ID_PM_2_MCU_EVT5   0x25
#define ID_PM_2_MCU_EVT6   0x26
#define ID_PM_2_MCU_EVT7   0x27
#define ID_PM_2_MCU_EVT8   0x28
#define ID_PM_2_MCU_EVT9   0x29
#define ID_PM_2_MCU_EVTA   0x2A
#define ID_PM_2_MCU_EVTB   0x2B
#define ID_PM_2_MCU_EVTC   0x2C
#define ID_PM_2_MCU_EVTD   0x2D
#define ID_PM_2_MCU_EVTE   0x2E
#define ID_PM_2_MCU_EVTF   0x2F
#define ID_PM_2_TSP_EVT0   0x30
#define ID_PM_2_TSP_THD_DATA_UPDATE   0x31
#define ID_PM_2_TSP_EVT2   0x32
#define ID_PM_2_TSP_EVT3   0x33
#define ID_PM_2_TSP_EVT4   0x34
#define ID_PM_2_TSP_EVT5   0x35
#define ID_PM_2_TSP_EVT6   0x36
#define ID_PM_2_TSP_EVT7   0x37
#define ID_PM_2_TSP_EVT8   0x38
#define ID_PM_2_TSP_EVT9   0x39
#define ID_PM_2_TSP_EVTA   0x3A
#define ID_PM_2_TSP_EVTB   0x3B
#define ID_PM_2_TSP_EVTC   0x3C
#define ID_PM_2_TSP_EVTD   0x3D
#define ID_PM_2_TSP_EVTE   0x3E
#define ID_PM_2_TSP_EVTF   0x3F

#define ID_PM_2_NVM_EVT0   0x50
#define ID_PM_2_NVM_EVT1   0x51
#define ID_PM_2_NVM_EVT2   0x52
#define ID_PM_2_NVM_EVT3   0x53
#define ID_PM_2_NVM_EVT4   0x54
#define ID_PM_2_NVM_EVT5   0x55
#define ID_PM_2_NVM_EVT6   0x56
#define ID_PM_2_NVM_EVT7   0x57
#define ID_PM_2_NVM_EVT8   0x58
#define ID_PM_2_NVM_EVT9   0x59
#define ID_PM_2_NVM_EVTA   0x5A
#define ID_PM_2_NVM_EVTB   0x5B
#define ID_PM_2_NVM_EVTC   0x5C
#define ID_PM_2_NVM_EVTD   0x5D
#define ID_PM_2_NVM_EVTE   0x5E
#define ID_PM_2_NVM_EVTF   0x5F

#define ID_PM_2_BLE_EVT0   0x60
#define ID_PM_2_BLE_EVT1   0x61
#define ID_PM_2_BLE_EVT2   0x62
#define ID_PM_2_BLE_EVT3   0x63
#define ID_PM_2_BLE_EVT4   0x64
#define ID_PM_2_BLE_EVT5   0x65
#define ID_PM_2_BLE_EVT6   0x66
#define ID_PM_2_BLE_EVT7   0x67
#define ID_PM_2_BLE_EVT8   0x68
#define ID_PM_2_BLE_EVT9   0x69
#define ID_PM_2_BLE_EVTA   0x6A
#define ID_PM_2_BLE_EVTB   0x6B
#define ID_PM_2_BLE_EVTC   0x6C
#define ID_PM_2_BLE_EVTD   0x6D
#define ID_PM_2_BLE_EVTE   0x6E
#define ID_PM_2_BLE_EVTF   0x6F

#define ID_PM_2_FW_SYNC        0x70
#define ID_PM_2_FW_THD_WORKMODE    0x71
#define ID_PM_2_FW_EVT2        0x72
#define ID_PM_2_FW_EVT3        0x73
#define ID_PM_2_FW_EVT4        0x74
#define ID_PM_2_FW_EVT5        0x75
#define ID_PM_2_FW_EVT6        0x76
#define ID_PM_2_FW_EVT7        0x77
#define ID_PM_2_FW_EVT8        0x78
#define ID_PM_2_FW_EVT9        0x79
#define ID_PM_2_FW_EVTA        0x7A
#define ID_PM_2_FW_EVTB        0x7B
#define ID_PM_2_FW_EVTC        0x7C
#define ID_PM_2_FW_EVTD        0x7D
#define ID_PM_2_FW_EVTE        0x7E
#define ID_PM_2_FW_EVTF        0x7F



/*ID NVM vivian define*/
#define ID_NVM_2_OTA_EVT0  0x10
#define ID_NVM_2_OTA_EVT1  0x11
#define ID_NVM_2_OTA_EVT2  0x12
#define ID_NVM_2_OTA_EVT3  0x13
#define ID_NVM_2_OTA_EVT4  0x14
#define ID_NVM_2_OTA_EVT5  0x15
#define ID_NVM_2_OTA_EVT6  0x16
#define ID_NVM_2_OTA_EVT7  0x17
#define ID_NVM_2_OTA_EVT8  0x18
#define ID_NVM_2_OTA_EVT9  0x19
#define ID_NVM_2_OTA_EVTA  0x1A
#define ID_NVM_2_OTA_EVTB  0x1B
#define ID_NVM_2_OTA_EVTC  0x1C
#define ID_NVM_2_OTA_EVTD  0x1D
#define ID_NVM_2_OTA_EVTE  0x1E
#define ID_NVM_2_OTA_EVTF  0x1F

#define ID_NVM_2_MCU_EVT0   0x20
#define ID_NVM_2_MCU_EVT1   0x21
#define ID_NVM_2_MCU_EVT2   0x22
#define ID_NVM_2_MCU_EVT3   0x23
#define ID_NVM_2_MCU_EVT4   0x24
#define ID_NVM_2_MCU_EVT5   0x25
#define ID_NVM_2_MCU_EVT6   0x26
#define ID_NVM_2_MCU_EVT7   0x27
#define ID_NVM_2_MCU_EVT8   0x28
#define ID_NVM_2_MCU_EVT9   0x29
#define ID_NVM_2_MCU_EVTA   0x2A
#define ID_NVM_2_MCU_EVTB   0x2B
#define ID_NVM_2_MCU_EVTC   0x2C
#define ID_NVM_2_MCU_EVTD   0x2D
#define ID_NVM_2_MCU_EVTE   0x2E
#define ID_NVM_2_MCU_EVTF   0x2F
#define ID_NVM_2_TSP_EVT0   0x30
#define ID_NVM_2_TSP_EVT1   0x31
#define ID_NVM_2_TSP_EVT2   0x32
#define ID_NVM_2_TSP_EVT3   0x33
#define ID_NVM_2_TSP_EVT4   0x34
#define ID_NVM_2_TSP_EVT5   0x35
#define ID_NVM_2_TSP_EVT6   0x36
#define ID_NVM_2_TSP_EVT7   0x37
#define ID_NVM_2_TSP_EVT8   0x38
#define ID_NVM_2_TSP_EVT9   0x39
#define ID_NVM_2_TSP_EVTA   0x3A
#define ID_NVM_2_TSP_EVTB   0x3B
#define ID_NVM_2_TSP_EVTC   0x3C
#define ID_NVM_2_TSP_EVTD   0x3D
#define ID_NVM_2_TSP_EVTE   0x3E
#define ID_NVM_2_TSP_EVTF   0x3F

#define ID_NVM_2_PM_EVT0   0x40
#define ID_NVM_2_PM_EVT1   0x41
#define ID_NVM_2_PM_EVT2   0x42
#define ID_NVM_2_PM_EVT3   0x43
#define ID_NVM_2_PM_EVT4   0x44
#define ID_NVM_2_PM_EVT5   0x45
#define ID_NVM_2_PM_EVT6   0x46
#define ID_NVM_2_PM_EVT7   0x47
#define ID_NVM_2_PM_EVT8   0x48
#define ID_NVM_2_PM_EVT9   0x49
#define ID_NVM_2_PM_EVTA   0x4A
#define ID_NVM_2_PM_EVTB   0x4B
#define ID_NVM_2_PM_EVTC   0x4C
#define ID_NVM_2_PM_EVTD   0x4D
#define ID_NVM_2_PM_EVTE   0x4E
#define ID_NVM_2_PM_EVTF   0x4F

#define ID_NVM_2_BLE_EVT0   0x60
#define ID_NVM_2_BLE_EVT1   0x61
#define ID_NVM_2_BLE_EVT2   0x62
#define ID_NVM_2_BLE_EVT3   0x63
#define ID_NVM_2_BLE_EVT4   0x64
#define ID_NVM_2_BLE_EVT5   0x65
#define ID_NVM_2_BLE_EVT6   0x66
#define ID_NVM_2_BLE_EVT7   0x67
#define ID_NVM_2_BLE_EVT8   0x68
#define ID_NVM_2_BLE_EVT9   0x69
#define ID_NVM_2_BLE_EVTA   0x6A
#define ID_NVM_2_BLE_EVTB   0x6B
#define ID_NVM_2_BLE_EVTC   0x6C
#define ID_NVM_2_BLE_EVTD   0x6D
#define ID_NVM_2_BLE_EVTE   0x6E
#define ID_NVM_2_BLE_EVTF   0x6F

#define ID_NVM_2_FW_SYNC   0x70
#define ID_NVM_2_FW_SET_CONFIG   0x71
#define ID_NVM_2_FW_SET_SYSTEM   0x72
#define ID_NVM_2_FW_SET_FAULT   0x73
#define ID_NVM_2_FW_GET_CONFIG   0x74
#define ID_NVM_2_FW_GET_SYSTEM   0x75
#define ID_NVM_2_FW_GET_FAULT   0x76
#define ID_NVM_2_FW_EVT7   0x77
#define ID_NVM_2_FW_EVT8   0x78
#define ID_NVM_2_FW_EVT9   0x79
#define ID_NVM_2_FW_EVTA   0x7A
#define ID_NVM_2_FW_EVTB   0x7B
#define ID_NVM_2_FW_EVTC   0x7C
#define ID_NVM_2_FW_EVTD   0x7D
#define ID_NVM_2_FW_EVTE   0x7E
#define ID_NVM_2_FW_EVTF   0x7F


/*ID BLE vivian define*/
#define ID_BLE_2_OTA_EVT0  0x10
#define ID_BLE_2_OTA_EVT1  0x11
#define ID_BLE_2_OTA_EVT2  0x12
#define ID_BLE_2_OTA_EVT3  0x13
#define ID_BLE_2_OTA_EVT4  0x14
#define ID_BLE_2_OTA_EVT5  0x15
#define ID_BLE_2_OTA_EVT6  0x16
#define ID_BLE_2_OTA_EVT7  0x17
#define ID_BLE_2_OTA_EVT8  0x18
#define ID_BLE_2_OTA_EVT9  0x19
#define ID_BLE_2_OTA_EVTA  0x1A
#define ID_BLE_2_OTA_EVTB  0x1B
#define ID_BLE_2_OTA_EVTC  0x1C
#define ID_BLE_2_OTA_EVTD  0x1D
#define ID_BLE_2_OTA_EVTE  0x1E
#define ID_BLE_2_OTA_EVTF  0x1F

#define ID_BLE_2_MCU_EVT0   0x20
#define ID_BLE_2_MCU_PTC_CTLCMD   0x21
#define ID_BLE_2_MCU_EVT2   0x22
#define ID_BLE_2_MCU_EVT3   0x23
#define ID_BLE_2_MCU_EVT4   0x24
#define ID_BLE_2_MCU_EVT5   0x25
#define ID_BLE_2_MCU_EVT6   0x26
#define ID_BLE_2_MCU_EVT7   0x27
#define ID_BLE_2_MCU_EVT8   0x28
#define ID_BLE_2_MCU_EVT9   0x29
#define ID_BLE_2_MCU_EVTA   0x2A
#define ID_BLE_2_MCU_EVTB   0x2B
#define ID_BLE_2_MCU_EVTC   0x2C
#define ID_BLE_2_MCU_EVTD   0x2D
#define ID_BLE_2_MCU_EVTE   0x2E
#define ID_BLE_2_MCU_EVTF   0x2F

#define ID_BLE_2_TSP_EVT0        0x30
#define ID_BLE_2_TSP_REQUEST_TOKEN    0x31
#define ID_BLE_2_TSP_EVT2        0x32
#define ID_BLE_2_TSP_EVT3        0x33
#define ID_BLE_2_TSP_EVT4        0x34
#define ID_BLE_2_TSP_EVT5        0x35
#define ID_BLE_2_TSP_EVT6        0x36
#define ID_BLE_2_TSP_EVT7        0x37
#define ID_BLE_2_TSP_EVT8        0x38
#define ID_BLE_2_TSP_EVT9        0x39
#define ID_BLE_2_TSP_EVTA        0x3A
#define ID_BLE_2_TSP_EVTB        0x3B
#define ID_BLE_2_TSP_EVTC        0x3C
#define ID_BLE_2_TSP_EVTD        0x3D
#define ID_BLE_2_TSP_EVTE        0x3E
#define ID_BLE_2_TSP_EVTF        0x3F

#define ID_BLE_2_PM_EVT0   0x40
#define ID_BLE_2_PM_EVT1   0x41
#define ID_BLE_2_PM_EVT2   0x42
#define ID_BLE_2_PM_EVT3   0x43
#define ID_BLE_2_PM_EVT4   0x44
#define ID_BLE_2_PM_EVT5   0x45
#define ID_BLE_2_PM_EVT6   0x46
#define ID_BLE_2_PM_EVT7   0x47
#define ID_BLE_2_PM_EVT8   0x48
#define ID_BLE_2_PM_EVT9   0x49
#define ID_BLE_2_PM_EVTA   0x4A
#define ID_BLE_2_PM_EVTB   0x4B
#define ID_BLE_2_PM_EVTC   0x4C
#define ID_BLE_2_PM_EVTD   0x4D
#define ID_BLE_2_PM_EVTE   0x4E
#define ID_BLE_2_PM_EVTF   0x4F

#define ID_BLE_2_NVM_EVT0   0x50
#define ID_BLE_2_NVM_EVT1   0x51
#define ID_BLE_2_NVM_EVT2   0x52
#define ID_BLE_2_NVM_EVT3   0x53
#define ID_BLE_2_NVM_EVT4   0x54
#define ID_BLE_2_NVM_EVT5   0x55
#define ID_BLE_2_NVM_EVT6   0x56
#define ID_BLE_2_NVM_EVT7   0x57
#define ID_BLE_2_NVM_EVT8   0x58
#define ID_BLE_2_NVM_EVT9   0x59
#define ID_BLE_2_NVM_EVTA   0x5A
#define ID_BLE_2_NVM_EVTB   0x5B
#define ID_BLE_2_NVM_EVTC   0x5C
#define ID_BLE_2_NVM_EVTD   0x5D
#define ID_BLE_2_NVM_EVTE   0x5E
#define ID_BLE_2_NVM_EVTF   0x5F

#define ID_BLE_2_FW_SYNC   0x70
#define ID_BLE_2_FW_PTC    0x71
#define ID_BLE_2_FW_EVT2   0x72
#define ID_BLE_2_FW_EVT3   0x73
#define ID_BLE_2_FW_EVT4   0x74
#define ID_BLE_2_FW_EVT5   0x75
#define ID_BLE_2_FW_EVT6   0x76
#define ID_BLE_2_FW_EVT7   0x77
#define ID_BLE_2_FW_EVT8   0x78
#define ID_BLE_2_FW_EVT9   0x79
#define ID_BLE_2_FW_EVTA   0x7A
#define ID_BLE_2_FW_EVTB   0x7B
#define ID_BLE_2_FW_EVTC   0x7C
#define ID_BLE_2_FW_EVTD   0x7D
#define ID_BLE_2_FW_EVTE   0x7E
#define ID_BLE_2_FW_EVTF   0x7F


/*ID FW vivian define*/

#define ID_FW_2_OTA_THD_EVT        0x10
#define ID_FW_2_OTA_UPG_MCU_REQ    0x11
#define ID_FW_2_OTA_UPG_MCU_RESULT  0x12

#define ID_FW_2_OTA_UPG_BLE_REQ    0x13
#define ID_FW_2_OTA_UPG_BLE_RESULT  0x14

#define ID_FW_2_OTA_NVM_LOCAL_OSID  0x15
#define ID_FW_2_OTA_PTC_TSP_REQISP  0x16
#define ID_FW_2_OTA_TSP_OSID_URL    0x17
#define ID_FW_2_OTA_EVT8  0x18
#define ID_FW_2_OTA_EVT9  0x19
#define ID_FW_2_OTA_EVTA  0x1A
#define ID_FW_2_OTA_EVTB  0x1B
#define ID_FW_2_OTA_EVTC  0x1C
#define ID_FW_2_OTA_EVTD  0x1D
#define ID_FW_2_OTA_EVTE  0x1E
#define ID_FW_2_OTA_EVTF  0x1F

#define ID_FW_2_MCU_PTC_SYNC_ACK        0x20
#define ID_FW_2_MCU_PTC_VEICHLE_ACK    0x21
#define ID_FW_2_MCU_PTC_REMOTE_CTL      0x22
#define ID_FW_2_MCU_PTC_UPG_BEGAIN    0x23
#define ID_FW_2_MCU_PTC_UPG_DATA      0x24
#define ID_FW_2_MCU_EVT5   0x25
#define ID_FW_2_MCU_EVT6   0x26
#define ID_FW_2_MCU_EVT7   0x27
#define ID_FW_2_MCU_EVT8   0x28
#define ID_FW_2_MCU_EVT9   0x29
#define ID_FW_2_MCU_EVTA   0x2A
#define ID_FW_2_MCU_EVTB   0x2B
#define ID_FW_2_MCU_EVTC   0x2C
#define ID_FW_2_MCU_EVTD   0x2D
#define ID_FW_2_MCU_EVTE   0x2E
#define ID_FW_2_MCU_EVTF   0x2F

#define ID_FW_2_TSP_PTC_SYNC_TIME_REQ          0x30
#define ID_FW_2_TSP_PTC_REMOTE_UPGRADE          0x31
#define ID_FW_2_TSP_PTC_VEICHLE_INFO_REPORT    0x32
#define ID_FW_2_TSP_PTC_REMOTE_CTL_ACK        0x33
#define ID_FW_2_TSP_PTC_TOKEN_REQ            0x34
#define ID_FW_2_TSP_PTC_VEICHILE_INFO_EVENT        0x35
#define ID_FW_2_TSP_PTC_TOKEN_ACK                   0x36
#define ID_FW_2_TSP_EVT7   0x37
#define ID_FW_2_TSP_EVT8   0x38
#define ID_FW_2_TSP_EVT9   0x39
#define ID_FW_2_TSP_EVTA   0x3A
#define ID_FW_2_TSP_EVTB   0x3B
#define ID_FW_2_TSP_EVTC   0x3C
#define ID_FW_2_TSP_EVTD   0x3D
#define ID_FW_2_TSP_EVTE   0x3E
#define ID_FW_2_TSP_EVTF   0x3F

#define ID_FW_2_PWR_EVT0   0x40
#define ID_FW_2_PWR_LTE_REBOOT   0x41
#define ID_FW_2_PWR_EVT2   0x42
#define ID_FW_2_PWR_EVT3   0x43
#define ID_FW_2_PWR_EVT4   0x44
#define ID_FW_2_PWR_EVT5   0x45
#define ID_FW_2_PWR_EVT6   0x46
#define ID_FW_2_PWR_EVT7   0x47
#define ID_FW_2_PWR_EVT8   0x48
#define ID_FW_2_PWR_EVT9   0x49
#define ID_FW_2_PWR_EVTA   0x4A
#define ID_FW_2_PWR_EVTB   0x4B
#define ID_FW_2_PWR_EVTC   0x4C
#define ID_FW_2_PWR_EVTD   0x4D
#define ID_FW_2_PWR_EVTE   0x4E
#define ID_FW_2_PWR_EVTF   0x4F

#define ID_FW_2_NVM_EVT0           0x50
#define ID_FW_2_NVM_SYNC           0x51
#define ID_FW_2_NVM_SET_CONFIG         0x52
#define ID_FW_2_NVM_SET_SYSTEM           0x53
#define ID_FW_2_NVM_SET_FAULT           0x54
#define ID_FW_2_NVM_GET_CONFIG           0x55
#define ID_FW_2_NVM_GET_SYSTEM           0x56
#define ID_FW_2_NVM_GET_FAULT           0x57
#define ID_FW_2_NVM_EVT8           0x58
#define ID_FW_2_NVM_EVT9           0x59
#define ID_FW_2_NVM_EVTA           0x5A
#define ID_FW_2_NVM_EVTB           0x5B
#define ID_FW_2_NVM_EVTC           0x5C
#define ID_FW_2_NVM_EVTD           0x5D
#define ID_FW_2_NVM_EVTE           0x5E
#define ID_FW_2_NVM_EVTF           0x5F

#define ID_FW_2_BLE_PTC_UPG_ACK                 0x60
#define ID_FW_2_BLE_PTC_UPG_BEGAIN              0x61
#define ID_FW_2_BLE_PTC_UPG_TRANSFER            0x62
#define ID_FW_2_BLE_PTC_UPG_END                 0x63
#define ID_FW_2_BLE_PTC_TOKEN                   0x64
#define ID_FW_2_BLE_PTC_CTLACK                  0x65
#define ID_FW_2_BLE_PTC_GENERALACK              0x66
#define ID_FW_2_BLE_PTC_STATEACK                0x67
#define ID_FW_2_BLE_PTC_CTLRESULT               0x68
#define ID_FW_2_BLE_PTC_VEICHLE_INFO_REPORT     0x69
#define ID_FW_2_BLE_EVTA   0x6A
#define ID_FW_2_BLE_EVTB   0x6B
#define ID_FW_2_BLE_EVTC   0x6C
#define ID_FW_2_BLE_EVTD   0x6D
#define ID_FW_2_BLE_EVTE   0x6E
#define ID_FW_2_BLE_EVTF   0x6F

#define ID_LOG_2_FW_SYNC   0x70

#endif
