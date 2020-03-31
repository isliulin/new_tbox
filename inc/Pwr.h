#ifndef   __LTE_H__
#define 	__LTE_H__

#include "BaseThread.h"
#include "QueueCommon.h"
#include "framework.h"

class CPwr : public TiotThread
{
public:
    static CPwr* GetInstance();
    static void FreeInstance();
	CPwr();
	virtual ~CPwr();
	BOOL Init();
    	void Deinit();
    	void Run();
protected:
    BOOL Processing();
    BOOL TimeoutProcessing();
    static CPwr* m_instance;
    BOOL    m_binit;
    void SendQueueTestTask();
    void ReceiveQueueTestTask();
};
#endif
