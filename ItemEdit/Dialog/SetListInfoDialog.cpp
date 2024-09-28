// SetListInfoDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ItemEdit.h"
#include "SetListInfoDialog.h"


// SetListInfoDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(SetListInfoDialog, CDialog)

SetListInfoDialog::SetListInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SetListInfoDialog::IDD, pParent), m_GiveToSetItemInfoDlg((DWORD)0)
{
}

SetListInfoDialog::~SetListInfoDialog()
{
}

void SetListInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SET_LIST, m_SetITListList);
}

BOOL SetListInfoDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	OutPutSetList();
	return true;
}

BEGIN_MESSAGE_MAP(SetListInfoDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &SetListInfoDialog::OnBnClickedButtonSelect)
END_MESSAGE_MAP()


// SetListInfoDialog 메시지 처리기입니다.

void SetListInfoDialog::OutPutSetList()
{
	m_SetITListList.DeleteAllItems();
	m_SetITListList.SetExtendedStyle(m_SetITListList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_SetITListList.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 100, -1);
	m_SetITListList.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 400, -1);

	m_vecSetIteminfoList = GLItemSet::GetInstance().GetSetItemInfoList();
	for(DWORD i =0; i < m_vecSetIteminfoList.size(); ++i)
	{
		CString strTmp;
		strTmp.Format(_T("%d"), m_vecSetIteminfoList[i].dwSetID);
		int TempInsertIndex = m_SetITListList.InsertItem(i, strTmp);
		m_SetITListList.SetItemText(TempInsertIndex, 1, m_vecSetIteminfoList[i].strName.c_str());
	}
}

void SetListInfoDialog::OnBnClickedButtonSelect()
{
	for(DWORD i = 0; i <m_vecSetIteminfoList.size(); ++i )
	{	
		if(LVIS_SELECTED == m_SetITListList.GetItemState(i, LVIS_SELECTED))
		{
			m_GiveToSetItemInfoDlg = m_vecSetIteminfoList[i].vecItemList[0];			
		}
	}

	CDialog::OnOK();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
