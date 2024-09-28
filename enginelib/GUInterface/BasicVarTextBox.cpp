#include "pch.h"

#include "./BasicVarTextBox.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "./BasicLineBoxEx.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "./UITextControl.h"
#include "../EngineLib/GUInterface/UIDebugSet.h"
#include "../../Enginelib/GUInterface/UITextControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CBasicVarTextBox::fMOUSEPOINT_GAP = 20.0f;

CBasicVarTextBox::CBasicVarTextBox (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)

	, m_pTextBox ( NULL )
    , m_pLineBox ( NULL )
	, m_vMousePointGap (fMOUSEPOINT_GAP,fMOUSEPOINT_GAP)

	, m_pSelfDummy ( NULL )
    , m_pFont9 ( NULL )

    , m_bBLOCK_MOUSETRACKING ( false )
{
}

CBasicVarTextBox::~CBasicVarTextBox ()
{
}

void CBasicVarTextBox::CreateSubControl ()
{
	m_pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_VAR_LINE_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxVarTextBox ( "BASIC_VAR_LINE_BOX" );
	pLineBox->SetNoUpdate ( true );
	RegisterControl ( pLineBox );
	m_pLineBox = pLineBox;

	CUIControl* pSelfDummy = new CUIControl(m_pEngineDevice);
	pSelfDummy->CreateSub ( this, "BASIC_VAR_LINE_BOX" );
	pSelfDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pSelfDummy );
	m_pSelfDummy = pSelfDummy;

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "VAR_TEXT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( m_pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetNoUpdate ( true );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CBasicVarTextBox::ClearText ()
{
	if ( m_pTextBox )
		m_pTextBox->ClearText ();
}

int CBasicVarTextBox::AddTextNoSplit (const CString& strText, D3DCOLOR dwColor )
{
	if ( m_pTextBox )
		return m_pTextBox->AddTextNoSplit ( strText, dwColor );

	return -1;
}

int	CBasicVarTextBox::SetTextNoSplit (const CString& strText, D3DCOLOR dwColor )
{
	if ( m_pTextBox )
		return m_pTextBox->SetTextNoSplit ( strText, dwColor );

	return -1;
}

void CBasicVarTextBox::SetText(const CString& strText, D3DCOLOR dwColor )
{
	if ( m_pTextBox )
		m_pTextBox->SetText( strText, dwColor );
}

void CBasicVarTextBox::AddString ( int nIndex, const CString& strText, const D3DCOLOR& dwColor )
{
	if ( m_pTextBox )
		m_pTextBox->AddString ( nIndex, strText, dwColor );
}

int CBasicVarTextBox::AddText (const CString& strText, D3DCOLOR dwColor )
{
	if ( m_pTextBox )
		return m_pTextBox->AddText ( strText, dwColor );

	return -1;
}

void CBasicVarTextBox::AddTextLongestLineSplit (const CString& strText, D3DCOLOR dwColor )
{
	float fLongestLine = GetLongestLine ();
	
	NS_UITEXTCONTROL::MULTILINETEXT& MLTextSet =
		NS_UITEXTCONTROL::GetMLTextWithoutCRLF ( strText, fLongestLine, m_pFont9 );

	for ( int i = 0; i < (int)MLTextSet.size(); ++i )
	{
		CString strTemp = MLTextSet[i].strLine;
		if ( strTemp.GetLength() )
		{
			AddTextNoSplit ( strTemp, dwColor );		
		}
	}	
}

void CBasicVarTextBox::SetTextAlign ( int nALIGN )
{
	m_pTextBox->SetTextAlign ( nALIGN );
}

void CBasicVarTextBox::SetUseOverColor ( bool bUseColor )
{
	m_pTextBox->SetUseOverColor ( (bUseColor)?TRUE:FALSE );
}

void CBasicVarTextBox::SetOverColor ( const D3DCOLOR& dwColor )
{
	int nIndex = m_pTextBox->GetCount () - 1;
	m_pTextBox->SetOverColor ( nIndex, dwColor );
}

void CBasicVarTextBox::SetLineInterval ( const float fLineInterval )
{
	m_pTextBox->SetLineInterval ( fLineInterval );
}

void CBasicVarTextBox::SetBlockMouseTracking ( bool bBlock )
{
	m_bBLOCK_MOUSETRACKING = bBlock;
}

void CBasicVarTextBox::SetMousePointGap (const D3DXVECTOR2& vGap)
{
	m_vMousePointGap = vGap;
}

int	CBasicVarTextBox::GetCount ()
{
	return m_pTextBox->GetCount ();
}

float CBasicVarTextBox::GetLongestLine ()
{
	if ( m_pTextBox )
		return (float)m_pTextBox->GetLongestLine ();

	return 0.0f;
}

UIRECT CBasicVarTextBox::CalcSize()
{
	if( !m_pTextBox )
		return UIRECT();

	const UIRECT& rcTextBoxLocalPos = m_pTextBox->GetLocalPos ();
	const UIRECT& rcOriginPos = GetLocalPos ();

	int nTotalLine = m_pTextBox->GetTotalLine ();
	const float fAllLine = m_pTextBox->CalcMaxHEIGHT( 0, nTotalLine );
	const float fTextBoxGapX = (float)m_pTextBox->GetLongestLine () - rcTextBoxLocalPos.sizeX;
	const float fTextBoxGapY = fAllLine - rcTextBoxLocalPos.sizeY;

	return UIRECT( 0.0f, 0.0f, rcOriginPos.sizeX + fTextBoxGapX, rcOriginPos.sizeY + fTextBoxGapY );
}

void CBasicVarTextBox::RePosControl ( int x, int y )
{
	const UIRECT& rcLocalPosDummy = m_pSelfDummy->GetLocalPos ();
	const UIRECT& rcOriginPos = GetLocalPos ();

	AlignSubControl ( rcOriginPos, rcLocalPosDummy );

	if ( m_pTextBox )
	{
		UIRECT rcLocalNewPos = CalcSize();

		rcLocalNewPos.SetPosition(
			float(x) + m_vMousePointGap.x,
			float(y) + m_vMousePointGap.y );

		AlignSubControl ( rcLocalPosDummy, rcLocalNewPos );

		SetGlobalPos ( rcLocalNewPos );		
	}
}

void CBasicVarTextBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () )
		return ;

	SetUseOverColor ( false );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( !m_bBLOCK_MOUSETRACKING )
		RePosControl ( x, y );	
}