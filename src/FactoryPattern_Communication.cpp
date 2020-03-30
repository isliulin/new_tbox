#include "FactoryPattern_Communication.h"


#include <fstream>
#include <iostream>
#include <string>
using namespace std;

extern McuStatus_t McuStatus;
uint8_t	mCurrentTboxPattern;	//TBox当前运行模式
TBox_Detecting_ST tboxDetectingPattern;

TBox_Config_ST ConfigShow;

bool testecall = false;
bool testblestate = false;
bool testnfcstate = false;
bool testfac = false;
bool testAPN = false;

char Detecting_Ecall[20];
bool APN2test = false;


#if 1

#define SYSFS_GPIO_EXPORT         "/sys/class/gpio/export"
#define SYSFS_GPIO_4G_LED         "40"
#define SYSFS_GPIO_WIFI_LED       "41"
#define SYSFS_GPIO_OUTPUT_VAL     "OUT"
#define SYSFS_GPIO_INPUT_VAL      "IN"
#define SYSFS_GPIO_4G_DIR         "/sys/class/gpio/gpio40/direction"
#define SYSFS_GPIO_WIFI_DIR       "/sys/class/gpio/gpio41/direction"
#define SYSFS_GPIO_4G_VAL         "/sys/class/gpio/gpio40/value"
#define SYSFS_GPIO_WIFI_VAL       "/sys/class/gpio/gpio41/value"
#define SYSFS_GPIO_VAL_H          "1"
#define SYSFS_GPIO_VAL_L          "0"

#define LTE_LED_NUM 	(0)
#define WIFI_LED_NUM	(1)


static int lteFd, wifiFd;
//static bool isSleepOn = false;



/*****************************************************************************
* Function Name : tbox_gpio_config
* Description   : GPIO配置
* Input			: None
* Output        : None
* Return        : 0:success, -1:failed
* Auther        : ygg

* Date         : 2018.01.18

*****************************************************************************/
int tbox_gpio_config()
{
	int fd;
    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    if (fd == -1)
    {
        printf("ERR: export pin open error.\n");
        return -1;
    }
    write(fd, SYSFS_GPIO_4G_LED, sizeof(SYSFS_GPIO_4G_LED));
    write(fd, SYSFS_GPIO_WIFI_LED, sizeof(SYSFS_GPIO_WIFI_LED));
    close(fd);

	fd = open(SYSFS_GPIO_4G_DIR, O_WRONLY);
    if (fd == -1)
    {
        printf("ERR: Config 4g pin direction open error.\n");
        return -1;
    }
    write(fd, SYSFS_GPIO_INPUT_VAL, sizeof(SYSFS_GPIO_INPUT_VAL));
    close(fd);

	fd = open(SYSFS_GPIO_WIFI_DIR, O_WRONLY);
    if (fd == -1)
    {
        printf("ERR: Config wifi pin direction open error.\n");
        return -1;
    }
    write(fd, SYSFS_GPIO_INPUT_VAL, sizeof(SYSFS_GPIO_INPUT_VAL));
    close(fd);

	return 0;
}

/*****************************************************************************
* Function Name : tbox_gpio_init
* Description   : GPIO初始化函数
* Input			: None
* Output        : None
* Return        : 0:success, -1:failed
* Auther        : ygg

* Date         : 2018.01.18

*****************************************************************************/
int tbox_gpio_init()
{
	tbox_gpio_config();
	unsigned char lte[12] = {0};
	unsigned char wifi[12] = {0};
    lteFd = open(SYSFS_GPIO_4G_VAL, O_RDWR);
    if (lteFd == -1)
    {
        printf("ERR: 4g value open error.\n");
        return -1;
    }
	read(lteFd, lte, 11);

	wifiFd = open(SYSFS_GPIO_WIFI_VAL, O_RDWR);
    if (wifiFd == -1)
    {
        printf("ERR: wifi pin value open error.\n");
        return -1;
    }
	read(wifiFd, wifi, 11);

	printf("hahah %d  %d\n",lte[0],wifi[0]);
	if(lte[0] == '0')
	{
		printf("000000\n");
		if(wifi[0] == '0')
		{
			memcpy(p_FAWACPInfo_Handle->VehicleCondData.Hardversion,"HWV0.00",7);
		}
		else if(wifi[0] == '1')
		{
			memcpy(p_FAWACPInfo_Handle->VehicleCondData.Hardversion,"HWV0.01",7);
		}
	}
	else if(lte[0] == '1')
	{
		if(wifi[0] == '0')
		{
			memcpy(p_FAWACPInfo_Handle->VehicleCondData.Hardversion,"HWV0.10",7);
		}
		else if(wifi[0] == '1')
		{
			memcpy(p_FAWACPInfo_Handle->VehicleCondData.Hardversion,"HWV0.11",7);
		}
	}
	
	printf("hw version is %s\n",p_FAWACPInfo_Handle->VehicleCondData.Hardversion);
	

	return 0;
}
#endif


FactoryPattern_Communication::FactoryPattern_Communication()
{
    sockfd = -1;
    accept_fd = -1;
	memset(&tboxDetectingPattern, 0, sizeof(tboxDetectingPattern));
}
FactoryPattern_Communication::~FactoryPattern_Communication()
{
	close(sockfd);
}

