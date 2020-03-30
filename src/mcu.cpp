/*
mcu:
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
#include "mcu.h"

static CMcu* CMcu::m_instance = NULL;
QueueInfo_ST mcuQueueInfoRev;
McuStatus_t McuStatus;//delete
GPS_Info_t	GPSINFO;//delete
pthread_mutex_t mcu_mutex_Tbuf = PTHREAD_MUTEX_INITIALIZER;//delete

ParaInfoHead_ST McuHead_ARRAY[10] ={
    {GR_THD_SOURCE_MCU,ID_MCU_2_FW_PTC_SYNC_REQ},
    {GR_THD_SOURCE_MCU,ID_MCU_2_FW_PTC_COMMON_ACK},
    {GR_THD_SOURCE_MCU,ID_MCU_2_TSP_PTC_REMOTE_CTL_ACK},
    {GR_THD_SOURCE_MCU,ID_MCU_2_OTA_PTC_UPG_DATA_REQ},
    {GR_THD_SOURCE_MCU,ID_MCU_2_OTA_PTC_UPG_END},
    {GR_THD_SOURCE_MCU,ID_MCU_2_TSP_PTC_VEICHLE_INFO_REPORT}
};

enum {
    SYNC_REQ     	= 0x00,
    COMM_ACK 	 	= 0x01,
    CTL_ACK 	 	= 0x04,
    UPG_DATA_REQ	= 0X06,
    UPG_END			= 0X0a,
    VEICHLE_INFO 	= 0X14
}CMD_ID;

uint8_t cmdId[10] =
{
    SYNC_REQ,
    COMM_ACK,
    CTL_ACK,
    UPG_DATA_REQ,
    UPG_END,
    VEICHLE_INFO
};


CMcu::CMcu()
{
    m_binit = false;
    rev_msg_len = 0;
    mcuWakeupevent  = STATUS_MCU_NORMAL;
    mcuUpgradeMode  = MCU_UPGRADE_STATE_IDLE;
    mFramework = Framework::GetInstance();
}

CMcu::~CMcu()
{

}

static CMcu* CMcu::GetInstance()
{
    if(m_instance == NULL)
    {
        m_instance = new CMcu;
    }
    return m_instance;
}

static void CMcu::FreeInstance()
{

    if(m_instance != NULL)
    {
        delete m_instance;
    }

    m_instance = NULL;

}

BOOL CMcu::Init()
{
    UartInit();
    if(pthread_create(&Mcu_RevQueue_Id, NULL, Mcu_RevQueue_Process, this) != 0)
        MCULOG("Cannot creat Mcu_RevQueue_Process:%s\n", strerror(errno));

    m_binit = CreateThread(); /*param is default time*/

    return m_binit;
}

BOOL CMcu::GetInitSts()
{
    return m_binit;
}


void CMcu::Deinit()
{
    KillThread();
}

void CMcu::Run()
{

    InformThread();

}

int CMcu::setSystemTime(struct tm *pTm)
{
	time_t timep;
	struct timeval tv;

	timep = mktime(pTm);
	tv.tv_sec = timep;
	tv.tv_usec = 0;
	if (settimeofday(&tv, (struct timezone*)0) < 0)
	{
		printf("Set system datatime error!\n");
		return -1;
	}

	return 0;
}

uint32_t CMcu::checkMcuUartData(unsigned char *pData, unsigned int size)
{
	bool startToRead = false;
	unsigned char tempData = 0;
	unsigned int retval;
	unsigned char *pos = pData;
	unsigned char runState = 0;
	bool escapeCode = false;
	while (!startToRead)
	{
		if ((uint32_t)(pos-pData) >= size)  break;

		if (read(m_FdUart, &tempData, 1) > 0)
		{
			//printf("8090test %02x\n",tempData);
			if (runState == 0) 
			{
				if (tempData == 0x7e) /*find start flag*/
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
					if (pos == pData) /*find start flag*/
					{
						pos = pData;
						*pos = tempData;
						runState = 1;
					}
					else			/*find end flag*/		
					{
						*++pos = tempData;
						startToRead = true;
						break;
					}
				}
				else /*receive body data*/
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
						if (tempData == 0x7d) escapeCode = true;

						*++pos = tempData;
					}
				}
			}
			else
			{
				runState = 0;
			}
		}
	}

	if ((startToRead == true) && (pos > pData + 1))
		retval = pos - pData + 1;
	else
		retval = 0;

	//MCULOG(" @@@@@@@@@@@@@@@@@@@@@@@@@ MCU received data! retval=%d", retval);
	//printf("%s(%d): datalen : %d\n", __func__, __LINE__, (int)retval);
