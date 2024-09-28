#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"

#include "PetQuickSkillSlot.h"
#include "PetSkillImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPetQuickSkillSlot::CPetQuickSkillSlot(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pSkillImage ( NULL )
	, m_pSkillMouseOver ( NULL )
	, m_pd3dDevice ( NULL )
	, m_pSkillStatic ( NULL )
	, m_wSlot ( 0 )
{
}

CPetQuickSkillSlot::~CPetQuickSkillSlot ()
{
}

void  CPetQuickSkillSlot::CreateSubControl ( const WORD wSlot )
{
	m_wSlot = wSlot;

	CreateSkillImage ();
	CreateSkillStatic ();
	CreateMouseOver ();
}

void CPetQuickSkillSlot::CreateSkillImage ()
{
	CPetSkillImage* pSkillImage = new CPetSkillImage(m_pGaeaClient, m_pEngineDevice);
	pSkillImage->CreateSub ( this, "PET_SKILL_IMAGE" );
	pSkillImage->CreateSubControl ();
	pSkillImage->SetUseRender ( TRUE );
	pSkillImage->SetVisibleSingle ( FALSE );
    RegisterControl ( pSkillImage );
	m_pSkillImage = pSkillImage;
}

void CPetQuickSkillSlot::CreateSkillStatic ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "PET_SKILL_SLOT_STATIC" );
	pTextBox->SetFont ( pFont9 );
	// 아래의 COMMENT는 데이터에서 읽어오지않는다;
	pTextBox->AddText ( COMMENT::PET_SKILLSLOT_TYPE[m_wSlot].c_str()  );
	pTextBox->SetVisibleSingle ( TRUE );
	RegisterControl ( pTextBox );
	m_pSkillStatic = pTextBox;
}

void CPetQuickSkillSlot::CreateMouseOver ()
{
	CUIControl* pSkillMouseOver = new CUIControl(m_pEngineDevice);
	pSkillMouseOver->CreateSub ( this, "PET_SKILL_MOUSE_OVER", UI_FLAG_DEFAULT, QUICK_SKILL_MOUSE_OVER );
	pSkillMouseOver->SetVisibleSingle ( TRUE );
	RegisterControl ( pSkillMouseOver );
	m_pSkillMouseOver = pSkillMouseOver;
}

void CPetQuickSkillSlot::UpdateSlot( SNATIVEID sNativeID )
{	
	if ( !m_pSkillImage )
	{
		GASSERT ( 0 && "스킬 이미지가 만들어지지 않았습니다."  );
		return ;
	}

	if ( sNativeID != NATIVEID_NULL() )
	{
		m_pSkillImage->SetSkill ( sNativeID );
		m_pSkillImage->SetVisibleSingle ( TRUE );
		m_pSkillImage->SetSkillProcess ( sNativeID );
	}
	else
	{
		m_pSkillImage->SetVisibleSingle ( FALSE );
		m_pSkillImage->ResetSkill ();
	}	
}

void CPetQuickSkillSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SNATIVEID sNativeID = m_pGaeaClient->GetPetClient()->m_sActiveSkillID[m_wSlot];
	UpdateSlot( sNativeID );
}

UIRECT CPetQuickSkillSlot::GetAbsPosSkillImage ()
{	
	if ( !m_pSkillImage )
	{
		GASSERT ( 0 && "m_pSkillImage가 NULL입니다." );
		return UIRECT ();
	}
	
	return m_pSkillImage->GetGlobalPos();	
}