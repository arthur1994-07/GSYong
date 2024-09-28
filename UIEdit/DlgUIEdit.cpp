// DlgUIEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UIEdit.h"
#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainFrm.h"

#include "DlgUIEdit.h"
#include "DlgSetAlign.h"
#include "DlgSetParent.h"
#include "ExtendUI/UIEditGroup.h"
#include "ExtendUI/UIEditButton.h"
#include "MainUI/InterfaceAdmin.h"
#include "../EngineLib/GUInterface/InterfaceCfg.h"



// DlgUIEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgUIEdit, CDialog)
CDlgUIEdit::CDlgUIEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUIEdit::IDD, pParent),
	m_nSelectedItem( -1 ),
	m_nPrevSelectedItem( -1 )
{
}

CDlgUIEdit::~CDlgUIEdit()
{
}

void CDlgUIEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTROL, m_CtrlList);
	DDX_Control(pDX, IDC_CHECK_VISIBLE, m_CheckVisible);
	DDX_Control(pDX, IDC_CHECK_VIEW_CTRL, m_CheckViewCtrl);
	DDX_Control(pDX, IDC_CHECK_VIEW_LINE, m_CheckViewLine);
	DDX_Control(pDX, IDC_CHECK_ZOOM, m_CheckZoom);
}

BOOL CDlgUIEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_CtrlList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES); 

	m_CtrlList.InsertColumn(LIST_NO, _T("NO"), 0, 30);
	m_CtrlList.InsertColumn(LIST_NAME, _T("UI CONTROL"), 0, 197);
	m_CtrlList.InsertColumn(LIST_PARENT, _T("PN"), 0, 42);
	m_CtrlList.InsertColumn(LIST_VISIBLE, _T("V"), 0, 25);

	m_CheckViewCtrl.SetCheck(TRUE);
	m_CheckViewLine.SetCheck(TRUE);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgUIEdit, CDialog)
	ON_WM_PAINT()

	ON_BN_CLICKED(IDC_BUTTON_PIC, &CDlgUIEdit::OnBnClickedButtonPic)
	ON_BN_CLICKED(IDC_BUTTON_BUTTON, &CDlgUIEdit::OnBnClickedButtonButton)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgUIEdit::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURESET, &CDlgUIEdit::OnBnClickedButtonTextureset)
	ON_BN_CLICKED(IDC_BUTTON_INFLIP, &CDlgUIEdit::OnBnClickedButtonInflip)
	ON_BN_CLICKED(IDC_BUTTON_CLICKFLIP, &CDlgUIEdit::OnBnClickedButtonClickflip)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTROL, &CDlgUIEdit::OnLvnItemchangedListControl)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgUIEdit::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CDlgUIEdit::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, &CDlgUIEdit::OnBnClickedButtonPaste)
	ON_BN_CLICKED(IDC_BUTTON_SET_PARENT, &CDlgUIEdit::OnBnClickedButtonSetParent)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CDlgUIEdit::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgUIEdit::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_CHECK_VISIBLE, &CDlgUIEdit::OnBnClickedCheckVisible)
	ON_BN_CLICKED(IDC_BUTTON_SET_INTERVAL, &CDlgUIEdit::OnBnClickedButtonSetInterval)
	ON_BN_CLICKED(IDC_BUTTON_SET_COLOR, &CDlgUIEdit::OnBnClickedButtonSetColor)
	ON_BN_CLICKED(IDC_CHECK_VIEW_CTRL, &CDlgUIEdit::OnBnClickedCheckViewCtrl)
	ON_BN_CLICKED(IDC_CHECK_VIEW_LINE, &CDlgUIEdit::OnBnClickedCheckViewLine)
	ON_BN_CLICKED(IDC_CHECK_ZOOM, &CDlgUIEdit::OnBnClickedCheckZoom)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_CONTROL, &CDlgUIEdit::OnNMSetfocusListControl)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONTROL, &CDlgUIEdit::OnNMClickListControl)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CDlgUIEdit::OnBnClickedButtonReset)
END_MESSAGE_MAP()

