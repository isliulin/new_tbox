#include "MQTTACP.h"
#include "AES_acp.h"
#include "OTAWiFi.h"
#include "common.h"
#if 0
pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_mutex_Rbuf = PTHREAD_MUTEX_INITIALIZER;

MQTT* MQTT::mqtt = NULL;


extern bool APN2test;
extern Timelog_t timelog;
extern bool  flaftime4g;

extern GPS_Info_t	GPSINFO;
extern bool bleupgrade;
extern bool ftpupgrade;

static uint16_t SocketReconnectCount  =0;
static uint16_t SocketDisconnectCount =0;


extern void SetTspReDatacallFlag(unsigned char ReDatacallFlag);

extern uint8_t GetTspReDatacallFlag(void);

extern uint8_t GetModemWorkOn(void);


void MQTT::mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(message->payloadlen)
	{
        printf("%s %s", message->topic, (char *)message->payload);
    }
	else
	{
        printf("%s (null)\n", message->topic);
    }
    //fflush(stdout);
	if(message->payload[0] == 0x7e)
	{
		switch(message->topic)
		{
			case mqtt->MQTT_Topic[RemoteCtrlTP]:

				break;
			default:
				break;
		}
	}
	
}

void MQTT::mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int mid)
{
	MQTTLOG("$$$$$$$$$$$$$$$$$$$$$$$\n\n\n");
	MQTTLOG("$$$$$$$$$$$$$$$$$$$$$$$\n\n\n");
	mqtt->TspWorkMode= TSP_WORK_MODE_REQ_MODEM_REBOOT;
	system("sys_reboot");
}

void MQTT::mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    if(!result)
	{
		MQTTLOG("####################\n\n\n");
		MQTTLOG("####################\n\n\n");
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq,NULL,mqtt->MQTT_Topic[RemoteCtrlTP],1);
    }
	else
	{
		MQTTLOG("@@@@@@@@@@@@@@@@@@@@\n\n\n");
		MQTTLOG("####################\n\n\n");
        fprintf(stderr, "Connect failed\n");
    }
}






void MQTT::Set_ReportRemoteConfigCmd( 	   callback_RemoteConfigCmd cb_RemoteConfigCmd )
{
	if(NULL == cb_RemoteConfigCmd)	return ;

	Req_McuRemoteCofig = cb_RemoteConfigCmd;
		
}
void MQTT::Set_reportRemoteCtrlCmd( 		callback_RemoteCtrlCmd cb_RemoteCtrlCmd )
{
	if(NULL == cb_RemoteCtrlCmd) return ;

	Req_McuRemoteCtrl = cb_RemoteCtrlCmd;
	
}

void MQTT::RegisterMcu_CallbackFun()
{
	Set_ReportRemoteConfigCmd( m_mcuUart->Mcu_RemoteConfigRes );
	Set_reportRemoteCtrlCmd( m_mcuUart->Mcu_RemoteCtrlRes );
}

static MQTT* MQTT::GetInstance()
{

	if(mqtt == NULL)
	{
		mqtt = new MQTT();
	}
	return mqtt;

}
#define  TBOX_WORK_MODE_SLEEP    0
#define  TBOX_WORK_MODE_WORKON	 1

MQTT::MQTT()
{
	
	m_ConnectedState = false;
	m_Upgrade= false;
	
	m_loginState = STATUS_LOGGIN_INIT;
	
	TspWorkMode  = TSP_WORK_MODE_First_Login;
	
	m_SendMode = 0;		//����ģʽ0:��ʱ����	1:����
	m_bEnableSendDataFlag = true;   //����ʹ�ܱ�־
	Headerlink = NULL;
	ConnectFaultTimes = 0;
	TboxSleepStatus = 1;
	InLoginFailTimes = 0;
	OutLoginFailTimes = 0;
	HighReportTime = 0;
	LowReportTime = 0;
	NowHighReportTime = 0;
	NowLowReportTime = 0;
	TSPACK_WorkWakeup_State = 0;
	TSPACK_WorkSleep_State = 0;
	Req_McuRemoteCtrl = NULL;
	Req_McuRemoteCofig = NULL;

	ftpupgrade = false;
	flaftime4g = true;

	session = true;
	mosq = NULL;
}

MQTT::~MQTT()
{
	pthread_mutex_destroy(&m_mutex);
	pthread_mutex_destroy(&m_mutex_Rbuf);
	
	if(mqtt != NULL)
	{
		delete mqtt;
		mqtt = NULL;
	}
}