int FactoryPattern_Communication::FactoryPattern_Communication_Init()
{
	int epoll_fd;
	struct epoll_event events[MAX_EVENT_NUMBER];

	////tbox_gpio_init();
	pfawacp =  CFAWACP::GetInstance();
	m_CMcu = CMcu::GetInstance();
	m_bleUart = bleUart::Get_BleInstance();
	
	//pfawacp->updateTBoxParameterInfo();
	pfawacp->readTBoxParameterInfo();

	//system("echo 1 > /sys/class/android_usb/android0/enable");

    while(1)
   {
	    if(socketConnect() == 0)
	    {
	    	FACTORYPATTERN_LOG("create socket ok!\n");
		
		    epoll_fd = epoll_create(5);
		    if(epoll_fd == -1)
		    {
		        printf("FactoryPattern: fail to create epoll!\n");
		        close(sockfd);
		        sleep(1);
		    }
		    else
		    {
		    	break;
		    }
	    }
	    sleep(1);
    }

    add_socketFd(epoll_fd, sockfd);

	while(1)
	{
		int num = epoll_wait(epoll_fd, events, MAX_EVENT_NUMBER, -1);
		if(num < 0)
		{
			printf("FactoryPattern: epoll failure!\n");
			break;
		}
		et_process(events, num, epoll_fd, sockfd);
	}
	
    close(sockfd);

	return 0;
}

int FactoryPattern_Communication::set_non_block(int fd)
{
    int old_flag = fcntl(fd, F_GETFL);
    if(old_flag < 0)
    {
        perror("fcntl");
        return -1;
    }
    if(fcntl(fd, F_SETFL, old_flag | O_NONBLOCK) < 0)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}

void FactoryPattern_Communication::add_socketFd(int epoll_fd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if(set_non_block(fd) == 0)
    	FACTORYPATTERN_LOG("set_non_block ok!\n");
}

int FactoryPattern_Communication::socketConnect()
{
    int opt = 1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        FACTORYPATTERN_ERROR("socket error!");
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(FACTORYPATTERN_SERVER);
    serverAddr.sin_port = htons(FACTORYPATTERN_PORT);

    if(bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        FACTORYPATTERN_ERROR("bind failed.\n");
        close(sockfd);
        return -1;
    }

    if(listen(sockfd, LISTEN_BACKLOG) == -1)
    {
        FACTORYPATTERN_ERROR("listen failed.\n");
        close(sockfd);
        return -1;
    }

    return 0;
}

void FactoryPattern_Communication::et_process(struct epoll_event *events, int number, int epoll_fd, int socketfd)
{
    int i;
    int dataLen;
    uint8_t buff[BUFFER_SIZE] = {0};

    for(i = 0; i < number; i++)
    {
        if(events[i].data.fd == socketfd)
        {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int connfd = accept(socketfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
            add_socketFd(epoll_fd, connfd);
			#if 0
				ofstream testrcv;
   				testrcv.open("/data/0808test.txt", ios::app); 
				testrcv <<"connect ok"  << endl;
				testrcv.close();
			#endif

        }
        else if(events[i].events & EPOLLIN)
        {
            FACTORYPATTERN_LOG("et mode: event trigger once!\n");
            memset(buff, 0, BUFFER_SIZE);
			
            dataLen = recv(events[i].data.fd, buff, BUFFER_SIZE, 0);
            accept_fd = events[i].data.fd;
			FACTORYPATTERN_LOG("FACTORYPATTERN_LOG recv : dataLen = %d\n", dataLen);
            if(dataLen <= 0)
            {	

				#if 0
				ofstream testrcv;
   				testrcv.open("/data/0808test.txt", ios::app); 
				testrcv <<"recv data less 0"  << endl;
				testrcv.close();

				uint8_t *dataBuff = (uint8_t *)malloc(BUFFER_SIZE);
				if(dataBuff == NULL)
				{
					FACTORYPATTERN_LOG("malloc dataBuff error!");
					return 1;
				}
				memset(dataBuff, 0, BUFFER_SIZE);
				uint8_t *pos = dataBuff;

				//protocol header
				*pos++ = (MSG_HEADER_T_ID >>8) & 0xFF;
				*pos++ = (MSG_HEADER_T_ID >>0) & 0xFF;

				//datalen
				*pos++=0x01;
				*pos++=0x01;

				//
				*pos++=0x01;

				//crc
				*pos++=0x01;
				*pos++=0x01;

				//end
				*pos++=0x0A;
				
	
				int length;
				if((length = send(accept_fd, dataBuff, 8, 0)) < 0)
				{
					close(accept_fd);
				}
				else
				{
					FACTORYPATTERN_LOG("Send data ok,length:%d\n", length);

				}

				if(dataBuff != NULL)
				{
					free(dataBuff);
					dataBuff = NULL;
				}
				#endif



				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL); 
                close(events[i].data.fd);
            }
            else
            {
                FACTORYPATTERN_LOG("FactoryPattern Recevied data len %d:\n", dataLen);
				//for(i = 0; i < dataLen; ++i)
				//	FACTORYPATTERN_LOG("%02x ", *(buff + i));
				//FACTORYPATTERN_LOG("\n\n");	

				#if 0

				ofstream testrcv;
   				testrcv.open("/data/0808test.txt", ios::app); 
				testrcv <<"receive datalen  "  << dataLen << endl;
				/*
				for(i = 0; i < dataLen; ++i)
				{
					
					testrcv << *(buff + i) << endl;
					FACTORYPATTERN("%02x ", *(buff + i));
					system("echo *(buff + i) > /data/log0808.txt");
				}
				testrcv.close();

				int fd = open("/data/log0808.txt", O_RDWR|O_CREAT);

  				write(fd, buff, strlen(buff));
  				close(fd);
				system("sync");*/



				uint8_t *dataBuff = (uint8_t *)malloc(BUFFER_SIZE);
				if(dataBuff == NULL)
				{
					FACTORYPATTERN_LOG("malloc dataBuff error!");
					return 1;
				}
				memset(dataBuff, 0, BUFFER_SIZE);
				uint8_t *pos = dataBuff;

				//protocol header
				*pos++ = (MSG_HEADER_T_ID >>8) & 0xFF;
				*pos++ = (MSG_HEADER_T_ID >>0) & 0xFF;

				//datalen
				*pos++=0x00;
				*pos++=0x00;

				//
				*pos++=0x01;

				//crc
				*pos++=0x00;
				*pos++=0x00;

				//end
				*pos++=0x0A;
				
	
				int length;
				if((length = send(accept_fd, dataBuff, 8, 0)) < 0)
				{
					ofstream testrcv;
   					testrcv.open("/data/0808test.txt", ios::app); 
					testrcv <<"send data fail"  << endl;
					testrcv.close();
					close(accept_fd);
				}
				else
				{
					ofstream testrcv;
   					testrcv.open("/data/0808test.txt", ios::app); 
					testrcv <<"send data ok length is" <<length << endl;
					testrcv.close();
					FACTORYPATTERN_LOG("Send data ok,length:%d\n", length);
				}

				if(dataBuff != NULL)
				{
					free(dataBuff);
					dataBuff = NULL;
				}
				#endif


	            //if(checkSum(buff, dataLen))
	            {
	                unpack_Protocol_Analysis(buff, dataLen);
	            }
            }
           
        }
        else
        {
            printf("something unexpected happened!\n");
        }
    }
}

