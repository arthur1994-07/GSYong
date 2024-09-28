#include "pch.h"

#include "./SmartComboBoxRollOver.h"
#include "./BasicTextBoxEx.h"
#include "./BasicScrollBarEx.h"
#include "./BasicLineBox.h"
#include "./BasicLineBoxSmart.h"
#include "./UITextControl.h"
//#include "./InnerInterface.h"

#include "../EngineLib/DxTools/DxFontMan.h"

#include "../EngineLib/GUInterface/BasicScrollThumbFrame.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSmartComboBoxRollOver::CSmartComboBoxRollOver(EngineDeviceMan* pEngineDevice)
    : CBasicComboBoxRollOver(pEngineDevice)
{
}

CSmartComboBoxRollOver::~CSmartComboBoxRollOver()
{
}

void CSmartComboBoxRollOver::CreateSmartComboBoxRollOver ( const char* szComboBoxControl, const char* szComboBoxLineInfoControl, bool bSelectInit )
{
    CUIControl ComboBoxControl(m_pEngineDevice);
    ComboBoxControl.Create ( 1, szComboBoxControl );
    const UIRECT& rComoboBox = ComboBoxControl.GetLocalPos();

    CUIControl LineInfoControl(m_pEngineDevice);
    LineInfoControl.Create ( 1, szComboBoxLineInfoControl );
    const UIRECT& rcLineInfo = LineInfoControl.GetLocalPos();

    DoCreateSmartComboBoxRollOver ( szComboBoxControl, szComboBoxLineInfoControl, bSelectInit, 
        UIRECT(rcLineInfo.left, rcLineInfo.top, rComoboBox.sizeX - rcLineInfo.right, rComoboBox.sizeY - rcLineInfo.bottom) );
}

void CSmartComboBoxRollOver::DoCreateSmartComboBoxRollOver ( const char* szComboBoxControl, const char* szComboBoxLineInfoControl, bool bSelectInit, const UIRECT& rcWorkSpace )
{
    m_rcWorkSpace = rcWorkSpace;

    // ����Ʈ ���ιڽ�
    CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
    pLineBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pLineBox->CreateSubControl( szComboBoxLineInfoControl );
    RegisterControl ( pLineBox );

    // ���� ��Ʈ�� ��þ�¡
    CUIControl TempControl(m_pEngineDevice);
    TempControl.Create ( 1, szComboBoxControl );
    const UIRECT rcParentOldPos = GetLocalPos ();
    const UIRECT rcParentNewPos = TempControl.GetLocalPos ();
    AlignSubControl ( rcParentOldPos, rcParentNewPos );
    SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );

    //-----------------------------------------------------------------------------------------------------//
    UIRECT rcWorkSpaceSize ( 0.0f, 0.0f, rcWorkSpace.sizeX, rcWorkSpace.sizeY );

    //	��Ʈ
    CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    //	�ؽ�Ʈ �ڽ�
    CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
    pTextBox->CreateSub ( this, "SMART_COMBOBOX_ROLLOVER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, COMBOBOX_ROLLOVER_TEXT_BOX );
    pTextBox->CreateMouseOver ( dwMOUSEOVERCOLOR );
    pTextBox->SetFont ( pFont8 );
    pTextBox->SetSensitive ( true );
    pTextBox->SetLimitLine ( nLIMITLINE );	
    pTextBox->SetSelectInit( bSelectInit );
    pTextBox->AlignSubControl( rcParentOldPos, rcWorkSpaceSize );
    pTextBox->SetGlobalPos( GetLocalPos() );
    pTextBox->SetLocalPos( D3DXVECTOR2(rcWorkSpace.left, rcWorkSpace.top) );
    RegisterControl ( pTextBox );
    m_pTextBox = pTextBox;

    //	��ũ�ѹ� ���ιڽ�
    CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
    pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pLineBoxCombo->CreateBaseBoxCombo ( "SMART_COMBOBOX_ROLLOVER_SCROLLBAR_BOX" );
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
    pLineBoxCombo->AlignSubControl( rcParentOldPos, rcWorkSpaceSize );
    pLineBoxCombo->AddLocalPos( D3DXVECTOR2(rcWorkSpace.left, rcWorkSpace.top) );

    int nTotalLine = m_pTextBox->GetVisibleLine();

    //	��ũ�ѹ�
    CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
    pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_COMBOBOX_ROLLOVER_SCROLLBAR );
    pScrollBar->CreateBaseScrollBar ( "SMART_COMBOBOX_ROLLOVER_SCROLLBAR" );
    pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
    pScrollBar->AlignSubControl( rcParentOldPos, rcWorkSpaceSize );
    pScrollBar->AddLocalPos( D3DXVECTOR2(rcWorkSpace.left, rcWorkSpace.top) );
    RegisterControl ( pScrollBar );
    m_pScrollBar = pScrollBar;
}

