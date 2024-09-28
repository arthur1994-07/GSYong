
// bjju.sns

#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "./UIPage.h"
#include "./UIWindowObject.h"
#include "./UIWindowObjectController.h"


////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_PAGE_CLOSE = UIMSG_WINDOW_OBJECT_CLOSE;
////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIPage::CUIPage(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pd3dDevice    ( NULL )
	, m_pParentWindow ( NULL )
	, m_strTitleName  ( ""   )
{
	//  Blank
}

CUIPage::~CUIPage ()
{
	Close ();
}

HRESULT CUIPage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( pd3dDevice )
	{
		m_pd3dDevice = pd3dDevice;
		return CUIGroup::InitDeviceObjects( pd3dDevice );
	}

	m_pd3dDevice = NULL;
	return S_FALSE;
}

HRESULT CUIPage::DeleteDeviceObjects()
{	
	m_pd3dDevice = NULL;
	return CUIGroup::DeleteDeviceObjects();
}

void CUIPage::Create ( const char* szControlKeyword, const char* pTitleName, UIGUID WndID )
{
	CUIControl::Create ( WndID, szControlKeyword, UI_FLAG_DEFAULT );

	m_strTitleName  = pTitleName;
	CreateSubControl();
}

void CUIPage::Begin ()
{
	BeginSubControl ();
}

void CUIPage::End ()
{
	EndSubControl ();
}

void CUIPage::Open	( CUIWindowObject* pParentWindow )
{
	m_pParentWindow = pParentWindow;
	OpenSubControl ();
}

void CUIPage::Close ()
{
	CloseSubControl ();
	m_pParentWindow = NULL;
}

void CUIPage::RequestPush ( const DWORD dwID, const DWORD dwArg1 )
{
	if ( m_pParentWindow )
	{
		m_pParentWindow->RequestPush( dwID, dwArg1 );
	}
}

void CUIPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

CBasicLineBox* CUIPage::CreateBackgroundControl ( char* szConatrolKeyword )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( pLineBoxCombo, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	RegisterControl ( pBody );
	RegisterControl ( pLineBoxCombo );

	return pLineBoxCombo;
}

CBasicLineBoxSmart* CUIPage::CreateLineBox( char* szConatrolKeyword, char* szTexInfo )
{
	CBasicLineBoxSmart* pLineBox;
	pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, szConatrolKeyword );
	pLineBox->CreateSubControl( szTexInfo );
	RegisterControl( pLineBox );

	return pLineBox;
}

CBasicTextBox* CUIPage::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CBasicTextBox* CUIPage::CreateNumberBox ( char* szConatrolKeyword )
{
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub	   ( this, szConatrolKeyword );
	pTextBox->SetFont	   ( pFont8 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
	RegisterControl ( pTextBox );

	return pTextBox;
}

CSwapImage* CUIPage::CreateSwapImage ( char* szConatrolKeyword, UIGUID ControlID )
{
	CSwapImage* pSwapImage = new CSwapImage(m_pEngineDevice);
	pSwapImage->CreateSub ( this, szConatrolKeyword, UI_FLAG_DEFAULT, ControlID );
	RegisterControl ( pSwapImage );

	return pSwapImage;
}

void CUIPage::GetOrgLocalPos ( UIRECT& OutRect )
{
	CString szControlKeyword = GetControlNameEx().c_str();

	CUIControl TempControl(m_pEngineDevice); 
	TempControl.Create ( 1, szControlKeyword ); // MEMO

	OutRect = TempControl.GetLocalPos ();
}

void CUIPage::AdapterPosition ( const std::string& strAdapeterKeyword, const bool bNegative )
{
    CUIControl TempControl(m_pEngineDevice); 
    TempControl.Create ( 1, strAdapeterKeyword.c_str() ); // MEMO

    UIRECT rcLocalPos = TempControl.GetLocalPos ();

    FLOAT fPosX = bNegative ? -rcLocalPos.left : rcLocalPos.left;
    FLOAT fPosY = bNegative ? -rcLocalPos.top  : rcLocalPos.top;

    SetLocalPos( D3DXVECTOR2(fPosX, fPosY) );
}
