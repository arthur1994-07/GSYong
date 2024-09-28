#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Item/ItemDurability.h"
#include "../../../RanLogic/RandomOption/RandomOption.h"
#include "../../../RanLogic/ItemPeriodExtension/ItemPeriodExtension.h"

//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "./ItemInfoTooltip.h"
#include "../../RanLogic/Item/ItemSet/GLItemSet.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float fMOUSEPOINT_GAP1 = 20.0f;
const float fReSizeOffset1 = 10.0f;
const float fResizeEndOffset1 = 8.0f;
const float fResistOffset1 = 10.0f;
const float FResitOffset2 = 0.0f;
const float fBoxItemOffset = 5.0f;

CItemInfoTooltip::CItemInfoTooltip( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIGroup(pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
	, m_pCharacter(NULL)
	, m_bLinkBoxStyle(false)
	, m_bDisplayResist(false)
    , m_bGroup(false)
	, m_pItemData(NULL)
	, m_pLineBox(NULL)
	, m_pDummyBase(NULL)
	, m_pDummyControl(NULL)
	, m_pDummyNameControl(NULL)
	, m_pDummyLabel5Control(NULL)

	, m_pItemImage(NULL)
	, m_pItemLabel1(NULL)
	, m_pItemLabel2(NULL)
	, m_pItemLabel3(NULL)
	, m_pItemLabel4(NULL)
	, m_pItemLabel5(NULL)
	, m_pItemSet(NULL)
	, m_pItemLabel6(NULL)

	, m_pWishListLine(NULL)
	, m_pWishListItem(NULL)

	, m_pResistSpirit(NULL)
	, m_pResistFire(NULL)
	, m_pResistIce(NULL)
	, m_pResistElectric(NULL)
	, m_pResistPoison(NULL)
	, m_pResistSpiritText(NULL)
	, m_pResistFireText(NULL)
	, m_pResistIceText(NULL)
	, m_pResistElectricText(NULL)
	, m_pResistPoisonText(NULL)
	, m_vMousePointGap(fMOUSEPOINT_GAP1, fMOUSEPOINT_GAP1)
	, m_pLink_LineBox(NULL)
	, m_pLink_Close(NULL)

	, m_PosX( 0 )
	, m_PosY( 0 )
	, m_vDragPos( 0.0f, 0.0f )
	, m_bBlockMousetracking( false )

	, m_nMouseX( 0 ) 
	, m_nMouseY( 0 )
    , m_fDurabilityPerformance(1.0f)
	, m_pResistTextGap( 0.0f )
	, m_iPageIndex( -1 )
	, m_pColorBox(NULL)
	, m_bIsDurableTip( false )
{
    for ( int i = 0; i < ITEM::SBOX::RANDOM_ITEM_SIZE; i++ )
        m_pBoxItemImages[i] = NULL;

	for ( int i = 0; i < EXCHANGE_ITEM_SIZE; i++ )
		m_pExchangeItemImages[i] = NULL;

	for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i)
		m_fDurabilityValue[i] = 1.0f;
}

CItemInfoTooltip::~CItemInfoTooltip()
{
}

void CItemInfoTooltip::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "ITEM_INFOR_TOOLTIP_RN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE_RN" );
        RegisterControl( pLinBoxSmart );
        m_pLineBox = pLinBoxSmart;
		m_pLineBox->SetDiffuseAlpha(180);
    }

	if ( m_pLink_LineBox )
	{
		m_pLineBox->SetVisibleSingle( FALSE );
	}

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, std::string(GetControlNameEx()).c_str() );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pDummyBase = pControl;

		pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_RN" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pDummyControl = pControl;

		pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_NAME_RN" );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pDummyNameControl = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pDummyLabel5Control = pControl;	
    }

    {
        CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
        pItemImage->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_IMAGE_RN", UI_FLAG_DEFAULT, ITEM_IMAGE );
        pItemImage->CreateSubControl();
        RegisterControl( pItemImage );
        m_pItemImage = pItemImage;
    }

    {
        CD3DFontPar* pFont12 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 12, D3DFONT_BOLD | D3DFONT_SHADOW_EX | D3DFONT_KSC5601 );

        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_NAME_RN" );
        pBasicTextBox->SetFont( pFont12 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pBasicTextBox );
        m_pItemName = pBasicTextBox;
    }

    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL1_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pItemLabel1 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pItemLabel2 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pItemLabel3 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pItemLabel4 = pBasicTextBox;
    }
	
	{
		CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pItemSet = pBasicTextBox;
	}

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pItemLabel5 = pBasicTextBox;
    }

	{
		CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pItemLabel6 = pBasicTextBox;
	}


	{
		CUIControl* pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, GetControlNameEx().c_str() );
		pControl->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
		pControl->SetUseRender( true );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pWishListLine = pControl;

		CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL2_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pWishListItem = pBasicTextBox;
	}

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_SPIRIT_RN" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pResistSpirit = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_FIRE_RN" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pResistFire = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ICE_RN" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pResistIce = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ELECTRIC_RN" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pResistElectric = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_POISON_RN" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pResistPoison = pControl;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_SPIRIT_TEXT_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pResistSpiritText = pBasicTextBox;

        pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_FIRE_TEXT_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pResistFireText = pBasicTextBox;

        pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ICE_TEXT_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pResistIceText = pBasicTextBox;

        pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ELECTRIC_TEXT_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pResistElectricText = pBasicTextBox;

        pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_POISON_TEXT_RN" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pResistPoisonText = pBasicTextBox;
    }

    {
        for ( int i = 0; i < ITEM::SBOX::RANDOM_ITEM_SIZE; i++ )
        {
            CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
            pItemImage->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_IMAGE_RN", UI_FLAG_DEFAULT, BOX_ITEM_IMAGES_START+i );
            pItemImage->CreateSubControl();
            RegisterControl( pItemImage );
            m_pBoxItemImages[i] = pItemImage;
        }

		for ( int i = 0; i < EXCHANGE_ITEM_SIZE; i++ )
		{
			CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
			pItemImage->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_IMAGE_RN", UI_FLAG_DEFAULT, EXCHANGE_ITEM_IMAGE_START+i );
			pItemImage->CreateSubControl();
			RegisterControl( pItemImage );
			m_pExchangeItemImages[i] = pItemImage;
		}
    }

	/*
	{		
		m_pColorBox = new CUIControl(m_pEngineDevice);
		m_pColorBox->CreateSub ( this, "ITEM_MOUSE_OVER" );
		m_pColorBox->SetDiffuse( NS_UITEXTCOLOR::GREEN );
		m_pColorBox->SetVisibleSingle( FALSE );
		RegisterControl ( m_pColorBox );
	}
	*/
}

void CItemInfoTooltip::CreateLinkBoxStyle()
{
	m_bLinkBoxStyle = true;

	m_pLink_LineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	m_pLink_LineBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_RN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pLink_LineBox->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE_RN" );
	RegisterControl( m_pLink_LineBox );
	m_pLink_LineBox->SetDiffuseAlpha(180);

	CreateCloseButton();
}

void CItemInfoTooltip::CreateCloseButton()
{
	m_pLink_Close = new CBasicButton( m_pEngineDevice );	
	m_pLink_Close->CreateSub( this, "ITEM_INFOR_TOOLTIP_CLOSEBUTTON_RN", UI_FLAG_RIGHT, CLOSE_BUTTON );
	m_pLink_Close->CreateFlip( "INFO_DISPLAY_LINK_CLOSE_F", CBasicButton::CLICK_FLIP );
	m_pLink_Close->CreateMouseOver( "INFO_DISPLAY_LINK_CLOSE_OVER" );
	RegisterControl( m_pLink_Close );
}

void CItemInfoTooltip::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_nMouseX = x;
	m_nMouseX = y;

	if( m_bBlockMousetracking )
	{
		if( m_PosX != x || m_PosY != y )
		{
			m_PosX = x;
			m_PosY = y;

			if( IsExclusiveSelfControl() )
			{
				SetGlobalPos( D3DXVECTOR2( x - m_vDragPos.x, y - m_vDragPos.y ) );
			}
		}
	}
	else
	{
		AllControlRePos( x, y );
	}
}

void CItemInfoTooltip::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case CLOSE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( m_pWishItem )
				{
					m_pInterface->CloseItemToolTipMulti( GetWndID() );
				}
				else if ( m_sItemcustom.GetNativeID().IsValidNativeID() )
				{
					m_pInterface->CloseItemToolTipMulti( GetWndID() );
				}

				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}

    for ( int i = BOX_ITEM_IMAGES_START; i <= BOX_ITEM_IMAGES_END; i++ )
    {

        if ( ControlID == i )
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                int nIndex = (i - BOX_ITEM_IMAGES_START);
                SITEMCUSTOM sCUSTOM( m_pBoxItemImages[nIndex]->GetItemID() );

				m_pInterface->SHOW_ITEM_INFO( sCUSTOM, FALSE, FALSE, FALSE, FALSE,FALSE, USHRT_MAX, USHRT_MAX );

				//m_pInterface->ShowItemToolTipMulti( sCUSTOM, m_nMouseX, m_nMouseY );
            }
        }
    }

	for ( int i = EXCHANGE_ITEM_IMAGE_START; i <= EXCHANGE_ITEM_IMAGE_END; i++ )
	{

		if ( ControlID == i )
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nIndex = (i - EXCHANGE_ITEM_IMAGE_START);
				SITEMCUSTOM sCUSTOM( m_pExchangeItemImages[nIndex]->GetItemID() );

				m_pInterface->SHOW_ITEM_INFO( sCUSTOM, FALSE, FALSE, FALSE, FALSE,FALSE, USHRT_MAX, USHRT_MAX );

			}
		}
	}
}

void CItemInfoTooltip::TranslateMessage( DWORD dwMsg )
{
	if( m_bBlockMousetracking )
	{
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			if( dwMsg & UIMSG_LB_DOWN )
			{
				SetExclusiveControl();

				UIRECT rcPos = GetGlobalPos();
				m_vDragPos.x = m_PosX - rcPos.left;
				m_vDragPos.y = m_PosY - rcPos.top;
			}
			else if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				ResetExclusiveControl();
			}
		}
		else if( CHECK_LB_UP_LIKE( dwMsg ) )
		{
			ResetExclusiveControl();
		}
	}
}

