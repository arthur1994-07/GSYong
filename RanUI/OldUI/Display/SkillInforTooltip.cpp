#include "StdAfx.h"
#include "../../../RanLogic/GLSkillToolTip.h" //봉주
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Skill/SkillImage.h"
#include "./SkillInforTooltip.h"

#include "../../../RanLogic/Skill/GLSkill.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------




const float fMOUSEPOINT_GAP = 20.0f;
const float fReSizeOffset = 10.0f;
const float fResizeEndOffset = 5.0f;
const float fResistOffset = 5.0f;

CSkillInfoTooltip::CSkillInfoTooltip( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIGroup(pEngineDevice)

	, m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
    , m_pOthertMultiSkillTooltip( NULL )
	, m_pLinkSkillTooltip( NULL )
	, m_pDealyActionSkillTooltip ( NULL )
	, m_pClose( NULL )
	, m_pLineBox(NULL)
	, m_pDummyControl(NULL)
	, m_pDummyNameControl(NULL)
	, m_pDummyLabel8Control(NULL)
	, m_pSkillImage(NULL)

	, m_pSkillName(NULL)
	, m_pSkillLabel(NULL)
	, m_pSkillLabel0(NULL)
	, m_pSkillLabel1(NULL)
	, m_pSkillLabel2(NULL)
	, m_pSkillLabel3(NULL)
	, m_pSkillLabel4(NULL)
	, m_pSkillLabel5(NULL)
	, m_pSkillLabel6(NULL)
	, m_pSkillLabel7(NULL)
	, m_pSkillLabel8(NULL)

	, m_bBlockMousetracking( false )
	, m_vMousePointGap( fMOUSEPOINT_GAP, fMOUSEPOINT_GAP )

	, m_PosX( 0 )
	, m_PosY( 0 )
	, m_vDragPos( 0.0f, 0.0f )

	, m_pTarget( NULL )
	, m_pSkillData( NULL )
	, m_pCharSkill( NULL )
    , m_wLevel(0)
    , m_wShowLevel(0)
	, m_bNextLevel( false )
{
}

CSkillInfoTooltip::~CSkillInfoTooltip()
{
}

void CSkillInfoTooltip::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SKILL_INFOR_TOOLTIP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SKILL_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pLineBox = pLinBoxSmart;
		m_pLineBox->SetDiffuseAlpha(180);
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SKILL_INFOR_TOOLTIP" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pDummyControl = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_NAME" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pDummyNameControl = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pDummyLabel8Control = pControl;
    }

    {
        CSkillImage* pSkillImage = new CSkillImage( m_pGaeaClient, m_pEngineDevice );
        pSkillImage->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_IMAGE", UI_FLAG_DEFAULT, SKILL_IMAGE );
        pSkillImage->CreateSubControl();
        pSkillImage->SetUseRender( TRUE );
        RegisterControl( pSkillImage );
        m_pSkillImage = pSkillImage;
    }

	CD3DFontPar* pFont12 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 12, _DEFAULT_FONT_SHADOW_EX_FLAG );

	{
		CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL" );
		pBasicTextBox->SetFont( pFont12 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		pBasicTextBox->SetDiffuse(NS_UITEXTCOLOR::DODGERBLUE);
		RegisterControl( pBasicTextBox );
		m_pSkillLabel = pBasicTextBox;
	}

	{
		CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL0" );
		pBasicTextBox->SetFont( pFont12 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pBasicTextBox );
		m_pSkillLabel0 = pBasicTextBox;
	}

    {
        CD3DFontPar* pFont12 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 12, D3DFONT_BOLD | D3DFONT_SHADOW_EX | D3DFONT_KSC5601 );

        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_NAME" );
        pBasicTextBox->SetFont( pFont12 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pBasicTextBox );
        m_pSkillName = pBasicTextBox;
    }

    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL1" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pSkillLabel1 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pSkillLabel2 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pSkillLabel3 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pSkillLabel4 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pSkillLabel5 = pBasicTextBox;
    }

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pSkillLabel6 = pBasicTextBox;
    }

	{
		CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pSkillLabel7 = pBasicTextBox;
	}

    {
        CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
        pBasicTextBox->CreateSub( this, "SKILL_INFOR_TOOLTIP_SKILL_LABEL2" );
        pBasicTextBox->SetFont( pFont9 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pBasicTextBox );
        m_pSkillLabel8 = pBasicTextBox;
    }

	 m_rcpSkillImage = m_pSkillImage->GetLocalPos();
	 m_rcSkillName   = m_pSkillName->GetLocalPos();
	 m_rcDummuname   = m_pDummyNameControl->GetLocalPos();
	 m_rcSkillLabel0 = m_pSkillLabel0->GetLocalPos();
	 m_rcSkillLabel1 = m_pSkillLabel1->GetLocalPos();
	 m_rcSkillLabel2 = m_pSkillLabel2->GetLocalPos();
}

void CSkillInfoTooltip::CreateCloseButton()
{
	m_pClose = new CBasicButton( m_pEngineDevice );
	m_pClose->CreateSub( this, "ITEM_INFOR_TOOLTIP_CLOSEBUTTON", UI_FLAG_LEFT | UI_FLAG_TOP, CLOSE_BUTTON );
	m_pClose->CreateFlip( "INFO_DISPLAY_LINK_CLOSE_F", CBasicButton::CLICK_FLIP );
	m_pClose->CreateMouseOver( "INFO_DISPLAY_LINK_CLOSE_OVER" );
	RegisterControl( m_pClose );
}

