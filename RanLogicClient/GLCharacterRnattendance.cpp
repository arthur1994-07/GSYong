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
	//! ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_FAIL") );
		return S_FALSE;
	}

	//	�̹� �������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_ALREADY") );
		return S_FALSE;
	}

	//	�̹� �Ϸ��� ����Ʈ���� �˻�.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	//if ( pQUEST_END )
	//{
	//	if ( !pQUEST_END->m_bCOMPLETE )
	//	{
	//		//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
	//		if ( !pQUEST->IsAGAIN() )
	//		{
	//			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONAGAIN") );
	//			return S_FALSE;
	//		}
	//	}
	//	else
	//	{
	//		//	������ �õ� �������� �˻�.
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

	//! quest ���� ������ ��û.
	//
	GLMSG::SNETPC_REQ_RNATTENDANCE_QUEST_START NetMsg(dwQUESTID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

	return S_OK;
}