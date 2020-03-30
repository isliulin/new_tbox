/*
tsp.cpp:
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
#include "QueueCommon.h"
#include "tsp.h"
#include <errno.h>
#include "TCTL.h"

static CTsp* CTsp::m_instance = NULL;

CTsp::CTsp()
{
	m_binit = false;
	session = true;
	ID_Queue_TSP_To_FW = (-1);
	ID_Queue_FW_To_TSP = (-1);
	m_MqttDisconnectReqFlag = FALSE;
	m_TimeSyncHeartReceiveFlag = 0;
	m_LocalTimeSyncState = 0;
	m_BokenReqFlag = 0;
}

CTsp::~CTsp()
{

}

static CTsp* CTsp::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new CTsp;
	}
	return m_instance;
}

 static void CTsp::FreeInstance()
{

    if(m_instance != NULL)
    {
        delete m_instance;
    }
	
    m_instance = NULL;

}
 
BOOL CTsp::Init()
{
	m_binit = CreateThread(); /*param is default time*/
	
	return m_binit;
}

void CTsp::Deinit()
{
    KillThread();
}

void CTsp::Run()
{

    InformThread();

}
BOOL CTsp::Processing()
{
    while(!m_bExit)
    {

#if QUEUE_TEST_MACRO
	ReceiveQueueTestTask();
	SendQueueTestTask();
	 Sleep(200*1000);
#else
        TspWorkTask();
	 ReceiveTspQueueTask();
	 //SendTspQueueTask();
	 printf("tsp processing \r\n");
        Sleep(250000); /*sleep 500ms */
#endif
    }

}

