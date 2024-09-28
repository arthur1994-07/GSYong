#include "pch.h"

#include "../DxTools/RENDERPARAM.h"

#include "UIControl.h"
#include "../DxTools/DxInputDevice.h"
#include "../DxTools/TextureManager.h"
#include "InterfaceCfg.h"

#include "UIMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIControl::CUIControl(EngineDeviceMan* pEngineDevice)
	: m_pEngineDevice(pEngineDevice)
    , m_WndID( 0 )
	, m_pTexture( NULL )
	, m_bVisible( TRUE )
	, m_fFadeElapsedTime( 0.f )
	, m_fFadeLimitTime( UI_FADETIME )
	, m_fVisibleRate ( 1.0f )
	, m_VisibleState( VISIBLE_MODE )
	, m_bTransparency( FALSE )
	, m_wAlignFlag( 0 )
	, m_bUseRender( TRUE )
	, m_pParent( NULL )
	, m_bCheckProtectSize( FALSE )
	, m_bCheckNonBoundary( false )
	, m_dwBeginAlpha( 255 )
	, m_bNO_UPDATE( FALSE )
	, m_dwMsg( 0 )
	, m_bExclusiveControl( FALSE )
	, m_bFocusControl( FALSE )
	, m_nFoldID( 0 )
	, m_bCheckBound(TRUE)
	, m_bALLWAYS_MSG(FALSE)
 	, m_bNoMSG(FALSE)
	, m_nTextureSizeWidth( 0 )
	, m_nTextureSizeHeight( 0 )
	, m_lResolutionBack( MAKELONG( 768, 1024 ) )
	, m_bChangeSize( FALSE )
	, m_pFont ( NULL )
	, m_fDefaultFontSize( 0.f )
	, m_bFlowBar( FALSE )
	, m_iFlowBarType( FLOW_BAR_RIGHT )
	, m_bLock( true )
{
}

CUIControl::~CUIControl()
{
	CUIControl::InvalidateDeviceObjects ();
	CUIControl::DeleteDeviceObjects ();
}


CUIControl*	CUIControl::GetTopParent ()
{	
	CUIControl* pCurNode = GetParent ();
	CUIControl* pParent = pCurNode;
	while ( pCurNode )
	{
		pParent = pCurNode;
		pCurNode = pCurNode->GetParent ();
	}

	return pParent;
}

void CUIControl::SetVisibleSingle(BOOL visible)
{

	m_bVisible = visible;
	m_fFadeElapsedTime = 0;

	if ( m_bVisible)
	{
		m_VisibleState = VISIBLE_MODE;
		m_fVisibleRate = 1.0f;
	}
	else
	{
		m_VisibleState = INVISIBLE_MODE;
		m_fVisibleRate = 0.0f;
	}

}

void CUIControl::SetVertexPos()
{
	m_UIVertex[0] = D3DXVECTOR2 ( m_rcGlobalPos.left,	m_rcGlobalPos.top );
	m_UIVertex[1] = D3DXVECTOR2 ( m_rcGlobalPos.right,	m_rcGlobalPos.top );
	m_UIVertex[2] = D3DXVECTOR2 ( m_rcGlobalPos.right,	m_rcGlobalPos.bottom );
	m_UIVertex[3] = D3DXVECTOR2 ( m_rcGlobalPos.left,	m_rcGlobalPos.bottom );
}

void CUIControl::SetVertexPos( float z )
{
	m_UIVertex[0].z = z;
	m_UIVertex[1].z = z;
	m_UIVertex[2].z = z;
	m_UIVertex[3].z = z;
}

void CUIControl::SetDiffuseAlpha ( WORD wAlpha )
{
    D3DCOLOR cColor = m_UIVertex[0].GetDiffuse();
    CUIControl::SetDiffuse ( D3DCOLOR_ARGB ( wAlpha, (cColor>>16)&0xFF, (cColor>>8)&0xFF, (cColor)&0xFF ) );
}

void CUIControl::SetAlignedGlobalPos(const UIRECT& Pos)
{
	m_rcGlobalPos = Pos;
	CheckProtectSize();
	CheckBoundary( true );
	SetVertexPos();
}

void CUIControl::SetGlobalPos(const UIRECT& Pos)
{
	m_rcGlobalPos = Pos;
	CheckProtectSize();
	CheckBoundary();
	SetVertexPos();
}

