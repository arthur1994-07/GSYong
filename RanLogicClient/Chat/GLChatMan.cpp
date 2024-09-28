#include "pch.h"
#include "GLChatMan.h"

#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../../enginelib/GUInterface/UITextUtil.h"
#include "../../enginelib/GUInterface/UITextControl.h"
#include "../../enginelib/Common/SUBPATH.h"
#include "../../SigmaCore/Util/EBTime.h"
#include "../../MfcExLib/RanFilter.h"

#include "shlobj.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/RANPARAM.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../GfxInterfaceBase.h"
#include "../Char/GLCharacter.h"
#include "../Club/GLClubClient.h"
#include "../Crow/GLCrowClient.h"
#include "../Land/GLLandManClient.h"
#include "../Party/GLPartyClient.h"

#include "../../RanLogic/Prison/LimitPrison.h"

GLChatMan::GLChatMan(void)
:	m_bChatLog( false )
,	m_dwChatLogType( 0 )
,	m_bGMBlock( false )
,	m_bPaperingBlock( false )
,	m_nPaperingCount( 0 )
,   m_fPaperingBlockStartTime( 0.0f )
,   m_bPrisonBlock( false )
,	m_dwLinkDataIndex( 0 )
,	m_dwItemLinkDataIndex( 0 )
,	m_dwPartyLinkDataIndex( 0 )
{
}

GLChatMan::GLChatMan( const GLChatMan& value )
{
}

GLChatMan::~GLChatMan(void)
{
	m_listChat.clear();
}

GLChatMan& GLChatMan::GetInstance()
{
	static GLChatMan Instance;
	return Instance;
}

void GLChatMan::MsgProc( LPVOID msgBuffer )
{
	if( NULL == msgBuffer )
		return;

	NET_MSG_GENERIC* pNetMsg = (NET_MSG_GENERIC*) msgBuffer;

	switch( pNetMsg->nType )
	{
	case NET_MSG_CHAT_LINK_FB :			// 링크용 채팅 메시지
		{
			MsgChatLinkMessage( pNetMsg );
		}
		break;

	case NET_MSG_CHAT_CTRL_FB:			// 관리자용 채팅메시지
		{
			MsgChatGlobalMessage( pNetMsg );
		}
		break;

	case NET_MSG_CHAT_PRIVATE_FAIL :
		{
			MsgChatPrivateFail( pNetMsg );
		}
		break;

	case NET_MSG_CHAT_BLOCK :
		{
			MsgChatBlock( pNetMsg );
		}
		break;
	}
}

void GLChatMan::MsgChatItemLinkMessage(  const CString& strName, SITEMCUSTOM& sItemCustom )
{
	GLMSG::NET_CHAT_LINK_FB	NetChatFB;

	{
		std::string strChatMsg, strTemp;
		CString     strCombine;

		// 링크 정보를 찾는다.
		SLINK_TYPE sLinkType;
		sLinkType.wMType = ITEM_LINK;
		sLinkType.wSType = ITEM_LINK_INVEN;
		strTemp    = sc::string::format( "%s%s%s", strLINK_LEFT_SIMBOL, sItemCustom.GETNAME().c_str(), strLINK_RIGHT_SIMBOL );			
		strChatMsg = sc::string::format( ID2GAMEINTEXT("PARTY_MBR_PICKUP_BRD"), strName, strTemp );

		const char* pCut = strstr( strChatMsg.c_str(), strTemp.c_str());
		if ( pCut )
		{
			int pCutLen = sc::string::ansiToUnicode(pCut).length();

			int nStart = sc::string::ansiToUnicode(strChatMsg).length() -  pCutLen;
			int nEnd = nStart + sc::string::ansiToUnicode(strTemp).length(); 


			NetChatFB.ADD( &sItemCustom, sizeof(sItemCustom));
			NetChatFB.sLinkDataRecv[0].sLinkType = sLinkType;
			NetChatFB.sLinkDataRecv[0].sLinkPos.nStart = nStart;
			NetChatFB.sLinkDataRecv[0].sLinkPos.nEnd = nEnd;
			NetChatFB.sLinkDataRecv[0].dwSize = sizeof(sItemCustom);
		}	

		NetChatFB.emType = CHAT_TYPE_CTRL_GLOBAL;
		NetChatFB.SetName(strName);
		NetChatFB.SetChatMsg(strChatMsg.c_str());

		GLChatMan::GetInstance().MsgSystemItemLinkMessage( &NetChatFB );
	}

}

void GLChatMan::MsgChatLinkMessage( LPVOID msgBuffer )
{
	GLMSG::NET_CHAT_LINK_FB* pNetMsg = (GLMSG::NET_CHAT_LINK_FB*) msgBuffer;
	// 두번 출력됨, 내가 보낸 메세지라면 출력하지않는다;
	if ( pNetMsg->dwChaNum == GLWidgetScript::GetInstance().m_pGaeaClient->GETMYDBNUM() )
		return;

	/*-- 감옥에서 채팅 금지 -------------------------------------------------------------------*/
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;

	if( pClient && pNetMsg->emType == CHAT_TYPE_LOUDSPEAKER ) {
		GLLandManClient* pLand = pClient->GetActiveMap();
		GLCharacter* pChar = pClient->GetCharacter();

		if( pLand && pChar ) {
			const SMAPNODE* pMapNode = pClient->FindMapNode( pLand->GetBaseMapID() );

			if( pMapNode && PRISONLIMIT.IsLimitStatus( pMapNode, pChar->UserLevel(), PML_CHATTING ) ) {
				return;
			}
		}
	}

	DWORD dwSize = 0;
	VEC_LINK_TEXT_DATA vecLinkTextData;
	for( DWORD i = 0; i < CHAT_LINK_MAX; ++i ) 
	{
		if( false == pNetMsg->sLinkDataRecv[ i ].VALID() )
			continue;

		int nIndex = AddLinkDataToList( pNetMsg->szName,
			pNetMsg->sLinkDataRecv[ i ].sLinkType,
			pNetMsg->sLinkDataRecv[ i ].sLinkPos,
			pNetMsg->LinkData + dwSize,
			pNetMsg->sLinkDataRecv[ i ].dwSize );

		dwSize += pNetMsg->sLinkDataRecv[ i ].dwSize;

		// LinkText로 변환
		if( 0 <= nIndex ) 
		{
			SLINK_TEXT_DATA sLinkTextData;
			sLinkTextData.nLinkTypeIndex = nIndex;
			sLinkTextData.sLinkPos = pNetMsg->sLinkDataRecv[ i ].sLinkPos;
			sLinkTextData.dwTextColor = GetLinkColor( nIndex );
			vecLinkTextData.push_back( sLinkTextData );
		}
	}

	bool bGM = false;
	DWORD dwType = ConvertChatType( (int) pNetMsg->emType, bGM );

	OnNotifyReceiveMsg( dwType, pNetMsg->szName, pNetMsg->szChatMsg, false, bGM, &vecLinkTextData );
}

