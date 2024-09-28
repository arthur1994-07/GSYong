#include "StdAfx.h"
#include "./LogBase.h"

#include "../DebugInclude.h"

namespace sc
{

LogBase::LogBase(EM_LOG_TYPE emType)
	: m_Type(emType)
	, m_bBugTrapLog(false)
{
}

LogBase::~LogBase()
{
}

} // namespace sc