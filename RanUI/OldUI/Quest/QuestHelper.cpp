# include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Quest/GLQuestPlay.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/Crow/GLCrowData.h"
#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "./WindowRenewal/QuestWindowRenewal.h"
#include "./WindowRenewal/List/QuestListRenewal.h"
#include "./WindowRenewal/Main/QuestMainRenewal.h"
#include "QuestHelper.h"

#include "../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CQuestHelper::CQuestHelper(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)	
    , m_pQuestIDArray(NULL)
    , m_nHelperNum(0)
    , m_nIndex(0)
{

    m_pQuestIDArray = RANPARAM::QuestDisplayID;

    for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; i++ )
    {
        m_pQuestText  [ i ] = NULL;
        m_pShowDetail [ i ] = NULL;
        m_dwDetailID  [ i ] = NATIVEID_NULL().dwID;
    }
}

CQuestHelper::~CQuestHelper()
{
}

void CQuestHelper::CreateSubControl()
{
	{
		for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; i++ )
		{
			CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

			m_pQuestText[ i ] = new CBasicTextBoxEx(m_pEngineDevice);
			m_pQuestText[ i ]->CreateSub( this, "QUEST_HELPER_TEXT", UI_FLAG_DEFAULT, ( QUEST_HELPER_TEXT_0 + i ) );
			m_pQuestText[ i ]->SetFont( pFont );
			m_pQuestText[ i ]->SetTextAlign( TEXT_ALIGN_LEFT );
			m_pQuestText[ i ]->SetLineInterval( 3.0f );
			m_pQuestText[ i ]->SetSensitive( true );
			m_pQuestText[ i ]->SetLimitLine( 10000 );
			RegisterControl( m_pQuestText[ i ] );
		}
	}

	{
		for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; i++ )
		{
			CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
			pButton->CreateSub( this, "QUEST_WINDOW_HELPER_SHOW", UI_FLAG_DEFAULT, ( QUEST_HELPER_SHOW_DETAIL_0 + i ) );
			pButton->CreateFlip( "QUEST_WINDOW_HELPER_SHOW_F", CBasicButton::RADIO_FLIP );
			pButton->SetControlNameEx ( "QUEST_WINDOW_HELPER_SHOW" );
			RegisterControl( pButton );
			m_pShowDetail[ i ] = pButton;
			m_pShowDetail[ i ]->SetVisibleSingle( FALSE );
		}
	}
}

void CQuestHelper::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; ++i )
    {
        if ( m_pShowDetail[ i ]->IsVisible() )
        {
            if ( m_pShowDetail[ i ]->IsFlip() )
            {
                if ( !m_pInterface->GetQuestWindowRenewal()->IsVisible()
                    || m_pInterface->GetQuestWindowRenewal()->GetQuestMainID() != m_dwDetailID[ i ] )
                    m_pShowDetail[ i ]->SetFlip( FALSE );
            }
            else
            {
                if ( m_pInterface->GetQuestWindowRenewal()->IsVisible()
                    && m_pInterface->GetQuestWindowRenewal()->GetQuestMainID() == m_dwDetailID[ i ] )
                    m_pShowDetail[ i ]->SetFlip( TRUE );
            }
        }
    }
}