void GLChatMan::MsgSystemItemLinkMessage( LPVOID msgBuffer )
{
	GLMSG::NET_CHAT_LINK_FB* pNetMsg = (GLMSG::NET_CHAT_LINK_FB*) msgBuffer;
	// 두번 출력됨, 내가 보낸 메세지라면 출력하지않는다;
	if ( pNetMsg->dwChaNum == GLWidgetScript::GetInstance().m_pGaeaClient->GETMYDBNUM() )
		return;

	DWORD dwSize = 0;
	VEC_LINK_TEXT_DATA vecLinkTextData;
	for( DWORD i = 0; i < CHAT_LINK_MAX; ++i ) 
	{
		if( false == pNetMsg->sLinkDataRecv[ i ].VALID() )
			continue;

		int nIndex = AddLinkDataToList( pNetMsg->szName,
			pNetMsg->sLinkDataRecv[ i ].sLinkType,
			pNetMsg->sLinkDataRecv[ i ].sLinkPos,
			pNetMsg->LinkData + dwSize,
			pNetMsg->sLinkDataRecv[ i ].dwSize );

		dwSize += pNetMsg->sLinkDataRecv[ i ].dwSize;

		// LinkText로 변환
		if( 0 <= nIndex ) 
		{
			SLINK_TEXT_DATA sLinkTextData;
			sLinkTextData.nLinkTypeIndex = nIndex;
			sLinkTextData.sLinkPos = pNetMsg->sLinkDataRecv[ i ].sLinkPos;
			sLinkTextData.dwTextColor = GetLinkColor( nIndex );
			vecLinkTextData.push_back( sLinkTextData );
		}
	}

	bool bGM = false;
	DWORD dwType = ConvertChatType( (int) pNetMsg->emType, bGM );

	OnSystemItemLinkMsg( dwType, pNetMsg->szName, pNetMsg->szChatMsg, false, bGM, &vecLinkTextData );
}

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
void GLChatMan::MsgSystemItemAndReferLinkMessage( LPVOID msgBuffer )
{
	GLMSG::NET_CHAT_LINK_FB* pNetMsg = (GLMSG::NET_CHAT_LINK_FB*) msgBuffer;
	// 두번 출력됨, 내가 보낸 메세지라면 출력하지않는다;
	if ( pNetMsg->dwChaNum == GLWidgetScript::GetInstance().m_pGaeaClient->GETMYDBNUM() )
		return;

	DWORD dwSize = 0;
	VEC_LINK_TEXT_DATA vecLinkTextData;
	for( DWORD i = 0; i < CHAT_LINK_MAX; ++i ) 
	{
		if( false == pNetMsg->sLinkDataRecv[ i ].VALID() )
			continue;

		int nIndex = AddLinkDataToList( pNetMsg->szName,
			pNetMsg->sLinkDataRecv[ i ].sLinkType,
			pNetMsg->sLinkDataRecv[ i ].sLinkPos,
			pNetMsg->LinkData + dwSize,
			pNetMsg->sLinkDataRecv[ i ].dwSize );

		dwSize += pNetMsg->sLinkDataRecv[ i ].dwSize;

		// LinkText로 변환
		if( 0 <= nIndex ) 
		{
			SLINK_TEXT_DATA sLinkTextData;
			sLinkTextData.nLinkTypeIndex = nIndex;
			sLinkTextData.sLinkPos = pNetMsg->sLinkDataRecv[ i ].sLinkPos;
			sLinkTextData.dwTextColor = GetLinkColor( nIndex );
			vecLinkTextData.push_back( sLinkTextData );
		}
	}

	bool bGM = false;
	DWORD dwType = ConvertChatType( (int) pNetMsg->emType, bGM );

	OnSystemItemAndReferLinkMsg( dwType, pNetMsg->szName, pNetMsg->szChatMsg, false, bGM, &vecLinkTextData );
}


void GLChatMan::MsgChatGlobalMessage( LPVOID msgBuffer )
{
	NET_CHAT_CTRL_FB* pNetMsg = (NET_CHAT_CTRL_FB*) msgBuffer;

	bool bGM = false;
	DWORD dwType = ConvertChatType( (int) pNetMsg->emType, bGM );

	OnNotifyReceiveMsg( dwType, "", pNetMsg->szChatMsg, false, bGM );
}

void GLChatMan::MsgChatPrivateFail( LPVOID msgBuffer )
{
	GLMSG::SNETPC_CHAT_PRIVATE_FAIL* pNetMsg = (GLMSG::SNETPC_CHAT_PRIVATE_FAIL*) msgBuffer;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_BasicChat_PrivateFail,
		"-n, -s", pNetMsg->emFail, pNetMsg->szName );
}

void GLChatMan::MsgChatBlock( LPVOID msgBuffer )
{
	GLMSG::SNETPC_CHAT_BLOCK* pNetMsg = (GLMSG::SNETPC_CHAT_BLOCK*) msgBuffer;

	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	GLCharacter* pChar =  pClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->m_tCHATBLOCK = pNetMsg->tChatBlock;

	CTime cTime( CTime::GetCurrentTime() );
	CTime cBLOCK( pNetMsg->tChatBlock );
	pClient->GetConvertServerTime( cTime );

	CTimeSpan cSPAN = cBLOCK - cTime;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_BasicChat_Block,
		"-l", cSPAN.GetTotalMinutes() );
}

void GLChatMan::SaveChatLog( const CString& strText, DWORD dwType )
{
	if( !(m_dwChatLogType & dwType) )
		return;

	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();

	// 현재시각
	CTime cTime( pClient->GetCurrentTime() );

	// 서버 타임으로 변환( TimeZone 계산 )
	pClient->GetConvertServerTime( cTime );

	const int nServerYear = (int) cTime.GetYear();
	const int nServerMonth = (int) cTime.GetMonth();
	const int nServerDay = (int) cTime.GetDay();
	const int nServerHour = (int) cTime.GetHour();
	const int nServerMinute = (int) cTime.GetMinute();

	CString strChatLog;
	strChatLog.Format( "%02d::%02d\t ", nServerHour, nServerMinute );

	strChatLog += "\t";

	strChatLog += strText;
	strChatLog += '\n';

	// 로그파일 로드 및 저장
	TCHAR szChatLogDir[ MAX_PATH ] = { 0 };
	SHGetSpecialFolderPath( NULL, szChatLogDir, CSIDL_PERSONAL, FALSE );

	const SUBPATH* pPath = pClient->GetSubPath();

	CString strChatLogDir( szChatLogDir );
	strChatLogDir += pPath->ChatLog().c_str();

	CString strChatLogFile;
	strChatLogFile.Format( "%s%d%02d%02d.txt", strChatLogDir, nServerYear, nServerMonth, nServerDay );

	FILE* pFile = NULL;
	pFile = _fsopen( strChatLogFile, "a", _SH_DENYNO );
	if( NULL == pFile )
		return;

	fprintf( pFile, strChatLog );
	fclose( pFile );
}

