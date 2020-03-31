#ifndef _FAWACP_H_
#define _FAWACP_H_

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/syslog.h>
#include "FAWACP_Data.h"
#include "VehicleData.h"
#include "HMAC_MD5.h"
#include "HmacSHA-1.h"
#include "mcu.h"
#include "SQLiteDatabase.h"
#include "TBoxDataPool.h"
#include "GpioWake.h"
#include "common.h"
#include "FTPClient.h"
#include "RingBuffer.h"


#include "DnsResolv.h"
#include "DataCall.h"
#include "bluetooth.h"
#include "/usr/include/linux/tcp.h"
#include "/usr/include/linux/socket.h"

#include <mosquitto.h>
#include "ssl.h"

#define 	MQTT  				1

#define 	MAXTOPIC  			13
#define 	TOPICLEN  			25

#define 	UpgradeStep_1 		1
#define 	UpgradeStep_2 		2
#define 	UpgradeStep_3 		3
#define	UpgradeStep_4 		4
#define	UpgradeStep_5 		5
#define 	UpgradeStep_6 		6
#define 	UpgradeStep_7 		7
#define 	UpgradeStep_8 		8

#define 	UPGRADE_MCU		1
#define 	UPGRADE_BLE		2
#define 	UPGRADE_4G		4

#define 	MQTT_VERSION_TEST   			0
#define 	MQTT_VERSION_NORMAL           	1

typedef enum MQTT_UPGRADE_TAG{
  UPGRADE_SUCCESS = 0, 
  UPGRADE_URL_ERROR,  
  UPGRADE_DOWNLOD_FAIL,
  UPGRADE_MD5_ERROR,
  UPGRADE_FILE_EXIST,
  UPGRADE_OSID_SAME,
  UPGRADE_INIT
}MQTT_UPGRADE_TAG;


typedef struct{
	uint8_t m_HOST[50];		//MQTT Server Host
  	uint16_t m_PORTS;
	uint16_t m_uReportFrequency;
	uint8_t m_PSK_ID[50];		
	uint8_t m_Car_Type[10];			
	uint8_t m_hexPSK[50];
	uint8_t m_strPSK[50];		
	uint16_t m_Alarm;
	uint16_t m_reportFrequency;
	bool m_LogSwitch;
	uint8_t m_PASSWARD[50];
	uint8_t m_CILENT_Id[50];		
       uint8_t m_USERNAME[50];		
       uint16_t m_PORT;		
       uint16_t m_KEEP_ALIVE;
       uint16_t m_MSG_MAX_SIZE;
       uint16_t m_TOPIC_MAX_SIZE;
	int  m_TestOrNormal;
}MQTT_Config;

#if 0
typedef struct{
	uint8_t VehicleUploadTP[20];		
	uint8_t RemoteCtrlTP[20];		
    	uint8_t RemoteCtrlReplyID[20];		
    	uint8_t RemoteUpgradeTP[24];		
    	uint8_t RemoteUpgradeReplyID[24];	
}MQTT_Topic;
#endif

typedef enum MQTT_TOPIC_TAG{
  VehicleUploadTP = 0, ////0
  RemoteCtrlTP,  ////1
  RemoteCtrlReplyID,////2
  RemoteUpgradeTP,////3
  RemoteUpgradeReplyID,////4
  VehicleUploadReplyTP,////5
  nouse,////6
  CamreCtrl,////7
  CameraCtrlReply,////8
  SyncTimeRequest,////9
  SyncTime,////10
  RespondToken,////11
  ReqBleToken,////12

}MQTT_TOPIC_TAG;


#define MQTT_HEADER  		0x7e

#define MQTT_HEADER_LEN  	1

#define MQTT_MSG_LEN  		2


#ifndef FAWACP_DEBUG
	#define FAWACP_DEBUG 		1
#endif


#ifndef TEST_ENV
	#define TEST_ENV			0
#endif

#ifndef MCU_SLEEP_VERSION
	#define MCU_SLEEP_VERSION	0
#endif

#ifndef MQTT_RECONNECT
	#define MQTT_RECONNECT		1
#endif

#ifndef OTA_UPGRADE
	#define OTA_UPGRADE		1
#endif

#ifndef MQTT_DEBUG_LOG
	#define MQTT_DEBUG_LOG		1
#endif