void CQuestHelper::UpdateQuestInfo()
{
    m_nIndex = 0;

    for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; i++ )
    {
        m_pQuestText[ i ]->SetVisibleSingle( FALSE );
        m_pShowDetail[ i ]->SetFlip(FALSE);
        m_pShowDetail[ i ]->SetVisibleSingle( FALSE );
        m_dwDetailID[ i ] = NATIVEID_NULL().dwID;
 
        m_pQuestText[ i ]->ClearText();

        if ( m_pQuestIDArray[ i ] != NATIVEID_NULL().dwID )
        {
            GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;
            GLQUESTPROG* pQuestProg = cQuestPlay.FindProc( m_pQuestIDArray[ i ] );
            if ( !pQuestProg ){
                m_pQuestIDArray[ i ] = NATIVEID_NULL().dwID;
                continue;
            }

            GLQUEST* pQuest = GLQuestMan::GetInstance().Find( m_pQuestIDArray[ i ] );
            if ( !pQuest ){
                m_pQuestIDArray[ i ] = NATIVEID_NULL().dwID;
                continue;
            }

            m_pQuestText[ m_nIndex ]->SetVisibleSingle( TRUE );
            m_pShowDetail[ m_nIndex ]->SetVisibleSingle( TRUE );
            m_dwDetailID[ m_nIndex ] = m_pQuestIDArray[ i ];

            if ( m_nIndex > 0 )
            {
                const UIRECT& rcTargetLP = m_pQuestText[ m_nIndex - 1 ]->GetLocalPos();
                const UIRECT& temp1 = m_pQuestText[ m_nIndex - 1 ]->GetGlobalPos();

                D3DXVECTOR2 vPos;
                vPos.x = rcTargetLP.left;
                vPos.y = rcTargetLP.bottom + fDISPLAY_INTERVAL;

                m_pQuestText[ m_nIndex ]->SetLocalPos( vPos );

                const UIRECT& rcTargetGP = m_pQuestText[ m_nIndex - 1 ]->GetGlobalPos();

                vPos.x = rcTargetGP.left;
                vPos.y = rcTargetGP.bottom + fDISPLAY_INTERVAL;

                m_pQuestText[ m_nIndex ]->SetGlobalPos( vPos );
            }

            const DWORD dwSTEP = pQuestProg->m_dwSTEP;
            GLQUEST_STEP* pQuestStep = pQuest->GetSTEP( dwSTEP );

            GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;	

            {
                const UIRECT& rcTargetLP = m_pQuestText[ m_nIndex ]->GetLocalPos();
                const UIRECT& rcWinLP = m_pShowDetail[ m_nIndex ]->GetLocalPos();

                D3DXVECTOR2 vPos;
                vPos.x = rcTargetLP.left - rcWinLP.sizeX - fDISPLAY_SHOW_INTERVAL;
                vPos.y = rcTargetLP.top;

                m_pShowDetail[ m_nIndex ]->SetLocalPos( vPos );


                const UIRECT& rcTargetGP = m_pQuestText[ m_nIndex ]->GetGlobalPos();
                const UIRECT& rcWinGP = m_pShowDetail[ m_nIndex ]->GetGlobalPos();

                vPos.x = rcTargetGP.left - rcWinGP.sizeX - fDISPLAY_SHOW_INTERVAL;
                vPos.y = rcTargetGP.top;

                m_pShowDetail[ m_nIndex ]->SetGlobalPos( vPos );
            }

            CString strTitle;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
				strTitle.Format( "%s:%s ", ID2GAMEWORD("QUEST_HELPER_TEXT",0), CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
			}
			else
			{
				strTitle.Format( "%s:%s ", ID2GAMEWORD("QUEST_HELPER_TEXT",0), pQuest->GetTITLE() );
			}

            m_pQuestText[ m_nIndex ]->AddText( strTitle, NS_UITEXTCOLOR::LIGHTSKYBLUE );

            if ( pQuestStep->IsNEED_QITEM() )
            {
                SNATIVEID dwItemID;
                DWORD dwMaxNum = 0;
                DWORD dwCurNum = 0;	
                CString strItem;

                for (size_t i=0; i<pQuestStep->m_vecMOBGEN_QITEM.size(); ++i)
                {
                    dwCurNum = 0;
                    dwItemID = pQuestStep->m_vecMOBGEN_QITEM[i].sNID;
                    dwMaxNum = pQuestStep->m_vecMOBGEN_QITEM[i].wNUM;			
                    const SITEM* pItem = GLogicData::GetInstance().GetItem(dwItemID);
                    if (!pItem)
                        continue;

                    const GLInventory::CELL_MAP *ItemList = pQuestProg->m_sINVENTORY.GetItemList();
                    GLInventory::CELL_MAP_CITER iter = ItemList->begin();
                    GLInventory::CELL_MAP_CITER iter_end = ItemList->end();

                    for (; iter!=iter_end; ++iter)
                    {
                        SINVENITEM* pInvenItem = (*iter).second;				
                        SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;

                        if (ref_ItemCustom.GetNativeID() == dwItemID)
                        {
                            dwCurNum += ref_ItemCustom.wTurnNum;
                        }
                    }

                    strItem.Format ( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),pItem->GetName(),dwCurNum,dwMaxNum); 
                    m_pQuestText[ m_nIndex ]->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );

                }
            }

            if ( pQuestStep->IsNEED_MOBKILL() || pQuestStep->IsNEED_PLAYERKILL() )
            {		
                CString strMob;

                DWORD dwMobId = pQuestStep->m_dwNID_MOBKILL;
                const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( dwMobId );
                if ( pCrowData )
                {
                    strMob.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
                        pCrowData->GetName(),
                        sSTEP_PROG.m_dwNUM_MOBKILL,
                        pQuestStep->m_dwNUM_MOBKILL ); 
                    m_pQuestText[ m_nIndex ]->AddText( strMob, NS_UITEXTCOLOR::LIGHTSKYBLUE );
                }
				else
				{
					strMob.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
						"蜡历",
						sSTEP_PROG.m_dwNUM_MOBKILL,
						pQuestStep->m_dwNUM_MOBKILL ); 
					m_pQuestText[ m_nIndex ]->AddText( strMob, NS_UITEXTCOLOR::LIGHTSKYBLUE );
				}
            }
			
			if ( pQuestStep->IsNEED_ITEMUSE() )
			{		
				CString strItem;

				DWORD dwItemId = pQuestStep->m_dwNID_ITEMUSE;
				SITEM* pITEM = GLogicData::GetInstance().GetItem ( SNATIVEID(dwItemId) );

				if ( pITEM )
				{
					strItem.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
						pITEM->GetName(),
						sSTEP_PROG.m_dwNUM_ITEMUSE,
						pQuestStep->m_dwNUM_ITEMUSE ); 
					m_pQuestText[ m_nIndex ]->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );
				}
				else
				{
					strItem.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
						"蜡历",
						sSTEP_PROG.m_dwNUM_ITEMUSE,
						pQuestStep->m_dwNUM_ITEMUSE ); 
					m_pQuestText[ m_nIndex ]->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );
				}
			}

			if ( pQuestStep->IsNEED_ITEMGRINDING() )
			{		
				CString strItem;

				DWORD dwItemId = pQuestStep->m_dwNID_ITEMGRINDING;
				SITEM* pITEM = GLogicData::GetInstance().GetItem ( SNATIVEID(dwItemId) );

				if ( pITEM )
				{
					strItem.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
						pITEM->GetName(),
						sSTEP_PROG.m_dwNUM_ITEMGRINDING,
						pQuestStep->m_dwNUM_ITEMGRINDING ); 
					m_pQuestText[ m_nIndex ]->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );
				}
				else
				{
					strItem.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
						"蜡历",
						sSTEP_PROG.m_dwNUM_ITEMGRINDING,
						pQuestStep->m_dwNUM_ITEMGRINDING ); 
					m_pQuestText[ m_nIndex ]->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );
				}
			}

			if ( pQuestStep->IsNEED_CONFTING() )
			{		
				CString strItem;

				//DWORD dwItemId = pQuestStep->m_dwNID_ITEMGRINDING;
				//SITEM* pITEM = GLogicData::GetInstance().GetItem ( SNATIVEID(dwItemId) );

				//if ( pITEM )
				//{
				//	strItem.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
				//		pITEM->GetName(),
				//		sSTEP_PROG.m_dwNUM_CONFTING,
				//		pQuestStep->m_dwNUM_CONFTING ); 
				//	m_pQuestText[ m_nIndex ]->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );
				//}
				//else
				{
					strItem.Format( "%s:%s %d/%d", ID2GAMEWORD("QUEST_HELPER_TEXT",1),
						pQuestStep->m_strOBJ_CONFTING.c_str(),
						sSTEP_PROG.m_dwNUM_CONFTING,
						pQuestStep->m_dwNUM_CONFTING ); 
					m_pQuestText[ m_nIndex ]->AddText( strItem, NS_UITEXTCOLOR::LIGHTSKYBLUE );
				}
			}

            {
                const UIRECT& rcTextBoxLocalPos = m_pQuestText[ m_nIndex ]->GetLocalPos();
                const UIRECT& rcOriginPos = m_pQuestText[ m_nIndex ]->GetLocalPos();

                int nTotalLine = m_pQuestText[ m_nIndex ]->GetTotalLine();
                const float fAllLine = m_pQuestText[ m_nIndex ]->CalcMaxHEIGHT( 0, nTotalLine );
                const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY;

                UIRECT rcLocalNewPos = UIRECT ( rcOriginPos.left, rcOriginPos.top,
                    rcOriginPos.sizeX, rcOriginPos.sizeY + fTextBoxGapY );

                m_pQuestText[ m_nIndex ]->SetLocalPos ( rcLocalNewPos );
            }

            {
                const UIRECT& rcTextBoxLocalPos = m_pQuestText[ m_nIndex ]->GetGlobalPos();
                const UIRECT& rcOriginPos = m_pQuestText[ m_nIndex ]->GetGlobalPos();

                int nTotalLine = m_pQuestText[ m_nIndex ]->GetTotalLine();
                const float fAllLine = m_pQuestText[ m_nIndex ]->CalcMaxHEIGHT( 0, nTotalLine );
                const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY;

                UIRECT rcLocalNewPos = UIRECT ( rcOriginPos.left, rcOriginPos.top,
                    rcOriginPos.sizeX, rcOriginPos.sizeY + fTextBoxGapY );

                m_pQuestText[ m_nIndex ]->SetGlobalPos( rcLocalNewPos );
            }

            m_nIndex++;
        }
    }
}