void CItemInfoTooltip::AllControlRePos( int x, int y )
{
    const UIRECT& rcTextBoxLocalPos = m_pDummyLabel5Control->GetLocalPos();
    const UIRECT& rcLocalPosDummy = m_pDummyControl->GetLocalPos();
    const UIRECT& rcOriginPos = GetLocalPos();

    AlignSubControl( rcOriginPos, rcLocalPosDummy );

	UIRECT rcNewUIPos;

	float fOriginItemNameSizeX = m_pDummyNameControl->GetLocalPos().sizeX;
	long lItemNameLength = m_pItemName->GetLongestLine();
    if ( static_cast<long>(fOriginItemNameSizeX) < lItemNameLength )
    {
        // 아이템 이름 텍스트 박스 크기 설정.
        UIRECT rcNewItemName = m_pItemName->GetLocalPos();
        rcNewItemName.sizeX = static_cast<float>(lItemNameLength);
        m_pItemName->SetLocalPos( rcNewItemName );

        // 아이템 설명 텍스트 박스 크기 설정.
        UIRECT rcNewLabel5 = m_pDummyLabel5Control->GetLocalPos();
        rcNewLabel5.sizeX = static_cast<float>(lItemNameLength);
        m_pItemLabel5->SetLocalPos( rcNewLabel5 );

        // UI 크기 설정.
        rcNewUIPos = rcLocalPosDummy;
        rcNewUIPos.sizeX += lItemNameLength - fOriginItemNameSizeX;
    }
    else
    {
        // 아이템 이름 텍스트 박스 크기 설정.
        UIRECT rcDummyItemName = m_pDummyNameControl->GetLocalPos();
        m_pItemName->SetLocalPos( rcDummyItemName );

        // 아이템 설명 텍스트 박스 크기 설정.
        UIRECT rcNewLabel5 = m_pDummyLabel5Control->GetLocalPos();
        m_pItemLabel5->SetLocalPos( rcNewLabel5 );

        // UI 크기 설정.
        rcNewUIPos = rcLocalPosDummy;
    }

	bool bInterval = false;
    float fPosY = m_pItemLabel1->GetLocalPos().top;

    {
        int nTotalLine = m_pItemLabel1->GetTotalLine();
        const float fAllLine = m_pItemLabel1->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pItemLabel1->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pItemLabel1->SetLocalPos( rcLabelNewLP );
    }

    {
		if( m_bIsDurableTip )
		{
			fPosY += 30;
		}
        D3DXVECTOR2 vPos;
        vPos.x = m_pItemLabel2->GetLocalPos().left;
        vPos.y = fPosY;
        m_pItemLabel2->SetLocalPos( vPos );

        int nTotalLine = m_pItemLabel2->GetTotalLine();
        const float fAllLine = m_pItemLabel2->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pItemLabel2->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pItemLabel2->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

    {
        if ( bInterval && m_pItemLabel3->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset1;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pItemLabel3->GetLocalPos().left;
        vPos.y = fPosY;
        m_pItemLabel3->SetLocalPos( vPos );

        int nTotalLine = m_pItemLabel3->GetTotalLine();
        const float fAllLine = m_pItemLabel3->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pItemLabel3->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pItemLabel3->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

    {
        if ( bInterval && m_pItemLabel4->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset1;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pItemLabel4->GetLocalPos().left;
        vPos.y = fPosY;
        m_pItemLabel4->SetLocalPos( vPos );

        int nTotalLine = m_pItemLabel4->GetTotalLine();
        const float fAllLine = m_pItemLabel4->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pItemLabel4->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pItemLabel4->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }
	
	{
		if ( bInterval && m_pItemSet->GetTotalLine() )
		{
			bInterval = false;
			fPosY += fReSizeOffset1;
		}

		D3DXVECTOR2 vPos;
		vPos.x = m_pItemSet->GetLocalPos().left;
		vPos.y = fPosY;
		m_pItemSet->SetLocalPos( vPos );

		int nTotalLine = m_pItemSet->GetTotalLine();
		const float fAllLine = m_pItemSet->CalcMaxHEIGHT( 0, nTotalLine );
		fPosY += fAllLine;

		const UIRECT& rcLabelLP = m_pItemSet->GetLocalPos();
		UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
		m_pItemSet->SetLocalPos( rcLabelNewLP );

		if ( nTotalLine )
			bInterval = true;
	}

    {
        if ( bInterval && m_pItemLabel5->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset1;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pItemLabel5->GetLocalPos().left;
        vPos.y = fPosY;
        m_pItemLabel5->SetLocalPos( vPos );

        int nTotalLine = m_pItemLabel5->GetTotalLine();
        const float fAllLine = m_pItemLabel5->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pItemLabel5->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pItemLabel5->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
           bInterval = true;
    }
	
	if( m_pWishListItem->GetTotalLine() )
	{
		fPosY += fReSizeOffset1;

		UIRECT rcLine;
		rcLine.left = 5.0f;
		rcLine.top = fPosY;
		rcLine.sizeX = rcNewUIPos.sizeX - 10.0f;
		rcLine.sizeY = 1.0f;
		m_pWishListLine->SetLocalPos( rcLine );

		if( bInterval )
        {
            bInterval = false;
            fPosY += fReSizeOffset1;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pWishListItem->GetLocalPos().left;
        vPos.y = fPosY;
        m_pWishListItem->SetLocalPos( vPos );

        int nTotalLine = m_pWishListItem->GetTotalLine();
        const float fAllLine = m_pWishListItem->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pWishListItem->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pWishListItem->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
           bInterval = true;
	}

	
    // 박스 아이템 내 아이템 이미지들.
    {
        for ( int i = 0; i < ITEM::SBOX::RANDOM_ITEM_SIZE; i++ )
            m_pBoxItemImages[i]->SetVisibleSingle( FALSE );

        if ( m_pItemData /*&& m_iPageIndex == -1*/ )
        {
			const EMITEM_TYPE emType(m_pItemData->BasicType());
            if ( ((emType == ITEM_BOX) 
				|| (emType == ITEM_RANDOMITEM) 
				|| (emType == ITEM_LOCK_BOX)
				||  (emType == ITEM_SELECTIVEFORM_BOX)) &&
                m_pItemData->sBox.ShowContents )
            {
                // TODO :  calc position.
				size_t nLineLimit = 10;
                bool bFirst = true;
                int ItemIndex = 0;
                D3DXVECTOR2 vPos;
                vPos.x = m_pItemLabel5->GetLocalPos().left + fBoxItemOffset;
                vPos.y = fPosY+fBoxItemOffset;

                if ( (emType == ITEM_RANDOMITEM) 
					|| (emType == ITEM_LOCK_BOX) )
                {
                    for ( size_t i = 0; i < m_pItemData->sRandomBox.vecBOX.size(); i++ )
                    {
                        SITEMCUSTOM ItemCustomInBox( m_pItemData->sRandomBox.vecBOX[i].nidITEM );

                        if ( ItemCustomInBox.GetNativeID() == NATIVEID_NULL() )
                            continue;

                        const UIRECT& rcItemImage = m_pBoxItemImages[0]->GetLocalPos();
                        const float fItemImageSizeX = m_pBoxItemImages[0]->GetLocalPos().sizeX;
                        const float fItemImageSizeY = m_pBoxItemImages[0]->GetLocalPos().sizeY;

						if ( i%nLineLimit == 0 && i != 0 )
						{
							vPos.y += (fItemImageSizeY + fBoxItemOffset);
							vPos.x -= ((fItemImageSizeX + fBoxItemOffset)*(nLineLimit - 1));
							bFirst = true;
						}

                        if ( bFirst )
                        {
                            fPosY += (fItemImageSizeY + fBoxItemOffset);
                            bFirst = false;
                        }
                        else
						{
                            vPos.x += (fItemImageSizeX + fBoxItemOffset);
						}

                        m_pBoxItemImages[ItemIndex]->SetLocalPos( vPos );
                        m_pBoxItemImages[ItemIndex]->SetVisibleSingle( TRUE );

                        m_pBoxItemImages[ItemIndex]->ResetItem();
                        m_pBoxItemImages[ItemIndex++]->SetItem( ItemCustomInBox );

                        if ( ItemIndex >= ITEM::SBOX::RANDOM_ITEM_SIZE )
                            break;
                    }
					/*
					if( nLineLimit <= m_pItemData->sRandomBox.vecBOX.size() )
					{
						vPos.x = m_pItemLabel5->GetLocalPos().left + fBoxItemOffset; 
						vPos.x += ((m_pBoxItemImages[0]->GetLocalPos().sizeX+fBoxItemOffset)*(nLineLimit-1));
					}
					*/
                }
                else if ( emType == ITEM_BOX 
					|| (emType == ITEM_SELECTIVEFORM_BOX) )
                {
					int nSize = min(ITEM::SBOX::ITEM_SIZE, (int)m_pItemData->sBox.vecItems.size());
                    for ( int i = 0; i < nSize; i++ )
                    {
                        SITEMCUSTOM ItemCustomInBox( m_pItemData->sBox.vecItems[i].nidITEM );
                        if ( ItemCustomInBox.GetNativeID() == NATIVEID_NULL() )
                            continue;

                        const UIRECT& rcItemImage = m_pBoxItemImages[0]->GetLocalPos();
                        const float fItemImageSizeX = m_pBoxItemImages[0]->GetLocalPos().sizeX;
                        const float fItemImageSizeY = m_pBoxItemImages[0]->GetLocalPos().sizeY;

						if ( i%nLineLimit == 0 && i != 0 )
						{
							vPos.y += (fItemImageSizeY + fBoxItemOffset);
							vPos.x -= ((fItemImageSizeX + fBoxItemOffset)*(nLineLimit - 1));
							bFirst = true;
						}

                        if ( bFirst )
                        {
                            fPosY += (fItemImageSizeY+fBoxItemOffset);
                            bFirst = false;
                        }
                        else
                            vPos.x += (fItemImageSizeX + fBoxItemOffset);

                        m_pBoxItemImages[ItemIndex]->SetLocalPos( vPos );
                        m_pBoxItemImages[ItemIndex]->SetVisibleSingle( TRUE );

                        m_pBoxItemImages[ItemIndex]->ResetItem();
                        m_pBoxItemImages[ItemIndex++]->SetItem( ItemCustomInBox );

                        if ( ItemIndex >= ITEM::SBOX::ITEM_SIZE )
                            break;
                    }
                }

				if( nLineLimit <= ItemIndex )
				{
					vPos.x = m_pItemLabel5->GetLocalPos().left + fBoxItemOffset; 
					vPos.x += ((m_pBoxItemImages[0]->GetLocalPos().sizeX+fBoxItemOffset)*(nLineLimit-1));
				}

                vPos.x += (m_pBoxItemImages[0]->GetLocalPos().sizeX + fBoxItemOffset);
                if ( rcNewUIPos.sizeX < vPos.x )
                    rcNewUIPos.sizeX = vPos.x;
            }
        }
		//else

		{
			if ( bInterval && m_pItemLabel6->GetTotalLine() )
			{
				bInterval = false;
				fPosY += fReSizeOffset1;
			}

			D3DXVECTOR2 vPos;
			vPos.x = m_pItemLabel6->GetLocalPos().left;
			vPos.y = fPosY;
			m_pItemLabel6->SetLocalPos( vPos );

			int nTotalLine = m_pItemLabel6->GetTotalLine();
			const float fAllLine = m_pItemLabel6->CalcMaxHEIGHT( 0, nTotalLine );
			fPosY += fAllLine;

			const UIRECT& rcLabelLP = m_pItemLabel6->GetLocalPos();
			UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
			m_pItemLabel6->SetLocalPos( rcLabelNewLP );

			if ( nTotalLine )
				bInterval = true;
		}

		for ( int i = 0; i < EXCHANGE_ITEM_SIZE; i++ )
			m_pExchangeItemImages[i]->SetVisibleSingle( FALSE );

		if ( m_pItemData && m_iPageIndex >= 0 )
		{			
			// TODO :  calc position.
			size_t nLineLimit = 10;
			bool bFirst = true;
			int ItemIndex = 0;
			D3DXVECTOR2 vPos;
			vPos.x = m_pItemLabel5->GetLocalPos().left + fBoxItemOffset;
			vPos.y = fPosY+fBoxItemOffset;

			const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcNativeID );

			if ( pCrowData )
			{
				GLInventorySale* rInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven( m_iPageIndex ));
				
				std::vector<sExNeed> rList;	
				
				EEXTYPE exType = rInven->GetExchangeList(m_pItemData->sBasicOp.GetNativeID(), m_wPosX, m_wPosY, rList );

				std::vector<sExNeed>::iterator _iter = rList.begin();

				int iRefCnt = 0;

				while( _iter != rList.end() && exType == EX_ITEM_TYPE)
				{
					SITEMCUSTOM ItemCustomInBox( _iter->sNativeID );

					if ( ItemCustomInBox.GetNativeID() != NATIVEID_NULL() )
					{
						const UIRECT& rcItemImage   = m_pExchangeItemImages[0]->GetLocalPos();
						const float fItemImageSizeX = m_pExchangeItemImages[0]->GetLocalPos().sizeX;
						const float fItemImageSizeY = m_pExchangeItemImages[0]->GetLocalPos().sizeY;

						if ( bFirst )
						{
							fPosY += (fItemImageSizeY+fBoxItemOffset);
							bFirst = false;
						}
						else
							vPos.x += (fItemImageSizeX + fBoxItemOffset);

						m_pExchangeItemImages[iRefCnt]->SetLocalPos( vPos );
						m_pExchangeItemImages[iRefCnt]->SetVisibleSingle( TRUE );

						m_pExchangeItemImages[iRefCnt]->ResetItem();
						m_pExchangeItemImages[iRefCnt]->SetItem( ItemCustomInBox );
						m_pExchangeItemImages[iRefCnt]->SetNumber(_iter->iNeedCount, 0 );

						if ( iRefCnt >= ITEM::SBOX::ITEM_SIZE )
							break;

						++iRefCnt;
					}
					
					++_iter;
				}

			}
						
			vPos.x += (m_pExchangeItemImages[0]->GetLocalPos().sizeX + fBoxItemOffset);
			if ( rcNewUIPos.sizeX < vPos.x )
				rcNewUIPos.sizeX = vPos.x;
			
		}
    }



    fPosY += fResizeEndOffset1;

    UIRECT rcLocalNewPos;

    if ( m_bLinkBoxStyle )
    {
        float fAddSizeX = 0.0f;

        if ( m_pLink_Close )
        {
            const UIRECT& rcRect = m_pLink_Close->GetLocalPos();
            fAddSizeX += rcRect.sizeX + 1;
        }

        rcLocalNewPos = UIRECT(
			float(x), float(y),
			rcNewUIPos.sizeX + fAddSizeX, fPosY );
    }
    else
    {
        rcLocalNewPos = UIRECT(
			float(x) + m_vMousePointGap.x, float(y) + m_vMousePointGap.y,
            rcNewUIPos.sizeX, fPosY );
    }

	if ( m_bDisplayResist )
	{
		// 위치 설정.
		{
			const UIRECT& rcLabel3LP = m_pItemLabel3->GetLocalPos();
			const float fResistSizeX = m_pResistFire->GetLocalPos().sizeX + 1.0f;
			const float fResistSizeY = m_pResistFire->GetLocalPos().sizeY;

			int nTotalLine = m_pItemLabel3->GetTotalLine() - 2;
			const float fAllLine = m_pItemLabel3->CalcMaxHEIGHT( 0, nTotalLine );
			
			D3DXVECTOR2 vPos;
			//vPos.x = fResistOffset1 * 6 - fResistSizeX * 10;
			vPos.x = rcLabel3LP.left - 10.0f;
			vPos.y = rcLabel3LP.top + fAllLine + 1.0f;

			vPos.x += fResistOffset1;
            vPos.y += FResitOffset2;
			m_pResistSpirit->SetLocalPos( vPos );
			vPos.x += fResistSizeX;
            vPos.y -= FResitOffset2;
			m_pResistSpiritText->SetLocalPos( vPos );
			vPos.x += fResistSizeX + m_pResistTextGap;

			vPos.x += fResistOffset1;
            vPos.y += FResitOffset2;
			m_pResistFire->SetLocalPos( vPos );
			vPos.x += fResistSizeX;
            vPos.y -= FResitOffset2;
			m_pResistFireText->SetLocalPos( vPos );
			vPos.x += fResistSizeX + m_pResistTextGap;

			vPos.x += fResistOffset1;
            vPos.y += FResitOffset2;
			m_pResistIce->SetLocalPos( vPos );
			vPos.x += fResistSizeX;
            vPos.y -= FResitOffset2;
			m_pResistIceText->SetLocalPos( vPos );
			vPos.x += fResistSizeX + m_pResistTextGap;

			vPos.x += fResistOffset1;
            vPos.y += FResitOffset2;
			m_pResistElectric->SetLocalPos( vPos );
			vPos.x += fResistSizeX;
            vPos.y -= FResitOffset2;
			m_pResistElectricText->SetLocalPos( vPos );
			vPos.x += fResistSizeX + m_pResistTextGap;

			vPos.x += fResistOffset1;
            vPos.y += FResitOffset2;
			m_pResistPoison->SetLocalPos( vPos );
			vPos.x += fResistSizeX;
            vPos.y -= FResitOffset2;
			m_pResistPoisonText->SetLocalPos( vPos );
			vPos.x += fResistSizeX;
		}

		m_pResistSpirit->SetVisibleSingle( TRUE );
		m_pResistFire->SetVisibleSingle( TRUE );
		m_pResistIce->SetVisibleSingle( TRUE );
		m_pResistElectric->SetVisibleSingle( TRUE );
		m_pResistPoison->SetVisibleSingle( TRUE );
	}

    AlignSubControl( rcLocalPosDummy, rcLocalNewPos );

    SetGlobalPos( rcLocalNewPos );
}

void CItemInfoTooltip::DisplayItemDurability( const SITEMCUSTOM& sItemCustom,
													 const BOOL bShopOpen, const BOOL bInMarket,
													 const BOOL bInPrivateMarket, const BOOL bUsePreview,
													 const BOOL bIsWEAR_ITEM, const BOOL bIsDurable, WORD wPosX, WORD wPosY,
													 SNATIVEID sNpcNativeID, int iPageIndex )
{
	if( !m_pGaeaClient )
		return;

	m_pCharacter = m_pGaeaClient->GetCharacter();
	if ( !m_pCharacter )
		return;

	// 포장되어진 물건은 툴팁표시를 위해 포장을 임시로 푼다.;
	SITEMCUSTOM sBoxWrappedItemCustom = sItemCustom;
	sBoxWrappedItemCustom.BoxUnwrapping();

	m_pItemData = GLogicData::GetInstance().GetItem( sBoxWrappedItemCustom.GetNativeID() );
	if ( !m_pItemData )
		return;

	m_bShopOpen = (0!=bShopOpen);
	m_bInMarket = (0!=bInMarket);
	m_bInPrivateMarket = (0!= bInPrivateMarket);
	m_bUsePreview = (0!=bUsePreview);
	m_bIsWEAR_ITEM = 0;
	m_wPosX = wPosX;
	m_wPosY = wPosY;
	m_sNpcNativeID = sNpcNativeID;
	m_iPageIndex = iPageIndex;

	m_bIsDurableTip = bIsDurable;

	// 아이템 내구도
	if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
	{	
		ResetDurabilityValue();

		if( m_bIsDurableTip )
		{
			m_fDurabilityPerformance = ItemDurability::GetInstance().GetPerformance(sBoxWrappedItemCustom);

			for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
			{
				if( GLCONST_CHAR::nDurabilityApply[i] ) m_fDurabilityValue[i] = m_fDurabilityPerformance;
			}
		}
		else
		{			
			m_fDurabilityPerformance = 1.f;
		}

	}

	AllControlRePos((int)wPosX, (int)wPosY);

	{
		SetItemImage( sItemCustom );

		if( m_bIsDurableTip ) 
			SetItemName( ID2GAMEWORD("DURABILITY_ITEM", 0) );
		else
			SetItemName( sItemCustom );

	}

	if( !m_bIsDurableTip )
		SetInformationLabel1( sBoxWrappedItemCustom );

	if( sBoxWrappedItemCustom.IsCSApplied() )
	{
		//ShowColorBox();
		SetInformationLabel2_cs( sBoxWrappedItemCustom );//기본, 랜덤 옵션
		SetInformationLabel3_cs( sBoxWrappedItemCustom );//가산효과
	}
	else
	{
		//HideColorBox();
		SetInformationLabel2( sBoxWrappedItemCustom );//기본, 랜덤 옵션
		SetInformationLabel3( sBoxWrappedItemCustom );//가산효과
	}

	if( !m_bIsDurableTip )
	{
		SetInformationLabel4( sBoxWrappedItemCustom );
		SetInformationLabelSetItem( sBoxWrappedItemCustom );
		SetInformationLabel5( sItemCustom );
		SetInformationLabel6( sItemCustom );

		DisplayWishListInfo();
	}	

	AllControlRePos((int)wPosX, (int)wPosY);	
}

void CItemInfoTooltip::DisplayItemInformation( const SITEMCUSTOM& sItemCustom,
                                               const BOOL bShopOpen, const BOOL bInMarket,
                                               const BOOL bInPrivateMarket, const BOOL bUsePreview,
                                               const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY,
                                               SNATIVEID sNpcNativeID, int iPageIndex )
{
    if( !m_pGaeaClient )
		return;

	m_pCharacter = m_pGaeaClient->GetCharacter();
    if ( !m_pCharacter )
		return;
	
    // 포장되어진 물건은 툴팁표시를 위해 포장을 임시로 푼다.;
    SITEMCUSTOM sBoxWrappedItemCustom = sItemCustom;
    sBoxWrappedItemCustom.BoxUnwrapping();

    m_pItemData = GLogicData::GetInstance().GetItem( sBoxWrappedItemCustom.GetNativeID() );
    if ( !m_pItemData )
		return;

    m_bShopOpen = (0!=bShopOpen);
    m_bInMarket = (0!=bInMarket);
    m_bInPrivateMarket = (0!= bInPrivateMarket);
    m_bUsePreview = (0!=bUsePreview);
    m_bIsWEAR_ITEM = (0!=bIsWEAR_ITEM);
    m_wPosX = wPosX;
    m_wPosY = wPosY;
    m_sNpcNativeID = sNpcNativeID;
	m_iPageIndex = iPageIndex;

	m_bIsDurableTip = false;
	
    // 아이템 내구도
    if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
	{		
        m_fDurabilityPerformance = ItemDurability::GetInstance().GetPerformance(sBoxWrappedItemCustom);

		if( m_pItemData->sBasicOp.emItemType == ITEM_SUIT &&
			m_pItemData->sSuitOp.emSuit == SUIT_HANDHELD )
		{
			m_fDurabilityPerformance = 1.f;
		}

		ResetDurabilityValue(m_fDurabilityPerformance);
	}
 
	AllControlRePos((int)wPosX, (int)wPosY);

    {// 이미지와 이름에 한해서는 포장박스상태의 ItemCustom 정보를 필요로 한다;.
        SetItemImage( sItemCustom );
        SetItemName( sItemCustom );
    }
    SetInformationLabel1( sBoxWrappedItemCustom );

	if( sBoxWrappedItemCustom.IsCSApplied() )
	{
		//ShowColorBox();
		SetInformationLabel2_cs( sBoxWrappedItemCustom );//기본, 랜덤 옵션
		SetInformationLabel3_cs( sBoxWrappedItemCustom );//가산효과
	}
	else
	{
		//HideColorBox();
		SetInformationLabel2( sBoxWrappedItemCustom );//기본, 랜덤 옵션
		SetInformationLabel3( sBoxWrappedItemCustom );//가산효과
	}

    SetInformationLabel4( sBoxWrappedItemCustom );
	SetInformationLabelSetItem( sBoxWrappedItemCustom );

    {// Label5에 한해서는 포장박스상태의 ItemCustom 정보를 필요로 한다;.
        SetInformationLabel5( sItemCustom );
    }

	SetInformationLabel6( sItemCustom );

	DisplayWishListInfo();
        
    AllControlRePos((int)wPosX, (int)wPosY);	
}

void CItemInfoTooltip::SetItemImage( const SITEMCUSTOM& sItemCustom )
{
    m_pItemImage->ResetItem();

    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
    if ( !pItemData )
        return;
    m_pItemImage->SetItem( sItemCustom );
}

void CItemInfoTooltip::SetItemName( const SITEMCUSTOM& sItemCustom )
{
    m_pItemName->SetTextNoSplit( sItemCustom.GETNAME().c_str(), ITEM_INFOR_COLOR::dwItemRank[ m_pItemData->sBasicOp.emLevel ] );
    //m_pItemName->AddText( strItemName.c_str(), ITEM_INFOR_COLOR::dwItemRank[ m_pItemData->sBasicOp.emLevel ] );
}

void CItemInfoTooltip::SetItemName( const char* pName )
{
	if( pName == NULL ) return;

	m_pItemName->SetTextNoSplit( pName, ITEM_INFOR_COLOR::dwItemRank[ m_pItemData->sBasicOp.emLevel ] );
}

void CItemInfoTooltip::SetInformationLabel1( const SITEMCUSTOM& sItemCustom )
{
    int nIndex;

    nIndex = m_pItemLabel1->AddText( "", ITEM_INFOR_COLOR::dwDefault );

    DWORD dwColor;

    // Info : 학원
    {
        if ( m_pItemData->sBasicOp.dwReqSchool==GLSCHOOL_ALL )
        {
            m_pItemLabel1->AddString( nIndex, ID2GAMEWORD( "ACADEMY_NAME", 3 ), ITEM_INFOR_COLOR::dwDefault );
            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
        }
        else
        {
            if ( m_pItemData->sBasicOp.dwReqSchool & index2school(m_pCharacter->m_wSchool) )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if ( m_pItemData->sBasicOp.dwReqSchool & GLSCHOOL_00)
            {
                m_pItemLabel1->AddString( nIndex,
                    GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_00)].c_str(),
                    dwColor );

                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }
            if ( m_pItemData->sBasicOp.dwReqSchool & GLSCHOOL_01)
            {
                m_pItemLabel1->AddString( nIndex,
                    GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_01)].c_str(),
                    dwColor );

                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }
            if ( m_pItemData->sBasicOp.dwReqSchool & GLSCHOOL_02)
            {
                m_pItemLabel1->AddString( nIndex,
                    GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_02)].c_str(),
                    dwColor );

                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }
        }
    }

    // Info : 착용 클래스
    {
        if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
        {
            const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
            if ( !pItemDisguise )   return;

            DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
            DWORD dwReqCharClass = m_pItemData->sBasicOp.dwReqCharClass;

            if ( (dwReqCharClass&m_pCharacter->m_emClass) && (dwReqCharClass_Disguise&m_pCharacter->m_emClass) )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if( dwReqCharClass )
            {
                if ( (dwReqCharClass==GLCC_ALL_TRICKER) && (dwReqCharClass_Disguise==GLCC_ALL_TRICKER)
                    || (dwReqCharClass == GLCC_MAN) && (dwReqCharClass_Disguise == GLCC_MAN)
                    || (dwReqCharClass == GLCC_WOMAN) && (dwReqCharClass_Disguise == GLCC_WOMAN) )
                {
                    m_pItemLabel1->AddString( nIndex, ID2GAMEWORD("ITEM_ALL_CLASS"), ITEM_INFOR_COLOR::dwDefault );
                }
                else
                {
                    bool bDistinction = false;

                    if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
                        || (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass&GLCC_ARMS_M)  && (dwReqCharClass_Disguise&GLCC_ARMS_M)
                        || (dwReqCharClass&GLCC_ARMS_W)  && (dwReqCharClass_Disguise&GLCC_ARMS_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass&GLCC_ARCHER_M)  && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
                        || (dwReqCharClass&GLCC_ARCHER_W)  && (dwReqCharClass_Disguise&GLCC_ARCHER_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS",
                            CharClassToClassIndex( GLCC_ARCHER_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass&GLCC_SPIRIT_M)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
                        || (dwReqCharClass&GLCC_SPIRIT_W)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass&GLCC_EXTREME_M)  && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
                        || (dwReqCharClass&GLCC_EXTREME_W)  && (dwReqCharClass_Disguise&GLCC_EXTREME_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass&GLCC_SCIENTIST_M)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
                        || (dwReqCharClass&GLCC_SCIENTIST_W)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass&GLCC_ASSASSIN_M)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
                        || (dwReqCharClass&GLCC_ASSASSIN_W)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
                            dwColor );
                    }

					if ( (dwReqCharClass&GLCC_TRICKER_M)  && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
						|| (dwReqCharClass&GLCC_TRICKER_W)  && (dwReqCharClass_Disguise&GLCC_TRICKER_W) )
					{
						if ( bDistinction )
							m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
						bDistinction = true;

						m_pItemLabel1->AddString( nIndex,
							ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
							dwColor );
					}

                    if ( (dwReqCharClass&GLCC_ETC_M)  && (dwReqCharClass_Disguise&GLCC_ETC_M)
                        || (dwReqCharClass&GLCC_ETC_W)  && (dwReqCharClass_Disguise&GLCC_ETC_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
                            dwColor );
                    }
                }
            }
        }
        else
        {
            DWORD dwReqCharClass = m_pItemData->sBasicOp.dwReqCharClass;

            if ( (dwReqCharClass&m_pCharacter->m_emClass) )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if( dwReqCharClass )
            {
                if ( dwReqCharClass==GLCC_ALL_TRICKER
                    || (dwReqCharClass == GLCC_MAN)
                    || (dwReqCharClass == GLCC_WOMAN) )
                {
                    m_pItemLabel1->AddString( nIndex, ID2GAMEWORD("ITEM_ALL_CLASS"), ITEM_INFOR_COLOR::dwDefault );
                }
                else
                {
                    bool bDistinction = false;
                    if ( (dwReqCharClass & GLCC_FIGHTER_M) 
                        || (dwReqCharClass & GLCC_FIGHTER_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass & GLCC_ARMS_M) 
                        || (dwReqCharClass & GLCC_ARMS_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass & GLCC_ARCHER_M)
                        || (dwReqCharClass & GLCC_ARCHER_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARCHER_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass & GLCC_SPIRIT_M)
                        || (dwReqCharClass & GLCC_SPIRIT_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ) ),
                            dwColor );
                    }
                    if ( (dwReqCharClass & GLCC_EXTREME_M)
                        || (dwReqCharClass & GLCC_EXTREME_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass & GLCC_SCIENTIST_M)
                        || (dwReqCharClass & GLCC_SCIENTIST_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) ),
                            dwColor );
                    }

                    if ( (dwReqCharClass & GLCC_ASSASSIN_M)
                        || (dwReqCharClass & GLCC_ASSASSIN_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
                            dwColor );
                    }

					if ( (dwReqCharClass & GLCC_TRICKER_M)
						|| (dwReqCharClass & GLCC_TRICKER_W) )
					{
						if ( bDistinction )
							m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
						bDistinction = true;

						m_pItemLabel1->AddString( nIndex,
							ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
							dwColor );
					}

                    if ( (dwReqCharClass & GLCC_ETC_M)
                        || (dwReqCharClass & GLCC_ETC_W) )
                    {
                        if ( bDistinction )
                            m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        bDistinction = true;

                        m_pItemLabel1->AddString( nIndex,
                            ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
                            dwColor );
                    }
                }
            }
        }
    }

    // Info : 성별
    {
        if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
        {
            const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
            if ( !pItemDisguise )   return;

            DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
            DWORD dwReqCharClass = m_pItemData->sBasicOp.dwReqCharClass;

            if ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) || (dwReqCharClass_Disguise!=GLCC_ALL_SCIENTIST) )
            {
                bool bMan = false;
                bool bWoman = false;

                if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
                    || (dwReqCharClass&GLCC_ARMS_M) && (dwReqCharClass_Disguise&GLCC_ARMS_M)
                    || (dwReqCharClass&GLCC_ARCHER_M) && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
                    || (dwReqCharClass&GLCC_SPIRIT_M) && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
                    || (dwReqCharClass&GLCC_EXTREME_M) && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
                    || (dwReqCharClass&GLCC_SCIENTIST_M) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
                    || (dwReqCharClass&GLCC_ASSASSIN_M) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
					|| (dwReqCharClass&GLCC_TRICKER_M) && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
                    || (dwReqCharClass&GLCC_ETC_M) && (dwReqCharClass_Disguise&GLCC_ETC_M) )
                {
                    bMan = true;
                }

                if ( (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W)
                    || (dwReqCharClass&GLCC_ARMS_W) && (dwReqCharClass_Disguise&GLCC_ARMS_W)
                    || (dwReqCharClass&GLCC_ARCHER_W) && (dwReqCharClass_Disguise&GLCC_ARCHER_W)
                    || (dwReqCharClass&GLCC_SPIRIT_W) && (dwReqCharClass_Disguise&GLCC_SPIRIT_W)
                    || (dwReqCharClass&GLCC_EXTREME_W) && (dwReqCharClass_Disguise&GLCC_EXTREME_W)
                    || (dwReqCharClass&GLCC_SCIENTIST_W) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W)
                    || (dwReqCharClass&GLCC_ASSASSIN_W) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W)
					|| (dwReqCharClass&GLCC_TRICKER_W) && (dwReqCharClass_Disguise&GLCC_TRICKER_W)
                    || (dwReqCharClass&GLCC_ETC_W) && (dwReqCharClass_Disguise&GLCC_ETC_W) )
                {
                    bWoman = true;
                }

                if ( !bMan || !bWoman )
                {
                    // 남, 여 모두 착용 가능하지 않을 때.
                    if ( bMan )
                    {
                        // 남자 착용 가능 할 때.
                        if ( (m_pCharacter->m_emClass & GLCC_FIGHTER_M) 
                            || (m_pCharacter->m_emClass & GLCC_ARMS_M)
                            || (m_pCharacter->m_emClass & GLCC_ARCHER_M)
                            || (m_pCharacter->m_emClass & GLCC_SPIRIT_M)
                            || (m_pCharacter->m_emClass & GLCC_EXTREME_M)
                            || (m_pCharacter->m_emClass & GLCC_SCIENTIST_M)
                            || (m_pCharacter->m_emClass & GLCC_ASSASSIN_M)
							|| (m_pCharacter->m_emClass & GLCC_TRICKER_M)
                            || (m_pCharacter->m_emClass & GLCC_ETC_M) )
                            dwColor = ITEM_INFOR_COLOR::dwDefault;
                        else
                            dwColor = ITEM_INFOR_COLOR::dwReqWrong;

                        m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        m_pItemLabel1->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
                    }
                    else
                    {
                        // 여자 착용 가능 할 때.
                        if ( (m_pCharacter->m_emClass & GLCC_FIGHTER_W) 
                            || (m_pCharacter->m_emClass & GLCC_ARMS_W)
                            || (m_pCharacter->m_emClass & GLCC_ARCHER_W)
                            || (m_pCharacter->m_emClass & GLCC_SPIRIT_W)
                            || (m_pCharacter->m_emClass & GLCC_EXTREME_W)
                            || (m_pCharacter->m_emClass & GLCC_SCIENTIST_W)
                            || (m_pCharacter->m_emClass & GLCC_ASSASSIN_W)
							|| (m_pCharacter->m_emClass & GLCC_TRICKER_W)
                            || (m_pCharacter->m_emClass & GLCC_ETC_W) )
                            dwColor = ITEM_INFOR_COLOR::dwDefault;
                        else
                            dwColor = ITEM_INFOR_COLOR::dwReqWrong;

                        m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        m_pItemLabel1->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
                    }
                }
            }
        }
        else
        {
            DWORD dwReqCharClass = m_pItemData->sBasicOp.dwReqCharClass;

            if ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) )
            {
                bool bMan = false;
                bool bWoman = false;

                if ( (dwReqCharClass & GLCC_FIGHTER_M) 
                    || (dwReqCharClass & GLCC_ARMS_M)
                    || (dwReqCharClass & GLCC_ARCHER_M)
                    || (dwReqCharClass & GLCC_SPIRIT_M)
                    || (dwReqCharClass & GLCC_EXTREME_M)
                    || (dwReqCharClass & GLCC_SCIENTIST_M)
                    || (dwReqCharClass & GLCC_ASSASSIN_M)
					|| (dwReqCharClass & GLCC_TRICKER_M)
                    || (dwReqCharClass & GLCC_ETC_M) )
                {
                    bMan = true;
                }

                if ( (dwReqCharClass & GLCC_FIGHTER_W) 
                    || (dwReqCharClass & GLCC_ARMS_W)
                    || (dwReqCharClass & GLCC_ARCHER_W)
                    || (dwReqCharClass & GLCC_SPIRIT_W)
                    || (dwReqCharClass & GLCC_EXTREME_W)
                    || (dwReqCharClass & GLCC_SCIENTIST_W)
                    || (dwReqCharClass & GLCC_ASSASSIN_W)
					|| (dwReqCharClass & GLCC_TRICKER_W)
                    || (dwReqCharClass & GLCC_ETC_W) )
                {
                    bWoman = true;
                }

                if ( !bMan || !bWoman )
                {
                    // 남, 여 모두 착용 가능하지 않을 때.
                    if ( bMan )
                    {
                        // 남자 착용 가능 할 때.
                        if ( (m_pCharacter->m_emClass & GLCC_FIGHTER_M) 
                            || (m_pCharacter->m_emClass & GLCC_ARMS_M)
                            || (m_pCharacter->m_emClass & GLCC_ARCHER_M)
                            || (m_pCharacter->m_emClass & GLCC_SPIRIT_M)
                            || (m_pCharacter->m_emClass & GLCC_EXTREME_M)
                            || (m_pCharacter->m_emClass & GLCC_SCIENTIST_M)
                            || (m_pCharacter->m_emClass & GLCC_ASSASSIN_M)
							|| (m_pCharacter->m_emClass & GLCC_TRICKER_M)
                            || (m_pCharacter->m_emClass & GLCC_ETC_M) )
                            dwColor = ITEM_INFOR_COLOR::dwDefault;
                        else
                            dwColor = ITEM_INFOR_COLOR::dwReqWrong;

                        m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        m_pItemLabel1->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
                    }
                    else
                    {
                        // 여자 착용 가능 할 때.
                        if ( (m_pCharacter->m_emClass & GLCC_FIGHTER_W) 
                            || (m_pCharacter->m_emClass & GLCC_ARMS_W)
                            || (m_pCharacter->m_emClass & GLCC_ARCHER_W)
                            || (m_pCharacter->m_emClass & GLCC_SPIRIT_W)
                            || (m_pCharacter->m_emClass & GLCC_EXTREME_W)
                            || (m_pCharacter->m_emClass & GLCC_SCIENTIST_W)
                            || (m_pCharacter->m_emClass & GLCC_ASSASSIN_W)
							|| (m_pCharacter->m_emClass & GLCC_TRICKER_W)
                            || (m_pCharacter->m_emClass & GLCC_ETC_W) )
                            dwColor = ITEM_INFOR_COLOR::dwDefault;
                        else
                            dwColor = ITEM_INFOR_COLOR::dwReqWrong;

                        m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
                        m_pItemLabel1->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
                    }
                }
            }
        }
    }

    nIndex = m_pItemLabel1->AddText( "", ITEM_INFOR_COLOR::dwDefault );

    // Info : 요구 레벨
    bool bDisplayLevel = false;
    {
        if ( m_pItemData->sBasicOp.wReqLevelDW || m_pItemData->sBasicOp.wReqLevelUP )
        {
            bDisplayLevel = true;

            std::string strTemp = "";
            bool bReqLevel = true;

            strTemp = sc::string::format( "%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 0), 
                m_pItemData->sBasicOp.wReqLevelDW );

            if ( m_pItemData->sBasicOp.wReqLevelUP )
            {
                strTemp += sc::string::format( "~%d", m_pItemData->sBasicOp.wReqLevelUP );

                bReqLevel = (m_pItemData->sBasicOp.wReqLevelUP >= m_pCharacter->m_wLevel);
            }

            if ( ( m_pItemData->sBasicOp.wReqLevelDW <= m_pCharacter->m_wLevel ) && bReqLevel )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            m_pItemLabel1->AddString( nIndex, strTemp.c_str(), dwColor );
        }
    }

    // Info : 요구 수치
    {
        std::string strTemp = "";

        SCHARSTATS& rItemStats = m_pItemData->sBasicOp.sReqStats;
		//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
		const SCHARSTATS& rCharStats = m_pCharacter->m_sSUMSTATS_SKILL;
        bool bDistinction = false;
        //	착용조건->힘
        if ( rItemStats.wPow )
        {
            strTemp = sc::string::format( "%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 0 ), rItemStats.wPow );

            if ( rItemStats.wPow <= rCharStats.wPow )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if ( bDisplayLevel )
            {
                bDisplayLevel = false;
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }

            if ( bDistinction )
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            bDistinction = true;

            m_pItemLabel1->AddString( nIndex, strTemp.c_str(), dwColor );
        }
        //	착용조건->체력
        if ( rItemStats.wStr )
        {
            strTemp = sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 1 ), rItemStats.wStr );

            if ( rItemStats.wStr <= rCharStats.wStr )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if ( bDisplayLevel )
            {
                bDisplayLevel = false;
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }

            if ( bDistinction )
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            bDistinction = true;

            m_pItemLabel1->AddString( nIndex, strTemp.c_str(), dwColor );
        }
        //	착용조건->정신력
        if ( rItemStats.wSpi)
        {
            strTemp = sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 2 ), rItemStats.wSpi );

            if ( rItemStats.wSpi <= rCharStats.wSpi )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if ( bDisplayLevel )
            {
                bDisplayLevel = false;
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }

            if ( bDistinction )
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            bDistinction = true;

            m_pItemLabel1->AddString( nIndex, strTemp.c_str(), dwColor );
        }
        //	착용조건->민첩성
        if ( rItemStats.wDex )
        {
            strTemp = sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 3 ), rItemStats.wDex );

            if ( rItemStats.wDex <= rCharStats.wDex )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if ( bDisplayLevel )
            {
                bDisplayLevel = false;
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }

            if ( bDistinction )
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            bDistinction = true;

            m_pItemLabel1->AddString( nIndex, strTemp.c_str(), dwColor );
        }
        //	착용조건->지력
        if ( rItemStats.wInt  )
        {
            strTemp = sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 4 ), rItemStats.wInt );

            if ( rItemStats.wInt <= rCharStats.wInt )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if ( bDisplayLevel )
            {
                bDisplayLevel = false;
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }

            if ( bDistinction )
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            bDistinction = true;

            m_pItemLabel1->AddString( nIndex, strTemp.c_str(), dwColor );
        }
        //	착용조건->근력
        if ( rItemStats.wSta )
        {
            strTemp = sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 5 ), rItemStats.wSta );

            if ( rItemStats.wSta <= rCharStats.wSta )
                dwColor = ITEM_INFOR_COLOR::dwDefault;
            else
                dwColor = ITEM_INFOR_COLOR::dwReqWrong;

            if ( bDisplayLevel )
            {
                bDisplayLevel = false;
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            }

            if ( bDistinction )
                m_pItemLabel1->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
            bDistinction = true;

            m_pItemLabel1->AddString( nIndex, strTemp.c_str(), dwColor );
        }
    }
}

