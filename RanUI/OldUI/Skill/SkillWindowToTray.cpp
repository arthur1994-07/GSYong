#include "StdAfx.h"
#include "SkillWindowToTray.h"
#include "../Skill/SkillImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD CSkillWindowToTray::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB ( 200, 255, 255, 255 );
const float CSkillWindowToTray::fDEFAULT_MOUSE_INTERPOLIATION = 20.0f;

CSkillWindowToTray::CSkillWindowToTray(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pSkillImage ( NULL )
    , m_sNativeID ( NATIVEID_NULL() )
    , m_bUseSnap ( FALSE )
{
}

CSkillWindowToTray::~CSkillWindowToTray()
{
}

void CSkillWindowToTray::CreateSubControl()
{
	CSkillImage* pSkillImage = new CSkillImage(m_pGaeaClient, m_pEngineDevice);
	pSkillImage->CreateSub( this, "SKILL_WINDOW_TO_TARY_IMAGE" );
	pSkillImage->CreateSubControl();
	pSkillImage->SetUseRender( TRUE );
	pSkillImage->SetVisibleSingle( FALSE );
	RegisterControl( pSkillImage );
    m_pSkillImage = pSkillImage;

	ResetSkill();
}

void CSkillWindowToTray::SetSkill( SNATIVEID sNativeID )
{
	if ( sNativeID == NATIVEID_NULL() )
	{
		GASSERT( 0 && "무효화한 ID가 설정되려고 합니다." );
		return ;
	}

	m_sNativeID = sNativeID;	
	m_pSkillImage->SetSkill( m_sNativeID );
	m_pSkillImage->SetVisibleSingle( TRUE );
	m_pSkillImage->SetDiffuse( dwDEFAULT_TRANSPARENCY );
}

void  CSkillWindowToTray::ResetSkill()
{
	m_pSkillImage->ResetSkill();
	m_pSkillImage->SetVisibleSingle( FALSE );
	m_sNativeID = NATIVEID_NULL();
}

void CSkillWindowToTray::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	
	if ( !m_pSkillImage ) return ;

	if ( m_sNativeID != NATIVEID_NULL() )
	{		
		if ( !IsUseSnap() )
		{
			const D3DXVECTOR2 vImagePos ( x - fDEFAULT_MOUSE_INTERPOLIATION, y - fDEFAULT_MOUSE_INTERPOLIATION );
			SetGlobalPos( vImagePos );
		}		
	}
}

MySkillWindowToTray::MySkillWindowToTray( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CSkillWindowToTray( pGaeaClient, pInterface, pEngineDevice )
{
}

void MySkillWindowToTray::CreateUIWindowAndRegisterOwnership()
{
    CSkillWindowToTray::Create( SKILL_WINDOW_TO_TRAY, "SKILL_WINDOW_TO_TRAY" );
    CSkillWindowToTray::CreateSubControl();
    CSkillWindowToTray::m_pInterface->UiRegisterControl( this );
    CSkillWindowToTray::m_pInterface->UiShowGroupTop( SKILL_WINDOW_TO_TRAY );
}

void MySkillWindowToTray::SetSkill( SNATIVEID sNativeID )
{
    CSkillWindowToTray::SetSkill( sNativeID );
}

SNATIVEID MySkillWindowToTray::GetSkill()
{
    return CSkillWindowToTray::GetSkill();
}

void MySkillWindowToTray::ResetSkill()
{
    CSkillWindowToTray::ResetSkill();
}

void MySkillWindowToTray::SetUIWindowGlobalPos( const UIRECT& rcPos )
{
    CSkillWindowToTray::SetGlobalPos( rcPos );
}

void MySkillWindowToTray::SetUseSnap( BOOL bUseSnap )
{
    CSkillWindowToTray::SetUseSnap( bUseSnap );
}