bool CQuestHelper::SetQuestID( DWORD dwQuestID, bool bPrintMsg )
{
    if ( dwQuestID == NATIVEID_NULL().dwID )	return false;

    if ( IsSameQuest(dwQuestID) )
    {
        ReSetQuestID( dwQuestID );

        GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );

		//pQUEST NULL 眉农
		if ( !pQuest )
			return false;

        std::string strText;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
			// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
			strText = sc::string::format( "%s%s", ID2GAMEINTEXT( "QUEST_HELPER_DEL" ), CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
		}
		else
		{
			strText = sc::string::format( "%s%s", ID2GAMEINTEXT( "QUEST_HELPER_DEL" ), pQuest->GetTITLE() );
		}
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::LIGHTSKYBLUE, strText );
        return true;
    }

    for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; ++i )
    {
        if ( m_pQuestIDArray[i] == NATIVEID_NULL().dwID )
        {
            m_pQuestIDArray[i] = dwQuestID;
            SetVisibleSingle( TRUE );

            GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
			//pQUEST NULL 眉农
			if ( !pQuest )
				return false;

			if( bPrintMsg )
			{
				std::string strText ;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString strKey;
					strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
					// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
					strText = sc::string::format( ID2GAMEINTEXT( "QUEST_HELPER_ADD" ), CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
				}
				else
				{
					strText = sc::string::format( ID2GAMEINTEXT( "QUEST_HELPER_ADD" ), pQuest->GetTITLE() );
				}
				m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::LIGHTSKYBLUE, strText );
			}

            UpdateQuestInfo();

            return true;
        }
    }

    m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "QUEST_HELPER_ERROR" ));

    return false;
}

