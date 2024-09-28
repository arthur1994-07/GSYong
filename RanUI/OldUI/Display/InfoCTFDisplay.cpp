#include "StdAfx.h"
#include "./InfoCTFDisplay.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CInfoCTFDisplay::CInfoCTFDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pBackground_Left(NULL)
    , m_pBackground_Body(NULL)
    , m_pBackground_Right(NULL)
    , m_pCTF_Progress(NULL)
    , m_pCTF_Time(NULL)
    , m_ppAuthInfo(NULL)
    , m_ppLine(NULL)
    , m_pd3dDevice(NULL)
    , m_nAuthInfoNums(0)
    , m_bCTFProgress(false)
{
}

CInfoCTFDisplay::~CInfoCTFDisplay ()
{
    SAFE_DELETE_ARRAY( m_ppAuthInfo );
    SAFE_DELETE_ARRAY( m_ppLine );
}

HRESULT CInfoCTFDisplay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;
    return CUIGroup::InitDeviceObjects( pd3dDevice );
}

HRESULT CInfoCTFDisplay::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;
    return CUIGroup::RestoreDeviceObjects( pd3dDevice );
}

void CInfoCTFDisplay::CreateSubControl ()
{
    {
        m_pBackground_Left = new CUIControl(m_pEngineDevice);
        m_pBackground_Left->CreateSub( this, "INFO_CTF_DISPLAY_BACKGROUND_LEFT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl(m_pBackground_Left);

        m_pBackground_Body = new CUIControl(m_pEngineDevice);
        m_pBackground_Body->CreateSub( this, "INFO_CTF_DISPLAY_BACKGROUND_BODY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl(m_pBackground_Body);

        m_pBackground_Right = new CUIControl(m_pEngineDevice);
        m_pBackground_Right->CreateSub( this, "INFO_CTF_DISPLAY_BACKGROUND_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl(m_pBackground_Right);
    }

    {
        CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        m_pCTF_Progress = new CBasicTextBox(m_pEngineDevice);
        m_pCTF_Progress->CreateSub( this, "INFO_CTF_DISPLAY_TEXT_PROGRESS", UI_FLAG_DEFAULT );
        m_pCTF_Progress->SetFont( pFont );
        m_pCTF_Progress->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        m_pCTF_Progress->SetOneLineText("");
        RegisterControl(m_pCTF_Progress);

        m_pCTF_Time = new CBasicTextBox(m_pEngineDevice);
        m_pCTF_Time->CreateSub( this, "INFO_CTF_DISPLAY_TEXT_TIME", UI_FLAG_DEFAULT );
        m_pCTF_Time->SetFont( pFont );
        m_pCTF_Time->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        m_pCTF_Time->SetOneLineText("");
        RegisterControl(m_pCTF_Time);
    }

    ResizePannel(2);
}

void CInfoCTFDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    DragMessage( CUIControl::GetMessageEx() );
    DragUpdate( x, y, m_vPos );
}

void CInfoCTFDisplay::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    //Blank
}

void CInfoCTFDisplay::Update()
{
    using namespace PVP;
    GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();

    if ( !pCTFClient )
    {
        GASSERT(!"CInfoCTFDisplay::Update() - pCTFClient is null");
        SetVisibleSingle(FALSE);
        return;
    }

    GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

    if ( !pLandMan )
    {
        SetVisibleSingle(FALSE);
        return;
    }

    SNATIVEID sMapID = pLandMan->GetBaseMapID();
    const GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = pCTFClient->GetGlobalState( sMapID );

    if ( sCTFState.IsNULL() )
    {
        SetVisibleSingle(FALSE);
        return;
    }

    SetVisibleSingle(TRUE);

    int  nAuthInfoNums    = 0;
    bool bIsAuthenticator = false;

    //! 점령전이 진행중이 아니라면
    if ( !sCTFState.IsSTART() )
    {
        //! 점령 학원만 보여주겠다
        nAuthInfoNums    = 1;
        bIsAuthenticator = false;
    }
    else
    {
        //! 인증기의 갯수를 가져온다.
        nAuthInfoNums    = (UINT)sCTFState.vecAUTHENTICATOR.size();
        bIsAuthenticator = true;
    }

    if ( m_nAuthInfoNums != nAuthInfoNums )
    {
        //! 생성 된 정보들을 제거한다
        for ( int i=0; i<(int)m_nAuthInfoNums; ++i )
        {
            GASSERT(m_ppAuthInfo[i]);
            DeleteControl( m_ppAuthInfo[i]->GetWndID(), true );
        }

        SAFE_DELETE_ARRAY(m_ppAuthInfo);

        for ( int i=0; i<(int)m_nAuthInfoNums-1; ++i )
        {
            GASSERT(m_ppLine[i]);
            DeleteControl( m_ppLine[i]->GetWndID(), true );
        }

        SAFE_DELETE_ARRAY(m_ppLine);

        if ( nAuthInfoNums == 0 )
        {
            GASSERT( !"CInfoCTFDisplay::Update() - nAuthInfoNums is zero" );
            return;
        }

        //! 갯수에 따른 사이즈 변경
        if ( nAuthInfoNums <= 2 )
        {
            ResizePannel( 2 );
        }
        else
        {
            ResizePannel( (UINT)nAuthInfoNums );
        }

        //! 인증 정보 생성
        m_ppAuthInfo = new AUTHINFO*[nAuthInfoNums];

        for ( int i=0; i<nAuthInfoNums; ++i )
        {
            std::string strLABEL;

            //! 인증기 정보
            if ( bIsAuthenticator )
            { 
               strLABEL = sCTFState.vecAUTHENTICATOR[i].GetName();
            }
            //! 점령학원 정보
            else 
            {
                strLABEL = ID2GAMEWORD("INFO_CTF_DISPLAY_TEXT", 0);
            }

            m_ppAuthInfo[i] = CreateAuthInfo( strLABEL, (UINT)i, (UINT)nAuthInfoNums );
        }

        //! 라인 생성
        if ( nAuthInfoNums > 1 )
        {
            m_ppLine = new CUIControl*[nAuthInfoNums-1]; 

            for ( int i=0; i<nAuthInfoNums-1; ++i )
            {
                m_ppLine[i] = CreateLine((UINT)i, (UINT)(nAuthInfoNums-1) );
            }
        }

        m_nAuthInfoNums = (UINT)nAuthInfoNums;

        //! 포지션 재정립용
        SetGlobalPos( GetGlobalPos() );
    }

    //! 텍스트
    if ( bIsAuthenticator )
    {
        std::string strTimeLeft = GetTimeLeft( sCTFState.tSTARTTIME + sCTFState.tPLAYTIME );

        m_pCTF_Progress->SetOneLineText( ID2GAMEWORD("INFO_CTF_DISPLAY_TEXT", 1), NS_UITEXTCOLOR::GOLD );
        m_pCTF_Time->SetOneLineText( strTimeLeft.c_str(), NS_UITEXTCOLOR::GOLD );
    }
    else
    {
        std::string strTimeNext = GetTimeLeft( sCTFState.tSTARTTIME );

        m_pCTF_Progress->SetOneLineText( ID2GAMEWORD("INFO_CTF_DISPLAY_TEXT", 2), NS_UITEXTCOLOR::GREENYELLOW );
        m_pCTF_Time->SetOneLineText( strTimeNext.c_str(), NS_UITEXTCOLOR::GREENYELLOW );
    }

    //! 점령 이미지
    if ( bIsAuthenticator )
    {
        for ( UINT i=0; i<(UINT)nAuthInfoNums; ++i )
        {
            m_ppAuthInfo[i]->SetAuthSchool( sCTFState.vecAUTHENTICATOR[i].emAuthSchool );
        }
    }
    else
    {
        m_ppAuthInfo[0]->SetAuthSchool( sCTFState.emAUTHSCHOOL );
    }
}

void CInfoCTFDisplay::ResizePannel( const UINT nMax )
{
    std::string strBase = GetControlNameEx();
    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, strBase.c_str() );
    const UIRECT& rcBaseLocal = BaseControl.GetLocalPos();

    std::string strInfo = "INFO_CTF_DISPLAY_AUTH_INFO_BASE";
    CUIControl InfoControl(m_pEngineDevice); 
    InfoControl.Create ( 1, strInfo.c_str() );
    const UIRECT& rcInfoLocal = InfoControl.GetLocalPos();

    //////////////////////////////////////////////////////////////////
    float fInfoWidth  = rcInfoLocal.left + rcInfoLocal.sizeX;

    UIRECT rcLeft  = m_pBackground_Left->GetLocalPos();
    UIRECT rcRight = m_pBackground_Right->GetLocalPos();
    UIRECT rcBody  = m_pBackground_Body->GetLocalPos();

    rcBody.sizeX = (fInfoWidth*(float)nMax) - rcLeft.sizeX - rcRight.sizeX;
    rcBody.right = rcBody.left + rcBody.sizeX;

    rcRight.left  = rcBody.right;
    rcRight.right = rcRight.left + rcRight.sizeX;

    m_pBackground_Body->SetLocalPos( rcBody );
    m_pBackground_Right->SetLocalPos( rcRight );

    //////////////////////////////////////////////////////////////////
    float fTotalWidth = rcRight.right - rcLeft.left;
    float fHalfWidth  = fTotalWidth * 0.5f;

    std::string strTextProgress = "INFO_CTF_DISPLAY_TEXT_PROGRESS";
    CUIControl TextProgressControl(m_pEngineDevice); 
    TextProgressControl.Create ( 1, strTextProgress.c_str() );
    UIRECT rcTextProgressLocal = TextProgressControl.GetLocalPos();

    std::string strTextTime = "INFO_CTF_DISPLAY_TEXT_TIME";
    CUIControl TextTimeControl(m_pEngineDevice); 
    TextTimeControl.Create ( 1, strTextTime.c_str() );
    UIRECT rcTextTime = TextTimeControl.GetLocalPos();

    float fTimeSize   = rcTextTime.right - rcTextProgressLocal.left;
    float fBeginTimeX = (fHalfWidth - fTimeSize/2.0f);

    rcTextTime.left  = (rcTextTime.left - rcTextProgressLocal.left) + fBeginTimeX;
    rcTextTime.right = rcTextTime.left + rcTextTime.sizeX;

    rcTextProgressLocal.left  = fBeginTimeX;
    rcTextProgressLocal.right = rcTextProgressLocal.left + rcTextProgressLocal.sizeX;

    m_pCTF_Progress->SetLocalPos(rcTextProgressLocal);
    m_pCTF_Time->SetLocalPos(rcTextTime);

    //////////////////////////////////////////////////////////////////
    UIRECT rcBaseGlobal = GetGlobalPos();
    rcBaseGlobal.right = rcBaseGlobal.left + fTotalWidth;
    rcBaseGlobal.sizeX = fTotalWidth;

	rcBaseGlobal = AlignMainControl( rcBaseGlobal );
    SetGlobalPos( rcBaseGlobal );
}

