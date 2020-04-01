#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/reboot.h>
#include "common.h"
#include "mcu.h"
#include "TBoxDataPool.h"
//#include "LTEModuleAtCtrl.h"
#include "ATControl.h"

//#include "GBT32960.h"
//#include "OTAUpgrade.h"
#include "LedControl.h"
#include "AdcVoltageCheck.h"
//#include "FTPSClient.h"
#include "DnsResolv.h"
#include "DEVControl.h"
#include "VoiceCall.h"
#include "simcom_common.h"
//#include "WiFiControl.h"
#include "SMSControl.h"
#include "DataCall.h"
#include "WDSControl.h"
#include "NASControl.h"
#include "GpioWake.h"
#include "dsi_netctrl.h"
#include "FAWACP.h"
#include "OTAWiFi.h"
//#include "IVI_Communication.h"
#include <sys/syslog.h>

#include "bluetooth.h"

#include "FactoryPattern_Communication.h"
//#include "MQTTACP.h"
#include "Pwr.h"
#include "mcu.h"
#include "tsp.h"
#include "Ble.h"
#include "Log.h"
#include "framework.h"
#include "Nvm.h"
#include "Ota.h"

#define CHE_YIN_NEW_TSP   1 //just for test

#if   (0 == CHE_YIN_NEW_TSP)
#define TSP_ADDR_STRING  (unsigned char*)"tbox.vcyber.com"
#else
#define TSP_ADDR_STRING  (unsigned char*)"vtbox.i-morefun.com"
#endif
#if 1
//天津一汽APN
//生产测试用APN
//#define APN1  "zwzgyq02.clfu.njm2mapn"
//#define APN2  "UNIM2M.NJM2MAPN"
//量产APN
#define APN1  (char*)"CMIOT"
#define APN2  (char*)"CMNET"
#endif

static uint8_t u8ModemWorkState = MODEM_STATE_INIT;
static uint8_t u8ModemCfunTimes = 0;
TBoxDataPool *dataPool = NULL;
//LTEModuleAtCtrl *LteAtCtrl = NULL;
//GBT32960 *p_GBT32960 = NULL;
//IVI_Communication iviCommunication;
FactoryPattern_Communication factorypattern;

extern bool testecall;
extern Timelog_t timelog;
char target_ip[16] = {0};
//unsigned char TspTargetIpGetNewFlag = 0;
uint8_t TspReDatacallReqFlag = 0;

//unsigned char SocketReDataCallState = 0; /* 0 == no datacall;  1 == datacall ing*/
extern bool flaftime4g;
char ModelLogSigBuff[512] = {0};
//extern bool alive;

/*****************************************************************************
* Function Name : apn_init
* Description   : apn 初始化,及设置apn
* Input			: None
* Output        : None
* Return        : 0:success, -1:failed
* Auther        : ygg
* Date          : 2018.03.19
*****************************************************************************/
int apn_init() {
  int ret;
  int apn_index = 4;
  char apn[30] = {0};
  char username[64] = {0};
  char password[64] = {0};
  int pdp_type;
  char *apntest = (char *) "cmnet";

  int retval;
  uint8_t i;
  char strResult[128];

  for (i = 0; i < 3; i++) {
    memset(strResult, 0, sizeof(strResult));
    retval = sendATCmd((char *) "AT+COPS?", (char *) "OK", strResult, sizeof(strResult), 2000);
    if (retval > 0) {
      if (strstr(strResult, "UNICOM")) {
        apntest = (char *) "3gnet";
      } else if (strstr(strResult, "CMCC")) {
        apntest = (char *) "cmnet";
      } else if (strstr(strResult, "CHN-CT")) {
        apntest = (char *) "ctnet";
      } else {
        apntest = (char *) "cmnet";
      }
      break;
    } else {
      apntest = (char *) "cmnet";
    }
  }

  //apn

  if (wds_init() == 0)//new sdk interface zhujw
    DEBUGLOG("wds_qmi_init success!\n");

  //profile_index: 4-->Private network; 6-->Public Network
  ret = get_apnInfo(apn_index, &pdp_type, apn, username, password);
  if (ret == FALSE) {
    printf("wds_GetAPNInfo Fail\n");
  } else {
    printf(">>>>>> apn[%d]=%s, pdp_type = %d, username=%s, password=%s\n",
           apn_index,
           apn,
           pdp_type,
           username,
           password);
  }
  if (strcmp(apn, APN1) != 0)
    set_apnInfo(4, 0, APN1, NULL, NULL);//new sdk interface zhujw
  apn_index = 6;
  memset(apn, 0, sizeof(apn));
  memset(username, 0, sizeof(username));
  memset(password, 0, sizeof(password));
  ret = get_apnInfo(apn_index, &pdp_type, apn, username, password); //new sdk interface
  if (ret == FALSE) {
    printf("wds_GetAPNInfo Fail\n");
  } else {
    printf(">>>>>> apn[%d]=%s, pdp_type = %d, username=%s, password=%s\n",
           apn_index,
           apn,
           pdp_type,
           username,
           password);
  }
#if 1
  if (strcmp(apn, APN1) != 0)
    set_apnInfo(6, 0, APN1, NULL, NULL);

#else

  if(apntest != NULL)
  {
      if(strcmp(apn, apntest) != 0)
      set_apnInfo(6, 0, apntest, NULL, NULL);
  }
  else
  {
      set_apnInfo(6, 0, NULL, NULL, NULL);
  }
#endif
  return 0;
}