void CDlgUIEdit::ResetList()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();

	m_CtrlList.DeleteAllItems();

	CString temp;
	int nUISize = pInterfaceAdmin->m_pUIContainer.size();

	for( int i=0;i<nUISize;i++ )
	{
		CUIEditGroup* pControl = pInterfaceAdmin->m_pUIContainer[i].pControl;
		if( !pControl )
			continue;

		temp.Format(_T("%d"), i + 1);
		m_CtrlList.InsertItem(i, temp);
		temp.Format(_T("%s"), pControl->GetControlNameEx().c_str());
		m_CtrlList.SetItemText(i, LIST_NAME, temp);

		CUIControl* pParentControl = pControl->GetParent();

		if(pParentControl && pParentControl->GetWndID() != 0)
		{
			int nUIIndex = pInterfaceAdmin->FindUIListIndex( pParentControl->GetWndID());
			temp.Format(_T( " %d" ), nUIIndex + 1);
		}
		else
		{
			temp.Format(_T("NONE"));
		}

		m_CtrlList.SetItemText(i, LIST_PARENT, temp);

		if(pControl->IsVisible())
		{
			temp.Format(_T("O"));
		}
		else
		{
			temp.Format(_T("X"));
		}

		m_CtrlList.SetItemText(i, LIST_VISIBLE, temp);
	}

	SetSelectedItem(m_nSelectedItem);
}



void CDlgUIEdit::SetSelectedUIInfoMain(CUIEditGroup* UIEditGroup)
{
	CString strParent;
	CString strType;

	if(UIEditGroup->GetParent())
	{
		strParent = UIEditGroup->GetParent()->GetControlNameEx().c_str();
	}
	else
	{
		strParent.Format(_T("NONE"));
	}

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
    strType = GetControlTypetoString(pInterfaceAdmin->GetSelectedControlType());

	SetDlgItemText(IDC_EDIT_NAME, UIEditGroup->GetControlNameEx().c_str());
	SetDlgItemText(IDC_EDIT_PARENT, strParent);
	SetDlgItemText(IDC_STATIC_TYPE, strType);
	SetDlgItemInt(IDC_EDIT_CONTROL_X, int(UIEditGroup->GetLocalPos().left));
	SetDlgItemInt(IDC_EDIT_CONTROL_Y, int(UIEditGroup->GetLocalPos().top));
	SetDlgItemInt(IDC_EDIT_CONTROL_WIDTH, int(UIEditGroup->GetLocalPos().sizeX));
	SetDlgItemInt(IDC_EDIT_CONTROL_HEIGHT, int(UIEditGroup->GetLocalPos().sizeY));
}

void CDlgUIEdit::SetSelectedUIInfoPos(int nX,int nY)
{
	CString	temp;

	temp.Format(_T("%d"), nX);
	SetDlgItemText(IDC_EDIT_CONTROL_X, temp);

	temp.Format(_T("%d"), nY);
	SetDlgItemText(IDC_EDIT_CONTROL_Y, temp);
}

void CDlgUIEdit::SetSelectedUITextureInfo(CString strTexName)
{
	SetDlgItemText(IDC_EDIT_FILE, strTexName);
}

void CDlgUIEdit::SetSelectedUIButtonIn(CString strInName)
{
	SetDlgItemText(IDC_EDIT_INFLIP, strInName);
}

void CDlgUIEdit::SetSelectedUIButtonClick(CString strClickName)
{
	SetDlgItemText(IDC_EDIT_CLICKFLIP, strClickName);
}

void CDlgUIEdit::SetSelectedUIButtonInfo(CString Btn1, CString Btn2, CString Btn3)
{
	SetDlgItemText(IDC_BUTTON_TEXTURESET, Btn1);
	SetDlgItemText(IDC_BUTTON_INFLIP, Btn2);
	SetDlgItemText(IDC_BUTTON_CLICKFLIP, Btn3);

	CString strComp;
	strComp.Format( _T( "No Use" ) );

	if( Btn1 == strComp )
		GetDlgItem(IDC_BUTTON_TEXTURESET)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_TEXTURESET)->EnableWindow(TRUE);

	if( Btn2 == strComp )
		GetDlgItem(IDC_BUTTON_INFLIP)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_INFLIP)->EnableWindow(TRUE);

	if( Btn3 == strComp )
		GetDlgItem(IDC_BUTTON_CLICKFLIP)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_CLICKFLIP)->EnableWindow(TRUE);
}