CInfoCTFDisplay::AUTHINFO* CInfoCTFDisplay::CreateAuthInfo( const std::string& strLABEL, const UINT nIndex, const UINT nMax )
{
    AUTHINFO* pInfo = new AUTHINFO(m_pEngineDevice);
    pInfo->CreateSub(this, "INFO_CTF_DISPLAY_AUTH_INFO_BASE" );
    pInfo->CreateSubControl( m_pd3dDevice, strLABEL );
    RegisterControl( pInfo );

    //! 위치를 옮긴다.
    const UIRECT& rcBase = GetGlobalPos();

    UIRECT rcInfoBase = pInfo->GetLocalPos();

    float fHalfWidth = rcInfoBase.sizeX/2.0f;
    rcInfoBase.left  = (2.0f*float(nIndex) - float(nMax-1)) * fHalfWidth + (rcBase.sizeX/2.0f - fHalfWidth);
    rcInfoBase.right = rcInfoBase.left + rcInfoBase.sizeX;
    pInfo->SetLocalPos( rcInfoBase );

    return pInfo;
}

CUIControl* CInfoCTFDisplay::CreateLine( const UINT nIndex, const UINT nMax )
{
    CUIControl* pLine = new CUIControl(m_pEngineDevice);
    pLine->CreateSub(this, "INFO_CTF_DISPLAY_LINE" );
    pLine->InitDeviceObjects( m_pd3dDevice );
    RegisterControl( pLine );

    //! 위치를 옮긴다.
    const UIRECT& rcBase = GetGlobalPos();

    std::string strInfo = "INFO_CTF_DISPLAY_AUTH_INFO_BASE";

    CUIControl InfoControl(m_pEngineDevice); 
    InfoControl.Create ( 1, strInfo.c_str() );
    const UIRECT& rcInfo = InfoControl.GetLocalPos();

    float fHalfInfoWidth = rcInfo.sizeX/2.0f;

    UIRECT rcLine = pLine->GetLocalPos();
    rcLine.left  = (2.0f*float(nIndex) - float(nMax-1)) * fHalfInfoWidth + (rcBase.sizeX/2.0f);
    rcLine.right = rcLine.left + rcLine.sizeX;
    pLine->SetLocalPos( rcLine );

    return pLine;
}

