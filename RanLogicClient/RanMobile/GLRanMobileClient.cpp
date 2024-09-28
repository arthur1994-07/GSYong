#include "pch.h"

#include "../../SigmaCore/String/StringUtil.h"
#include "../../enginelib/GUInterface/UITextControl.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/RanMobile/GLRanMobile.h"

#include "../Char/GLCharacter.h"
#include "../Widget/GLWidgetScript.h"
#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"

#include "GLRanMobileClient.h"

GLRanMobileClient::GLRanMobileClient()
: m_nTotalCharCount( 0 )
{
	m_pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
}

GLRanMobileClient::~GLRanMobileClient()
{
}

GLRanMobileClient& GLRanMobileClient::GetInstance()
{
	static GLRanMobileClient instance;
	return instance;
}

std::string GLRanMobileClient::GetHomePageURL()
{
	return NSRanMobile::GLRanMobile::GetInstance().GetHomePageURL();
}

std::string GLRanMobileClient::GetAndroidDownlaodURL()
{
	return NSRanMobile::GLRanMobile::GetInstance().GetAndroidDownloadURL();
}

std::string GLRanMobileClient::GetIOSDownloadURL()
{
	return NSRanMobile::GLRanMobile::GetInstance().GetIOSDownloadURL();
}

LONGLONG GLRanMobileClient::GetRecognizePrice()
{
	return NSRanMobile::GLRanMobile::GetInstance().GetRecognizePrice();
}

LONGLONG GLRanMobileClient::GetExtendPrice()
{
	return NSRanMobile::GLRanMobile::GetInstance().GetExtendPrice();
}

LuaTable GLRanMobileClient::GetRewardItem()
{
	LuaTable tbRet( GLWidgetScript::GetInstance().GetLuaState() );

	const NSRanMobile::VEC_REWARD& vecReward = NSRanMobile::GLRanMobile::GetInstance().GetReward();

	NSRanMobile::CITER_REWARD iter = vecReward.begin();
	NSRanMobile::CITER_REWARD iterEnd = vecReward.end();

	for( int nTableIndex = 1; iter != iterEnd; ++iter )
	{
		const NSRanMobile::SRewardItem& sReward = (*iter);

		SITEM* pItem = GLogicData::GetInstance().GetItem( sReward.sID );
		if( NULL == pItem )
			continue;

		LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
		tb.set( 1, sReward.sID.Mid() );
		tb.set( 2, sReward.sID.Sid() );
		tb.set( 3, pItem->GetName() );
		tb.set( 4, ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ] );
		tb.set( 5, sReward.wCount );
		tb.set( 6, sReward.dwNecessaryMPoint );
		tbRet.set( nTableIndex++, tb );
	}

	return tbRet;
}

int GLRanMobileClient::GetMaxSendJewelCount()
{
	return NSRanMobile::GLRanMobile::GetInstance().GetMaxSendJewelCount();
}

void GLRanMobileClient::ReqCreateMoney( const std::string& strNickName )
{
	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_MONEY );
	MsgCA.SetName( strNickName.c_str() );
	m_pClient->NETSENDTOFIELD( &MsgCA );
}

void GLRanMobileClient::ReqCreateCash( const std::string& strNickName )
{
	// 연동권 소지하고 있는지 확인
	if( NULL == m_pClient->GetCharacter()->m_cInventory.FindItem( ITEM_RANMOBILE_CONNECTION ) )
		return;

	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_CACHE );
	MsgCA.SetName( strNickName.c_str() );
	m_pClient->NETSENDTOFIELD( &MsgCA );
}

void GLRanMobileClient::ReqExtendTime()
{
	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_EXTEND_TIME );
	m_pClient->NETSENDTOFIELD( &MsgCA );
}

void GLRanMobileClient::ReqInfo()
{
	m_vecCharInfo.clear();

	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_INFO );
	m_pClient->NETSENDTOFIELD( &MsgCA );
}

