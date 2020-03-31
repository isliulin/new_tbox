/***************************************************************
COta class from baseThread
author:vivian
date:2020-03-17

2020-03-19
risk: if tar released ,unkown file too much, will ocuppy memeroy

***************************************************************/
#include "common.h"
#include "framework.h"
#include "QueueCommon.h"
#include "ATControl.h"
#include "tboxComnFun.h"
#include "DataCall.h"
#include "Ota.h"

static COta* COta::m_instance = NULL;

static COta* COta::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new COta;
	}
	return m_instance;
}



 static void COta::FreeInstance()
{

    if(m_instance != NULL)
    {
        delete m_instance;
    }
	
    m_instance = NULL;

}
 

/*
==============================================

Init private variables
==============================================
*/
COta :: COta(void) 
{

/*private variables init*/

	
	m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;
	m_u8WorkMode 	= WORKMODE_NOTREADY;

	QuId_OTA_2_FW 	= -1;
	QuId_FW_2_OTA 	= -1;

}


/*
==============================================
==============================================
*/
COta ::~COta(void)
{
	
}
void COta::ReceiveQueueTestTask()
{
	int len = 0;
	QueueInfo_ST  QueueTest;

	   if(Framework::GetInstance()->ID_Queue_FW_To_OTA  > 0)
	   {
			len =msgrcv(Framework::GetInstance()->ID_Queue_FW_To_OTA,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);
			if(len == -1)
			{
				printf("jason add ota receive from  fw no data \r\n");
				printf("jason add ota receive from  fw no data \r\n");
				printf("jason add ota receive from  fw no data \r\n");
			}
			else
			{
				printf("jason add ota receive from  fw len = %d \r\n",len);
				printf("jason add ota receive from  fw QueueTest.Msgs[0] = %x \r\n",QueueTest.Msgs[0]);
				printf("jason add ota receive from  fw QueueTest.Msgs[1] = %x \r\n",QueueTest.Msgs[1]);
				printf("jason add ota receive from  fw QueueTest.Msgs[2] = %x \r\n",QueueTest.Msgs[2]);
				printf("jason add ota receive from  fw QueueTest.Msgs[3] = %x \r\n",QueueTest.Msgs[3]);
				printf("jason add ota receive from  fw QueueTest.Msgs[4] = %x \r\n",QueueTest.Msgs[4]);
				printf("jason add ota receive from  fw QueueTest.Msgs[500] = %x \r\n",QueueTest.Msgs[500]);
				printf("jason add ota receive from  fw QueueTest.Msgs[501] = %x \r\n",QueueTest.Msgs[501]);
				printf("jason add ota receive from  fw QueueTest.Msgs[502] = %x \r\n",QueueTest.Msgs[502]);
				printf("jason add ota receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[503]);
				printf("jason add ota receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[504]);
			}
	   }

}

 void COta::SendQueueTestTask()
{
	QueueInfo_ST  QueueTest ;
	int len = 0;
	static int count = 0;
	QueueTest.mtype = 1;

	   if( (Framework::GetInstance()->ID_Queue_OTA_To_FW  > 0) && (count++ %5 ==0))
	   {
	   	QueueTest.Msgs[0] = 0x44;
		QueueTest.Msgs[1] = 0x55;
	   	QueueTest.Msgs[2] = 0x66;
	   	QueueTest.Msgs[3] = 0x77;
	   	QueueTest.Msgs[4] = 0x88;
		
	   	QueueTest.Msgs[500] = 0x88;
		QueueTest.Msgs[501] = 0x77;
	   	QueueTest.Msgs[502] = 0x66;
	   	QueueTest.Msgs[503] = 0x55;
	   	QueueTest.Msgs[504] = 0x44;

		 len = msgsnd(Framework::GetInstance()->ID_Queue_OTA_To_FW,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),IPC_NOWAIT);
		if(len == -1)
		{
			printf("jason add ota send to Framework error \r\n");

		}
		else
		{
			printf("jason add ota send to mcu Framework success\r\n");

		}
	   }

 }



/*
==============================================
=============================================
*/
BOOL COta::Init(void)
{
	/*some class init*/
	
	m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;	
	
	memset(&m_stRevMsg,0,sizeof(OTA_RevData_ST));
	memset(&m_stSndMsg,0,sizeof(OTA_SendData_ST));
	memset(&m_stProgress,0,sizeof(OTA_Progress_ST));	
	
	m_binit = CreateThread(); /*param is default time*/	

	m_u8WorkMode = WORKMODE_INIT;
	m_u8VerCheckFlag = TRUE;//POWER ON TO QUERY OSID VERSION

    #if 0
	/*if()*/
	
	{		
		m_u8WorkMode = WORKMODE_NORMAL;
	}
	
	#endif 

	time(&m_tLastTime);
	time(&m_tCurrentTime);

	return m_binit;


}


/*
==============================================
==============================================
*/
uint8_t COta::GetInitSts(void)
{

	return m_binit;

}


/*
==============================================
==============================================
*/
void COta::Deinit(void)
{

	KillThread();
	
	m_stProgress.m_u8UpgradeStep 	= STEP_OTA_UPGRADE_INIT;
	m_u8WorkMode 	= WORKMODE_NOTREADY;
	
}


/*
==============================================
==============================================
*/
BOOL COta ::Processing(void)
{
	 while(!m_bExit)
 	{
#if  QUEUE_TEST_MACRO
		 //ReceiveQueueTestTask();
 		 //SendQueueTestTask();
 		 QuId_OTA_2_FW 	= Framework::GetInstance()->ID_Queue_OTA_To_FW;//Framework::instance()->ID_Queue_OTA_To_FW;
		 QuId_FW_2_OTA   = Framework::GetInstance()->ID_Queue_FW_To_OTA;//Framework::instance()->ID_Queue_FW_To_OTA;
		
		

		if(QuId_OTA_2_FW != -1 && QuId_FW_2_OTA != -1)
		{	
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_TBOXSEND_VERSION_QUERY);

			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_MCU_PTC_UPD_ACK);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_MCU_PTC_UPD_BEGIN);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_MCU_PTC_UPD_END);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_MCU_PTC_UPD_TRANSFER);

			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_BLE_PTC_UPD_ACK);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_BLE_PTC_UPD_BEGAIN);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_BLE_PTC_UPD_END);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_BLE_PTC_UPD_TRANSFER);

			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_NVM_SAVE_OSID);
			
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_FW_UPG_STS);
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_FW_SYNC);
		}
		 
		 Sleep(OTA_THREAD_PERIOD);