void CTsp::ReceiveQueueTestTask()
{
	int len = 0;
	QueueInfo_ST  QueueTest;

	   if(Framework::GetInstance()->ID_Queue_FW_To_TSP  > 0)
	   {
			len =msgrcv(Framework::GetInstance()->ID_Queue_FW_To_TSP,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);
			if(len == -1)
			{
				printf("jason add TSP receive from  fw no data \r\n");
				printf("jason add TSP receive from  fw no data \r\n");
				printf("jason add TSP receive from  fw no data \r\n");
			}
			else
			{
				printf("jason add TSP receive from  fw len = %d \r\n",len);
				printf("jason add TSP receive from  fw QueueTest.Msgs[0] = %x \r\n",QueueTest.Msgs[0]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[1] = %x \r\n",QueueTest.Msgs[1]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[2] = %x \r\n",QueueTest.Msgs[2]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[3] = %x \r\n",QueueTest.Msgs[3]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[4] = %x \r\n",QueueTest.Msgs[4]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[500] = %x \r\n",QueueTest.Msgs[500]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[501] = %x \r\n",QueueTest.Msgs[501]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[502] = %x \r\n",QueueTest.Msgs[502]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[503]);
				printf("jason add TSP receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[504]);
			}
	   }

}

 void CTsp::SendQueueTestTask()
{
	QueueInfo_ST  QueueTest ;
	int len = 0;
	static int count = 0;
	QueueTest.mtype = 1;

	   if( (Framework::GetInstance()->ID_Queue_TSP_To_FW  > 0) && (count++ %5 ==0))
	   {
	   	QueueTest.Msgs[0] = 0x33;
		QueueTest.Msgs[1] = 0x44;
	   	QueueTest.Msgs[2] = 0x55;
	   	QueueTest.Msgs[3] = 0x66;
	   	QueueTest.Msgs[4] = 0x77;
		
	   	QueueTest.Msgs[500] = 0x77;
		QueueTest.Msgs[501] = 0x66;
	   	QueueTest.Msgs[502] = 0x55;
	   	QueueTest.Msgs[503] = 0x44;
	   	QueueTest.Msgs[504] = 0x33;

		 len = msgsnd(Framework::GetInstance()->ID_Queue_TSP_To_FW,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),IPC_NOWAIT);
		if(len == -1)
		{
			printf("jason add tsp send to Framework error \r\n");

		}
		else
		{
			printf("jason add tsp send to mcu Framework success\r\n");

		}
	   }

 }
BOOL CTsp::TimeoutProcessing()
{
	printf("jason add tsp timeout processing \r\n");
	return true;
}

int CTsp::Apn_init()
{
	int ret;
	int apn_index = 4;
	char apn[30] = {0};
	char username[64] = {0};
	char password[64] = {0};
	int pdp_type;
	char * apntest = (char*)"cmnet";

	int retval;
	uint8_t i;
	char strResult[128];
		
	for (i = 0; i < 3; i++)
	{
		memset(strResult, 0, sizeof(strResult));
		retval = sendATCmd((char*)"AT+COPS?", (char*)"OK", strResult, sizeof(strResult), 2000);
		if (retval > 0)
		{
			if(strstr(strResult, "UNICOM"))
			{
				apntest = (char*)"3gnet";
			}
			else if(strstr(strResult, "CMCC"))
			{
				apntest = (char*)"cmnet";
			}
			else if(strstr(strResult, "CHN-CT"))
			{
				apntest = (char*)"ctnet";
			}
			else
			{
				apntest = (char*)"cmnet";
			}
			break;
		}
		else
		{
			apntest = (char*)"cmnet";
		}
	}

	//apn 
	if(wds_init() == 0)//new sdk interface zhujw
	   DEBUGLOG("wds_qmi_init success!\n");

	//profile_index: 4-->Private network; 6-->Public Network
	ret = get_apnInfo(apn_index, &pdp_type, apn, username, password);
	if(ret == FALSE)
	{
		printf("wds_GetAPNInfo Fail\n");
	}
	else
	{
		printf(">>>>>> apn[%d]=%s, pdp_type = %d, username=%s, password=%s\n", apn_index, apn, pdp_type, username, password);
	}
	
	if(strcmp(apn, APN1) != 0)
	{
		set_apnInfo(4, 0, APN1, NULL, NULL);//new sdk interface zhujw
	}
	
	apn_index = 6;
	memset(apn, 0, sizeof(apn));
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	ret = get_apnInfo(apn_index, &pdp_type, apn, username, password); //new sdk interface
	if(ret == FALSE)
	{
		printf("wds_GetAPNInfo Fail\n");
	}
	else
	{
		printf(">>>>>> apn[%d]=%s, pdp_type = %d, username=%s, password=%s\n", apn_index, apn, pdp_type, username, password);
	}
	if(strcmp(apn, APN1) != 0)
	{
		set_apnInfo(6, 0, APN1, NULL, NULL);
	}

	return 0;
}

int CTsp::NetworkInit()
{
	int ret = 0;
	char buff[100];
	char imei[100];
	char cimi[100];
	char iccid[100];
	

	// get IMEI
	memset(imei, 0, sizeof(imei));
	dataPool->getPara(IMEI_INFO, (void *)imei, sizeof(imei));
	printf("imei:%s\n",imei);

	memset(buff, 0, sizeof(buff));
	if(getIMEI(buff, sizeof(buff)) == 0)
	{
		printf("IMEI:%s\n",buff);

		if(memcmp(imei, buff,(strlen(buff))) != 0)
		{
			dataPool->setPara(IMEI_INFO, (void *)buff, strlen(buff));
		}
	}

	memset(cimi, 0, sizeof(cimi));
	dataPool->getPara(CIMI_INFO, (void *)cimi, sizeof(cimi));
	printf("cimi:%s\n",cimi);

	memset(buff, 0, sizeof(buff));
	if(getCIMI(buff, sizeof(buff)) == 0)
	{
		printf("CIMI:%s\n",buff);

		if(memcmp(cimi, buff,(strlen(buff))) != 0)
		{
			dataPool->setPara(CIMI_INFO, (void *)buff, strlen(buff));
		}
	}

	if(getCPIN() > 0)
	{
		tboxInfo.networkStatus.isSIMCardPulledOut = 1;
		//enableMobileNetwork(1*2, networkConnectionStatus);

		memset(iccid, 0, sizeof(iccid));
		dataPool->getPara(SIM_ICCID_INFO, (void *)iccid, sizeof(iccid));

		// get ICCID
		memset(buff, 0, sizeof(buff));
		if(getICCID(buff, sizeof(buff)) == 0)
		{
			printf("ICCID:%s\n",buff);
			if(memcmp(iccid, buff,(strlen(buff))) != 0)
			{
				dataPool->setPara(SIM_ICCID_INFO, (void *)buff, strlen(buff));
			}
		}
	}
	else
	{
		printf("no sim card\n");
		tboxInfo.networkStatus.isSIMCardPulledOut = 0;
		ret = -1;
	}

	return ret;
}

void CTsp::ReceiveTspQueueTask()
{
    int ret_value = 0;
    int msg_len = 0;
    uint8_t  u8ReportInterval =0;
	QueueInfo_ST m_stTspPublishData;
	time_t tmNow;
	time(&tmNow);
	if(ID_Queue_FW_To_TSP  != -1)
	{
	    msg_len = msgrcv(ID_Queue_FW_To_TSP,&m_stTspPublishData,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);

	    if(msg_len  == (-1))
	    {
		printf("jason add for receive error \r\n");
		printf("jason add for receive error \r\n");
	    }
	    else if (m_stTspPublishData.head.Gr == GR_THD_SOURCE_FW)
	    {
	    	     if(mosq == NULL)
	    	     {
			  return ;
		     }
			switch(m_stTspPublishData.head.Id)
			{
				case ID_FW_2_TSP_PTC_VEICHLE_INFO_REPORT:
					if(tmNow < m_timeLastReport)
					{
						time(&tmNow);
						time(&m_timeLastReport);
					}
					else
					{
						if(tboxInfo.operateionStatus.isGoToSleep == 0)
						{
							u8ReportInterval = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval;
						}
						else
						{
							u8ReportInterval = p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval;
						}

 						if((tmNow - m_timeLastReport) > u8ReportInterval)
 						{
							time(&m_timeLastReport);
							ret_value = mosquitto_publish(mosq,NULL,MQTT_Topic[VehicleUploadTP],msg_len,m_stTspPublishData.Msgs,1,0);
						}
					}

					#if 0
					printf("tsp rev chen uqeue Data:");
					for(int i=0;i<msg_len-2;i++)
					{
						printf("%02x ",m_stTspPublishData.Msgs[i]);
					}
					printf("\r\n");

					printf("m_stTspPublishData.Msgs msg len =%d\r\n",msg_len);
					#endif
					break;
				case ID_FW_2_TSP_PTC_VEICHILE_INFO_EVENT:
					time(&m_timeSendTimeSync);
					ret_value = mosquitto_publish(mosq,NULL,MQTT_Topic[VehicleUploadTP],msg_len,m_stTspPublishData.Msgs,1,0);
					break;
				case ID_FW_2_TSP_PTC_REMOTE_CTL_ACK:
					ret_value = mosquitto_publish(mosq,NULL,MQTT_Topic[RemoteCtrlReplyID],msg_len,m_stTspPublishData.Msgs,0,0);
					FAWACPLOG("topic is %s\n",MQTT_Topic[RemoteCtrlReplyID]);
					FAWACPLOG("mosquitto_publish ret = %d\n",ret_value);
					FAWACPLOG("mqtt_config.m_TestOrNormal = %d\r\n",m_stMqttConfig.m_TestOrNormal);
					break;
				case ID_FW_2_TSP_PTC_REMOTE_UPGRADE:
					ret_value = mosquitto_publish(mosq,NULL,MQTT_Topic[RemoteUpgradeReplyID],msg_len,m_stTspPublishData.Msgs,0,0);
					FAWACPLOG("topic is %s\n",MQTT_Topic[RemoteUpgradeReplyID]);
					FAWACPLOG("mosquitto_publish ret = %d\n",ret_value);
					break;
				case ID_FW_2_TSP_PTC_SYNC_TIME_REQ:
					ret_value = mosquitto_publish(mosq,NULL,MQTT_Topic[SyncTimeRequest],msg_len,m_stTspPublishData.Msgs,0,0);
					FAWACPLOG("topic is %s\n",MQTT_Topic[SyncTimeRequest]);
					FAWACPLOG("mosquitto_publish ret = %d\n",ret_value);
					break;
				case ID_FW_2_TSP_PTC_TOKEN_REQ:
					m_BokenReqFlag = 1;
					ret_value = mosquitto_publish(mosq,NULL,MQTT_Topic[ReqBleToken],msg_len,m_stTspPublishData.Msgs,0,0);
					FAWACPLOG("topic is %s\n",MQTT_Topic[ReqBleToken]);
					FAWACPLOG("mosquitto_publish ret = %d\n",ret_value);
					break;
				case ID_FW_2_TSP_PTC_TOKEN_ACK:
					ret_value = mosquitto_publish(mosq,NULL,MQTT_Topic[ReqBleToken],msg_len,m_stTspPublishData.Msgs,0,0);
					FAWACPLOG("topic is %s\n",MQTT_Topic[ReqBleToken]);
					FAWACPLOG("mosquitto_publish ret = %d\n",ret_value);
					break;
				default:
					break;
				}

	    }
	    else
	    {
			printf("jason add for group id error m_stTspPublishData.head.Gr = %d \r\n",m_stTspPublishData.head.Gr);
			printf("jason add for group id error m_stTspPublishData.head.Gr = %d \r\n",m_stTspPublishData.head.Gr);
	    }
	}
	else
	{
		//ID_Queue_FW_To_TSP  = msgget(KEY_QUEUE_FW_TO_TSP ,IPC_CREAT|0777);
		ID_Queue_FW_To_TSP = Framework::GetInstance()->ID_Queue_FW_To_TSP;

	}
	
	if(ID_Queue_TSP_To_FW == -1)
	{
		ID_Queue_TSP_To_FW = Framework::GetInstance()->ID_Queue_TSP_To_FW;

	}
	else
	{

	}
}
void CTsp::SendTspQueueTask()
{
    QueueInfo_ST m_stTspPublishDataSnd;
    static uint8_t count = 0; ;
    int ret_value;
    int msg_len;
    time_t m_timeNow;
    time(&m_timeNow);
    if(m_timeNow < m_timeSendTimeSync)
    {
    	time(&m_timeSendTimeSync);
    }
    else if(m_timeNow - m_timeSendTimeSync > TSP_SYNC_TIME_PERIOD)
    {
    
    }
	if(ID_Queue_TSP_To_FW  != -1)
	{
		m_stTspPublishDataSnd.mtype 	= 1;
		m_stTspPublishDataSnd.head.Gr 	= (uint8_t)GR_THD_SOURCE_TSP;
		m_stTspPublishDataSnd.head.Id 	= (uint8_t)ID_TSP_2_MCU_PTC_REMOTE_CTL;
		memset(m_stTspPublishDataSnd.Msgs,0,sizeof(m_stTspPublishDataSnd.Msgs));
		//for(int i =0;i<56;i++)
		
		if((count ++)%5 == 0)
		{
			//ret_value = VIF_Parse((uint8_t *)m_stTspPublishDataSnd.Msgs,64,&m_instance->g_stMcuVif);
	            if(ret_value == RET_FAIL)
	            {
	                printf("[** tsp.cpp->SendTspQueueTask->VCTL_Parse**]%s(%d): ret_value RET_FAIL!!!\n", __func__, __LINE__);
	            }
	            else if(ret_value == RET_INVALID)
	            {
	                printf("[** tsp.cpp->SendTspQueueTask->VCTL_Parse**]%s(%d): ret_value RET_INVALID!!!\n", __func__, __LINE__);
	            }
	            else
	            {
	            
	            }
		
		    msg_len = msgsnd(Framework::GetInstance()->ID_Queue_TSP_To_FW,&m_stTspPublishDataSnd,256,IPC_NOWAIT);
		    if(msg_len  == (-1))
		    {
				printf("jason add for send queue error \r\n");
				printf("jason add for send queue error \r\n");
		    }
		    else
		    {
				printf("jason send for group id success m_stTspPublishData.head.Gr = %d \r\n",m_stTspPublishDataSnd.head.Gr);
				printf("jason send for group id success m_stTspPublishData.head.Gr = %d \r\n",m_stTspPublishDataSnd.head.Gr);
				printf("jason send for group id success m_stTspPublishData.head.Gr = %d \r\n",m_stTspPublishDataSnd.head.Gr);
				printf("jason send for group id success m_stTspPublishData.head.Gr = %d \r\n",m_stTspPublishDataSnd.head.Gr);
		    }
		}
	}
	else
	{
		//ID_Queue_FW_To_TSP  = msgget(KEY_QUEUE_FW_TO_TSP ,IPC_CREAT|0777);
		ID_Queue_TSP_To_FW = Framework::GetInstance()->ID_Queue_TSP_To_FW;
	}
}

void CTsp::TspWorkTask()
{
	int ret = 0;
	static uint8_t u8ModemWaitTime = 0;
	static uint8_t u8ModemDataCallCount= 0;
	static nas_serving_system_type_v01 nas_status = {0};
	static int NasCsstate = 0;
	static int NasPsstate = 0;
	int loop;
	static datacall_info_type datacall_info;
	static uint8_t u8AcpReportTime  = 0;
	static uint8_t u8WaitNewIPTime = 0;
	static uint8_t  ConnectFaultTimes = 0;

	static uint8_t u8CountCheckNetState = 0;
	
	static uint8_t SocketReconnectCount = 0;

	if(m_MqttDisconnectReqFlag)
	{
		m_u8TspWorkMode = TSP_WORK_MODE_MQTT_STATE_DISCONNECT;
	}
	switch(m_u8TspWorkMode)
	{
		 case TSP_WORK_MODE_MODEM_STATE_INIT:
		 		if (atctrl_init() < 0)
		 		{
		 		
					 printf("jason add for Atctrl_init failed!!!\r\n");
				}

				if(NetworkInit() < 0)
				{
					u8ModemWaitTime ++;
					if(u8ModemWaitTime >= TSP_WORK_MODE_MODEM_WAIT_INIT_TIME_OUT) 
					{
						u8ModemWaitTime = 0;
						m_u8TspWorkMode = TSP_WORK_MODE_MODEM_STATE_CHECK_PS_CS;
						printf("NetworkInit time out ,enter ps cs state \r\n");
					}
					else
					{
						break;
					}
				}
				if(Apn_init() == 0)
				{
				   printf("apn_init success!");
				}
				if(datacall_init() == 0)
				{
					printf("dataCall_init success!");
				}
				if(voice_init() == 0)
				{
				   printf("voiceCall_init success!");
				}
				if(sms_init((sms_ind_cb_fcn)process_simcom_ind_message) == 0)
				{
				   printf("message_init success!");
				}
				if(nas_init() == 0)
				{
					printf("nas_init success!");
				}
				u8ModemWaitTime = 0;
				m_u8TspWorkMode = TSP_WORK_MODE_MODEM_STATE_CHECK_PS_CS;					
				
		 case TSP_WORK_MODE_MODEM_STATE_CHECK_PS_CS:
				get_NetworkType(&nas_status);//new sdk interface
				if(nas_status.registration_state == NAS_REGISTERED_V01)
				{
					if(m_stNetStatus.networkRegSts != 1)
					{
						m_stNetStatus.networkRegSts  = 1;
					}
					if(tboxInfo.networkStatus.networkRegSts != 1)
					{
						tboxInfo.networkStatus.networkRegSts = 1;
					}
				}
				else
				{
					if(tboxInfo.networkStatus.networkRegSts == 1)
					{
						tboxInfo.networkStatus.networkRegSts = 0;
					}
					if(m_stNetStatus.networkRegSts != 0)
					{
						m_stNetStatus.networkRegSts  = 0;
					}
				}
				if(nas_status.cs_attach_state == NAS_CS_ATTACHED_V01)
				{
					if(NasCsstate != NAS_CS_ATTACHED_V01)
					{
						NasCsstate = NAS_CS_ATTACHED_V01;
					}
				}
				else
				{
					if(NasCsstate != nas_status.cs_attach_state)
					{
						NasCsstate = nas_status.cs_attach_state;
					}
				}
				if(nas_status.ps_attach_state == NAS_PS_ATTACHED_V01)
				{
					if(NasPsstate != NAS_PS_ATTACHED_V01)
					{
						NasPsstate = NAS_PS_ATTACHED_V01;
					}
				}
				else
				{

					if(NasPsstate != nas_status.ps_attach_state)
					{
						NasPsstate = nas_status.ps_attach_state;
					}

				}
				if((NasPsstate == NAS_PS_ATTACHED_V01) && (NasCsstate == NAS_CS_ATTACHED_V01) \
					&&(nas_status.registration_state == NAS_REGISTERED_V01))
				{
					u8ModemWaitTime  = 0;
					u8ModemDataCallCount = 0;
					m_u8TspWorkMode = TSP_WORK_MODE_MODEM_STATE_DATA_CALL;
				}
				else
				{
					u8ModemWaitTime++;
					if(u8ModemWaitTime >= TSP_WORK_MODE_MODEM_WAIT_PSCS_TIME_OUT)
					{
						u8ModemWaitTime = 0;
						m_u8TspWorkMode = TSP_WORK_MODE_MODEM_STATE_DATA_CALL;
						printf("jason add for modem pscs timeout failed ,enter data call state!! \r\n");
					}
					else
					{
						break;
					}
				}

		 case TSP_WORK_MODE_MODEM_STATE_DATA_CALL:
				////memset(target_ip,0,16);
				start_dataCall(app_tech_auto, DSI_IP_VERSION_4, 4,APN1,NULL,NULL);//new sdk interface		
				m_u8TspWorkMode = TSP_WORK_MODE_MODEM_STATE_DATA_CALL_CHECK;
				u8ModemWaitTime = 0;
		 	//break;
		 case TSP_WORK_MODE_MODEM_STATE_DATA_CALL_CHECK:

			get_datacall_info_by_profile(4,&datacall_info);//new sdk interface
			if(datacall_info.status == DATACALL_DISCONNECTED)
			{
				m_stNetStatus.isLteNetworkAvailable = NETWORK_NULL;
				
				//tboxInfo.networkStatus.isLteNetworkAvailable = NETWORK_NULL;
				if(u8ModemWaitTime++ >= TSP_WORK_MODE_MODEM_WAIT_DC_TIME_OUT) 
				{
					u8ModemWaitTime = 0;						
					u8ModemDataCallCount++;
					if(u8ModemDataCallCount >= TSP_WORK_MODE_MODEM_DATACALL_MAX_COUNT)
					{
					    	u8ModemDataCallCount = 0;
						m_u8TspWorkMode = TSP_WORK_MODE_REQ_REBOOT;
					  	printf("jason add datacall timeout failed, enter reboot system !!!!!! \r\n");
					}
					else
					{
						m_u8TspWorkMode = TSP_WORK_MODE_MODEM_STATE_DATA_CALL;
						
					}
				}					
			}
			else
			{
				u8ModemDataCallCount = 0;
				tboxInfo.networkStatus.isLteNetworkAvailable = NETWORK_LTE;
				m_stNetStatus.isLteNetworkAvailable = NETWORK_LTE;
				m_u8TspWorkMode = TSP_WORK_MODE_MQTT_STATE_DOWN_LOAD_PARAM;		
			}
			break;
		case TSP_WORK_MODE_MQTT_STATE_DOWN_LOAD_PARAM:
			 if(ReadMqttParamInfo() == 0)
			 {
				DownLoadMqttAuthPara();
				break;
			 }
			 else
			 {
				m_u8TspWorkMode = TSP_WORK_MODE_MQTT_STATE_INIT;
				printf("jason add for %s %d %d %s %s %s %s %d %d %d %s",m_stMqttConfig.m_HOST,m_stMqttConfig.m_PORTS,m_stMqttConfig.m_uReportFrequency,m_stMqttConfig.m_PSK_ID,m_stMqttConfig.m_Car_Type,m_stMqttConfig.m_strPSK,m_stMqttConfig.m_hexPSK,m_stMqttConfig.m_Alarm,m_stMqttConfig.m_reportFrequency,m_stMqttConfig.m_LogSwitch,m_stMqttConfig.m_PASSWARD);
			 }
			
		case TSP_WORK_MODE_MQTT_STATE_INIT:

				InitAcpTBoxAuthData();
				if(ReadTboxParamInfo() == -1)
				{
					InitTboxParamInfo();
				}

				if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval <=9)
				{
					p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[0].ReportTimeInterval = 10;	
					UpdateTboxParamInfo();
					
				}
				if(p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval<=9)
				{
					p_FAWACPInfo_Handle->RemoteDeviceConfigInfo[1].ReportTimeInterval = 60;
					UpdateTboxParamInfo();
					
				}
				dataPool->getTboxConfigInfo(VinID, p_FAWACPInfo_Handle->Vin, 17);
				
				UpdataMQTTTopic(p_FAWACPInfo_Handle->Vin);
				
				memcpy(m_stMqttConfig.m_USERNAME,p_FAWACPInfo_Handle->Vin,17);
				
				m_stMqttConfig.m_USERNAME[17] = '\0';

				mosquitto_lib_init();
				mosq = mosquitto_new(m_stMqttConfig.m_USERNAME,session,NULL);/*jason change for sn client id */
				if(!mosq)
				{
					FAWACPLOG("create client failed..\n");
					mosquitto_lib_cleanup();
					break;
				}
				mosquitto_connect_callback_set(mosq, mqtt_connect_callback);
				mosquitto_message_callback_set(mosq, mqtt_message_callback);
				mosquitto_disconnect_callback_set(mosq, mqtt_disconnect_callback);

				ret = mosquitto_username_pw_set(mosq,m_stMqttConfig.m_USERNAME,m_stMqttConfig.m_PASSWARD);
				printf("mosquitto_username_pw_set ret =%d\n",ret);
				
				ret = mosquitto_tls_psk_set(mosq, m_stMqttConfig.m_hexPSK, m_stMqttConfig.m_PSK_ID, NULL);
				printf("mosquitto_tls_psk_set ret = %d\n",ret);
				
				m_u8TspWorkMode = TSP_WORK_MODE_MQTT_STATE_CONNECT;
				break;
		case TSP_WORK_MODE_MQTT_STATE_CONNECT :		
				MQTTConnect();
				if(m_ConnectedState == false)
				{
					if(ConnectFaultTimes > 3)
					{
						ConnectFaultTimes = 0;
						m_u8TspWorkMode = TSP_WORK_MODE_MQTT_STATE_DISCONNECT;

						printf("jason add for retry data call \r\n");
					}
					else
					{
						ConnectFaultTimes++;
						SocketReconnectCount ++;
						if(SocketReconnectCount >= 30)
						{
							SocketReconnectCount = 0;
							m_u8TspWorkMode = TSP_WORK_MODE_REQ_REBOOT;
							break;
						}
					
					}

				}
				else
				{
					ConnectFaultTimes = 0;
					SocketReconnectCount = 0;
					loop = mosquitto_loop_start(mosq);
					//loop = mosquitto_loop_forever(mosq, -1, 1);
					if(loop != MOSQ_ERR_SUCCESS)
					{
						printf("mosquitto loop error\r\n");
						m_u8TspWorkMode = TSP_WORK_MODE_REQ_REBOOT;
						break;
					}
					m_u8TspWorkMode = TSP_WORK_MODE_MQTT_STATE_IDLE;
					break;
				}
				break;
		case TSP_WORK_MODE_MQTT_STATE_IDLE:
			if(u8CountCheckNetState > TSP_WORK_MODE_CHECK_TIME)
			{
				u8CountCheckNetState = 0;
				get_datacall_info_by_profile(4,&datacall_info);
				printf("jason add for datacall_info.status  =%d \r\n",datacall_info.status);
			}
			else
			{
				u8CountCheckNetState++;
			}
			if(datacall_info.status == DATACALL_DISCONNECTED)
			{
				m_stNetStatus.isLteNetworkAvailable = NETWORK_NULL;
				m_u8TspWorkMode = TSP_WORK_MODE_MQTT_STATE_DISCONNECT;
				
			}
			break;
		case TSP_WORK_MODE_MQTT_STATE_DISCONNECT:
			
			if(mosq != NULL)
			{
				mosquitto_disconnect(mosq);
				mosquitto_loop_stop(mosq,1);
				mosquitto_destroy(mosq);
			}
			mosquitto_lib_cleanup();
			
			m_u8TspWorkMode = TSP_WORK_MODE_MODEM_STATE_CHECK_PS_CS;
			
			break;
		 case TSP_WORK_MODE_REQ_REBOOT:
		 	  // system("sys_reboot");
		 	  printf("jason add for req reboot system \r\n");
		 	break;
		 default:
		 	break;
	}

}
int CTsp::InitMqttParamInfo()
{
	int fd;
	FAWACPLOG("Init MQTT para!!!!!!!!!!!!!!!!!!");
	fd = open(MQTT_PARAMETER_INFO_PATH, O_RDWR | O_CREAT|O_TRUNC, 0666);
	if (-1 == fd)
	{
		FAWACPLOG("MQTT init open file failed\n");
		return -1;
	}


	if (-1 == write(fd, &m_stMqttConfig, sizeof(stMqttConfig)))
	{
		FAWACPLOG("write file error!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int CTsp::UpdateMqttParamInfo()
{
	int fd;

	fd = open(MQTT_PARAMETER_INFO_PATH, O_RDWR, 0666);
	if(-1 == fd)
	{
		FAWACPLOG("MQTT update open file failed\n");
		return -1;
	}

	if(-1 == write(fd, &m_stMqttConfig, sizeof(stMqttConfig)))
	{
		FAWACPLOG("write file error!\n");
		return -1;
	}

	close(fd);

	return 0;
}
BOOL CTsp::ReadMqttParamInfo()
{
	int fd;
	int retval;

	fd = open(MQTT_PARAMETER_INFO_PATH, O_RDONLY, 0666);
	if(fd < 0)
	{
		FAWACPLOG("File does not exist!\n");
		return 0;
	}
	else
	{
		FAWACPLOG("File Open success\n");
		retval = read(fd, &m_stMqttConfig, sizeof(stMqttConfig));
		if(retval > 0)
		{
			FAWACPLOG("retval:%d\n",retval);
		}

		close(fd);

	}

	return 1;
}
void CTsp::InitAcpTBoxAuthData()
{

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

}
int CTsp::InitTboxParamInfo()
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
int CTsp::ReadTboxParamInfo()
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

int CTsp::UpdateTboxParamInfo()
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

void CTsp::Init_FAWACP_dataInfo()
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

	
	m_tspTimestamp.Year = p_tm->tm_year-90;
	m_tspTimestamp.Month = p_tm->tm_mon+1;
	m_tspTimestamp.Day = p_tm->tm_mday;
	m_tspTimestamp.Hour = p_tm->tm_hour;
	m_tspTimestamp.Minutes = p_tm->tm_min;
	m_tspTimestamp.Seconds = p_tm->tm_sec;
	m_tspTimestamp.msSeconds = Time.tv_usec / 1000; 

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
void CTsp::UpdataMQTTTopic(uint8_t * vin)
{
	if(vin != NULL)
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
}

void CTsp::mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	char log_str[128] = {0};
	sprintf(log_str, "%s%d", "mqtt connect cb result: ", result);
	if(mosq == NULL) 
	{
		return;
	}
	if(!result)
	{
		/* Subscribe to broker information topics on successful connect. */
		printf("\n****************************************************\n");
		mosquitto_subscribe(mosq,NULL,m_instance->MQTT_Topic[RemoteCtrlTP],1); /*subscribe remote control cmd */
		mosquitto_subscribe(mosq,NULL,m_instance->MQTT_Topic[RemoteUpgradeTP],1);
		//mosquitto_subscribe(mosq,NULL,cfawacp->MQTT_Topic[CamreCtrl],1);
		mosquitto_subscribe(mosq,NULL,m_instance->MQTT_Topic[SyncTime],1);
		mosquitto_subscribe(mosq,NULL,m_instance->MQTT_Topic[RespondToken],1);
		printf("%s(%d): connect callback result = %d\n", __func__, __LINE__, result);
	}
	else
	{
		printf("%s(%d): connect callback result = %d\n", __func__, __LINE__, result);
	}
}

void CTsp::mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int mid)
{
	FAWACPLOG("$$$$$$$$$$$$$$$$$$$$$$$%d\n\n\n", mid);

	m_instance->m_MqttDisconnectReqFlag = TRUE;			
}
/*
	struct mosquitto_message
	{	
		int mid;	
		char *topic;	
		void *payload;	
		int payloadlen;	
		int qos;	
		bool retain;
	};

*/
void CTsp::mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	char log_str[128] = {0};
	int len = 0;
	QueueInfo_ST  QueueSend;
	fwTCTL_RemoteCtrl_UN RemoteCtrlData;
	if(message->payloadlen)
	{
		FAWACPLOG("jason add mqtt receive message %s %s %d\n", message->topic, (char *)message->payload,message->payloadlen);
	}
	else
	{
		FAWACPLOG("jason add mqtt receive message %s (null)\n", message->topic);
	}

	if(message->payloadlen > MSG_MAX_LEN)
	{
		////clog add 
	}
	else
	{
		//memcpy(QueueSend.Msgs,(char *)message->payload,message->payloadlen);

	}

	//memcpy(strtohex,(char *)message->payload,message->payloadlen);
#if 0
	for(int i =0;i<message->payloadlen;i++)
	{

		printf("%02x ",QueueSend.Msgs[i]);
	}
	printf("\n");
#endif
	QueueSend.head.Gr = GR_THD_SOURCE_TSP;

	if(m_instance->ID_Queue_TSP_To_FW > 0)
	{
		if(!strcmp(message->topic,m_instance->MQTT_Topic[RemoteCtrlTP]))
		{

			QueueSend.head.Id  = ID_TSP_2_MCU_PTC_REMOTE_CTL;

			//m_instance->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
		}
		else if(!strcmp(message->topic,m_instance->MQTT_Topic[RemoteUpgradeTP]))
		{
			QueueSend.head.Id = ID_TSP_2_OTA_VERSION_QUERY;
			printf( " recevied jason add uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu\r\n");
			//m_instance->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
		}
		else if(!strcmp(message->topic,m_instance->MQTT_Topic[SyncTime]))
		{
			//cfawacp->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
			uint8_t *pos = QueueSend.Msgs + MQTT_HEADER_LEN + MQTT_MSG_LEN + 5;
			m_instance->ShowdataTime(pos);		
			m_instance->m_LocalTimeSyncState = 1;
			m_instance->m_TimeSyncHeartReceiveFlag = 1;
			//sprintf(log_str, "%s%d", "sync time state: ", m_instance->m_LocalTimeSyncState);			
			printf( "sync time state: recevied jason add rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr\r\n");

		}
		else if(!strcmp(message->topic,m_instance->MQTT_Topic[RespondToken]))
		{
			if(m_instance->m_BokenReqFlag == 1)
			{
				m_instance->m_BokenReqFlag = 0;
			}
			else
			{
				m_instance->m_BokenReqFlag =0;
				return;
			}
			QueueSend.head.Id  = ID_TSP_2_BLE_TOKEN_ACK;
			//m_instance->Unpack_FAWACP_FrameData(strtohex + MQTT_HEADER_LEN + MQTT_MSG_LEN, message->payloadlen -MQTT_HEADER_LEN - MQTT_MSG_LEN);
			printf( "sync time state: recevied jason add sssssssssssssssssssssssssssssssss\r\n");

		}
		memcpy(QueueSend.Msgs,(char *)message->payload,message->payloadlen);
	for(int i =0;i<message->payloadlen;i++)
	{

		printf("%02x ",QueueSend.Msgs[i]);
	}
	printf("\n");

		
		//memcpy(QueueSend.Msgs,(char *)message->payload,message->payloadlen);
		tspTCTL_Parse(message->payload,message->payloadlen, (fwTCTL_RemoteCtrl_UN *)QueueSend.Msgs);
		
	 	len = msgsnd(m_instance->ID_Queue_TSP_To_FW,&QueueSend,message->payloadlen -3 + 2,IPC_NOWAIT);
		
	 for(int i =0;i<message->payloadlen;i++)
	{

		printf("%02x ",QueueSend.Msgs[i]);
	}
	printf("\n");
	
		if(len == -1)
		{
			printf("jason add tsp send to Framework error \r\n");

		}
		else
		{
			printf("jason add tsp send to mcu Framework success\r\n");

		}
	}

}
void CTsp::DownLoadMqttAuthPara()
{
	FILE *fp;
	uint8_t *dataBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	uint8_t *mqttBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);

	uint8_t *vinBuff = (uint8_t *)malloc(20);

	if((dataBuff == NULL)||(vinBuff == NULL)||(mqttBuff == NULL))
	{
		FAWACPLOG("malloc dataBuff error!");
	}

	memset(dataBuff, 0, MALLOC_DATA_LEN);
	memset(vinBuff, 0, 20);
	memset(mqttBuff, 0, MALLOC_DATA_LEN);
	memset(&m_stMqttConfig,sizeof(m_stMqttConfig),0);
	
	dataPool->getTboxConfigInfo(VinID, vinBuff, 17);
	vinBuff[17] = '\0';
	strcpy(m_stMqttConfig.m_USERNAME,vinBuff);
	FAWACPLOG("^^^^^^vin%s\n",m_stMqttConfig.m_USERNAME);
