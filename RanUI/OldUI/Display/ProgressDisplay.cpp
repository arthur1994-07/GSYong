#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../InnerInterface.h"

#include "./ProgressDisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CProgressDisplay::CProgressDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient  ( pGaeaClient )
    , m_pInterface   ( pInterface )
    , m_strPosition  ( "PROGRESS_DISPLAY" )
    , m_pCurInfo     ( NULL )
{
}

CProgressDisplay::~CProgressDisplay ()
{
}

HRESULT CProgressDisplay::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
    HRESULT hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );

    if ( m_pCurInfo )
    {
        ResizeControl( m_pCurInfo->emType );
    }

    return hr;
}

void CProgressDisplay::CreateSubControl()
{
    std::string strBaseControl ( GetControlNameEx() );

    CD3DFontPar* pFont        = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 16, _DEFAULT_FONT_FLAG );
    CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 16, D3DFONT_SHADOW | D3DFONT_ASCII );

    // Attendance, Activity
    {
        m_SchoolRecord.pBackground = new CBasicLineBoxSmart(m_pEngineDevice);
        m_SchoolRecord.pBackground->CreateSub ( this, "PROGRESS_DISPLAY_BACKGROUND", UI_FLAG_DEFAULT | UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_SchoolRecord.pBackground->CreateSubControl( "PROGRESS_DISPLAY_BACKGROUND_TEXINFO" );
        m_SchoolRecord.pBackground->SetVisibleSingle( FALSE );
        RegisterControl( m_SchoolRecord.pBackground );

        m_SchoolRecord.pSchoolBack = new CUIControl(m_pEngineDevice);
        m_SchoolRecord.pSchoolBack->CreateSub ( this, "PROGRESS_DISPLAY_SCHOOL_BACK", UI_FLAG_CENTER_X );
        m_SchoolRecord.pSchoolBack->SetVisibleSingle( FALSE );
        RegisterControl( m_SchoolRecord.pSchoolBack );

        m_SchoolRecord.pSchoolImage = new CSwapImage(m_pEngineDevice);
        m_SchoolRecord.pSchoolImage->CreateSub ( this, "PROGRESS_DISPLAY_SCHOOL_IMAGE", UI_FLAG_CENTER_X );
        RegisterControl( m_SchoolRecord.pSchoolImage );

        m_SchoolRecord.pTextBox = new CBasicTextBox(m_pEngineDevice);
        m_SchoolRecord.pTextBox->CreateSub( this, "PROGRESS_DISPLAY_TEXTBOX", UI_FLAG_DEFAULT );
        m_SchoolRecord.pTextBox->SetFont( pFont );
        m_SchoolRecord.pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( m_SchoolRecord.pTextBox );

        m_SchoolRecord.pTextBox_Shadow = new CBasicTextBox(m_pEngineDevice);
        m_SchoolRecord.pTextBox_Shadow->CreateSub( this, "PROGRESS_DISPLAY_TEXTBOX", UI_FLAG_DEFAULT );
        m_SchoolRecord.pTextBox_Shadow->SetFont( pFont_Shadow );
        m_SchoolRecord.pTextBox_Shadow->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( m_SchoolRecord.pTextBox_Shadow );
    }

    // CTF
    {
        m_CTF.pBack = new CSwapImage(m_pEngineDevice);
        m_CTF.pBack->CreateSub ( this, "PROGRESS_DISPLAY_SCHOOL_BACK", UI_FLAG_DEFAULT );
        m_CTF.pBack->SetVisibleSingle( FALSE );
        RegisterControl( m_CTF.pBack );

        m_CTF.pSchool = new CSwapImage(m_pEngineDevice);
        m_CTF.pSchool->CreateSub ( this, "PROGRESS_DISPLAY_CTF_SCHOOL", UI_FLAG_DEFAULT );
        m_CTF.pSchool->SetVisibleSingle( FALSE );
        RegisterControl( m_CTF.pSchool );

        m_CTF.pText = new CSwapImage(m_pEngineDevice);
        m_CTF.pText->CreateSub ( this, "PROGRESS_DISPLAY_CTF_TEXT", UI_FLAG_DEFAULT );
        m_CTF.pText->SetVisibleSingle( FALSE );
        RegisterControl( m_CTF.pText );

        m_CTF.pTextBox_Shadow = new CBasicTextBox(m_pEngineDevice);
        m_CTF.pTextBox_Shadow->CreateSub( this, "PROGRESS_DISPLAY_TEXTBOX", UI_FLAG_DEFAULT );
        m_CTF.pTextBox_Shadow->SetFont( pFont_Shadow );
        m_CTF.pTextBox_Shadow->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( m_CTF.pTextBox_Shadow );
    }

	// PUBLIC_UI
	{
		m_PUBLIC_UI.pBack = new CSwapImage(m_pEngineDevice);
		m_PUBLIC_UI.pBack->CreateSub ( this, "PROGRESS_DISPLAY_SCHOOL_BACK", UI_FLAG_DEFAULT );
		m_PUBLIC_UI.pBack->SetVisibleSingle( FALSE );
		RegisterControl( m_PUBLIC_UI.pBack );

		m_PUBLIC_UI.pSchool = new CSwapImage(m_pEngineDevice);
		m_PUBLIC_UI.pSchool->CreateSub ( this, "PROGRESS_DISPLAY_CTF_SCHOOL", UI_FLAG_DEFAULT );
		m_PUBLIC_UI.pSchool->SetVisibleSingle( FALSE );
		RegisterControl( m_PUBLIC_UI.pSchool );

		m_PUBLIC_UI.pText = new CSwapImage(m_pEngineDevice);
		m_PUBLIC_UI.pText->CreateSub ( this, "PROGRESS_DISPLAY_CTF_TEXT", UI_FLAG_DEFAULT );
		m_PUBLIC_UI.pText->SetVisibleSingle( FALSE );
		RegisterControl( m_PUBLIC_UI.pText );

		m_PUBLIC_UI.pTextBox_Shadow = new CBasicTextBox(m_pEngineDevice);
		m_PUBLIC_UI.pTextBox_Shadow->CreateSub( this, "PROGRESS_DISPLAY_TEXTBOX", UI_FLAG_DEFAULT );
		m_PUBLIC_UI.pTextBox_Shadow->SetFont( pFont_Shadow );
		m_PUBLIC_UI.pTextBox_Shadow->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_PUBLIC_UI.pTextBox_Shadow );
	}
}

void CProgressDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !m_pCurInfo )
    {
        return;
    }

    m_fAniTime += fElapsedTime;

    switch ( m_pCurInfo->emType )
    {
    case UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_COMPLETE   : Update_Activity_Complete   ( fElapsedTime ); break;
    case UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_PROGRESS   : Update_Activity_Progress   ( fElapsedTime ); break;
    case UI::EMPROGRESSDISPLAY_TYPE_ATTENDANCE_COMPLETE : Update_Attendance_Complete ( fElapsedTime ); break;

    case UI::EMPROGRESSDISPLAY_TYPE_CTF_START: 
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_STOP:
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_READY:
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_SM:
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_HA:
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_BH:
        Update_CTF( fElapsedTime ); 
        break;
	case UI::EMPROGRESSDISPLAY_TYPE_PUBLIC:
		Update_Public_UI( fElapsedTime );
		break;
	case UI::EMPROGRESSDISPLAY_TYPE_RN_CDM_START:
	case UI::EMPROGRESSDISPLAY_TYPE_RN_CDM_END:
		update_CDM( fElapsedTime );
		break;
	case UI::EMPROGRESSDISPLAY_TYPE_LOTTO_START:
	case UI::EMPROGRESSDISPLAY_TYPE_LOTTO_END:
		Update_CTF( fElapsedTime );
		break;
    };

    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CProgressDisplay::SetVisibleSingle( BOOL bVisible )
{
    //Blank
}

