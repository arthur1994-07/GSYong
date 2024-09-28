#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanGfxUI/GfxInterface.h"

#include "../Land/GLLandManClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Char/GLCharClient.h"
#include "./GLTradeClient.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


GLTradeClient::GLTradeClient(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
    , m_bTrade(FALSE)
{
}

GLTradeClient::~GLTradeClient(void)
{
}

BOOL GLTradeClient::Start ( DWORD dwTargetID )
{
	Reset ();

	m_bTrade = TRUE;

	/*std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetActiveMap()->GetChar(dwTargetID);
	if (!pChar)
        return FALSE;*/

	m_sTarTrade.SetTarget ( dwTargetID );
	m_sMyTrade.SetTarget ( m_pGaeaClient->GetCharacter()->GetGaeaID() );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_CloseWindow );

	return TRUE;
}

BOOL GLTradeClient::Reset ()
{
	m_bTrade = FALSE;

	m_sMyTrade.Reset();
	m_sTarTrade.Reset();

	m_sPreTradeItem.RESET();

	return TRUE;
}

BOOL GLTradeClient::Valid ()
{
	return m_bTrade;
}

void GLTradeClient::SetPreItem ( const SINVEN_POS& sPreItem )
{
	m_sPreTradeItem = sPreItem;
}


SINVENITEM* GLTradeClient::FindPosItem ( const WORD wPosX, const WORD wPosY )
{
	return m_sMyTrade.GetTradeBox().FindPosItem ( wPosX, wPosY );
}

SINVENITEM* GLTradeClient::FindItemBack ( const WORD wBackX, const WORD wBackY )
{
	return m_sMyTrade.GetTradeBox().FindItemBack ( wBackX, wBackY );
}

BOOL GLTradeClient::IsInsertable ( const WORD wPosX, const WORD wPosY )
{
	return m_sMyTrade.GetTradeBox().IsInsertable ( wPosX, wPosY );
}

BOOL GLTradeClient::IsExInsertable ( const WORD wPosX, const WORD wPosY )
{
	GLInventory sBoxTemp;
	sBoxTemp.Assign ( m_sMyTrade.GetTradeBox() );
	
	SINVENITEM *pOldItem = sBoxTemp.FindPosItem ( wPosX, wPosY );
	sBoxTemp.DeleteItem ( pOldItem->wPosX, pOldItem->wPosY );

	return sBoxTemp.IsInsertable ( wPosX, wPosY );
}

BOOL GLTradeClient::IsAgreeAble ()
{
	if ( m_sMyTrade.IsLock() && m_sTarTrade.IsLock() ) // �Ѵ� �ŷ�â�� ��� �־�� �Ѵ�. by luxes.
		return m_sMyTrade.IsAgreeAble();

	return FALSE;
}

void GLTradeClient::BlockAgree ()
{
	m_sMyTrade.BlockAgree ();
	m_sTarTrade.BlockAgree ();
}

BOOL GLTradeClient::MsgTradeFB(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRADE_FB* pPacket = (GLMSG::SNET_TRADE_FB*) nmg;
    if (pPacket->m_Crc32 != sc::string::getCrc32(pPacket, sizeof(GLMSG::SNET_TRADE_FB)-sizeof(unsigned int)))
        pPacket->emAns = EMTRADE_CANCEL;
	
	switch (pPacket->emAns)
	{
	case EMTRADE_OK:
		{
			BOOL bOk = Start(pPacket->dwTargetID);
			if (!bOk)
			{
				//	Note : �ŷ��� ���� ���� ���߽��ϴ�.
				GLMSG::SNET_TRADE_CANCEL NetMsg;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
				break;
			}

			//PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();

			CString strName = pPacket->szMasterName;
			/*std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar ( pPacket->dwTargetID );
			if (pChar)*/
                //strName = pChar->GetName();

			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Trade);
		}
		break;

	case EMTRADE_CANCEL:
		//	������ �ŷ��� ���� �Ͽ����ϴ�.
		PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
		/*std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar ( pPacket->dwTargetID );
		if (!pChar)
            return FALSE;*/
		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(
                ID2GAMEINTEXT("TRADE_FB_CANCEL"),
                pPacket->szMasterName));
		break;
	};

	return TRUE;
}