/*****************************************************************************
* Function Name : mcuInitThread
* Description   : mcu 线程初始化
* Input			: void *args 
* Output        : None
* Return        : NULL
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
void *mcuInitThread(void *args) {
  pthread_detach(pthread_self());

  return NULL;
}

/*****************************************************************************
* Function Name : bleInitThread
* Description   : ble 线程初始化
* Input			: void *args 
* Output        : None
* Return        : NULL
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
void *bleInitThread(void *args) {
  pthread_detach(pthread_self());
  //bleUart *p_bleUart = new bleUart();
  bleUart::Get_BleInstance()->Run_BleTask();

  return NULL;
}

/*****************************************************************************
* Function Name : dataCallDailCheck
* Description   : 私有网络断开重复拨号
* Input			: void *args 
* Output        : None
* Return        : NULL
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
void *dataCallDailCheck(void *args) {
  datacall_info_type datacall_info;
  pthread_detach(pthread_self());
  while (1) {
    //get_datacall_info(&datacall_info);
    get_datacall_info_by_profile(4, &datacall_info);//new sdk interface
    if (datacall_info.status == DATACALL_DISCONNECTED) {
      if (tboxInfo.networkStatus.isLteNetworkAvailable != NETWORK_NULL)
        tboxInfo.networkStatus.isLteNetworkAvailable = NETWORK_NULL;
      //dataCallDail();
      start_dataCall(app_tech_auto, DSI_IP_VERSION_4, 4, APN1, NULL, NULL);//new sdk interface
      sleep(5);
    } else {
      if (tboxInfo.networkStatus.isLteNetworkAvailable != NETWORK_LTE)
        tboxInfo.networkStatus.isLteNetworkAvailable = NETWORK_LTE;
      sleep(5);
#if 0
      int ret;
        char ip[16]={0};
      ret = query_ip_from_dns("www.baidu.com",
                             datacall_info.pri_dns_str,
                             datacall_info.sec_dns_str,
                             ip);
          printf("baidu ip=%s len=%d\n",ip, strlen(ip));
          memset(ip,0,sizeof(ip));
      ret = query_ip_from_dns("www.yahoo.com",
                             datacall_info.pri_dns_str,
                             datacall_info.sec_dns_str,
                             ip);
          printf("yahoo ip=%s len=%d\n",ip, strlen(ip));
#endif
    }
  }

  return NULL;
}

void tboxsignal_log(uint8_t signal) {
  FILE *fd;
  struct stat buf;
  stat("./signal.log", &buf);
  if (buf.st_size > 10 * 1000 * 1000) { // 超过 10 兆则清空内容
    fd = fopen("/data/signal.log", "w");
  } else {
    fd = fopen("/data/signal.log", "at");
  }

  if (NULL == fd) {
    return;
  }
  char szLine[512] = {0,};

  struct tm *p_tm = NULL; //时间的处理
  time_t tmp_time;
  tmp_time = time(NULL);
  p_tm = gmtime(&tmp_time);
  int nLen1;

  nLen1 = sprintf(szLine,
                  "tbox signal is %d on \n[%04d-%02d-%02d %02d:%02d:%02d]\n",
                  signal,
                  p_tm->tm_year + 1900,
                  p_tm->tm_mon,
                  p_tm->tm_mday,
                  p_tm->tm_hour,
                  p_tm->tm_min,
                  p_tm->tm_sec);

  // int nLen1 = sprintf(szLine, "tbox crash on \n[%04d-%02d-%02d %02d:%02d:%02d]\n", p_tm->tm_year-90, p_tm->tm_mon,  p_tm->tm_mday,  p_tm->tm_hour,  p_tm->tm_min, p_tm->tm_sec);
  fwrite(szLine, 1, strlen(szLine), fd);

  fflush(fd);
  fclose(fd);
  fd = NULL;
}

/*****************************************************************************
* Function Name : gpioWakeThread
* Description   : gpio 唤醒脚检测
* Input			: void *args 
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
void *gpioWakeThread(void *args) {
  pthread_detach(pthread_self());
  gipo_wakeup_init();

  return (void *) 0;
}

#if 0
/*****************************************************************************
* Function Name : GBT32960Thread
* Description   : gb32960线程初始化
* Input			: void *args 
* Output        : None
* Return        : 0:success
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
void *GBT32960Thread(void *args)
{
  pthread_detach(pthread_self());
    p_GBT32960 = new GBT32960();
  return (void *)0;
}
#endif

void *FAWACPThread(void *args) {
  pthread_detach(pthread_self());

  CFAWACP::GetInstance()->RUN_FAWACP_TASK();

  return (void *) 0;
}

#if 0
void *MQTTThread(void *args)
{
  pthread_detach(pthread_self());

  MQTT::GetInstance()->RUN_MQTT_TASK();

  return (void *)0;
}
#endif
#if 0
void *IVIThread(void *args)
{
  pthread_detach(pthread_self());
  while(1)
  {
    iviCommunication.IVI_Communication_Init();
    sleep(2);
  }
  return (void *)0;
}
#endif

void *factoryThread(void *args) {
  pthread_detach(pthread_self());
  while (1) {
    factorypattern.FactoryPattern_Communication_Init();
    sleep(2);
  }
  return (void *) 0;
}

/*
unsigned char GetTspNewIpFlag(void)
{
	return TspTargetIpGetNewFlag;
}

void SetTspNewIpFlag(unsigned char NewFlag)
{
	 TspTargetIpGetNewFlag = NewFlag;
}
*/

