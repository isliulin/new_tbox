#ifndef __LOG_H__
#define __LOG_H__

#include "BaseThread.h"
#include "QueueCommon.h"
#include "framework.h"

class CLog : public TiotThread
{
	public:
	    static CLog* GetInstance();
	    static void FreeInstance();
		CLog();
		virtual ~CLog();
		BOOL Init();
	    	void Deinit();
	    	void Run();
	protected:
	    BOOL Processing();
	    BOOL TimeoutProcessing();
	    static CLog* m_instance;
	    BOOL    m_binit;
	    void SendQueueTask();
	    void ReceiveQueueTask();

};


#endif
