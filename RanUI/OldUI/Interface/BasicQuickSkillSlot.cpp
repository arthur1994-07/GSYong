#include "StdAfx.h"
#include "BasicQuickSkillSlot.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../Skill/SkillImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicQuickSkillSlot::CBasicQuickSkillSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pSkillImage( NULL )
    , m_pSkillMouseOver( NULL )
    , m_pd3dDevice( NULL )
{
}

CBasicQuickSkillSlot::~CBasicQuickSkillSlot()
{
}

void  CBasicQuickSkillSlot::CreateSubControl()
{
    CreateSkillImage();
    CreateMouseOver();
}

void CBasicQuickSkillSlot::CreateSkillImage()
{
    CSkillImage* pSkillImage = new CSkillImage(m_pGaeaClient, m_pEngineDevice);
    pSkillImage->CreateSub( this, "QUICK_SKILL_IMAGE" );
    pSkillImage->CreateSubControl();
    pSkillImage->SetUseRender( TRUE );
    pSkillImage->SetVisibleSingle( FALSE );
    RegisterControl( pSkillImage );
    m_pSkillImage = pSkillImage;
}

void CBasicQuickSkillSlot::SetUseSkillImageTwinkle( bool bUse )
{
    if( !m_pSkillImage)
    {
        GASSERT( 0 && "CBasicQuickSkillSlot::SetUseSkillImageTwinkle(), m_pSkillImage is NULL." );
        return ;
    }

    m_pSkillImage->SetUseTwinkle( bUse );
}

void	CBasicQuickSkillSlot::UpdateSlot( SNATIVEID sNativeID )
{	
    if ( !m_pSkillImage )
    {
        GASSERT ( 0 && "스킬 이미지가 만들어지지 않았습니다."  );
        return ;
    }

    if ( sNativeID != NATIVEID_NULL() )
    {
        m_pSkillImage->SetSkill( sNativeID );
        m_pSkillImage->SetVisibleSingle( TRUE );
        m_pSkillImage->SetSkillProcess( sNativeID );
    }
    else
    {
        m_pSkillImage->SetVisibleSingle( FALSE );
        m_pSkillImage->ResetSkill();
    }	
}

void CBasicQuickSkillSlot::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    SNATIVEID sNativeID = m_pGaeaClient->GetCharacter()->GetskillRunSlot();
    UpdateSlot( sNativeID );
}

void CBasicQuickSkillSlot::CreateMouseOver()
{
    CUIControl* pSkillMouseOver = new CUIControl(m_pEngineDevice);
    pSkillMouseOver->CreateSub( this, "QUICK_SKILL_MOUSE_OVER", UI_FLAG_DEFAULT, QUICK_SKILL_MOUSE_OVER );
    pSkillMouseOver->SetVisibleSingle( TRUE );
    RegisterControl( pSkillMouseOver );
    m_pSkillMouseOver = pSkillMouseOver;
}

UIRECT  CBasicQuickSkillSlot::GetAbsPosSkillImage()
{	
    if ( !m_pSkillImage )
    {
        GASSERT ( 0 && "널입니다." );
        return UIRECT ();
    }

    return m_pSkillImage->GetGlobalPos();	
}

const UIRECT& CBasicQuickSkillSlot::GetSkillImageGlobalPos()
{
    return m_pSkillImage->GetGlobalPos();
}

void CBasicQuickSkillSlot::TurnHorizontal ()
{
    m_pSkillImage->ResetControl     ( GetGlobalPos(), "QUICK_SKILL_IMAGE_H" );
    m_pSkillMouseOver->ResetControl ( GetGlobalPos(), "QUICK_SKILL_MOUSE_OVER_H" );
}

void CBasicQuickSkillSlot::TurnVertical ()
{
    m_pSkillImage->ResetControl     ( GetGlobalPos(), "QUICK_SKILL_IMAGE" );
    m_pSkillMouseOver->ResetControl ( GetGlobalPos(), "QUICK_SKILL_MOUSE_OVER" );
}

MyBasicQuickSkillSlot::MyBasicQuickSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CBasicQuickSkillSlot( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyBasicQuickSkillSlot::CreateUIWindowAndRegisterOwnership()
{
    CBasicQuickSkillSlot::Create( BASIC_QUICK_SKILL_SLOT, "BASIC_QUICK_SKILL_SLOT" );
    CBasicQuickSkillSlot::CreateSubControl();
    CBasicQuickSkillSlot::SetUseSkillImageTwinkle( false ); // 스킬 종료시 깜빡이는 액션을 사용하지 않습니다.
    CBasicQuickSkillSlot::m_pInterface->UiRegisterControl( this );
    CBasicQuickSkillSlot::m_pInterface->UiShowGroupBottom( BASIC_QUICK_SKILL_SLOT );
}

const UIRECT& MyBasicQuickSkillSlot::GetSkillImageGlobalPos()
{
    return CBasicQuickSkillSlot::GetSkillImageGlobalPos();
}