#else


		QuId_OTA_2_FW 	= Framework::GetInstance()->ID_Queue_OTA_To_FW;//Framework::instance()->ID_Queue_OTA_To_FW;
		QuId_FW_2_OTA   = Framework::GetInstance()->ID_Queue_FW_To_OTA;//Framework::instance()->ID_Queue_FW_To_OTA;
		
		
		if(QuId_OTA_2_FW != -1 && QuId_FW_2_OTA != -1)
		{	
			m_u8WorkMode = WORKMODE_NORMAL;	
		}
		else
		{
			m_u8WorkMode = WORKMODE_NOTREADY;
			OTALOG("[ERROR] fw not ready!!\n");
		}
		

		

		switch(m_u8WorkMode)
		{
			case WORKMODE_NOTREADY:
				#if 0
				ota_NvmInitSts = NVM::GetInstance()->NVM_GetNvmInitSts();				
				ota_FwInitSts = FrameWork::GetInstance()->FW_GetNvmInitSts();				
				ota_PmInitSts = PowerManage::GetInstance()->FW_GetPMInitSts();

				if(	ota_NvmInitSts == TRUE() &&\
					ota_FwInitSts == TRUE() &&\
					ota_PmInitSts == TRUE() 
				  )
				{
					ota_WorkMode = WORKMODE_INIT;
				}
				#endif
				
				OTALOG("[OTA_PROCESS]WORKMODE_NOTREADY!!\n");
				break;
					
			case WORKMODE_INIT:
				//ota_Init();
				OTALOG("[OTA_PROCESS]WORKMODE_INIT!!\n");
				break;
			
			case WORKMODE_NORMAL:
				//if(thd_r_tspisConnect)
				//ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_MCU_PTC_UPD_BEGIN);
				ota_Sync_Process();
				ota_QueueReceive_Process();	
				ota_Upgrade_Process();
				break;
			default:
				break;
		}
		Sleep(OTA_THREAD_PERIOD);
#endif
 	}
	return TRUE;

}





/*
==============================================

==============================================
*/
BOOL COta ::TimeoutProcessing(void)
{

	uint8_t ota_NvmInitSts = TRUE;
	uint8_t ota_FwInitSts = TRUE;
	uint8_t ota_PmInitSts = TRUE;
	uint8_t ota_TspInitSts = TRUE;
	
#if 0

	ota_NvmInitSts = NVM::GetInstance()->NVM_GetInitSts();				
	ota_FwInitSts = FrameWork::GetInstance()->FW_GetInitSts();				
	ota_PmInitSts = PowerManage::GetInstance()->FW_GetInitSts();				
	ota_TspInitSts = Tsp::GetInstance()->Tsp_GetInitSts();
	
#endif

	if(ota_NvmInitSts == TRUE  &&\
	   	ota_FwInitSts == TRUE  &&\
	   	ota_PmInitSts == TRUE  
	  )	  
	{
	
		QuId_OTA_2_FW 	= Framework::GetInstance()->ID_Queue_OTA_To_FW;//Framework::instance()->ID_Queue_OTA_To_FW;
		QuId_FW_2_OTA   = Framework::GetInstance()->ID_Queue_FW_To_OTA;//Framework::instance()->ID_Queue_FW_To_OTA;
		
	

		if(QuId_OTA_2_FW != -1 && QuId_FW_2_OTA != -1)
			{	
				m_u8WorkMode = WORKMODE_NORMAL;	
				//InformThread();
			}
		else
			{
				OTALOG("[ERROR] fw not ready!!\n");
			}
	}
	  
	OTALOG("[OTA_TimeOutProcessing]TimeOut!!\n");

	
	return true;
}




/*
==============================================
==============================================
*/
void COta ::Run(void)
{
	OTALOG("[OTA_RUN] NULL PROCESS!!\n");
	InformThread();
}


/*
==============================================
==============================================
*/
static BOOL COta ::ota_Sync_Process(void)
{

	static uint8_t ota_SyncCnt = 0;
	/*only need send SYNC ID*/

	ota_SyncCnt ++;
	
	if(ota_SyncCnt >= CNT_SYNC_5S)
	{
		ota_SyncCnt = 0;
		ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_FW_SYNC);
	}
	
	OTALOG("[OTA_SYNC]  SEND SYNC!!\n");


}



/*
==============================================
==============================================
*/
static BOOL COta ::ota_QueueReceive_Process(void)
{
	int readLen = 0;

	readLen = msgrcv(QuId_FW_2_OTA,&m_stRevMsg,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);
	
	if(readLen > 0)
	{
		OTALOG("[SUCCESS]RevData len %d \n",readLen);
	}
	else
	{
		OTALOG("[WARN] no receive data !!!\n");
		return FALSE;
	}

	switch(m_stRevMsg.head.Gr)
	{
		case GR_THD_SOURCE_FW:
			switch(m_stRevMsg.head.Id)
			{
				case ID_FW_2_OTA_THD_EVT:
					break;

				
				case ID_FW_2_OTA_UPG_MCU_REQ:
					/*with address and offset*/
				     //1.req address
				     //2. req len	
				     memcpy(&(m_stProgress.m_stMcu.offfset),m_stRevMsg.Msgs,4);
				     memcpy(&(m_stProgress.m_stMcu.reqLen),m_stRevMsg.Msgs+4,2);
					 m_stProgress.m_stMcu.m_IsReq = TRUE;
					// m_stProgress.m_stMcu.m_upgStep = 1;
					 
					 OTALOG("START\r\n");					 
					 for(int i = 0 ;i < 16; i ++)
				 	{
				 		printf("%02x ",m_stRevMsg.Msgs[i]);
				 	} 
					 OTALOG("%02x\r\n",m_stProgress.m_stMcu.offfset);
					 OTALOG("%02x\r\n",m_stProgress.m_stMcu.reqLen);						 
					 OTALOG("END\r\n");
					
					break;
				case ID_FW_2_OTA_UPG_MCU_RESULT:
					m_stProgress.m_stMcu.res = 0;//m_stRevMsg.quedata.EcuIspRes;
					 //0 succcess
					 //1 fail
					break;

				case ID_FW_2_OTA_UPG_BLE_REQ:
					/*with address and offset*/
				     //1.req address
				     //2. req len					     
					m_stProgress.m_stBle.offfset = 0;//m_stRevMsg.quedata.transfer.offset;
					m_stProgress.m_stBle.reqLen = 0;//m_stRevMsg.quedata.transfer.reqlen;
					m_stProgress.m_stBle.m_IsReq = TRUE;
				
					break;
				case ID_FW_2_OTA_UPG_BLE_RESULT:
					 //0 ***succcess***
					 //1 ***fail***
					m_stProgress.m_stBle.res = 0;// m_stRevMsg.quedata.EcuIspRes;
					break;
				
				case ID_FW_2_OTA_NVM_LOCAL_OSID:
					LocalOSID = 0;//m_stRevMsg.quedata.fwData.LocalOsid;
					m_VcurOrlast = 0;//m_stRevMsg.quedata.fwData.CurOrLast;
					
					break;
				case ID_FW_2_OTA_PTC_TSP_REQISP:
					m_TspReq = TRUE;//m_stRevMsg.quedata.TspReq;
					break;
				
				case ID_FW_2_OTA_TSP_OSID_URL:					
					memset(TspUrl,0,OTA_URL_LEN);
					memcpy(TspUrl,"URLTEST"/*m_stRevMsg.quedata.version.url*/,OTA_URL_LEN);
					 
					break;
					
			}
			break;
		
		case GR_THD_SOURCE_PM:	
		case GR_THD_SOURCE_NVM:
		case GR_THD_SOURCE_TSP:
		case GR_THD_SOURCE_MCU:
		case GR_THD_SOURCE_BLE:
		default:
			OTALOG("[WARNING]:%x UNUSEFUL GROUP RECEIVED !!!\n",m_stRevMsg.head.Gr);
			break;
	}
			   
	return TRUE;

}





