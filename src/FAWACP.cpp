#include "FAWACP.h"
#include "AES_acp.h"
#include "OTAWiFi.h"
#include "common.h"
#include <fstream>
#include <iostream>
#include <string>
#include "json.h"
using namespace std;

#define IP 		"192.168.0.2"
#define CAMERA_PORT 		9002
#define user_max	2
#define CAMERACTRL_ITEM_NUMBERS		10
#define  TBOX_WORK_MODE_SLEEP    		0
#define  TBOX_WORK_MODE_WORKON	 	1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Rbuf = PTHREAD_MUTEX_INITIALIZER;

CFAWACP* CFAWACP::cfawacp = NULL;

FAWACPInfo_Handle FAWAcpHandle;
FAWACPInfo_Handle *p_FAWACPInfo_Handle = &FAWAcpHandle;
extern bool APN2test;
Timelog_t timelog;
bool  flaftime4g;

extern GPS_Info_t	GPSINFO;
extern bool bleupgrade;
bool ftpupgrade;
bool upgradeflag;
//bool alive;
bool IsCameraUpload;
bool uploadCamera_result = false;
char CameraCtrl_result[CAMERACTRL_ITEM_NUMBERS] = {0};

uint8_t	ALLRootKey[16];//RootKey

uint8_t	CameraGensorTest[7];

int keepalive = 0;
int keepidle = 15;//300;//
int keepinterval = 5;
int keepcount = 5;

uint16_t SocketReconnectCount  =0;
uint16_t SocketDisconnectCount =0;

//extern unsigned char GetModemWorkState(void);
//extern void SetModemWorkState( uint8_t WorkState);
extern void SetTspReDatacallFlag(unsigned char ReDatacallFlag);
//extern unsigned char GetTspNewIpFlag(void);
extern uint8_t GetTspReDatacallFlag(void);

extern uint8_t GetModemWorkOn(void);


void CFAWACP::Set_ReportRemoteConfigCmd( callback_RemoteConfigCmd cb_RemoteConfigCmd )
{
	if(NULL == cb_RemoteConfigCmd)	return ;

	Req_McuRemoteCofig = cb_RemoteConfigCmd;

}
void CFAWACP::Set_reportRemoteCtrlCmd( callback_RemoteCtrlCmd cb_RemoteCtrlCmd )
{
	if(NULL == cb_RemoteCtrlCmd) return ;

	Req_McuRemoteCtrl = cb_RemoteCtrlCmd;

}

void CFAWACP::RegisterMcu_CallbackFun()
{
	Set_ReportRemoteConfigCmd( m_CMcu->Mcu_RemoteConfigRes );
	Set_reportRemoteCtrlCmd( m_CMcu->Mcu_RemoteCtrlRes );
}

CFAWACP* CFAWACP::GetInstance()
{

	if(cfawacp == NULL)
	{
		cfawacp = new CFAWACP();
	}
	
	return cfawacp;

}

int txt_put(int osid_p, char *version, int osid_c)
{
	FILE *fp = NULL;

	char line[16] = {0};
	int n, i;

	fp = fopen(OSVERSION_TXT, "rb+");
	if(fp == NULL)
	{
		return -1;
	}
	n = snprintf(line, 15, "%d\n", osid_p);
	n = fputs(line, fp);
	memset(line, 0, 16);
	n = snprintf(line, 15, "%s", version);
	n = fputs(line, fp);
	memset(line, 0, 16);
	n = snprintf(line, 15, "%d\n", osid_c);
	n = fputs(line, fp);

	fclose(fp);

	return 0;
}

int txt_get(int *osid_p, char *version, int *osid_c)
{
	FILE *fp = NULL;
	char p_osid[16] = {0};
	char c_osid[16] = {0};
	char *ptr = NULL;

	fp = fopen(OSVERSION_TXT, "r");
	if(fp == NULL)
	{
		return -1;
	}
	ptr = fgets(p_osid, 16, fp);
	if(ptr == NULL)
	{
		fclose(fp);
		system(RM_OSVERSION_TXT);
		return -1;
	}
	ptr = fgets(version, 16, fp);
	if(ptr == NULL)
	{
		fclose(fp);
		system(RM_OSVERSION_TXT);
		return -1;
	}
	ptr = fgets(c_osid, 16, fp);
	if(ptr == NULL)
	{
		fclose(fp);
		system(RM_OSVERSION_TXT);
		return -1;
	}
	fclose(fp);
	*osid_p = atoi(p_osid);
	*osid_c = atoi(c_osid);

	return 0;
}

int get_osid_from_file(int stat)
{
	FILE *fp = NULL;
	char buf[128] = {0};
	char version[16] = {0};
	int osid_p, osid_c, osid, ret, i;
	/*ersion*/
	if (access(OSVERSION_TXT, F_OK) == 0)
	{
	}
	else
	{
		fp = fopen(OSVERSION_TXT, "w");
		if (fp == NULL)
		{
			return -1;
		}
		
		strncpy(buf, "0\n00000\n0\n", 32);
		for(i = 0; buf[i] != '\0'; ++i)
		{
			fputc(buf[i], fp);
		}
		fflush(fp);
		fclose(fp);
		fp = NULL;
	}
	ret = txt_get(&osid_p, version, &osid_c);
	if(ret != 0)
	{
		return -1;
	}
	switch (stat)
	{
		case MQTT_VERSION_TEST:
			osid = osid_p;
			break;
		case MQTT_VERSION_NORMAL:
			osid = osid_c;
			break;
		default:
			break;
	}
	return osid;
}

/************/
int save_osid_in_file(int stat, int osid)
{
	int osid_p, osid_c, ret;
	char version[16] = {0};

	ret = txt_get(&osid_p, version, &osid_c);
	if (ret != 0)
	{
		return -1;
	}
	switch (stat)
	{
		case MQTT_VERSION_TEST:
			txt_put(osid, version, osid_c);
			break;
		case MQTT_VERSION_NORMAL:
			txt_put(osid_p, version, osid);
			break;
		default:
			break;
	}
	return 0;
}

int version_cmp(char *os)
{
	int osid_p, osid_c, ret;
	char version[16] = {0};

	ret = txt_get(&osid_p, version, &osid_c);
	if (ret != 0)
	{
		return 0;
	}
	if (strncmp(version, os, 5) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	return 1;
}


/************/
#if MQTT_DEBUG_LOG
int LogLevel = 3;

/* ************ */

char *get_log_level(int log_level)
{
	switch(log_level)
	{
		case MQTT_LOG_FATAL:
			return "FATAL";
		case MQTT_LOG_ERROR:
			return "ERROR";
		case MQTT_LOG_WARN:
			return "WARN";
		case MQTT_LOG_INFO:
			return "INFO";
		case MQTT_LOG_TRACE:
			return "TRACE";
		case MQTT_LOG_DEBUG:
			return "DEBUG";
		default:
			return "OTHER";
	}
}

void get_log_time(char *time_str)
{
	struct tm		sys_time		= {0};
	struct timeval	time_val		= {0};
	time_t			current_time	= {0};

	char usec[20] = {0};
	char msec[20] = {0};

	if(time_str == NULL)
	{
		return ;
	}

	current_time = time(NULL);
	localtime_r(&current_time, &sys_time);

	gettimeofday(&time_val, NULL);

	sprintf(usec, "%06d", time_val.tv_usec);
	strncpy(msec, usec, 3);
	sprintf(time_str, "[%04d-%02d-%02d %02d:%02d:%02d.%3.3s]",
			sys_time.tm_year+1900, sys_time.tm_mon+1, sys_time.tm_mday,
			sys_time.tm_hour, sys_time.tm_min, sys_time.tm_sec, msec);
}



void write_mqtt_log(char *code_file, char *function_name, int code_line, int log_level, char *content)
{
	FILE *fp = NULL;
	struct stat file_stat;
	char filename[128] = {0};
	char LogContent[2048] = {0};
	char TimeStr[128] = {0};
	if(code_file == NULL || function_name == NULL || content == NULL)
	{
		return ;
	}

	if(log_level > LogLevel)
	{
		return ;
	}

	sprintf(filename, "%s", "/data/mylog");
	printf("filename %s\n", filename);

	bzero(&file_stat, sizeof(file_stat));
	stat(filename, &file_stat);
	printf("file size: %d\n", file_stat.st_size);
	if(file_stat.st_size >= 10240000)//10MB
	{
		sprintf(filename, "%s", "/data/mylog");
		printf("filename: %s\n", filename);
		
		fp = fopen(filename, "w");
		fclose(fp);
	}
	fp = fopen(filename, "at+");
	if(fp == NULL)
	{
		return ;
	}

	get_log_time(TimeStr);
	fputs(TimeStr, fp);

	//	snprintf(LogContent, sizeof(LogContent) - 1, "[%s][%s][%04d][%s]%s\n", file_name, function_name, code_line, get_log_level(log_level), content);
	snprintf(LogContent, sizeof(LogContent) - 1, "[%s][%04d][%s]%s\n", function_name, code_line, get_log_level(log_level), content);
	fputs(LogContent, fp);

	fflush(fp);
	fclose(fp);

	fp = NULL;

	return ;
}
#endif


void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)  
{  
	short i;  
	unsigned char highByte, lowByte;  

	for (i = 0; i < sourceLen; i += 2)  
	{  
		highByte = toupper(source[i]);  
		lowByte  = toupper(source[i + 1]);  

		if (highByte > 0x39)  
			highByte -= 0x37;  
		else  
			highByte -= 0x30;  

		if (lowByte > 0x39)  
			lowByte -= 0x37;  
		else  
			lowByte -= 0x30;  

		dest[i / 2] = (highByte << 4) | lowByte;  
	}  
	return ;  
}  


void CFAWACP::mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	uint8_t *strtohex = (uint8_t *)malloc(MALLOC_DATA_LEN);
	char log_str[128] = {0};

	if(strtohex == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
	}
	if(message->payloadlen)
	{
		FAWACPLOG("%s %s %d\n", message->topic, (char *)message->payload,message->payloadlen);
	}
	else
	{
		FAWACPLOG("%s (null)\n", message->topic);
	}


	memcpy(strtohex,(char *)message->payload,message->payloadlen);
	for(int i =0;i<message->payloadlen;i++)
	{

		printf("%02x ",strtohex[i]);
	}
	printf("\n");
	if(strtohex[0] == MQTT_HEADER)
	{
		if(!strcmp(message->topic,cfawacp->MQTT_Topic[RemoteCtrlTP]))
		{

			cfawacp->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
		}
		else if(!strcmp(message->topic,cfawacp->MQTT_Topic[RemoteUpgradeTP]))
		{
			printf( " recevied jason add uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu\r\n");
			cfawacp->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
		}
		#if 0
		else if(!strcmp(message->topic,cfawacp->MQTT_Topic[CamreCtrl]))
		{
			cfawacp->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
		}
		#endif
		else if(!strcmp(message->topic,cfawacp->MQTT_Topic[SyncTime]))
		{
			//cfawacp->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
			uint8_t *pos = strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN + 5;
			cfawacp->ShowdataTime(pos);		
			cfawacp->m_LocalTimeSyncState = 1;
			cfawacp->m_TimeSyncHeartReceiveFlag = 1;

			
			sprintf(log_str, "%s%d", "sync time state: ", cfawacp->m_LocalTimeSyncState);
			if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
			{
				WRITELOGFILE(MQTT_LOG_INFO, log_str);
			}
			printf( "sync time state: recevied jason add rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr\r\n");

		}
		else if(!strcmp(message->topic,cfawacp->MQTT_Topic[RespondToken]))
		{
			cfawacp->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);

			printf( "sync time state: recevied jason add sssssssssssssssssssssssssssssssss\r\n");

		}

	}
	if(NULL != strtohex)
	{
		free(strtohex);
		strtohex = NULL;
	}

}

void CFAWACP::mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int mid)
{
	char log_str[128] = {0};
	FAWACPLOG("$$$$$$$$$$$$$$$$$$$$$$$%d\n\n\n", mid);
	sprintf(log_str, "mqtt disconnect");
	if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
	{
		WRITELOGFILE(MQTT_LOG_INFO, log_str);
	}
	cfawacp->TspWorkMode = TSP_WORK_MODE_SOCKET_DISCONNECT;			
}

void CFAWACP::mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	char log_str[128] = {0};
	sprintf(log_str, "%s%d", "mqtt connect cb result: ", result);
	if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
	{
		WRITELOGFILE(MQTT_LOG_INFO, log_str);
	}
	if(!result)
	{
		/* Subscribe to broker information topics on successful connect. */
		printf("\n****************************************************\n");
		mosquitto_subscribe(mosq,NULL,cfawacp->MQTT_Topic[RemoteCtrlTP],1); /*subscribe remote control cmd */
		mosquitto_subscribe(mosq,NULL,cfawacp->MQTT_Topic[RemoteUpgradeTP],1);
		//mosquitto_subscribe(mosq,NULL,cfawacp->MQTT_Topic[CamreCtrl],1);
		mosquitto_subscribe(mosq,NULL,cfawacp->MQTT_Topic[SyncTime],1);
		mosquitto_subscribe(mosq,NULL,cfawacp->MQTT_Topic[RespondToken],1);
		printf("%s(%d): connect callback result = %d\n", __func__, __LINE__, result);
	}
	else
	{
		printf("%s(%d): connect callback result = %d\n", __func__, __LINE__, result);
	}
}

void CFAWACP::UpdataMQTTTopic(uint8_t * vin)
{
	memset(MQTT_Topic, sizeof(MQTT_Topic), '0');
	for(int i=0;i<MAXTOPIC;i++)
	{
		if((i<3) || (i==7) || (i==8) || (i==9) )
		{
			snprintf(MQTT_Topic[i], 20, "%d/%s",i,vin);
			MQTT_Topic[i][19] = '\0';		
			FAWACPLOG("%s\n",MQTT_Topic[i]);
		}
		else if((i==10) || (i==11) ||(i==12))
		{
			snprintf(MQTT_Topic[i], 21, "%d/%s",i,vin);
			MQTT_Topic[i][20] = '\0';		
			FAWACPLOG("%s\n",MQTT_Topic[i]);
		}
		else
		{
			snprintf(MQTT_Topic[i], 24, "%d/%d/%d/%s", i, FIRM_NUM1, FIRM_NUM2, vin);
			MQTT_Topic[i][23] = '\0';		
			FAWACPLOG("%s\n",MQTT_Topic[i]);
		}
	}
}

int CFAWACP::MQTTConnect()
{
	int ret;
	char log_str[128] = {0};
	mqtt_config.m_KEEP_ALIVE = 60;
	FAWACPLOG("host is %s  post is %d \n\n", mqtt_config.m_HOST, mqtt_config.m_PORTS);
	ret = mosquitto_connect(mosq, mqtt_config.m_HOST, mqtt_config.m_PORTS, mqtt_config.m_KEEP_ALIVE);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		m_ConnectedState = false;
		printf("%s(%d): mqtt_connect result: %s\n", __func__, __LINE__, mosquitto_strerror(ret));
	}
	else
	{
		m_ConnectedState = true;
		FAWACPLOG("mqtt success00000000000000000000\n\n");		
	}

	sprintf(log_str, "%s%s", "mqtt connect state: ", mosquitto_strerror(ret));
	if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
	{
		WRITELOGFILE(MQTT_LOG_INFO, log_str);
	}
	return 0;
}


uint16_t    CFAWACP::PackBLEPeriodicCarInfoData(uint8 * Pdatebuff,uint16 u16BuffLen)
{
	uint8_t u8MsgType = 0; 
	uint16_t u16dataLen = 0;

	if(Pdatebuff == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
		return -1;
	}
	if(TboxSleepStatus == TBOX_WORK_MODE_SLEEP)
	{
		u8MsgType = MESSAGE_TYPE_START + 9;
	}
	else
	{
		u8MsgType = MESSAGE_TYPE_START + 2;
	}
	
	u16dataLen = cfawacp->PackFAWACP_FrameData(Pdatebuff, u16BuffLen, ACPApp_VehicleCondUploadID,u8MsgType , AcpCryptFlag_IS);
	return u16dataLen;
}



uint16_t    CFAWACP::MQTTtimingReportingData(uint8_t MsgType, AcpAppID_E AppID)
{

	static int times = 0;
	uint16_t dataLen;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL){
		FAWACPLOG("malloc dataBuff error!");
		return -1;
	}
	memset(dataBuff, 0, MALLOC_DATA_LEN);

	dataLen = cfawacp->PackFAWACP_FrameData(dataBuff, MALLOC_DATA_LEN, AppID, MsgType, AcpCryptFlag_IS);


	if(MsgType == 2){	
		times++;
	}

#if 0
	if((AppID == 6) && (MsgType == 6))
	{
		printf("jason add test _______________________________++++++++++++++++++++++++ \r\n");

		for(int i = 0;i< dataLen;i++)
		{
			printf("%02x ",dataBuff[i]);
		}
	}
#endif
	pthread_mutex_lock(&mutex_Rbuf);
	cfawacp->m_RingBuff.WriteData(dataBuff, dataLen);
	pthread_mutex_unlock(&mutex_Rbuf);

	free(dataBuff);
	dataBuff = NULL;

	return 0;
}


uint16_t CFAWACP::MQTTReqBleTokenData(uint8_t MsgType, AcpAppID_E AppID)
{

	uint16_t dataLen;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
		return -1;
	}
	memset(dataBuff, 0, MALLOC_DATA_LEN);

	dataLen = cfawacp->PackFAWACP_FrameData(dataBuff, MALLOC_DATA_LEN, AppID, MsgType, AcpCryptFlag_IS);

#if 1
	for(int i = 0;i<dataLen;i++)
	{
		printf("0x%02x ",dataBuff[i]);
	}
#endif
	pthread_mutex_lock(&mutex_Rbuf);
	cfawacp->m_RingBuff.WriteData(dataBuff, dataLen);
	pthread_mutex_unlock(&mutex_Rbuf);

	free(dataBuff);
	dataBuff = NULL;

	return 0;
}

void *CFAWACP::MQTTSendThread(void *arg)
{
	pthread_detach(pthread_self());
	CFAWACP *pCFAWACP = (CFAWACP*)arg;

	int ret = 0, i = 0;
	static char connect_err = 0;
	int datalen = 0;
	int TotalLen = 0;
	uint8_t Temp = 0;
	uint8_t *PRead = NULL;
	uint8_t Buff[1024] = {0};
	char log_str[128] = {0};
	while(1)
	{
		memset(Buff, 0, sizeof(Buff));
		//		printf("%s(%d): m_ConnectedState: %d\n", __func__, __LINE__, pCFAWACP->m_ConnectedState);
		if( pCFAWACP->m_ConnectedState == true )
		{
			if(pCFAWACP->m_RingBuff.GetUsingDataLen() > 10)	//33
			{
				PRead = pCFAWACP->m_RingBuff.Get_PReadAddr();
				//printf("%s(%d): PRead[0] = %d\n", __func__, __LINE__, PRead[0]);
				if(PRead[0] == 0x7e)
				{
					datalen = pCFAWACP->m_RingBuff.Get_DataLen( 2 );
					//printf("\n%s(%d):!!!!!!!!!!!!!!!!!!!!!!%d\n\n",__func__, __LINE__, datalen);
					TotalLen = (datalen + 3);
					//if(pCFAWACP->m_RingBuff.GetUsingDataLen() >= TotalLen)
					{
						ret = pCFAWACP->m_RingBuff.ReadData(Buff, TotalLen);
						//printf("%s(%d): payload data:\n", __func__, __LINE__);
#if 0
						for(i =0;i<ret;i++)
						{
							printf("%02x ",Buff[i]);
						}
#endif
						FAWACPLOG("datalen: %d, App id: %d\n\n",ret,Buff[3] & 0x3f);
						if(ret == TotalLen)
						{
							switch (Buff[3] & 0x3f)
							{
								case ACPApp_VehicleCondUploadID:
									ret = mosquitto_publish(pCFAWACP->mosq,NULL,pCFAWACP->MQTT_Topic[VehicleUploadTP],TotalLen,Buff,0,0);
									FAWACPLOG("topic is %s\n",pCFAWACP->MQTT_Topic[VehicleUploadTP]);
									FAWACPLOG("mosquitto_publish ret = %d\n",ret);
									break;
								case ACPApp_RemoteCtrlID:
									ret = mosquitto_publish(pCFAWACP->mosq,NULL,pCFAWACP->MQTT_Topic[RemoteCtrlReplyID],TotalLen,Buff,0,0);
									FAWACPLOG("topic is %s\n",pCFAWACP->MQTT_Topic[RemoteCtrlReplyID]);
									FAWACPLOG("mosquitto_publish ret = %d\n",ret);
									FAWACPLOG("mqtt_config.m_TestOrNormal = %d\r\n",pCFAWACP->mqtt_config.m_TestOrNormal);

									break;
								case ACPApp_RemoteUpgradeID:
									ret = mosquitto_publish(pCFAWACP->mosq,NULL,pCFAWACP->MQTT_Topic[RemoteUpgradeReplyID],TotalLen,Buff,0,0);
									FAWACPLOG("topic is %s\n",pCFAWACP->MQTT_Topic[RemoteUpgradeReplyID]);
									FAWACPLOG("mosquitto_publish ret = %d\n",ret);
									break;
								case ACPApp_CameraRecordID:
									ret = mosquitto_publish(pCFAWACP->mosq,NULL,pCFAWACP->MQTT_Topic[CameraCtrlReply],TotalLen,Buff,0,0);
									FAWACPLOG("topic is %s\n",pCFAWACP->MQTT_Topic[CameraCtrlReply]);
									FAWACPLOG("mosquitto_publish ret = %d\n",ret);
									break;
								case ACPApp_SyncTimeRequestID:
									ret = mosquitto_publish(pCFAWACP->mosq,NULL,pCFAWACP->MQTT_Topic[SyncTimeRequest],TotalLen,Buff,0,0);
									FAWACPLOG("topic is %s\n",pCFAWACP->MQTT_Topic[SyncTimeRequest]);
									FAWACPLOG("mosquitto_publish ret = %d\n",ret);
									break;
								case ACPApp_ReqBleTokenID:
									ret = mosquitto_publish(pCFAWACP->mosq,NULL,pCFAWACP->MQTT_Topic[ReqBleToken],TotalLen,Buff,0,0);
									FAWACPLOG("topic is %s\n",pCFAWACP->MQTT_Topic[ReqBleToken]);
									FAWACPLOG("mosquitto_publish ret = %d\n",ret);
									break;
								default:
									break;
							}

							if(ret != 0)
							{
								connect_err++;
								if(connect_err >= 10)
								{
									sprintf(log_str, "%s%d(%d)", "publish state: ", ret, connect_err);
									if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
									{
										WRITELOGFILE(MQTT_LOG_INFO, log_str);	
									}
									connect_err = 0;
									pCFAWACP->TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REBOOT;
								}
							}
							else
							{
								connect_err = 0;
							}
							/*  */
#if 0
							pCFAWACP->m_mqtt_pb_ret = ret;
							pCFAWACP->m_mqtt_pb_year = (Buff[9]>>2);
#endif
						}
					}

				}
				else
				{
					pCFAWACP->m_RingBuff.ReadData(&Temp, 1);
				} 
			}
			else
			{
				sleep(1);
			}
		}
		else
		{
			pCFAWACP->m_RingBuff.Init_RingBuffer();
			sleep(1);
		}
	}
	pthread_exit(0);

}

#define  CHECK_VERSION_TIMER        	20

#define  CHECK_VERSION_COUNT       	5
void *CFAWACP::StartCheckOSVersionThread(void *arg)
{
	CFAWACP *pCFAWACP = (CFAWACP*)arg;
	pCFAWACP->CheckOSVersionThread();
}
#define 	BLE_TOKEN_REQ_TIMEOUT      	 			15  // 30 s
#define 	BLE_CONNECT_TIMEOUT      	 				30  // 30 s


void *CFAWACP::CheckOSVersionThread(void)
{
	int osid, VersionCmpCount = 0;
	char osversion[16] = {0};
	time_t timenow = 0;
	uint8_t  u8DownLoadCount = 0;

	uint16_t  u16BleAutoUpdateTime = 24;
	
	dataPool->getPara(BleAutoUpDateTime_ID, &u16BleAutoUpdateTime, sizeof(u16BleAutoUpdateTime));	
	sprintf(osversion, "%s\n", TBOX_4G_VERSION);
	while(1)
	{
		if(VersionCmpCount < 3)
		{
		 	VersionCmpCount++;
			if(version_cmp(osversion) != 0)   /*if version  info same,update failed, */
			{
				osid = get_osid_from_file(MQTT_VERSION_TEST);
				//save_osid_in_file(MQTT_VERSION_NORMAL, osid);
				txt_put(osid, osversion, osid);

			}
			else
			{
				osid = get_osid_from_file(MQTT_VERSION_NORMAL);
				save_osid_in_file(MQTT_VERSION_TEST, osid);
			}
		}
		
		if((m_u8TryDownLoadFlag) &&(u8DownLoadCount == 0))
		{
			u8DownLoadCount = 60;	/*1 minus*/
			//m_u8TryDownLoadCount++;
			if(DownLoadUpgradePackage())
			{
				m_u8TryDownLoadCount = 0;
				m_u8TryDownLoadFlag = 0;
				u8DownLoadCount = 0;
			}
		}
		else
		{
			if(u8DownLoadCount > 0) 
			{
				u8DownLoadCount--;
			}
		}
		
		if(UpDateBleTokenTime != 0)
		{
			time(&timenow);
			if((timenow - UpDateBleTokenTime) > (u16BleAutoUpdateTime *3600))
			{
				BleReqUpdateTokenFlag = 1;
				u8BleTokenReqTimeOut = 0;
				time(&UpDateBleTokenTime);

			}
		}
		if(m_LocalTimeSyncState)
		{
			if(m_bleUart->GetBleConnectStatus() == BLE_CONNECT_STATUS_CONNECTED)
			{

			}
			else
			{
				if(BleReqUpdateTokenFlag == 1)
				{
					if(u8BleTokenReqTimeOut == 0)
					{
						MQTTReqBleTokenData(1,ACPApp_ReqBleTokenID);
						u8BleTokenReqTimeOut = BLE_TOKEN_REQ_TIMEOUT;					
						printf("jason add  time out  token req \r\n");
					}
					if(u8BleTokenReqTimeOut > 0)
					{
						u8BleTokenReqTimeOut --;
					}
				}
			}
		}
	
		if(m_bleUart->m_u8BleUpdateConnectTimeout <= BLE_CONNECT_TIMEOUT)
		{
			m_bleUart->m_u8BleUpdateConnectTimeout++;
		}
		else
		{
			m_bleUart->m_u8BleUpdateConnectTimeout = 0;
			m_bleUart->SetBleConnectStatus(BLE_CONNECT_STATUS_DISCONNECT);
		}
		if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
		{
		   	BackUpSyncMQTTParaAndPemFile();
		}
		sleep(1);
		
	}

	pthread_exit(NULL);
}

void *CFAWACP::CameraThread(void *arg)
{
	pthread_detach(pthread_self());
	CFAWACP *pCFAWACP = (CFAWACP*)arg;
	FILE *fp;
	fp = NULL;
	system("ifconfig usb0 192.168.0.2");
	uint8_t *mqttBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(mqttBuff == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
	}
	memset(mqttBuff, 0, MALLOC_DATA_LEN);


	fp = popen("curl –connect-timeout 20 -m 20 \"http://192.168.0.1/cgi-bin/hisnet/client.cgi?-operation=register&-ip=192.168.0.2\"","r");
	fread(mqttBuff, sizeof(uint8_t), MALLOC_DATA_LEN, fp);

	FAWACPLOG("^^^^^^^%s\n\n\n",mqttBuff);
	if(strstr((char*)mqttBuff,"Success"))
	{
		printf("jason add camera register success \r\n");
	}
	else
	{
		printf("jason add camera register failed \r\n");
	}
	int ret = -1;
	int sockfd = -1;
	int fd = -1;
	char buf[1024] = {'0'};
	socklen_t addrlen;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	addrlen = sizeof(server_addr);
	memset(&server_addr,0,sizeof(server_addr));
	memset(&client_addr,0,sizeof(client_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(CAMERA_PORT);
	server_addr.sin_addr.s_addr = inet_addr(IP);
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("socket\n");
		return NULL;
	}


	ret = bind(sockfd,(struct sockaddr *)&server_addr,addrlen);
	if(ret < 0)
	{
		perror("bind\n");
		return NULL;
	}
	ret = listen(sockfd,user_max);
	if(ret < 0)
	{
		perror("listen\n");
		return NULL;
	}
	printf("listen is ok\n");




	static int count =0;
	while(1)
	{	
		fd = accept(sockfd,(struct sockaddr *)&client_addr,&addrlen);
		if(fd < 0)
		{
			perror("accept\n");
			return NULL;
		}
		printf("client is ok\n");
		FAWACPLOG("camera tcp event\n\n\n\n");
#if 0
		count ++;
		if(count == 5)
		{
			FAWACPLOG("usb check event\n");
			int i;
			int j;
			fp = popen("ifconfig -a","r");
			fread(mqttBuff, sizeof(uint8_t), MALLOC_DATA_LEN, fp);
			FAWACPLOG("^^^^^^^%s\n\n\n",mqttBuff);

			for(i =1,j=0;i>0;i)
			{
				if(strstr((char*)mqttBuff,"usb0"))
				{
					FAWACPLOG("usb check 11\n");
					FAWACPLOG("usb0 is OK\n");
					j = 1;
					if(strstr((char*)mqttBuff,"192.168.0.2"))
					{
						FAWACPLOG("usb check ok\n");
						j = 2;
						break;
					}
					continue;


				}
				else
				{

					memset(mqttBuff, 0, MALLOC_DATA_LEN);
					i = fread(mqttBuff, sizeof(uint8_t), MALLOC_DATA_LEN, fp);
					FAWACPLOG("^^^^^^^%s\n\n\n",mqttBuff);
				}
			}
			if(j == 1)
			{
				system("ifconfig usb0 192.168.0.2");
				sleep(1);
				fp = popen("curl \"http://192.168.0.1/cgi-bin/hisnet/client.cgi?-operation=register&-ip=192.168.0.2\"","r");
			}

			count = 0;	
		}
#endif


		ret = recv(fd,buf,sizeof(buf),0);
		if(ret > 0)
		{
			//printf("buf = %s\n",buf);
			for(int i =0;i<ret;i++)
			{
				//printf(" %d is %c  %02x \n",i,buf[i],buf[i]);
			}
			if(strstr((char*)buf,"GSENSORMNG_COLLISION"))
			{
				FAWACPLOG("gsensor event\n");
				pCFAWACP->Unpack_Gsensor_Data(buf,ret);
			}
			memset(buf,0,1024);
		}
		usleep(500*1000);

	}
	close(fd);
	fd = -1;
	pclose(fp);
	if(NULL != mqttBuff)
	{
		free(mqttBuff);
		mqttBuff = NULL;
	}
	pthread_exit(0);

}


int CFAWACP::readMQTTParameterInfo()
{
	int fd;
	int retval;

	fd = open(MQTT_PARAMETER_INFO_PATH, O_RDONLY, 0666);
	if(fd < 0)
	{
		FAWACPLOG("File does not exist!\n");
		return -1;
	}
	else
	{
		FAWACPLOG("File Open success\n");
		retval = read(fd, &mqtt_config, sizeof(MQTT_Config));
		if(retval > 0)
		{
			FAWACPLOG("retval:%d\n",retval);
		}

		close(fd);

	}

	return 0;
}


int CFAWACP::initMQTTParameterInfo()
{
	int fd;
	FAWACPLOG("Init MQTT para!!!!!!!!!!!!!!!!!!");
	fd = open(MQTT_PARAMETER_INFO_PATH, O_RDWR | O_CREAT|O_TRUNC, 0666);
	if (-1 == fd)
	{
		FAWACPLOG("MQTT init open file failed\n");
		return -1;
	}


	if (-1 == write(fd, &mqtt_config, sizeof(MQTT_Config)))
	{
		FAWACPLOG("write file error!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int CFAWACP::updateMQTTParameterInfo()
{
	int fd;

	fd = open(MQTT_PARAMETER_INFO_PATH, O_RDWR, 0666);
	if(-1 == fd)
	{
		FAWACPLOG("MQTT update open file failed\n");
		return -1;
	}

	if(-1 == write(fd, &mqtt_config, sizeof(MQTT_Config)))
	{
		FAWACPLOG("write file error!\n");
		return -1;
	}

	close(fd);

	return 0;
}
void CFAWACP::BackUpSyncMQTTParaAndPemFile()
{
	int fd;
	int retval;

	if(access(PEM_FILE_NAME,F_OK) == 0) /*have valid file*/
	{
		if(access(BACKUP_PEM_FILE_NAME,F_OK) != 0) /*don't have backup file*/
		{
			system("cp -rf /data/111.pem  /cache/111.pem");
		}
	}
	else
	{
		if(access(BACKUP_PEM_FILE_NAME,F_OK) == 0) /*don't have backup file*/
		{
			system("cp -rf   /cache/111.pem  /data/111.pem");
		}

	}

	if(access(MQTTPARA_FILE,F_OK) == 0) /*have valid file*/
	{
		if(access(BACKUP_MQTTPARA_FILE,F_OK) != 0) /*don't have backup file*/
		{
			system("cp -rf /data/MQTTPara  /cache/MQTTPara");
			system("cp -rf /data/tbox  /cache/backupTbox");

		}
	}
	else
	{
		if(access(BACKUP_MQTTPARA_FILE,F_OK) == 0) /*don't have backup file*/
		{
			system("cp -rf   /cache/MQTTPara  /data/MQTTPara");
		}

	}
}
void CFAWACP::RUN_FAWACP_TASK()
{

	uint8_t u8AcpReportTime;
	uint8_t u8WaitTspLoginTime;
	uint8_t u8WaitNewIPTime;
	uint8_t u8SendCount;
	m_CMcu = CMcu::GetInstance();
	m_bleUart = bleUart::Get_BleInstance();
	RegisterMcu_CallbackFun();

	int ret = 0;
	bool testcamera = false;
	static int count =0;
	//FILE *usbfp;
	time_t timenow = 0;
	char mqttBuff[MALLOC_DATA_LEN];
	int car_report = 0;
	//m_CMcu->packProtocolData(0x82, 0x53, "TEST0000000000010", 1, 0);
#if 0
	{
		json_object *jsonobj = NULL;
		json_object *tmpjson = NULL;
		json_object *datajson = NULL;
		jsonobj = json_object_from_file("/data/upgrade.txt");
		json_object_object_get_ex(jsonobj , "os" , &tmpjson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));
		json_object_object_get_ex(tmpjson , "version" , &datajson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));
		json_object_object_get_ex(tmpjson , "md5" , &datajson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));

		json_object_object_get_ex(jsonobj , "mcu" , &tmpjson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));
		json_object_object_get_ex(tmpjson , "version" , &datajson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));
		json_object_object_get_ex(tmpjson , "md5" , &datajson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));

		json_object_object_get_ex(jsonobj , "ble" , &tmpjson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));
		json_object_object_get_ex(tmpjson , "version" , &datajson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));
		json_object_object_get_ex(tmpjson , "md5" , &datajson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));
	}
