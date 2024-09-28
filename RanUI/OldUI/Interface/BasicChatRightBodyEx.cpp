#include "StdAfx.h"
#include "BasicChatRightBody.h"

#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Util/EBTime.h"

#include "../../../MfcExLib/RanFilter.h"

#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../../EngineLib/Common/SubPath.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DxInputDevice.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/Common/SubPath.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Command/dxincommand.h"
#include "../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../../RanLogicClient/Emoticon/Emoticon.h"
#include "../../../RanLogicClient/ReferChar/ReferChar.h"

#include "../../InnerInterface.h"

#include "../Interface/ChatType.h"
#include "../Interface/SystemMessageDisplay.h"
#include "../Display/HeadChatDisplayMan.h"
#include "../Post/PostBoxWindow.h"
#include "../Post/PostBoxSendPage.h"

#include "BasicTextBoxExLink.h"
#include "BasicChatRightBody.h"

#include "shlobj.h"

#include "../../../RanLogic/Prison/LimitPrison.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CBasicChatRightBody::ADD_CHATLOG ( const CString& strChat )
{
	if( strChat.IsEmpty() ) return;

	NORMAL_CHAT_LOG_ITER iter = m_ChatLog.begin ();
	NORMAL_CHAT_LOG_ITER iter_end = m_ChatLog.end ();
	for ( ; iter != iter_end; ++iter )	//	혹시 중간에 있다면...
	{
		if( (*iter) == strChat )
		{
			m_ChatLog.erase( iter );
			m_ChatPos = OUT_OF_RANGE;
			break;
		}
	}

	m_ChatLog.push_front ( strChat );

	if ( m_ChatLog.size () > 50 )
	{
		m_ChatLog.pop_back ();
	}
}

VOID CBasicChatRightBody::DecreaseTime( float fElapsedTime )			
{ 
	if( m_bPapering && ( m_fLifeTime > 0.0f ) ) 
	{
		m_fLifeTime -= fElapsedTime;
	}
}

BOOL CBasicChatRightBody::IsChatBlock()
{
	GLCharacter* pChar =  m_pGaeaClient->GetCharacter();
	if ( pChar == NULL )
	{
		return false;
	}

	if ( pChar->m_tCHATBLOCK == 0 )
	{
		return false;
	}

	CTime cBLOCK(pChar->m_tCHATBLOCK);
	CTime cCUR(CTime::GetCurrentTime());

	// 채팅금지 시간인지 검사	
	if ( cBLOCK > cCUR )
	{
		return true;
	}

	// 채팅금지이 초과 되었을 경우는 시간을 리셋, 채팅금지 해제
	pChar->m_tCHATBLOCK = 0;
	//pChar->m_bCHATBLOCK = false;

	return false;
}

BOOL CBasicChatRightBody::IsPapering( const CString& strOrigin )
{
	if ( GetLifeTime() > 0.0f ) 
		return TRUE;
	else 
		m_bPapering = FALSE;

	//	Memo :	같은 메세지 중복 처리
	if ( m_strLastMsg == strOrigin )
	{
		++m_nInputCount;

		if ( m_nInputCount > GLCONST_CHAR::nCHAT_OVERINPUT_COUNT )
		{
			m_bPapering = TRUE;
		}
	}
	else 
	{
		m_nInputCount = 0;			// 연속되지 않을 경우 초기화
		m_strLastMsg = strOrigin;	// 이전 메세지 저장
	}

	//	Memo :	연속된 메세지 중복 처리
	static FLOAT s_fStartTime = 0.f;
	static FLOAT s_fEndTime = 0.f;
	static BOOL bStart(TRUE);

	if ( bStart )
	{
		s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
		bStart = FALSE;
	}
	else
	{
		s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );

        if ( ( s_fEndTime-s_fStartTime) <= GLCONST_CHAR::fCHAT_OVERINPUT_TIMER )
		{
			// 설정된 시간 안에 메세지가 들어왔다.
			++m_nTimeCount;

            if ( m_nTimeCount >= GLCONST_CHAR::nCHAT_OVERINPUT_COUNT )
			{
				m_bPapering = TRUE;
			}
			else
			{
				s_fStartTime = s_fEndTime;
			}
		}
		else
		{
			m_nTimeCount = 0;	// 연속되지 않을 경우 초기화
			bStart = TRUE;
		}
	}

	if ( m_bPapering )
	{
        std::string strNotify = sc::string::format( ID2GAMEINTEXT("CHAT_PAPERING_MODE"), GLCONST_CHAR::wCHAT_OVERINPUT_CANCEL_TIMER );
		AddStringToNORMAL( strNotify.c_str(), NS_UITEXTCOLOR::ORANGE );
		SetLifeTime( GLCONST_CHAR::wCHAT_OVERINPUT_CANCEL_TIMER );

		//	Memo :	모든 체크 변수들을 초기화 한다.
		m_nInputCount = 0;
		m_strLastMsg.Empty();
		m_nTimeCount = 0;
		bStart = TRUE;

		return TRUE;
	}

	return FALSE;
}

bool CBasicChatRightBody::SEND_COMMON_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData )
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient*	pNetClient = pGlobalStage->GetNetClient();
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();


	CString strTrim = strOrigin;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;



	CString strTemp( strOrigin );

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strTemp, vecTextLinkData, vecLinkDataBasic );

	strTemp.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시
	
	if ( CRanFilter::GetInstance().ChatFilter( strTemp ) )
	{
		strTemp = CRanFilter::GetInstance().GetProverb();
	}

	// 서버 전송 데이터로 변경
	if ( pCharacter ) 
		pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_NORMAL, pCharacter->m_szName, strTemp.GetString(), vecLinkDataBasic );

	ADD_CHATLOG ( strOrigin.GetString() );

	//	클라이언트 데이터로 변환		
	VEC_LINK_TEXT_DATA vecLinkData;
	m_pInterface->ConvertLinkData( vecTextLinkData, vecLinkData );
	m_pInterface->ConvertRecvData( strTemp, vecLinkData );	


	CString strType;
	strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",0), pCharacter->m_szName );
	CString strCombine = strType + strTemp;	

	int nLength = strType.GetLength();

	for ( DWORD i = 0; i < vecLinkData.size(); ++i )
	{
		vecLinkData[i].sLinkPos.ADD( nLength );
	}

	if( pCharacter->m_tCHATBLOCK == 0 )
	{		
		AddStringToChatLink ( strCombine, CHAT_CONST::CHAT_NORMAL, vecLinkData, (pCharacter->m_dwUserLvl >= USER_USER_GM ) ? true : false );
		ShowSelfMessageOnHead ( strTemp );
	}	

	return true;
}