uint8_t FactoryPattern_Communication::checkSum(uint8_t *pData, int len)
{
	unsigned int u16InitCrc = 0xffff;
	unsigned int i;
	unsigned char j;
	unsigned char u8ShitBit;
	uint8_t *pos = pData+2;
	uint16_t crcDatalen = ((pos[0] << 8) & 0xFF00) | (pos[1] & 0x00FF);
	uint16_t crc16 = ((pos[len-3] << 8) & 0xFF00) | (pos[len-2] & 0x00FF);

	FACTORYPATTERN("FACTORYPATTERN: crcDatalen = %d, crc16 = %d,", crcDatalen, crc16);
		
	for(i = 0; i < crcDatalen; i++)
	{		
		u16InitCrc ^= pos[i];
		for(j=0; j<8; j++)
		{
			u8ShitBit = u16InitCrc&0x01;
			u16InitCrc >>= 1;
			if(u8ShitBit != 0)
			{
				u16InitCrc ^= 0xa001;
			}		
		}
	}
	if(crc16 == u16InitCrc)
		return 0;
	else
		return 1;
}


uint8_t FactoryPattern_Communication::checkSum_BCC(uint8_t *pData, uint16_t len)
{
	uint8_t *pos = pData;
	uint8_t checkSum = *pos++;
	
	for(uint16_t i = 0; i<(len-1); i++)
	{
		checkSum ^= *pos++;
	}

	return checkSum;
}
//切换生产配置测试模式
uint8_t FactoryPattern_Communication::unpack_ChangeTboxMode(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	uint8_t nCurMode = pos[0];


	mCurrentTboxPattern = nCurMode;

	//kill other thread
	pack_hande_data_and_send( MSG_COMMAND_ModeID, 0);

	return 0;
}