void CItemInfoTooltip::SetInformationLabel2( const SITEMCUSTOM& sItemCustom )
{
    std::string strTemp = "";

	// 코스튬 부착 아이템 옵션
	const ITEM::SSUIT* psDisguiseSuit = NULL;
	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
	if( NULL != pDisguise )
		psDisguiseSuit = &pDisguise->sSuitOp;

    if ( m_bIsWEAR_ITEM )
        m_pItemLabel2->AddText( ID2GAMEWORD("WEAR_ITEM"), ITEM_INFOR_COLOR::dwEquipCostum );

    // Info : 타입.
	// 내구도 팁이 아닐시에만
	if( !m_bIsDurableTip )
    {
        switch ( m_pItemData->BasicType() )
        {
        case ITEM_SUIT:
            {
                if ( m_pItemData->Suit() == SUIT_HANDHELD )
                {
                    // 무기 일때.
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						strTemp = sc::string::format( "%s(%s)",
							ID2GAMEWORD( "ITEM_BASIC_INFO", 11 ),
							CGameTextMan::GetInstance().GetCommentText("ITEMATTACK", m_pItemData->sSuitOp.emAttack).GetString() );
					}
					else
					{
						strTemp = sc::string::format( "%s(%s)",
							ID2GAMEWORD( "ITEM_BASIC_INFO", 11 ),
							COMMENT::ITEMATTACK[ m_pItemData->sSuitOp.emAttack ] );
					}
                    m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
                }
                else
                {
                    // 무기가 아닌 복장류 일때.
					if ( m_pItemData->sSuitOp.IsSTATE(EMITEM_SUIT_UNIQUE) == true )
					{
						
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							strTemp = sc::string::format( "%s(%s)",
								CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", m_pItemData->sSuitOp.emSuit).GetString(),
								ID2GAMEWORD("ITEM_BASIC_INFO", 19));
						}
						else
						{
							strTemp = sc::string::format( "%s(%s)",
								COMMENT::ITEMSUIT[ m_pItemData->sSuitOp.emSuit ].c_str(),
								ID2GAMEWORD("ITEM_BASIC_INFO", 19));
						}
					}
					else
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							strTemp = sc::string::format( "%s",
								CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", m_pItemData->sSuitOp.emSuit).GetString());
						}
						else
						{
							strTemp = sc::string::format( "%s",
								COMMENT::ITEMSUIT[ m_pItemData->sSuitOp.emSuit ].c_str());
						}
					}
                    int nIndex = m_pItemLabel2->AddText(strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault);				
					
                    // 코스튬 체크.
                    if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
                    {
                        const SITEM* pDisguiseData = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
                        if ( !pDisguiseData )   return;

                        strTemp = sc::string::format( "%s :%s", ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ), pDisguiseData->GetName() );			
                        m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );

                        if ( pDisguiseData->IsColorChange() )
                        {
                            strTemp = sc::string::format( "(%s %s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ),
                                ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 0 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                        else
                        {
                            strTemp = sc::string::format( "(%s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                    }
                    //else if ( m_pItemData->sBasicOp.dwFlags & ITEM_DISGUISE )
					else if ( sItemCustom.IsDisguise() )
                    {
                        if ( m_pItemData->IsColorChange() )
                        {
                            strTemp = sc::string::format( "(%s %s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ),
                                ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 0 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                        else
                        {
                            strTemp = sc::string::format( "(%s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                    }
                }
            }
            break;
		case ITEM_LOCK_BOX:
			{
				std::string strBasicType;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					strBasicType = m_pItemData->BasicType() < ITEM_NSIZE ? CGameTextMan::GetInstance().GetCommentText("ITEMTYPE", m_pItemData->BasicType()).GetString(): "";
				}
				else
				{
					strBasicType = COMMENT::ItemTypeName( m_pItemData->BasicType() ).c_str();
				}
				strBasicType.append(sItemCustom.bVietnamGainItem ? CGameTextMan::GetInstance().GetGameWord("LOCKBOX_TOOLTIP_LOCK", 0).GetString() : CGameTextMan::GetInstance().GetGameWord("LOCKBOX_TOOLTIP_LOCK", 1).GetString());
				m_pItemLabel2->AddText( strBasicType.c_str(), ITEM_INFOR_COLOR::dwDefault );				
			}
			break;

        default:
            {
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					m_pItemLabel2->AddText( m_pItemData->BasicType() < ITEM_NSIZE ? CGameTextMan::GetInstance().GetCommentText("ITEMTYPE", m_pItemData->BasicType()).GetString():"",
						ITEM_INFOR_COLOR::dwDefault );
				}
				else
				{
					m_pItemLabel2->AddText( COMMENT::ItemTypeName( m_pItemData->BasicType() ).c_str(),
						ITEM_INFOR_COLOR::dwDefault );
				}
            }
        }
    }

	GLCharacter* const pChar = m_pGaeaClient->GetCharacter();
	EMCHARINDEX charIdx = GLCI_NUM_ETC;
	int nLevel = -1;
	if ( pChar )
	{
		charIdx = pChar->GETCHARINDEX();
		nLevel = pChar->GETLEVEL();
	}

    // Info : 내구도
    {
        // 내구도 기능 사용 여부
        if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
        {   
            if ( m_pItemData->BasicType() == ITEM_SUIT && !m_bIsDurableTip )
            {
                // 내구도가 설정된 장비라면
                if( ItemDurability::GetInstance().IsUse(sItemCustom) )
                {
                    CString strDurability;
                    strDurability.Format( "%s :", ID2GAMEWORD( "ITEM_BASIC_INFO", 18 ) );
                    int nIndex = m_pItemLabel2->AddText( strDurability.GetString(), ITEM_INFOR_COLOR::dwDefault );
					
					// 내구도 컬러
					DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
					if( m_fDurabilityPerformance < 1.0f && m_fDurabilityPerformance > 1.0f )
						dwColor = ItemDurability::GetInstance().GetTextColor(sItemCustom, dwColor);

                    strDurability.Format("%d/%d (%d%%)", (int)ItemDurability::GetInstance().Get(sItemCustom), (int)ItemDurability::GetInstance().GetMax(sItemCustom), ItemDurability::GetInstance().GetPercent(sItemCustom) );
                    m_pItemLabel2->AddString( nIndex, strDurability.GetString(), dwColor );
                }
            }
        }
    }
	
    // Info : 공격
    {		
        //	공격력
		
		GLPADATA& sDamage = sItemCustom.getdamage(0.0f, charIdx, nLevel);
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID()); 
		
		int nMinDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wLow ); 
		int nMaxDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wHigh );
		int nRndOptDamage = static_cast<int>( sDamage.wLow - pTempITEM.sSuitOp.gdDamage.wLow );
		int nExtraValue = static_cast<int>( sItemCustom.GETGRADE_DAMAGE() );
		BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_DAMAGE);
		
		//코스튬 적용 시 코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptDamage -= static_cast<int>( psDisguiseSuit->gdDamage.wLow ); 

				//기본 로직
				//nMinDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wLow * m_fDurabilityPerformance);
				//nMaxDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wHigh * m_fDurabilityPerformance );
				nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurabilityPerformance );
				nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityPerformance );
				
				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptDamage += static_cast<int>( psDisguiseSuit->gdDamage.wLow );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				GLDWDATA& damage = sItemCustom.GETDAMAGE(charIdx, nLevel);
				nMinDmg = static_cast<int>(damage.nNow * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
				nMaxDmg = static_cast<int>(damage.nMax * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );
				nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityValue[DURABILITY_APPLY_BASIC] );
				
				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				GLDWDATA& damage = sItemCustom.GETDAMAGE(charIdx, nLevel);
				nMinDmg = static_cast<int>(damage.nNow * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) - nRndOptDamage - nExtraValue;
				nMaxDmg = static_cast<int>(damage.nMax * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) - nRndOptDamage - nExtraValue;
			}
		}

		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwInchant1 = ITEM_INFOR_COLOR::dwInchant1;
		DWORD dwInchant2 = ITEM_INFOR_COLOR::dwInchant2;
		
		//VAR_PARAM 함수에서 1 이하일때 1로 잡아주는 코드가 있어서 튤팁에서 조건 검사를 한번 더 해줘야함
		if ( nMinDmg || nMaxDmg || nRndOptDamage || nExtraValue || uGRADE )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->gdDamage.wNow && 0 != psDisguiseSuit->gdDamage.wMax )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			if ( uGRADE )
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );
				
				strTemp = sc::string::format( "%d", nMinDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );
				
				if(nRndOptDamage > 0 )
				{				
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
					strTemp = sc::string::format( "(+%d)", nExtraValue );
				else if ( nExtraValue < 0 )
					strTemp = sc::string::format( "(%d)", nExtraValue );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );

				m_pItemLabel2->AddString( nIndex, " ~ ", dwColor );
				
				strTemp = sc::string::format( "%d", nMaxDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );
				
				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
					strTemp = sc::string::format( "(+%d)", nExtraValue );
				else if ( nExtraValue < 0 )
					strTemp = sc::string::format( "(%d)", nExtraValue );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				
			}
			else
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );
				
				strTemp = sc::string::format( "%d", nMinDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );
				
				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				m_pItemLabel2->AddString( nIndex, " ~ ", dwColor );
				
				strTemp = sc::string::format( "%d", nMaxDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

			}
		}
	}
	//

	// Info : 방어
	{
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID()); 
		//여기서 해당 아이템 정보가져옴

		//	방어력
		int nBasicDef = static_cast<int>( pTempITEM.sSuitOp.nDefense);
		int nRndOptDef = static_cast<int>( sItemCustom.getdefense(charIdx, nLevel) - pTempITEM.sSuitOp.nDefense);
		int nExtraValue = sItemCustom.GETGRADE_DEFENSE();
		BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_DEFENSE);

		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwInchant1 = ITEM_INFOR_COLOR::dwInchant1;
		DWORD dwInchant2 = ITEM_INFOR_COLOR::dwInchant2;
		
			//내구도 적용 시
		if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
		{
			//nBasicDef = static_cast<int>( nBasicDef * m_fDurabilityPerformance );
			nRndOptDef = static_cast<int>( nRndOptDef * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );
			nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityValue[DURABILITY_APPLY_BASIC] );

			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicDef = static_cast<int>(sItemCustom.GETDEFENSE(charIdx, nLevel) * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) - nRndOptDef - nExtraValue;
		}		


		DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
		if( NULL != psDisguiseSuit )
		{
			if( 0 != psDisguiseSuit->nDefense )
				dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
		}
		
		if ( nBasicDef || uGRADE || nRndOptDef )
		{
			if ( uGRADE )
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nBasicDef );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDef > 0)
				{
					strTemp = sc::string::format( "(+%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDef < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
				{
					strTemp = sc::string::format( "(+%d)", nExtraValue );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				}
				else if ( nExtraValue < 0 )
				{
					strTemp = sc::string::format( "(%d)", nExtraValue );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				}
					
			}
			else
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nBasicDef );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDef > 0)
				{
					strTemp = sc::string::format( "(+%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDef < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
			}
		}
		
	}


	// Info : 명중률, 회피율, 소모 SP
	{
		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwRndOptColor = ITEM_INFOR_COLOR::dwInchant1;
		
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID()); 

		//	명중률
		int nBasicHitRate = static_cast<int>( pTempITEM.sSuitOp.nHitRate );
		int nRndOptHitRate = static_cast<int>( sItemCustom.GETHITRATE() - pTempITEM.sSuitOp.nHitRate );
		
		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptHitRate -= psDisguiseSuit->nHitRate;

				//nBasicHitRate = static_cast<int>( nBasicHitRate * m_fDurabilityPerformance );
				nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurabilityPerformance );	
				
				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptHitRate += psDisguiseSuit->nHitRate;

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicHitRate = static_cast<int>( sItemCustom.GETHITRATE() * m_fDurabilityPerformance ) - nRndOptHitRate;				
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				//nBasicHitRate = static_cast<int>( nBasicHitRate * m_fDurabilityPerformance );
				nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicHitRate = static_cast<int>(sItemCustom.GETHITRATE() * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) - nRndOptHitRate;
			}
		}
		
		if ( nBasicHitRate ||  nRndOptHitRate )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->nHitRate )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 3) );
			int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicHitRate );
			m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptHitRate > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptHitRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptHitRate < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptHitRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}

		//	회피율
		int nBasicAvoidRate = static_cast<int>( pTempITEM.sSuitOp.nAvoidRate );
		int nRndOptAvoidRate = static_cast<int>( sItemCustom.GETAVOIDRATE() - pTempITEM.sSuitOp.nAvoidRate );
		
		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptAvoidRate -= psDisguiseSuit->nAvoidRate;

				//nBasicAvoidRate = static_cast<int>( nBasicAvoidRate * m_fDurabilityPerformance );
				nRndOptAvoidRate = static_cast<int>( (nRndOptAvoidRate -  psDisguiseSuit->nAvoidRate) * m_fDurabilityPerformance ); 
				
				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptAvoidRate += psDisguiseSuit->nAvoidRate;

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicAvoidRate = static_cast<int>(sItemCustom.GETAVOIDRATE() * m_fDurabilityPerformance) - nRndOptAvoidRate;		
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f  )
			{
				//nBasicAvoidRate = static_cast<int>( nBasicAvoidRate * m_fDurabilityPerformance );
				nRndOptAvoidRate = static_cast<int>( nRndOptAvoidRate * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] ); 

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicAvoidRate = static_cast<int>(sItemCustom.GETAVOIDRATE() * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) - nRndOptAvoidRate;	
			}
		}

		if ( nBasicAvoidRate ||  nRndOptAvoidRate )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->nAvoidRate )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 4) );
			int nIndex =  m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicAvoidRate );
			m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptAvoidRate > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptAvoidRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptAvoidRate < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptAvoidRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}

		//	소모 SP
		int nBasicReqSP = static_cast<int>( pTempITEM.sSuitOp.wReqSP );
		int nRndOptReqSP = static_cast<int>( sItemCustom.GETREQ_SP() - pTempITEM.sSuitOp.wReqSP);

		
		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptReqSP -= static_cast<int>( psDisguiseSuit->wReqSP );

				//nBasicReqSP = static_cast<int>( nBasicReqSP * m_fDurabilityPerformance );
				nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurabilityPerformance ); 
				
				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptReqSP += static_cast<int>( psDisguiseSuit->wReqSP );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicReqSP = static_cast<int>( sItemCustom.GETREQ_SP()* m_fDurabilityPerformance ) - nRndOptReqSP;
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				//nBasicReqSP = static_cast<int>( nBasicReqSP * m_fDurabilityPerformance );
				nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicReqSP = static_cast<int>(sItemCustom.GETREQ_SP() * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) - nRndOptReqSP;
			}
		}
		

		if ( nBasicReqSP || nRndOptReqSP )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->wReqSP )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 7)  );
			int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicReqSP );
			m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptReqSP > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptReqSP );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptReqSP < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptReqSP );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}
	}

    // Info : 소지량
    {
        if ( m_pItemData->sDrugOp.wPileNum > 1 )
        {
            WORD wPileNum = m_pItemData->sDrugOp.wPileNum;
            WORD wTurnNum = sItemCustom.wTurnNum;
            if ( m_bInPrivateMarket )	//	개인 상점 구매창에서 보이는 툴팁일 때
            {
                strTemp = sc::string::format("%s :%d", ID2GAMEWORD("ITEM_TURN_INFO", 0 ), wTurnNum);
                m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );

                return ;
            }

            if ( m_bInMarket )  // 상점 구매창에서 보이는 툴팁일 때
                wTurnNum = m_pItemData->GETAPPLYNUM();
            strTemp = sc::string::format("%s :%d/%d", ID2GAMEWORD("ITEM_TURN_INFO", 0 ), wTurnNum, wPileNum);

            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
    }

    // Info : 약품류 사용 효과
    {
        if ( m_pItemData->BasicType() == ITEM_CURE )
        {
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if( m_pItemData->sDrugOp.dwCureVolume == 0 )
					strTemp = sc::string::format( "%s", CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString() );
				else
					strTemp = sc::string::format( "%s :%d",CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString(), m_pItemData->sDrugOp.dwCureVolume );
			}
			else
			{
				if( m_pItemData->sDrugOp.dwCureVolume == 0 )
					strTemp = sc::string::format( "%s", COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str() );
				else
					strTemp = sc::string::format( "%s :%d",COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str(), m_pItemData->sDrugOp.dwCureVolume );
			}

            if ( m_pItemData->sDrugOp.bRatio )
            {
                strTemp += "%";
            }

            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
    }

    // Info : 재사용 시간
    {
        //  재사용 시간
        if ( m_pItemData->sBasicOp.IsCoolTime() )
        {
            std::string strTime = "";
            CTimeSpan cCoolTime( m_pItemData->sBasicOp.dwCoolTime );

            if ( cCoolTime.GetHours() > 0 )
                strTime += sc::string::format( "%d%s ", cCoolTime.GetHours(), ID2GAMEWORD("ITEM_BASIC_TIME", 0) );
            if ( cCoolTime.GetMinutes() > 0 )	
                strTime += sc::string::format( "%d%s ", cCoolTime.GetMinutes(), ID2GAMEWORD("ITEM_BASIC_TIME", 1) );
            if ( cCoolTime.GetSeconds() > 0 )	
                strTime += sc::string::format( "%d%s ", cCoolTime.GetSeconds(), ID2GAMEWORD("ITEM_BASIC_TIME", 2) );

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 8), strTime );			
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }

        //	남은시간
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if ( pCharacter && pCharacter->IsCoolTime( m_pItemData->sBasicOp.sNativeID ) )
        {
            std::string strTime = "";
            __time64_t tCurTime =  m_pGaeaClient->GetCurrentTime().GetTime();
            __time64_t tCoolTime = pCharacter->GetMaxCoolTime( m_pItemData->sBasicOp.sNativeID );				

            CTimeSpan cReTime( tCoolTime - tCurTime );

            if ( cReTime.GetHours() > 0 )	
                strTime += sc::string::format( "%d%s ", cReTime.GetHours(), ID2GAMEWORD("ITEM_BASIC_TIME", 0) );
            if ( cReTime.GetMinutes() > 0 )	
                strTime += sc::string::format( "%d%s ", cReTime.GetMinutes(), ID2GAMEWORD("ITEM_BASIC_TIME", 1) );
            if ( cReTime.GetSeconds() > 0 )	
                strTime += sc::string::format( "%d%s ", cReTime.GetSeconds(), ID2GAMEWORD("ITEM_BASIC_TIME", 2) );

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 9), strTime );			
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }

		if( m_pItemData->sDrugOp.dwERList > 0 )
		{
			std::string str = "";

			for( int i = 0; i < ITEM_DRUG_ER_SIZE; ++i )
			{
				DWORD dwValue = 1 << i;

				if( dwValue & m_pItemData->sDrugOp.dwERList )
				{
					if( i > 0 )
						str += " + ";
						
					EMITEM_DRUG_ER eType = (EMITEM_DRUG_ER)i;

					switch( eType )
					{
					case ITEM_DRUG_ER_STUN        : str += sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_BONUS", 26) ); break;
					case ITEM_DRUG_ER_HALLUCINATE : str += sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_BONUS", 49) ); break;
					case ITEM_DRUG_ER_NON_DRUG    : str += sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_BONUS", 34) ); break;
					}
				}
			}

			if( !str.empty() )
			{
				str = sc::string::format( "%s : %s", ID2GAMEWORD("ITEM_BASIC_INFO", 21), str );
				m_pItemLabel2->AddText( str.c_str(), ITEM_INFOR_COLOR::dwDefault );
			}
		}
    }

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcNativeID );

	EMITEM_TYPE emType = m_pItemData->BasicType();

	bool bKeyTip = false;

	if ( pCrowData && m_iPageIndex >= 0 && m_pWishListItem->GetTotalLine() == 0 )
	{
		GLInventorySale* rInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven( m_iPageIndex ));

		EEXTYPE exType = rInven->GetExChangeType(sItemCustom.GetNativeID(), m_wPosX, m_wPosY);

		if( exType == EX_ITEM_TYPE && m_pWishItem == NULL ) 
			bKeyTip = true;

	}

	switch(emType)
	{
	case ITEM_SKILL :
	case ITEM_PET_SKILL:
	case ITEM_GRINDING:
	case ITEM_VEHICLE:
	case ITEM_PET_CARD:
	case ITEM_LUNCHBOX:
	case ITEM_PET_DUALSKILL:
		{
			if(bKeyTip ){
				strTemp = ID2GAMEINTEXT("HELP_SHOW_ITEM_TOOLTIP");
				m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
			}
		}
		break;
	}
	
    switch ( emType )
    {
    case ITEM_SKILL:
        {			
            DisplaySkillItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_PET_SKILL:
        {
            DisplayPetSkillItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_GRINDING:
        {
            //m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_CATEGORY_EX", 0 ), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_VIETNAM_EXPGET:
        {
            if ( m_pItemData->sDrugOp.bRatio )
                strTemp = sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_EXP", 1) , m_pItemData->sDrugOp.dwCureVolume );
            else
                strTemp = sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_EXP", 0), m_pItemData->sDrugOp.dwCureVolume );

            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_VIETNAM_ITEMGET:
        {
            strTemp = sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_ITEM", 0) , m_pItemData->sDrugOp.dwCureVolume );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_VEHICLE:
        {
            DisplayVehicleItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_PET_CARD:
        {
            DisplayPetCardItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_LUNCHBOX:
        {
            DisplayLunchBoxItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_PET_DUALSKILL:
        {
            DisplayPetDualSkillItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_POINT_CARD_REFUND:
    case ITEM_POINT_CARD_NOT_REFUND:
        {
            // Note : 포인트 양 표시.
            strTemp = sc::string::format( ID2GAMEWORD("POINT_CARD_POINT_VOLUME"), m_pItemData->sDrugOp.dwCureVolume );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_RANDOMITEM:
    case ITEM_BOX:
	case ITEM_SELECTIVEFORM_BOX:
        strTemp = ID2GAMEINTEXT("HELP_SHOW_ITEM_TOOLTIP");
        m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        break;
    };

	
    // Info : 그 외.
    EMITEM_TYPE emItemType = m_pItemData->BasicType();

    if( emItemType != ITEM_PET_SKILL && emItemType != ITEM_PET_DUALSKILL
        && emItemType != ITEM_POINT_CARD_REFUND && emItemType != ITEM_POINT_CARD_NOT_REFUND )
    {
        // Note : 시귀/직귀 카드에 맵이름 좌표 출력
        if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_REGEN )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_BASIC_INFO", 17 ), ITEM_INFOR_COLOR::dwDefault );

            GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	

            CString strName( m_pGaeaClient->GetMapName( pCharacter->m_sStartMapID ) );
            if( strName == _T("(null)") ) strName.Empty();

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_CALL_MAP", 0 ), strName );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );

            SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode( pCharacter->m_sStartMapID );
            if( pMapNode )
            {
                GLLevelFileClient cLevelFile(m_pGaeaClient);
                BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
                if( bOk )
                {	
                    D3DXVECTOR3 vStartPos;
                    PDXLANDGATE pGate = cLevelFile.GetLandGateMan()->FindLandGate( pCharacter->m_dwStartGate );
                    if( pGate )
                    {
                        vStartPos = pGate->GetGenPos( DxLandGate::GEN_RENDUM );
                    }
                    else
                    {
                        pGate = cLevelFile.GetLandGateMan()->FindLandGate( DWORD(0) );
                        if( pGate ) vStartPos = pGate->GetGenPos( DxLandGate::GEN_RENDUM );
                    }

                    int nPosX(0), nPosY(0);
                    cLevelFile.GetMapAxisInfo().Convert2MapPos( vStartPos.x, vStartPos.z, nPosX, nPosY );

                    strTemp = sc::string::format( "%s :%d,%d", ID2GAMEWORD("ITEM_CALL_MAP", 1 ), nPosX, nPosY );
                    m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
                }
            }
        }
        else if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_LASTCALL )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_BASIC_INFO", 17 ), ITEM_INFOR_COLOR::dwDefault );

            GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

            if( pCharacter->m_sLastCallMapID.IsValidNativeID() )
            {
                CString strName( m_pGaeaClient->GetMapName( pCharacter->m_sLastCallMapID ) );
                if( strName == _T("(null)") ) strName.Empty();

                strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_CALL_MAP", 0 ), strName );
                m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );

                SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode( pCharacter->m_sLastCallMapID );
                if( pMapNode )
                {
                    GLLevelFileClient cLevelFile(m_pGaeaClient);
                    BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
                    if( bOk )
                    {	
                        int nPosX(0), nPosY(0);
                        cLevelFile.GetMapAxisInfo().Convert2MapPos( pCharacter->m_vLastCallPos.x, pCharacter->m_vLastCallPos.z, nPosX, nPosY );

                        strTemp = sc::string::format( "%s :%d,%d", ID2GAMEWORD("ITEM_CALL_MAP", 1 ), nPosX, nPosY );
                        m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
                    }
                }
            }
            else
            {
                m_pItemLabel2->AddText( ID2GAMEWORD("ITEM_CALL_MAP", 2 ), ITEM_INFOR_COLOR::dwReqWrong );
            }
        }
        else if ( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_TELEPORT )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_BASIC_INFO", 17 ), ITEM_INFOR_COLOR::dwDefault );

            CString strName( m_pGaeaClient->GetMapName( m_pItemData->sBasicOp.sSubID ) );
            if( strName == _T("(null)") ) strName.Empty();

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_TELEPORT_MAP", 0 ), strName );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );

            strTemp = sc::string::format( "%s :%d,%d", ID2GAMEWORD("ITEM_TELEPORT_MAP", 1 ), m_pItemData->sBasicOp.wPosX, m_pItemData->sBasicOp.wPosY );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
		else if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_CAPSULE_TYPE1 || m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_CAPSULE_TYPE2 )
		{
			char szTemp[30] = {0, };
			WORD wCurLev = m_pCharacter->GETLEVEL();
			LONGLONG llCurExp = m_pCharacter->GETEXP();
			LONGLONG llCurMaxExp = m_pCharacter->GETMAXEXP();
			WORD wExpetedLev = 0;
			LONGLONG llRemainExp = 0;
			LONGLONG llRemainMaxExp = 0;
			float fPercent = 0;
			
			strTemp = sc::string::format( "%lu", (LONGLONG)sItemCustom.wDurability * 100000000 );

			sc::string::AddCommaToNum( strTemp.c_str(), szTemp );

			// 랩업을 하는 경우
			if( GLOGICEX::GLCALCULATEEXP( wCurLev, llCurExp, (LONGLONG)sItemCustom.wDurability * 100000000, wExpetedLev, llRemainExp, llRemainMaxExp) == TRUE ) 
			{
				fPercent = float(llRemainExp) / float(llRemainMaxExp);
			}
			// 랩업을 못하는 경우
			else
			{
				wExpetedLev = wCurLev;
				fPercent = float(llCurExp + llRemainExp) / float(llCurMaxExp);
			}

			// 툴팁 표시에 최대 레벨을 넘어서지 않도록
			if( wExpetedLev > GLCONST_CHAR::wMAX_LEVEL )
			{
				wExpetedLev = GLCONST_CHAR::wMAX_LEVEL;
				fPercent = 1;
			}
			else if(wExpetedLev == GLCONST_CHAR::wMAX_LEVEL)
			{
				if(fPercent > 1) 
					fPercent = 1;
			}

			std::string strExp = sc::string::format( "Exp : %s ( %dLv, %.2f%% )", szTemp, wExpetedLev, fPercent * 100.0f);
            m_pItemLabel2->AddText( strExp.c_str(), ITEM_INFOR_COLOR::dwDefault );
		}
		else if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE1 || m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE2 )
		{
			char szBeforeTemp[30] = {0, };
			char szAfterTemp[30] = {0, };
			LONGLONG llCurExp = m_pCharacter->GETEXP();
			LONGLONG llCurMaxExp = m_pCharacter->GETMAXEXP();
			LONGLONG llCompressorExp = 0;
			float fPercent = 0;

			if( llCurExp > llCurMaxExp ) llCurExp = llCurMaxExp;

			if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE2 )
				llCompressorExp = 100000000000;
			else
				llCompressorExp = 10000000000;

			strTemp = sc::string::format( "%lu", llCurExp );
			sc::string::AddCommaToNum( strTemp.c_str(), szBeforeTemp );
			fPercent = float(llCurExp) / float(llCurMaxExp);

			std::string strExp = sc::string::format( "Before : %s ( %.2f%% )", szBeforeTemp, fPercent * 100.0f);
            m_pItemLabel2->AddText( strExp.c_str(), ITEM_INFOR_COLOR::dwDefault );

			if( llCompressorExp <= llCurExp )
			{
				strTemp = sc::string::format( "%lu", llCurExp - llCompressorExp );
				sc::string::AddCommaToNum( strTemp.c_str(), szAfterTemp );
				fPercent = float(llCurExp - llCompressorExp) / float(llCurMaxExp);
			
				std::string strExp = sc::string::format( "After : %s ( %.2f%% )", szAfterTemp, fPercent * 100.0f);
				m_pItemLabel2->AddText( strExp.c_str(), ITEM_INFOR_COLOR::dwDefault );
			}
			else
			{
				std::string strExp = sc::string::format( "After : %s ", ID2GAMEWORD("ITEM_EXP_COMPRESSOR", 0) );
				m_pItemLabel2->AddText( strExp.c_str(), ITEM_INFOR_COLOR::dwDefault );
			}
		}
    }	
	// Info : 랜덤 수치
	{
		if( sItemCustom.IsSetRandOpt() )
		{
			DWORD dwRandomAbilityColor = ITEM_INFOR_COLOR::dwRandomAbility;

			{
				int nRandOptType = sItemCustom.GETOptTYPE1();
				float fVal = sItemCustom.GETOptVALUE1() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
			{
				int nRandOptType = sItemCustom.GETOptTYPE2();
				float fVal = sItemCustom.GETOptVALUE2() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
			{
				int nRandOptType = sItemCustom.GETOptTYPE3();
				float fVal = sItemCustom.GETOptVALUE3() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
			{
				int nRandOptType = sItemCustom.GETOptTYPE4();
				float fVal = sItemCustom.GETOptVALUE4() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
		}
	}
	//

    // 아이템 미리보기
    if ( m_bUsePreview )
    {
        if ( m_pInterface->ItemPreviewWindowIsPreviewItem( sItemCustom ) )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD("ITEM_USEPREVIEW_RN"), ITEM_INFOR_COLOR::dwDefault );
        }
    }
}

void CItemInfoTooltip::SetInformationLabel3( const SITEMCUSTOM& sItemCustom )
{
    const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

    std::string strText = "";

    // Info : 가산 수치
    {
        int arrVALUE[EMADD_SIZE];
        SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

        for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
        {
            EMITEM_ADDON emTYPE = sSUIT.sADDON[i].emTYPE;

            if ( emTYPE != EMADD_NONE )
            {
				//내구도 적용
                int nVALUE = static_cast<int>( sSUIT.sADDON[i].nVALUE * m_fDurabilityValue[DURABILITY_APPLY_ADD] );

                switch ( emTYPE )
                {
                    //	밑에서 한번에 계산
				case EMADD_HP:			
					{
						if( m_bIsDurableTip ) { arrVALUE[EMADD_HP] += nVALUE; }
					}					
					break;
				case EMADD_MP:
					{
						if( m_bIsDurableTip ) { arrVALUE[EMADD_MP] += nVALUE; }
					}					
					break;
				case EMADD_SP:			
					{
						if( m_bIsDurableTip ) { arrVALUE[EMADD_SP] += nVALUE; }
					}
					break;
				case EMADD_MA:			
					{
						if( m_bIsDurableTip ) { arrVALUE[EMADD_MA] += nVALUE; }
					}
					break;
                case EMADD_HITRATE:		arrVALUE[EMADD_HITRATE] += nVALUE;		break;
                case EMADD_AVOIDRATE:	arrVALUE[EMADD_AVOIDRATE] += nVALUE;	break;
                case EMADD_DAMAGE:		arrVALUE[EMADD_DAMAGE] += nVALUE;		break;
                case EMADD_DEFENSE:		arrVALUE[EMADD_DEFENSE] += nVALUE;		break;
                case EMADD_STATS_POW:	arrVALUE[EMADD_STATS_POW] += nVALUE;	break;
                case EMADD_STATS_STR:	arrVALUE[EMADD_STATS_STR] += nVALUE;	break;
                case EMADD_STATS_SPI:	arrVALUE[EMADD_STATS_SPI] += nVALUE;	break;
                case EMADD_STATS_DEX:	arrVALUE[EMADD_STATS_DEX] += nVALUE;	break;
                case EMADD_STATS_INT:	arrVALUE[EMADD_STATS_INT] += nVALUE;	break;
                case EMADD_STATS_STA:	arrVALUE[EMADD_STATS_STA] += nVALUE;	break;
                case EMADD_PA:			arrVALUE[EMADD_PA] += nVALUE;			break;
                case EMADD_SA:			arrVALUE[EMADD_SA] += nVALUE;			break;
                };
            }
        }

		if( m_bIsDurableTip )
		{
			arrVALUE[EMADD_HP] += static_cast<int>(sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_HP) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION]);
			arrVALUE[EMADD_MP] += static_cast<int>(sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MP) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION]);
			arrVALUE[EMADD_SP] += static_cast<int>(sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_SP) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION]);

			float fRATE  = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];
			float fVALUE = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];

			fVALUE += sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MAGIC) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];

			if ( fRATE!=0 || fVALUE!=0)
			{
				arrVALUE[EMADD_MA] = INT(arrVALUE[EMADD_MA]*(100.0f+fRATE)*0.01f) + INT(fVALUE);
			}
		}
		else
		{
			//	전체 옵션을 따로 계산
			arrVALUE[EMADD_HP] = static_cast<int>( sItemCustom.GETADDHP() * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
			arrVALUE[EMADD_MP] = static_cast<int>( sItemCustom.GETADDMP() * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
			arrVALUE[EMADD_SP] = static_cast<int>( sItemCustom.GETADDSP() * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
			arrVALUE[EMADD_MA] = static_cast<int>( sItemCustom.GETADDMA() * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
		}

        //	코스튬 능력치
        if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
        {
            //if ( pItem && pItem->sBasicOp.IsDISGUISE() )
            const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.nidDISGUISE);

            SITEMCUSTOM sCustomeItem(sItemCustom.nidDISGUISE);
            if (pItem&&sCustomeItem.IsDisguise())
            {
                for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
                {
                    EMITEM_ADDON emTYPE = pItem->sSuitOp.sADDON[i].emTYPE;

                    if ( emTYPE != EMADD_NONE )
                    {
                        int nVALUE = pItem->sSuitOp.sADDON[i].nVALUE;

                        switch ( emTYPE )
                        {
                        case EMADD_HP:          arrVALUE[EMADD_HP] += nVALUE;		    break;
                        case EMADD_MP:          arrVALUE[EMADD_MP] += nVALUE;		    break;
                        case EMADD_SP:          arrVALUE[EMADD_SP] += nVALUE;		    break;
                        case EMADD_MA:          arrVALUE[EMADD_MA] += nVALUE;		    break;
                        case EMADD_HITRATE:		arrVALUE[EMADD_HITRATE] += nVALUE;		break;
                        case EMADD_AVOIDRATE:	arrVALUE[EMADD_AVOIDRATE] += nVALUE;	break;
                        case EMADD_DAMAGE:		arrVALUE[EMADD_DAMAGE] += nVALUE;		break;
                        case EMADD_DEFENSE:		arrVALUE[EMADD_DEFENSE] += nVALUE;		break;
                        case EMADD_STATS_POW:	arrVALUE[EMADD_STATS_POW] += nVALUE;	break;
                        case EMADD_STATS_STR:	arrVALUE[EMADD_STATS_STR] += nVALUE;	break;
                        case EMADD_STATS_SPI:	arrVALUE[EMADD_STATS_SPI] += nVALUE;	break;
                        case EMADD_STATS_DEX:	arrVALUE[EMADD_STATS_DEX] += nVALUE;	break;
                        case EMADD_STATS_INT:	arrVALUE[EMADD_STATS_INT] += nVALUE;	break;
                        case EMADD_STATS_STA:	arrVALUE[EMADD_STATS_STA] += nVALUE;	break;
                        case EMADD_PA:			arrVALUE[EMADD_PA] += nVALUE;			break;
                        case EMADD_SA:			arrVALUE[EMADD_SA] += nVALUE;			break;
                        };

                    }
                }
            }
        }
        
        // 내구도 컬러
        DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;
      
        for( int i=1; i<EMADD_SIZE; ++i)
        {
            if( arrVALUE[i] != 0 )
            {
                strText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADDON_INFO", i) );

                int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

                if ( arrVALUE[i] > 0 )
                    strText = sc::string::format( "+%d", arrVALUE[i] );
                else if ( arrVALUE[i] < 0 )
                    strText = sc::string::format( "%d", arrVALUE[i] );

                m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );

            }
        }
    }
    //

	float fDurableValue = m_fDurabilityValue[DURABILITY_APPLY_ADD];

	if( m_bIsDurableTip )
	{
		fDurableValue = 1.0f;
	}

    // Info : 특수 기능
    {		
        EMITEM_VAR emITEM_VAR = m_pItemData->sSuitOp.sVARIATE.emTYPE;
        EMITEM_VAR emITEM_VOL = m_pItemData->sSuitOp.sVOLUME.emTYPE;

        float arrVALUE[EMVAR_SIZE];
        SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

        //	특수기능 ( 변화율 )
        //
        if ( emITEM_VAR != EMVAR_NONE )
        {
            switch ( emITEM_VAR )
            {
            case EMVAR_HP:
            case EMVAR_MP:
            case EMVAR_SP:
            case EMVAR_AP:
                break;
            default:
                arrVALUE[emITEM_VAR] = ( m_pItemData->sSuitOp.sVARIATE.fVariate * fDurableValue);
                break;
            };
        }

        arrVALUE[EMVAR_HP] = ( sItemCustom.GETINCHP() * fDurableValue );
        arrVALUE[EMVAR_MP] = ( sItemCustom.GETINCMP() * fDurableValue );
        arrVALUE[EMVAR_SP] = ( sItemCustom.GETINCSP() * fDurableValue );
        arrVALUE[EMVAR_AP] = ( sItemCustom.GETINCAP() * fDurableValue ); 

        //	코스튬 능력치
        float fCostumVolume = 0.0f;

        if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
        {
            //if ( pItem && pItem->sBasicOp.IsDISGUISE() )
            const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.nidDISGUISE);

            SITEMCUSTOM sCustomeItem(sItemCustom.nidDISGUISE);
            if ( pItem && sCustomeItem.IsDisguise())
            {
                EMITEM_VAR emITEM_VAR_COS = pItem->sSuitOp.sVARIATE.emTYPE;
                EMITEM_VAR emITEM_VOL_COS = pItem->sSuitOp.sVOLUME.emTYPE;

                if ( emITEM_VAR_COS != EMVAR_NONE )
                {
                    float fVariate = pItem->sSuitOp.sVARIATE.fVariate;		

                    switch ( emITEM_VAR_COS )
                    {
                    case EMVAR_HP:
                    case EMVAR_MP:
                    case EMVAR_SP:
                    case EMVAR_AP:
                        arrVALUE[emITEM_VAR_COS] += fVariate;
                        break;
                    default:
                        arrVALUE[emITEM_VAR_COS] += fVariate;
                        break;
                    };
                }

                if ( emITEM_VOL_COS != EMVAR_NONE)
                {
                    fCostumVolume = pItem->sSuitOp.sVOLUME.fVolume;
                }
            }
        }


        // 내구도 컬러 //이동 속도는 여기서 처리가 안됨
        DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;
       
        for ( int i = 1; i < EMVAR_SIZE; ++i )
        {
            if ( arrVALUE[i] != 0.0f ) 
            {
					if ( m_pItemData->BasicType() != ITEM_PET_FOOD
						&& m_pItemData->BasicType() != ITEM_VEHICLE_OIL )
						arrVALUE[i] *= COMMENT::ITEMVAR_SCALE[i];

				CString strTemp = sc::string::format( "%.2f", arrVALUE[i] ).c_str();

				CutZero(strTemp,2);

                strText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", i ) );
                int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

                strText = sc::string::format( "%s", strTemp);

                if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
                {
					const SITEM* pNativeItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
                    const SITEM* pDisguiseItem = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
					if( NULL != pNativeItem && NULL != pDisguiseItem )
					{
						if( COMMENT::IsITEMVAR_SCALE( pDisguiseItem->sSuitOp.sVARIATE.emTYPE ) ||
							COMMENT::IsITEMVAR_SCALE( pNativeItem->sSuitOp.sVARIATE.emTYPE ) )
							strText += "%";
					}
                }
                else
                {
					if ( COMMENT::IsITEMVAR_SCALE(emITEM_VAR) )
						strText += "%";
                }

                m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
            }
        }


        // 경험치 배율
        float fExpMultiple = m_pItemData->GetExpMultiple(); //내구도 적용

		//내구도 적용
		if( (fExpMultiple > 1.0f) || (m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f) )
		{
			fExpMultiple = ( (fExpMultiple - 1.0f) * m_fDurabilityValue[DURABILITY_APPLY_ADD] ) + 1.0f;
		}

        const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.nidDISGUISE);

        SITEMCUSTOM sCustomeItem(sItemCustom.nidDISGUISE);
        if ( pItem && sCustomeItem.IsDisguise())
        {
            fExpMultiple += pItem->GetExpMultiple() - 1.0f;
            fExpMultiple = max( fExpMultiple, 1.0f );
        }
		
        if ( fExpMultiple != 1.0f )
        {
			CString strTemp = sc::string::format( "%.2f", fExpMultiple * 100.0f - 100.0f ).c_str();

			CutZero(strTemp,2);

            strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_EX", 0 ));
            int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

            strText = sc::string::format( "+%s%%", strTemp);
            m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
        }

        //	특수기능 ( 변화량 )
		EMITEM_VAR emDisguiseItemVol = EMVAR_NONE;
		if( NULL != pItem )
			emDisguiseItemVol = pItem->sSuitOp.sVOLUME.emTYPE;

		if( emITEM_VOL != emDisguiseItemVol && emITEM_VOL != EMVAR_NONE && emDisguiseItemVol != EMVAR_NONE )
		{
			float fVOLUME = 0.0f;
			int nIndex = 0;
			CString strTemp;

			// Basic--------------------------------------------
			if( emITEM_VOL == EMVAR_MOVE_SPEED )
				fVOLUME = sItemCustom.GETMOVESPEED() * m_fDurabilityValue[DURABILITY_APPLY_ADD];
			else
				fVOLUME = m_pItemData->sSuitOp.sVOLUME.fVolume * m_fDurabilityValue[DURABILITY_APPLY_ADD];
			
			if( 0.0f != fVOLUME )
			{
				strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				CutZero( strTemp, 4 );
				strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ) );
				nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp );
				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}

			// Disguise------------------------------------------------
			if( emDisguiseItemVol == EMVAR_MOVE_SPEED )
				fVOLUME = sItemCustom.GETMOVESPEED();
			else
				fVOLUME = fCostumVolume;
			
			if( 0.0f != fVOLUME )
			{
				strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				CutZero( strTemp, 4 );
				strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emDisguiseItemVol ) );
				nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp );
				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}			
		}
		else if( emITEM_VOL != EMVAR_NONE || emDisguiseItemVol != EMVAR_NONE )
        {
			float fVOLUME = 0.0f;
			if( emITEM_VOL == EMVAR_MOVE_SPEED || emDisguiseItemVol == EMVAR_MOVE_SPEED )
				fVOLUME = sItemCustom.GETMOVESPEED() * fDurableValue;
			else
				fVOLUME = (m_pItemData->sSuitOp.sVOLUME.fVolume * fDurableValue) + fCostumVolume;	//공격속도
					
			if( 0.0f != fVOLUME )
			{
				CString strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				CutZero( strTemp, 4 );

				int nIndex = 0;

				if( emITEM_VOL == emDisguiseItemVol || emDisguiseItemVol == EMVAR_NONE )
					strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ) );
				else if( emITEM_VOL == EMVAR_NONE )
					strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emDisguiseItemVol ) );

				nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp );
				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}
        }
    }
    //
	// Info : 저항
	{
		//	기본 저항값
		int nELEC   = sSUIT.sResist.nElectric;
		int nFIRE   = sSUIT.sResist.nFire;
		int nICE    = sSUIT.sResist.nIce;
		int nPOISON = sSUIT.sResist.nPoison;
		int nSPIRIT = sSUIT.sResist.nSpirit;
		
		//보너스 효과 분리
		int nAddELEC( static_cast<int>(sItemCustom.GETRESIST_ELEC() - nELEC) ); 
		int	nAddFIRE( static_cast<int>(sItemCustom.GETRESIST_FIRE() - nFIRE) ); 
		int	nAddICE( static_cast<int>(sItemCustom.GETRESIST_ICE() - nICE) ); 
		int	nAddPOISON( static_cast<int>(sItemCustom.GETRESIST_POISON() - nPOISON) ); 
		int	nAddSPIRIT( static_cast<int>(sItemCustom.GETRESIST_SPIRIT() - nSPIRIT) );

		if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
		{
            const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.nidDISGUISE);

            SITEMCUSTOM sCustomeItem(sItemCustom.nidDISGUISE);
			
			if( m_fDurabilityPerformance < 1.0f )
			{
				nELEC   = static_cast<int>( nELEC   * m_fDurabilityPerformance );
				nFIRE   = static_cast<int>( nFIRE   * m_fDurabilityPerformance );
				nICE    = static_cast<int>( nICE    * m_fDurabilityPerformance );
				nPOISON = static_cast<int>( nPOISON * m_fDurabilityPerformance );
				nSPIRIT = static_cast<int>( nSPIRIT * m_fDurabilityPerformance );

				nAddELEC   = static_cast<int>( nAddELEC   * m_fDurabilityPerformance );
				nAddFIRE   = static_cast<int>( nAddFIRE   * m_fDurabilityPerformance );
				nAddICE    = static_cast<int>( nAddICE    * m_fDurabilityPerformance );
				nAddPOISON = static_cast<int>( nAddPOISON * m_fDurabilityPerformance );
				nAddSPIRIT = static_cast<int>( nAddSPIRIT * m_fDurabilityPerformance );
			}

			//코스튬 옵션 추가 - 코스튬 옵션은 내구도 영향을 받지 않음
			if ( pItem && sCustomeItem.IsDisguise())
			{
				nAddELEC       += pItem->sSuitOp.sResist.nElectric;
				nAddFIRE       += pItem->sSuitOp.sResist.nFire;
				nAddICE        += pItem->sSuitOp.sResist.nIce;
				nAddPOISON     += pItem->sSuitOp.sResist.nPoison;
				nAddSPIRIT     += pItem->sSuitOp.sResist.nSpirit;
			}	
		}
		else
		{
			if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
			{
				nELEC   = static_cast<int>( nELEC   * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nFIRE   = static_cast<int>( nFIRE   * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nICE    = static_cast<int>( nICE    * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nPOISON = static_cast<int>( nPOISON * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nSPIRIT = static_cast<int>( nSPIRIT * m_fDurabilityValue[DURABILITY_APPLY_ADD] );

				nAddELEC   = static_cast<int>( nAddELEC   * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nAddFIRE   = static_cast<int>( nAddFIRE   * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nAddICE    = static_cast<int>( nAddICE    * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nAddPOISON = static_cast<int>( nAddPOISON * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nAddSPIRIT = static_cast<int>( nAddSPIRIT * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
			}

		}

		if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT ||
			nAddELEC || nAddFIRE || nAddICE || nAddPOISON || nAddSPIRIT )
		{
			// 내구도 컬러
			DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;

			m_bDisplayResist = true;

			//strText = sc::string::format("%s :", ID2GAMEWORD ("ITEM_BASIC_INFO", 12) );
			strText = " ";
			m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );
			
			int nIndex = 0;
			//if ( nSPIRIT )
			{
				strText = sc::string::format("%d", nSPIRIT);
				nIndex = m_pResistSpiritText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddSPIRIT )
				{
					strText = sc::string::format("(+%d)", nAddSPIRIT);
					m_pResistSpiritText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nFIRE )
			{
				strText = sc::string::format("%d", nFIRE );
				nIndex = m_pResistFireText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddFIRE )
				{
					strText = sc::string::format("(+%d)", nAddFIRE);
					m_pResistFireText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nICE )
			{
				strText = sc::string::format("%d", nICE );
				nIndex = m_pResistIceText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddICE )
				{
					strText = sc::string::format("(+%d)", nAddICE );
					m_pResistIceText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nELEC )
			{
				strText = sc::string::format("%d", nELEC );
				nIndex = m_pResistElectricText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddELEC )
				{
					strText = sc::string::format("(+%d)", nAddELEC );
					m_pResistElectricText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nPOISON )
			{
				strText = sc::string::format("%d", nPOISON );
				nIndex = m_pResistPoisonText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddPOISON )
				{
					strText = sc::string::format("(+%d)", nAddPOISON );
					m_pResistPoisonText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}
			
			//추가 저항 증가 옵션이 없을 경우 출력 간격 조절 22는 임의의 숫자 보기 좋은 간격!
			if(nAddSPIRIT || nAddFIRE || nAddICE || nAddELEC || nAddPOISON)
				m_pResistTextGap = 20.0f;
			else
				m_pResistTextGap = 0.0f;
		}
	}
   
    //
}

void CItemInfoTooltip::SetInformationLabel3_RandomOption( int nRandOptType, float fVal, bool bIsPerRandOpt, DWORD dwAddAbility )
{
    std::string strDescText = "";

    if( m_fDurabilityPerformance < 1.0f || m_fDurabilityPerformance > 1.0f )
        fVal = fVal * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];
	
    if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
		return;

	if ( fVal == 0.0f )
		return;

	strDescText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );
	int nIndex = m_pItemLabel2->AddText( strDescText.c_str(), ITEM_INFOR_COLOR::dwRandomAbility );

	strDescText = "";

	if ( fVal > 0.0f) 
		strDescText += _T('+');
	if ( fVal < 0.0f) 
		strDescText += _T('-');

	if ( bIsPerRandOpt )
	{
		CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
		CutZero(strTemp,2);
		strDescText += sc::string::format("%s", strTemp);

		strDescText += _T('%');
	}
	else
		strDescText += sc::string::format( "%d", static_cast<int>(fVal) );

	const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
	if ( _bShowMinMax == true )
	{
		const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		const RandomOption::Entity* const _pEntity(_managerRandomOption.find(m_pItemData->sRandomOpt.szNAME));		
		if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
		{
			RandomOption::Information _information;
			if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
			{
				if ( bIsPerRandOpt == true )
					strDescText += sc::string::format( "(%.2f~%.2f)", _information.valueFloatLow, _information.valueFloatHigh);
				else
					strDescText += sc::string::format( "(%d~%d)", _information.valueIntegerLow, _information.valueIntegerHigh);
			}			
		}
	}	

	m_pItemLabel2->AddString( nIndex, strDescText.c_str(), dwAddAbility );
}
void CItemInfoTooltip::SetInformationLabel4( const SITEMCUSTOM& sItemCustom )
{
    std::string strText = "";

    //	상태이상
    EMSTATE_BLOW emBLOW = m_pItemData->sSuitOp.sBLOW.emTYPE;

    if ( emBLOW !=EMBLOW_NONE )
    {
        float fLife = m_pItemData->sSuitOp.sBLOW.fLIFE;
        float fRate = m_pItemData->sSuitOp.sBLOW.fRATE;

		float fVAR1;
		float fVAR2;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			fVAR1 = m_pItemData->sSuitOp.sBLOW.fVAR1 * COMMENT::BLOW_VAR1_SCALE[emBLOW];
			fVAR2 = m_pItemData->sSuitOp.sBLOW.fVAR2 * COMMENT::BLOW_VAR2_SCALE[emBLOW];
		}
		else
		{
			fVAR1 = m_pItemData->sSuitOp.sBLOW.fVAR1 * COMMENT::BLOW_VAR1_SCALE[emBLOW];
			fVAR2 = m_pItemData->sSuitOp.sBLOW.fVAR2 * COMMENT::BLOW_VAR2_SCALE[emBLOW];
		}        

		ITEM_TOOLTIP_STATE_BLOW emStateBlow = ITEM_TOOLTIP_VALUE;

		if ( fVAR1 != 0.0f )
		{
			if ( fVAR2 != 0.0f )
				emStateBlow = ITEM_TOOLTIP_VALUE_COUNT_2;
			else
				emStateBlow = ITEM_TOOLTIP_VALUE_COUNT_1_VAR1;
		}
		else
		{
			if( fVAR2 != 0.0f )
				emStateBlow = ITEM_TOOLTIP_VALUE_COUNT_1_VAR2;
		}

		//if ( fVAR1 < 0.0f )
			//fVAR1 = -fVAR1;

		//if ( fVAR2 < 0.0f )
			//fVAR2 = -fVAR2;

        switch ( emBLOW )
        {
        case EMBLOW_NUMB:   //	마비.
            {
				//확률은 수치값이 100%가 더해져서 적용되고 있다 표기출력은 - 100% (ex.딜레이변화 1.3 >> 툴팁 표시 스킬딜레이 +30%)
				if(fVAR2 > (1.0f * COMMENT::BLOW_VAR2_SCALE[emBLOW])) 
					fVAR2 -=  (1.0f * COMMENT::BLOW_VAR2_SCALE[emBLOW]);

				strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_NUMB_RN" ), fRate, fLife, fVAR1 );
				
				//+기호 붙이기
				if( fVAR2 > 0.0f )
					strText += sc::string::format(" +%2.f%%", fVAR2);
				else
					strText += sc::string::format(" %2.f%%", fVAR2);

				m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        case EMBLOW_STUN:   //	기절.
            {
                strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STUN_RN" ), fRate, fLife );
                m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        case EMBLOW_STONE:  //	석화.
            {
				strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STONE_RN" ), fRate, fLife, fVAR1, fVAR2 );
				m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        case EMBLOW_BURN:   //	화염.
            {
				//음수표기
                if ( fVAR2 > 0.0f )
                    fVAR2 = -fVAR2;

                strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_BURN_RN" ), fRate, fLife, fVAR2 );
                m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        case EMBLOW_FROZEN: //	냉동.
            {
				strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_FROZEN_RN" ), fRate, fLife, fVAR1 );
				
				//+기호 붙이기
				if( fVAR2 > 0.0f )
					strText += sc::string::format(" +%2.f%%", fVAR2);
				else
					strText += sc::string::format(" %2.f%%", fVAR2);

				m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        case EMBLOW_MAD:    //	착란.
            {
				//음수표기
				if ( fVAR1 > 0.0f )
					fVAR1 = -fVAR1;

				//음수표기
				if ( fVAR2 > 0.0f )
					fVAR2 = -fVAR2;
				strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_MAD_RN" ), fRate, fLife, fVAR1, fVAR2 );
				m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        case EMBLOW_POISON: //	중독.
            {
                if ( fVAR2 > 0.0f )
                    fVAR2 = -fVAR2;

                strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_POISON_RN" ), fRate, fLife, fVAR2 );
                m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        case EMBLOW_CURSE:  //	저주.
            {
				strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_CURSE_RN" ), fLife, fRate, fVAR1 );
				m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
            }
            break;

        default:
            {
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strText = sc::string::format("%s :%s", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 0 ),CGameTextMan::GetInstance().GetCommentText("BLOW", emBLOW).GetString());
				else
					strText = sc::string::format("%s :%s", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 0 ), COMMENT::BLOW[emBLOW].c_str() );

                m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );

                strText = sc::string::format("%s :%.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 1 ), fLife );
                m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );

                strText = sc::string::format("%s :%.2f%%", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 2 ), fRate );
                m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );

                if( fVAR1 != 0.0f )
                {
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						if ( COMMENT::IsBLOW1_PER(emBLOW) )
							strText = sc::string::format("%s :%.2f%%", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", emBLOW).GetString(), fVAR1 );
						else
							strText = sc::string::format("%s :%.2f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", emBLOW).GetString(), fVAR1 );
					}
					else
					{
						if ( COMMENT::IsBLOW1_PER(emBLOW) )
							strText = sc::string::format("%s :%.2f%%", COMMENT::BLOW_VAR1[emBLOW].c_str(), fVAR1 );
						else
							strText = sc::string::format("%s :%.2f", COMMENT::BLOW_VAR1[emBLOW].c_str(), fVAR1 );
					}

                    m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
                }

                if( fVAR2 != 0.0f )
                {
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						if ( COMMENT::IsBLOW2_PER(emBLOW) )
							strText = sc::string::format("%s :%.2f%%", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", emBLOW).GetString(), fVAR2 );
						else
							strText =sc::string::format("%s :%.2f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", emBLOW).GetString(), fVAR2 );
					}
					else
					{
						if ( COMMENT::IsBLOW2_PER(emBLOW) )
							strText = sc::string::format("%s :%.2f%%", COMMENT::BLOW_VAR2[emBLOW].c_str(), fVAR2 );
						else
							strText = sc::string::format("%s :%.2f", COMMENT::BLOW_VAR2[emBLOW].c_str(), fVAR2 );
					}
                    m_pItemLabel4->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
                }
            }
        };
    }

	if( m_pItemData->sItemSkill.sSkillID.IsValidNativeID() )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(m_pItemData->sItemSkill.sSkillID);

		if( pSkill == NULL ) return;
		
		switch( m_pItemData->sItemSkill.emSkillTarget )
		{
		case EMITEM_SKILL_ALL    :
			{
				strText = sc::string::format( ID2GAMEINTEXT( "ITEM_SKILL_LINK_INFO", 0 ), (int)(m_pItemData->sItemSkill.fOccuRate*100), pSkill->GetNameStr().c_str());
				m_pItemLabel4->AddText( strText.c_str(), NS_UITEXTCOLOR::DODGERBLUE );
			}
			break;
		case EMITEM_SKILL_SELF   :
			{
				strText = sc::string::format( ID2GAMEINTEXT( "ITEM_SKILL_LINK_INFO", 1 ), (int)(m_pItemData->sItemSkill.fOccuRate*100), pSkill->GetNameStr().c_str());
				m_pItemLabel4->AddText( strText.c_str(), NS_UITEXTCOLOR::DODGERBLUE );
			}
			break;
		case EMITEM_SKILL_ENERMY :
			{
				strText = sc::string::format( ID2GAMEINTEXT( "ITEM_SKILL_LINK_INFO", 2 ), (int)(m_pItemData->sItemSkill.fOccuRate*100), pSkill->GetNameStr().c_str());
				m_pItemLabel4->AddText( strText.c_str(), NS_UITEXTCOLOR::DODGERBLUE );
			}
			break;
		case EMITEM_SKILL_MOB    :
			{
				strText = sc::string::format( ID2GAMEINTEXT( "ITEM_SKILL_LINK_INFO", 3 ), (int)(m_pItemData->sItemSkill.fOccuRate*100), pSkill->GetNameStr().c_str());
				m_pItemLabel4->AddText( strText.c_str(), NS_UITEXTCOLOR::DODGERBLUE );
			}
			break;
		}
		
	}
	
}
void CItemInfoTooltip::SetInformationLabelSetItem( const SITEMCUSTOM& sItemCustom )
{
	if ( !GLUseFeatures::GetInstance().IsUsingSetItemUse() ) return;

	const SSETITEM *sSetItem = GLItemSet::GetInstance().GetInfoSetItem(sItemCustom.GetNativeID());
			
	if(NULL == sSetItem) return;	//셋트아이템 검색안됨 -> 출력 안함
		
	//현재 슬롯에 가지고 있는 동일한 셋트 아이템 목록 검색
	SSETITEMSEARCHINFO sInfoPutOnSetItem;;

	for(UINT i = 0; i < m_pCharacter->m_vInfoPutOnSetItem.size(); ++i)
	{
		if( sSetItem->dwSetID == m_pCharacter->m_vInfoPutOnSetItem[i].dwSetID )
		{
			sInfoPutOnSetItem = m_pCharacter->m_vInfoPutOnSetItem[i];
			break;
		}
	}
	
	//튤팁 출력
	SetInformationLabelSetItemName(sSetItem, sInfoPutOnSetItem);
	SetInformationLabelSetItemParts(sSetItem, sInfoPutOnSetItem);
	SetInformationLabelSetItemBonus(sSetItem, sInfoPutOnSetItem);
	SetInformationLabelSetItemPreview();
}

void CItemInfoTooltip::SetInformationLabelSetItemName( const SSETITEM *sSetItem , const SSETITEMSEARCHINFO &sInfoPutOnSetItem )
{
	std::string strResult = "";	//필수
	
	strResult = sc::string::format("%s", ID2GAMEWORD("ITEM_INFO_SET", 0)) ;	//셋트 : 
	strResult += sc::string::format("%s ", sSetItem->strName ); //셋트 명칭
	strResult += sc::string::format( "(%d/%d)", sInfoPutOnSetItem.vItemID.size(), static_cast<int>(sSetItem->byMaxToolTipItemList) ); //셋트 명칭	
	m_pItemSet->AddText( strResult.c_str(), ITEM_INFOR_COLOR::dwSetItem );
}

void CItemInfoTooltip::SetInformationLabelSetItemParts( const SSETITEM *sSetItem, const SSETITEMSEARCHINFO &sInfoPutOnSetItem )
{
	std::string strResult = "";	
	
	DWORD dwStringColor = ITEM_INFOR_COLOR::dwTrade;
	
	int nIndex(0);
		
	SNATIVEID FindRingID(false), FindAccessoryID(false);

	for(UINT i = 0; i < sSetItem->byMaxToolTipItemList; ++i)	//파츠 출력 갯수 변경시 여기
	{
		SITEM *sItem = GLogicData::GetInstance().GetItem( sSetItem->vecItemList[i] );
		if ( NULL == sItem ) continue;
		
		//색상
		dwStringColor = ITEM_INFOR_COLOR::dwTrade;//비활성
		for(UINT j = 0; j < sInfoPutOnSetItem.vItemID.size(); ++j)
		{			
			SITEM *sItemTarget = GLogicData::GetInstance().GetItem( sInfoPutOnSetItem.vItemID[j] );

			if( sItemTarget->sSuitOp.emSuit == sItem->sSuitOp.emSuit )
			{
				//반지 중복 활성화 방지용
				if( sItemTarget->sSuitOp.emSuit == SUIT_FINGER )
				{
					//이전에 반지가 검색이 되지 않았다면 아이디 입력 후 활성화 가능
					if( FindRingID != sItemTarget->BasicId()) 
						FindRingID = sItemTarget->BasicId();
					else
						continue;
				}
				
				//장신구 중복 활성화 방지용
				if( sItemTarget->sSuitOp.emSuit == SUIT_ACCESSORIES )
				{
					//이전에 반지가 검색이 되지 않았다면 아이디 입력 후 활성화 가능
					if( FindAccessoryID != sItemTarget->BasicId()) 
						FindAccessoryID = sItemTarget->BasicId();
					else
						continue;
				}
				
				//활성
				dwStringColor = ITEM_INFOR_COLOR::dwDefault;
				break;
			}
		}

		//출력
		if(i)	
		{
			//간격 문자
			m_pItemSet->AddString( nIndex, " / ", ITEM_INFOR_COLOR::dwTrade );	
			
			//그외
			strResult = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_PARTS", sItem->sSuitOp.emSuit) );
			m_pItemSet->AddString( nIndex, strResult.c_str(), dwStringColor );
		}
		else	    
		{
			//처음 출력 " - " 때문에 예외상황 처리		
			nIndex = m_pItemSet->AddText(" - ", ITEM_INFOR_COLOR::dwTrade );

			strResult = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_PARTS", sItem->sSuitOp.emSuit) );
			m_pItemSet->AddString(nIndex, strResult.c_str(), dwStringColor );
		}
	}

}

void CItemInfoTooltip::SetInformationLabelSetItemBonus( const SSETITEM *sSetItem, const SSETITEMSEARCHINFO &sInfoPutOnSetItem )
{
	//방법2. 소스 깔끔하게, for문 2번 사용, vector 공간 미리 할당
	std::vector<std::string> strSetBonus;
	strSetBonus.resize(SLOT_TSIZE);

	//출력할 문자열 생성
	for(BYTE i = 0; i < sSetItem->vecBonus.size(); ++i)
	{
		//사용 가능한 옵션 확인 
		if( GLItemSet::GetInstance().IsUsableOpt(sSetItem->vecBonus[i].sSpec.emSPEC) )
		{    
			UINT nTerms(sSetItem->vecBonus[i].byTerms);

			//처음 표기 할 때만
			if( strSetBonus[nTerms].empty() ) 
				strSetBonus[nTerms] = sc::string::format( "%d %s ", nTerms, ID2GAMEWORD("ITEM_INFO_SET", 0));
			else
				//간격문자
				strSetBonus[nTerms] += ", ";

			//보너스명
			strSetBonus[nTerms] += sc::string::format( "%d ",  ID2GAMEWORD("ITEM_INFO_SET_BONUS", static_cast<int>(sSetItem->vecBonus[i].sSpec.emSPEC)) );

			//수치
			float fVal = sSetItem->vecBonus[i].sSpec.sSPEC.fVAR1;

			if ( fVal> 0.0f) 
				strSetBonus[nTerms] += _T('+');

			if ( GLItemSet::GetInstance().IsPerentOpt( sSetItem->vecBonus[i].sSpec.emSPEC ) )
			{
				strSetBonus[nTerms] += sc::string::format( "%.2f", (fVal * 100.0f) );
				strSetBonus[nTerms] += _T("%");
			}
			else
			{
				strSetBonus[nTerms] += sc::string::format( "%d", static_cast<int>(fVal) );
			}		
		}	
	}
	
	//출력
	for(UINT i = 0; i < strSetBonus.size(); ++i)
	{	
		if(!strSetBonus[i].empty())
		{
			//출력 색상
			DWORD dwStringColor( ITEM_INFOR_COLOR::dwTrade );

			if( i <= sInfoPutOnSetItem.vItemID.size() )
				dwStringColor = ITEM_INFOR_COLOR::dwSetItem;

			m_pItemSet->AddText( strSetBonus[i].c_str(), dwStringColor );
		}
	}

}

void CItemInfoTooltip::SetInformationLabelSetItemPreview(void)
{
	std::string strResult = "";

	strResult = ID2GAMEINTEXT("ITEM_INFO_SET_PREVIEW_TOOLTIP");
	m_pItemSet->AddText( strResult.c_str(), ITEM_INFOR_COLOR::dwDefault );
}

void CItemInfoTooltip::SetInformationLabel5( const SITEMCUSTOM& sItemCustom )
{
    std::string strText = "";

	bool bDisplayComment = false;
	bool bDisplayPrice   = false;
	bool bDisplayReq     = false;

    // Info : 아이템 설명
    {
        if ( (m_pItemData->BasicType() != ITEM_SKILL) || (m_pItemData->BasicType() != ITEM_PET_SKILL) )
        {
            const char* chComment = m_pItemData->GetComment();
            if ( chComment )
                strText = sc::string::format( "%s", chComment );
        }
        // Note : 실행 되지 않는 코드 입니다.
        else
        {
            SNATIVEID sNativeID = sItemCustom.GetBoxWrappedItemID();
            const SITEM* pItemData = GLogicData::GetInstance().GetItem( sNativeID );
            if (!pItemData)
                return;
            SNATIVEID sSkillID = pItemData->sSkillBookOp.sSkill_ID;

            //	Note : 스킬 정보 가져옴.
            PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
            if ( pSkill )
            {
                const char* chDesc = pSkill->GetDesc();
                if ( chDesc )
                    strText = sc::string::format( "%s", chDesc );
            }
        }

        if( strText.length() != 0 )
        {
            m_pItemLabel5->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
            bDisplayComment = true;
        }
    }

    // Info : 거래 및 버리기
    {
        if ( bDisplayComment )
            m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

        //DWORD dwItemFlag = m_pItemData->sBasicOp.dwFlags;
		//DWORD dwItemFlag = sItemCustom.IsFlags();

        DWORD dwTradeColor = ITEM_INFOR_COLOR::dwDefault;

        bool bAllTradeOption = false;
        //if ( (dwItemFlag & TRADE_SALE) && (dwItemFlag & TRADE_THROW) && (dwItemFlag & TRADE_EXCHANGE) )
		if ( sItemCustom.IsSale() && sItemCustom.IsThrow() && sItemCustom.IsExchange() )
            bAllTradeOption = true;

        if ( !bAllTradeOption )
        {
            strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 0) );
            //if ( dwItemFlag & TRADE_SALE )
			if ( sItemCustom.IsSale() )
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
                dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
            }
            else
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
                dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
            }
			int nIndex = m_pItemLabel5->AddText(strText.c_str(), dwTradeColor );
			m_pItemLabel5->AddString(nIndex, " / ", ITEM_INFOR_COLOR::dwDefault );
			
            strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 1) );
            //if ( dwItemFlag & TRADE_THROW )
			if ( sItemCustom.IsThrow() )
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
                dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
            }
            else
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
                dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
            }
			m_pItemLabel5->AddString( nIndex, strText.c_str(), dwTradeColor );
			m_pItemLabel5->AddString(nIndex, " / ", ITEM_INFOR_COLOR::dwDefault );

            strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 6) );
            //if ( dwItemFlag & TRADE_THROW )
			if ( sItemCustom.IsNotStorage() )
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
                dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
            }
            else
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
                dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
            }
			m_pItemLabel5->AddString( nIndex, strText.c_str(), dwTradeColor );
			m_pItemLabel5->AddString(nIndex, " / ", ITEM_INFOR_COLOR::dwDefault );


            strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 3) );
            //if ( dwItemFlag & TRADE_EXCHANGE )
			if ( sItemCustom.IsExchange() )
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
                dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
            }
            else
            {
                //strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
                dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
            }
			m_pItemLabel5->AddString( nIndex, strText.c_str(), dwTradeColor );
			

            // 란 포인트 거래
			if ( GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
			{
				m_pItemLabel5->AddString(nIndex, " / ", ITEM_INFOR_COLOR::dwDefault );
				strText = sc::string::format( "%s ", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 4) );
				int RPTradeCountMax = m_pItemData->sBasicOp.RanPointTradeCount();
				int RPTradeCountNow = sItemCustom.wRanPointTradeCount;

				//if ( dwItemFlag & TRADE_EXCHANGE || 0 < RPTradeCountMax - RPTradeCountNow )
				if ( sItemCustom.IsExchange() && 0 < RPTradeCountMax - RPTradeCountNow )
				{
					//strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
					dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
				}
				else
				{
					//strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
					dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
				}

				if( 0 < RPTradeCountMax )
					strText += sc::string::format( " (%1%/%2%)", RPTradeCountMax-RPTradeCountNow, RPTradeCountMax );

				m_pItemLabel5->AddString( nIndex, strText.c_str(), dwTradeColor );
			}

            // 포장 가능 여부 표시 : 포장지 포장 가능 여부는 기존 거래 및 버리기 옵션과 다르게 포장 가능인 아이템에 한하여 메시지 출력;
            if ( sItemCustom.IsBoxWrappable() )
            {
                m_pItemLabel5->AddString(nIndex, " / ", ITEM_INFOR_COLOR::dwDefault );
                strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 5) );
                dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
                nIndex = m_pItemLabel5->AddString( nIndex, strText.c_str(), dwTradeColor );
            }
        }
        else
        {
            // 포장 가능 여부 표시 : 포장지 포장 가능 여부는 기존 거래 및 버리기 옵션과 다르게 포장 가능인 아이템에 한하여 메시지 출력;
            if ( sItemCustom.IsBoxWrappable() )
            {
                strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 5) );
                dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
                int nIndex = m_pItemLabel5->AddText(strText.c_str(), dwTradeColor );
            }
        }
    }

    // Info : 판매 구입 가격
    {
        if ( m_pItemData->sBasicOp.dwBuyPrice || m_bInPrivateMarket )
        {
            if ( m_bInPrivateMarket )
			{
				bDisplayPrice = true;
                m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

                bool bOPENER;
                DWORD dwPrivateMarketID;
                m_pInterface->GetPrivateMarketInfo( bOPENER, dwPrivateMarketID );
                if ( bOPENER )	//	파는 사람
                {
                    GLPrivateMarket& sPMarket = m_pGaeaClient->GetCharacter()->m_sPMarket;

                    const SSALEITEM* pSALE = sPMarket.GetItem( SNATIVEID( m_wPosX, m_wPosY ) ); 
                    if ( pSALE )
                    {
                        std::string strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pSALE->llPRICE, 3, "," );
                        strText = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 15), strMoney );
                        m_pItemLabel5->AddText( strText.c_str(), MONEYCOLOR::GetMoneyColor(pSALE->llPRICE) );
                    }						
                }
                else		//	사는 사람
                {
                    std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar( dwPrivateMarketID );
                    if ( pCLIENT )
                    {
                        GLPrivateMarket& sPMarket = pCLIENT->m_sPMarket;

                        const SSALEITEM* pSALE = sPMarket.GetItem( SNATIVEID( m_wPosX, m_wPosY ) ); 
                        if ( pSALE )
                        {
                            std::string strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pSALE->llPRICE, 3, "," );
                            strText = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 16), strMoney );			
                            m_pItemLabel5->AddText( strText.c_str(), MONEYCOLOR::GetMoneyColor(pSALE->llPRICE) );
                        }
                    }
                }
            }				
            else if ( m_bShopOpen && m_bInMarket )	// 살때 가격
			{
				bDisplayPrice = true;
                m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

                LONGLONG llNpcSellPrice = 0;

                volatile LONGLONG llPrice = 0;
                volatile double fSHOP_RATE = m_pGaeaClient->GetCharacter()->GetBuyRate();

                if( m_sNpcNativeID.wMainID != 0 && m_sNpcNativeID.wSubID != 0 )
                {
                    const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcNativeID );
                    if( pCrowData != NULL )
                    {
                        LONGLONG dwNpcPrice = pCrowData->GetNpcSellPrice( m_pItemData->sBasicOp.sNativeID.dwID );
                        if( dwNpcPrice == 0 )
                        {								
                            llNpcSellPrice = static_cast<LONGLONG>( m_pItemData->sBasicOp.dwBuyPrice );
                            llPrice = llNpcSellPrice * static_cast<LONGLONG>(fSHOP_RATE) / 100;
                        }
                        else
                        {
                            llNpcSellPrice = dwNpcPrice;
                            llPrice = llNpcSellPrice;								
                        }
                    }

                }				

                std::string strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llPrice, 3, "," );

                strText = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 13), strMoney );

                //	커미션 액수
                DWORD dwCOMMISSION_MONEY = static_cast<DWORD>(llPrice - llNpcSellPrice);

                LONGLONG lMoney  = m_pGaeaClient->GetCharacterLogic().GetInvenMoney();
                D3DCOLOR dwColor = NS_UITEXTCOLOR::RED;

                if( llPrice <= lMoney )
                {
                    dwColor = MONEYCOLOR::GetMoneyColor(llPrice);//NS_UITEXTCOLOR::PALEGREEN;
                }

				if( m_iPageIndex == -1 )
					m_pItemLabel5->AddText( strText.c_str(), dwColor );
            }
            //else if ( m_bShopOpen && (m_pItemData->sBasicOp.dwFlags & TRADE_SALE) ) // 팔때 가격
			else if ( m_bShopOpen && sItemCustom.IsSale() ) // 팔때 가격
			{
				bDisplayPrice = true;
                m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

                volatile float fSHOP_RATE = m_pGaeaClient->GetCharacter()->GetSaleRate();
                volatile float fSALE_DISCOUNT = fSHOP_RATE * 0.01f;

                volatile LONGLONG dwPrice = m_pItemData->GETSELLPRICE ( sItemCustom.wTurnNum );
                volatile LONGLONG dwSALE_PRICE = LONGLONG ( dwPrice * fSALE_DISCOUNT );					

                //	커미션 액수
                volatile LONGLONG dwDISPRICE = m_pItemData->GETSELLPRICE ( sItemCustom.wTurnNum );
                LONGLONG dwCOMMISSION_MONEY = dwDISPRICE - dwSALE_PRICE;

                std::string strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( dwSALE_PRICE, 3, "," );
                strText = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 14), strMoney );

                m_pItemLabel5->AddText( strText.c_str(), MONEYCOLOR::GetMoneyColor((LONGLONG)dwSALE_PRICE) );
            }
        }
    }

	// Info : 요구 포인트
	{
		//  착용조건->요구활동
		if ( m_pItemData->sBasicOp.wReqActPointDW || m_pItemData->sBasicOp.wReqActPointUP )
		{
			if ( !bDisplayPrice && !bDisplayReq )
				m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );
			
			std::string strText;

			int emType = m_pItemData->sBasicOp.emReqActivityType;
			bool bReqActPoint = true;
			
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				strText = sc::string::format( "%s :%s(%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 3), 
					CGameTextMan::GetInstance().GetCommentText("ACTIVITY_CLASS", (INT)emType).GetString(), m_pItemData->sBasicOp.wReqActPointDW );
			}
			else
			{
				strText = sc::string::format( "%s :%s(%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 3), 
					COMMENT::ACTIVITY_CLASS[emType].c_str(), m_pItemData->sBasicOp.wReqActPointDW );
			}

			if ( m_pItemData->sBasicOp.wReqActPointUP )
			{
				strText += sc::string::format( "~%d)",m_pItemData->sBasicOp.wReqActPointUP );
				bReqActPoint = (m_pItemData->sBasicOp.wReqActPointUP >= m_pGaeaClient->GetCharacter()->m_llActivityPoint[emType] );
			}
			else
			{
				strText += ")";
			}

			bReqActPoint = bReqActPoint && (m_pItemData->sBasicOp.wReqActPointDW <= m_pGaeaClient->GetCharacter()->m_llActivityPoint[emType] );

			DWORD dwColor = 0;

			if ( bReqActPoint )
				dwColor = ITEM_INFOR_COLOR::dwDefault;
			else
				dwColor = ITEM_INFOR_COLOR::dwReqWrong;

			m_pItemLabel5->AddText( strText.c_str(), dwColor );

			bDisplayReq = true;
		}

		//  착용조건->기여도
		if ( m_pItemData->sBasicOp.dwReqContribution )
		{
			if ( !bDisplayPrice && !bDisplayReq )
				m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

			bool bReqPoint = true;

			std::string strText;
			strText = sc::string::format( "%s :%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 4), m_pItemData->sBasicOp.dwReqContribution );

			bReqPoint = (m_pItemData->sBasicOp.dwReqContribution <= (DWORD)m_pGaeaClient->GetCharacter()->m_nContributionPoint);

			DWORD dwColor = 0;

			if ( bReqPoint )
				dwColor = ITEM_INFOR_COLOR::dwDefault;
			else
				dwColor = ITEM_INFOR_COLOR::dwReqWrong;

			m_pItemLabel5->AddText( strText.c_str(), dwColor );

			bDisplayReq = true;
		}
	}

    // Info : 만료일
    {
        // 코스츔 기간 표시
        if ( sItemCustom.tDISGUISE != 0 )
        {				
            CTime cTime(sItemCustom.tDISGUISE);

            //	서버 타임으로 변환( TimeZone 계산 )
            m_pGaeaClient->GetConvertServerTime( cTime );

            if ( cTime.GetYear() > 1970 )
            {
				m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

                std::string strExpireDate;

                strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
                    (cTime.GetYear()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

                strText = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 7), strExpireDate );			
                m_pItemLabel5->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwLimitTime );
            }
        }

        //	기간표시
        //if ( m_pItemData->IsTIMELMT() )
		if ( sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
        {
            CTime cTime(sItemCustom.tBORNTIME);

            //	서버 타임으로 변환( TimeZone 계산 )
            m_pGaeaClient->GetConvertServerTime( cTime );

            if ( cTime.GetYear() > 1970 )
            {
				m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

                CTimeSpan sLMT(m_pItemData->sDrugOp.tTIME_LMT);
                cTime += sLMT + sItemCustom.GetPeriodExBasic();

                std::string strExpireDate;
                strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
                    (cTime.GetYear ()%2000), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

				INT nPriodExMaxCount(-1);
				if ( ItemPeriodExtension::Manager::GetInstance().IsRegItem(sItemCustom.GetNativeID(), nPriodExMaxCount) )
				{
					if ( nPriodExMaxCount == 0 )
						strExpireDate.append( ID2GAMEWORD( "ITEM_PERIODEX_TOOLTIP", 0 ) );
					else
						strExpireDate.append( sc::string::format(ID2GAMEWORD( "ITEM_PERIODEX_TOOLTIP", 1 ), sItemCustom.cPeriodExtensionCount - nPriodExMaxCount ) );
				}

                strText = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 4), strExpireDate );			
                m_pItemLabel5->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwLimitTime );
            }
        }
		// 코스튬 능력치 시간
		if( sItemCustom.costumeUser.tEndTime > 0 )
		{
			CTime cTime(sItemCustom.costumeUser.tEndTime);

			//	서버 타임으로 변환( TimeZone 계산 )
			m_pGaeaClient->GetConvertServerTime( cTime );

			if ( cTime.GetYear() > 1970 )
			{
				m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

				std::string strExpireDate;

				strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
					(cTime.GetYear()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

				strText = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 20), strExpireDate );			
				m_pItemLabel5->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwLimitTime );
			}
		}
		// 일시강화 만료일
		if( sItemCustom.sTLGrind.tFireTime > 0 )
		{
			CTime cTime(sItemCustom.sTLGrind.tFireTime);

			//	서버 타임으로 변환( TimeZone 계산 )
			m_pGaeaClient->GetConvertServerTime( cTime );

			if ( cTime.GetYear() > 1970 )
			{
				m_pItemLabel5->AddText( "", ITEM_INFOR_COLOR::dwDefault );

				std::string strExpireDate;

				strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
					(cTime.GetYear()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

				int iValue = 0;

				if ( m_pItemData->sSuitOp.gdDamage == GLPADATA(0,0) )
				{
					iValue = sItemCustom.cDEFENSE;
				}
				else
				{
					iValue = sItemCustom.cDAMAGE;
				}

				strText = sc::string::format( "%s : (+%d) %s", ID2GAMEWORD("ITEM_BASIC_INFO", 22), iValue, strExpireDate );			
				m_pItemLabel5->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwLimitTime );
			}
		}
    }
}