/*



/*
==============================================
==============================================
*/
 BOOL COta ::ota_Upgrade_Process(void)
{

	int res;	
	static uint8_t delayCnt = 0;
	

	switch(m_stProgress.m_u8UpgradeStep)
		{		
			case STEP_OTA_UPGRADE_INIT:
				OTALOG("[STEP_OTA_UPGRADE_INIT] \n");
				ota_STP1_CheckOTA_Condition();
				break;
			
			case STEP_OTA_UPGRADE_DOWNLOAD:
				OTALOG("[STEP_OTA_UPGRADE_DOWNLOAD] \n");
				ota_STP2_DownLoadFile();				
				break;	
			
			case STEP_OTA_UPGRADE_DELAYTIME:
				
				OTALOG("[STEP_OTA_UPGRADE_DELAYTIME] \n");				
				delayCnt ++;
				if(delayCnt >= CNT_DOWNLOAD_DELAY_100MS )
				{
					delayCnt = 0;
					m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_FILECHECK;
				}				
				break;
				
			case STEP_OTA_UPGRADE_FILECHECK:				
				
				OTALOG("[STEP_OTA_UPGRADE_FILECHECK] \n");				
				ota_STP3_CheckDownLoadFile();
			
				break;

			case STEP_OTA_UPGRADE_ISP_ECU:				
				
				OTALOG("[STEP_OTA_UPGRADE_ISP_ECU] \n");
				
				if(m_stProgress.m_stMcu.m_NeedIsp)
					ota_STP4_UpgradeMcu_Process();
				else
					m_stProgress.m_stMcu.m_upgStep = 5;

				
				if(m_stProgress.m_stBle.m_NeedIsp)
					ota_STP4_UpgradeBle_Process();
				else
					m_stProgress.m_stBle.m_upgStep = 5;
					
				
				if(m_stProgress.m_stMcu.m_upgStep == 5 && \
				   m_stProgress.m_stBle.m_upgStep == 5)
				{
				
					
					if(m_stProgress.m_stLte.m_NeedIsp)
						m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_ISP_LTE;
					else						
						m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_ISP_FINISH;
				}
				break;
				   
			case STEP_OTA_UPGRADE_ISP_LTE:				
				ota_STP5_UpgradeLte_Process();
				break;	
			
			case STEP_OTA_UPGRADE_ISP_FINISH:
				ota_STP6_Upgrade_Finish();
				break;
			
			
			default:
				break;
		}
	return TRUE;

}


/*
===================================================================
===================================================================
*/
 BOOL COta ::ota_STP1_CheckOTA_Condition(void)
{

	//static OTA_ptcUpgradeFlow_ENUM eumCheckStep = FLOW_STEP1_TSP_UPGRADE_CMD_DETECT;
	static uint16_t TimeOutCnt = 0;
	int res;

	switch(m_stProgress.eumCheckStep)
	{		
			case FLOW_STEP1_TSP_UPGRADE_CMD_DETECT: 	

				time(&m_tCurrentTime);

				if(m_tCurrentTime - m_tLastTime >= TIME_WAIT_1HOUR)
				{
					m_u8VerCheckFlag = TRUE;
					time(&m_tLastTime);
					
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					#if 0
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");
					OTALOG("[PERIOD_24H]:TRIGGER! : \n");

					#endif
				}

				/*IF TSP TRIGGER UPGRADE CMD  SHOULD SET m_u8VerCheckFlag = TRUE*/
				if(m_TspReq)
				{
					m_TspReq = 0;
					m_u8VerCheckFlag = TRUE;
					time(&m_tLastTime);
					
					
					OTALOG("[EVT_TSP_REQ]:TRIGGER! : \n");
				}
				
				if(m_u8VerCheckFlag)
				{					
					OTALOG("[step1]:attension !Get Upgrade Request : \n");
					m_u8VerCheckFlag = 0;
					m_stProgress.eumCheckStep = FLOW_STEP2_TBOX_VERSION_QUERY;
				}
				else
				{
					OTALOG("[step1]:no UpgradeReq  search upgfile if and goto Upgrade: \n");
					ota_SearchUpgradeFile();
					
				}
				break;
				
			case FLOW_STEP2_TBOX_VERSION_QUERY: 

			//when retry 1s send 
				OTALOG("[step2]:ota_SendQueue start : \n");
				res = ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_TBOXSEND_VERSION_QUERY);
				if(res != SUCCESS)
				{
					OTALOG("[step2]:ota_SendQueue ERROR : \n");
					
 					// vivian@modify ADD TIME OUT SEND 5S,RETRY TIME;NOW IS 100MS
						TimeOutCnt ++;
						if(TimeOutCnt > CNT_URL_TIMEOUT_5S)
						{
							TimeOutCnt = 0;
							m_stProgress.eumCheckStep = FLOW_STEP1_TSP_UPGRADE_CMD_DETECT;
						}

				}
				else
				{
					
					OTALOG("[step2]:go to step3  FLOW_STEP3_TSP_VERSION_INFO: \n");
					m_stProgress.eumCheckStep = FLOW_STEP3_TSP_VERSION_INFO;
					TimeOutCnt = 0;
				}			
				break;
				
			case FLOW_STEP3_TSP_VERSION_INFO://waiting for url and osid 

				/*TspOSID && TspUrl UPDATE IN ota_QUEUE_RX_PTC_Process()*/

				/* vivian@modify make sure the OSID & URL IS FROM TSP RECENTLY*/			
				//if(TspOSID.isEmpty() || TspUrl.isEmpty())
				//if(strlen(TspOSID) == 0 || strlen(TspUrl) == 0)
				if(strlen(TspUrl) == 0)
				{			
					TimeOutCnt ++;
					
					if(TimeOutCnt >= CNT_URL_TIMEOUT_15S)
					{
						m_stProgress.eumCheckStep =FLOW_STEP1_TSP_UPGRADE_CMD_DETECT; //timeout,clean isp step						
						OTALOG("[step3][ERROR]:time out 15s!!! : \n");
				
					}
					OTALOG("[step3][ALERT]:not get osid and url yet : \n");
				}
				else
				{
					if(TspOSID == LocalOSID)
					{					
						m_stProgress.m_DownLoadRes = RES_OTA_DOWNLOAD_ERROR_SAMEOSVERSION;
						ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK);
						OTALOG("[step3][ERROR]:SAME OSID : \n");
					
					}
					else
					{						
						m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_DOWNLOAD;
						OTALOG("[step3][SUCCESS]:GOTO STEP3 : \n");
					}			
					
					m_stProgress.eumCheckStep = FLOW_STEP1_TSP_UPGRADE_CMD_DETECT;
				}
				break;		
			
				
			default:
			break;
		}

	return TRUE;
}


