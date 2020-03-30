#include "common.h"
#include "TBoxDataPool.h"

extern TBoxDataPool *dataPool;
const unsigned char sysVerNumber[3] = {1, 0, 0};
unsigned char mcuVerNumber[3] = {1, 0, 0};

uint8_t log_FAWACP_Recverr_value = 0;
uint8_t log_FAWACP_Senderr_value = 0;
uint8_t log_FAWACP_Timeout_event_value = 0;
uint8_t log_FAWACP_Remote_Awake_value =0;

char FAWACP_Recverr_buff[128] = {0};
char FAWACP_Senderr_buff[128] = {0};
char FAWACP_Timeout_event_buff[128] = {0};
char FAWACP_Remote_event_buff[128] = {0};

uint8_t LogFinish = 0;			/*0 no error to write*/


/*****************************************************************************
* Function Name : getDate
* Description   : 获取当前日期
* Input			: char *pDest
*                 int size
* Output        : None
* Return        : None
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
void getDate(char *pDest, int size)
{
	unsigned char i;
	unsigned char sysMonth = 0;
	char *pos = pDest;
	char year[5] = {0};
	char month[5] = {0};
	char day[5] = {0};
	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug","Sep", "Oct", "Nov", "Dec"};
	char date[] = __DATE__;
	char time[] = __TIME__;

	//printf("Date:%s,Time:%s\n", date, time);

	sscanf(date, "%s %s %s", month, day, year);
	//printf("month:%s,day:%s,year:%s\n", month, day, year);

	for(i = 0; i<12; i++)
	{
		if(strncmp(month, months[i], 3) == 0)
		{
			sysMonth = i+1;
			break;
		}
	}
	//printf("%02x\n", sysMonth);

	memset(pDest, 0, size);
	memcpy(pos, year, 4);
	pos = pos + 4;

	*pos++ = (char)((sysMonth%100)/10+0x30);
	*pos++ = (char)(sysMonth%10+0x30);

	//printf("day len %d\n",(int)strlen(day));
	if(strlen(day) < 2)
	{
		*pos++ = 0x30; // 0x30-> ASCII: 0
		*pos++ = day[0];
	}
	else
	{
		memcpy(pos, day, 2);
		pos += 2;
	}
	
	//*pos++ = 0x5F;    // 0x5F-> ASCII: _
	//memcpy(pos, time, 8);
	
	//printf("pDest:%s\n", pDest);
}

#if 0
/*****************************************************************************
* Function Name : getSoftwareVerion
* Description   : 获取当前日期
* Input			: char *pBuff
*                 unsigned int size
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int getSoftwareVerion(char *pBuff, unsigned int size)
{
	char sysVerion[12] = {0};
	char sysDateTime[32] = {0};

	memset(pBuff, 0, size);
	
	strcat(pBuff, RELEASE_NOTE);
	sprintf(sysVerion, "V%d.%d.%d", sysVerNumber[0], sysVerNumber[1],sysVerNumber[2]);
	strcat(pBuff, sysVerion);
	strcat(pBuff, BUILD_NOTE);
	getDate(sysDateTime, sizeof(sysDateTime));
	strcat(pBuff, sysDateTime);

	return 0;
}

#endif


/*****************************************************************************
* Function Name : getSoftwareVerion
* Description   : 获取当前日期
* Input			: char *pBuff
*                 unsigned int size
* Output        : None
* Return        : 0:success, -1:failed
* Auther        : ygg
* Date          : 2018.04.09
*****************************************************************************/
int getSoftwareVerion(char *pBuff, unsigned int size)
{
	char sysDateTime[32] = {0};
	if(pBuff == NULL)
		return -1;
	
	memset(pBuff, 0, size);
	strcat(pBuff, PROJECT_NAME);
	strcat(pBuff, ".");
//	strcat(pBuff, PLATFORM_CODE);
//	strcat(pBuff, ".");
	strcat(pBuff, SUPPORT_GBT32960_VER);
	strcat(pBuff, ".");
	strcat(pBuff, SUPPORT_ACP_VER);
	strcat(pBuff, ".");
//	strcat(pBuff, FILE_VER_PREFIX);
	strcat(pBuff, FILE_VERSION);
	strcat(pBuff, ".");
	getDate(sysDateTime, sizeof(sysDateTime));
	strcat(pBuff, sysDateTime);
	
	return 0;
}


#if 0
void SetGlDebugLog(unsigned char log)
{
	glDebugLogSwitch = log;
}
unsigned char GetGlDebugLog(void)
{
	return glDebugLogSwitch;

}
#endif


