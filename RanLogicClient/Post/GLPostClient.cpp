#include "pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"

#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "./GLPostClient.h"
#include "./GLPostClientDefine.h"

#include "../../SigmaCore/DebugInclude.h"


void SAPOSTHISTORY::SET(GLGaeaClient* pGaeaClient, GLMSG::SNET_POST_SEND& nmg)
{
    GLCharacter* pCharacter = pGaeaClient->GetCharacter();

    this->llPostID = ID_NULL;

    this->byPostState = nmg.byPostState;
    this->byPostClass = nmg.byPostClass;
    this->byPostType  = nmg.byPostType;

    this->iSender  = nmg.iSender;
    this->iRecver  = nmg.iRecver;

    this->iDesign  = nmg.iDesign;
    this->llCharge = nmg.llCharge;

    this->SenderName    [0] = NULL;
    this->SendDate		    = 0;
    this->RecvDate		    = 0;
    this->ReturnDate	    = 0;
    this->RecvBackDate	    = 0;
    this->DeleteDate	    = 0;

    if ( pCharacter )
    {
        strcpy_s ( this->SenderName, POST_NAME_LENGTH, pCharacter->GetName() );
    }

    strcpy_s ( this->RecverName, POST_NAME_LENGTH, nmg.RecverName );

    // Note : 시간을 현재 시간으로 임의로 세팅해준다.
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        CTime cTime(st);
        this->SendDate = cTime.GetTime();
    }
}

//---------------------------------------------------------------------------------//

const DWORD SAPOSTCLIENT::GetExpireDay(GLGaeaClient* pGaeaClient)
{
    CTime cSendTime;

	if ( m_ReturnDate != 0 )
	{
		cSendTime = CTime( m_ReturnDate );
	}
	else
	{
		if ( m_SendDate != 0 )
		{
            cSendTime = CTime( m_SendDate );
		}
		else
		{
			return 0;
		}
	}

	pGaeaClient->GetConvertServerTime( cSendTime );

	CTime cCurrentTime;
	cCurrentTime = CTime::GetCurrentTime();
	pGaeaClient->GetConvertServerTime( cCurrentTime );

	CTimeSpan cDiffTime = cCurrentTime - cSendTime;
	LONGLONG llSec = cDiffTime.GetTotalSeconds();

	DWORD dwDay = 0;

	if ( llSec <= GLPost::llPOST_EXPIRE_DURATION_SEC )
	{
		dwDay = (DWORD)(GLPost::llPOST_EXPIRE_DURATION_SEC - llSec)/60/60/24;
	}

	dwDay += 1;

	if ( GLCONST_POST::dwPOST_EXPIRE_DURATION_DAY < dwDay )
	{
		dwDay = GLCONST_POST::dwPOST_EXPIRE_DURATION_DAY;
	}

	return dwDay;
}

//---------------------------------------------------------------------------------//
GLPostClient::GLPostClient(GLGaeaClient* pGaeaClient)
	: m_pGaeaClient(pGaeaClient)
	, m_bReqPost				 ( FALSE )
 	, m_bReqSend				 ( FALSE )
	, m_dwLastSendTime			 ( 0 )
	, m_dwLastRecvTime			 ( 0 )
    , m_HistoryInfoSet           ( SPOSTHISTORYINFOSET::SPOSTHISTORYALLINFO )
    , m_SendHistoryInfoSet       ( SPOSTHISTORYINFOSET::SPOSTHISTORYSENDINFO )
    , m_RecvHistoryInfoSet       ( SPOSTHISTORYINFOSET::SPOSTHISTORYRECVINFO )
{
}

GLPostClient::~GLPostClient(void)
{
}

HRESULT GLPostClient::FrameMove ( float fTime, float fElapsedTime )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	// Note : 우체통을 사용할 수 있는지 체크해서 꺼준다.
	{
		if (pChar &&
            pChar->m_emPOSTBOXIDTYPE != EMPOSTBOXID_TYPE_NONE &&
            pChar->m_sPOSTBOXID != NATIVEID_NULL())
		{
			if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
				NSWIDGET_SCRIPT::CF_PostBox_IsOpen ) )
            {
				bool bClose = false;
				if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
					NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen ) )
				{
					bClose = true;
				}

				if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
					NSWIDGET_SCRIPT::g_strFunc_Trade_IsOpen ) )
				{
					bClose = true;
				}

				if( false == pChar->IsPOSTBOXUSEABLE( pChar->m_emPOSTBOXIDTYPE, pChar->m_sPOSTBOXID ) )
				{
					bClose = true;
				}

				if (bClose)
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_PostBox_Close );
					return S_FALSE;
				}
			}
		}
	}

	return S_OK;
}

void GLPostClient::ClearReceivePost ()
{
	if ( !m_ReceiveMap.empty() )
	{
		m_ReceiveMap.clear();
	}

	if ( !m_ReceiveVec.empty() )
	{
		m_ReceiveVec.clear();
	}
}


const DWORD	GLPostClient::GetMaxPost ()
{
	return (DWORD)m_ReceiveVec.size();
}


// Note : 받은 우편 갯수중에 읽지 않은 우편을 센다
const DWORD	GLPostClient::GetReceivedNewPost ()
{
    DWORD dwCount = 0;

    SPOSTRCVVECITER FindVecIter = m_ReceiveVec.begin();
    for ( ; FindVecIter!=m_ReceiveVec.end(); ++FindVecIter )
    {
        SAPOSTCLIENT* pPost = (*FindVecIter);

        if ( !pPost )
        {
            continue;
        }

        if ( pPost->IsUnRead() )
        {
            ++dwCount;
        }
    }

    return dwCount;
}


// Note : 전송 가능한 메일인가
const BOOL GLPostClient::IsSendAblePost	( GLMSG::SNET_POST_SEND* nmg )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	// Note : 레벨이 부족하면
	if ( (DWORD)pCharacter->GETLEVEL() < GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL )
	{
		return FALSE;
	}

	// Note : 쿨타임 검사
	if ( IsSendCoolTime() )
	{
		return FALSE;
	}

	SINVENITEM* pInvenItem = NULL;
	if( nmg->xAttach01 != USHRT_MAX || nmg->yAttach01 != USHRT_MAX )
	{
		pInvenItem = pCharacter->m_cInventory.GetItem( nmg->xAttach01, nmg->yAttach01 );
		if( !pInvenItem )
			return FALSE;

		if( pInvenItem->GetNativeID() != nmg->idAttach01 )
			return FALSE;

		if( pInvenItem->TurnNum() < nmg->nAttach01 )
			return FALSE;
	}

	// Note : 금액 청구시 아이템이 있어야한다.
	if ( nmg->llCharge && !pInvenItem )
	{
		return FALSE;
	}

	// Note : 금액첨부와 청구는 동시에 할 수 없다.
	if ( nmg->llAttachMoney && nmg->llCharge )
	{
		return FALSE;
	}

	// Note : 수수료 검사
	{
		BOOL		bAttachDesign = (nmg->iDesign) ? TRUE : FALSE;
		BOOL		bAttachItem   = (pInvenItem) ? TRUE : FALSE;
		LONGLONG	llCommision   = GLPostClient::GetCommision ( bAttachDesign, bAttachItem, nmg->llAttachMoney );

		GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

		if ( sCharData.GetInvenMoney() < (nmg->llAttachMoney + llCommision) )
		{
			return FALSE;
		}
	}

	// Note : 보유 금액검사
	if ( nmg->llAttachMoney )
	{
		GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

		if ( sCharData.GetInvenMoney() < nmg->llAttachMoney )
		{
			return FALSE;
		}
	}

	// Note : 금액 최대치 검사
	if ( GLCONST_POST::llPOST_MAX_ATTACH_MONEY < nmg->llAttachMoney )
	{
		return FALSE;
	}

	// Note : 청구 최대치 검사
	if ( GLCONST_POST::llPOST_MAX_ATTACH_MONEY < nmg->llCharge )
	{
		return FALSE;
	}

	return TRUE;
}

// Note : 반송 가능한 메일인가
const BOOL GLPostClient::IsReturnAblePost ( SAPOSTCLIENT* pPost )
{
	if ( !pPost )
	{
		return FALSE;
	}

	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	// Note : 이미 반송 상태면 실패
	if ( pPost->IsReturnPost() )
	{
		return FALSE;
	}

	// Note : 일반 우편이 아니라면 실패
	if ( pPost->m_byPostClass != EMPOST_USER )
	{
		return FALSE;
	}

	// Note : 텍스트 타입이라면 실패
	if ( pPost->m_byPostType == EMPOST_TEXT )
	{
		return FALSE;
	}

	// Note : 우편물의 첨부물을 수취한 상태면 실패
	if (   pPost->m_byAttach01Accept
		|| pPost->m_byChargePayment 
		|| pPost->m_byMoneyAccept	)
	{
		return FALSE;
	}

	return TRUE;
}

