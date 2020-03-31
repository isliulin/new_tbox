#ifndef  __TSP_H__
#define  __TSP_H__

#include "BaseThread.h"

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
#include "json.h"
#include "tspTCTL.h"


#define    TSP_THREAD_PERIOD     			(0.5)     			/*peroid   500ms*/
#define    TSP_SYNC_TIME_PERIOD           	(120)

#define APN1  (char*)"CMIOT"

#define APN2  (char*)"CMNET"

#define   TSP_WORK_MODE_MODEM_STATE_INIT   			 					0

#define   TSP_WORK_MODE_MODEM_STATE_CHECK_PS_CS    						1

#define   TSP_WORK_MODE_MODEM_STATE_DATA_CALL      						2

#define   TSP_WORK_MODE_MODEM_STATE_DATA_CALL_CHECK					3

#define   TSP_WORK_MODE_MQTT_STATE_DOWN_LOAD_PARAM					4

#define   TSP_WORK_MODE_MQTT_STATE_INIT									5

#define   TSP_WORK_MODE_MQTT_STATE_CONNECT								6

#define   TSP_WORK_MODE_MQTT_STATE_IDLE									7

#define   TSP_WORK_MODE_MQTT_STATE_DISCONNECT							8

#define   TSP_WORK_MODE_REQ_REBOOT										10


#define 	TSP_WORK_MODE_MODEM_WAIT_INIT_TIME_OUT	 					(TSP_THREAD_PERIOD * 20)	  	/* 10s */
#define 	TSP_WORK_MODE_MODEM_WAIT_PSCS_TIME_OUT    					(TSP_THREAD_PERIOD * 20)   	/* 10s */
#define 	TSP_WORK_MODE_MODEM_WAIT_DC_TIME_OUT	 					(TSP_THREAD_PERIOD * 20) 	/* 10s */
#define 	TSP_WORK_MODE_CHECK_TIME 					 					(TSP_THREAD_PERIOD * 10) 	/* 5s */

#define TSP_WORK_MODE_MODEM_DATACALL_MAX_COUNT	  					3	/**/


typedef struct
{
	uint8_t isSIMCardPulledOut;
	uint8_t isLteNetworkAvailable;
	uint8_t NetworkConnectionType;
	uint8_t wifiConnectionType;
	uint8_t networkRegSts;
	uint8_t signalStrength;
}NetworkStatus;
typedef struct
{
	uint8_t m_u8BokenValue[6];
}ST_BleValue;

typedef struct{
	uint8_t m_HOST[50];		//MQTT Server Host
  	uint16_t m_PORTS;
	uint16_t m_uReportFrequency;
	uint8_t m_PSK_ID[50];		
	uint8_t m_Car_Type[10];			
	uint8_t m_hexPSK[50];
	uint8_t m_strPSK[50];		
	uint16_t m_Alarm;
	uint16_t m_reportFrequency;
	bool m_LogSwitch;
	uint8_t m_PASSWARD[50];
	uint8_t m_CILENT_Id[50];		
       uint8_t m_USERNAME[50];		
       uint16_t m_PORT;		
       uint16_t m_KEEP_ALIVE;
       uint16_t m_MSG_MAX_SIZE;
       uint16_t m_TOPIC_MAX_SIZE;
	int  m_TestOrNormal;
}stMqttConfig;

class CTsp : public TiotThread
{
	public:
	    static CTsp* GetInstance();
	    static void FreeInstance();
		CTsp();
		virtual ~CTsp();
		BOOL Init();
	    	void Deinit();
	    	void Run();
	protected:
		void ReceiveQueueTestTask();
		void SendQueueTestTask();
	    BOOL Processing();
	    void TspWorkTask();
	    void ReceiveTspQueueTask();
	    void SendTspQueueTask();

	    BOOL TimeoutProcessing();
	    static CTsp* m_instance;
	    BOOL    m_binit;
	    uint8_t m_u8TspWorkMode;
	    NetworkStatus m_stNetStatus;
	    stMqttConfig m_stMqttConfig;
	    TimeStamp_t	m_tspTimestamp;//tsp
	    uint8_t MQTT_Topic[MAXTOPIC][TOPICLEN];
	    bool session;
	    struct mosquitto *mosq;
	    
	    BOOL m_ConnectedState;
	    int ID_Queue_FW_To_TSP;
	    int ID_Queue_TSP_To_FW;
	    //QueueInfo_ST m_stTspPublishData;
	    QueueInfo_ST m_stTspSendQueueData;
	    bool   m_LocalTimeSyncState;
	    bool   m_TimeSyncHeartReceiveFlag;
	    bool   m_MqttDisconnectReqFlag;
	    bool   m_BokenReqFlag;
	private:
		int Apn_init();
		int NetworkInit();
		int MQTTConnect();
		void InitAcpTBoxAuthData();
		int ShowdataTime(uint8_t *pos);
		void DownLoadMqttAuthPara();
		void Init_FAWACP_dataInfo();

		int InitTboxParamInfo();
		int ReadTboxParamInfo();
		int UpdateTboxParamInfo();

		int InitMqttParamInfo();
		int UpdateMqttParamInfo();
		BOOL ReadMqttParamInfo();
		void UpdataMQTTTopic(uint8_t * vin);
		static void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result);
		static void mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int mid);
		static void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
		time_t m_timeLastReport;
		time_t m_timeSendTimeSync;
		time_t m_timeReceiveTimeSync;

		/* queue relation */

};

#endif

