#include "bluetooth.h"


bleUart * bleUart::m_bleUart = NULL;
extern bool testblestate;
extern bool testnfcstate;
extern bool testfac;
extern TBox_Config_ST ConfigShow;
bool bleupgrade;
uint8_t nfctest;
uint8_t bletest;



static bleUart *bleUart::Get_BleInstance()
{
	if(m_bleUart == NULL)
	{
		m_bleUart = new bleUart();
	}
	return m_bleUart;
}
static void * bleUart::StartBleThread(void *arg)
{
	if(arg != NULL)
	{
		bleUart *pbleUart = (bleUart*)arg;
		pbleUart->BleThread();
	}
}
#define 	BLE_REQ_CONNECT_STATUS_TIMEOUT  	10 //10 s
#define	BLE_REQ_CONNECT_STATUS_COUNT		3

void *bleUart::BleThread(void)
{
	uint8_t u8BleReqConnectStatusTimeOut = 0;
	uint8_t u8BleReqConnectStatusCount = 0;

	while(1)
	{
		if(GetBleConnectStatus() == BLE_CONNECT_STATUS_UNKOWN) 
		{
			if((u8BleReqConnectStatusTimeOut == 0) && (u8BleReqConnectStatusCount < BLE_REQ_CONNECT_STATUS_COUNT))
			{
				SendReqConnectStatusToBLE();
				if(u8BleReqConnectStatusCount < 250) u8BleReqConnectStatusCount++;
				u8BleReqConnectStatusTimeOut = BLE_REQ_CONNECT_STATUS_TIMEOUT;
			}
			if(u8BleReqConnectStatusTimeOut > 0)
			{
				u8BleReqConnectStatusTimeOut --;
			}
		}
		else
		{
			if(u8BleReqConnectStatusTimeOut == 0)
			{
				SendReqConnectStatusToBLE();
				u8BleReqConnectStatusTimeOut = BLE_REQ_CONNECT_STATUS_TIMEOUT * 6; // 1 min 
			}
			if(u8BleReqConnectStatusTimeOut > 0)
			{
				u8BleReqConnectStatusTimeOut --;
			}
		}
		sleep(1);
	}

}

void bleUart::Run_BleTask()
{
	int nread = 0;
    unsigned int len = 0;

	pfawacp =  CFAWACP::GetInstance();
	m_CMcu = CMcu::GetInstance();
	bleUartInit();
	/*
	if(pthread_create(&m_BleThreadId, NULL, StartBleThread, this) != 0)
		BLELOG("Cannot creat SendThread:%s\n", strerror(errno));
	*/
	printf("%s(%d): ======================Run BleTask======================\n", __func__ , __LINE__);
	unsigned char *p_mcuBuffer = (unsigned char*)malloc(BT_BUFF_LEN);
	if(p_mcuBuffer == NULL)
		BLELOG("malloc p_mcuBuffer failed.\n");

	while(1)
	{
		memset(p_mcuBuffer, 0, BT_BUFF_LEN);

		len = checkBleUartData(p_mcuBuffer, BT_BUFF_LEN);
		if (len > 0)
		{
			BLELOG("\n\n");
			BLELOG("Recv BLE data::");
			for(int i = 0; i < len; i++ )
				BLE_NO("%02x ", p_mcuBuffer[i]);
			BLELOG("\n\n");
		
			nread = unpackBleUartData(p_mcuBuffer, len);
			if (nread == -1)
			{
				BLELOG("unpack data failed!");
			}
		}
		usleep(10*1000);
	}
	if (p_mcuBuffer != NULL)
	{
		free(p_mcuBuffer);
		p_mcuBuffer = NULL;
	}
	
}

/*****************************************************************************
* Function Name : bleUart
* Description   : 构造函数
* Input			: None
* Output        : None
* Return        : None
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
bleUart::bleUart()
{
	fd = -1;
	m_id = 0;

	m_BleReqFlag = 0;
	m_u8BleUpdateConnectTimeout = 0;
	nfctest = 1;
	bletest = 1;
	bleupgrade = false;
	m_BleConnectStatus = BLE_CONNECT_STATUS_UNKOWN;
}

/*****************************************************************************
* Function Name : close_uart
* Description   : 
* Input		 : None
* Output         : None
* Return        : None
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
void bleUart::close_uart()
{
	close(fd);
	fd = -1;
}

/*****************************************************************************
* Function Name : CMcuInit
* Description   : mcu串口初始化 
* Input			: None
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::bleUartInit()
{
    unsigned int ui32count = 0;

    while (ui32count++ < 10)
    {
        if ((fd = open(BLUETOOTH_UART_DEVICE, O_RDWR | O_NOCTTY )) == -1)
        {
			usleep(200);
            BLELOG("Can't Open Serial Port!,ui32count = %d\n", ui32count);
			if(ui32count >= 9)//LIUYINGYING
				exit(-1);
        }
        else
        {
            BLELOG("fd:%d, pid:%d\n", fd, getpid());
            break;
        }
    }

    ui32count = 0;
    while (ui32count++ < 10)
    {
        if (0 > setUartSpeed(fd, BLE_UART_SPEED))
        {
            BLELOG("set mcu uart speed failed!ui32count = %d\n", ui32count);
			if(ui32count >= 9)//LIUYINGYING
				exit(-1);
            sleep(1);
        }
        else
        {
            BLELOG("set uart speed sucessed!");
            break;
        }
    }

    ui32count = 0;
    while (ui32count++ < 10)
    {
        if (setUartParity(fd, MCU_UART_DATA_BITS, MCU_UART_STOP_BITS, MCU_UART_CHECK_BIT) == -1)
        {
            BLELOG("set mcu uart speed failed!,ui32count=%d", ui32count);
			if(ui32count >= 9)//LIUYINGYING
				exit(-1);
            sleep(1);
        }
        else
        {
            BLELOG("set uart parity sucessed!");
            break;
        }
    }

    return 0;
}

/*****************************************************************************
* Function Name : setUartSpeed
* Description   : 设置串口波特率
* Input			: int fd,
*                 int speed
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::setUartSpeed(int fd, int speed)
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

/*****************************************************************************
* Function Name : setUartParity
* Description   : 设置串奇偶性、数据位和停止位
* Input			: int fd,
*                 int databits,
*                 int stopbits,
*                 int parity
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::setUartParity(int fd, int databits, int stopbits, int parity)
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

/*****************************************************************************
* Function Name : bleUartReceiveData
* Description   : 设置串奇偶性、数据位和停止位
* Input			: None
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::bleUartReceiveData()
{

    int nread;
    unsigned int len;
    unsigned char *p_mcuBuffer = (unsigned char*)malloc(BT_BUFF_LEN);
	if(p_mcuBuffer == NULL)
	{
		BLELOG("malloc p_mcuBuffer failed.\n");
		return -1;
	}
    memset(p_mcuBuffer, 0, BT_BUFF_LEN);

  //  BLELOG("start to read mcu data\n");

    len = checkBleUartData(p_mcuBuffer, BT_BUFF_LEN);
    if (len > 0)
    {
        nread = unpackBleUartData(p_mcuBuffer, len);
        if (nread == -1)
        {
            BLELOG("unpack data failed!");
        }
    }
	if (p_mcuBuffer != NULL)
	{
		free(p_mcuBuffer);
		p_mcuBuffer = NULL;
	}

	return 0;
}


/*
int bleUart::registerCallback_reportDate(callBack_reportDate func)
{
	if (func == NULL)
		return -1;
	
	reportDataFunc = func;

	return 0;
}*/