void CSkillInfoTooltip::AllControlRePos( int x, int y )
{
    const UIRECT& rcTextBoxLocalPos = m_pSkillLabel2->GetLocalPos();

    const UIRECT& rcLocalPosDummy = m_pDummyControl->GetLocalPos();
    const UIRECT& rcOriginPos = GetLocalPos ();

    AlignSubControl( rcOriginPos, rcLocalPosDummy );

    float fOriginSkillNameSizeX = m_pDummyNameControl->GetLocalPos().sizeX;
    long lSkillNameLength = m_pSkillName->GetLongestLine();

    UIRECT rcNewUIPos;
    if ( static_cast<long>(fOriginSkillNameSizeX) < lSkillNameLength )
    {
        // 스킬 이름 텍스트 박스 크기 설정.
        UIRECT rcNewSKillName = m_pSkillName->GetLocalPos();
        rcNewSKillName.sizeX = static_cast<float>(lSkillNameLength);
        m_pSkillName->SetLocalPos( rcNewSKillName );

        // 스킬 설명 텍스트 박스 크기 설정.
        UIRECT rcNewLabel8 = m_pDummyLabel8Control->GetLocalPos();
        rcNewLabel8.sizeX = static_cast<float>(lSkillNameLength);
        m_pSkillLabel8->SetLocalPos( rcNewLabel8 );

        // UI 크기 설정.
        rcNewUIPos = rcLocalPosDummy;
        rcNewUIPos.sizeX += lSkillNameLength - fOriginSkillNameSizeX;
    }
    else
    {
        // 스킬 이름 텍스트 박스 크기 설정.
        const UIRECT& rcDummySkillName = m_pDummyNameControl->GetLocalPos();
        m_pSkillName->SetLocalPos( rcDummySkillName );

        // 스킬 설명 텍스트 박스 크기 설정.
        UIRECT rcNewLabel8 = m_pDummyLabel8Control->GetLocalPos();
        m_pSkillLabel8->SetLocalPos( rcNewLabel8 );

        // UI 크기 설정.
        rcNewUIPos = rcLocalPosDummy;
    }

    float fPosY = rcTextBoxLocalPos.top;
    bool bInterval = false;

    {
        int nTotalLine = m_pSkillLabel2->GetTotalLine();
        const float fAllLine = m_pSkillLabel2->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pSkillLabel2->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pSkillLabel2->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

    {
        if ( bInterval && m_pSkillLabel3->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pSkillLabel3->GetLocalPos().left;
        vPos.y = fPosY;
        m_pSkillLabel3->SetLocalPos( vPos );

        int nTotalLine = m_pSkillLabel3->GetTotalLine();
        const float fAllLine = m_pSkillLabel3->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pSkillLabel3->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pSkillLabel3->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

    {
        if ( bInterval && m_pSkillLabel4->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pSkillLabel4->GetLocalPos().left;
        vPos.y = fPosY;
        m_pSkillLabel4->SetLocalPos( vPos );

        int nTotalLine = m_pSkillLabel4->GetTotalLine();
        const float fAllLine = m_pSkillLabel4->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pSkillLabel4->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pSkillLabel4->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

    {
        if ( bInterval && m_pSkillLabel5->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pSkillLabel5->GetLocalPos().left;
        vPos.y = fPosY;
        m_pSkillLabel5->SetLocalPos( vPos );

        int nTotalLine = m_pSkillLabel5->GetTotalLine();
        const float fAllLine = m_pSkillLabel5->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pSkillLabel5->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pSkillLabel5->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

    {
        if ( bInterval && m_pSkillLabel6->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pSkillLabel6->GetLocalPos().left;
        vPos.y = fPosY;
        m_pSkillLabel6->SetLocalPos( vPos );

        int nTotalLine = m_pSkillLabel6->GetTotalLine();
        const float fAllLine = m_pSkillLabel6->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pSkillLabel6->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pSkillLabel6->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

	{
		if ( bInterval && m_pSkillLabel7->GetTotalLine() )
		{
			bInterval = false;
			fPosY += fReSizeOffset;
		}

		D3DXVECTOR2 vPos;
		vPos.x = m_pSkillLabel7->GetLocalPos().left;
		vPos.y = fPosY;
		m_pSkillLabel7->SetLocalPos( vPos );

		int nTotalLine = m_pSkillLabel7->GetTotalLine();
		const float fAllLine = m_pSkillLabel7->CalcMaxHEIGHT( 0, nTotalLine );
		fPosY += fAllLine;

		const UIRECT& rcLabelLP = m_pSkillLabel7->GetLocalPos();
		UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
		m_pSkillLabel7->SetLocalPos( rcLabelNewLP );

		if ( nTotalLine )
			bInterval = true;
	}

    {
        if ( bInterval && m_pSkillLabel8->GetTotalLine() )
        {
            bInterval = false;
            fPosY += fReSizeOffset;
        }

        D3DXVECTOR2 vPos;
        vPos.x = m_pSkillLabel8->GetLocalPos().left;
        vPos.y = fPosY;
        m_pSkillLabel8->SetLocalPos( vPos );

        int nTotalLine = m_pSkillLabel8->GetTotalLine();
        const float fAllLine = m_pSkillLabel8->CalcMaxHEIGHT( 0, nTotalLine );
        fPosY += fAllLine;

        const UIRECT& rcLabelLP = m_pSkillLabel8->GetLocalPos();
        UIRECT rcLabelNewLP = UIRECT( rcLabelLP.left, rcLabelLP.top, rcLabelLP.sizeX, fAllLine );
        m_pSkillLabel8->SetLocalPos( rcLabelNewLP );

        if ( nTotalLine )
            bInterval = true;
    }

	if( m_pClose )
	{
		const float fOffset = 5.0f;
		UIRECT rtButton = m_pClose->GetLocalPos();
		rtButton.left = rcNewUIPos.sizeX - rtButton.sizeX - fOffset;
		rtButton.right = rcNewUIPos.sizeX - fOffset;
		m_pClose->SetLocalPos( rtButton );
	}
    fPosY += fResizeEndOffset;

    UIRECT pParents = GetLocalPos();

    //GlobalPos 결정 + 라인박스 크기 결정;
    switch( m_emTooltipType )
    {
    case BASIC_TOOLTIP:
        {
            const UIRECT rcLocalNewPos = UIRECT(
                float(x) + m_vMousePointGap.x , float(y) + m_vMousePointGap.y,
                rcNewUIPos.sizeX, fPosY );

            AlignSubControl( rcLocalPosDummy, rcLocalNewPos );
            SetGlobalPos( rcLocalNewPos );
        }
        break;
    case MULTISKILL_MAIN_TOOLTIP:
        {
            float fSubSkillTooltipSize = 0.f;
				
			if( m_pInterface->UiIsVisibleGroup( SKILL_INFOR_TOOLTIP_MULTI ) )
				fSubSkillTooltipSize += rcNewUIPos.sizeX;
			if( m_pInterface->UiIsVisibleGroup( SKILL_INFOR_TOOLTIP_LINK ) )
				fSubSkillTooltipSize += rcNewUIPos.sizeX;
			if( m_pInterface->UiIsVisibleGroup( SKILL_INFOR_TOOLTIP_DELAY_ACTION ) )
				fSubSkillTooltipSize += rcNewUIPos.sizeX;

            // 멀티스킬(메인스킬)은 옆에 붙을 서브스킬툴팁의 크기까지 고려해서 글로벌 포지션 정함;
            const UIRECT rcLocalNewPos = UIRECT(
            float(x) + m_vMousePointGap.x , float(y) + m_vMousePointGap.y,
            rcNewUIPos.sizeX, fPosY );

            const UIRECT rcGlobalNewPos = UIRECT( rcLocalNewPos.left , rcLocalNewPos.top, rcLocalNewPos.sizeX + fSubSkillTooltipSize, rcLocalNewPos.sizeY );

            //localPos와 GlobalPos를 따로 둔 이유는 라인박스크기까지 변경되지 않게 하기 위해서임;
            AlignSubControl( rcLocalPosDummy, rcLocalNewPos );
            SetGlobalPos( rcGlobalNewPos );
        }
        break;
    case MULTISKILL_SUB_TOOLTIP:
        {
            float fSubSkillTooltipPosX = m_pOthertMultiSkillTooltip->GetGlobalPos().left + rcNewUIPos.sizeX;

            // 멀티스킬(서브스킬)은 메인스킬의 글로벌 위치를 참조하여 배치;
            const UIRECT rcLocalNewPos = UIRECT(
                fSubSkillTooltipPosX , float(y) + m_vMousePointGap.y,
                rcNewUIPos.sizeX, fPosY );

            AlignSubControl( rcLocalPosDummy, rcLocalNewPos );
            SetGlobalPos( rcLocalNewPos );
        }
        break;
	case MULTISKILL_LINK_TOOLTIP:
		{
			float fLinkSkillTooltipPosX = float(x) + m_vMousePointGap.x + rcNewUIPos.sizeX;
			float fLinkSkillTooltipPosY = float(y) + m_vMousePointGap.y;
			if( m_pLinkSkillTooltip && m_pOthertMultiSkillTooltip )
			{
				if( m_pInterface->UiIsVisibleGroup( SKILL_INFOR_TOOLTIP_MULTI ) )
				{
					fLinkSkillTooltipPosX += rcNewUIPos.sizeX;
				}

				if( m_pInterface->UiIsVisibleGroup( SKILL_INFOR_TOOLTIP_DELAY_ACTION ) )
				{
					fLinkSkillTooltipPosX = m_pLinkSkillTooltip->GetGlobalPos().right - 
						rcNewUIPos.sizeX - m_pDealyActionSkillTooltip->GetLocalPos().sizeX;
				}

				const UIRECT rcLocalNewPos = UIRECT(
					fLinkSkillTooltipPosX , fLinkSkillTooltipPosY,
					rcNewUIPos.sizeX, fPosY );

				AlignSubControl( rcLocalPosDummy, rcLocalNewPos );
				if( GetParent() )
					GetParent()->SetGlobalPos( rcLocalNewPos );
			}
			else if( m_pOthertMultiSkillTooltip )
			{
				if( m_pOthertMultiSkillTooltip->IsVisible() )
				{
					fLinkSkillTooltipPosX = m_pOthertMultiSkillTooltip->GetLocalPos().left;
					fLinkSkillTooltipPosY = m_pOthertMultiSkillTooltip->GetLocalPos().bottom;
				}

				const UIRECT rcLocalNewPos = UIRECT(
					fLinkSkillTooltipPosX , fLinkSkillTooltipPosY,
					rcNewUIPos.sizeX, fPosY );

				rcNewUIPos.sizeX = fLinkSkillTooltipPosX;

				AlignSubControl( rcLocalPosDummy, rcLocalNewPos );
				SetLocalPos( rcLocalNewPos );

				CUIControl* pParents = GetParent();
				if( pParents )
				{
					UIRECT rcGlobalNewPos = pParents->GetGlobalPos();
					rcGlobalNewPos.sizeY += fPosY;
					pParents->SetGlobalPos( rcGlobalNewPos );
				}
			}
		}
		break;
	case MULTISKILL_DELAY_ACTION_TOOLTIP:
		{
            float fSubSkillTooltipPosX = m_pOthertMultiSkillTooltip->GetGlobalPos().right - rcNewUIPos.sizeX;

            // 멀티스킬(서브스킬)은 메인스킬의 글로벌 위치를 참조하여 배치;
            const UIRECT rcLocalNewPos = UIRECT(
                fSubSkillTooltipPosX , float(y) + m_vMousePointGap.y,
                rcNewUIPos.sizeX, fPosY );

            AlignSubControl( rcLocalPosDummy, rcLocalNewPos );
            SetGlobalPos( rcLocalNewPos );
		}
		break;
    }
}

void CSkillInfoTooltip::SetBlockMouseTracking( bool bTracking )
{
	m_bBlockMousetracking = bTracking;
}

void CSkillInfoTooltip::DisplaySkillInformation( GLCHARLOGIC* pTarget, const GLSKILL* pSkill, const WORD wShowLevel, const WORD _emTooltipType, const bool bShowReq /* = false */, const bool bShowLv  )
{
    if( !pTarget || !pSkill )
        return;

    if ( wShowLevel < 0 || wShowLevel > (pSkill->m_sBASIC.dwMAXLEVEL+1) )  //ShowLevel은 레벨보다 1더 큰 값이기때문에 MaxLevel에 1을 더해서 검사한다.
    {
        return;
    }

    ResetInformation();

    m_pTarget = pTarget;
    m_pSkillData = pSkill;
    m_pCharSkill = pTarget->GETLEARNED_SKILL( pSkill->GetId() );
    m_wLevel = max( wShowLevel - 1 , 0 );
    m_wShowLevel = wShowLevel;

    m_emTooltipType = _emTooltipType;   //툴팁 타입 ( ex. 기본, 멀티스킬타입 주스킬, 멀티스킬타입 서브스킬 );

    const bool bSubSkill = ( m_emTooltipType == MULTISKILL_SUB_TOOLTIP || 
							 m_emTooltipType == MULTISKILL_LINK_TOOLTIP ||
							 m_emTooltipType == MULTISKILL_DELAY_ACTION_TOOLTIP );

    // 추가 효과 스킬(멀티스킬타입 서브스킬)에서만 출력된다.;
	if( m_emTooltipType == MULTISKILL_SUB_TOOLTIP )
		SetInformationLabel0_1();
	// 발동 스킬 효과;
	else if( m_emTooltipType == MULTISKILL_LINK_TOOLTIP )
		SetInformationLabel0_2();
	else if( m_emTooltipType == MULTISKILL_DELAY_ACTION_TOOLTIP )
		SetInformationLabel0_3();
    SetSkillImage();
    SetSkillName(bShowLv);
    SetInformationLabel1();
    SetInformationLabel2( bSubSkill );

    //추가 효과 스킬(멀티스킬타입 서브스킬)은 기존 툴팁과는 형태,위치가 다르다.(한칸씩 밑으로 내려감);
    UIRECT rcSkillLabel0 = m_pSkillLabel0->GetLocalPos();
    m_pSkillImage->SetLocalPos( D3DXVECTOR2 ( m_rcpSkillImage.left, m_rcpSkillImage.top + ( bSubSkill ? rcSkillLabel0.sizeY : 0 ) ) );
    m_pSkillName->SetLocalPos( D3DXVECTOR2 ( m_rcSkillName.left, m_rcSkillName.top + ( bSubSkill ? rcSkillLabel0.sizeY : 0 ) ) );
    m_pDummyNameControl->SetLocalPos( D3DXVECTOR2 ( m_rcDummuname.left, m_rcDummuname.top + ( bSubSkill ? rcSkillLabel0.sizeY : 0 ) ) );
    m_pSkillLabel1->SetLocalPos( D3DXVECTOR2 ( m_rcSkillLabel1.left, m_rcSkillLabel1.top + ( bSubSkill ? rcSkillLabel0.sizeY : 0 ) ) );
    m_pSkillLabel2->SetLocalPos( D3DXVECTOR2 ( m_rcSkillLabel2.left, m_rcSkillLabel2.top + ( bSubSkill ? rcSkillLabel0.sizeY : 0 ) ) );

    if ( bShowReq ) //습득 요구조건 출력 여부
    {
        SetInformationLabel3();
    }
    SetInformationLabel4();
    SetInformationLabel5();
    SetInformationLabel5_1();
    SetInformationLabel5_2();
    SetInformationLabel6();
	SetInformationLabel7();
    SetInformationLabel8();
}

void CSkillInfoTooltip::ResetInformation()
{
	m_pTarget = NULL;
    m_pSkillData = NULL;
    m_pCharSkill = NULL;
	m_bNextLevel = false;

    m_pSkillImage->ResetSkill();
    m_pSkillName->ClearText();
	m_pSkillLabel1->ClearText();
    m_pSkillLabel1->ClearText();
    m_pSkillLabel2->ClearText();
    m_pSkillLabel3->ClearText();
    m_pSkillLabel4->ClearText();
    m_pSkillLabel5->ClearText();
    m_pSkillLabel6->ClearText();
	m_pSkillLabel7->ClearText();
    m_pSkillLabel8->ClearText();
}

void CSkillInfoTooltip::SetOtherMultiSkillTooltip( const CSkillInfoTooltip* pSkillTooltip )
{
    m_pOthertMultiSkillTooltip = pSkillTooltip;
}

void CSkillInfoTooltip::SetLinkSkillTooltip( const CSkillInfoTooltip* pSkillTooltip )
{
	m_pLinkSkillTooltip = pSkillTooltip;
}

void CSkillInfoTooltip::SetDealyActionSSkillTooltip( const CSkillInfoTooltip* pSkillTooltip )
{
	m_pDealyActionSkillTooltip = pSkillTooltip;
}


SNATIVEID CSkillInfoTooltip::GetSkillID()
{
	if( m_pSkillData )
		return m_pSkillData->GetId();

	return NATIVEID_NULL();
}

void CSkillInfoTooltip::SetSkillImage()
{
    m_pSkillImage->ResetSkill();
    m_pSkillImage->SetSkill( m_pSkillData->GetId() );
}

void CSkillInfoTooltip::SetSkillName( const bool bShowLv )
{
    std::string strSkillName = "";
 
    // Info : 이름
    if ( m_pCharSkill != NULL || 0 < m_wShowLevel )
	{
		if( bShowLv )
			strSkillName = sc::string::format( "%s (%dLv)", m_pSkillData->GetNameStr(), m_wShowLevel );
		else
			strSkillName = sc::string::format( "%s", m_pSkillData->GetNameStr() );
	}
    else
	{
        strSkillName = sc::string::format( "%s", m_pSkillData->GetNameStr() );
	}

	m_pSkillName->ClearText();
    m_pSkillName->AddText( strSkillName.c_str(), SKILL_INFOR_COLOR::dwSkillName );
}

void CSkillInfoTooltip::SetInformationLabel0_1()
{
	std::string strText = "";

	int nIndex = 0;

	// Info : 추가스킬
	{   
		strText = sc::string::format("%s", ID2GAMEWORD( "MULTI_SKILL_TEXT" ) );
		nIndex = m_pSkillLabel0->AddText( strText.c_str(), NS_UITEXTCOLOR::DODGERBLUE );
        nIndex = m_pSkillLabel->AddText( " ", NS_UITEXTCOLOR::DODGERBLUE );
        m_pSkillLabel->SetTextImage( nIndex, "SPEC_ACT_GRADE_OPEN_OVER", 16 );
        m_pSkillLabel->SetDiffuse( NS_UITEXTCOLOR::DODGERBLUE );
	}
}

void CSkillInfoTooltip::SetInformationLabel0_2()
{
	std::string strText = "";

	int nIndex = 0;

	// Info : 발동스킬;
	{   
		strText = sc::string::format("%s", ID2GAMEWORD( "LINK_SKILL_TEXT" ) );
		nIndex = m_pSkillLabel0->AddText( strText.c_str(), NS_UITEXTCOLOR::DODGERBLUE );
		nIndex = m_pSkillLabel->AddText( " ", NS_UITEXTCOLOR::DODGERBLUE );
		m_pSkillLabel->SetTextImage( nIndex, "SPEC_ACT_GRADE_OPEN_OVER", 16 );
		m_pSkillLabel->SetDiffuse( NS_UITEXTCOLOR::DODGERBLUE );
	}
};

void CSkillInfoTooltip::SetInformationLabel0_3()
{
	std::string strText = "";

	int nIndex = 0;

	// Info : 지연 스킬
	{   
		strText = sc::string::format("%s", ID2GAMEWORD( "DEALY_ACTION_SKILL_TEXT" ) );
		nIndex = m_pSkillLabel0->AddText( strText.c_str(), NS_UITEXTCOLOR::DODGERBLUE );
		nIndex = m_pSkillLabel->AddText( " ", NS_UITEXTCOLOR::DODGERBLUE );
		m_pSkillLabel->SetTextImage( nIndex, "SPEC_ACT_GRADE_OPEN_OVER", 16 );
		m_pSkillLabel->SetDiffuse( NS_UITEXTCOLOR::DODGERBLUE );
	}
}

void CSkillInfoTooltip::SetInformationLabel1()
{
    std::string strText = "";

    int nIndex = 0;

    // Info : 등급
    {
        strText = sc::string::format("%d%s", m_pSkillData->m_sBASIC.dwGRADE, ID2GAMEWORD( "SKILL_BASIC_INFO", 1 ) );
        strText += "/";
        nIndex = m_pSkillLabel1->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwSkillGrade );
    }

    // Info : 속성
    {
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			strText = sc::string::format(" %s", CGameTextMan::GetInstance().GetCommentText("ELEMENT", m_pSkillData->m_sAPPLY.emELEMENT).GetString());
		else
			strText = sc::string::format( " %s", COMMENT::ELEMENT[ m_pSkillData->m_sAPPLY.emELEMENT ] );
        m_pSkillLabel1->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
    }

}

void CSkillInfoTooltip::SetInformationLabel2( BOOL bMultiSkill )
{
    std::string strText = "";

    // Info : 적용 타입
    {
        switch ( m_pSkillData->m_sBASIC.emACTION_TYPE )
        {
        case SKILL::EMACTION_TYPE_NORMAL:
            {
                strText = sc::string::format( "%s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 0 ) );
            }
            break;

        case SKILL::EMACTION_TYPE_BUFF:
            {
                strText = sc::string::format( "%s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 1 ) );
            }
            break;

        case SKILL::EMACTION_TYPE_DEBUFF:
            {
                strText = sc::string::format( "%s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 2 ) );
            }
            break;

        case SKILL::EMACTION_TYPE_LIMIT:
            {
                strText = sc::string::format( "%s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 3 ) );
            }
            break;
        };

        if ( m_pSkillData->m_sBASIC.emROLE == SKILL::EMROLE_NORMAL )
        {
            switch ( m_pSkillData->m_sBASIC.emAPPLY )
            {
            case SKILL::EMAPPLY_MELEE:
                {
                    strText += sc::string::format( "/ %s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 4 ) );
                }
                break;

            case SKILL::EMAPPLY_RANGE:
                {
                    strText += sc::string::format( "/ %s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 5 ) );
                }
                break;

            case SKILL::EMAPPLY_MAGIC:
                {
                    if ( m_pSkillData->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_PARTY )
                        strText += sc::string::format( "/ %s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 6 ) );
                    else
                        strText += sc::string::format( "/ %s", ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 7 ) );
                }
                break;
            };
        }
        else if ( m_pSkillData->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
        {
            strText += ID2GAMEWORD( "SKILL_BASIC_INFO", 8 );
        }

        m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
    }

	if ( !bMultiSkill )
	{
		// Info : 사용 무기
		{
			const SKILL::SBASIC& sBASIC = m_pSkillData->m_sBASIC;

			if ( (sBASIC.emUSE_LITEM && (sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE)) || 
				(sBASIC.emUSE_RITEM && (sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE)) )
			{
				strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 9 ) );

				bool bDistinction = false;
				if ( sBASIC.emUSE_LITEM )
				{
					if ( sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE )
					{
						bDistinction = true;

						strText += sc::string::format( "%s", ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_LITEM - 1 ) );
					}
				}

				if ( sBASIC.emUSE_RITEM )
				{
					if ( sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE )
					{
						if ( bDistinction )
							strText += "/";

						strText += sc::string::format( "%s", ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_RITEM - 1 ) );
					}
				}

				if ( m_pSkillData->m_sLEARN.bHiddenWeapon )
				{
					if( m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME )
						strText += sc::string::format( ", %s", ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", SKILL::SKILLATT_EXTREME_GLOVE - 1 ) );
				}

				m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
			}
		}

		// Info : 소모 조건
		{
			//	사용시 소진량
			if ( m_pSkillData->m_sBASIC.emROLE != SKILL::EMROLE_PASSIVE )
			{
	//             WORD wLevel = 0;
	//             if ( m_pCharSkill )
	//             {
	//                 wLevel = m_pCharSkill->wLevel;
	//                 if ( m_bNextLevel ) wLevel = m_pCharSkill->wLevel + 1;
	//             }

				const SKILL::SAPPLY& sAPPLY = m_pSkillData->m_sAPPLY;
				const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[m_wLevel];

				bool bDisplay = false;
				int nIndex = 0;

				bool bDistinction = false;

				//	화살 개수
				if ( 0 < sDATA_LVL.wUSE_ARROWNUM )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 0), sDATA_LVL.wUSE_ARROWNUM);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}

				//	부적 개수
				if ( 0 < sDATA_LVL.wUSE_CHARMNUM )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 1), sDATA_LVL.wUSE_CHARMNUM);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}

				//	총알 개수
				if ( 0 < sDATA_LVL.wUSE_BULLETNUM )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7), sDATA_LVL.wUSE_BULLETNUM);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}

				// 배터리 소모량
				if ( 0 < sDATA_LVL.nUSE_BATTERY )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7), sDATA_LVL.nUSE_BATTERY);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}

				//	HP 소진량
				if ( 0 < sDATA_LVL.wUSE_HP )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 3), sDATA_LVL.wUSE_HP);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}

				//	MP 소진량
				if ( 0 < sDATA_LVL.wUSE_MP )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 4), sDATA_LVL.wUSE_MP);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}

				//	SP 소진량
				if ( 0 < sDATA_LVL.wUSE_SP )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 5), sDATA_LVL.wUSE_SP);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}

				//	CP 소진량
				if ( 0 < sDATA_LVL.wUSE_CP )
				{
					if ( !bDisplay )
					{
						bDisplay = true;
						strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ) );
						nIndex = m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
					}

					if ( bDistinction )
						m_pSkillLabel2->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
					bDistinction = true;

					strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 6), sDATA_LVL.wUSE_CP);
					m_pSkillLabel2->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
				}
			}
		}

		// Info : 재사용 시간
		{
	//         WORD wLevel = 0;
	//         if ( m_pCharSkill )
	//         {
	//             wLevel = m_pCharSkill->wLevel;
	//             if ( m_bNextLevel ) wLevel = m_pCharSkill->wLevel + 1;
	//         }

			const SKILL::SAPPLY& sAPPLY = m_pSkillData->m_sAPPLY;
			const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[m_wLevel];

			//	딜레이 타임
			if ( 0 < sDATA_LVL.fDELAYTIME )
			{
				strText = sc::string::format("%s :%2.1f%s",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 0),
					sDATA_LVL.fDELAYTIME, ID2GAMEWORD( "WAITSERVER_TIMELEFT_UNIT", 0 ) );

				m_pSkillLabel2->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
			}
		}
	}
}