bool CBasicChatRightBody::SEND_PRIVATE_MESSAGE( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	CString strTrim = strOrigin;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;

	if ( m_strPrivateName.GetLength () )
	{
		ADD_FRIEND ( m_strPrivateName );
		m_pInterface->ADD_FRIEND_NAME_TO_EDITBOX ( m_strPrivateName );
	}
	else return false;

	//	필터링
	CString strTemp( strOrigin );

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strTemp, vecTextLinkData, vecLinkDataBasic );

	strTemp.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시
	
	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
	{
		strTemp = CRanFilter::GetInstance().GetProverb();
	}

	// 서버 전송 데이터로 변경
	if ( pCharacter ) 
		pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_PRIVATE, m_strPrivateName.GetString(), strTemp.GetString(), vecLinkDataBasic );

	
	VEC_LINK_TEXT_DATA vecLinkData;
	m_pInterface->ConvertLinkData( vecTextLinkData, vecLinkData );
	m_pInterface->ConvertRecvData( strTemp, vecLinkData );	

	CString strType(m_strPrivateName), strType2;
	strType = sc::string::format( ID2GAMEWORD("CHAT_TYPE",1), m_strPrivateName ).c_str();
//	strType.Append(ID2GAMEWORD("CHAT_TYPE",1));
	strType2.Format("[%s]", strType );	
	CString strCombine = strType2 + strTemp;


	int nLength = strType2.GetLength();
	for ( DWORD i = 0; i < vecLinkData.size(); ++i )
	{
		vecLinkData[i].sLinkPos.ADD( nLength );
	}

	AddStringToChatLink ( strCombine, CHAT_CONST::CHAT_PRIVATE, vecLinkData, bGM );

	return true;
}

bool CBasicChatRightBody::SEND_PARTY_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	CString strTrim = strOrigin;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;

	//	필터링
	CString strTemp = strOrigin;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strTemp, vecTextLinkData, vecLinkDataBasic );

	strTemp.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시
	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
	{
		strTemp = CRanFilter::GetInstance().GetProverb();
	}

	// 서버 전송 데이터로 변경
	CHAT_CONST::CHAT_TYPE emChatType = CHAT_CONST::CHAT_PARTY;
	if ( pCharacter ) 
	{		
		GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();		
		if ( pPartyManager->isExpeditionSubMaster() && pPartyManager->isMasterChatting() )
			emChatType = CHAT_CONST::CHAT_MASTER;

		pCharacter->ReqSendChatMsg ( emChatType, pCharacter->m_szName, strTemp.GetString(), vecLinkDataBasic );
	}


	CString strType;
	strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",2), pCharacter->m_szName );
	CString strCombine = strType + strTemp;

	VEC_LINK_TEXT_DATA vecLinkData;
	m_pInterface->ConvertLinkData( vecTextLinkData, vecLinkData );
	m_pInterface->ConvertRecvData( strCombine, vecLinkData );	

	AddStringToChatLink ( strCombine, emChatType, vecLinkData, bGM );

	return true;
}

bool CBasicChatRightBody::SEND_TOALL_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();


	CString strTrim = strOrigin;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;


	CString strMsg = strOrigin;
	
	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strMsg, vecTextLinkData, vecLinkDataBasic );

	strMsg.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pCharacter ) pCharacter->ReqLoudSpeaker ( strMsg.GetString (), vecLinkDataBasic );

	return true;
}

bool CBasicChatRightBody::SEND_GUILD_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	CString strTrim = strOrigin;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;

	CString strMsg = strOrigin;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strMsg, vecTextLinkData, vecLinkDataBasic );

	strMsg.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();



	// 서버 전송 데이터로 변경
	if ( pCharacter ) 
		pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_GUILD, pCharacter->m_szName, strMsg.GetString(), vecLinkDataBasic );


	return true;
}

bool CBasicChatRightBody::SEND_ALLIANCE_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	CString strMsg = strOrigin;

	CString strTrim = strMsg;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strMsg, vecTextLinkData, vecLinkDataBasic );

	strMsg.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pCharacter ) 
		pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_ALLIANCE, pCharacter->m_szName, strMsg.GetString(), vecLinkDataBasic );

	return true;
}

bool CBasicChatRightBody::SEND_AREA_MESSAGE( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	CString strMsg = strOrigin;

	CString strTrim = strMsg;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strMsg, vecTextLinkData, vecLinkDataBasic );

	strMsg.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pCharacter ) pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_AREA, pCharacter->m_szName, strMsg.GetString(), vecLinkDataBasic );

	return true;
}

bool CBasicChatRightBody::SEND_PARTY_RECRUIT_MESSAGE( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	CString strMsg = strOrigin;

	CString strTrim = strMsg;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strMsg, vecTextLinkData, vecLinkDataBasic );

	strMsg.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pCharacter ) pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_PARTY_RECRUIT, pCharacter->m_szName, strMsg.GetString(), vecLinkDataBasic );

	return true;
}

bool CBasicChatRightBody::SEND_SYSTEM_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
        return false;
	if ( pCharacter->UserLevel() < USER_USER_GM )
        return true;

	CString strMsg = strOrigin;

	CString strTrim = strMsg;
	strTrim.Trim ( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( !strTrim.GetLength () ) return false;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;
	
	// 서버 전송 데이터로 변환
	m_pInterface->ConvertSendData ( strMsg, vecTextLinkData, vecLinkDataBasic );

	strMsg.TrimLeft ( CHAT_CONST::CHAT_SIMBOL_BLANK );	//	좌측 공백 무시

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pCharacter ) pCharacter->ReqSendChatMsg ( CHAT_CONST::CHAT_SYSTEM, pCharacter->m_szName, strMsg.GetString(), vecLinkDataBasic );

	return true;
}

bool CBasicChatRightBody::SEND_FACTION_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( NULL == pCharacter )
		return false;

	CString strMsg = strOrigin;

	CString strTrim = strMsg;
	strTrim.Trim( CHAT_CONST::CHAT_SIMBOL_BLANK );
	if ( 0 == strTrim.GetLength() )
		return false;

	VEC_LINK_DATA_BASIC vecLinkDataBasic;

	// 서버 전송 데이터로 변환;
	m_pInterface->ConvertSendData(
		strMsg, vecTextLinkData, vecLinkDataBasic );

	// 좌측 공백 무시;
	strMsg.TrimLeft( CHAT_CONST::CHAT_SIMBOL_BLANK );

	// 메시지 전송;
	pCharacter->ReqSendChatMsg(
		CHAT_CONST::CHAT_FACTION,
		pCharacter->m_szName,
		strMsg.GetString(),
		vecLinkDataBasic );

	return true;
}

bool CBasicChatRightBody::SEND_CHAT_MESSAGE ( const CString& strOrigin, VEC_LINK_TEXT_DATA_EX vecTextLinkData )
{
	GLCharacter* pChar =  m_pGaeaClient->GetCharacter();
	if( pChar == NULL )
		return FALSE;

	if( m_dwChatType == CHAT_CONST::CHAT_NORMAL || m_dwChatType == CHAT_CONST::CHAT_TOALL ||  
		m_dwChatType == CHAT_CONST::CHAT_AREA ) 
	{
		if( IsChatBlock() )
		{
			CTime cTime = CTime::GetCurrentTime();
			CTime cBLOCK = pChar->m_tCHATBLOCK;
			m_pGaeaClient->GetConvertServerTime( cTime );
			
			CTimeSpan cSPAN = cBLOCK - cTime;			

			std::string strChatBlock = sc::string::format( ID2GAMEINTEXT("CHAT_BLOCK_STATE"), cSPAN.GetTotalMinutes() );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, strChatBlock );
			return FALSE;
		}

		if ( IsPapering( strOrigin ) )
        {
            std::string strText( ID2GAMEINTEXT("NOTIFY_CHAT_OVER_INPUT") );
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, strText );

            return FALSE;
        }
	}