void CItemInfoTooltip::SetInformationLabel6( const SITEMCUSTOM& sItemCustom )
{
	std::string strText = "";
	std::string strTemp = "";
		
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcNativeID );

	if ( pCrowData && m_iPageIndex >= 0 )
	{
		strText = sc::string::format( "%s", ID2GAMEWORD("TRADE_ITEM_CONDITION", 0) );	

		m_pItemLabel6->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );

		GLInventorySale* rInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven( m_iPageIndex ));

		std::vector<sExNeed> rList;	

		EEXTYPE exType = rInven->GetExchangeList(sItemCustom.GetNativeID(), m_wPosX, m_wPosY, rList );

		std::vector<sExNeed>::iterator _iter = rList.begin();

		switch( exType )
		{
		case EX_POINT_TYPE :
			{				
				strText = "";

				__int64 iRanPt64 = m_pCharacter->PointShopPoint();

				D3DCOLOR dwColor = ITEM_INFOR_COLOR::dwDefault;
									
				while( _iter != rList.end() )
				{
					if( iRanPt64 < _iter->iNeedCount )
						dwColor = NS_UITEXTCOLOR::RED;

					strText = sc::string::format( "%s%s ", strText, NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( _iter->iNeedCount, 3, "," ));

					++_iter;
				}
								
				strTemp = sc::string::format( "%s", ID2GAMEWORD("TRADE_ITEM_CONDITION", 1) );

				int iIdx = m_pItemLabel6->AddText( strTemp.c_str(), dwColor );
				
				m_pItemLabel6->AddString(iIdx, strText.c_str(), dwColor );
			}
			break;
		case EX_LVP_TYPE   :
			{				
				strText = "";

				__int64 iLvPt64 = m_pCharacter->m_nLiving;

				D3DCOLOR dwColor = ITEM_INFOR_COLOR::dwDefault;

				while( _iter != rList.end() )
				{
					if( iLvPt64 < _iter->iNeedCount )
						dwColor = NS_UITEXTCOLOR::RED;

					strText = sc::string::format( "%s%d ", strText, NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( _iter->iNeedCount, 3, "," ));

					++_iter;
				}

				strTemp = sc::string::format( "%s", ID2GAMEWORD("TRADE_ITEM_CONDITION", 2) );

				int iIdx = m_pItemLabel6->AddText( strTemp.c_str(), dwColor );

				m_pItemLabel6->AddString(iIdx, strText.c_str(), dwColor );
			}
			break;
		}
	}	
}