/*
==============================================
==============================================
*/
BOOL COta ::ota_STP2_DownLoadFile(void)
{
	
	char target_ip_new[16] = {0};
	char host_protocol[8] = {0};
	uint16_t host_port = 0;
	char file_path[64] = {0};
	char domain_name[32] = {0};
	uint8_t cmd[512];
	
	datacall_info_type datacall_info;

	get_datacall_info_by_profile(4,&datacall_info);//new sdk interface


	if(datacall_info.status == DATACALL_CONNECTED)
	{

		int ret = query_ip_from_dns(domain_name, datacall_info.pri_dns_str ,/*datacall_info.pri_dns_str*/datacall_info.sec_dns_str , target_ip_new);
		OTALOG("[RESULT]query_ip_from_dns : %d\n", (int)ret);
	}

	if( strlen(target_ip_new) != 0 )
	{
		sprintf(cmd, "wget -c --timeout=%d \"%s%s:%d%s\" -O \"%s\"", HTTP_DOWNLOAD_TIMEOUT, host_protocol, target_ip_new, host_port, file_path, OTA_MQTT_UPDATE_FILE);
	}
	else
	{
		sprintf(cmd, "wget -c --timeout=%d \"%s\" -O \"%s\"", HTTP_DOWNLOAD_TIMEOUT, TspUrl, OTA_MQTT_UPDATE_FILE);
	}
	
	system(cmd);
	system("sync");	

	m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_DELAYTIME;
	m_stProgress.m_stMcu.m_upgStep = 0;
	m_stProgress.m_stBle.m_upgStep = 0;
	m_stProgress.m_stLte.m_upgStep = 0;
	m_stProgress.m_IspFileCnt  = 0;

	return TRUE;

}



/*
==============================================
==============================================
*/
BOOL COta ::ota_STP3_CheckDownLoadFile(void)
{	
	static uint8_t DownLoadRetryTime = 0;	
	
	uint8_t CalculateMd5[OTA_MD5_LEN]= {0};	
	uint8_t thd_r_TarMd5[OTA_MD5_LEN]= {0};
	uint8_t cmd[OTA_CMD_LEN]={0};
	
	int Len = 0;


    /*==========================================
		step1:make sure wether there is a tar file
	============================================*/
	if (SUCCESS == access( OTA_MQTT_UPDATE_FILE, F_OK ))	
	{
		OTALOG("[SUCCESS] DOWND MQTT TAR FILE : \n");
	
		//ota_SendQueue(GR_PTC,ID_UPG,TSP_UPGRADE_SUCCESS);
		m_stProgress.m_DownLoadRes = RES_OTA_DOWNLOAD_SUCCESS;
		ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK);
		
	}
	else
	{
		//ota_SendQueue(GR_PTC,ID_ACK,TSP_UPGRADE_URL_ERROR);	
		
		OTALOG("[ERROR] DOWND MQTT TAR FILE \n");
		m_stProgress.m_DownLoadRes = RES_OTA_DOWNLOAD_ERROR_URL;
		ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK);

		DownLoadRetryTime ++;
		if(DownLoadRetryTime <= CNT_DOWNLOAD_RETRY)
		{
			m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_DOWNLOAD;
			OTALOG("[RETRY] GOTO DOWNLOAD FILE : %d\n",DownLoadRetryTime);
		}
		else
		{
			OTALOG("[FAIL] GOTO STEP1_DETECT CMD : \n");
			DownLoadRetryTime = 0;
			m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;
			
		}
		
	}


	/*=================================================
		step2:tar file is download :md5 checksum step
	=================================================*/
	memset(CalculateMd5,0,OTA_MD5_LEN);
	
	m_hmacMd5.md5test( OTA_MQTT_UPDATE_FILE ,CalculateMd5 );
	
	for(int j = 0; j < OTA_MD5_LEN; j ++)
	{
		if(CalculateMd5[j]!=thd_r_TarMd5[j])
		{
			OTALOG("[ERROR] MD5 COMPARE!!!\n");
			
			sprintf(cmd, "rm -rf %s\0", OTA_MQTT_UPDATE_FILE );
			system( cmd );
			system("sync");	
			
			//RespondTspPacket(ACPApp_RemoteUpgradeID, UpgradeStep_5, AcpCryptFlag_IS, 0);
			//ota_SendQueue(GR_PTC,ID_ACK,TSP_MD5_ERROR);	
			
			m_stProgress.m_DownLoadRes = RES_OTA_DOWNLOAD_ERROR_MD5ERROR;
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK);			
			m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;
				
			return FALSE;
		}
		else
		{			
			/*vivian@modify done OSID SHOULD SAVE AFTER TAR DOWNLOAD SUCCESS*/
			ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_NVM_SAVE_OSID/*SAVE_OSID*/);
		}
	}

	
	
	/*========================================================
		step3:-zxvf tar file:get MCU/BLE/LTE UpgradeFile step
		               delete tar file
	=========================================================*/
	if(access(OTA_MQTT_UPDATE_FILE, F_OK) == 0)
	{
		sprintf(cmd, "tar -zxvf %s -C /data", OTA_MQTT_UPDATE_FILE);
		
		{
			OTALOG("[CMD] %s\n",cmd);
			system(cmd);
			system("sync");
			system(OTA_RM_MQTT_UPDATE_FILE);
		}
	}
	else
	{
		/**/			
		m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;
	}

	
	/*=====================================================
	step4:	make sure there is useful file after -zxvf
	=======================================================*/
		int res = 0;
		res = ota_SearchUpgradeFile();

	if(!res)
  	{
   		/*snd queue ptc RES_OTA_DOWNLOAD_ERROR_DOWNLOAD*/
		m_stProgress.m_DownLoadRes = RES_OTA_DOWNLOAD_ERROR_DOWNLOAD;
		ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK);
		
		//ota_SendQueue(GR_PTC,ID_ACK,TSP_DOWNLOAD_ERROR);
		/*vivian@modify SHOULD MAKE SURE TO DELETE ALL FILES*/
		
		OTALOG("[WARNING] after -zxvf tar, no usefull file !!!\n");
		
		m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;
		return FALSE;
  	}

	return TRUE;

}


