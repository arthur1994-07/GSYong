#ifndef SC_LOG_EDIT_BOX_H_
#define SC_LOG_EDIT_BOX_H_

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
 *
 * \bug 
 *
 */
class LogEditBox : public LogBase
{
public:
	LogEditBox(EM_LOG_TYPE emType, HWND hEditBox);
	virtual ~LogEditBox();

private:
	DWORD m_dwLogIndex;
	TCHAR m_arrayszBuffer[LOG_POOL_SIZE][LOG_BUFFER_SIZE];

protected:
	HWND m_hEditBox;

public:	
	virtual void write(const char* szLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
	virtual void write(const std::string& strLog, EM_LOG_LEVEL emLevel, bool bType=true, bool bTime=true) override;
};

}; // namespace sc

#endif //