void CSkillInfoTooltip::SetInformationLabel3()
{
    std::string strText = "";

//     WORD wDispLevel = 0;
//     if ( m_pCharSkill )	//	배운 스킬
//         wDispLevel = m_pCharSkill->wLevel + 1;	

    // Info : 습득 조건
    {
        bool bMaster = ( 0 != m_pTarget->ISMASTER_SKILL( m_pSkillData->m_sBASIC.sNATIVEID ) );
        if ( bMaster)    //마스터한 스킬은요구조건을 보여주지 않음
            return;

        //	요구치 정보
        int nIndex = 0;

        bool bDistinction = false;
		bool bOr = false;

        strText = sc::string::format( "%s :", ID2GAMEWORD( "SKILL_BASIC_INFO", 11 ) );
        nIndex = m_pSkillLabel3->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );

        const SKILL::SLEARN& sLEARN = m_pSkillData->m_sLEARN;
        const SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[m_wLevel];


        //	1. 요구보유스킬
        SNATIVEID NeedSkillID = m_pSkillData->m_sLEARN.sSKILL;			
        if ( NeedSkillID != NATIVEID_NULL() )
        {
            bool bVALID = false;
            bool bNeedSkillLevel = false;

            std::string strNeedSkillName;
            std::string strNeedSkillLevel;

            PGLSKILL pNeedSkill = GLSkillMan::GetInstance().GetData( NeedSkillID.wMainID, NeedSkillID.wSubID );
            if( pNeedSkill ) strNeedSkillName = sc::string::format("%s", pNeedSkill->GetName() );
            bVALID = ( 0 != m_pTarget->ISLEARNED_SKILL( NeedSkillID ) );

            //	2. 요구보유스킬레벨
            if ( 0 < sLVL.dwSKILL_LVL )
            {
                strNeedSkillLevel = sc::string::format(" %dLv", (sLVL.dwSKILL_LVL + 1) );
                bNeedSkillLevel = true;

                SCHARDATA2::SKILL_MAP& map = m_pTarget->m_ExpSkills;				
                SCHARDATA2::SKILL_MAP_ITER iter = map.find ( NeedSkillID.dwID );
                if ( iter != map.end() )
                {
                    SCHARSKILL& rCharSkill = (*iter).second;

                    //	색 조절
                    bVALID = ( 0 != rCharSkill.wLevel >= sLVL.dwSKILL_LVL );
                }
            }

            if ( bDistinction )
                m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
            bDistinction = true;

            if ( bVALID )
                m_pSkillLabel3->AddString( nIndex, strNeedSkillName.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strNeedSkillName.c_str(), SKILL_INFOR_COLOR::dwReqWrong );

            if ( bNeedSkillLevel )
            {
                if ( bVALID )
                    m_pSkillLabel3->AddString( nIndex, strNeedSkillLevel.c_str(), SKILL_INFOR_COLOR::dwDefault );
                else
                    m_pSkillLabel3->AddString( nIndex, strNeedSkillLevel.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
            }
        }

        //	3. 요구경험치
        if ( 0 < sLVL.dwSKP )
        {
            if ( bDistinction )
                m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
            bDistinction = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 2), sLVL.dwSKP);	

            if ( m_pTarget->m_dwSkillPoint >= sLVL.dwSKP )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        //	4. 요구레벨
        if ( 0 < sLVL.dwLEVEL )
        {
            if ( bDistinction )
                m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
            bDistinction = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 3), sLVL.dwLEVEL);

            if ( m_pTarget->GETLEVEL () >= int(sLVL.dwLEVEL) )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        //	Stats
        //	1. 요구힘
		//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
        if ( 0 < sLVL.sSTATS.wPow )
        {
            if ( bDistinction )
                m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
            bDistinction = true;
			bOr = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 0), sLVL.sSTATS.wPow);

            if ( m_pTarget->m_sSUMSTATS_SKILL.wPow >= sLVL.sSTATS.wPow  )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        //	2. 요구체력
        if ( 0 < sLVL.sSTATS.wStr )
        {
			if ( bOr && sLEARN.bOnlyOneStats )
			{
				strText = sc::string::format( " %s ",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS_OR_TEXT") );
				m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
			}
			else if ( bDistinction )
			{
				m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
			}
			bOr = true;
            bDistinction = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 1), sLVL.sSTATS.wStr);

            if ( m_pTarget->m_sSUMSTATS_SKILL.wStr >= sLVL.sSTATS.wStr  )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        //	3. 요구정신
        if ( 0 < sLVL.sSTATS.wSpi )
        {
			if ( bOr && sLEARN.bOnlyOneStats )
			{
				strText = sc::string::format( " %s ",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS_OR_TEXT") );
				m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
			}
			else if ( bDistinction )
			{
				m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
			}
			bOr = true;
			bDistinction = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 2), sLVL.sSTATS.wSpi);

            if ( m_pTarget->m_sSUMSTATS_SKILL.wSpi >= sLVL.sSTATS.wSpi  )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        //	4. 요구민첩
        if ( 0 < sLVL.sSTATS.wDex )
        {
			if ( bOr && sLEARN.bOnlyOneStats )
			{
				strText = sc::string::format( " %s ",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS_OR_TEXT") );
				m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
			}
			else if ( bDistinction )
			{
				m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
			}
			bOr = true;
			bDistinction = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 3), sLVL.sSTATS.wDex);

            if ( m_pTarget->m_sSUMSTATS_SKILL.wDex >= sLVL.sSTATS.wDex )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        //	5. 요구지력
        if ( 0 < sLVL.sSTATS.wInt )
        {
			if ( bOr && sLEARN.bOnlyOneStats )
			{
				strText = sc::string::format( " %s ",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS_OR_TEXT") );
				m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
			}
			else if ( bDistinction )
			{
				m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
			}
			bOr = true;
			bDistinction = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 4), sLVL.sSTATS.wInt);

            if ( m_pTarget->m_sSUMSTATS_SKILL.wInt >= sLVL.sSTATS.wInt )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        //	6. 요구근력
        if ( 0 < sLVL.sSTATS.wSta )
        {
			if ( bOr && sLEARN.bOnlyOneStats )
			{
				strText = sc::string::format( " %s ",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS_OR_TEXT") );
				m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
			}
			else if ( bDistinction )
			{
				m_pSkillLabel3->AddString( nIndex, "  ", SKILL_INFOR_COLOR::dwDefault );
			}
			bOr = true;
			bDistinction = true;

            strText = sc::string::format("%s %d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 5), sLVL.sSTATS.wSta);

            if ( m_pTarget->m_sSUMSTATS_SKILL.wPow >= sLVL.sSTATS.wSta )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }

        if( GLUseFeatures::GetInstance().IsUsingRenewSkillWindow() )
        {
            //	7. 요구 골드
            // 스킬창 개선으로 인한 새로운 조건

            // 스킬 스크롤을 사용하도록 설정한 경우에는 스킬을 오직 스킬 스크롤로만 습득 할 수 있다.
            bool bUsingScrool = GLogicData::GetInstance().GetRnSkillUseScrool( m_pSkillData->GetId() );
            if( bUsingScrool )
                return;

            strText = sc::string::format("%s : ",ID2GAMEWORD( "RNSKILL_SLOT_NEED", 1) );
            nIndex = m_pSkillLabel3->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault  );

            DWORD dwUsingMoney = GLogicData::GetInstance().GetRnSkillUseMoney( m_pSkillData->GetId() );

			strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( dwUsingMoney, 3, "," );

            if ( dwUsingMoney <= m_pTarget->GetInvenMoney() )
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            else
                m_pSkillLabel3->AddString( nIndex, strText.c_str(), SKILL_INFOR_COLOR::dwReqWrong );
        }
    }
}

