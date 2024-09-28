#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "./NotifyClientBase.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../SigmaCore/DebugInclude.h"

INotifyClientBase::INotifyClientBase( 
    GLGaeaClient* pGaeaClient,
    const sc::MGUID& Guid, 
    const __time64_t& Date, 
    const NOTIFT_TYPE Type )
    : m_pGaeaClient(pGaeaClient)
    , m_Guid(Guid)
    , m_Date(Date)
    , m_Type(Type)
    , m_MessageBoxType(INotifyClientBase::TYPE_OKCANCEL)
{
}

INotifyClientBase::~INotifyClientBase()
{
}

std::string INotifyClientBase::GetDateStr() const
{
    return sc::time::DateTimeFormat(m_Date, false, false);
}

//! NOTIFY_TYPE의 인덱스와 NOTIFT_TYPE의 인덱스가 같아야 한다.
std::string INotifyClientBase::GetTypeStr() const
{
    return ID2GAMEWORD( "NOTIFY_TYPE", GetType() );
}