// Note : 삭제 가능한 메일인가
const BOOL GLPostClient::IsDeleteAblePost ( SAPOSTCLIENT* pPost, const BOOL bAttachDelete )
{
	if ( !pPost )
	{
		return FALSE;
	}

	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	// Note : 이미 삭제된 우편이라면
	if ( pPost->IsDeletePost() )
	{
		return FALSE;
	}

	if ( !bAttachDelete )
	{
		// Note : 아이템이 첨부 되어있다면 실패
		if ( pPost->m_byAttach01Accept == FALSE && pPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
		{
			return FALSE;
		}	

		// Note : 금액이 첨부 되어있다면 실패
		if ( pPost->m_byMoneyAccept == FALSE && pPost->m_llAttachMoney )
		{
			return FALSE;
		}
	}

	// Note : 대금 청구이고 지불을 하지 않았을때
	if ( pPost->m_byChargePayment == FALSE && pPost->m_llCharge )
	{
		return FALSE;
	}

	return TRUE;
}

bool GLPostClient::IsAttachPost( SAPOSTCLIENT* pPost )
{
    if ( !pPost )
        return false;

    // Note : 아이템이 첨부 되어있는지 확인.
    if ( pPost->m_byAttach01Accept == FALSE && pPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
        return true;

    // Note : 금액이 첨부 되어있는지 확인.
    if ( pPost->m_byMoneyAccept == FALSE && pPost->m_llAttachMoney )
        return true;

    return false;
}

const BOOL GLPostClient::IsSendCoolTime ()
{
	DWORD dwCurrentPostSend = ::GetTickCount();	

	if ( dwCurrentPostSend > m_dwLastSendTime + GLCONST_POST::dwPOST_SEND_COOL_TIME_MS )
	{				
		return FALSE;
	}

	return TRUE;
}


const BOOL GLPostClient::IsRecvCoolTime ()
{
	DWORD dwCurrentPostRecv = ::GetTickCount();	

	if ( dwCurrentPostRecv > m_dwLastRecvTime + GLCONST_POST::dwPOST_RECV_COOL_TIME_MS )
	{				
		return FALSE;
	}

	return TRUE;
}

const BOOL GLPostClient::ReqReceivePost ()
{
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	// Note : 요청중이면 취소
	if ( m_bReqPost	)
	{
		return FALSE;
	}

	// Note : 쿨타임이면 취소
	if ( IsRecvCoolTime() )
	{
		return FALSE;
	}

	ClearReceivePost ();

	GLMSG::SNET_POST_GET msg;
	msg.bGetOnlyInfo = FALSE;
	msg.iRecver		 = (int)m_pGaeaClient->GetCharacter()->GetCharID();
	m_pGaeaClient->NETSENDTOAGENT(&msg);

	m_bReqPost = TRUE;

	return TRUE;
}

const BOOL GLPostClient::ReqReceivePostInfo ()
{
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	GLMSG::SNET_POST_GET msg;
	msg.bGetOnlyInfo = TRUE;
	msg.iRecver		 = (int)m_pGaeaClient->GetCharacter()->GetCharID();
	m_pGaeaClient->NETSENDTOAGENT(&msg);

	return TRUE;
}

const BOOL GLPostClient::ReqReceiveCurrentHistory ( SPOSTHISTORYINFOSET* currentInfoSet )
{
    // Note : 우편함 사용 불가능이면
    if ( !GLUseFeatures::GetInstance().IsUsingPost() )
    {
        return FALSE;
    }

    // Note : 요청중이면 취소
    if ( currentInfoSet->bReqHistory )
    {
        return FALSE;
    }

    // Note : 쿨타임이면 취소
    if ( currentInfoSet->IsHistCoolTime() )
    {
        return FALSE;
    }

    currentInfoSet->ClearHistory();

    switch( currentInfoSet->spostHistoryType )
    {
        case SPOSTHISTORYINFOSET::SPOSTHISTORYALLINFO:
            {
                GLMSG::SNET_POST_GET_STATE msg;
                msg.iRecver = (int)m_pGaeaClient->GetCharacter()->GetCharID();
                m_pGaeaClient->NETSENDTOAGENT(&msg);
            }
            break;

        case SPOSTHISTORYINFOSET::SPOSTHISTORYSENDINFO:
            {
                GLMSG::SNET_POST_GET_SEND_STATE msg;
                msg.iRecver = (int)m_pGaeaClient->GetCharacter()->GetCharID();
                m_pGaeaClient->NETSENDTOAGENT(&msg);
            }
            break;

        case SPOSTHISTORYINFOSET::SPOSTHISTORYRECVINFO:
            {
                GLMSG::SNET_POST_GET_RECV_STATE msg;
                msg.iRecver = (int)m_pGaeaClient->GetCharacter()->GetCharID();
                m_pGaeaClient->NETSENDTOAGENT(&msg);
            }
            break;
    }

    currentInfoSet->bReqHistory = TRUE;

    return TRUE;
}

const BOOL GLPostClient::ReqReceiveHistory ()
{
    return ReqReceiveCurrentHistory( &m_HistoryInfoSet );
}

const BOOL GLPostClient::ReqReceiveSendHistory ()
{
    return ReqReceiveCurrentHistory( &m_SendHistoryInfoSet );
}

const BOOL GLPostClient::ReqReceiveRecvHistory ()
{
    return ReqReceiveCurrentHistory( &m_RecvHistoryInfoSet );
}

const BOOL GLPostClient::ReqSendPost ( GLMSG::SNET_POST_SEND* nmg )
{
	static DWORD dwREQID = 0;

	if ( !nmg )
	{
		return FALSE;
	}

	// Note : 이미 요청 중이라면 취소
	if ( m_bReqSend )
	{
		return FALSE;
	}

	if ( !IsSendAblePost( nmg ) )
	{
		return FALSE;
	}

	GLMSG::SNET_POST_REQ_CHANUM Msg;
	Msg.SetChaName(nmg->RecverName);
	Msg.nReqID = (int) dwREQID;
	m_pGaeaClient->NETSENDTOAGENT(&Msg);

	SPOSTSENDREQ sPOSTSENDREQ;
	sPOSTSENDREQ.ReqID = dwREQID;
	sPOSTSENDREQ.Data  = (*nmg);
	m_SendQueue.push( sPOSTSENDREQ );

	dwREQID++;

	// Note : 우편 요청 등록
	m_bReqSend = TRUE;

	return TRUE;
}

const BOOL GLPostClient::ReqOpenPost ( SAPOSTCLIENT* pPost )
{
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	if ( !pPost )
	{
		return FALSE;
	}

	// Note : 우편물이 읽지 않은 상태라면
	if ( pPost->IsUnRead() )
	{
		// Note : 반송 타입일때
		if ( pPost->m_byPostState == EMPOST_RETURN )
		{
			// Note : 읽은 날짜를 채워준다
			SYSTEMTIME st;
			GetLocalTime(&st);	
			CTime cTime(st);
            pPost->m_RecvBackDate = cTime.GetTime();
		}

		// Note : 읽은 상태로 교체
		pPost->m_byPostState = EMPOST_READ;

		// Note : 갯수의 정보를 변경한다.
		if ( m_sPostInfo.nNew )
		{
			m_sPostInfo.nNew--;
			m_sPostInfo.nReportNewPost = m_sPostInfo.nNew;
		}

		m_sPostInfo.nRead++;
	}

	GLMSG::SNET_POST_OPEN Netmsg;
	Netmsg.llPostID = pPost->m_llPostID;
	m_pGaeaClient->NETSENDTOAGENT(&Netmsg);

	return TRUE;
}

const BOOL GLPostClient::ReqReturnPost ( SAPOSTCLIENT* pPost )
{
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	if ( !pPost )
	{
		return FALSE;
	}

	if ( !IsReturnAblePost ( pPost ) )
	{
		return FALSE;
	}

	SPOSTRCVMAPITER FindMapIter = m_ReceiveMap.find( pPost->m_llPostID );
	if ( FindMapIter == m_ReceiveMap.end() )
	{
		return FALSE;
	}

	SPOSTRCVVECITER FindVecIter = std::find( m_ReceiveVec.begin(), m_ReceiveVec.end(), pPost );
	if ( FindVecIter == m_ReceiveVec.end() )
	{
		return FALSE;
	}

	// Note : 삭제/반환 대기중으로 만든다.
	FindMapIter->second.dwSTATE |= ( SAPOSTCLIENT::EMSTATE_WAIT_DELETE | SAPOSTCLIENT::EMSTATE_WAIT_RETURN );

	GLMSG::SNET_POST_RETURN Netmsg;
	Netmsg.llPostID = pPost->m_llPostID;
	m_pGaeaClient->NETSENDTOAGENT(&Netmsg);

	return TRUE;
}

const BOOL GLPostClient::ReqDeletePost ( SAPOSTCLIENT* pPost, const BOOL bAttachDelete )
{	
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	if ( !pPost )
	{
		return FALSE;
	}

	if ( !IsDeleteAblePost ( pPost, bAttachDelete ) )
	{
		return FALSE;
	}

	SPOSTRCVMAPITER FindMapIter = m_ReceiveMap.find( pPost->m_llPostID );
	if ( FindMapIter == m_ReceiveMap.end() )
	{
		return FALSE;
	}

	SPOSTRCVVECITER FindVecIter = std::find( m_ReceiveVec.begin(), m_ReceiveVec.end(), pPost );
	if ( FindVecIter == m_ReceiveVec.end() )
	{
		return FALSE;
	}

	// Note : 삭제 대기중으로 만든다.
	FindMapIter->second.dwSTATE |= SAPOSTCLIENT::EMSTATE_WAIT_DELETE;

	GLMSG::SNET_POST_DELETE Netmsg;
	Netmsg.llPostID = pPost->m_llPostID;
	m_pGaeaClient->NETSENDTOAGENT(&Netmsg);

	return TRUE;
}

const BOOL GLPostClient::ReqTakeItem ( SAPOSTCLIENT* pPost )
{
	// Note : 우편함 사용 불가능이면
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	if ( !pPost )
	{
		return FALSE;
	}

	// Note : 일반 우편 타입이라면
	if ( pPost->m_byPostType == EMPOST_TEXT )
	{
		return FALSE;
	}

	GLMSG::SNET_POST_OPEN_TAKE Netmsg;
	Netmsg.llPostID			= pPost->m_llPostID;
	Netmsg.iRecver			= (int)m_pGaeaClient->GetCharacter()->GetCharID();
	Netmsg.byAttach01Accept	= (BYTE)FALSE;
	Netmsg.byMoneyAccept	= (BYTE)FALSE;

	// Note : 대금 청구라면
	if ( pPost->m_llCharge )
	{
		// Note : 아이템이 존재하지 않을때 실패
		if ( pPost->m_sAttach01.GetNativeID() == NATIVEID_NULL() )
		{
			return FALSE;
		}

		// Note : 이미 받은 상태라면 실패
		if ( pPost->m_byAttach01Accept )
		{
			return FALSE;
		}

		// Note : 이미 청구한 상태라면 실패
		if ( pPost->m_byChargePayment )
		{
			return FALSE;
		}

		// Note : 소지금액보다 많다면 실패
		GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

		if ( sCharData.GetInvenMoney() < pPost->m_llCharge )
		{
			return FALSE;
		}

		Netmsg.byAttach01Accept = (BYTE)TRUE;
	}
	// Note : 일반 첨부 물품 받기
	else
	{
		// Note : 아이템이 존재하지 않거나 이미 받은 상태라면 받지 않는다.
		if ( pPost->m_byAttach01Accept == FALSE && pPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
		{
			Netmsg.byAttach01Accept = (BYTE)TRUE;
		}
		else
		{
			Netmsg.byAttach01Accept = (BYTE)FALSE;
		}

		// Note :금액이 존재하지 않거나 이미 받은 상태라면 받지 않는다.
		if ( pPost->m_byMoneyAccept == FALSE && pPost->m_llAttachMoney )
		{
			Netmsg.byMoneyAccept = (BYTE)TRUE;
		}
		else
		{
			Netmsg.byMoneyAccept = (BYTE)FALSE;
		}
	}

	if ( Netmsg.byAttach01Accept == (BYTE)FALSE && Netmsg.byMoneyAccept == (BYTE)FALSE )
	{
		return FALSE;
	}
	
	// Note : 아이템 삽입가능 여부를 검사
	if ( Netmsg.byAttach01Accept )
	{
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		
		if ( !pCharacter )
		{
			return FALSE;
		}

        //수량이 많은 아이템일 경우가 생김
        //인벤에 전체수량을 받을 공간이 있는지 검사해야함.
        const DWORD nEmpty = pCharacter->m_cInventory.GetAmountEmptyPile( pPost->m_sAttach01.GetNativeID(), false );
        if ( pPost->m_sAttach01.wTurnNum > nEmpty ) //빈 공간보다 갯수가 많으면
        {
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
				"-n", 0 );

            return FALSE;
        }
	}

	//CString strText = ID2GAMEINTEXT("POSTBOX_ERROR_MSG_TAKE_NOTENOUGH_INVEN");
	//PrintMsgText(NS_UITEXTCOLOR::DISABLE, strText );

	m_pGaeaClient->NETSENDTOAGENT(&Netmsg);

	return TRUE;
}

const BOOL GLPostClient::MsgReceivePost ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_GET_DATA_FB *msg = (GLMSG::SNET_POST_GET_DATA_FB *)nmg;

	if ( msg->nRowCount == POST_ON_PROCESSING )
	{
		m_bReqPost = FALSE;
		return FALSE;
	}

	if ( msg->nRowCount == POST_COOTIME_REMAINS )
	{
		m_bReqPost = FALSE;
		return FALSE;
	}

	int nPostNum  = msg->nPostNum;
	int nRowCount = min ((DWORD)msg->nRowCount, GLCONST_POST::dwPOST_HOW_MANY_GET); //PostBox size cannot exceed 30.

	if ( nRowCount == 0 || nPostNum == 0 )
	{
		m_bReqPost = FALSE;
	}

	SAPOST&			sPost		= msg->aPOST;
	DWORD			dwRecver	= (DWORD)msg->iRecver;
	SPOSTRCVMAPITER FindIter	= m_ReceiveMap.end();
	SPOSTRCVVECITER FindVecIter = m_ReceiveVec.end();

	if ( nRowCount == 0 )
	{
		goto _RESULT;
	}
	else if ( nPostNum >= nRowCount )
	{
		GASSERT ( !"메일 갯수가 최대치를 초과 했습니다." );
	}

	if ( m_pGaeaClient->GetCharacter()->GetCharID() != dwRecver )
	{
		return FALSE;
	}

	// Note : 같은 우편 아이디를 찾지 못했을때는 삽입한다.
	FindIter = m_ReceiveMap.find( sPost.m_llPostID );
	if ( FindIter == m_ReceiveMap.end() )
	{
		std::pair<SPOSTRCVMAPITER,bool> ret = m_ReceiveMap.insert( SPOSTRCVPAIR(sPost.m_llPostID, sPost) );

		if (ret.second == false)
		{
			FindIter = ret.first;
			goto _ASSIGN;
		}

		m_ReceiveVec.push_back( &(ret.first->second) );

		goto _RESULT;
	}


_ASSIGN :
	// Note : 찾았을 경우 대입 한다.
	FindIter->second = sPost;

_RESULT :
	// Note : 우편을 다 받았을 경우에만
	if ( nRowCount == 0 || nPostNum == 0 )
	{
		m_dwLastRecvTime = ::GetTickCount();

		// UI에 완료 처리 메소드
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_Msg_ReceivePost );

	}

	return TRUE;
}


