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
#include "common.h"
#include "Ble.h"

ParaInfoHead_ST BleHead_ARRAY[10] ={
    {GR_THD_SOURCE_BLE,ID_BLE_2_TSP_REQUEST_TOKEN},
    {GR_THD_SOURCE_BLE,ID_BLE_2_MCU_PTC_CTLCMD}
};

enum {
    TOKEN_REQ     	= 0x04,
    CTL_CMD 	 	= 0x01,
}BLE_CMD_ID;

uint8_t blecmdId[10] =
{
    TOKEN_REQ,
    CTL_CMD
};

static CBle* CBle::m_instance = NULL;

CBle::CBle()
{
	m_binit = FALSE;
	ble_rev_msg_len = 0;

}

CBle::~CBle()
{

}

static CBle* CBle::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new CBle;
	}
	return m_instance;
}

 static void CBle::FreeInstance()
{

    if(m_instance != NULL)
    {
        delete m_instance;
    }
	
    m_instance = NULL;

}
 
BOOL CBle::Init()
{

	m_binit = FALSE;

	memset(&bleQueueInfoRev,0,sizeof(QueueInfo_ST));
	memset(&bleQueueInfoSnd,0,sizeof(QueueInfo_ST));	

	m_U8ShakeHandFlg = TRUE;

	m_FdUart = -1;

	if(bleUartInit() == SUCCESS)
		BLELOG("\n\n\n\n=======================[success]: bleUartInit success=======================\n\n\n\n");
	else
		BLELOG("\n\n\n\n=======================[error]: bleUartInit error=======================\n\n\n\n");

    if(pthread_create(&Ble_RevQueue_Id, NULL, ble_RevQueue_Process, this) != 0)
            BLELOG("Cannot creat ble_RevQueue_Process:%s\n", strerror(errno));

		
	m_binit = CreateThread(); /*param is default time*/
	
	return m_binit;
}

BOOL CBle::GetInitSts(void)
{
	return m_binit;
}

BOOL CBle::Deinit()
{
    KillThread();

    return TRUE;
}

void CBle::Run()
{
   //change chen
   InformThread();

}

uint8_t CBle::ble_ReceiveQueue(uint8_t Id)
{

	switch(Id)
	{
	    case ID_FW_2_BLE_PTC_VEICHLE_INFO_REPORT:
	        ble_Receive_FW_Veichle_Info();
	        break;
	    case ID_FW_2_BLE_PTC_TOKEN:
	        ble_Receive_FW_Token();
	        break;
		case ID_FW_2_BLE_PTC_UPG_ACK:
			break;
		case ID_FW_2_BLE_PTC_UPG_BEGAIN:
			break;
		case ID_FW_2_BLE_PTC_UPG_TRANSFER:
			break;
		case ID_FW_2_BLE_PTC_UPG_END:
			break;
		case ID_FW_2_BLE_PTC_CTLACK:/*Control command Ack*/
			break;
		case ID_FW_2_BLE_PTC_GENERALACK:/*General Ack*/
			break;
		case ID_FW_2_BLE_PTC_STATEACK:/*state ack*/
			break;
		case ID_FW_2_BLE_PTC_CTLRESULT:/*control result*/
			break;
		default:
			break;
	}
	
}

uint8_t CBle::ble_SendQueue(uint8_t Id)
{

	int res = 0;
	uint8_t Cmdlen = 0;

	bleQueueInfoSnd.head.Gr = GR_THD_SOURCE_BLE;
	bleQueueInfoSnd.head.Id = Id;

	switch(Id)
	{
		case ID_BLE_2_FW_SYNC:
			Cmdlen = 2;
			break;
		case ID_BLE_2_FW_PTC:
			break;
		default:
			break;
	}
	
	res = msgsnd(QuId_BLE_2_FW,&bleQueueInfoSnd,Cmdlen,IPC_NOWAIT);
		
		if(res == -1)
		{						
			BLELOG("[ERROR]: LEN  == 0:%d\n",res);
			return FAIL;
		}
		else
		{
			return SUCCESS;
		}

}