void GLChatMan::ChatLog( bool bChatLog, DWORD dwChatLogType )
{
	if ( bChatLog ) 
	{
		m_bChatLog = true;
		m_dwChatLogType |= dwChatLogType;

		// CreteDirectory...
		TCHAR szChatLogDir[ MAX_PATH ] = { 0 };
		SHGetSpecialFolderPath( NULL, szChatLogDir, CSIDL_PERSONAL, FALSE );

		const SUBPATH* pPath = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetSubPath();

		std::string strChatLogDir( szChatLogDir );
		strChatLogDir += pPath->ChatLog(); //SUBPATH::CHATLOG;

		CreateDirectory( strChatLogDir.c_str() , NULL );
	}
	else 
	{
		m_dwChatLogType &= ~( dwChatLogType );
		if( 0 == m_dwChatLogType )
			m_bChatLog = false;
	}
}

void GLChatMan::AddChatLink( const SLINK_DATA_BASIC& sLinkDataBasic )
{
	CString strLinkName;
	DWORD dwColor = NS_UITEXTUTIL::ENABLE;

	if( false == GetLinkData( sLinkDataBasic, strLinkName, dwColor ) )
		return;
	
	if( false == sLinkDataBasic.VALID() )
		return;

	// SLINK_TEXT_DATA_EX 생성을 위해 데이터 넣어주기
	LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
	tbData.set( "wMType", sLinkDataBasic.sLinkType.wMType );
	tbData.set( "wSType", sLinkDataBasic.sLinkType.wSType );
	tbData.set( "nStart", (int) 0 );	// sLinkPos.nStart - 루아에서 설정하도록 함.
	tbData.set( "nEnd", (int) 0 );		// sLinkPos.nEnd - 루아에서 설정하도록 함.
	tbData.set( "dwData1", sLinkDataBasic.dwData1 );
	tbData.set( "dwData2", sLinkDataBasic.dwData2 );
	tbData.set( "dwColor", dwColor );

	GLWidgetCallFuncValue args[ 2 ];
	args[ 0 ].SetTable( &tbData );
	args[ 1 ].SetString( strLinkName.GetBuffer() );

	GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_AddChatLink, args, 2 );
}

void GLChatMan::OnNotifyReceiveMsg( DWORD dwType, const char* szName, const char* strMsg, 
								    bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData )
{
	CString strTemp = strMsg;
	if( TRUE == CRanFilter::GetInstance().ChatFilter( strTemp ) )
		strTemp = CRanFilter::GetInstance().GetProverb();

	int nLinkTextDataListCnt = 1;
	LuaTable tbLinkTextDataList( GLWidgetScript::GetInstance().GetLuaState() );

	if( NULL != pvecLinkTextData )
	{
		VEC_LINK_TEXT_DATA_CITER citer = pvecLinkTextData->begin();
		for( ; citer != pvecLinkTextData->end(); ++citer )
		{
			LuaTable tbLinkTextData( GLWidgetScript::GetInstance().GetLuaState() );
			tbLinkTextData.set( "nLinkTypeIndex", citer->nLinkTypeIndex );
			tbLinkTextData.set( "nStart", citer->sLinkPos.nStart );
			tbLinkTextData.set( "nEnd", citer->sLinkPos.nEnd );
			tbLinkTextData.set( "dwColor", citer->dwTextColor );

			tbLinkTextDataList.set( nLinkTextDataListCnt++, tbLinkTextData );
		}
	}

	std::string strName;
	//if( EMGFX_FONT_KOREAN == GfxInterfaceBase::GetInstance()->GetFontLanguage() )
	//	strName = szName;
	//else
	strName = szName;
	//strName = sc::string::ansi_to_utf8( std::string( szName ) );

	LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
	tbData.set( "nType", dwType );
	tbData.set( "strName", strName.c_str() );
	tbData.set( "strMsg", strTemp.GetBuffer() );
	tbData.set( "tbLinkDataList", tbLinkTextDataList );
	tbData.set( "bSelf", bSelf );
	tbData.set( "bGM", bGM );

	GLWidgetCallFuncValue valData;
	valData.SetTable( &tbData );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_BasicChat_ReceiveMsg, &valData, 1 );
}

void GLChatMan::OnSystemItemLinkMsg( DWORD dwType, const char* szName, const char* strMsg, 
								   bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData )
{
	CString strTemp = strMsg;
	int nLinkTextDataListCnt = 1;
	LuaTable tbLinkTextDataList( GLWidgetScript::GetInstance().GetLuaState() );

	if( NULL != pvecLinkTextData )
	{
		VEC_LINK_TEXT_DATA_CITER citer = pvecLinkTextData->begin();
		for( ; citer != pvecLinkTextData->end(); ++citer )
		{
			LuaTable tbLinkTextData( GLWidgetScript::GetInstance().GetLuaState() );
			tbLinkTextData.set( "nLinkTypeIndex", citer->nLinkTypeIndex );
			tbLinkTextData.set( "nStart", citer->sLinkPos.nStart );
			tbLinkTextData.set( "nEnd", citer->sLinkPos.nEnd );
			tbLinkTextData.set( "dwColor", citer->dwTextColor );

			tbLinkTextDataList.set( nLinkTextDataListCnt++, tbLinkTextData );
		}
	}

	std::string strName(szName);
	LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
	tbData.set( "nType", dwType );
	tbData.set( "strName", strName.c_str() );
	tbData.set( "strMsg", strTemp.GetBuffer() );
	tbData.set( "tbLinkDataList", tbLinkTextDataList );
	tbData.set( "bSelf", bSelf );
	tbData.set( "bGM", bGM );

	GLWidgetCallFuncValue valData;
	valData.SetTable( &tbData );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PrintLinkItemSystemMsg, &valData, 1 );
}

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
void GLChatMan::OnSystemItemAndReferLinkMsg( DWORD dwType, const char* strName, const char* strMsg, 
								             bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData )
{
	CString strTemp = strMsg;
	if( TRUE == CRanFilter::GetInstance().ChatFilter( strTemp ) )
		strTemp = CRanFilter::GetInstance().GetProverb();

	int nLinkTextDataListCnt = 1;
	LuaTable tbLinkTextDataList( GLWidgetScript::GetInstance().GetLuaState() );

	if( NULL != pvecLinkTextData )
	{
		VEC_LINK_TEXT_DATA_CITER citer = pvecLinkTextData->begin();
		for( ; citer != pvecLinkTextData->end(); ++citer )
		{
			LuaTable tbLinkTextData( GLWidgetScript::GetInstance().GetLuaState() );
			tbLinkTextData.set( "nLinkTypeIndex", citer->nLinkTypeIndex );
			tbLinkTextData.set( "nStart", citer->sLinkPos.nStart );
			tbLinkTextData.set( "nEnd", citer->sLinkPos.nEnd );
			tbLinkTextData.set( "dwColor", citer->dwTextColor );

			tbLinkTextDataList.set( nLinkTextDataListCnt++, tbLinkTextData );
		}
	}

	LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
	tbData.set( "nType", dwType );
	tbData.set( "strName", strName );
	tbData.set( "strMsg", strTemp.GetBuffer() );
	tbData.set( "tbLinkDataList", tbLinkTextDataList );
	tbData.set( "bSelf", bSelf );
	tbData.set( "bGM", bGM );

	GLWidgetCallFuncValue valData;
	valData.SetTable( &tbData );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PrintItemLinkAndReferLinkSystemMsg, &valData, 1 );
}


