#include "StdAfx.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogicClient/Product/GLProductClient.h"

#include "ProductTypeTapButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
//hsshin 제작 분야선택 : 탭버튼에 숙련도 툴팁과 학습기능을 추가 (2012.1.23)
CProductTypeTapButton::CProductTypeTapButton( EngineDeviceMan* pEngineDevice )
	: TAPBUTTON_RNBUTTON( pEngineDevice )
	, m_pStudyIcon_Study_Public( NULL )
	, m_pStudyIcon_Study_Need( NULL )
    , m_pStudyIcon_Study_Need_Cross( NULL )
	, m_pStudyIcon_Study_Studied( NULL )
	, m_pStudyIcon_Study_Unable( NULL )
	, m_pStudyIcon_Study_Disable( NULL )
	, m_pSkillTooltip( NULL )
    , m_pSkillTooltip_Title( NULL )
	, m_eStudyState( E_STUDY_NONE )
	, m_iSkill( 0 )
{
}

void CProductTypeTapButton::CreateSubControl(const CreateArg& arg)
{
	RnButton::CreateSubControl(arg);

	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//학습정보 아이콘
	{
		CUIControl* pUIIcon = new CUIControl( m_pEngineDevice );
		pUIIcon->CreateSub( this, "PRODUCT_WINDOW_TYPE_STUDY_NO_NEED", UI_FLAG_RIGHT | UI_FLAG_TOP );
		RegisterControl( pUIIcon );
		m_pStudyIcon_Study_Public = pUIIcon;
	}
    {
        CUIControl* pUIIcon = new CUIControl( m_pEngineDevice );
        pUIIcon->CreateSub( this, "PRODUCT_WINDOW_TYPE_STUDY_NEED", UI_FLAG_RIGHT | UI_FLAG_TOP , E_CID_STUDY_BUTTON );
        RegisterControl( pUIIcon );
        m_pStudyIcon_Study_Need = pUIIcon;
    }
    {
        CUIControl* pUIIcon = new CUIControl( m_pEngineDevice );
        pUIIcon->CreateSub( this, "PRODUCT_WINDOW_TYPE_STUDY_NEED_CROSS", UI_FLAG_RIGHT | UI_FLAG_TOP );
        RegisterControl( pUIIcon );
        m_pStudyIcon_Study_Need_Cross = pUIIcon;
    }
	{
		CUIControl* pUIIcon = new CUIControl( m_pEngineDevice );
		pUIIcon->CreateSub( this, "PRODUCT_WINDOW_TYPE_STUDY_STUDIED", UI_FLAG_RIGHT | UI_FLAG_TOP );
		RegisterControl( pUIIcon );
		m_pStudyIcon_Study_Studied = pUIIcon;
	}
	{
		CUIControl* pUIIcon = new CUIControl( m_pEngineDevice );
		pUIIcon->CreateSub( this, "PRODUCT_WINDOW_TYPE_STUDY_UNABLE", UI_FLAG_RIGHT | UI_FLAG_TOP );
		RegisterControl( pUIIcon );
		m_pStudyIcon_Study_Unable = pUIIcon;
	}
	{
		CUIControl* pUIIcon = new CUIControl( m_pEngineDevice );
		pUIIcon->CreateSub( this, "PRODUCT_WINDOW_TYPE_DISABLE", UI_FLAG_RIGHT | UI_FLAG_TOP );
		RegisterControl( pUIIcon );
		m_pStudyIcon_Study_Disable = pUIIcon;
	}
	AllHideIcon();

	//숙련도 툴팁
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
	pLineBox->CreateSub( this, "PRODUCT_WINDOW_TYPE_STUDY_TOOLTIP", UI_FLAG_DEFAULT);
	pLineBox->CreateSubControl( "PRODUCT_WINDOW_TYPE_STUDY_TOOLTIP_BOX" );
    pLineBox->SetDiffuseAlpha( (WORD)( 255.0f * 0.7f ) );
	RegisterControl( pLineBox );
	m_pSkillTooltip_Title = pLineBox;
	m_pSkillTooltip_Title->SetVisibleSingle(FALSE);

	CBasicTextBox* pTextBox		= new CBasicTextBox( m_pEngineDevice );
	pTextBox->CreateSub( this, "PRODUCT_WINDOW_TYPE_STUDY_TOOLTIP", UI_FLAG_DEFAULT );
	pTextBox->SetFont ( pFont9 );	
	pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y  );
	RegisterControl( pTextBox );
	pTextBox->SetText( sc::string::format( "%d", 0 ).c_str() );
	m_pSkillTooltip = pTextBox;
	m_pSkillTooltip->SetVisibleSingle(FALSE);
}

