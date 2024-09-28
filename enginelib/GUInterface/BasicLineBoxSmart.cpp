#include "pch.h"


#include "./BasicLineBoxSmart.h"
#include "./SwapImage.h"
#include "./InterfaceCfg.h"

#include "../DxTools/RENDERPARAM.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicLineBoxSmart::CBasicLineBoxSmart (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pImage_Background( NULL )
    , m_pLineBody( NULL )
    , m_pLineTop( NULL )
    , m_pLineRightTop( NULL )
    , m_pLineRight( NULL )
    , m_pLineRightBootm( NULL )
    , m_pLineBottom( NULL )
    , m_pLineLeftBottom( NULL )
    , m_pLineLeft( NULL )
    , m_pLineLeftTop( NULL )

{
}

CBasicLineBoxSmart::~CBasicLineBoxSmart ()
{
}


void CBasicLineBoxSmart::CreateSubControl ( const char* szTextureInfoControl )
{
    if( CInterfaceCfg::GetInstance().ArrangeInfo( szTextureInfoControl, m_sControl ) )
    {
        CreateControl();
        return;
    }

    std::string strError = sc::string::format(" \"%s\" 컨트롤 Id를 찾지 못하였습니다.", szTextureInfoControl );
    GASSERT_MSG( 0, strError );
    return;
}