void CQuestHelper::ReSetQuestID( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID )	return;

	for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_pQuestIDArray[i] == dwQuestID )
		{
			m_pQuestIDArray[i] = NATIVEID_NULL().dwID;

			break;
		}
	}

	if ( !IsQuestPrint() )	SetVisibleSingle( FALSE );

    UpdateQuestInfo();
}

bool CQuestHelper::IsQuestPrint()
{
	for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_pQuestIDArray[i] != NATIVEID_NULL().dwID )	return true;			
	}

	return false;
}

bool CQuestHelper::IsSameQuest( DWORD dwQuestID )
{
	for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; ++i )
	{
		if ( m_pQuestIDArray[i] == dwQuestID )	return true;			
	}

	return false;

}

void CQuestHelper::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( cID, dwMsg );

    switch ( cID )
    {
    case QUEST_HELPER_SHOW_DETAIL_0:
    case QUEST_HELPER_SHOW_DETAIL_1:
    case QUEST_HELPER_SHOW_DETAIL_2:
    case QUEST_HELPER_SHOW_DETAIL_3:
    case QUEST_HELPER_SHOW_DETAIL_4:
        {
            int nIndex = cID - QUEST_HELPER_SHOW_DETAIL_0;
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                NSGUI::SetCharMoveBlock();

                if( UIMSG_LB_UP & dwMsg )
                {
                    if ( m_pShowDetail[ nIndex ]->IsFlip() )
                    {
                        UIGUID QuestWindowID = m_pInterface->GetQuestWindowRenewal()->GetWndID();

                        m_pInterface->UiHideGroup( QuestWindowID );
                    }
                    else
                    {
                        for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; i++ )
                            m_pShowDetail[ i ]->SetFlip( FALSE );

                        m_pShowDetail[ nIndex ]->SetFlip( TRUE );

                        if ( m_dwDetailID[ nIndex ] != NATIVEID_NULL().dwID )
                        {
                            m_pInterface->QUEST_HELPTER_TO_WINDOW( m_dwDetailID[ nIndex ] );

                            {
                                D3DXVECTOR2 vPos;

                                const UIRECT& rcTarLP = m_pInterface->GetQuestWindowRenewal()->GetUIWindowLocalPos();
                                const UIRECT& rcWinLP = m_pShowDetail[ nIndex ]->GetLocalPos();

                                vPos.x = rcWinLP.left - rcTarLP.sizeX - fDISPLAY_INTERVAL;
                                vPos.y = rcTarLP.top;

                                m_pInterface->GetQuestWindowRenewal()->SetUIWindowLocalPos( vPos );

                                const UIRECT& rcTarGP = m_pInterface->GetQuestWindowRenewal()->GetUIWindowGlobalPos();
                                const UIRECT& rcWinGP = m_pShowDetail[ nIndex ]->GetGlobalPos();

                                vPos.x = rcWinGP.left - rcTarGP.sizeX - fDISPLAY_INTERVAL;
                                vPos.y = rcTarGP.top;

                                m_pInterface->GetQuestWindowRenewal()->SetUIWindowGlobalPos( vPos );
                            }
                        }
                    }
                }
            }
        }
        break;
    }
}