void CBle::ble_Sync_Process(void)
{
	if(m_U8ShakeHandFlg == TRUE)
	{
		ble_SendQueue(ID_BLE_2_FW_SYNC);
	}
}

void * CBle::ble_RevQueue_Process(void *arg)
{

	uint8_t DestGroup = 0;

	m_instance->ble_rev_msg_len = msgrcv(Framework::GetInstance()->ID_Queue_FW_To_BLE,&m_instance->bleQueueInfoRev,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);
	if(m_instance->ble_rev_msg_len > 0)
	{
        BLELOG("[**ble.cpp->Rcv_Queue_Mcu_To_BLE**]%s(%d): receive datalen : %d --- type : %d--%d\n", __func__, __LINE__, (int)m_instance->ble_rev_msg_len,m_instance->bleQueueInfoRev.head.Gr,m_instance->bleQueueInfoRev.head.Id);
			
		switch (m_instance->bleQueueInfoRev.head.Gr) 				
		{
			case GR_THD_SOURCE_FW:
				m_instance->ble_ReceiveQueue(m_instance->bleQueueInfoRev.head.Id);
	        	break;	
	    	default:
	    		BLELOG("[ERROR] ILLEGAL GR_THD_SOURCE!!:%d\r\n",m_instance->bleQueueInfoRev.head.Gr);				
	        	break;
		}				
	}
	else
	{
		BLELOG("[**ble.cpp->Rcv_Queue_Mcu_To_BLE**]  no receive data !!!,errno=%d[%s]\n",errno,strerror(errno));
	}	
		
    Sleep(BLE_THREAD_PERIOD); /*sleep 500ms*/
}