/*****************************************************************************
* Function Name : checkBleUartData
* Description   : 数据检查
* Input			: unsigned char *pData
*                 unsigned int size
* Output        : None
* Return        : retval:数据长度, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
uint32_t bleUart::checkBleUartData(unsigned char *pData, unsigned int size)
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
		if (read(fd, &tempData, 1) > 0)
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
		else{
			BLE_NO("data <0\n");
			{
				/*nfccount++;
				if(nfccount == 60)
				{
					bletest = 0x01;
				}*/
			//lentt=write(fd, &data, 1);
			//BLELOG("write testdata is %d\n",lentt);
			}	
			//break;
		}
	}
	//nfccount = 0;
	if ((startToRead == true) && (pos > pData + 1))
		retval = pos - pData + 1;
	else
		retval = 0;
	
	//printf("%s(%d): @@@@@@@@@@@@@@@@@@@@@@@@@ ble received data! retval=%d", __func__, __LINE__, retval);
//	for(int j=0; j<(int)retval; j++)
//		BLE_NO("%02x",*(pData+j));
//	for(int i=0; i<(int)retval; i++)
//		BLE_NO("data[%d] %02x\n ",i,*(pData+i));
//	BLE_NO("\n\n"); 
	
	return retval;
}

/*****************************************************************************
* Function Name : unpackBleUartData
* Description   : 解包mcu数据
* Input			: uint8_t *pData
*                 unsigned int datalen
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::unpackBleUartData(uint8_t *pData, unsigned int datalen)
{
    uint16_t checkCrc;
	uint16_t serialNumber = 0;
	uint16_t rpserialNumber = 0;
    uint16_t bodylen = 0;
    uint8_t cmdId;

    if ((pData == NULL) || (datalen <= 4))
        return -1;

    bodylen = (pData[1] << 8) + pData[2];
    if ((uint32_t)(bodylen+11)!= datalen)
        return -1;

    checkCrc = (pData[1+7+bodylen] << 8) + pData[1+7+bodylen+1];
    if (checkCrc != Crc16Check(&pData[1], datalen-4))
    {
        BLELOG("check crc16 failed!\n");
        return -1;
    }

    cmdId = pData[3];
	//printf("%s(%d): cmdId:%02x \n", __func__ , __LINE__ , cmdId);
	
    switch (cmdId)
    {
		case BLE_Control_CMD:	//0x02
			serialNumber = (pData[4] << 8) + pData[5];
			
			unpack_Control(pData, datalen);
			packDataWithRespone(TBOX_GENERAL_RESP, 0x02, NULL, 0, serialNumber);
			if(m_BleReqFlag == 1)
			{
				m_BleReqFlag = 0;
				packbleProtocolData(0x82, 0x01, NULL, 0, serialNumber);
				printf("jason add for send token \r\n");
			}
		break;
		case BLE_GENERAL_RESP:
			serialNumber = (pData[4] << 8) + pData[5];
			if(bodylen < 12)
				break;
			//memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion, 0, 12);
			memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion, &pData[8], 12);
			//printf("jason add BLE_GENERAL_RESP BLE version is %s\r\n", p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion);

			bletest = 0x00;
		break;
		case BLE_Reply_State:
			serialNumber = (pData[4] << 8) + pData[5];
			testnfcstate = true;
			if(testfac == false)
			{
				unpack_blereply(pData, datalen);
			}
			packDataWithRespone(TBOX_GENERAL_RESP, 0x02, NULL, 0, serialNumber);
		break;
		case BLE_Control_Reply:
			serialNumber = (pData[4] << 8) + pData[5];
			testblestate = true;
			unpack_bleRemoteCtrl(pData, datalen);
		break;
		case 0x88: /*req ble update info  */
			if(bleupgrade == false)
			{
				serialNumber = (pData[4] << 8) + pData[5];
				bleupgrade = true;
				check_ble_file_is_exist();
			}
		break;
		case 0x06: /* req ble update data */
			ble_apply_for_data(pData, datalen);
		break;
		case 0x0a: /* ble update finish */
 			if((pData[8]==0)||(pData[8]==2))
 			{
 				system(RM_BLE_FILE);
 				system("sync");
 				if(!access(BLE_FILE_NAME, F_OK))
 					system(RM_BLE_FILE);
 			}
 			bleupgrade = false;
		break;
		default:
            BLELOG("cmdid error!\n");
        break;
    }
	
    return 0;
}

