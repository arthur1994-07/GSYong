#include "StdAfx.h"
#include "./WebWindowBase.h"

#include "../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../EngineLib/Common/CommonWeb.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL CWebWindowBase::s_bSkipVisibleWeb = FALSE;

CWebWindowBase::CWebWindowBase( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx( pInterface, pEngineDevice )
    , m_iAlignLeft( 2 )
    , m_iAlignTop( 2 )
    , m_iAlignRight( 4 )
    , m_iAlignBottom( 4 )
{
    m_iWebID = COMMON_WEB::IssueNewID();
}

void CWebWindowBase::SetAddress( const TCHAR* pszAddress )
{
	if( pszAddress )
		m_strAddress = pszAddress;
	else
		m_strAddress.clear();
}

VOID CWebWindowBase::SetPostArgument( const TCHAR* pszPOST )
{
	if ( pszPOST )
		m_strPostArgument = pszPOST;
	else
		m_strPostArgument.clear();
}

VOID CWebWindowBase::AlignPosition()
{
    if( COMMON_WEB::GetVisible( m_iWebID ) )
	{
        const UIRECT& rcBody = GetBody()->GetGlobalPos();

        COMMON_WEB::MoveWindow(
            m_iWebID,
			static_cast< INT >( rcBody.left ) + m_iAlignLeft,
			static_cast< INT >( rcBody.top ) + m_iAlignTop,
			static_cast< INT >( rcBody.sizeX ) - m_iAlignRight,
			static_cast< INT >( rcBody.sizeY ) - m_iAlignBottom,
            TRUE );
	}
	else
	{
        long lResolution = m_pInterface->UiGetResolution();
	    WORD wX_RES = HIWORD( lResolution );
	    WORD wY_RES = LOWORD( lResolution );

		COMMON_WEB::MoveWindow(
			m_iWebID,
			static_cast< INT >( wX_RES ),
			static_cast< INT >( wY_RES ),
			0,
			0,
			FALSE );
	}
}

VOID CWebWindowBase::SetVisibleSingle( BOOL bVisible )
{
    if( !s_bSkipVisibleWeb )
    {
        if( bVisible )
        {
            COMMON_WEB::SetVisible( m_iWebID, bVisible );
			COMMON_WEB::Navigate( m_iWebID, GetAddress(), TRUE, m_strPostArgument.empty() ? NULL : m_strPostArgument.c_str() );
            AlignPosition();
        }
        else
        {
            COMMON_WEB::SetVisible( m_iWebID, bVisible );
		    COMMON_WEB::Navigate( m_iWebID, _T( "" ), FALSE, m_strPostArgument.empty() ? NULL : m_strPostArgument.c_str() );
            AlignPosition();
            COMMON_WEB::SetFocusParent();
        }
    }

    CUIWindowEx::SetVisibleSingle( bVisible );
}

VOID CWebWindowBase::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if( !IsVisible() )
		return;

	AlignPosition();
}

VOID* CWebWindowBase::GetWebBrowser()
{
	return COMMON_WEB::GetWebBrowser( m_iWebID );
}

BOOL CWebWindowBase::IsCompleteLoad()
{
	return COMMON_WEB::IsCompleteLoad( m_iWebID );
}

VOID CWebWindowBase::SetCompleteLoad( BOOL bCompleteLoad )
{
	COMMON_WEB::SetCompleteLoad( m_iWebID, bCompleteLoad );
}