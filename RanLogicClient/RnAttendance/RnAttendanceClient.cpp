#include "pch.h"
#include "StdAfx.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
//#include "../../RanUI/InnerInterface.h"
#include "RnAttendanceClient.h"



RnAttendanceClient::RnAttendanceClient( GLGaeaClient* pGaeaClient )
: m_pGaeaClient( pGaeaClient )
, m_nDayCount( 1 )
, m_pAttendanceQuest( NULL )
, m_AttendanceProg( NULL )
, m_dwAttendanceQuestID ( UINT_MAX )
, m_bTodayCompleted( false )
{

}

RnAttendanceClient::~RnAttendanceClient()
{

}

void RnAttendanceClient::SetTotalDay( int nTotaDay )
{

}

void RnAttendanceClient::SetWeekSchdule()
{

}

void RnAttendanceClient::SetDaySchdule()
{

}

void RnAttendanceClient::GetAttendanceCompleted()
{
	m_vecAttendanceCompletedDay.clear();
	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;

	AttendanceQuestFind(cQuestPlay.GetQuestProc());
	AttendanceQuestFind(cQuestPlay.GetQuestEnd());

}

int RnAttendanceClient::IsAttendanceDay( int nDate )
{
	GetAttendanceCompleted();

	//int nAnyDay = 0;
	//BOOST_FOREACH(int nAttendanceDay , m_vecAttendanceCompletedDay)
	//{
	//	++nAnyDay
	//	if ( nAttendanceDay == nDate )
	//	{
	//		return nAnyDay;
	//	}
	//}
	//return 0;

	for(unsigned nAnyDay = 0; nAnyDay < m_vecAttendanceCompletedDay.size(); nAnyDay++ )
	{
		if ( m_vecAttendanceCompletedDay[nAnyDay] == nDate )
		{
			return nAnyDay + 1;
		}

	}

	return 0;
}

void RnAttendanceClient::AttendanceQuestFind( GLQuestPlay::MAPQUEST& sQuest )
{
	GLQuestPlay::MAPQUEST_ITER pos = sQuest.begin();
	GLQuestPlay::MAPQUEST_ITER end = sQuest.end();
	for ( ; pos!=end; ++pos )
	{
		GLQUESTPROG *pPROG = (*pos).second;

		m_pAttendanceQuest = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
		if ( m_pAttendanceQuest )
		{
			if ( m_pAttendanceQuest->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
			{
				//출석 퀘스트 ID 저장
				m_AttendanceProg = pPROG;
				m_dwAttendanceQuestID = pPROG->m_sNID.dwID;

				CTime TToday = CTime::GetCurrentTime();
				BOOST_FOREACH(LONGLONG QuestStart , pPROG->m_vectQuestStart)
				{
					CTime TQuestStart = QuestStart;

					if( TToday.GetYear() == TQuestStart.GetYear() &&
						TToday.GetMonth() == TQuestStart.GetMonth() )
					{
						m_vecAttendanceCompletedDay.push_back(TQuestStart.GetDay());
					}

				}
				// 정렬
				std::sort( m_vecAttendanceCompletedDay.begin(), m_vecAttendanceCompletedDay.end(), std::less<int>() );

				// 연속적인 값 제거
				std::vector<int>::iterator pos = std::unique( m_vecAttendanceCompletedDay.begin(), m_vecAttendanceCompletedDay.end() );

				// 새로운 논리적인 끝 위치로부터 나머지 원소 제거
				m_vecAttendanceCompletedDay.erase( pos, m_vecAttendanceCompletedDay.end() );

				m_nDayCount = static_cast<int>(m_vecAttendanceCompletedDay.size());

				if ( !pPROG->m_bCOMPLETE )
				{
					m_nDayCount =  m_nDayCount + 1;
					m_bTodayCompleted = false;
				}
				else
				{
					m_bTodayCompleted = true;
				}


				GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );

				DWORD dwItemId = GLQuestMan::GetInstance().GetRnattendanceRewardNDay( m_nDayCount ) ;

				if (dwItemId != UINT_MAX)
				{
					SNATIVEID sItemId( dwItemId );
					SITEMCUSTOM sItemCustom( sItemId );
					pQuest->m_vecGiftITEM.clear();
					pQuest->m_vecGiftITEM.push_back(sItemCustom);
				}
			}
		}
	}
}