BOOL CBle::Processing(void)
{
#if QUEUE_TEST_MACRO
    while(!m_bExit)
    {
	 	ReceiveQueueTestTask();
 		SendQueueTestTask();
		Sleep(BLE_THREAD_PERIOD *10);
    }
#else
	BLELOG("[PRINTF] Processing START!!\r\n");
	unsigned int len;
	unsigned int ret_value;
	mFramework = Framework::GetInstance();
	if(mFramework == NULL)
	{
		BLELOG("[ERROR] FRAME WORK NOT WORK!!!!!\r\n");
	}

    unsigned char *p_bleBuffer = (unsigned char*)malloc(BLE_BUFF_LEN);
    if(p_bleBuffer == NULL)
    {
        BLELOG("malloc p_bleBuffer failed.\n");
    }

    while(!m_bExit)
    {
        memset(p_bleBuffer, 0, BLE_BUFF_LEN);

        len = ble_ReadUartData(p_bleBuffer, BLE_BUFF_LEN);
        if (len > 0)
        {
            BLELOG("===================================================len:%d\r\n",len);
            BLELOG("===================================================len:%d\r\n",len);
            BLELOG("Ble uart rev Data:");
            for(int i=0;i<len;i++)
            {
                BLELOG("%02x ",p_bleBuffer[i]);
            }
            BLELOG("\r\n");
            BLELOG("===================================================len:%d\r\n",len);
            BLELOG("===================================================len:%d\r\n",len);

            ret_value = BIF_Parse((uint8_t *)p_bleBuffer,len,&m_instance->g_stBleBif);
            if(ret_value == RET_FAIL)
            {
                BLELOG("[** ble.cpp->processing->BIF_Parse**]%s(%d): ret_value ble cmd RET_FAIL!!!\n", __func__, __LINE__);
            }
            else if(ret_value == RET_INVALID)
            {
                BLELOG("[** ble.cpp->processing->BIF_Parse**]%s(%d): ret_value ble cmd RET_INVALID!!!\n", __func__, __LINE__);
            }
            else
            {
                BLELOG("[** ble.cpp->processing->VIF_Parse**]%s(%d): ret_value RET_OK ble cmd = %2x!!\n", __func__,__LINE__,m_instance->g_stBleBif.stBifInfo.Head.CmdID);

                for(uint8_t i=0;i<BLE_CMD_MAX_COUNT;i++)
                {
                    if(m_instance->g_stBleBif.stBifInfo.Head.CmdID == blecmdId[i])
                    {
                        bleQueueInfoSnd.head = BleHead_ARRAY[i];
                        BLELOG("[** ble.cpp->processing->BIF_Parse**]GR:%d--ID:%d:\n", bleQueueInfoSnd.head.Gr, bleQueueInfoSnd.head.Id);
                        break;
                    }
                }

                bleQueueInfoSnd.mtype = 1;
                memset(bleQueueInfoSnd.Msgs,0,sizeof(bleQueueInfoSnd.Msgs));
                memcpy(bleQueueInfoSnd.Msgs, p_bleBuffer, len);
                int ret_len = msgsnd(m_instance->mFramework->ID_Queue_BLE_To_FW,(void *)&bleQueueInfoSnd,len+sizeof(ParaInfoHead_ST),IPC_NOWAIT);
                if (ret_len < 0)
                {
                    BLELOG("ble.cpp->processing msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
                }
                else
                {
                    BLELOG("ble.cpp->processing msgsnd() write msg ble to fw ok\n"); 
                }
            }
        }
		else
		{
            printf("ble.cpp->processing uart checkBleUartData no data!!!\n");			
		}
    }

    if (p_bleBuffer != NULL){
        free(p_bleBuffer);
        p_bleBuffer = NULL;
    }	
#endif
}

BOOL CBle::TimeoutProcessing()
{

	uint8_t ble_FwInitSts = 0;
	
#if 0				
	ble_FwInitSts = FrameWork::GetInstance()->FW_GetInitSts();	
#endif

	BLELOG("[WARN] TIMEOUT PROCESS Ocurr!!\n");


	if(ble_FwInitSts == TRUE)	 
	{		
		QuId_BLE_2_FW = Framework::GetInstance()->ID_Queue_BLE_To_FW;

		//ble_UartStart();			
		InformThread();
	}

	return TRUE;
}


int CBle::bleUartInit(void)
{
    unsigned int ui32count = 0;
	int res = -1;

		/*STEP1  OPEN FILE*/
		ui32count = 10;//2MS		
		while (m_FdUart == -1 && ui32count > 0)
		{
			ui32count --;
			m_FdUart = open(BLUETOOTH_UART_DEVICE, O_RDWR | O_NOCTTY );
			usleep(200);
			if(m_FdUart != -1) break;			
		}
		if(m_FdUart == -1)
		{
			 BLELOG("[FAIL]UART OPEN PORT!!!\n");
			 return FAIL;
		}
		else
		{
			 BLELOG("[SUCCESS]UART OPEN PORT!!!\n");
		}

		
		/*STEP2  SET SPEED*/
		ui32count = 10;//2MS		
		while (res == -1 && ui32count > 0)
		{
			ui32count --;
			res = SetUartSpeed(m_FdUart, BLE_UART_SPEED);
			sleep(1);
			if(res != -1) break;			
		}
		if(res == -1)
		{
			 BLELOG("[FAIL]UART set speed!!!\n");
			 return FAIL;
		}
		else
		{
			 BLELOG("[SUCCESS]set speed!!!\n");
		}

		/*STEP3  SET PRIORITY*/
		res = -1;
		ui32count = 10;//2MS		
		while (res == -1 && ui32count > 0)
		{
			ui32count --;
			res = SetUartParity(m_FdUart, BLE_UART_DATA_BITS, BLE_UART_STOP_BITS, BLE_UART_CHECK_BIT);
			sleep(1);
			if(res != -1) break;			
		}
		if(res == -1)
		{
			 BLELOG("[FAIL]UART set speed!!!\n");
			 return FAIL;
		}
		else
		{
			 BLELOG("[SUCCESS]set speed!!!\n");
		}

    return SUCCESS;
}


void CBle::ble_UartRx_Process(void)
{
	
		
}

uint32_t CBle::ble_ReadUartData(unsigned char *pData, unsigned int size)
{
	bool startToRead = false;
	unsigned char tempData = 0;
	unsigned int retval;
	unsigned char *pos = pData;
	unsigned char runState = 0;
	bool escapeCode = false;
	char buf[512] = {0};
	int lent;
	uint8_t data = 0x01;
	uint32_t lentt;	
   
	while (!startToRead)
	{
		if ((uint32_t)(pos-pData) >= size)
			break;
		
		if (read(m_FdUart, &tempData, 1) > 0)
		{
			if (runState == 0)
			{
				if (tempData == 0x7e)
				{
					pos = pData;
					*pos = tempData;
					runState = 1;
				}
				
			}
			else if (runState == 1)
			{
				if (tempData == 0x7e)
				{
					if (pos == pData)
					{
						pos = pData;
						*pos = tempData;
						runState = 1;
					}
					else
					{
						*++pos = tempData;
						startToRead = true;
						break;
					}
				}
				else
				{
					if (escapeCode == true)
					{
						if (tempData == 0x01)
							*pos = 0x7D;
						else if (tempData == 0x02)
							*pos = 0x7E;
						else
							runState = 0;
						escapeCode = false;
					}
					else
					{
						if (tempData == 0x7d)
							escapeCode = true;
						*++pos = tempData;
					}
				}
			}
			else
			{
				runState = 0;
			}
		}
		else
		{
			BLE_NO("data <0\n");			
		}
	}
	if ((startToRead == true) && (pos > pData + 1))
		retval = pos - pData + 1;
	else
		retval = 0;
	
	
	return retval;
}
	

int CBle::SetUartParity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        perror("Setup Serial!");
        return -1;
    } 
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;

    switch (databits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size.\n");
            return -1;
    }

    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB; /* Clear parity enable */
            options.c_iflag &= ~INPCK;  /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);/* set to odd parity check */
            options.c_iflag |= INPCK;            /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;  /* Enable parity */
            options.c_cflag &= ~PARODD; /* convert to event parity check */
            options.c_iflag |= INPCK;   /* Disnable parity checking */
            break;
        case 's':
        case 'S':
            /* as no parity */
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported parity.\n");
            return -1;
    }

    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits.\n");
            return -1;
    }

    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = BLE_UART_TIMEOUT_MSECONDS; /* set timeout for 50 mseconds */
    options.c_cc[VMIN] = 0;
    options.c_cflag |= (CLOCAL | CREAD);

    /* Select the line input */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* Select the line output */
    options.c_oflag &= ~OPOST;
    /* prevant 0d mandatory to 0a */
    options.c_iflag &= ~ICRNL;

    /* Cancellation of software flow control
     * options.c_iflag &=~(IXON | IXOFF | IXANY);
     */
    options.c_iflag &= ~IXON;

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("Activate Serial failed.\n");
        return -1;
    }
	
    return 0;
}



