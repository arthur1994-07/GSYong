#include "StdAfx.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "CostumColorInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCostumColorInfo::CCostumColorInfo(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pColor1Back(NULL)
    , m_pColor2Back(NULL)
    , m_pColor1Color(NULL)
    , m_pColor2Color(NULL)
{
}

CCostumColorInfo::~CCostumColorInfo(void)
{
}


void CCostumColorInfo::CreateSubControl ()
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "COLOR1_INFO_BACK" );
	pControl->SetVisibleSingle ( TRUE );
	RegisterControl ( pControl );
	m_pColor1Back = pControl;
	m_pColor1Back->SetUseRender( TRUE );
	m_pColor1Back->SetDiffuse( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "COLOR2_INFO_BACK" );
	pControl->SetVisibleSingle ( TRUE );
	RegisterControl ( pControl );
	m_pColor2Back = pControl;
	m_pColor2Back->SetUseRender( TRUE );
	m_pColor2Back->SetDiffuse( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "COLOR1_INFO_COLOR" );
	pControl->SetVisibleSingle ( TRUE );
	RegisterControl ( pControl );
	m_pColor1Color = pControl;
	m_pColor1Color->SetUseRender( TRUE );
	m_pColor1Color->SetDiffuse( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "COLOR2_INFO_COLOR" );
	pControl->SetVisibleSingle ( TRUE );
	RegisterControl ( pControl );
	m_pColor2Color = pControl;
	m_pColor2Color->SetUseRender( TRUE );
	m_pColor2Color->SetDiffuse( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
}

void CCostumColorInfo::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CCostumColorInfo::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case INFO_COLOR1_BACK:
	case INFO_COLOR2_BACK:
	case INFO_COLOR1_COLOR:
	case INFO_COLOR2_COLOR:
		{
		}
		break;
	}
}

void CCostumColorInfo::SetSelectColor( DWORD dwMainColor, DWORD dwSubColor )
{
	DWORD dwColor = dwMainColor;
	int	R,G,B;
	R = (dwColor&0xff0000)>>16;
	G = (dwColor&0xff00)>>8;
	B = dwColor&0xff;

	m_pColor1Color->SetDiffuse( D3DCOLOR_ARGB( 255, R, G, B ) );

	dwColor = dwSubColor;
	R = (dwColor&0xff0000)>>16;
	G = (dwColor&0xff00)>>8;
	B = dwColor&0xff;

	m_pColor2Color->SetDiffuse( D3DCOLOR_ARGB( 255, R, G, B ) );
}