const BOOL GLPostClient::MsgReceivePostInfo ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_GET_INFO_FB *msg = (GLMSG::SNET_POST_GET_INFO_FB *)nmg;
	DWORD dwRecver = (DWORD)msg->iRecver;

	if ( m_pGaeaClient->GetCharacter()->GetCharID() != dwRecver )
	{
		return FALSE;
	}

	m_sPostInfo.nTotal = msg->cntAllPost;

	if ( GLCONST_POST::dwPOST_HOW_MANY_GET < (DWORD)msg->cntAllPost )
	{
		m_sPostInfo.nWaiting = msg->cntAllPost - GLCONST_POST::dwPOST_HOW_MANY_GET;
	}
	else
	{
		m_sPostInfo.nWaiting = 0;
	}

	
	m_sPostInfo.nNew  = (DWORD) ( msg->cntNewPost );
	m_sPostInfo.nRead = (DWORD) ( msg->cntAllPost ) - m_sPostInfo.nWaiting - m_sPostInfo.nNew;

	

	if( m_sPostInfo.nNew > m_sPostInfo.nReportNewPost )	// 새로운 메시지가 보고한 새 메세지보다 많다면 
	{
		// 새메세지가 왔음을 알린다.
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_AcceptPost, "-dw", m_sPostInfo.nNew );
		m_sPostInfo.nReportNewPost = m_sPostInfo.nNew;
	}

    if( m_sPostInfo.nNew < m_sPostInfo.nReportNewPost )	
	{
		// 작다면 초기화한다. 이는 메세지 확인으로 새 메세지가 줄수있기때문이다.
		m_sPostInfo.nReportNewPost = 0;
	}
	
	return TRUE;
}

const BOOL GLPostClient::MsgReceiveCurrentHistory ( NET_MSG_GENERIC* nmg, SPOSTHISTORYINFOSET* currentInfoSet )
{
	GLMSG::SNET_POST_GET_STATE_FB *msg = (GLMSG::SNET_POST_GET_STATE_FB *)nmg;

	if ( msg->nRowCount == POST_ON_PROCESSING )
	{
		currentInfoSet->bReqHistory = FALSE;
		return FALSE;
	}

	if ( msg->nRowCount == POST_COOTIME_REMAINS )
	{
		currentInfoSet->bReqHistory = FALSE;
		return FALSE;
	}

	int nPostNum  = msg->nPostNum;
	int nRowCount = min ((DWORD)msg->nRowCount, currentInfoSet->dwStateHowManyGet); //PostBox size cannot exceed 30.

	if ( nRowCount == 0 || nPostNum == 0 )
	{
		currentInfoSet->dwLastHistTime = ::GetTickCount();

		currentInfoSet->bReqHistory = FALSE;
	}

	if ( nRowCount == 0 )
	{
		return TRUE;
	}
	else if ( nPostNum >= nRowCount )
	{
		GASSERT ( !"메일 갯수가 최대치를 초과 했습니다." );
	}

	DWORD dwRecver = (DWORD)msg->iRecver;
	if ( m_pGaeaClient->GetCharacter()->GetCharID() != dwRecver )
	{
		return FALSE;
	}

	SAPOSTSTATE& sHistory = msg->aPOSTSTATE;

	// Note : 같은 우편 아이디를 찾지 못했을때는 삽입한다.
	SPOSTHISTORYMAPITER FindIter = currentInfoSet->HistoryMap.find( sHistory.llPostID );
	if ( FindIter == currentInfoSet->HistoryMap.end() )
	{
		std::pair<SPOSTHISTORYMAPITER,bool> ret = currentInfoSet->HistoryMap.insert( SPOSTHISTORYPAIR(sHistory.llPostID, sHistory) );

		if (ret.second == false)
		{
			FindIter = ret.first;
		}
        else
        {
		    currentInfoSet->HistoryVec.push_back( &(ret.first->second) );

		    return TRUE;
        }
	}

	// Note : 찾았을 경우 대입 한다.
	FindIter->second = sHistory;

	return TRUE;
}