/*
==============================================
==============================================
*/

 BOOL COta ::ota_STP4_UpgradeMcu_Process(void)
{
	//static uint8_t mcu_upg_step = 0;
	int fileFd = -1;
	unsigned char buff[ISP_BUF_SIZE]={0};	
	uint8_t cmd[ISP_CMD_SIZE]= {0};
	int len;	
	static unsigned int offsetAddr = 0;
	int ret = 0;
	
	
	if(m_stProgress.m_stMcu.m_NeedIsp)
	{
	
		OTALOG("[PRINTF] NEED ISP,step:%d\n",m_stProgress.m_stMcu.m_upgStep);
		
		switch(m_stProgress.m_stMcu.m_upgStep)
		{
			case 0:/*open file*/
				
				fileFd = open(OTA_MCU_FILE_NAME, O_RDONLY);
				
				if (fileFd < 0)
				{
					OTALOG("[ERROR] Open file:%s error\n", OTA_MCU_FILE_NAME);
					m_stProgress.m_stMcu.m_NeedIsp = 0;
				}
				else
				{
					OTALOG("[SUCCCESS] OPEN OK!\n");
					ret = ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_MCU_PTC_UPD_BEGIN);
					if(ret  == SUCCESS)
					{
						m_stProgress.m_stMcu.m_upgStep = 1;
						//m_stProgress.m_stMcu.m_upgStep = 5;
						offsetAddr = 0;
					}
					else
					{
						OTALOG("[FAIL] ota_SendQueue\n");
					}
					
				}
				close(fileFd);
				fileFd = -1;
				break;
				
			case 1:
				
				if(m_stProgress.m_stMcu.m_IsReq)//receive address 
				{		
					m_stProgress.m_stMcu.m_IsReq = FALSE;
					fileFd = open(OTA_MCU_FILE_NAME, O_RDONLY);

					if(fileFd < 0 )break;
					else
					{
						OTALOG("[SUCCCESS] step1 OPEN OK!\n");
					}
					//m_stProgress.m_stMcu.m_IsReq = FALSE;
					//OTALOG("[SUCCCESS] step1 OPEN OK!\n");
					memset(m_stSndMsg.Msgs,0,MSG_MAX_LEN);
					//OTALOG("[SUCCCESS] gggggggggggg\n");
					
				    lseek(fileFd, offsetAddr, SEEK_SET);
					//OTALOG("[SUCCCESS] hhhhhhhhhhhhhh!\n");
					//uint8_t *pos =NULL;
					//pos = m_stSndMsg.Msgs;
					m_stSndMsg.Msgs[0] = 0;
					memcpy(m_stSndMsg.Msgs +1,&m_stProgress.m_stMcu.offfset,sizeof(m_stProgress.m_stMcu.offfset));
					//pos += sizeof(m_stProgress.m_stMcu.offfset);
					memcpy(m_stSndMsg.Msgs +5,&m_stProgress.m_stMcu.reqLen,sizeof(m_stProgress.m_stMcu.reqLen));
					//pos += sizeof(m_stProgress.m_stMcu.reqLen);
					
					len = read(fileFd, m_stSndMsg.Msgs +7, m_stProgress.m_stMcu.reqLen);
					
					printf("[PRINT]read file %s, offsetAddr:%x,nRead = %d\n",OTA_MCU_FILE_NAME, m_stProgress.m_stMcu.offfset,m_stProgress.m_stMcu.reqLen);	
					
					
					if(len > 0)
					{		
						offsetAddr += len;
					}
					else
					{						
						OTALOG("[ERROR] File End\n",OTA_MCU_FILE_NAME, len);
						m_stProgress.m_stMcu.m_upgStep = 2;//delete file
					}
					
					if(len > 0) //(len == m_stProgress.m_stMcu.reqLen)
					{
						//OTALOG("[ota_Upg_mcu_step4]read len match\n");						
						OTALOG("[SUCCESS] READ DATA LEN MATCH\n");						
						ret = ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_MCU_PTC_UPD_TRANSFER);
						if(ret == SUCCESS)
						{
							offsetAddr += len;
						}
						else
						{
							OTALOG("[FAIL] SEND DATA TO FW!!!\n");	
						}
					}
					else
					{						
						OTALOG("[WARN] File End !!!\n");			
					}
					
					if(m_stProgress.m_stMcu.offfset +  \
						m_stProgress.m_stMcu.reqLen >=		\
						m_stProgress.m_stMcu.Info.m_FileLen -1) 
					{
											
						OTALOG("[PRINTF]mcu read end!!!\n");	
						m_stProgress.m_stMcu.m_upgStep = 2;
					}
				}
				else
				{
						OTALOG("[PRINTF] WAITING FOR DATA REQ! \n");
						//step =0 for max 3 times (5s each time) if >3  goto step total:STEP_OTA_UPGRADE_INIT;
				}
				close(fileFd);
				fileFd = -1;
				break;
				
			case 2:	

				//waiting for MCU END FLAG //OR TIMEOUT
				m_stProgress.m_stMcu.m_upgStep = 3;		

				//m_stProgress.m_stMcu.m_upgStep = 4;	
			    OTALOG("[PRINTF] UPGRADE END!!\n");				
				ota_SendQueue(GR_THD_SOURCE_OTA,ID_OTA_2_MCU_PTC_UPD_END);
				break;
				
			case 3:
				
				
			
				sprintf(cmd, "rm -rf %s\0", OTA_MCU_FILE_NAME );
				system(cmd);
				system("sync");	
						
			    OTALOG("[PRINTF] DELETE MCU FILE!!\n");
				m_stProgress.m_stMcu.m_upgStep = 4;
				
				break;
				
			case 4:
					//waiting for mcu response upgrage success!!
					//****compare mcu version before upgrade and after upgrade
					//****mcu old version is from fw
					//****mcu new version is from fw
					//****if difference upgrade success
					if(m_stProgress.m_stMcu.res == SUCCESS)
					{
						//m_stProgress.m_stMcu.m_NeedIsp = 0;			
						m_stProgress.m_IspFileCnt --;
						OTALOG("[PRINTF] GOTO INIT STS!!\n");
						m_stProgress.m_stMcu.m_upgStep = 5;
					}
					else
					{
						OTALOG("[PRINTF] WAITING FOR ENDS!!\n");
						//TIMEOUT
					}
			
				break;
			case 5:
				break;
				
			default:
					
				break;
		}
		
	}
	else
	{
		OTALOG("[ALERT] NO MCU FILE !!\n");
	}

	return TRUE;

}