void CProgressDisplay::Show ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText )
{
    INFOCAPS sInfo(emType, strText);

    if ( !m_InfoQueue.empty() )
    {
        std::list<INFOCAPS>::iterator finditer = std::find(m_InfoQueue.begin(), m_InfoQueue.end(), sInfo);

        if ( finditer != m_InfoQueue.end() )
        {
            (*finditer) = sInfo;
            return;
        }
    }

    m_InfoQueue.push_back( sInfo );

	if ( m_pCurInfo )
	{
		std::list<INFOCAPS>::iterator iterBegin = m_InfoQueue.begin();
		std::list<INFOCAPS>::iterator iterEnd   = m_InfoQueue.end();

		if ( 1 < m_InfoQueue.size() )
		{
			std::inplace_merge ( ++iterBegin, --iterEnd, iterEnd );
		}
	}
	else
	{
		m_InfoQueue.sort();
	}

    if ( !m_InfoQueue.size() )
    {
        GASSERT( !"CProgressDisplay::Show() - m_InfoQueue.size() is null" );
        return;
    }
}

void CProgressDisplay::ShowPublic( const std::string& strText )
{
	Show( UI::EMPROGRESSDISPLAY_TYPE_PUBLIC, strText );
}

void CProgressDisplay::Begin ()
{
    if ( m_InfoQueue.empty() )
    {
        GASSERT( !"CProgressDisplay::Begin() - m_InfoQueue is empty" );
        return;
    }

    m_pCurInfo = &m_InfoQueue.front();

    if ( !m_pCurInfo )
    {
        GASSERT( !"CProgressDisplay::Begin() - m_pCurInfo is null" );
        return;
    }

    SetDiffuse( D3DCOLOR_ARGB(255,255,255,255) );
    m_fAniTime = 0.0f;

    m_SchoolRecord.SetVisibleSingle(FALSE);
    m_CTF.SetVisibleSingle(FALSE);

    switch ( m_pCurInfo->emType )
    {
    case UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_COMPLETE: Begin_Activity_Complete   (); break;
    case UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_PROGRESS: Begin_Activity_Progress   (); break;
    case UI::EMPROGRESSDISPLAY_TYPE_ATTENDANCE_COMPLETE: Begin_Attendance_Complete (); break;
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_START: Begin_CTF_Start(); break;
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_STOP: Begin_CTF_Stop(); break;
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_READY: Begin_CTF_Ready(); break;
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_SM: Begin_CTF_CapturedBySM(); break;
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_HA: Begin_CTF_CapturedByHA(); break;
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_BH: Begin_CTF_CapturedByBH(); break;
	case UI::EMPROGRESSDISPLAY_TYPE_PUBLIC:
		Begin_Public_UI_Ready();
		break;
	case UI::EMPROGRESSDISPLAY_TYPE_RN_CDM_START:
		Begin_RN_CDM_UI_Start();
		break;
	case UI::EMPROGRESSDISPLAY_TYPE_RN_CDM_END:
		Begin_RN_CDM_UI_End();
		break;
	case UI::EMPROGRESSDISPLAY_TYPE_LOTTO_START:
		Begin_CTF_Ready();
		break;
	case UI::EMPROGRESSDISPLAY_TYPE_LOTTO_END:
		Begin_CTF_Ready();
		break;
    };

    m_pCurInfo->IsBegin = true;

    ResizeControl ( m_pCurInfo->emType );

    CUIControl::SetVisibleSingle( TRUE );
}


void CProgressDisplay::Hide ()
{
    if ( !m_InfoQueue.empty() )
    {
        m_InfoQueue.pop_front();
    }

    m_pCurInfo = NULL;
    CUIControl::SetVisibleSingle( FALSE );
}