/*****************************************************************************
* Function Name : Crc16Check
* Description   : 数据检验
* Input			: uint8_t *pData
*                 uint32_t len
* Output        : None
* Return        : ui16Crc:校验码
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
unsigned int bleUart::Crc16Check(unsigned char* pData, uint32_t len)
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



int bleUart::unpack_Control(unsigned char *pData, unsigned int len)
{
	int times = 0;
	uint8_t TempData = 0x01;
	int nMcuWake4GStatus = 0;
       uint8_t u8CmdNeedTwoCmd = 0;
	uint8_t cmd = pData[8];
	CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemTotal = 1;
	switch(cmd)
	{
		case 0x00: /*req token value*/
			printf("jason add for receive req token value \r\n");
			m_BleReqFlag = 1;
			return;
		case 0x01:  /*open lock cmd*/
			printf("jason add for receive open lock cmd \r\n");

			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=1;
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=0;
			break;
		case 0x02:  /*close lock cmd*/
			printf("jason add for receive close lock cmd \r\n");

			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=1;
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=1;
			break;
		case 0x03:  /*power valid cmd*/
			printf("jason add for receive power valid cmd \r\n");

			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=3;
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=1;
			break;
		case 0x04:  /*power invalid cmd*/
			printf("jason add for receive power invalid cmd \r\n");

			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=3;
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=0;
			break;
		case 0x05:  /* drop-off  back car cmd*/
			printf("jason add for receive drop-off cmd \r\n");

			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0] = 1;////2;  /*close lock cmd*/
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0] = 1;
			u8CmdNeedTwoCmd = CMD_NEED_SEND_SECOND_POWRR_INVALID;
			break;
		case 0x06: /*force open lock cmd ( power ) */
			printf("jason add for receive force open lock  cmd \r\n");
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0] = 3;//2;
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0] = 1;//0;
			u8CmdNeedTwoCmd = CMD_NEED_SEND_SECOND_OPEN_LOCK;
			break;
		case 0x08: /*flash light and whistle cmd(find car cmd)*/
			printf("jason add for receive force open lock  cmd \r\n");
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0] = VehicleBody_TrackingCarID;//2;
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0] = 1;//0;
			break;			
		default:
			break;
	}
	
	if(tboxInfo.operateionStatus.isGoToSleep == 0)
	{
#if 0
		//#define MCU_WAKEUP_MODEM_GPIO  74 //sim7600.pin72
		modem_ri_notify_mcu();

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
			   		/*do nothing*/
			   	}
			}
			else
			{
			   	/*do nothing*/
			}
			printf("VALUE == %d, sync == %d, times == %d\n", nMcuWake4GStatus, m_CMcu->get_Uart_Sync_Value(), times);
			
		}while(times <= MCU_WAKEUP_TIMES);
		
		if(times >= MCU_WAKEUP_TIMES)
		{
			//nothing
		}
#endif
		m_CMcu->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
#if 0
		while(m_CMcu->Get_mcuWakeupEventValue() == STATUS_WAKEUP_EVENT)
		{
			sleep(1);
		}
#endif
	}

	m_CMcu->Mcu_RemoteCtrlRes();
	
	if(u8CmdNeedTwoCmd != 0)
	{
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemTotal = 1;
		if(u8CmdNeedTwoCmd == CMD_NEED_SEND_SECOND_POWRR_INVALID)
		{
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0] = 3;////2; /*power invalid cmd*/
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0] = 0;
			CFAWACP::GetInstance()->BleReqUpdateTokenFlag = 1;
			CFAWACP::GetInstance()->u8BleTokenReqTimeOut = 0;

		}
		else
		{
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0] = 1;
			CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0] = 0;
		}
		m_CMcu->Mcu_RemoteCtrlRes();
	}
	return 0;
}

int bleUart::unpack_blereply(unsigned char *pData, unsigned int len)
{
	int times = 0;
	int nMcuWake4GStatus = 0;
	uint8_t TempData = 0x01;
	uint8_t cmd = pData[8];
	CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemTotal = 1;

	if(tboxInfo.operateionStatus.isGoToSleep == 0)
	{
#if 0
		//#define MCU_WAKEUP_MODEM_GPIO  74 //sim7600.pin72
	//	modem_ri_notify_mcu();
		modem_ri_notify_mcu_high();
		usleep(1000*1500);
		uart_wakeup(1);
		do
		{
			times++;
			//2019-01-14 KK modify ：修改唤醒主动向MCU发送数据请求
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
			   	/*do nothing*/
			   	}
			}
			else
			{
			   	/*do nothing*/
			}
			printf("VALUE == %d, sync == %d, times == %d\n", nMcuWake4GStatus, m_CMcu->get_Uart_Sync_Value(), times);
			
		}while(times <= MCU_WAKEUP_TIMES);
		
		if(times >= MCU_WAKEUP_TIMES)
		{
			//nothing
		}
		modem_ri_notify_mcu_low();
		
#endif
		m_CMcu->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
#if 0
		while(m_CMcu->Get_mcuWakeupEventValue() == STATUS_WAKEUP_EVENT)
		{
			sleep(1);
		}
#endif
	}
	if((p_FAWACPInfo_Handle->VehicleCondData.PowerState ==0) && (p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.drivingDoor==0) && (p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.copilotDoor ==0))
	{
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=1;
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=0;
		m_CMcu->Mcu_RemoteCtrlRes();
		sleep(1);
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=3;
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=1;
		m_CMcu->Mcu_RemoteCtrlRes();
	}
	else if((p_FAWACPInfo_Handle->VehicleCondData.PowerState ==1) && (p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.drivingDoor==0) && (p_FAWACPInfo_Handle->VehicleCondData.CarDoorState.bitState.copilotDoor ==0))
	{
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=1;
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=1;
		m_CMcu->Mcu_RemoteCtrlRes();
		sleep(1);
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0]=3;
		CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]=0;
		m_CMcu->Mcu_RemoteCtrlRes();
	}
	/*switch(cmd)
	{
		case 0x01:  //开锁
		pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=1;
		pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=0;
		m_CMcu->cb_RemoteCtrlCmd();
		sleep(1);
		pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=3;
		pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=1;
		m_CMcu->cb_RemoteCtrlCmd();
		break;
		case 0x02:  //关锁
		pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=1;
		pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=1;
		m_CMcu->cb_RemoteCtrlCmd();
		sleep(1);
		pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=3;
		pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=0;
		m_CMcu->cb_RemoteCtrlCmd();
		break;
		default:
		break;
	}*/
	//m_CMcu->cb_RemoteCtrlCmd();
	return 0;
}
uint8_t bleUart::GetBleConnectStatus(void)
{
	return m_BleConnectStatus;
}
void bleUart::SetBleConnectStatus(uint8_t u8ConnectStatus)
{
	 m_BleConnectStatus = u8ConnectStatus;
}