#endif
	if(pthread_create(&SendThreadId, NULL, MQTTSendThread, this) != 0)
		FAWACPLOG("Cannot creat SendThread:%s\n", strerror(errno));
	if(pthread_create(&CheckOSVersionId, NULL, StartCheckOSVersionThread, this) != 0)
		FAWACPLOG("Cannot creat SendThread:%s\n", strerror(errno));

	while(1)
	{
		switch(TspWorkMode)
		{
			case TSP_WORK_MODE_WAIT_MCU_FINISH:

				FAWACPLOG("jason add faw 9999999999999999999999999999\r\n");

				if(m_CMcu->flagGetTboxCfgInfo)
				{
					TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REDATACALL;
				}
				break;
			case TSP_WORK_MODE_SOCKET_DISCONNECT:			

				printf("jason add faw 111111111111111111111111111111\r\n");
				disconnectSocket();
				TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REDATACALL;

			case TSP_WORK_MODE_REQ_MODEM_REDATACALL:

				FAWACPLOG("jason add faw 222222222222222222222222222222222\r\n");
				if(GetTspReDatacallFlag() == 1)
				{
					if(u8WaitNewIPTime++ < 6)
						break;
				}
				TspWorkMode = TSP_WORK_MODE_WAIT_MODEM_FINISH;
				break; 
			case TSP_WORK_MODE_WAIT_MODEM_FINISH :

				FAWACPLOG("jason add faw 333333333333333333333333333333\r\n");
				if(GetModemWorkOn()/*GetTspNewIpFlag() == 1*/)
				{
					TspWorkMode = TSP_WORK_MODE_FIRST_LOGIN;
					ConnectFaultTimes = 0;

					FAWACPLOG("jason add faw 44444444444444444444444444444\r\n");
					break;
				}
				else
				{
					u8WaitNewIPTime++;
					if(u8WaitNewIPTime > 160) /*80s time out */ 
					{
						TspWorkMode = TSP_WORK_MODE_FIRST_LOGIN;

						ConnectFaultTimes = 0;

						FAWACPLOG("jason add faw 55555555555555555555555555\r\n");
						break;
					}
					else
					{						
						break;
					}
				}				
			case TSP_WORK_MODE_REQ_MODEM_REBOOT:
				if(m_bleUart->GetBleConnectStatus() == BLE_CONNECT_STATUS_CONNECTED)
				{
					break;
				}
				if(u8WaitNewIPTime++ < 10)/**/
				{
					break;
				}

				FAWACPLOG("jason add faw reboot state ^^^^^^^^^^^^^^^^^^^^^^^^\r\n");
				u8SendCount = 2;
				do{
					if(sendATCmd((char*)"at+cfun=0", (char*)"OK", NULL, 0, 5000) <=0 )
					{
						FAWACPLOG("jason add error send +cfun=0 cmd failed XXXXXXXXXXXXXXX\r\n");
					}
					else
					{
						FAWACPLOG("jason add send +cfun=0 cmd success \r\n");
						break;
					}
					sleep(5);
					u8SendCount--;
				}while(u8SendCount > 0);
				modem_ri_notify_mcu_high();
				sleep(1);
				modem_ri_notify_mcu_low();
				sleep(1);
				u8SendCount = 2;
				do{
					if(sendATCmd((char*)"at+cfun=1", (char*)"OK", NULL, 0, 5000) <=0 )
					{
						FAWACPLOG("jason add error send +cfun=1 cmd failed XXXXXXXXXXXXXXX\r\n");
					}
					else
					{
						FAWACPLOG("jason add send +cfun=1 cmd success \r\n");
						break;
					}
					sleep(5);
					u8SendCount--;
				}while(u8SendCount > 0);				
				sleep(1);
				//system( RM_QMUX_SOCKET );
#if 0
				mosquitto_destroy(mosq);
				mosquitto_lib_cleanup();
#endif
				system("sys_reboot");
				break;
			case TSP_WORK_MODE_FIRST_LOGIN:
			    m_LocalTimeSyncState = 0;
				/* */
				if(readMQTTParameterInfo()== -1)
				{
					//wait vin
					uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
					if(dataBuff == NULL)
					{
						FAWACPLOG("malloc dataBuff error!");
					}
					uint8_t *mqttBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
					if(mqttBuff == NULL)
					{
						FAWACPLOG("malloc dataBuff error!");
					}
					uint8_t *vinBuff = (uint8_t *)malloc(20);
					if(vinBuff == NULL)
					{
						FAWACPLOG("malloc dataBuff error!");
					}
					memset(dataBuff, 0, MALLOC_DATA_LEN);
					memset(vinBuff, 0, 20);
					memset(mqttBuff, 0, MALLOC_DATA_LEN);
					memset(&mqtt_config,sizeof(mqtt_config),0);
					dataPool->getTboxConfigInfo(VinID, vinBuff, 17);
					vinBuff[17] = '\0';
					strcpy(mqtt_config.m_USERNAME,vinBuff);
					FAWACPLOG("^^^^^^vin%s\n",mqtt_config.m_USERNAME);
#if TEST_ENV
					sprintf(dataBuff,"%s%s%s%s%s","curl -k --cert ", "/data/111.pem"," -X POST --header 'Content-Type: application/json' --header 'Accept: application/json' 'https://beta-devicegateway-init.i-morefun.com:6443/api/tboxInitialization?sn=", vinBuff,"'");
#else
					sprintf(dataBuff,"%s%s%s%s%s","curl -k --cert ", PEM_FILE_NAME," -X POST --header 'Content-Type: application/json' --header 'Accept: application/json' 'https://device-init.i-morefun.com/api/tboxInitialization?sn=", vinBuff,"'");
#endif
					//sprintf(dataBuff,"%s%s%s%s%s","curl -k --cert ",PEM_FILE_NAME," -X POST --header 'Content-Type: application/json' --header 'Accept: application/json' 'https://111.200.239.208:18840/api/tboxInitialization?sn=", vinBuff,"'");
					FAWACPLOG("^^^^^^curl %s\n",dataBuff);
#if 0
					char * mqtturl = "curl -k --cert 111.pem -X POST --header 'Content-Type: application/json' --header 'Accept: application/json' 'https://111.200.239.208:18840/api/tboxInitialization?sn=TBOX03181201A0456'";
#endif


					FILE *fp;
					fp = popen(dataBuff,"r");
					fread(mqttBuff, sizeof(uint8_t), MALLOC_DATA_LEN, fp);

					FAWACPLOG("^^^^^^^%s\n\n\n",mqttBuff);
					if(strstr((char*)mqttBuff,"ic0001"))
					{
						FAWACPLOG("&&&&&&&&&&\n\n");
#if 0
						FILE *fpp = NULL;
						fpp = fopen("/data/jsontest.txt", "w+");	
						fputs(mqttBuff, fpp);
						fclose(fpp);

#endif
						json_object *jsonobj = NULL;
						json_object *tmpjson = NULL;
						json_object *datajson = NULL;
						jsonobj = json_tokener_parse(mqttBuff);
						json_object_object_get_ex(jsonobj , "data" , &tmpjson);
						FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));


						json_object_object_get_ex(tmpjson , "gwAddress" , &datajson);
						FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));

						strcpy(mqtt_config.m_HOST , json_object_get_string(datajson));
						FAWACPLOG("*********%s\n",mqtt_config.m_HOST);
						//char * test = mqtt_config.m_HOST;
						char *ptr = strchr((char *)mqtt_config.m_HOST,':');
						FAWACPLOG("*********%s\n",ptr);
						if (ptr != NULL)
						{
							*ptr = '\0';
							mqtt_config.m_PORTS = atoi(++ptr);
							printf("*********%s  %d\n",mqtt_config.m_HOST,mqtt_config.m_PORTS);

						}

						json_object_object_get_ex(tmpjson , "uReportFrequency" , &datajson);
						mqtt_config.m_uReportFrequency = atoi(json_object_get_string(datajson));
						FAWACPLOG("*********%d\n",mqtt_config.m_uReportFrequency);


						json_object_object_get_ex(tmpjson , "pskId" , &datajson);
						strcpy(mqtt_config.m_PSK_ID , json_object_get_string(datajson));
						FAWACPLOG("*********%s\n",mqtt_config.m_PSK_ID);

						json_object_object_get_ex(tmpjson , "carType" , &datajson);
						strcpy(mqtt_config.m_Car_Type , json_object_get_string(datajson));
						FAWACPLOG("*********%s\n",mqtt_config.m_Car_Type);

						json_object_object_get_ex(tmpjson , "pskString" , &datajson);
						strcpy(mqtt_config.m_strPSK , json_object_get_string(datajson));
						FAWACPLOG("*********%s\n",mqtt_config.m_strPSK);

						json_object_object_get_ex(tmpjson , "alarmThreshold" , &datajson);
						mqtt_config.m_Alarm = atoi(json_object_get_string(datajson));
						FAWACPLOG("*********%d\n",mqtt_config.m_Alarm);

						json_object_object_get_ex(tmpjson , "reportFrequency" , &datajson);
						mqtt_config.m_reportFrequency = atoi(json_object_get_string(datajson));
						FAWACPLOG("*********%d\n",mqtt_config.m_reportFrequency);

						json_object_object_get_ex(tmpjson , "logSwitch" , &datajson);
						mqtt_config.m_LogSwitch = json_object_get_int(datajson);
						FAWACPLOG("*********%d\n",mqtt_config.m_LogSwitch);

						json_object_object_get_ex(tmpjson , "snPassword" , &datajson);
						strcpy(mqtt_config.m_PASSWARD , json_object_get_string(datajson));
						FAWACPLOG("*********%s\n",mqtt_config.m_PASSWARD);

						{
							char szTmp[3] = {0};
							for( int i = 0; i < strlen(mqtt_config.m_strPSK); i++ )  
							{  
								sprintf( szTmp, "%02X", (unsigned char) mqtt_config.m_strPSK[i] );  
								memcpy( &mqtt_config.m_hexPSK[i * 2], szTmp, 2 );  
							}
							FAWACPLOG("$$$$$$$$$%d %s\n",strlen(mqtt_config.m_hexPSK),mqtt_config.m_hexPSK);			
						}
						initMQTTParameterInfo();
						mqtt_config.m_TestOrNormal = 1;
						system("sync");
						//json_object_put(jsonobj);
						json_object_put(tmpjson);
						//json_object_put(datajson);
					}
					else
					{
						FAWACPLOG("###########\n\n");
					}
					pclose(fp);
					if(NULL != dataBuff)
					{
						free(dataBuff);
						dataBuff = NULL;
					}
					if(NULL != mqttBuff)
					{
						free(mqttBuff);
						mqttBuff = NULL;
					}
					if(NULL != vinBuff)
					{
						free(vinBuff);
						vinBuff = NULL;
					}

				}
				else
				{
					TspWorkMode = TSP_WORK_MODE_MQTT_INIT;
#if 0 ///jason add test
					strcpy(mqtt_config.m_HOST,"124.193.128.204");
					mqtt_config.m_HOST[17]= 0;
					mqtt_config.m_PORTS = 60000;
#endif
					printf("@@@@@@@@@@@@@%s %d %d %s %s %s %s %d %d %d %s",mqtt_config.m_HOST,mqtt_config.m_PORTS,mqtt_config.m_uReportFrequency,mqtt_config.m_PSK_ID,mqtt_config.m_Car_Type,mqtt_config.m_strPSK,mqtt_config.m_hexPSK,mqtt_config.m_Alarm,mqtt_config.m_reportFrequency,mqtt_config.m_LogSwitch,mqtt_config.m_PASSWARD);
				}



#if 0
				memset(&mqtt_config,sizeof(mqtt_config),0);
				strcpy(mqtt_config.m_HOST,"111.200.239.208");
				mqtt_config.m_PORTS = 18836;
				strcpy(mqtt_config.m_PSK, "123456");
				strcpy(mqtt_config.m_PSK_ID,"yyhhuunnmmtt");
				strcpy(mqtt_config.m_USERNAME ,"guo");
				strcpy(mqtt_config.m_PASSWARD ,"123456");
				mqtt_config.m_KEEP_ALIVE = 60;
#endif
				break;

			case TSP_WORK_MODE_MQTT_INIT:


				InitAcpTBoxAuthData();
				if(readTBoxParameterInfo() == -1){
					initTBoxParameterInfo();
				}

#if 0 
				p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval = mqtt_config.m_uReportFrequency;
				p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = mqtt_config.m_ReportFrequency;
				p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery = mqtt_config.m_Alarm;
				updateTBoxParameterInfo();
#endif

				if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval <=9)
				{
					p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval = 10;
					if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
					{
						updateTBoxParameterInfo();
					}
				}
				if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval<=9)
				{
					p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = 60;
					if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
					{
						updateTBoxParameterInfo();
					}
				}
				if(tboxInfo.operateionStatus.isGoToSleep == 0) /*sleep on*/
				{
					u8AcpReportTime = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval;
				}
				else //if(tboxInfo.operateionStatus.isGoToSleep == 1 && m_CMcu->AccStatus== STATUS_ACC_OFF)
				{/*work on*/
					u8AcpReportTime = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval;
					//p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval = 20;
				}

				u8WaitNewIPTime = 0 ;
				dataPool->getTboxConfigInfo(VinID, p_FAWACPInfo_Handle->Vin, 17);
				FAWACPLOG("tbox vin is %s\n",p_FAWACPInfo_Handle->Vin);
				UpdataMQTTTopic(p_FAWACPInfo_Handle->Vin);
				memcpy(mqtt_config.m_USERNAME,p_FAWACPInfo_Handle->Vin,17);
				mqtt_config.m_USERNAME[17] = '\0';

				mosquitto_lib_init();
				mosq = mosquitto_new(mqtt_config.m_USERNAME,session,NULL);/*jason change for sn client id */
				if(!mosq)
				{
					FAWACPLOG("create client failed..\n");
					mosquitto_lib_cleanup();
					break;
				}

				mosquitto_connect_callback_set(mosq, mqtt_connect_callback);
				mosquitto_message_callback_set(mosq, mqtt_message_callback);
				mosquitto_disconnect_callback_set(mosq, mqtt_disconnect_callback);

#if 0
				strcpy(mqtt_config.m_HOST,"111.200.239.208");
				mqtt_config.m_PORTS = 18836;
				strcpy(mqtt_config.m_PSK, "vvb46dQKvZK1f451AjO1");
				strcpy(mqtt_config.m_PSK_ID,"BiIPwvW02rfHNPdIGbxI");
				strcpy(mqtt_config.m_USERNAME ,"guo");
				strcpy(mqtt_config.m_PASSWARD ,"123456");
				mqtt_config.m_KEEP_ALIVE = 60;
#endif


				FAWACPLOG("$$$$$$$$$$$$$$$$$%s %d %d %s %s %s %s %d %d %d %s %s",mqtt_config.m_HOST,mqtt_config.m_PORTS,mqtt_config.m_uReportFrequency,mqtt_config.m_PSK_ID,mqtt_config.m_Car_Type,mqtt_config.m_strPSK,mqtt_config.m_hexPSK,mqtt_config.m_Alarm,mqtt_config.m_reportFrequency,mqtt_config.m_LogSwitch,mqtt_config.m_USERNAME,mqtt_config.m_PASSWARD);


				ret = mosquitto_username_pw_set(mosq,mqtt_config.m_USERNAME,mqtt_config.m_PASSWARD);
				//ret = mosquitto_username_pw_set(mosq,mqtt_config.m_USERNAME,"Q1iNaDlq");
				FAWACPLOG("mosquitto_username_pw_set ret =%d\n",ret);


				ret = mosquitto_tls_psk_set(mosq, mqtt_config.m_hexPSK, mqtt_config.m_PSK_ID, NULL);
				//ret = mosquitto_tls_psk_set(mosq,"4F616C5970477872743269484E4E535570657246","ApeQwLv0lKIjkk1xw60p ",NULL);
				FAWACPLOG("mosquitto_tls_psk_set ret = %d\n",ret);

				FAWACPLOG("IIIIIIIIIIIIIIIIIIIIIIIIIIIIIII");
				TspWorkMode = TSP_WORK_MODE_SOCKET_CONNECT;

				break;


			case TSP_WORK_MODE_SOCKET_CONNECT :		
				FAWACPLOG("jason add faw 6666666666666666666666666666666666\r\n");
				MQTTConnect();
				if(m_ConnectedState == false)
				{
					if(ConnectFaultTimes > 3)
					{
						ConnectFaultTimes = 0;
						//SetTspReDatacallFlag(1);
						TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REDATACALL;

						FAWACPLOG("jason add faw 777777777777777777777777777777\r\n");
						break;
					}
					else
					{
						SocketReconnectCount ++;
						if(SocketReconnectCount>=30)
							TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REBOOT;
						ConnectFaultTimes++;
						break;
					}

				}
				else
				{
					TspWorkMode = TSP_WORK_MODE_PERIOD_REPORT;
					m_LocalTimeSyncState = 0;
					m_TimeSyncHeartReceiveFlag = 0;
					FAWACPLOG("jason add faw 888888888888888888888888888888888\r\n");
					ConnectFaultTimes = 0;
					int loop;
					loop = mosquitto_loop_start(mosq);
					//loop = mosquitto_loop_forever(mosq, -1, 1);
					FAWACPLOG("mosquitto_loop_start = %d\r\n",loop);
					if(loop != MOSQ_ERR_SUCCESS)
					{
						FAWACPLOG("mosquitto loop error\r\n");
						//TspWorkMode=TSP_WORK_MODE_SOCKET_CONNECT;
						//break;
					}
					break;
				}

			case TSP_WORK_MODE_PERIOD_REPORT:
			default:
				//OutLoginFailTimes = 0;
				time(&timelog.time4g);
				//time(&NowHighReportTime);
				time(&NowLowReportTime);
				if(NowLowReportTime < LowReportTime)
				{
					time(&LowReportTime);
					time(&UpgradeTime);
				}
#if OTA_UPGRADE
				if((m_LocalTimeSyncState == 1) && (upgradeflag == true))
				{
					time(&UpdateSelfCheckTime);
					upgradeflag = false;
					MQTTtimingReportingData(UpgradeStep_6, ACPApp_RemoteUpgradeID);
					sleep(1);
					MQTTtimingReportingData(UpgradeStep_2, ACPApp_RemoteUpgradeID);
				}
				if(NowLowReportTime - UpdateSelfCheckTime > 86400)
				{
					FAWACPLOG("24h update\n");
					//printf("%s(%d) CuORLa = %d\n\n", __func__, __LINE__, mqtt_config.CuORLa);
					if(m_LocalTimeSyncState == 1)//连接上了且同步过时间，才上报车况
					{
						MQTTtimingReportingData(2, ACPApp_RemoteUpgradeID);
					}
					time(&UpdateSelfCheckTime);

				}
#endif
				//if(m_LocalTimeSyncState != 1)
				{
					if(NowLowReportTime - timenow >= TSP_MQTT_SYNC_INTERVAL_TIME)
					{
						time(&timenow);
						MQTTtimingReportingData(MESSAGE_TYPE_START, ACPApp_SyncTimeRequestID);
						if(m_TimeSyncHeartReceiveFlag == 0)
						{
							m_LocalTimeSyncCount ++;
							if(m_LocalTimeSyncCount >= TSP_MQTT_SYNC_REBOOT_COUNT)
							{
								m_LocalTimeSyncCount = 0;
								TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REBOOT;
								break;
							}
						}
						if(m_TimeSyncHeartReceiveFlag != 0)
						{
							m_LocalTimeSyncCount = 0;
							m_TimeSyncHeartReceiveFlag = 0;
						}
					}
				}
			
				if(TboxSleepStatus == TBOX_WORK_MODE_SLEEP)
				{
					if(tboxInfo.operateionStatus.isGoToSleep != TboxSleepStatus) /*wake up event */
					{

						if(m_LocalTimeSyncState == 1)
						{
							MQTTtimingReportingData(MESSAGE_TYPE_START + 3, ACPApp_VehicleCondUploadID);
						}
						TboxSleepStatus = tboxInfo.operateionStatus.isGoToSleep;
					}
					else if(NowLowReportTime - LowReportTime >= u8AcpReportTime) /* sleep heart data*/
					{
						if(u8AcpReportTime != p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval)
						{
							u8AcpReportTime = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval;
						}

						//printf("\n%s(%d): localtimesync_state = %d\n", __func__, __LINE__, m_LocalTimeSyncState);
						printf("\n%s(%d): TboxSleepStatus %d  u8AcpReportTime %d\n", __func__, __LINE__, TboxSleepStatus, u8AcpReportTime);
						FAWACPLOG("jason add asp work on report i = %d TSPACK_WorkWakeup_State = %d TSPACK_WorkSleep_State =%d \r\n",u8AcpReportTime,TSPACK_WorkWakeup_State,TSPACK_WorkSleep_State);
						if(m_LocalTimeSyncState == 1)
						{
							MQTTtimingReportingData(MESSAGE_TYPE_START + 9, ACPApp_VehicleCondUploadID);
						}

						time(&LowReportTime);
						TSPACK_WorkWakeup_State = 0;
						TSPACK_WorkSleep_State++;
					}

				}
				else
				{
					if(tboxInfo.operateionStatus.isGoToSleep != TboxSleepStatus) /*sleep  event*/
					{
						if(m_LocalTimeSyncState == 1)//连接上了且同步过时间，才上报车况
						{
							MQTTtimingReportingData(MESSAGE_TYPE_START + 8, ACPApp_VehicleCondUploadID);
						}

						TboxSleepStatus = tboxInfo.operateionStatus.isGoToSleep;

					}
					else if(NowLowReportTime - LowReportTime >= u8AcpReportTime) /* normal data*/
					{
						if(CMcu::GetInstance()->AccStatus == STATUS_ACC_ON)
						{
							u8AcpReportTime = 10;
						}
						else
						{
							u8AcpReportTime = 20;

						}

						//cfawacp->MQTTtimingReportingData(1, ACPApp_CameraRecordID);

						//printf("\n%s(%d): localtimesync_state = %d\n", __func__, __LINE__, m_LocalTimeSyncState);
						printf("\n%s(%d): TboxSleepStatus %d  u8AcpReportTime %d\n", __func__, __LINE__, TboxSleepStatus, u8AcpReportTime);
						FAWACPLOG("jason add asp work on report time = %d TSPACK_WorkWakeup_State = %d TSPACK_WorkSleep_State =%d \r\n",u8AcpReportTime,TSPACK_WorkWakeup_State,TSPACK_WorkSleep_State);
						if(m_LocalTimeSyncState == 1)
						{
							MQTTtimingReportingData(MESSAGE_TYPE_START + 2, ACPApp_VehicleCondUploadID);
						}
						time(&LowReportTime);

						TSPACK_WorkSleep_State = 0;
						TSPACK_WorkWakeup_State++;
						FAWACPLOG("jason add asp work on report time = %d TSPACK_WorkWakeup_State = %d TSPACK_WorkSleep_State =%d \r\n",u8AcpReportTime,TSPACK_WorkWakeup_State,TSPACK_WorkSleep_State);

					}

				}
				if(NowLowReportTime - UpgradeTime >= 20) /* normal data*/
				{
					printf("ACC: %d\nPower: %d\nCurrentSpeed: %d\n", CMcu::GetInstance()->AccStatus, p_FAWACPInfo_Handle->VehicleCondData.PowerState, p_FAWACPInfo_Handle->VehicleCondData.CurrentSpeed);
					if((CMcu::GetInstance()->AccStatus != STATUS_ACC_ON) && (p_FAWACPInfo_Handle->VehicleCondData.PowerState == 0) && (p_FAWACPInfo_Handle->VehicleCondData.CurrentSpeed == 0))
					{
						car_report++;
						if(car_report > 4)
						{
							car_report = 0;
							AccOffUpgrade();
						}
					}
					else
					{
						car_report = 0;
					}
					time(&UpgradeTime);
				}
#if 0
				{
					//char *mqttBuff = (char *)malloc(MALLOC_DATA_LEN);

					/* if(mqttBuff == NULL)
					   {
					   FAWACPLOG("malloc dataBuff error!");
					   }*/
					memset(mqttBuff, 0, MALLOC_DATA_LEN);

					count ++;
					if(count == 100)
					{
						FAWACPLOG("usb check event\n");
						int i;
						int j;
						usbfp = popen("ifconfig -a","r");
						fread(mqttBuff, sizeof(uint8_t), MALLOC_DATA_LEN, usbfp);
						FAWACPLOG("^^^^^^^%s\n\n\n",mqttBuff);

						for(i =1,j=0;i>0;i)
						{
							char * test = mqttBuff;
							if(strstr(test,"usb0"))
							{
								FAWACPLOG("usb check 11\n");
								FAWACPLOG("usb0 is OK\n");
								j = 1;
								if(strstr(test,"192.168.0.2"))
								{
									FAWACPLOG("usb check ok\n");
									j = 2;
									break;
								}
								else
								{
									break;
								}

								//continue;


							}
							else
							{

								memset(mqttBuff, 0, MALLOC_DATA_LEN);
								i = fread(mqttBuff, sizeof(uint8_t), MALLOC_DATA_LEN, usbfp);
								FAWACPLOG("^^^^^^^%s\n\n\n",mqttBuff);
							}
						}
						if(j == 1)
						{
							FAWACPLOG("usb send ifconfig\n");
							system("ifconfig usb0 192.168.0.2");
							sleep(1);
							usbfp = popen("curl –connect-timeout 20 -m 20 \"http://192.168.0.1/cgi-bin/hisnet/client.cgi?-operation=register&-ip=192.168.0.2\"","r");
						}
						FAWACPLOG("usb check count is %d\n",count);
						pclose(usbfp);
						count = 0;	

						/*
						   if(NULL != mqttBuff)
						   {
						   free(mqttBuff);
						   mqttBuff = NULL;
						   }
						   */
					}
				}
#endif
				break;

		}

		usleep(500 * 1000);
	}

	pthread_exit(0);
}



CFAWACP::CFAWACP()
{
	m_Socketfd = -1;
	m_ConnectedState = false;
	m_Upgrade= false;

	m_loginState = STATUS_LOGGIN_INIT;

#ifndef MQTT
	TspWorkMode  = TSP_WORK_MODE_SOCKET_DISCONNECT;
#else
	TspWorkMode  = TSP_WORK_MODE_WAIT_MCU_FINISH;
#endif
	m_SendMode = 0;		
	m_bEnableSendDataFlag = true; 
	Headerlink = NULL;
	ConnectFaultTimes = 0;
	TboxSleepStatus = 1;
	InLoginFailTimes = 0;
	OutLoginFailTimes = 0;
	m_LocalTimeSyncState = 0;
	m_LocalTimeSyncCount = 0;
	m_TimeSyncHeartReceiveFlag = 0;
	HighReportTime = 0;
	LowReportTime = 0;
	UpgradeTime = 0;
	NowHighReportTime = 0;
	NowLowReportTime = 0;
	UpdateSelfCheckTime = 0;
	TimeCheckTime = 0;
	UpDateBleTokenTime = 0;
	TSPACK_WorkWakeup_State = 0;
	TSPACK_WorkSleep_State = 0;
	Req_McuRemoteCtrl = NULL;
	Req_McuRemoteCofig = NULL;
	m_u8TryDownLoadCount = 0;
	m_u8TryDownLoadFlag = 0;
	ftpupgrade = false;
	upgradeflag = true;
	BleReqUpdateTokenFlag = 1;
	u8BleTokenReqTimeOut = 0;
	flaftime4g = true;

	session = true;
	mosq = NULL;

	memset(&CameraGPSData,0,sizeof(CameraGPSData));

	IsCameraUpload = false;
}

CFAWACP::~CFAWACP()
{
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex_Rbuf);
	disconnectSocket();
	if(cfawacp != NULL)
	{
		delete cfawacp;
		cfawacp = NULL;
	}
}

#if 0   //��ʹ��
int CFAWACP::VehicleReissueSend()
{
	int id = 0;
	int dataLen = 0;
	m_SendMode = 1;
	unsigned char *databuff = NULL;
	databuff = (unsigned char *)malloc(DATA_BUFF_SIZE);
	if(NULL == databuff){
		printf("databuff malloc error\n");
		return -1;
	}
	id = pSqliteDatabase->queryMinisSendID();
	if(id == 0){
		m_SendMode = 0;
		free(databuff);
		databuff = NULL;
		return -1;
	}
	pSqliteDatabase->updataisSend(0, id);
	pSqliteDatabase->readData(databuff, &dataLen, id);
	m_CMcu->unpack_updatePositionInfo(databuff, dataLen);
	cfawacp->timingReportingData(MESSAGE_TYPE_START + 2, ACPApp_VehicleCondUploadID);
	m_SendMode = 0;
	if(NULL != databuff){
		free(databuff);
		databuff = NULL;
	}
	return 0;
}
#endif

#if 0
void *CFAWACP::SendThread(void *arg)
{
	pthread_detach(pthread_self());
	CFAWACP *pCFAWACP = (CFAWACP*)arg;

	int ret = 0;
	int datalen = 0;
	int TotalLen = 0;
	uint8_t Temp = 0;
	uint8_t *PRead = NULL;
	uint8_t Buff[1024] = {0};

	while(1)
	{
		memset(Buff, 0, sizeof(Buff));
		if( pCFAWACP->m_ConnectedState == true )
		{
			printf("!!!!!!!!!!!!!!!!!!!!!!");
#if 0
			printf("pCFAWACP->m_RingBuff.GetUsingDataLen() == %d\n", pCFAWACP->m_RingBuff.GetUsingDataLen());
#endif
			if(pCFAWACP->m_RingBuff.GetUsingDataLen() > FIXATION_DATA_LEN)	//33
			{
				PRead = pCFAWACP->m_RingBuff.Get_PReadAddr();
				//if( (PRead[0] == ACP_FIRST_SOF) && (PRead[1] == ACP_SECOND_SOF) && (PRead[2] == ACP_THIRD_SOF) && (PRead[3] == ACP_FOUR_SOF) )
				if(PRead[0] == 0x7e)
				{
					//datalen = ((PRead[27] << 8) | PRead[28]);
					datalen = pCFAWACP->m_RingBuff.Get_DataLen(2);
					printf("!!!!!!!!!!!!!!!!!!!!!!%d\n\n",datalen);
					TotalLen = (datalen + 3);
					//if(pCFAWACP->m_RingBuff.GetUsingDataLen() >= TotalLen)
					{
						ret = pCFAWACP->m_RingBuff.ReadData(Buff, TotalLen);
						for(int i =0;i<TotalLen;i++)
						{
							printf("%02x ",Buff[i]);
						}
						printf("!!!!!!!!!!!!!!!!!!!!!!%d\n\n",ret);
						if(ret == TotalLen)
						{
							ret = mosquitto_publish(pCFAWACP->mosq,NULL,pCFAWACP->MQTT_Topic[0],TotalLen,Buff,0,1);
							printf("mosquitto_publish ret = %d\n",ret);
							/*if(send(pCFAWACP->m_Socketfd, Buff, TotalLen, 0) < 0 )
							  {
							  SocketDisconnectCount++;
							  FAWACPLOG("Send data failed.dataLen:%d\n",datalen);
							  FAWACP_ERROR_Log(2, errno,SocketDisconnectCount);
							  }*/
						}
					}

				}
				else
				{
					pCFAWACP->m_RingBuff.ReadData(&Temp, 1);
				}
			}
			else
			{
				sleep(1);
			}
		}
		else
		{
			pCFAWACP->m_RingBuff.Init_RingBuffer();
			sleep(1);
		}
	}
	pthread_exit(0);

}
#endif
/***************************************************************************
 * Function Name: TimeOutThread
 * Function Introduction:
 *              ���1s��ѯ��������ڵ㣬�ж��Ƿ��г�ʱ�ڵ㣬
 ����¼��ʱ������ִ��ָ�������������ɾ����ʱ�ڵ�
Data:
 ****************************************************************************/