const BOOL GLPostClient::MsgReceiveHistory( NET_MSG_GENERIC* nmg )
{
	if( MsgReceiveCurrentHistory( nmg, &m_HistoryInfoSet ) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_HistoryPageUpdate );
		return true;
	}

	return false;
}

const BOOL GLPostClient::MsgReceiveSendHistory ( NET_MSG_GENERIC* nmg )
{
	if( MsgReceiveCurrentHistory( nmg, &m_SendHistoryInfoSet ) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_HistoryPageSendUpdate );
		return true;
	}
	
	return false;
}

const BOOL GLPostClient::MsgReceiveRecvHistory ( NET_MSG_GENERIC* nmg )
{
	if( MsgReceiveCurrentHistory( nmg, &m_RecvHistoryInfoSet ) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_HistoryPageRecieveUpdate );

		return true;
	}

	return false;
}

const BOOL GLPostClient::MsgSendPostFB_CharID ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_REQ_CHANUM_FB *msg = (GLMSG::SNET_POST_REQ_CHANUM_FB *)nmg;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return FALSE;

	while ( !m_SendQueue.empty() )
	{
		SPOSTSENDREQ&		   sPOSTSENDREQ = m_SendQueue.front();
		GLMSG::SNET_POST_SEND& sNetMsg		= sPOSTSENDREQ.Data;
		sNetMsg.iRecver						= msg->nChaNum;
		sNetMsg.nUserNumRecver				= msg->nUserNum;

		// Note : 실패했을시에 대한 처리를 해준다.
		//		  첨부 되어있는 금액이나 아이템을 되돌려줌. (bjju.post)
		if ( sPOSTSENDREQ.ReqID != (DWORD)msg->nReqID )
		{
			m_SendQueue.pop();
			continue;
		}

		// Note : 해당 유저를 찾지 못했을 경우
		if ( msg->nChaNum == -1 )
		{
            m_SendQueue.pop();

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
				"-n", 1 );

			break;
		}

		if ( sNetMsg.iSender == sNetMsg.iRecver )
		{
			m_SendQueue.pop();

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
				"-n", 4 );

			break;
		}

		if ( !IsSendAblePost( &sNetMsg ) )
		{
            m_SendQueue.pop();

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
				"-n", 2 );

			break;
		}
		
		m_pGaeaClient->NETSENDTOFIELD(&sNetMsg);
		
		// Note : 추가하기전 가상현황 추가
		InsertVirtualHistory ( &sNetMsg );

		m_SendQueue.pop();

		return TRUE;
	}

	// Note : 우편 등록해지
	m_bReqSend = FALSE;

	// Note : 여기 까지 오면 실패한 것이다.
	return FALSE;
}

const BOOL GLPostClient::MsgSendPostFB_Result ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_SEND_FB *msg = (GLMSG::SNET_POST_SEND_FB *)nmg;
	DWORD dwSender = (DWORD)msg->iSender;

	if ( m_pGaeaClient->GetCharacter()->GetCharID() != dwSender )
	{		
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
			"-n", 2 );

		// Note : 삽입했던 가상 히스토리를 삭제
		PopVirtualHistory ();

		return FALSE;
	}

	// Note : 요청중 초기화
	m_bReqSend = FALSE;

	if ( msg->byResut == EMPOST_OK )
	{
		// Note : 쿨타임 등록
		m_dwLastSendTime = ::GetTickCount();
	}
	// Note : 에러
	else
	{
		switch ( msg->byResut )
		{
		// Note : 일반오류
		default:
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
					"-n", 2 );
			}
			break;

		// Note : 쿨타임 에러
		case EMPOST_COOLTIME_REMAIN:
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
					"-n", 3 );
			}
			break;

		// Note : 같은 계정 불가능
		case EMPOST_SAME_ACCOUNT:
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
					"-n", 4 );
			}
			break;
		}

		// Note : 삽입했던 가상 히스토리를 삭제
		PopVirtualHistory ();

		return FALSE;
	}

	// Note : 전송성공 알림	
	m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("POSTBOX_MSG_SEND"));

	// Send 페이지를 리셋한다.
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
		"-n", 5 );

	return TRUE;
}

const BOOL GLPostClient::MsgReturnPost ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_RETURN_FB *msg = (GLMSG::SNET_POST_RETURN_FB *)nmg;

	LONGLONG llPostID = msg->llPostID;
	INT		 Result   = msg->Result;

	if ( Result != EMPOST_OK )
	{
		switch ( Result )
		{
		case EMPOST_RECVER_NOTEXIST:
			break;
		case EMPOST_POST_NOT_EXIST:
			break;
		case EMPOST_SYSTEMPOST_RETURN:
			break;
		case EMPOST_EMPTY_RETURN:
			break;
		case EMPOST_INVALID_RETURN:
			break;
		default:
			break;
		};

		return FALSE;
	}

	// Note : 해당 원소를 찾는다.
	SPOSTRCVMAPITER FindMapIter = m_ReceiveMap.find( llPostID );
	if ( FindMapIter == m_ReceiveMap.end() )
	{
		return FALSE;
	}

	SPOSTRCVVECITER FindVecIter = std::find( m_ReceiveVec.begin(), m_ReceiveVec.end(), &(FindMapIter->second) );
	if ( FindVecIter == m_ReceiveVec.end() )
	{
		return FALSE;
	}

	// Note : 읽지 않은 우편이라면 새 우편의 개수를 하나줄인다.
	SAPOSTCLIENT* pPost = (*FindVecIter);
	if ( pPost && pPost->IsUnRead() )
	{
		if ( m_sPostInfo.nNew )
		{
			m_sPostInfo.nNew--;
			m_sPostInfo.nReportNewPost = m_sPostInfo.nNew;
			// 우편을 읽는다면 보고한 메세지는 새로운 메세지와 같은 값을 만들어 새로운 메세지가 늘면 보고하도록한다.
		}
	}

	// Note : 삭제하기전 반환 업데이트
	UpdateHistory ( (*FindVecIter) );

	// Note : 해당 원소를 삭제
	m_ReceiveMap.erase ( FindMapIter );
	m_ReceiveVec.erase ( FindVecIter );

	// Note : 전체 우편 갯수를 줄인다.
	if ( m_sPostInfo.nTotal )
	{
		--m_sPostInfo.nTotal;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_ReceivePageUpdate );	

	return TRUE;
}

const BOOL GLPostClient::MsgDeletePost ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_DELETE_FB *msg = (GLMSG::SNET_POST_DELETE_FB *)nmg;

	LONGLONG llPostID = msg->llPostID;

	// Note : 해당 원소를 찾는다.
	SPOSTRCVMAPITER FindMapIter = m_ReceiveMap.find( llPostID );
	if ( FindMapIter == m_ReceiveMap.end() )
	{
		return FALSE;
	}

	SPOSTRCVVECITER FindVecIter = std::find( m_ReceiveVec.begin(), m_ReceiveVec.end(), &(FindMapIter->second) );
	if ( FindVecIter == m_ReceiveVec.end() )
	{
		return FALSE;
	}

	// Note : 읽지 않은 우편이라면 새 우편의 개수를 하나줄인다.
	SAPOSTCLIENT* pPost = (*FindVecIter);
	if ( pPost && pPost->IsUnRead() )
	{
		if ( m_sPostInfo.nNew )
		{
			m_sPostInfo.nNew--;
			m_sPostInfo.nReportNewPost = m_sPostInfo.nNew;

		}
	}

	// Note : 삭제하기전 현황을 업데이트
	UpdateHistory ( (*FindVecIter) );

	// Note : 해당 원소를 삭제
	m_ReceiveMap.erase ( FindMapIter );
	m_ReceiveVec.erase ( FindVecIter );

	// Note : 전체 우편 갯수를 줄인다.
	if ( m_sPostInfo.nTotal )
	{
		--m_sPostInfo.nTotal;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_ReceivePageUpdate );	

	return TRUE;
}