void CProgressDisplay::Update ()
{
	//! 현재 Info가 있으나 보이지 않는 상태라면
	if ( m_pCurInfo && !IsVisible() )
	{
		CUIControl::SetVisibleSingle(TRUE);
	}

    //! 현재 Info가 없거나 Info가 시작 상태가 아니라면
    if ( !m_pCurInfo || !m_pCurInfo->IsBegin )
    {
        if ( !m_InfoQueue.empty() )
        {
            Begin();
        }

        return;
    }
}

void CProgressDisplay::ResizeControl ( const UI::EMPROGRESSDISPLAY_TYPE emType )
{
    switch ( emType )
    {
    case UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_COMPLETE: ResizeControl_SchoolRecord(); break;
    case UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_PROGRESS: ResizeControl_SchoolRecord(); break;
    case UI::EMPROGRESSDISPLAY_TYPE_ATTENDANCE_COMPLETE: ResizeControl_SchoolRecord(); break;

    case UI::EMPROGRESSDISPLAY_TYPE_CTF_START: 
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_STOP:
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_SM:
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_HA:
    case UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_BH:
        ResizeControl_CTF(); 
        break;

    case UI::EMPROGRESSDISPLAY_TYPE_CTF_READY:
        ResizeControl_CTF_Ready();
	case UI::EMPROGRESSDISPLAY_TYPE_PUBLIC:
		ResizeControl_Public_UI_Ready();
        break;

	case UI::EMPROGRESSDISPLAY_TYPE_RN_CDM_START: 
	case UI::EMPROGRESSDISPLAY_TYPE_RN_CDM_END:
		ResizeControl_CDM(); 
		break;

	case UI::EMPROGRESSDISPLAY_TYPE_LOTTO_START:
	case UI::EMPROGRESSDISPLAY_TYPE_LOTTO_END:
		ResizeControl_CTF_Ready();
		break;
    };
}

void CProgressDisplay::ResizeControl_SchoolRecord ()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );

    // Base의 사이즈를 구함
    std::string strBaseControl ( m_strPosition );

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, strBaseControl.c_str() );
    UIRECT rcBase = BaseControl.GetLocalPos();

    // 텍스트의 사이즈를 구함
    SIZE strSize;

    if ( !m_pCurInfo->strText.empty() )
    {
        CD3DFontPar* pFont = m_SchoolRecord.pTextBox->GetFont();
        pFont->GetTextExtent ( m_pCurInfo->strText.c_str(), strSize );
    }
    else
    {
        strSize.cx = 1;
        strSize.cy = 1;
    }

    // Gap 사이즈를 구함
    CUIControl GapControl(m_pEngineDevice); 
    GapControl.Create ( 1, "PROGRESS_DISPLAY_BACKGROUND_GAP" );
    const UIRECT& rcGapLocal = GapControl.GetLocalPos();

    float fTextSizeWidth  = (float)strSize.cx + rcGapLocal.sizeX*2.0f;
    float fTextSizeHeight = (float)strSize.cy + rcGapLocal.sizeY*2.0f;

    // Background의 위치와 사이즈를 변경
    CUIControl BackgroundControl(m_pEngineDevice); 
    BackgroundControl.Create ( 1, "PROGRESS_DISPLAY_BACKGROUND" );
    const UIRECT& rcBackgroundLocal = BackgroundControl.GetLocalPos();

    UIRECT rcBackground(rcBackgroundLocal.left, rcBackgroundLocal.top, fTextSizeWidth, fTextSizeHeight);
    m_SchoolRecord.pBackground->AlignSubControl( m_SchoolRecord.pBackground->GetLocalPos(), rcBackground );
    m_SchoolRecord.pBackground->SetLocalPos( rcBackground );

    // TextBox의 위치와 사이즈를 변경
    m_SchoolRecord.pTextBox->SetLocalPos ( rcBackground );
    m_SchoolRecord.pTextBox_Shadow->SetLocalPos( rcBackground );

    // 부모의 사이즈를 변경
    {
        UIRECT rcParentLocal( rcBase );

        if ( rcBase.left + rcBase.sizeX < rcBackgroundLocal.left + fTextSizeWidth )
        {
            rcParentLocal.sizeX = (rcBackgroundLocal.left - rcBase.left) + fTextSizeWidth;
            rcParentLocal.right = rcParentLocal.left + rcParentLocal.sizeX;
        }

        if ( rcBase.top + rcBase.sizeY < rcBackgroundLocal.top + fTextSizeHeight )
        {
            rcParentLocal.sizeY  = (rcBackgroundLocal.top - rcBase.top) + fTextSizeHeight;
            rcParentLocal.bottom = rcParentLocal.top + rcParentLocal.sizeY;
        }

        SetLocalPos ( rcParentLocal );
        AlignSubControl ( rcParentLocal, rcParentLocal );

        UIRECT rcNewBase ( rcBase );
        rcNewBase.sizeX  = rcParentLocal.sizeX;
        rcNewBase.sizeY  = rcParentLocal.sizeY;

        // 800 x 600 을 기준으로한 정렬
        rcNewBase = AlignMainControl ( rcNewBase );
        rcNewBase.right  = rcNewBase.left + rcParentLocal.sizeX;
        rcNewBase.bottom = rcNewBase.top  + rcParentLocal.sizeY;
        SetGlobalPos ( rcNewBase );
    }
}