void *CFAWACP::TimeOutThread(void *arg)
{
	pthread_detach(pthread_self());
	CFAWACP *pCFAWACP = (CFAWACP*)arg;

	while(1)
	{	


		if(pCFAWACP->ConnectFaultTimes == 90)
		{
			FAWACP_ERROR_Log(6, errno,0);
			pCFAWACP->disconnectSocket();
			system( RM_QMUX_SOCKET );
			sleep(2);
			//	system("pkill -9 tbox");
			system("reboot");
		}

		sleep(1);
	}
	pthread_exit(0);
}



int CFAWACP::CheckTimer(LinkTimer_t *pb, uint8_t *flag)
{
	time_t NewTime = 0;
	time(&NewTime);
	switch(*flag)
	{
		case 0:
			if((NewTime - pb->time) >= 10)
			{
				*flag = 1;
				FAWACPLOG("ACP Application:%d First timeout\n",pb->AcpAppID);
				RespondTspPacket((AcpAppID_E)pb->AcpAppID, pb->MsgType, AcpCryptFlag_IS, pb->TspSoure);
				return 0;
			}
			break;
		case 1:
			if((NewTime - pb->time) >= 20)
			{
				*flag = 2;
				FAWACPLOG("ACP Application:%d second timeout\n",pb->AcpAppID);
				FAWACP_ERROR_Log(3, errno,0);
				m_loginState = STATUS_LOGGIN_INIT;
				m_ConnectedState = false;
				//Headerlink=NULL;
				return 0;
			}
			break;
		default:
			break;
	}
	return -1;
}

void CFAWACP::printf_link(LinkTimer_t **head)
{
	int ret = -1;
	LinkTimer_t *pb, *pf;
	pf = *head;
	FAWACPLOG("head == %p", *head);

	while(pf != NULL)
	{
		switch(pf->AcpAppID)
		{
			case 2:
				ret = CheckTimer(pf, &TimeOutType.RemoteCtrlFlag);
				break;
			case 3:
				ret = CheckTimer(pf, &TimeOutType.VehQueCondFlag);
				break;
			case 4:
				ret = CheckTimer(pf, &TimeOutType.RemoteCpnfigFlag);
				break;
			case 5:
				ret = CheckTimer(pf, &TimeOutType.UpdateRootkeyFlag);
				break;
			case 9:
				ret = CheckTimer(pf, &TimeOutType.VehGPSFlag);
				break;
			case 11:
				ret = CheckTimer(pf, &TimeOutType.RemoteDiagnoFlag);
				break;
			default:
				break;
		}

		if(ret == 0)
		{
			pthread_mutex_lock(&mutex);
			*head = pf->next;
			free(pf);
			pf = *head;
			pthread_mutex_unlock(&mutex);
			ret = -1;
		}
		else
		{
			while(pf->next != NULL)
			{
				pb = pf;
				pf = pf->next;
				switch(pf->AcpAppID)
				{
					case 2:
						ret = CheckTimer(pf, &TimeOutType.RemoteCtrlFlag);
						break;
					case 3:
						ret = CheckTimer(pf, &TimeOutType.VehQueCondFlag);
						break;
					case 4:
						ret = CheckTimer(pf, &TimeOutType.RemoteCpnfigFlag);
						break;
					case 5:
						ret = CheckTimer(pf, &TimeOutType.UpdateRootkeyFlag);
						break;
					case 9:
						ret = CheckTimer(pf, &TimeOutType.VehGPSFlag);
						break;
					case 11:
						ret = CheckTimer(pf, &TimeOutType.RemoteDiagnoFlag);
						break;
					default:
						break;
				}
				if(ret == 0)
				{
					pthread_mutex_lock(&mutex);
					pb->next = pf->next;
					free(pf);
					pthread_mutex_lock(&mutex);
					ret = -1;
				}
			}
			return ;
		}
	}
}


void CFAWACP::insert(LinkTimer_t **head,LinkTimer_t *p_new)
{
	LinkTimer_t *pb;
	pb = *head;

	if(*head == NULL)
	{
		*head = p_new;
		p_new->next = NULL;
		return ;
	}

	while(pb->next != NULL)
	{
		pb = pb->next;
	}

	pb->next = p_new;
	p_new->next = NULL;
}

LinkTimer_t *CFAWACP::search (LinkTimer_t *head,uint8_t AcpAppID)
{
	LinkTimer_t *pb;
	pb = head;

	if(head == NULL)
		return NULL;

	while((pb->AcpAppID!=AcpAppID) && (pb->next!=NULL ))
	{
		pb = pb->next;
	}

	if(pb->AcpAppID == AcpAppID)
	{
		return pb;
	}
	return NULL;
}

void CFAWACP::delete_link(LinkTimer_t **head,uint8_t AcpAppID)
{
	LinkTimer_t *pb, *pf;
	pb = *head;
	if(*head == NULL)
		return ;

	while((pb->AcpAppID != AcpAppID) && (pb->next != NULL ))
	{
		pf = pb;
		pb = pb->next;
	}

	if(pb->AcpAppID == AcpAppID)
	{
		if(pb == *head)
		{
			*head = pb->next;
		}
		else
		{
			pf->next = pb->next;
		}
		free(pb);
	}
}

int CFAWACP::socketConnect()
{
	int fd;
	int WaitIPTimeOut = 0;
#if 0
	char ServerIP[20];
	int  port;
	if(getServerIP(1, ServerIP, sizeof(ServerIP), NULL, "https://znwl-uat-exttj.faw.cn") == -1)  //(const char *)/*"https://znwl-uat-exttj.faw.cn"*/
	{
		FAWACPLOG("get ServerIP Fail\n");
		return -1;
	}
	port = (int )FAW_SERVER_PORT;
	FAWACPLOG("ServerIP:%s\n",ServerIP);

	if(getServerIP(0, ServerIP, sizeof(ServerIP), &port, NULL) == -1)
		return -1;
	FAWACPLOG("ServerIP: %s, port: %d\n", ServerIP, port);
#endif
#define DEBUG_TEST 1

#if DEBUG_TEST == 0
	//���ط���������
	memset(&m_socketaddr, 0, sizeof(m_socketaddr));
	m_socketaddr.sin_family = AF_INET;
	m_socketaddr.sin_port = htons(10003);
	m_socketaddr.sin_addr.s_addr = inet_addr("113.88.178.84");
#elif DEBUG_TEST == 1
	//�ȴ���ȡ����IP
	if(strlen(target_ip) == 0)
	{
		printf("jason get target_ip failed ererrererererererer\r\n");
		return -1;
	}
	memset(&m_socketaddr, 0, sizeof(m_socketaddr));
	m_socketaddr.sin_family = AF_INET;
	//m_socketaddr.sin_port = htons(FAW_SERVER_PORT);
	m_socketaddr.sin_port = htons(11000);
	m_socketaddr.sin_addr.s_addr = inet_addr(target_ip);
#endif
#if 0
	int ret_socketConnect = -1;
	FAWACPLOG("Connect to ACP server: server IP:%s,PORT:%d\n",inet_ntoa(m_socketaddr.sin_addr),ntohs(m_socketaddr.sin_port));
	if((m_Socketfd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
	{
		FAWACPLOG("ACP m_Socketfd:%d\n",m_Socketfd);
		ret_socketConnect = connect(m_Socketfd, (struct sockaddr*) &m_socketaddr, sizeof(m_socketaddr));
		FAWACPLOG("ACP ret_socketConnect:%d\n",ret_socketConnect);
		if (ret_socketConnect != -1)
		{
			APN2test = true;
			FAWACPLOG("Connect to server successfully,server IP:%s,PORT:%d\n",inet_ntoa(m_socketaddr.sin_addr),ntohs(m_socketaddr.sin_port));
			m_ConnectedState = true;
			//ConnectFaultTimes = 0;

			setsockopt(m_Socketfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
			setsockopt(m_Socketfd, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle));
			setsockopt(m_Socketfd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepinterval, sizeof(keepinterval));
			setsockopt(m_Socketfd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcount, sizeof(keepcount));

			FAWACP_ERROR_Log(8,0,0);

			//sendLoginAuthApplyMsg();
		}
		else
		{
			FAWACP_ERROR_Log(5, errno,0);
			m_ConnectedState = false;
			//m_loginState = STATUS_LOGGIN_INIT;
			FAWACPLOG("connect failed close m_Socketfd: %d\n",ret_socketConnect);
			return -1;
		}
	}
	else
	{
		printf("jason add socket create failed \r\n ");

	}
#endif

	if(mosquitto_connect(mosq, HOST, PORTS, KEEP_ALIVE))
	{
		fprintf(stderr, "Unable to connect.\n");
		m_ConnectedState = false;
		printf("mqtt failed00000000000000000000\n\n");

	}
	else
	{
		m_ConnectedState = true;
		printf("mqtt success00000000000000000000\n\n");		
	}


	return 0;
}
#if 0
//������������ip��ַ
int CFAWACP::getServerIP(int flag, char *ip, int ip_size, int *port, const char *domainName)
{
	FAWACPLOG("domainName = %s\n",domainName);
	if(flag == 0)
	{
		if((ip != NULL) && (port != NULL) && (ip_size >= 16))
		{
			uint8_t IP_Port[6];

			dataPool->getPara(SERVER_IP_PORT_ID, IP_Port, 6);

			memset(ip, 0, sizeof(ip));
			sprintf(ip, "%d.%d.%d.%d", IP_Port[0], IP_Port[1], IP_Port[2], IP_Port[3]);

			*port = (IP_Port[4] << 8) + IP_Port[5];
		}
	}
	else if(flag == 1)
	{
		if((ip != NULL) && (domainName != NULL) && (ip_size >= 16))
		{
			struct hostent *server = gethostbyname(domainName);
			if(server == NULL)
			{
				FAWACPLOG("get ip address fail\n");
			}
			else
			{
				FAWACPLOG("get ip address succees\n");
				strcpy(ip, inet_ntoa(*((struct in_addr *)server->h_addr_list)));
				FAWACPLOG("ServerIP:%s\n",ip);
			}
		}
	}
	else
	{
		return -1;
	}
}

#endif

int CFAWACP::disconnectSocket()
{
#ifndef MQTT
	if(m_Socketfd != -1)
	{
		shutdown(m_Socketfd, SHUT_RDWR);
		close(m_Socketfd);
		m_Socketfd = -1;
	}
	m_ConnectedState = false;
	m_loginState = STATUS_LOGGIN_INIT;
	TspWorkMode = TSP_WORK_MODE_SOCKET_DISCONNECT;
	FAWACPLOG("Close socket m_Socketfd=%d",m_Socketfd);
#else
#if MQTT_RECONNECT
	printf("mqtt disconnect!\n");
	mosquitto_loop_stop(mosq,1);
	printf("mqtt disconnect!\n");
	mosquitto_destroy(mosq);
	printf("mqtt disconnect!\n");
	mosquitto_lib_cleanup();	
	printf("mqtt disconnect!\n");
#endif
	m_ConnectedState = false;
	SetTspReDatacallFlag(1);
	//	TspWorkMode = TSP_WORK_MODE_SOCKET_DISCONNECT;
#endif
	return 0;
}

int CFAWACP::initTBoxParameterInfo()
{
	int fd;
	FAWACPLOG("Init TBox para!!!!!!!!!!!!!!!!!!");
	fd = open(ACPPARAMETER_INFO_PATH, O_RDWR | O_CREAT|O_TRUNC, 0666);
	if (-1 == fd)
	{
		FAWACPLOG("FAWACP init open file failed\n");
		return -1;
	}

	Init_FAWACP_dataInfo();

	if (-1 == write(fd, p_FAWACPInfo_Handle, sizeof(FAWACPInfo_Handle)))
	{
		FAWACPLOG("write file error!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int CFAWACP::readTBoxParameterInfo()
{
	int fd;
	int retval;

	fd = open(ACPPARAMETER_INFO_PATH, O_RDONLY, 0666);
	if(fd < 0)
	{
		FAWACPLOG("File does not exist!\n");
		return -1;
	}
	else
	{
		FAWACPLOG("File Open success\n");
		retval = read(fd, p_FAWACPInfo_Handle, sizeof(FAWACPInfo_Handle));
		if(retval > 0)
		{
			FAWACPLOG("retval:%d\n",retval);
		}

		close(fd);

	}

	return 0;
}

int CFAWACP::updateTBoxParameterInfo()
{
	int fd;

	fd = open(ACPPARAMETER_INFO_PATH, O_RDWR, 0666);
	if(-1 == fd)
	{
		FAWACPLOG("FAWACP update open file failed\n");
		return -1;
	}

	if(-1 == write(fd, p_FAWACPInfo_Handle, sizeof(FAWACPInfo_Handle)))
	{
		FAWACPLOG("write file error!\n");
		return -1;
	}

	close(fd);

	return 0;
}

/****************************************************************
 ****************************************************************/
void CFAWACP::Init_FAWACP_dataInfo()
{
	uint8_t VerTboxMCU[12] = {0};
	uint8_t VerTboxOS[12] = {0x0};
	uint8_t EmergedCall[20] = {0};
	uint8_t LoadCell[20] = {0};
	uint8_t InformationCell[20] = {0};
	uint8_t WhitelistCall[15] = {0};
	uint8_t VerIVI[16] = {0};

	//strcat(VerTboxOS, FILE_VERSION);
	//memset(&TimeOutType, 0, sizeof(TimeOutType_t));
	memset(p_FAWACPInfo_Handle->New_RootKey, 0, sizeof(p_FAWACPInfo_Handle->New_RootKey));
	struct tm *p_tm = NULL; 
	time_t tmp_time;
	tmp_time = time(NULL);
	p_tm = gmtime(&tmp_time);
	
	struct timeval Time;
	gettimeofday(&Time, NULL);

#if 1
	
	m_tspTimestamp.Year = p_tm->tm_year-90;
	m_tspTimestamp.Month = p_tm->tm_mon+1;
	m_tspTimestamp.Day = p_tm->tm_mday;
	m_tspTimestamp.Hour = p_tm->tm_hour;
	m_tspTimestamp.Minutes = p_tm->tm_min;
	m_tspTimestamp.Seconds = p_tm->tm_sec;
	m_tspTimestamp.msSeconds = Time.tv_usec / 1000; 
#endif

	p_FAWACPInfo_Handle->VehicleCondData.GPSData.degree = 0;	
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.second = p_tm->tm_sec;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.minute = p_tm->tm_min;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.hour = p_tm->tm_hour;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.day = p_tm->tm_mday;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.month = p_tm->tm_mon;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.year = p_tm->tm_year - 90;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.altitude = 0;	
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitudeState = 0x2;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude = 0;	
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitudeState = 0x2;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude = 0;
	p_FAWACPInfo_Handle->VehicleCondData.GPSData.GPSState = 0x0;

	p_FAWACPInfo_Handle->VehicleCondData.CarIDState = 3;	

	p_FAWACPInfo_Handle->VehicleCondData.RemainedOil.RemainedOilValue = 0x7F;
	p_FAWACPInfo_Handle->VehicleCondData.RemainedOil.RemainedOilGrade = 0;
	p_FAWACPInfo_Handle->VehicleCondData.Odometer = 0x00;
	p_FAWACPInfo_Handle->VehicleCondData.Battery = 0xFFFF;
	p_FAWACPInfo_Handle->VehicleCondData.CurrentSpeed = 0x7FFE;
	p_FAWACPInfo_Handle->VehicleCondData.LTAverageSpeed = 0x7FFE;
	p_FAWACPInfo_Handle->VehicleCondData.STAverageSpeed = 0x7FFE;
	p_FAWACPInfo_Handle->VehicleCondData.LTAverageOil = 0xFFFE;
	p_FAWACPInfo_Handle->VehicleCondData.STAverageOil = 0xFFFE;
	//
	p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.drivingDoor = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.copilotDoor = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.leftRearDoor = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.rightRearDoor = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.rearCanopy = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.engineCover = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarLockState.bitState.rightRearLock = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarLockState.bitState.leftRearLock = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarLockState.bitState.copilotLock = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.CarLockState.bitState.drivingLock = 0X0;
	p_FAWACPInfo_Handle->VehicleCondData.sunroofState = 0x0;

	p_FAWACPInfo_Handle->VehicleCondData.WindowState.bitState.leftFrontWindow = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.WindowState.bitState.rightFrontWindow = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.WindowState.bitState.leftRearWindow = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.WindowState.bitState.rightRearWindow = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.CarlampState.bitState.headlights = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.CarlampState.bitState.positionlights = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.CarlampState.bitState.nearlights = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.CarlampState.bitState.rearfoglights = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.CarlampState.bitState.frontfoglights = 0x0;

	p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightrearTyrePress = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftrearTyrePress = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightfrontTyrePress = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftfrontTyrePress = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightrearTemperature = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftrearTemperature = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightfrontTemperature = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftfrontTemperature = 0xFF;
	memcpy(p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU,VerTboxMCU,sizeof(VerTboxMCU));
	p_FAWACPInfo_Handle->VehicleCondData.EngineState = 0x0;
	
	p_FAWACPInfo_Handle->VehicleCondData.WheelState.bitState.wheeldegree = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.WheelState.bitState.wheeldirection = 0x0;
	
	for(uint8_t i = 0; i < 19; i++){
		p_FAWACPInfo_Handle->VehicleCondData.PastRecordSpeed[i] = 0x7FFF;
	}
	
	for(uint8_t i = 0; i < 19; i++){
		p_FAWACPInfo_Handle->VehicleCondData.PastRecordWheel[i].bitState.wheeldegree = 0x0;
		p_FAWACPInfo_Handle->VehicleCondData.PastRecordWheel[i].bitState.wheeldirection = 0x0;
	}
	p_FAWACPInfo_Handle->VehicleCondData.EngineSpeed = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.Gearstate = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.HandbrakeState = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.ParkingState = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.Powersupplymode = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.Safetybeltstate = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.RemainUpkeepMileage = 0x0;

	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.Temperature = 0X7E;
	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.blowingMode = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.blowingLevel = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.inOutCirculateState = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.defrostState = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.autoState = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.compressorState = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.airconditionerState = 0x0;
	 
	p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.overspeedTime = 0xFFFE;
	p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.wheelTime = 0xFE;
	p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.decelerateTime = 0xFE;
	p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.accelerateTime = 0xFFFE;

	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.BatAveraTempera = 0x3C;
	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.elecTempera = 0x3C;
	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.elecSOH = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.quantity = 0xFF;
	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.electricity = 0x1F40;
	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.voltage = 0x14A;

	p_FAWACPInfo_Handle->VehicleCondData.ChargeState = 0x0;
	//p_FAWACPInfo_Handle->VehicleCondData.ChargeState.remainChargeTime = 0x0;
	//p_FAWACPInfo_Handle->VehicleCondData.ChargeState.chargeMode = 0x0;	
	memcpy(p_FAWACPInfo_Handle->VehicleCondData.VerTboxOS,"MPU_V1.1.083",12);
	memcpy(p_FAWACPInfo_Handle->VehicleCondData.Tboxersion,"MPU_V1.1.08300",14);
	memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion,"000000000000",12);

	memcpy(p_FAWACPInfo_Handle->VehicleCondData.VerIVI, VerIVI, sizeof(VerIVI));
	p_FAWACPInfo_Handle->VehicleCondData.ChargeConnectState = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.BrakePedalSwitch = 0x0;	
	p_FAWACPInfo_Handle->VehicleCondData.AcceleraPedalSwitch = 0x0;	
	
	p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.TransverseAccele = 0xFFE;
	p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.LongituAccele = 0xFFE;
	p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.YawVelocity = 0xFFE;
	p_FAWACPInfo_Handle->VehicleCondData.AmbientTemperat.AmbientTemperat = 0x7FE;
	
	p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.MainPositRelayCoilState = 0;
	p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.MainNegaRelayCoilState = 0;
	p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.PrefillRelayCoilState = 0;
	p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.RechargePositRelayCoilState = 0;
	p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.RechargeNegaRelayCoilState = 0;
	p_FAWACPInfo_Handle->VehicleCondData.ResidualRange = 0;
	
	p_FAWACPInfo_Handle->VehicleCondData.NewEnergyHeatManage.BatteryHeatRequest = 0;
	p_FAWACPInfo_Handle->VehicleCondData.NewEnergyHeatManage.Motor1CoolRequest = 0;
	p_FAWACPInfo_Handle->VehicleCondData.NewEnergyHeatManage.Motor2CoolRequest = 0;
	
	p_FAWACPInfo_Handle->VehicleCondData.VehWorkMode.VehWorkMode = 0x0;
	
	p_FAWACPInfo_Handle->VehicleCondData.MotorWorkState.Motor1Workstate = 0x0;
	p_FAWACPInfo_Handle->VehicleCondData.MotorWorkState.Motor2Workstate = 0x0;
	
	p_FAWACPInfo_Handle->VehicleCondData.HighVoltageState = 0x0;
	
	
	p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Lock = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Window = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Sunroof = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_TrackingCar = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Lowbeam = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_LuggageCar = 0;
	
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Control.dataBit.dataState = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Control.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_CompressorSwitch.dataBit.dataState = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_CompressorSwitch.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Temperature.dataBit.dataState = 0x0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Temperature.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_SetAirVolume.dataBit.dataState = 0x0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_SetAirVolume.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_FrontDefrostSwitch.dataBit.dataState = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_FrontDefrostSwitch.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Heatedrear.dataBit.dataState = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Heatedrear.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_BlowingMode.dataBit.dataState = 0x0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_BlowingMode.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_InOutCirculate.dataBit.dataState = 0x0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_InOutCirculate.dataBit.flag = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_AutoSwitch.dataBit.dataState = 0;
	p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_AutoSwitch.dataBit.flag = 0;

	p_FAWACPInfo_Handle->RemoteControlData.EngineState.EngineState_Switch = 0;
	p_FAWACPInfo_Handle->RemoteControlData.PowerState.PowerState_Switch = 0;
	
	p_FAWACPInfo_Handle->RemoteControlData.VehicleSeat.VehicleSeat_DrivingSeat = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleSeat.VehicleSeat_Copilotseat = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleSeat.VehicleSeat_DrivingSeatMemory = 0;
	
	p_FAWACPInfo_Handle->RemoteControlData.VehicleCharge.VehicleCharge_Immediate = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleCharge.VehicleCharge_Appointment = 0;
	
	p_FAWACPInfo_Handle->RemoteControlData.VehicleAutopark.VehicleWifiStatus = 0;
	p_FAWACPInfo_Handle->RemoteControlData.VehicleAutopark.VehicleAutoOut = 0;
	
	p_FAWACPInfo_Handle->RemoteControlData.FunctionConfigStatus = 0;

	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].DeviceNo = 0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].EngineStartTime = 15;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].SamplingSwitch = 0;

	dataPool->getPara(B_CALL_ID, (void *)LoadCell, sizeof(LoadCell));
	dataPool->getPara(E_CALL_ID, (void *)EmergedCall, sizeof(EmergedCall));
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].EmergedCall,EmergedCall,15);
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].WhitelistCall,WhitelistCall,15);
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].CollectTimeInterval = 10;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval = 20;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].CollectTimeGpsSpeedInterval = 5;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeGpsSpeedInterval = 5;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ChargeMode = 0x0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ChargeSchedule.ScheduChargStartData = 0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ChargeSchedule.ScheduChargStartTime = 0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ChargeSchedule.ScheduChargEndData = 0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ChargeSchedule.ScheduChargEndTime = 0;
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LoadCell,LoadCell,15);
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].InformationCell,InformationCell,15);

	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery = 110;

	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].DeviceNo = 1;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].EngineStartTime = 15;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].SamplingSwitch = 0;
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].EmergedCall, EmergedCall, sizeof(EmergedCall));
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].WhitelistCall, WhitelistCall, sizeof(WhitelistCall));
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].CollectTimeInterval = 10;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = 60;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].CollectTimeGpsSpeedInterval = 5;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeGpsSpeedInterval = 5;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ChargeMode = 0x0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ChargeSchedule.ScheduChargStartData = 0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ChargeSchedule.ScheduChargStartTime = 0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ChargeSchedule.ScheduChargEndData = 0;
	p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ChargeSchedule.ScheduChargEndTime = 0;
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].LoadCell,LoadCell,15);
	memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].InformationCell,InformationCell,15);

	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEMSState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTCUState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEmissionState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTSRSState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTESPState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTABSState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEPASState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTOilPressureState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLowOilIDState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBrakeFluidLevelState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBatteryChargeState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBBWState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTPMSState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTSTTState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTExtLightState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTESCLState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEngineOverwaterState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTElecParkUnitState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAHBState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTACSState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWSState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLDWState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBlindSpotDetectState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAirconManualState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTHVSystemState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTHVInsulateState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTHVILState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBatteryChargeState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEVCellState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPowerMotorState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEParkState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEVCellLowBatteryState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEVCellOverTemperateState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPowerMotorOverTemperateState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTConstantSpeedSystemFailState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTChargerFaultState = 0x0;			 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAirFailureState = 0x0;			 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlternateAuxSystemFailState = 0x0; 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAutoEmergeSystemFailState = 0x0;	 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTReverRadarSystemFailState = 0x0;	 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTElecGearSystemFailState = 0x0;	 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftFrontTirePressAlarm = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftFrontTireTempAlarm = 0x0; 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightFrontTirePressAlarm = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightrontTireTempAlarm = 0x0;  
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftRearTirePressAlarm = 0x0;  
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftRearTireTempAlarm = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightRearTirePressAlarm = 0x0; 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightRearTireTempAlarm = 0x0;  
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTDCDCConverterFaultState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTVehControllerFailState = 0x0;		
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainPositRelayCoilFault = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainNegaRelayCoilFault = 0x0; 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.PrefillRelayCoilFault = 0x0;  
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.RechargePositRelayCoilFault = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.RechargeNegaRelayCoilFault = 0x0; 
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainPositiveRelayFault = 0x0;	   
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainNagetiveRelayFault = 0x0;	   

	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTSROverSpeedAlarmState = 0x0;	//TSR
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTSRLimitSpeedState = 0x0;		//TSR
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAEBInterventionState = 0x0;	//AEB
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTABSInterventionState = 0x0;	//ABS
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTASRInterventionState = 0x0;	//ASR
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTESPInterventionState = 0x0;	//ESP
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTDSMAlarmState = 0x0;			//DSM
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTowHandOffDiskState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTACCState = 0x0;				//ACC
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTACCSetSpeedState = 0x0;		//ACC
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWAlarmState = 0x0;			//FCW
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWState = 0x0;				//FCW
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWAlarmAccePedalFallState = 0x0;//FCW
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWAlarmFirstBrakeState = 0x0;	//FCW
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTSLDWState = 0x0;				//LDW
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLDWAlarmState = 0x0;			//LDW
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLDWAlarmDireDiskResponseState = 0x0;//LDW
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKAState = 0x0;				//LKA
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKAInterventionState = 0x0;	//LKA
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKADriverTakeOverPromptState = 0x0;//LKA
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKADriverResponsState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLBSDState = 0x0;				
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDLeftSideAlarmState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDRightSideAlarmState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDAlarmReftWheelRespState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDAlarmFirstBrakeState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDAlarmPedalAcceState = 0x0;		
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossLeftReportState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossRightReportState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossAlarmWhellState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossAlarmStopState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossAlarmAcceTreadleState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistLeftAlarmState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistRightAlarmState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistDireRepsonState = 0x0;
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistFirstStopState = 0x0;	
	p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistAcceDropState = 0x0;	
}

#define DEV_NO 0
void CFAWACP::InitAcpTBoxAuthData()
{
#if DEV_NO == 1
	memcpy(p_FAWACPInfo_Handle->Vin, "LFPZ1APCXH5F90106", strlen("LFPZ1APCXH5F90106"));
	memcpy(p_FAWACPInfo_Handle->ICCID, "89860198700303143983", strlen("89860198700303143983"));
	memcpy(p_FAWACPInfo_Handle->IMEI, "861477035453602", strlen("861477035453602"));
	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, "D2061812280A431760000000000001", strlen("D2061812280A431760000000000001"));
	memcpy(p_FAWACPInfo_Handle->IVISerialNumber, "000000000000000000000000000001", strlen("000000000000000000000000000001"));
#elif DEV_NO == 2//
	memcpy(p_FAWACPInfo_Handle->Vin, "LFPZ1APCXH5F90107", strlen("LFPZ1APCXH5F90107"));
	memcpy(p_FAWACPInfo_Handle->ICCID, "89860198700303143868", strlen("89860198700303143868"));
	memcpy(p_FAWACPInfo_Handle->IMEI, "861477034976124", strlen("861477034976124"));
	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, "D2061812280A431760000000000002", strlen("D2061812280A431760000000000002"));
	memcpy(p_FAWACPInfo_Handle->IVISerialNumber, "000000000000000000000000000002", strlen("000000000000000000000000000002"));
#elif DEV_NO == 3//
	memcpy(p_FAWACPInfo_Handle->Vin, "LMGFE3G88D1000601", strlen("LMGFE3G88D1000601"));
	memcpy(p_FAWACPInfo_Handle->ICCID, "89860117750028636521", strlen("89860117750028636521"));
	memcpy(p_FAWACPInfo_Handle->IMEI, "861477035453859", strlen("861477035453859"));
	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, "D2061812280A431760000000000003", strlen("D2061812280A431760000000000003"));
	memcpy(p_FAWACPInfo_Handle->IVISerialNumber, "000000000000000000000000000003", strlen("000000000000000000000000000003"));
#elif DEV_NO == 5
	memcpy(p_FAWACPInfo_Handle->Vin, "LFPZ1APCXH5F90109", strlen("LFPZ1APCXH5F90109"));
	memcpy(p_FAWACPInfo_Handle->ICCID, "89860918700300514749", strlen("89860918700300514749"));
	memcpy(p_FAWACPInfo_Handle->IMEI, "861477035992781", strlen("861477035992781"));
	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, "D2061812280A431760000000000016", strlen("D2061812280A431760000000000016"));
	memcpy(p_FAWACPInfo_Handle->IVISerialNumber, "000000000000000000000000000016", strlen("000000000000000000000000000016"));
#elif DEV_NO == 6
	memcpy(p_FAWACPInfo_Handle->Vin, "LFPZ1APCXH5F90110", strlen("LFPZ1APCXH5F90110"));
	memcpy(p_FAWACPInfo_Handle->ICCID, "89860918700300514491", strlen("89860918700300514491"));
	memcpy(p_FAWACPInfo_Handle->IMEI, "861477035454936", strlen("861477035454936"));
	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, "D2061812280A4317600000000000017", strlen("D2061812280A4317600000000000017"));
	memcpy(p_FAWACPInfo_Handle->IVISerialNumber, "0000000000000000000000000000017", strlen("0000000000000000000000000000017"));
#elif DEV_NO == 7
	memcpy(p_FAWACPInfo_Handle->Vin, "LFPZ1APCXH5F90113", strlen("LFPZ1APCXH5F90112"));
	memcpy(p_FAWACPInfo_Handle->ICCID, "89860918700300514806", strlen("89860918700300514806"));
	memcpy(p_FAWACPInfo_Handle->IMEI, "861477035994803", strlen("861477035994803"));
	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, "D2061812280A4317600000000000012", strlen("D2061812280A4317600000000000012"));
	memcpy(p_FAWACPInfo_Handle->IVISerialNumber, "0000000000000000000000000000012", strlen("0000000000000000000000000000012"));
#elif DEV_NO == 8
	memcpy(p_FAWACPInfo_Handle->Vin, "LFPZ1APCXH5F90104", strlen("LFPZ1APCXH5F90104"));
	memcpy(p_FAWACPInfo_Handle->ICCID, "89860918700300514707", strlen("89860918700300514707"));
	memcpy(p_FAWACPInfo_Handle->IMEI, "861477035994118", strlen("861477035994118"));
	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, "D2061812280A4317600000000000013", strlen("D2061812280A4317600000000000013"));
	memcpy(p_FAWACPInfo_Handle->IVISerialNumber, "0000000000000000000000000000013", strlen("0000000000000000000000000000013"));
#endif

	memcpy(p_FAWACPInfo_Handle->VehicleCondData.VerTboxOS,"MPU_V1.1.083",12);//
	//memcpy(p_FAWACPInfo_Handle->VehicleCondData.Tboxersion,"MPU_V1.1.04500",14);

#if 1
	
	dataPool->getTboxConfigInfo(VinID, p_FAWACPInfo_Handle->Vin, sizeof(p_FAWACPInfo_Handle->Vin));
	//memcpy(p_FAWACPInfo_Handle->Vin,"TBOX03181201A0463",17);

	//get ICCID
	memset(p_FAWACPInfo_Handle->ICCID, 0, sizeof(p_FAWACPInfo_Handle->ICCID));
	dataPool->getPara(SIM_ICCID_INFO, p_FAWACPInfo_Handle->ICCID, sizeof(p_FAWACPInfo_Handle->ICCID));
	//get IMEI
	memset(p_FAWACPInfo_Handle->IMEI, 0, sizeof(p_FAWACPInfo_Handle->IMEI));
	dataPool->getPara(IMEI_INFO, p_FAWACPInfo_Handle->IMEI, sizeof(p_FAWACPInfo_Handle->IMEI));