//生产配置
uint8_t FactoryPattern_Communication::unpack_Analysis_Config(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	uint8_t TAG_ID = pos[0];	
	int Datalen = (*(pos+1)<<8) + *(pos+2);
	pos += 3;

	TBox_Config_ST tboxConfigST;

	//tboxConfigST.TBoxSerialNumber_Len = *pos++;
	//memcpy(tboxConfigST.TBoxSerialNumber, pos, 30);	
	//pos+=30;

	/*tboxConfigST.ServerDomain_Len = *pos++;
	memcpy(tboxConfigST.ServerDomain, pos, 30);	
	pos+=30;

	tboxConfigST.Port_Len = *pos++;
	tboxConfigST.Port=(pos[0] << 8) + pos[1];
	pos+=2;

	tboxConfigST.APN1_Len = *pos++;
	memcpy(tboxConfigST.APN1, pos, 22);	
	pos += 22;

	tboxConfigST.APN2_Len = *pos++;
	memcpy(tboxConfigST.APN2, pos, 22);	
	pos += 22;


	tboxConfigST.TBoxVIN_Len 	 = *pos++;
	memcpy(tboxConfigST.Tbox_VIN, pos, 17);	
	pos += 17;*/

	//tboxConfigST.VehicleType_Len = *pos++;
	//tboxConfigST.VehicleType = *pos++;

	/*tboxConfigST.SK_Len = *pos++;
	memcpy(tboxConfigST.SK, pos, 6);	
	pos += 6;

	tboxConfigST.PowerDomain_Len = *pos++;
	tboxConfigST.PowerDomain 	 = *pos++;

	tboxConfigST.RootKey_Len 	 = *pos++;
	memcpy(tboxConfigST.RootKey, pos, 32);	
	pos += 32;*/

	
	
	
	//tboxConfigST.SupplierSN_Len= *pos++;	//SN长度
	//memcpy(tboxConfigST.SupplierSN, pos, 11);//供应商序列号SN
	//pos += 11;

	
	//写入文件
	/*int fd = open(Pattern_FILE, O_RDWR|O_CREAT);
    if ( -1 == fd)
    {
		printf("updatePara open file failed\n");
        return -1;
    }
    
    if ( -1 == write(fd, &tboxConfigST, sizeof(TBox_Config_ST)))
    {
		printf("updatePara write file error!\n");
		close(fd);
        return -1;
    }
    close(fd);
	system("sync");

	
	int fd2;
	int retval;
	
	fd2 = open(Pattern_FILE, O_RDONLY, 0666);
	if(fd2 < 0)
	{
		return -1;
	}
	else
	{
		retval = read(fd2, &ConfigShow, sizeof(TBox_Config_ST));
		close(fd2);
	}*/

	//m_CMcu->packProtocolData(0x82,0x52,NULL,1,0);

	//tboxConfigST.TBoxVIN_Len 	 = *pos++;
	memcpy(tboxConfigST.Tbox_VIN, pos, 17);	
	m_CMcu->packProtocolData(0x82,0x53,tboxConfigST.Tbox_VIN,1,0);
	pos += 4;


	memcpy(tboxConfigST.Tbox_CARNAME, pos, 2);
	tboxConfigST.Carid = tboxConfigST.Tbox_CARNAME[1] - '0';
	m_CMcu->packProtocolData(0x82,0x60,&tboxConfigST.Carid,1,0);
	pos += 5;


	memcpy(ConfigShow.Tbox_BLENAME, pos, 8);	
	m_bleUart->packbleProtocolData(0x84,0x05,NULL,1,0);
	sleep(1);
	m_bleUart->packbleProtocolData(0x84,0x01,NULL,1,0);
	pos += 8;
	pos++;

	pos+=3;
	
	pos++;
	pos++;
	if(*pos == '1')
	{
	  #if 0
		char buff[1024];
		FILE *fp;
		for(int i =0;i<3;i++)
		{
			fp = popen("ping -c 2 -i 10 vtbox.i-morefun.com","r");
			fread(buff, sizeof(char), sizeof(buff), fp);
			string str(buff);
			std::size_t found = str.find("time=");
			if (found!=std::string::npos)
			{
				testAPN = true;
				pclose(fp);
				break;
				//system("echo 1 > /data/true.log");
			}
			else
			{
				testAPN = false;
				pclose(fp);
				sleep(5);
				//system("echo 1 > /data/false.log");
			}
			
		}
		#else //// jason.sun add 
		for(int i =0;i<60;i++)
		{
		
			//if(pfawacp->m_loginState != STATUS_LOGGIN_FINISH)
			if(tboxInfo.networkStatus.isLteNetworkAvailable != NETWORK_LTE)
			{
			}
			else
			{
				testAPN = true;
				break;
			}
			sleep(1);
		}
		#endif
	}
	else
	{
		
	}


	//设置蓝牙广播名字
	//m_CMcu->packProtocolData(0x82,0x54,NULL,1,0);
	//m_CMcu->packProtocolData(0x82,0x55,NULL,1,0);

	//set up datapool
	#if 0
	//dataPool->setTboxConfigInfo(VinID, &ConfigShow.Tbox_VIN, 17);

	//需要配置rootkey

	
	uint8_t *decryptkey;
	decryptkey = (uint8_t *)malloc(256);
	decrypttest(tboxConfigST.RootKey, 32, 1,decryptkey);
	uint8_t writekey[32];
	for(int i =0;i<32;++i)
	{
		if(i>=16)
		{
			writekey[i]=decryptkey[i-16];
		}
		else
		{
			writekey[i]=decryptkey[i];
		}
	}

    writeTsp_rootkey(writekey,32);

	memcpy(p_FAWACPInfo_Handle->TBoxSerialNumber, tboxConfigST.TBoxSerialNumber, 30);
	free(decryptkey);
	decryptkey = NULL;
	#endif


	//返回数据
	return 0;
}
//生产指标项测试
uint8_t FactoryPattern_Communication::unpack_Analysis_Detecting(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	uint8_t TAG_ID = pos[0];
	uint16_t tagLen;

	
	int nVoltage_SRS 	 = 0;//检测SRS电压值
	
	
	switch(TAG_ID)
	{
	case TAGID_CONFIG:		//生产配置
		unpack_Analysis_Config(pData, len);
		break;
	case TAGID_CANSTATUS:	//Can连通状态
		//状态直接读取
		//需要测试逻辑
		
		break;
	case TAGID_CANDATA:		//Can数据
		break;
	case TAGID_SIXSENSOR:	//六轴传感器
		break;
	case TAGID_NFC:		//EMMC
		break;
	case TAGID_WIFI:		//WIFI
		//GetDetecting_Wifi(tboxDetectingPattern.WifiTrigger);
		break;
	case TAGID_BT:			//BT
		break;
	case TAGID_IVISTATUS:	//IVI
		break;
	case TAGID_APN2:		//APN2
		//APN2test = pfawacp->TestsocketConnect();  
		break;
	case TAGID_ECall:		//Ecall
		pos++;
		tagLen = (*(pos++)<<8) + *(pos++);
		memset(Detecting_Ecall, 0, sizeof(Detecting_Ecall));
		memcpy(Detecting_Ecall, pos, tagLen);
		testecall = true;
		//voiceCall(Detecting_Ecall, PHONE_TYPE_E);
		//voiceCall("13828468101", PHONE_TYPE_E);

		break;
	case TAGID_GPSOpen:		//GPS开路
		break;
	case TAGID_GPSInterrupt://GPS短路
		break;
	case TAGID_GPSSIGN:		//GPS信号
		break;
	case TAGID_MAINPOWER:   //主电电源
		break;
	case TAGID_RESERVEPOWER://备用电源
		break;
	case TAGID_ACCOFF:		//Acc off
		testfac = true;
		break;
	case TAGID_TBOXINFO:	//Tbox信息
		//GetDetecting_TboxInfo(tboxDetectingPattern.TBoxInfoTrigger);
		break;

	case TAGID_LOCKDOOR:
		Control_test(TAG_ID);	
		break;
	case TAGID_OPENDOOR:
		Control_test(TAG_ID);	
		break;
	case TAGID_POWEROFF:	
		Control_test(TAG_ID);
		break;
	case TAGID_POWERON:	
		Control_test(TAG_ID);
		break;
	case TAGID_CARFIND:	
		Control_test(TAG_ID);
		break;
	default:
		break;	
		
	}

	pack_hande_data_and_send( MSG_COMMAND_Detecting, TAG_ID);

	return 0;
}


