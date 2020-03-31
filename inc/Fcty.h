#ifndef __FCTY_H__
#define  __FCTY_H__

#include "BaseThread.h"
#include "framework.h"
#include "QueueCommon.h"


#define FCTY_DEBUG 1

#if FCTY_DEBUG 	
	#define FCTYLOG(format,...) printf("== FCTY == FILE: %s, FUN: %s, LINE: %d "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define FCTY_NO(format,...) printf(format,##__VA_ARGS__)
#else	
	#define FCTYLOG(format,...)
	#define FCTY_NO(format,...)
#endif



#define FCTY_THREAD_PERIOD   	(500*1000)//MS


class CFcty : public TiotThread
{
	public:
	    static CFcty* GetInstance();
	    static void FreeInstance();
		CFcty();
		virtual ~CFcty();
		BOOL Init();
	    void Deinit();
		BOOL GetInitSts(void);
	    void Run();
		Framework*      mFramework;
		
	protected:
	    BOOL Processing();
	    BOOL TimeoutProcessing();
	    static CFcty* m_instance;
	    BOOL    m_binit;
};

#endif