uint8_t GetModemWorkOn(void) {
  return (u8ModemWorkState == MODEM_STATE_IDLE_CHECK);
}

uint8_t GetTspReDatacallFlag(void) {
  return TspReDatacallReqFlag;
}

void SetTspReDatacallFlag(uint8_t ReDatacallFlag) {
  TspReDatacallReqFlag = ReDatacallFlag;
}
int NetworkInit() {
  int ret = 0;
  char buff[100];
  char imei[100];
  char cimi[100];
  char iccid[100];


  // get IMEI
  memset(imei, 0, sizeof(imei));
  dataPool->getPara(IMEI_INFO, (void *) imei, sizeof(imei));
  printf("imei:%s\n", imei);

  memset(buff, 0, sizeof(buff));
  if (getIMEI(buff, sizeof(buff)) == 0) {
    printf("IMEI:%s\n", buff);

    if (memcmp(imei, buff, (strlen(buff))) != 0) {
      dataPool->setPara(IMEI_INFO, (void *) buff, strlen(buff));
    }
  }

  memset(cimi, 0, sizeof(cimi));
  dataPool->getPara(CIMI_INFO, (void *) cimi, sizeof(cimi));
  printf("cimi:%s\n", cimi);

  memset(buff, 0, sizeof(buff));
  if (getCIMI(buff, sizeof(buff)) == 0) {
    printf("CIMI:%s\n", buff);

    if (memcmp(cimi, buff, (strlen(buff))) != 0) {
      dataPool->setPara(CIMI_INFO, (void *) buff, strlen(buff));
    }
  }

  if (getCPIN() > 0) {
    tboxInfo.networkStatus.isSIMCardPulledOut = 1;
    //enableMobileNetwork(1*2, networkConnectionStatus);

    memset(iccid, 0, sizeof(iccid));
    dataPool->getPara(SIM_ICCID_INFO, (void *) iccid, sizeof(iccid));

    // get ICCID
    memset(buff, 0, sizeof(buff));
    if (getICCID(buff, sizeof(buff)) == 0) {
      printf("ICCID:%s\n", buff);
      if (memcmp(iccid, buff, (strlen(buff))) != 0) {
        dataPool->setPara(SIM_ICCID_INFO, (void *) buff, strlen(buff));
      }
    }
  } else {
    printf("no sim card\n");
    tboxInfo.networkStatus.isSIMCardPulledOut = 0;
    ret = -1;
  }

  return ret;
}