const BOOL GLPostClient::MsgTakeItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_OPEN_TAKE_FB *msg = (GLMSG::SNET_POST_OPEN_TAKE_FB *)nmg;

	LONGLONG llPostID = msg->llPostID;
	INT		 Result   = msg->result;

	if (Result != EMPOST_OK)
	{
		switch( msg->result )
		{
		case EMPOST_WRONG_CHARGE:
			break;

		case EMPOST_POST_NOT_EXIST:
			break;

		case EMPOST_REACCEPT_ITEM:
			break;

		case EMPOST_REACCEPT_MONEY:
			break;

		case EMPOST_ETC:
			break;

		case EMPOST_NOTENOUGH_INVEN:			
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_PostBox_ErrMsg,
					"-n", 0 );
			}
			break;

		default:
			break;
		};

		return FALSE;
	}
	// Note : 해당 원소를 찾는다.
	SPOSTRCVMAPITER FindMapIter = m_ReceiveMap.find( llPostID );
	if ( FindMapIter == m_ReceiveMap.end() )
	{
		return FALSE;
	}

	SAPOSTCLIENT& sPost = (FindMapIter->second);

	if ( msg->byMoneyAccept )
	{
		if ( sPost.m_llAttachMoney )
		{   
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::LIGHTSKYBLUE,
                sc::string::format(ID2GAMEINTEXT("POSTBOX_MSG_TAKE_MONEY"), sPost.m_llAttachMoney));

			sPost.m_byMoneyAccept = TRUE;
			sPost.m_llAttachMoney = 0;
		}
		else if ( sPost.m_llCharge )
		{            
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::DISABLE,
                sc::string::format(ID2GAMEINTEXT("POSTBOX_MSG_TAKE_CHARGE"), sPost.m_llCharge));

			sPost.m_byChargePayment = TRUE;
			sPost.m_llCharge		  = 0;
		}
	}

	if ( msg->byAttach01Accept )
	{
		if ( sPost.m_byChargePayment == FALSE && sPost.m_llCharge )
		{            
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::DISABLE,
                sc::string::format(ID2GAMEINTEXT("POSTBOX_MSG_TAKE_CHARGE"), sPost.m_llCharge));

			sPost.m_byChargePayment = TRUE;
			sPost.m_llCharge		  = 0;
		}

		SITEM* pItem = GLogicData::GetInstance().GetItem ( sPost.m_sAttach01.GetNativeID() );

		if ( pItem )
		{            
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::LIGHTSKYBLUE,
                sc::string::format(ID2GAMEINTEXT("POSTBOX_MSG_TAKE_ITEM"), sPost.m_sAttach01.GETNAME().c_str()));
		}

		sPost.m_byAttach01Accept	  = TRUE;
		sPost.m_sAttach01 = SITEMCUSTOM( NATIVEID_NULL() );
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PostBox_ReceivePageUpdate );	

	return TRUE;
}

// Note : 새로온 메일을 알림
const BOOL GLPostClient::MsgReceivePostNotify ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_POST_NOTIFY_RELAY *msg = (GLMSG::SNET_POST_NOTIFY_RELAY *)nmg;

	LONGLONG llPostID = msg->llPostID;
	DWORD	 dwRecver = (DWORD)msg->iRecver;

	if ( m_pGaeaClient->GetCharacter()->GetCharID() != dwRecver )
	{
		return FALSE;
	}

	// Note : 우편 정보를 요청
	return ReqReceivePostInfo ();
}

// Note : 우편현황을 재요청하지 않고 갱신하기 위해 사용한다.
const BOOL GLPostClient::InsertVirtualHistory ( GLMSG::SNET_POST_SEND* nmg )
{
	if ( !nmg )
	{
		return FALSE;
	}

    SAPOSTHISTORY sHistory;
    sHistory.SET(m_pGaeaClient, (*nmg));
    sHistory.llPostID = SAPOSTHISTORY::ID_NULL;

    //SPOSTHISTORYLISTITER iter = m_HistoryInfoSet.HistoryVirtualList.insert( m_HistoryInfoSet.HistoryVirtualList.end(), sHistory );
    //
    //if ( iter == m_HistoryInfoSet.HistoryVirtualList.end() )
    //{
    //    return FALSE;
    //}
    //
    //// Note : 히스토리 벡터에 가상으로 추가
    //m_HistoryInfoSet.HistoryVec.push_back( &(*iter) );

    SPOSTHISTORYLISTITER iter = m_SendHistoryInfoSet.HistoryVirtualList.insert( m_SendHistoryInfoSet.HistoryVirtualList.end(), sHistory );

    if ( iter == m_SendHistoryInfoSet.HistoryVirtualList.end() )
    {
        return FALSE;
    }

    // Note : 히스토리 벡터에 가상으로 추가
    m_SendHistoryInfoSet.HistoryVec.push_back( &(*iter) );

	return TRUE;
}

const BOOL GLPostClient::PopVirtualHistory ()
{
//    if ( !m_HistoryInfoSet.HistoryVec.empty() )
//    {
//        SPOSTHISTORYVECITER iter = m_HistoryInfoSet.HistoryVec.end();
//
//        while ( 1 )
//        {
//            --iter;
//
//            SAPOSTHISTORY* pHistory = (*iter);
//
//            if ( !pHistory )
//            {
//                if ( iter == m_HistoryInfoSet.HistoryVec.begin() )
//                {
//                    return FALSE;
//                }
//
//                continue;
//            }
//
//            // Note : 가상 히스토리라면 삭제한다.
//            if ( pHistory->llPostID == SAPOSTHISTORY::ID_NULL )
//            {
//                m_HistoryInfoSet.HistoryVec.erase( iter );
//                return TRUE;
//            }
//
//            if ( iter == m_HistoryInfoSet.HistoryVec.begin() )
//            {
//                return FALSE;
//            }
//        }
//    }


    if ( !m_SendHistoryInfoSet.HistoryVec.empty() )
    {
        SPOSTHISTORYVECITER iter = m_SendHistoryInfoSet.HistoryVec.end();

        while ( 1 )
        {
            --iter;

            SAPOSTHISTORY* pHistory = (*iter);

            if ( !pHistory )
            {
                if ( iter == m_SendHistoryInfoSet.HistoryVec.begin() )
                {
                    return FALSE;
                }

                continue;
            }

            // Note : 가상 히스토리라면 삭제한다.
            if ( pHistory->llPostID == SAPOSTHISTORY::ID_NULL )
            {
                m_SendHistoryInfoSet.HistoryVec.erase( iter );
                return TRUE;
            }

            if ( iter == m_SendHistoryInfoSet.HistoryVec.begin() )
            {
                return FALSE;
            }
        }
    }

	return FALSE;
}

const BOOL GLPostClient::UpdateHistory ( SAPOSTCLIENT* pPost )
{
	if ( !pPost )
	{
		return FALSE;
	}

	// Note : 해당 원소를 찾는다.
	SPOSTHISTORYMAPITER FindMapIter;
    
    //FindMapIter = m_HistoryInfoSet.HistoryMap.find( pPost->m_llPostID );
	//if ( FindMapIter == m_HistoryInfoSet.HistoryMap.end() )
	//{
	//	return FALSE;
	//}

    FindMapIter = m_RecvHistoryInfoSet.HistoryMap.find( pPost->m_llPostID );
    if ( FindMapIter == m_RecvHistoryInfoSet.HistoryMap.end() )
    {
        return FALSE;
    }

	SAPOSTHISTORY& sHistory = FindMapIter->second;

	sHistory.SET( (*pPost) );

	// Note : 반송타입 체크
	if ( pPost->dwSTATE&SAPOSTCLIENT::EMSTATE_WAIT_RETURN )
	{
		// Note : 타입을 바꾼다
		sHistory.byPostState = EMPOST_RETURN;

		// Note : 반송날짜를 채워준다.
		SYSTEMTIME st;
		GetLocalTime(&st);	
		CTime cTime(st);
        sHistory.ReturnDate = cTime.GetTime();

		// Note : 받는사람과 보낸사람을 교환.
		int nSwap = sHistory.iRecver;
		sHistory.iRecver = sHistory.iSender;
		sHistory.iSender = nSwap;

		char strSwap[CHR_ID_LENGTH];
		::StringCchCopy(strSwap,			 CHR_ID_LENGTH, sHistory.RecverName );
		::StringCchCopy(sHistory.RecverName, CHR_ID_LENGTH, sHistory.SenderName );
		::StringCchCopy(sHistory.SenderName, CHR_ID_LENGTH, strSwap				);

		// Note : 벡터의 해당 데이터를 맨 뒤에 붙인다.
		//SPOSTHISTORYVECITER FindVecIter = std::find( m_HistoryInfoSet.HistoryVec.begin(), m_HistoryInfoSet.HistoryVec.end(), &sHistory );
        //
		//if ( FindVecIter != m_HistoryInfoSet.HistoryVec.end() )
		//{
		//	m_HistoryInfoSet.HistoryVec.erase( FindVecIter );
		//	m_HistoryInfoSet.HistoryVec.push_back( &sHistory );
		//}

        // Note : 보낸 현황과 받은 현황이 분리되었으므로, 받은 현황에서는 제거하고, 보낸 현황에서는 추가한다
        SPOSTHISTORYVECITER FindVecIter = std::find( m_RecvHistoryInfoSet.HistoryVec.begin(), m_RecvHistoryInfoSet.HistoryVec.end(), &sHistory );

        if ( FindVecIter != m_RecvHistoryInfoSet.HistoryVec.end() )
        {
            m_RecvHistoryInfoSet.HistoryVec.erase( FindVecIter );
            m_SendHistoryInfoSet.HistoryVec.push_back( &sHistory );
        }
	}
	// Note : 삭제타입 체크
	else if ( pPost->dwSTATE&SAPOSTCLIENT::EMSTATE_WAIT_DELETE )
	{
		switch ( sHistory.byPostState )
		{
		case EMPOST_UNREAD:
		case EMPOST_READ:
			sHistory.byPostState = EMPOST_USER_DELETE;
			break;

		}

		// Note : 삭제날짜를 채워준다.
		SYSTEMTIME st;
		GetLocalTime(&st);	
		CTime cTime(st);	
        sHistory.DeleteDate = cTime.GetTime();
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PostBox_HistoryPageRecieveUpdate );
	
	return TRUE;
}