void CProductTypeTapButton::SetState( const BYTE eState )
{
	if ( eState < E_STUDY_PUBLIC || eState > E_STUDY_UNABLE )
	{
		return;
	}
	m_eStudyState = eState;
	AllHideIcon();
	switch( m_eStudyState )
	{
	case E_STUDY_PUBLIC :
		m_pStudyIcon_Study_Public->SetVisibleSingle( TRUE );
		break;
	case E_STUDY_NEED :
		m_pStudyIcon_Study_Need->SetVisibleSingle( TRUE );
        m_pStudyIcon_Study_Need_Cross->SetVisibleSingle(TRUE);
		break;
	case E_STUDY_STUDIED :
		m_pStudyIcon_Study_Studied->SetVisibleSingle( TRUE );
		break;
	case E_STUDY_DISABLE :
		m_pStudyIcon_Study_Disable->SetVisibleSingle( TRUE );
		break;
	case E_STUDY_UNABLE :
		m_pStudyIcon_Study_Unable->SetVisibleSingle( TRUE );
		break;
	default:
		return;
	}
}

void CProductTypeTapButton::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{   
    CUIGroup::TranslateUIMessage(ControlID, dwMsg);
    switch( ControlID )
    {
    case E_CID_STUDY_BUTTON:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_DOWN_LIKE( dwMsg ) )
        {
            SetMessageEx( ProductTypeMSG::UI_MSG_PRODUCT_STUDY );   //제작학습 확인메시지박스 팝업 요청 메시지
        }
        break;
    }
}

void CProductTypeTapButton::SetSkill( const int iSkill )
{
	if ( iSkill < 0 || iSkill > Product::MAX_SKILL)	//숙련도는 0~999 까지 가능
	{
		return;
	}
	m_iSkill = iSkill;
	m_pSkillTooltip->SetText( sc::string::format( "%d", iSkill ).c_str() );
}

void CProductTypeTapButton::SetShowSkillTooltip(const bool bState)
{
	if ( m_pSkillTooltip_Title == NULL || m_pSkillTooltip == NULL )
	{
		return;
	}
	m_pSkillTooltip_Title->SetVisibleSingle(bState);
	m_pSkillTooltip->SetVisibleSingle(bState);
}

const bool CProductTypeTapButton::isShowSkillTooltip()
{
	if ( m_pSkillTooltip_Title == NULL || m_pSkillTooltip == NULL )
	{
		return FALSE;
	}
	return m_pSkillTooltip->IsVisible() ? true : false;
}

const BYTE CProductTypeTapButton::GetState()
{
	return m_eStudyState;
}

void CProductTypeTapButton::AllHideIcon()
{
	m_pStudyIcon_Study_Public->SetVisibleSingle( FALSE );
	m_pStudyIcon_Study_Need->SetVisibleSingle( FALSE );
    m_pStudyIcon_Study_Need_Cross->SetVisibleSingle( FALSE );
	m_pStudyIcon_Study_Studied->SetVisibleSingle( FALSE );
	m_pStudyIcon_Study_Unable->SetVisibleSingle( FALSE );
	m_pStudyIcon_Study_Disable->SetVisibleSingle( FALSE );
}