const std::string CInfoCTFDisplay::GetTimeLeft( __time64_t tDest )
{
    CTime tCurTime( m_pGaeaClient->GetCurrentTime() );

    //	서버 타임으로 변환( TimeZone 계산 )
    m_pGaeaClient->GetConvertServerTime( tCurTime );

    CTimeSpan sTimeLeft( tDest - tCurTime.GetTime() );

    if ( sTimeLeft < 0 )
    {
        sTimeLeft = 0;
    }

    std::string strFormat = sc::string::format("%02d : %02d : %02d", sTimeLeft.GetHours(), sTimeLeft.GetMinutes(), sTimeLeft.GetSeconds() );
    return strFormat;
}

//----------------------------------------------------------------------------------------------------//

void CInfoCTFDisplay::AUTHINFO::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& strLABEL )
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    pLABEL = new CBasicTextBox(m_pEngineDevice);
    pLABEL->InitDeviceObjects( pd3dDevice );
    pLABEL->CreateSub( this, "INFO_CTF_DISPLAY_AUTH_INFO_LABEL" );
    pLABEL->SetFont( pFont );
    pLABEL->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    pLABEL->SetOneLineText( strLABEL.c_str(), NS_UITEXTCOLOR::WHITE );
    RegisterControl( pLABEL );

    pMARK = new CSwapImage(m_pEngineDevice);
    pMARK->InitDeviceObjects( pd3dDevice );
    pMARK->CreateSub( this, "INFO_CTF_DISPLAY_AUTH_INFO_MARK" );
    pMARK->SetImage( "INFO_CTF_DISPLAY_AUTH_INFO_MARK_NONE" );
    RegisterControl( pMARK );
}