void CDlgUIEdit::SetSelectedUIInfo(int nItem)
{
	if(nItem == -1)
	{
		ClearUIInfo();
		return;
	}

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
    CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();

	if(tempUI == NULL)
	{
		ClearUIInfo();
		return;
	}

	CString strParent;
	CString strType;

	if(tempUI->GetParent())
	{
		strParent = tempUI->GetParent()->GetControlNameEx().c_str();
	}
	else
	{
		strParent.Format(_T("NONE"));
	}

	strType = GetControlTypetoString(pInterfaceAdmin->GetSelectedControlType());

	SetSelectedUIInfoMain(tempUI);

	SetSelectedUITextureInfo(tempUI->GetTextureName());
	SetSelectedUIButtonIn(_T(""));
	SetSelectedUIButtonClick(_T(""));

	if(pInterfaceAdmin->GetSelectedControlType() == BUTTON_CON) 
	{
		CUIEditButton* pWin = ( CUIEditButton* )( tempUI );
		SetSelectedUIButtonIn(pWin->m_pMouseInFlip->GetTextureName());
		SetSelectedUIButtonClick(pWin->m_pMouseClickFlip->GetTextureName());
		SetSelectedUIButtonInfo(_T("Texture Set"), _T("In Flip"), _T("Click Flop"));
	}
	else if(pInterfaceAdmin->GetSelectedControlType() == ANIMATION_CON)
	{
		SetSelectedUIButtonInfo(_T("Set Ani"), _T("Play Ani"), _T("Stop Ani"));

	}
	else
    {
        SetSelectedUIButtonInfo(_T("Texture Set"), _T("No Use"), _T("No Use"));
    }
}

void CDlgUIEdit::SetSelectedUIOptionInfo(int nItem)
{
	if(m_nSelectedItem == -1)
	{
		m_CheckViewCtrl.SetCheck(false);
		m_CheckViewLine.SetCheck(false);
	}
	else
	{
        CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
        CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
		CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();

		m_CheckViewCtrl.SetCheck(tempUI->IsVisible());
		m_CheckViewLine.SetCheck(tempUI->GetLineView());

		CString temp;

		if(tempUI->IsVisible())
		{
			temp.Format(_T("O"));
		}
		else
		{
			temp.Format(_T("X"));
		}

		m_CtrlList.SetItemText(nItem, LIST_VISIBLE, temp);
	}
}

void CDlgUIEdit::ClearUIInfo()
{
	SetDlgItemText(IDC_EDIT_NAME, _T(""));
	SetDlgItemText(IDC_EDIT_PARENT, _T(""));
	SetDlgItemText(IDC_EDIT_CONTROL_X, _T(""));
	SetDlgItemText(IDC_EDIT_CONTROL_Y, _T(""));
	SetDlgItemText(IDC_EDIT_CONTROL_WIDTH, _T(""));
	SetDlgItemText(IDC_EDIT_CONTROL_HEIGHT, _T(""));
}

void CDlgUIEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

}

//void CDlgUIEdit::OnBnClickedButtonDialog()
//{
//	pInterfaceAdmin->MakeDynamicDialog();
//	ResetList();
//}

void CDlgUIEdit::OnBnClickedButtonPic()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->MakeDynamicControl();
	ResetList();
}

void CDlgUIEdit::OnBnClickedButtonButton()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->MakeDynamicButton();
	ResetList();
}

void CDlgUIEdit::OnBnClickedButtonStatic()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->MakeDynamicStatic();
	ResetList();
}

void CDlgUIEdit::OnBnClickedButtonAni()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->MakeDynamicAnimation();
	ResetList();
}

void CDlgUIEdit::OnBnClickedButtonScroll()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->MakeDynamicScroll();
}

void CDlgUIEdit::OnBnClickedButtonText()
{
	//CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	//if(tempUI != NULL)
	//{
	//	if(pInterfaceAdmin->GetSelectedControlType() == STATIC_CON)
	//	{
	//		CDlgStaticText DlgStaticText;
	//		DlgStaticText.DoModal();
	//	}
	//	else
	//	{
	//		CDlgTipText DlgTipText;
	//		DlgTipText.DoModal();
	//	}
	//}
	//else
	//{
	//	MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	//}
}

void CDlgUIEdit::OnBnClickedButtonAlign()
{
	//CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	//if(tempUI != NULL)
	//{
	//	CDlgSetAlign DlgSetAlign;
	//	DlgSetAlign.DoModal();
	//}
	//else
	//{
	//	MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	//}
}