int bleUart::unpack_bleinfo(unsigned char *pData, unsigned int len)
{
	uint8_t result[100]={0};
	uint8_t cmd = pData[8];
	uint8_t length = pData[9];
	//memcpy(result,pData[10],length);
	memcpy(result, pData+10, length);//zhujw
	//printf("**ble name is %s\n",result);
	return 0;
}



/*****************************************************************************
* Function Name : packDataWithRespone
* Description   : 应答函数
* Input			: uint8_t responeCmd
*                 uint8_t subCmd
*                 uint8_t *needToPackData
*                 uint16_t len
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::packDataWithRespone(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNumber)
{
	int i,headLen;
	int totalDataLen;
	unsigned char *pos = NULL;
	unsigned short int dataLen;
	unsigned int checkCode;
	static unsigned short int serialNO = 0;
	
	unsigned char *pBuff = (unsigned char *)malloc(BT_BUFF_LEN);
	if(pBuff == NULL)
		return -1;

    memset(pBuff, 0, BT_BUFF_LEN);
	
	pos = pBuff;
	
	*pos++ = 0x7e;
	//data length
	*pos++ = 0;
	*pos++ = 0;
	//cmd
	*pos++ = responeCmd;
	//serial NO.
	*pos++ = (serialNO & 0xff00) >> 8;
	*pos++ = (serialNO & 0xff) >> 0;

	//data property: the data which be send has been encrypted
	*pos++ = 0;
	*pos++ = 0;
	*pos++ = subCmd;
	*pos++ = (serialNumber & 0xff00) >> 8;
	*pos++ = (serialNumber & 0xff) >> 0;

	headLen = pos-pBuff;
	BLELOG("headLen = %d \t subCmd:%02x\n", headLen, subCmd);

	switch (responeCmd)
	{
		case TBOX_GENERAL_RESP:
			dataLen = 3;
			*pos++ = 0x00;
			break;
		default:
			BLELOG("The cmd error!\n");
			break;
	}

	//BLELOG("dataLen = %d\n",dataLen);
	if(1)
	{
	pBuff[1] = (dataLen & 0xff00)>>8;
	pBuff[2] = (dataLen & 0xff)>>0;
	}
	pos += dataLen;
	
	//Calculated check code
	checkCode = Crc16Check(&pBuff[1], pos-pBuff-1);
	BLELOG("checkCode = %0x\n", checkCode);
	
	*pos++ = (checkCode & 0xff00)>>8;
	*pos++ = (checkCode & 0xff)>>0;

	*pos++ = 0x7e;

	totalDataLen = pos-pBuff;
	BLELOG("totalDataLen =%d", totalDataLen);

	BLELOG("\n\n\nBefore escape Data:");
	for(i = 0; i < totalDataLen; i++)
		BLE_NO("%02x ", *(pBuff+i));
	BLE_NO("\n");

	escape_bleUart_data(pBuff, totalDataLen);

	serialNO++;
	if (serialNO > 10000)
		serialNO = 0;

	if (pBuff != NULL)
	{
		free(pBuff);
		pBuff = NULL;
	}

    return 0;
}

/*****************************************************************************
* Function Name : packbleProtocolData
* Description   : 
* Input			: uint8_t responeCmd
*                 uint8_t subCmd
*                 uint8_t *needToPackData
*                 uint16_t len
*                 uint16_t serialNum
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::packbleProtocolData(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNum)
{
	int i, totalDataLen;
	unsigned char headLen;
	unsigned char *pos = NULL;
	unsigned short int dataLen;
	unsigned int checkCode;
	unsigned short int attribution = 0;
	static unsigned short int serialNO = 0;
	
	unsigned char *pData = (unsigned char *)malloc(BT_BUFF_LEN);
	if(pData == NULL)
		return -1;
	memset(pData, 0, BT_BUFF_LEN);

	pos = pData;
	
	*pos++ = 0x7e;
	//data length
	*pos++ = 0;
	*pos++ = 0;
	//cmd
	*pos++ = responeCmd;
	//serial NO.
	*pos++ = (serialNO & 0xff00) >> 8;
	*pos++ = (serialNO & 0xff) >> 0;

	//data property: the data which be send has been encrypted
	*pos++ = 0;
	*pos++ = 0;

	headLen = pos-pData;
	BLELOG("ble headLen = %d\n", headLen);

	switch (responeCmd)
	{
		case 0x83:   //
			//Response command
			*pos++ = subCmd;
			dataLen = pos-pData-headLen;
			break;
		case 0x82:   
			//Response command

		
			*pos++ = subCmd;
			*pos++ = 0x06;
			//memcpy(pos,"123456",6);  /* change from secret key to token value, by jason.sun */

			memcpy(pos,m_BleToken,sizeof(m_BleToken));

			pos += sizeof(m_BleToken);
			
			dataLen = pos-pData-headLen;

			break;
		case 0x84:   //
			*pos++ = subCmd;
			{
				switch (subCmd)
				{
					case 0x01:
						break;
					case 0x05:
						memcpy(pos,ConfigShow.Tbox_BLENAME,8);
						//memcpy(pos,"12345678",8);
						pos+=8;
						break;
					case 0x06:
						break;
					case 0x07:
						break;
				}
			}
			//Response command
			dataLen = pos-pData-headLen;
			break;
		case TBOX_REQ_CFGINFO:
			if(needToPackData == NULL)
			{
				*pos++ = 0;
			}
			else
			{
				*pos++ = *needToPackData;
			}
			dataLen = pos-pData-headLen;
			break;
		case 0x87:
			*pos++ = subCmd;
			dataLen = pos-pData-headLen;
			break;
		default:
			break;
	}
	BLELOG("dataLen = %d\n",dataLen);
	pData[1] = (dataLen & 0xff00)>>8;
	pData[2] = (dataLen & 0xff)>>0;

	//pos += dataLen;
	
	//Calculated check code
	checkCode = Crc16Check(&pData[1], pos-pData-1);
	BLELOG("checkCode = %0x\n", checkCode);
	
	*pos++ = (checkCode & 0xff00)>>8;
	*pos++ = (checkCode & 0xff)>>0;

	*pos++ = 0x7e;

	totalDataLen = pos-pData;
	BLELOG("totalDataLen =%d", totalDataLen);

	BLELOG("Before escape Data:");
	//for(i = 0; i < totalDataLen; i++)
	//	printf("%02x ", *(pData+i));
	//printf("\n");

	serialNO++;
	if (serialNO > 10000)
		serialNO = 0;

	escape_bleUart_data(pData, totalDataLen);

	if (pData != NULL)
	{
		free(pData);
		pData = NULL;
	}

	return 0;
}