#endif
	uint8_t pRootKey[] = {0x7b, 0xde, 0x18, 0x70, 0x1a, 0xc2, 0x52, 0xb9, 0xa9, 0xb6, 0x8a, 0x08, 0x8c, 0x9a, 0x8f, 0x82};
	memcpy(ALLRootKey, pRootKey, 16);
	//memcpy(p_FAWACPInfo_Handle->RootKey, pRootKey, sizeof(p_FAWACPInfo_Handle->RootKey));
	memset(p_FAWACPInfo_Handle->AuthToken, 0, sizeof(p_FAWACPInfo_Handle->AuthToken));
	memset(p_FAWACPInfo_Handle->SKey, 0, sizeof(p_FAWACPInfo_Handle->SKey));

	uint16_t tempLen = 0;
	//
	tempLen = 17;
	AcpTBox_AuthApplyMsg.CarManufactureID.Element_LenInfo.Element_Len = 1;
	AcpTBox_AuthApplyMsg.CarManufactureID.CarManufactureID = AcpCarManufactureID_TianJing;

	AcpTBox_AuthApplyMsg.TU_VIN.Element_LenInfo.elementLenBit.Identifier = 1;
	AcpTBox_AuthApplyMsg.TU_VIN.Element_LenInfo.elementLenBit.MoreFlag = 0;
	AcpTBox_AuthApplyMsg.TU_VIN.Element_LenInfo.elementLenBit.DataLen = tempLen&0xFF1F;
	memcpy(AcpTBox_AuthApplyMsg.TU_VIN.CarTU_ID, p_FAWACPInfo_Handle->Vin, sizeof(p_FAWACPInfo_Handle->Vin));

	tempLen = 48;
	AcpTsp_AkeyMsg.Len_High.elementLenBit.Identifier = 0;
	AcpTsp_AkeyMsg.Len_High.elementLenBit.MoreFlag = 1;
	AcpTsp_AkeyMsg.Len_High.elementLenBit.DataLen = ((tempLen<<4)>>11)&0xFF1F;
	AcpTsp_AkeyMsg.Len_Low.elementLenBit.MoreFlag = 0;
	AcpTsp_AkeyMsg.Len_Low.elementLenBit.DataLen  = ((tempLen<<9)>>9)&0xFF7F;
	memset(AcpTsp_AkeyMsg.AkeyC_Tsp, 0, sizeof(AcpTsp_AkeyMsg.AkeyC_Tsp));
	memset(AcpTsp_AkeyMsg.Rand1_Tsp, 0, sizeof(AcpTsp_AkeyMsg.Rand1_Tsp));

	tempLen = 48;
	AcpTBox_CTMsg.Len_High.elementLenBit.Identifier = 0;
	AcpTBox_CTMsg.Len_High.elementLenBit.MoreFlag = 1;
	AcpTBox_CTMsg.Len_High.elementLenBit.DataLen = ((tempLen<<4)>>11)&0xFF1F;
	AcpTBox_CTMsg.Len_Low.elementLenBit.MoreFlag = 0;
	AcpTBox_CTMsg.Len_Low.elementLenBit.DataLen  = ((tempLen<<9)>>9)&0xFF7F;
	memset(AcpTBox_CTMsg.Rand1CT_TBox, 0, sizeof(AcpTBox_CTMsg.Rand1CT_TBox));
	memset(AcpTBox_CTMsg.Rand2_Tbox, 0, sizeof(AcpTBox_CTMsg.Rand2_Tbox));

	tempLen = 33;
	AcpTsp_CSMsg.Len_High.elementLenBit.Identifier = 0;
	AcpTsp_CSMsg.Len_High.elementLenBit.MoreFlag = 1;
	AcpTsp_CSMsg.Len_High.elementLenBit.DataLen = ((tempLen<<4)>>11)&0xFF1F;
	AcpTsp_CSMsg.Len_Low.elementLenBit.MoreFlag = 0;
	AcpTsp_CSMsg.Len_Low.elementLenBit.DataLen  = ((tempLen<<9)>>9)&0xFF7F;
	memset(AcpTsp_CSMsg.Rand2CS_Tsp, 0, sizeof(AcpTsp_CSMsg.Rand2CS_Tsp));
	AcpTsp_CSMsg.RT_Tsp = 0;

	tempLen = 17;
	AcpTBox_RSMsg.LenInfo.elementLenBit.Identifier = 0;
	AcpTBox_RSMsg.LenInfo.elementLenBit.MoreFlag = 0;
	AcpTBox_RSMsg.LenInfo.elementLenBit.DataLen = tempLen&0xFF1F;
	memset(AcpTBox_RSMsg.Rand3_TBox, 0, sizeof(AcpTBox_RSMsg.Rand3_TBox));
	AcpTBox_RSMsg.RS_Tbox = 0;

	tempLen = 64;
	AcpTsp_SKeyMsg.Len_High.elementLenBit.Identifier = 0;
	AcpTsp_SKeyMsg.Len_High.elementLenBit.MoreFlag = 1;
	AcpTsp_SKeyMsg.Len_High.elementLenBit.DataLen = ((tempLen<<4)>>11)&0xFF1F;
	AcpTsp_SKeyMsg.Len_Low.elementLenBit.MoreFlag = 0;
	AcpTsp_SKeyMsg.Len_Low.elementLenBit.DataLen  = ((tempLen<<9)>>9)&0xFF7F;
	memset(AcpTsp_SKeyMsg.SkeyC_Tsp, 0, sizeof(AcpTsp_SKeyMsg.SkeyC_Tsp));
	memset(AcpTsp_SKeyMsg.Rand3CS_Tsp, 0, sizeof(AcpTsp_SKeyMsg.Rand3CS_Tsp));

	tempLen = 112;
	AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Element_Len_High.elementLenBit.Identifier = 1;
	AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Element_Len_High.elementLenBit.MoreFlag = 1;
	AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Element_Len_High.elementLenBit.DataLen = ((tempLen<<4)>>11)&0xFF1F;
	AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Element_Len_Low.elementLenBit.MoreFlag = 0;
	AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Element_Len_Low.elementLenBit.DataLen  = ((tempLen<<9)>>9)&0xFF7F;

	memcpy(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Vin,p_FAWACPInfo_Handle->Vin,sizeof(p_FAWACPInfo_Handle->Vin));
	memcpy(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IMEI,p_FAWACPInfo_Handle->IMEI,sizeof(p_FAWACPInfo_Handle->IMEI));
	memcpy(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.ICCID,p_FAWACPInfo_Handle->ICCID,sizeof(p_FAWACPInfo_Handle->ICCID));
	memcpy(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IVISerialNumber,p_FAWACPInfo_Handle->IVISerialNumber,sizeof(p_FAWACPInfo_Handle->IVISerialNumber));
	memcpy(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.TBoxSerialNumber,p_FAWACPInfo_Handle->TBoxSerialNumber,sizeof(p_FAWACPInfo_Handle->TBoxSerialNumber));

	tempLen = 4;
	AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.Element_LenInfo.elementLenBit.Identifier = 0;
	AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.Element_LenInfo.elementLenBit.MoreFlag = 0;
	AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.Element_LenInfo.elementLenBit.DataLen = tempLen&0xFF1F;
	memset(AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken, 0, sizeof(AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken));
}

void *CFAWACP::receiveThread(void *arg)
{
	CFAWACP *pCFAWACP = (CFAWACP*)arg;
	pthread_detach(pthread_self());

	int length;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
	}

	while(1)
	{
		if(pCFAWACP->m_ConnectedState == true)
		{
			FAWACPLOG("Waiting to receive data:");
			memset(dataBuff, 0, MALLOC_DATA_LEN);
			length = recv(pCFAWACP->m_Socketfd, dataBuff, MALLOC_DATA_LEN, 0);
			FAWACPLOG("Receive data --->length == %d:",length);
			if(length > 0)
			{
				for(uint16_t i = 0; i< length; i++)
					FAWACP_NO("%02x", *(dataBuff + i));
				FAWACP_NO("\n\n");

				if(pCFAWACP->checkReceivedData(dataBuff, length) == 0)
				{
					pCFAWACP->Unpack_FAWACP_FrameData(dataBuff+DATA_SOFEOF_SIZE+SIGNATURE_SIZE, length - 2*DATA_SOFEOF_SIZE - SIGNATURE_SIZE);
				}
			}
			else if(0 == length)
			{
				FAWACPLOG("Receive data length = %d.\n", length);				
				//pCFAWACP->m_loginState = STATUS_LOGGIN_INIT;
				//pCFAWACP->m_ConnectedState = false;
				// printf("====== program exit wait for next connect.======\n");
			}
			else
			{
				if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
				{

					//printf("jason add receive socket errno \r\n");
					continue;
				}
				else
				{
					//alive = false;
					SocketDisconnectCount++;
					//printf("====== ret < 0 socket error program exit ======.\n");					
					FAWACP_ERROR_Log(1, errno,SocketDisconnectCount);

				}
			}
		}
		else
		{
			sleep(1);
		}
	}
	if(NULL != dataBuff)
	{
		free(dataBuff);
		dataBuff = NULL;
	}
	pthread_exit(0);
}

uint16_t CFAWACP::checkReceivedData(uint8_t *pData, uint32_t len)
{
	uint8_t *pos = pData;
	uint8_t ProtocolSignature[SIGNATURE_SIZE] = {0};

	if(len < 2*DATA_SOFEOF_SIZE + SIGNATURE_SIZE)
	{
		FAWACPLOG("Received data Length error:%d\n",len);
		return -1;
	}

	uint32_t Sof = FAWACP_PROTOCOL_SOF;
	if(Sof != htonl(*(uint32_t *)pos))
		return -1;

	uint32_t Eof = FAWACP_PROTOCOL_EOF;
	if(Eof != htonl(*(uint32_t *)(pos+len-DATA_SOFEOF_SIZE)))
		return -1;

	FAWACPLOG("Data SOFEOF check success!\n");
	//
	uint16_t Len_Payload = len - 2*DATA_SOFEOF_SIZE - SIGNATURE_SIZE;
	PackData_Signature(pos + DATA_SOFEOF_SIZE + SIGNATURE_SIZE, Len_Payload, ProtocolSignature);

	if(memcmp(ProtocolSignature, pos+DATA_SOFEOF_SIZE, SIGNATURE_SIZE) != 0)
	{
		FAWACPLOG("Received data signature error.\n");
		return -1;
	}
	FAWACPLOG("Received data signature check success.\n");

	return 0;
}

//
// 
int	CFAWACP::sendLoginAuth(uint8_t MsgType, AcpCryptFlag_E CryptFlag)
{
	uint16_t dataLen;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
		return -1;
	}
	memset(dataBuff, 0, sizeof(dataBuff));
	//
	dataLen = PackFAWACP_FrameData(dataBuff, MALLOC_DATA_LEN, ACPApp_AuthenID, MsgType, CryptFlag);
	pthread_mutex_lock(&mutex_Rbuf);
	m_RingBuff.WriteData(dataBuff, dataLen);
	pthread_mutex_unlock(&mutex_Rbuf);
#if 0
	if((dataLen = send(m_Socketfd, dataBuff, dataLen, 0)) < 0)
	{
		SocketDisconnectCount++;
		FAWACPLOG("Send data failed.dataLen:%d\n",dataLen);
		FAWACP_ERROR_Log(2, errno,SocketDisconnectCount);
		free(dataBuff);
		dataBuff = NULL;
		m_loginState = 0;
		m_ConnectedState = false;
		return -1;
	}
#endif
	free(dataBuff);
	dataBuff = NULL;
	return 0;
}
//	TBox
int	CFAWACP::sendLoginAuthApplyMsg()
{
	sendLoginAuth(MESSAGE_TYPE_START+1, AcpCryptFlag_NULL);
	return 0;
}
//	CT Message
int	CFAWACP::sendLoginCTMsg()
{
	//	uint16_t AkeyLen = 0;
	//	uint16_t Rand1CTLen;

	//uint8_t i = 0;
	//RootKeyAKeyC_Tsp->AKey_TBoxAKey_TBox
	Decrypt_AES128Data(ALLRootKey, AcpTsp_AkeyMsg.AkeyC_Tsp, sizeof(AcpTsp_AkeyMsg.AkeyC_Tsp), AKey_TBox, AcpCryptFlag_IS);
	//AKey_TBoxRand1_Tsp->Rand1CT_TBox
	Encrypt_AES128Data(AKey_TBox, AcpTsp_AkeyMsg.Rand1_Tsp, sizeof(AcpTsp_AkeyMsg.Rand1_Tsp), AcpTBox_CTMsg.Rand1CT_TBox, AcpCryptFlag_IS);

	//Rand2_Tbox
	HMACMd5_digest((const char *)ALLRootKey, AcpTBox_CTMsg.Rand1CT_TBox, sizeof(AcpTBox_CTMsg.Rand1CT_TBox), AcpTBox_CTMsg.Rand2_Tbox);

	sendLoginAuth(MESSAGE_TYPE_START+3, AcpCryptFlag_NULL);
	return 0;
}

//	RS Message
int	CFAWACP::sendLoginRSMsg()
{
	uint8_t	Rand2CT_TBox[32];

	//AKey_TBoxRand2_Tbox->Rand2CT_TBox 
	Encrypt_AES128Data(AKey_TBox, AcpTBox_CTMsg.Rand2_Tbox, sizeof(AcpTBox_CTMsg.Rand2_Tbox), Rand2CT_TBox, AcpCryptFlag_IS);

	//Rand2CS_Tsp->RS_Tbox
	AcpTBox_RSMsg.RS_Tbox = memcmp(Rand2CT_TBox,AcpTsp_CSMsg.Rand2CS_Tsp, sizeof(Rand2CT_TBox));

	if(AcpTBox_RSMsg.RS_Tbox != 0){
		FAWACPLOG("Rand2CT_TBox");
		m_loginState = STATUS_LOGGIN_INIT;
		return -1;
	}
	//
	HMACMd5_digest((const char *)ALLRootKey, Rand2CT_TBox, sizeof(Rand2CT_TBox), AcpTBox_RSMsg.Rand3_TBox);

	sendLoginAuth(MESSAGE_TYPE_START+5, AcpCryptFlag_NULL);
	return 0;
}

//	AuthReadyMsg 7/8
int	CFAWACP::sendLoginAuthReadyMsg()
{
	uint8_t  Rand3CT_TBox[32];
	//AKey_TBoxSkeyC_Tsp->SKey_TBoxAKey_TBox
	Decrypt_AES128Data(AKey_TBox, AcpTsp_SKeyMsg.SkeyC_Tsp, sizeof(AcpTsp_SKeyMsg.SkeyC_Tsp), SKey_TBox, AcpCryptFlag_IS);
	//SKey_TBoxRand3_Tbox->Rand3CT_TBox SKey_TBox
	Encrypt_AES128Data(SKey_TBox, AcpTBox_RSMsg.Rand3_TBox, sizeof(AcpTBox_RSMsg.Rand3_TBox), Rand3CT_TBox, AcpCryptFlag_IS);
	//Rand3CS_Tsp->true:OK,false:fail
	if(0 != memcmp(Rand3CT_TBox, AcpTsp_SKeyMsg.Rand3CS_Tsp, sizeof(Rand3CT_TBox)))
	{
		FAWACPLOG("Rand3CT_TBoxRand3CS_Tsp");
		m_loginState = STATUS_LOGGIN_INIT;
		return -1;
	}
	//skey
	memcpy(p_FAWACPInfo_Handle->SKey, SKey_TBox, sizeof(SKey_TBox));
	//
	sendLoginAuth(MESSAGE_TYPE_START+7, AcpCryptFlag_IS);
	return 0;
}

//
uint16_t	CFAWACP::timingReportingData(uint8_t MsgType, AcpAppID_E AppID)
{
	if(false == cfawacp->m_bEnableSendDataFlag)
		return -1;
	static int times = 0;
	uint16_t dataLen;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL){
		FAWACPLOG("malloc dataBuff error!");
		return -1;
	}
	memset(dataBuff, 0, MALLOC_DATA_LEN);
	//
	FAWACPLOG("error test\n\n\n");
	dataLen = cfawacp->PackFAWACP_FrameData(dataBuff, MALLOC_DATA_LEN, AppID, MsgType, AcpCryptFlag_IS);

	//
	if(MsgType == 2){	//
		//pSqliteDatabase->insertData(dataBuff, dataLen, times);
		times++;
	}

	printf("&&&&&&&&&&&&&&\n\n");
	for(int i = 0;i<dataLen;i++)
	{
		printf("%02x ",dataBuff[i]);
	}
	pthread_mutex_lock(&mutex_Rbuf);
	cfawacp->m_RingBuff.WriteData(dataBuff, dataLen);
	pthread_mutex_unlock(&mutex_Rbuf);
#if 0
	if((dataLen = send(cfawacp->m_Socketfd, dataBuff, dataLen, 0)) < 0)
	{
		SocketDisconnectCount++;
		FAWACPLOG("Send data failed.dataLen:%d\n",dataLen);
		FAWACP_ERROR_Log(2, errno,SocketDisconnectCount);
		free(dataBuff);
		dataBuff = NULL;
		cfawacp->m_loginState = 0;
		cfawacp->m_ConnectedState = false;
		return -1;
	}
#endif
	FAWACPLOG("Send data ok.dataLen:%d\n",dataLen);
	free(dataBuff);
	dataBuff = NULL;

	return 0;
}


//MCU
void CFAWACP::TspRemoteCtrlRes()
{
	cfawacp->RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, cfawacp->TspctrlSource);
}


//
int CFAWACP::RespondTspPacket(AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID)
{
	if((false == cfawacp->m_bEnableSendDataFlag) && (applicationID != ACPApp_UpdateKeyID))
		return -1;

	uint16_t dataLen;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
		return -1;
	}
	memset(dataBuff, 0, MALLOC_DATA_LEN);
	//
	dataLen = PackFAWACP_FrameData(dataBuff, MALLOC_DATA_LEN, applicationID, MsgType, CryptFlag, TspSourceID);
	#if 0
	for(int i =0;i<dataLen;i++)
	{
		FAWACPLOG("ctcl reply data %02x\n",dataBuff[i]);
	}
	#endif
	pthread_mutex_lock(&mutex_Rbuf);
	cfawacp->m_RingBuff.WriteData(dataBuff, dataLen);
	pthread_mutex_unlock(&mutex_Rbuf);
#if 0
	if((dataLen = send(m_Socketfd, dataBuff, dataLen, 0)) < 0)
	{
		SocketDisconnectCount++;
		FAWACPLOG("Send data failed.dataLen:%d\n",dataLen);
		FAWACP_ERROR_Log(2, errno,SocketDisconnectCount);
		free(dataBuff);
		dataBuff = NULL;
		m_loginState = 0;
		m_ConnectedState = false;
		return -1;
	}
#endif
	FAWACPLOG("Send data ok.dataLen:%d\n",dataLen);
	free(dataBuff);
	dataBuff = NULL;
	/*if(m_AcpRemoteCtrlList.SubitemCode[0]==11)
	  {
	//disconnectSocket();
	//Headerlink=NULL;
	disconnectSocket();

	}*/
	return 0;
}

/****************************************************************/
//
void CFAWACP::PackData_Signature(uint8_t *dataBuff, uint16_t len, uint8_t *OutBuff)
{
	uint8_t *pos = dataBuff;
	//SHA-1
	m_SHA1OpData.Encrpy_SHA1Data(dataBuff, len, OutBuff);
}
//
void CFAWACP::Packet_AcpHeader(AcpAppHeader_t& Header, AcpAppID_E AppID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint16_t MsgLength)
{
	//
	Header.Reserved_ID = 0;
	Header.PrivateFlag = 1;
	Header.Acp_AppID 	= AppID;
	//
	Header.Reserved_T = 0;
	Header.TestFlag = VERSION_FLAG;
	Header.CryptFlag = CryptFlag;
	Header.MsgType	 =	MsgType;
	//
	Header.VersionFlag	= 1;
	Header.Version		= 0;
	Header.MsgCtrlFlag	= 2;
	//
	Header.MsgLength	=	MsgLength;
}
//
uint16_t CFAWACP::AddData_AcpHeader(uint8_t *dataBuff, AcpAppHeader_t Header)
{
	uint8_t *pos = dataBuff;

	*pos++ = ((Header.Reserved_ID << 7) & 0x80) |		//
		((Header.PrivateFlag << 6) & 0x40)|//
		(Header.Acp_AppID & 0x3F);  //Acp
	/**/
	*pos++ = ((Header.Reserved_T << 7) & 0x80) |		//
		((Header.TestFlag << 6) & 0x40) |	//
		((Header.CryptFlag << 5) & 0x20)|	//
		(Header.MsgType & 0x1F);	//
	
	*pos++ = ((Header.VersionFlag << 7) & 0x80) |		
		((Header.Version << 4) & 0x70) |	
		(Header.MsgCtrlFlag & 0x0F);
	*pos++ = (Header.MsgLength >> 8) & 0xFF;	
	*pos++ = Header.MsgLength & 0xFF;			

	return (uint16_t)(pos-dataBuff);
}

int CFAWACP::insertlink(uint8_t applicationID, uint8_t MsgType, uint8_t TspSourceID)
{
	time_t BuffTime = 0;
	LinkTimer_t *p_new;
	//
	p_new = (LinkTimer_t *)malloc(sizeof(LinkTimer_t));
	if(NULL == p_new)
	{
		FAWACPLOG("malloc ElementBuff error!");
		return -1;
	}
	memset(p_new, 0, sizeof(LinkTimer_t));

	p_new->AcpAppID = applicationID;
	p_new->MsgType = MsgType;
	p_new->TspSoure = TspSourceID;

	time(&BuffTime);
	p_new->time = BuffTime;

	insert(&Headerlink, p_new);
	return 0;
}
//
uint32_t CFAWACP::PackFAWACP_PayloadData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID)
{
	uint32_t dataLen = 0;
	uint8_t *pos = dataBuff;
	//Element
	//
	uint8_t *ElementBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(ElementBuff == NULL)
	{
		FAWACPLOG("malloc ElementBuff error!");
		return -1;
	}
	memset(ElementBuff, 0, sizeof(ElementBuff));

	FAWACPLOG("PackFAWACP_PayloadData:AppID = %d, MsgID = %d, CryptFlag = %d \n",applicationID, MsgType, CryptFlag );

	//PayloadBuff 
	switch (applicationID)
	{
		case ACPApp_AuthenID://
			switch (MsgType)
			{
				case 1:			//TBox Auth Apply Message
					dataLen = AuthApplyDataDeal(ElementBuff);
					break;
				case 3:			//CT Message
					//
					TimeDeal(ElementBuff);
					dataLen = CTMsgDataDeal(ElementBuff + DATA_TIMESTAMP_SIZE) + DATA_TIMESTAMP_SIZE;
					break;
				case 5:			//RS Message
					//
					TimeDeal(ElementBuff);
					dataLen = RSMsgDataDeal(ElementBuff + DATA_TIMESTAMP_SIZE) + DATA_TIMESTAMP_SIZE;
					break;
				case 7:			//Auth Ready Message
					//
					TimeDeal(ElementBuff);
					dataLen = AuthReadyMsgDataDeal(ElementBuff + DATA_TIMESTAMP_SIZE) + DATA_TIMESTAMP_SIZE;
					break;
				default:
					break;
			}
			break;
		case ACPApp_HeartBeatID://
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE+DATA_AUTHTOKEN_SIZE;
			break;
		case ACPApp_RemoteCtrlID://
			//
			TimeDeal(ElementBuff);
#ifndef MQTT
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, m_CMcu->RCtrlErrorCode);//

			//m_CMcu->RCtrlErrorCode=0;
			m_CMcu->RCtrlErrorCode=0;
			dataLen = RemoteCtrlCommandDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
#else
			//printf("\n%s(%d): error code: %d\n", __func__, __LINE__, m_CMcu->RCtrlErrorCode);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE, m_CMcu->RCtrlErrorCode);//
			m_CMcu->RCtrlErrorCode=0;
			dataLen = RemoteCtrlCommandDeal(ElementBuff + DATA_TIMESTAMP_SIZE + 1);
			dataLen += DATA_TIMESTAMP_SIZE  + 1;
#endif
#if 0
			TimeOutType.RemoteCtrlTspSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_RemoteCtrlID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
#endif
			break;
		case ACPApp_QueryVehicleCondID://
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, 0);
			//
			dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE, 0);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
#if 0
			TimeOutType.VehQueCondSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_QueryVehicleCondID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
#endif
			break;
		case ACPApp_RemoteConfigID://
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, 0);
			dataLen = RemoteConfigCommandDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
#if 0
			TimeOutType.RemoteCpnfigSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_RemoteConfigID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
#endif
			break;
		case ACPApp_UpdateKeyID:
			switch(MsgType)
			{
				case 1://Rootkey
					TimeDeal(ElementBuff);
					AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
					break;
				case 3://Rootkey
					//
					TimeDeal(ElementBuff);
					AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
					SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
					ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, 0);
					//RootKeyData
					dataLen = RootKeyDataDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
					dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
#if 0
					TimeOutType.UpdateRootkeySource = TspSourceID;
					pthread_mutex_lock(&mutex);
					insertlink( ACPApp_UpdateKeyID, MsgType, TspSourceID);
					pthread_mutex_unlock(&mutex);
#endif
					break;
				default:
					break;
			}
			break;
		case ACPApp_RemoteUpgradeID://
			//
			FAWACPLOG("remote upgrade id %d  %d\n",applicationID,MsgType);
			TimeDeal(ElementBuff);
			//AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			//SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			Upgrade_CommandResponse = 0;
			//ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE, Upgrade_CommandResponse);
			switch(MsgType)
			{
				case UpgradeStep_2:
					dataLen = RemoteUpgrade_ResponseDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
					break;
				case UpgradeStep_3:
					dataLen = RemoteUpgrade_DownloadStartDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
					break;
				case UpgradeStep_4:
					dataLen = 0;
					//dataLen = RemoteUpgrade_DownloadResultDeal(ElementBuff + DATA_TIMESTAMP_SIZE + 1);
					break;
				case UpgradeStep_5:
					ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE, Upgrade_CommandResponse);
					dataLen = RemoteUpgrade_DownloadResultDeal(ElementBuff + DATA_TIMESTAMP_SIZE +1);
					dataLen +=1;
					break;
				case UpgradeStep_6:
					dataLen = RemoteUpgrade_UpdateResultDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
					break;
				case UpgradeStep_8:
					dataLen = RemoteUpgrade_ResponseDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
				default:
					break;
			}
			dataLen += DATA_TIMESTAMP_SIZE;
			break;
		case ACPApp_ReqBleTokenID:
			switch(MsgType)
			{
				case 1:
					TimeDeal(ElementBuff);
					////*(ElementBuff+DATA_TIMESTAMP_SIZE + 1) = 0x0;////  Request Source
					//AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
					dataLen += DATA_TIMESTAMP_SIZE ;//+DATA_AUTHTOKEN_SIZE;
					printf("jason add responed first \r\n");

					
					break;
				case 2:
				default:
					TimeDeal(ElementBuff);
					////*(ElementBuff+DATA_TIMESTAMP_SIZE + 1) = 0x0;////  Request Source
					BleTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
					dataLen += DATA_TIMESTAMP_SIZE+MQTT_BLE_TOKEN_SIZE;
					printf("jason add responed second \r\n");

					break;
			}
			break;
		case ACPApp_EmergencyDataID://
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			//
			dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, 0);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE;
			break;
		case ACPApp_VehicleCondUploadID:
			//
			TimeDeal(ElementBuff);
			//AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			//
			if(1 == MsgType){
				dataLen = ReportGPSSpeedDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			}//
			else if(2 == MsgType){
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE, 0);
			}//
			else if(18 == MsgType){
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE, 1);
			}//
			else if(20 == MsgType){
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE, 2);
			}//
			else{
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE, 0);
			}
			//dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE;
			dataLen += DATA_TIMESTAMP_SIZE;
			break;
		case ACPApp_GPSID: 	//
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, 0);
			//GPSλ
			dataLen = VehicleGPSDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
#if 0
			TimeOutType.VehGPSSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_GPSID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
#endif
			break;
		case ACPApp_VehicleAlarmID://
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			//
			dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, 0);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE;
			break;
		case ACPApp_RemoteDiagnosticID://
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, 0);
			dataLen = RemoteDiagnosticDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
#if 0
			TimeOutType.RemoteDiagnoSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_RemoteDiagnosticID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
#endif
			break;
		case ACPApp_CameraRecordReplyID:
			FAWACPLOG("^^^^^^^\n\n\n");
			TimeDeal(ElementBuff);
			FAWACPLOG("^^^^^^^\n\n\n");
			if(1 == MsgType){
				dataLen = Camera_Gsensor(ElementBuff + DATA_TIMESTAMP_SIZE);
			}//
			else if(3 == MsgType)
			{

			}
			else if(4 == MsgType)
			{
				FAWACPLOG("^^^^^^^\n\n\n");
				uint8_t *pos = ElementBuff + DATA_TIMESTAMP_SIZE;
				if(uploadCamera_result)
					*pos++ =0;
				else
				{
					*pos++ =1;
				}
				uploadCamera_result = false;
				dataLen = 2;
			}
			else if(MsgType == 6)
			{
				dataLen = CameraCtrl_resultDeal(ElementBuff + DATA_TIMESTAMP_SIZE, CameraCtrl_result);
			}
#if 0
			else if(MsgType == 8)
			{
				dataLen = CameraCtrl_resultDeal(ElementBuff + DATA_TIMESTAMP_SIZE, CameraCtrl_result);
			}
			else if(MsgType == 10)
			{
				dataLen = CameraCtrl_resultDeal(ElementBuff + DATA_TIMESTAMP_SIZE, CameraCtrl_result);
			}
			else if(MsgType == 12)
			{
				dataLen = CameraCtrl_resultDeal(ElementBuff + DATA_TIMESTAMP_SIZE, CameraCtrl_result);
			}
			else if(MsgType == 14)
			{
				dataLen = CameraCtrl_resultDeal(ElementBuff + DATA_TIMESTAMP_SIZE, CameraCtrl_result);
			}
#endif
			dataLen += DATA_TIMESTAMP_SIZE;
			break;
		case ACPApp_SyncTimeRequestID:
			TimeDeal(ElementBuff);
			dataLen += DATA_TIMESTAMP_SIZE;
			break;
		default:
			break;
	}
	//
#if 0
	uint16_t Encrypt_len = 0;
	uint8_t *EncryptBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(EncryptBuff == NULL){
		FAWACPLOG("malloc EncryptBuff error!");
		return -1;
	}

	FAWACPLOG("before Encrypt PayloadData length==%d\n",dataLen);
	for(uint16_t i = 0; i < dataLen; i++)
		FAWACP_NO("%02x ", *(ElementBuff + i));
	FAWACP_NO("\n\n");

	Encrypt_len = Encrypt_AES128Data(ALLRootKey, ElementBuff, dataLen, EncryptBuff, CryptFlag);

	FAWACPLOG("after Encrypt PayloadData length==%d\n",Encrypt_len);
	for(uint16_t i = 0; i < Encrypt_len; i++)
		FAWACP_NO("%02x ", *(EncryptBuff + i));
	FAWACP_NO("\n\n");

	memset(ElementBuff, 0, sizeof(ElementBuff));
	Decrypt_AES128Data(ALLRootKey, EncryptBuff, Encrypt_len, ElementBuff, CryptFlag);
	for(uint16_t i = 0; i < dataLen; i++)
		FAWACP_NO("%02x ", *(ElementBuff + i));
	FAWACP_NO("\n\n");

#endif
	//
	AcpAppHeader_t Header;
	Packet_AcpHeader(Header, applicationID, MsgType, CryptFlag, DATA_HEADER_SIZE+dataLen);
	AddData_AcpHeader(pos, Header);
	pos += DATA_HEADER_SIZE;
	//
	memcpy(pos, ElementBuff, dataLen);
	pos += dataLen;

	//if(NULL	!= EncryptBuff){
	//free(EncryptBuff);
	//EncryptBuff = NULL;
	//}
	if(NULL != ElementBuff){
		free(ElementBuff);
		ElementBuff = NULL;
	}
	return (uint16_t)(pos-dataBuff);
}


//
uint16_t CFAWACP::PackFAWACP_FrameData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID)
{
	uint16_t PayloadLen;
	uint8_t *pos = dataBuff;
	uint8_t *pData = dataBuff;
	// Packet SOF
	uint32_t Sof = FAWACP_PROTOCOL_SOF;
	//SOFEOFDeal(pos, Sof);
	//pos += 	DATA_SOFEOF_SIZE;
	*pos++ = 0x7e;
	*pos++ = 0;
	*pos++ = 0;
	//Packet PAYLOAD
	uint8_t *PayloadBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(PayloadBuff == NULL){
		FAWACPLOG("malloc PayloadBuff error!");
		return -1;
	}
	memset(PayloadBuff, 0, MALLOC_DATA_LEN);
	
	PayloadLen = PackFAWACP_PayloadData(PayloadBuff, MALLOC_DATA_LEN, applicationID, MsgType, CryptFlag, TspSourceID);
	// signature
	//uint8_t *SignatureBuff = (uint8_t *)malloc(SIGNATURE_SIZE);
	//if(SignatureBuff == NULL){
	//	FAWACPLOG("malloc SignatureBuff error!");
	//	return -1;
	//}
	//memset(SignatureBuff, 0, SIGNATURE_SIZE);
	//PackData_Signature(PayloadBuff, PayloadLen, SignatureBuff);
	//memcpy(pos, SignatureBuff, SIGNATURE_SIZE);
	//pos += SIGNATURE_SIZE;
	// Payload
	memcpy(pos, PayloadBuff, PayloadLen);
	pos += PayloadLen;
	//printf("***********%d",PayloadLen);
	pData[1] = ((PayloadLen >> 8) & 0xff);
	pData[2] = (PayloadLen & 0xfF);
	// Packet EOF
	//uint32_t Eof = FAWACP_PROTOCOL_EOF;
	//SOFEOFDeal(pos, Eof);
	//pos += DATA_SOFEOF_SIZE;

	//	int testlength = DATA_SOFEOF_SIZE + SIGNATURE_SIZE + DATA_SOFEOF_SIZE + PayloadLen;
	//	int datalen = (int )(pos - dataBuff);
	//	for(int i =0;i < testlength;i++)
	//	{
	//printf("data[%d]= %02x\n",i,dataBuff[i]);
	//	}

	//	printf("\n%s(%d): datalen = %d\n", datalen);
	//	for(int i =0;i < datalen;i++)
	//	{
	//		printf("%02x ", dataBuff[i]);
	//	}
	//	printf("\n");

	//if(NULL != SignatureBuff){
	//	free(SignatureBuff);
	//	SignatureBuff = NULL;
	//}
	if(NULL != PayloadBuff){
		free(PayloadBuff);
		PayloadBuff = NULL;
	}

	return (uint16_t)(pos-dataBuff);
}