void process_simcom_ind_message(simcom_event_e event, void *cb_usr_data) {
  int i;
  int retval;
  switch (event) {
    case SIMCOM_EVENT_VOICE_CALL_IND: {
      //唤醒系统
      if (tboxInfo.operateionStatus.isGoToSleep == 0) {
        printf("0000000000 voice call\n");
        //tboxInfo.operateionStatus.isGoToSleep = 1;
        //tboxInfo.operateionStatus.wakeupSource = 1;
        //lowPowerMode(1, 1);
        //modem_ri_notify_mcu();
      }

      //iviCommunication.TBOX_Voicall_State(cb_usr_data);

#if 1 //new sdk interface
      call_info_type call_list;
      memcpy(&call_list, cb_usr_data, sizeof(call_list));

      for (i = 0; i < QMI_VOICE_CALL_INFO_MAX_V02; i++) {
        if (call_list.call_info[i].call_id != 0) {
          printf(">>>>>>voice callback:\n");
          printf("index[%d], call_id=%d, state=%d, direction = %d, number=%s\n",
                 i,
                 call_list.call_info[i].call_id,
                 call_list.call_info[i].call_state,
                 call_list.call_info[i].direction,
                 strlen(call_list.call_info[i].phone_number) >= 1 ? call_list.call_info[i].phone_number : "unkonw"
          );

          printf("<<<<<<<<<<< \n");

          if (call_list.call_info[i].call_state == CALL_STATE_END_V02) {
            tboxInfo.operateionStatus.phoneType = 0;
            printf("$$$call end");
          }
          if (call_list.call_info[i].call_state == CALL_STATE_CONVERSATION_V02) {
            printf("$$$call working");
            if (testecall) {
              for (i = 0; i < 3; i++) {
                //retval = sendATCmd((char*)"AT+CLOOPBACK=3,1", (char*)"OK", NULL, 0, 5000);
                retval = sendATCmd((char *) "AT+CLOOPBACK=3,1", (char *) "OK", NULL, 0, 5000);
                //sendATCmd((char*)"AT+CMICGAIN=7", (char*)"OK", NULL, 0, 5000);
                //sendATCmd((char*)"at+cwiic=0x34,0x6c,0x40,1", (char*)"OK", NULL, 0, 5000);
                if (retval > 0)
                  break;
              }
            }
          }
        }
      }
#endif
    }
      break;

    case SIMCOM_EVENT_SMS_PP_IND: {

      if (tboxInfo.operateionStatus.isGoToSleep == 0) {
        printf("11111111111111 sms \n");
        //tboxInfo.operateionStatus.isGoToSleep = 1;
        //tboxInfo.operateionStatus.wakeupSource = 2;
        //lowPowerMode(1, 1);
        //modem_ri_notify_mcu();
      }

      sms_info_type sms_info;
      memcpy((void *) &sms_info, cb_usr_data, sizeof(sms_info));

      printf("\n-----------receive message --------------------------\n");
      printf("address=%s\n", sms_info.source_address);
      for (i = 0; i < strlen(sms_info.message_content); i++) {
        printf("0x%02X ", sms_info.message_content[i]);
      }
      printf("\n");
    }
      break;

    case SIMCOM_EVENT_NETWORK_IND: {
      network_info_type network_info;
      memcpy((void *) &network_info, cb_usr_data, sizeof(network_info));
      //   printf("\n---------network info---------------------------\n");
      /*  printf("network_info: register=%d, cs=%d, ps=%d,radio_if=%d\n",
                 network_info.registration_state,
                 network_info.cs_attach_state,
                 network_info.ps_attach_state,
                 network_info.radio_if_type);	*/
      if (network_info.registration_state == NAS_REGISTERED_V01) {
        if (tboxInfo.networkStatus.networkRegSts != 1)
          tboxInfo.networkStatus.networkRegSts = 1;
#if 0
        if(lteLedStatus != 2)
          lte_led_blink(500,500);
#endif
      }
      if (network_info.registration_state != NAS_REGISTERED_V01) {
        if (tboxInfo.networkStatus.networkRegSts == 1)
          tboxInfo.networkStatus.networkRegSts = 0;
#if 0
        if(lteLedStatus != 1)
          lte_led_on();
#endif
      }
    }
      break;
    case SIMCOM_EVENT_DATACALL_CONNECTED_IND:
    case SIMCOM_EVENT_DATACALL_DISCONNECTED_IND:  //new sdk interface
    {
      int ret;
      char target_ip_new[16] = {0};
      datacall_info_type datacall_info;
      //get_datacall_info(&datacall_info);
      get_datacall_info_by_profile(4, &datacall_info);//new sdk interface

      if (datacall_info.status == DATACALL_CONNECTED) {
        int use_dns = 1;
        printf("datacall_ind1: if_name=%s,ip=%s,mask=%d\n",
               datacall_info.if_name,
               datacall_info.ip_str,
               datacall_info.mask);
        printf("datacall_ind2: dns1=%s,dns2=%s,gw=%s\n",
               datacall_info.pri_dns_str,
               datacall_info.sec_dns_str,
               datacall_info.gw_str);
        if (use_dns) {
          //天津一汽:"znwl-uat-cartj.faw.cn"
          //ret = query_ip_from_dns("znwl-uat-cartj.faw.cn", datacall_info.pri_dns_str ,datacall_info.pri_dns_str , target_ip_new);

          ret = query_ip_from_dns(TSP_ADDR_STRING,
                                  datacall_info.pri_dns_str,/*datacall_info.pri_dns_str*/
                                  datacall_info.sec_dns_str,
                                  target_ip_new);

          if (ret != 0) {
            printf("query ip fail\n");
            ret = query_ip_from_dns(TSP_ADDR_STRING,
                                    (char *) "114.114.114.114",
                                    (char *) "114.114.114.114",
                                    target_ip_new);
            if (ret != 0) {
              printf("query ip fail\n");
              break;
            }
          }

          printf("target_ip:%s\n", target_ip_new);

          set_host_route(target_ip, target_ip_new, datacall_info.if_name);
#if 0// jason add for add default route
          system("route del default");
          sprintf(cmd,"route add default %s", datacall_info.if_name);
          system(cmd);
#endif
          strncpy(target_ip, target_ip_new, sizeof(target_ip_new));
          if (strlen(target_ip) != 0) {
#if 0
            //TspTargetIpGetNewFlag = 1;
            SetTspNewIpFlag(1);
#endif
          }
        } else {
          set_host_route(target_ip, target_ip, datacall_info.if_name);
        }
      }
#if 0
      else
       {
         SetSocketReDatacallFlag(1);
       }
      SetSocketReDatacallState(0);
#endif
    }

      break;
    default:break;
  }
}