#if TEST_ENV
	sprintf(dataBuff,"%s%s%s%s%s","curl -k --cert ", "/data/111.pem"," -X POST --header 'Content-Type: application/json' --header 'Accept: application/json' 'https://beta-devicegateway-init.i-morefun.com:6443/api/tboxInitialization?sn=", vinBuff,"'");
#else
	sprintf(dataBuff,"%s%s%s%s%s","curl -k --cert ", PEM_FILE_NAME," -X POST --header 'Content-Type: application/json' --header 'Accept: application/json' 'https://device-init.i-morefun.com/api/tboxInitialization?sn=", vinBuff,"'");
#endif
	//sprintf(dataBuff,"%s%s%s%s%s","curl -k --cert ",PEM_FILE_NAME," -X POST --header 'Content-Type: application/json' --header 'Accept: application/json' 'https://111.200.239.208:18840/api/tboxInitialization?sn=", vinBuff,"'");
	FAWACPLOG("^^^^^^curl %s\n",dataBuff);
	fp = popen(dataBuff,"r");
	fread(mqttBuff, sizeof(uint8_t), MALLOC_DATA_LEN, fp);

	FAWACPLOG("^^^^^^^%s\n\n\n",mqttBuff);

	if(strstr((char*)mqttBuff,"ic0001"))
	{
		FAWACPLOG("&&&&&&&&&&\n\n");
		json_object *jsonobj = NULL;
		json_object *tmpjson = NULL;
		json_object *datajson = NULL;
		jsonobj = json_tokener_parse(mqttBuff);
		json_object_object_get_ex(jsonobj , "data" , &tmpjson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(tmpjson));
		json_object_object_get_ex(tmpjson , "gwAddress" , &datajson);
		FAWACPLOG("**************%s\n\n",json_object_get_string(datajson));
		strcpy(m_stMqttConfig.m_HOST , json_object_get_string(datajson));
		FAWACPLOG("*********%s\n",m_stMqttConfig.m_HOST);
		//char * test = mqtt_config.m_HOST;
		char *ptr = strchr((char *)m_stMqttConfig.m_HOST,':');
		FAWACPLOG("*********%s\n",ptr);
		if (ptr != NULL)
		{
			*ptr = '\0';
			m_stMqttConfig.m_PORTS = atoi(++ptr);
			printf("*********%s  %d\n",m_stMqttConfig.m_HOST,m_stMqttConfig.m_PORTS);

		}

		json_object_object_get_ex(tmpjson , "uReportFrequency" , &datajson);
		m_stMqttConfig.m_uReportFrequency = atoi(json_object_get_string(datajson));
		FAWACPLOG("*********%d\n",m_stMqttConfig.m_uReportFrequency);


		json_object_object_get_ex(tmpjson , "pskId" , &datajson);
		strcpy(m_stMqttConfig.m_PSK_ID , json_object_get_string(datajson));
		FAWACPLOG("*********%s\n",m_stMqttConfig.m_PSK_ID);

		json_object_object_get_ex(tmpjson , "carType" , &datajson);
		strcpy(m_stMqttConfig.m_Car_Type , json_object_get_string(datajson));
		FAWACPLOG("*********%s\n",m_stMqttConfig.m_Car_Type);

		json_object_object_get_ex(tmpjson , "pskString" , &datajson);
		strcpy(m_stMqttConfig.m_strPSK , json_object_get_string(datajson));
		FAWACPLOG("*********%s\n",m_stMqttConfig.m_strPSK);

		json_object_object_get_ex(tmpjson , "alarmThreshold" , &datajson);
		m_stMqttConfig.m_Alarm = atoi(json_object_get_string(datajson));
		FAWACPLOG("*********%d\n",m_stMqttConfig.m_Alarm);

		json_object_object_get_ex(tmpjson , "reportFrequency" , &datajson);
		m_stMqttConfig.m_reportFrequency = atoi(json_object_get_string(datajson));
		FAWACPLOG("*********%d\n",m_stMqttConfig.m_reportFrequency);

		json_object_object_get_ex(tmpjson , "logSwitch" , &datajson);
		m_stMqttConfig.m_LogSwitch = json_object_get_int(datajson);
		FAWACPLOG("*********%d\n",m_stMqttConfig.m_LogSwitch);

		json_object_object_get_ex(tmpjson , "snPassword" , &datajson);
		strcpy(m_stMqttConfig.m_PASSWARD , json_object_get_string(datajson));
		FAWACPLOG("*********%s\n",m_stMqttConfig.m_PASSWARD);

		{
			char szTmp[3] = {0};
			for( int i = 0; i < strlen(m_stMqttConfig.m_strPSK); i++ )  
			{  
				sprintf( szTmp, "%02X", (unsigned char) m_stMqttConfig.m_strPSK[i] );  
				memcpy( &m_stMqttConfig.m_hexPSK[i * 2], szTmp, 2 );  
			}
			FAWACPLOG("$$$$$$$$$%d %s\n",strlen(m_stMqttConfig.m_hexPSK),m_stMqttConfig.m_hexPSK);			
		}
		InitMqttParamInfo();
		m_stMqttConfig.m_TestOrNormal = 1;
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

int CTsp::MQTTConnect()
{
	int ret;
	char log_str[128] = {0};
	m_stMqttConfig.m_KEEP_ALIVE = 60;
	printf("jason add for host is %s  post is %d \n\r", m_stMqttConfig.m_HOST, m_stMqttConfig.m_PORTS);
	ret = mosquitto_connect(mosq, m_stMqttConfig.m_HOST, m_stMqttConfig.m_PORTS, m_stMqttConfig.m_KEEP_ALIVE);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		m_ConnectedState = false;
		printf("%s(%d): mqtt_connect result: %s\n", __func__, __LINE__, mosquitto_strerror(ret));
	}
	else
	{
		m_ConnectedState = true;
		printf("mqtt success^_^^_^^_^^_^^_^^_^^_^^_^^_^^_^^_^^_^^_^\n\r");		
	}
	return 0;
}