void CDlgUIEdit::OnBnClickedButtonOk()
{
	CString	temp;
	int tempInt;
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		for(int i=0;i<signed(pInterfaceAdmin->m_pUIContainer.size());i++)
		{
			if(i == pInterfaceAdmin->FindUIListIndex(tempUI))
				continue;

			GetDlgItemText(IDC_EDIT_NAME, temp);
			if(temp == pInterfaceAdmin->m_pUIContainer[i].pControl->GetControlNameEx().c_str())
			{
				MessageBox(_T("Rename Control`s Name(Same)"),_T("Error"),MB_OK);
				return;
			}
		}

		UIRECT rcUIControl;

		tempInt = GetDlgItemInt(IDC_EDIT_CONTROL_WIDTH);
		rcUIControl.sizeX  = ( float )( tempInt );
		tempInt = GetDlgItemInt(IDC_EDIT_CONTROL_HEIGHT);
		rcUIControl.sizeY  = (float )( tempInt ); 
		tempInt = GetDlgItemInt(IDC_EDIT_CONTROL_X);
		rcUIControl.left   = ( float )( tempInt ); 
		tempInt = GetDlgItemInt(IDC_EDIT_CONTROL_Y);
		rcUIControl.top    = ( float )( tempInt ); 
		rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
		rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

		GetDlgItemText(IDC_EDIT_NAME, temp);
		tempUI->SetControlNameEx( temp.GetBuffer() );
		tempUI->SetLocalPos(rcUIControl);
		tempUI->InitDeviceObjects(DXUTGetD3DDevice());

		if(tempUI->GetParent())
		{
			UIRECT rcParent = tempUI->GetParent()->GetGlobalPos();
			tempUI->AlignSubControlEX( rcParent, rcParent );
		}
		else 
		{
			UIRECT rcNewWindow( 0, 0, pInterfaceAdmin->m_nViewWidth, pInterfaceAdmin->m_nViewHeight );
			tempUI->AlignMainControlEX( rcNewWindow, rcNewWindow );
		}

		tempUI->RestoreDeviceObjects(DXUTGetD3DDevice());


		ResetList();

		SetSelectedUIInfoMain(tempUI);

	}
	else
	{
		MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedButtonTextureset()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

		if(pFrame->m_cDlgEditText.IsWindowVisible())
		{
			pFrame->m_cDlgEditText.ShowWindow( SW_HIDE );
			pFrame->m_cDlgMainView.ShowWindow( SW_SHOWNOACTIVATE );
		}
		else
		{
			pFrame->m_cDlgMainView.ShowWindow( SW_HIDE );

			if(pInterfaceAdmin->GetSelectedControlType() == ANIMATION_CON)
			{
				//pFrame->m_cDlgEditText.SetTextureMode(ANIMATION_CON);
				//pFrame->m_cDlgEditText.ShowWindow(SW_SHOW);
				//pFrame->m_cDlgEditText.SetTextureInfo(tempUI->GetTextureName(),tempUI->GetTexturePos().left,
				//	tempUI->GetTexturePos().top,tempUI->GetTexturePos().right,tempUI->GetTexturePos().bottom);
				//pFrame->m_cDlgEditText.Invalidate();

				//CDlgSetAni DlgSetAni;
				//DlgSetAni.DoModal();
			}
			else
			{
				pFrame->m_cDlgEditText.SetTextureMode(PICURE_CON);
				pFrame->m_cDlgEditText.ShowWindow(SW_SHOW);
				pFrame->m_cDlgEditText.SetTextureInfo(tempUI->GetTextureName(), tempUI->GetTexturePos().left, tempUI->GetTexturePos().top,
					tempUI->GetTexturePos().right,tempUI->GetTexturePos().bottom);
				pFrame->m_cDlgEditText.Invalidate();
			}
		}
	}
	else
	{
		MessageBox(_T("You have to Select UIControl.."),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedButtonInflip()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIControl* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		if(pInterfaceAdmin->GetSelectedControlType() == BUTTON_CON)
		{
			CUIEditButton* pWin = ( CUIEditButton* ) (tempUI);

			CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

			if(pFrame->m_cDlgEditText.IsWindowVisible())
			{
				pFrame->m_cDlgEditText.ShowWindow( SW_HIDE );
				pFrame->m_cDlgMainView.ShowWindow( SW_SHOWNOACTIVATE );
			}
			else
			{
				pFrame->m_cDlgEditText.SetTextureMode(BUTTON_IN_CON);
				pFrame->m_cDlgMainView.ShowWindow( SW_HIDE );
				pFrame->m_cDlgEditText.ShowWindow( SW_SHOW );
				pFrame->m_cDlgEditText.SetTextureInfo(pWin->m_pMouseInFlip->GetTextureName(),pWin->m_pMouseInFlip->GetTexturePos().left,
					pWin->m_pMouseInFlip->GetTexturePos().top,pWin->m_pMouseInFlip->GetTexturePos().right,pWin->m_pMouseInFlip->GetTexturePos().bottom);
				pFrame->m_cDlgEditText.Invalidate();
			}
		}
		else if(pInterfaceAdmin->GetSelectedControlType() == ANIMATION_CON)
		{
			//CUIEditAnimation* pWin = dynamic_cast<CUIEditAnimation*> (tempUI);
			//pWin->PlayAni();
		}
		else
		{
			MessageBox(_T("You have to Select Button Control."),_T("Error"),MB_OK);
		}
	}
	else
	{
		MessageBox(_T("You have to Select UIControl.."),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedButtonClickflip()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		if(pInterfaceAdmin->GetSelectedControlType() == BUTTON_CON)
		{
			CUIEditButton* pWin = ( CUIEditButton* ) (tempUI);

			CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

			if(pFrame->m_cDlgEditText.IsWindowVisible())
			{
				pFrame->m_cDlgEditText.ShowWindow( SW_HIDE );
				pFrame->m_cDlgMainView.ShowWindow( SW_SHOWNOACTIVATE );
			}
			else
			{
				pFrame->m_cDlgEditText.SetTextureMode(BUTTON_CLICK_CON);
				pFrame->m_cDlgMainView.ShowWindow( SW_HIDE );
				pFrame->m_cDlgEditText.ShowWindow( SW_SHOW );
				pFrame->m_cDlgEditText.SetTextureInfo(pWin->m_pMouseClickFlip->GetTextureName(),pWin->m_pMouseClickFlip->GetTexturePos().left,
					pWin->m_pMouseClickFlip->GetTexturePos().top,pWin->m_pMouseClickFlip->GetTexturePos().right,pWin->m_pMouseClickFlip->GetTexturePos().bottom);
				pFrame->m_cDlgEditText.Invalidate();
			}
		}
		else if(pInterfaceAdmin->GetSelectedControlType() == ANIMATION_CON)
		{
			//CUIEditAnimation* pWin = dynamic_cast<CUIEditAnimation*> (tempUI);
			//pWin->StopAni();
		}
		else
		{
			MessageBox(_T("You have to Select Button Control."),_T("Error"),MB_OK);
		}
	}
	else
	{
		MessageBox(_T("You have to Select UIControl.."),_T("Error"),MB_OK);
	}
}

BOOL CDlgUIEdit::isEdit()
{
	CWnd* cWnd;
	cWnd = this->GetFocus();

	if(cWnd == GetDlgItem(IDC_EDIT_NAME))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_CONTROL_WIDTH))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_CONTROL_HEIGHT))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_CONTROL_X))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_CONTROL_Y))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_INTERVAL))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_COLOR_A))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_COLOR_R))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_COLOR_G))
		return TRUE;
	if(cWnd == GetDlgItem(IDC_EDIT_COLOR_B))
		return TRUE;
	//if(cWnd == GetDlgItem(IDC_LIST_CONTROL))
	//	return TRUE;

	return FALSE;
}

