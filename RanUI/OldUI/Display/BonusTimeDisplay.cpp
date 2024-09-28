#include "StdAfx.h"
#include "BonusTimeDisplay.h"
#include "BonusTimeType.h"
#include "../../InnerInterface.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static const float fKeepTime = 5.0f;

CBonusTimeDisplay::CBonusTimeDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
{
}

CBonusTimeDisplay::~CBonusTimeDisplay()
{
}

void CBonusTimeDisplay::CreateSubControl()
{

	m_pBonus_TYPE = new CBonusTimeType(m_pEngineDevice);
	m_pBonus_TYPE->CreateSub ( this, "BONUS_TIME" , UI_FLAG_DEFAULT, BONUS_TIME );
	m_pBonus_TYPE->CreateSubControl ( "BONUS_TIME" );
	m_pBonus_TYPE->SetVisibleSingle ( FALSE );
	m_pBonus_TYPE->SetUseRender ( FALSE );
	m_pBonus_TYPE->STOP ();
	m_pBonus_TYPE->RESET ();		
	RegisterControl ( m_pBonus_TYPE );

	m_pBonus_KEEP = new CUIControl(m_pEngineDevice);
	m_pBonus_KEEP->CreateSub ( this,  "BONUS_TIME" );
	m_pBonus_KEEP->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pBonus_KEEP );


	m_pPositionControl = new CUIControl(m_pEngineDevice);
	m_pPositionControl->CreateSub ( this, "BONUSTIME_DISPLAY_POSITION" );
	m_pPositionControl->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pPositionControl );
}

bool	CBonusTimeDisplay::START ()
{
	m_pBonus_TYPE->SetVisibleSingle ( TRUE );
    m_pBonus_TYPE->START ();

	return true;
}

bool	CBonusTimeDisplay::RESET ()
{
	m_pBonus_TYPE->RESET ();

    return true;
}

bool	CBonusTimeDisplay::STOP ()
{
	m_pBonus_TYPE->STOP ();
	m_pBonus_TYPE->SetVisibleSingle ( FALSE );

	return true;
}

void CBonusTimeDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	D3DXVECTOR3 vPos = m_pGaeaClient->GetCharacter()->GetPosBodyHeight();

	static D3DXVECTOR3 vScreenBack;
	D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

	// 마우스 움직임에 흔들림을 보정한다.
	if( abs( vScreenBack.x - vScreen.x ) < 1.0f )
	{
		vScreen.x = vScreenBack.x;
	}
	
	bool bPLAYING( false );

	float fTime = 0.0f;

	const UIRECT& rcOriginPos = m_pBonus_TYPE->GetGlobalPos ();

	D3DXVECTOR2 vPos2;
	vPos2.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ));
	vPos2.y = m_pPositionControl->GetGlobalPos().top;

	if ( m_pBonus_TYPE->ISPLAYING () )
	{
		m_pBonus_TYPE->SetGlobalPos ( vPos2 );

		bPLAYING = true;
	}
	else
	{
		STOP ();
	}

	vScreenBack = vScreen;

	if ( !bPLAYING )	SetVisibleSingle( FALSE );
}

