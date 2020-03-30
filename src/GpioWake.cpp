#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include <pthread.h>
#include "GpioWake.h"
#include "bluetooth.h"



#define MODEM_WAKEUP_MCU_GPIO  77 //sim7600.pin87
#define GPIO77_LOCKNAME "GPIO77_Wakeup_modem"
#define MCU_WAKEUP_MODEM_GPIO  74 //sim7600.pin72
#define GPIO74_LOCKNAME "GPIO74_Wakeup_modem"

#define GPIO_LOW_LEVEL_MS    100

//#define WAKE_UP_MCU_TIME    1800  // 3 * 60 * 10 3分钟 

#define GPIO_SLEEP_MODEM_COUNT 		15

#define MAX_BUF 128


#define PIN72_ISR_LOCK  system_power_lock(GPIO74_LOCKNAME)
#define PIN72_ISR_UNLOCK system_power_unlock(GPIO74_LOCKNAME)

#define FC_WAKE  system_power_lock("FC_LOCK")
#define FC_SLEEP system_power_unlock("FC_LOCK")

#define PIN87_LOCK  system_power_lock(GPIO77_LOCKNAME)
#define PIN87_UNLOCK system_power_unlock(GPIO77_LOCKNAME)

#define FC_NO_SLEEP system_power_lock("Host")

static int openOrCloseWifi = -1;
uint8_t wifi_RemindStatus = 0;

extern bool testfac;

#ifdef GPIO_WAKE_DEBUG_LOG
void gpio_wake_log(int status)
{
	static int wake_up_count = 0;
	static int sleep_count = 0;
	
	char buff[100]={};

	if(status)
	{
		wake_up_count++;
		sprintf(buff,"%s,wakeup,count=%d\r\n",GPIO_WAKE_DEBUG_LOG,wake_up_count);
	}
	else
	{
		sleep_count++;
		sprintf(buff,"%s,sleep,count=%d\r\n",GPIO_WAKE_DEBUG_LOG,sleep_count);
	}
	if(CMcu::GetInstance()->GetSystemBatteryStatus() == SYSTEM_BATTRY_STATUS_NORMAL)
	{
		sys_mylog(buff);
	}
}
#endif

int lowPowerMode(int isSleep, int isCloseWifi);


void system_power_lock(const char *lock_id)
{
	int fd;

	fd = open("/sys/power/wake_lock", O_WRONLY);
	if (fd < 0) {
		printf("wake_lock,error %d\n",fd);
		return;
	}

	write(fd, lock_id, strlen(lock_id));

	close(fd);
}
void system_power_unlock(const char *lock_id)
{
	int fd;
	fd = open("/sys/power/wake_unlock", O_WRONLY);
	if (fd < 0) {
		printf("wake_unlock,error %d\n",fd);
		return;
	}

	write(fd, lock_id, strlen(lock_id));

	close(fd);
}

int gpio_file_create(int gpio)
{
	int sfd = -1;
	char checkstr[50] = {0};
	char configstr[10] = {0};
	int reto,len;

	sprintf(checkstr,"/sys/class/gpio/gpio%d/value",gpio);
	if(0 == access(checkstr, F_OK)){
		return 0;
	}

	sfd = open("/sys/class/gpio/export",O_WRONLY);
	if(sfd < 0){
		printf("%s:%d,open file error,%d\n",__FUNCTION__,__LINE__,sfd);
		return sfd;
	}

	len = sprintf(configstr,"%d",gpio);
	reto = write(sfd,configstr,len);

	if(reto != len){
		printf("create gpio(%d) files:%d,%d,%d\n",gpio,__LINE__,len,reto);
		return reto;
	}
	usleep(1000);
	reto = access(checkstr, F_OK);
	if(0 > reto){
		printf("%s:%d,gpio file(%s)not exist\n",__FUNCTION__,__LINE__,checkstr);
	}

	close(sfd);
	return reto;
}
int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];
 
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("gpio(%d)/direction\n",gpio);
		return fd;
	}
 
	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
 
	close(fd);
	return 0;
}
int gpio_set_value(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];
 
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value\n");
		return fd;
	}
 
	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
 
	close(fd);
	return 0;
}


uint8_t GPIO_GetMcuWake4GStatus(int *pStatus)
{	

	//return cat_gpio_value (MCU_WAKEUP_MODEM_GPIO,pStatus);
	*pStatus = tboxInfo.operateionStatus.isGoToSleep;
	return 0;
}

void Set_CpuFreqValue(     int Value )
{
	uint8_t cmd[128] = {0};

	memset(cmd, 0, sizeof(cmd));
	sprintf( cmd, "echo %d > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", Value );
	system( cmd );

	return ;
}