/****************************************************************/
//
uint16_t	CFAWACP::Unpacket_AcpHeader(AcpAppHeader_t& Header, uint8_t *pData, int32_t DataLen)
{
	uint8_t *pos = pData;
	Header.Reserved_ID = (*pos >> 7) & 0x01;
	Header.PrivateFlag = (*pos >> 6) & 0x01;
	Header.Acp_AppID 	 = *pos++ & 0x3F;

	Header.Reserved_T = (*pos >> 7) & 0x01;
	Header.TestFlag = (*pos >> 6) & 0x01;
	Header.CryptFlag = (*pos >> 5) & 0x01;
	Header.MsgType = *pos++ & 0x1F;

	Header.VersionFlag = (*pos >> 7) & 0x01;
	Header.Version = (*pos >> 4) & 0x07;
	Header.MsgCtrlFlag = *pos++ & 0x0F;

	Header.MsgLength = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
	pos += 2;

	return 0;
}
//
uint16_t CFAWACP::unpackDataLenDeal(uint8_t *pTemp, uint16_t &DataLen)
{
	uint8_t *pos = pTemp;
	uint flag = (*pos >> 5) & 0x01;
	if(1 == flag){
		DataLen = ((pos[0] << 7) & 0x0F80) | (pos[1] & 0x7F);
		pos += 2;
	}else{
		DataLen = *pos++ & 0x1F;
	}
	return uint16_t(pos - pTemp);
}
//
uint16_t CFAWACP::Unpack_FAWACP_FrameData(uint8_t *pData, int32_t DataLen)
{
	uint8_t *pos = pData;
	uint8_t *PayloadBuff;
	//����ͷ
	AcpAppHeader_t Header;
	Unpacket_AcpHeader(Header, pos, DATA_HEADER_SIZE);

	uint8_t	applicationID = Header.Acp_AppID;
	int32_t payload_size = Header.MsgLength - DATA_HEADER_SIZE;
	uint8_t MsgType = Header.MsgType;
	FAWACPLOG("MsgLength == %d",Header.MsgLength);
	FAWACPLOG("Acp_AppID == %d",Header.Acp_AppID);
	FAWACPLOG("MsgType == %d",Header.MsgType);
	FAWACPLOG("CryptFlag == %d",Header.CryptFlag);

	if(DataLen != Header.MsgLength){
		FAWACPLOG("MsgLength error!\n");
		//return -1;
	}

	uint16_t Decrypt_len;
	PayloadBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(NULL == PayloadBuff){
		FAWACPLOG("malloc PayloadBuff error!");
		return -1;
	}

	/*for(uint16_t i = 0; i < DataLen; i++)

	  FAWACP_NO("data first %02x ", *(pData + i));

	  FAWACP_NO("\n\n");*/
#ifdef MQTT
	Header.CryptFlag  = 0;
#endif
	//Decrypt_len = Decrypt_AES128Data(ALLRootKey, (pos+DATA_HEADER_SIZE), payload_size, PayloadBuff, (AcpCryptFlag_E)Header.CryptFlag);
	//payload_size = Decrypt_len;
	//payload_size = 92;
	memcpy(PayloadBuff,(pos+DATA_HEADER_SIZE),payload_size);

	printf("jason add Unpack_FAWACP_FrameData  after  PayloadData length==%d\n",payload_size);

	for(uint16_t i = 0; i < payload_size; i++)
		printf("%02x ", *(PayloadBuff + i));

	printf("\n\n");

	// Packet PAYLOAD
	FAWACPLOG("unpack CFAWACP FrameData applicationID:%02x", applicationID);
	
	/*if(tboxInfo.operateionStatus.isGoToSleep == 0)
	  {
	  tboxInfo.operateionStatus.isGoToSleep = 1;
	  tboxInfo.operateionStatus.wakeupSource = 2;
	  lowPowerMode(1);
	  modem_ri_notify_mcu();
	  }*/

	
	//if(m_loginState != STATUS_LOGGIN_FINISH && applicationID != ACPApp_AuthenID)
	//return -1;

	switch (applicationID)
	{
		case ACPApp_AuthenID:
			UnpackData_AcpLoginAuthen(PayloadBuff, payload_size, MsgType);
			break;
		case ACPApp_HeartBeatID:
			break;
		case ACPApp_RemoteCtrlID:
			if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
			{
				tbox_log(3,MsgType);
			}
			FAWACPLOG("mqtt app 2\n");
			UnpackData_AcpRemoteCtrl(PayloadBuff, payload_size, MsgType);
			break;
		case ACPApp_QueryVehicleCondID:
			UnpackData_AcpQueryVehicleCond(PayloadBuff, payload_size, applicationID, MsgType);
			break;
		case ACPApp_RemoteConfigID:
			UnpackData_AcpRemoteConfig(PayloadBuff, payload_size, MsgType);
			break;
		case ACPApp_UpdateKeyID:	
			UnpackData_AcpUpdateRootKey(PayloadBuff, payload_size, MsgType);
			break;
		case ACPApp_RemoteUpgradeID:

			if(ftpupgrade)
			{
				Upgrade_CommandResponse = 1;
				//RespondTspPacket(ACPApp_RemoteUpgradeID, 2, AcpCryptFlag_IS, 0);				
				printf("jason add receive remote upgrade cmd form tsp,error because now upgading \r\n");
			}
			else
			{
				if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
				{
					ftpupgrade = true;
					UnpackData_AcpRemoteUpgrade(PayloadBuff, payload_size, MsgType);
					printf("jason add receive remote upgrade cmd form tsp\r\n");
					
				}
				else
				{
					Upgrade_CommandResponse = 1;
					///write_mqtt_log
					tbox_log(15,0);

				}
			}
			break;
		case ACPApp_SpondBleTokenID:
			UnpackData_AcpBleToken(PayloadBuff, payload_size, MsgType);
			break;
		case ACPApp_RemoteUpLoadLogID:

			break;
		case ACPApp_EmergencyDataID:
			break;
		case ACPApp_VehicleCondUploadID:
			UnpackData_AcpVehCondUpload(PayloadBuff, payload_size, MsgType);
			break;
		case ACPApp_GPSID: 	
			UnpackData_AcpQueryVehicleCond(PayloadBuff, payload_size, applicationID, MsgType);
			break;
		case ACPApp_VehicleAlarmID:
			break;
		case ACPApp_RemoteDiagnosticID:
			UnpackData_AcpRemoteDiagnostic(PayloadBuff, payload_size, MsgType);
			break;
		case ACPApp_CameraRecordID:
			//			UnpackData_AcpCameraCtrl(PayloadBuff, MsgType);
			/************************scy******************************/
			/*
			   cfawacp->RespondTspPacket(ACPApp_CameraRecordID, 3, 0, 0);
			   sleep(1);
			   if(!CamreaUpload)
			   {
			   CamreaUpload =true;
			   cfawacp->UnpackData_CameraUpload(PayloadBuff, payload_size, MsgType);

			   }

			   FAWACPLOG("#########\n");
			   sleep(1);
			   cfawacp->RespondTspPacket(ACPApp_CameraRecordID, 4, 0, 0);
			   FAWACPLOG("#########\n");
			   */
			/************************scy******************************/

			break;
		default:
			break;
	}
	if(NULL != PayloadBuff){
		free(PayloadBuff);
		PayloadBuff = NULL;
	}
	return 0;
} 

/***********************************************************************************
 ************************************************************************************/

uint16_t CFAWACP::Unpack_Gsensor_Data(uint8_t *pData, int32_t DataLen)
{
	uint8_t *pos = pData;
	uint8_t *PayloadBuff;

	json_object *jsonobj = NULL;
	json_object *tmpjson = NULL;

	jsonobj = json_tokener_parse(pos);
	json_object_object_get_ex(jsonobj , "eventid" , &tmpjson);
	FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));

	json_object_object_get_ex(jsonobj , "arg1" , &tmpjson);
	FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));

	json_object_object_get_ex(jsonobj , "arg2" , &tmpjson);
	FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));

	json_object_object_get_ex(jsonobj , "result" , &tmpjson);
	FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));

	json_object_object_get_ex(jsonobj , "payload" , &tmpjson);
	FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));

	memset(CameraGensorTest,0,sizeof(CameraGensorTest));
	strcpy(CameraGensorTest , json_object_get_string(tmpjson));
	FAWACPLOG("*********%s\n",CameraGensorTest);

	cfawacp->MQTTtimingReportingData(1, ACPApp_CameraRecordReplyID);
	json_object_put(tmpjson);
}



uint16_t CFAWACP::AuthApplyDataDeal(uint8_t *pTemp)
{
	uint8_t *pos = pTemp;
	*pos++ = AcpTBox_AuthApplyMsg.CarManufactureID.Element_LenInfo.Element_Len;
	//	*pos++ = AcpTBox_AuthApplyMsg.CarManufactureID.CarManufactureID;
	*pos++ =p_FAWACPInfo_Handle->VehicleCondData.CarIDState;
	*pos++ = AcpTBox_AuthApplyMsg.TU_VIN.Element_LenInfo.Element_Len;
	memcpy(pos, AcpTBox_AuthApplyMsg.TU_VIN.CarTU_ID, sizeof(AcpTBox_AuthApplyMsg.TU_VIN.CarTU_ID));
	pos += sizeof(AcpTBox_AuthApplyMsg.TU_VIN.CarTU_ID);
	memcpy(pos,AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IMEI,sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IMEI));
	pos += sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IMEI);
	memcpy(pos,AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.ICCID,sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.ICCID));
	pos += sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.ICCID);
	return (uint16_t)(pos-pTemp);
}

uint16_t CFAWACP::CTMsgDataDeal(uint8_t *pTemp)
{
	uint8_t *pos = pTemp;

	*pos++ = AcpTBox_CTMsg.Len_High.Element_Len;
	*pos++ = AcpTBox_CTMsg.Len_Low.Element_Len_Extend;

	memcpy(pos, AcpTBox_CTMsg.Rand1CT_TBox, sizeof(AcpTBox_CTMsg.Rand1CT_TBox));
	pos += sizeof(AcpTBox_CTMsg.Rand1CT_TBox);
	memcpy(pos, AcpTBox_CTMsg.Rand2_Tbox, sizeof(AcpTBox_CTMsg.Rand2_Tbox));
	pos += sizeof(AcpTBox_CTMsg.Rand2_Tbox);
	return (uint16_t)(pos-pTemp);
}

uint16_t CFAWACP::RSMsgDataDeal(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;

	*pos++ = AcpTBox_RSMsg.LenInfo.Element_Len;

	memcpy(pos, AcpTBox_RSMsg.Rand3_TBox, sizeof(AcpTBox_RSMsg.Rand3_TBox));
	pos += sizeof(AcpTBox_RSMsg.Rand3_TBox);
	*pos++ = AcpTBox_RSMsg.RS_Tbox;

	return (uint16_t)(pos-pTemp);
}

uint16_t CFAWACP::AuthReadyMsgDataDeal(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;

	*pos++ = AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Element_Len_High.Element_Len;
	*pos++ = AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Element_Len_Low.Element_Len_Extend;
	memcpy(pos,AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Vin,sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Vin));
	pos += sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.Vin);
	memcpy(pos,AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IMEI,sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IMEI));
	pos += sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IMEI);
	memcpy(pos,AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.ICCID,sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.ICCID));
	pos += sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.ICCID);
	memcpy(pos,AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IVISerialNumber,sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IVISerialNumber));
	pos += sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.IVISerialNumber);
	memcpy(pos,AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.TBoxSerialNumber,sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.TBoxSerialNumber));
	pos += sizeof(AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.TBoxSerialNumber);
	//*pos ++ = AcpTBox_AuthReadyMsg.AuthReadyMsg_VehicleDescriptor.RawData & 0xFF;

	return (uint16_t)(pos-pTemp);
}

void CFAWACP::SOFEOFDeal(uint8_t* pTemp, uint32_t nTempNum)
{
	*pTemp++ = (nTempNum >> 24)&0xff;
	*pTemp++ = (nTempNum >> 16)&0xff;
	*pTemp++ = (nTempNum >> 8)&0xff;
	*pTemp++ = (nTempNum >> 0)&0xff;
}

uint16_t CFAWACP::addDataLen(uint8_t *pTemp, uint16_t DataLen, uint8_t Identifier, uint8_t flag)//301
{
	uint8_t *pos = pTemp;
	ElementLen_t ElementLen;
	if(1 == flag){
		ElementLen.Element_Len_High.elementLenBit.Identifier = Identifier;
		ElementLen.Element_Len_High.elementLenBit.MoreFlag = flag;
		ElementLen.Element_Len_High.elementLenBit.DataLen = ((DataLen << 4) >> 11) & 0xFF1F;

		ElementLen.Element_Len_Low.elementLenBit.MoreFlag = 0;
		ElementLen.Element_Len_Low.elementLenBit.DataLen = ((DataLen << 9) >> 9) & 0xFF7F;

		*pos++ = ElementLen.Element_Len_High.Element_Len;
		*pos++ = ElementLen.Element_Len_Low.Element_Len_Extend;
	}else{
		ElementLen.Element_Len_High.elementLenBit.Identifier = Identifier;
		ElementLen.Element_Len_High.elementLenBit.MoreFlag = flag;
		ElementLen.Element_Len_High.elementLenBit.DataLen = DataLen;

		*pos++ = ElementLen.Element_Len_High.Element_Len;
	}

	return uint16_t(pos - pTemp);
}


void CFAWACP::TimeDeal(uint8_t* pTemp, uint8_t isFlagLen)
{
	AcpTimeStamp_t  AcpTimeStamp;  //TimeStampʱ���
	memset(&AcpTimeStamp, 0, sizeof(AcpTimeStamp));
	if(isFlagLen == 1)
	{
		AcpTimeStamp.Element_LenInfo.Element_Len = 6;
		*pTemp++ = AcpTimeStamp.Element_LenInfo.Element_Len;
	}		
	TimeStampPart(pTemp, &(AcpTimeStamp.TimeStampInfo));
}

void CFAWACP::TimeStampPart(uint8_t* pTemp, TimeStamp_t *pTimeStamp)
{
	if(NULL != pTemp && NULL != pTimeStamp)
	{
		struct tm *p_tm = NULL; 
		time_t tmp_time;
		tmp_time = time(NULL);
		p_tm = gmtime(&tmp_time);
		struct timeval Time;
		gettimeofday(&Time, NULL);

		if(m_loginState == STATUS_LOGGIN_ING && p_FAWACPInfo_Handle->VehicleCondData.GPSData.GPSState == 0){
			pTimeStamp->Year = m_tspTimestamp.Year;
			pTimeStamp->Month = m_tspTimestamp.Month;
			pTimeStamp->Day = m_tspTimestamp.Day;
			pTimeStamp->Hour = m_tspTimestamp.Hour;
			pTimeStamp->Minutes = m_tspTimestamp.Minutes;
			pTimeStamp->Seconds = m_tspTimestamp.Seconds;
			pTimeStamp->msSeconds = m_tspTimestamp.msSeconds;
		}else{
			pTimeStamp->Year = p_tm->tm_year - 90;
			pTimeStamp->Month = p_tm->tm_mon + 1;
			pTimeStamp->Day = p_tm->tm_mday;
			pTimeStamp->Hour = p_tm->tm_hour;
			pTimeStamp->Minutes = p_tm->tm_min;
			pTimeStamp->Seconds = p_tm->tm_sec;
			pTimeStamp->msSeconds = Time.tv_usec / 1000; 
		}
		*pTemp++ = ((pTimeStamp->Year)<<2)|(((pTimeStamp->Month)>>2)&0x03);
		*pTemp++ = (((pTimeStamp->Month)<<6)&0xc0)|((pTimeStamp->Day)<<1)|(((pTimeStamp->Hour)>>4)&0x01);
		*pTemp++ = (((pTimeStamp->Hour)<<4)&0xf0)|(((pTimeStamp->Minutes)>>2)&0x0f);
		*pTemp++ = (((pTimeStamp->Minutes)<<6)&0xc0)|(pTimeStamp->Seconds);
		*pTemp++ = (((pTimeStamp->msSeconds)>>8)&0xff);
		*pTemp++ = (((pTimeStamp->msSeconds)>>0)&0xff);
	}
	//printf("send to tsp time mon is %d day is %d\n",pTimeStamp->Month,pTimeStamp->Day );
}
//
void CFAWACP::AuthTokenDeal(uint8_t* pTemp)
{
	uint16_t AuthTokenlen = 4;

	*pTemp++ = AuthTokenlen & 0xFF;
	memcpy(pTemp, AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken, sizeof(AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken));
}
void CFAWACP::BleTokenDeal(uint8_t* pTemp)
{
	memcpy(pTemp, BleToken, sizeof(BleToken));
}
//
void CFAWACP::SourceDeal(uint8_t* pTemp, uint8_t TspSourceID)
{
	AcpSourceID_t AcpSourceID;
	memset(&AcpSourceID, 0, sizeof(AcpSourceID));
	AcpSourceID.Element_LenInfo.Element_Len = 1;
	AcpSourceID.SourceID = TspSourceID;

	*pTemp++ = AcpSourceID.Element_LenInfo.Element_Len;
	*pTemp++ = AcpSourceID.SourceID;
}
//
void CFAWACP::ErrorCodeDeal(uint8_t* pTemp, uint8_t nErrorCode)
{
	AcpErrorCode_t  AcpErrorCode;			//ErrorCode
	memset(&AcpErrorCode, 0, sizeof(AcpErrorCode));
	AcpErrorCode.Element_LenInfo.Element_Len = 1;
	AcpErrorCode.ErrorCode = nErrorCode; 

	//*pTemp++ = AcpErrorCode.Element_LenInfo.Element_Len;
	*pTemp++ = AcpErrorCode.ErrorCode;
}


uint16_t CFAWACP::SignCodeDeal(uint8_t* pTemp, uint8_t inValid, uint16_t SignCde)
{
	uint8_t *pos = pTemp;

	SignCodeBit_U SignCode;
	memset(&SignCode, 0, sizeof(SignCode));

	SignCode.SignCodeBit = inValid;
	SignCode.SignCodeBit = SignCode.SignCodeBit << 13 | SignCde;
	
	*pos++ = (SignCode.SignCodeBit >> 8) & 0xFF;
	*pos++ = (SignCode.SignCodeBit >> 0) & 0xFF;

	return uint16_t(pos - pTemp);
}
uint16_t CFAWACP::add_GPSData(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_GPSID);
	//printf("$$$$$$$$$$$%02x\n",p_FAWACPInfo_Handle->VehicleCondData.GPSData.GPSState);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.GPSState <<5 ) & 0x60)| ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude >> 24 ) & 0x1F);//�Ӹߵ�ַ��ʼ���
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude >>16) & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitudeState << 6 ) & 0xC0) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude >> 23 ) & 0x3F);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude >> 15) & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude >> 7) & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude << 1) & 0xFE) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitudeState >> 1) & 0X01);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitudeState << 7) & 0X80) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.altitude >> 7) & 0X7F);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.altitude << 1) & 0xFE) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.year >> 5) & 0X01);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.year << 3) & 0XF8) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.month >> 1) & 0X07);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.month << 7) & 0X80) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.day << 2) & 0X7C) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.hour >> 3) & 0X03);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.hour << 5) & 0XE0) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.minute >> 1) & 0X1F);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.minute << 7) & 0X80) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.second << 1) & 0X7E) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.degree >> 8) & 0X01);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.GPSData.degree & 0XFF);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addRemainedOil(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_RemainedOilID);

	*pos++ = 0; //ACP
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.RemainedOil.RemainedOilValue;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addOdometer(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_OdometerID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.Odometer >> 24) & 0XFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.Odometer >> 16) & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.Odometer >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.Odometer & 0xFF);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addBattery(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	//	int i, datalen;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_BatteryID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.Battery >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.Battery & 0xFF);
#if 0
	datalen = pos - pTemp;
	printf("%s(%d): datalen = %d\t", __func__, __LINE__, datalen);
	printf("Battery: %d\n", p_FAWACPInfo_Handle->VehicleCondData.Battery);

	for(i = 0; i<datalen; i++)
	{
		printf("%02x ", pTemp[i]);
	}

	printf("\n");
#endif
	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addCurrentSpeed(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_CurrentSpeedID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.CurrentSpeed >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.CurrentSpeed & 0xFF);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addLTAverageSpeed(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_LTAverageSpeedID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.LTAverageSpeed >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.LTAverageSpeed & 0xFF);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addSTAverageSpeed(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_STAverageSpeedID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.STAverageSpeed >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.STAverageSpeed & 0xFF);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addLTAverageOil(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_LTAverageOilID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.LTAverageOil >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.LTAverageOil & 0xFF);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addSTAverageOil(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_STAverageOilID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.STAverageOil >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.STAverageOil & 0xFF);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addCarDoorState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_CarDoorStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.CarDoorState;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addCarLockState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_CarLockState);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarLockState.CarLockState;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addSunroofState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_SunroofStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.sunroofState;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addWindowState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_WindowStateID);

	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.WindowState.WindowState >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.WindowState.WindowState  & 0xFF;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addCarlampState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_CarlampStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarlampState.CarlampState;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addTyreState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_TyreStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftfrontTemperature;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightfrontTemperature;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftrearTemperature;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightrearTemperature;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftfrontTyrePress;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightfrontTyrePress;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.leftrearTyrePress;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.TyreState.rightrearTyrePress;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addVerTboxMCU(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	//
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_VerTboxMCUID);

	memcpy(pos,p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU, sizeof(p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU));
	pos += sizeof(p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU);

	return uint16_t(pos - pTemp);
}

//
uint16_t CFAWACP::addVerTboxBLE(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	//
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_VerTboxBLEID);

//	printf("%s(%d): Bluetooth version: %s\n", __func__, __LINE__, p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion);
	memcpy(pos,p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion, sizeof(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion));
	pos += sizeof(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion);

	return uint16_t(pos - pTemp);
}


//
uint16_t CFAWACP::addEngineState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	//
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_EngineStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.EngineState;

	return uint16_t(pos - pTemp);
}
//
uint16_t CFAWACP::addWheelState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	//
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_WheelStateID);

	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.WheelState.WheelState >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.WheelState.WheelState & 0xFF;

	return uint16_t(pos - pTemp);
}
//
uint16_t CFAWACP::addPastRecordSpeed(uint8_t* pTemp)
{
	int Id = 0;
	int dataLen = 0;
	uint8_t *pos = pTemp;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL){
		FAWACPLOG("malloc dataBuff error!");
	}
	uint8_t *PayloadBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(PayloadBuff == NULL){
		FAWACPLOG("malloc PayloadBuff error!");
	}
	//
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_PastRecordSpeedID);

	//
	Id = pSqliteDatabase->queryMaxID();
	for(int i = 0; i < 19; i++)
	{
		memset(dataBuff, 0, sizeof(dataBuff));
		memset(PayloadBuff, 0, sizeof(PayloadBuff));
		//
		pSqliteDatabase->readData(dataBuff, &dataLen, Id);

		if(Id-- <= 0)
			p_FAWACPInfo_Handle->VehicleCondData.PastRecordSpeed[i] = 0;
		else
		{
			Decrypt_AES128Data(ALLRootKey, (dataBuff + SIGNATURE_SIZE + DATA_SOFEOF_SIZE), (dataLen - SIGNATURE_SIZE - DATA_SOFEOF_SIZE*2), PayloadBuff, AcpCryptFlag_IS);
			p_FAWACPInfo_Handle->VehicleCondData.PastRecordSpeed[i] = (PayloadBuff[56] << 8) + PayloadBuff[57];
		}
		*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.PastRecordSpeed[i] >> 8) & 0xFF;
		*pos++ = p_FAWACPInfo_Handle->VehicleCondData.PastRecordSpeed[i] & 0xFF;
	}
	if(NULL != dataBuff){
		free(dataBuff);
		dataBuff = NULL;
	}
	if(NULL != PayloadBuff){
		free(PayloadBuff);
		PayloadBuff = NULL;
	}
	return uint16_t(pos - pTemp);
}
//
uint16_t CFAWACP::addPastRecordWheelState(uint8_t* pTemp)
{
	int Id = 0;
	int ret = 0;
	int dataLen = 0;
	uint8_t *pos = pTemp;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL)
	{
		FAWACPLOG("malloc dataBuff error!");
	}
	uint8_t *PayloadBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(PayloadBuff == NULL)
	{
		FAWACPLOG("malloc PayloadBuff error!");
	}

	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_PastRecordWheelStateID);

	Id = pSqliteDatabase->queryMaxID();
	for(int i = 0;i < 19; i++)
	{
		memset(dataBuff, 0, sizeof(dataBuff));
		memset(PayloadBuff, 0, sizeof(PayloadBuff));

		pSqliteDatabase->readData(dataBuff, &dataLen, Id);
		if(Id-- <= 0)
			p_FAWACPInfo_Handle->VehicleCondData.PastRecordWheel[i].WheelState = 0;
		else
		{
			ret = Decrypt_AES128Data(ALLRootKey, (dataBuff + 29), (dataLen - 33), PayloadBuff, AcpCryptFlag_IS);
#if 0
			printf("Is Decrypt data buff ret = %d:::\n", ret);		
			for(int i = 0; i< ret; i++)
				printf("%02x ", dataBuff[29+i]);
			printf("\n\n\n");
			printf("Is Decrypt Read Data ret = %d:::\n", ret);		
			for(int i = 0; i< ret; i++)
				printf("%02x ", PayloadBuff[i]);
			printf("\n\n\n");
#endif
			p_FAWACPInfo_Handle->VehicleCondData.PastRecordWheel[i].WheelState = (dataBuff[123] << 8) + dataBuff[124];
		}
		*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.PastRecordWheel[i].WheelState >> 8) & 0xFF;
		*pos++ = p_FAWACPInfo_Handle->VehicleCondData.PastRecordWheel[i].WheelState & 0xFF;
	}
	if(NULL != dataBuff){
		free(dataBuff);
		dataBuff = NULL;
	}
	if(NULL != PayloadBuff){
		free(PayloadBuff);
		PayloadBuff = NULL;
	}
	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addEngineSpeed(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_EngineSpeedID);

	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.EngineSpeed >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.EngineSpeed & 0xFF;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addGearState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_GearStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.Gearstate;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addHandBrakeState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_HandBrakeStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.HandbrakeState;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addParkingState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_ParkingStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.ParkingState;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addPowerSupplyMode(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_PowerSupplyModeID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.KeyState;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addSafetyBelt(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_SafetyBeltID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.Safetybeltstate;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addRemainUpkeepMileage(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_RemainUpkeepMileageID);

	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.RemainUpkeepMileage >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.RemainUpkeepMileage & 0xFF;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addAirconditionerInfo(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_AirconditionerInfoID);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.Temperature >> 5) & 0x03);


	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.Temperature << 3) & 0xF8) |
		(p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.blowingMode  & 0x07);

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.blowingLevel << 5) & 0XE0) |
		((p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.inOutCirculateState << 4) & 0X10) |
		((p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.defrostState << 3) & 0X08) |
		((p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.autoState << 2) & 0X04) |
		((p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.compressorState << 1) & 0X02) |
		(p_FAWACPInfo_Handle->VehicleCondData.AirconditionerInfo.airconditionerState & 0X01);

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addKeepingstateTime(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_KeepingstateTimeID);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.overspeedTime >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.overspeedTime & 0XFF;	
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.wheelTime;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.decelerateTime;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.KeepingstateTime.accelerateTime;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addPowerCellsState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	//	int i, datalen;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_PowerCellsStateID);

	*pos++ = 0;
	*pos++ = 0;
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.RemaineBattery >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.RemaineBattery & 0XFF;
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.electricity >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.electricity & 0XFF;
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.voltage >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.voltage & 0XFF;

#if 0
	datalen = pos-pTemp;

	printf("%s(%d): datalen = %d\t", __func__, __LINE__, datalen);
	printf("RemaineBattery: %d\t", p_FAWACPInfo_Handle->VehicleCondData.RemaineBattery);
	printf("electrictity: %d\t", p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.electricity);
	printf("voltage: %d\n", p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.voltage);

	for(i = 0; i<datalen; i++)
	{
		printf("%02x ", pTemp[i]);
	}

	printf("\n");
#endif
	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addChargeState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;	
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_ChargeStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.ChargeState;


	return uint16_t(pos - pTemp);
}
uint16_t CFAWACP::addVerTboxOS(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_VerTboxOSID);
	//memcpy(pos, p_FAWACPInfo_Handle->VehicleCondData.VerTboxOS, 12);
	//memcpy(pos, "MPU_V1.1.223",sizeof(p_FAWACPInfo_Handle->VehicleCondData.VerTboxOS));
	memcpy(pos, TBOX_4G_VERSION, 5);
	pos += 5;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU[7];
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU[9];
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU[10];
	*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion[7];
	*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion[9];
	*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion[10];
	*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion[11];

	//memcpy(pos, "MPU_V1.1.089",12);
	//pos += sizeof(p_FAWACPInfo_Handle->VehicleCondData.VerTboxOS);
	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addVerIVI(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_VerIVIID);

	memcpy(pos, p_FAWACPInfo_Handle->VehicleCondData.VerIVI, sizeof(p_FAWACPInfo_Handle->VehicleCondData.VerIVI));
	pos += sizeof(p_FAWACPInfo_Handle->VehicleCondData.VerIVI);

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addChargeConnectState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_ChargeConnetStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.ChargeConnectState;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addBrakePedalSwitch(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_BrakePedalSwitchID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.BrakePedalSwitch;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addAcceleraPedalSwitch(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;

	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_AcceleraPedalSwitchID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.AcceleraPedalSwitch;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addYaWSensorInfoSwitch(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_YaWSensorInfoSwitchID);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.TransverseAccele >> 8) & 0x0F;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.TransverseAccele & 0xFF;
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.LongituAccele >> 8) & 0x0F;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.LongituAccele & 0xFF;
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.YawVelocity >> 8) & 0x0F;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.YaWSensorInfoSwitch.YawVelocity & 0xFF;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addAmbientTemperat(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_AmbientTemperatID);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.AmbientTemperat.AmbientTemperat >> 5) & 0x07;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.AmbientTemperat.AmbientTemperat & 0xFF;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addPureElecRelayState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_PureElecRelayID);

	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.RechargeNegaRelayCoilState << 4) | 
		(p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.RechargePositRelayCoilState << 3) |
		(p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.PrefillRelayCoilState << 2) |
		(p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.MainNegaRelayCoilState << 1) |
		p_FAWACPInfo_Handle->VehicleCondData.PureElecRelayState.MainPositRelayCoilState;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addResidualRange(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_RemainderRangeID);

	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.ResidualRange >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.ResidualRange & 0xFF;

	return uint16_t(pos - pTemp);
}



uint16_t CFAWACP::addNewEnergyHeatManage(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;

	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_NewEnergyHeatManageID);
	
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.NewEnergyHeatManage.Motor2CoolRequest << 4) |
		(p_FAWACPInfo_Handle->VehicleCondData.NewEnergyHeatManage.Motor1CoolRequest << 1) |
		(p_FAWACPInfo_Handle->VehicleCondData.NewEnergyHeatManage.BatteryHeatRequest);

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addVehWorkMode(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_VehWorkModeID);
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.VehWorkMode.VehWorkMode;

	return uint16_t(pos - pTemp);
}


uint16_t CFAWACP::addMotorWorkState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_MotorWorkStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.EcEngineState;

	return uint16_t(pos - pTemp);
}



uint16_t CFAWACP::addMotorSpeed(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;

	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_MotorSpeedID);

	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.EcEngineSpeed >> 8) & 0xFF;
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.EcEngineSpeed & 0xFF;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addPowerState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;

	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_PowerdID);

	
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.PowerState;

	return uint16_t(pos - pTemp);
}



uint16_t CFAWACP::addNodeMissState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_GwNodeMissID);
	
	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarGwNodeMiss;

	return uint16_t(pos - pTemp);
}