void CSkillInfoTooltip::SetInformationLabel4()
{
    std::string strText = "";

    // Info : 스킬 영역
    {
        const SKILL::SBASIC& sBASIC = m_pSkillData->m_sBASIC;
        const SKILL::SAPPLY& sAPPLY = m_pSkillData->m_sAPPLY;
//         WORD wLevel = 0;
//         if ( m_pCharSkill )
//         {
//             wLevel = m_pCharSkill->wLevel;
//             if ( m_bNextLevel ) wLevel = m_pCharSkill->wLevel + 1;
//         }

        const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[m_wLevel];

        switch ( m_pSkillData->m_sBASIC.emIMPACT_TAR )
        {
        case TAR_SELF: // 자신
            switch ( m_pSkillData->m_sBASIC.emIMPACT_REALM )
            {
            case REALM_SELF: // '목표' 자신
                strText = sc::string::format( "%s", ID2GAMEINTEXT( "SKILL_AREA_SELF_SELF" ) );
                break;

            case REALM_ZONE: // '목표' 주위
                switch ( m_pSkillData->m_sBASIC.emIMPACT_SIDE )
                {
                case EMIMPACT_SIDE_PARTY: // 자기편에게
                    if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                        strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_OUR" ),
                        sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                    break;

                case EMIMPACT_SIDE_ENEMY: // 적에게
                    if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                        strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_ENEMY" ),
                        sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                    break;
                case EMIMPACT_SIDE_ANYBODY: // 모두에게
                    if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                        strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_ANYBODY" ),
                        sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                    break;
                };
            };
            break;

        case TAR_SPEC: // 타겟
            {
                switch ( m_pSkillData->m_sBASIC.emIMPACT_REALM )
                {
                case REALM_SELF: // '목표' 자신
                    switch ( m_pSkillData->m_sBASIC.emIMPACT_SIDE )
                    {
                    case EMIMPACT_SIDE_PARTY: // 자기편에게
                        if ( 0 < sBASIC.wTARRANGE )
                            strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_OUR" ),
                            sBASIC.wTARRANGE * 0.1f );
                        break;
                    case EMIMPACT_SIDE_ENEMY: // 적에게
                        if ( 0 < sBASIC.wTARRANGE )
                            strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_ENEMY" ),
                            sBASIC.wTARRANGE * 0.1f );
                        break;
                    case EMIMPACT_SIDE_ANYBODY: // 모두에게
                        if ( 0 < sBASIC.wTARRANGE )
                            strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_ANYBODY" ),
                            sBASIC.wTARRANGE * 0.1f );
                        break;
                    };

                case REALM_ZONE: // '목표' 주위
                    switch ( m_pSkillData->m_sBASIC.emIMPACT_SIDE )
                    {
                    case EMIMPACT_SIDE_PARTY: // 자기편에게
                        if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                            strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_OUR" ),
                            sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                        break;
                    case EMIMPACT_SIDE_ENEMY: // 적에게
                        if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                            strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_ENEMY" ),
                            sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                        break;
                    case EMIMPACT_SIDE_ANYBODY: // 모두에게
                        if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                            strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_ANYBODY" ),
                            sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                        break;
                    };

                case REALM_FANWIZE:
                    switch ( m_pSkillData->m_sBASIC.emIMPACT_SIDE )
                    {
                    case EMIMPACT_SIDE_ENEMY:
                        if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYANGLE && 0 < sDATA_LVL.wTARNUM )
                            strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_FANWIZE_ENEMY" ),
                            sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYANGLE, sDATA_LVL.wTARNUM );
                        break;
                    };
                };
            }
            break;

        case TAR_SELF_TOSPEC: // 자신부터 타겟까지
            switch ( m_pSkillData->m_sBASIC.emIMPACT_SIDE )
            {
            case EMIMPACT_SIDE_ENEMY:
                WORD wPierce = sDATA_LVL.wPIERCENUM + m_pTarget->GETSUM_PIERCE() + 1;

                if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wTARNUM && 0 < wPierce )
                    strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TOSPEC_ENEMY" ),
                    sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wTARNUM, wPierce );
                break;
            };
            break;

        case TAR_ZONE: // 지면 위치의 주변 타겟
            switch ( m_pSkillData->m_sBASIC.emIMPACT_SIDE )
            {
            case EMIMPACT_SIDE_PARTY:
                if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                    strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_OUR" ),
                    sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                break;
            case EMIMPACT_SIDE_ENEMY:
                if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                    strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_ENEMY" ),
                    sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                break;
            case EMIMPACT_SIDE_ANYBODY:
                if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
                    strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_ANYBODY" ),
                    sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
                break;
            };
            break;

        case TAR_LAND: // 지면 위치
			{
				if ( m_pSkillData->m_sBASIC.wTARRANGE <= 0)
				{
					strText = sc::string::format( "%s", ID2GAMEINTEXT( "SKILL_AREA_LAND" ) );
				}
				else
				{
					strText = sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_LAND_RANGE" ), m_pSkillData->m_sBASIC.wTARRANGE * 0.1f  );
				}
			}
            break;
        };
    }

    if ( strText.length() )
    {
        m_pSkillLabel4->AddText( ID2GAMEWORD("SKILL_BASIC_INFO", 12), SKILL_INFOR_COLOR::dwSkillRange );
        m_pSkillLabel4->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwSkillRange );
    }
}

