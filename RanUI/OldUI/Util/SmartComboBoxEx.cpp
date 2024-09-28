//#include "pch.h"
#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxButton.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"


#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "RnButton.h"
#include "SmartComboBoxEx.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{

	const char* strTextt ="v";
}

CSmartComboBoxBasicEx::CSmartComboBoxBasicEx (EngineDeviceMan* pEngineDevice)
: CBasicComboBox(pEngineDevice)
, m_pComboBox_RollOver(NULL)
, m_nSelectIndex(0)
{
}

CSmartComboBoxBasicEx::~CSmartComboBoxBasicEx ()
{
}

void CSmartComboBoxBasicEx::CreateSmartComboBox ( const char* szComboBoxControl, const char* szTextureInfo )
{
	CreateSmartComboImage ( szTextureInfo );
	CreateSmartTextBox ();

	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szComboBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );
}

void CSmartComboBoxBasicEx::CreateSmartComboBoxCustom ( const char* szComboBoxControl, 
													 const char* szTextureInfo,
													 const char* szButtonPos, 
													 const char* szButtonNormal, 
													 const char* szButtonOver,
													 const char* szButtonDown)
{
	CreateSmartComboImageCustom ( szTextureInfo, szButtonPos, szButtonNormal, szButtonOver, szButtonDown );
	CreateSmartTextBox ();

	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szComboBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	//SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
	SetLocalPos (UIRECT(rcParentNewPos.left, rcParentNewPos.top,rcParentNewPos.sizeX + 19,rcParentNewPos.sizeY));

	CreateComboBoxRllOver();
}

void CSmartComboBoxBasicEx::SetVisibleSingleRollOver(BOOL fBool)
{
	if ( fBool )
		m_pComboBox_RollOver->SetVisibleSingle(TRUE);
	else
		m_pComboBox_RollOver->SetVisibleSingle(FALSE);
}

void CSmartComboBoxBasicEx::CreateSmartComboImage ( const char* szTextureInfo )
{
	
	CBasicLineBoxSmart* pLineBoxCombo = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateSubControl ( szTextureInfo );
	RegisterControl ( pLineBoxCombo );
	CBasicLineBoxButton* pLineBoxComboButton = new CBasicLineBoxButton(m_pEngineDevice);
	pLineBoxComboButton->CreateSub ( this, "BASIC_LINE_BOX_BUTTON", UI_FLAG_RIGHT, BASIC_COMBOBOX_BUTTON_DOWN  );
	pLineBoxComboButton->CreateBaseButton ( "BASIC_COMBOBOX_BUTTON_DOWN", "BASIC_COMBOBOX_BUTTON_DOWN_IMAGE", "BASIC_COMBOBOX_BUTTON_DOWN_IMAGE_F" );
	RegisterControl ( pLineBoxComboButton );
}

void CSmartComboBoxBasicEx::CreateSmartComboImageCustom ( const char* szTextureInfo, 
													   const char* szButtonPos, 
													   const char* szButtonNormal, 
													   const char* szButtonOver,
													   const char* szButtonDown)
{
	CBasicLineBoxSmart* pLineBoxCombo = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_COMBOBOX_LINE_TEXTURE );
	pLineBoxCombo->CreateSubControl ( szTextureInfo );
	RegisterControl ( pLineBoxCombo );


	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

	//버튼
	RnButton* pLineBoxComboButton = NULL;
	RnButton::CreateArg arg;
	//////////////////////////////////////////////////////////////////////////
	arg.defaultTextureKey = szButtonNormal;
	arg.mouseOverTextureKey = szButtonOver;
	arg.mouseClickTextureKey = szButtonDown;
	//////////////////////////////////////////////////////////////////////////

	arg.pFont = pFont8;
	arg.dwColor = NS_UITEXTCOLOR::WHITE;
	arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
	// 초대하기 버튼
	//CString strText = strTextt;
	//arg.text = strText.GetString();
	//arg.text = sc::string::format( "%s",strTextt);

	pLineBoxComboButton = new RnButton(m_pEngineDevice);
	pLineBoxComboButton->CreateSub(this, szButtonPos, UI_FLAG_RIGHT, BASIC_COMBOBOX_BUTTON_DOWN );
	pLineBoxComboButton->CreateSubControl(arg);
	RegisterControl(pLineBoxComboButton);

	m_pControl= new CUIControl( m_pEngineDevice );
	m_pControl->CreateSub(this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_COMBOBOX );
	RegisterControl( m_pControl );

	UIRECT rcButton = GetLocalPos();
	UIRECT rcControl = m_pControl->GetLocalPos();
	m_pControl->SetLocalPos(
		UIRECT(rcControl.left,
		rcControl.top,
		rcControl.sizeX + rcButton.sizeX,
		rcControl.sizeY));
}