void MQTT::RUN_MQTT_TASK()
{
    


   
	uint8_t u8AcpReportTime;
	uint8_t u8WaitTspLoginTime;
	uint8_t u8WaitNewIPTime;
	uint8_t u8SendCount;
	m_mcuUart = mcuUart::Get_McuInstance();
	m_bleUart = bleUart::Get_BleInstance();
    pfawacp =  CFAWACP::GetInstance();

	int ret = 0;

	while(1)
	{
		switch(TspWorkMode)
		{
			case TSP_WORK_MODE_REQ_MODEM_REBOOT:
				if(u8WaitNewIPTime++ < 10)/*�ȴ�5s ��λ*/
				{
					break;
				}
				#if 0 ////jason add 2019.4.15
				//wds_qmi_release();//old interface zhujw
				wds_deinit();
				//nas_qmi_release();
				nas_dinit();
				//voice_qmi_release();
				voice_deinit();
				//mcm_sms_release();
				sms_deinit();
			
				#endif
				MQTTLOG("jason add faw reboot state ^^^^^^^^^^^^^^^^^^^^^^^^\r\n");
				u8SendCount = 2;
				do{
					if(sendATCmd((char*)"at+cfun=0", (char*)"OK", NULL, 0, 5000) <=0 )
					{
						MQTTLOG("jason add error send +cfun=0 cmd failed XXXXXXXXXXXXXXX\r\n");
					}
					else
					{
						MQTTLOG("jason add send +cfun=0 cmd success \r\n");
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
						MQTTLOG("jason add error send +cfun=1 cmd failed XXXXXXXXXXXXXXX\r\n");
					}
					else
					{
						MQTTLOG("jason add send +cfun=1 cmd success \r\n");
						break;
					}
					sleep(5);
					u8SendCount--;
				}while(u8SendCount > 0);				
				sleep(1);
				//system( RM_QMUX_SOCKET );
				system("sys_reboot");
				break;
			case TSP_WORK_MODE_First_Login:
				/*
    			if(readMQTTParameterInfo()==-1)
    			{
       			 //下载配置文件
				}
				检测是否有注册
				*/
				#if 1
				memset(&mqtt_config,sizeof(mqtt_config),0);
				strcpy(mqtt_config.m_HOST,"111.200.239.208");
				mqtt_config.m_PORTS = 18836;
				strcpy(mqtt_config.m_PSK, "123456");
				strcpy(mqtt_config.m_PSK_ID,"yyhhuunnmmtt");
				strcpy(mqtt_config.m_USERNAME ,"guo");
				strcpy(mqtt_config.m_PASSWARD ,"123456");
				mqtt_config.m_KEEP_ALIVE = 60;

				#endif
				MQTTLOG("FFFFFFFFFFFFFFFFFFFFFFFF");
				TspWorkMode = TSP_WORK_MODE_MQTT_INIT;
				break;
			case TSP_WORK_MODE_MQTT_INIT:
				RegisterMcu_CallbackFun();
	
				pfawacp->InitAcpTBoxAuthData();
				if(pfawacp->readTBoxParameterInfo() == -1){
					pfawacp->initTBoxParameterInfo();
				}

				if(pthread_create(&SendThreadId, NULL, SendThread, this) != 0)
					MQTTLOG("Cannot creat SendThread:%s\n", strerror(errno));
				

				if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval <=9)
				{
					p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval = 10;
					pfawacp->updateTBoxParameterInfo();
				}
				if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval<=9)
				{
					p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = 60;
					pfawacp->updateTBoxParameterInfo();
				}
				if(tboxInfo.operateionStatus.isGoToSleep == 0) /*sleep on*/
				{
					u8AcpReportTime = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval;
				}
				else //if(tboxInfo.operateionStatus.isGoToSleep == 1 && m_mcuUart->AccStatus== STATUS_ACC_OFF)
				{/*work on*/
					u8AcpReportTime = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval;
					//p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval = 20;
				}

				u8WaitNewIPTime = 0 ;

				UpdataMQTTTopic(p_FAWACPInfo_Handle->Vin);

				mosquitto_lib_init();
				mosq = mosquitto_new(NULL,session,NULL);
				if(!mosq)
				{
					MQTTLOG("create client failed..\n");
					mosquitto_lib_cleanup();
					break;
				}

				mosquitto_connect_callback_set(mosq, mqtt_connect_callback);
				mosquitto_message_callback_set(mosq, mqtt_message_callback);
				mosquitto_disconnect_callback_set(mosq, mqtt_disconnect_callback);
				

				ret = mosquitto_username_pw_set(mosq,mqtt_config.m_USERNAME,mqtt_config.m_PASSWARD);
				MQTTLOG("mosquitto_username_pw_set ret =%d\n",ret);

				ret = mosquitto_tls_psk_set(mosq,mqtt_config.m_PSK,mqtt_config.m_PSK_ID,NULL);
				MQTTLOG("mosquitto_tls_psk_set ret = %d\n",ret);
				int loop;
				MQTTLOG("IIIIIIIIIIIIIIIIIIIIIIIIIIIIIII");
				TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REDATACALL;
				//ret = mosquitto_subscribe(mosq,NULL,MQTT_Topic[RemoteCtrlTP],1);//订阅主题
				//MQTTLOG("mosquitto_subscribe ret =%d\n",ret);
				break;

			case TSP_WORK_MODE_REQ_MODEM_REDATACALL:
				
				MQTTLOG("jason add faw 222222222222222222222222222222222\r\n");
				if(GetTspReDatacallFlag() == 1)
				{
					if(u8WaitNewIPTime++ < 6)
					break;
				}
				TspWorkMode = TSP_WORK_MODE_WAIT_MODEM_FINISH;
				break; 
			case TSP_WORK_MODE_WAIT_MODEM_FINISH :
				
				MQTTLOG("jason add faw 333333333333333333333333333333\r\n");
				if(GetModemWorkOn()/*GetTspNewIpFlag() == 1*/)
				{
					TspWorkMode = TSP_WORK_MODE_SOCKET_CONNECT;
					ConnectFaultTimes = 0;
					
				MQTTLOG("jason add faw 44444444444444444444444444444\r\n");
				}
				else
				{
					u8WaitNewIPTime++;
					if(u8WaitNewIPTime > 160) /*80s time out */ 
					{
						TspWorkMode = TSP_WORK_MODE_SOCKET_CONNECT;
						
						ConnectFaultTimes = 0;
						
					MQTTLOG("jason add faw 55555555555555555555555555\r\n");
					}
					else
					{						
						break;
					}
				}
			case TSP_WORK_MODE_SOCKET_CONNECT :		
				MQTTLOG("jason add faw 6666666666666666666666666666666666\r\n");
				MQTTConnect();
				if(m_ConnectedState == false)
				{
					if(ConnectFaultTimes > 3) /*����3�β��ɹ������²���*/
					{
						//m_ConnectedState = true;
						ConnectFaultTimes = 0;
						TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REDATACALL;
						
					    MQTTLOG("jason add faw 777777777777777777777777777777\r\n");
						break;
					}
					else
					{
						SocketReconnectCount ++;
						if(SocketReconnectCount>=90)
							TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REBOOT;
						ConnectFaultTimes++;
						break;
					}
					#if 0
					{
						SocketReconnectCount ++;
						FAWACP_ERROR_Log(7, errno,SocketReconnectCount); /* re connect count*/
						if(flaftime4g)
						{
							time(&timelog.time4g);
							flaftime4g = false;
						}
					}
					#endif
				}
				else
				{
					TspWorkMode = TSP_WORK_MODE_WAIT_MCU_FINISH;
					
					MQTTLOG("jason add faw 888888888888888888888888888888888\r\n");
					ConnectFaultTimes = 0;
				}
			case TSP_WORK_MODE_WAIT_MCU_FINISH:
				
				MQTTLOG("jason add faw 9999999999999999999999999999\r\n");
				u8WaitTspLoginTime = 0;				
				//InitAcpTBoxAuthData();
				//ret = mosquitto_subscribe(mosq,NULL,MQTT_Topic[RemoteCtrlTP],1);//订阅主题
				//MQTTLOG("mosquitto_subscribe ret =%d\n",ret);
				loop = mosquitto_loop_start(mosq);
				MQTTLOG("mosquitto_loop_start = %d\n",loop);
				if(loop != MOSQ_ERR_SUCCESS)
				{
					MQTTLOG("mosquitto loop error\n");
                    break;
					
				}

				TspWorkMode = TSP_WORK_MODE_PERIOD_REPORT;
				break;
			

			case TSP_WORK_MODE_PERIOD_REPORT:
			default:
				//OutLoginFailTimes = 0;
				time(&timelog.time4g);
				//time(&NowHighReportTime);
				time(&NowLowReportTime);
				if(NowLowReportTime < LowReportTime)
				{
					time(&LowReportTime);
				}
				if(TboxSleepStatus == TBOX_WORK_MODE_SLEEP)
				{
					if(tboxInfo.operateionStatus.isGoToSleep != TboxSleepStatus) /*wake up event*/
					{
						MQTTtimingReportingData(MESSAGE_TYPE_START + 3, ACPApp_VehicleCondUploadID);
						TboxSleepStatus = tboxInfo.operateionStatus.isGoToSleep;
					}
					else if(NowLowReportTime - LowReportTime >= u8AcpReportTime) /* sleep heart data*/
					{
						if(u8AcpReportTime != p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval)
						{
							u8AcpReportTime = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval;
						}
						
						MQTTLOG("jason add asp work on report time = %d TSPACK_WorkWakeup_State = %d TSPACK_WorkSleep_State =%d \r\n",u8AcpReportTime,TSPACK_WorkWakeup_State,TSPACK_WorkSleep_State);
						MQTTtimingReportingData(MESSAGE_TYPE_START + 9, ACPApp_VehicleCondUploadID);
						time(&LowReportTime);
						TSPACK_WorkWakeup_State = 0;
						TSPACK_WorkSleep_State++;
					}

				}
				else
				{
					if(tboxInfo.operateionStatus.isGoToSleep != TboxSleepStatus) /*sleep  event*/
					{
						MQTTtimingReportingData(MESSAGE_TYPE_START + 8, ACPApp_VehicleCondUploadID);
						TboxSleepStatus = tboxInfo.operateionStatus.isGoToSleep;

					}
					else if(NowLowReportTime - LowReportTime >= u8AcpReportTime) /* normal data*/
					{

						#if 0
						if(u8AcpReportTime != p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval)
						{
							u8AcpReportTime = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval;
						}
						#endif
						if(mcuUart::Get_McuInstance()->AccStatus == STATUS_ACC_ON)
						{
							u8AcpReportTime = 10;
						}
						else
						{
							u8AcpReportTime = 20;

						}
						
						MQTTtimingReportingData(MESSAGE_TYPE_START + 2, ACPApp_VehicleCondUploadID);
						time(&LowReportTime);
						
						TSPACK_WorkSleep_State = 0;
						TSPACK_WorkWakeup_State++;
						MQTTLOG("jason add asp work on report time = %d TSPACK_WorkWakeup_State = %d TSPACK_WorkSleep_State =%d \r\n",u8AcpReportTime,TSPACK_WorkWakeup_State,TSPACK_WorkSleep_State);

						#if 0
						m_mcuUart->packProtocolData( TBOX_QUERY_PARA_CMD, 0, NULL, 0, 0 );
						#endif
						
					}

				}
				#if 0
				if(TSPACK_WorkSleep_State >= TCPACK_SLEEP_TIMEOUT_TIMES || TSPACK_WorkWakeup_State >= TCPACK_WAKEUP_TIMEOUT_TIMES)
				{
					InLoginFailTimes++;					
					disconnectSocket();/*�ر�socket����*/
					if(InLoginFailTimes > LOGIN_MAX_TIMES)
					{
						u8WaitNewIPTime = 0;
						TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REBOOT;
						MQTTLOG("jason add tsp reboot @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \r\n");
					}
					else
					{
						SetTspReDatacallFlag(1);
						TSPACK_WorkSleep_State = 0;
						TSPACK_WorkWakeup_State = 0;
						TspWorkMode = TSP_WORK_MODE_REQ_MODEM_REDATACALL;
						
						MQTTLOG("jason add tsp redatacall #############################\r\n");
					}
				}
				#endif
				break;
		
		}
		
		usleep(500 * 1000);
	}
	pthread_exit(0);
}



