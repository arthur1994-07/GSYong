#include "pch.h"
#include "./UIWindowTitle.h"
#include "./BasicTextBox.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIWindowTitle::CUIWindowTitle (EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pTitleName ( NULL )
{	
}

CUIWindowTitle::~CUIWindowTitle ()
{
}

void CUIWindowTitle::CreateTitle ( char* szLeft, char* szMid, char* szRight )
{
	WORD wAlignFlag;

	CUIControl* pLeft = new CUIControl(m_pEngineDevice);	
    wAlignFlag = UI_FLAG_LEFT;
    pLeft->CreateSub ( this, szLeft, wAlignFlag );
	RegisterControl ( pLeft );

	CUIControl* pMid = new CUIControl(m_pEngineDevice);
	wAlignFlag = UI_FLAG_XSIZE; // Note : 가운데 이미지는 크기가 변한다.
    pMid->CreateSub ( this, szMid, wAlignFlag );
	RegisterControl ( pMid );

	CUIControl* pRight = new CUIControl(m_pEngineDevice);
	wAlignFlag = UI_FLAG_RIGHT;
    pRight->CreateSub ( this, szRight, wAlignFlag );
	RegisterControl ( pRight );
}

void CUIWindowTitle::CreateTitleName ( char* szTextBox, const char* szText, CD3DFontPar* pFont8 )
{
	WORD wAlignFlag;

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	wAlignFlag = UI_FLAG_DEFAULT;
	pTextBox->CreateSub ( this, szTextBox, wAlignFlag );
	pTextBox->SetFont ( pFont8 );
	pTextBox->SetOneLineText ( szText, NS_UITEXTUTIL::WINDOWTITLE );
	RegisterControl ( pTextBox );
	m_pTitleName = pTextBox;
}

void CUIWindowTitle::SetTitleName ( const char* szTitleName )
{
	if ( m_pTitleName )
	{
		m_pTitleName->SetOneLineText ( szTitleName, NS_UITEXTUTIL::WINDOWTITLE );
	}
}

void CUIWindowTitle::SetTitleAlign ( int nAlign )
{
	if ( m_pTitleName )
	{
		m_pTitleName->SetTextAlign ( nAlign );
	}
}