void GLRanMobileClient::ReqInfoAll()
{
	m_vecCharInfo.clear();

	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_ALL );
	m_pClient->GetGlobalStage()->NetSend( &MsgCA );
}

void GLRanMobileClient::ReqSendJewel( WORD wCount )
{
	// 보석 개수 체크
	if( wCount > m_pClient->GetCharacter()->m_cInventory.CountTurnItem( ITEM_RANMOBILE_JEWEL ) )
	{
		m_pClient->PrintMsgTextDlg( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "RANMOBILE_TEXT14" ) );
		return;
	}

	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_SEND_JEWEL, wCount );
	m_pClient->NETSENDTOFIELD( &MsgCA );
}

void GLRanMobileClient::ReqGetDailyItem()
{
	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_GET_DAILY_ITEM );
	m_pClient->NETSENDTOFIELD( &MsgCA );
}

void GLRanMobileClient::ReqChangeItem( WORD wIndex )
{
	if( true == m_vecCharInfo.empty() )
		return;

	const NSRanMobile::VEC_REWARD& vecReward = NSRanMobile::GLRanMobile::GetInstance().GetReward();
	if( vecReward.size() <= wIndex )
		return;

	const SMobileCharInfo& sCharInfo = m_vecCharInfo[ 0 ];
	const NSRanMobile::SRewardItem& sReward = vecReward[ wIndex ];

	// MPoint 체크
	if( sReward.dwNecessaryMPoint > sCharInfo.nRMPoint )
		return;

	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA(
		EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM, wIndex );
	m_pClient->NETSENDTOFIELD( &MsgCA );
}

void GLRanMobileClient::PrintMsgResult( GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE* pNetMsg )
{
	m_pClient->PrintConsoleText(sc::string::format(
		"RanMobile : DBNum %1%(must 0), ChaNum %2%(must 0), Ret %3%, Command %4%", 
		pNetMsg->dwUserDbNum, pNetMsg->dwChaDBNum, pNetMsg->nResult, pNetMsg->nCommand ) );

	CString strName(pNetMsg->sMobileInfo.szChaName);
	CTime ctDateTime(pNetMsg->sMobileInfo.tExpiredDate);
	CString strTime = ctDateTime.Format("%Y-%m-%d %H:%M:%S");

	m_pClient->PrintConsoleText(sc::string::format(
		"ChaName %2%, ExpiredDate %3%, Point %4%, Jewel %5%, Deleted %6%", 
		strName, strTime, pNetMsg->sMobileInfo.nRMPoint, 
		pNetMsg->sMobileInfo.nRMJewel, pNetMsg->sMobileInfo.bDeleted));
}