void CSkillInfoTooltip::SetInformationLabel5()
{
    std::string strText = "";


    const SKILL::SAPPLY& sAPPLY = m_pSkillData->m_sAPPLY;
    const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[m_wLevel];

    SKILL::EMROLE emRole = m_pSkillData->m_sBASIC.emROLE;

    SKILL_INFO_TYPE emInforType = INFO_NONE_TYPE;
    if ( emRole == SKILL::EMROLE_NORMAL )
    {
        if ( sDATA_LVL.fLIFE )
        {
            if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                emInforType = NORMAL_DURATION_PLUS;
            else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                emInforType = NORMAL_DURATION_MINUS;
        }
        else
        {
            if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                emInforType = NORMAL_NONDURATION_PLUS;
            else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                emInforType = NORMAL_NONDURATION_MINUS;
        }
    }
    else if ( emRole == SKILL::EMROLE_PASSIVE )
    {
        if ( sDATA_LVL.fLIFE )
        {
            if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                emInforType = PASSIVE_DURATION_PLUS;
            else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                emInforType = PASSIVE_DURATION_MINUS;
        }
        else
        {
            if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                emInforType = PASSIVE_NONDURATION_PLUS;
            else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                emInforType = PASSIVE_NONDURATION_MINUS;
        }
    }

    // Info : 적용 타입
        if ( emInforType != INFO_NONE_TYPE )
        {
            strText = ID2GAMEWORD( "SKILL_BASIC_INFO", 13);
            m_pSkillLabel5->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
        }

    strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pTarget, m_pSkillData, m_wLevel );
    m_pSkillLabel5->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
}