void CDlgUIEdit::SetSelectedItem(int nItem)
{
	//if(m_nSelectedItem != nItem)
	//{
	m_CtrlList.SetSelectionMark(nItem);
	m_CtrlList.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_CtrlList.EnsureVisible(nItem, false);

	if(nItem != -1)
	{
		SetSelectedUIInfo(nItem);
		SetSelectedUIOptionInfo(nItem);
	}

	m_nSelectedItem = nItem;
	//}
}

void CDlgUIEdit::OnLvnItemchangedListControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nItem = pNMLV->iItem; 

	if(nItem != m_nSelectedItem)
		m_nSelectedItem = nItem;

	*pResult = 0;
}

void CDlgUIEdit::OnBnClickedButtonDelete()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		pInterfaceAdmin->DelUIList(tempUI);
	}
	else
	{
		MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedButtonCopy()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		pInterfaceAdmin->CopyUIList(pInterfaceAdmin->FindUIListIndex(tempUI));							
	}
	else
	{
		MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedButtonPaste()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->PasteUIList();
}

void CDlgUIEdit::OnBnClickedButtonSetParent()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		CDlgSetParent DlgSetParent;
		DlgSetParent.DoModal();
	}
	else
	{
		MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	}
	ResetList();
}

void CDlgUIEdit::OnBnClickedButtonLoad()
{
	//CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();

	CString szFilter = _T("Xml file  (*.xml)|*.xml|");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg ( TRUE,_T(".xml"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pFrame );

	CString strInitDir = CInterfaceCfg::GetInstance().GetPath();
	dlg.m_pOFN->lpstrInitialDir = strInitDir;
	if ( dlg.DoModal() == IDOK )
	{
		pInterfaceAdmin->ClearUIList();
		pInterfaceAdmin->LoadXML(dlg.GetFileName() );
		ResetList();
	}
}

void CDlgUIEdit::OnBnClickedButtonMerge()
{
	//CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

	//CString szFilter = _T("Xml file  (*.xml)|*.xml|");

	////	Note : 파일 오픈 다이알로그를 만듬.
	//CFileDialog dlg(TRUE,_T(".xml"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, pFrame);

	//CString strInitDir = CORE_COMMON::SystemInfo::getAppPath();
	//dlg.m_pOFN->lpstrInitialDir = strInitDir;

	//if ( dlg.DoModal() == IDOK )
	//{
	//	pInterfaceAdmin->LoadXML(dlg.GetFileName());
	//	ResetList();
	//}
}

void CDlgUIEdit::OnBnClickedButtonSave()
{
	//CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();

	CString szFilter = _T("Xml file  (*.xml)|*.xml|");

	//	Note : 파일 저장 다이알로그를 만듬.
	CFileDialog dlg(FALSE, _T(".xml"), NULL, OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT, szFilter, pFrame);

	CString strInitDir = CInterfaceCfg::GetInstance().GetPath();
	dlg.m_pOFN->lpstrInitialDir = strInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		pInterfaceAdmin->SaveXML(dlg.GetFileName());
		ResetList();
	}
}