void RnAttendanceClient::SetAttendanceQuestData( DWORD dwQuestID )
{
	SATTENDANCEQUESTDATA sAttendanceQuestData;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;

	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );

	bool bIsPlaying = false;
	if (pQuestProg)
	{
		bIsPlaying = true;
		sAttendanceQuestData.bComplete = pQuestProg->CheckCOMPLETE();
	}
	else
	{
		pQuestProg = cQuestPlay.FindEnd( dwQuestID );
		bIsPlaying = false;
	}

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );

		if ( pQuest )
		{
			sAttendanceQuestData.dwMoney = pQuest->m_dwGiftMONEY;
			sAttendanceQuestData.lnExp = pQuest->m_lnGiftEXP;
			sAttendanceQuestData.dwQuestID = dwQuestID;

			if ( pQuest->m_sAttendanceOption.nType != EMATTENDANCE_TYPE_NONE )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString strKey;
					strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
					sAttendanceQuestData.strTitle = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
				}
				else
				{
					sAttendanceQuestData.strTitle = pQuest->m_strTITLE;
				}

				if( static_cast<int>(pQuest->m_vecGiftITEM.size()) > 0 )
					sAttendanceQuestData.dwItemID = pQuest->m_vecGiftITEM[0].GetNativeID().dwID;

				GLQUEST_STEP* pQuestStep = pQuest->GetSTEP(0);

				GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;	

				if (!pQuestStep)
					return;

				sAttendanceQuestData.strProgress ="";

				if (bIsPlaying)
				{
					switch( pQuest->m_sAttendanceOption.nType )
					{
					case EMATTENDANCE_TYPE_CONNECTION:
						{
							DWORD dwMINITE_BY_HOUR = 60;
							const DWORD dwLimitTime = pQuest->m_dwLimitTime;
							const DWORD dwLimitHour = dwLimitTime / dwMINITE_BY_HOUR;
							const DWORD dwLimitMinute = dwLimitTime % dwMINITE_BY_HOUR;

							const float fLapsTime = pQuestProg->m_fLAPSTIME;
							const DWORD dwLapsTime = ((DWORD)fLapsTime) / dwMINITE_BY_HOUR;	// 초->분단위
							const DWORD dwLapsHour = dwLapsTime / dwMINITE_BY_HOUR;
							const DWORD dwLapsMinute = dwLapsTime % dwMINITE_BY_HOUR;

							CString strLimitTime;
							if ( dwLimitHour )
							{
								strLimitTime.Format ( "%d%s %d%s",
									dwLimitHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
									dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
							}
							else
							{
								strLimitTime.Format ( "%d%s",
									dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
							}

							CString strLapsTime;
							if ( dwLapsHour )
							{
								strLapsTime.Format ( "%d%s %d%s",
									dwLapsHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
									dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
							}
							else
							{
								strLapsTime.Format ( "%d%s",
									dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
							}

							sAttendanceQuestData.strProgress = sc::string::format("[%s/%s]", strLapsTime, strLimitTime );
						}
						break;
					case EMATTENDANCE_TYPE_USER_KILL:
						{
							pQuestStep->m_dwNUM_MOBKILL;

							CString strCombine;
							CString strCommentText;
// 							if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
// 							{
// 								strCommentText = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 							}
// 							else
							{
								strCommentText = pQuestStep->GetOBJ_MOBKILL();
							}
							if ( sSTEP_PROG.m_dwNUM_MOBKILL >= 0 )
							{
								strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString(),
									sSTEP_PROG.m_dwNUM_MOBKILL,
									pQuestStep->m_dwNUM_MOBKILL );
							}
							else
							{
								strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString() );
							}

							sAttendanceQuestData.strProgress = strCombine;
						}
						break;
					case EMATTENDANCE_TYPE_MOB_KILL:
						{
							pQuestStep->m_dwNUM_MOBKILL;

							CString strCombine;
							CString strCommentText;
// 							if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
// 							{
// 								strCommentText = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 							}
// 							else
							{
								strCommentText = pQuestStep->GetOBJ_MOBKILL();
							}
							if ( sSTEP_PROG.m_dwNUM_MOBKILL >= 0 )
							{
								strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString(),
									sSTEP_PROG.m_dwNUM_MOBKILL,
									pQuestStep->m_dwNUM_MOBKILL );
							}
							else
							{
								strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString());
							}

							sAttendanceQuestData.strProgress = strCombine;
						}
						break;
					case EMATTENDANCE_TYPE_LEVEL_UP:
						{
							pQuestStep->m_dwNUM_LEVELUP;

							CString strCombine;
							CString strComment;
// 							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 							{
// 								strComment = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_LEVEL_UP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 							}
// 							else
							{
								strComment = pQuestStep->GetOBJ_LEVEL_UP();
							}

							if ( sSTEP_PROG.m_dwNUM_LEVELUP >= 0 )
							{
								strCombine.Format( "%s %s [%d/%d]"
									, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
									, strComment.GetString()
									, sSTEP_PROG.m_dwNUM_LEVELUP
									, pQuestStep->m_dwNUM_LEVELUP );
							}
							else
							{
								strCombine.Format( "%s %s"
									, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
									, strComment.GetString());
							}

							sAttendanceQuestData.strProgress = strCombine;

						}
						break;
					case EMATTENDANCE_TYPE_ITEM_GET:
						{
							//pAttendanceStep->
							pQuestStep->m_dwNUM_ITEMUSE;
							CString strCombine;
							if ( sSTEP_PROG.m_dwNUM_ITEMUSE >= 0 )
							{
// 								if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 								{
// 									strCombine.Format( " %s [%d/%d]"
// 										, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString()
// 										, sSTEP_PROG.m_dwNUM_ITEMUSE
// 										, pQuestStep->m_dwNUM_ITEMUSE );
// 								}
// 								else
								{
									strCombine.Format( " %s [%d/%d]", pQuestStep->GetOBJ_ITEMUSE(),
										sSTEP_PROG.m_dwNUM_ITEMUSE,
										pQuestStep->m_dwNUM_ITEMUSE );
								}
							}
							sAttendanceQuestData.strProgress = strCombine;
						}
						break;
					case EMATTENDANCE_TYPE_ITEM_USE:
						{
							pQuestStep->m_dwNUM_ITEMUSE;
							CString strCombine;
							if ( sSTEP_PROG.m_dwNUM_ITEMUSE >= 0 )
							{
// 								if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 								{
// 									strCombine.Format( " %s [%d/%d]"
// 										, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString()
// 										, sSTEP_PROG.m_dwNUM_ITEMUSE
// 										, pQuestStep->m_dwNUM_ITEMUSE );
// 								}
// 								else
								{
									strCombine.Format( " %s [%d/%d]", pQuestStep->GetOBJ_ITEMUSE(),
										sSTEP_PROG.m_dwNUM_ITEMUSE,
										pQuestStep->m_dwNUM_ITEMUSE );
								}
							}
							sAttendanceQuestData.strProgress = strCombine;
						}
						break;
					case EMATTENDANCE_TYPE_MOVE_POS:
						{
							//pAttendanceStep->
						}
						break;
					case EMATTENDANCE_TYPE_GRINDING_ITEM:
						{
							CString strCombine;
							if ( sSTEP_PROG.m_dwNUM_ITEMGRINDING >= 0 )
							{
// 								if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 								{
// 									strCombine.Format( " %s [%d/%d]"
// 										, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_GRINDING, CGameTextMan::EM_COMMENT_TEXT).GetString()
// 										, sSTEP_PROG.m_dwNUM_ITEMGRINDING
// 										, pQuestStep->m_dwNUM_ITEMGRINDING );
// 								}
// 								else
								{
									strCombine.Format( " %s [%d/%d]", pQuestStep->GetOBJ_ITEMGRINDING(),
										sSTEP_PROG.m_dwNUM_ITEMGRINDING,
										pQuestStep->m_dwNUM_ITEMGRINDING );
								}
							}
							sAttendanceQuestData.strProgress = strCombine;
						}
						break;
					case EMATTENDANCE_TYPE_SPARRING:
						{
							pQuestStep->m_dwNUM_CONFTING;

							CString strCombine;
							if ( sSTEP_PROG.m_dwNUM_CONFTING >= 0 )
							{
								strCombine.Format( " [%d/%d]", sSTEP_PROG.m_dwNUM_CONFTING,
									pQuestStep->m_dwNUM_CONFTING );
							}
							sAttendanceQuestData.strProgress = strCombine;
						}
						break;
					}
				}

				if ( pQuestProg && pQuestProg->CheckCOMPLETE())
				{
					CTime TToday = CTime::GetCurrentTime();
					pQuestProg->m_vectQuestStart;
					BOOST_FOREACH(LONGLONG lnQuestStart,pQuestProg->m_vectQuestStart)
					{
						CTime TQuestStart(lnQuestStart);
						if( TQuestStart.GetYear() == TToday.GetYear() &&
							TQuestStart.GetMonth() == TToday.GetMonth() &&
							TQuestStart.GetDay() == TToday.GetDay() )
						{
							sAttendanceQuestData.strProgress ="[완료]";
						}
					}
				}
			}
		}

		m_sAttendanceQuestData.push_back(sAttendanceQuestData);
}