void CUIControl::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	m_rcGlobalPos.left	=vPos.x;
	m_rcGlobalPos.right	=vPos.x + m_rcGlobalPos.sizeX; // MEMO : 여기도 -1이 들어가야 할거 같은데 변경 불가.
	m_rcGlobalPos.top	=vPos.y;
	m_rcGlobalPos.bottom=vPos.y + m_rcGlobalPos.sizeY;

    CheckBoundary(); // MEMO
	SetVertexPos();
}

void CUIControl::AddPosition ( const D3DXVECTOR2& vPos )
{
    m_rcLocalPos.left	 += vPos.x;
    m_rcLocalPos.right	 += vPos.x;
    m_rcLocalPos.top	 += vPos.y;
    m_rcLocalPos.bottom  += vPos.y;

    m_rcGlobalPos.left	 += vPos.x;
    m_rcGlobalPos.right	 += vPos.x;
    m_rcGlobalPos.top	 += vPos.y;
    m_rcGlobalPos.bottom += vPos.y;

    CheckBoundary (); // MEMO
    SetVertexPos();
}

void CUIControl::SetBorder( const D3DXVECTOR4& vBorder )
{
    m_rcBorder.left 	= vBorder.x;
    m_rcBorder.right	= vBorder.x + vBorder.w;
    m_rcBorder.top	    = vBorder.y;
    m_rcBorder.bottom   = vBorder.y + vBorder.z;
    m_rcBorder.sizeX    = vBorder.w;
    m_rcBorder.sizeY    = vBorder.z;

}

void CUIControl::CreateSub ( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag, UIGUID WndID )
{
	GASSERT( szControlKeyword );

	if( pParent )
		SetParent ( pParent );

	INTERFACE_CFG uiCfg;
    // 새로운 UIXML에서 찾지 못할경우 구형 UIXML에서 검색한다.
    if( !CUIConfigMgr::GetInstance().FindConfig( szControlKeyword, uiCfg ) )
    {
        // 예전 UIXML에서 텍스처 검
        if( CInterfaceCfg::GetInstance().ArrangeInfo( szControlKeyword, uiCfg ) == FALSE )
        {
            std::string strError = sc::string::format(" \"%s\" 컨트롤 Id를 찾지 못하였습니다.", szControlKeyword );
            GASSERT_MSG( 0, strError );
            return;
        }
    }

	

	SetWndID( WndID );

	// ex 버전은 USHRT_MAX세팅됨
	if( wAlignFlag == USHRT_MAX )
		SetAlignFlag ( uiCfg.wAlignFlag );
	else
		SetAlignFlag ( wAlignFlag );

	SetUseRender ( uiCfg.strTextureName.GetLength () > 0 );
	SetControlNameEx ( szControlKeyword );

	m_rcLocalPos = uiCfg.rcControlPos;

	UIRECT globalRect;

	if( pParent )
	{
		UIRECT rcParentPos = pParent->GetGlobalPos ();
		globalRect = UIRECT(
            rcParentPos.left + uiCfg.rcControlPos.left,
            rcParentPos.top + uiCfg.rcControlPos.top,
			uiCfg.rcControlPos.sizeX,
            uiCfg.rcControlPos.sizeY );
	}
	else
	{
		globalRect = AlignMainControl ( uiCfg.rcControlPos );
	}

    CUIControl::SetBorder( uiCfg.vBorder );
	CUIControl::SetGlobalPos( globalRect );
	CUIControl::SetTexturePos( uiCfg.rcTexturePos );
	CUIControl::SetTextureName( uiCfg.strTextureName );
}

void CUIControl::SetTexturePos ( const UIRECT& rcTexPos )
{
	m_rcTexurePos = rcTexPos;

	m_UIVertex[0].SetTexturePos(m_rcTexurePos.left, m_rcTexurePos.top);
	m_UIVertex[1].SetTexturePos(m_rcTexurePos.right, m_rcTexurePos.top);
	m_UIVertex[2].SetTexturePos(m_rcTexurePos.right, m_rcTexurePos.bottom);
	m_UIVertex[3].SetTexturePos(m_rcTexurePos.left, m_rcTexurePos.bottom);
}

void CUIControl::SetTexturePos(int index, const D3DXVECTOR2& vPos)
{
	m_UIVertex[index].SetTexturePos(vPos.x, vPos.y);
}