void MQTT::UpdataMQTTTopic(uint8_t * vin)
{
    memset(MQTT_Topic, sizeof(MQTT_Topic), '0');
    for(int i=0;i<MAXTOPIC;i++)
    {
        snprintf(MQTT_Topic[i], 20, "%d/%s",i,vin);
		MQTTLOG("@@@@@@@@@@@@@@@@@@@@\n\n\n");
        MQTTLOG("%s\n",MQTT_Topic[i]);
		MQTTLOG("@@@@@@@@@@@@@@@@@@@@\n\n\n");
    }
}

int MQTT::MQTTConnect()
{
	
	
	if(mosquitto_connect(mosq, mqtt_config.m_HOST, mqtt_config.m_PORTS, mqtt_config.m_KEEP_ALIVE))
	{
		fprintf(stderr, "Unable to connect.\n");
		m_ConnectedState = false;
		MQTTLOG("mqtt failed00000000000000000000\n\n");
			
	}
	else
	{
		m_ConnectedState = true;
		MQTTLOG("mqtt success00000000000000000000\n\n");		
	}

	
	return 0;
}


uint16_t    MQTT::MQTTtimingReportingData(uint8_t MsgType, AcpAppID_E AppID)
{
	
	static int times = 0;
	uint16_t dataLen;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(dataBuff == NULL){
		MQTTLOG("malloc dataBuff error!");
		return -1;
	}
	memset(dataBuff, 0, MALLOC_DATA_LEN);
	
	dataLen = pfawacp->PackFAWACP_FrameData(dataBuff, MALLOC_DATA_LEN, AppID, MsgType, AcpCryptFlag_IS);
	
	
	if(MsgType == 2){	
		times++;
	}

	MQTTLOG("&&&&&&&&&&&&&&\n\n");
	for(int i = 0;i<dataLen;i++)
	{
		MQTTLOG("%02x ",dataBuff[i]);
	}
	pthread_mutex_lock(&m_mutex_Rbuf);
	m_MQTTRingBuff.WriteData(dataBuff, dataLen);
	pthread_mutex_unlock(&m_mutex_Rbuf);

	free(dataBuff);
	dataBuff = NULL;

	return 0;
}