void CDlgUIEdit::OnBnClickedCheckVisible()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->SetGraphVisible(m_CheckVisible.GetCheck());

	if(m_CheckVisible.GetCheck())
	{
		SetDlgItemInt(IDC_EDIT_INTERVAL, pInterfaceAdmin->GetLineInterval());
		SetDlgItemInt(IDC_EDIT_COLOR_A, pInterfaceAdmin->GetGraphLineColor().a);
		SetDlgItemInt(IDC_EDIT_COLOR_R, pInterfaceAdmin->GetGraphLineColor().r);
		SetDlgItemInt(IDC_EDIT_COLOR_G, pInterfaceAdmin->GetGraphLineColor().g);
		SetDlgItemInt(IDC_EDIT_COLOR_B, pInterfaceAdmin->GetGraphLineColor().b);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_INTERVAL, _T(""));
		SetDlgItemText(IDC_EDIT_COLOR_A, _T(""));
		SetDlgItemText(IDC_EDIT_COLOR_R, _T(""));
		SetDlgItemText(IDC_EDIT_COLOR_G, _T(""));
		SetDlgItemText(IDC_EDIT_COLOR_B, _T(""));
	}
}

void CDlgUIEdit::OnBnClickedButtonSetInterval()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	int nInterval = GetDlgItemInt(IDC_EDIT_INTERVAL);

	if(nInterval > 1)
	{
		pInterfaceAdmin->SetLineInterval(nInterval);
	}
	else
	{
		MessageBox(_T("Line InterVal > 1"),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedButtonSetColor()
{
	int ColorA = GetDlgItemInt(IDC_EDIT_COLOR_A);
	int ColorR = GetDlgItemInt(IDC_EDIT_COLOR_R);
	int ColorG = GetDlgItemInt(IDC_EDIT_COLOR_G);
	int ColorB = GetDlgItemInt(IDC_EDIT_COLOR_B);

	if(ColorA < 0 || ColorA >255)
	{
		MessageBox(_T("0 < Color Value < 255(Alpha Box)"),_T("Error"),MB_OK);
		return;
	}

	if(ColorR < 0 || ColorR >255)
	{
		MessageBox(_T("0 < Color Value < 255(Red Box)"),_T("Error"),MB_OK);
		return;
	}

	if(ColorG < 0 || ColorG >255)
	{
		MessageBox(_T("0 < Color Value < 255(Green Box)"),_T("Error"),MB_OK);
		return;
	}

	if(ColorB < 0 || ColorB >255)
	{
		MessageBox(_T("0 < Color Value < 255(Blue Box)"),_T("Error"),MB_OK);
		return;
	}

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->SetGraphLineColor(ColorA, ColorR,ColorG,ColorB);
}

void CDlgUIEdit::OnBnClickedCheckViewCtrl()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{	
		tempUI->SetVisibleSingle( m_CheckViewCtrl.GetCheck());
		SetSelectedUIOptionInfo(m_nSelectedItem);
	}
	else
	{
		MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedCheckViewLine()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{	
		tempUI->SetLineView(m_CheckViewLine.GetCheck());
	}
	else
	{
		MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	}
}

void CDlgUIEdit::OnBnClickedCheckShowTip()
{
	//CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();

	//if(tempUI != NULL)
	//{	
	//	if(tempUI->GetTipFile().GetLength() > 0 && tempUI->GetTipId().GetLength() > 0)
	//	{
	//		tempUI->SetUseTip(m_CheckShowTip.GetCheck());
	//	}
	//	else
	//	{
	//		MessageBox(_T("First You have to Set Tip."),_T("Error"),MB_OK);
	//		m_CheckShowTip.SetCheck(FALSE);
	//	}
	//}
	//else
	//{
	//	MessageBox(_T("You have to Select UIControl."),_T("Error"),MB_OK);
	//	m_CheckShowTip.SetCheck(FALSE);
	//}
}

void CDlgUIEdit::OnBnClickedCheckZoom()
{
	// 줌윈도우 보기, 안보기
	if(m_CheckZoom.GetCheck())
	{
		CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pFrame->m_cDlgMainView.ShowWindow(SW_SHOW);
	}
	else
	{
		CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pFrame->m_cDlgMainView.ShowWindow(SW_HIDE);
	}
}

void CDlgUIEdit::OnNMSetfocusListControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetFocus();

	*pResult = 0;
}

void CDlgUIEdit::OnNMClickListControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();

	m_nSelectedItem = m_CtrlList.GetNextItem(-1, LVNI_SELECTED);

	pInterfaceAdmin->SetSelectedControl(m_nSelectedItem, false);

	SetSelectedUIInfo(m_nSelectedItem);
	SetSelectedUIOptionInfo(m_nSelectedItem);

	*pResult = 0;
}