void CProgressDisplay::ResizeControl_CTF ()
{
    SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );

    // Base의 사이즈를 구함
    std::string strBaseControl ( m_strPosition );

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, strBaseControl.c_str() );
    UIRECT rcBase = BaseControl.GetLocalPos();

    // 부모의 사이즈를 변경
    {
        SetLocalPos ( rcBase );

        // 800 x 600 을 기준으로한 정렬
        UIRECT rcNewBase = AlignMainControl ( rcBase );
        rcNewBase.right  = rcNewBase.left + rcNewBase.sizeX;
        rcNewBase.bottom = rcNewBase.top  + rcNewBase.sizeY;
        SetGlobalPos ( rcNewBase );
    }
}

void CProgressDisplay::ResizeControl_CTF_Ready ()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_TOP );

    // Base의 사이즈를 구함
    std::string strBaseControl ( m_strPosition );

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, strBaseControl.c_str() );
    UIRECT rcBase = BaseControl.GetLocalPos();

    // 텍스트의 사이즈를 구함
    SIZE strSize;

    if ( !m_pCurInfo->strText.empty() )
    {
        CD3DFontPar* pFont = m_CTF.pTextBox_Shadow->GetFont();
        pFont->GetTextExtent ( m_pCurInfo->strText.c_str(), strSize );
    }
    else
    {
        strSize.cx = 1;
        strSize.cy = 1;
    }

    // Gap 사이즈를 구함
    CUIControl GapControl(m_pEngineDevice); 
    GapControl.Create ( 1, "PROGRESS_DISPLAY_BACKGROUND_GAP" );
    const UIRECT& rcGapLocal = GapControl.GetLocalPos();

    float fTextSizeWidth  = (float)strSize.cx + rcGapLocal.sizeX*2.0f;
    float fTextSizeHeight = (float)strSize.cy + rcGapLocal.sizeY*2.0f;

    // TextBox의 위치와 사이즈를 변경
    CUIControl BackgroundControl(m_pEngineDevice); 
    BackgroundControl.Create ( 1, "PROGRESS_DISPLAY_BACKGROUND_CTF_READY" );
    const UIRECT& rcBackgroundLocal = BackgroundControl.GetLocalPos();

    UIRECT rcBackground(rcBackgroundLocal.left, rcBackgroundLocal.top, fTextSizeWidth, fTextSizeHeight);
    m_CTF.pTextBox_Shadow->SetLocalPos( rcBackground );

    // 부모의 사이즈를 변경
    {
        UIRECT rcParentLocal( rcBase );

        if ( rcBase.left + rcBase.sizeX < rcBackgroundLocal.left + fTextSizeWidth )
        {
            rcParentLocal.sizeX = (rcBackgroundLocal.left - rcBase.left) + fTextSizeWidth;
            rcParentLocal.right = rcParentLocal.left + rcParentLocal.sizeX;
        }

        if ( rcBase.top + rcBase.sizeY < rcBackgroundLocal.top + fTextSizeHeight )
        {
            rcParentLocal.sizeY  = (rcBackgroundLocal.top - rcBase.top) + fTextSizeHeight;
            rcParentLocal.bottom = rcParentLocal.top + rcParentLocal.sizeY;
        }

        SetLocalPos ( rcParentLocal );
        AlignSubControl ( rcParentLocal, rcParentLocal );

        UIRECT rcNewBase ( rcBase );
        rcNewBase.sizeX  = rcParentLocal.sizeX;
        rcNewBase.sizeY  = rcParentLocal.sizeY;

        // 800 x 600 을 기준으로한 정렬
        rcNewBase = AlignMainControl ( rcNewBase );
        rcNewBase.right  = rcNewBase.left + rcParentLocal.sizeX;
        rcNewBase.bottom = rcNewBase.top  + rcParentLocal.sizeY;
        SetGlobalPos ( rcNewBase );
    }
}