void *MQTT::SendThread(void *arg)
{
	pthread_detach(pthread_self());
	MQTT *mqtt = (MQTT*)arg;

	int ret = 0;
	int datalen = 0;
	int TotalLen = 0;
	uint8_t Temp = 0;
	uint8_t *PRead = NULL;
	uint8_t Buff[1024] = {0};

	while(1)
	{
		memset(Buff, 0, sizeof(Buff));
		if( mqtt->m_ConnectedState == true )
		{
			MQTTLOG("!!!!!!!!!!!!!!!!!!!!!!");
			#if 0
			MQTTLOG("pCFAWACP->m_RingBuff.GetUsingDataLen() == %d\n", pCFAWACP->m_RingBuff.GetUsingDataLen());
			#endif
			if(mqtt->m_MQTTRingBuff.GetUsingDataLen() > FIXATION_DATA_LEN)	//33
			{
				PRead = mqtt->m_MQTTRingBuff.Get_PReadAddr();
				//if( (PRead[0] == ACP_FIRST_SOF) && (PRead[1] == ACP_SECOND_SOF) && (PRead[2] == ACP_THIRD_SOF) && (PRead[3] == ACP_FOUR_SOF) )
				if(PRead[0] == 0x7e)
				{
					//datalen = ((PRead[27] << 8) | PRead[28]);
					datalen = mqtt->m_MQTTRingBuff.Get_DataLen( 2 );
					MQTTLOG("!!!!!!!!!!!!!!!!!!!!!!%d\n\n",datalen);
					TotalLen = (datalen + 3);
					//if(pCFAWACP->m_RingBuff.GetUsingDataLen() >= TotalLen)
					{
						ret = mqtt->m_MQTTRingBuff.ReadData(Buff, TotalLen);
						for(int i =0;i<TotalLen;i++)
						{
							MQTTLOG("%02x ",Buff[i]);
						}
						MQTTLOG("!!!!!!!!!!!!!!!!!!!!!!%d\n\n",ret);
						if(ret == TotalLen)
						{
							ret = mosquitto_publish(mqtt->mosq,NULL,mqtt->MQTT_Topic[0],TotalLen,Buff,0,1);
							MQTTLOG("mosquitto_publish ret = %d\n",ret);
							/*if(send(pCFAWACP->m_Socketfd, Buff, TotalLen, 0) < 0 )
							{
								SocketDisconnectCount++;
								MQTTLOG("Send data failed.dataLen:%d\n",datalen);
								FAWACP_ERROR_Log(2, errno,SocketDisconnectCount);
							}*/
						}
					}

				}
				else
				{
					mqtt->m_MQTTRingBuff.ReadData(&Temp, 1);
				}
			}
			else
			{
				sleep(1);
			}
		}
		else
		{
			mqtt->m_MQTTRingBuff.Init_RingBuffer();
			sleep(1);
		}
	}
	pthread_exit(0);

}