void CSmartComboBoxBasicEx::CreateSmartTextBox ()
{
	//	폰트
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	텍스트 박스
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "BASIC_COMBOBOX_TEXTBOX", UI_FLAG_XSIZE, BASIC_COMBOBOX_TEXTBOX );
	pTextBox->SetFont ( pFont8 );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CSmartComboBoxBasicEx::CreateComboBoxRllOver()
{
	CBasicComboBoxRollOver * pComboBoxRollOver;
	pComboBoxRollOver = m_pComboBox_RollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
	pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_COMBOBOX_ROLLOVER );
	pComboBoxRollOver->CreateBaseComboBoxRollOver( "BASIC_COMBOBOX_ROLLOVER" );
	pComboBoxRollOver->SetVisibleSingle( FALSE );
	RegisterControl( pComboBoxRollOver );

	const UIRECT& rcParentOldPos = GetLocalPos ();
	UIRECT rcOld = m_pComboBox_RollOver->GetLocalPos();
	//로오버박스 세로길이
	m_pComboBox_RollOver->AlignSubControl(rcOld,UIRECT(0,rcParentOldPos.sizeY,rcParentOldPos.sizeX,rcParentOldPos.sizeY*4.0f));
	m_pComboBox_RollOver->SetLocalPos ( D3DXVECTOR2 ( 0, rcParentOldPos.sizeY ) );

	m_pComboBox_RollOver->ClearText();

}

void CSmartComboBoxBasicEx::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{

	switch ( ControlID )
	{
	case BASIC_COMBOBOX_ROLLOVER:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				ResetMessageEx ();
				AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER_SCROLL );
				//AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER_CHANGE );

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					int nIndex = m_pComboBox_RollOver->GetSelectIndex ();

					if ( nIndex < 0 )
					{
						return ;
					}
					m_nSelectIndex = nIndex;
						
	
					CString strTemp = m_pComboBox_RollOver->GetSelectText ( nIndex );
					SetText(strTemp);
					
					//m_pComboBox_RollOver->SetVisibleSingle(FALSE);

					AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER_CHANGE );
					//DWORD dwMSG = m_pComboBox_RollOver->GetMessageEx();
					//DWORD dwMSG = GetMessageEx();

					if (m_pComboBox_RollOver->IsVisible())
					{						
						m_pComboBox_RollOver->SetVisibleSingle(FALSE);
						DWORD dwMsg = GetMessageEx ();
						//if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
						{
							dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
						}
						//ResetMessageEx ();
						AddMessageEx ( dwMsg );
						//AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER_CHANGE );
					}
					
				}
			}
			else
			{

			}
		}
		break;

	case BASIC_COMBOBOX_LINE_TEXTURE:
	case BASIC_COMBOBOX_BUTTON_DOWN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					if (m_pComboBox_RollOver->IsVisible())
					{
						m_pComboBox_RollOver->SetVisibleSingle(FALSE);
						DWORD dwMsg = GetMessageEx ();
						if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
						{
							dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
						}
						ResetMessageEx ();
						AddMessageEx ( dwMsg );
					}
					else
					{
						DWORD dwMsg = GetMessageEx ();
						if ( !(dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
						{
							ResetMessageEx ();
							AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER_SCROLL );
						}
						m_pComboBox_RollOver->SetVisibleSingle(TRUE);
					}
				}
			}
			else
			{

			}
		break;
		}

	default:
		{
			//if ( CHECK_LB_UP_LIKE ( dwMsg ) || CHECK_LB_DOWN_LIKE ( dwMsg ) )
			//{
			//	m_pComboBox_RollOver->SetVisibleSingle(FALSE);
			//}
		}
		break;
	}
		CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

}

void CSmartComboBoxBasicEx::SetRollOverAddText( const CString strText, int nIndex )
{
	m_pComboBox_RollOver->AddText( strText );
}
int CSmartComboBoxBasicEx::GetCount()
{
	return m_pComboBox_RollOver->GetCount();
}

CString CSmartComboBoxBasicEx::GetRollOverText(int nIndex )
{
	return m_pComboBox_RollOver->GetSelectText ( nIndex );
}

void CSmartComboBoxBasicEx::SetSelectIndex( int nIndex )
{
	if( !m_pComboBox_RollOver->IsValidIndex( nIndex ) )
		return;

	m_nSelectIndex = nIndex;
	m_pComboBox_RollOver->SetSelectIndex(nIndex);
	CString strTemp = m_pComboBox_RollOver->GetSelectText ( nIndex );
	SetText(strTemp);
}

int CSmartComboBoxBasicEx::GetSelectIndex()
{
	return m_nSelectIndex;
}

BOOL CSmartComboBoxBasicEx::IsVisibleComboxRoolOver()
{
	return m_pComboBox_RollOver->IsVisible();
}

void CSmartComboBoxBasicEx::ClearText()
{
	m_pComboBox_RollOver->ClearText();
}


