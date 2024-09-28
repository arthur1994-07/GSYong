#ifndef _CORE_COMMON_LOG_CONSOLE_H_
#define _CORE_COMMON_LOG_CONSOLE_H_

#pragma once

#include "./LogDefine.h"
#include "./LogBase.h"

namespace sc
{
/**
 * \ingroup sc
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2007.03.14
 *
 * \author Jgkim
 *
 * \history
 * 2007.03.14 Jgkim Create
  *
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 * 
 * \todo 
 * 나중에 만들자 ㅡ.ㅡ;
 *
 * \bug 
 *
 */

class LogConsole : public LogBase
{
public:
	    LogConsole(void);
virtual ~LogConsole(void);

        bool    Init(const TCHAR* pzName);
        void    Destroy();

virtual void    write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
virtual void    write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;

protected:
        void    SetWindow(int iWidth = 600, int iHeight = 480);

private:
        HANDLE  m_hConsole;
};

}; // end of namespace CORE_COMMON
#endif // _CORE_COMMON_LOG_CONSOLE_H_