void CUIControl::SetDiffuse(D3DCOLOR _diffuse)
{
	m_UIVertex[0].SetDiffuse(_diffuse);
	m_UIVertex[1].SetDiffuse(_diffuse);
	m_UIVertex[2].SetDiffuse(_diffuse);
	m_UIVertex[3].SetDiffuse(_diffuse);
}

void CUIControl::SetTextureName ( const CString& strTextureName )
{
	if (!strTextureName.IsEmpty())
        m_strTextureName = strTextureName;	
	else
        m_strTextureName.Empty();
}

void CUIControl::SetLocalPos ( const D3DXVECTOR2& vPos )
{
	UIRECT rcLocalPosNew = GetLocalPos();
	rcLocalPosNew.left = vPos.x;
	rcLocalPosNew.top = vPos.y;
	SetLocalPos( rcLocalPosNew );
}

void CUIControl::AddLocalPos ( const D3DXVECTOR2& vPos )
{
    UIRECT rcLocalPosNew = GetLocalPos();
    rcLocalPosNew.left   += vPos.x;
    rcLocalPosNew.right  += vPos.x;
    rcLocalPosNew.top    += vPos.y;
    rcLocalPosNew.bottom += vPos.y;
    SetLocalPos( rcLocalPosNew );
}

BOOL CUIControl::SetFade ( BOOL bFadeIn )
{	
	if ( bFadeIn )
        return CUIControl::SetFadeIn ();
	else
        return CUIControl::SetFadeOut();
}

BOOL CUIControl::IsFading()
{
	if (m_VisibleState == FADEIN_MODE || m_VisibleState == FADEOUT_MODE)
        return TRUE;
	else
        return FALSE;
}

void CUIControl::SetTransparentOption ( BOOL bTransparency )
{
	m_bTransparency = bTransparency;
}

void CUIControl::ResizeWindow(const UIRECT& rcNew)
{
	SetControlSize( (int) rcNew.sizeX, (int) rcNew.sizeY );
}

void CUIControl::ResizeWindow(const D3DXVECTOR2& vNew)
{
	SetControlSize( (int) vNew.x, (int) vNew.y );
}

void CUIControl::AlignMainControlEX ( const UIRECT& rcLastParentPos, const UIRECT& rcNewParentPos )
{
	WORD X_RES = (WORD) rcNewParentPos.sizeX;
	WORD Y_RES = (WORD) rcNewParentPos.sizeY;

	WORD LAST_X_RES = (WORD) rcLastParentPos.sizeX;
	WORD LAST_Y_RES = (WORD) rcLastParentPos.sizeY;


	const UIRECT& rcPos = GetLocalPos();
	UIRECT rcNewPos = rcPos;
	const WORD wFlag = GetAlignFlag ();

	//	sizeX
	if ( wFlag & UI_FLAG_XSIZE )
	{
		rcNewPos.left = 0.0f;
		rcNewPos.sizeX = (float)X_RES;
	}
	else if( wFlag & UI_FLAG_XSIZE_PROPORTION )
	{
		rcNewPos.sizeX = rcNewPos.sizeX * (float)X_RES/ (float)LAST_X_RES;
	}

	//	sizeY
	if ( wFlag & UI_FLAG_YSIZE )
	{
		rcNewPos.top = 0.0f;
		rcNewPos.sizeY = (float)Y_RES;
	}
	else if( wFlag &  UI_FLAG_YSIZE_PROPORTION )
	{
		rcNewPos.sizeY = rcNewPos.sizeY * (float)Y_RES / (float)LAST_Y_RES;
	}

	m_rcLocalPos.SetPosition( rcNewPos.left, rcNewPos.top, rcNewPos.sizeX, rcNewPos.sizeY );
	CUIControl::SetGlobalPos ( m_rcLocalPos );
}