void GLChatMan::OnNotifyClickLink( int nLinkIndex )
{
	SLINK_DATA_INDEX* pLinkDataIndex = GetLinkBasicData( nLinkIndex );
	if( NULL == pLinkDataIndex )
		return;

	switch( pLinkDataIndex->sLinkType.wMType )
	{
	case ITEM_LINK :
		{
			MAP_ITEM_LINK_DATA_INDEX_ITER iter = m_mapItemLinkData.find( pLinkDataIndex->nLinkIndex );
			if( m_mapItemLinkData.end() == iter )
				break;

			const SLINK_DATA_ITEM& sData = iter->second;
			
			std::string strHeader = NSWIDGET_SCRIPT::GetItemInfoHeader( sData.sItem );
			std::string strInfo = NSWIDGET_SCRIPT::GetItemInfo( sData.sItem );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ItemLinkInfo_Open,
				"-n, -n, -n, -n, -s, -s",
				sData.sItem.Mid(),
				sData.sItem.Sid(),
				sData.sItem.GetBoxWrappedItemID().Mid(),
				sData.sItem.GetBoxWrappedItemID().Sid(),
				strHeader.c_str(),
				strInfo.c_str() );
		}
		break;

	case PARTY_LINK :
		{
			switch ( pLinkDataIndex->sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( pLinkDataIndex->nLinkIndex );
					if( m_mapPartyLinkData.end() == iter )
						break;

					const GLPARTY_LINK& sLink = iter->second;

					LuaTable tbMemberList( GLWidgetScript::GetInstance().GetLuaState() );
					for( int i = 0; i < MAXPARTY; ++i )
					{
						if( FALSE == sLink.sPartyMem[ i ].VALID() )
							continue;

						LuaTable tbMember( GLWidgetScript::GetInstance().GetLuaState() );
						tbMember.set( 1, sLink.sPartyMem[ i ].dwGaeaID );
						tbMember.set( 2, sLink.sPartyMem[ i ].emClass );
						tbMember.set( 3, sLink.sPartyMem[ i ].m_Name );
						tbMember.set( 4, sLink.sPartyMem[ i ].wLevel );
						tbMemberList.set( i + 1, tbMember );
					}

					GLWidgetCallFuncValue args[ 3 ];
					args[ 0 ].SetTable( &tbMemberList );
					args[ 1 ].SetBool( TRUE == sLink.bOnce ? true : false );
					args[ 2 ].SetInt( pLinkDataIndex->nLinkIndex );

					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_PartyRecruit_Open, args, 3 );

					break;
				}
				break;
			case PARTY_LINK_ACCEPT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( pLinkDataIndex->nLinkIndex );	
					if ( pos == m_mapPartyLinkData.end() )
						break;

					// 파티모집 UI표시
					GLPARTY_LINK& sPartyLink = pos->second;

					//	파티장 확인
					if ( !sPartyLink.sPartyMem[0].VALID() )
						break;

					if ( sPartyLink.bOnce )
						break;

					sPartyLink.bOnce = true;

					GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter()->ReqPartyLinkLure( true, sPartyLink.dwHashKey, sPartyLink.dwTimeKey, sPartyLink.sPartyMem[0].dwGaeaID );

					break;
				}
				break;
			case PARTY_LINK_REFUSE:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( pLinkDataIndex->nLinkIndex );	
					if ( pos == m_mapPartyLinkData.end() ) 
						break;

					// 파티모집 UI표시
					GLPARTY_LINK& sPartyLink = pos->second;

					//	파티장 확인
					if ( !sPartyLink.sPartyMem[0].VALID() )
						break;

					if ( sPartyLink.bOnce )
						break;

					sPartyLink.bOnce = true;

					GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter()->ReqPartyLinkLure( false, sPartyLink.dwHashKey, sPartyLink.dwTimeKey, sPartyLink.sPartyMem[0].dwGaeaID );

					break;
				}
				break;
			};
		}
		break;

	case QUEST_LINK :
	case SKILL_LINK :
	case CROW_LINK :
	case MAP_LINK :
		break;
	}
}

bool GLChatMan::GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor )
{
	GLCharacter* pCharacter = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL != pCharacter )
	{
		if( true == pCharacter->GetLinkData( sLinkDataBasic, strLinkName, dwColor ) )
			return true;
	}

	return false;
}

DWORD GLChatMan::GetLinkColor( int nLinkIndex )
{
	DWORD dwColor = NS_UITEXTCOLOR::CHAT_NORMAL;
	if( nLinkIndex < 0 )
		return dwColor;

	SLINK_DATA_INDEX* pLinkBasic = GetLinkBasicData( nLinkIndex );
	if( NULL == pLinkBasic )
		return dwColor;
	if( pLinkBasic->nLinkIndex < 0 )
		return dwColor;

	int nDataIndex = pLinkBasic->nLinkIndex;
	switch( pLinkBasic->sLinkType.wMType ) 
	{
	case ITEM_LINK :
		{
			switch( pLinkBasic->sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN :
			case ITEM_LINK_PUTON :
			case ITEM_LINK_USER_STORAGE :
			case ITEM_LINK_CLUB_STORAGE :
			case ITEM_LINK_CROW_SALE :
				{
					//	이미 항목이 있을경우 삭제하고 새로운 데이터로 갱신
					MAP_ITEM_LINK_DATA_INDEX_ITER iter = m_mapItemLinkData.find( nDataIndex );	
					if( iter == m_mapItemLinkData.end() )
						return dwColor;

					SLINK_DATA_ITEM& sData = iter->second;

					const SITEM* pItem = GLogicData::GetInstance().GetItem( sData.sItem.GetNativeID() );
					if( NULL == pItem )
						return dwColor;

					//아이템 링크별로 색상이 다르도록 수정 
					//dwColor = COMMENT::ITEMCOLOR[pItem->sBasicOp.emLevel];
					dwColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];
				}
				break;
			}
		}
		break;

	case PARTY_LINK :
		{
			switch( pLinkBasic->sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT :
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( nDataIndex );
					if( iter == m_mapPartyLinkData.end() )
						return dwColor;

					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;
				}
				break;

			case PARTY_LINK_ACCEPT :
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( nDataIndex );
					if( iter == m_mapPartyLinkData.end() )
						return dwColor;

					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;
				}
				break;
			case PARTY_LINK_REFUSE :
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return dwColor;

					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;
				}
				break;
			}
		}
		break;

	case QUEST_LINK :
	case SKILL_LINK :
	case CROW_LINK :
	case MAP_LINK :
		{
		}
		break;
	}

	return dwColor;
}