uint8_t  cat_gpio_value(unsigned int gpio,int *pStatus)
{
	int fd;
	uint8_t value = 0;
	char buf[MAX_BUF];
	char temp[3] = {0};
	//uint8_t ret = RET_FAIL;
	if(NULL == pStatus)
	{
		return RET_INVALID;
	}
	else
	{
	}
 
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value",gpio);
 
	fd = open(buf, O_RDONLY|O_NONBLOCK);
	if (fd < 0) 
	{
		perror("gpio/set-value\n");
		return RET_FAIL;
	}
	 
	lseek(fd,(off_t)0, SEEK_SET);
	read(fd,temp,3);

	value = atoi(temp);
 
	close(fd);
	*pStatus = value;
	
	return RET_OK;
}



static int gpio_set_isr(unsigned gpio)
{
	char fvalue_path[MAX_BUF];
	int fd = -1,ret = -1;

	sprintf(fvalue_path,"/sys/class/gpio/gpio%d/edge",gpio);
	fd = open(fvalue_path,O_RDWR);
	if(fd < 0){
		printf("gpio_set_isr write %s error %d\n",fvalue_path,ret);
		return fd;
	}
	
	if((ret = write(fd,"both",5)) < 1){
		close(fd);
		printf("gpio_set_isr write %s error,%d\n",fvalue_path,ret);
		return ret;
	}
	close(fd);

	return (ret < 0)?ret:0;
}
#define MODEM_WAKEUP_MCU_TIME 100
int modem_ri_notify_mcu(void)
{
	PIN87_LOCK;
	gpio_set_value(MODEM_WAKEUP_MCU_GPIO,1);
	poll(0,0,MODEM_WAKEUP_MCU_TIME);
	gpio_set_value(MODEM_WAKEUP_MCU_GPIO,0);
	PIN87_UNLOCK;
}

int modem_ri_notify_mcu_high(void)
{
	PIN87_LOCK;
	gpio_set_value(MODEM_WAKEUP_MCU_GPIO,1);
	PIN87_UNLOCK;
}
int modem_ri_notify_mcu_low(void)
{
	PIN87_LOCK;
	gpio_set_value(MODEM_WAKEUP_MCU_GPIO,0);
	PIN87_UNLOCK;
}

int gpio_can_wakeup(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/can_wakeup", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}

	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);

	close(fd);
	return 0;
}
int uart_wakeup(unsigned char bwakeup)
{
	int ufd = open(UART_SLEEP_CTL, O_WRONLY);
	if (ufd < 0) {
		printf(" %s error %d\n",UART_SLEEP_CTL, ufd);
		return -1;
	}
	if(bwakeup){
		write(ufd, "on", 2);
	}else{
		write(ufd, "auto", 4);
	}
	close(ufd);
	return 0;
}

int gpio_init()
{
    int ret;
    ret = gpio_file_create(MODEM_WAKEUP_MCU_GPIO);
    ret += gpio_set_dir(MODEM_WAKEUP_MCU_GPIO, 1);
    ret += gpio_set_value(MODEM_WAKEUP_MCU_GPIO, 0);

    ret += gpio_file_create(MCU_WAKEUP_MODEM_GPIO);
    ret += gpio_set_dir(MCU_WAKEUP_MODEM_GPIO, 0);
    ret += gpio_set_isr(MCU_WAKEUP_MODEM_GPIO);
    ret += gpio_can_wakeup(MCU_WAKEUP_MODEM_GPIO, 1);
    return ret;
}

int lowPowerMode(int isSleep, int isCloseWifi)
{
	// 0:go to sleep, unlock system
	if(isSleep == 0)
	{
		if(tboxInfo.operateionStatus.isGoToSleep != 0)
			tboxInfo.operateionStatus.isGoToSleep = 0;

		CMcu::GetInstance()->Set_Uart_Sync_Value(0);

#if NEW_UPGRADE_MCU_MODE
		if(CMcu::GetInstance()->mcuUpgradeMode == MCU_UPGRADE_STATE_IDLE)
		{
			uart_wakeup(0);
		}
#else
		if(CMcu::GetInstance()->mcuUpgradeMode == MCU_UPGRADE_STATE_FINISH)
		{
			CMcu::GetInstance()->mcuUpgradeMode = MCU_UPGRADE_STATE_IDLE;
			//uart_wakeup(0);
		}
#endif
		printf("\n\n ===== start go to sleep, unlock system ===== \n\n");

		//system("echo 1500 > /sys/class/tty/ttyHS1/device/power/autosuspend_delay_ms");

		//CFAWACP::GetInstance()->SleepUpgrade();

		if(testfac)
		{
			system("echo auto > /sys/class/tty/ttyHS0/device/power/control"); 
			CFAWACP::GetInstance()->m_Upgrade = false;
			CFAWACP::GetInstance()->disconnectSocket();
			FC_SLEEP;
		}
	}
	else    // wake up sys, lock system
	{
		FC_WAKE;
		DEBUGLOG("=========== isSleep:%d, isCloseWifi:%d, openOrCloseWifi:%d tboxInfo.operateionStatus.wifiStartStatus %d\n",isSleep, isCloseWifi, openOrCloseWifi, tboxInfo.operateionStatus.wifiStartStatus);
		if(tboxInfo.operateionStatus.isGoToSleep != 1) tboxInfo.operateionStatus.isGoToSleep = 1;
		
#if NEW_UPGRADE_MCU_MODE
		if(CMcu::GetInstance()->mcuUpgradeMode != MCU_UPGRADE_STATE_IDLE)
		{
			uart_wakeup(1);
		}
#else
		if(CMcu::GetInstance()->mcuUpgradeMode == MCU_UPGRADE_STATE_WAIT_MCU_REQ)
		{
			//uart_wakeup(1);
		}
#endif
		printf("\n\n ===== start wake up sys, lock system ===== \n\n");
	}

	return 0;
}