BOOL GLTradeClient::MsgTradeLockTar( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_LOCK_TAR *pNetMsg = (GLMSG::SNET_TRADE_LOCK_TAR *) nmg;

	BOOL bMY = m_pGaeaClient->GetCharacter()->GetGaeaID() == pNetMsg->dwGaeaID;

	if( TRUE == bMY )
	{
		m_sMyTrade.SetLock( TRUE );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_TradeMineButtonLock,
			"-b", m_sMyTrade.IsLock() == TRUE ? true : false );
	}
	else
	{
		m_sTarTrade.SetLock( TRUE );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_TradeTarButtonLock,
			"-b", m_sTarTrade.IsLock() == TRUE ? true : false );
	}

	BlockAgree ();

	return TRUE;
}

BOOL GLTradeClient::MsgTradeAgreeTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_AGREE_TAR* pPacket = (GLMSG::SNET_TRADE_AGREE_TAR*) nmg;
    if (pPacket->m_Crc32 != sc::string::getCrc32(pPacket, sizeof(GLMSG::SNET_TRADE_AGREE_TAR)-sizeof(unsigned int)))
        return FALSE;
	
	BOOL bMY = m_pGaeaClient->GetCharacter()->GetGaeaID() == pPacket->dwGaeaID;

	switch ( pPacket->emFB )
	{
	case EMTRADE_AGREE_OK:
		if ( bMY )	m_sMyTrade.SetAgree();
		else		m_sTarTrade.SetAgree();
		break;

	case EMTRADE_AGREE_CANCEL:
		if ( bMY )
            m_sMyTrade.ReSetAgree();
		else
            m_sTarTrade.ReSetAgree();
		break;

	case EMTRADE_AGREE_TIME: //	�ð� ����� �ٽ� �õ� �Ͻʽÿ�.
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("TRADE_AGREE_TIME") );
		break;
	};

	if( TRUE == bMY )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_TradeMineButtonAgree,
			"-b", m_sMyTrade.GetAgree() == TRUE ? true : false );
	}
	else
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_TradeTarButtonAgree,
			"-b", m_sTarTrade.GetAgree() == TRUE ? true : false );
	}

	return TRUE;
}

BOOL GLTradeClient::MsgTradeItemResistTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_ITEM_REGIST_TAR *pNetMsg = (GLMSG::SNET_TRADE_ITEM_REGIST_TAR *) nmg;

	// �ŷ� ��� ����.
	//m_sMyTrade.SetLock( FALSE );
	//m_sTarTrade.SetLock( FALSE );

	SINVENITEM &sInsert = pNetMsg->sInsert;

	if( m_pGaeaClient->GETMYGAEAID() == pNetMsg->dwGaeaID )
	{
		if(m_sMyTrade.InsertItem ( &sInsert ))
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Trade_UpdateSlot,
				"-n, -n, -b", sInsert.wPosX, sInsert.wPosY, true );

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Trade_UpdateInvenSlot,
				"-n, -n, -b", sInsert.wBackX, sInsert.wBackY, false );
		}
	}
	else
	{
		if(m_sTarTrade.InsertItem ( &sInsert ))	  
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Trade_UpdateSlot,
				"-n, -n, -b", sInsert.wPosX, sInsert.wPosY, false );
		}

	}

	BlockAgree();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_TradeTarButtonAgree, "-b", m_sTarTrade.GetAgree() == TRUE ? true : false );
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_TradeTarButtonLock, "-b", m_sTarTrade.IsLock() == TRUE ? true : false );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeMineButtonAgree, "-b", m_sMyTrade.GetAgree() == TRUE ? true : false );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeMineButtonLock, "-b", m_sMyTrade.IsLock() == TRUE ? true : false );

	return TRUE;
}