LuaTable GLPostClient::GetReceivePagePost()
{
	LuaTable tbPostList( GLWidgetScript::GetInstance().GetLuaState() );
	GLPostClient::SPOSTRCVVEC& RcvPostVec = m_ReceiveVec;

	int nMaxPost = m_ReceiveVec.size()-1;

	int nPostCount = 1;
	for ( int i=nMaxPost; i>=0; --i )
	{
		LuaTable tbPost( GLWidgetScript::GetInstance().GetLuaState() );
		BYTE emBoxType = 0;
		BYTE emType = GLPostClientDefine::EMRECEIVEITEMTYPE_DEFAULT;
		BYTE emPostFlag = 0;
		DWORD dwDesign = 0;
		DWORD dwItemID = 0;
		CString strDate = "";
		CString strLeftDay = "";

		// Note : 삭제 대기중인 우편이라면 스킵한다.
		if ( RcvPostVec[i] && RcvPostVec[i]->dwSTATE&SAPOSTCLIENT::EMSTATE_WAIT_DELETE )
		{
			continue;
		}

		if ( RcvPostVec[i]->IsUnRead() )
		{
			emBoxType = GLPostClientDefine::EMRECEIVEITEMBOXTYPE_NEW;
		}
		else
		{
			emBoxType = GLPostClientDefine::EMRECEIVEITEMBOXTYPE_DEFAULT;
		}

		// Note : 디자인 카드
		if ( emBoxType != GLPostClientDefine::EMRECEIVEITEMBOXTYPE_EMPTY )
		{
			dwDesign = RcvPostVec[i]->m_iDesign;
		}

		// Note : 박스 플래그
		if ( emBoxType != GLPostClientDefine::EMRECEIVEITEMBOXTYPE_EMPTY )
		{
			if ( RcvPostVec[i]->m_byPostClass == EMPOST_GM )
			{
				emPostFlag = EMPOST_GM;
			}
			else if ( RcvPostVec[i]->m_byPostClass == EMPOST_SYSTEM )
			{
				emPostFlag = EMPOST_SYSTEM;
			}
		}

		if ( RcvPostVec[i]->IsReturnPost() )
		{
			emType = GLPostClientDefine::EMRECEIVEITEMTYPE_RETURN;
		}
		else
		{
			// 대금 청구 일때
			if ( !RcvPostVec[i]->m_byChargePayment && RcvPostVec[i]->m_llCharge )
			{
				emType = GLPostClientDefine::EMRECEIVEITEMTYPE_BILLING;
			}
			// 금액 첨부 일때
			else if ( !RcvPostVec[i]->m_byMoneyAccept && RcvPostVec[i]->m_llAttachMoney )
			{
				emType = GLPostClientDefine::EMRECEIVEITEMTYPE_MONEY;
			}
		}

		if ( RcvPostVec[i]->m_byAttach01Accept || RcvPostVec[i]->m_sAttach01.GetNativeID() == NATIVEID_NULL() )
		{
			dwItemID = NATIVEID_NULL().Id();
		}
		else
		{
			dwItemID = RcvPostVec[i]->m_sAttach01.Id();
		}

		// 보낸날짜;
		CTime recentSendDate ( RcvPostVec[i]->GetRecentSendDate() );
		m_pGaeaClient->GetConvertServerTime( recentSendDate );
		strDate.Format( "%04d-%02d-%02d", recentSendDate.GetYear(), recentSendDate.GetMonth(), recentSendDate.GetDay() );

		// 남은날짜;
		DWORD dwDay = RcvPostVec[i]->GetExpireDay(m_pGaeaClient);		
		strLeftDay.Format( "%d%s", dwDay, ID2GAMEWORD("POSTBOX_EXPIRE_DATE", 0) );

		tbPost.set( "boxType", emBoxType );
		tbPost.set( "type", emType );
		tbPost.set( "flag", emPostFlag );
		tbPost.set( "date", strDate.GetString() );
		tbPost.set( "from", RcvPostVec[i]->m_SenderName );
		tbPost.set( "leftDay", strLeftDay.GetString() );
		tbPost.set( "subject", RcvPostVec[i]->m_Title );
		tbPost.set( "itemId", dwItemID );
		tbPost.set( "turnNum", RcvPostVec[i]->m_sAttach01.wTurnNum );
		tbPost.set( "isRead", TRUE == RcvPostVec[ i ]->IsUnRead() ? false : true );
		tbPost.set( "Index", i );

		tbPostList.set( nPostCount, tbPost );
		++nPostCount;
	}

	return tbPostList;
}

LuaTable GLPostClient::GetSendPageDesignType()
{
	LuaTable tbDesignType( GLWidgetScript::GetInstance().GetLuaState() );

	int nDesignCount = 1;
	for ( int i=0; i<EMPOSTBOX_DESIGN_TYPE_SIZE; ++i )
	{
		LuaTable tbDesign( GLWidgetScript::GetInstance().GetLuaState() );
		tbDesign.set(1, COMMENT::POSTBOX_DESIGN_TYPE[ i ].c_str() );

		tbDesignType.set( nDesignCount, tbDesign );
		++nDesignCount;
	}

	return tbDesignType;
}

const DWORD GLPostClient::GetSendPageSlotItem()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return NATIVEID_NULL().dwID;

	return pCharacter->GetPostItem().Id();
}

const WORD GLPostClient::GetSendPageSlotItemTurnNum()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return 0;

	return pCharacter->GetPostItem().wTurnNum;
}