uint16_t FactoryPattern_Communication::Control_test(uint8_t tag)
{
	uint16_t dataLen = 0;
	pfawacp->m_AcpRemoteCtrlList.SubitemTotal = 1;
	switch (tag)
	{
		case TAGID_LOCKDOOR:
			pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=1;
			pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=1;
			break;
		case TAGID_OPENDOOR:
			pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=1;
			pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=0;
			break;
		case TAGID_POWEROFF:
			pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=3;
			pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=0;
			break;
		case TAGID_POWERON:
			pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=3;
			pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=1;
			break;
		case TAGID_CARFIND:
			pfawacp->m_AcpRemoteCtrlList.SubitemCode[0]=2;
			pfawacp->m_AcpRemoteCtrlList.SubitemValue[0]=1;
			break;
		default:
			FACTORYPATTERN_LOG("cmd error");
			break;
	}
	m_CMcu->Mcu_RemoteCtrlRes(); 
	return dataLen;
}

uint8_t FactoryPattern_Communication::unpack_Protocol_Analysis(uint8_t *pData, int len)
{
	int dataLen;
	uint16_t DispatcherMsgLen;
	uint8_t *pos = pData;

	if((pData[0] == 0x55) && (pData[1] == 0xAA) && (pData[len-1] == 0x0A))
	{
		FACTORYPATTERN_LOG("FACTORYPATTERN  Analysis header&nail check ok!\n");
	}else{
		FACTORYPATTERN_LOG("FACTORYPATTERN  Analysis header&nail check fail !\n");
		return -1;
	}
	//消息体长度
	uint16_t MsgBodyLen = (*(pos+2)<<8) + *(pos+3);//((pos[2] << 8) & 0xFF00) | (pos[3] & 0x00FF);	
	//消息命令字
	uint8_t MsgCommandID = *(pos+4);
	FACTORYPATTERN_LOG("FACTORYPATTERN_LOG: msg body len = %d commandID = %d\n", MsgBodyLen, MsgCommandID);
	
	switch (MsgCommandID)
	{
		case MSG_COMMAND_ModeID://模式选择
			//m_bleUart->packbleProtocolData(0x84,0x06,NULL,1,0);
		    testfac = true;
			unpack_ChangeTboxMode(pos+5, MsgBodyLen);
			sleep(3);
			m_CMcu->packProtocolData(0x82,0x50,NULL,1,0);
			m_bleUart->packbleProtocolData(0x84,0x06,NULL,1,0);
			break;
		
		case MSG_COMMAND_Detecting:	//生产配置测试模式
			
   			
			unpack_Analysis_Detecting(pos+5, MsgBodyLen);
			break;
		default:
			FACTORYPATTERN_LOG("FACTORYPATTERN： ApplicationID error!\n");
			break;		
	}

	return 0;
}


//打包
uint8_t FactoryPattern_Communication::pack_hande_data_and_send(uint16_t MsgID, uint8_t TestTag)
{
	uint16_t dataLen;
	int length;

	uint8_t *dataBuff = (uint8_t *)malloc(BUFFER_SIZE);
	if(dataBuff == NULL)
	{
		FACTORYPATTERN_LOG("malloc dataBuff error!");
		return 1;
	}
	memset(dataBuff, 0, BUFFER_SIZE);
	uint8_t *pos = dataBuff;
	
	//protocol header
	*pos++ = (MSG_HEADER_T_ID >>8) & 0xFF;
	*pos++ = (MSG_HEADER_T_ID >>0) & 0xFF;
	
	//datalen
	*pos++=0;
	*pos++=0;
	*pos++ = MsgID;
	
	dataLen =  pack_Protocol_Data(pos, BUFFER_SIZE, MsgID, TestTag);	
	//Fill in total data length
	dataBuff[2] = (dataLen>>8) & 0xFF;
	dataBuff[3] = (dataLen>>0) & 0xFF;	

	pos+=dataLen;

	//crc
	//uint16_t crc16 = checkSum_BCC(dataBuff+2, 4);
	//*pos++ = (crc16>>8) & 0xFF;
	//*pos++ = (crc16>>0) & 0xFF;
	*pos++=0;
	*pos++=0;

	//end
	*pos++=0x0A;
	
	dataLen	= pos-dataBuff;

	
	
	if((length = send(accept_fd, dataBuff, dataLen, 0)) < 0)
	{
		close(accept_fd);
	}
	else
	{
		FACTORYPATTERN_LOG("Send data ok,length:%d\n", length);
	}

	if(dataBuff != NULL)
	{
		free(dataBuff);
		dataBuff = NULL;
	}	
	if(TestTag == 0x10)
	{
		sleep(3);
		m_CMcu->packProtocolData(0x82,0x51,NULL,1,0);
	}
	return 0;
}