void CUIControl::AlignSubControlEX( const UIRECT& rcLastParentPos, const UIRECT& rcNewParentPos )
{
	WORD X_RES = (WORD) rcNewParentPos.sizeX;
	WORD Y_RES = (WORD) rcNewParentPos.sizeY;

	WORD LAST_X_RES = (WORD) rcLastParentPos.sizeX;
	WORD LAST_Y_RES = (WORD) rcLastParentPos.sizeY;

	const UIRECT& rcOldPos = GetLocalPos ();
	UIRECT rcNewPos = rcOldPos;

	const WORD wFlag = GetAlignFlag ();

	//	sizeX
	//if ( wFlag & UI_FLAG_XSIZE )
	//{
	//	rcNewPos.left = 0.0f;
	//	rcNewPos.sizeX = X_RES;
	//}
	//else if( wFlag & UI_FLAG_XSIZE_PROPORTION )
	//{
	//	rcNewPos.sizeX = rcNewPos.sizeX * (float)X_RES/ (float)LAST_X_RES;
	//}

	////	sizeY
	//if ( wFlag & UI_FLAG_YSIZE )
	//{
	//	rcNewPos.top = 0.0f;
	//	rcNewPos.sizeY = Y_RES;
	//}
	//else if( wFlag & UI_FLAG_YSIZE_PROPORTION )
	//{
	//	rcNewPos.sizeY = rcNewPos.sizeY * (float)Y_RES / (float)LAST_Y_RES;
	//}

	////	x
	//if ( wFlag & UI_FLAG_LEFT )
	//{
	//	rcNewPos.left = 0.0f;
	//}
	//else if ( wFlag & UI_FLAG_RIGHT )
	//{
	//	rcNewPos.left = X_RES - rcNewPos.sizeX;
	//}
	//else if ( wFlag & UI_FLAG_CENTER_X )
	//{
	//	rcNewPos.left = (X_RES - rcNewPos.sizeX)/2.f;
	//}

	////	y
	//if ( wFlag & UI_FLAG_TOP )
	//{
	//	rcNewPos.top = 0.0f;
	//}
	//else if ( wFlag & UI_FLAG_BOTTOM )
	//{
	//	rcNewPos.top = Y_RES - rcNewPos.sizeY;
	//}
	//else if ( wFlag & UI_FLAG_CENTER_Y )
	//{
	//	rcNewPos.top = (Y_RES - rcNewPos.sizeY)/2.f;
	//}

	m_rcLocalPos.SetPosition( rcNewPos.left, rcNewPos.top, rcNewPos.sizeX, rcNewPos.sizeY );
	CUIControl::SetGlobalPos ( UIRECT(rcNewParentPos.left + m_rcLocalPos.left , rcNewParentPos.top + m_rcLocalPos.top ,m_rcLocalPos.sizeX , m_rcLocalPos.sizeY) );

	CheckProtectSize (); // MEMO : 사이즈를 일정 크기로 유지한다.
}

void CUIControl::SetFlowBar(const UIRECT& rcOriginPos, const UIRECT& rcOriginTexturePos )
{
	if ( m_bFlowBar )
        return; // Memo : 계속 On/Off 시 Origin 값들이 계속 변하는 것을 방지. by luxes.

	m_bFlowBar = TRUE;

	m_rcOriginPos = rcOriginPos;
	m_rcOriginTexturePos = rcOriginTexturePos;
}

void CUIControl::ResetFlowBar()
{
	m_bFlowBar = FALSE;
}