void CSmartComboBoxRollOver::SetMouseOverColor ( const D3DCOLOR cColor )
{
    if ( !m_pTextBox )
    {
        return;
    }

    m_pTextBox->SetMouseOverColor( cColor );
}

int CSmartComboBoxRollOver::AddTextNoSplit(
    const CString& str,
    bool bVisibleScroll,
    const D3DCOLOR& TextColor)
{
    int nRet = m_pTextBox->AddTextNoSplit( str,  TextColor );

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

void CSmartComboBoxRollOver::ResizeToTextSize ( bool bVisibleScroll )
{
    if ( !m_pTextBox )
    {
        return;
    }

    const UIRECT& rcOldPos       = GetLocalPos();
    const UIRECT& rcOldGlobalPos = GetGlobalPos();
    const UIRECT& rcScrollBarPos = m_pScrollBar->GetLocalPos();

    float fGapHorizontal = (rcOldPos.sizeX - m_rcWorkSpace.right)  + m_rcWorkSpace.left;
    float fGapVertical   = (rcOldPos.sizeY - m_rcWorkSpace.bottom) + m_rcWorkSpace.top;

    // Note : Width�� �����Ѵ�.
    float fMaxTextWidth = static_cast<float>(m_pTextBox->GetLongestPart());

    UIRECT rcNewPos = UIRECT(rcOldPos.left, rcOldPos.top, fMaxTextWidth + fGapHorizontal + rcScrollBarPos.sizeX + 1.0f, rcOldPos.sizeY);
    AlignSubControl( rcOldPos, rcNewPos );
    SetLocalPos( rcNewPos );

    UIRECT        rcNewGlobalPos = UIRECT(rcOldGlobalPos.left, rcOldGlobalPos.top, fMaxTextWidth + fGapHorizontal + rcScrollBarPos.sizeX + 1.0f, rcOldGlobalPos.sizeY);
    SetGlobalPos( rcNewGlobalPos );

    // Note : ���� Height�� �����Ѵ�.
    int   nTotalLine = m_pTextBox->GetTotalLine ();
    float fHeight    = m_pTextBox->CalcMaxHEIGHT ( 0, nTotalLine );

    rcNewPos = UIRECT(rcOldPos.left, rcOldPos.top, rcOldPos.sizeX, fHeight + fGapVertical );
    AlignSubControl( rcOldPos, rcNewPos );
    SetLocalPos( rcNewPos );

    rcNewGlobalPos = UIRECT(rcOldGlobalPos.left, rcOldGlobalPos.top, rcOldGlobalPos.sizeX, fHeight + fGapVertical );
    SetGlobalPos( rcNewGlobalPos );

    m_rcWorkSpace = UIRECT( m_rcWorkSpace.left, m_rcWorkSpace.top, fMaxTextWidth + rcScrollBarPos.sizeX + 1.0f, fHeight );

    m_pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );

    if ( bVisibleScroll )
    {
        m_pScrollBar->SetVisibleSingle ( TRUE );
        m_pLineBox->SetVisibleSingle ( TRUE );
    }
    else
    {
        m_pScrollBar->SetVisibleSingle ( FALSE );
        m_pLineBox->SetVisibleSingle ( FALSE );
    }
}