#if 0
	for(int i=0; i<(int)retval; i++)
		printf("%02x ",*(pData+i));
#endif

	return retval;
}


 void CMcu::ReceiveQueueTestTask()
{
	QueueInfo_ST  QueueTest;
	int len = 0;

	   if(Framework::GetInstance()->ID_Queue_FW_To_MCU  > 0)
	   {
			len =msgrcv(Framework::GetInstance()->ID_Queue_FW_To_MCU,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),1,IPC_NOWAIT);
			if(len == -1)
			{
				printf("jason add mcu receive from  fw no data \r\n");
				printf("jason add mcu receive from  fw no data \r\n");
				printf("jason add mcu receive from  fw no data \r\n");
			}
			else
			{
				printf("jason add mcu receive from  fw len = %d \r\n",len);
				printf("jason add mcu receive from  fw QueueTest.Msgs[0] = %x \r\n",QueueTest.Msgs[0]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[1] = %x \r\n",QueueTest.Msgs[1]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[2] = %x \r\n",QueueTest.Msgs[2]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[3] = %x \r\n",QueueTest.Msgs[3]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[4] = %x \r\n",QueueTest.Msgs[4]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[500] = %x \r\n",QueueTest.Msgs[500]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[501] = %x \r\n",QueueTest.Msgs[501]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[502] = %x \r\n",QueueTest.Msgs[502]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[503]);
				printf("jason add mcu receive from  fw QueueTest.Msgs[503] = %x \r\n",QueueTest.Msgs[504]);
			}
	   }

}

 void CMcu::SendQueueTestTask()
{
	QueueInfo_ST  QueueTest;
	QueueTest.mtype = 1;
	int len = 0;
	static int count = 0;
	   if( (Framework::GetInstance()->ID_Queue_MCU_To_FW  > 0) && (count++ %5 ==0))
	   {
	   	QueueTest.Msgs[0] = 0x11;
		QueueTest.Msgs[1] = 0x22;
	   	QueueTest.Msgs[2] = 0x33;
	   	QueueTest.Msgs[3] = 0x44;
	   	QueueTest.Msgs[4] = 0x55;
		
	   	QueueTest.Msgs[500] = 0x55;
		QueueTest.Msgs[501] = 0x44;
	   	QueueTest.Msgs[502] = 0x33;
	   	QueueTest.Msgs[503] = 0x22;
	   	QueueTest.Msgs[504] = 0x11;

		 len = msgsnd(Framework::GetInstance()->ID_Queue_MCU_To_FW,&QueueTest,sizeof(QueueInfo_ST) -sizeof(long),IPC_NOWAIT);
		if(len == -1)
		{
			printf("jason add mcu send to Framework error \r\n");
			printf("jason add mcu send to Framework error \r\n");
			printf("jason add mcu send to Framework error \r\n");
		}
		else
		{
			printf("jason add mcu send to mcu Framework =%d\r\n",len);
			printf("jason add mcu send to mcu Framework =%d\r\n",len);
			printf("jason add mcu send to mcu Framework =%d\r\n",len);
		}
	   }

}
void *CMcu::Mcu_RevQueue_Process(void *arg)
{
    pthread_detach(pthread_self());

    uint8_t *TX_BUFF = NULL;

    TX_BUFF = (uint8_t *)malloc(BUFF_LEN);
    if(NULL == TX_BUFF)
    {
        MCULOG("Malloc TX_BUFF error!\n");
        return NULL;
    }
    while(1)
    {
        memset(TX_BUFF, 0, BUFF_LEN);

#if 1
        m_instance->rev_msg_len = msgrcv(m_instance->mFramework->ID_Queue_FW_To_MCU,&mcuQueueInfoRev,sizeof(QueueInfo_ST),0,IPC_NOWAIT);
        if(m_instance->rev_msg_len > 0)
        {
           // printf("[**mcu.cpp->Mcu_RevQueue_Process**]%s(%d): receive datalen : %d --- type : %d--%d\n", __func__, __LINE__, (int)m_instance->rev_msg_len,mcuQueueInfoRev.head.Gr,mcuQueueInfoRev.head.Id);

            switch (mcuQueueInfoRev.head.Gr) {
            case GR_THD_SOURCE_OTA:
                if(mcuQueueInfoRev.head.Id == ID_OTA_2_MCU_PTC_UPD_BEGIN)
                {

                }
                else if(mcuQueueInfoRev.head.Id == ID_OTA_2_MCU_PTC_UPD_TRANSFER)
                {

                }
                break;
            case GR_THD_SOURCE_TSP:
                if(mcuQueueInfoRev.head.Id == ID_TSP_2_MCU_PTC_REMOTE_CTL)
                {
                    memcpy(TX_BUFF,mcuQueueInfoRev.Msgs,BUFF_LEN);
                    write(m_instance->m_FdUart, TX_BUFF, BUFF_LEN);
                }
                else if(mcuQueueInfoRev.head.Id == ID_TSP_2_MCU_PTC_SET_PARM)
                {

                }
                break;
            case GR_THD_SOURCE_PM:
                break;
            case GR_THD_SOURCE_NVM:
                break;
            case GR_THD_SOURCE_BLE:
                if(mcuQueueInfoRev.head.Id == ID_TSP_2_MCU_PTC_REMOTE_CTL)
                {

                }
                else if(mcuQueueInfoRev.head.Id == ID_TSP_2_MCU_PTC_SET_PARM)
                {

                }
                break;
            case GR_THD_SOURCE_FW://ack
                CMcu::GetInstance()->mcu_ReceiveQueue(mcuQueueInfoRev.head.Id);
                break;
            default:
                break;
            }

        }
        //else
        //printf("[**GR_THD_SOURCE_MCU FW**]%s(%d): no receive data !!!\n", __func__, __LINE__);
#endif

        usleep(100*1000);
    }

    if(NULL != TX_BUFF)
    {
        free(TX_BUFF);
        TX_BUFF = NULL;
    }

    pthread_exit(0);
}