uint16_t MQTT::Unpack_MQTT_FrameData(uint8_t *pData, int32_t DataLen)
{
	uint8_t *pos = pData;
	uint8_t *PayloadBuff;
	
	AcpAppHeader_t Header;
	Unpacket_AcpHeader(Header, pos, DATA_HEADER_SIZE);

	uint8_t	applicationID = Header.Acp_AppID;
	int32_t payload_size = Header.MsgLength - DATA_HEADER_SIZE;
	uint8_t MsgType = Header.MsgType;
	MQTTLOG("MsgLength == %d",Header.MsgLength);
	MQTTLOG("Acp_AppID == %d",Header.Acp_AppID);
	MQTTLOG("MsgType == %d",Header.MsgType);
	MQTTLOG("CryptFlag == %d",Header.CryptFlag);
	//Payload ����ǰ�����ж�
	if(DataLen != Header.MsgLength){
		MQTTLOG("MsgLength error!\n");
		return -1;
	}
	
	
	switch (applicationID)
	{
		
		case ACPApp_RemoteCtrlID://Զ�̿���
			tbox_log(3,MsgType);
			UnpackData_AcpRemoteCtrl(PayloadBuff, payload_size, MsgType);
			break;
		#if 0
		case ACPApp_RemoteUpgradeID://Զ������
			if(ftpupgrade)
			{
				Upgrade_CommandResponse = 1;
				RespondTspPacket(ACPApp_RemoteUpgradeID, 2, AcpCryptFlag_IS, 0);				
				printf("jason add receive remote upgrade cmd form tsp,error because now upgading \r\n");
			}
			else
			{
				ftpupgrade = true;
				UnpackData_AcpRemoteUpgrade(PayloadBuff, payload_size, MsgType);
				printf("jason add receive remote upgrade cmd form tsp\r\n");
			}
			
			break;
		#endif
		default:
			break;
	}
	
} 