extern int uart_debug();

#if 0
void do_enter_recovery_reset(void)
{
    sleep(2);
    syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, 
        LINUX_REBOOT_CMD_RESTART2, "recovery");
}
#endif

void do_enter_backup_reset(void) {
  sleep(2);
  system("reboot");
}

#if 0 //zhujw
int enter_recovery_mode_delay(void)
{
  pthread_t id;
  int ret;
  ret = pthread_create(&id, NULL, (void *)do_enter_recovery_reset, NULL);
  return 0;
}
#endif

/* call for user, update module*/
int system_update() {
  printf("system_update in\n");
  //set flag to send DM session(non fota update)
  FILE *fp = NULL;
  system("mkdir -p /data/dme");
  system("touch /data/dme/start_DM_session");
  fp = fopen("/data/dme/start_DM_session", "w+");
  if (fp == NULL) {
    return -1;
  } else {
    fclose(fp);
    system("sync");
    /*modify to not delete temp files
    system("rm /data/dme/DMS.tre");
    system("rm /data/dme/DevDetail.tre");
    system("rm /data/dme/DevInfo.tre");
    system("rm /data/dme/aclData.acl");
    system("rm /data/dme/eventlist.cfg");
    system("rm /data/dme/init_active_date_file");
    system("rm /data/dme/rs_log.txt");
    system("rm /data/dme/session.log");*/
  }

  system("echo off > /sys/power/autosleep");
  ////(void)enter_recovery_mode_delay();
  return 0;
}

#define MODE_WAIT_INIT_TIME_OUT   10  /*10s */
#define MODE_WAIT_PSCS_TIME_OUT    10 /*10s */
#define MODE_WAIT_DC_TIME_OUT   10  /*10S */

#define MODE_DATACALL_MAX_COUNT    3  /**/

