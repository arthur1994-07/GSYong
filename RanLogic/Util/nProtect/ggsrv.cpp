#include "pch.h"

#ifndef _DEBUG
#include "../../../=nProtect/ggsrv25.h"
#endif

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#ifndef _DEBUG
    GGAUTHS_API void NpLog(int mode, char* msg)
    {
	    if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) //Select log mode. 
    #ifdef WIN32
		    OutputDebugString(msg);
    #else
		    //printf(msg);
		    CConsoleMessage::GetInstance()->WriteNPROTECT(msg);
    #endif
    }
#endif

#ifndef _DEBUG
    GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
    {
	    //printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", 
	    //		report->nType==1?"GameGuard Ver":"Protocol Num", 
	    //		report->dwBefore,
	    //		report->dwNext);

	    sc::writeLogError(
            sc::string::format(
                "NPROTECT:GGAuth version update [%1%] : [%2%] -> [%3%] \n",
                report->nType==1?"GameGuard Ver":"Protocol Num", 
                report->dwBefore,
                report->dwNext));
    };
#endif