BOOL GLTradeClient::MsgTradeItemReMoveTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_ITEM_REMOVE_TAR *pNetMsg = (GLMSG::SNET_TRADE_ITEM_REMOVE_TAR *) nmg;

	// �ŷ� ��� ����.
	m_sMyTrade.SetLock( FALSE );
	m_sTarTrade.SetLock( FALSE );

	if( m_pGaeaClient->GETMYGAEAID() == pNetMsg->dwGaeaID )
	{
		// �κ��丮 ��Ȱ�� ���� ����.
		SINVENITEM* pInvenItem = m_sMyTrade.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if( NULL != pInvenItem )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Trade_UpdateInvenSlot,
				"-n, -n, -b", pInvenItem->wBackX, pInvenItem->wBackY, true );
		}

		if(m_sMyTrade.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY ))
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Trade_UpdateSlot,
				"-n, -n, -b", pNetMsg->wPosX, pNetMsg->wPosY, true );
		}
	}
	else
	{
		if(m_sTarTrade.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY ))
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Trade_UpdateSlot,
				"-n, -n, -b", pNetMsg->wPosX, pNetMsg->wPosY, false );
		}
	}

	BlockAgree ();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeTarButtonAgree, "-b", m_sTarTrade.GetAgree() == TRUE ? true : false );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeTarButtonLock, "-b", m_sTarTrade.IsLock() == TRUE ? true : false );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeMineButtonAgree, "-b", m_sMyTrade.GetAgree() == TRUE ? true : false );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeMineButtonLock, "-b", m_sMyTrade.IsLock() == TRUE ? true : false );

	return TRUE;
}

BOOL GLTradeClient::MsgTradeMoneyTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_MONEY_TAR *pNetMsg = (GLMSG::SNET_TRADE_MONEY_TAR *) nmg;

	BOOL bMY = m_pGaeaClient->GetCharacter()->GetGaeaID() == pNetMsg->dwGaeaID;

	if (bMY)
        m_sMyTrade.SetMoney(pNetMsg->lnMoney);
	else
        m_sTarTrade.SetMoney(pNetMsg->lnMoney);

	// �ŷ� ��� ����.
	m_sMyTrade.SetLock( FALSE );
	m_sTarTrade.SetLock( FALSE );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeMineButtonLock, "-b", m_sMyTrade.IsLock() == TRUE ? true : false );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_TradeTarButtonLock, "-b", m_sTarTrade.IsLock() == TRUE ? true : false );
	
	if( FALSE == bMY )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_TradeTarTextMoney,
			"-l", m_sTarTrade.GetMoney() );
	}

	BlockAgree ();

	return TRUE;
}

BOOL GLTradeClient::MsgTradeCompleteTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_COMPLETE_TAR *pNetMsg = (GLMSG::SNET_TRADE_COMPLETE_TAR *) nmg;

	//	�ŷ��� ���������� �Ϸ�Ǿ����ϴ�.
	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("TRADE_COMPLETE") );

	Reset ();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_TradeClose );

	// �ŷ��� ������ ��ī�� ������ �������ش�.
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->m_mapPETCardInfoTemp.clear();
	pCharacter->m_mapVEHICLEItemInfoTemp.clear();

	return TRUE;
}

BOOL GLTradeClient::MsgTradeCancelTar ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_TRADE_CANCEL_TAR *pNetMsg = (GLMSG::SNET_TRADE_CANCEL_TAR *) nmg;

	switch ( pNetMsg->emCancel )
	{
	case EMTRADE_CANCEL_NORMAL:
		//	�Ϲ����� �ŷ� ���.
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_NORMAL") );
		break;

	case EMTRADE_CANCEL_OTHER:
		//	������� �ŷ� ���.
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_OTHER") );
		break;

	case EMTRADE_CANCEL_MYINVEN:
		//	�ŷ� ������ �ڽ��� �κ��� ���� ����.
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_MYINVEN") );
		break;

	case EMTRADE_CANCEL_TARINVEN:
		//	�ŷ� ������ ������� �κ��� ���� ����.
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_CANCEL_TARINVEN") );
		break;
	};

	Reset ();

	GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_TradeClose);

	return TRUE;
}

void GLTradeClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_TRADE_FB:
		MsgTradeFB(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_LOCK_TAR:
		MsgTradeLockTar( nmg );
		break;

	case NET_MSG_GCTRL_TRADE_AGREE_TAR:
		MsgTradeAgreeTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR:
		MsgTradeItemResistTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR:
		MsgTradeItemReMoveTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_MONEY_TAR:
		MsgTradeMoneyTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_COMPLETE_TAR:
		MsgTradeCompleteTar(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_CANCEL_TAR:
		MsgTradeCancelTar(nmg);
		break;
	};
}