BOOL CMcu::Processing()
{
#if QUEUE_TEST_MACRO
	while(!m_bExit)
		{
		SendQueueTestTask();
		ReceiveQueueTestTask();
		Sleep(200*1000);
		}
#else
	printf("mcu processing \r\n");
	QueueInfo_ST mcuQueueInfoSnd;
	unsigned int len;
	int ret_value;

    unsigned char *p_mcuBuffer = (unsigned char*)malloc(BUFF_LEN);
    if(p_mcuBuffer == NULL)
    {
        MCULOG("malloc p_mcuBuffer failed.\n");
    }

    while(!m_bExit)
    {
        memset(p_mcuBuffer, 0, BUFF_LEN);

        len = checkMcuUartData(p_mcuBuffer, BUFF_LEN);
        if (len > 0)
        {
        #if 0
            printf("Mcu uart Data:");
            for(int i=0;i<len;i++)
            {
                printf("%02x ",p_mcuBuffer[i]);
            }
            printf("\r\n");
		#endif

            ret_value = VIF_Parse((uint8_t *)p_mcuBuffer,len,&m_instance->g_stMcuVif);
            if(ret_value == RET_FAIL)
            {
                //printf("[** mcu.cpp->processing->VIF_Parse**]%s(%d): ret_value RET_FAIL!!!\n", __func__, __LINE__);
            }
            else if(ret_value == RET_INVALID)
            {
               // printf("[** mcu.cpp->processing->VIF_Parse**]%s(%d): ret_value RET_INVALID!!!\n", __func__, __LINE__);
            }
            else
            {
                //VIF_Printf();
                //memset(m_instance->g_stMcuVif.Buf,0,sizeof(VIF_VeichleInformation_UN));
                //memcpy(m_instance->g_stMcuVif.Buf,p_mcuBuffer,len);

              //  printf("[** mcu.cpp->processing->VIF_Parse**]%s(%d): ret_value RET_OK cmd = %2x!!\n", __func__, __LINE__,m_instance->g_stMcuVif.stVifInfo.Head.CmdID);
				if(m_instance->g_stMcuVif.stVifInfo.Head.CmdID == UPG_DATA_REQ)
				{

				#if 1
					printf("############OTA###########[MCU] Get UPG CMD\r\n");
				#else
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						printf("############OTA###########[MCU] Get UPG CMD\r\n");
						

							printf("\r\n");
							for(int i=0;i<len;i++)
							{
								printf("%02x ",m_instance->g_stMcuVif.Buf[i]);
							}
							printf("\r\n");
#endif
				}

                for(uint8_t i = 0; i < MCU_CMD_MAX_COUNT; i ++)
                {
                    if(m_instance->g_stMcuVif.stVifInfo.Head.CmdID == cmdId[i])
                    {
                        mcuQueueInfoSnd.head = McuHead_ARRAY[i];
                       // printf("[** mcu.cpp->processing->VIF_Parse**]GR:%d--ID:%d:\n", mcuQueueInfoSnd.head.Gr, mcuQueueInfoSnd.head.Id);
                        break;
                    }
                }

                mcuQueueInfoSnd.mtype = 1;
                memset(mcuQueueInfoSnd.Msgs,0,sizeof(mcuQueueInfoSnd.Msgs));
                memcpy(mcuQueueInfoSnd.Msgs, p_mcuBuffer, len);
                int ret_len = msgsnd(m_instance->mFramework->ID_Queue_MCU_To_FW,(void *)&mcuQueueInfoSnd,len+sizeof(ParaInfoHead_ST),IPC_NOWAIT);
                if (ret_len < 0)
                {
                   // printf("mcu.cpp->processing msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno));
                }
                //printf("[**chenziqiang 2020-03-19 13:46**]%s(%d): send datalen : %d\n", __func__, __LINE__, (int)len);
            }
        }
		else
		{
           // printf("mcu.cpp->processing uart checkMcuUartData no data!!!\n");			
		}

		//	Sleep(100*1000);
    }

	if (p_mcuBuffer != NULL){
        free(p_mcuBuffer);
        p_mcuBuffer = NULL;
    }
	#endif
}