void RnAttendanceClient::SetAttendanceQuest()
{
	m_vecAttendanceQuestId.clear();

	if ( m_dwAttendanceQuestID == UINT_MAX )
	{
		GetAttendanceCompleted();

	}
	//출석 퀘스트 ID 저장
	if ( m_dwAttendanceQuestID != UINT_MAX )
	{
		m_vecAttendanceQuestId.push_back(m_dwAttendanceQuestID);
	}

	BOOST_FOREACH( DWORD dwQuestID, m_vecAttendanceQuestIdServerToClient )
	{
		m_vecAttendanceQuestId.push_back(dwQuestID);
	}

	SetCondition();
}

void RnAttendanceClient::SetCondition()
{
	m_sAttendanceQuestData.clear();

	BOOST_FOREACH( DWORD dwQuestID, m_vecAttendanceQuestId )
	{
		SetAttendanceQuestData( dwQuestID );
	}

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;

	SetSelectCondition(cQuestPlay.GetQuestProc());
	SetSelectCondition(cQuestPlay.GetQuestEnd());

}

void RnAttendanceClient::SetQuestTitle()
{

}

std::vector<SATTENDANCEQUESTDATA> RnAttendanceClient::GetAttendanceQuestData()
{
	return m_sAttendanceQuestData;
}

