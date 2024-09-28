#include "StdAfx.h"

#include "CheckBox.h"

#include "../../../EngineLib/GUInterface/InterfaceCfg.h"

#include "../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCheckBox::CCheckBox(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_bChecked ( FALSE )
{
	// Blank
}

CCheckBox::~CCheckBox ()
{
	//Blank
}

void CCheckBox::CreateSubControl ( const char* szCheckOnKeyword, const char* szCheckOffKeyword )
{	
    UITextureList::SControl sCheckOn, sCheckOff;

    if( CInterfaceCfg::GetInstance().ArrangeInfo( szCheckOnKeyword, sCheckOn ) == FALSE )
    {
        std::string strError = sc::string::format(" \"%s\" 컨트롤 Id를 찾지 못하였습니다.", szCheckOnKeyword );
        GASSERT_MSG( 0, strError );
        return;
    }
    if( CInterfaceCfg::GetInstance().ArrangeInfo( szCheckOffKeyword, sCheckOff ) == FALSE )
    {
        std::string strError = sc::string::format(" \"%s\" 컨트롤 Id를 찾지 못하였습니다.", szCheckOffKeyword );
        GASSERT_MSG( 0, strError );
        return;
    }

    m_rcTexPosOn = sCheckOn.m_rcTexturePos;
    m_strTextureNameOn = sCheckOn.m_strFileName.c_str();

    m_rcTexPosOff = sCheckOff.m_rcTexturePos;
    m_strTextureNameOff = sCheckOff.m_strFileName.c_str();

	SetUseRender ( TRUE );

	Reset ();
}

void CCheckBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DWORD dwMsg = GetMessageEx();

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		if ( dwMsg&UIMSG_LB_DOWN )
		{
			Toggle();
            AddMessageEx( UIMSG_CHECKBOX_PUSH );
		}
	}
}

void CCheckBox::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CCheckBox::Reset ()
{
	SetCheck ( FALSE );
}

void CCheckBox::SetCheck ( const BOOL bCheck )
{
	m_bChecked = bCheck;

	if ( m_bChecked )
	{
		SetCheckOn ();
	}
	else
	{
		SetCheckOff ();
	}
}

void CCheckBox::SetCheckOn  ()
{
	m_bChecked = TRUE;

	CUIControl::SetTexturePos  ( m_rcTexPosOn );
	CUIControl::SetTextureName ( m_strTextureNameOn );
}

void CCheckBox::SetCheckOff ()
{
	m_bChecked = FALSE;

	CUIControl::SetTexturePos  ( m_rcTexPosOff );
	CUIControl::SetTextureName ( m_strTextureNameOff );
}