LuaTable GLPostClient::GetReceivePageSelectPost( int nSelect )
{
	LuaTable tbPost( GLWidgetScript::GetInstance().GetLuaState() );

	tbPost.set("date", "" );
	tbPost.set("from", "" );
	tbPost.set("subject", "" );
	tbPost.set("contents", "" );
	tbPost.set("leftDay", "" );
	tbPost.set("design", 0 );

	int nIndex = CalcSelectIndex( nSelect );
	if( 0 > nIndex || m_ReceiveVec.size() <= nIndex )
		return tbPost;

	SAPOSTCLIENT* selectPost = m_ReceiveVec[ nIndex ];
	if( NULL == selectPost )
		return tbPost;

	CString strTime;
	LONGLONG llMoney = 0;
	DWORD dwItemId = NATIVEID_NULL().Id();
	int nTurnNum = 0;

	bool bBilling = false;
	bool bReceive = true;
	bool bReturnAble = true;
	bool bDeleteAble = true;

	// 보낸 날짜;
	CTime sendDate ( selectPost->m_SendDate );
	m_pGaeaClient->GetConvertServerTime( sendDate );
	strTime.Format( "%04d-%02d-%02d %02d:%02d:%02d", sendDate.GetYear(), sendDate.GetMonth(), sendDate.GetDay(),
		sendDate.GetHour(), sendDate.GetMinute(), sendDate.GetSecond());

	// 첨부,청구 금액;
	if ( !selectPost->m_byChargePayment && selectPost->m_llCharge )
	{
		// 대금 청구일시 -를 붙인다;
		llMoney = -selectPost->m_llCharge;
		bBilling = true;
	}
	else if ( !selectPost->m_byMoneyAccept && selectPost->m_llAttachMoney )
	{
		llMoney = selectPost->m_llAttachMoney;
	}
	else
	{
		llMoney =  0;
	}

	// 첨부 아이템;
	if ( !selectPost->m_byAttach01Accept && selectPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
	{
		const SITEM* pItemData = GLogicData::GetInstance().GetItem ( selectPost->m_sAttach01.GetNativeID() );

		dwItemId = selectPost->m_sAttach01.GetNativeID().Id();
		nTurnNum = selectPost->m_sAttach01.wTurnNum;
	}

	// 돈을 이미 받았거나 대금을 지불했을때;
	if ( selectPost->m_byMoneyAccept || selectPost->m_byChargePayment )
	{
		// 아이템도 없다면;
		if ( selectPost->m_sAttach01.GetNativeID() == NATIVEID_NULL() || selectPost->m_byAttach01Accept )
		{
			bReceive = FALSE;
		}
	}

	// 아이템이 없는 상태에서;
	if ( selectPost->m_sAttach01.GetNativeID() == NATIVEID_NULL() || selectPost->m_byAttach01Accept )
	{
		if ( selectPost->m_llAttachMoney == 0 && selectPost->m_llCharge == 0 )
		{
			bReceive = FALSE;
		}
	}

	// 반송이 가능하다면;
	if ( IsReturnAblePost(selectPost) == false )
	{
		bReturnAble = false;
	}

	// 삭제가 가능하다면;
	if ( IsDeleteAblePost(selectPost) == false )
	{
		bDeleteAble = false;
	}

	// 남은 날짜;
	DWORD dwDay = selectPost->GetExpireDay(m_pGaeaClient);
	CString strLeftTime;
	strLeftTime.Format( "%d%s", dwDay, ID2GAMEWORD("POSTBOX_EXPIRE_DATE", 0) );

	tbPost.set("date", strTime.GetString() );
	tbPost.set("from", selectPost->m_SenderName );
	tbPost.set("subject", selectPost->m_Title );
	tbPost.set("contents", selectPost->m_Content );
	tbPost.set("leftDay", strLeftTime.GetString() );

	tbPost.set("money", llMoney );
	tbPost.set("billing", bBilling );
	tbPost.set("receive", bReceive );
	tbPost.set("returnAble", bReturnAble );
	tbPost.set("deleteAble", bDeleteAble );
	tbPost.set("itemId", dwItemId );
	tbPost.set("turnNum", nTurnNum );
	tbPost.set("design", selectPost->m_iDesign );
	tbPost.set("Index", nIndex );


	return tbPost;
}

LuaTable GLPostClient::GetHistoryInfoSet( GLPostClient::SPOSTHISTORYINFOSET& historyInfoSet )
{
	LuaTable tbHistoryList( GLWidgetScript::GetInstance().GetLuaState() );

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return tbHistoryList;

	GLPostClient::SPOSTHISTORYVEC& historyVec = historyInfoSet.HistoryVec;
	int nMaxSize = historyInfoSet.GetMaxHistory()-1;

	int nHistory = 1;
	for( int i=nMaxSize; i>=0; --i )
	{
		LuaTable tbHistory( GLWidgetScript::GetInstance().GetLuaState() );

		SAPOSTHISTORY* history = historyVec[i];
		if( !history )
			continue;

		// Note : 우편 타입 이미지
		if( history->IsReturnPost() )
		{
			tbHistory.set( "type", 3 );
		}
		else
		{
			// 수신
			if ( history->iRecver == pChar->GetCharID() )
			{
				tbHistory.set( "type", 1 );
			}
			// 발신
			else
			{
				tbHistory.set( "type", 2 );
			}
		}

		// 카드인지 텍스트인지 여부
		tbHistory.set( "card" ,  false );
		tbHistory.set( "item",  false );
		tbHistory.set( "money",  false );
		tbHistory.set( "billing",  false );

		if ( history->iDesign )
		{
			tbHistory.set( "card",  true );
		}
		else
		{
			tbHistory.set( "card",  false );
		}

		// 아이템 첨부 여부
		switch ( history->byPostType )
		{
		case EMPOST_ITEM:
		case EMPOST_MONEYITEM:
			{
				tbHistory.set( "item",  true );
			}
			break;
		}

		// 금액첨부/대금청구 여부
		if ( history->llCharge )
		{
			tbHistory.set( "billing",  true );
		}
		else
		{
			switch ( history->byPostType )
			{
			case EMPOST_MONEY:
			case EMPOST_MONEYITEM:
				tbHistory.set( "money",  true );
				break;
			}
		}

		// Note : 날짜
		{
			CTime cSendTime ( history->GetRecentSendDate () );
			m_pGaeaClient->GetConvertServerTime( cSendTime );

			CString strTime;
			strTime.Format( "[%02d.%02d.%02d]", cSendTime.GetYear()%100, cSendTime.GetMonth(), cSendTime.GetDay() );
			tbHistory.set( "sendTime",  strTime.GetString() );
		}

		// Note : 이름
		{
			// Note : 수신이라면 발신자
			if ( history->iRecver == pChar->GetCharID() )
			{
				tbHistory.set( "name",  history->SenderName );
			}
			else
			{
				tbHistory.set( "name",  history->RecverName );
			}
		}

		// 반송우편이라면
		if ( history->IsReturnPost() )
		{
			tbHistory.set( "bReturnPost",  true );

			if ( history->IsDeletePost () )
			{
				// Note : 삭제된 반송우편
				tbHistory.set( "bDeletePost",  true );				
			}
			else
			{
				tbHistory.set( "bDeletePost",  false );

				// Note : 반송 받음
				if ( history->iRecver == pChar->GetCharID() )
				{
					tbHistory.set( "bSendPost",  false );
				}
				// Note : 반송 보냄
				else
				{
					tbHistory.set( "bSendPost",  true );
				}
			}
		}
		else
		{
			tbHistory.set( "IsReturnPost",  false );

			// Note : 우편물의 상태에 따른 정보를 보여준다.
			if ( history->iRecver == pChar->GetCharID() )
			{
				tbHistory.set( "bSendPost",  false );

				if ( history->IsDeletePost () )
				{
					tbHistory.set( "bDeletePost",  true );
				}
				else
				{
					tbHistory.set( "bDeletePost",  false );
				}
			}
			else
			{
				tbHistory.set( "bSendPost",  true );

				if ( history->IsUnRead() )
				{
					tbHistory.set( "bReadPost",  false );
				}
				else
				{
					tbHistory.set( "bReadPost",  true );
				}
			}
		}

		tbHistoryList.set( nHistory, tbHistory );
		++nHistory;
	}

	return tbHistoryList;
}

LuaTable GLPostClient::GetHistoryPageInfoSet()
{
	LuaTable tbHistory( GLWidgetScript::GetInstance().GetLuaState() );

	return GetHistoryInfoSet( m_HistoryInfoSet );
}

LuaTable GLPostClient::GetHistorySendPageInfoSet()
{
	LuaTable tbHistory( GLWidgetScript::GetInstance().GetLuaState() );

	return GetHistoryInfoSet( m_SendHistoryInfoSet );
}

LuaTable GLPostClient::GetHistoryRecievePageInfoSet()
{
	LuaTable tbHistory( GLWidgetScript::GetInstance().GetLuaState() );

	return GetHistoryInfoSet( m_RecvHistoryInfoSet );
}

const DWORD GLPostClient::GetHistorySendPageMaxPage()
{
	return 0;
}

const DWORD GLPostClient::GetHistoryRecievePageMaxPage()
{
	return 0;
}

LuaTable GLPostClient::GetPreviewPagePost()
{
	LuaTable tbPost( GLWidgetScript::GetInstance().GetLuaState() );

	// 날짜
	CTime cCurrentTime = CTime::GetCurrentTime();

	CString strTime;
	strTime.Format( "%04d-%02d-%02d %02d:%02d:%02d", cCurrentTime.GetYear(), cCurrentTime.GetMonth(), cCurrentTime.GetDay(),
		cCurrentTime.GetHour(), cCurrentTime.GetMinute(), cCurrentTime.GetSecond());	

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	tbPost.set( 1, strTime.GetString() );
	tbPost.set( 2, pCharacter->GetName() );
	tbPost.set( 3, pCharacter->GetPostItem().Id() );
	tbPost.set( 4, pCharacter->GetPostItem().wTurnNum );

	//tbPost.set( 5, pCharacter->GetPostItem().GetNativeID() == NATIVEID_NULL() ? 0 : 1 );

	return tbPost;
}

bool GLPostClient::ReqReceivePageRefresh()
{
	// Note : 받기 쿨타임
	if ( IsRecvCoolTime())
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_RECV_COOLTIME") );
		return false;
	}

	if ( !IsRequirePost() && !IsRequireSend() )
	{
		ReqReceivePostInfo();
		ReqReceivePost();
		//m_pPage_History->ResetRequestState();

		return true;
	}

	return false;
}

void GLPostClient::ReqSelectPostOpen( const unsigned int nSelect )
{
	int nIndex = CalcSelectIndex( nSelect );
	if( 0 > nIndex || m_ReceiveVec.size() <= nIndex )
		return;

	SAPOSTCLIENT* selectPost = m_ReceiveVec[ nIndex ];
	if( NULL == selectPost )
		return;

	if( TRUE == selectPost->IsUnRead() )
		ReqOpenPost(selectPost);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PostBox_SelectedPostOpen, "-n", nSelect );
}

void GLPostClient::ReqSelectPostDelete( const unsigned int nSelect, bool bAttachDelete )
{
	int nIndex = CalcSelectIndex( nSelect );
	if( 0 > nIndex || m_ReceiveVec.size() <= nIndex )
		return;

	SAPOSTCLIENT* selectPost = m_ReceiveVec[ nIndex ];
	if ( !selectPost )
		return;

	if ( !bAttachDelete && IsAttachPost(selectPost) )
		return;

	if ( !ReqDeletePost(selectPost, bAttachDelete) )
	{
		m_pGaeaClient->GetPostClient()->ReqReturnPost(selectPost);
	}
}

void GLPostClient::ReqSelectPostTake( const unsigned int nSelect )
{
	int nIndex = CalcSelectIndex( nSelect );
	if( 0 > nIndex || m_ReceiveVec.size() <= nIndex )
		return;

	SAPOSTCLIENT* selectPost = m_ReceiveVec[ nIndex ];
	if ( !selectPost )
		return;

	// Note : 대금 청구일때 소지한 금액보다 대금이 많을경우 실패
	if ( selectPost->m_byChargePayment == FALSE && selectPost->m_llCharge )
	{
		GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

		if ( sCharData.GetInvenMoney() < selectPost->m_llCharge )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_MSG_TAKE_CHAR_MONEY_LACK") );
			return;
		}
	}

	ReqTakeItem(selectPost);
}

void GLPostClient::ReqSelectPostReturn( const unsigned int nSelect )
{
	int nIndex = CalcSelectIndex( nSelect );
	if( 0 > nIndex || m_ReceiveVec.size() <= nIndex )
		return;

	SAPOSTCLIENT* selectPost = m_ReceiveVec[ nIndex ];
	if ( !selectPost )
		return;

	if ( IsReturnAblePost(selectPost) )
	{
		ReqReturnPost(selectPost);
	}
}