void CSkillInfoTooltip::SetInformationLabel5_1()
{
    std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Impact( m_pSkillData, m_wLevel );

    BOOST_FOREACH( std::string& strText, vecString)
    {
        m_pSkillLabel5->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
    }
}

void CSkillInfoTooltip::SetInformationLabel5_2()
{

    std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Special( m_pSkillData, m_wLevel );

    BOOST_FOREACH( std::string& strText, vecString)
    {
        m_pSkillLabel5->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
    }      
}

void CSkillInfoTooltip::SetInformationLabel6()
{
    std::string strText = "";

    // Info : 상태이상 효과
    {
        const SKILL::SAPPLY& sAPPLY = m_pSkillData->m_sAPPLY;
        EMSTATE_BLOW emBLOW = sAPPLY.emSTATE_BLOW;

        if ( emBLOW != EMBLOW_NONE )
        {
//             WORD wLevel = 0;
//             if ( m_pCharSkill )
//             {
//                 wLevel = m_pCharSkill->wLevel;
//                 if ( m_bNextLevel ) wLevel = m_pCharSkill->wLevel + 1;
//             }

            const SKILL::SSTATE_BLOW& sSTATE_BLOW = sAPPLY.sSTATE_BLOW[m_wLevel];

            float fLife = sAPPLY.sDATA_LVL[ m_wLevel ].fLIFE;
            float fRate = sSTATE_BLOW.fRATE;

            float fVAR1 = sSTATE_BLOW.fVAR1;
            float fVAR2 = sSTATE_BLOW.fVAR2;

            SKILL_INFO_STATE_BLOW emStateBlow = VALUE_COUNT_0;

            if ( fVAR1 != 0.0f )
            {
                if ( fVAR2 != 0.0f )
                    emStateBlow = VALUE_COUNT_2;
                else
                    emStateBlow = VALUE_COUNT_1_VAR1;
            }
            else
            {
                if( fVAR2 != 0.0f )
                    emStateBlow = VALUE_COUNT_1_VAR2;
            }

            if ( fVAR1 < 0.0f )
                fVAR1 = -fVAR1;

            if ( fVAR2 < 0.0f )
                fVAR2 = -fVAR2;

            switch ( emBLOW )
            {
            case EMBLOW_NUMB:   //	마비.
                {
                    switch ( emStateBlow )
                    {
                    case VALUE_COUNT_2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_NUMB" ), fRate, fLife, fVAR1 * 100.0f, fVAR2 * 100.0f );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;

                    case VALUE_COUNT_1_VAR1:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_NUMB_1" ), fRate, fLife, fVAR1 * 100.0f );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;
                        
                    case VALUE_COUNT_1_VAR2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_NUMB_2" ), fRate, fLife, fVAR2 * 100.0f );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;
                    };
                }
                break;

            case EMBLOW_STUN:   //	기절.
                {
                    strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STUN" ), fRate, fLife );
                    m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                }
                break;

            case EMBLOW_STONE:  //	석화.
                {
                    switch ( emStateBlow )
                    {
                    case VALUE_COUNT_2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STONE" ), fRate, fLife, fVAR1 * 100.0f, fVAR2 );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;

                    case VALUE_COUNT_1_VAR1:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STONE_1" ), fRate, fLife, fVAR1 * 100.0f );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;

                    case VALUE_COUNT_1_VAR2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STONE_2" ), fRate, fLife, fVAR2 );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;
                    };
                }
                break;

            case EMBLOW_BURN:   //	화염.
                {
                    switch ( emStateBlow )
                    {						
					case VALUE_COUNT_2:
						{
							strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_BURN" ), fRate, fLife, fVAR1 * 100.0f, fVAR2);
						}
						break;
                    case VALUE_COUNT_1_VAR1:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_BURN_1" ), fRate, fLife, fVAR1 * 100.0f);
                        }
                        break;
					case VALUE_COUNT_1_VAR2:
						{
							strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_BURN_2" ), fRate, fLife, fVAR2 );
						}
						break;
					default:
						return;
                    };
					m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                }
                break;

            case EMBLOW_FROZEN: //	냉동.
                {
                    switch ( emStateBlow )
                    {
                    case VALUE_COUNT_2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_FROZEN" ), fRate, fLife, fVAR1 * 100.0f, fVAR2 * 100.0f );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;

                    case VALUE_COUNT_1_VAR1:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_FROZEN_1" ), fRate, fLife, fVAR1 * 100.0f );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;

                    case VALUE_COUNT_1_VAR2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_FROZEN_2" ), fRate, fLife, fVAR2 * 100.0f );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;
                    };
                }
                break;

            case EMBLOW_MAD:    //	착란.
                {
                    switch ( emStateBlow )
                    {
                    case VALUE_COUNT_2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_MAD" ), fRate, fLife, fVAR1, fVAR2 );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;
                        
                    case VALUE_COUNT_1_VAR1:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_MAD_1" ), fRate, fLife, fVAR1 );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;

                    case VALUE_COUNT_1_VAR2:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_MAD_2" ), fRate, fLife, fVAR2 );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;
                    };
                }
                break;

            case EMBLOW_POISON: //	중독.
                {
                    switch ( emStateBlow )
                    {
					case VALUE_COUNT_2:
						{
							strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_POISON" ), fRate, fLife, fVAR1, fVAR2 );
						}
						break;
                    case VALUE_COUNT_1_VAR1:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_POISON_1" ), fRate, fLife, fVAR1 );
                        }
                        break;
					case VALUE_COUNT_1_VAR2:
						{
							strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_POISON_2" ), fRate, fLife, fVAR2 );
						}
						break;
					default:
						return;					
                    };
					m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                }
                break;

            case EMBLOW_CURSE:  //	저주.
                {
                    switch ( emStateBlow )
                    {
                    case VALUE_COUNT_1_VAR1:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_CURSE" ), fLife, fRate, fVAR1 );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;

                    case VALUE_COUNT_0:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_CURSE_1" ), fLife );
                            m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
                        }
                        break;
                    };
                }
                break;

            default:
                {
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						m_pSkillLabel6->AddText( CGameTextMan::GetInstance().GetCommentText("BLOW", sAPPLY.emSTATE_BLOW).GetString(), SKILL_INFOR_COLOR::dwElementalAbility );
						if ( 0 < sSTATE_BLOW.fRATE )
						{			
							strText = sc::string::format( "%s :%2.1f", ID2GAMEWORD( "SKILL_STATE_BLOW", 0), sSTATE_BLOW.fRATE );
							m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
						}

						if ( 0 < sSTATE_BLOW.fVAR1 )
						{
							strText = sc::string::format( "%s :%2.1f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", sAPPLY.emSTATE_BLOW).GetString(), sSTATE_BLOW.fVAR1 );
							m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
						}

						if ( 0 < sSTATE_BLOW.fVAR2 )
						{
							strText = sc::string::format( "%s :%2.1f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", sAPPLY.emSTATE_BLOW).GetString(), sSTATE_BLOW.fVAR2 );
							m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
						}
					}
					else
					{
						m_pSkillLabel6->AddText( COMMENT::BLOW [ sAPPLY.emSTATE_BLOW ].c_str(), SKILL_INFOR_COLOR::dwElementalAbility );

						if ( 0 < sSTATE_BLOW.fRATE )
						{			
							strText = sc::string::format( "%s :%2.1f", ID2GAMEWORD( "SKILL_STATE_BLOW", 0), sSTATE_BLOW.fRATE );
							m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
						}

						if ( 0 < sSTATE_BLOW.fVAR1 )
						{
							strText = sc::string::format( "%s :%2.1f", COMMENT::BLOW_VAR1[sAPPLY.emSTATE_BLOW].c_str(), sSTATE_BLOW.fVAR1 );
							m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
						}

						if ( 0 < sSTATE_BLOW.fVAR2 )
						{
							strText = sc::string::format( "%s :%2.1f", COMMENT::BLOW_VAR2[sAPPLY.emSTATE_BLOW].c_str(), sSTATE_BLOW.fVAR2 );
							m_pSkillLabel6->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwElementalAbility );
						}
					}
                }
            };
        }
    }
}