BOOL CMcu::TimeoutProcessing()
{
	printf("jason add mcu timeout processing \r\n");
	return true;
}

void CMcu::close_uart()
{
	close(m_FdUart);
}

int CMcu::UartInit()
{
    unsigned int ui32count = 0;

    while (ui32count++ < 10)
    {
        if ((m_FdUart = open(MCU_UART_DEVICE, O_RDWR | O_NOCTTY)) == -1)
        {
            usleep(20 * 1000);
            if(ui32count == 9)
            {
                MCULOG("Can't Open Serial Port!,ui32count = %d\n", ui32count);
                //exit(-1);
            }
        }
        else
        {
            //MCULOG("fd:%d, pid:%d\n", m_FdUart, getpid());
            break;
        }
    }

    ui32count = 0;
    while (ui32count++ < 10)
    {
        if (0 > SetUartSpeed(m_FdUart, MCU_UART_SPEED))
        {
            sleep(1);
            if(ui32count == 9)
            {
                MCULOG("set mcu uart speed failed!ui32count = %d\n", ui32count);
                //exit(-1);
            }
        }
        else
        {
            //MCULOG("set uart speed sucessed!");
            break;
        }
    }

    ui32count = 0;
    while (ui32count++ < 10)
    {
        if (SetUartParity(m_FdUart, MCU_UART_DATA_BITS, MCU_UART_STOP_BITS, MCU_UART_CHECK_BIT) == -1)
        {
            if(ui32count == 9)
            {
                MCULOG("set mcu uart speed failed!,ui32count=%d", ui32count);
                //exit(-1);
            }
            sleep(1);
        }
        else
        {
            //MCULOG("set uart parity sucessed!");
            break;
        }
    }

	return 0;
}

