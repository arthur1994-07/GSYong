#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "PostBoxPage.h"
#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"

#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../Item/ItemImageEx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_POSTBOX_PAGE_CLOSE = UIMSG_POSTBOX_WINDOW_OBJECT_CLOSE;
////////////////////////////////////////////////////////////////////

CPostBoxPage::CPostBoxPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pd3dDevice(NULL)
	, m_pParentWindow(NULL)
	, m_strTitleName("")
{
	//  Blank
}

CPostBoxPage::~CPostBoxPage ()
{
	Close ();
}

HRESULT CPostBoxPage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( pd3dDevice )
	{
		m_pd3dDevice = pd3dDevice;
		return CUIGroup::InitDeviceObjects( pd3dDevice );
	}

	m_pd3dDevice = NULL;
	return S_FALSE;
}

HRESULT CPostBoxPage::DeleteDeviceObjects()
{	
	m_pd3dDevice = NULL;
	return CUIGroup::DeleteDeviceObjects();
}

void CPostBoxPage::Create ( const char* szControlKeyword, const char* pTitleName, UIGUID WndID )
{
	CUIControl::Create ( WndID, szControlKeyword, UI_FLAG_DEFAULT );

	m_strTitleName  = pTitleName;
	CreateSubControl();
}

void CPostBoxPage::Begin ()
{
	BeginSubControl ();
}

void CPostBoxPage::End ()
{
	EndSubControl ();
}

void CPostBoxPage::Open	( CPostBoxWindowObject* pParentWindow )
{
	m_pParentWindow = pParentWindow;
	OpenSubControl ();
}

void CPostBoxPage::Close ()
{
	CloseSubControl ();
	m_pParentWindow = NULL;
}

void CPostBoxPage::RequestPush ( const DWORD dwRequest )
{
	if ( m_pParentWindow )
	{
		m_pParentWindow->RequestPush( dwRequest );
	}
}

void CPostBoxPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

CBasicLineBox* CPostBoxPage::CreateBackgroundControl ( char* szConatrolKeyword )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( pLineBoxCombo, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	RegisterControl ( pBody );
	RegisterControl ( pLineBoxCombo );

	return pLineBoxCombo;
}

CBasicTextBox* CPostBoxPage::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CBasicTextBox* CPostBoxPage::CreateNumberBox ( char* szConatrolKeyword )
{
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub	   ( this, szConatrolKeyword );
	pTextBox->SetFont	   ( pFont8 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
	RegisterControl ( pTextBox );

	return pTextBox;
}

CItemImageEx* CPostBoxPage::CreateItemImage ( char* szConatrolKeyword, UIGUID ControlID )
{
	CItemImageEx* pItemImage = new CItemImageEx ( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub ( this, szConatrolKeyword, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

CSwapImage* CPostBoxPage::CreateSwapImage ( char* szConatrolKeyword, UIGUID ControlID )
{
	CSwapImage* pSwapImage = new CSwapImage(m_pEngineDevice);
	pSwapImage->CreateSub ( this, szConatrolKeyword, UI_FLAG_DEFAULT, ControlID );
	RegisterControl ( pSwapImage );

	return pSwapImage;
}