uint16_t CFAWACP::addCarType(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_CARTYPEID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarIDState;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addIccid(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_ICCID);
	memset(p_FAWACPInfo_Handle->ICCID, 0, sizeof(p_FAWACPInfo_Handle->ICCID));
	dataPool->getPara(SIM_ICCID_INFO, p_FAWACPInfo_Handle->ICCID, sizeof(p_FAWACPInfo_Handle->ICCID));
	//FAWACPLOG("iccid is %s\n",p_FAWACPInfo_Handle->ICCID);
	memcpy(pos,p_FAWACPInfo_Handle->ICCID,sizeof(p_FAWACPInfo_Handle->ICCID));

	pos += sizeof(p_FAWACPInfo_Handle->ICCID);

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addRentMode(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_RentModeID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.RentInfo;

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addGPSDetail(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_GPSDeatilID);
	memcpy(pos,p_FAWACPInfo_Handle->VehicleCondData.GPSDetailInfo,sizeof(p_FAWACPInfo_Handle->VehicleCondData.GPSDetailInfo));

	pos += sizeof(p_FAWACPInfo_Handle->VehicleCondData.GPSDetailInfo);

	return uint16_t(pos - pTemp);
}

uint16_t CFAWACP::addSignalValue(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_4GSIGNALVALUEID);

	*pos++ = tboxInfo.networkStatus.signalStrength;

	return uint16_t(pos - pTemp);
}



uint16_t CFAWACP::addHighVoltageState(uint8_t* pTemp)
{
	uint8_t *pos = pTemp;
	pos += SignCodeDeal(pos, 0, ACPSIGNCODE_HighVolageStateID);

	*pos++ = p_FAWACPInfo_Handle->VehicleCondData.HighVoltageState;

	return uint16_t(pos - pTemp);
}




uint16_t CFAWACP::RemoteCtrlCommandDeal(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;	

	uint16_t tempLen = (m_AcpRemoteCtrlList.SubitemTotal *2) +1;
	//pos += addDataLen(pos, tempLen, 0, 1); 

	*pos++ = m_AcpRemoteCtrlList.SubitemTotal;
	for(uint16_t i = 0; i < m_AcpRemoteCtrlList.SubitemTotal; i++)
	{
		*pos++ = m_AcpRemoteCtrlList.SubitemCode[i] & 0xFF;	

		printf("\n%s(%d): ctrl item: %d\n", __func__, __LINE__, m_AcpRemoteCtrlList.SubitemCode[i]);
		switch(m_AcpRemoteCtrlList.SubitemCode[i])
		{
			case VehicleBody_LockID:
				if(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Lock != 0)
					p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Lock = 0x01;
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Lock;
				printf("\ndoor %d\n", *(pos-1));
				break;
			case VehicleBody_TrackingCarID:
				if(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_TrackingCar != 0x00)
					p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_TrackingCar = 0x01;
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_TrackingCar;
				break;
			case Power_SwitchID:
				if(p_FAWACPInfo_Handle->RemoteControlData.PowerState.PowerState_Switch != 0)
					p_FAWACPInfo_Handle->RemoteControlData.PowerState.PowerState_Switch = 0x01;
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.PowerState.PowerState_Switch;
				break;
			case EngineState_SwitchID:
				if(p_FAWACPInfo_Handle->RemoteControlData.EngineState.EngineState_Switch != 0x00)
					p_FAWACPInfo_Handle->RemoteControlData.EngineState.EngineState_Switch = 0x01;
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.EngineState.EngineState_Switch;
				break;
			case SleepTime_ID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.SleepTime;// 
				break;
			case SleepNow_ID:
				if(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.SleepNow != 0x00)
					p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.SleepNow = 0x01;
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.SleepNow;//
				break;
			case ReportTime_ID:
				//*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval;//
				*pos++ = 0;
				break;

			case CheckBleState_ID:
				memcpy(pos,p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleMAC, 6);
				pos+=6;
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleWorkState;//
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleConnectState;//
				memcpy(pos,p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW, 6);
				pos+=6;
				memcpy(pos,p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion, 12);
				pos+=12;
				break;
			case BroadCase_ID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleBKState;//
				break;
			case SendPW_ID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePWState;//
				break;
			case SetCar_ID:
				//*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarIDState;//
				*pos++ = 0;
				break;
			case SetLowVc_ID:
				//*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarIDState;//
				*pos++ = 0;
				break;

			case ResetEC3_ID:
				//*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarIDState;//
				*pos++ = 0;
				break;

			case ResetVin_ID:
				//*pos++ = p_FAWACPInfo_Handle->VehicleCondData.CarIDState;//
				*pos++ = 0;
				break;

			case ResetTbox_ID:
				*pos++ = 0;
				break;

			case WakeMCU_ID:
				*pos++ = 0;
				break;

			case OpenCloseLog_ID:
				*pos++ = 0;
				break;

			case UploadLog_ID:
				*pos++ = 0;
				break;
			case UploadGPSInfo_ID:
				*pos++ = GPSINFO.Antenna_State;
				*pos++ = GPSINFO.Satelliate_Number;
				for(int i = 0; i < GPSINFO.Satelliate_Number; i++)
				{
					*pos++ = GPSINFO.Satelliate[i].Satelliate_ID;
					*pos++ = GPSINFO.Satelliate[i].Satelliate_RSSI;
				}
				break;
			case VehicleBody_WindowID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Window;
				break;
			case VehicleBody_SunroofID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Sunroof;
				break;

			case VehicleBody_LowbeamID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_Lowbeam;
				break;
			case VehicleBody_LuggageCarID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.VehicleBody_LuggageCar;
				break;			
			case Airconditioner_ControlID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Control.BitType;
				break;
			case Airconditioner_CompressorSwitchID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_CompressorSwitch.BitType;
				break;
			case Airconditioner_TemperatureID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Temperature.BitType;
				break;
			case Airconditioner_SetAirVolumeID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_SetAirVolume.BitType;
				break;
			case Airconditioner_FrontDefrostSwitchID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_FrontDefrostSwitch.BitType;
				break;
			case Airconditioner_HeatedrearID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_Heatedrear.BitType;
				break;
			case Airconditioner_BlowingModeID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_BlowingMode.BitType;
				break;
			case Airconditioner_InOutCirculateID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_InOutCirculate.BitType;
				break;
			case Airconditioner_AutoSwitchID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.Airconditioner.Airconditioner_AutoSwitch.BitType;//Auto
				break;

			case VehicleSeat_DrivingSeatID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleSeat.VehicleSeat_DrivingSeat;
				break;
			case VehicleSeat_CopilotseatID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleSeat.VehicleSeat_Copilotseat;
				break;
			case VehicleSeat_DrivingSeatMomeryID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleSeat.VehicleSeat_DrivingSeatMemory;
				break;
			case VehicleChargeMode_ImmediateID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleCharge.VehicleCharge_Immediate;
				break;
			case VehicleChargeMode_AppointmentID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleCharge.VehicleCharge_Appointment;
				break;
			case VehicleChargeMode_EmergencyCharg:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleCharge.VehicleCharge_EmergenCharg;
				break;
			case VehicleWIFIStatusID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleAutopark.VehicleWifiStatus;//WIFI
				break;
			case VehicleAutoOUTID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.VehicleAutopark.VehicleAutoOut;
				break;
			case FeatureConfig_StateID:
				*pos++ = p_FAWACPInfo_Handle->RemoteControlData.FunctionConfigStatus;	
				break;
			default:
				break;
		}
	}
	if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
	{
		tbox_log(6,m_AcpRemoteCtrlList.SubitemCode[0]);
	}
	return uint16_t(pos - dataBuff);
}



uint16_t CFAWACP::ReportVehicleCondDeal(uint8_t* dataBuff, uint8_t flag)
{
	uint16_t tempLen = 0;
	static uint8_t CollectPacketSN = 1;
	uint16_t signCodeTotal = 0;
	uint8_t *pos = dataBuff;
	uint8_t *pData = dataBuff;

	//*pos++ = 0;
	//*pos++ = 0;
	*pos++ = m_SendMode;//AcpVehicleCondition.SendMode;
	if(CollectPacketSN == 254)
		CollectPacketSN = 1;
	*pos++ = CollectPacketSN++;
	TimeDeal(pos, 0);
	pos += DATA_TIMESTAMP_SIZE - 1;
	if(flag == 0)
		signCodeTotal = MAX_SIGNCODE_TOTAL;
	else if(flag == 1)
		signCodeTotal = MAX_SIGNCODE_TOTAL - 2 + MAX_FAULT_SIGNA + MAX_DRIVEACTION_SIGNA;
	else if(flag == 2)
		signCodeTotal = MAX_SIGNCODE_TOTAL - 2 + MAX_DRIVEACTION_SIGNA;
	else
		signCodeTotal = MAX_SIGNCODE_TOTAL - 2;

	*pos++	= (signCodeTotal >> 8) & 0xFF;
	*pos++	= signCodeTotal & 0xFF;

#if 0
	p_FAWACPInfo_Handle->VehicleCondData.RemainedOil.RemainedOilValue = 99;
	p_FAWACPInfo_Handle->VehicleCondData.Odometer = 123456;
	p_FAWACPInfo_Handle->VehicleCondData.CurrentSpeed = 11111;
	p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.CarDoorState =127;
	p_FAWACPInfo_Handle->VehicleCondData.CarLockState.CarLockState = 127;
	p_FAWACPInfo_Handle->VehicleCondData.sunroofState = 1;
	p_FAWACPInfo_Handle->VehicleCondData.WindowState.WindowState = 0x7fff;
	p_FAWACPInfo_Handle->VehicleCondData.CarlampState.CarlampState = 127;
	p_FAWACPInfo_Handle->VehicleCondData.EngineState = 2;
	p_FAWACPInfo_Handle->VehicleCondData.EngineSpeed = 12345;
	p_FAWACPInfo_Handle->VehicleCondData.Gearstate =3;
	p_FAWACPInfo_Handle->VehicleCondData.HandbrakeState = 1;
	p_FAWACPInfo_Handle->VehicleCondData.KeyState =1;
	p_FAWACPInfo_Handle->VehicleCondData.ChargeState = 1;
	p_FAWACPInfo_Handle->VehicleCondData.ChargeConnectState = 1;
	p_FAWACPInfo_Handle->VehicleCondData.BrakePedalSwitch = 1;
	p_FAWACPInfo_Handle->VehicleCondData.ResidualRange = 12345;
	p_FAWACPInfo_Handle->VehicleCondData.EcEngineState = 1;
	p_FAWACPInfo_Handle->VehicleCondData.EcEngineSpeed = 12345;
	p_FAWACPInfo_Handle->VehicleCondData.PowerState = 1;
	p_FAWACPInfo_Handle->VehicleCondData.RemaineBattery = 666;
	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.electricity = 5000;
	p_FAWACPInfo_Handle->VehicleCondData.PowerCellsState.voltage = 333;



#endif
	for(uint16_t i = 0; i < MAX_SIGNCODE_TOTAL; i++)
	{
		switch(i)
		{
			case ACPSIGNCODE_GPSID:
				pos += add_GPSData(pos);				
				break;
			case ACPSIGNCODE_RemainedOilID:
				pos += addRemainedOil(pos);
				break;
			case ACPSIGNCODE_OdometerID:
				pos += addOdometer(pos);
				break;
			case ACPSIGNCODE_BatteryID:
				pos += addBattery(pos);
				break;
			case ACPSIGNCODE_CurrentSpeedID:
				pos += addCurrentSpeed(pos);
				break;
			case ACPSIGNCODE_CarDoorStateID:
				pos += addCarDoorState(pos);
				break;
			case ACPSIGNCODE_CarLockState:
				pos += addCarLockState(pos);
				break;
			case ACPSIGNCODE_SunroofStateID:
				pos += addSunroofState(pos);
				break;
			case ACPSIGNCODE_WindowStateID:
				pos += addWindowState(pos);
				break;
			case ACPSIGNCODE_CarlampStateID:
				pos += addCarlampState(pos);
				break;
			case ACPSIGNCODE_EngineStateID:
				pos += addEngineState(pos);
				break;
			case ACPSIGNCODE_EngineSpeedID:
				pos += addEngineSpeed(pos);
				break;
			case ACPSIGNCODE_GearStateID:
				pos += addGearState(pos);
				break;
			case ACPSIGNCODE_HandBrakeStateID:
				pos += addHandBrakeState(pos);
				break;
			case ACPSIGNCODE_PowerSupplyModeID:
				pos += addPowerSupplyMode(pos);
				break;
			case ACPSIGNCODE_PowerCellsStateID:
				pos += addPowerCellsState(pos);
				break;
			case ACPSIGNCODE_ChargeStateID:
				pos += addChargeState(pos);
				break;
			case ACPSIGNCODE_ChargeConnetStateID:
				pos += addChargeConnectState(pos);
				break;
			case ACPSIGNCODE_BrakePedalSwitchID:
				pos += addBrakePedalSwitch(pos);	
				break;
			case ACPSIGNCODE_RemainderRangeID:
				pos += addResidualRange(pos);
				break;
			case ACPSIGNCODE_SafetyBeltID:
				pos += addSafetyBelt(pos);
				break;
			case ACPSIGNCODE_MotorWorkStateID:
				pos += addMotorWorkState(pos);
				break;
			case ACPSIGNCODE_MotorSpeedID:
				pos += addMotorSpeed(pos);
				break;
			case ACPSIGNCODE_PowerdID:
				pos += addPowerState(pos);
				break;
			case ACPSIGNCODE_VerTboxOSID:
				pos += addVerTboxOS(pos);
				break;
			case ACPSIGNCODE_VerTboxMCUID:
				pos += addVerTboxMCU(pos);
				break;

			case ACPSIGNCODE_VerTboxBLEID:
				pos += addVerTboxBLE(pos);
				break;

			case ACPSIGNCODE_GwNodeMissID:
				pos += addNodeMissState(pos);
				break;

			case ACPSIGNCODE_4GSIGNALVALUEID:
				pos += addSignalValue(pos);
				break;


			case ACPSIGNCODE_CARTYPEID:
				pos += addCarType(pos);
				break;

			case ACPSIGNCODE_ICCID:
				pos += addIccid(pos);
				break;

			case ACPSIGNCODE_RentModeID:
				pos += addRentMode(pos);
				break;

			case ACPSIGNCODE_GPSDeatilID:
				pos += addGPSDetail(pos);
				break;


			case ACPSIGNCODE_LTAverageSpeedID:
				pos += addLTAverageSpeed(pos);
				break;
			case ACPSIGNCODE_STAverageSpeedID:
				pos += addSTAverageSpeed(pos);
				break;
			case ACPSIGNCODE_LTAverageOilID:
				pos += addLTAverageOil(pos);
				break;
			case ACPSIGNCODE_STAverageOilID:
				pos += addSTAverageOil(pos);
				break;
			case ACPSIGNCODE_TyreStateID:
				pos += addTyreState(pos);
				break;


			case ACPSIGNCODE_WheelStateID:
				pos += addWheelState(pos);
				break;
			case ACPSIGNCODE_PastRecordSpeedID:
				if(flag == 0)
				{
					pos += addPastRecordSpeed(pos);
				}
				break;
			case ACPSIGNCODE_PastRecordWheelStateID:
				if(flag == 0)
				{
					pos += addPastRecordWheelState(pos);
				}
				break;



			case ACPSIGNCODE_ParkingStateID:
				pos += addParkingState(pos);
				break;


			case ACPSIGNCODE_RemainUpkeepMileageID:
				pos += addRemainUpkeepMileage(pos);
				break;
			case ACPSIGNCODE_AirconditionerInfoID:
				pos += addAirconditionerInfo(pos);
				break;
			case ACPSIGNCODE_KeepingstateTimeID:
				pos += addKeepingstateTime(pos);
				break;



			case ACPSIGNCODE_VerIVIID:
				pos += addVerIVI(pos);
				break;
			case ACPSIGNCODE_AcceleraPedalSwitchID:
				pos += addAcceleraPedalSwitch(pos);
				break;
			case ACPSIGNCODE_YaWSensorInfoSwitchID:
				pos += addYaWSensorInfoSwitch(pos);
				break;
			case ACPSIGNCODE_AmbientTemperatID:
				pos += addAmbientTemperat(pos);
				break;
			case ACPSIGNCODE_PureElecRelayID:
				pos +=addPureElecRelayState(pos);
				break;

			case ACPSIGNCODE_NewEnergyHeatManageID:
				pos += addNewEnergyHeatManage(pos);
				break;
			case ACPSIGNCODE_VehWorkModeID:
				pos += addVehWorkMode(pos);
				break;

			case ACPSIGNCODE_HighVolageStateID:
				pos += addHighVoltageState(pos);
				break;
			default:
				break;
		}
	}
	
	if(flag == 1)
	{
		pos += FaultSignDeal(pos);
	}

	if(flag == 2)
	{
		pos += addFaultSignDeal(pos);
	}

	tempLen = uint16_t(pos - dataBuff) - 2;

	//pData[0] = ((tempLen >> 7) | 0x20);
	//pData[1] = (tempLen & 0x7F);

	//FAWACPLOG("data Response tempLen==%d\n",tempLen);
	//FAWACP_NO("pData[0]: %02x pData[1]: %02x", pData[0], pData[1]);
	//FAWACP_NO("\n\n");

	return uint16_t(pos - dataBuff);
}


uint16_t CFAWACP::RemoteConfigCommandDeal(uint8_t *dataBuff)
{
	uint8_t EmergedCall[20] = {0};

	uint8_t *pos = dataBuff;

	uint16_t tempLen = 57;
	pos += addDataLen(pos, tempLen, 0, 1);

	*pos++ = m_AcpRemoteConfig.DeviceTotal;		
	for(uint8_t i = 0; i < m_AcpRemoteConfig.DeviceTotal; i++)
	{
		for(uint8_t k = 0; k < m_AcpRemoteConfig.DeviceTotal; k++)
		{
			
			if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].DeviceNo == m_AcpRemoteConfig.DeviceConfig[i].DeviceNo)
			{
				*pos++ = (m_AcpRemoteConfig.DeviceConfig[i].DeviceNo >> 8) & 0xFF;
				*pos++ = m_AcpRemoteConfig.DeviceConfig[i].DeviceNo & 0xFF;		
				*pos++ = m_AcpRemoteConfig.DeviceConfig[i].ConfigCount;			
				for(uint8_t j = 0; j < m_AcpRemoteConfig.DeviceConfig[i].ConfigCount; j++)
				{
					*pos++ = m_AcpRemoteConfig.DeviceConfig[i].ConfigItem[j].ConfigItemNum;	
					switch(m_AcpRemoteConfig.DeviceConfig[i].ConfigItem[j].ConfigItemNum)
					{
						case ACPCfgItem_EngineStartTimeID:
							*pos++ = (p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].EngineStartTime >> 8) & 0xFF;
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].EngineStartTime & 0xFF;
							break;
						case ACPCfgItem_SamplingSwitchID:	
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].SamplingSwitch;
							break;
						case ACPCfgItem_EmergedCallID:
							dataPool->getPara(E_CALL_ID, (void *)EmergedCall, sizeof(EmergedCall));
							memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].EmergedCall,EmergedCall,15);
							memcpy(pos, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].EmergedCall, 15);
							pos += 15;
							break;
						case ACPCfgItem_WhitelistCallID:
							memcpy(pos, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].WhitelistCall, 15);
							pos += 15;
							break;
						case ACPCfgItem_CollectTimeIntervalID:
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].CollectTimeInterval;
							break;
						case ACPCfgItem_ReportTimeIntervalID:	
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ReportTimeInterval;
							break;
						case ACPCfgItem_CollectTimeGpsSpeedIntervalID:
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].CollectTimeGpsSpeedInterval;
							break;
						case ACPCfgItem_ReportTimeGpsSpeedIntervalID:
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ReportTimeGpsSpeedInterval;
							break;
						case ACPCfgItem_ChargeModeID:
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ChargeMode;
							break;
						case ACPCfgItem_ChargeScheduleID:
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ChargeSchedule.ScheduChargStartData;
							*pos++ = (p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ChargeSchedule.ScheduChargStartTime >> 8) & 0xFF;
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ChargeSchedule.ScheduChargStartTime & 0xFF;
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ChargeSchedule.ScheduChargEndData;
							*pos++ = (p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ChargeSchedule.ScheduChargEndTime >> 8) & 0xFF;
							*pos++ = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].ChargeSchedule.ScheduChargEndTime & 0xFF;
							break;
						case ACPCfgItem_LoadCellID:
							memcpy(pos, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].LoadCell, 15);
							pos += 15;
							break;
						case ACPCfgItem_InformationCellID:
							memcpy(pos, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[k].InformationCell, 15);
							pos += 15;
							break;
						default:
							break;
					}
				}
			}
		}
	}
	return uint16_t(pos - dataBuff);
}

//RootKey
uint16_t CFAWACP::RootKeyDataDeal(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;
	
	uint16_t tempLen = 32;
	pos += addDataLen(pos, tempLen, 0, 1);

	Encrypt_AES128Data(p_FAWACPInfo_Handle->New_RootKey, p_FAWACPInfo_Handle->New_RootKey, sizeof(p_FAWACPInfo_Handle->RootKey), pos, AcpCryptFlag_IS);
	pos += tempLen;

	return uint16_t(pos - dataBuff);
}

 
uint16_t CFAWACP::RemoteUpgrade_ResponseDeal(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;

#if 0
	uint16_t tempLen = 4 + m_AcpRemoteUpgrade.UpdateFile_Len + m_AcpRemoteUpgrade.Ver_Len;
	//pos += addDataLen(pos, tempLen, 1, 1);

	*pos++ = m_AcpRemoteUpgrade.DeviceNo;
	*pos++ = m_AcpRemoteUpgrade.UpdateFile_Len;
	memcpy(pos, m_AcpRemoteUpgrade.UpdateFile_Param, m_AcpRemoteUpgrade.UpdateFile_Len);
	pos += m_AcpRemoteUpgrade.UpdateFile_Len;
	*pos++ = m_AcpRemoteUpgrade.Ver_Len;
	memcpy(pos, m_AcpRemoteUpgrade.VerNo, m_AcpRemoteUpgrade.Ver_Len);
	pos += m_AcpRemoteUpgrade.Ver_Len;
	FAWACPLOG("Ver_Len%d\n",m_AcpRemoteUpgrade.Ver_Len);
	FAWACPLOG("UpdateFile_Len%s\n",m_AcpRemoteUpgrade.VerNo);
	*pos++ = 0;
#endif
	*pos++ = mqtt_config.m_TestOrNormal;
	FAWACPLOG("cuorla is %d\r\n",mqtt_config.m_TestOrNormal);
	printf("jason add ##########cuorla is %d\n",mqtt_config.m_TestOrNormal);

	return uint16_t(pos - dataBuff);
}
uint16_t CFAWACP::CameraCtrl_resultDeal(uint8_t *dataBuff, char *result)
{
	/*  */
	uint8_t *pos = dataBuff;
	uint16_t datalen = 0;

	//	*pos++ = result;

	//[]
	memcpy(pos, result, CAMERACTRL_ITEM_NUMBERS);
	memset(result, 0, CAMERACTRL_ITEM_NUMBERS);
	pos += CAMERACTRL_ITEM_NUMBERS;
	datalen = (uint16_t)(pos-dataBuff);

	return datalen;
}
uint16_t CFAWACP::Camera_Gsensor(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;

	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.GPSState <<5 ) & 0x60)| ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude >> 24 ) & 0x1F);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude >>16) & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude >> 8) & 0xFF);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitude & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.longitudeState << 6 ) & 0xC0) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude >> 23 ) & 0x3F);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude >> 15) & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude >> 7) & 0xFF);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitude << 1) & 0xFE) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitudeState >> 1) & 0X01);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.latitudeState << 7) & 0X80) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.altitude >> 7) & 0X7F);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.altitude << 1) & 0xFE) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.year >> 5) & 0X01);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.year << 3) & 0XF8) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.month >> 1) & 0X07);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.month << 7) & 0X80) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.day << 2) & 0X7C) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.hour >> 3) & 0X03);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.hour << 5) & 0XE0) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.minute >> 1) & 0X1F);
	*pos++ = ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.minute << 7) & 0X80) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.second << 1) & 0X7E) | ((p_FAWACPInfo_Handle->VehicleCondData.GPSData.degree >> 8) & 0X01);
	*pos++ = (p_FAWACPInfo_Handle->VehicleCondData.GPSData.degree & 0XFF);

	if(CameraGensorTest[2] == '-')
	{
		*pos++ = 0x01;
	}
	else if(CameraGensorTest[2] == '+')
	{
		*pos++ = 0x00;
	}

	memcpy(pos,&CameraGensorTest[3],3);
	pos +=3;
	//*pos++ = 0x01;
	//*pos++ = 0x02;
	//*pos++ = 0x03;

	return uint16_t(pos - dataBuff);
}

uint16_t CFAWACP::RemoteUpgrade_DownloadStartDeal(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;

	uint16_t tempLen = 4 + m_AcpRemoteUpgrade.UpdateFile_Len + m_AcpRemoteUpgrade.Ver_Len;
	pos += addDataLen(pos, tempLen, 1, 1);

	*pos++ = m_AcpRemoteUpgrade.DeviceNo;
	*pos++ = m_AcpRemoteUpgrade.UpdateFile_Len;
	memcpy(pos, m_AcpRemoteUpgrade.UpdateFile_Param, m_AcpRemoteUpgrade.UpdateFile_Len);
	pos += m_AcpRemoteUpgrade.UpdateFile_Len;
	*pos++ = m_AcpRemoteUpgrade.Ver_Len;
	memcpy(pos, m_AcpRemoteUpgrade.VerNo, m_AcpRemoteUpgrade.Ver_Len);
	pos += m_AcpRemoteUpgrade.Ver_Len;
	*pos++ = Upgrade_Reserve;

	return uint16_t(pos - dataBuff);
}

uint16_t CFAWACP::RemoteUpgrade_DownloadResultDeal(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;

#if 0
	uint16_t tempLen = 4 + m_AcpRemoteUpgrade.UpdateFile_Len + m_AcpRemoteUpgrade.Ver_Len;
	//pos += addDataLen(pos, tempLen, 1, 1);

	*pos++ = m_AcpRemoteUpgrade.DeviceNo;
	*pos++ = m_AcpRemoteUpgrade.UpdateFile_Len;
	memcpy(pos, m_AcpRemoteUpgrade.UpdateFile_Param, m_AcpRemoteUpgrade.UpdateFile_Len);
	pos += m_AcpRemoteUpgrade.UpdateFile_Len;
	*pos++ = m_AcpRemoteUpgrade.Ver_Len;

	memcpy(pos, m_AcpRemoteUpgrade.VerNo, m_AcpRemoteUpgrade.Ver_Len);
	pos += m_AcpRemoteUpgrade.Ver_Len;
#endif

	*pos++ = Upgrade_DownloadState;

	return uint16_t(pos - dataBuff);
}

uint16_t CFAWACP::RemoteUpgrade_UpdateStartDeal(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;

	uint16_t tempLen = 4 + m_AcpRemoteUpgrade.UpdateFile_Len + m_AcpRemoteUpgrade.Ver_Len;
	pos += addDataLen(pos, tempLen, 1, 1);

	*pos++ = m_AcpRemoteUpgrade.DeviceNo;
	*pos++ = m_AcpRemoteUpgrade.UpdateFile_Len;
	memcpy(pos, m_AcpRemoteUpgrade.UpdateFile_Param, m_AcpRemoteUpgrade.UpdateFile_Len);
	pos += m_AcpRemoteUpgrade.UpdateFile_Len;
	*pos++ = m_AcpRemoteUpgrade.Ver_Len;
	memcpy(pos, m_AcpRemoteUpgrade.VerNo, m_AcpRemoteUpgrade.Ver_Len);
	pos += m_AcpRemoteUpgrade.Ver_Len;
	*pos++ = Upgrade_Reserve;

	return uint16_t(pos - dataBuff);
}

uint16_t CFAWACP::RemoteUpgrade_UpdateResultDeal(uint8_t *dataBuff)
{
	uint8_t *pos = dataBuff;
	int osid = 0;

	osid = get_osid_from_file(MQTT_VERSION_NORMAL);
	printf("jason add for %s(%d): osid = %d\n", __func__, __LINE__, osid);
	*pos++ = mqtt_config.m_TestOrNormal;
	*pos++ = (osid>>8) & 0xff;
	*pos++ = osid&0xff;

	return uint16_t(pos - dataBuff);
}
uint16_t CFAWACP::ReportGPSSpeedDeal(uint8_t* dataBuff)
{
	static uint8_t CollectPacketSN = 0;
	uint8_t *pos = dataBuff;
	uint16_t SignTotal = 2;
	uint16_t tempLen = 31;

	pos += addDataLen(pos, tempLen, 0, 0);
	*pos++ = m_SendMode;
	if(CollectPacketSN == 254)
		CollectPacketSN = 0;
	*pos++ = CollectPacketSN++;
	TimeDeal(pos, 0);
	pos += DATA_TIMESTAMP_SIZE - 1;
	*pos++ = (SignTotal >> 8) & 0xFF;
	*pos++ = SignTotal & 0xFF;
	pos += add_GPSData(pos);
	pos += addCurrentSpeed(pos);

	return uint16_t(pos - dataBuff);
}


uint16_t CFAWACP::FaultSignDeal(uint8_t* dataBuff)
{
	uint8_t *pos = dataBuff;
	uint16_t datalen = 0;

	for(uint16_t i = 100; i < 145; i++)
	{
		switch(i)
		{
			case ACPCODEFAULT_EMSID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EMSID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEMSState;
				break;
			case ACPCODEFAULT_TCUID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_TCUID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTCUState;
				break;
			case ACPCODEFAULT_EmissionID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EmissionID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEmissionState;
				break;
			case ACPCODEFAULT_SRSID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_SRSID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTSRSState;
				break;
			case ACPCODEFAULT_ESPID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ESPID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTESPState;
				break;
			case ACPCODEFAULT_ABSID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ABSID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTABSState;
				break;
			case ACPCODEFAULT_EPASID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EPASID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEPASState;
				break;
			case ACPCODEFAULT_OilPressureID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_OilPressureID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTOilPressureState;
				break;
			case ACPCODEFAULT_LowOilID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LowOilID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLowOilIDState;
				break;
			case ACPCODEFAULT_BrakeFluidLevelID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BrakeFluidLevelID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBrakeFluidLevelState;
				break;
			case ACPCODEFAULT_BatteryChargeID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BatteryChargeID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBatteryChargeState;
				break;
			case ACPCODEFAULT_BBWID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BBWID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBBWState;
				break;
			case ACPCODEFAULT_TPMSID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_TPMSID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTPMSState;
				break;
			case ACPCODEFAULT_STTID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_STTID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTSTTState;
				break;
			case ACPCODEFAULT_ExtLightID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ExtLightID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTExtLightState;
				break;
			case ACPCODEFAULT_ESCLID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ESCLID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTESCLState;
				break;
			case ACPCODEFAULT_EngineOverwaterID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EngineOverwaterID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEngineOverwaterState;
				break;
			case ACPCODEFAULT_ElecParkUnitID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ElecParkUnitID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTElecParkUnitState;
				break;
			case ACPCODEFAULT_AHBID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AHBID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAHBState;
				break;
			case ACPCODEFAULT_ACCID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ACCID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTACSState;
				break;
			case ACPCODEFAULT_FCWSID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_FCWSID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWSState;
				break;
			case ACPCODEFAULT_LDWID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LDWID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLDWState;
				break;
			case ACPCODEFAULT_BlindSpotDetectID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BlindSpotDetectID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBlindSpotDetectState;
				break;
			case ACPCODEFAULT_AirconManualID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AirconManualID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAirconManualState;
				break;
			case ACPCODEFAULT_HVSystemID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_HVSystemID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTHVSystemState;
				break;
			case ACPCODEFAULT_HVInsulateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_HVInsulateID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTHVInsulateState;
				break;
			case ACPCODEFAULT_HVILID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_HVILID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTHVILState;
				break;
				//			case ACPCODEFAULT_BatteryChargeID:
				//				SignCodelen = SignCodeDeal(pos, 1, ACPCODEFAULT_BatteryChargeID);
				//				pos += SignCodelen;
				//				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBatteryChargeState;
				//				break;
			case ACPCODEFAULT_EVCellID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EVCellID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEVCellState;
				break;
			case ACPCODEFAULT_PowerMotorID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_PowerMotorID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPowerMotorState;
				break;
			case ACPCODEFAULT_EParkID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EParkID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEParkState;
				break;
			case ACPCODEFAULT_EVCellLowBatteryID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EVCellLowBatteryID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEVCellLowBatteryState;
				break;
			case ACPCODEFAULT_EVCellOverTemperateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_EVCellOverTemperateID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTEVCellOverTemperateState;
				break;
			case ACPCODEFAULT_PowerMotorOverTemperateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_PowerMotorOverTemperateID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPowerMotorOverTemperateState;
				break;
			case ACPCODEFAULT_ConstantSpeedSystemFailID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ConstantSpeedSystemFailID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTConstantSpeedSystemFailState;
				break;
			case ACPCODEFAULT_ChargerFaultID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ChargerFaultID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTChargerFaultState;
				break;
			case ACPCODEFAULT_AirFailureID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AirFailureID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAirFailureState;
				break;
			case ACPCODEFAULT_AlternateAuxSystemFailID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AlternateAuxSystemFailID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlternateAuxSystemFailState;
				break;
			case ACPCODEFAULT_AutoEmergeSystemFailID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AutoEmergeSystemFailID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAutoEmergeSystemFailState;
				break;
			case ACPCODEFAULT_ReverRadarSystemFailID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ReverRadarSystemFailID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTReverRadarSystemFailState;
				break;
			case ACPCODEFAULT_ElecGearSystemFailID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ElecGearSystemFailID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTElecGearSystemFailState;
				break;
			case ACPCODEFAULT_TyreAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_TyreAlarmID);				
				*pos++ = (p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightRearTireTempAlarm << 3) |
					p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightRearTirePressAlarm;
				*pos++ = (p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftRearTireTempAlarm << 3) |
					p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftRearTirePressAlarm;
				*pos++ = (p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightrontTireTempAlarm << 3) |
					p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.RightFrontTirePressAlarm;
				*pos++ = (p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftFrontTireTempAlarm << 3) |
					p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTyreAlarmState.LeftFrontTirePressAlarm;
				break;
			case ACPCODEFAULT_DCDCConverterFaultID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_DCDCConverterFaultID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTDCDCConverterFaultState;
				break;
			case ACPCODEFAULT_VehControllerFailID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_VehControllerFailID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTVehControllerFailState;
				break;
			case ACPCODEFAULT_PureElecRelayCoilFualtID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_PureElecRelayCoilFualtID);				
				*pos++ = (p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainNagetiveRelayFault << 6) |
					(p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainPositiveRelayFault << 5) |
					(p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.RechargeNegaRelayCoilFault << 4) |
					(p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.RechargePositRelayCoilFault << 3) |
					(p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.PrefillRelayCoilFault << 2) |
					(p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainNegaRelayCoilFault << 1) |
					p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTPureElecRelayCoilState.MainPositRelayCoilFault;
				break;
			default:
				break;
		}		
	}

	pos += addFaultSignDeal(pos);


	return uint16_t(pos - dataBuff);
}


