#ifndef _GPIO_WAKE_H_
#define _GPIO_WAKE_H_
#include <stdint.h>
#include "LedControl.h"
#include "TBoxDataPool.h"
//#include "WiFiControl.h"
#include "common.h"
#include "FAWACP.h"
//#include "mcuUart.h"
#include "mcu.h"




#define GPIO_WAKE_DEBUG_LOG	  (char*)"gpio wake"

#ifndef MCU_UART_DEVICE
#define MCU_UART_DEVICE	        (char*)"/dev/ttyHS1"    /* for mcu uart */
#endif 

#ifndef UART_SLEEP_CTL
#define UART_SLEEP_CTL			(char*)"/sys/class/tty/ttyHS1/device/power/control"
#endif


#ifndef MCU_FILE_NAME
#define MCU_FILE_NAME         (char*)"/data/MCU.bin"
#endif

#ifndef LOG_FILE
#define LOG_FILE			  (char*)"/data/mylog"
#endif

#ifndef LOG_UP_FILE
#define LOG_UP_FILE			  (char*)"/data/myuplog"
#endif
 
#define RET_OK       0
#define RET_FAIL     1
#define RET_INVALID  2


#define MCU_WAKE_4G_SLEEPSTATUS   0
#define MCU_WAKE_4G_WORKSTATUS   1

#define CPU_MIN_FREQ	(400000)
#define CPU_MAX_FREQ	(1305600)

//#define bool int
//#define false 0
//#define true 1


  


/*****************************************************************************
* Function Name : gipo_wakeup_init
* Description   : gpio唤醒脚的配置
* Input         : None
* Output        : None
* Return        : int 
* Auther        : ygg
* Date          : 2018.03.18
*****************************************************************************/
extern int gipo_wakeup_init();


/*****************************************************************************
* Function Name : modem_ri_notify_mcu
* Description   : 用于4g唤醒mcu
* Input         : None
* Output        : None
* Return        : int 
* Auther        : ygg
* Date          : 2018.03.18
*****************************************************************************/
extern int modem_ri_notify_mcu(void);
 
extern int modem_ri_notify_mcu_high(void);
extern int modem_ri_notify_mcu_low(void);

extern uint8_t  cat_gpio_value(unsigned int gpio,int *pStatus);

extern int uart_wakeup(unsigned char bwakeup);

extern uint8_t GPIO_GetMcuWake4GStatus(int *pStatus);

extern void Set_CpuFreqValue(     int Value );


/*****************************************************************************
* Function Name : lowPowerMode
* Description   : go to low power mode,
                  isSleep ->0,  go to sleep
                  isSleep ->1,  wake up system
                  isCloseWifi ->0, close
                  isCloseWifi ->1, open
	              isCloseWifi ->-1, Indicate the other source,
	                                so don't close wifi
* Input         : None
* Output        : None
* Return        : 0: success 
* Auther        : ygg
* Date          : 2018.03.18
*****************************************************************************/
extern int lowPowerMode(int isSleep, int isCloseWifi);


#endif