void CItemInfoTooltip::DisplayWishListItemInfo( WishList::ItemSPtr pWishItem )
{
	m_pWishItem = pWishItem;

	if( !m_pWishItem )
		return;

	switch( pWishItem->GetType() )
	{
	case WishList::ETypeNpcStoreEx :
		{
			WishList::SItemNpcExStore* pWishStore = dynamic_cast<WishList::SItemNpcExStore*>(pWishItem.get());

			if( pWishStore ) {
				DisplayItemInformation(
					pWishStore->m_sItem,
					FALSE, FALSE, FALSE, FALSE, FALSE,
					pWishStore->m_iX, pWishStore->m_iY,
					pWishStore->m_sCrowID, pWishStore->m_iPage );
			}
		}
		break;
	default :
		{
			DisplayItemInformation(
				m_pWishItem->m_sItem,
				FALSE, FALSE, FALSE, FALSE, FALSE,
				USHRT_MAX, USHRT_MAX,
				NATIVEID_NULL() );
		}
		break;
	}
	
}

void CItemInfoTooltip::DisplayWishListItemInfo( SITEMCUSTOM& sItemcustom )
{
	m_sItemcustom = sItemcustom;

	if( !sItemcustom.GetNativeID().IsValidNativeID() )
		return;

	DisplayItemInformation(
		sItemcustom,
		FALSE, FALSE, FALSE, FALSE, FALSE,
		USHRT_MAX, USHRT_MAX,
		NATIVEID_NULL() );
}