int CQuestHelper::GetHelperNumber()
{
    m_nHelperNum = 0;

    for ( int i = 0; i < RANPARAM::MAX_QUEST_DISPLAY; i++ )
    {
        if ( m_pQuestIDArray[ i ] != NATIVEID_NULL().dwID )
            m_nHelperNum++;
    }

    return m_nHelperNum;
}

MyQuestHelper::MyQuestHelper( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CQuestHelper( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyQuestHelper::CreateUIWindowAndRegisterOwnership()
{
    CQuestHelper::Create( QUEST_HELPER_DISPLAY, "QUEST_HELPER_DISPLAY", UI_FLAG_RIGHT );
    CQuestHelper::CreateSubControl();
    CQuestHelper::m_pInterface->UiRegisterControl( this );
    CQuestHelper::m_pInterface->UiShowGroupBottom( QUEST_HELPER_DISPLAY );
}

bool MyQuestHelper::SetQuestID( DWORD dwQuestID, bool bCheckPrintMsg )
{
    return CQuestHelper::SetQuestID( dwQuestID, bCheckPrintMsg );
}

void MyQuestHelper::ReSetQuestID( DWORD dwQuestID )
{
    CQuestHelper::ReSetQuestID( dwQuestID );
}

bool MyQuestHelper::IsSameQuest( DWORD dwQuestID )
{
    return CQuestHelper::IsSameQuest( dwQuestID );
}

int MyQuestHelper::GetHelperCount()
{
    return CQuestHelper::GetHelperNumber();
}

int MyQuestHelper::GetHelperMaxCount()
{
    return CQuestHelper::GetMaxHelperNumber();
}

void MyQuestHelper::UpdateQuestInfo()
{
    CQuestHelper::UpdateQuestInfo();
}