#define MQTT_LOG_FATAL			0
#define MQTT_LOG_ERROR			1
#define MQTT_LOG_WARN			2
#define MQTT_LOG_INFO			3
#define MQTT_LOG_TRACE			4
#define MQTT_LOG_DEBUG			5


#if MQTT_DEBUG_LOG
void write_mqtt_log(char *code_file, char *function_name, int code_line, int log_level, char *content);
#define WRITELOGFILE(level, msg)	write_mqtt_log(__FILE__, __func__, __LINE__, level, msg)
#else
#define WRITELOGFILE(level, msg)		{}
#endif

#if FAWACP_DEBUG
	#define FAWACPLOG(format,...) printf("### FAWACP ### %s [%d] "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define FAWACP_NO(format,...) printf(format,##__VA_ARGS__)
#else

	#define FAWACPLOG(format,...)
	#define FAWACP_NO(format,...)
#endif

/* mqtt  */
/*
 *		   3/FIRM_NUM1/FIRM_NUM2/sn
 *		   4/FIRM_NUM1/FIRM_NUM2/sn
 */
/* ***************************** */
#define FIRM_NUM1		1
#define FIRM_NUM2		1

#define VERSION_FLAG	1		//0,;1,

#define TBOX_VERSION    "MPU_V1.1.02"//TBOX12
#define IVI_VERSION		"IVI_1.0.02"//IVI16
#define REISSUE_TIME	2	//
#define SLEEPTIMEOUT	5
#define TCPACK_SLEEP_TIMEOUT_TIMES		3
#define TCPACK_WAKEUP_TIMEOUT_TIMES		5
#define BLE_UPGRADE_TIMES				5
#define LOGIN_MAX_TIMES					3


#ifndef MQTT_UPDATE_FILE
#define MQTT_UPDATE_FILE         (char*)"/data/mqttupdate.tar.gz"
#define RM_MQTT_UPDATE_FILE   (char*)"rm -f  /data/mqttupdate.tar.gz"
#endif

#ifndef OSVERSION_TXT
#define OSVERSION_TXT				(char*)"/data/osversion.txt"
#define RM_OSVERSION_TXT			(char*)"rm -f /data/osversion.txt"
#endif

#ifndef PEM_FILE_NAME
#define PEM_FILE_NAME         			(char*)"/data/111.pem"
#endif

#ifndef RM_PEM_FILE
#define RM_PEM_FILE         			(char*)"rm -rf /data/111.pem"
#endif

#ifndef  BACKUP_PEM_FILE_NAME
#define   BACKUP_PEM_FILE_NAME         (char*)"/cache/111.pem"
#endif

#ifndef MQTTPARA_FILE
#define MQTTPARA_FILE         			 (char*)"/data/MQTTPara"
#endif

#ifndef RM_MQTTPARA_FILE
#define RM_MQTTPARA_FILE         		(char*)"rm -rf /data/MQTTPara"
#endif

#ifndef BACKUP_MQTTPARA_FILE
#define BACKUP_MQTTPARA_FILE         	(char*)"/cache/MQTTPara"
#endif


#ifndef LTE_FILE_NAME
#define LTE_FILE_NAME         "/data/ostbox16"
#endif

#ifndef RM_LTE_FILE
#define RM_LTE_FILE           (char*)"rm -rf /data/ostbox16"
#endif

#ifndef MCU_FILE_NAME
#define MCU_FILE_NAME         (char*)"/data/MCU.bin"
#endif

#ifndef RM_MCU_FILE
#define RM_MCU_FILE           (char*)"rm -rf /data/MCU.bin"
#endif

#ifndef BLE_FILE_NAME
#define BLE_FILE_NAME         (char*)"/data/BLEAPP"
#endif

#ifndef RM_BLE_FILE
#define RM_BLE_FILE           (char*)"rm -rf /data/BLEAPP"
#endif

#define RM_QMUX_SOCKET		  "rm /data/qmux_client_socket*"

#define MCU_WAKEUP_TIMES							(10)
#define HTTP_DOWNLOAD_TIMEOUT 					(30)
#define OTA_CHECK_TIME                                                  (86400)

#define TSP_MQTT_SYNC_INTERVAL_TIME                        (120) /*2 min*/
#define TSP_MQTT_SYNC_REBOOT_COUNT                        (3) /*2 min*/