uint16_t FactoryPattern_Communication::pack_Protocol_Data(uint8_t *pData, int len, uint16_t MsgID, uint8_t TestTag)
{
	uint16_t dataLen = 0;
	switch (MsgID)
	{
		case MSG_COMMAND_ModeID:
			dataLen=pack_mode_data(pData,len);
			break;
		case MSG_COMMAND_Detecting:
			dataLen = pack_Detect_data(pData,len,TestTag);
			break;
		default:
			FACTORYPATTERN_LOG("cmd error");
			break;
	}
	return dataLen;
}

uint16_t FactoryPattern_Communication::pack_mode_data(uint8_t *pData, int len)
{
	char buff[50];
	uint8_t *pos = pData;

	FILE * fdsystem;
	sleep(2);
	tbox_gpio_init();
	fdsystem = fopen("/etc/version", "r");
	memset(p_FAWACPInfo_Handle->VehicleCondData.TboxSystem,0,13);
	fgets((char*)p_FAWACPInfo_Handle->VehicleCondData.TboxSystem,13,fdsystem);

	//模式
	*pos++=mCurrentTboxPattern;
	//判断权限
	*pos++=0;
	
	//dataPool->getPara(SIM_ICCID_INFO, (void *)buff, sizeof(buff));
	//memcpy(pos,buff,20);
	//pos+=20;
	
	*pos++=tboxInfo.networkStatus.signalStrength;
	
	memset(buff, 0, sizeof(buff));
	dataPool->getPara(SIM_ICCID_INFO, (void *)buff, sizeof(buff));
	memcpy(pos,buff,20);
	pos+=20;
	*pos++ = '\0';


	memset(buff, 0, sizeof(buff));
	dataPool->getPara(IMEI_INFO, (void *)buff, sizeof(buff));
	memcpy(pos,buff,15);
	pos+=15;
	*pos++ = '\0';


	
	memcpy(pos, TBOX_4G_VERSION, 12);
	pos+=12;
	*pos++ = '\0';

	memcpy(pos, "MPU_V1.1.00900", 14);
	pos+=14;
	*pos++ = '\0';

	memcpy(pos, p_FAWACPInfo_Handle->VehicleCondData.TboxSystem, 12);
	pos+=12;
	*pos++ = '\0';

	if(p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU != 'P')
		sleep(10);
	memcpy(pos, p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU, sizeof(p_FAWACPInfo_Handle->VehicleCondData.VerTboxMCU));
	pos+=12;
	*pos++ = '\0';

	memcpy(pos, p_FAWACPInfo_Handle->VehicleCondData.Hardversion, 7);
	pos+=7;
	*pos++ = '\0';



	fflush(fdsystem);
    fclose(fdsystem);
    fdsystem = NULL;
	return (uint16_t)(pos-pData); 
}

uint16_t FactoryPattern_Communication::pack_Detect_data(uint8_t *pData, int len, uint8_t TestTag)
{
	uint16_t dataLen = 0;
	switch (TestTag)
	{
		case TAGID_CONFIG:
			dataLen=pack_Config_data(pData,len);
			break;
		case TAGID_CANSTATUS:
			dataLen=pack_CANSTATUS_data(pData,len);
			break;
		case TAGID_CANDATA:
			dataLen = pack_CANDATA_data(pData,len);
			break;
		case TAGID_SIXSENSOR:
			dataLen = pack_SIXSENSOR_data(pData,len);
			break;
		case TAGID_NFC:
			dataLen = pack_Emmc_data(pData,len);
			break;
		case TAGID_WIFI:
			dataLen = pack_WIFI_data(pData,len);
			break;
		case TAGID_BT:
			dataLen = pack_BT_data(pData,len);
			break;
		case TAGID_IVISTATUS:
			dataLen = pack_IVISTATUS_data(pData,len);
			break;
		case TAGID_APN2:
			dataLen = pack_APN2_data(pData,len);
			break;
		case TAGID_ECall:
			dataLen = pack_ECall_data(pData,len);
			break;
		case TAGID_GPSOpen:
			dataLen = pack_GPSOpen_data(pData,len);
			break;
		case TAGID_GPSInterrupt:
			dataLen = pack_GPSSHORT_data(pData,len);
			break;
		case TAGID_GPSSIGN:
			dataLen = pack_GPSSIGN_data(pData,len);
			break;
		case TAGID_MAINPOWER:   //主电电源
			dataLen = pack_MAINPOWER_data(pData,len);
			break;
		case TAGID_RESERVEPOWER://备用电源
			dataLen = pack_RESERVEPOWER_data(pData,len);
			break;
		case TAGID_ACCOFF:		//Acc off
			dataLen = pack_ACCOFF_data(pData,len);
			m_bleUart->packbleProtocolData(0x84,0x07,NULL,1,0);
			
			sleep(1);
			//m_CMcu->packProtocolData(0x82,0x51,NULL,1,0);
			break;
		case TAGID_TBOXINFO:	//Tbox信息
			dataLen = pack_TBOXINFO_data(pData,len);
			break;

		case TAGID_LOCKDOOR:	//Tbox信息
			dataLen = pack_LOCKDOOR_data(pData,len);
			break;
		case TAGID_OPENDOOR:	//Tbox信息
			dataLen = pack_OPENDOOR_data(pData,len);
			break;
		case TAGID_POWEROFF:	//Tbox信息
			dataLen = pack_POWEROFF_data(pData,len);
			break;
		case TAGID_POWERON:	//Tbox信息
			dataLen = pack_POWERON_data(pData,len);
			break;
		case TAGID_CARFIND:	//Tbox信息
			dataLen = pack_CARFIND_data(pData,len);
			break;
		default:
			FACTORYPATTERN_LOG("tag error");
			break;
	}
	return dataLen;
}