/*
==============================================
==============================================
*/

 BOOL COta ::ota_STP4_UpgradeBle_Process(void)
{

	uint8_t ble_upg_step = 0;
	 int fileFd = -1;
	//unsigned char buff[ISP_BUF_SIZE];
	uint8_t cmd[ISP_CMD_SIZE]= {0};
	int len;	
	int res =-1;

	static unsigned int offsetAddr = 0;
	
	if(m_stProgress.m_stBle.m_NeedIsp)
	{
		OTALOG("[PRINTF] NEED ISP,step:%d\n",m_stProgress.m_stBle.m_upgStep);
		switch(m_stProgress.m_stBle.m_upgStep)
		{
			case 0:
				
				fileFd = open(OTA_BLE_FILE_NAME, O_RDONLY);
				if (fileFd < 0)
				{
					OTALOG("[ERROR] Open file:%s \n",OTA_BLE_FILE_NAME);
					m_stProgress.m_stBle.m_NeedIsp = 0;
				}
				else
				{
					OTALOG("[SUCCESS] Open file:%s \n",OTA_BLE_FILE_NAME);
					
					ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_BLE_PTC_UPD_BEGAIN);
					m_stProgress.m_stBle.m_upgStep = 1;
					offsetAddr = 0;
				}
				close(fileFd);
				fileFd = -1;
				break;
				
			case 1:
				if(1)//m_stProgress.m_stBle.m_IsReq)//receive address 
				
				{
					fileFd = open(OTA_BLE_FILE_NAME, O_RDONLY);

					if(fileFd < 0 )break;
					else
					{
						OTALOG("[SUCCCESS] step1 OPEN OK!\n");
					}
					//m_stProgress.m_stMcu.m_IsReq = FALSE;
					//OTALOG("[SUCCCESS] step1 OPEN OK!\n");
					memset(m_stSndMsg.Msgs,0,MSG_MAX_LEN);
					//OTALOG("[SUCCCESS] gggggggggggg\n");
					
				    lseek(fileFd, offsetAddr, SEEK_SET);
					//OTALOG("[SUCCCESS] hhhhhhhhhhhhhh!\n");
					
					len = read(fileFd, m_stSndMsg.Msgs, ISP_BUF_SIZE);
					
					if(len > 0)// == m_stProgress.m_stBle.reqLen)
					{
						//OTALOG("[ota_Upg_mcu_step4]read len match\n");
						offsetAddr += len;
						OTALOG("[PRINT]read file %s, offsetAddr:%x,nRead = %d\n",OTA_BLE_FILE_NAME, offsetAddr,len);
						res = ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_BLE_PTC_UPD_TRANSFER);
					}
					else
					{
						
						
						OTALOG("[ERROR] File End\n",OTA_BLE_FILE_NAME, len);
						//ble_upg_res = len_mismatch_error;
						//ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_BLE_PTC_UPD_ACK);
						m_stProgress.m_stBle.m_upgStep = 2;//delete file
						
					}

						if(len > 0) //(len == m_stProgress.m_stBle.reqLen)
					{
						//OTALOG("[ota_Upg_mcu_step4]read len match\n");						
						OTALOG("[SUCCESS] READ DATA LEN MATCH\n");						
						ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_BLE_PTC_UPD_TRANSFER);
					}
					else
					{						
						OTALOG("[WARN] File End !!!\n");			
					}

					
					if(m_stProgress.m_stBle.offfset +  \
						m_stProgress.m_stBle.reqLen >=		\
						m_stProgress.m_stBle.Info.m_FileLen -1) 
					{
						m_stProgress.m_stBle.m_upgStep = 2;
					}
				}
				close(fileFd);
				fileFd = -1;
				
				break;
				
			case 2:
				
				m_stProgress.m_stBle.m_upgStep = 3;
				OTALOG("[PRINTF] UPG END\n");
				ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_BLE_PTC_UPD_END);			
				break;
				
			case 3:
				
				
				sprintf( cmd, "rm -rf %s\0", OTA_BLE_FILE_NAME );
				system( cmd );
				system("sync");
				 
				 m_stProgress.m_stBle.m_upgStep = 4;

				 OTALOG("[PRINTF] DELETE BLE FILE!!\n");
				break;
				
			case 4:
					//waiting for mcu response upgrage success!!
					if(m_stProgress.m_stBle.res == SUCCESS)
					{
						//m_stProgress.m_stBle.m_NeedIsp = 0;			
						m_stProgress.m_IspFileCnt --;
						m_stProgress.m_stBle.m_upgStep = 5;
						
						OTALOG("[PRINTF] GOTO STEP5!!\n");
					}
					else
					{
						OTALOG("[PRINTF] WAITING FOR ENDS!!\n");
					}
			
					
			
				break;
			case 5:
				break;
				
			default:
				break;
		}
		
	}
	else
	{
		OTALOG("[ALERT] NO USEFUL BLE FILE!!\n");
	}
	return TRUE;

}