//#if defined( TH_PARAM )
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	if (sp == SP_THAILAND)
	{
		if ( !IsThaiCheck( strOrigin ) )
			return FALSE;
	}
//#endif 

//#if defined( VN_PARAM )
	if (sp == SP_VIETNAM)
	{
		if ( !IsVnCheck( strOrigin ) ) return FALSE;
	}
//#endif 

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();

	bool bPrison = false;
	bool bEnable = true;

	if( pLand && pChar ) {
		const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLand->GetBaseMapID() );

		if( pMapNode && PRISONLIMIT.IsLimitStatus( pMapNode, pChar->UserLevel(), PML_CHATTING ) ) {
			bPrison = true;	
		}
	}

	if( bPrison ) {
		bEnable = (m_dwChatType != CHAT_CONST::CHAT_NORMAL) ? false : true;
	}	

	if( !bEnable ) {
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "CHAT_PRISION_LIMIT" ) );
		return FALSE;
	}

	bool bGM(pChar->m_dwUserLvl >= USER_USER_GM );
	switch ( m_dwChatType )
	{
	case CHAT_CONST::CHAT_PRIVATE:			return SEND_PRIVATE_MESSAGE( strOrigin, vecTextLinkData, bGM );
	case CHAT_CONST::CHAT_PARTY:			return SEND_PARTY_MESSAGE( strOrigin, vecTextLinkData, bGM );
	case CHAT_CONST::CHAT_GUILD:			return SEND_GUILD_MESSAGE( strOrigin, vecTextLinkData, bGM );
	case CHAT_CONST::CHAT_ALLIANCE:			return SEND_ALLIANCE_MESSAGE( strOrigin, vecTextLinkData, bGM );
	case CHAT_CONST::CHAT_AREA:				return SEND_AREA_MESSAGE( strOrigin, vecTextLinkData, bGM );
	case CHAT_CONST::CHAT_PARTY_RECRUIT:	return SEND_PARTY_RECRUIT_MESSAGE( strOrigin, vecTextLinkData, bGM );
	case CHAT_CONST::CHAT_TOALL:			return SEND_TOALL_MESSAGE( strOrigin, vecTextLinkData, bGM );
	case CHAT_CONST::CHAT_SYSTEM:			return SEND_SYSTEM_MESSAGE( strOrigin, vecTextLinkData );
	case CHAT_CONST::CHAT_FACTION:			return SEND_FACTION_MESSAGE( strOrigin, vecTextLinkData );

	default:
		{
			//	Note : 제스쳐.
			int nMOTION = CEmoticon::GetInstance().GetEmoticonOfTerm ( strOrigin.GetString() );
			if ( nMOTION!=UINT_MAX )
			{
				m_pGaeaClient->GetCharacter()->ReqGESTURE ( nMOTION );
			}

			return SEND_COMMON_MESSAGE ( strOrigin, vecTextLinkData );
		}
		break;
	}
}

HRESULT CBasicChatRightBody::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	m_pScrollBar_ALL->GetThumbFrame()->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );

	return hr;
}

void CBasicChatRightBody::EditBoxControlCopy()
{
    if (DxInputDevice::GetInstance().IsCtrlDown() && (DxInputDevice::GetInstance().GetKeyState(DIK_C) & DXKEY_DOWN))
    {                        
        if (m_pEditBox->IsBegin())
        {            
            const CString& EditStr = m_pEditBox->GetEditString();
            sc::string::stringToClipboard(EditStr.GetString());
        }
    }
}

void CBasicChatRightBody::EditBoxControlPaste()
{
    if (DxInputDevice::GetInstance().IsCtrlDown() && (DxInputDevice::GetInstance().GetKeyState(DIK_V) & DXKEY_DOWN))
    {                        
        if (m_pEditBox->IsBegin())
        {
            std::string ClipBoardText;
            sc::string::clipboardToString2(NULL, ClipBoardText);
			m_pEditBox->InsertString(ClipBoardText.c_str());
        }
    }
}