uint16_t	MQTT::Unpacket_AcpHeader(AcpAppHeader_t& Header, uint8_t *pData, int32_t DataLen)
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
	//ʱ�������
	ShowdataTime(pos);
	pos += DATA_TIMESTAMP_SIZE;
	
	uint8_t datanouse = 0x01;

	switch(MsgType)
	{
		case 1:
			
			m_AcpRemoteCtrlList.SubitemTotal = *pos++;
			MQTTLOG("Subitem total == %d",m_AcpRemoteCtrlList.SubitemTotal);
			for(uint16_t i = 0; i < m_AcpRemoteCtrlList.SubitemTotal; i++)
			{
				m_AcpRemoteCtrlList.SubitemCode[i] = *pos++;
				MQTTLOG("Subitem Code == %d",m_AcpRemoteCtrlList.SubitemCode[i]);

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
							//2019-01-14 KK modify ���޸Ļ���������MCU������������
						//	m_mcuUart->packProtocolData(TBOX_REQ_CFGINFO, 0, &TempData, 1, 0);
							usleep(1000*500);
							if(RET_OK == GPIO_GetMcuWake4GStatus(&nMcuWake4GStatus))
							{
							   if((MCU_WAKE_4G_WORKSTATUS == nMcuWake4GStatus) && (MCU_4G_SYNC_SHAKEHAND_OK == m_mcuUart->get_Uart_Sync_Value()))
							   	{
							   	  m_mcuUart->Set_Uart_Sync_Value(0);
								  break;
							   	}
							   else
							   	{
							   	}
							}
							else
							{
							}
							printf("VALUE == %d, sync == %d, times == %d\n", nMcuWake4GStatus, m_mcuUart->get_Uart_Sync_Value(), times);
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
					m_mcuUart->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
					
					tbox_log(10,0);
					#if 0
					while(m_mcuUart->Get_mcuWakeupEventValue() == STATUS_WAKEUP_EVENT)
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
						MQTTLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case VehicleBody_TrackingCarID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.VehicleBody_TrackingCar = *pos++;
						MQTTLOG("VehicleBody_TrackingCar == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case Power_SwitchID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.PowerState.PowerState_Switch = *pos++;
						MQTTLOG("Power_SwitchID == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case EngineState_SwitchID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.EngineState.EngineState_Switch = *pos++;
						MQTTLOG("EngineState_Switch == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case SleepTime_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.SleepTime = *pos++;
						MQTTLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						break;
					case SleepNow_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = m_AcpRemoteCtrlCommandData.VehicleBody.SleepNow = *pos++;
						MQTTLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						if( tboxInfo.operateionStatus.isGoToSleep == 0 )
						{
							RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
							return 0;
						}
						break;
					case ReportTime_ID:
						p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = *pos++;
						
						if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval<=9)
						{
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = 60;
						}
						MQTTLOG("ReportTimeInterval == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval);
						updateTBoxParameterInfo();

						//01-05 KK modify callback fun
					//	cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
						break;
					case CheckBleState_ID:
						m_AcpRemoteCtrlList.SubitemValue[i]  = *pos++;
						MQTTLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						if(bleupgrade==false)
						{
							m_bleUart->cb_bleRemoteCtrlCmd();
						}
						return 0;
						break;

					case BroadCase_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;
						MQTTLOG("VehicleBody_Lock == %d",m_AcpRemoteCtrlList.SubitemValue[i]);
						if(bleupgrade==false)
						{
							m_bleUart->cb_bleRemoteCtrlCmd();
						}
						return 0;
						break;

					case SendPW_ID:
						memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW, 0, 6);
						memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW, pos, 6);
						MQTTLOG("BlePW == %s",p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW);
						if(bleupgrade==false)
						{
							m_bleUart->cb_bleRemoteCtrlCmd();
						}
						return 0;
						break;
					case SetCar_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;
						m_mcuUart->packProtocolData(0x82, 0x60, m_AcpRemoteCtrlList.SubitemValue, 1, 0);
						sleep(2);

						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						sleep(3);
						disconnectSocket();
						return 0;
						break;
					case SetLowVc_ID:
						p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery = *pos++;
						
						if((p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery<80)||(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery>140))
						{
							p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery = 110;
						}
						MQTTLOG("LOW battery == %d\n",p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].LowBattery);
						updateTBoxParameterInfo();
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
						break;
					case ResetEC3_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;

						p_FAWACPInfo_Handle->VehicleCondData.CarIDState = m_AcpRemoteCtrlList.SubitemValue[i];
						m_mcuUart->packProtocolData(0x82, 0x61, m_AcpRemoteCtrlList.SubitemValue, 1, 0);
						
						updateTBoxParameterInfo();
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						return 0;
						break;
					case ResetVin_ID:
						memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin, 0, 17);
						memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin, pos, 17);
						MQTTLOG("BlePW == %s",p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin);
						for(int i = 0;i<3;i++)
						{	
							m_mcuUart->packProtocolData(0x82, 0x53, p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.ResetVin, 1, 0);
							m_mcuUart->packProtocolData(0x92, 0, &datanouse, 1, 0);
						}
						
						//01-05 KK modify callback fun
						//cb_TspRemoteCtrl();
						RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						sleep(2);
						disconnectSocket();
						InitAcpTBoxAuthData();
						return 0;
						break;
					case ResetTbox_ID:
						m_AcpRemoteCtrlList.SubitemValue[i] = *pos++;
						if(m_AcpRemoteCtrlList.SubitemValue[i]==0x00)
						{
							system("rm /data/AcpPara");
						//	system("rm /data/TBoxPara");
							dataPool->setPara(TBOX_TRCUKLOADING_ID, &temp, sizeof(temp));
						}
						else if(m_AcpRemoteCtrlList.SubitemValue[i]==0x01)
						{
							system("rm -rf /data/LteUpgrade.bin");
							system("rm -rf /data/MCU.bin");
							system("rm -rf /data/BLEAPP");
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
						m_mcuUart->close_uart();
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
						m_mcuUart->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
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
						m_mcuUart->packProtocolData( TBOX_QUERY_PARA_CMD, 0, NULL, 0, 0 );
					//	RespondTspPacket(ACPApp_RemoteCtrlID, 2, AcpCryptFlag_IS, TspctrlSource);
						break;
					
					default:
						break;
				}
			}
			tbox_log(4,MsgType);
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
			MQTTLOG("AcpRemoteControl ack is ok!\n");
			break;
		default:
			break;
	}
	return 0;
}








