#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
// 
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../GLGaeaClient.h"
#include "./GLCharClient.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------




void GLCharacter::MsgProcessRnattendance( NET_MSG_GENERIC* pMsg )
{
	m_pGaeaClient->MsgRnattendanceProcess( pMsg );
}


HRESULT GLCharacter::ReqAttendanceQuestStart ( DWORD dwQUESTID )
{
	//! 퀘스트 시작을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_FAIL") );
		return S_FALSE;
	}

	//	이미 진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_ALREADY") );
		return S_FALSE;
	}

	//	이미 완료한 퀘스트인지 검사.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	//if ( pQUEST_END )
	//{
	//	if ( !pQUEST_END->m_bCOMPLETE )
	//	{
	//		//	포기(실패)한 퀘스트를 다시 시도 불가능한 경우.
	//		if ( !pQUEST->IsAGAIN() )
	//		{
	//			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONAGAIN") );
	//			return S_FALSE;
	//		}
	//	}
	//	else
	//	{
	//		//	여러번 시도 가능한지 검사.
	//		if ( !pQUEST->IsREPEAT() )
	//		{
	//			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONREPEAT") );
	//			return S_FALSE;
	//		}

	//		if ( pQUEST->IsDaily() )
	//		{
	//			CTime cTime = CTime::GetCurrentTime();
	//			CTime cTimeEnd(pQUEST_END->m_tEndTime);

	//			if (cTime.GetDay() == cTimeEnd.GetDay() &&
	//				cTime.GetMonth() == cTimeEnd.GetMonth() &&
	//				cTime.GetYear() == cTimeEnd.GetYear() )
	//			{
	//				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONREPEAT") );
	//				return S_FALSE;
	//			}
	//		}
	//	}
	//}

	//! quest 시작 서버에 요청.
	//
	GLMSG::SNETPC_REQ_RNATTENDANCE_QUEST_START NetMsg(dwQUESTID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

	return S_OK;
}