uint16_t FactoryPattern_Communication::pack_Config_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	
	*pos++=TAGID_CONFIG_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;


	return (uint16_t)(pos-pData); 
}

uint16_t FactoryPattern_Communication::pack_CANSTATUS_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_CANSTATUS_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	//*pos++=0x00;
	*pos++=McuStatus.CANStatus;
	return (uint16_t)(pos-pData); 
}
uint16_t FactoryPattern_Communication::pack_CANDATA_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_CANDATA_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;
	//*pos++=McuStatus.CANStatus;
	return (uint16_t)(pos-pData); 
}
uint16_t FactoryPattern_Communication::pack_SIXSENSOR_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_SIXSENSOR_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	
	if(McuStatus.GyroscopeData)
	{
		*pos++=1;
	}
	else
		*pos++=0;
		
	//*pos++=1;
	return (uint16_t)(pos-pData); 
}
uint16_t FactoryPattern_Communication::pack_Emmc_data(uint8_t *pData, int len)
{	
	uint8_t *pos = pData;
	*pos++=TAGID_NFC_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	if(testnfcstate==true)
	{
		*pos++=1;
	}
	else
		*pos++=0;
	testnfcstate = false;
	return (uint16_t)(pos-pData); 
}
uint16_t FactoryPattern_Communication::pack_WIFI_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_WIFI_Rep;
	uint16_t wifilen;
	
	wifilen = strlen((char*)tboxDetectingPattern.WifiTrigger.ssid);
	*pos++=uint8_t(wifilen);
	memcpy(pos,tboxDetectingPattern.WifiTrigger.ssid, wifilen);
	pos+=wifilen;

	wifilen = strlen((char*)tboxDetectingPattern.WifiTrigger.password);
	*pos++=uint8_t(wifilen);
	memcpy(pos,tboxDetectingPattern.WifiTrigger.password, wifilen);
	pos+=wifilen;


	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_BT_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_BT_Rep;


	*pos++=0;
	*pos++=1;
	//判断是否成功

	if(testblestate == true)
	{
		*pos++=1;
	}
	else 
		*pos++=0;
	testblestate = false;
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_IVISTATUS_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_IVISTATUS_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	if(testAPN)
	{
		*pos++=1;
	}
	else
	{
		*pos++=0;
	}
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_APN2_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_APN2_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	
	if(APN2test)
	{
	*pos++=1;
	}
	else
		*pos++=1;  //现在服务器不通  正式版要改为0
	
	//*pos++=1;
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_ECall_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_ECall_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_GPSOpen_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_GPSOpen_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	
	if(McuStatus.GpsAntenna==0x01)
	{
		*pos++=0;
	}
	else
		*pos++=1;

	//*pos++=1;
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_GPSSHORT_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_GPSInterrupt_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	//*pos++=McuStatus.GpsPoist;
	
	if(McuStatus.GpsAntenna==0x02)
		*pos++=0;         //现在GPS有问题 一直显示短路 正式版本改为0
	else
		*pos++=1;
		
	//*pos++=1;
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_GPSSIGN_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_GPSSIN_Rep;

	uint16_t Gpslen;
	Gpslen = 2*McuStatus.GpsSatelliteNumber + 3;

	*pos++=(Gpslen>>8)&0xFF;
	*pos++=(Gpslen>>0)&0xFF;
	//GPStotal
	*pos++=(McuStatus.GPStime>>8)&0xFF;
	*pos++=(McuStatus.GPStime>>0)&0xFF;
	
	*pos++=McuStatus.GpsSatelliteNumber;


	//McuStatus.GpsSatelliteNumber
	
	for(int i = 0; i < int(McuStatus.GpsSatelliteNumber); i++)
	{
		//*pos++=(McuStatus.GpsSatelliteInter[i]>>8) & 0xFF;
		//*pos++=(McuStatus.GpsSatelliteInter[i]>>0) & 0xFF;	

		*pos++=McuStatus.GpsNumber[i];
		*pos++=McuStatus.GpsStrength[i];
	}
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_MAINPOWER_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_MAINPOWER_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	if(McuStatus.MainPower>110&&McuStatus.MainPower<130) /*10.5V --13.5V*/
	{
		*pos++=1;
	}
	else
		*pos++=0;
	
	//*pos++=1;
	
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_RESERVEPOWER_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_RESERVEPOWER_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	/*if(McuStatus.DeputyPower)
	{
		*pos++=1;
	}
	else
	*pos++=0;*/
	*pos++=1;
	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_ACCOFF_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_ACCOFF_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;

	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_TBOXINFO_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_TBOXINFO_Rep;

	uint16_t TboxinfoLen = 90;
	*pos++ = (TboxinfoLen >> 8)&0xFF;
	*pos++ = (TboxinfoLen>> 0)&0xFF;
	 
	 //tbox info
	char buff[50];
	//TBox_PatterInfo m_tboxinfo = {0};

	//memcpy(pos,ConfigShow.Tbox_VIN,17);
	//pos+=17;

	//*pos++=ConfigShow.VehicleType;

	memset(buff, 0, sizeof(buff));
	dataPool->getPara(SIM_ICCID_INFO, (void *)buff, sizeof(buff));
	memcpy(pos,buff,20);
	pos+=20;

	memset(buff, 0, sizeof(buff));
	dataPool->getPara(IMEI_INFO, (void *)buff, sizeof(buff));
	memcpy(pos,buff,15);
	pos+=15;


	memset(buff, 0, sizeof(buff));
	dataPool->getPara(CIMI_INFO, (void *)buff, sizeof(buff));
	memcpy(pos,buff,15);
	pos+=15;

	//memcpy(pos,ConfigShow.TBoxSerialNumber,30);
	//pos+=30;


	//SN从8090获取
	//dataPool->getTboxConfigInfo(SUPPLYPN_ID, ConfigShow.SupplierSN, sizeof(ConfigShow.SupplierSN));
	//memcpy(ConfigShow.SupplierSN,"12345678901",11);  //测试数据 从8090获取