/*****************************************************************************
* Function Name : escape_bleUart_data
* Description   : 
* Input			: unsigned char *pData
*                 int len
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int bleUart::escape_bleUart_data(unsigned char *pData, int len)
{
	int i;
	int totalEscapeDataLen;
	int escapeDataLen;
	int escapeTimes = 0;
	unsigned char *pBuffData = NULL;

	unsigned char *pBuff = (unsigned char *)malloc(BLE_UPGRADE_BUFF);
	if(pBuff == NULL)
		return -1;
    memset(pBuff, 0, BLE_UPGRADE_BUFF);

	pBuffData = pBuff;
	*pBuffData = 0x7e;
	//BLELOG("Escape *pBuffData:%02x, pBuff=%02x addr:%x\n",*pBuffData,*pBuff, pBuffData);
	pBuffData++;

	escapeDataLen = len-2;
	//BLELOG("escapeDataLen:%d\n", escapeDataLen);

	for(i = 0; i < escapeDataLen; i++)
	{
		if((pData[i+1] != 0x7e) && (pData[i+1] != 0x7d))
		{
			*pBuffData = pData[i+1];
			pBuffData++;
		}
		else
		{
			if(pData[i+1] == 0x7e)
			{
				*pBuffData++ = 0x7d;
				*pBuffData++ = 0x02;
			}
			else if(pData[i+1] == 0x7d)
			{
				*pBuffData++ = 0x7d;
				*pBuffData++ = 0x01;
			}
			escapeTimes++;
			BLELOG("escapeTimes:%d\n", escapeTimes);
		}		
	}

	*pBuffData++ = 0x7e;

	//BLELOG("escapeTimes:%d\n", escapeTimes);
	totalEscapeDataLen = escapeDataLen+escapeTimes+2;
	//BLELOG("totalEscapeDataLen:%d\n", totalEscapeDataLen);
	uint32_t lenw;
	lenw=write(fd, pBuff, totalEscapeDataLen);
	BLELOG("\n\nble write data len is %d\n",lenw);
	for(int i = 0; i < lenw; i++ )
	{
		BLE_NO("%02x ",pBuff[i] );
	}
	BLELOG("\n\n");
	/*AES_CBC_encrypt_decrypt();

	printOutMsg(pBuff, totalEscapeDataLen);

	uint8_t outputData[1024];
	uint16_t len1;
	memset(outputData, 0,sizeof(outputData));

	aes_encrypt_cbc(outputData, &len1, pBuff, totalEscapeDataLen);

	printf("\n\nlen1 = %d\n\n", len1);

	uint8_t outputData1[1024];
	uint16_t len2;
	memset(outputData1, 0,sizeof(outputData1));

	aes_decrypt_cbc(outputData1, &len2, outputData, len1);
	
	printf("\n\nlen2 = %d\n\n", len2); */

	if (pBuff != NULL)
	{
		free(pBuff);
		pBuff = NULL;
	}

	return 0;
}
int bleUart::write_bleUart_data(unsigned char *pData, int len)
{
	uint32_t lenw;
	if(pData == NULL)
	{
		return -1;
	}
	lenw=write(fd, pData, len);
	BLELOG("111write data is %d\n",lenw);	
	for(int i =0;i<lenw;i++)
	{
		BLELOG("car data is %02x\n",pData[i]);
	}
}


int bleUart::SendReqConnectStatusToBLE(void)
{
	packbleProtocolData(0x84, 0x04, NULL, 0, 0);
}

int bleUart::SendCarInfoToBLE(void)
{		
	uint16_t dataLen = 0;
	unsigned int checkCode = 0;

	uint8_t *PayloadBuff = NULL;
	unsigned char *pos = NULL;

	PayloadBuff = (uint8_t *)malloc(MALLOC_DATA_LEN);
	if(PayloadBuff == NULL)
	{
		FAWACPLOG("malloc PayloadBuff error!");
		return -1;
	}
	memset(PayloadBuff, 0, MALLOC_DATA_LEN);
	pos = PayloadBuff;
	
	*pos++ = 0x7e;
	//data length
	*pos++ = 0;
	*pos++ = 0;
	//cmd
	*pos++ = 0x14;
	//serial NO.
	*pos++ = 0;
	*pos++ = 0;

	//data property: the data which be send has been encrypted
	*pos++ = 0;
	*pos++ = 0;

	 dataLen = CFAWACP::GetInstance()->PackBLEPeriodicCarInfoData(pos,MALLOC_DATA_LEN - 7);

	 //printf("data len is %d \r\n",dataLen);
	 PayloadBuff[1] = (dataLen & 0xff00)>>8;
	 PayloadBuff[2] = (dataLen & 0xff)>>0;

	 pos = (pos + dataLen);
	 
	 checkCode = Crc16Check(&PayloadBuff[1], dataLen + 7);
	// printf("checkCode  is %x \r\n",checkCode);

	*pos++ = (checkCode & 0xff00)>>8;
	*pos++ = (checkCode & 0xff)>>0;

	*pos++ = 0x7e;

	 escape_bleUart_data(PayloadBuff,dataLen + 11);
	
}

