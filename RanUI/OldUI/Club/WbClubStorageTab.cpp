#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../InnerInterface.h"
#include "./WbClubStoragePage.h"
#include "./WbClubStorageUseRecordPage.h"
#include "./WbClubStorageInputEdit.h"
#include "./WbClubStorageTab.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


CWbClubStorageTab::CWbClubStorageTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pProfitsText(NULL)
    , m_pProfitsBtn(NULL)
    , m_pKeepMoneyText(NULL)
    , m_pDepositBtn(NULL)
    , m_pWithDrawBtn(NULL)
    , m_pStorageUseRecordPage(NULL)
    , m_pInputEdit(NULL)
{
    for ( int i = 0; i < nMAX_STORAGE_CHANNEL; i++ )
        m_pPage[i] = NULL;
}

CWbClubStorageTab::~CWbClubStorageTab()
{
}

void CWbClubStorageTab::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_BACK2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );
    }

    {
        for ( int i = 0; i < nMAX_STORAGE_CHANNEL; i++ )
        {
            std::string strUI = sc::string::format( "WB_CLUB_WINDOW_STORAGE_TAB_STORAGE_PAGE_%1%", i );

            CWbClubStoragePage* pStoragePage = new CWbClubStoragePage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pStoragePage->CreateSub ( this, strUI.c_str(), UI_FLAG_DEFAULT, (STORAGE_PAGE_START+i) );
            pStoragePage->CreateSubControl();
            pStoragePage->InitStoragePage( i );
            RegisterControl( pStoragePage );
            m_pPage[i] = pStoragePage;
        }
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_PROFITS_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_KEEPMONEY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont11Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 11, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_PROFITS_TEXT1" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_MONEY", 0), NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_PROFITS_TEXT2" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pProfitsText = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_KEEPMONEY_TEXT1" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_MONEY", 1), NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_KEEPMONEY_TEXT2" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pKeepMoneyText = pTextBox;
    }

    {
        CD3DFontPar* pFont11Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 11, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont11Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_MONEY", 2);
        RnButton* pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_STORAGE_TAB_PROFITS_BTN", UI_FLAG_DEFAULT, PROFITS_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pProfitsBtn = pRnButton;

        arg.text = ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_MONEY", 3);
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_STORAGE_TAB_KEEPMONEY_DEPOSIT_BTN", UI_FLAG_DEFAULT, DEPOSIT_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pDepositBtn = pRnButton;

        arg.text = ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_MONEY", 4);
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_STORAGE_TAB_KEEPMONEY_WITHDRAW_BTN", UI_FLAG_DEFAULT, WITHDRAW_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pWithDrawBtn = pRnButton;
    }

    {
        CWbClubStorageUseRecordPage* pClubStorageUseRecord = new CWbClubStorageUseRecordPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubStorageUseRecord->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD" );
        pClubStorageUseRecord->CreateSubControl();
        RegisterControl( pClubStorageUseRecord );
        m_pStorageUseRecordPage = pClubStorageUseRecord;
    }

    {
        CWbClubStorageInputEdit* pInputEdit = new CWbClubStorageInputEdit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pInputEdit->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT", UI_FLAG_DEFAULT, INPUT_EIDT );
        pInputEdit->CreateSubControl();
        RegisterControl( pInputEdit );
        pInputEdit->SetVisibleSingle( FALSE );
        m_pInputEdit = pInputEdit;
    }
}

void CWbClubStorageTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubStorageTab::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    if ( m_pInputEdit->IsVisible() )
        return;

    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    if ( STORAGE_PAGE_START <= ControlID && ControlID <= STORAGE_PAGE_END )
    {
        //if ( CHECK_MOUSE_IN( dwMsg ) )
        //{
        //    int nPosX, nPosY;
        //    m_pPage[m_nOpenPage]->GetItemIndex( &nPosX, &nPosY );

        //    CDebugSet::ToView ( 1, 3, "[storage] Page:%d %d / %d", m_nOpenPage, nPosX, nPosY );

        //    if ( nPosX < 0 || nPosY < 0 ) return ;

        //    //			if ( CUIMan::GetFocusControl () == this )
        //    {
        //        const SINVENITEM* pInvenItem = m_pPage[m_nOpenPage]->GetItem ( nPosX, nPosY );
        //        if (pInvenItem && (pInvenItem->sItemCustom.sNativeID != NATIVEID_NULL()))
        //        {
        //            m_pInterface->SHOW_ITEM_INFO(
        //                pInvenItem->sItemCustom,
        //                FALSE, FALSE, FALSE, TRUE, FALSE,
        //                pInvenItem->wPosX, pInvenItem->wPosY );
        //        }
        //    }

        //    if (dwMsg & UIMSG_LB_UP)
        //    {
        //        if (m_bSplitItem)
        //        {
        //            m_bSplitItem = FALSE;

        //            WORD wSplitPosX, wSplitPosY;
        //            GetSplitPos ( &wSplitPosX, &wSplitPosY );
        //            BOOL bDiffPos = !( nPosX == wSplitPosX && nPosY == wSplitPosY );
        //            //BOOL bSplitable = m_pGaeaClient->IsClubStorageSplitItem ( m_nOpenPage, wSplitPosX, wSplitPosY );
        //            if ( bDiffPos /*&& bSplitable*/ )
        //            {
        //                m_pInterface->DoModal ( ID2GAMEINTEXT ( "SPLIT_ITEM" ), UI::MODAL_QUESTION, UI::EDITBOX_NUMBER, UI::MODAL_CLUB_STORAGE_SPLIT_ITEM );
        //            }
        //            else
        //            {
        //                //m_pGaeaClient->ReqClubStorageTo(m_nOpenPage, nPosX, nPosY);
        //            }
        //        }
        //        return;
        //    }

        //    if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED )
        //    {
        //        if ( dwMsg & UIMSG_RB_UP )
        //        {
        //            const SINVENITEM* pInvenItem = m_pPage[m_nOpenPage]->GetItem ( nPosX, nPosY );
        //            if (pInvenItem && (pInvenItem->sItemCustom.sNativeID != NATIVEID_NULL()))
        //            {
        //                SINVEN_POS sInvenPos;
        //                sInvenPos.wPosX = nPosX;
        //                sInvenPos.wPosY = nPosY;

        //                SLINK_DATA_BASIC sLinkDataBasic;
        //                sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
        //                sLinkDataBasic.sLinkType.wSType = ITEM_LINK_CLUB_STORAGE;
        //                sLinkDataBasic.dwData1 = m_nOpenPage;
        //                sLinkDataBasic.dwData2 = sInvenPos.dwPos;					

        //                m_pInterface->ChatToLink( sLinkDataBasic );
        //                return ;
        //            }
        //        }
        //    }

        //    if ( dwMsg & UIMSG_LB_DOWN )
        //    {
        //        SetSplitPos ( nPosX, nPosY );
        //        m_bSplitItem = TRUE;
        //        return ;
        //    }
        //}
    }

    switch ( ControlID )
    {
    case PROFITS_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pGaeaClient->ClubIncomeMoneyToStorageMoney();
            }
        }
        break;

    case DEPOSIT_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                const UIRECT& rcBtnGP = m_pDepositBtn->GetGlobalPos();
                const UIRECT& rcEditLP = m_pInputEdit->GetLocalPos();

                D3DXVECTOR2 vPos;
                vPos.x = rcBtnGP.right - rcEditLP.sizeX;
                vPos.y = rcBtnGP.top - rcEditLP.sizeY;

				const UIRECT& rcBtLocalPos = m_pDepositBtn->GetLocalPos();
				
				float fLocalX = rcBtLocalPos.left - rcEditLP.sizeX + rcBtnGP.sizeX;
				float fLocalY = rcBtLocalPos.top  - rcEditLP.sizeY;

				const UIRECT rcNewPos = UIRECT( fLocalX, fLocalY, rcEditLP.sizeX, rcEditLP.sizeY );

				m_pInputEdit->SetLocalPos(rcNewPos);
                m_pInputEdit->SetGlobalPos( vPos );

                m_pInputEdit->SetDepositType();
                m_pInputEdit->SetVisibleSingle( TRUE );
            }
        }
        break;

    case WITHDRAW_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                const UIRECT& rcBtnGP = m_pWithDrawBtn->GetGlobalPos();
                const UIRECT& rcEditLP = m_pInputEdit->GetLocalPos();

                D3DXVECTOR2 vPos;
                vPos.x = rcBtnGP.right - rcEditLP.sizeX;
                vPos.y = rcBtnGP.top - rcEditLP.sizeY;

				const UIRECT& rcBtLocalPos = m_pWithDrawBtn->GetLocalPos();

				float fLocalX = rcBtLocalPos.left - rcEditLP.sizeX + rcBtnGP.sizeX;
				float fLocalY = rcBtLocalPos.top  - rcEditLP.sizeY;

				const UIRECT rcNewPos = UIRECT( fLocalX, fLocalY, rcEditLP.sizeX, rcEditLP.sizeY );

				m_pInputEdit->SetLocalPos(rcNewPos);
                m_pInputEdit->SetGlobalPos( vPos );

                m_pInputEdit->SetWithDrawType();
                m_pInputEdit->SetVisibleSingle( TRUE );
            }
        }
        break;
    };
}

void CWbClubStorageTab::SetVisibleSingle( BOOL bVisible )
{
    if ( bVisible == TRUE )
    {
        Refresh();
    }

    CUIGroup::SetVisibleSingle( bVisible );
}

void CWbClubStorageTab::Refresh()
{
    if ( m_pGaeaClient->ReqGetClubStorage() )
    {
        ClearInfo();

        m_pProfitsBtn->SetVisibleSingle( FALSE );
        m_pDepositBtn->SetVisibleSingle( FALSE );
        m_pWithDrawBtn->SetVisibleSingle( FALSE );
    }
    m_pStorageUseRecordPage->Refresh();

    for ( int i = 0; i < nMAX_STORAGE_CHANNEL; i++ )
        m_pPage[i]->InitStoragePage( i );
}

void CWbClubStorageTab::ClubStorageUpdateMoney()
{
    std::string strProfits = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_pGaeaClient->ClubIncomeMoney(), 3, "," );

    m_pProfitsText->SetText( strProfits.c_str(), NS_UITEXTCOLOR::WHITE );

    std::string strKeepMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_pGaeaClient->ClubStorageMoney(), 3, "," );

    m_pKeepMoneyText->SetText( strKeepMoney.c_str(), NS_UITEXTCOLOR::WHITE );

    m_pProfitsBtn->SetVisibleSingle( TRUE );
    m_pDepositBtn->SetVisibleSingle( TRUE );
    m_pWithDrawBtn->SetVisibleSingle( TRUE );
}

void CWbClubStorageTab::ClubStorageUpdateLog()
{
    m_pStorageUseRecordPage->ClubStorageUpdateLog();
}

void CWbClubStorageTab::ClearInfo()
{
    m_pProfitsText->ClearText();
    m_pKeepMoneyText->ClearText();
}