uint16_t CFAWACP::addFaultSignDeal(uint8_t* dataBuff)
{
	uint8_t *pos = dataBuff;

	for(uint16_t i = 500; i < 537; i++)
	{
		switch(i)
		{
			case ACPCODEFAULT_TSROverSpeedAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_TSROverSpeedAlarmID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTSROverSpeedAlarmState;
				break;
			case ACPCODEFAULT_TSRLimitSpeedID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_TSRLimitSpeedID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTSRLimitSpeedState;
				break;
			case ACPCODEFAULT_AEBInterventionID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AEBInterventionID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAEBInterventionState;
				break;
			case ACPCODEFAULT_ABSInterventionID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ABSInterventionID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTABSInterventionState;
				break;
			case ACPCODEFAULT_ASRInterventionID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ASRInterventionID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTASRInterventionState;
				break;
			case ACPCODEFAULT_ESPInterventionID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ESPInterventionID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTASRInterventionState;
				break;
			case ACPCODEFAULT_DSMAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_DSMAlarmID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTDSMAlarmState;
				break;
			case ACPCODEFAULT_TowHandOffDiskID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_TowHandOffDiskID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTTowHandOffDiskState;
				break;
			case ACPCODEFAULT_ACCStateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ACCStateID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTACCState;
				break;
			case ACPCODEFAULT_ACCSetSpeedID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_ACCSetSpeedID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTACCSetSpeedState;
				break;
			case ACPCODEFAULT_FCWAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_FCWAlarmID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWAlarmState;
				break;
			case ACPCODEFAULT_FCWStateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_FCWStateID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWState;
				break;
			case ACPCODEFAULT_FCWAlarmAccePedalFallID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_FCWAlarmAccePedalFallID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWAlarmAccePedalFallState;
				break;
			case ACPCODEFAULT_FCWAlarmFirstBrakeID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_FCWAlarmFirstBrakeID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTFCWAlarmFirstBrakeState;
				break;
			case ACPCODEFAULT_LDWStateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LDWStateID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTSLDWState;
				break;
			case ACPCODEFAULT_LDWAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LDWAlarmID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLDWAlarmState;
				break;
			case	ACPCODEFAULT_LDWAlarmDireDiskResponseID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LDWAlarmDireDiskResponseID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLDWAlarmDireDiskResponseState;
				break;
			case ACPCODEFAULT_LKAStateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LKAStateID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKAState;
				break;
			case ACPCODEFAULT_LKAInterventionID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LKAInterventionID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKAInterventionState;
				break;
			case ACPCODEFAULT_LKADriverTakeOverPromptID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LKADriverTakeOverPromptID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKADriverTakeOverPromptState;
				break;
			case ACPCODEFAULT_LKADriverResponsID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_LKADriverResponsID);				
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLKADriverResponsState;
				break;
			case ACPCODEFAULT_BSDStateID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BSDStateID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTLBSDState;
				break;
			case ACPCODEFAULT_BSDLeftSideAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BSDLeftSideAlarmID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDLeftSideAlarmState;
				break;
			case ACPCODEFAULT_BSDRightSideAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BSDRightSideAlarmID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDRightSideAlarmState;
				break;
			case ACPCODEFAULT_BSDAlarmReftWheelRespID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BSDAlarmReftWheelRespID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDAlarmReftWheelRespState;
				break;
			case ACPCODEFAULT_BSDAlarmFirstBrakeID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BSDAlarmFirstBrakeID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDAlarmFirstBrakeState;
				break;
			case ACPCODEFAULT_BSDAlarmPedalAcceID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_BSDAlarmPedalAcceID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTBSDAlarmPedalAcceState;
				break;
			case ACPCODEFAULT_CrossLeftReportID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_CrossLeftReportID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossLeftReportState;
				break;
			case ACPCODEFAULT_CrossRightReportID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_CrossRightReportID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossRightReportState;
				break;
			case ACPCODEFAULT_CrossAlarmWhellID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_CrossAlarmWhellID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossAlarmWhellState;
				break;
			case ACPCODEFAULT_CrossAlarmStopID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_CrossAlarmStopID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossAlarmStopState;
				break;
			case ACPCODEFAULT_CrossAlarmAcceTreadleID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_CrossAlarmAcceTreadleID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTCrossAlarmAcceTreadleState;
				break;
			case ACPCODEFAULT_AlterTrackAssistLeftAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AlterTrackAssistLeftAlarmID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistLeftAlarmState;
				break;
			case ACPCODEFAULT_AlterTrackAssistRightAlarmID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AlterTrackAssistRightAlarmID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistRightAlarmState;
				break;

			case ACPCODEFAULT_AlterTrackAssistDireRepsonID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AlterTrackAssistDireRepsonID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistDireRepsonState;
				break;
			case ACPCODEFAULT_AlterTrackAssistFirstStopID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AlterTrackAssistFirstStopID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistFirstStopState;
				break;
			case ACPCODEFAULT_AlterTrackAssistAcceDropID:
				pos += SignCodeDeal(pos, 0, ACPCODEFAULT_AlterTrackAssistAcceDropID);
				*pos++ = p_FAWACPInfo_Handle->AcpCodeFault.ACPCODEFAULTAlterTrackAssistAcceDropState;
				break;
			default:
				break;
		}
	}
	return uint16_t(pos - dataBuff);
}




uint16_t CFAWACP::VehicleGPSDeal(uint8_t* dataBuff)
{
	static uint8_t CollectPacketSN = 0;
	uint8_t *pos = dataBuff;
	uint16_t tempLen = 27;
	uint16_t SignTotal = 1;

	pos += addDataLen(pos, tempLen, 0, 0);
	*pos++ = m_SendMode;
	if(CollectPacketSN == 254)
		CollectPacketSN = 0;
	*pos++ = CollectPacketSN++;
	
	TimeDeal(pos, 0);
	pos += DATA_TIMESTAMP_SIZE - 1;
	
	*pos++ = (SignTotal >> 8) & 0xFF;
	*pos++ = SignTotal & 0xFF;
	
	pos += add_GPSData(pos);

	return uint16_t(pos - dataBuff);
}

uint16_t CFAWACP::RemoteDiagnosticDeal(uint8_t *dataBuff)
{
	uint16_t dataLen = 0;
	uint32_t temp = 0;
	uint8_t *pos = dataBuff;
	uint8_t *pTemp = dataBuff;
	
	*pos++ = 0;
	*pos++ = 0;
	
	*pos++ = m_RemoteDiagnoseResult.DiagnoseTotal;

	for(uint8_t i = 0; i < m_RemoteDiagnoseResult.DiagnoseTotal; i++)
	{
		*pos++ = m_RemoteDiagnoseResult.DiagnoseCodeFault[i].DiagnoseCode;			
		*pos++ = m_RemoteDiagnoseResult.DiagnoseCodeFault[i].DiagnoseCodeFaultTotal;	
		for(uint8_t k = 0; k < m_RemoteDiagnoseResult.DiagnoseCodeFault[i].DiagnoseCodeFaultTotal; k++)
		{
			temp = m_RemoteDiagnoseResult.DiagnoseCodeFault[i].DiagnoseCodeFault[k];
			*pos++ = (temp >> 24) & 0xFF;
			*pos++ = (temp >> 16) & 0xFF;
			*pos++ = (temp >> 8) & 0xFF;
			*pos++ = temp & 0xFF;
		}
	}
	
	dataLen = uint16_t(pos - dataBuff) - 2;
	pTemp[0] = ((dataLen >> 7) | 0x20);
	pTemp[1] = (dataLen & 0x7F);

	return uint16_t(pos - dataBuff);
}


/***********************************************************************************
 ************************************************************************************/

int CFAWACP::ShowdataTime(uint8_t *pos)
{
	uint8_t Tbuff[32] = {};
	struct tm _tm;

	int year = (int )((pos[1] >> 2) & 0x3F) + 1990;
	int month = ((pos[1] << 2) & 0x0C) | ((pos[2] >> 6) & 0x03);
	int day = (pos[2] >> 1) & 0x1F;
	int hour = ((pos[2] << 4) & 0x10) | ((pos[3] >> 4) & 0x0F);
	int minutes = ((pos[3] << 2) & 0x3C) | ((pos[4] >> 6) & 0x03);
	int seconds = (pos[4] & 0x3F);
	int msSeconds = pos[5]|pos[6];

	sprintf((char *)Tbuff, "%d-%d-%d-%d-%d-%d-%d",year, month, day, hour, minutes, seconds, msSeconds);
	FAWACPLOG("Time Stamp: %s\n", Tbuff);

#if 1
	if(m_loginState == STATUS_LOGGIN_ING && p_FAWACPInfo_Handle->VehicleCondData.GPSData.GPSState == 0){
		m_tspTimestamp.Year = year - 1990;
		m_tspTimestamp.Month = month;
		m_tspTimestamp.Day = day;
		m_tspTimestamp.Hour = hour;
		m_tspTimestamp.Minutes = minutes;
		m_tspTimestamp.Seconds = seconds;
		m_tspTimestamp.msSeconds = msSeconds;
	}
	//if(p_FAWACPInfo_Handle->VehicleCondData.GPSData.GPSState == 0)
	{
		_tm.tm_year = year - 1900; /* 1900 */
		//80901
		_tm.tm_mon = month - 1;    /* 0[0,11] */
		_tm.tm_mday = day;     /*  [1,31] */
		_tm.tm_hour = hour;     /*  [0,23] */
		_tm.tm_min = minutes;      /*  [0,59] */
		_tm.tm_sec = seconds;      /*  [0,59] */

		m_CMcu->setSystemTime(&_tm);
		printf("set time ok\n");
	}
#endif
	return 0;
}

uint16_t 	CFAWACP::UnpackData_AcpLoginAuthen(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;
	uint16_t DataLen = 0;
	m_loginState = STATUS_LOGGIN_ING;
	
	ShowdataTime(pos);	
	pos += 7;
	
	pos += unpackDataLenDeal(pos,DataLen);
	FAWACPLOG("DataLen == %d\n",DataLen);
	switch(MsgType)
	{
		case 2:		//AKey Apply Message
			memcpy(AcpTsp_AkeyMsg.AkeyC_Tsp, pos, sizeof(AcpTsp_AkeyMsg.AkeyC_Tsp));	//AKEY(C)
			pos += sizeof(AcpTsp_AkeyMsg.AkeyC_Tsp);
			memcpy(AcpTsp_AkeyMsg.Rand1_Tsp, pos, sizeof(AcpTsp_AkeyMsg.Rand1_Tsp));	//Randl
			pos += sizeof(AcpTsp_AkeyMsg.Rand1_Tsp);
			sendLoginCTMsg();
			break;
		case 4:		//CS Message
			memcpy(AcpTsp_CSMsg.Rand2CS_Tsp, pos, sizeof(AcpTsp_CSMsg.Rand2CS_Tsp));	//Rand2(CS)
			pos += sizeof(AcpTsp_CSMsg.Rand2CS_Tsp);
			AcpTsp_CSMsg.RT_Tsp = *pos++;				//RT
			sendLoginRSMsg();
			break;
		case 6:		//Skey Message
			memcpy(AcpTsp_SKeyMsg.SkeyC_Tsp, pos, sizeof(AcpTsp_SKeyMsg.SkeyC_Tsp));	//SKey(C)
			pos += sizeof(AcpTsp_SKeyMsg.SkeyC_Tsp);
			memcpy(AcpTsp_SKeyMsg.Rand3CS_Tsp, pos, sizeof(AcpTsp_SKeyMsg.Rand3CS_Tsp));//Rand3(CS)
			pos += sizeof(AcpTsp_SKeyMsg.Rand3CS_Tsp);
			sendLoginAuthReadyMsg();
			break;
		case 8:		//Auth Ready ACK Message
			memcpy(AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken, pos, sizeof(AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken));	//SKey(C)
			pos += sizeof(AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken);

			memcpy(p_FAWACPInfo_Handle->AuthToken, AcpTsp_AuthReadyACKMsg.ACKMsg_AuthToken.AuthToken, sizeof(p_FAWACPInfo_Handle->AuthToken));//AuthToken
			m_loginState = STATUS_LOGGIN_FINISH;
			m_bEnableSendDataFlag = true;
			time(&HighReportTime);
			time(&LowReportTime);
			
			if(TimeOutType.RemoteCtrlFlag == 2)
			{
				FAWACPLOG("TimeOut RemoteCtrlFlag");
				RespondTspPacket(ACPApp_RemoteCtrlID, 1, AcpCryptFlag_IS, TimeOutType.RemoteCtrlTspSource);
				delete_link(&Headerlink, ACPApp_RemoteCtrlID);
			}
			if(TimeOutType.VehQueCondFlag == 2)
			{
				FAWACPLOG("TimeOut VehQueCondFlag");
				RespondTspPacket(ACPApp_QueryVehicleCondID, 1, AcpCryptFlag_IS, TimeOutType.VehQueCondSource);
				delete_link(&Headerlink, ACPApp_QueryVehicleCondID);
			}
			if(TimeOutType.RemoteCpnfigFlag == 2)
			{
				FAWACPLOG("TimeOut RemoteCpnfigFlag");
				RespondTspPacket(ACPApp_RemoteConfigID, 1, AcpCryptFlag_IS, TimeOutType.RemoteCpnfigSource);
				delete_link(&Headerlink, ACPApp_RemoteConfigID);
			}
			if(TimeOutType.UpdateRootkeyFlag == 2)
			{
				FAWACPLOG("TimeOut UpdateRootkeyFlag");
				RespondTspPacket(ACPApp_UpdateKeyID, 3, AcpCryptFlag_IS, TimeOutType.UpdateRootkeySource);
				delete_link(&Headerlink, ACPApp_UpdateKeyID);
			}
			if(TimeOutType.VehGPSFlag == 2)
			{
				FAWACPLOG("TimeOut VehGPSFlag");
				RespondTspPacket(ACPApp_GPSID, 1, AcpCryptFlag_IS, TimeOutType.VehGPSSource);
				delete_link(&Headerlink, ACPApp_GPSID);
			}
			if(TimeOutType.RemoteDiagnoFlag == 2)
			{
				FAWACPLOG("TimeOut RemoteDiagnoFlag");
				RespondTspPacket(ACPApp_RemoteDiagnosticID, 1, AcpCryptFlag_IS, TimeOutType.RemoteDiagnoSource);
				delete_link(&Headerlink, ACPApp_RemoteDiagnosticID);
			}
			memset(&TimeOutType, 0, sizeof(TimeOutType_t));

			FAWACPLOG("login success !!!\n\n\n");
			break;
		default:
			break;
	}
	return 0;
}
void  CFAWACP::ftp_upload_log(uint8_t *playload)
{
	int ii, jj;
	uint8_t 	URL_Len;		

#if 0	
	uint8_t 	URL_IP[MAX_UPGRADEPARAM_LEN];			
	char		URL_Port[MAX_UPGRADEPARAM_LEN]; 		
	uint8_t 	URL_Param[MAX_UPGRADEPARAM_LEN];		

	uint8_t *pos = playload;
	URL_Len = *pos++;

	FAWACPLOG("URL_Len��%02x\n",URL_Len);

	for(ii=6, jj=0;ii<int(URL_Len);ii++)
	{
		if(*(pos+ii)==':')
		{
			memcpy(URL_IP, pos+6, ii-6);
			printf("FTP ip is %s\n",URL_IP);
			jj=ii;
		}
		else if(*(pos+ii)=='/')
		{
			memcpy(URL_Port, pos+jj+1, ii-jj-1);
			//m_AcpRemoteUpgrade.URL_Port[i-j]='\0';
			memcpy(URL_Param, pos+ii,int(URL_Len)-ii);
			printf("FTP port is %s\n",URL_Port);
			printf("FTP url is %s\n",URL_Param);
		}
	}

	int Pasvfdlog;
	int PasvPortlog;
	char PasvIplog[32];
	memset(PasvIplog, 0, sizeof(PasvIplog));

	int port = atoi(URL_Port);
	printf("&&&&&& is %d\r\n",port);
	unsigned char output[16]; 

	FTPClient * ftpclientlog = new FTPClient(/*target_ip_new_log*/URL_IP, port);
	printf("555\n");

	if(ftpclientlog->loginFTPServer("anonymous", NULL) == -1)
	{
		//return -1;
	}

	if(ftpclientlog->setFTPServerToPasvMode(PasvIplog, &PasvPortlog) == -1)
	{
		//return -1;
	}
	if(ftpclientlog->connectPasvServer(PasvIplog, PasvPortlog, &Pasvfdlog,0) != -1)
	{
		if(access(LOG_FILE,F_OK) == 0)
		{
			system("cp /data/mylog /data/myuplog");
			system("sync");
			if(ftpclientlog->uploadFile(Pasvfdlog,URL_Param,LOG_UP_FILE) == 0)
			{
				printf("666\n");
			}
			system("rm /data/myuplog");
			system("sync");
		}
	}
#else

	char	URL[MAX_UPGRADEPARAM_LEN] = {0};
	char	URL_IP[MAX_UPGRADEPARAM_LEN] = {0};

	uint16_t port;
	char protocl[10]={0};
	char file_path[32] = {0};

	uint8_t *pos = playload;
	URL_Len = *pos++;

	FAWACPLOG("URL_Len %02x\n",URL_Len);
	memcpy(URL, pos, URL_Len);
	parse_http_url(URL, protocl, &port, URL_IP, file_path);

	int Pasvfdlog;
	int PasvPortlog;
	char PasvIplog[32];
	//	memset(PasvIplog, 0, sizeof(PasvIplog));
	printf("IP: %s", URL_IP);
	printf("port: %d\n",port);

	FTPClient * ftpclientlog = new FTPClient(URL_IP, port);
	printf("555\n");

	if(ftpclientlog->loginFTPServer("anonymous", NULL) == -1)
	{
		//return -1;
	}

	if(ftpclientlog->setFTPServerToPasvMode(PasvIplog, &PasvPortlog) == -1)
	{
		//return -1;
	}
	if(ftpclientlog->connectPasvServer(URL_IP, PasvPortlog, &Pasvfdlog,0) != -1)
	{
		if(access(LOG_FILE,F_OK) == 0)
		{
			system("cp /data/mylog /data/myuplog");
			system("sync");
			if(ftpclientlog->uploadFile(Pasvfdlog, file_path,LOG_UP_FILE) == 0)
			{
				printf("666\n");
			}
			system("rm /data/myuplog");
			system("sync");
		}
	}
#endif
	ftpclientlog->quitFTP();

}
uint16_t CFAWACP::UnpackData_AcpBleToken(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType)
{
	uint8_t RequestSource;
	uint8_t *pos = PayloadBuff;
	ShowdataTime(pos);


	pos += DATA_TIMESTAMP_SIZE;

	RequestSource = *pos;
	
	pos += 1;

	switch(MsgType)
	{
		case 1:
			memcpy(BleToken, pos, sizeof(BleToken));
			memcpy(m_bleUart->m_BleToken, pos, sizeof(m_bleUart->m_BleToken));
			MQTTReqBleTokenData(2,ACPApp_ReqBleTokenID);
			time(&UpDateBleTokenTime);
			m_bleUart->cb_bleRemoteConfigInfo();

			#if 0
			for(int i=0;i<6;i++)
			{
				printf("jason add token [%d] =%x\r\n",i,BleToken[i]);
			}
			#endif
			break;
		case 2:
			#if 1
			for(int i=0;i<6;i++)
			{
				printf("jason add bletoken [%d] =%x\r\n",i,m_bleUart->m_BleToken[i]);
			}
			#endif
			
			BleReqUpdateTokenFlag = 0;
			break;
	}

}