void CBasicChatRightBody::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( m_bFREEZE ) return ;

	switch ( ControlID )
	{
	case CHAT_EDITBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );

				if ( UIMSG_LB_DOWN & dwMsg )
				{
					ADD_FRIEND_LIST();
					EDIT_BEGIN ();
				}
			}
			else
			{
				if ( m_pEditBox->IsMODE_NATIVE () )
				{
					DWORD dwNativeMsg = m_pNativeButton->GetMessageEx();
					if (CHECK_MOUSE_IN(dwNativeMsg))
                        return;
				}
				else
				{
					DWORD dwEnglishMsg = m_pEnglishButton->GetMessageEx();
					if (CHECK_MOUSE_IN(dwEnglishMsg))
                        return;          
				}

				if ( m_pEditBox->IsBegin () )
				{
					if ((UIMSG_LB_DOWN & dwMsg) || (UIMSG_MB_DOWN & dwMsg) || (UIMSG_RB_DOWN & dwMsg))
					{
						if ( !(DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED) )
							EDIT_END ();
					}
				}
			}
		}
		break;
		
	case CHAT_NATIVE_BUTTON:
	case CHAT_ENGLISH_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pEditBox->DoMODE_TOGGLE ();
			}
		}
		break;

	case CHAT_CHANNEL_TOTAL:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_TOTAL;
			}
		}
		break;

	case CHAT_CHANNEL_CHANNEL:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_CHANNEL;
			}
		}
		break;

	case CHAT_CHANNEL_PARTY:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_PARTY;
			}
		}
		break;

	case CHAT_CHANNEL_CLUB:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_CLUB;
			}
		}
		break;

	case CHAT_CHANNEL_ALLIANCE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_ALLIANCE;
			}
		}
		break;

	case CHAT_CHANNEL_SYSTEM:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_SYSTEM;
			}
		}
		break;

	case CHAT_TEXTBOX_ALL:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					int nLinkIndex = m_pTextBox_ALL->GetSelectWordData();
					if ( nLinkIndex != -1 ) 
					{
						//	링크 처리
						CDebugSet::ToView( 7,1, "Select Data m_pTextBox_ALL : %d", nLinkIndex );

						m_pInterface->LinkDataProcess( nLinkIndex );
						return;
					}
				}

				if ( UIMSG_LB_DUP & dwMsg )
				{
					const int nIndex = m_pTextBox_ALL->GetSelectPos();
					if( nIndex < 0 || m_pTextBox_ALL->GetCount () <= nIndex )
						return;

                    CString strWord = m_pTextBox_ALL->GetText( nIndex );
					//CString strWord = m_pTextBox_ALL->GetSelectWordText();

					// Note : 우체통이 열려있다면 이름을 붙여넣는다.
					if ( ADD_NAME_TO_POSTBOX ( strWord ) )
					{
						break;
					}
					else
					{
						AUTO_PRIVATE_CHAT( strWord );
					}
				}

				if ( CHECK_MOUSEIN_RBUPLIKE(dwMsg) )
				{
					const int nIndex = m_pTextBox_ALL->GetSelectPos();
					if( nIndex < 0 || m_pTextBox_ALL->GetCount() <= nIndex )
						return;

					CString strWord = m_pTextBox_ALL->GetText( nIndex );

					if ( DxInputDevice::GetInstance().GetKeyState(DIK_LALT)&DXKEY_PRESSED )
					{
						SLINK_DATA_INDEX* pLinkBasic = m_pInterface->GetLinkBasicData( m_pTextBox_ALL->GetSelectWordData() );
						if (!pLinkBasic) return;
						if ( pLinkBasic->nLinkIndex < 0 ) return;

						const int nDataIndex = pLinkBasic->nLinkIndex;
						switch ( pLinkBasic->sLinkType.wMType ) 
						{
						case ITEM_LINK:
							{
								SLINK_DATA_ITEM sLinkDataItem;
								if ( (DxInputDevice::GetInstance().GetKeyState( DIK_LALT ) & DXKEY_PRESSED) &&
									 (m_pInterface->GetLinkdataIndex(sLinkDataItem, nDataIndex)))
								{
									SSETITEM* pSetItem = const_cast<SSETITEM*>(GLItemSet::GetInstance().GetInfoSetItem(sLinkDataItem.sItem.GetNativeID()));
									if ( pSetItem )
									{
										STARGETID sTargetID(CROW_ITEM, sLinkDataItem.sItem.GetNativeID().dwID );
										m_pInterface->SetitemGroupComposition(pSetItem, sTargetID);
										m_pInterface->OpenSetitemPreviewWindow();
									}
								}
							}
							break;
						}
					}
					else
					{
						if( GLUseFeatures::GetInstance().IsUsingReferChar() )
						{
							ReferChar( strWord );
						}
					}
				}
			}
		}
		break;
	case CHAT_TEXTBOX_PARTY:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					int nLinkIndex = m_pTextBox_PARTY->GetSelectWordData();
					if ( nLinkIndex != -1 ) 
					{
						//	링크 처리
						CDebugSet::ToView( 7,2, "Select Data m_pTextBox_PARTY : %d", nLinkIndex );

						m_pInterface->LinkDataProcess( nLinkIndex );
						return;
					}
				}
			}
		}
		break;
	case CHAT_TEXTBOX_CLUB:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					int nLinkIndex = m_pTextBox_CLUB->GetSelectWordData();
					if ( nLinkIndex != -1 ) 
					{
						//	링크 처리
						CDebugSet::ToView( 7,3, "Select Data m_pTextBox_CLUB : %d", nLinkIndex );

						m_pInterface->LinkDataProcess( nLinkIndex );
						return;
					}
				}
			}
		}
		break;
	case CHAT_TEXTBOX_ALLIANCE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					int nLinkIndex = m_pTextBox_ALLIANCE->GetSelectWordData();
					if ( nLinkIndex != -1 ) 
					{
						//	링크 처리
						CDebugSet::ToView( 7,4, "Select Data m_pTextBox_ALL : %d", nLinkIndex );

						m_pInterface->LinkDataProcess( nLinkIndex );
						return;
					}
				}
			}
		}
		break;
	case CHAT_TEXTBOX_CHANNEL:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					int nLinkIndex = m_pTextBox_CHANNEL->GetSelectWordData();
					if ( nLinkIndex != -1 ) 
					{
						//	링크 처리
						CDebugSet::ToView( 7,5, "Select Data m_pTextBox_ALL : %d", nLinkIndex );

						m_pInterface->LinkDataProcess( nLinkIndex );
						return;
					}
				}
			}
		}
		break;
	case CHAT_TEXTBOX_SYSTEM:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					int nLinkIndex = m_pTextBox_SYSTEM->GetSelectWordData();
					if ( nLinkIndex != -1 ) 
					{
						//	링크 처리
						CDebugSet::ToView( 7,6, "Select Data m_pTextBox_ALL : %d", nLinkIndex );

						m_pInterface->LinkDataProcess( nLinkIndex );
						return;
					}
				}
			}
		}
		break;

	}
}

void CBasicChatRightBody::AddStringToNORMAL ( CString strTemp, D3DCOLOR dwColor, bool bGM )
{
	if ( m_pTextBox_ALL && m_pScrollBar_ALL )
	{
        std::string strType = "";
        std::string strName = "";
        std::string strContent = "";

        GetSeparateCharName( strTemp, strType, strName, strContent );
        int nIndex = m_pTextBox_ALL->AddText( strType.c_str(), dwColor );		

		if( bGM )			
			m_pTextBox_ALL->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_GM );
		else
			m_pTextBox_ALL->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_CHAR_NAME );

        m_pTextBox_ALL->AddString( nIndex, strContent.c_str(), dwColor );

		//m_pTextBox_ALL->AddText( strTemp, dwColor );

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar_ALL->GetThumbFrame ();
			if ( !pThumbFrame )
			{
				GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
				return ;
			}

			int nTotal, nView;
			pThumbFrame->GetState ( nTotal, nView );	

			const int nTotalLine = m_pTextBox_ALL->GetTotalLine ();
			pThumbFrame->SetState ( nTotalLine, nView );
			pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
		}
	}
}

void CBasicChatRightBody::AddStringToNORMAL ( WORD wChatType, CString strTemp, D3DCOLOR dwColor, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM )
{
	if ( m_pTextBox_ALL && m_pScrollBar_ALL )
	{

        std::string strType = "";
        std::string strName = "";
        std::string strContent = "";

        GetSeparateCharName( wChatType, strTemp, vecLinkTextData, strType, strName, strContent );
        int nIndex = m_pTextBox_ALL->AddText( strType.c_str(), dwColor );

		if( bGM )
			m_pTextBox_ALL->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_GM );
		else
			m_pTextBox_ALL->AddString( nIndex, strName.c_str(), NS_UITEXTCOLOR::CHAT_CHAR_NAME );

        m_pTextBox_ALL->AddStringLink( nIndex, strContent.c_str(), vecLinkTextData, dwColor );

        //m_pTextBox_ALL->AddTextLink( strTemp, vecLinkTextData, dwColor );

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar_ALL->GetThumbFrame ();
			if ( !pThumbFrame )
			{
				GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
				return ;
			}

			int nTotal, nView;
			pThumbFrame->GetState ( nTotal, nView );	

			const int nTotalLine = m_pTextBox_ALL->GetTotalLine();
			pThumbFrame->SetState ( nTotalLine, nView );
			pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
		}
	}
}

