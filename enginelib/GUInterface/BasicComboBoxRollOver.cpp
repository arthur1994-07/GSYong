#include "pch.h"
#include "BasicComboBoxRollOver.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "BasicLineBox.h"
#include "UITextControl.h"
#include "NSGUI.h"
//#include "InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CBasicComboBoxRollOver::nSTARTLINE = 0;
const int CBasicComboBoxRollOver::nLIMITLINE = 256; //! �̴ϸʿ��� 100���� �Ѱ� ������̶� 256���� �÷ȴ�.
const int CBasicComboBoxRollOver::nNOT_SELECTED = -1;
const D3DCOLOR	CBasicComboBoxRollOver::dwMOUSEOVERCOLOR = D3DCOLOR_ARGB(0xFF,0x69,0x69,0x69);

CBasicComboBoxRollOver::CBasicComboBoxRollOver(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_nSelectIndex ( nNOT_SELECTED )
	, m_pLineBox(NULL)
	, m_pTextBox(NULL)
	, m_pScrollBar(NULL)
	, m_dwCurMaxItem(0)
{
}

CBasicComboBoxRollOver::~CBasicComboBoxRollOver ()
{
}

void CBasicComboBoxRollOver::CreateBaseComboBoxRollOver ( char* szComboBoxControl, bool bSelectInit /*= TRUE*/ )
{
	CreateComboBoxRollOverImage (bSelectInit);
	CreateScrollBar ();

    CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szComboBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CBasicComboBoxRollOver::CreateWhiteComboBoxRollOver ( char* szComboBoxControl, bool bSelectInit /*= TRUE*/ )
{
    CreateWhiteComboBoxRollOverImage(bSelectInit);
    CreateScrollBar();

    CUIControl TempControl(m_pEngineDevice);
    TempControl.Create( 1, szComboBoxControl );
    const UIRECT& rcParentOldPos = GetLocalPos();
    const UIRECT& rcParentNewPos = TempControl.GetLocalPos();
    AlignSubControl( rcParentOldPos, rcParentNewPos );

    SetLocalPos( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CBasicComboBoxRollOver::CreateComboBoxRollOverImage( bool bSelectInit )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxCombo ( "BASIC_COMBOBOX_ROLLOVER_BACK" );
	RegisterControl ( pLineBoxCombo );

	//	�ǵ������� �������� ������ �ʰ� �մϴ�.
	//pLineBoxCombo->SetUseRenderLine ( CBasicLineBox::TOP, FALSE );

	//	��Ʈ
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	�ؽ�Ʈ �ڽ�
	CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pTextBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, COMBOBOX_ROLLOVER_TEXT_BOX );
	pTextBox->CreateMouseOver ( dwMOUSEOVERCOLOR );
	pTextBox->SetFont ( pFont8 );
	//pTextBox->SetSensitive ( true );
	pTextBox->SetLimitLine ( nLIMITLINE );	
	pTextBox->SetSelectInit( bSelectInit );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CBasicComboBoxRollOver::CreateWhiteComboBoxRollOverImage ( bool bSelectInit )
{
    CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
    pLinBoxSmart->CreateSub( this, "POINT_SHOP_SEARCH_WORD_LIST_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pLinBoxSmart->CreateSubControl( "POINT_SHOP_SEARCH_WORD_LIST_BACK_LINE" );
    RegisterControl( pLinBoxSmart );

    //	�ǵ������� �������� ������ �ʰ� �մϴ�.
    //pLineBoxCombo->SetUseRenderLine ( CBasicLineBox::TOP, FALSE );

    //	��Ʈ
    CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    //	�ؽ�Ʈ �ڽ�
    CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
    pTextBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, COMBOBOX_ROLLOVER_TEXT_BOX );
    pTextBox->CreateMouseOver ( dwMOUSEOVERCOLOR );
    pTextBox->SetFont ( pFont8 );
    //pTextBox->SetSensitive ( true );
    pTextBox->SetLimitLine ( nLIMITLINE );	
    pTextBox->SetSelectInit( bSelectInit );
    RegisterControl ( pTextBox );
    m_pTextBox = pTextBox;
}

void CBasicComboBoxRollOver::CreateScrollBar ()
{
	//	��ũ�ѹ� �޸� ���� ����, ȸ�� ���
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxCombo ( "BASIC_COMBOBOX_ROLLOVER_SCROLLBAR_BOX" );
	RegisterControl ( pLineBoxCombo );
	m_pLineBox = pLineBoxCombo;

	//	�簢 �ڽ� ����� ���� ������� �������ϰ� �� ���İ� ������
	//	���ϴ� ũ���� �̹����Դϴ�. ��, 'COMBOBOX_ROLLOVER_SCROLLBAR'�� ������
	//	��ŭ�� ũ��� ��������. �׷���, �� ���Ŀ� ��ũ�ѹٰ� ��� ������¡�� �Ǵ���,
	//	XSIZE�� �������Ǿ�� �ȵǱ� ������ �� �÷��״� �� ���� ���Դϴ�.
	pLineBoxCombo->ResetAlignFlagLine ();
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::LEFT, UI_FLAG_YSIZE );
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::RIGHT,UI_FLAG_YSIZE );
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::BOTTOM, UI_FLAG_BOTTOM );
	pLineBoxCombo->SetAlignFlag ( UI_FLAG_YSIZE | UI_FLAG_RIGHT );

	int nTotalLine = m_pTextBox->GetVisibleLine();

	//	��ũ�ѹ�
	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_COMBOBOX_ROLLOVER_SCROLLBAR );
	pScrollBar->CreateBaseScrollBar ( "BASIC_COMBOBOX_ROLLOVER_SCROLLBAR" );
	pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
	RegisterControl ( pScrollBar );
	m_pScrollBar = pScrollBar;
}