#define  TSP_WORK_MODE_REQ_MODEM_REDATACALL		0
#define  TSP_WORK_MODE_REQ_MODEM_REBOOT			1
#define  TSP_WORK_MODE_WAIT_MODEM_FINISH  			2
#define  TSP_WORK_MODE_WAIT_MCU_FINISH				3
#define  TSP_WORK_MODE_SOCKET_DISCONNECT			4
#define  TSP_WORK_MODE_SOCKET_CONNECT				5
#define  TSP_WORK_MODE_LOGIN_TSP						6
#define  TSP_WORK_MODE_WAIT_LOGIN_TSP				7
#define  TSP_WORK_MODE_PERIOD_REPORT					8

#define  TSP_WORK_MODE_FIRST_LOGIN   					9
#define  TSP_WORK_MODE_MQTT_INIT						10
#define  TSP_WORK_MODE_MQTT_REQ_SYNC_TIME			11


typedef enum TSP_LOGIN_STATE_TAG{
  STATUS_LOGGIN_INIT = 0x00, 
  STATUS_LOGGIN_ING,  
  STATUS_LOGGIN_FINISH,
  STATUS_LOGGIN_ALL
}TSP_LOGIN_STATE_ENNUM;


typedef int (*callback_RemoteCtrlCmd)();
typedef int (*callback_RemoteConfigCmd)(uint8_t, uint16_t);
/* FAWACP data information */
typedef struct{
	uint8_t Vin[17];		//VIN
	uint8_t IMEI[15];		//SIMIMEI
	uint8_t ICCID[20];	        //SIMCCID(200x00)
	uint8_t IVISerialNumber[30]; //IVI0,ASCII0x30,0x00)
	uint8_t TBoxSerialNumber[30];//Tbox0x00)
	uint8_t	AuthToken[4];	//
	uint8_t	New_RootKey[16];//ootKey
	uint8_t	SKey[16];	//
	uint8_t	RootKey[16];//

	VehicleCondData_t	VehicleCondData; //
	RemoteControlData_t	RemoteControlData;//
	RemoteDeviceConfig_t	RemoteDeviceConfigInfo[MAX_DEVICE_NO];//
	AcpCodeFault_t			AcpCodeFault;	//

	uint8_t	voltageFaultSRSState;//SRS
}FAWACPInfo_Handle;


typedef struct LinkTimer{
	uint8_t AcpAppID;
	uint8_t MsgType;
	uint8_t TspSoure;
	time_t  time;
	struct LinkTimer *next;
}LinkTimer_t;


//TSP source
typedef struct{
	uint8_t RemoteCtrlFlag;
	uint8_t RemoteCtrlTspSource;
	uint8_t	VehQueCondFlag;
	uint8_t VehQueCondSource;
	uint8_t RemoteCpnfigFlag;
	uint8_t RemoteCpnfigSource;
	uint8_t UpdateRootkeyFlag;
	uint8_t UpdateRootkeySource;
	uint8_t VehGPSFlag;
	uint8_t VehGPSSource;
	uint8_t RemoteDiagnoFlag;
	uint8_t RemoteDiagnoSource;
}TimeOutType_t;




typedef struct timelog{
	
	time_t  time8090;
	time_t  time4g;
	
}Timelog_t;

//Timelog_t timelog;

#define mqtttest 1
#if mqtttest == 1

#define CILENT_Id "1234"
#define HOST "111.200.239.208"
#define PORT  18835
#define PORTS 18836
#define PSK "123456"
#define PSK_ID "yyhhuunnmmtt"
#define KEEP_ALIVE 60
#define MSG_MAX_SIZE  512
#define TOPIC_MAX_SIZE 1
#define USERNAME "guo"
#define PASSWARD "123456"

#endif


class CMcu;
class bleUart;

class CFAWACP
{
private:
	CFAWACP();

public:
	~CFAWACP();
	static CFAWACP* GetInstance();
	void RUN_FAWACP_TASK();
	/*		*/
	int socketConnect();
	/*		*/
	int disconnectSocket();

	int TestsocketConnect();  //

	int AccOffUpgrade();

	void parse_http_url( char *url, char* host_protocol, uint16_t *host_port , char *domain_name, char *file_path);

/*******************************************/
	/*  */
	void InitAcpTBoxAuthData();
	/*  */
	void Init_FAWACP_dataInfo();
	/*	*/
	int initTBoxParameterInfo();
	/**/
	int readTBoxParameterInfo();

	
	/**/
	int updateTBoxParameterInfo();

