#ifndef _MQTTACP_H_
#define _MQTTACP_H_
#if 0
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
//#include "FAWACP_Data.h"
//#include "VehicleData.h"
#include "HMAC_MD5.h"
#include "HmacSHA-1.h"
//#include "mcuUart.h"
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
#include "FAWACP.h"

#ifndef MQTT_DEBUG
	#define MQTT_DEBUG 	1
#endif


#if MQTT_DEBUG
	#define MQTTLOG(format,...) printf("### MQTTACP ### %s [%d] "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define MQTT_NO(format,...) printf(format,##__VA_ARGS__)
#else

	#define MQTTLOG(format,...)
	#define MQTT_NO(format,...)
#endif

#define VERSION_FLAG	1		//0,测试;1,正式


#define SLEEPTIMEOUT	5
#define TCPACK_SLEEP_TIMEOUT_TIMES		3
#define TCPACK_WAKEUP_TIMEOUT_TIMES		5
#define BLE_UPGRADE_TIMES				5
#define LOGIN_MAX_TIMES					3

#ifndef LTE_FILE_NAME
#define LTE_FILE_NAME         (char*)"/data/LteUpgrade.bin"
#endif

#ifndef RM_LTE_FILE
#define RM_LTE_FILE           (char*)"rm -rf /data/LteUpgrade.bin"
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

#define MCU_WAKEUP_TIMES		(10)
#define HTTP_DOWNLOAD_TIMEOUT 	(3)

#define  TSP_WORK_MODE_REQ_MODEM_REDATACALL		0
#define  TSP_WORK_MODE_REQ_MODEM_REBOOT			1
#define  TSP_WORK_MODE_WAIT_MODEM_FINISH  		2
#define  TSP_WORK_MODE_WAIT_MCU_FINISH			3
#define  TSP_WORK_MODE_SOCKET_DISCONNECT		4
#define  TSP_WORK_MODE_SOCKET_CONNECT			5
#define  TSP_WORK_MODE_LOGIN_TSP				6
#define  TSP_WORK_MODE_WAIT_LOGIN_TSP			7
#define  TSP_WORK_MODE_PERIOD_REPORT			8

#define  TSP_WORK_MODE_First_Login   			9
#define  TSP_WORK_MODE_MQTT_INIT				10





#define MAXTOPIC  6
#define TOPICLEN  20



typedef int (*callback_RemoteCtrlCmd)();
typedef int (*callback_RemoteConfigCmd)(uint8_t, uint16_t);



#if 1

typedef struct{
	uint8_t m_HOST[50];		//MQTT Server Host
	uint8_t m_CILENT_Id[15];		
    uint8_t m_PSK[15];		
    uint8_t m_PSK_ID[15];		
    uint8_t m_USERNAME[15];		
    uint8_t m_PASSWARD[15];		

    uint16_t m_PORT;
    uint16_t m_PORTS;
    uint16_t m_KEEP_ALIVE;
    uint16_t m_MSG_MAX_SIZE;
    uint16_t m_TOPIC_MAX_SIZE;

}MQTT_Config;


typedef struct{
	uint8_t VehicleUploadTP[20];		
	uint8_t RemoteCtrlTP[20];		
    uint8_t RemoteCtrlReplyID[20];		
    uint8_t RemoteUpgradeTP[20];		
    uint8_t RemoteUpgradeReplyID[20];		
    uint8_t GetLogTP[20];		


}MQTT_Topic;

typedef enum MQTT_TOPIC_TAG{
  VehicleUploadTP = 0, 
  RemoteCtrlTP,  
  RemoteCtrlReplyID,
  RemoteUpgradeTP,
  RemoteUpgradeReplyID,
  GetLogTP
}MQTT_TOPIC_TAG;
#endif

class mcuUart;
class bleUart;
class CFAWACP;

class MQTT
{
private:
	MQTT();

public:
	~MQTT();
	static MQTT* GetInstance();
	void RUN_MQTT_TASK();
	/*	连接网络	*/
	int MQTTConnect();
	/*	断开网络	*/
	int disconnectSocket();

	int TestsocketConnect();  //生产测试

	int SleepUpgrade();