void GLPostClient::ReqSelectPostTakeItem( const unsigned int nSelect )
{
	int nIndex = CalcSelectIndex( nSelect );
	if( 0 > nIndex || m_ReceiveVec.size() <= nIndex )
		return;

	SAPOSTCLIENT* selectPost = m_ReceiveVec[ nIndex ];
	if ( !selectPost )
		return;

	if( TRUE == selectPost->IsUnRead() )
		ReqOpenPost(selectPost);

	ReqTakeItem( selectPost );
}

const bool GLPostClient::HaveReceivePostAttachments( const unsigned int nSelect )
{
	int nIndex = CalcSelectIndex( nSelect );
	if( 0 > nIndex || m_ReceiveVec.size() <= nIndex )
		return false;

	SAPOSTCLIENT* selectPost = m_ReceiveVec[ nIndex ];
	if ( !selectPost )
		return false;

	// Note : 아이템이나 금액이 첨부 되어있다면
	if ( selectPost->m_byMoneyAccept == FALSE && selectPost->m_llAttachMoney )
		return true;

	if ( selectPost->m_byAttach01Accept == FALSE && selectPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
		return true;

	// Note : 대금 청구이고 지불을 하지 않았을때
	if ( selectPost->m_byChargePayment == FALSE && selectPost->m_llCharge )
		return true;

	return false;
}

void GLPostClient::ResetCharPostItemData()
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	pChar->SetPostItem(SITEMCUSTOM(NATIVEID_NULL()));
}

const LONGLONG GLPostClient::GetBaseFee()
{
	return (LONGLONG)GLCONST_POST::dwPOST_BASE_FEE;
}

const LONGLONG GLPostClient::GetMaxAttachMoney()
{
	return GLCONST_POST::llPOST_MAX_ATTACH_MONEY;
}

const LONGLONG GLPostClient::GetCommission ( const bool IsAttachDesign, const bool IsAttachItem, const LONGLONG llAttachMoney )
{
	//LONGLONG llAttachMoney = static_cast<LONGLONG>( _atoi64 ( strAttachMoney ) );
	LONGLONG llCommission = GetCommision ( IsAttachDesign, IsAttachItem, llAttachMoney );

	return llCommission;
}

const bool GLPostClient::ReqSendPostTable( LuaTable tbPost )
{
	if( GLUseFeatures::GetInstance().IsUsingPostSendBlock() )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, "Not available.");
		return FALSE;
	}

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if ( !pChar || !pChar->IsValidBody() )
	{
		return FALSE;
	}

	// Note : 레벨 체크
	if ( (DWORD)pChar->GETLEVEL() < GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL )
	{
		CString strText;
		strText.Format ( ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_LEVEL"), GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL );
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText.GetString() );

		return FALSE;
	}

	// Note : 보내는 중일때
	if ( IsRequireSend() )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_PROCESSING") );
		return FALSE;
	}

	// Note : 우편 쿨타임
	if ( IsSendCoolTime() )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_COOLTIME") );
		return FALSE;
	}

	// Note : 대금청구일시 청구 금액이 있어야한다.
	if ( tbPost.get< bool >( "billing" ) )
	{
		if ( !tbPost.get< LONGLONG >( "money" ) )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_BILLING_ZERO") );
			return FALSE;
		}
	}

	// Note : 수수료 검사
	{
		LONGLONG	llCommission   = tbPost.get< LONGLONG >( "commission" );
		if( !tbPost.get< bool >( "billing" ) )
		{
			llCommission = llCommission + tbPost.get< LONGLONG >( "money" );
		}

		GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

		if ( sCharData.GetInvenMoney() < llCommission )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_MONEY_LACK") );
			return FALSE;
		}
	}

	bool bAttachMoney = false;
	bool bAttachItem = false;
	// Note : 메세지 채우기
	{
		GLMSG::SNET_POST_SEND NetMsg;
		NetMsg.iSender = (int)pChar->GetCharID();

		// Note : 받는 사람의 정보는 지금 채울 필요 없음.
		NetMsg.iRecver		  = 0;
		NetMsg.nUserNumRecver = 0;

		// Note : 우편 타입
		NetMsg.byPostClass = EMPOST_USER;
		NetMsg.byPostState = EMPOST_UNREAD;
		//NetMsg.byPostType  = tbPost.get< BYTE >( "type" );

		// Note : 대금 청구일 경우
		if ( tbPost.get< bool >( "billing" ) )
		{
			NetMsg.llAttachMoney = 0;
			NetMsg.llCharge		 = tbPost.get< LONGLONG >( "money" );
			NetMsg.byPayment	 = 1;
		}
		// Note : 금액 첨부일 경우
		else
		{
			NetMsg.llAttachMoney = tbPost.get< LONGLONG >( "money" );
			NetMsg.llCharge		 = 0;
			NetMsg.byPayment	 = 0;
			if( 0 < NetMsg.llAttachMoney )
				bAttachMoney = true;
		}

		// Note : 첨부 아이템		
		SNATIVEID attachItemID = pChar->GetPostItem().Id(); //tbPost.get< DWORD >( "item" );
		if ( attachItemID != NATIVEID_NULL() )
		{
			const SINVEN_POS& sPos = pChar->GetPostItemPos();
			NetMsg.xAttach01 = sPos.wPosX;
			NetMsg.yAttach01 = sPos.wPosY;
			NetMsg.nAttach01 = pChar->GetPostItem().wTurnNum; //tbPost.get< WORD >( "itemCount" );
			NetMsg.idAttach01 = attachItemID;
			bAttachItem = true;
		}
		else
		{
			NetMsg.xAttach01 = USHRT_MAX;
			NetMsg.yAttach01 = USHRT_MAX;
			NetMsg.nAttach01 = 0;
			NetMsg.idAttach01 = NATIVEID_NULL();
		}

		if ( bAttachMoney && bAttachItem )
		{
			NetMsg.byPostType = EMPOST_MONEYITEM;
		}
		else
		{	
			if (bAttachMoney)
			{
				NetMsg.byPostType = EMPOST_MONEY;
			}
			else if (bAttachItem)
			{
				NetMsg.byPostType = EMPOST_ITEM;
			}
			else
			{
				NetMsg.byPostType = EMPOST_TEXT;
			}
		}

		// Note : 텍스트
		StringCchCopy( NetMsg.RecverName,	CHR_ID_LENGTH,			tbPost.get< char* >( "receiveName" ) );
		StringCchCopy( NetMsg.Title,		sizeof(NetMsg.Title)  ,	tbPost.get< char* >( "title" ) );
		StringCchCopy( NetMsg.Content,		sizeof(NetMsg.Content),	tbPost.get< char* >( "contents" ) );

		// Note : 디자인 카드
		NetMsg.iDesign = tbPost.get< int >( "design" );

		// Note : 전송
		if ( !ReqSendPost(&NetMsg) )
		{
			return FALSE;
		}

		// 우편발송후 첨부 아이템 초기화
		pChar->SetPostItemPos( SINVEN_POS() );
		pChar->SetPostItem( SITEMCUSTOM() );
		//m_pInterface->PostBoxWindowDoModalMsg(ID2GAMEINTEXT("MODAL_POSTBOX", 5));

		return TRUE;
	}

	return FALSE;
}

void GLPostClient::ReqUseInvenPostBoxCard()
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if( !pChar || !pChar->IsValidBody() )
	{
		return;
	}

	WORD wPosX, wPosY;
	pChar->GetItemPosInInven( ITEM_POSTBOX_CARD, wPosX, wPosY );
	if( wPosX != USHRT_MAX && wPosY != USHRT_MAX )
	{
		pChar->ReqPostBoxOpenCard( wPosX, wPosY );
	}
}

bool GLPostClient::IsWidgetOpenAble( const bool bPrint )
{
	// Note : 우편함 사용 불가능이면
	if ( false == GLUseFeatures::GetInstance().IsUsingPost() )
		return false;

	// Note : 이미 열려 있다면
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::CF_PostBox_IsOpen ) )
		return false;

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( NULL == pChar || false == pChar->IsValidBody() )
		return false;

	// Note : 받기 쿨타임 체크
	if( TRUE == m_pGaeaClient->GetPostClient()->IsRecvCoolTime() )
	{
		if( true == bPrint )
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_RECV_COOLTIME" ) );

		return false;
	}

	// Note : 대련중에 열 수 없음
	if( EMCONFT_NONE != pChar->GetConftType( pChar->GetGaeaID() ) )
	{
		if( true == bPrint )
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_OPEN_CONFT" ) );

		return false;
	}

	// Note : 개인 상점 중에 열수 없음
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen ) )
	{
		if( true == bPrint )
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_OPEN_PMARKET" ) );

		return false;
	}

	// Note : 거래 중에 열수 없음
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_Trade_IsOpen ) )
	{
		if( true == bPrint )
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_OPEN_TRADE") );

		return false;
	}

	// Note : 우편을 요청중이면 열 수 없다.
	if( TRUE == m_pGaeaClient->GetPostClient()->IsRequirePost() )
	{
		if( true == bPrint )
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_OPEN_PROCESSING") );

		return false;
	}

	return true;
}

int GLPostClient::CalcSelectIndex( int nSelect )
{
	return int( m_ReceiveVec.size() - nSelect - 1 );
}

const SAPOSTCLIENT* GLPostClient::GetSAPOSTCLIENT_ReceiveVec( DWORD dwIndex ) const
{
	if ( dwIndex >= m_ReceiveVec.size() )
		return NULL;

	return m_ReceiveVec[dwIndex];
}