int CTsp::ShowdataTime(uint8_t *pos)
{
	struct tm _tm;
	time_t timep;
	struct timeval tv;
	
	int year = (int )((pos[1] >> 2) & 0x3F) + 1990;
	int month = ((pos[1] << 2) & 0x0C) | ((pos[2] >> 6) & 0x03);
	int day = (pos[2] >> 1) & 0x1F;
	int hour = ((pos[2] << 4) & 0x10) | ((pos[3] >> 4) & 0x0F);
	int minutes = ((pos[3] << 2) & 0x3C) | ((pos[4] >> 6) & 0x03);
	int seconds = (pos[4] & 0x3F);
	int msSeconds = pos[5]|pos[6];
	
	_tm.tm_year = year - 1900; /* 1900 */
	_tm.tm_mon = month - 1;    /* 0[0,11] */
	_tm.tm_mday = day;     /*  [1,31] */
	_tm.tm_hour = hour;     /*  [0,23] */
	_tm.tm_min = minutes;      /*  [0,59] */
	_tm.tm_sec = seconds;      /*  [0,59] */

	timep = mktime(&_tm);
	tv.tv_sec = timep;
	tv.tv_usec = 0;
	if (settimeofday(&tv, (struct timezone*)0) < 0)
	{
		printf("Set system datatime error!\n");
		return -1;
	}
		printf("set time ok\n");
	
	return 0;
}