int CBasicComboBoxRollOver::AddText(
    const CString& str,
    bool bVisibleScroll /*= FALSE*/,
    bool bOneLine /*= FALSE*/,
    const D3DCOLOR& TextColor /*= NS_UITEXTCOLOR::WHITE*/ )
{
	int nRet;

	// Increasing Item Count;
	m_dwCurMaxItem++;

	if( bOneLine )
        nRet = m_pTextBox->AddOneLineText( str, true, TextColor );
		//nRet = m_pTextBox->AddOneLineText( str, TextColor ); //   Memo : ���� ���� �� �� �Ѱ��ְ� �ִ� �� ���� ���� ���� �����Ͽ����ϴ�.
	else
		nRet = m_pTextBox->AddText( str, TextColor );

	int nTotal = m_pTextBox->GetTotalLine ();
	int nViewPerPage = m_pTextBox->GetVisibleLine();

	if ( nTotal <= nViewPerPage )
	{
		if( !bVisibleScroll ) 
		{
			m_pScrollBar->SetVisibleSingle ( FALSE );
			m_pLineBox->SetVisibleSingle ( FALSE );
		}else{
			m_pScrollBar->SetVisibleSingle ( TRUE );
			m_pLineBox->SetVisibleSingle ( TRUE );
		}
	}
	else
	{
		m_pScrollBar->SetVisibleSingle ( TRUE );
		m_pLineBox->SetVisibleSingle ( TRUE );

		m_pScrollBar->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
	}

	return nRet;
}

void  CBasicComboBoxRollOver::ClearText ()
{
	m_pTextBox->ClearText ();

	m_pScrollBar->SetVisibleSingle ( FALSE );
	m_pLineBox->SetVisibleSingle ( FALSE );

	m_pScrollBar->GetThumbFrame()->SetState ( 1, 1 );

	m_dwCurMaxItem = 0;
}

void CBasicComboBoxRollOver::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		// Note : ĳ���� ��� �����.
        NSGUI::SetCharMoveBlock();
	}

	switch ( ControlID )
	{
	case BASIC_COMBOBOX_ROLLOVER_SCROLLBAR:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER_SCROLL );
			}
		}
		break;

	case COMBOBOX_ROLLOVER_TEXT_BOX:
		{
			if ( CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_nSelectIndex = m_pTextBox->GetSelectPos ();
			}
		}
		break;
	}
}

void CBasicComboBoxRollOver::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_nSelectIndex = nNOT_SELECTED;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bFirstControl )
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();
		const int nTotalLine = m_pTextBox->GetTotalLine ();
		const int nLinePerOneView = m_pTextBox->GetVisibleLine();
		if ( nLinePerOneView < nTotalLine )
		{
			m_pScrollBar->GetThumbFrame()->SetState ( nTotalLine, nLinePerOneView );

			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;
			m_pTextBox->SetCurLine ( nPos );			
		}
	}
}

void CBasicComboBoxRollOver::SetSelectIndex ( int nIndex )
{ 
	m_pTextBox->SetSelectPos( nIndex );
//	m_pTextBox->SetCurLine( nIndex ); 
	m_nSelectIndex = nIndex; 

	m_pTextBox->UpdateMouseOverImage();

}

CString	CBasicComboBoxRollOver::GetSelectText ( int nIndex )
{
	if ( !m_pTextBox )
	{
		GASSERT ( 0 && "m_pTextBox�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return CString();
	}

	return m_pTextBox->GetText ( nIndex );
}

void CBasicComboBoxRollOver::SetScrollPercent ( float fPercent )
{
	if ( !m_pScrollBar )
	{
		GASSERT ( 0 && "m_pScrollBar�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return;
	}

	m_pScrollBar->GetThumbFrame()->SetPercent ( fPercent );
}

void CBasicComboBoxRollOver::SetTextData( int nIndex, DWORD dwData )
{
	if ( !m_pTextBox )
	{
		GASSERT ( 0 && "m_pTextBox�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return;
	}

	m_pTextBox->SetTextData( nIndex, dwData );
}

DWORD CBasicComboBoxRollOver::GetTextData( int nIndex )
{
	if ( !m_pTextBox )
	{
		GASSERT ( 0 && "m_pTextBox�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return UINT_MAX;
	}

	return m_pTextBox->GetTextData( nIndex );
}

void CBasicComboBoxRollOver::SetVisibleThumb( bool bVisible )
{
	if ( !m_pScrollBar )
	{
		GASSERT ( 0 && "m_pScrollBar�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return;
	}

	m_pScrollBar->SetVisibleThumb( bVisible );
}

void CBasicComboBoxRollOver::SetTextColor ( int nIndex, const D3DCOLOR cColor )
{
	if ( !m_pTextBox )
	{
		GASSERT ( 0 && "m_pTextBox�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return;
	}

	m_pTextBox->SetUseTextColor( nIndex, TRUE );
	m_pTextBox->SetTextColor   ( nIndex, cColor );
}

int CBasicComboBoxRollOver::GetCount()
{
    return m_pTextBox->GetCount();
}

const bool CBasicComboBoxRollOver::IsValidIndex( DWORD dwIndex ) const
{
	if( IsEmpty() )
		return false;

	if( dwIndex >= m_dwCurMaxItem )
		return false;

	return true;
}