int CBle::SetUartSpeed(int fd, int speed)
{
	int speed_arr[] = { B921600, B460800, B230400, B115200, B38400, B19200,
						B9600,	 B4800,   B2400,   B1200,	B300,	B38400,
						B19200,  B9600,   B4800,   B2400,	B1200,	B300};
	
	int name_arr[] = { 921600, 460800, 230400, 115200, 38400, 19200,
					   9600,   4800,   2400,   1200,   300,   38400,
					   19200,  9600,   4800,   2400,   1200,  300};
    int status;
    struct termios Opt;
    tcgetattr(fd, &Opt);
	
    for (unsigned int i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
    {
        if (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            if ((status = tcsetattr(fd, TCSANOW, &Opt)) != 0)
            {
                perror("tcsetattr fd!");
                return -1;
            }
            tcflush(fd, TCIOFLUSH);
        }
    }
	BLELOG("SET SPEED OK!!!!!\n");
	
    return 0;
}


void CBle::ReceiveQueueTestTask()
{
	int len = 0;
	QueueInfo_ST  QueueTest ;
	QueueTest.mtype = 1;
	   if(Framework::GetInstance()->ID_Queue_FW_To_BLE  > 0)
	   {
			len =msgrcv(Framework::GetInstance()->ID_Queue_FW_To_BLE,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long) ,1,IPC_NOWAIT);
			if(len == -1)
			{
				printf("jason add BLE receive from  fw no data \r\n");
				printf("jason add BLE receive from  fw no data \r\n");
				printf("jason add BLE receive from  fw no data \r\n");
			}
			else
			{
				printf("jason add BLE receive from  fw len = %d \r\n",len);
				printf("jason add BLE receive from  fw QueueTest.Msgs[0] = %x \r\n",QueueTest.Msgs[0]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[1] = %x \r\n",QueueTest.Msgs[1]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[2] = %x \r\n",QueueTest.Msgs[2]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[3] = %x \r\n",QueueTest.Msgs[3]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[4] = %x \r\n",QueueTest.Msgs[4]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[500] = %x \r\n",QueueTest.Msgs[500]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[501] = %x \r\n",QueueTest.Msgs[501]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[502] = %x \r\n",QueueTest.Msgs[502]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[503]);
				printf("jason add BLE receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[504]);
			}
	   }

}

 void CBle::SendQueueTestTask()
{
	QueueInfo_ST  QueueTest ;
	QueueTest.mtype = 1;

	int len = 0;
	static int count = 0;
	   if( (Framework::GetInstance()->ID_Queue_BLE_To_FW  > 0) && (count++ %5 ==0))
	   {
	   	QueueTest.Msgs[0] = 0x22;
		QueueTest.Msgs[1] = 0x33;
	   	QueueTest.Msgs[2] = 0x44;
	   	QueueTest.Msgs[3] = 0x55;
	   	QueueTest.Msgs[4] = 0x66;
		
	   	QueueTest.Msgs[500] = 0x66;
		QueueTest.Msgs[501] = 0x55;
	   	QueueTest.Msgs[502] = 0x44;
	   	QueueTest.Msgs[503] = 0x33;
	   	QueueTest.Msgs[504] = 0x22;

		 len = msgsnd(Framework::GetInstance()->ID_Queue_BLE_To_FW,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long) ,IPC_NOWAIT);
		if(len == -1)
		{
			printf("jason add BLE send to Framework error \r\n");
			printf("jason add BLE send to Framework error \r\n");
			printf("jason add BLE send to Framework error \r\n");
		}
		else
		{
			printf("jason add BLE send to mcu Framework =%d\r\n",len);
			printf("jason add BLE send to mcu Framework =%d\r\n",len);
			printf("jason add BLE send to mcu Framework =%d\r\n",len);
		}
	   }

}


int CBle::ble_Receive_FW_Veichle_Info(void)
{
    printf("ble rev fw uqeue veichle info Data:");
    for(int i=0;i<ble_rev_msg_len-2;i++)
    {
        printf("%02x ",bleQueueInfoRev.Msgs[i]);
    }
    printf("\r\n");

    ssize_t wLen;
	wLen = write(m_FdUart, bleQueueInfoRev.Msgs, m_instance->ble_rev_msg_len-2);
	printf("ble.cpp->mcu_Receive_FW_Veichle_Info send veichle info ble uart wLen:%d\r\n",wLen);
	
	return TRUE;
}


int CBle::ble_Receive_FW_Token(void)
{
	printf("ble rev fw uqeue token value Data:");
    for(int i=0;i<ble_rev_msg_len-2;i++)
    {
        printf("%02x ",bleQueueInfoRev.Msgs[i]);
    }
    printf("\r\n");

	ssize_t wLen;
	wLen = write(m_FdUart, bleQueueInfoRev.Msgs, m_instance->ble_rev_msg_len-2);
	printf("ble.cpp->ble_Receive_FW_Token value send tokento ble uart wLen:%d\r\n",wLen);

	return TRUE;
}