/*****************************************************************************
* Function Name : calculate_files_CRC
* Description   : calculat
* Input			: None
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.05.22
*****************************************************************************/
int bleUart::calculate_files_CRC(char *fileName, uint32_t *crc)
{
	int nRead;
	uint8_t buff[1024];
	int fd = 0;
	int isFirst = 0;

	fd = open(fileName, O_RDONLY);
	if (fd < 0)
	{
		BLELOG("Open file:%s error.\n", fileName);
		return -1;
	}else
		BLELOG("Open file:%s success.\n", fileName);

	memset(buff, 0, sizeof(buff));

	while(1)
	{
		if((nRead = read(fd, buff, sizeof(buff))) > 0)
		{
			if(isFirst == 0)
			{
				*crc = crc32Check(0xFFFFFFFF, buff, nRead);
				isFirst = 1;
			}
			else
			{
				*crc = crc32Check(*crc, buff, nRead);
			}
		}
		else
		{
			isFirst = 0;
			break;
		}
	}
	BLELOG("crc test 1126\n");
	/*while ((nRead = read(fd, buff, sizeof(buff))) > 0)
	{
		*crc = crc32Check(buff, nRead);
	}*/

	close(fd);

	return 0;
}

void bleUart::cb_bleRemoteConfigInfo()
{
	packbleProtocolData(0x82, 0x01, NULL, 0, 0);
}

int bleUart::cb_bleRemoteCtrlCmd()
{
	int totalDataLen;
	unsigned char *pos = NULL;
	unsigned short dataLen, headLen;
	unsigned int checkCode;
		
	int SubitemTotal = CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemTotal;
	static unsigned short serialNO = 0;
	unsigned char *pBuff = (unsigned char *)malloc(BT_BUFF_LEN);
	if(pBuff == NULL)
	{
		printf("%s(%d): pbuff = null\n", __func__, __LINE__);
		return -1;
	}
	memset(pBuff, 0, BT_BUFF_LEN);
	pos = pBuff;
	printf("===========Begin Remote Ctrl seccess ===========!!\n");
	*pos++ = 0x7e;
	//data length
	*pos++ = 0;
	*pos++ = 0;
	//cmd
	*pos++ = 0x84;
	//serial NO.
	*pos++ = (serialNO & 0xff00) >> 8;
	*pos++ = (serialNO & 0xff) >> 0;

	//data property: the data which be send has been encrypted
	*pos++ = 0;
	*pos++ = 0;

	headLen = pos-pBuff;
	BLELOG("headLen = %d++%d++%d\n", headLen,CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[0],CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[0]);
	for(int i = 0; i < SubitemTotal; i++)
	{
		switch(CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemCode[i])
	{
		case CheckBleState_ID:
		if(CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[i]==0)
		{
			*pos++ = 0x04;
		}	
			break;
		case BroadCase_ID:
			if(CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[i]==0)
		{
			*pos++ = 0x02;
		}
		else if(CFAWACP::GetInstance()->m_AcpRemoteCtrlList.SubitemValue[i]==1)
		{
			*pos++ = 0x01;
		}
			break;
		case SendPW_ID:
			*pos++ = 0x03;
			memcpy(pos,p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW,6);
			pos+=6;
			break;


		case SleepTime_ID:
			*pos++ = 0x05;
			break;
		case SleepNow_ID:
			*pos++ = 0x06;
			break;
		
		default:
				goto NONSUPPORT_FUNCTION;
			break;
	}
		//*pos++ = pthis->m_AcpRemoteCtrlList.SubitemValue[i];
	}

	serialNO++;
		dataLen = (unsigned short)(pos-pBuff) - headLen;
	BLELOG("dataLen = %d\n",dataLen);

		pBuff[1] = (dataLen & 0xff00)>>8;
		pBuff[2] = (dataLen & 0xff)>>0;

		//Calculated check code
		checkCode = m_bleUart->Crc16Check(&pBuff[1], pos-pBuff-1);
		//MCULOG("checkCode = %0x\n", checkCode);

		*pos++ = (checkCode & 0xff00)>>8;
		*pos++ = (checkCode & 0xff)>>0;

		*pos++ = 0x7e;

		totalDataLen = pos-pBuff;
	BLELOG("totalDataLen =%d", totalDataLen);

		BLELOG("\n\n\nBefore escape Data:");
		for(int i = 0; i < totalDataLen; i++)
			BLE_NO("%02x ", *(pBuff+i));
		BLELOG("\n");
		//if(bleupgrade == false)
		//{
			if(m_bleUart->escape_bleUart_data(pBuff, totalDataLen) == -1)
			{
				MCULOG();
				return -1;
			}
			BLELOG("===========Send Remote Ctrl seccess ===========!!\n");
		//}
		
NONSUPPORT_FUNCTION:
	if (serialNO > 65534)
		serialNO = 0;

	if (pBuff != NULL)
	{
		free(pBuff);
		pBuff = NULL;
	}
	return 0;
}