#if 0
int gipo_wakeup_init()
{
	int ret = 0;
	int fd;
	char fvalue_path[MAX_BUF]={0};
	struct pollfd read_pollfd;
	int ch_time = -1;
	static uint8_t GpioHighCount = 0;
	static uint8_t GpioLowCount = 0;
	static uint16_t WakeUpMcuTimer = 0;
	if(gpio_init() != 0)
	{
		printf("gpio_init error\n");
		return -1;
	}
	//FC_WAKE;
	sprintf(fvalue_path,"/sys/class/gpio/gpio%d/value",MCU_WAKEUP_MODEM_GPIO);
	fd = open(fvalue_path,O_RDONLY|O_NONBLOCK);
	if(fd< 0)
	{
		printf("open %s,fd error %d\n",fvalue_path,fd);
		return -1;
	}

	memset(&read_pollfd,0,sizeof(read_pollfd));
	read_pollfd.fd=fd;
	read_pollfd.events = (POLLERR |POLLPRI);
	 ch_time = GPIO_LOW_LEVEL_MS;
	//12.29 KK modeify add = modem_ri_notify_mcu():: 4G远程更新时,重启发送脉冲唤醒MCU
//	modem_ri_notify_mcu();
	CMcu::GetInstance()->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
	uart_wakeup(0);/*串口设置为自动模式*/
	while(1)
	{
		//printf("polling,time = %d\n",ch_time);
		ret = poll(&read_pollfd, 1, ch_time);
		if(ret < 0)
		{
			close(fd);

			if(gpio_init() != 0)
			{
				printf("gpio_init error\n");
				//return -1;
			}
			//FC_WAKE;
			sprintf(fvalue_path,"/sys/class/gpio/gpio%d/value",MCU_WAKEUP_MODEM_GPIO);
			fd = open(fvalue_path,O_RDONLY|O_NONBLOCK);
			if(fd< 0)
			{
				printf("open %s,fd error %d\n",fvalue_path,fd);
				system("reboot");
			}

			memset(&read_pollfd,0,sizeof(read_pollfd));
			read_pollfd.fd=fd;
			read_pollfd.events = (POLLERR |POLLPRI);
			 ch_time = GPIO_LOW_LEVEL_MS;
			printf(" GPIO error check \r\n");
			//break;
		}

		if((ret == 0) && (ch_time == GPIO_LOW_LEVEL_MS)) /*超时时间到了*/
		{
			char rbuff[3]={0};
			int lvalue = 0;
			read_pollfd.revents = 0;
			ret = lseek(fd,(off_t)0, SEEK_SET);
			ret += read(fd,rbuff,3);
		//	printf("Timeout GPIO is %s\n", rbuff);
			lvalue = atoi(rbuff);
			if(!lvalue)
			{
				GpioLowCount ++;
				GpioHighCount = 0;
			}
			else
			{
				GpioHighCount ++;
				GpioLowCount = 0;
			}
			
			if(GpioHighCount > GPIO_SLEEP_MODEM_COUNT) 
			{
				GpioHighCount = 0;
				if(tboxInfo.operateionStatus.isGoToSleep != 1)
				{
					tboxInfo.operateionStatus.wakeupSource = 0;
					lowPowerMode(1, 1);
					//printf("jason add ++++++++++++++++++++++++++++++++++++++++++++++GPIO is %s\r\n", rbuff);
					#ifdef GPIO_WAKE_DEBUG_LOG
					gpio_wake_log(tboxInfo.operateionStatus.isGoToSleep);
					#endif
				}
			}
			
			if(GpioLowCount > GPIO_SLEEP_MODEM_COUNT)
			{
				GpioLowCount = 0;
				if(tboxInfo.operateionStatus.isGoToSleep != 0)
				{
					PIN72_ISR_UNLOCK;
					tboxInfo.operateionStatus.wakeupSource = -1;
					lowPowerMode(0, 0);
					#ifdef GPIO_WAKE_DEBUG_LOG
						gpio_wake_log(tboxInfo.operateionStatus.isGoToSleep);
					#endif
				}
			}
		}
		if((read_pollfd.revents & read_pollfd.events) == read_pollfd.events)
		{
			char rbuff[3]={0};
			read_pollfd.revents = 0;
			PIN72_ISR_LOCK;
			ret = lseek(fd,(off_t)0, SEEK_SET);
			ret += read(fd,rbuff,3);
			
			GpioHighCount = 0;
			GpioLowCount = 0;
			ch_time = GPIO_LOW_LEVEL_MS;//wait 10ms
		}

	}
	return ret;
}
#else
int gipo_wakeup_init()
{
	int ret = 0;
	int fd;
	char fvalue_path[MAX_BUF]={0};
	struct pollfd read_pollfd;
	int ch_time = -1;
	static uint8_t GpioHighCount = 0;
	static uint8_t GpioLowCount = 0;
	uint16_t WakeUpMcuTimer = 0;
	char rbuff[3]={0};
	int lvalue = 0;

	if(gpio_init() != 0)
	{
		printf("gpio_init error\n");
		return -1;
	}
	FC_WAKE;
	FC_NO_SLEEP;
	sprintf(fvalue_path,"/sys/class/gpio/gpio%d/value",MCU_WAKEUP_MODEM_GPIO);
	fd = open(fvalue_path,O_RDONLY);
	if(fd< 0)
	{
		printf("open %s,fd error %d\n",fvalue_path,fd);
		return -1;
	}

	CMcu::GetInstance()->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
	//uart_wakeup(0);/*1_2_054 串口设置为自动模式*/
	uart_wakeup(1);/*1_2_055 串口设置为ON 模式*/
	while(1)
	{
		memset(rbuff, 0, sizeof(rbuff));
		ret = lseek(fd,(off_t)0, SEEK_SET);
		ret += read(fd,rbuff,3);

		lvalue = atoi(rbuff);
		if(!lvalue)
		{
			GpioLowCount ++;
			GpioHighCount = 0;
		}
		else
		{
			GpioHighCount ++;
			GpioLowCount = 0;
		}
		
		if(GpioHighCount > GPIO_SLEEP_MODEM_COUNT) 
		{
			GpioHighCount = 0;
			if(tboxInfo.operateionStatus.isGoToSleep != 1)
			{
				tboxInfo.operateionStatus.wakeupSource = 0;
				lowPowerMode(1, 1);
				#ifdef GPIO_WAKE_DEBUG_LOG
				gpio_wake_log(tboxInfo.operateionStatus.isGoToSleep);
				#endif
			}
		}
		
		if(GpioLowCount > GPIO_SLEEP_MODEM_COUNT)
		{
			GpioLowCount = 0;
			if(tboxInfo.operateionStatus.isGoToSleep != 0)
			{
				tboxInfo.operateionStatus.wakeupSource = -1;
				lowPowerMode(0, 0);
#if MCU_SLEEP_VERSION
#else
				CMcu::GetInstance()->u8RemoteControlFlag = 0;
				printf("%s(%d): RemoteControlFlag %d\n", __func__, __LINE__, CMcu::GetInstance()->u8RemoteControlFlag);
#endif				
#ifdef GPIO_WAKE_DEBUG_LOG
				gpio_wake_log(tboxInfo.operateionStatus.isGoToSleep);
#endif
			}
		}
		if(CMcu::GetInstance()->u8GpsErrFlag == MCU_GPS_STATUS_ERR)
		{
			WakeUpMcuTimer++;
			if((WakeUpMcuTimer > 20) &&(WakeUpMcuTimer < 25))
			{
				WakeUpMcuTimer = 25;
				CMcu::GetInstance()->Set_mcuWakeupEventValue( STATUS_WAKEUP_EVENT );
				printf("jason add for wakeup event for 8090\r\n");
			}
			else if(WakeUpMcuTimer > 35)
			{
				WakeUpMcuTimer = 0;
				CMcu::GetInstance()->SendPowerSwitchSame();
				CMcu::GetInstance()->u8GpsErrFlag = MCU_GPS_STATUS_RECOVERY;
				printf("jason add for wakeup recovery for 8090\r\n");

			}
			
		}
		usleep(1000*200);
	}
	return ret;
}

#endif