uint16_t CFAWACP::UnpackData_AcpRemoteCtrl(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;
	uint16_t DataLen = 0;
	uint8_t RequestSource = 0;
	uint8_t tempvalue = 0;
	uint8_t u8SendCount;
	int times = 0;
	int temp = N_TRACKLOADINGSTATUS;
	int nMcuWake4GStatus = 0;
	//
	ShowdataTime(pos);
	pos += DATA_TIMESTAMP_SIZE;

#ifndef MQTT
	//AuthToken
	pos++;
	memcpy(AuthToken, pos, sizeof(AuthToken));
	if( memcmp(p_FAWACPInfo_Handle->AuthToken, AuthToken, sizeof(AuthToken)) != 0)
		FAWACPLOG("AuthToken is fault !");
	pos += sizeof(AuthToken);
	//Source
	pos++;
	TspctrlSource = *pos++;
	FAWACPLOG("AcpRemoteCtrl analysis:");
#endif
	uint8_t datanouse = 0x01;

	switch(MsgType)
	{
		case 1:
#ifndef MQTT
			if(search(Headerlink, ACPApp_RemoteCtrlID) != NULL)
				return 0;
			
			pos += unpackDataLenDeal(pos, DataLen);
			FAWACPLOG("DataLen == %d\n",DataLen);
#endif
			RequestSource = *pos++;
			FAWACPLOG("Request Message Source -- %d\n",RequestSource);

			m_AcpRemoteCtrlList.SubitemTotal = *pos++;
			FAWACPLOG("Subitem total == %d",m_AcpRemoteCtrlList.SubitemTotal);
			for(uint16_t i = 0; i < m_AcpRemoteCtrlList.SubitemTotal; i++)
			{
				m_AcpRemoteCtrlList.SubitemCode[i] = *pos++;
				FAWACPLOG("Subitem Code == %d",m_AcpRemoteCtrlList.SubitemCode[i]);

				//#define MCU_WAKEUP_MODEM_GPIO  74 //sim7600.pin72
				//	modem_ri_notify_mcu();
#if 0
				if( m_AcpRemoteCtrlList.SubitemCode[i] != 6 )
				{
					uint8_t TempData = 0x01;
					modem_ri_notify_mcu_high();
					usleep(1000*100);
					if(tboxInfo.operateionStatus.isGoToSleep == 0/*&&(m_AcpRemoteCtrlList.SubitemCode[i]!=6)*/)
					{
						usleep(1000*1500);
						uart_wakeup(1);
						do
						{
							times++;
							//2019-01-14 KK modify
							//	m_CMcu->packProtocolData(TBOX_REQ_CFGINFO, 0, &TempData, 1, 0);
							usleep(1000*500);
							if(RET_OK == GPIO_GetMcuWake4GStatus(&nMcuWake4GStatus))
							{
								if((MCU_WAKE_4G_WORKSTATUS == nMcuWake4GStatus) && (MCU_4G_SYNC_SHAKEHAND_OK == m_CMcu->get_Uart_Sync_Value()))
								{
									m_CMcu->Set_Uart_Sync_Value(0);
									break;
								}
								else
								{
								}
							}
							else
							{
							}
							printf("VALUE == %d, sync == %d, times == %d\n", nMcuWake4GStatus, m_CMcu->get_Uart_Sync_Value(), times);
						}while(times <= MCU_WAKEUP_TIMES);

						if(times >= MCU_WAKEUP_TIMES)
						{
							//nothing
						}
					}
					modem_ri_notify_mcu_low();
				}
#endif


				if( m_AcpRemoteCtrlList.SubitemCode[i] != SleepNow_ID /* && tboxInfo.operateionStatus.isGoToSleep == 0 */)
				{
					//modem_ri_notify_mcu_high();
					m_CMcu->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
#if MCU_SLEEP_VERSION
#else
					m_CMcu->u8RemoteControlFlag = 1;
					printf("%s(%d): RemoteControlFlag %d\n", __func__, __LINE__, m_CMcu->u8RemoteControlFlag);
#endif
					if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
					{
						tbox_log(10,0);
					}
#if 0
					while(m_CMcu->Get_mcuWakeupEventValue() == STATUS_WAKEUP_EVENT)
					{
						sleep(1);
					}
#endif
				}
				printf("m_AcpRemoteCtrlList.SubitemCode[i] == %d\n", m_AcpRemoteCtrlList.SubitemCode[i]);

				switch(m_AcpRemoteCtrlList.SubitemCode[i])
				{
					case VehicleBody_LockID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.VehicleBody_Lock = *pos++;
						FAWACPLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleBody_TrackingCarID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.VehicleBody_TrackingCar = *pos++;
						FAWACPLOG("VehicleBody_TrackingCar == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Power_SwitchID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.PowerState.PowerState_Switch = *pos++;
						FAWACPLOG("Power_SwitchID == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case EngineState_SwitchID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.EngineState.EngineState_Switch = *pos++;
						FAWACPLOG("EngineState_Switch == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case SleepTime_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.SleepTime = *pos++;
						FAWACPLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case SleepNow_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.SleepNow = *pos++;
						FAWACPLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						#if 0 ////jason change for no sleep version for instant sleep
						if( tboxInfo.operateionStatus.isGoToSleep == 0 )
						{
							RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
							return 0;
						}
						#endif
						break;
					case ReportTime_ID:
						p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = *pos++;

						if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval<=9)
						{
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = 60;
						}
						FAWACPLOG("ReportTimeInterval == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval);
						if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
						{
							updateTBoxParameterInfo();
						}
						//01-05 KK modify callback fun
						//	cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
					case CheckBleState_ID:
						m_AcpRemoteCtrlList.SubitemValue[i]  = *pos++;
						FAWACPLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						if(bleupgrade==false)
						{
							m_bleUart->cb_bleRemoteCtrlCmd();
						}
						return 0;
					case BroadCase_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;
						FAWACPLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						if(bleupgrade==false)
						{
							m_bleUart->cb_bleRemoteCtrlCmd();
						}
						return 0;
					case SendPW_ID:
						memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW, 0, 6);
						memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW, pos, 6);
						FAWACPLOG("BlePW == %s",p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW);
						if(bleupgrade==false)
						{
							m_bleUart->cb_bleRemoteCtrlCmd();
						}
						return 0;
					case SetCar_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;
						m_CMcu->packProtocolData(0x82, 0x60, m_AcpRemoteCtrlList.SubitemValue, 1, 0);
						sleep(2);

						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						sleep(3);
						//disconnectSocket();
						return 0;
					case SetLowVc_ID:
						p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery = *pos++;

						if((p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery<80)||(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery>140))
						{
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery = 110;
						}
						FAWACPLOG("LOW battery == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery);
						updateTBoxParameterInfo();
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
					case ResetEC3_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;

						p_FAWACPInfo_Handle->VehicleCondData.CarIDState = m_AcpRemoteCtrlList.SubitemValue[i];
						m_CMcu->packProtocolData(0x82, 0x61, m_AcpRemoteCtrlList.SubitemValue, 1, 0);

						updateTBoxParameterInfo();
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
					case ResetVin_ID:
						memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin, 0, 17);
						memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin, pos, 17);
						FAWACPLOG("BlePW == %s",p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin);
						for(int i = 0;i<3;i++)
						{	
							m_CMcu->packProtocolData(0x82, 0x53, p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin, 1, 0);
							m_CMcu->packProtocolData(0x92, 0, &datanouse, 1, 0);
						}
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						if(access("/data/111.pem", F_OK) == 0)
							system(RM_MQTTPARA_FILE);
						sleep(1);
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						sleep(2);
						disconnectSocket();
						InitAcpTBoxAuthData();
						return 0;
					case ResetTbox_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;
						if(m_AcpRemoteCtrlList.SubitemValue[i]==0x00)  /*  reset tbox*/
						{
							system("rm /data/AcpPara");
							//	system("rm /data/TBoxPara");
							if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
							{
								dataPool->setPara(TBOX_TRCUKLOADING_ID, &temp, sizeof(temp));
							}
						}
						else if(m_AcpRemoteCtrlList.SubitemValue[i]==0x01)/*  reset tbox and delete update file*/
						{
							system("rm -rf /data/LteUpgrade.bin");
							system("rm -rf /data/MCU.bin");
							system("rm -rf /data/BLEAPP");
							system(RM_LTE_FILE);
							system(RM_MQTT_UPDATE_FILE);
						}
						system("sync");
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						sleep(2);
#if 0
						wds_deinit();
						//nas_qmi_release();
						nas_dinit();
						//voice_qmi_release();
						voice_deinit();
						//mcm_sms_release();
						sms_deinit();
						disconnectSocket();
						m_CMcu->close_uart();
#endif
						u8SendCount = 2;
						do{
							if(sendATCmd((char*)"at+cfun=0", (char*)"OK", NULL, 0, 5000) <=0 )
							{
								printf("jason add error send +cfun=0 cmd failed XXXXXXXXXXXXXXX\r\n");
							}
							else
							{
								printf("jason add send +cfun=0 cmd success \r\n");
								break;
							}
							sleep(5);
							u8SendCount--;
						}while(u8SendCount > 0);

						printf("################### exit 2 ###########################\n");
						//sendATCmd((char*)"at+cfun=0", (char*)"OK", NULL, 0, 5000);
						sleep(1);
						modem_ri_notify_mcu_high();
						sleep(1);				
						modem_ri_notify_mcu_low();
						sleep(1);
						u8SendCount = 2;
						do{
							if(sendATCmd((char*)"at+cfun=1", (char*)"OK", NULL, 0, 5000) <=0 )
							{
								printf("jason add error send +cfun=1 cmd failed XXXXXXXXXXXXXXX\r\n");
							}
							else
							{
								printf("jason add send +cfun=1 cmd success \r\n");
								break;
							}
							sleep(5);
							u8SendCount--;
						}while(u8SendCount > 0);						
						//sendATCmd((char*)"at+cfun=1", (char*)"OK", NULL, 0, 5000);						
						//system( RM_QMUX_SOCKET );
						sleep(1);						
						system("sys_reboot");
						break;
					case WakeMCU_ID:
						//	modem_ri_notify_mcu();
						m_CMcu->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
					case OpenCloseLog_ID:
						tempvalue = *pos++;
						dataPool->setPara(GL_DEBUGLOG_SWITCH_ID, &tempvalue, sizeof(tempvalue));
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
					case UploadLog_ID:

						ftp_upload_log(pos);						
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
					case UploadGPSInfo_ID:
						m_CMcu->packProtocolData( TBOX_QUERY_PARA_CMD, 0, NULL, 0, 0 );
						//	RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						break;
						
					case DropOffCar_ID:
						break;
						
					case SetUpdateTokenTime_ID:
						tempvalue = *pos++;
						dataPool->setPara(BleAutoUpDateTime_ID, &tempvalue, sizeof(tempvalue));

						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);

						break;
						
					case VehicleBody_WindowID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.VehicleBody_Window = *pos++;
						FAWACPLOG("VehicleBody_Window == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleBody_SunroofID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.VehicleBody_Sunroof = *pos++;
						FAWACPLOG("VehicleBody_Sunroof == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;

					case VehicleBody_LowbeamID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.VehicleBody_Lowbeam = *pos++;
						FAWACPLOG("VehicleBody_Lowbeam == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleBody_LuggageCarID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.VehicleBody_LuggageCar = *pos++;
						FAWACPLOG("VehicleBody_LuggageCar == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;					
					case Airconditioner_ControlID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_Control = *pos++;
						FAWACPLOG("Airconditioner_Control == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_CompressorSwitchID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_CompressorSwitch = *pos++;
						FAWACPLOG("Airconditioner_CompressorSwitch == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_TemperatureID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_Temperature.BitType = *pos++;
						FAWACPLOG("Airconditioner_Temperature == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_SetAirVolumeID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_SetAirVolume.BitType = *pos++;
						FAWACPLOG("Airconditioner_SetAirVolume == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_FrontDefrostSwitchID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_FrontDefrostSwitch = *pos++;
						FAWACPLOG("Airconditioner_FrontDefrostSwitch == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_HeatedrearID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_Heatedrear = *pos++;
						FAWACPLOG("Airconditioner_Heatedrear == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_BlowingModeID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_BlowingMode.BitType = *pos++;
						FAWACPLOG("Airconditioner_BlowingMode == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_InOutCirculateID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_InOutCirculate.BitType = *pos++;
						FAWACPLOG("Airconditioner_InOutCirculate == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Airconditioner_AutoSwitchID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.Airconditioner.Airconditioner_AutoSwitch = *pos++;
						FAWACPLOG("Airconditioner_AutoSwitch == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleSeat_DrivingSeatID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleSeat.VehicleSeat_DrivingSeat = *pos++;
						FAWACPLOG("VehicleSeat_DrivingSeat == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleSeat_CopilotseatID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleSeat.VehicleSeat_Copilotseat = *pos++;	
						FAWACPLOG("VehicleSeat_Copilotseat == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleSeat_DrivingSeatMomeryID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleSeat.VehicleSeat_DrivingSeatMemory = *pos++;
						FAWACPLOG("VehicleSeat_DrivingSeatMemory == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleChargeMode_ImmediateID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleCharge.VehicleCharge_Immediate = *pos++;
						FAWACPLOG("VehicleCharge_Immediate == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleChargeMode_AppointmentID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleCharge.VehicleCharge_Appointment = *pos++;
						FAWACPLOG("VehicleCharge_Appointment == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleChargeMode_EmergencyCharg:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleCharge.VehicleCharge_EmergenCharg = *pos++;
						FAWACPLOG("VehicleCharge_EmergenCharg == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleWIFIStatusID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleAutopark.VehicleWifiStatus = *pos++;
						FAWACPLOG("VehicleWifiStatus == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleAutoOUTID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleAutopark.VehicleAutoOut = *pos++;
						FAWACPLOG("VehicleAutoOut == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					default:
						break;
				}
			}
			if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
			{
				tbox_log(4,MsgType);
			}
			//01-05 KK modify mcu callback
			//reportRemoteCtrlCmd(m_CMcu->cb_RemoteCtrlCmd);
			Req_McuRemoteCtrl();
			break;
		case 0:
			if(search(Headerlink, ACPApp_RemoteCtrlID) != NULL)
			{
				pthread_mutex_lock(&mutex);
				delete_link(&Headerlink, ACPApp_RemoteCtrlID);
				pthread_mutex_unlock(&mutex);
				TimeOutType.RemoteCtrlFlag = 0;
			}
			FAWACPLOG("AcpRemoteControl ack is ok!\n");
			break;
		default:
			break;
	}
	return 0;
}

uint16_t CFAWACP::UnpackData_AcpVehCondUpload(uint8_t *PayloadBuff, uint16_t payload_size,  uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;

	FAWACPLOG("UnpackData_AcpVehCondUpload  MSGTYPE == %d \r\n", MsgType);
	switch(MsgType)
	{
		case 2:	//
		case 9:	//
			TSPACK_WorkWakeup_State = 0;
			TSPACK_WorkSleep_State = 0;
			InLoginFailTimes = 0;
			printf("jason add asp work on report ack\r\n");
			break;
		default:
			break;
	}

}




uint16_t CFAWACP::UnpackData_AcpQueryVehicleCond(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t applicationID, uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;
	uint16_t DataLen = 0;
	uint8_t RequestSource = 0;
	uint8_t TspSourceID   =	0;
	ShowdataTime(pos);
	pos += DATA_TIMESTAMP_SIZE;
	//AuthToken
	pos++;
	memcpy(AuthToken, pos, sizeof(AuthToken));
	if( memcmp(p_FAWACPInfo_Handle->AuthToken, AuthToken, sizeof(AuthToken)) != 0)
		FAWACPLOG("AuthToken is fault !");
	pos += sizeof(AuthToken);
	//Source
	pos++;
	TspSourceID = *pos++;

	switch(MsgType)
	{
		case 1:
			if(search(Headerlink, applicationID) != NULL)
				return 0;
			pos += unpackDataLenDeal(pos,DataLen);

			RequestSource = *pos++;
			FAWACPLOG("Request Message Source -- %d\n",RequestSource);
			m_AcpVehicleCondCommandList.CollectTime = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
			pos += 2;
			m_AcpVehicleCondCommandList.CollectCount = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
			pos += 2;
			m_AcpVehicleCondCommandList.SignTotal = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
			pos += 2;

			RespondTspPacket((AcpAppID_E )applicationID, MsgType+1, AcpCryptFlag_IS, TspSourceID);
			break;
		case 0:
			if(search(Headerlink, applicationID) != NULL)
			{	
				pthread_mutex_lock(&mutex);
				delete_link(&Headerlink, applicationID);
				pthread_mutex_unlock(&mutex);
				if(applicationID == ACPApp_GPSID)
				{
					TimeOutType.VehGPSFlag = 0;
				}
				else
				{
					TimeOutType.VehQueCondFlag = 0;
				}
			}
			FAWACPLOG("AcpQueryVehicleCond ack is OK \n");
			break;
		default:
			break;
	}
	return 0;
}


/*******unpack*****/
uint16_t CFAWACP::UnpackData_AcpRemoteConfig(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;
	uint16_t DataLen = 0;
	uint8_t RequestSource = 0;
	uint8_t TspSourceID   =	0;
	
	ShowdataTime(pos);
	pos += DATA_TIMESTAMP_SIZE;
	//AuthToken
	pos++;
	memcpy(AuthToken, pos, sizeof(AuthToken));
	if( memcmp(p_FAWACPInfo_Handle->AuthToken, AuthToken, sizeof(AuthToken)) != 0)
		FAWACPLOG("AuthToken is fault !");
	pos += sizeof(AuthToken);
	//Source
	pos++;
	TspSourceID = *pos++;
	
	switch(MsgType)
	{
		case 1:
			if(search(Headerlink, ACPApp_RemoteConfigID) != NULL)
				return 0;
			
			pos += unpackDataLenDeal(pos,DataLen);

			RequestSource = *pos++;
			FAWACPLOG("Request Message Source -- %d\n",RequestSource);

			m_AcpRemoteConfig.DeviceTotal = *pos++;	
			FAWACPLOG("Device Total == %d\n",m_AcpRemoteConfig.DeviceTotal);
			for(uint8_t i = 0; i < m_AcpRemoteConfig.DeviceTotal; i++)
			{
				m_AcpRemoteConfig.DeviceConfig[i].DeviceNo = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
				pos += 2;
				FAWACPLOG("DeviceNo == %d\n",m_AcpRemoteConfig.DeviceConfig[i].DeviceNo);
				m_AcpRemoteConfig.DeviceConfig[i].ConfigCount = *pos++;
				FAWACPLOG("ConfigCount == %d\n",m_AcpRemoteConfig.DeviceConfig[i].ConfigCount);
				for(uint8_t k = 0; k < m_AcpRemoteConfig.DeviceConfig[i].ConfigCount; k++)
				{
					m_AcpRemoteConfig.DeviceConfig[i].ConfigItem[k].ConfigItemNum = *pos++;
					FAWACPLOG("ConfigItemNum == %d\n",m_AcpRemoteConfig.DeviceConfig[i].ConfigItem[k].ConfigItemNum);
					switch(m_AcpRemoteConfig.DeviceConfig[i].ConfigItem[k].ConfigItemNum)
					{
						case ACPCfgItem_EngineStartTimeID:
							 
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EngineStartTime = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
							pos += 2;
							FAWACPLOG("EngineStartTime == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EngineStartTime);
							switch(i)
							{
								case 0:	//tbox
									//01-05 KK modify mcu callback
									//reportRemoteConfigCmd(m_CMcu->cb_RemoteConfigCmd, 0x01, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EngineStartTime);
									Req_McuRemoteCofig( 0x01, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EngineStartTime );
									break;
								default:
									break;
							}
							break;
						case ACPCfgItem_SamplingSwitchID:
							
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].SamplingSwitch = *pos++;
							FAWACPLOG("SamplingSwitch == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].SamplingSwitch);
							break;
						case ACPCfgItem_EmergedCallID:
							memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EmergedCall,pos,15);
							dataPool->setPara(E_CALL_ID, (void *)p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EmergedCall, strlen(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EmergedCall));	
							FAWACPLOG("EmergedCall == %s\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].EmergedCall);
							pos += 15;
							break;
						case ACPCfgItem_WhitelistCallID:
							 
							memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].WhitelistCall,pos,15);
							FAWACPLOG("WhitelistCall == %s\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].WhitelistCall);
							pos += 15;
							break;
						case ACPCfgItem_CollectTimeIntervalID:
							
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].CollectTimeInterval = *pos++;
							FAWACPLOG("CollectTimeInterval == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].CollectTimeInterval);
							break;
						case ACPCfgItem_ReportTimeIntervalID:
							
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ReportTimeInterval = *pos++;						
							switch(i)
							{
								case 0:	//tbox
									//01-05 KK modify mcu callback
									//reportRemoteConfigCmd(m_CMcu->cb_RemoteConfigCmd, 0x02, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].CollectTimeInterval);
									Req_McuRemoteCofig( 0x02, p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].CollectTimeInterval );
									break;
								default:
									break;
							}
							break;
						case ACPCfgItem_CollectTimeGpsSpeedIntervalID:
							
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].CollectTimeGpsSpeedInterval = *pos++;
							FAWACPLOG("CollectTime GpsSpeed Interval == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].CollectTimeGpsSpeedInterval);
							break;
						case ACPCfgItem_ReportTimeGpsSpeedIntervalID:
							
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ReportTimeGpsSpeedInterval = *pos++;
							FAWACPLOG("ReportTime GpsSpeed Interval == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ReportTimeGpsSpeedInterval);
							break;
						case ACPCfgItem_ChargeModeID:
							
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ChargeMode = *pos++;
							FAWACPLOG("ChargeMode == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ChargeMode);
							break;
						case ACPCfgItem_ChargeScheduleID:
							
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ChargeSchedule.ScheduChargStartData = *pos++;
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ChargeSchedule.ScheduChargStartTime = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
							pos += 2;
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ChargeSchedule.ScheduChargEndData = *pos++;
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].ChargeSchedule.ScheduChargEndTime = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
							pos += 2;
							break;
						case ACPCfgItem_LoadCellID:
							memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].LoadCell,pos,15);
							dataPool->setPara(B_CALL_ID, (void *)p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].LoadCell, strlen(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].LoadCell));	
							FAWACPLOG("LoadCell == %s\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].LoadCell);
							pos += 15;
							break;
						case ACPCfgItem_InformationCellID:
							memcpy(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[i].InformationCell,pos,15);
							pos += 15;
							break;
						default:
							break;
					}
				}				
			}
			if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
			{
				updateTBoxParameterInfo();
			}
			RespondTspPacket(ACPApp_RemoteConfigID, MsgType+1, AcpCryptFlag_IS, TspSourceID);
			break;
		case 0:
			if(search(Headerlink, ACPApp_RemoteConfigID) != NULL)
			{
				pthread_mutex_lock(&mutex);
				delete_link(&Headerlink, ACPApp_RemoteConfigID);
				pthread_mutex_unlock(&mutex);
				TimeOutType.RemoteCpnfigFlag = 0;
			}
			FAWACPLOG("AcpRemoteConfig ack is ok!\n");
			break;
		default:
			break;
	}
	return 0;
}

/*******unpack RootkeyData********/
uint16_t CFAWACP::UnpackData_AcpUpdateRootKey(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;
	uint16_t DataLen = 0;
	uint8_t RequestSource = 0;
	uint8_t TspSourceID   =	0;
	uint8_t New_RootKey[32] = {0};
	
	ShowdataTime(pos);
	pos += DATA_TIMESTAMP_SIZE;
	//AuthToken
	pos++;
	memcpy(AuthToken, pos, sizeof(AuthToken));
	if(memcmp(p_FAWACPInfo_Handle->AuthToken, AuthToken, sizeof(AuthToken)) != 0)
		FAWACPLOG("AuthToken is fault !");
	pos += sizeof(AuthToken);
	//Source
	pos++;
	TspSourceID = *pos++;
	switch(MsgType)
	{
		case 2:
			if(search(Headerlink, ACPApp_UpdateKeyID) != NULL)
				return 0;
			m_bEnableSendDataFlag = false;
			pos += unpackDataLenDeal(pos,DataLen);

			RequestSource = *pos++;
			FAWACPLOG("Request Message Source -- %d\n",RequestSource);
			Decrypt_AES128Data(ALLRootKey, pos, 32, New_RootKey, AcpCryptFlag_IS);
			memcpy(p_FAWACPInfo_Handle->New_RootKey, New_RootKey, sizeof(p_FAWACPInfo_Handle->New_RootKey));
			pos += 32;
			RespondTspPacket(ACPApp_UpdateKeyID, MsgType + 1, AcpCryptFlag_IS, TspSourceID);
			break;
		case 0:
			if(search(Headerlink, ACPApp_UpdateKeyID) != NULL)
			{
				pthread_mutex_lock(&mutex);
				delete_link(&Headerlink, ACPApp_UpdateKeyID);
				pthread_mutex_unlock(&mutex);
				TimeOutType.UpdateRootkeyFlag = 0;
			}
			memcpy(ALLRootKey,p_FAWACPInfo_Handle->New_RootKey,ROOTKEY_SIZE);
			m_bEnableSendDataFlag = true;
			FAWACPLOG("UpdateRootKey ack is ok! \n");
			break;
		default:
			break;
	}
	return 0;
}
int CFAWACP::DownLoadUpgradePackage()
{
	#define  DOWNLOAD_UPGRADE_MIN_SIGNAL_LEVEL     (35)
	
	char domain_name[32] = {0};
	char target_ip_new[16] = {0};
	char host_protocol[8] = {0};
	uint16_t host_port = 0;
	char file_path[64] = {0};
	uint8_t cmd[256]= {0};
	uint8_t output[16]= {0};
	int SignalLevel  = 0;
	int SignalMode  = 0;
	int RetValue  = 0;

	datacall_info_type datacall_info;
	printf("jason add DownLoadUpgradePackage ^_^^_^^_^^_^^_^^_^^_^^_^^_^^_^\r\n");

	parse_http_url((char *)m_AcpRemoteUpgrade.URL_IP, host_protocol, &host_port, domain_name, file_path);

	get_datacall_info_by_profile(4,&datacall_info);//new sdk interface
	
	if(datacall_info.status == DATACALL_CONNECTED)
	{

		int ret = query_ip_from_dns(domain_name, datacall_info.pri_dns_str ,/*datacall_info.pri_dns_str*/datacall_info.sec_dns_str , target_ip_new);
		//	printf("target_ip_new == %s\n", target_ip_new);
		FAWACPLOG("3333333333333%d\n",ret);
	}
	else
	{
		printf("jason add datacall_info.status is disconnected \r\n");
		return RetValue;
	}
	
	get_SignalStrength(&SignalLevel,&SignalMode);
	
	if(SignalLevel < DOWNLOAD_UPGRADE_MIN_SIGNAL_LEVEL)
	{
		printf("jason add signal too low SignalLevel =%d ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ \r\n",SignalLevel);

		printf("jason add signal too low  ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ ^v^ \r\n");

		return RetValue;
	}
	if(strlen(target_ip_new) != 0 )
	{
		sprintf(cmd, "wget -c --timeout=%d \"%s%s:%d%s\" -O \"%s\"", HTTP_DOWNLOAD_TIMEOUT, host_protocol, target_ip_new, host_port, file_path, MQTT_UPDATE_FILE);
	}
	else
	{
		sprintf(cmd, "wget -c --timeout=%d \"%s\" -O \"%s\"", HTTP_DOWNLOAD_TIMEOUT, m_AcpRemoteUpgrade.URL_IP, MQTT_UPDATE_FILE);
	}
	system(cmd);
	system("sync");

	printf("jason add signal wget  WWWWWWWWWWWWWWWW \r\n");

	m_u8TryDownLoadCount++;
	
	if (!access( MQTT_UPDATE_FILE, F_OK ))	
	{
		Upgrade_DownloadState = UPGRADE_SUCCESS;
		memset(output,0,16);
		m_Hmacmd5OpData.md5test( MQTT_UPDATE_FILE ,output );
		for(int j=0;j<16;j++)
		{
			if(output[j]!=m_AcpRemoteUpgrade.UpdateFile_MD5[j])
			{
				Upgrade_DownloadState = UPGRADE_MD5_ERROR;
				Upgrade_CommandResponse=UPGRADE_URL_ERROR;
				sprintf( cmd, "rm -rf %s\0", MQTT_UPDATE_FILE );
				system( cmd );
				RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, 0);
				printf("md5 error\r\n");
				system("sync");
				break;
			}
		}
		if(UPGRADE_SUCCESS == Upgrade_DownloadState)
		{	
			RetValue = 1;
			RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, 0);
			printf("jason add download success hahhahahahaahhahahahhahahha \r\n");

			return RetValue;
		}
	}

	if(m_u8TryDownLoadCount > 3)
	{
		RetValue = 1;
		Upgrade_DownloadState = UPGRADE_URL_ERROR;		
		Upgrade_CommandResponse=UPGRADE_URL_ERROR;
		RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, 0);
		printf("jason add download failed fuckfuckfuckfuckfuckfuckfuckfuckfuck \r\n");
		ftpupgrade = false;
	}
	else
	{

	}
	return RetValue;
}


/*******unpack********/
uint16_t CFAWACP::UnpackData_AcpRemoteUpgrade(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;
	uint16_t DataLen = 0;
	uint8_t RequestSource = 0;
	uint8_t TspSourceID   =	0;
	uint8_t *Objective_file = NULL;
	uint8_t u8SendCount;
	//
	ShowdataTime(pos);
	pos += DATA_TIMESTAMP_SIZE;
	pos++;
	//AuthToken
	//pos++;
	//memcpy(AuthToken, pos, sizeof(AuthToken));
	//if( memcmp(p_FAWACPInfo_Handle->AuthToken, AuthToken, sizeof(AuthToken)) != 0)
	//	FAWACPLOG("AuthToken is fault !");
	//pos += sizeof(AuthToken);
	//Source
	//pos++;
	//TspSourceID = *pos++;
	Upgrade_CommandResponse = 0;
	int i, ret;
	uint8_t cmd[256]= {0};
	uint8_t output[16]= {0};

	uint16_t host_port = 0;
	char domain_name[32] = {0};
	char file_path[64] = {0};
	char host_protocol[8] = {0};
	char target_ip_new[16] = {0};
	char log_str[128] = {0};
	char *os = NULL;
	switch(MsgType)
	{
		case UpgradeStep_1:
			{
				FAWACPLOG("current or latest is %d\n",*pos);
				#if 0
				if(mqtt_config.m_TestOrNormal != *pos)
				{
					mqtt_config.m_TestOrNormal = *pos;
					updateMQTTParameterInfo();
					system("sync");
				}
				#endif
				cfawacp->MQTTtimingReportingData(UpgradeStep_2, ACPApp_RemoteUpgradeID);
				ftpupgrade = false;
				break;
			}
		case UpgradeStep_3:
			{
				RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_4, AcpCryptFlag_IS, TspSourceID);

				Upgrade_DownloadState = UPGRADE_INIT;
				*pos++;//cu or la
				m_AcpRemoteUpgrade.OSID = ((*pos++) << 8) + (*pos++);
				
				FAWACPLOG("osid  is %d\n",m_AcpRemoteUpgrade.OSID);
				printf("jason add UpgradeStep_3 osid  is %d \r\n",m_AcpRemoteUpgrade.OSID);

				memcpy(m_AcpRemoteUpgrade.UpdateFile_MD5, pos, 16);
				pos += 16;

				m_AcpRemoteUpgrade.URL_Len = *pos++;
				FAWACPLOG("URL_Len is%02x\n",m_AcpRemoteUpgrade.URL_Len);


				memcpy(m_AcpRemoteUpgrade.URL_IP, pos,int(m_AcpRemoteUpgrade.URL_Len));
				m_AcpRemoteUpgrade.URL_IP[int(m_AcpRemoteUpgrade.URL_Len)] = '\0';
				FAWACPLOG("m_AcpRemoteUpgrade.URL_IP = %s\n", m_AcpRemoteUpgrade.URL_IP);
				os = strstr((char *)m_AcpRemoteUpgrade.URL_IP, "ver");
				if(os != NULL)
				{
					sprintf(log_str, "%s%33.30s", "sdk version: ", os);
					if(m_CMcu->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
					{
						WRITELOGFILE(MQTT_LOG_INFO, log_str);
					}
					printf("%s(%d): %33.30s\n", __func__, __LINE__, os);
					/*ver1602317020180221242.tar.gz*/
					/*1.4sdk*/
					if(strstr(os, REMOTE_UPDATA_OS_KEY_WORD) != NULL)  /*same version info ,save osid */
					{
						ret = save_osid_in_file(MQTT_VERSION_NORMAL, (int)m_AcpRemoteUpgrade.OSID);
					}
				}
				/*************************/
				parse_http_url((char *)m_AcpRemoteUpgrade.URL_IP, host_protocol, &host_port, domain_name, file_path);
				FAWACPLOG("Url information is %s\n%d\n%s\n%s\n", host_protocol, &host_port, domain_name, file_path);

				/*OSID*/
				ret = get_osid_from_file(MQTT_VERSION_NORMAL);
				if (ret == -1)
				{
					printf("get osid error!\n");
				}
				else
				{
					if(ret == m_AcpRemoteUpgrade.OSID)

					{
						/*do something*/
						printf("OSID same\n");
						Upgrade_DownloadState = UPGRADE_OSID_SAME;
						RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, TspSourceID);
						ftpupgrade = false;
						return 0;
					}
					else
					{
						/* do something */
						//printf("go to download upgrade pack.\n");
						//printf("MD5 verifying.\n");
						/*save OSID to 3th line*/
						ret = save_osid_in_file(MQTT_VERSION_TEST, (int)m_AcpRemoteUpgrade.OSID);
					}
				}
				
				m_u8TryDownLoadFlag = 1;
#if 0
				datacall_info_type datacall_info;

				get_datacall_info_by_profile(4,&datacall_info);//new sdk interface
				if(datacall_info.status == DATACALL_CONNECTED)
				{

					int ret = query_ip_from_dns(domain_name, datacall_info.pri_dns_str ,/*datacall_info.pri_dns_str*/datacall_info.sec_dns_str , target_ip_new);
					//	printf("target_ip_new == %s\n", target_ip_new);
					FAWACPLOG("3333333333333%d\n",ret);
				}

				if( strlen(target_ip_new) != 0 )
				{
					sprintf(cmd, "wget -c --timeout=%d \"%s%s:%d%s\" -O \"%s\"", HTTP_DOWNLOAD_TIMEOUT, host_protocol, target_ip_new, host_port, file_path, MQTT_UPDATE_FILE);
				}
				else
				{
					sprintf(cmd, "wget -c --timeout=%d \"%s\" -O \"%s\"", HTTP_DOWNLOAD_TIMEOUT, m_AcpRemoteUpgrade.URL_IP, MQTT_UPDATE_FILE);
				}
				//	printf("cmd == %s\n", cmd);
				system(cmd);
				system("sync");
				FAWACPLOG("4444444444444444\n");
				for(uint8_t i = 0; i < 3; i++)
				{
					if (!access( MQTT_UPDATE_FILE, F_OK ))	
					{
						Upgrade_DownloadState = UPGRADE_SUCCESS;
						break;
					}
					else
					{
						Upgrade_DownloadState = UPGRADE_URL_ERROR;		
						system(cmd);
						system("sync");
						sleep(1);
					}
				}

				if(Upgrade_DownloadState ==UPGRADE_URL_ERROR)			
				{
					FAWACPLOG("555555555555555\n");
					Upgrade_CommandResponse=UPGRADE_URL_ERROR;
					RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, TspSourceID);
					ftpupgrade = false;
					return 0;
				}

				memset(output,0,16);
				m_Hmacmd5OpData.md5test( MQTT_UPDATE_FILE ,output );
				for(int j=0;j<16;j++)
				{
					if(output[j]!=m_AcpRemoteUpgrade.UpdateFile_MD5[j])
					{
						FAWACPLOG("6666666666666666\n");
						Upgrade_DownloadState = UPGRADE_MD5_ERROR;
						Upgrade_CommandResponse=UPGRADE_URL_ERROR;
						sprintf( cmd, "rm -rf %s\0", MQTT_UPDATE_FILE );
						system( cmd );
						RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, 0);
						printf("md5 error\n");
						system("sync");
						break;
					}
				}

				if(UPGRADE_SUCCESS == Upgrade_DownloadState)
				{
					//sprintf(cmd, "tar -zxvf %s -C /data", MQTT_UPDATE_FILE);
					//FAWACPLOG("cmd == %s\n", cmd);
					//system(cmd);
					//system("sync");
					FAWACPLOG("7777777777777777777777\n");
					RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, 0);
				}
#endif

			}
			break;
		case UpgradeStep_7:
			FAWACPLOG("current or latest is %d\n",*pos);
			printf("jason add current or latest is %d~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n",*pos);
			if(mqtt_config.m_TestOrNormal != *pos)
			{
				mqtt_config.m_TestOrNormal = *pos;
				updateMQTTParameterInfo();
				system("sync");
				printf("current or latest is %d\n",*pos);

			}
			RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_8, AcpCryptFlag_IS, 0);
			ftpupgrade = false;
			break;

		case 0:
			FAWACPLOG("AcpRemoteUpgrade ACK is OK\r\n");
			break;
		default:
			break;
	}
	return 0;
}

int CFAWACP::AccOffUpgrade()
{
	int times = 0;
	int nMcuWake4GStatus = 0;
	static int Ble_UpgradeTimes = 0;
	uint8_t TempData = 0x01;
	uint8_t u8SendCount;
	uint8_t cmd[256]= {0};
	static int upgrade_stat = 0;

	printf("\n%d: %s\n", __LINE__, __func__);

	if(access(MQTT_UPDATE_FILE, F_OK) == 0)
	{
		sprintf(cmd, "tar -zxvf %s -C /data", MQTT_UPDATE_FILE);
		FAWACPLOG("cmd == %s\n", cmd);
		system(cmd);
		system("sync");
		system("rm -f /data/mqttupdate.tar.gz");
	}
	ftpupgrade = false;
	if(upgrade_stat != 0)
	{
		switch (upgrade_stat)
		{
			case UPGRADE_MCU:
				if(access(MCU_FILE_NAME, F_OK) != 0)
					upgrade_stat = 0;
				break;
			case UPGRADE_BLE:
				if(access(BLE_FILE_NAME, F_OK) != 0)
					upgrade_stat = 0;
				break;
			case UPGRADE_4G:
				if(access(LTE_FILE_NAME, F_OK) != 0)
					upgrade_stat = 0;
				break;
		}
	}
	else
	{
		if(access(MCU_FILE_NAME, F_OK) == 0)
		{
			upgrade_stat = UPGRADE_MCU;
		}
		else if(access(BLE_FILE_NAME, F_OK) == 0)
		{
			upgrade_stat = UPGRADE_BLE;
		}
		else if(access(LTE_FILE_NAME, F_OK) == 0)
		{
			upgrade_stat = UPGRADE_4G;
		}
		else
		{
			upgrade_stat = 0;
		}
		switch (upgrade_stat)
		{
			case 1:
				printf("%s(%d): mcu upgrade !!!!!!!!\n\n\n\n", __func__, __LINE__);
				m_CMcu->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
				m_CMcu->mcuUpgradeMode = MCU_UPGRADE_STATE_WAIT_MCU_REQ;
				check_mcu_file_is_exist();
				break;
			case 2:
				printf("%s(%d): ble upgrade !!!!!!!!\n\n\n\n", __func__, __LINE__);
				m_bleUart->packbleProtocolData(0x87,0x01,NULL,1,0);
				break;
			case 4:
				printf("\n%d: %s\n", __LINE__, __func__);
				system("mv /data/ostbox16 /data/LteUpgrade.bin");
				system("rm /data/ostbox17");
				system("rm /data/ostbox18");
				system("sync");
				u8SendCount = 2;
				do{
					if(sendATCmd((char*)"at+cfun=0", (char*)"OK", NULL, 0, 5000) <=0 )
					{
						printf("jason add error send +cfun=0 cmd failed XXXXXXXXXXXXXXX\r\n");
					}
					else
					{
						printf("jason add send +cfun=0 cmd success \r\n");
						break;
					}
					sleep(5);
					u8SendCount--;
				}while(u8SendCount > 0);
				modem_ri_notify_mcu_high();
				sleep(1);
				modem_ri_notify_mcu_low();
				sleep(1);
				u8SendCount = 2;
				do{
					if(sendATCmd((char*)"at+cfun=1", (char*)"OK", NULL, 0, 5000) <=0 )
					{
						printf("jason add error send +cfun=1 cmd failed XXXXXXXXXXXXXXX\r\n");
					}
					else
					{
						printf("jason add send +cfun=1 cmd success \r\n");
						break;
					}
					sleep(5);
					u8SendCount--;
				}while(u8SendCount > 0);
				//LteAtCtrl->~LTEModuleAtCtrl();
				//system( RM_QMUX_SOCKET );
				printf("################### exit 2 ###########################\n");
				sleep(1);
				system("sys_reboot");

				break;
			default:
				break;
		}
	}
	printf("\n%d:%s\n", __LINE__, __func__);

	return 0;
}

/*******unpack*******/
uint16_t CFAWACP::UnpackData_AcpRemoteDiagnostic(uint8_t *PayloadBuff, uint16_t payload_size, uint8_t MsgType)
{
	uint8_t *pos = PayloadBuff;
	uint16_t DataLen = 0;
	uint8_t RequestSource = 0;
	uint8_t TspSourceID   =	0;
	
	ShowdataTime(pos);
	pos += DATA_TIMESTAMP_SIZE;
	//AuthToken
	pos++;
	memcpy(AuthToken, pos, sizeof(AuthToken));
	if( memcmp(p_FAWACPInfo_Handle->AuthToken, AuthToken, sizeof(AuthToken)) != 0)
		FAWACPLOG("AuthToken is fault !");
	pos += sizeof(AuthToken);
	//Source
	pos++;
	TspSourceID = *pos++;
	switch(MsgType)
	{
		case 1:
			if(search(Headerlink, ACPApp_RemoteDiagnosticID) != NULL)
				return 0;

			pos += unpackDataLenDeal(pos,DataLen);

			RequestSource = *pos++;
			FAWACPLOG("Request Message Source -- %d\n",RequestSource);

			m_RemoteDiagnose.DiagnoseTotal = *pos++;
			for(uint8_t i = 0; i < m_RemoteDiagnose.DiagnoseTotal; i++)
				m_RemoteDiagnose.DiagnoseCode[i] = *pos++;

			RespondTspPacket(ACPApp_RemoteDiagnosticID, MsgType+1, AcpCryptFlag_IS, TspSourceID);
			break;
		case 0:
			if(search(Headerlink, ACPApp_RemoteDiagnosticID) != NULL)
			{
				pthread_mutex_lock(&mutex);
				delete_link(&Headerlink, ACPApp_RemoteDiagnosticID);
				pthread_mutex_unlock(&mutex);
				TimeOutType.RemoteDiagnoFlag = 0;
			}
			FAWACPLOG("AcpRemoteDiagnostic ack is ok!\n");
			break;
		default:
			break;
	}
	return 0;
}



void CFAWACP::HMACMd5_digest(const char *SecretKey, unsigned char *Content, uint16_t InLen, uint8_t OutBuff[16])
{
	int i ,j;
	unsigned char *tempBuffer = (unsigned char *)malloc(MAX_FILE + 64); 
	unsigned char Buffer2[80]; 
	unsigned char key[16];
	unsigned char ipad[64], opad[64];

	if(strlen(SecretKey) > 16 )
		m_Hmacmd5OpData.md5_digest(SecretKey, strlen(SecretKey), key);
	else if(strlen(SecretKey) < 16 )
	{
		i=0;
		while(SecretKey[i]!='\0')
		{
			key[i]=SecretKey[i];
			i++;
		}
		while(i<16)
		{
			key[i]=0x00;
			i++;
		}
	}
	else
	{
		for(i=0;i<16;i++)
			key[i]=SecretKey[i];
	}

	for(i=0;i<64;i++)
	{
		ipad[i]=0x36;
		opad[i]=0x5c;
	}

	for(i=0;i<16;i++)
	{
		ipad[i]=key[i] ^ ipad[i]; 
		opad[i]=key[i] ^ opad[i];   
	}

	for(i=0;i<64;i++)
		tempBuffer[i]=ipad[i];
	for(i=64;i<InLen+64;i++)
		tempBuffer[i]=Content[i-64];

	m_Hmacmd5OpData.md5_digest(tempBuffer, InLen+64, OutBuff);

	for(j=0; j < 64; j++)
		Buffer2[j] = opad[j];
	for(i = 64; i< 80; i++)
		Buffer2[i] = OutBuff[i-64];

	m_Hmacmd5OpData.md5_digest(Buffer2, 80, OutBuff);
}


void CFAWACP::parse_http_url( char *url, char* host_protocol, uint16_t *host_port , char *domain_name, char *file_path)
{
	char *protocol[] = {"http://","https://","ftp://",NULL};
	int i,len = 0;

	//
	for (i = 0;protocol[i] != NULL;i++)
		if (!strncmp(url,protocol[i],strlen(protocol[i]))){
			len = strlen(protocol[i]);
			memcpy(host_protocol, url, len);
			break;
		}

	char *ptr = strchr(url + len,':');
	if (ptr != NULL){
		//
		strncpy( domain_name,url + len,strlen(url + len) - strlen(ptr));
		//
		sscanf(++ptr,"%5hu", host_port);
		//
		if (*host_port > 65535){
			printf("invalid port\n");
			return ;
		}
		//
		while(*ptr != '/')
			ptr++;
		strcpy(file_path,ptr);
	}
	else{
		ptr = strchr(url + len,'/');
		
		strncpy( domain_name, url + len, strlen(url + len) - strlen(ptr));
		
		*host_port = 80;
		
		strcpy(file_path,ptr);
	}
}