void CItemInfoTooltip::ResetInformation()
{
    m_pItemImage->ResetItem();
    m_pItemName->ClearText();
    m_pItemLabel1->ClearText();
    m_pItemLabel2->ClearText();
    m_pItemLabel3->ClearText();
    m_pItemLabel4->ClearText();
    m_pItemLabel5->ClearText();
	m_pItemLabel6->ClearText();

	m_pItemSet->ClearText();

	m_pWishListLine->SetVisibleSingle( FALSE );
	m_pWishListItem->ClearText();

    m_pResistSpirit->SetVisibleSingle( FALSE );
    m_pResistFire->SetVisibleSingle( FALSE );
    m_pResistIce->SetVisibleSingle( FALSE );
    m_pResistElectric->SetVisibleSingle( FALSE );
    m_pResistPoison->SetVisibleSingle( FALSE );

    m_pResistSpiritText->ClearText();
    m_pResistFireText->ClearText();
    m_pResistIceText->ClearText();
    m_pResistElectricText->ClearText();
    m_pResistPoisonText->ClearText();

	m_pWishItem.reset();

    m_bDisplayResist = false;
}

void CItemInfoTooltip::DisplaySkillItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom )
{
    std::string strText = "";

    SNATIVEID sNativeID = sItemCustom.GetNativeID();
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	
    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sNativeID );
    SNATIVEID sSkillID = pItemData->sSkillBookOp.sSkill_ID;

    //	Note : 스킬 정보 가져옴.
    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
    if ( pSkill )
    {

        {
            //	목표 가능거리, 왼손, 오른손 도구
            const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;		

            if ( sBASIC.wTARRANGE )
            {
                strText = sc::string::format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_TARGET_RANGE", 0), sBASIC.wTARRANGE);
                m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
            }

            if ( sBASIC.emUSE_LITEM )
            {
                if ( sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE )
                {
                    strText = sc::string::format( "%s :%s", 
                        ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 0), 
                        ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_LITEM - 1 ) );

                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                }
            }

            if ( sBASIC.emUSE_RITEM )
            {
                if ( sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE )
                {
                    strText = sc::string::format( "%s :%s", 
                        ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 1), 
                        ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_RITEM - 1 ) );

                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                }
            }
        }

        //	이미 배운 스킬
        if ( pCharacter->GETLEARNED_SKILL ( sSkillID ) )
        {
            m_pTextBox->AddText( ID2GAMEWORD( "ITEM_SKILL_CONDITION", 0 ), ITEM_INFOR_COLOR::dwReqWrong );
            return ;
        }

        //	요구치 정보
        {
            m_pTextBox->AddText( ID2GAMEWORD( "ITEM_SKILL_CONDITION", 3 ), ITEM_INFOR_COLOR::dwDefault );	

            const WORD wLevel = 0;
            SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
            SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[wLevel];

            //	1. 요구보유스킬
            SNATIVEID NeedSkillID = pSkill->m_sLEARN.sSKILL;			
            if ( NeedSkillID != NATIVEID_NULL() )
            {
                BOOL bVALID = FALSE;
                BOOL bNeedSkillLevel = FALSE;

                std::string strNeedSkillName;
                std::string strNeedSkillLevel;

                PGLSKILL pNeedSkill = GLSkillMan::GetInstance().GetData( NeedSkillID.wMainID, NeedSkillID.wSubID );
                if ( pNeedSkill ) strNeedSkillName = sc::string::format("%s :%s", ID2GAMEWORD( "SKILL_ADVANCED_INFO", 0), pNeedSkill->GetName() );
                bVALID = pCharacter->ISLEARNED_SKILL( NeedSkillID );			

                //	2. 요구보유스킬레벨
                if ( 0 < sLVL.dwSKILL_LVL )
                {
                    strNeedSkillLevel = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 1), (sLVL.dwSKILL_LVL + 1) );
                    bNeedSkillLevel = TRUE;

                    SCHARDATA2::SKILL_MAP& map = pCharacter->m_ExpSkills;				
                    SCHARDATA2::SKILL_MAP_ITER iter = map.find ( NeedSkillID.dwID );
                    if ( iter != map.end() )
                    {
                        SCHARSKILL& rCharSkill = (*iter).second;

                        //	색 조절
                        bVALID = rCharSkill.wLevel >= sLVL.dwSKILL_LVL;						
                    }
                }

                if ( bVALID )
                    m_pTextBox->AddText( strNeedSkillName.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strNeedSkillName.c_str(), ITEM_INFOR_COLOR::dwReqWrong );

                if ( bNeedSkillLevel )
                {
                    if ( bVALID )
                        m_pTextBox->AddText( strNeedSkillLevel.c_str(), ITEM_INFOR_COLOR::dwDefault );
                    else
                        m_pTextBox->AddText( strNeedSkillLevel.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
                }
            }

            //	3. 요구경험치
            if ( 0 < sLVL.dwSKP )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 2), sLVL.dwSKP);

                if ( pCharacter->m_dwSkillPoint >= sLVL.dwSKP )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }

            //	4. 요구레벨
            if ( 0 < sLVL.dwLEVEL )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 3), sLVL.dwLEVEL);

                if ( pCharacter->GETLEVEL() >= int(sLVL.dwLEVEL) )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }

            //	Stats
            //	1. 요구힘
			//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
			const SCHARSTATS& rCharStats= pCharacter->m_sSUMSTATS_SKILL;
            if ( 0 < sLVL.sSTATS.wPow )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 0), sLVL.sSTATS.wPow);

                if ( rCharStats.wPow >= sLVL.sSTATS.wPow )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }

            //	2. 요구체력
            if ( 0 < sLVL.sSTATS.wStr )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 1), sLVL.sSTATS.wStr);

                if ( rCharStats.wStr >= sLVL.sSTATS.wStr )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }

            //	3. 요구정신
            if ( 0 < sLVL.sSTATS.wSpi )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 2), sLVL.sSTATS.wSpi);

                if ( rCharStats.wSpi >= sLVL.sSTATS.wSpi )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }

            //	4. 요구민첩
            if ( 0 < sLVL.sSTATS.wDex )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 3), sLVL.sSTATS.wDex);

                if ( rCharStats.wDex >= sLVL.sSTATS.wDex )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }

            //	5. 요구지력
            if ( 0 < sLVL.sSTATS.wInt )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 4), sLVL.sSTATS.wInt);

                if ( rCharStats.wInt >= sLVL.sSTATS.wInt )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }

            //	6. 요구근력
            if ( 0 < sLVL.sSTATS.wSta )
            {
                strText = sc::string::format("%s :%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 5), sLVL.sSTATS.wSta);

                if ( rCharStats.wPow >= sLVL.sSTATS.wSta )
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
                else
                    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwReqWrong );
            }
        }
    }
}

