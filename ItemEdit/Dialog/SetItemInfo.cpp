// SetItemInfo.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "../ItemEdit.h"
#include "./SetItemInfo.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "./SetListInfoDialog.h"


#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../EngineLib/GUInterface/BasicButton.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/BasicVarTextBox.h"

// SetItemInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(SetItemInfo, CDialog)

SetItemInfo::SetItemInfo(CWnd* pParent /*=NULL*/)
	: CDialog(SetItemInfo::IDD, pParent)
{	
}

SetItemInfo::~SetItemInfo()
{
}

void SetItemInfo::SetNativeID(const SNATIVEID *Item_ID) 
{ 
	m_NativeID.dwID		= Item_ID->dwID;
	m_NativeID.wMainID	= Item_ID->wMainID; 
	m_NativeID.wSubID		= Item_ID->wSubID;
}

void SetItemInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST6, m_cIDList);
	DDX_Control(pDX, IDC_LIST5, m_cBonus);
}

BOOL SetItemInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	FindItemSetInfo();		//필요한 세트아이템 정보들을 검사해서 세팅
	OutPutNoList();		//효과와 ID가 아닌 것들을 출력
	IDListSetting();		//세트를 구성하는 아이템 ID들 출력
	BonusListSetting();		//세트 갯수에 따른 효과들 출력
	
	return true;
}

BEGIN_MESSAGE_MAP(SetItemInfo, CDialog)

	ON_BN_CLICKED(IDC_SELECT_BUTTON, &SetItemInfo::OnBnClickedSelectButton)	
	ON_BN_CLICKED(IDC_RELOAD_BUTTON, &SetItemInfo::OnBnClickedReloadButton)
END_MESSAGE_MAP()

// SetItemInfo 메시지 처리기입니다.
void SetItemInfo::FindItemSetInfo()
{
	if(GLItemSet::GetInstance().IsSetItem(m_NativeID))
	{
		m_SetItem = *GLItemSet::GetInstance().GetInfoSetItem(m_NativeID);
		WORD TempdwID = m_SetItem.dwSetID;
		m_SetList = *GLItemSet::GetInstance().GetInfoSetItemList(TempdwID);
	}
}

void SetItemInfo::OutPutNoList()
{
	if(GLItemSet::GetInstance().IsSetItem(m_NativeID))
	{
		SetWin_Num_int(this, IDC_SETNUM, m_SetItem.dwSetID);
		SetWin_Text(this, IDC_SETNAME, m_SetItem.strName.c_str());
		SetWin_Text(this, IDC_SETFILENAME, m_SetList.strSetName.c_str());
	}
}

void SetItemInfo::IDListSetting()
{	
	m_cIDList.DeleteAllItems();
	
	m_cIDList.SetExtendedStyle(m_cIDList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN column;
	char* List[3] = {"MID", "SID", "Name"};
	int Width[3] = {100, 100, 400};
	for(int i = 0; i < 3; ++i)
	{
		m_cIDList.DeleteColumn(i);
		column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		column.fmt = LVCFMT_LEFT;
		column.iSubItem = -1;
		column.cx = Width[i];		 
		column.pszText = List[i];
		m_cIDList.InsertColumn(i, &column);
	}

	if(GLItemSet::GetInstance().IsSetItem(m_NativeID))
	{	
		for(DWORD i = 0; i < m_SetItem.vecItemList.size(); ++i)
		{	
	
			CString strTmp;
			strTmp.Format(_T("%d"), (int)m_SetItem.vecItemList[i].wMainID);
			int InsertIndex = m_cIDList.InsertItem(i, strTmp);	
			strTmp.Format(_T("%d"), (int)m_SetItem.vecItemList[i].wSubID);
			m_cIDList.SetItem(InsertIndex, 1, LVIF_TEXT, strTmp, 0, 0, 0, 0);		
			SITEM* TempItem = NULL;
			TempItem = GLItemMan::GetInstance().GetItem(m_SetItem.vecItemList[i]);
			if(TempItem != NULL)
			m_cIDList.SetItem(InsertIndex, 2, LVIF_TEXT, TempItem->GetName(), 0, 0, 0, 0);	
		}
	}
}

void SetItemInfo::BonusListSetting()
{	
	m_cBonus.DeleteAllItems();
	m_cBonus.SetExtendedStyle(m_cBonus.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_cBonus.DeleteColumn(0);
	m_cBonus.InsertColumn(0, _T("착용(p)"), LVCFMT_LEFT, 100, -1);
	m_cBonus.DeleteColumn(1);
	m_cBonus.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 1200, -1);

	if(GLItemSet::GetInstance().IsSetItem(m_NativeID))
	{
		BYTE TempCount = 0;
		for(DWORD i = 0; i < m_SetItem.vecBonus.size(); ++i)
		{	
			CString strTmp;
			if(i > 0 && (m_SetItem.vecBonus[i].byTerms == m_SetItem.vecBonus[i - 1].byTerms)) 
			{
				TempCount++;
				continue;
			}
			strTmp.Format(_T("%d"), (int)m_SetItem.vecBonus[i].byTerms);
			strTmp.Append("피스");
			int InsertIndex = m_cBonus.InsertItem(i - TempCount, strTmp );		
			std::string tempstr;			
			GLItemSet::GetInstance().GetItemBonusStr(&tempstr, &m_SetItem, (UINT)m_SetItem.vecBonus[i].byTerms);
			m_cBonus.SetItem(InsertIndex, 1, LVIF_TEXT, tempstr.c_str(), 0, 0, 0, 0);		
		}
	}
}


void SetItemInfo::OnBnClickedSelectButton()
{
	SetListInfoDialog dlg;
	dlg.DoModal();
	m_NativeID = dlg.GetReturnID();

	if(m_NativeID.wMainID != 0 || m_NativeID.wSubID != 0)  OnInitDialog();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



void SetItemInfo::OnBnClickedReloadButton()
{
	std::string tempStr = _T("SetItemList.lua");
	GLItemSet::GetInstance().LoadFile(tempStr, true);
	OnInitDialog();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