void CSkillInfoTooltip::SetInformationLabel7()
{
	std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Link( m_pSkillData, m_wLevel );

	if ( vecString.empty() == FALSE )
	{
		m_pSkillLabel7->AddText( ID2GAMEWORD( "SKILL_BASIC_INFO", 14), NS_UITEXTCOLOR::LINK_SKILL_TOOLTIP );

		BOOST_FOREACH( std::string& strText, vecString )
		{
			m_pSkillLabel7->AddText( strText.c_str(), NS_UITEXTCOLOR::LINK_SKILL_TOOLTIP );
		}
	}

	const SKILL::SAPPLY& sAPPLY = m_pSkillData->m_sAPPLY;
	const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[m_wLevel];
	EMIMPACT_TARGET emIMPACT_TAR = m_pSkillData->m_sBASIC.emIMPACT_TAR;

	WORD wMid =0;
	WORD wSid = 0;
	BOOL bIsDELAY_ACTION_SKILL = FALSE;
	BOOST_FOREACH( SKILL::SSPEC_ADDON sSPEC_ADDON, m_pSkillData->m_sAPPLY.vecSPEC)
	{
		if ( sSPEC_ADDON.emSPEC == EMSPECA_DELAY_ACTION_SKILL )
		{
			m_pSkillLabel7->AddText(" ");
			wMid = static_cast<WORD>(sSPEC_ADDON.sSPEC[m_wLevel].fVAR1);
			wSid = static_cast<WORD>(sSPEC_ADDON.sSPEC[m_wLevel].fVAR2);
			bIsDELAY_ACTION_SKILL = TRUE;
			m_pSkillLabel7->AddText(GLSkillToolTip::MakeTooltipSting_In_Skill_Special( 
				sAPPLY.sDATA_LVL[ m_wLevel ].fLIFE, 
				wMid, 
				wSid,
				EMSPECA_DELAY_ACTION_SKILL,
				sSPEC_ADDON.sSPEC[m_wLevel],
				emIMPACT_TAR,
				TRUE, sAPPLY.sCAData.sSPEC_CA[m_wLevel]).c_str(), NS_UITEXTCOLOR::LINK_SKILL_TOOLTIP);
		}
	}  
}

void CSkillInfoTooltip::SetInformationLabel8()
{
    std::string strText = "";
    // Info : 스킬 설명
    {
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_pSkillData->GetId() );
        if ( pSkill && pSkill->GetDesc() )
        {
            int StrLength = static_cast<int>( strlen( pSkill->GetDesc() ) );
            if ( StrLength )
            {
                strText = sc::string::format( "%s", pSkill->GetDesc()  );
                m_pSkillLabel8->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwDefault );
            }
        }
    }
}

void CSkillInfoTooltip::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

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
        AllControlRePos( x , y );
	}
}

void CSkillInfoTooltip::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case CLOSE_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->CloseSkillToolTipMulti( GetWndID() );
			}
		}
		break;
	}
}

void CSkillInfoTooltip::TranslateMessage( DWORD dwMsg )
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

void CSkillInfoTooltip::SetBGAlpha( int nValue )
{
	m_pLineBox->SetDiffuseAlpha(nValue);
}