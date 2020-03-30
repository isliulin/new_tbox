#include <stdint.h>
#include "tspTCTL.h"

#include "framework.h"


//static Framework* Framework::m_instance = NULL;
Framework*      mFramework;

uint8_t tspTCTL_Parse(uint8_t *pBuf, uint8_t len, fwTCTL_RemoteCtrl_UN *pData)
{
	//tspTCTL_RemoteCtrl_UN  	g_tspTctl;

		uint8_t ret = RET_FAIL;
		int i= 0;
		if((NULL == pBuf) || (len > TCTL_RemoteCtrl_DATA_LEN)||(pData == NULL))
		{	
			return RET_INVALID;
		}
		else
		{


//			memcpy(pData->Buf, pBuf, len);
//			msgsnd(mFramework->ID_Queue_TSP_To_FW,&tspQueueInfo,sizeof(fwTCTL_RemoteCtrl_UN)+2,IPC_NOWAIT);
//			Framework::Snd_Queue_Tsp_To_FW((void *)*g_stTctl.stTctlInfo.Ctrl, len - 40);
//			ret = RET_OK;
			//memcpy(g_tspTctl.Buf,pBuf,len);
			memcpy(pData->Buf, pBuf+3, len -3);
			ret = RET_OK;
		}
	
		return ret;

}