int CMcu::SetUartParity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		perror("Setup Serial!");
		return -1;
	} 
	options.c_cflag &= ~CSIZE;

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
	options.c_cc[VTIME] = MCU_UART_TIMEOUT_MSECONDS; /* set timeout for 50 mseconds */
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

int CMcu::SetUartSpeed(int fd, int speed)
{

    int speed_arr[] =
    {
        B921600, B460800, B230400, B115200, B38400, B19200,
        B9600,	 B4800,   B2400,   B1200,	B300,	B38400,
        B19200,  B9600,   B4800,   B2400,	B1200,	B300
    };

    int name_arr[] =
    {
        921600, 460800, 230400, 115200, 38400, 19200,
        9600,   4800,   2400,   1200,   300,   38400,
        19200,  9600,   4800,   2400,   1200,  300
    };

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
	//MCULOG("SET SPEED OK!!!!!\n");

	return 0;
}

unsigned int CMcu::Crc16Check(unsigned char* pData, uint32_t len)
{
	unsigned int ui16InitCrc = 0xffff;
	unsigned int ui16Crc = 0;
	unsigned int ui16i;
	unsigned char ui8j;
	unsigned char ui8ShiftBit;

	for(ui16i = 0;ui16i<len;ui16i++)
	{		
		ui16InitCrc ^= pData[ui16i];
		for(ui8j=0;ui8j<8;ui8j++)
		{
			ui8ShiftBit = ui16InitCrc&0x01;
			ui16InitCrc >>= 1;
			if(ui8ShiftBit != 0)
			{
				ui16InitCrc ^= 0xa001;
			}		
		}
	}

	ui16Crc = ui16InitCrc;
	return ui16Crc;
}



uint8 CMcu::GetSystemBatteryStatus()
{
	return 0;
}

void CMcu::Set_mcuWakeupEventValue(int value)
{
	mcuWakeupevent = value;
}

int CMcu::Get_mcuWakeupEventValue()
{
	return mcuWakeupevent;
}

void CMcu::Set_Uart_Sync_Value(int value)
{	
	return 0;
}


int CMcu::get_Uart_Sync_Value()
{
	return 0;
}

int CMcu::packProtocolData(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNum)
{
	return 0;
}

int CMcu::escape_mcuUart_data(unsigned char *pData, int len)
{
	return 0;
}

int CMcu::pack_mcuUart_upgrade_data(unsigned char cmd, bool isReceivedDataCorrect, int mcuOrPlcFlag)
{
	return 0;
}

unsigned short int CMcu::pack_upgrade_cmd(unsigned char *pData, int len, int mcuOrPlcFlag)
{
	return 0;
}

unsigned short int CMcu::pack_upgrade_data(unsigned char *pData, int len, bool isReceivedDataCorrect, int mcuOrPlcFlag)
{
	return 0;
}

int CMcu::Mcu_RemoteCtrlRes()
{

	return 0;
}

int CMcu::Mcu_RemoteConfigRes(uint8_t SubitemCode, uint16_t SubitemVal)
{

	return 0;
}

int CMcu::SendPowerSwitchSame(void)
{
    return 0;
}