DWORD GLChatMan::ConvertChatType( int nType, bool& bGM )
{
	bGM = CHAT_TYPE_GLOBAL == nType || CHAT_ADD_TYPE_FOPR_GM < nType ? true : false;

	DWORD dwChatType = 0;

	switch( nType )
	{
	case CHAT_TYPE_GLOBAL :		// 관리용 글로벌 메시지
		dwChatType = GLCHAT_DEF::EMCHAT_SYSTEM;
		break;

	case CHAT_TYPE_NORMAL :		// 일반적 채팅 메시지
	case CHAT_TYPE_NORMAL_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_NORMAL;
		break;

	case CHAT_TYPE_PRIVATE :	// 개인적인 메시지, 귓말, 속삭임
	case CHAT_TYPE_PRIVATE_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_PRIVATE;
		break;

	case CHAT_TYPE_PRIVATE_PARTY_RECRUIT:
		dwChatType = GLCHAT_DEF::EMCHAT_PRIVATE_PARTY_RECRUIT;
		break ;

	case CHAT_TYPE_PARTY :		// 파티원에게 전달되는 메시지
	case CHAT_TYPE_PARTY_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_PARTY;
		break;

	case CHAT_TYPE_MASTER :		// 파티원에게 전달되는 마스터의 메시지
	case CHAT_TYPE_MASTER_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_MASTER;
		break;

	case CHAT_TYPE_GUILD :		// 길드원에게 전달되는 메시지
	case CHAT_TYPE_GUILD_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_GUILD;
		break;

	case CHAT_TYPE_ALLIANCE :
	case CHAT_TYPE_ALLIANCE_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_ALLIANCE;
		break;

	case CHAT_TYPE_AREA :
	case CHAT_TYPE_AREA_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_AREA;
		break;

	case CHAT_TYPE_PARTY_RECRUIT :
	case CHAT_TYPE_PARTY_RECRUIT_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_PARTY_RECRUIT;
		break;

	case CHAT_TYPE_LOUDSPEAKER :
	case CHAT_TYPE_LOUDSPEAKER_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_TOALL;
		break;

	case CHAT_TYPE_FACTION :
	case CHAT_TYPE_FACTION_GM :
		dwChatType = GLCHAT_DEF::EMCHAT_FACTION;
		break;

	case CHAT_TYPE_CTRL_GLOBAL :
		dwChatType = GLCHAT_DEF::EMCHAT_SYSTEM;
		break;
	}

	return dwChatType;
}

void GLChatMan::ConvertLinkData( const VEC_LINK_TEXT_DATA_EX& vecLinkDataIN, VEC_LINK_TEXT_DATA& vecLinkDataOut )
{
	// 클라이언트 링크 데이터를 서버데이터로 변환 
	for( DWORD i = 0; i < vecLinkDataIN.size(); ++i )
	{
		int nLinkIndex = AddLinkDataToList_Client( 
			vecLinkDataIN[ i ].sLinkDataBasic, &vecLinkDataIN[ i ].sItemcustom );

		if( 0 <= nLinkIndex )
		{
			SLINK_TEXT_DATA sLinkTextData;
			sLinkTextData.sLinkPos = vecLinkDataIN[ i ].sLinkDataBasic.sLinkPos;
			sLinkTextData.dwTextColor = vecLinkDataIN[ i ].dwTextColor;
			sLinkTextData.nLinkTypeIndex = nLinkIndex;

			vecLinkDataOut.push_back( sLinkTextData );
		}
	}	
}