void GLRanMobileClient::MsgCommandResponse( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE* pNetMsg = ( GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE*) nmg;

	switch( pNetMsg->nCommand )
	{
	case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_GM:
		{
			PrintMsgResult( pNetMsg );
		}
	case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_MONEY:
	case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_CACHE:
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgRecognize,
				"-n", pNetMsg->nResult );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_DELETE_GM:
		{
			PrintMsgResult( pNetMsg );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_GM:
		{
			PrintMsgResult( pNetMsg );
		}
	case EM_RAN_MOBILE::EM_RAN_MOBILE_INFO: 
		{
			SMobileCharInfo sInfo;
			sInfo.dwChaNum = pNetMsg->sMobileInfo.dwChaNum;
			sInfo.strName = pNetMsg->sMobileInfo.szChaName;
			sInfo.tStartDate = pNetMsg->sMobileInfo.tStartData;
			sInfo.tExpiredDate = pNetMsg->sMobileInfo.tExpiredDate;
			sInfo.nRMJewel = pNetMsg->sMobileInfo.nRMJewel;
			sInfo.nRMPoint = pNetMsg->sMobileInfo.nRMPoint;
			sInfo.bDeleted = pNetMsg->sMobileInfo.bDeleted;
			m_vecCharInfo.push_back( sInfo );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgInfo,
				"-n", pNetMsg->nResult );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_ALL_GM: 
		{
			PrintMsgResult( pNetMsg );
		}
	case EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_ALL : 
		{
			SMobileCharInfo sInfo;
			sInfo.dwChaNum = pNetMsg->sMobileInfo.dwChaNum;
			sInfo.strName = pNetMsg->sMobileInfo.szChaName;
			sInfo.tStartDate = pNetMsg->sMobileInfo.tStartData;
			sInfo.tExpiredDate = pNetMsg->sMobileInfo.tExpiredDate;
			sInfo.nRMJewel = pNetMsg->sMobileInfo.nRMJewel;
			sInfo.nRMPoint = pNetMsg->sMobileInfo.nRMPoint;
			sInfo.bDeleted = pNetMsg->sMobileInfo.bDeleted;
			m_vecCharInfo.push_back( sInfo );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_ALL_END : 
		{
			m_nTotalCharCount = m_vecCharInfo.size();

			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgInfoAll,
				"-n", pNetMsg->nResult );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_GET_POINT_GM:
		{
			PrintMsgResult( pNetMsg );
		}
	case EM_RAN_MOBILE::EM_RAN_MOBILE_GET_POINT:
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgGetPoint,
				"-n", pNetMsg->nResult );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_GET_JEWEL_GM:
		{
			PrintMsgResult( pNetMsg );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_UPDATE_DATE_GM:
		{
			PrintMsgResult( pNetMsg );
		}
	case EM_RAN_MOBILE::EM_RAN_MOBILE_EXTEND_TIME:
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgExtendTime,
				"-n", pNetMsg->nResult );

			ReqInfo();
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_UPDATE_POINT_GM:
		{
			PrintMsgResult( pNetMsg );
		}
	case EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM:
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgChangeItem,
				"-n", pNetMsg->nResult );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_UPDATE_JEWEL_GM:
		{
			PrintMsgResult( pNetMsg );
		}
	case EM_RAN_MOBILE::EM_RAN_MOBILE_SEND_JEWEL:
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgSendJewel,
				"-n", pNetMsg->nResult );
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_GET_DAILY_ITEM :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_RanMobile_MsgDailyReward,
				"-n", pNetMsg->nResult );
		}
		break;
	}
}

LuaTable GLRanMobileClient::GetCharInfo( WORD wIndex )
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );

	if( wIndex < 0 || wIndex >= m_vecCharInfo.size() )
		return tb;

	const SMobileCharInfo& sInfo = m_vecCharInfo[ wIndex ];

	CTime ctDateTime( sInfo.tExpiredDate );
	CString strTime = ctDateTime.Format( "%Y-%m-%d %H:%M" );

	tb.set( 1, sInfo.strName.c_str() );
	tb.set( 2, strTime.GetBuffer() );
	tb.set( 3, sInfo.nRMPoint );
	tb.set( 4, sInfo.nRMJewel );
	tb.set( 5, sInfo.bDeleted );
	tb.set( 6, sInfo.dwChaNum );
	
	return tb;
}

LuaTable GLRanMobileClient::GetCharInfoAll()
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );

	for( size_t i = 0; i < m_vecCharInfo.size(); ++i )
	{
		const SMobileCharInfo& sInfo = m_vecCharInfo[ i ];

		float fRatio = 0.0f;

		__time64_t tCurTime = m_pClient->GetCurrentTime().GetTime();
		__time64_t tTime = sInfo.tExpiredDate - tCurTime;

		if( 0 < tTime )
		{
			fRatio = 100.0f - double( tCurTime - sInfo.tStartDate ) / double( sInfo.tExpiredDate - sInfo.tStartDate) * 100.0f;
		}

		CTime ctDateTime( sInfo.tExpiredDate );
		CString strTime = ctDateTime.Format( "%Y-%m-%d %H:%M" );

		LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
		tbData.set( 1, sInfo.strName.c_str() );
		tbData.set( 2, fRatio );
		tbData.set( 3, sInfo.dwChaNum );
		tbData.set( 4, strTime.GetBuffer() );
		tb.set( i + 1, tbData );
	}

	return tb;
}