//#if 1 //VIVIAN_ADD_MONITOR
int CMcu::mcu_ReceiveQueue(uint8_t Id)
{
    switch(Id)
    {
    case ID_FW_2_MCU_PTC_SYNC_ACK:
        m_instance->mcu_Receive_FW_Sync();
        break;
    case ID_FW_2_MCU_PTC_VEICHLE_ACK:
		m_instance->mcu_Receive_FW_Veichle_Ack();
        if(mcuSyncFlag)
        {
            //chenziqiang should add more function
            // run uart TX ACK TO 8090
        }
		break;
	case ID_FW_2_MCU_PTC_REMOTE_CTL:
		m_instance->mcu_Receive_FW_Remote_Ctl();
        break;
	case ID_FW_2_MCU_PTC_UPG_BEGAIN:	
		
		ssize_t wLen;
		wLen = write(m_instance->m_FdUart, mcuQueueInfoRev.Msgs, m_instance->rev_msg_len-2);
	#if 1
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
	#else
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
		
		
		printf("\r\n");
		printf("##########OTA##############\n");
		for(int i =0; i< wLen; i++)
		printf("%0x ",mcuQueueInfoRev.Msgs[i]);
		
		printf("\r\n");
		
		printf("##########OTA##############\n");
		#endif
		
		break;
	case ID_FW_2_MCU_PTC_UPG_DATA:
		ssize_t dLen;
		dLen = write(m_instance->m_FdUart, mcuQueueInfoRev.Msgs, m_instance->rev_msg_len-2);
	
#if 1
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
#else
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA\r\n");
			
			
			printf("\r\n:%d\r\n",dLen);
			printf("#####UART_WRITE#####ID_FW_2_MCU_PTC_UPG_DATA#############\n");
			for(int i =0; i< dLen; i++)
			printf("%0x ",mcuQueueInfoRev.Msgs[i]);
			
			printf("\r\n");
			
			printf("######UART_WRITE####ID_FW_2_MCU_PTC_UPG_DATA##############\n");
	#endif
		break;
	default:
		break;
    }
}
//#endif

int CMcu::mcu_Sync_Init(void)
{
    mcuSyncTimeOutCnt = MCU_SYNC_CNT;
    mcuSyncFlag = TRUE;
}


int CMcu::mcu_Receive_FW_Sync(void)
{
	printf("mcu.cpp->mcu_Receive_FW_Sync Mcu uart Data:");

    for(int i=0;i<m_instance->rev_msg_len-2;i++)
    {
        printf("%02x ",mcuQueueInfoRev.Msgs[i]);
    }
    printf("\r\n");

	ssize_t wLen;
    wLen = write(m_instance->m_FdUart, mcuQueueInfoRev.Msgs, m_instance->rev_msg_len-2);
	printf("mcu.cpp->mcu_Receive_FW_Sync Mcu uart wLen:%d\r\n",wLen);
    mcuSyncTimeOutCnt = MCU_SYNC_CNT;
	
	return TRUE;
}

int CMcu::mcu_Receive_FW_Veichle_Ack(void)
{
	//printf("mcu.cpp->mcu_Receive_FW_veichle_ack Mcu uart Data:");

	#if 0
	for(int i=0;i<m_instance->rev_msg_len-2;i++)
	{
		printf("%02x ",mcuQueueInfoRev.Msgs[i]);
	}
	printf("\r\n");
	#endif

	ssize_t wLen;
	wLen = write(m_instance->m_FdUart, mcuQueueInfoRev.Msgs, m_instance->rev_msg_len-2);
	//printf("mcu.cpp->mcu_Receive_FW_Sync Mcu uart wLen:%d\r\n",wLen);
	mcuSyncTimeOutCnt = MCU_SYNC_CNT;

	return TRUE;
}

int CMcu::mcu_Receive_FW_Remote_Ctl(void)
{
	#if 0
	printf("mcu.cpp->mcu_Receive_FW_Remote_Ctl Mcu uart Data:");
	for(int i=0;i<m_instance->rev_msg_len-2;i++)
	{
		printf("%02x ",mcuQueueInfoRev.Msgs[i]);
	}
	printf("\r\n");

	#endif

	ssize_t wLen;
	wLen = write(m_instance->m_FdUart, mcuQueueInfoRev.Msgs, m_instance->rev_msg_len-2);
	printf("mcu.cpp->mcu_Receive_FW_Remote_Ctl Mcu uart wLen:%d\r\n",wLen);
	mcuSyncTimeOutCnt = MCU_SYNC_CNT;

	return TRUE;
}


int CMcu::mcu_Sync_Process_200ms(void)
{
    if(mcuSyncTimeOutCnt <= 0)
    {
        mcuSyncFlag = FALSE;
    }
    else
    {
        mcuSyncTimeOutCnt --;
    }
}





