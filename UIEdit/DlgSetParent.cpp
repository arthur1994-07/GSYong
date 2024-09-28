#include "StdAfx.h"
#include "DlgSetParent.h"

#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/d3dfont.h"

#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainFrm.h"

#include "../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"

#include "../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../EngineLib/GUInterface/BasicTextButton.h"
#include "../EngineLib/GUInterface/BasicLineBox.h"

#include "./ExtendUI/UIEditGroup.h"

#include "./MainUI/InterfaceAdmin.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgSetParent, CDialog)

CDlgSetParent::CDlgSetParent(CWnd* pParent /*=NULL*/)
: m_nSelectedItem(-1), CDialog(CDlgSetParent::IDD, pParent)
{
}

CDlgSetParent::~CDlgSetParent()
{
}

void CDlgSetParent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PARENT, m_ParentList);
}

BOOL CDlgSetParent::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ParentList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES); 
	m_ParentList.InsertColumn(LIST_NO, _T("NO"), 0, 60);
	m_ParentList.InsertColumn(LIST_NAME, _T("NAME"), 0, 390);

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	for (int i=0; i<pInterfaceAdmin->m_pUIContainer.size(); ++i)
	{
		CString temp;
		temp.Format(_T("%d"), i + 1);
		m_ParentList.InsertItem(i, temp);
		m_ParentList.SetItemText(i, LIST_NAME, pInterfaceAdmin->m_pUIContainer[i].pControl->GetControlNameEx().c_str());
	}
    
	CUIControl* tempUI = pInterfaceAdmin->GetSelectedControl();

	//current Information ctrlUI
	SetDlgItemInt(IDC_STATIC_CTRL_NO, pInterfaceAdmin->FindUIListIndex(tempUI->GetWndID()) + 1);
	SetDlgItemText(IDC_STATIC_CTRL_NAME, tempUI->GetControlNameEx().c_str());

	if(tempUI->GetParent())
	{
		m_nSelectedItem = pInterfaceAdmin->FindUIListIndex(tempUI->GetParent()->GetWndID());

		//current Information parnet
		SetDlgItemInt(IDC_STATIC_PARENT_NO, m_nSelectedItem + 1);
		SetDlgItemText(IDC_STATIC_PARENT_NAME, tempUI->GetParent()->GetControlNameEx().c_str());
	}

	m_ParentList.SetSelectionMark(m_nSelectedItem);
	m_ParentList.SetItemState(m_nSelectedItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_ParentList.EnsureVisible(m_nSelectedItem, false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgSetParent, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CDlgSetParent::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PARENT, &CDlgSetParent::OnLvnItemchangedListParent)
	//	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_PARENT, &CDlgSetParent::OnNMRdblclkListParent)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PARENT, &CDlgSetParent::OnNMDblclkListParent)
END_MESSAGE_MAP()
void CDlgSetParent::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}

void CDlgSetParent::OnBnClickedOk()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	if(m_nSelectedItem == pInterfaceAdmin->FindUIListIndex(pInterfaceAdmin->GetSelectedControl()))
	{
		MessageBox(_T("You don't Select Self UIControl."), _T("Error"), MB_OK);
		return;
	}
	if(m_nSelectedItem != - 1)
	{
		CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();

		if(pInterfaceAdmin->m_pUIContainer[m_nSelectedItem].pControl->GetParent() == tempUI)
		{
			MessageBox(_T("You don't Set Parent Child UIControl."), _T("Error"), MB_OK);
			return;
		}

		pInterfaceAdmin->SetParentControl(tempUI, pInterfaceAdmin->m_pUIContainer[m_nSelectedItem].pControl);

		//if( tempUI->GetParent() )
		//{
		//	CUIEditGroup* pWin = dynamic_cast<CUIEditGroup*> (tempUI->GetParent());
		//	pWin->DelChild(tempUI->GetWndID());
		//}
		//tempUI->SetParent(pInterfaceAdmin->m_pUIContainer[m_nSelectedItem].pControl);

		//CUIEditGroup* pWin = dynamic_cast<CUIEditGroup*> (pInterfaceAdmin->m_pUIContainer[m_nSelectedItem].pControl);
		//pWin->AddChild(tempUI->GetWndID());

		//UIRECT rcOld,rcParent;

		//rcParent = tempUI->GetParent()->GetLocalPos();
		//rcOld    = tempUI->GetLocalPos();

		//float fX,fY;

		//if(rcOld.left < rcParent.left)
		//	fX = 0;
		//else
		//	fX = rcOld.left - rcParent.left;

		//if(rcOld.top < rcParent.top)
		//	fY = 0;
		//else
		//	fY = rcOld.top - rcParent.top;

		//tempUI->SetLocalPos(D3DXVECTOR2(rcOld.left - rcParent.left,rcOld.top - rcParent.top ));
		//tempUI->AlignSubControlEX(rcParent,rcParent);
		//tempUI->RestoreDeviceObjects( DXUTGetD3DDevice() );

		pInterfaceAdmin->m_pMainDlg->ResetList();
	}
	OnOK();
}

void CDlgSetParent::OnLvnItemchangedListParent(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelectedItem = pNMLV->iItem;

	*pResult = 0;
}

void CDlgSetParent::OnNMDblclkListParent(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nSelectedItem != -1)
		OnBnClickedOk();
	*pResult = 0;
}