CPwr *gl_CPwr_instance = NULL;
CMcu *gl_CMcu_instance = NULL;
CTsp *gl_CTsp_instance = NULL;
CBle *gl_CBle_instance = NULL;
CLog *gl_CLog_instance = NULL;
CNvm *gl_CNvm_instance = NULL;
COta *gl_COta_instance = NULL;

Framework *gl_CFrameWork_instance = NULL;

int main(int argc, char *argv[]) {

  printf("[BUILD_TIME] %s %s \n", __DATE__, __TIME__);

#if 0
  static unsigned char datacalltimes = 0;
  static unsigned char rebootflag = 0;
  static unsigned char datacallcount = 0;
  static unsigned char SigCount	= 0;
  static unsigned int NetWorkDisconnectCount = 0;
  static unsigned int NetWorkConnectCount = 0;
#endif

#if 0
  static int MaxSignalValue = 0;
  static int MinSignalValue = 0;
  static int NasCsstate = 0;
  static int NasPsstate = 0;
  static unsigned char u8ModemWaitTime = 0;
  static unsigned char u8ModemDataCallCount = 0;
  static unsigned char glDebugLogSwitch = 0;
  static unsigned char u8ModemCfunEnable = 0;
  timelog.time4g = 0;
  uint8_t u8SendCount = 0;

  int ret;
  uint32_t count = 0;
  char sysVersion[128] = {0};
  pthread_t mcuInitThreadId;
  pthread_t bleInitThreadId;
  //pthread_t GBT32960ThreadId;
  pthread_t gpioWakeId;
  pthread_t FAWACPThreadId;
  //pthread_t IVIThreadId;

  pthread_t factoryThreadId;
  pthread_t MQTTThreadId;

  pthread_attr_t IVIAttr;
  struct sched_param sched;

  ret = pthread_attr_init(&IVIAttr);
  if (ret != 0)
    printf("pthread_attr_init error!\n");

  sched.sched_priority = 99;
  pthread_attr_setschedpolicy(&IVIAttr, SCHED_FIFO); //SCHED_RR;
  pthread_attr_setschedparam(&IVIAttr, &sched);
  pthread_attr_setinheritsched(&IVIAttr, PTHREAD_EXPLICIT_SCHED);

  //uart_debug();
  openlog("SIMCOM_DEMO", LOG_PID, LOG_USER);

  getSoftwareVerion(sysVersion, sizeof(sysVersion));
  DEBUGLOG("System version:%s", sysVersion);

  dataPool = new TBoxDataPool();

#endif
  //LteAtCtrl = new LTEModuleAtCtrl();
#if 0
  //atCtrlInit();
   atctrl_init();//new sdk interface
   NetworkInit();
  if(apn_init() == 0)
     DEBUGLOG("apn_init success!");

  if(datacall_init() == 0)//new sdk interface
    DEBUGLOG("dataCall_init success!");

  if(voice_init() == 0)
     DEBUGLOG("voiceCall_init success!");

  if(sms_init((sms_ind_cb_fcn)process_simcom_ind_message) == 0)	 //new sdk interface
    DEBUGLOG("message_init success!");
  if(nas_init() == 0)
    DEBUGLOG("nas_init success!");
#endif

  bleUart::Get_BleInstance();
  CFAWACP::GetInstance();

  //MQTT::GetInstance();
  gl_CFrameWork_instance = Framework::GetInstance();
  gl_CPwr_instance = CPwr::GetInstance();
  gl_CMcu_instance = CMcu::GetInstance();
  gl_CTsp_instance = CTsp::GetInstance();
  gl_CBle_instance = CBle::GetInstance();
  gl_CLog_instance = CLog::GetInstance();
  gl_CNvm_instance = CNvm::GetInstance();
  gl_COta_instance = COta::GetInstance();

  if (gl_CFrameWork_instance != NULL) {
    if (gl_CFrameWork_instance->Init()) {
      gl_CFrameWork_instance->Run();
    } else {
      printf("gl_CFrameWork_instance->Init() fail\r\n");
    }
  }
#if 1
  if (gl_CPwr_instance != NULL) {
    if (gl_CPwr_instance->Init()) {
      gl_CPwr_instance->Run();
    } else {
      printf("gl_CPwr_instance->Init() \r\n");
    }
  }
#endif

#if 1
  if (gl_CMcu_instance != NULL) {
    if (gl_CMcu_instance->Init()) {
      gl_CMcu_instance->Run();
    } else {
      printf("gl_CMcu_instance->Init() \r\n");
    }
  }

#endif
#if 0
  if (gl_CTsp_instance != NULL) {
    if (gl_CTsp_instance->Init()) {
      gl_CTsp_instance->Run();
    } else {
      printf("gl_CTsp_instance->Init() \r\n");
    }
  }
#endif

#if 0
  if(gl_CBle_instance != NULL)
  {
    if(gl_CBle_instance->Init())
    {
      gl_CBle_instance->Run();

    }
    else
    {
      printf("gl_CBle_instance->Init() \r\n");
    }
  }
#endif

#if 0
  if(gl_CLog_instance != NULL)
  {
    if(gl_CLog_instance->Init())
    {
      gl_CLog_instance->Run();

    }
    else
    {
      printf("gl_CLog_instance->Init() \r\n");
    }
  }
#endif
#if 1
  if (gl_CNvm_instance != NULL) {
    if (gl_CNvm_instance->Init()) {
      gl_CNvm_instance->Run();

    } else {
      printf("gl_CNvm_instance->Init() \r\n");
    }
  }

#endif
#if 0
  if(gl_COta_instance != NULL)
  {
    if(gl_COta_instance->Init())
    {
      gl_COta_instance->Run();

    }
    else
    {
      printf("gl_COta_instance->Init() \r\n");
    }
  }

#endif

#if 0
  ret = pthread_create(&mcuInitThreadId, NULL, mcuInitThread, NULL);
  if(0 != ret)
  {
    printf("can't create thread: %s\n",strerror(ret));
    exit(-1);
  }


  system("echo on > /sys/class/tty/ttyHS0/device/power/control");

  ret = pthread_create(&bleInitThreadId, NULL, bleInitThread, NULL);
  if(0 != ret)
  {
    printf("can't create thread: %s\n",strerror(ret));
    exit(-1);
  }

#endif
#if 0
  ret = pthread_create(&dataCallDailId, NULL, dataCallDailCheck, NULL);
  if(0 != ret)
  {
    printf("can't create thread dataCallDailCheck : %s\n",strerror(ret));
    exit(-1);
  }
#endif

#if 0
  ret = pthread_create(&gpioWakeId, NULL, gpioWakeThread, NULL);
  if(0 != ret)
  {
    printf("can't create thread gpioWakeThread : %s\n",strerror(ret));
    exit(-1);
  }
#endif

#if 0
  ret = pthread_create(&GBT32960ThreadId, NULL, GBT32960Thread, NULL);
  if(0 != ret)
  {
    printf("can't create thread GBT32960Thread : %s\n",strerror(ret));
    exit(-1);
  }
#endif
#if 0
  ret = pthread_create(&FAWACPThreadId, NULL, FAWACPThread, NULL);
  if(0 != ret)
  {
    printf("can't create FAWACPThread:%s\n",strerror(ret));
    exit(-1);
  }
#endif
#if 0
  ret = pthread_create(&MQTTThreadId, NULL, MQTTThread, NULL);
  if(0 != ret)
  {
    printf("can't create MQTTThread:%s\n",strerror(ret));
    exit(-1);
  }
#endif

#if 0
  {
    ret = pthread_create(&IVIThreadId, &IVIAttr, IVIThread, NULL);
    if(0 != ret)
    {
      printf("can't create thread: %s\n",strerror(ret));
      exit(-1);
    }
    else
      printf("*****************IVI creat OK");
  }
#endif
#if 0
  ret = pthread_create(&factoryThreadId, NULL, factoryThread, NULL);
  if(0 != ret)
  {
    printf("can't create thread: %s\n",strerror(ret));
    exit(-1);
  }
  else
  {
    printf("*****************FactoryTest creat OK");
  }

#endif
#if 0
  //dataCallDail();//old sdk interface
  start_dataCall(app_tech_auto, DSI_IP_VERSION_4, 4,APN1,NULL,NULL);//new sdk interface zhujw
#endif

//  time_t NewTime = 0;
//  datacall_info_type datacall_info;
//  nas_serving_system_type_v01 nas_status;

//	Set_CpuFreqValue(CPU_NAX_FREQ);
  while (1) {

    Framework::GetInstance()->fw_NvmSendQueue(ID_FW_2_NVM_SET_CONFIG);
    sleep(5);
  }

  return 0;
}