void CProgressDisplay::ResizeControl_Public_UI_Ready ()
{
	if ( !m_pCurInfo )
	{
		return;
	}

	SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_TOP );

	// Base의 사이즈를 구함
	std::string strBaseControl ( m_strPosition );

	CUIControl BaseControl(m_pEngineDevice); 
	BaseControl.Create ( 1, strBaseControl.c_str() );
	UIRECT rcBase = BaseControl.GetLocalPos();

	// 텍스트의 사이즈를 구함
	SIZE strSize;

	if ( !m_pCurInfo->strText.empty() )
	{
		CD3DFontPar* pFont = m_PUBLIC_UI.pTextBox_Shadow->GetFont();
		pFont->GetTextExtent ( m_pCurInfo->strText.c_str(), strSize );
	}
	else
	{
		strSize.cx = 1;
		strSize.cy = 1;
	}

	// Gap 사이즈를 구함
	CUIControl GapControl(m_pEngineDevice); 
	GapControl.Create ( 1, "PROGRESS_DISPLAY_BACKGROUND_GAP" );
	const UIRECT& rcGapLocal = GapControl.GetLocalPos();

	float fTextSizeWidth  = (float)strSize.cx + rcGapLocal.sizeX*2.0f;
	float fTextSizeHeight = (float)strSize.cy + rcGapLocal.sizeY*2.0f;

	// TextBox의 위치와 사이즈를 변경
	CUIControl BackgroundControl(m_pEngineDevice); 
	BackgroundControl.Create ( 1, "PROGRESS_DISPLAY_BACKGROUND_CTF_READY" );
	const UIRECT& rcBackgroundLocal = BackgroundControl.GetLocalPos();

	UIRECT rcBackground(rcBackgroundLocal.left, rcBackgroundLocal.top, fTextSizeWidth, fTextSizeHeight);
	m_PUBLIC_UI.pTextBox_Shadow->SetLocalPos( rcBackground );

	// 부모의 사이즈를 변경
	{
		UIRECT rcParentLocal( rcBase );

		if ( rcBase.left + rcBase.sizeX < rcBackgroundLocal.left + fTextSizeWidth )
		{
			rcParentLocal.sizeX = (rcBackgroundLocal.left - rcBase.left) + fTextSizeWidth;
			rcParentLocal.right = rcParentLocal.left + rcParentLocal.sizeX;
		}

		if ( rcBase.top + rcBase.sizeY < rcBackgroundLocal.top + fTextSizeHeight )
		{
			rcParentLocal.sizeY  = (rcBackgroundLocal.top - rcBase.top) + fTextSizeHeight;
			rcParentLocal.bottom = rcParentLocal.top + rcParentLocal.sizeY;
		}

		SetLocalPos ( rcParentLocal );
		AlignSubControl ( rcParentLocal, rcParentLocal );

		UIRECT rcNewBase ( rcBase );
		rcNewBase.sizeX  = rcParentLocal.sizeX;
		rcNewBase.sizeY  = rcParentLocal.sizeY;

		// 800 x 600 을 기준으로한 정렬
		rcNewBase = AlignMainControl ( rcNewBase );
		rcNewBase.right  = rcNewBase.left + rcParentLocal.sizeX;
		rcNewBase.bottom = rcNewBase.top  + rcParentLocal.sizeY;
		SetGlobalPos ( rcNewBase );
	}
}