int GLChatMan::AddLinkDataToList_Client( const SLINK_DATA_BASIC& sLinkDataBasic, const SITEMCUSTOM* pItemCustom )
{
	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();

	int nLinkIndex = -1;

	if( FALSE == sLinkDataBasic.VALID() )
		return nLinkIndex;

	switch( sLinkDataBasic.sLinkType.wMType )
	{
	case ITEM_LINK :
		{
			switch( sLinkDataBasic.sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN :
				{
					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData1;

					GLCharacter* pCharacter = pClient->GetCharacter();
					if( NULL == pCharacter )
						break;

					SINVENITEM* pInvenItem = pCharacter->m_cInventory.GetItem( sInvenPos.wPosX, sInvenPos.wPosY );
					if( NULL == pInvenItem )
						break;

					SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;

					nLinkIndex = AddLinkDataToList (
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						sLinkDataBasic.sLinkPos,
						&sItemCustom,
						sizeof( SITEMCUSTOM ) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_PUTON :
				{
					EMSLOT emSlot = (EMSLOT) sLinkDataBasic.dwData1;
					if( SLOT_TSIZE <= emSlot )
						break;

					GLCharacter* pCharacter = pClient->GetCharacter();
					if( NULL == pCharacter || FALSE == pCharacter->VALID_SLOT_ITEM( emSlot ) )
						break;

					SITEMCUSTOM sItemCustom = pCharacter->m_PutOnItems[ emSlot ];

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						sLinkDataBasic.sLinkPos,
						&sItemCustom,
						sizeof( SITEMCUSTOM ) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_USER_STORAGE :
				{
					DWORD dwChannel = sLinkDataBasic.dwData1;
					if( MAX_CLUBSTORAGE <= dwChannel )
						break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;

					GLCharacter* pCharacter = pClient->GetCharacter();
					if( NULL == pCharacter )
						break;

					SINVENITEM* pInvenItem = pCharacter->m_cStorage[ dwChannel ].FindPosItem( sInvenPos.wPosX, sInvenPos.wPosY );
					if( NULL == pInvenItem )
						break;

					SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						sLinkDataBasic.sLinkPos,
						&sItemCustom,
						sizeof(SITEMCUSTOM) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_CLUB_STORAGE :
				{
					GLCharacter* pCharacter = pClient->GetCharacter();
					if( NULL == pCharacter )
						break;

					if( false == pClient->GetMyClub()->IsValid())
						break;
					if( pClient->GetMyClub()->MasterCharDbNum() != pCharacter->m_CharDbNum )
						break;

					DWORD dwChannel = sLinkDataBasic.dwData1;
					if( dwChannel >= MAX_CLUBSTORAGE )
						break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;

					SINVENITEM* pInvenItem = pClient->GetMyClub()->FindPosItem( dwChannel, sInvenPos.wPosX, sInvenPos.wPosY );
					if( NULL == pInvenItem )
						break;

					SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						sLinkDataBasic.sLinkPos,
						&sItemCustom,
						sizeof( SITEMCUSTOM ) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_CROW_SALE :
				{
					GLCharacter* pCharacter = pClient->GetCharacter();
					if( NULL == pCharacter )
						break;

					DWORD dwNpcGlobID = ( sLinkDataBasic.dwData1 & 0xFFFF0000 ) >> 16;
					DWORD dwChannel = ( sLinkDataBasic.dwData1 & 0x0000FFFF );

					GLLandManClient* pLand = pClient->GetActiveMap();
					if( NULL == pLand )
						break;

					std::tr1::shared_ptr< GLCrowClient > pCrow = pLand->GetCrow( dwNpcGlobID );
					if( NULL == pCrow )
						break;

					if( pCrow->m_pCrowData->GetSaleNum() <= dwChannel )
						break;

					GLInventory* pInven = pCrow->m_pCrowData->GetSaleInven( dwChannel );
					if( NULL == pInven )
						break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;

					SINVENITEM* pSaleItem = pInven->GetItem( sInvenPos.wPosX, sInvenPos.wPosY );
					if( NULL == pSaleItem )
						break;

					SITEMCUSTOM sItemCustom = pSaleItem->sItemCustom;

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						sLinkDataBasic.sLinkPos,
						&sItemCustom,
						sizeof( SITEMCUSTOM ) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_NORMAL :
				{
					nLinkIndex = AddLinkDataToList( "",
						sLinkDataBasic.sLinkType,
						sLinkDataBasic.sLinkPos,
						(void*) pItemCustom,
						sizeof( SITEMCUSTOM ) );

					return nLinkIndex;
				}
				break;
			}
		}
		break;

	case PARTY_LINK :
		{
			switch( sLinkDataBasic.sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT :
				{
					// 나의 정보를 입력하는곳이다.
					// 파티장이 아니면 안된다.
					GLCharacter* pCharacter = pClient->GetCharacter();
					if( NULL == pCharacter )
						break;

					GLPartyClient* const pMyParty = pClient->GetMyPartyClient();

					const bool bParty = pMyParty->isValid();
					if( true == bParty && false == pMyParty->isMaster( pCharacter->GetGaeaID() ) )
						break;

					GLPARTY_LINK sPartyLink;
					int nPartyMem = 1;

					if( true == bParty ) 
					{						
						sPartyLink.sMapID  = sLinkDataBasic.dwData1;						

						for( DWORD i = 0; i < pMyParty->GetNMember(); ++i )
						{
							GLPartySlotClient* const pMember = pMyParty->GetMember( i );
							if( pMember == NULL )
								continue;

							sPartyLink.sPartyMem[ i + 1 ].dwGaeaID = pMember->m_dwGaeaID;
							sPartyLink.sPartyMem[ i + 1 ].emClass = pMember->m_emClass;
							sPartyLink.sPartyMem[ i + 1 ].SetName( pMember->m_szName );
							sPartyLink.sPartyMem[ i + 1 ].wLevel = (WORD) pMember->m_Level;
						}
					}
					else
					{
						sPartyLink.sMapID  = sLinkDataBasic.dwData1;

						sPartyLink.sPartyMem[ 0 ].dwGaeaID = pCharacter->GetGaeaID();
						sPartyLink.sPartyMem[ 0 ].emClass = pCharacter->m_emClass;
						sPartyLink.sPartyMem[ 0 ].SetName( pCharacter->m_szName );
						sPartyLink.sPartyMem[ 0 ].wLevel = pCharacter->GetLevel();
					}

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						sLinkDataBasic.sLinkPos,
						&sPartyLink,
						sizeof( GLPARTY_LINK_MEM ) * nPartyMem + sizeof( SNATIVEID ) );

					return nLinkIndex;
				}
				break;

			case PARTY_LINK_ACCEPT:				//	해당 사항이 없다.
				break;

			case PARTY_LINK_REFUSE:				//	해당 사항이 없다.
				break;
			}
		}
		break;

	case QUEST_LINK :
	case SKILL_LINK :
	case CROW_LINK :
	case MAP_LINK :
		{
		}
		break;
	}

	return nLinkIndex;
}

int GLChatMan::AddLinkDataToList( const std::string& strCharName, const SLINK_TYPE& sLinkType, 
								  const SLINK_DATA_POS& sLinkPos, void* pBuffer, DWORD dwSize )
{
	int nLinkIndex = -1;

	if( false == sLinkType.VALID() )
		return nLinkIndex;

	// 인덱스를 모두 소모한 경우 0 으로 초기화한다;
	if ( CHAT_LINK_BASIC_DATA_MAX <= m_dwLinkDataIndex )
	{
		m_dwLinkDataIndex = 0;	
	}

	// 이미 사용중인 Index 라면 지운다;
	MAP_LINK_DATA_INDEX_ITER iter = m_mapLinkData.find( m_dwLinkDataIndex );
	if ( iter != m_mapLinkData.end() )
	{
		const SLINK_DATA_INDEX& sLinkIndex = iter->second;
		DeleteLinkData( sLinkIndex );
		m_mapLinkData.erase( iter );
	}

	// 링크를 추가한다;
	int nDataIndex = AddLinkDataToTypeList( strCharName, sLinkType, pBuffer, dwSize );
	if( 0 <= nDataIndex ) 
	{
		SLINK_DATA_INDEX sLinkDataAdd;
		sLinkDataAdd.sLinkType = sLinkType;
		sLinkDataAdd.sLinkPos = sLinkPos;
		sLinkDataAdd.nLinkIndex = nDataIndex;
		m_mapLinkData.insert( std::make_pair( m_dwLinkDataIndex, sLinkDataAdd ) );

		nLinkIndex = m_dwLinkDataIndex;	

		m_dwLinkDataIndex++;

		return nLinkIndex;
	}

	return nLinkIndex;
}

int	GLChatMan::AddLinkDataToTypeList( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize )
{
	int nIndex = -1;

	switch( sLinkType.wMType ) 
	{
	case ITEM_LINK :
		{
			switch( sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN :
			case ITEM_LINK_PUTON :
			case ITEM_LINK_USER_STORAGE :
			case ITEM_LINK_CLUB_STORAGE :
			case ITEM_LINK_CROW_SALE :
			case ITEM_LINK_NORMAL :
				{
					SITEMCUSTOM sItemCustom;

					if( dwSize != sizeof( SITEMCUSTOM ) )
						return nIndex;

					memcpy( &sItemCustom, pBuffer, dwSize );

					// 인덱스를 모두 소모한 경우 0 으로 초기화한다;
					if ( CHAT_LINK_BASIC_DATA_MAX <= m_dwItemLinkDataIndex )
						m_dwItemLinkDataIndex = 0;

					// 이미 사용중인 Index 라면 지운다;
					MAP_ITEM_LINK_DATA_INDEX_ITER iter = m_mapItemLinkData.find( m_dwItemLinkDataIndex );
					if ( iter != m_mapItemLinkData.end() )
					{
						m_mapItemLinkData.erase( iter );
					}

					// 아이템 링크를 추가한다;
					SLINK_DATA_ITEM sData;
					sData.strCharName = strCharName;
					sData.sItem = sItemCustom;

					m_mapItemLinkData.insert( std::make_pair( m_dwItemLinkDataIndex, sData ) );
					nIndex = m_dwItemLinkDataIndex;

					m_dwItemLinkDataIndex++;

					return nIndex;
				}
				break;
			}
		}
		break;

	case PARTY_LINK :
		{
			switch( sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT :
				{
					GLPARTY_LINK sPartyLink;
					DWORD dwOffset  = 0;
					DWORD dwCurSize = 0;

					// 맵아이디복사
					memcpy( &sPartyLink.sMapID, pBuffer, sizeof( SNATIVEID ) );
					dwOffset += sizeof( SNATIVEID );

					if( dwSize < dwOffset )
						return nIndex;

					dwCurSize = dwSize - dwOffset;

					if( 0 != dwCurSize % sizeof( GLPARTY_LINK_MEM ) )
						return nIndex;

					int nPartyNum = dwCurSize / sizeof( GLPARTY_LINK_MEM );
					if( MAXPARTY < nPartyNum )
						return nIndex;

					memcpy( sPartyLink.sPartyMem, (BYTE*) pBuffer+dwOffset, dwCurSize );

					// 인덱스를 모두 소모한 경우 0 으로 초기화한다;
					if ( CHAT_LINK_BASIC_DATA_MAX <= m_dwPartyLinkDataIndex )
						m_dwPartyLinkDataIndex = 0;

					// 이미 사용중인 Index 라면 지운다;
					MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( m_dwPartyLinkDataIndex );
					if ( iter != m_mapPartyLinkData.end() )
					{
						m_mapPartyLinkData.erase( iter );
					}

					// 파티 링크를 추가한다;
					GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
					for( int i = 0; i < MAXPARTY; ++i )
					{
						if( FALSE == sPartyLink.sPartyMem[ i ].VALID() )
							continue;

						// 내 파티일 경우 지원 요청 못하도록한다.
						if( sPartyLink.sPartyMem[ i ].dwGaeaID == pChar->GetGaeaID() )
						{
							sPartyLink.bOnce = TRUE;
							break;
						}
					}

					m_mapPartyLinkData.insert( std::make_pair( m_dwPartyLinkDataIndex, sPartyLink ) );
					nIndex = m_dwPartyLinkDataIndex;

					m_dwPartyLinkDataIndex++;

					return nIndex;
				}
				break;

			case PARTY_LINK_ACCEPT :
				{
					const DWORD* const pBuffer4Byte = (const DWORD* const) pBuffer;
					GLPARTY_LINK sPartyLink;
					sPartyLink.sPartyMem[ 0 ].dwGaeaID = pBuffer4Byte[ 0 ];
					sPartyLink.dwHashKey = pBuffer4Byte[ 1 ];
					sPartyLink.dwTimeKey = pBuffer4Byte[ 2 ];

					// 인덱스를 모두 소모한 경우 0 으로 초기화한다;
					if ( CHAT_LINK_BASIC_DATA_MAX <= m_dwPartyLinkDataIndex )
						m_dwPartyLinkDataIndex = 0;

					// 이미 사용중인 Index 라면 지운다;
					MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( m_dwPartyLinkDataIndex );
					if ( iter != m_mapPartyLinkData.end() )
					{
						m_mapPartyLinkData.erase( iter );
					}

					// 파티 링크를 추가한다;
					m_mapPartyLinkData.insert( std::make_pair( m_dwPartyLinkDataIndex, sPartyLink ) );
					nIndex = m_dwPartyLinkDataIndex;

					m_dwPartyLinkDataIndex++;

					return nIndex;
				}
				break;

			case PARTY_LINK_REFUSE :
				{
					GLPARTY_LINK sPartyLink;
					memcpy( &sPartyLink.sPartyMem[ 0 ].dwGaeaID, pBuffer, dwSize );

					// 인덱스를 모두 소모한 경우 0 으로 초기화한다;
					if ( CHAT_LINK_BASIC_DATA_MAX <= m_dwPartyLinkDataIndex )
						m_dwPartyLinkDataIndex = 0;

					// 이미 사용중인 Index 라면 지운다;
					MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( m_dwPartyLinkDataIndex );
					if ( iter != m_mapPartyLinkData.end() )
					{
						m_mapPartyLinkData.erase( iter );
					}

					// 파티 링크를 추가한다;
					m_mapPartyLinkData.insert( std::make_pair( m_dwPartyLinkDataIndex, sPartyLink ) );
					nIndex = m_dwPartyLinkDataIndex;

					m_dwPartyLinkDataIndex++;

					return nIndex;
				}
				break;
			}
		}
		break;

	case QUEST_LINK :
	case SKILL_LINK :
	case CROW_LINK :
	case MAP_LINK :
		{
		}
		break;
	}

	return nIndex;
}

void GLChatMan::DeleteLinkData( const SLINK_DATA_INDEX& sLinkIndex )
{
	int nLinkIndex = sLinkIndex.nLinkIndex;
	if( nLinkIndex < 0 )
		return;

	switch( sLinkIndex.sLinkType.wMType ) 
	{
	case ITEM_LINK :
		{
			switch( sLinkIndex.sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN :
			case ITEM_LINK_PUTON :
			case ITEM_LINK_USER_STORAGE :
			case ITEM_LINK_CLUB_STORAGE :
			case ITEM_LINK_CROW_SALE :
			case ITEM_LINK_NORMAL :
				{
					MAP_ITEM_LINK_DATA_INDEX_ITER iter = m_mapItemLinkData.find( nLinkIndex );	
					if( iter != m_mapItemLinkData.end() )
						m_mapItemLinkData.erase( iter );
				}
				break;
			}
		}
		break;

	case PARTY_LINK:
		{
			switch( sLinkIndex.sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT :
			case PARTY_LINK_ACCEPT :
			case PARTY_LINK_REFUSE :
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( nLinkIndex );
					if( iter != m_mapPartyLinkData.end() )
						m_mapPartyLinkData.erase( iter );
				}
				break;
			}
		}
		break;
	}
}

SLINK_DATA_INDEX* GLChatMan::GetLinkBasicData( int nLinkIndex )
{
	MAP_LINK_DATA_INDEX_ITER iter = m_mapLinkData.find( nLinkIndex );
	if( iter == m_mapLinkData.end() )
		return NULL;	

	return &iter->second;
}

void GLChatMan::AddChatMessage( int nLogID, DWORD dwType, const std::string& strMsg, DWORD dwColor,
							    const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
								const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName )
{
	GLChat chat( nLogID, dwType, strMsg, dwColor );
	chat.m_vecCustomColor = vecCustomColor;
	chat.m_sReferName = sReferName;
	m_listChat.push_back( chat );

	if( GLCHAT_DEF::g_wMaxChatCount < m_listChat.size() )
		m_listChat.pop_front();

	if( true == m_bChatLog )
		SaveChatLog( strMsg.c_str(), dwType );
}

void GLChatMan::AddLinkMessage( int nLogID, DWORD dwType, const std::string& strMsg, DWORD dwColor, 
							   const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
							   const VEC_LINK_TEXT_DATA& vecLinkIndex, 
							   const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName )
{
	GLChat chat( nLogID, dwType, strMsg, dwColor );
	chat.m_vecCustomColor = vecCustomColor;
	chat.m_vecLinkTextData = vecLinkIndex;
	chat.m_sReferName = sReferName;
	m_listChat.push_back( chat );

	if( GLCHAT_DEF::g_wMaxChatCount < m_listChat.size() )
		m_listChat.pop_front();

	if( true == m_bChatLog )
		SaveChatLog( strMsg.c_str(), dwType );
}

void GLChatMan::ClearMessage( int nLogID )
{
	LIST_CHAT_ITER iter = m_listChat.begin();
	for( ; iter != m_listChat.end(); )
	{
		if( nLogID == iter->m_nLogID )
			iter = m_listChat.erase( iter );
		else
			++iter;
	}
}

void GLChatMan::GetChatList( int nLogID, LIST_CHAT& listChat )
{
	LIST_CHAT_ITER iter = m_listChat.begin();
	for( ; iter != m_listChat.end(); ++iter )
	{
		if( nLogID == iter->m_nLogID )
			listChat.push_back( (*iter) );
	}
}

void GLChatMan::GetChatList( int nLogID, DWORD dwType, LIST_CHAT& listChat )
{
	LIST_CHAT_ITER iter = m_listChat.begin();
	for( ; iter != m_listChat.end(); ++iter )
	{
		if( nLogID == iter->m_nLogID && dwType & iter->m_dwType )
			listChat.push_back( (*iter) );
	}
}

void GLChatMan::SetChatMacro( WORD wIndex, const char* strText )
{
	if( CHAT_MACRO_NUM <= wIndex )
		return;

	RANPARAM::ChatMacro[ wIndex ] = strText;
}

std::string GLChatMan::GetChatMacro( WORD wIndex )
{
	if( CHAT_MACRO_NUM <= wIndex )
		return "";

	return RANPARAM::ChatMacro[ wIndex ].GetBuffer();
}

LuaTable GLChatMan::CreatePartyLink()
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );

	SLINK_DATA_BASIC sLinkDataBasic;
	sLinkDataBasic.sLinkType.wMType = PARTY_LINK;
	sLinkDataBasic.sLinkType.wSType = PARTY_LINK_RECRUIT;
	sLinkDataBasic.dwData1 = GLWidgetScript::GetInstance().m_pGaeaClient->GetActiveMapID().getBaseMapID().Id();

	CString strLinkName;
	DWORD dwColor = NS_UITEXTUTIL::ENABLE;

	if( false == GetLinkData( sLinkDataBasic, strLinkName, dwColor ) )
		return tb;

	if( false == sLinkDataBasic.VALID() )
		return tb;

	LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
	tbData.set( "wMType", sLinkDataBasic.sLinkType.wMType );
	tbData.set( "wSType", sLinkDataBasic.sLinkType.wSType );
	tbData.set( "nStart", (int) 0 );	// sLinkPos.nStart - 루아에서 설정하도록 함.
	tbData.set( "nEnd", (int) 0 );		// sLinkPos.nEnd - 루아에서 설정하도록 함.
	tbData.set( "dwData1", sLinkDataBasic.dwData1 );
	tbData.set( "dwData2", sLinkDataBasic.dwData2 );
	tbData.set( "dwColor", dwColor );

	tb.set( 1, tbData );
	tb.set( 2, strLinkName.GetBuffer() );

	return tb;
}

void GLChatMan::SetPartyLinkOnce( int nLinkIndex )
{
	MAP_PARTY_LINK_DATA_INDEX_ITER iter = m_mapPartyLinkData.find( nLinkIndex );
	if( iter == m_mapPartyLinkData.end() )
		return;

	iter->second.bOnce = TRUE;
}

bool GLChatMan::CheckBlock( const CString& strMsg )
{
	// GM 채팅 블락 시간 다 끝났는지 확인
	if( true == m_bGMBlock )
	{
		GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
		GLCharacter* pChar = pClient->GetCharacter();

		CTime cTime( CTime::GetCurrentTime() );
		CTime cBlock( pChar->m_tCHATBLOCK );
		pClient->GetConvertServerTime( cTime );

		CTimeSpan cSpan = cBlock - cTime;
		if( 0 >= cSpan.GetTimeSpan() )
			m_bGMBlock = false;
	}
	// 도배 채팅 블락 확인
	else
	{
		if( false == m_bPaperingBlock && false == strMsg.IsEmpty() )
		{
			// 같은 메시지 들어왔는지 확인
			if( strMsg == m_strLastChat )
			{
				++m_nPaperingCount;

				if( m_nPaperingCount > GLCONST_CHAR::nCHAT_OVERINPUT_COUNT )
				{
					m_fPaperingBlockStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
					m_bPaperingBlock = true;
				}
			}
			else
			{
				m_nPaperingCount = 0;
				m_strLastChat = strMsg;
			}
		}
		else if( true == m_bPaperingBlock )
		{
			// 블락 시간 지났나?
			float fCurTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
			if( fCurTime - m_fPaperingBlockStartTime >= GLCONST_CHAR::wCHAT_OVERINPUT_CANCEL_TIMER )
			{
				m_nPaperingCount = 0;
				m_bPaperingBlock = false;
			}
		}
		
		/*-- 감옥에서 채팅 금지 -------------------------------------------------------------------*/
		/*
		GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;

		if( pClient ) {
			GLLandManClient* pLand = pClient->GetActiveMap();
			GLCharacter* pChar = pClient->GetCharacter();

			if( pLand && pChar ) {
				const SMAPNODE* pMapNode = pClient->FindMapNode( pLand->GetBaseMapID().dwID );

				if( pMapNode && PRISONLIMIT.IsLimitStatus( pMapNode, pChar->UserLevel(), PML_CHATTING ) ) {
					m_bPrisonBlock = true;
				}
				else {
					m_bPrisonBlock = false;
				}
			}
		}
		*/
	}

	return m_bGMBlock || m_bPaperingBlock || m_bPrisonBlock;
}

LuaTable GLChatMan::GetChatBlock()
{
	enum BLOCKTYPE
	{
		TYPE_NONE = -1,
		TYPE_PAPERING = 0,
		TYPE_GM,
		TYPE_PRISON,
	};

	BLOCKTYPE emType = TYPE_NONE;
	bool bBlock = CheckBlock();
	unsigned int nSec = 0;
	
	if( true == m_bGMBlock )
	{
		GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
		GLCharacter* pChar = pClient->GetCharacter();

		CTime cTime( CTime::GetCurrentTime() );
		CTime cBlock( pChar->m_tCHATBLOCK );
		pClient->GetConvertServerTime( cTime );

		CTimeSpan cSpan = cBlock - cTime;

		nSec = unsigned int( cSpan.GetTotalSeconds() );
		emType = TYPE_GM;
	}
	else if( true == m_bPaperingBlock )
	{
		float fCurTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );

		nSec = unsigned int( GLCONST_CHAR::wCHAT_OVERINPUT_CANCEL_TIMER - WORD( fCurTime - m_fPaperingBlockStartTime ) );
		emType = TYPE_PAPERING;
	}
	else if( true == m_bPrisonBlock )
	{
		emType = TYPE_PRISON;
	}

	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
	tb.set( 1, bBlock );
	tb.set( 2, nSec );
	tb.set( 3, (int) emType );

	return tb;
}