void CItemInfoTooltip::DisplayPetSkillItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom )
{
    std::string strText = "";

    SNATIVEID sNativeID = sItemCustom.GetNativeID();
    GLPetClient* pPetClient = m_pGaeaClient->GetPetClient();
    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sNativeID );
    SNATIVEID sSkillID = pItemData->sSkillBookOp.sSkill_ID;

    //	Note : 스킬 정보 가져옴.
    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
    if ( pSkill )
    {
        //	이미 배운 스킬
        if ( pPetClient->ISLEARNED_SKILL( sSkillID ) )
            m_pTextBox->AddText( ID2GAMEWORD( "ITEM_SKILL_CONDITION", 0 ), ITEM_INFOR_COLOR::dwReqWrong );
    }
}

void CItemInfoTooltip::DisplayVehicleItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom )
{
    std::string strText = "";

    //strText = sc::string::format( "%s %s", ID2GAMEWORD ("ITEM_VEHICLE_NAME",0), COMMENT::VEHICLE_TYPE[pItemData->sVehicle.emVehicleType].c_str() );
    //m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );

    bool bInfo = true;


    VEHICLEITEMINFO_MAP_ITER iter = m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfo.find ( sItemCustom.m_VehicleDbNum );
    if ( iter == m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfo.end() )
    {
        iter = m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfoTemp.find ( sItemCustom.m_VehicleDbNum );
        if (iter == m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfoTemp.end())
            bInfo = false;
    }


    float fAddOnSpeed = 0.0f;

    if ( bInfo ) 
    {
        SVEHICLEITEMINFO sVehicleItemInfo = (*iter).second;

        for ( int i = 0; i < ACCE_TYPE_SIZE; ++i ) 
        {							
            fAddOnSpeed += sVehicleItemInfo.m_PutOnItems[i].GETMOVESPEED();
        }
    }

    if ( bInfo ) 
    {
        SVEHICLEITEMINFO sVehicleItemInfo = (*iter).second;

        for ( int i = 0; i < ACCE_TYPE_SIZE; ++i ) 
        {							
            const SITEM* pItem = GLogicData::GetInstance().GetItem ( sVehicleItemInfo.m_PutOnItems[i].GetNativeID() );
            if ( pItem )
            {
                strText = sc::string::format("%s %s", ID2GAMEWORD ("ITEM_VEHICLE_SLOT", i), pItem->GetName() );

                m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
            }
        }

        strText = sc::string::format( "%s", ID2GAMEWORD ("ITEM_VEHICLE_BOOST_LEARN", sVehicleItemInfo.m_dwBoosterID ) );
        m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );

        strText = sc::string::format( "%s %.2f%%", ID2GAMEWORD ("ITEM_VEHICLE_BATTERY", 0), sVehicleItemInfo.m_nFull/10.0f );
        m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
    }
}