#if 1
int sys_mylog(char *plog)
{
#define LOG_PATH	"/data/mylog"

	int glDebugLogSwitch = 0;
	char buff[1024] = "";
	struct tm *p_tm = NULL; //Ã¦â€”Â¶Ã©â€”Â´Ã§Å¡â€žÃ¥Â¤â€žÃ§Â?	
	time_t tmp_time;
	tmp_time = time(NULL);
	p_tm = gmtime(&tmp_time);

	FILE *flogfd;

    struct stat buf;
	dataPool->getPara(GL_DEBUGLOG_SWITCH_ID, &glDebugLogSwitch, sizeof(glDebugLogSwitch));
    sprintf(buff,"%d-%d-%d-%d-%d::%s\r\n",p_tm->tm_mon+1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,plog);
	if(glDebugLogSwitch == 1)
	{
		stat(LOG_PATH, &buf);
		if(buf.st_size> 10*1000*1000)
		{ // Ã¨Â¶â€¦Ã¨Â¿â€?10 Ã¥â€¦â€ Ã¥Ë†â„¢Ã¦Â¸â€¦Ã§Â©ÂºÃ¥â€ â€¦Ã¥Â®Â?
			flogfd = fopen("/data/mylog", "w");
		} 
		else 
		{
			flogfd = fopen("/data/mylog", "at");
		}
		if(flogfd == NULL)
		{
			//printf("jason +++++++++++++++++++ filelog open failed\n");
			return -1;
		}
		//flogfd = fopen(LOG_PATH, "a+");
		//printf("jason +++++++++++++++++++ filelog open \r\n");

		fwrite(buff, 1, strlen(buff), flogfd);

		//fsync(flogfd);
	    fflush(flogfd);

		fclose(flogfd);
	}
	else
	{
		//printf("jason +++++++++++++++++++ filelog close \r\n");
	}
	return 0;
}
#endif



char *GetFAWACPRecverrStr()
{
	return FAWACP_Recverr_buff;
}

char *GetFAWACPSenderrStr()
{
	return FAWACP_Senderr_buff;
}

char *GetFAWACPTimeout_eventStr()
{
	return FAWACP_Timeout_event_buff;
}

char *GetFAWACPRemoteCtlStr()
{
	return FAWACP_Remote_event_buff;
}

uint8_t GetFAWACP_Recverrvalue()
{
	return log_FAWACP_Recverr_value;
}

uint8_t GetFAWACP_Senderrvalue()
{
	return log_FAWACP_Senderr_value;
}

uint8_t GetFAWACP_Timeouteventvalue()
{
	return log_FAWACP_Timeout_event_value;
}

uint8_t GetFAWACP_RemoteAwakevalue()
{
	return log_FAWACP_Remote_Awake_value;
}

void SetFAWACP_Recverrvalue(uint8_t value)
{
	log_FAWACP_Recverr_value = value;
}

void SetFAWACP_Senderrvalue(uint8_t value)
{
	log_FAWACP_Senderr_value = value;
}

void SetFAWACP_Timeouteventvalue(uint8_t	value)
{
	log_FAWACP_Timeout_event_value = value;
}
void SetFAWACP_RemoteAwakevalue(uint8_t	value)
{
	log_FAWACP_Remote_Awake_value = value;
}

void FAWACP_ERROR_Log(int status, int errcode,int times)
{
	char buff[32] = {0};
	int glDebugLogSwitch = 0;

	dataPool->getPara(GL_DEBUGLOG_SWITCH_ID, &glDebugLogSwitch, sizeof(glDebugLogSwitch));
	if(glDebugLogSwitch == 0) return ;

	switch(status)
	{
		case 1:
			log_FAWACP_Recverr_value = 1;
			sprintf(FAWACP_Recverr_buff,"RECV_ERR:SignalStrength:%d::ERRON:%s-%d,disconnect count =%d\r\n",tboxInfo.networkStatus.signalStrength, strerror(errcode), errcode,times);
			sys_mylog(FAWACP_Recverr_buff);
		break;
		case 2:
			log_FAWACP_Senderr_value = 1;
			sprintf(FAWACP_Senderr_buff,"SEND_ERR:SignalStrength:%d::ERRON:%s-%d,disconnect count =%d\r\n",tboxInfo.networkStatus.signalStrength, strerror(errcode), errcode,times);
		
			sys_mylog(FAWACP_Senderr_buff);
			break;
		case 3:
			log_FAWACP_Timeout_event_value = 1;
			sprintf(FAWACP_Timeout_event_buff,"TIMEOUT_ERR:SignalStrength:%d::ERRON:%s-%d\r\n",tboxInfo.networkStatus.signalStrength, strerror(errcode), errcode);
		
			sys_mylog(FAWACP_Timeout_event_buff);
			break;
		case 4:
			log_FAWACP_Remote_Awake_value = 1;
			sprintf(FAWACP_Remote_event_buff,"remote_awake:SignalStrength:%d\r\n",tboxInfo.networkStatus.signalStrength);
		
			sys_mylog(FAWACP_Remote_event_buff);
			break;
		case 5:
			sprintf(buff,"socket connect failed");
			sys_mylog(buff);
		case 6:
			sprintf(buff,"60s time out kill tbox");			
			sys_mylog(buff);
			break;
		case 7:
			sprintf(buff,"socket reconnect count =%d",times);			
			sys_mylog(buff);
			break;
		case 8:
			sprintf(buff,"socket connect auth success\r\n");			
			sys_mylog(buff);
			break;
		default:
			break;
	}
}