	void BackUpSyncMQTTParaAndPemFile();

	void ftp_upload_log(uint8_t *playload);


	int readMQTTParameterInfo();
	int initMQTTParameterInfo();
	int updateMQTTParameterInfo();

/************************************************/
	/* */
	static void *receiveThread(void *arg);
	/**/
	static void *TimeOutThread(void *arg);

	static void *SendThread(void *arg);

	static void *MQTTSendThread(void *arg);
	
	static void *StartCheckOSVersionThread(void *arg);

	void *CheckOSVersionThread(void);

	static void *CameraThread(void *arg);

/******************************************/
	/**/
	void printf_link(LinkTimer_t **head);
	/**/
	int CheckTimer(LinkTimer_t *pb, uint8_t *flag);
	/**/
	int insertlink(uint8_t applicationID, uint8_t MsgType, uint8_t TspSourceID);
	/**/
	void insert(LinkTimer_t **head,LinkTimer_t *p_new);
	/**/
	LinkTimer_t *search (LinkTimer_t *head,uint8_t AcpAppID);
	/**/
	void delete_link(LinkTimer_t **head,uint8_t AcpAppID);

private:	
	/**/

/********************************************/
	int	sendLoginAuth(uint8_t MsgType, AcpCryptFlag_E CryptFlag);
	/*	TBox */
	int	sendLoginAuthApplyMsg();
	/*	CT Message   */
	int	sendLoginCTMsg();
	/*	RS Message   */
	int	sendLoginRSMsg();
	/*	AuthReadyMsg   */
	int	sendLoginAuthReadyMsg();
	
public:	
	callback_RemoteConfigCmd 	Req_McuRemoteCofig;
	callback_RemoteCtrlCmd		Req_McuRemoteCtrl;

	void Set_ReportRemoteConfigCmd(        callback_RemoteConfigCmd cb_RemoteConfigCmd );
	void Set_reportRemoteCtrlCmd(       callback_RemoteCtrlCmd cb_RemoteCtrlCmd );

	void RegisterMcu_CallbackFun();

	
	static void TspRemoteCtrlRes();

	int VehicleReissueSend();
	int RespondTspPacket(AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID);

	int ShowdataTime(uint8_t *pos);
	int getServerIP(int flag, char *ip, int ip_size, int *port, const char *domainName);
	