#if 0

uint16_t MQTT::PackMQTT_FrameData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID)
{
	uint16_t PayloadLen;
	uint8_t *pos = dataBuff;
	uint8_t *pData = dataBuff;
// Packet SOF
	*pos++ = 0x7e;
	*pos++ = 0;
	*pos++ = 0;
//Packet PAYLOAD
	uint8_t *PayloadBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(PayloadBuff == NULL){
		MQTTLOG("malloc PayloadBuff error!");
		return -1;
	}
	memset(PayloadBuff, 0, sizeof(PayloadBuff));
	//ִ���˼��ܺ������
	PayloadLen = PackMQTT_PayloadData(PayloadBuff, MALLOC_DATA_LEN, applicationID, MsgType, CryptFlag, TspSourceID);

// Payload
	memcpy(pos, PayloadBuff, PayloadLen);
	pos += PayloadLen;
	MQTTLOG("***********%d",PayloadLen);
	pData[1] = ((PayloadLen >> 8) & 0xff);
	pData[2] = (PayloadLen & 0xfF);


	if(NULL != PayloadBuff){
		free(PayloadBuff);
		PayloadBuff = NULL;
	}

	return (uint16_t)(pos-dataBuff);
}

uint32_t MQTT::PackMQTT_PayloadData(uint8_t *dataBuff, uint16_t bufSize, AcpAppID_E applicationID, uint8_t MsgType, AcpCryptFlag_E CryptFlag, uint8_t TspSourceID)
{
	uint32_t dataLen = 0;
	uint8_t *pos = dataBuff;
	//Element
	uint8_t *ElementBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(ElementBuff == NULL)
	{
		MQTTLOG("malloc ElementBuff error!");
		return -1;
	}
	memset(ElementBuff, 0, sizeof(ElementBuff));

MQTTLOG("PackFAWACP_PayloadData:AppID = %d, MsgID = %d, CryptFlag = %d \n",applicationID, MsgType, CryptFlag );
	
//PayloadBuff ִ
	switch (applicationID)
	{
		case ACPApp_AuthenID:
			break;
		case ACPApp_HeartBeatID:
			//
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE+DATA_AUTHTOKEN_SIZE;
			break;
		case ACPApp_RemoteCtrlID:
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, m_mcuUart->RCtrlErrorCode);//��������ʱΪ0
			//m_mcuUart->RCtrlErrorCode=0;
			m_mcuUart->RCtrlErrorCode=0;
			dataLen = RemoteCtrlCommandDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
			#if 0
			TimeOutType.RemoteCtrlTspSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_RemoteCtrlID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
			#endif
			break;
		case ACPApp_QueryVehicleCondID:
			
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, 0);
			//ȫ���ź�����
			dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE, 0);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
			#if 0
			TimeOutType.VehQueCondSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_QueryVehicleCondID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
			#endif
			break;
		case ACPApp_RemoteConfigID:
			
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
		case ACPApp_RemoteUpgradeID://Զ������
			//ʱ���
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, Upgrade_CommandResponse);
			switch(MsgType)
			{
				case 2:
					dataLen = RemoteUpgrade_ResponseDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
					break;
				case 3:
					dataLen = RemoteUpgrade_DownloadStartDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
					break;
				case 4:
					dataLen = RemoteUpgrade_DownloadResultDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
					break;
				case 5:
					dataLen = RemoteUpgrade_UpdateStartDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
					break;
				case 6:
					dataLen = RemoteUpgrade_UpdateResultDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
					break;
				default:
					break;
			}
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
			break;
		case ACPApp_EmergencyDataID://������Ԯ�����ϱ�
			//ʱ���
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			//ȫ���ź�����
			dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, 0);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE;
			break;
		case ACPApp_VehicleCondUploadID://���������ϱ� 1)�¼������ϱ� 2)��ͨ����10S��ʱ�ϱ� 3) λ�úͳ��ٸ�Ƶ5s�ϱ�
			//ʱ���
			TimeDeal(ElementBuff);
			if(1 == MsgType){
				dataLen = ReportGPSSpeedDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE);
			}//��Ƶ�ϱ������г�������
			else if(2 == MsgType){
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE, 0);
			}//�ϱ�������Ϣ�͹����¼�(��������ʷ��Ϣ)
			else if(18 == MsgType){
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, 1);
			}//�ϱ�������Ϣ�ͼ�ʻ��Ϊ���������¼�(��������ʷ��Ϣ)
			else if(20 == MsgType){
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, 2);
			}//�ϱ�������Ϣ��������ʷ����(��������ʷ��Ϣ)
			else{
				dataLen = ReportVehicleCondDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, 0);
			}
			//dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE;
			dataLen += DATA_TIMESTAMP_SIZE;
			break;
		case ACPApp_GPSID: 	//������λ
			//ʱ���
			TimeDeal(ElementBuff);
			AuthTokenDeal(ElementBuff + DATA_TIMESTAMP_SIZE);
			SourceDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE, TspSourceID);
			ErrorCodeDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE, 0);
			//GPSλ�������ϱ�
			dataLen = VehicleGPSDeal(ElementBuff + DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE);
			dataLen += DATA_TIMESTAMP_SIZE + DATA_AUTHTOKEN_SIZE + DATA_SOURCE_SIZE + DATA_ERRORCODE_SIZE;
			#if 0
			TimeOutType.VehGPSSource = TspSourceID;
			pthread_mutex_lock(&mutex);
			insertlink( ACPApp_GPSID, MsgType, TspSourceID);
			pthread_mutex_unlock(&mutex);
			#endif
			break;
		default:
			break;
	}
	
	AcpAppHeader_t Header;
	Packet_AcpHeader(Header, applicationID, MsgType, CryptFlag, DATA_HEADER_SIZE+dataLen);
	AddData_AcpHeader(pos, Header);
	pos += DATA_HEADER_SIZE;
	
	memcpy(pos, ElementBuff, dataLen);
	pos += dataLen;

	if(NULL != ElementBuff){
	free(ElementBuff);
	ElementBuff = NULL;
	}
	return (uint16_t)(pos-dataBuff);
}