void tbox_log(int flag, uint8_t msgid)
{
	int glDebugLogSwitch = 0;

	dataPool->getPara(GL_DEBUGLOG_SWITCH_ID, &glDebugLogSwitch, sizeof(glDebugLogSwitch));

	if(glDebugLogSwitch == 1)
	{
	    FILE* fd;
	    struct stat buf;
	    stat("/data/mylog", &buf);
	    if(buf.st_size> 10*1000*1000){ // è¶…è¿‡ 10 å…†åˆ™æ¸…ç©ºå†…å®¹
	        fd = fopen("/data/mylog", "w");
	    } else {
	        fd = fopen("/data/mylog", "at");
	    }
	    
	    if (NULL == fd)
	    {
	        //exit(0);
	        return;
	    }
		char szLine[512] = {0};

		struct tm *p_tm = NULL; //æ—¶é—´çš„å¤„ç?
		time_t tmp_time;
		tmp_time = time(NULL);
		p_tm = gmtime(&tmp_time);
		int nLen1;
		switch (flag)
		{
			case 1:
			nLen1 = sprintf(szLine, "tbox 300s no connect on \n[%04d-%02d-%02d %02d:%02d:%02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec);
			break;
			case 2:
			nLen1 = sprintf(szLine, "MCU 120s no connect on \n[%04d-%02d-%02d %02d:%02d:%02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec);
			break;
			case 3:
			nLen1 = sprintf(szLine, "4G receive TSP cmd \n[%04d-%02d-%02d %02d:%02d:%02d  %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
			break;
			case 4:
			nLen1 = sprintf(szLine, "4G send cmd to 8090 \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
			break;
			case 5:
			nLen1 = sprintf(szLine, "8090 reply cmd to 4G \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
			break;
			case 6:
			nLen1 = sprintf(szLine, "4G reply cmd to TSP \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
			break;
			case 7:	
				if(msgid)
				{
					nLen1 = sprintf(szLine, "4G mode cpu nomal \n[%04d-%02d-%02d %02d:%02d:%02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec);
				}
				else
				{
					nLen1 = sprintf(szLine, "4G mode cpu slower \n[%04d-%02d-%02d %02d:%02d:%02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec);
				}
			break;
			case 8:
				
			nLen1 = sprintf(szLine, "4G uart send to mcu  \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
				break;
			case 9:
				
			nLen1 = sprintf(szLine, "4G receive mcu sync head \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
				break;
			case 10:
			nLen1 = sprintf(szLine, "4G remote control 1 step \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
				break;
			case 11:
			nLen1 = sprintf(szLine, "4G remote control 2 step \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
				break;
			case 12:
			nLen1 = sprintf(szLine, "4G remote control 3 step \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
				break;
			case 13:
			nLen1 = sprintf(szLine, "4G remote control 4 step \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec, msgid);
				break;
			case 14:
			nLen1 = sprintf(szLine, "4G remote control 5 step \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,	p_tm->tm_mday,	p_tm->tm_hour,	p_tm->tm_min, p_tm->tm_sec, msgid);
			break;
			case 15:
			nLen1 = sprintf(szLine, "4G upgrade failed because volt fault  step \n[%04d-%02d-%02d %02d:%02d:%02d %02d]\n", p_tm->tm_year+1900, p_tm->tm_mon,	p_tm->tm_mday,	p_tm->tm_hour,	p_tm->tm_min, p_tm->tm_sec, msgid);
			break;
		}
	   // int nLen1 = sprintf(szLine, "tbox crash on \n[%04d-%02d-%02d %02d:%02d:%02d]\n", p_tm->tm_year-90, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec);
		fwrite(szLine, 1, strlen(szLine), fd);

	    fflush(fd);
	    fclose(fd);
	    fd = NULL;
	}
}