void CBasicChatRightBody::GetSeparateCharName( WORD wChatType, const CString& strText, VEC_LINK_TEXT_DATA& vecLinkTextData, std::string& strOutType, std::string& strOutName, std::string& strOutContent )
{
    CString strType = "";
    CString strName = "";
    CString strContent = "";

    int nBegin = 0;
    int nEnd = 0;

	switch ( wChatType )
	{
	case CHAT_CONST::CHAT_PRIVATE:
		{
			INT nFind(-1);
			std::string strFind(sc::string::format(ID2GAMEWORD("CHAT_TYPE",9), "").c_str());
			nFind = (nFind = strText.Find(strFind.c_str())) == -1 ? 0 : nFind;

			if ( nFind > 0 )
			{
				strName = strText.Mid(1, nFind-1);
				strType = strText.Mid(0, 1);
			}
			nEnd = nFind;
		}
		break;
	default:
		{
			nBegin = strText.Find( _T('('));
			if ( nBegin >= 0 ) 
			{
				nEnd = strText.Find( _T("):") );
				if ( nEnd > 0 ) 
				{
					int nCnt =  nEnd - nBegin - 1; 

					strName = strText.Mid( nBegin+1, nCnt );

					strType = strText.Mid( 0, nBegin+1 );
				}
				else
				{
					nBegin = 0;
					nEnd = 0;
				}
			}
			else
			{
				nBegin = 0;
				nEnd = 0;
			}
		}
		break;
	}

    strContent = strText.Mid( nEnd );

    strOutType = stringformat( "%1%", strType );
    strOutName = stringformat( "%1%", strName );
    strOutContent = stringformat( "%1%", strContent );

    for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
    {
        vecLinkTextData[i].sLinkPos.nStart -= nEnd;
        vecLinkTextData[i].sLinkPos.nEnd -= nEnd;
    }
}

void CBasicChatRightBody::GetSeparateCharName( const CString& strText, std::string& strOutType, std::string& strOutName, std::string& strOutContent )
{
    CString strType = "";
    CString strName = "";
    CString strContent = "";

    int nBegin = 0;
    int nEnd = 0;

    nBegin = strText.Find( _T('('));
    if ( nBegin >= 0 ) 
    {
        nEnd = strText.Find( _T("):") );
        if ( nEnd > 0 ) 
        {
            int nCnt =  nEnd - nBegin - 1; 

            strName = strText.Mid( nBegin+1, nCnt );

            strType = strText.Mid( 0, nBegin+1 );
        }
        else
        {
            nBegin = 0;
            nEnd = 0;
        }
    }
    else
    {
        nBegin = 0;
        nEnd = 0;
    }

    strContent = strText.Mid( nEnd );

    strOutType = stringformat( "%1%", strType );
    strOutName = stringformat( "%1%", strName );
    strOutContent = stringformat( "%1%", strContent );
}

void CBasicChatRightBody::AddStringToChatEx ( CString strTemp, WORD wType, bool bGM )
{
	D3DCOLOR dwTempColor = CHAT_INFOR_COLOR::CHAT_NORMAL;

    bool bOverInput = false;

    std::string strChatMsg = sc::string::format( "%s", strTemp );
	switch ( wType )
	{
	case CHAT_CONST::CHAT_NORMAL:			//일반 채팅 모드
        {
		    dwTempColor = CHAT_INFOR_COLOR::CHAT_NORMAL;
        }
		break;

	case CHAT_CONST::CHAT_PRIVATE:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_PRIVATE;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_CHANNEL, m_pScrollBar_CHANNEL );
		}
		break;	

	case CHAT_CONST::CHAT_PARTY:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_PARTY;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_PARTY, m_pScrollBar_PARTY );
		}
		break;
	case CHAT_CONST::CHAT_MASTER:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_MASTER;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_PARTY, m_pScrollBar_PARTY );
		}
		break;

	case CHAT_CONST::CHAT_GUILD:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_GUILD;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_CLUB, m_pScrollBar_CLUB );
		}
		break;
	case CHAT_CONST::CHAT_ALLIANCE:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_ALLIANCE;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_ALLIANCE, m_pScrollBar_ALLIANCE );
		}
		break;
	
	case CHAT_CONST::CHAT_AREA:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_AREA;
//			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_CHANNEL, m_pScrollBar_CHANNEL );
		}
		break;
	
	case CHAT_CONST::CHAT_PARTY_RECRUIT:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT;
//			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_CHANNEL, m_pScrollBar_CHANNEL );
		}
		break;

	case CHAT_CONST::CHAT_TOALL:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_TOALL;			
		}
		break;

	case CHAT_CONST::CHAT_SYSTEM:			//에러 또는 시스템 메시지
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_SYSTEM;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_SYSTEM, m_pScrollBar_SYSTEM );
		}
		break;

	case CHAT_CONST::CHAT_FACTION:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_FACTION;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_ALLIANCE, m_pScrollBar_ALLIANCE );
		}
		break;
	}

	//	강제로 추가하는 경우에는
	//	스테이트를 무시한다.
	if ( !m_bIgnoreState && !(m_wDisplayState & wType) ) return ;

	AddStringToNORMAL ( strTemp, dwTempColor, bGM );

	if ( m_bChatLog ) SAVE_CHATLOG( strTemp, wType );
	RECORD_CHAT( strTemp, wType );
}


void CBasicChatRightBody::AddStringToChatLink ( CString strTemp, WORD wType, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM )
{
	D3DCOLOR dwTempColor = CHAT_INFOR_COLOR::CHAT_NORMAL;

	switch ( wType )
	{
	case CHAT_CONST::CHAT_NORMAL:			//일반 채팅 모드
		dwTempColor = CHAT_INFOR_COLOR::CHAT_NORMAL;
		break;

	case CHAT_CONST::CHAT_PRIVATE:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_PRIVATE;
			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_CHANNEL, m_pScrollBar_CHANNEL, vecLinkTextData, wType, bGM );
		}
		break;	

	case CHAT_CONST::CHAT_PARTY:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_PARTY;
			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_PARTY, m_pScrollBar_PARTY, vecLinkTextData, wType, bGM );
		}
		break;
	case CHAT_CONST::CHAT_MASTER:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_MASTER;
			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_PARTY, m_pScrollBar_PARTY, vecLinkTextData, wType, bGM );
		}
		break;
	case CHAT_CONST::CHAT_GUILD:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_GUILD;
			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_CLUB, m_pScrollBar_CLUB, vecLinkTextData, wType, bGM );
		}
		break;
	case CHAT_CONST::CHAT_ALLIANCE:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_ALLIANCE;
			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_ALLIANCE, m_pScrollBar_ALLIANCE, vecLinkTextData, wType, bGM );
		}
		break;
	
	case CHAT_CONST::CHAT_AREA:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_AREA;
//			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_CHANNEL, m_pScrollBar_CHANNEL, vecLinkTextData );
		}
		break;
	
	case CHAT_CONST::CHAT_PARTY_RECRUIT:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT;