void RnAttendanceClient::MsgRnattendanceProcess( NET_MSG_GENERIC* pMsg )
{
	switch ( pMsg->nType )
	{
	case NET_MSG_RNATTENDANCE_QUEST_LIST_FC:
		MsgRnattendanceQuestList( pMsg );
		break;
	}
}

void RnAttendanceClient::MsgRnattendanceQuestList( NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
	GLMSG::SNETPC_RNATTENDANCE_QUEST_LIST_FC RecvData;

	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	m_vecAttendanceQuestIdServerToClient.clear();

	BOOST_FOREACH( DWORD dwQuestID, RecvData.vecQuestIDList )
	{
		m_vecAttendanceQuestIdServerToClient.push_back(dwQuestID);
	}

	SetAttendanceQuest();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Attendance );
}

int RnAttendanceClient::GetAttendanceCount()
{
	return static_cast<int>(m_sAttendanceQuestData.size());
}

void RnAttendanceClient::ResetData()
{
	m_vecAttendanceQuestIdServerToClient.clear();
	m_vecAttendanceQuestId.clear();
	m_pAttendanceQuest = NULL;
	m_AttendanceProg = NULL;
	m_dwAttendanceQuestID = UINT_MAX;
	m_vecAttendanceCompletedDay.clear();
	m_sAttendanceQuestData.clear();
}

void RnAttendanceClient::SetSelectCondition(GLQuestPlay::MAPQUEST& sQuest)
{
	GLQuestPlay::MAPQUEST_ITER pos = sQuest.begin();
	GLQuestPlay::MAPQUEST_ITER end = sQuest.end();
	for ( ; pos!=end; ++pos )
	{
		GLQUESTPROG *pPROG = (*pos).second;

		GLQUEST* pQUEST  = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
		if ( pQUEST )
		{
			if ( pQUEST->m_sSTARTOPT.nJOIN != EMQJ_ATTENDANCE )
			{
				CTime TToday = CTime::GetCurrentTime();
				CTime CQuestStartTime(pPROG->m_tStartTime);

				if ( ( pQUEST->m_sAttendanceOption.nType != EMATTENDANCE_TYPE_NONE ) &&
					( CQuestStartTime.GetYear() == TToday.GetYear() ) &&
					( CQuestStartTime.GetMonth() == TToday.GetMonth() ) &&
					( CQuestStartTime.GetDay() == TToday.GetDay() ) )
				{
					SetAttendanceQuestData( pPROG->m_sNID.dwID );
				}
			}
		}
	}
}

void RnAttendanceClient::ClearAttendanceQuestIdServerToClient()
{
	m_vecAttendanceQuestIdServerToClient.clear();
}

int RnAttendanceClient::GetDayCount()
{
	return m_nDayCount;
}

bool RnAttendanceClient::IsTodayCompleted()
{
	return m_bTodayCompleted;
}

SDAYDATA RnAttendanceClient::GetSelectDay()
{
	return m_sSelectDay;
}

void RnAttendanceClient::SetSelectDay(int nDayofWeek, 
									  int nDays, 
									  int nMonth, 
									  int nYear,
									  int nAnyDay)
{
	if (nMonth == 0)
	{
		nYear = nYear - 1;
		nMonth = 12;
	}

	if (nMonth == 13)
	{ 
		nMonth = 1;
		nYear = nYear + 1;
	}

	m_sSelectDay.m_nAnyDay = nAnyDay;
	m_sSelectDay.m_nDayofWeek = nDayofWeek;
	m_sSelectDay.m_nDays = nDays; // 날짜
	m_sSelectDay.m_nMonth = nMonth; // 달
	m_sSelectDay.m_nYear = nYear; //년
	//m_sSelectDay.m_emBoxState; //날짜 박스의 상태
	//m_sSelectDay.m_bIsAttend; //출석 상태
	//m_sSelectDay.m_strEvevt;
}