int bleUart::unpack_bleRemoteCtrl(unsigned char *pData, uint16_t datalen)
{
	unsigned char *pos = pData;
	uint8_t SubitemCode;
	uint8_t SubitemCodeParam;
	int RemoteCtrlStatus = 0;

	SubitemCode = pData[8];
	SubitemCodeParam = pData[9];
	BLELOG("===============ble Remote Ctrl Msg===============\n");
	BLELOG("ble SubitemCode == %d, SubitemCodeParam == %d\n",SubitemCode, SubitemCodeParam);
	for(int8_t i = 0; i < datalen; i++)
		BLE_NO("%02x ", pData[i]);
	BLELOG("\n\n\n");
	pos+=9;
	switch(SubitemCode)
	{
		case 1:
			p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleBKState = SubitemCodeParam;
			//packbleProtocolData(0x84,0x06,NULL,1,0);
			//sleep(1);
			//packbleProtocolData(0x84,0x07,NULL,1,0);
			break;
		case 2:
			p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleBKState = SubitemCodeParam;
			//packbleProtocolData(0x84,0x01,NULL,1,0);
			break;
		case 3:
			p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePWState = SubitemCodeParam;
			break;
		case 4:
			memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleMAC, 0, 6);
			memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleMAC, pos, 6);
			
			pos+=6;
			p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleWorkState = *pos++;
			p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleConnectState = *pos++;

			m_BleConnectStatus = p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleConnectState ;
			m_u8BleUpdateConnectTimeout = 0;						
			memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW, 0, 6);
			memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BlePW, pos, 6);
			pos+=6;
			memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleBKName, 0, 8);
			memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleBKName, pos, 8);
			pos+=8;
			//memset(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion, 0, 12);
			memcpy(p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion, pos, 12);
			pos+=12;
			BLELOG("BLE version is %s\n", p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion);
			//printf("jason add BLE version is %s\r\n", p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleVersion);

			break;
		case 5:
			p_FAWACPInfo_Handle->RemoteControlData.VehicleBody.BleBKState = SubitemCodeParam;
			break;
		default:
			break;
	}
	CFAWACP::GetInstance()->TspRemoteCtrlRes();

	return 0;
}




//============================================= ble upgrade ======================================================
#if 1
int bleUart::pack_bleUart_upgrade_data(unsigned char cmd, bool isReceivedDataCorrect, int mcuOrPlcFlag)
{
	int i,headLen;
	int totalDataLen;
	unsigned char *pos = NULL;
	unsigned short int dataLen;
	unsigned int checkCode;
	
	static unsigned short int serialNo = 0;
	unsigned char *pData = (unsigned char *)malloc(BLE_UPGRADE_BUFF);
	if(pData == NULL)
		return -1;
    memset(pData, 0, BLE_UPGRADE_BUFF);
	
	BLELOG("bleOrPlcFlag = %d\n",mcuOrPlcFlag);
	pos = pData;
	
	*pos++ = 0x7e;
	//data length
	*pos++ = 0;
	*pos++ = 0;
	//cmd
	*pos++ = cmd;
	//serial NO.
	*pos++ = (serialNo & 0xff00) >> 8;
	*pos++ = (serialNo & 0xff) >> 0;
	//data property: the data which be send has been encrypted
	*pos++ = 0;
	*pos++ = 0;

	headLen = pos-pData;
	BLELOG("headLen = %d\n", headLen);
	
	switch (cmd)
	{
		case 0x8a:
			dataLen = pack_bleupgrade_cmd(pData, headLen, mcuOrPlcFlag);
			break;
		case 0x86:
			dataLen = pack_bleupgrade_data(pData, headLen, isReceivedDataCorrect, mcuOrPlcFlag);
			break;
		case 0x0a:
			system(RM_BLE_FILE);
			break;
		default:
			MCULOG("The cmd error!\n");
			break;
	}
	
	BLELOG("dataLen = %d\n",dataLen);

	pData[1] = (dataLen & 0xff00)>>8;
	pData[2] = (dataLen & 0xff)>>0;

	pos += dataLen;
	
	//Calculated check code
	checkCode = Crc16Check(&pData[1], pos-pData-1);
	BLELOG("checkCode = %0x\n", checkCode);
	
	*pos++ = (checkCode & 0xff00)>>8;
	*pos++ = (checkCode & 0xff)>>0;

	*pos++ = 0x7e;

	totalDataLen = pos-pData;
	BLELOG("totalDataLen =%d", totalDataLen);

	BLELOG("Before escape Data:");
	for(i = 0; i < totalDataLen; i++)
		BLE_NO("%02x ", *(pData+i));
	BLE_NO("\n");
	
	escape_bleUart_data(pData, totalDataLen);

    serialNo++;
    if (serialNo > 10000)
        serialNo = 0;

	if (pData != NULL)
	{
		free(pData);
		pData = NULL;
	}

    return 0;
}

unsigned short int bleUart::pack_bleupgrade_cmd(unsigned char *pData, int len, int mcuOrPlcFlag)
{
	unsigned char fileNameLen;
	unsigned char *pos = NULL;

	if(len != 8)
		return -1;

	pos = pData;
	pos += len;

	if(mcuOrPlcFlag == 0)
	{
		fileNameLen = strlen(BLE_FILE_NAME);
		BLELOG("fileNameLen:%d\n", fileNameLen);
	
		*pos++ = 0x00;
		memcpy(pos, upgradeInfo.bleVersionSize, 4);
		pos += 4;
		//Check code
		memcpy(pos, upgradeInfo.bleVersionCrc16, 2);
		pos += 2;
		//MCU upgrade file name length
		*pos++ = fileNameLen;
		memcpy(pos, BLE_FILE_NAME, fileNameLen);
		pos+= fileNameLen;
		//MCU version length
		*pos++ = 0x04;
		//MCU version
		memcpy(pos, upgradeInfo.newbleVersion, 4);
		pos += 4;
	}
	else if(mcuOrPlcFlag == 1)
	{
		fileNameLen = strlen(BLE_FILE_NAME);
		BLELOG("fileNameLen:%d\n", fileNameLen);
	
		*pos++ = 0x01;
		/*memcpy(pos, upgradeInfo.bleVersionSize, 4);
		pos += 4;*/
		*pos++ = 0xff;
		*pos++ = 0xff;
		*pos++ = 0xff;
		*pos++ = 0xff;
		//Check code
		memcpy(pos, upgradeInfo.bleVersionCrc16, 2);
		pos += 2;
		//MCU upgrade file name length
		*pos++ = fileNameLen;
		memcpy(pos, BLE_FILE_NAME, fileNameLen);
		pos+= fileNameLen;
		//MCU version length
		*pos++ = 0x04;
		//MCU version
		memcpy(pos, upgradeInfo.newbleVersion, 4);
		pos += 4;
	}
	
	//Calculated data length
	BLELOG("Pack data len:%d\n",(int)(pos-pData-len));
	
	return (unsigned short int)(pos-pData-len);
}