void CBasicLineBoxSmart::CreateControl()
{
    const char* strTextureName = m_sControl.m_strFileName.c_str();

    CUIControl* pLine = NULL;
    // Body
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_BODY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineBody = pLine;

    // Background
    CSwapImage* pSwapImage = new CSwapImage(m_pEngineDevice);
    pSwapImage->CreateSub		( this, "BASIC_LINE_BOX_SMART_BACKGROUND", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    RegisterControl ( pSwapImage );
    m_pImage_Background = pSwapImage;

    // Left
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_LEFT", UI_FLAG_YSIZE );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineLeft = pLine;

    // Top
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_TOP", UI_FLAG_XSIZE );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineTop = pLine;

    // Right
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_RIGHT", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineRight = pLine;

    // Bottom
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_BOTTOM", UI_FLAG_BOTTOM | UI_FLAG_XSIZE );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineBottom = pLine;

    // LeftTop
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_LEFTTOP", UI_FLAG_LEFT | UI_FLAG_TOP );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineLeftTop = pLine;

    // RightTop
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_RIGHTTOP", UI_FLAG_RIGHT | UI_FLAG_TOP );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineRightTop = pLine;

    // LeftBottom
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_LEFTBOTTOM", UI_FLAG_LEFT | UI_FLAG_BOTTOM );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineLeftBottom = pLine;

    // RightBottom
    pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub      ( this, "BASIC_LINE_BOX_SMART_RIGHTBOTTOM", UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
    pLine->SetUseRender   ( TRUE );
    pLine->SetTextureName ( strTextureName );
    RegisterControl ( pLine );
    m_pLineRightBootm = pLine;

    Refresh();
}

void CBasicLineBoxSmart::SetBackgroundImage ( const char* szControlKeyword )
{
	if ( szControlKeyword == NULL )
	{
		SetBackgroundView ( FALSE );
	}
	else
	{
		m_pImage_Background->SetImage( szControlKeyword );
		SetBackgroundView ( TRUE );
	}
}

void CBasicLineBoxSmart::SetBackgroundImageEx ( const char* szControlKeyword, const UINT32 nTexPosX, const UINT32 nTexPosY, const UINT32 nTexWidth, const UINT32 nTexHeight )
{
	if ( szControlKeyword == NULL )
	{
		SetBackgroundView ( FALSE );
	}
	else
	{
		m_pImage_Background->SetImageEx( szControlKeyword, nTexPosX, nTexPosY, nTexWidth, nTexHeight );
		SetBackgroundView ( TRUE );
	}
}

void CBasicLineBoxSmart::SetBackgroundView ( const BOOL bEnable )
{
	m_pImage_Background->SetUseRender( bEnable );
}


void CBasicLineBoxSmart::SetLocalPos( const UIRECT& Pos )
{
	CUIGroup::SetLocalPos(Pos);
	Refresh();
}

void CBasicLineBoxSmart::SetLocalPos( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetLocalPos(vPos);
	Refresh();
}

void CBasicLineBoxSmart::SetGlobalPos( const UIRECT& Pos )
{
    CUIGroup::SetGlobalPos(Pos);
    Refresh();
}

void CBasicLineBoxSmart::SetGlobalPos( const D3DXVECTOR2& vPos )
{
    CUIGroup::SetGlobalPos(vPos);
    Refresh();
}

void CBasicLineBoxSmart::SetTexturePos( const UIRECT& rcTexPos )
{
    CUIGroup::SetTexturePos(rcTexPos);
    Refresh();
}

void CBasicLineBoxSmart::SetTexturePos(int index, const D3DXVECTOR2& vPos)
{
    CUIGroup::SetTexturePos(index, vPos);
    Refresh();
}

void CBasicLineBoxSmart::Refresh()
{
    
    SetBorder( m_sControl.m_vBorder );
    float TextureSizeX = m_sControl.m_vSize.x;
    float TextureSizeY = m_sControl.m_vSize.y;

    UITEXSIZERECT rcLINETEXGLOBAL( m_sControl.m_rcTexturePos, TextureSizeX, TextureSizeY );

    float fLeftSize   = m_sControl.m_vBorder.x;
    float fTopSize	  = m_sControl.m_vBorder.y;
    float fRightSize  = m_sControl.m_vBorder.z;
    float fBottomSize = m_sControl.m_vBorder.w;

    UIRECT rcParent	= GetGlobalPos();

    UITEXSIZERECT rcLINEPOS;
    UITEXSIZERECT rcLINETEX;


    // BackGroung 
    if( m_pImage_Background )
        m_pImage_Background->SetControlSize	( (int)rcParent.sizeX, (int)rcParent.sizeY );

    // Body
    rcLINEPOS.POSX  = fLeftSize;
    rcLINEPOS.POSY  = fTopSize;
    rcLINEPOS.SIZEX = rcParent.sizeX - fLeftSize - fRightSize;
    rcLINEPOS.SIZEY = rcParent.sizeY - fTopSize  - fBottomSize;


    float fSub = 0.5f;
    if( fLeftSize + fTopSize + fRightSize + fBottomSize <= 0 )
        fSub = 0;

    rcLINETEX		= rcLINEPOS;
    rcLINETEX.SIZEX = rcLINETEXGLOBAL.SIZEX - fLeftSize - fRightSize - fSub;
    rcLINETEX.SIZEY = rcLINETEXGLOBAL.SIZEY - fTopSize  - fBottomSize - fSub;

    LineControlSet ( m_pLineBody, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

    //left
    rcLINEPOS.POSX  = 0.0f;
    rcLINEPOS.POSY  = fTopSize;
    rcLINEPOS.SIZEX = fLeftSize;
    rcLINEPOS.SIZEY = rcParent.sizeY - fTopSize - fBottomSize;

    rcLINETEX		= rcLINEPOS;
    rcLINETEX.SIZEY = rcLINETEXGLOBAL.SIZEY - fTopSize - fBottomSize;

    LineControlSet ( m_pLineLeft, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

    // Top
	rcLINEPOS.POSX  = fLeftSize;
	rcLINEPOS.POSY  = 0.0f;
	rcLINEPOS.SIZEX = rcParent.sizeX - fLeftSize - fRightSize;
	rcLINEPOS.SIZEY = fTopSize;

	rcLINETEX	    = rcLINEPOS;
	rcLINETEX.SIZEX = rcLINETEXGLOBAL.SIZEX - fLeftSize - fRightSize;

	LineControlSet ( m_pLineTop, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );
	
	// Right
	rcLINEPOS.POSX  = rcParent.sizeX - fRightSize;
	rcLINEPOS.POSY  = fTopSize;
	rcLINEPOS.SIZEX = fRightSize;
	rcLINEPOS.SIZEY = rcParent.sizeY - fTopSize - fBottomSize;

	rcLINETEX	    = rcLINEPOS;
	rcLINETEX.POSX  = rcLINETEXGLOBAL.SIZEX - fRightSize;
	rcLINETEX.SIZEY = rcLINETEXGLOBAL.SIZEY - fTopSize - fBottomSize;

	LineControlSet ( m_pLineRight, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

	// Bottom
	rcLINEPOS.POSX  = fLeftSize;
	rcLINEPOS.POSY  = rcParent.sizeY - fBottomSize;
	rcLINEPOS.SIZEX = rcParent.sizeX - fLeftSize - fRightSize;
	rcLINEPOS.SIZEY = fBottomSize;

	rcLINETEX	    = rcLINEPOS;
	rcLINETEX.POSY  = rcLINETEXGLOBAL.SIZEY - fBottomSize;
	rcLINETEX.SIZEX = rcLINETEXGLOBAL.SIZEX - fLeftSize - fRightSize;

	LineControlSet ( m_pLineBottom, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

	// LeftTop
	rcLINEPOS.POSX  = 0.0f;
	rcLINEPOS.POSY  = 0.0f;
	rcLINEPOS.SIZEX = fLeftSize;
	rcLINEPOS.SIZEY = fTopSize;

	rcLINETEX = rcLINEPOS;

	LineControlSet ( m_pLineLeftTop, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

	// RightTop
	rcLINEPOS.POSX  = rcParent.sizeX - fRightSize;
	rcLINEPOS.POSY  = 0.0f;
	rcLINEPOS.SIZEX = fRightSize;
	rcLINEPOS.SIZEY = fTopSize;

	rcLINETEX = rcLINEPOS;
	rcLINETEX.POSX = rcLINETEXGLOBAL.SIZEX - fRightSize;

	LineControlSet ( m_pLineRightTop, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

	// LeftBottom
	rcLINEPOS.POSX  = 0.0f;
	rcLINEPOS.POSY  = rcParent.sizeY - fBottomSize;
	rcLINEPOS.SIZEX = fLeftSize;
	rcLINEPOS.SIZEY = fBottomSize;

	rcLINETEX = rcLINEPOS;
	rcLINETEX.POSY = rcLINETEXGLOBAL.SIZEY - fBottomSize;

	LineControlSet ( m_pLineLeftBottom, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

	// RightBottom
	rcLINEPOS.POSX  = rcParent.sizeX - fRightSize;
	rcLINEPOS.POSY  = rcParent.sizeY - fBottomSize;
	rcLINEPOS.SIZEX = fRightSize;
	rcLINEPOS.SIZEY = fBottomSize;

	rcLINETEX = rcLINEPOS;
	rcLINETEX.POSX = rcLINETEXGLOBAL.SIZEX - fRightSize;
	rcLINETEX.POSY = rcLINETEXGLOBAL.SIZEY - fBottomSize;

	LineControlSet ( m_pLineRightBootm, rcLINEPOS, rcLINETEX, rcLINETEXGLOBAL, TextureSizeX, TextureSizeY );

}

void CBasicLineBoxSmart::LineControlSet ( CUIControl* pControl, UITEXSIZERECT& InPos, UITEXSIZERECT& InLocalTexRect, UITEXSIZERECT& InGlobalTexRect, const float fTexSizeX, const float fTexSizeY )
{
    if( !pControl )
        return;

    float fMicroX = 0.25f/fTexSizeX;
    float fMicroY = 0.25f/fTexSizeY;

    pControl->SetLocalPos	 ( D3DXVECTOR2( InPos.POSX, InPos.POSY ) );
    pControl->SetControlSize ( (int)InPos.SIZEX, (int)InPos.SIZEY	 );

    //m_rcTexturePos = UIRECT( left/X_Size+fMicroX, top/Y_Size+fMicroY, sizeX/X_Size, sizeY/Y_Size );

    UIRECT rcTexLocal;
    UIRECT rcTexGlobal;
    UIRECT rcTexResult;

    InLocalTexRect .Get( rcTexLocal,  fTexSizeX, fTexSizeY );
    InGlobalTexRect.Get( rcTexGlobal, fTexSizeX, fTexSizeY );

    rcTexResult.left   = rcTexGlobal.left + rcTexLocal.left - fMicroX;
    rcTexResult.top    = rcTexGlobal.top  + rcTexLocal.top  - fMicroY;
    rcTexResult.sizeX  = rcTexLocal.sizeX;
    rcTexResult.sizeY  = rcTexLocal.sizeY;

    rcTexResult.right  = rcTexResult.left  + rcTexResult.sizeX;
    rcTexResult.bottom = rcTexResult.right + rcTexResult.sizeY;

    pControl->SetTexturePos ( rcTexResult );
}