void CInfoCTFDisplay::AUTHINFO::SetAuthSchool ( ENUM_SCHOOL emAuthSchool )
{
    if ( !pMARK )
    {
        return;
    }

    switch ( emAuthSchool )
    {
    default:
    case SCHOOL_NONE:
        {
            pMARK->SetImage( "INFO_CTF_DISPLAY_AUTH_INFO_MARK_NONE" );
        }
        break;

    case SCHOOL_SM:
    case SCHOOL_HA:
    case SCHOOL_BH:
        {
            std::string strImage = sc::string::format( "INFO_CTF_DISPLAY_AUTH_INFO_MARK_%1%", emAuthSchool );
            pMARK->SetImage( strImage.c_str() );
        }
        break;
    };
}

//----------------------------------------------------------------------------------------------------//

MyInfoCTFDisplay::MyInfoCTFDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CInfoCTFDisplay( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyInfoCTFDisplay::CreateUIWindowAndRegisterOwnership()
{
    CInfoCTFDisplay::Create( INFO_CTF_DISPLAY, "INFO_CTF_DISPLAY" );
    CInfoCTFDisplay::CreateSubControl();
    CInfoCTFDisplay::SetAlignFlag( UI_FLAG_CENTER_X );
    CInfoCTFDisplay::m_pInterface->UiRegisterControl( this, true );
    CInfoCTFDisplay::m_pInterface->UiShowGroupFocus( INFO_CTF_DISPLAY );
}

void MyInfoCTFDisplay::Update()
{
    CInfoCTFDisplay::Update();
}
