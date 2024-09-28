#include "StdAfx.h"
#include "ReferSkillSlot.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../InnerInterface.h"

#include "../Skill/SkillSlot.h"
#include "../Skill/SkillImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CReferSkillSlot::CReferSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIGroup( pEngineDevice )

	, m_pGaeaClient( pGaeaClient )
	, m_pInterface( pInterface )

	, m_pSkillImage( NULL )
	, m_pTextBox( NULL )
{
}

void CReferSkillSlot::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    //테두리와 배경
    CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
    pLinBoxSmart->CreateSub( this, "ANOTHER_SKILL_SLOT_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pLinBoxSmart->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    RegisterControl( pLinBoxSmart );

	//스킬 아이콘 이미지
    {
        CSkillImage* pSkillImage = new CSkillImage ( m_pGaeaClient, m_pEngineDevice );
        pSkillImage->CreateSub( this, "ANOTHER_SKILL_SLOT_IMAGE", UI_FLAG_DEFAULT, SKILL_SLOT_IMAGE );
        pSkillImage->CreateSubControl();
        pSkillImage->SetUseRender( TRUE );
        RegisterControl( pSkillImage );
        m_pSkillImage = pSkillImage;
    }

	//텍스트
    {
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "ANOTHER_SKILL_SLOT_TEXT" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        pStaticText->SetLineInterval( 4.0f );
        RegisterControl( pStaticText );
        m_pTextBox = pStaticText;
    }
}

void CReferSkillSlot::SetNativeSkillID( SNATIVEID sNativeID, WORD nLevel, BOOL bMaster )
{
	m_sNativeID = sNativeID;

    m_pTextBox->ClearText();
	m_pSkillImage->ResetSkill();

	if( !m_pSkillImage )
        return;

    if( !m_pTextBox )
        return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sNativeID );
	if( !pSkill )
		return;

	m_pSkillImage->SetSkill( m_sNativeID );

    if( nLevel > 0 )	//	배운 스킬이 있을 때
    {
		m_pSkillImage->SetDiffuseAlpha( 255 );

		CString strText;
		strText.Format(
			"%s %d [%s]",
			ID2GAMEWORD( "SKILL_SLOT", 0 ),
			pSkill->m_sBASIC.dwGRADE,
			pSkill->GetName() );
		m_pTextBox->AddTextNoSplit( strText, NS_UITEXTCOLOR::WHITE );

        if( bMaster )
			strText = ID2GAMEWORD( "SKILL_SLOT", 4 );
		else
            strText.Format( "%s %d ", ID2GAMEWORD( "SKILL_SLOT", 3 ), nLevel );
		m_pTextBox->AddTextNoSplit( strText, NS_UITEXTCOLOR::WHITE );
    }
    else	//	배운 스킬이 없을 경우
    {
		m_pSkillImage->SetDiffuseAlpha( 100 );

		CString strText;
		strText.Format(
			"%s %d [%s]",
			ID2GAMEWORD( "SKILL_SLOT", 0 ),
			pSkill->m_sBASIC.dwGRADE,
			pSkill->GetName() );
		m_pTextBox->AddTextNoSplit( strText, NS_UITEXTCOLOR::DARKGRAY );

        strText.Format( "%s %d", ID2GAMEWORD( "SKILL_SLOT", 3 ), 0 );
        m_pTextBox->AddTextNoSplit( strText, NS_UITEXTCOLOR::DARKGRAY );
    }
}

SNATIVEID CReferSkillSlot::GetNativeSkillID()
{
    return m_sNativeID;
}