	/****************************************************************
	* Function Name: PackData_Signature
	* Auther	   : liuyj
	* Date		   : 2017-12-01
	* inParam		: dataBuff;
	* outParam		
	* Description  : 
	* Return	   :
	*****************************************************************/
	void PackData_Signature(uint8_t *dataBuff, uint16_t len, uint8_t *OutBuff);
	/****************************************************************
	* Function Name: PackFAWACP_FrameData
	* Auther	   : liuyj
	* Date		   : 2017-12-01
	* Description  : 
	* inParam	   : dataBuff:;
					 applicationID:;CryptFlag:,TspSourceID:Tspsourc¼
	* outParam	   dataBuff
	* Return	   : 
	*****************************************************************/
	uint16_t PackFAWACP_FrameData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID = 0);
	
	/****************************************************************
	* Function Name	: PackFAWACP_PayloadData
	* Auther		: liuyj
	* Date			: 2017-12-01
	* Description	: Payload
	* inParam		: dataBuff;bufSize;
					  applicationID;MsgType;CryptFlag
	* outParam		dataBuff
	* Return		: 
	*****************************************************************/
	uint32_t PackFAWACP_PayloadData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID = 0);

	/****************************************************************
	* Function Name: Unpack_FAWACP_FrameData
	* Auther	   : liuyj
	* Date		   : 2017-12-01
	* Description  : 
	* input		   : pData;DataLen;
	* Return	   : the length of packed data
	*****************************************************************/
	uint16_t Unpack_FAWACP_FrameData(uint8_t *pData, int32_t DataLen);

	uint16_t unpackDataLenDeal(uint8_t *pTemp, uint16_t &DataLen);
	uint16_t UnpackData_AcpLoginAuthen(uint8_t *PayloadBuff,uint16_t payload_size,uint8_t MsgType);
	uint16_t Unpacket_AcpHeader(AcpAppHeader_t& Header, uint8_t *pData, int32_t DataLen);
	uint16_t UnpackData_AcpRemoteCtrl(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpCameraCtrl(uint8_t *PayloadBuff, uint8_t MsgType);
	uint16_t UnpackData_AcpQueryVehicleCond(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t applicationID, uint8_t MsgType);
	uint16_t UnpackData_AcpRemoteConfig(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpUpdateRootKey(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpRemoteUpgrade(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpVehCondUpload(uint8_t *PayloadBuff, uint16_t payload_size,  uint8_t MsgType);
	uint16_t UnpackData_AcpRemoteDiagnostic(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	static uint16_t timingReportingData(uint8_t MsgType, AcpAppID_E AppID);

	uint16_t   PackBLEPeriodicCarInfoData(uint8 * Pdatebuff,uint16 u16BuffLen);

/*  */
	char CameraUpload(uint8_t *PayloadBuff);

/*  */
       int  DownLoadUpgradePackage();

	uint16_t unpack_CameraCtrlDeal(uint8_t *cmddata);
/*  */
	uint8_t resolution_ratio(uint8_t cmd);
	uint8_t frame_rate(uint8_t cmd);
	uint8_t cycle_camera(uint8_t cmd);
	uint8_t open_camera(void);
	uint8_t stop_camera(void);
	uint8_t Gsensor_sensitivity(uint8_t cmd);
	uint8_t Video_type(uint8_t cmd);

	static uint16_t MQTTtimingReportingData(uint8_t MsgType, AcpAppID_E AppID);

	uint16_t Unpack_Gsensor_Data(uint8_t *pData, int32_t DataLen);
protected:
	/****************************************************************
	* Function Name: SOFEOFDeal
	* Auther	   : liuyj
	* Date		   : 2017-12-16
	* Description  : 
	* Param        :pTemp, nTempNum
	* Return	   : no
	*****************************************************************/
	void SOFEOFDeal(uint8_t* pTemp, uint32_t nTempNum);
	
	uint16_t addDataLen(uint8_t *pTemp, uint16_t DataLen, uint8_t Identifier, uint8_t flag);
	uint16_t checkReceivedData(uint8_t *pData, uint32_t len);
	//uint16_t unpackDataLenDeal(uint8_t *pTemp, uint16_t &DataLen);
	/****************************************************************
	* Function Name: TimeDeal
	* Auther	   : liuyj
	* Date		   : 2017-12-15
	* Description  : 
	* Return	   : no
	*Param		   pTemp
	*****************************************************************/	
	void TimeDeal(uint8_t* pTemp, uint8_t isFlagLen = 1);
	/****************************************************************
	* Function Name: TimeStampPart
	* Auther	   : liuyj
	* Date		   : 2017-12-18
	* Description  : 
	* Return	   : no
	*Param		   pTemp, pTimeStamp
	*****************************************************************/
	void TimeStampPart(uint8_t* pTemp, TimeStamp_t *pTimeStamp); 
	/****************************************************************
	* Function Name: AuthTokenDeal
	* Auther	   : liuyj
	* Date		   : 2017-12-16
	* Description  : 
	* Param        : pTemp
	* Return	   : no
	*****************************************************************/
	void AuthTokenDeal(uint8_t* pTemp);
	/****************************************************************
	* Function Name: SourceDeal
	* Auther	   : liuyj
	* Date		   : 2017-12-16
	* Description  : 
	* Param        : pTemp,TspSourceID:source,TSPTBox0
	* Return	   : no
	*****************************************************************/
	void BleTokenDeal(uint8_t* pTemp);
	uint16_t MQTTReqBleTokenData(uint8_t MsgType, AcpAppID_E AppID);
	void SourceDeal(uint8_t* pTemp, uint8_t nfalgTsp = 1);
	uint16_t UnpackData_AcpBleToken(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);

	/****************************************************************
	* Function Name: ErrorCodeDeal
	* Auther	   : liuyj
	* Date		   : 2017-12-16
	* Description  : 
	* Param        : pTemp,nErrorCode:nErrorCode
	* Return	   : no
	*****************************************************************/
	void ErrorCodeDeal(uint8_t* pTemp, uint8_t nErrorCode);	

	//
	uint16_t AuthApplyDataDeal(uint8_t *pTemp);	
	//
	uint16_t CTMsgDataDeal(uint8_t *pTemp);
	//
	uint16_t RSMsgDataDeal(uint8_t* pTemp);
	//
	uint16_t AuthReadyMsgDataDeal(uint8_t* pTemp);
	//
	uint16_t RemoteCtrlCommandDeal(uint8_t *dataBuff);
	//
	uint16_t ReportVehicleCondDeal(uint8_t* dataBuff, uint8_t flag);
	//
	uint16_t RemoteConfigCommandDeal(uint8_t *dataBuff);
	//RootKey
	uint16_t RootKeyDataDeal(uint8_t *dataBuff);
	//
	uint16_t RemoteUpgrade_ResponseDeal(uint8_t *dataBuff);
	//
	uint16_t RemoteUpgrade_DownloadStartDeal(uint8_t *dataBuff);
	//
	uint16_t RemoteUpgrade_DownloadResultDeal(uint8_t *dataBuff);
	//
	uint16_t RemoteUpgrade_UpdateStartDeal(uint8_t *dataBuff);
	//
	uint16_t RemoteUpgrade_UpdateResultDeal(uint8_t *dataBuff);
	//
	uint16_t ReportGPSSpeedDeal(uint8_t* dataBuff);
	//
	uint16_t FaultSignDeal(uint8_t* dataBuff);
	//
	uint16_t VehicleGPSDeal(uint8_t* dataBuff);
	//
	uint16_t RemoteDiagnosticDeal(uint8_t *dataBuff);
	int	sendLoginMsg(uint8_t MsgType, AcpCryptFlag_E CryptFlag);
	uint16_t SignCodeDeal(uint8_t* pTemp, uint8_t inValid, uint16_t SignCde);
	uint16_t addVerTboxOS(uint8_t* pTemp);
	uint16_t addChargeState(uint8_t* pTemp);
	uint16_t addPowerCellsState(uint8_t* pTemp);
	uint16_t addKeepingstateTime(uint8_t* pTemp);
	uint16_t addAirconditionerInfo(uint8_t* pTemp);
	uint16_t addRemainUpkeepMileage(uint8_t* pTemp);
	uint16_t addSafetyBelt(uint8_t* pTemp);
	uint16_t addPowerSupplyMode(uint8_t* pTemp);
	uint16_t addParkingState(uint8_t* pTemp);
	uint16_t addHandBrakeState(uint8_t* pTemp);
	uint16_t addGearState(uint8_t* pTemp);
	uint16_t addEngineSpeed(uint8_t* pTemp);
	uint16_t addPastRecordWheelState(uint8_t* pTemp);
	uint16_t addPastRecordSpeed(uint8_t* pTemp);
	uint16_t addWheelState(uint8_t* pTemp);
	uint16_t addEngineState(uint8_t* pTemp);
	uint16_t addVerTboxMCU(uint8_t* pTemp);
	uint16_t addVerTboxBLE(uint8_t* pTemp);
	uint16_t addTyreState(uint8_t* pTemp);
	uint16_t addCarlampState(uint8_t* pTemp);
	uint16_t addWindowState(uint8_t* pTemp);
	uint16_t addSunroofState(uint8_t* pTemp);
	uint16_t addCarLockState(uint8_t* pTemp);
	uint16_t addCarDoorState(uint8_t* pTemp);
	uint16_t addSTAverageOil(uint8_t* pTemp);
	uint16_t addLTAverageOil(uint8_t* pTemp);
	uint16_t addSTAverageSpeed(uint8_t* pTemp);
	uint16_t addLTAverageSpeed(uint8_t* pTemp);
	uint16_t addCurrentSpeed(uint8_t* pTemp);
	uint16_t addBattery(uint8_t* pTemp);
	uint16_t addOdometer(uint8_t* pTemp);
	uint16_t addRemainedOil(uint8_t* pTemp);
	uint16_t add_GPSData(uint8_t* pTemp);
	uint16_t addVerIVI(uint8_t* pTemp);
	uint16_t addChargeConnectState(uint8_t* pTemp);
	uint16_t addBrakePedalSwitch(uint8_t* pTemp);
	uint16_t addAcceleraPedalSwitch(uint8_t* pTemp);
	uint16_t addYaWSensorInfoSwitch(uint8_t* pTemp);
	uint16_t addAmbientTemperat(uint8_t* pTemp);
	uint16_t addPureElecRelayState(uint8_t* pTemp);
	uint16_t addResidualRange(uint8_t* pTemp);
	uint16_t addNewEnergyHeatManage(uint8_t* pTemp);
	uint16_t addVehWorkMode(uint8_t* pTemp);
	uint16_t addMotorWorkState(uint8_t* pTemp);
	uint16_t addHighVoltageState(uint8_t* pTemp);
	uint16_t addFaultSignDeal(uint8_t* dataBuff);

	uint16_t addMotorSpeed(uint8_t* pTemp);
	uint16_t addPowerState(uint8_t* dataBuff);
	

	uint16_t addNodeMissState(uint8_t* pTemp);


	uint16_t addSignalValue(uint8_t* pTemp);
	uint16_t addCarType(uint8_t* pTemp);
	uint16_t addIccid(uint8_t* pTemp);
	uint16_t addRentMode(uint8_t* pTemp);
	uint16_t addGPSDetail(uint8_t* pTemp);

	uint16_t Camera_Gsensor(uint8_t *dataBuff);
	uint16_t CameraCtrl_resultDeal(uint8_t *dataBuff, char *result);


private:
//
	void Packet_AcpHeader(AcpAppHeader_t& Header, AcpAppID_E AppID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint16_t MsgLength);
	uint16_t AddData_AcpHeader(uint8_t *dataBuff, AcpAppHeader_t Header);
public:	
	
	CMcu *m_CMcu;
	bleUart *m_bleUart;
	
	
	RingBuffer		m_RingBuff;
	HmacSHA 		m_SHA1OpData;
	HMAC_MD5 		m_Hmacmd5OpData;

	/*	SHA-1*/
	int Encrypt_SHA1Data(uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t OutLen = 20);
	/*	SHA-1	*/
	int Decrypt_SHA1Data(const uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t OutLen);
	/*	AES128	*/
//	void Encrypt_AES128Data(uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t& OutLen, AcpCryptFlag_E CryptFlag, uint8_t *Key);
	/*	AES128	*/
//	void Decrypt_AES128Data(uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t& OutLen, AcpCryptFlag_E CryptFlag, uint8_t *Key);
	/*	HMACMD5*/
	void HMACMd5_digest(const char *SecretKey, uint8_t *Content, uint16_t InLen, uint8_t OutBuff[16]);
	/*	CRC32*/
	uint32_t Crc32_Count(unsigned long inCrc32, const uint8_t *buf, int size);

protected:
	//
//	ISetVehicleConditionData(VehicleCondition_Data &VehicleData);
	

private:
	static CFAWACP* cfawacp;
	int TSPACK_WorkWakeup_State;
	int TSPACK_WorkSleep_State;

public:
	struct  sockaddr_in 	m_socketaddr;
	time_t HighReportTime;
	time_t LowReportTime;
	time_t ReissueReportTime;
	time_t NowHighReportTime;
	time_t NowLowReportTime;
	time_t UpdateSelfCheckTime;
	time_t TimeCheckTime;
	time_t UpgradeTime;

	time_t UpDateBleTokenTime;

	int InLoginFailTimes;
	int OutLoginFailTimes;
	uint8_t TspctrlSource;
	int 	m_Socketfd;			//socket ID
	bool 	m_ConnectedState;	//
	uint8_t m_u8TryDownLoadCount;
	uint8_t m_u8TryDownLoadFlag;

#if 0
	int		m_mqtt_pb_ret;
	uint8_t	m_mqtt_pb_year;
#endif
//	volatile char	m_mqtt_connect_cb_result;//mqtt_connect
	bool   m_LocalTimeSyncState;
	bool   m_TimeSyncHeartReceiveFlag;

	uint8_t m_LocalTimeSyncCount;
	bool    m_Upgrade;          //
	uint8_t m_loginState;		//
	uint8_t	m_SendMode;	//
	bool	m_bEnableSendDataFlag;//
	uint8_t m_RemoteMode;		//
	uint8_t TboxSleepStatus;
	uint8_t	ConnectFaultTimes;
	uint8_t m_ReDataCallCount;
	LinkTimer_t *Headerlink;
	TimeOutType_t TimeOutType;
	TimeStamp_t	m_tspTimestamp;//tsp
	uint8_t  TspWorkMode;
	uint8_t u8ModemReDataCallCount;
	
	uint8_t BleToken[6];
	uint8_t u8BleTokenReqTimeOut;
       uint8_t BleReqUpdateTokenFlag;
	//
	//uint8_t	RootKey[16];	//
	uint8_t AuthToken[4];
	uint8_t AKey_TBox[16];
	uint8_t SKey_TBox[16];	 //(AKey_TBox Decrypt SKeyC_Tsp)
	AcpTBoxAuthApplyMsg_t	AcpTBox_AuthApplyMsg;//TBoxuth Apply Message	
	AcpTBoxCTMsg_t			AcpTBox_CTMsg;		 //TBoxT Message	
	AcpTBoxRSMsg_t			AcpTBox_RSMsg;		 //TBoxS Message	
	AcpTBoxAuthReadyMsg_t	AcpTBox_AuthReadyMsg;//TBoxAuth Ready Message	

	AcpTspAkeyMsg_t			AcpTsp_AkeyMsg;		//Key Message
	AcpTspCSMsg_t			AcpTsp_CSMsg;		//
	AcpTspSKeyMsg_t			AcpTsp_SKeyMsg;		//
	AcpTspAuthReadyACKMsg_t	AcpTsp_AuthReadyACKMsg;//
/*TSP*/
	//// uint8_t	RootKey_Tsp[32];	//
	//// uint8_t	AKey_TSP[16];	//
	//uint8_t	AKeyC_Tsp[32];	//
	////uint8_t	Rand1_Tsp[16];	//
	
	//uint8_t Rand1CS_Tsp[32];//(AKey_TSP Encrypt Rand1_Tsp)
	//uint8_t Rand2CS_Tsp[32];/(AKey_TSP Encrypt Rand2_Tbox)[Send3]
	//uint8_t RT_Tsp;		  //Rand1CS_Tsp == Rand1CT_TBox[Send3]
	
	////uint8_t	SKey_Tsp[16];	//
	//uint8_t	SKeyC_Tsp[32];	//(AKey_TSP Encrypt SKey_Tsp)[Send5]
	//uint8_t Rand3C;//(SKey_Tsp Encrypt Rand3_Tbox)[Send5]	
/*TBox*/
	//uint8_t	RootKey_Tbox[32];	 
	//uint8_t AKey_TBox[16];	 //AKey(RootKey Decrypt AkeyC_Tsp)
	//uint8_t	Rand1CT_TBox[32];//(AKey_TBox Encrypt Rand1_Tsp)[Send2]
	//uint8_t   Rand2_Tbox[16];  //Send
	
	//uint8_t	Rand2CT_TBox[32];//AKey_TBox Encrypt Rand2CS_Tsp)	
	//uint8_t   Rand3_Tbox[16];  //[Send4]
	//uint8_t	RS_TBox;		 //Rand2CT_TBox == Rand2CS_Tsp[Send4]
	

	//uint8_t Rand3CT_TBox[32];//(SKey_TBox Encrypt Rand3_Tbox)
	//uint8_t	BSKeySuccess;	 //Rand3CT_TBox == Rand3CS_Tsp

	AcpRemoteControlCommand_t	m_AcpRemoteCtrlCommandData;	//
	AcpRemoteControl_RawData 	m_AcpRemoteCtrlList;			//
	AcpVehicleCondCommandList_t	m_AcpVehicleCondCommandList;	//
	AcpRemoteConfig_RawData		m_AcpRemoteConfig;				//
	AcpRemoteUpgrade_t			m_AcpRemoteUpgrade;			//
	RemoteDiagnose_t			m_RemoteDiagnose;					//
	RemoteDiagnoseResult_t		m_RemoteDiagnoseResult;		//

	uint8_t Upgrade_CommandResponse;	//
	uint8_t Upgrade_Reserve;				//
	uint8_t Upgrade_DownloadState;		//
	uint8_t Upgrade_Result;				//
	
	pthread_t receiveThreadId;
	pthread_t TimeOutThreadId;
	pthread_t SendThreadId;
	pthread_t CheckOSVersionId;
	pthread_t CameraThreadId;
public:
	//mqtt member

	bool session;
	struct mosquitto *mosq;
	
	uint8_t MQTT_Topic[MAXTOPIC][TOPICLEN];
    	MQTT_Config mqtt_config;
    	void UpdataMQTTTopic(uint8_t * vin);
	static void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
	static void mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int mid);
	static void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result);

	int MQTTConnect();

	GPSPositionData_t	CameraGPSData;
};

extern FAWACPInfo_Handle *p_FAWACPInfo_Handle;
extern TBoxDataPool *dataPool;
extern char target_ip[16];

#endif