/*
==============================================
==============================================
*/
 BOOL COta ::ota_STP5_UpgradeLte_Process(void)
{
	static uint8_t lte_upgrade_step = 0;

	if(m_stProgress.m_stLte.m_NeedIsp)
	{

		switch(m_stProgress.m_stLte.m_upgStep)
		{
			case 0:
				
				#ifdef SDK_14_SUPPORT				
				OTALOG("[PRINTF] SDK14 LTE UPGRADE !!\n");
				system("mv /data/ostbox16 /data/LteUpgrade.bin");
				system("rm /data/ostbox17");
				system("rm /data/ostbox18");
				#endif
				
                #ifdef SDK_13_SUPPORT        		
				OTALOG("[PRINTF] SDK13 LTE UPGRADE !!\n");         
                 	system("mv /data/ostbox17 /data/LteUpgrade.bin");
                 	system("rm /data/ostbox16");
                 	system("rm /data/ostbox18");
                #endif

				 #ifdef SDK_12_SUPPORT      		
				OTALOG("[PRINTF] SDK12 LTE UPGRADE !!\n");           
                 	system("mv /data/ostbox18 /data/LteUpgrade.bin");
                 	system("rm /data/ostbox16");
                 	system("rm /data/ostbox17");
                #endif
				
				system("sync");
				
				m_stProgress.m_stLte.m_upgStep = 1;

				/*modem  ENTER INTO fly mode*/
				/*if not enter flymode, GPIO CTL MCU WILL FAIL*/
			case 1:
				
				if(sendATCmd((char*)"at+cfun=0", (char*)"OK", NULL, 0, 5000) <=0 )
				{
							
					OTALOG("[FAIL] LTE ENTER FLY MODE!!\n");
				}
				else
				{
							
					OTALOG("[SUCCESS] LTE ENTER FLY MODE!!\n");
					m_stProgress.m_stLte.m_upgStep = 2;
				}
				
				break;
					
			case 2:
				/*REQ MCU TO WAKEUP*/			
							
				OTALOG("[RESET] REQ PM RESET!!\n");
				
				ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_PM_RESET);
				
				m_stProgress.m_stLte.m_upgStep = 3;
			
				break;
				
			case 3:
				/*
				WAIT FOR  MCU WAKEUP SUCCESS
				*/
				//if(MCU_RESET_OK)
				m_stProgress.m_stLte.m_upgStep = 4;				
				break;

			case 4:
				/*modem  ENTER INTO NORMAL mode*/
				if(sendATCmd((char*)"at+cfun=1", (char*)"OK", NULL, 0, 5000) <=0 )
				{							
					OTALOG("[FAIL] LTE ENTER NORMAL MODE!!\n");
				}
				else
				{						
					OTALOG("[SUCCESS] LTE ENTER NORMAL MODE!!\n");
					m_stProgress.m_stLte.m_upgStep = 5;
					//m_u8UpgradeStep = STEP_OTA_UPGRADE_FINISH_RESETREQ;
					//m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;					
				}				
				break;				

			case 5:				
				/*REQ PM TO RESET LTE */	
				int res;
				res = ota_SendQueue(GR_THD_SOURCE_OTA, ID_OTA_2_TSP_THD_RESET_LTE);
			#if 1
				system("sys_reboot");
				while(1);
			#endif
				if(res != -1)
				{
									
					OTALOG("[SUCCESS][STEP5] QUEUE SEND LTE RESET!!\n");
					m_stProgress.m_stLte.m_upgStep = 6;			
					m_stProgress.m_IspFileCnt --;
				}
				else
				{
					OTALOG("[FAIL][STEP5] QUEUE SEND LTE RESET!!\n");
				}
				
				break;
				
			case 6:	
				
				/*waiting for PM RESET 4G SUCCESS*/
			    //if (LTE_RESET_SUCCESS)				
				OTALOG("[WAIT] LTE RESET SUCCESS!!\n");
			    m_stProgress.m_stLte.m_upgStep = 7;
				break;
			
			case 7:						
				m_stProgress.m_stLte.m_NeedIsp = 0;
				m_stProgress.m_stLte.m_upgStep = 0;
				m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_ISP_FINISH;				
				OTALOG("[PRINTF] LAST STEP GOTO INIT STS!!\n");			
			break;
			
		   default:				
			 break;
		}
	
	}
	else
	{
	   OTALOG("[ALERT] NO USEFULL BLE FILE !!\n");
	   m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_ISP_FINISH;
	}

	return TRUE;

}


 BOOL COta::ota_STP6_Upgrade_Finish(void)
{
	static uint8_t flag = 0;

	if(m_stProgress.m_OtaUpgFlg == TRUE && m_stProgress.m_IspFileCnt == 0)
	{
	
		m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_INIT;
		m_stProgress.m_OtaUpgFlg  	 = 0;
		
		m_stProgress.m_stMcu.m_upgStep = 0;
		m_stProgress.m_stBle.m_upgStep = 0;
		m_stProgress.m_stLte.m_upgStep = 0;
		
		m_stProgress.m_stLte.m_NeedIsp = FALSE;
		m_stProgress.m_stBle.m_NeedIsp = FALSE;
		m_stProgress.m_stMcu.m_NeedIsp = FALSE;

		m_stProgress.m_stLte.m_IsReq = FALSE;
		m_stProgress.m_stBle.m_IsReq = FALSE;
		m_stProgress.m_stMcu.m_IsReq = FALSE;
		
		OTALOG("[ALERT] ota_STP6_Upgrade_Finish finish all ota !!\n");
	}
	return TRUE;
}

 BOOL COta::ota_SearchUpgradeFile(void)
{
	if(access(OTA_MCU_FILE_NAME, F_OK) == SUCCESS)
	{
		OTALOG("[PRINTF]: FIND MCU FILE\n");
		m_stProgress.m_stMcu.m_NeedIsp = TRUE;
		m_stProgress.m_IspFileCnt ++;
		Common_GetFileCRCAndLen(OTA_MCU_FILE_NAME,&m_stProgress.m_stMcu.Info.m_nCRC,&m_stProgress.m_stMcu.Info.m_FileLen);
		OTALOG("[PRINTF]#####%02x,%02x\r\n",m_stProgress.m_stMcu.Info.m_nCRC,m_stProgress.m_stMcu.Info.m_FileLen);
	}

	if(access(OTA_BLE_FILE_NAME, F_OK) == SUCCESS)
	{
		m_stProgress.m_stBle.m_NeedIsp = TRUE;
		m_stProgress.m_IspFileCnt ++;
		Common_GetFileCRCAndLen(OTA_BLE_FILE_NAME,&m_stProgress.m_stBle.Info.m_nCRC,&m_stProgress.m_stBle.Info.m_FileLen);
	}

	if(access(OTA_LTE_FILE_NAME, F_OK) == SUCCESS)
	{
		m_stProgress.m_stLte.m_NeedIsp = TRUE;
		m_stProgress.m_IspFileCnt ++;
		
		Common_GetFileCRCAndLen(OTA_LTE_FILE_NAME,&m_stProgress.m_stLte.Info.m_nCRC,&m_stProgress.m_stLte.Info.m_FileLen);
	}
	if(m_stProgress.m_stMcu.m_NeedIsp || m_stProgress.m_stBle.m_NeedIsp ||m_stProgress.m_stLte.m_NeedIsp)
		{

			m_stProgress.m_OtaUpgFlg = TRUE;
			
		}
	
	if(m_stProgress.m_stMcu.m_NeedIsp || m_stProgress.m_stBle.m_NeedIsp )
	{
		m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_ISP_ECU;
		m_stProgress.m_stMcu.m_upgStep = 0;
		m_stProgress.m_stBle.m_upgStep = 0;
		
		if(m_stProgress.m_stMcu.m_NeedIsp)
		{
			m_stProgress.m_stMcu.m_IsReq = FALSE;
		}
		if(m_stProgress.m_stBle.m_NeedIsp)
		{
			m_stProgress.m_stBle.m_IsReq = FALSE;
		}
		return TRUE;
	}
	else if(m_stProgress.m_stLte.m_NeedIsp)
	{
	
		m_stProgress.m_u8UpgradeStep = STEP_OTA_UPGRADE_ISP_LTE;	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

 
int COta::ota_SendQueue(uint8_t Gr,uint8_t Id)
{
	uint16_t Cmdlen = 0;
	uint16_t resLen = 0;
	
	m_stSndMsg.head.Gr = Gr;
	m_stSndMsg.head.Id = Id;
	m_stSndMsg.mtype= 1;

//	memset(m_stSndMsg.Msgs,0,MSG_MAX_LEN);
	
#if 1

	OTALOG("[PRINTF] GR=%X,ID = %X!\n",Gr,Id);

	switch(Id)
	{
		case ID_OTA_2_FW_SYNC:
			m_stSndMsg.Msgs[0] = 0X00;
			break;
		
		//case ID_OTA_2_TSP_THD_RESET_LTE:
		//	m_stSndMsg.stThd.data = 0X01;
		//	break;
		
		case ID_OTA_2_TSP_PTC_TBOXSEND_VERSION_QUERY:
			m_stSndMsg.Msgs[0] = 0X00;
			break;
		
		case ID_OTA_2_TSP_PTC_VERSIONCHECK_ACK:
			m_stSndMsg.Msgs[0] = 0X00;
			break;
		
		case ID_OTA_2_PM_RESET:
			m_stSndMsg.Msgs[0] = 0X00;
			break;

		case ID_OTA_2_NVM_SAVE_OSID:
			m_stSndMsg.Msgs[0] = 0X00;
			break;

		case ID_OTA_2_MCU_PTC_UPD_ACK:	
			m_stSndMsg.Msgs[0] = 0X00;		
			break;

		case ID_OTA_2_MCU_PTC_UPD_BEGIN:
			
			OTALOG("ID_OTA_2_MCU_PTC_UPD_BEGIN m_FileLen:%02X\n",m_stProgress.m_stMcu.Info.m_FileLen);
			OTALOG("ID_OTA_2_MCU_PTC_UPD_BEGIN m_FileCRC:%02X\n",m_stProgress.m_stMcu.Info.m_nCRC);
		
			memcpy(m_stSndMsg.Msgs,&m_stProgress.m_stMcu.Info,sizeof(OTA_SocSendInfo_ST));			
			break;

		case ID_OTA_2_MCU_PTC_UPD_TRANSFER:	

		#if 1
			OTALOG("\r\n###OTA_TRANSFER start\r\n");
		#else	
		
			printf("\r\n###OTA_TRANSFER start\r\n");
			for(int i =0; i< 530; i ++)
				printf("%02x ",m_stSndMsg.Msgs[i]);
			
			printf("\r\n###OTA_TRANSFER end\r\n");
			#endif
			break;
			
		case ID_OTA_2_MCU_PTC_UPD_END:	
			m_stSndMsg.Msgs[0] = 0X00;		
			break;

			case ID_OTA_2_BLE_PTC_UPD_ACK:	
			m_stSndMsg.Msgs[0] = 0X00;		
			break;

		case ID_OTA_2_BLE_PTC_UPD_BEGAIN:	
			m_stSndMsg.Msgs[0] = 0X00;		
			break;

		case ID_OTA_2_BLE_PTC_UPD_TRANSFER:	
			m_stSndMsg.Msgs[0] = 0X00;		
			break;
			
		case ID_OTA_2_BLE_PTC_UPD_END:	
			m_stSndMsg.Msgs[0] = 0X00;		
			break;
	
		default:
			break;
	}
	
#endif
	//Cmdlen = 2;	
	resLen = msgsnd(QuId_OTA_2_FW,(void *)&m_stSndMsg,sizeof(QueueInfo_ST)-sizeof(long),IPC_NOWAIT);
	
	if(resLen == -1)
	{				   		
		OTALOG("[ERROR]: LEN MISMATCH :cmdlen %d,reslen:%d\n",Cmdlen,resLen);
		return -1;
	}
	else
	{
		return 0;
	}
	
}

#if 0
void COta::OTA_GetOTAUpgradeProgress(void)
{
}

#endif