void CItemInfoTooltip::DisplayPetCardItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom )
{
    std::string strText = "";

    if ( sItemCustom.PetDbNum() == 0 )
        return;

    PETCARDINFO_MAP_ITER iter = m_pGaeaClient->GetCharacter()->m_mapPETCardInfo.find( sItemCustom.PetDbNum() );
    if ( iter == m_pGaeaClient->GetCharacter()->m_mapPETCardInfo.end() )
    {
        iter = m_pGaeaClient->GetCharacter()->m_mapPETCardInfoTemp.find ( sItemCustom.PetDbNum() );
        if ( iter == m_pGaeaClient->GetCharacter()->m_mapPETCardInfoTemp.end() ) return;
    }

    SPETCARDINFO sPetCardInfo = (*iter).second;

    //strText = sc::string::format( "%s %s", ID2GAMEWORD("ITEM_PET_NAME",0), sPetCardInfo.m_szName );
    //m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string>	vecPET_TYPE;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("PET_TYPE", vecPET_TYPE, CGameTextMan::EM_COMMENT_TEXT);
		strText = sc::string::format( "%s %s", ID2GAMEWORD("ITEM_PET_TYPE", 0), vecPET_TYPE[sPetCardInfo.m_emTYPE].c_str() );
	}
	else
	{
		strText = sc::string::format( "%s %s", ID2GAMEWORD("ITEM_PET_TYPE", 0), COMMENT::PET_TYPE[sPetCardInfo.m_emTYPE].c_str() );
	}
    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );

    strText = sc::string::format( "%s %.2f%%", ID2GAMEWORD("ITEM_PET_FULL", 0), sPetCardInfo.m_nFull/10.0f );
    m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );

    if ( 2 <= sPetCardInfo.m_wMaxSkillSlot )
    {
        strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_PET_DUALSKILL", 1) );
        m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
    }
    else
    {
        strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_PET_DUALSKILL", 0) );
        m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
    }

    const SITEM* pItem = GLogicData::GetInstance().GetItem( sPetCardInfo.m_PutOnItems[ACCETYPEA].GetNativeID() );
    if ( pItem )
    {
        strText = sc::string::format("%s %s", ID2GAMEWORD("ITEM_PET_SLOTA", 0), pItem->GetName() );
        m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
    }

    pItem = GLogicData::GetInstance().GetItem( sPetCardInfo.m_PutOnItems[ACCETYPEB].GetNativeID() );
    if ( pItem )
    {
        strText = sc::string::format("%s %s", ID2GAMEWORD("ITEM_PET_SLOTB", 0), pItem->GetName() );
        m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
    }

    if ( sItemCustom.tDISGUISE != 0 )
    {
        const CTime& currentTime = m_pGaeaClient->GetCurrentTime();
        CTime startTime   = sItemCustom.tBORNTIME;
        CTimeSpan timeSpan = currentTime - startTime;
        if( timeSpan.GetTotalSeconds() < sItemCustom.tDISGUISE )
        {
            strText = sc::string::format("[%s]", ID2GAMEWORD("ITEM_PET_USE_SKINPACK", 0) );
            m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
            strText = sc::string::format("%s : %dsec", ID2GAMEWORD("CLUB_BATTLE_TIME", 0), sItemCustom.tDISGUISE - timeSpan.GetTotalSeconds() );
            m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
    }

    bool bDisplayEnablePetSkill = false;
    PETSKILL_MAP_CITER pos = sPetCardInfo.m_ExpSkills.begin();
    PETSKILL_MAP_CITER pos_end = sPetCardInfo.m_ExpSkills.end();
    for ( ;pos != pos_end; ++pos )
    {
        const PETSKILL& sPetSkill = (*pos).second;
        PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( sPetSkill.sNativeID );
        if ( pSKILL )
        {
            if ( !bDisplayEnablePetSkill )
            {
                bDisplayEnablePetSkill = true;
                m_pTextBox->AddText( ID2GAMEWORD("ITEM_PET_SKILLS", 0 ), ITEM_INFOR_COLOR::dwDefault );
            }

            strText = sc::string::format("%s", pSKILL->GetName() );
            m_pTextBox->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
    }
}

void CItemInfoTooltip::DisplayLunchBoxItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom )
{
    std::string strText = "";

    SNATIVEID sNativeID = sItemCustom.GetNativeID();
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	
    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sNativeID );
    SNATIVEID sSkillID = m_pItemData->sSkillBookOp.sSkill_ID;
    WORD wSkillLevel = m_pItemData->sSkillBookOp.wSkill_Level;

    //	Note : 스킬 정보 가져옴.
    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
    if ( !pSkill )
    {
        return;
    }

    // Note : 효과정보 출력
    CString strImpact;
    pSkill->GetDescFormatByLevel( strImpact, wSkillLevel, GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );
    m_pTextBox->AddText( strImpact, ITEM_INFOR_COLOR::dwDefault );
}

void CItemInfoTooltip::DisplayPetDualSkillItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom )
{
    GLPetClient* pPetClient = m_pGaeaClient->GetPetClient();

    //	이미 듀얼 스킬을 배웠다면
    if ( 2 <= pPetClient->m_wMaxSkillSlot )
        m_pTextBox->AddText( ID2GAMEWORD( "ITEM_SKILL_CONDITION", 0 ), ITEM_INFOR_COLOR::dwReqWrong );
}

void CItemInfoTooltip::DisplayWishListInfo()
{
	if( !m_pWishItem )
		return;

	m_pWishListLine->SetVisibleSingle( TRUE );

	CString strText( ID2GAMEWORD( "WISHLIST_TOOLTIP", 0 ) );
	m_pWishListItem->SetText( strText, D3DCOLOR_XRGB( 12, 255, 0 ) );

	BYTE nType = m_pWishItem->GetType();
	switch( nType )
	{
	case WishList::ETypeNpcStoreEx:
		{
			WishList::SItemNpcExStore* pItem = dynamic_cast< WishList::SItemNpcExStore* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText.Format(
				"%s (%s / %d, %d)",
				GLogicData::GetInstance().GetCrowName( pItem->m_sCrowID ).c_str(),
				m_pGaeaClient->GetMapName( pItem->m_sMapID ),
				pItem->m_nMapX,
				pItem->m_nMapY );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;
	case WishList::ETypeNpcStore:
		{
			WishList::SItemNpcStore* pItem = dynamic_cast< WishList::SItemNpcStore* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText.Format(
				"%s (%s / %d, %d)",
				GLogicData::GetInstance().GetCrowName( pItem->m_sCrowID ).c_str(),
				m_pGaeaClient->GetMapName( pItem->m_sMapID ),
				pItem->m_nMapX,
				pItem->m_nMapY );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );

			strText.Format(
				"%s : %s",
				ID2GAMEWORD( "ITEM_BASIC_INFO", 14 ),
				NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ) );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypePointStore:
		{
			WishList::SItemPointStore* pItem = dynamic_cast< WishList::SItemPointStore* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = ID2GAMEWORD( "WISHLIST_TOOLTIP", 1 );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );

			strText.Format(
				"%s : %s",
				ID2GAMEWORD( "ITEM_BASIC_INFO", 14 ),
				NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ) );
			m_pWishListItem->AddText( strText, D3DCOLOR_XRGB( 0, 192, 255 ) );
		}
		break;

	case WishList::ETypePrivatePerson:
		{
			WishList::SItemPrivatePerson* pItem = dynamic_cast< WishList::SItemPrivatePerson* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = sc::string::format(
				ID2GAMEWORD( "WISHLIST_TOOLTIP", 2 ),
				pItem->m_strCharName.c_str() ).c_str();
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );

			strText.Format(
				"%s : %s",
				ID2GAMEWORD( "ITEM_BASIC_INFO", 14 ),
				NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ) );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypeChatLink:
		{
			WishList::SItemChatLink* pItem = dynamic_cast< WishList::SItemChatLink* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = sc::string::format(
				ID2GAMEWORD( "WISHLIST_TOOLTIP", 3 ),
				pItem->m_strCharName.c_str() ).c_str();
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypeActivate:
		{
			WishList::SItemActivate* pItem = dynamic_cast< WishList::SItemActivate* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText.Format(
				"%s : %s",
				ID2GAMEWORD( "ITEM_BASIC_INFO", 14 ),
				NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ) );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypeTrade:
		{
			WishList::SItemTrade* pItem = dynamic_cast< WishList::SItemTrade* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = sc::string::format(
				ID2GAMEWORD( "WISHLIST_TOOLTIP", 4 ),
				pItem->m_strCharName.c_str() ).c_str();
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypeReferChar:
		{
			WishList::SItemReferChar* pItem = dynamic_cast< WishList::SItemReferChar* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = sc::string::format(
				ID2GAMEWORD( "WISHLIST_TOOLTIP", 5 ),
				pItem->m_strCharName.c_str() ).c_str();
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypeInventory:
		{
			WishList::SItemInventory* pItem = dynamic_cast< WishList::SItemInventory* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = ID2GAMEWORD( "WISHLIST_TOOLTIP", 6 );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypeBank:
		{
			WishList::SItemBank* pItem = dynamic_cast< WishList::SItemBank* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = ID2GAMEWORD( "WISHLIST_TOOLTIP", 7 );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;

	case WishList::ETypeLocker:
		{
			WishList::SItemLocker* pItem = dynamic_cast< WishList::SItemLocker* >( m_pWishItem.get() );
			if( !pItem )
				return;

			strText = ID2GAMEWORD( "WISHLIST_TOOLTIP", 8 );
			m_pWishListItem->AddText( strText, ITEM_INFOR_COLOR::dwDefault );
		}
		break;
	}
}

void CItemInfoTooltip::CutZero( CString& cstr, int ncount  )
{
	// '.' 점이 없다는 것은 소수점 이하 자리가 없으므로 리턴
	if (cstr.Find('.') == -1)
		return;

	CString temp = cstr;

	//ncount 소숫점 몇째 자리 까지 잘라낼찌 정한다
	for (int i = 0; i < ncount + 1; i++ )
	{
		temp = cstr.Right( 1 );

		if ( temp.Find('0') != -1 || temp.Find('.') != -1 )
			cstr = cstr.Left( cstr.GetLength() - 1 );
		else
			break;
	}
}

void CItemInfoTooltip::SetBGAlpa( WORD wValue )
{
	m_pLineBox->SetDiffuseAlpha(wValue);
}

void CItemInfoTooltip::SetGroup()
{
    m_bGroup = true;
}

void CItemInfoTooltip::SetVisibleSingle( BOOL bVisible )
{
    if ( bVisible == FALSE )
    {
        if ( m_bGroup )
        {
            if( m_pWishItem )
            {
                m_pInterface->CloseItemToolTipMulti( GetWndID() );
            }
            else if ( m_sItemcustom.GetNativeID().IsValidNativeID() )
            {
                m_pInterface->CloseItemToolTipMulti( GetWndID() );
            }
            else
            {
                m_pInterface->UiHideGroup( GetWndID() );
            }

            m_bGroup = false;
        }
    }

    CUIGroup::SetVisibleSingle( bVisible );
}

void CItemInfoTooltip::ResetDurabilityValue( float fResetValue /* = 1.0f */ )
{
	for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
	{
		m_fDurabilityValue[i] = fResetValue;
	}
}