CString CDlgUIEdit::GetControlTypetoString(int nType)
{
	CString strType;

	//switch(nType)
	//{
	//case DIALOG_CON:
	//	strType = _T("DIALOG_CON");
	//	break;
	//case PICURE_CON:
	//	strType = _T("PICURE_CON");
	//	break;
	//case BUTTON_CON:
	//	strType =  _T("BUTTON_CON");
	//	break;
	//case BUTTON_IN_CON:
	//	strType =  _T("BUTTON_IN_CON");
	//	break;
	//case BUTTON_CLICK_CON:
	//	strType =  _T("BUTTON_CLICK_CON");
	//	break;
	//case STATIC_CON:
	//	strType =  _T("STATIC_CON");
	//	break;
	//case EDIT_CON:
	//	strType =  _T("EDIT_CON");
	//	break;
	//case PROGRESS_CON:
	//	strType =  _T("PROGRESS_CON");
	//	break;
	//case ANIMATION_CON:
	//	strType =  _T("ANIMATION_CON");
	//	break;
	//case ANIMATION_SUB_CON:
	//	strType =  _T("ANIMATION_SUB_CON");
	//	break;
	//case BUTTON_TEXT_CON:
	//	strType =  _T("BUTTON_TEXT_CON");
	//	break;
	//default:
	//	strType = _T("NONE");
	//	break;
	//}

	return strType;
}
void CDlgUIEdit::OnBnClickedButtonReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	int nResult; 

	nResult = MessageBox(_T("UI 리스트를 초기화 하시겠습니까?"), _T("Message"), MB_YESNO);

	if(nResult == IDYES)
	{
		pInterfaceAdmin->ClearUIList();
	}
	else if(nResult == IDNO)
	{
		return;
	}
}


// DlgUIEdit 메시지 처리기입니다.