void CUIControl::FlowBarUpdate( float fPercent )
{
	if ( !IsFlowBar() )
        return;
	if ( fPercent < 0.0f || fPercent > 1.0f )
        return;

	switch ( m_iFlowBarType )
	{
	case FLOW_BAR_RIGHT:
		{
			UIRECT rcWindow = GetGlobalPos();
			rcWindow.sizeX  = m_rcOriginPos.sizeX * fPercent;

			//rcWindow.left   = rcWindow.left;
			rcWindow.right  = rcWindow.left + rcWindow.sizeX;
			//rcWindow.top    = rcWindow.top;
			//rcWindow.bottom = rcWindow.bottom;
			rcWindow.sizeY  = m_rcOriginPos.sizeY;
			SetGlobalPos(rcWindow);

			UIRECT rcTexutre = GetTexturePos();
			rcTexutre.sizeX = m_rcOriginTexturePos.sizeX * fPercent;

			if ( rcTexutre.sizeX >= m_rcOriginTexturePos.sizeX )
				rcTexutre.sizeX = m_rcOriginTexturePos.sizeX;
			else if ( rcTexutre.sizeX <= 0.0f )
				rcTexutre.sizeX = 0.0f;

			//rcTexutre.left = rcTexutre.left;
			rcTexutre.right = rcTexutre.left + rcTexutre.sizeX;
			//rcTexutre.top = rcTexutre.top;
			//rcTexutre.bottom = rcTexutre.bottom;
			rcTexutre.sizeY = m_rcOriginTexturePos.sizeY;
			SetTexturePos( rcTexutre );
		}
		break;

	case FLOW_BAR_LEFT:
		{

			UIRECT rcWindow = GetGlobalPos();
			rcWindow.sizeX = m_rcOriginPos.sizeX * fPercent;

			if ( rcWindow.sizeX >= m_rcOriginPos.sizeX )
			{
				rcWindow.sizeX = m_rcOriginPos.sizeX;
				fPercent = 1.0f;
			}
			else if ( rcWindow.sizeX <= 0.0f )
			{
				rcWindow.sizeX = 0.0f;
				fPercent = 0.0f;
			}

			rcWindow.left = rcWindow.right - rcWindow.sizeX;
			//rcWindow.right = rcWindow.right;
			//rcWindow.top = rcWindow.top;
			//rcWindow.bottom = rcWindow.bottom;
			rcWindow.sizeY = m_rcOriginPos.sizeY;
			SetGlobalPos( rcWindow );

			UIRECT rcTexutre = GetTexturePos();
			rcTexutre.sizeX = m_rcOriginTexturePos.sizeX * fPercent;

			if ( rcTexutre.sizeX >= m_rcOriginTexturePos.sizeX )
				rcTexutre.sizeX = m_rcOriginTexturePos.sizeX;
			else if ( rcTexutre.sizeX <= 0.0f )
				rcTexutre.sizeX = 0.0f;

			rcTexutre.left = rcTexutre.right - rcTexutre.sizeX;
			//rcTexutre.right = rcTexutre.right;
			//rcTexutre.top = rcTexutre.top;
			//rcTexutre.bottom = rcTexutre.bottom;
			rcTexutre.sizeY = m_rcOriginTexturePos.sizeY;
			SetTexturePos( rcTexutre );
		}
		break;

	case FLOW_BAR_UP:
		{
			UIRECT rcWindow = GetGlobalPos();

			rcWindow.sizeY = static_cast<float>( static_cast<int>( m_rcOriginPos.sizeY * fPercent ) );
			//rcWindow.left = rcWindow.left;
			//rcWindow.right = rcWindow.right;
			rcWindow.top = rcWindow.bottom - rcWindow.sizeY;
			//rcWindow.bottom = rcWindow.bottom;
			rcWindow.sizeX = m_rcOriginPos.sizeX;
			SetGlobalPos( rcWindow );

			UIRECT rcTexutre = GetTexturePos();
			rcTexutre.sizeY = m_rcOriginTexturePos.sizeY * fPercent;

			if ( rcTexutre.sizeY >= m_rcOriginTexturePos.sizeY )
				rcTexutre.sizeY = m_rcOriginTexturePos.sizeY;
			else if ( rcTexutre.sizeY <= 0.0f )
				rcTexutre.sizeY = 0.0f;

			//rcTexutre.left = rcTexutre.left;
			//rcTexutre.right = rcTexutre.right;
			rcTexutre.top = rcTexutre.bottom - rcTexutre.sizeY;
			//rcTexutre.bottom = rcTexutre.bottom;
			rcTexutre.sizeX = m_rcOriginTexturePos.sizeX;
			SetTexturePos( rcTexutre );
		}
		break;

	case FLOW_BAR_DOWN:
		{
			UIRECT rcWindow = GetGlobalPos();
			rcWindow.sizeY = m_rcOriginPos.sizeY * fPercent;

			//rcWindow.left = rcWindow.left;
			//rcWindow.right = rcWindow.right;
			//rcWindow.top = rcWindow.top;
			rcWindow.bottom = rcWindow.top + rcWindow.sizeY;
			rcWindow.sizeX = m_rcOriginPos.sizeX;
			SetGlobalPos( rcWindow );

			UIRECT rcTexutre = GetTexturePos();
			rcTexutre.sizeY = m_rcOriginTexturePos.sizeY * fPercent;

			if ( rcTexutre.sizeY >= m_rcOriginTexturePos.sizeY )
				rcTexutre.sizeY = m_rcOriginTexturePos.sizeY;
			else if ( rcTexutre.sizeY <= 0.0f )
				rcTexutre.sizeY = 0.0f;

			//rcTexutre.left = rcTexutre.left;
			//rcTexutre.right = rcTexutre.right;
			//rcTexutre.top = rcTexutre.top;
			rcTexutre.bottom = rcTexutre.top + rcTexutre.sizeY;
			rcTexutre.sizeX = m_rcOriginTexturePos.sizeX;
			SetTexturePos( rcTexutre );
		}
		break;
	}
		
}