//	memcpy(pos,ConfigShow.SupplierSN,16);
	//pos+=16;

	return (uint16_t)(pos-pData);

}

uint16_t FactoryPattern_Communication::pack_LOCKDOOR_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_LOCKDOOR_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;

	return (uint16_t)(pos-pData);
}

uint16_t FactoryPattern_Communication::pack_OPENDOOR_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_OPENDOOR_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;

	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_POWEROFF_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_POWEROFF_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;

	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_POWERON_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_POWERON_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;

	return (uint16_t)(pos-pData);
}
uint16_t FactoryPattern_Communication::pack_CARFIND_data(uint8_t *pData, int len)
{
	uint8_t *pos = pData;
	*pos++=TAGID_CARFIND_Rep;
	*pos++=0;
	*pos++=1;
	//判断是否成功
	*pos++=1;

	return (uint16_t)(pos-pData);
}

/*********************************************************/
//测试wifi
uint8_t FactoryPattern_Communication::GetDetecting_Wifi(TBox_Detecting_Wifi &detecting_Wifi)
{
	int ret;
	int authType = 5;
	int encryptMode = 4;
	char ssid[20] = {0};
	char password[16] = {0};
	char buff[50];
	memset(buff, 0, sizeof(buff));
	
	//get wifi open or close state.
    if(!access("/sys/class/net/wlan0", F_OK))//判断文件存在
    {
        //wifi is open
		detecting_Wifi.Wifistate = 1; //OK
    }
	else
	{
        //wifi is close
//		ret = wifi_OpenOrClose(1);//old sdk
		//open wifi
		wifi_power(1);//zhujw
		wifi_led_on();
		sleep(4);
		if(!access("/sys/class/net/wlan0", F_OK))//判断文件存在
			detecting_Wifi.Wifistate = 1; //OK
		else
			detecting_Wifi.Wifistate = 0; //fault
	}
	//获取Wifi ssid
	memset(ssid, 0, sizeof(ssid));
    //wifi_get_ap_ssid(ssid, AP_INDEX_STA);//old sdk 
    get_ssid(ssid, AP_INDEX_STA);//new sdk
	
	//获取Wifi password
	memset(password, 0, sizeof(password));
	//wifi_get_ap_auth(&authType, &encryptMode, password, AP_INDEX_STA);//old sdk
	get_auth(&authType, &encryptMode, password, AP_INDEX_STA);//new sdk

	memcpy(detecting_Wifi.ssid, ssid, strlen(ssid));
	memcpy(detecting_Wifi.password,password,strlen(password));
	return 0;
	
}
//获取Tbox信息
uint8_t FactoryPattern_Communication::GetDetecting_TboxInfo(TBox_PatterInfo PatterInfo)
{
	int  ret;
	char buff[50];
	
	//VIN
	memset(buff, 0, sizeof(buff));
	ret = dataPool->getTboxConfigInfo(VinID, buff, sizeof(buff));
	ret = strlen(buff);
	//memcpy(PatterInfo.VIN, buff, ret);
	
	//车型号
	//PatterInfo.VehicleType = 2;
	uint8_t		Sim_IMEI[15];	//IMEI
	memcpy(PatterInfo.Sim_IMEI, Sim_IMEI, sizeof(Sim_IMEI));
	uint8_t		ICCID[20];		//ICCID
	memcpy(PatterInfo.ICCID, ICCID, sizeof(ICCID));
	uint8_t		Sim_IMSI[15];	//IMSI
	memcpy(PatterInfo.Sim_IMSI, Sim_IMSI, sizeof(Sim_IMSI));
	uint8_t		Sim_Num[11];	//Sim卡号
	//memcpy(PatterInfo.Sim_Num, Sim_Num, sizeof(Sim_Num));
	uint8_t		Supply_PN[11];	//供应商零件号
	memcpy(PatterInfo.Supply_PN, Supply_PN, sizeof(Supply_PN));
	return 0;
}