unsigned short int bleUart::pack_bleupgrade_data(unsigned char *pData, int len, bool isReceivedDataCorrect, int mcuOrPlcFlag)
{
	int nRead;
	unsigned char *pos = NULL;
	unsigned int offsetAddr;
	unsigned short int packetLen;
	unsigned char buff[BLE_UPGRADE_BUFF] = {0};
	int fileFd = -1;

	if(len != 8)
		return -1;

	pos = pData;
	pos += len;

	if(isReceivedDataCorrect == true)
	{
		*pos++ = 0x00;
		memcpy(pos, dataPacketOffsetAddr, 4);
		pos += 4;
		memcpy(pos, dataPacketLen, 2);
		pos += 2;
		
		//get plc upgrade data's offset addr and len
		offsetAddr	= (dataPacketOffsetAddr[0]) << 24;
		offsetAddr += (dataPacketOffsetAddr[1]) << 16;
		offsetAddr += (dataPacketOffsetAddr[2]) << 8;
		offsetAddr += (dataPacketOffsetAddr[3]) << 0;
		BLELOG("offsetAddr = %d\n", offsetAddr);
		
		packetLen  = (dataPacketLen[0]) << 8;
		packetLen += (dataPacketLen[1]) << 0;
		BLELOG("packetLen = %d\n", packetLen);
		
		if(mcuOrPlcFlag == 0)
		{
			fileFd = open(BLE_FILE_NAME, O_RDONLY);
			if (fileFd < 0)
			{
				BLELOG("Open file:%s error.\n", BLE_FILE_NAME);
				return -1;
			}

			lseek(fileFd, offsetAddr, SEEK_SET);

			memset(buff, 0, BLE_UPGRADE_BUFF);
			if((nRead = read(fileFd, buff, packetLen)) != -1)
			{
				BLELOG("read file %s, nRead = %d\n", BLE_FILE_NAME, nRead);
				memcpy(pos, buff, nRead);
				pos += nRead;
			}
			
			close(fileFd);
		}
		
	}
	else
	{
		*pos++ = 0x01;
		BLELOG("Can received mcu respond!\n");
	}
	
	BLELOG("Pack data len:%d\n",(int)(pos-pData-len));
	return (unsigned short int)(pos-pData-len);
}

void bleUart::ble_apply_for_data(unsigned char* pData, uint32_t len)
{
	int i;
	unsigned int addr;
	unsigned short int packetLen; 
	unsigned char *pos = pData;
	unsigned int verionLength;
	unsigned int fileDatalen;
	unsigned char fileLen;
	
	fileLen = *(pos+8);
	BLELOG("fileLen:%d\n", fileLen);

	memset(fileName, 0, 64);
	memcpy(fileName, pos+8+1, fileLen);
	BLELOG("fileName:%s\n", fileName);

	//get plc upgrade data's addr and len
	addr  = *(pos+8+1+fileLen) << 24;
	addr += *(pos+8+1+fileLen+1) << 16;
	addr += *(pos+8+1+fileLen+2) << 8;
	addr += *(pos+8+1+fileLen+3) << 0;
	BLELOG("offsetAddr = %d\n", addr);
	
	packetLen  = *(pos+8+1+fileLen+4) << 8;
	packetLen += *(pos+8+1+fileLen+5) << 0;
	BLELOG("packetLen = %d\n", packetLen);

	fileDatalen = addr + (unsigned int)packetLen;
	
	if(strcmp((char*)fileName, BLE_FILE_NAME) == 0)
	{
		upgradeInfo.upgradeFlag = 0;
		verionLength  = (upgradeInfo.bleVersionSize[0]) << 24;
		verionLength += (upgradeInfo.bleVersionSize[1]) << 16;
		verionLength += (upgradeInfo.bleVersionSize[2]) << 8;
		verionLength += (upgradeInfo.bleVersionSize[3]) << 0;
		
		len = strlen(BLE_FILE_NAME); 
		printf("len:%d\n", len);
		
		if((fileLen == len) && (memcmp(BLE_FILE_NAME, fileName, len) == 0) && \
			(fileDatalen <= verionLength))
		{
			BLELOG("Data offset addr and length:\n");
			memcpy(dataPacketOffsetAddr, pos+8+1+fileLen, 4);
			
			for(i = 0; i<4; i++)
				BLELOG("%02x ",*(dataPacketOffsetAddr+i));
			BLELOG("\n");
			
			memcpy(dataPacketLen, pos+8+1+fileLen+4, 2);
			
			for(i = 0; i<2; i++)
				BLELOG("%02x ",*(dataPacketLen+i));
			BLELOG("\n");

			pack_bleUart_upgrade_data(0x86, true, 0);	
		}
		else
		{

			pack_bleUart_upgrade_data(0x86, false, 0);

		}
	}
	
}


#endif

static const unsigned int crc32tab[] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
	0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
	0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
	0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
	0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
	0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
	0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
	0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
	0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
	0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
	0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
	0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
	0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
	0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
	0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
	0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
	0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
	0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
	0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
	0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
	0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
	0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
	0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
	0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
	0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
	0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
	0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
	0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
	0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
	0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
	0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
	0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
	0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
	0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
	0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
	0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
	0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
	0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
	0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
	0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
	0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
	0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
	0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
	0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
	0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

#if 0
unsigned int bleUart::crc32Check(unsigned char *buff, unsigned int size)
{
    unsigned int i, crc;
    crc = 0xFFFFFFFF;

    for (i = 0; i < size; i++)
        crc = crc32tab[(crc ^ buff[i]) &0xff] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFF;
}
#endif

#if 1
unsigned int bleUart::crc32Check(unsigned int crc, unsigned char *buff, unsigned int size)
{
    unsigned int i;

    for (i = 0; i < size; i++)
        crc = crc32tab[(crc ^ buff[i]) &0xff] ^ (crc >> 8);

    return crc;
}

#endif