//			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_CHANNEL, m_pScrollBar_CHANNEL, vecLinkTextData );
		}
		break;

	case CHAT_CONST::CHAT_TOALL:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_TOALL;			
		}
		break;

	case CHAT_CONST::CHAT_SYSTEM:			//에러 또는 시스템 메시지
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_SYSTEM;
			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_SYSTEM, m_pScrollBar_SYSTEM, vecLinkTextData, wType, bGM );
		}
		break;

	case CHAT_CONST::CHAT_FACTION:
		{
			dwTempColor = CHAT_INFOR_COLOR::CHAT_FACTION;
			ADD_CHAT_PAGE_TEXTBOX_LINK ( strTemp, dwTempColor, m_pTextBox_ALLIANCE, m_pScrollBar_ALLIANCE, vecLinkTextData, wType, bGM );
		}
		break;
	}
	
	//	강제로 추가하는 경우에는
	//	스테이트를 무시한다.
	if ( !m_bIgnoreState && !(m_wDisplayState & wType) ) return ;

	AddStringToNORMAL ( wType, strTemp, dwTempColor, vecLinkTextData, bGM );

	if ( m_bChatLog ) SAVE_CHATLOG( strTemp, wType );
	RECORD_CHAT( strTemp, wType );
}

void CBasicChatRightBody::AddStringToSystemMessage ( CString strTemp, D3DCOLOR dwColor )
{

	strTemp.Format ( "[%s]%s", ID2GAMEWORD("CHAT_TYPE",8), strTemp );

	ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwColor, m_pTextBox_SYSTEM, m_pScrollBar_SYSTEM );

	if ( m_wDisplayState & CHAT_CONST::CHAT_SYSTEM )
	{
		AddStringToNORMAL ( strTemp, dwColor );
		
		if ( m_bChatLog ) SAVE_CHATLOG( strTemp, CHAT_CONST::CHAT_SYSTEM );
		RECORD_CHAT( strTemp, CHAT_CONST::CHAT_SYSTEM );
	}
}

void CBasicChatRightBody::CHANGE_CHAT_TYPE( int nChatType ) 
{
	if ( m_dwChatType == nChatType ) return;

	DWORD dwColor = CHAT_INFOR_COLOR::CHAT_NORMAL;
	CString strText;

	switch ( nChatType ) 
	{
	case CHAT_CONST::CHAT_NORMAL:
		dwColor = CHAT_INFOR_COLOR::CHAT_NORMAL;
		strText = ID2GAMEINTEXT("CHAT_NORMAL_MODE_EX");
		break;
	case CHAT_CONST::CHAT_PRIVATE:
		dwColor = CHAT_INFOR_COLOR::CHAT_PRIVATE;
		strText = ID2GAMEINTEXT("CHAT_PRIVATE_MODE_EX");
		break;
	case CHAT_CONST::CHAT_PARTY:			
		dwColor = CHAT_INFOR_COLOR::CHAT_PARTY;
		strText = ID2GAMEINTEXT("CHAT_PARTY_MODE_EX");
		break;
	case CHAT_CONST::CHAT_GUILD:			
		dwColor = CHAT_INFOR_COLOR::CHAT_GUILD;
		strText = ID2GAMEINTEXT("CHAT_GUILD_MODE_EX");
		break;
	case CHAT_CONST::CHAT_ALLIANCE:			
		dwColor = CHAT_INFOR_COLOR::CHAT_ALLIANCE;
		strText = ID2GAMEINTEXT("CHAT_ALLIANCE_MODE_EX");
		break;
	case CHAT_CONST::CHAT_AREA:				
		dwColor = CHAT_INFOR_COLOR::CHAT_AREA;
		strText = ID2GAMEINTEXT("CHAT_AREA_MODE_EX");
		break;
	case CHAT_CONST::CHAT_PARTY_RECRUIT:	
		dwColor = CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT;
		strText = ID2GAMEINTEXT("CHAT_PARTY_RECRUIT_MODE_EX");
		break;
	case CHAT_CONST::CHAT_TOALL:			
		dwColor = CHAT_INFOR_COLOR::CHAT_TOALL;
		strText = ID2GAMEINTEXT("CHAT_TOALL_MODE_EX");
		break;
	case CHAT_CONST::CHAT_FACTION:
		dwColor = CHAT_INFOR_COLOR::CHAT_FACTION;
		strText = ID2GAMEINTEXT("CHAT_FACTION_MODE_EX");
		break;
	}

	m_pEditBox->SetTextColor ( dwColor );
	AddStringToNORMAL ( strText, dwColor );	

	SetCHATTYPE( nChatType );
	EDIT_BEGIN();
}

void CBasicChatRightBody::BEGIN_NORMAL_CHAT ( const CString& strMessage )
{	
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_NORMAL );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_NORMAL_MODE_EX"), CHAT_INFOR_COLOR::CHAT_NORMAL );	

	SetCHATTYPE ( CHAT_CONST::CHAT_NORMAL ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );	
}

void	CBasicChatRightBody::BEGIN_PRIVATE_CHAT ( const CString& strName, const CString strMessage )
{
	if ( strName.GetLength() ) m_strPrivateName = strName;

	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_PRIVATE );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_PRIVATE_MODE_EX"), CHAT_INFOR_COLOR::CHAT_PRIVATE );

	SetCHATTYPE ( CHAT_CONST::CHAT_PRIVATE ); 	
	
	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void	CBasicChatRightBody::BEGIN_PARTY_CHAT ( const CString& strMessage )
{
	DWORD dwColor;
	dwColor = CHAT_INFOR_COLOR::CHAT_PARTY;

	m_pEditBox->SetTextColor ( dwColor );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_PARTY_MODE_EX"), dwColor );

	SetCHATTYPE ( CHAT_CONST::CHAT_PARTY ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );

	
}