uint16_t CFAWACP::ReportVehicleCondDeal(uint8_t* dataBuff, uint8_t flag)
{
	uint16_t tempLen = 0;
	static uint8_t CollectPacketSN = 1;
	uint16_t signCodeTotal = 0;
	uint8_t *pos = dataBuff;
	uint8_t *pData = dataBuff;

	
	*pos++ = m_SendMode;//AcpVehicleCondition.SendMode;
	if(CollectPacketSN == 254)
		CollectPacketSN = 1;
	
	*pos++ = CollectPacketSN++;
	
	TimeDeal(pos, 0);
	pos += DATA_TIMESTAMP_SIZE - 1;
	//�ɼ��ź�����
	if(flag == 0)
		signCodeTotal = MAX_SIGNCODE_TOTAL;
	else if(flag == 1)
		signCodeTotal = MAX_SIGNCODE_TOTAL - 2 + MAX_FAULT_SIGNA + MAX_DRIVEACTION_SIGNA;//�ر�ע��
	else if(flag == 2)
		signCodeTotal = MAX_SIGNCODE_TOTAL - 2 + MAX_DRIVEACTION_SIGNA;
	else
		signCodeTotal = MAX_SIGNCODE_TOTAL - 2;

	*pos++	= (signCodeTotal >> 8) & 0xFF;
	*pos++	= signCodeTotal & 0xFF;
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
	//��������ź�
	if(flag == 1)
	{
		pos += FaultSignDeal(pos);
	}
				
	if(flag == 2)
	{
		pos += addFaultSignDeal(pos);
	}
	
	tempLen = uint16_t(pos - dataBuff) - 2;//���ݳ��Ȳ�������������ֵ����
	

	MQTTLOG("data Response tempLen==%d\n",tempLen);
	FAWACP_NO("pData[0]: %02x pData[1]: %02x", pData[0], pData[1]);
	FAWACP_NO("\n\n");

	return uint16_t(pos - dataBuff);
}
#endif
#endif