HRESULT GLTradeClient::FrameMove ( float fTime, float fElapsedTime )
{
	if (!Valid())
        return S_FALSE;
	if (!m_sMyTrade.IsLock() || !m_sTarTrade.IsLock())
        return S_FALSE;

	m_sMyTrade.FrameMove(fTime, fElapsedTime);
	m_sTarTrade.FrameMove(fTime, fElapsedTime);

	return S_OK;
}

BOOL GLTradeClient::ReqTradeMoney ( LONGLONG lnMoney )
{
	GLMSG::SNET_TRADE_MONEY	NetMsg;
	NetMsg.lnMoney = lnMoney;
    //NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return TRUE;
}

BOOL GLTradeClient::ReqTradeOpen()
{
	std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_pGaeaClient->GetCharacter()->GetTarPtoP() );
	if ( !pCharClient ) 
	return E_FAIL;

	if( m_pGaeaClient->GetCharacter()->VehicleState() || pCharClient->VehicleState() )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NO_TRADE_VEHICLE") );	
		return E_FAIL;
	}

	if ( m_pGaeaClient->GetCharacter()->m_sPMarket.IsOpen() )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NO_TRADE_VEHICLE") );
		return E_FAIL;
	}
	else
	{
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return E_FAIL;
		}
		// �Ÿ� üũ
		float fDistance = D3DXVec3Length ( &D3DXVECTOR3( pCharacter->GetPosition() - pCharClient->GetPosition() ) );
		float fTradeRange = static_cast< float >( pCharClient->GETBODYRADIUS() + pCharacter->GETBODYRADIUS() + 30 );
		float fTradeableDistance = fTradeRange + 15.0f;
		if ( fDistance > fTradeableDistance ) 
		{
			return E_FAIL;
		}

		GLMSG::SNET_TRADE NetMsg(pCharClient->GetGaeaID());
		NetMsg.dwChaNum = pCharacter->CharDbNum();
		NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
		return S_OK;
	}

	return E_FAIL;
}

BOOL GLTradeClient::ReqTradeLock()
{
	if ( !m_pGaeaClient->GetTradeClient()->Valid () )
		return E_FAIL;
	if ( m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock() )
		return E_FAIL; // ��� �ִٸ� �� ����� ����.


	GLMSG::SNET_TRADE_LOCK NetMsg;
	NetMsg.dwGaeaID = m_pGaeaClient->GetCharacter()->GetGaeaID();
	NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! �ŷ� ����.
BOOL GLTradeClient::ReqTradeAgree ()
{
	if ( !m_pGaeaClient->GetTradeClient()->Valid () )
		return E_FAIL;

	if ( !m_pGaeaClient->GetTradeClient()->IsAgreeAble() )
	{
		// �ŷ�â ��� �� ����
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_NEED_LOCK") );
		return E_FAIL;
	}

	//! �ŷ��� ���� �����۵��� ������ ������ �ִ��� �˻�.
	//
	if ( !m_pGaeaClient->GetCharacter()->IsVaildTradeInvenSpace () )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_AGREE_FAIL_MYINVEN") );
		return E_FAIL;
	}

	GLMSG::SNET_TRADE_AGREE NetMsg;
	NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! �ŷ� ���.
BOOL GLTradeClient::ReqTradeCancel ()
{
	if ( !m_pGaeaClient->GetTradeClient()->Valid () )
		return E_FAIL;

	GLMSG::SNET_TRADE_CANCEL NetMsg;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_TradeClose );

	return S_OK;
}

void GLTradeClient::ReqTradeAns(bool OKnCancel)
{
	if ( OKnCancel )
	{				
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();      

		// P2P �ŷ� ���� ó��
		GLMSG::SNET_TRADE_TAR_ANS NetMsg;
		NetMsg.dwChaNum = pCharacter->CharDbNum();
		NetMsg.dwMasterID = m_sTarTrade.GetTarget();
		NetMsg.emAns = EMTRADE_OK;
		NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
	else 
	{
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		//	P2P �ŷ� ���� ó��
		GLMSG::SNET_TRADE_TAR_ANS NetMsg;
		NetMsg.dwChaNum = pCharacter->CharDbNum();
		NetMsg.dwMasterID =m_sTarTrade.GetTarget();
		NetMsg.emAns = EMTRADE_CANCEL;
		NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
}