	void parse_http_url( char *url, char* host_protocol, uint16_t *host_port , char *domain_name, char *file_path);

/*******************初始化相关函数************************/
	/* 初始化登陆鉴权数据 */
	void InitAcpTBoxAuthData();
	/* 初始化车身相关数据 */
	void Init_FAWACP_dataInfo();
	/*	初始化参数信息	*/
	int initTBoxParameterInfo();
	/*	读取参数信息	*/
	int readTBoxParameterInfo();
	/*	更新参数信息	*/
	int updateTBoxParameterInfo();
	void ftp_upload_log(uint8_t *playload);

/*********************线程函数***************************/
	/*	接收数据 	*/
	static void *receiveThread(void *arg);
	/*	超时处理 	*/
	static void *TimeOutThread(void *arg);

	static void *SendThread(void *arg);



private:	
	/*	登陆服务器认证 <登陆认证过程中出现任何异常则重新开始认证> */

/*****************发送打包登陆信息相关函数***************************/
	int	sendLoginAuth(uint8_t MsgType, AcpCryptFlag_E CryptFlag);
	/*	TBox认证请求   */
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
	* Function Name: PackMQTT_FrameData
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 整数据包
	* inParam	   : dataBuff:整包数据缓存;bufSize:数据长度;
					 applicationID:服务标识号;MsgType:服务消息类型;CryptFlag:加密标志,TspSourceID:Tsp发起的命令source值
	* outParam	   ：dataBuff：打包后数据缓存
	* Return	   : 打包后数据长度
	*****************************************************************/
	uint16_t PackMQTT_FrameData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID = 0);
	
	/****************************************************************
	* Function Name	: PackFAWACP_PayloadData
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description	: 数据体Payload打包
	* inParam		: dataBuff：待打包数据缓存;bufSize:缓存大小;
					  applicationID:服务标识号;MsgType:服务消息类型;CryptFlag:加密标志
	* outParam		：dataBuff：打包后数据缓存
	* Return		: 打包后数据长度
	*****************************************************************/
	uint32_t PackMQTT_PayloadData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID = 0);

	/****************************************************************
	* Function Name: Unpack_MQTT_FrameData
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 解析协议数据包
	* input		   : pData:数据缓存;DataLen:数据长度;
	* Return	   : the length of packed data
	*****************************************************************/
	uint16_t Unpack_MQTT_FrameData(uint8_t *pData, int32_t DataLen);

	uint16_t unpackDataLenDeal(uint8_t *pTemp, uint16_t &DataLen);
	uint16_t UnpackData_AcpLoginAuthen(uint8_t *PayloadBuff,uint16_t payload_size,uint8_t MsgType);
	uint16_t Unpacket_AcpHeader(AcpAppHeader_t& Header, uint8_t *pData, int32_t DataLen);
	uint16_t UnpackData_AcpRemoteCtrl(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpQueryVehicleCond(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t applicationID, uint8_t MsgType);
	uint16_t UnpackData_AcpRemoteConfig(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpUpdateRootKey(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpRemoteUpgrade(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t UnpackData_AcpVehCondUpload(uint8_t *PayloadBuff, uint16_t payload_size,  uint8_t MsgType);
	uint16_t UnpackData_AcpRemoteDiagnostic(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType);
	uint16_t MQTTtimingReportingData(uint8_t MsgType, AcpAppID_E AppID);
protected:
	/****************************************************************
	* Function Name: SOFEOFDeal
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 帧头和尾的处理
	* Param        :pTemp:跟随指针, nTempNum:值
	* Return	   : no
	*****************************************************************/
	void SOFEOFDeal(uint8_t* pTemp, uint32_t nTempNum);
	//数据长度处理
	uint16_t addDataLen(uint8_t *pTemp, uint16_t DataLen, uint8_t Identifier, uint8_t flag);
	uint16_t checkReceivedData(uint8_t *pData, uint32_t len);
	//uint16_t unpackDataLenDeal(uint8_t *pTemp, uint16_t &DataLen);
	/****************************************************************
	* Function Name: TimeDeal
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 时间戳处理
	* Return	   : no
	*Param		   ：pTemp：跟随指针
	*****************************************************************/	
	void TimeDeal(uint8_t* pTemp, uint8_t isFlagLen = 1);
	/****************************************************************
	* Function Name: TimeStampPart
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 时间戳处理
	* Return	   : no
	*Param		   ：pTemp：跟随指针, pTimeStamp:时间戳的数据体
	*****************************************************************/
	void TimeStampPart(uint8_t* pTemp, TimeStamp_t *pTimeStamp); 
	/****************************************************************
	* Function Name: AuthTokenDeal
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 车辆身份令牌
	* Param        : pTemp:跟随指针
	* Return	   : no
	*****************************************************************/
	void AuthTokenDeal(uint8_t* pTemp);
	/****************************************************************
	* Function Name: SourceDeal
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 指令源信息
	* Param        : pTemp:跟随指针,TspSourceID:source源,由TSP发起的值。TBox发起的值为0
	* Return	   : no
	*****************************************************************/
	void SourceDeal(uint8_t* pTemp, uint8_t nfalgTsp = 1);
	/****************************************************************
	* Function Name: ErrorCodeDeal
	* Auther	   : Yangjun
	* Date		   : 2019-5-18
	* Description  : 错误信息处理
	* Param        : pTemp:跟随指针,nErrorCode:nErrorCode值
	* Return	   : no
	*****************************************************************/
	void ErrorCodeDeal(uint8_t* pTemp, uint8_t nErrorCode);	

	//处理AuthApply数据体(启动时必须先初始化车厂标识和VIN等相关参数)
	uint16_t AuthApplyDataDeal(uint8_t *pTemp);	
	//处理CTMsg数据体
	uint16_t CTMsgDataDeal(uint8_t *pTemp);
	//处理RSMsg数据体
	uint16_t RSMsgDataDeal(uint8_t* pTemp);
	//处理AuthReady数据体
	uint16_t AuthReadyMsgDataDeal(uint8_t* pTemp);
	//远程控制打包
	uint16_t RemoteCtrlCommandDeal(uint8_t *dataBuff);
	//车况上报数据打包
	uint16_t ReportVehicleCondDeal(uint8_t* dataBuff, uint8_t flag);
	//远程配置数据打包
	uint16_t RemoteConfigCommandDeal(uint8_t *dataBuff);
	//RootKey更换
	uint16_t RootKeyDataDeal(uint8_t *dataBuff);
	//远程升级【命令返回】数据结构打包
	uint16_t RemoteUpgrade_ResponseDeal(uint8_t *dataBuff);
	//远程升级【下载开始】数据结构打包
	uint16_t RemoteUpgrade_DownloadStartDeal(uint8_t *dataBuff);
	//远程升级【下载结果】数据结构打包
	uint16_t RemoteUpgrade_DownloadResultDeal(uint8_t *dataBuff);
	//远程升级【升级开始】数据结构打包
	uint16_t RemoteUpgrade_UpdateStartDeal(uint8_t *dataBuff);
	//远程升级【升级结果】数据结构打包
	uint16_t RemoteUpgrade_UpdateResultDeal(uint8_t *dataBuff);
	//位置和车速定时上传---GPS位置信号+实时车速
	uint16_t ReportGPSSpeedDeal(uint8_t* dataBuff);
	//故障信号打包
	uint16_t FaultSignDeal(uint8_t* dataBuff);
	//车辆定位---GPS位置信号
	uint16_t VehicleGPSDeal(uint8_t* dataBuff);
	//远程诊断
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


private:
//打包头数据
	void Packet_AcpHeader(AcpAppHeader_t& Header, AcpAppID_E AppID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint16_t MsgLength);
	uint16_t AddData_AcpHeader(uint8_t *dataBuff, AcpAppHeader_t Header);
public:	
	
	mcuUart *m_mcuUart;
	bleUart *m_bleUart;
	CFAWACP *pfawacp;
	
	RingBuffer		m_MQTTRingBuff;
	HmacSHA 		m_SHA1OpData;
	HMAC_MD5 		m_Hmacmd5OpData;

	/*	SHA-1加密	*/
	int Encrypt_SHA1Data(uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t OutLen = 20);
	/*	SHA-1解密	*/
	int Decrypt_SHA1Data(const uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t OutLen);
	/*	AES128加密	*/
//	void Encrypt_AES128Data(uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t& OutLen, AcpCryptFlag_E CryptFlag, uint8_t *Key);
	/*	AES128解密	*/
//	void Decrypt_AES128Data(uint8_t *InBuff, uint16_t InLen, uint8_t *OutBuff, uint16_t& OutLen, AcpCryptFlag_E CryptFlag, uint8_t *Key);
	/*	HMACMD5算法	*/
	void HMACMd5_digest(const char *SecretKey, uint8_t *Content, uint16_t InLen, uint8_t OutBuff[16]);
	/*	CRC32校验	*/
	uint32_t Crc32_Count(unsigned long inCrc32, const uint8_t *buf, int size);

protected:
	//接口：MCU写入结构数据
//	ISetVehicleConditionData(VehicleCondition_Data &VehicleData);

private:
	static MQTT* mqtt;
	int TSPACK_WorkWakeup_State;
	int TSPACK_WorkSleep_State;

public:
	struct  sockaddr_in 	m_socketaddr;
	time_t HighReportTime;
	time_t LowReportTime;
	time_t ReissueReportTime;
	time_t NowHighReportTime;
	time_t NowLowReportTime;
	time_t NowReissueReportTime;
	int InLoginFailTimes;
	int OutLoginFailTimes;
	uint8_t TspctrlSource;
	int 	m_Socketfd;			//socket ID
	bool 	m_ConnectedState;	//连接状态
	bool    m_Upgrade;          //升级状态
	uint8_t m_loginState;		//登陆状态(0:未登陆,1:正在登陆,2:已登陆)
	uint8_t	m_SendMode;	//发送方式
	bool	m_bEnableSendDataFlag;//使能数据发送标志(用于RootKey更换时)
	uint8_t m_RemoteMode;		//远程模式标识
	uint8_t TboxSleepStatus;
	uint8_t	ConnectFaultTimes;
	LinkTimer_t *Headerlink;
	TimeOutType_t TimeOutType;
	TimeStamp_t	m_tspTimestamp;//tsp时间
	uint8_t  TspWorkMode;
	uint8_t   u8ModemReDataCallCount;

	//登陆鉴权相关数据
	//uint8_t	RootKey[16];	//根密钥(与终端对应)(需存全局)
	uint8_t AuthToken[4];
	uint8_t AKey_TBox[16];
	uint8_t SKey_TBox[16];	 //解密后的会话密钥(AKey_TBox Decrypt SKeyC_Tsp)


	AcpRemoteControlCommand_t	m_AcpRemoteCtrlCommandData;	//远程控制命令数据参数值
	AcpRemoteControl_RawData 	m_AcpRemoteCtrlList;		//远程控制命令列表
	AcpVehicleCondCommandList_t	m_AcpVehicleCondCommandList;//车况查询指令信号列表
	AcpRemoteConfig_RawData		m_AcpRemoteConfig;			//远程配置列表
	AcpRemoteUpgrade_t			m_AcpRemoteUpgrade;			//升级数据
	RemoteDiagnose_t			m_RemoteDiagnose;			//远程诊断命令列表
	RemoteDiagnoseResult_t		m_RemoteDiagnoseResult;		//远程诊断结果


	uint8_t Upgrade_CommandResponse;	//升级命令接收状态0 为成功，1 为失败
	uint8_t Upgrade_Reserve;			//下载开始 默认为 0，无实际意义
	uint8_t Upgrade_DownloadState;		//升级包下载状态
										//0：成功; 1：失败-驾驶员未确认下载
										//2：失败-驾驶员取消下载
										//3：失败-URL 地址相关错误
										//4：失败-下载失败 
										//5：失败-MD5 检测失败
	uint8_t Upgrade_Result;				//升级结果状态
										//0：成功; 
										//1：失败-车主未确认升级
										//2：失败-车主取消升级
										//3：失败-升级过程失败

	pthread_t receiveThreadId;
	pthread_t TimeOutThreadId;
	pthread_t SendThreadId;
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
	
};

extern FAWACPInfo_Handle *p_FAWACPInfo_Handle;
extern TBoxDataPool *dataPool;
extern char target_ip[16];
#endif
#endif