void	CBasicChatRightBody::BEGIN_GUILD_CHAT ( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_GUILD );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_GUILD_MODE_EX"), CHAT_INFOR_COLOR::CHAT_GUILD );

	SetCHATTYPE ( CHAT_CONST::CHAT_GUILD ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void	CBasicChatRightBody::BEGIN_ALLIANCE_CHAT( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_ALLIANCE );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_ALLIANCE_MODE_EX"), CHAT_INFOR_COLOR::CHAT_ALLIANCE );

	SetCHATTYPE ( CHAT_CONST::CHAT_ALLIANCE ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void	CBasicChatRightBody::BEGIN_AREA_CHAT( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_AREA );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_AREA_MODE_EX"), CHAT_INFOR_COLOR::CHAT_AREA );

	SetCHATTYPE ( CHAT_CONST::CHAT_AREA ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void CBasicChatRightBody::BEGIN_PARTY_RECRUIT_CHAT( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_PARTY_RECRUIT_MODE_EX"), CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT );

	SetCHATTYPE ( CHAT_CONST::CHAT_PARTY_RECRUIT ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void	CBasicChatRightBody::BEGIN_TOALL_CHAT ( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_TOALL );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_TOALL_MODE_EX"), CHAT_INFOR_COLOR::CHAT_TOALL );

	SetCHATTYPE ( CHAT_CONST::CHAT_TOALL ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void CBasicChatRightBody::BEGIN_SYSTEM_CHAT ( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_SYSTEM );
	SetCHATTYPE ( CHAT_CONST::CHAT_SYSTEM ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void CBasicChatRightBody::BEGIN_FACTION_CHAT ( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( CHAT_INFOR_COLOR::CHAT_FACTION );
	SetCHATTYPE ( CHAT_CONST::CHAT_FACTION ); 

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void	CBasicChatRightBody::ADD_FRIEND_LIST ()
{
	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();
	if (FriendMap.size () == m_nFRIENDSIZE_BACK)
        return;

	friendlogic::FRIENDMAP_CITER iter = FriendMap.begin();
	friendlogic::FRIENDMAP_CITER iter_end = FriendMap.end();
	for ( ; iter != iter_end; ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if (sFriend->m_Online)
            ADD_FRIEND(sFriend->m_ChaName.c_str()); // 온라인이면 리스트에 추가한다.
	}

	m_nFRIENDSIZE_BACK = (int)FriendMap.size ();
}

void	CBasicChatRightBody::ADD_FRIEND ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	//	삽입
	NAMELIST_ITER iter = m_NameList.begin ();
	NAMELIST_ITER iter_end = m_NameList.end ();
	for ( ; iter != iter_end; ++iter )	//	혹시 중간에 있다면...
	{
		if( (*iter) == strName )
		{
			m_NameList.erase( iter );
			m_NamePos = 0; // 삭제된 이름이 있다면 다시 포지션을 0으로 설정
			break;
		}
	}

	m_NameList.push_front( strName );

	if ( m_NameList.size () > 30 )
	{
		m_NameList.pop_back();
	}
}

void CBasicChatRightBody::DEL_FRIEND( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	if( !m_NameList.empty() ) m_NameList.remove( strName );
}

void CBasicChatRightBody::EDIT_BEGIN ( const CString& strMessage )
{
	if ( strMessage.GetLength () ) m_pEditBox->SetEditString ( strMessage );

	m_pEditBox->BeginEdit ();
}

bool CBasicChatRightBody::SEND_CHAT_MESSAGE ()
{
	bool no_message = false;   
	
	CString strInput = m_pEditBox->GetEditString ();
    std::string strBack(strInput.GetString());
    sc::string::trimLeft(strBack, CHAT_CONST::CHAT_SIMBOL_BLANK); // 좌측 공백 무시

	if (strInput.GetLength())
	{
		bool bincmd(false);
		bincmd = m_pGaeaClient->ExecuteCommand(strBack);
		if (!bincmd)
		{						
			if (!SEND_CHAT_MESSAGE(strInput, m_pEditBox->m_vecLinkTextData))
			{
				no_message = true;
			}
		}
	}
	else
	{
		no_message = true;
	}

	EDIT_CLEAR();

	if (no_message)
        EDIT_END();
	return true;
}

void CBasicChatRightBody::EDIT_END ()
{
	m_pEditBox->EndEdit ();
}

void CBasicChatRightBody::ShowSelfMessageOnHead ( const CString& strOrigin )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	IHeadChatDisplayMan* pHeadChatDisplayMan = m_pInterface->GetHeadChatDisplayMan();

	if( pHeadChatDisplayMan && pCharacter )
	{
		D3DXVECTOR3 *pPos = m_pGaeaClient->FindCharHeadPos( pCharacter->m_szName );
		if ( pPos )
		{
			D3DXVECTOR3 vPos = *pPos;
			D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

			int nPosX = (int)( vScreen.x);
			int nPosY = (int)( vScreen.y);

			D3DXVECTOR2 vCharPos = D3DXVECTOR2 ( float(nPosX), float(nPosY ) );

			const bool bParty = m_pGaeaClient->GetMyPartyClient()->isValid();
			DWORD dwIDColor = bParty ? NS_UITEXTCOLOR::GREENYELLOW : NS_UITEXTCOLOR::IDCOLOR;
            pHeadChatDisplayMan->AddChatMsg( pCharacter->m_szName, dwIDColor, UI::ToString( strOrigin ), NS_UITEXTCOLOR::DEFAULT, vCharPos );
		}
	}
}

void CBasicChatRightBody::ChatLog( bool bChatLog, int nChatLogType )
{
	if ( bChatLog ) 
	{
		m_bChatLog = true;
		m_nChatLogType |= nChatLogType;
		
		// CreteDirectory...
		TCHAR szChatLogDir[MAX_PATH]={0};
		SHGetSpecialFolderPath( NULL, szChatLogDir, CSIDL_PERSONAL, FALSE );
        
        const SUBPATH* pPath = m_pGaeaClient->GetSubPath();

		std::string strChatLogDir(szChatLogDir);
		strChatLogDir += pPath->ChatLog(); //SUBPATH::CHATLOG;

		CreateDirectory( strChatLogDir.c_str() , NULL );

	}
	else 
	{
		m_nChatLogType &= ~(nChatLogType);
		if ( !m_nChatLogType ) m_bChatLog = false;
	}
}

void CBasicChatRightBody::SAVE_CHATLOG( CString strTemp, WORD wType )
{

	int nType = wType;
	
	if (!(m_nChatLogType & nType))
        return;

	// 현재시각
	CTime cTime(m_pGaeaClient->GetCurrentTime());

	//	서버 타임으로 변환( TimeZone 계산 )
	m_pGaeaClient->GetConvertServerTime( cTime );

	const int nServerYear = (int) cTime.GetYear ();
	const int nServerMonth = (int) cTime.GetMonth ();
	const int nServerDay = (int) cTime.GetDay ();
	const int nServerHour = (int) cTime.GetHour ();
	const int nServerMinute = (int) cTime.GetMinute();

	CString strChatLog;
	strChatLog.Format("%02d::%02d\t ", nServerHour, nServerMinute );

	strChatLog += "\t";

	strChatLog += strTemp;
	strChatLog += '\n';


	// 로그파일 로드 및 저장

	TCHAR szChatLogDir[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szChatLogDir, CSIDL_PERSONAL, FALSE );

    const SUBPATH* pPath = m_pGaeaClient->GetSubPath();

	CString strChatLogDir(szChatLogDir);
	strChatLogDir += pPath->ChatLog().c_str(); //SUBPATH::CHATLOG;
	
	CString strChatLogFile;
	
	strChatLogFile.Format( "%s%d%02d%02d.txt", strChatLogDir.GetString(), nServerYear, nServerMonth, nServerDay );

	FILE* file=NULL;
	file = _fsopen ( strChatLogFile.GetString(), "a", _SH_DENYNO );
	if ( !file )
        return;

	fprintf( file, strChatLog.GetString() );

	fclose( file );

}

void CBasicChatRightBody::RECORD_CHAT( const CString strChatMsg, WORD wType  )
{
	SRecordChatMsg recordChatMsg;
	CTime curTime(m_pGaeaClient->GetCurrentTime());
	
	//	서버 타임으로 변환( TimeZone 계산 )
	m_pGaeaClient->GetConvertServerTime( curTime );

	// 2분의 시간이 지난 메시지 삭제를 위해 지난 메시지를 체크한다.
	{		
		RECORD_CHAT_LOG_ITER iter = m_vecRecordChatMsg.begin();
		int i = 0;
		// 최대 10개까지만 검사
        // 추가될때마다 10개씩 지우므로 무작정 쌓이지는 않는다.
        // 2분동안 대량의 메세지가 등록된 이후에 동작하지 않는 경우를 제외라면
        // 10개씩 지워도 문제는 없을것 같다.
		for( ; i < 10 &&  iter != m_vecRecordChatMsg.end(); i++ )
		{
			recordChatMsg = *iter;
			CTimeSpan timeSpan = curTime - recordChatMsg.recordTime;
			if( timeSpan.GetMinutes() >= 2 ) 
                iter = m_vecRecordChatMsg.erase(iter);
            else 
                ++iter;
		}
	}

	// 메시지 추가
	{
		recordChatMsg.recordTime = curTime;
		recordChatMsg.strChatMsg = strChatMsg;
		m_vecRecordChatMsg.push_back( recordChatMsg );
	}
}

CString CBasicChatRightBody::GET_RECORD_CHAT()
{
	SRecordChatMsg recordChatMsg;
	RECORD_CHAT_LOG_ITER iter = m_vecRecordChatMsg.begin();
	CString strLine, strReturn;
	for( ; iter != m_vecRecordChatMsg.end(); ++iter )
	{
		recordChatMsg = *iter;
		strLine = "";
		strLine.Format( "[%04d:%02d:%02d %02d:%02d:%02d] %s \n", 
						recordChatMsg.recordTime.GetYear(), 
						recordChatMsg.recordTime.GetMonth(),
						recordChatMsg.recordTime.GetDay(),
						recordChatMsg.recordTime.GetHour(),
						recordChatMsg.recordTime.GetMinute(),
						recordChatMsg.recordTime.GetSecond(),
						recordChatMsg.strChatMsg.GetString() );

		strReturn += strLine;
											   
	}

	return strReturn;
}

bool CBasicChatRightBody::ADD_NAME_TO_POSTBOX ( const CString& strData )
{
	if ( m_pInterface->UiIsVisibleGroup( POSTBOX_WINDOW ) )
	{
		IPostBoxWindow* pPostBoxWindow = m_pInterface->GetPostBoxWindow();
		if ( !pPostBoxWindow )
		{
			return false;
		}

        bool bSendPageEnable = pPostBoxWindow->IsSendPageEnalbe();

		if ( !bSendPageEnable )
		{
			return false;
		}

		if ( !pPostBoxWindow->IsSendPageOpen() )
		{
			return false;
		}

		//	귓속말을 제외한 상황일때 체크
		int nBegin = strData.Find( _T('('));
		if ( nBegin >= 0 ) 
		{
			int nEnd = strData.Find( _T("):") );
			if ( nEnd > 0 ) 
			{
				int nCnt =  nEnd - nBegin - 1; 

				m_pInterface->UiShowGroupFocus( POSTBOX_WINDOW );
				pPostBoxWindow->Set_TO_Editbox( UI::ToString( strData.Mid( nBegin+1, nCnt ) ) );

				return true;
			}
		}

		//	귓속말일때 체크
		nBegin = strData.Find( _T('['));
		if ( nBegin >= 0 ) 
		{
			std::string strFind = sc::string::format( ID2GAMEWORD("CHAT_TYPE",9), "" ) + "]";
			int nEnd = strData.Find( strFind.c_str() );
			if ( nEnd > 0 ) 
			{
				int nCnt =  nEnd - nBegin - 1; 

				m_pInterface->UiShowGroupFocus( POSTBOX_WINDOW );
                pPostBoxWindow->Set_TO_Editbox( UI::ToString( strData.Mid( nBegin+1, nCnt ) ) );

				return true;
			}
		}
	}

	return false;
}

bool CBasicChatRightBody::AUTO_PRIVATE_CHAT( const CString& strData )
{
	//	귓속말을 제외한 상황일때 체크
	int nBegin = strData.Find( _T('('));
	if ( nBegin >= 0 ) 
	{
		int nEnd = strData.Find( _T("):") );
		if ( nEnd > 0 ) 
		{
			int nCnt =  nEnd - nBegin - 1; 
			BEGIN_PRIVATE_CHAT( strData.Mid( nBegin+1, nCnt ) );
			return true;
		}
	}

	//	귓속말일때 체크
	nBegin = strData.Find( _T('['));
	if ( nBegin >= 0 ) 
	{
		std::string strFind = sc::string::format( ID2GAMEWORD("CHAT_TYPE",9), "" ) + "]";
		int nEnd = strData.Find( strFind.c_str() );
		if ( nEnd > 0 ) 
		{
			int nCnt =  nEnd - nBegin - 1; 
			BEGIN_PRIVATE_CHAT( strData.Mid( nBegin+1, nCnt ) );
			return true;
		}
	}

	return false;
}

bool CBasicChatRightBody::ReferChar( const CString& strData )
{
	if( strData.IsEmpty() )
		return false;

	ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
	if( !pReferChar )
		return false;

	//	귓속말을 제외한 상황일때 체크
	int nBegin = strData.Find( _T('('));
	if ( nBegin >= 0 ) 
	{
		int nEnd = strData.Find( _T("):") );
		if ( nEnd > 0 ) 
		{
			int nCnt =  nEnd - nBegin - 1; 
			CString strName = strData.Mid( nBegin+1, nCnt );

            if( strcmp(ID2GAMEWORD("NOTIFY_NAME"), strName) == 0 )
                return false;

			if( pReferChar->Request( m_pGaeaClient->GetConnServerGroup(), strName.GetString() ) )
				//m_pInterface->UiShowGroupFocus( REFERCHAR_WINDOW );

			return true;
		}
	}

	//	귓속말일때 체크
	nBegin = strData.Find( _T('['));
	if ( nBegin >= 0 ) 
	{
		std::string strFind = sc::string::format( ID2GAMEWORD("CHAT_TYPE",9), "" ) + "]";
		int nEnd = strData.Find( strFind.c_str() );
		if ( nEnd > 0 ) 
		{
			int nCnt =  nEnd - nBegin - 1; 
			CString strName = strData.Mid( nBegin+1, nCnt );

			if( pReferChar->Request( m_pGaeaClient->GetConnServerGroup(), strName.GetString() ) )
				//m_pInterface->UiShowGroupFocus( REFERCHAR_WINDOW );

			return true;
		}
	}

	return false;
}

BOOL CBasicChatRightBody::IsThaiCheck( const CString& strOrigin )
{
	// 태국어 문자 조합 체크
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
	    if ( !STRUTIL::CheckThaiString(strOrigin) )
	    {
		    AddStringToSystemMessage( ID2GAMEINTEXT("CHAT_THAI_CHECK_ERROR") , NS_UITEXTCOLOR::NEGATIVE );
		    return FALSE;
        }
	}

    return TRUE;
}

BOOL CBasicChatRightBody::IsVnCheck( const CString& strOrigin )
{
	// 베트남 문자 조합 체크 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if ( STRUTIL::CheckVietnamString( strOrigin ) )
	    {
		    AddStringToSystemMessage( ID2GAMEINTEXT("CHAT_VN_CHECK_ERROR") , NS_UITEXTCOLOR::NEGATIVE );
		    return FALSE;
	    }
    }

	return TRUE;
}