void CProgressDisplay::SetPositionAlign ( const std::string& strControl )
{
    m_strPosition = strControl;

    const UIRECT& rcOldGlobalPos = GetGlobalPos();

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, strControl.c_str() );
    const UIRECT& rcBaseLocal = BaseControl.GetLocalPos();

    UIRECT rcNewBase ( UIRECT(rcBaseLocal.left, rcBaseLocal.right, rcOldGlobalPos.sizeX, rcOldGlobalPos.sizeY ) );

    // 800 x 600 을 기준으로한 정렬
    rcNewBase = AlignMainControl ( rcNewBase );
    rcNewBase.right  = rcNewBase.left + rcNewBase.sizeX;
    rcNewBase.bottom = rcNewBase.top  + rcNewBase.sizeY;
    SetGlobalPos ( rcNewBase );
}

void CProgressDisplay::SCHOOLRECORD::SetVisibleSingle( BOOL bVisible )
{
    if ( pBackground ) pBackground->SetVisibleSingle( bVisible );
    if ( pSchoolBack ) pSchoolBack->SetVisibleSingle( bVisible );
    if ( pSchoolImage ) pSchoolImage->SetVisibleSingle( bVisible );
    if ( pTextBox ) pTextBox->SetVisibleSingle( bVisible );
    if ( pTextBox_Shadow ) pTextBox_Shadow->SetVisibleSingle( bVisible );
}

void CProgressDisplay::CTF::SetVisibleSingle( BOOL bVisible )
{
    if ( pBack ) pBack->SetVisibleSingle( bVisible );
    if ( pSchool ) pSchool->SetVisibleSingle( bVisible );
    if ( pText ) pText->SetVisibleSingle( bVisible );
    if ( pTextBox_Shadow ) pTextBox_Shadow->SetVisibleSingle( bVisible );
}

//----------------------------------------------------------------------------------------------------//

MyProgressDisplay::MyProgressDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CProgressDisplay( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyProgressDisplay::CreateUIWindowAndRegisterOwnership()
{
    CProgressDisplay::Create( PROGRESS_DISPLAY, "PROGRESS_DISPLAY" );
    CProgressDisplay::CreateSubControl();
    CProgressDisplay::m_pInterface->UiRegisterControl( this );
    //CProgressDisplay::m_pInterface->UiShowGroupTop( PROGRESS_DISPLAY );
	CProgressDisplay::m_pInterface->UiShowGroupUpdate( PROGRESS_DISPLAY );

    CProgressDisplay::SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );
    CProgressDisplay::SetVisibleSingle( FALSE );
}

void MyProgressDisplay::Show ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText )
{
	CProgressDisplay::Show( emType, strText );
}

void MyProgressDisplay::ShowPublic( const std::string& strText )
{
	CProgressDisplay::ShowPublic(strText );
}

void MyProgressDisplay::Update ()
{
    CProgressDisplay::Update();
}

void MyProgressDisplay::ctf_hide_display()
{
	// TODO : #RM 695
	CProgressDisplay::ctf_hide_display();
}