#include "pch.h"
#include "../SigmaCore/MsgPack/MinMsgPack.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Json/MinJson.h"
#include "../SigmaCore/Util/DateTime.h"

#include "../EngineLib/DxTools/DxClubMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"

#include "../MfcExLib/RanFilter.h"

#include "../RanLogic/Msg/ClubMsg.h"
#include "../RanLogic/Msg/JsonMsgDefine.h"
#include "../RanLogic/Club/ClubLogManClient.h"

#include "./Club/GLClubClient.h"
#include "./Land/GLLandManClient.h"
#include "./Stage/DxGlobalStage.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLGaeaClient::GetClubLog(club::EM_LOG_TYPE LogType, WORD Page, WORD PageSize, std::vector<club::LOG_DATA>& Result)
{   
    DWORD CurTime = ::GetTickCount();
    // 마지막 update 시간이 1분이 지났으면 새롭게 요청한다.
    if ((CurTime - m_pClubLog->GetLastUpdateTime()) > 60000)
    {
        ReqClubLog(LogType, Page, PageSize*5);
        m_pClubLog->SetLastUpdateTime(CurTime);         
    }

    m_pClubLog->Search(Result, Page, PageSize, LogType);
    if (!Result.empty())
        return true;
    else
        return false;
}

lua_tinker::table GLGaeaClient::GetClubLogText(club::EM_LOG_TYPE LogType, WORD Page, WORD PageSize)
{
	lua_tinker::table tbClubLog( GLWidgetScript::GetInstance().GetLuaState() );
	lua_tinker::table tbClubLogText( GLWidgetScript::GetInstance().GetLuaState() );
	lua_tinker::table tbClubLogDate( GLWidgetScript::GetInstance().GetLuaState() );

	std::vector<club::LOG_DATA> Result;
	DWORD CurTime = ::GetTickCount();
	// 마지막 update 시간이 1분이 지났으면 새롭게 요청한다.
	if ((CurTime - m_pClubLog->GetLastUpdateTime()) > 60000)
	{
		ReqClubLog(LogType, Page, PageSize*5);
		m_pClubLog->SetLastUpdateTime(CurTime);         
	}
	m_pClubLog->Search(Result, Page, PageSize, LogType);

	int Size(Result.size());
	std::string _time("");	
	for(int index = 0; index < Size; ++index)
	{	
		_time = sc::time::DateTimeFormat(Result[index].m_LogDate, true, false);
		tbClubLogDate.set(index + 1, _time.c_str());
		tbClubLogText.set(index + 1, Result[index].m_LogText.c_str());
	}
	
	tbClubLog.set("Date", tbClubLogDate);
	tbClubLog.set("LogText", tbClubLogText);

	return tbClubLog;
}

void GLGaeaClient::ClubLogPrint(WORD Page, WORD PageSize)
{
    std::vector<club::LOG_DATA> Result;
    GetClubLog(club::LOG_ALL, Page, PageSize, Result);
    for (size_t i=0; i<Result.size(); ++i)
    {
        const club::LOG_DATA& LogData = Result[i];
        std::string Msg(
            sc::string::format(
            "%1% %2% %3% %4%",
            LogData.m_LogNum,
            static_cast<int> (LogData.m_LogType),
            sc::time::DateTimeFormat(LogData.m_LogDate, true, false),
            LogData.m_LogText));
        PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);
    }
}

void GLGaeaClient::ReqClubLog(club::EM_LOG_TYPE LogType, WORD Page, WORD PageSize)
{
    if (m_pMyClub->IsValid())
    {
        NET_CLUB_LOG_CA SendData;
        SendData.m_LogType  = LogType;
        SendData.m_Page     = Page;
        SendData.m_PageSize = PageSize;

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        NETSENDTOAGENT(NET_MSG_CLUB_LOG_CA, SendBuffer);
    }
}

void GLGaeaClient::MsgClubLogAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    club::LOG_DATA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    // Log save
    m_pClubLog->Insert(
        RecvData.m_LogNum,
        RecvData.m_LogDate,
        static_cast<club::EM_LOG_TYPE> (RecvData.m_LogType),
        RecvData.m_LogText);
}

void GLGaeaClient::MsgClubLogAc_End(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_LOG_AC_END RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    if (RecvData.m_ResultSize == 0)
    {
        //PrintMsgDlg(NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("MSG_CLUB_LOG_AC_END"));
        return;
    }

    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateLog );
}
