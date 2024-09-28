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
#include "../PrivateMarketSearchBuy/PrivateMarketClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::MsgPMarketTitleFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_TITLE_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_TITLE_FB *) pMsg;
    m_sPMarket.SetTITLE ( pNetMsg->m_PrivateMarketTitle );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_MsgTitleFb, "-s", pNetMsg->m_PrivateMarketTitle );

    //	Note : 인터페이스에 갱신?
}

void GLCharacter::MsgPMarketRegItemFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_REGITEM_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_REGITEM_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMPMARKET_REGITEM_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_FAIL") );
        break;

    case EMPMARKET_REGITEM_FB_OK:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_OK") );

            SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
            if ( pINVENITEM )
            {
                m_sPMarket.RegItem ( *pINVENITEM, pNetMsg->llMoney, pNetMsg->dwNum, pNetMsg->sSALEPOS );

				m_pGaeaClient->GetPrivateMarketClient()->UpdatePrivateMarketItemList( m_sPMarket, CharDbNum(), true );
            }
        }
        break;

    case EMPMARKET_REGITEM_FB_MAXNUM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_MAXNUM") );
        break;

    case EMPMARKET_REGITEM_FB_LIMITED_MONEY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("PRIVATE_LIMITED_A_UINIT_PRICE") );
        break;

    case EMPMARKET_REGITEM_FB_REGITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_REGITEM") );
        break;

    case EMPMARKET_REGITEM_FB_NOSALE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_NOSALE") );
        break;
    };
}

void GLCharacter::MsgPMarketDisItemFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_DISITEM_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_DISITEM_FB *) pMsg;

    m_sPMarket.DisItem ( pNetMsg->sSALEPOS );

	m_pGaeaClient->GetPrivateMarketClient()->UpdatePrivateMarketItemList( m_sPMarket, CharDbNum(), true );
    //	Note : 인터페이스에 갱신?
}

void GLCharacter::MsgPMarketOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_OPEN_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_OPEN_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMPMARKET_OPEN_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_FAIL") );
        break;

    case EMPMARKET_OPEN_FB_OK:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_OK") );
            m_sPMarket.DoMarketOpen();
			m_pGaeaClient->GetPrivateMarketClient()->MsgPrivateMarketOpenFb();

			// [shhan][2015.06.15] 내 상점을 열때만 정리하도록 한다.
			//
			// [shhan][2015.06.15] 물리적 메모리를 정리한다.
			//						상점을 사용하면 안쓰는 물리적메모리를 정리하도록 한다.
			HANDLE hProcess	= GetCurrentProcess();
			if (hProcess)
			{
				EmptyWorkingSet( hProcess );
			}
        }
        break;

    case EMPMARKET_OPEN_FB_ALREADY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_ALREADY") );
        break;

    case EMPMARKET_OPEN_FB_EMPTY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_EMPTY") );
        break;
    };
}

void GLCharacter::MsgPMarketBuyFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_BUY_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_BUY_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMPMARKET_BUY_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_FAIL") );
        break;
    case EMPMARKET_BUY_FB_OK:
        if ( pNetMsg->dwGaeaID == m_dwGaeaID )
        {
            SSALEITEM *pSALEITEM = m_sPMarket.GetItem ( pNetMsg->sSALEPOS );
            if ( pSALEITEM )
            {
                SITEM *pITEM = GLogicData::GetInstance().GetItem ( pSALEITEM->sITEMCUSTOM.GetNativeID() );
                if ( pITEM )
                {
                    std::string strText = sc::string::format( ID2GAMEINTEXT("EMPMARKET_BUY_FB_OK_SELLER"), pSALEITEM->sITEMCUSTOM.GETNAME().c_str(), pNetMsg->dwNum ).c_str();
                    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
					m_pGaeaClient->GetPrivateMarketClient()->UpdatePrivateMarketItemList( m_sPMarket, CharDbNum(), true );
                }
            }
        }
        else
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPMARKET_BUY_FB_OK") );

			std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar( pNetMsg->dwGaeaID );
			if ( pCLIENT == NULL )
				return;

			GLPrivateMarket& sPMarket = pCLIENT->m_sPMarket;
			m_pGaeaClient->GetPrivateMarketClient()->UpdatePrivateMarketItemList( sPMarket, pCLIENT->CharDbNum(), false );
        }
        break;
    case EMPMARKET_BUY_FB_NUM:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NUM") );

            std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar ( pNetMsg->dwGaeaID );
            if ( pCHAR )
            {
                bool bSOLD = pNetMsg->dwNum == 0;
                pCHAR->m_sPMarket.SetSaleState ( pNetMsg->sSALEPOS, pNetMsg->dwNum, bSOLD );
            }
        }
        break;
    case EMPMARKET_BUY_FB_LOWMONEY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_LOWMONEY") );
        break;
    case EMPMARKET_BUY_FB_SOLD:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_SOLD") );
        break;
    case EMPMARKET_BUY_FB_NOINVEN:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NOINVEN") );
        break;
    case EMPMARKET_BUY_FB_NOTIME:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NOTIME") );
        break;
    };
}

void GLCharacter::MsgPMarketItemUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD *pNetMsg = (GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD *) pMsg;

    SSALEITEM *pSALEITEM = m_sPMarket.GetItem ( pNetMsg->sSALEPOS );
    if ( pSALEITEM )
    {
        pSALEITEM->bSOLD = pNetMsg->bSOLD;
        pSALEITEM->dwNUMBER = pNetMsg->dwNUMBER;
        pSALEITEM->sITEMCUSTOM.wTurnNum = (WORD) pNetMsg->dwNUMBER;

        SINVENITEM *pINVENITEM = m_sPMarket.GetInven().GetItem(pNetMsg->sSALEPOS.wMainID,pNetMsg->sSALEPOS.wSubID);
        if ( pINVENITEM )
        {
            pINVENITEM->sItemCustom.wTurnNum = (WORD) pNetMsg->dwNUMBER;
        }
    }
}

void GLCharacter::MsgPMarketCloseBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_CLOSE_BRD *pNetNsg = (GLMSG::SNETPC_PMARKET_CLOSE_BRD *)pMsg;

    DoPrivateMarketClose();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_Close );
}

void GLCharacter::MsgPMarketSearchItemResult(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT *pNetResultMsg = (GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT *)pMsg;	

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_UpdateSearchPage );
}


void GLCharacter::MsgPrivateMarketSearch( NET_MSG_GENERIC* pMsg )
{
	//m_pGaeaClient->GetPrivateMarketClient()->SetSearchPageData();
}
