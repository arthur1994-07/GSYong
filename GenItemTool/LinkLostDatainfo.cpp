// LinkLostDatainfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GenItemTool.h"
#include "LinkLostDatainfo.h"
#include "../enginelib/G-Logic/GLogic.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"



// LinkLostDatainfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(LinkLostDatainfo, CDialog)

LinkLostDatainfo::LinkLostDatainfo(CWnd* pParent /*=NULL*/)
	: CDialog(LinkLostDatainfo::IDD, pParent)
{
	CountLinkLost[LINKLOST_ITEM] = 0;
	CountLinkLost[LINKLOST_SPECID] = 0;
	CountLinkLost[LINKLOST_GENITEMFILE] = 0;
	CountLinkLost[LINKLOST_CROW] = 0;
}

LinkLostDatainfo::~LinkLostDatainfo()
{	
	 unsigned int _Size = m_LinkLostData.size();
	 for(unsigned int _index = 0; _index < _Size; ++_index)
	 {
		delete m_LinkLostData[_index];
	 }	
}

void LinkLostDatainfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LINKLOST_ITEM, m_ListLinkLostItem);
	DDX_Control(pDX, IDC_LIST_LINKLOST_SPECID, m_ListLinkLostSpecID);
	DDX_Control(pDX, IDC_LIST_LINKLOST_GENITEMFILE, m_ListLinkLostGenItemFile);
	DDX_Control(pDX, IDC_LIST_LINKLOST_CROW, m_ListLinkLostCrow);
	DDX_Control(pDX, IDC_LIST_NOUSE_SPECID, m_ListNoUseSpecID);
}


BEGIN_MESSAGE_MAP(LinkLostDatainfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LINKLOST_ITEM_FIND, &LinkLostDatainfo::OnBnClickedButtonLinklostItemFind)
	ON_BN_CLICKED(IDC_BUTTON_LINKLOST_SPECID_FIND, &LinkLostDatainfo::OnBnClickedButtonLinklostSpecidFind)
	ON_BN_CLICKED(IDC_BUTTON_LINKLOST_GENITEMFILE_FIND, &LinkLostDatainfo::OnBnClickedButtonLinklostGenitemfileFind)
	ON_BN_CLICKED(IDC_BUTTON_LINKLOST_CROW_FIND, &LinkLostDatainfo::OnBnClickedButtonLinklostCrowFind)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &LinkLostDatainfo::OnBnClickedButtonReload)
	ON_BN_CLICKED(IDC_BUTTON_LINKLOST_CSV, &LinkLostDatainfo::OnBnClickedButtonLinklostCsv)
END_MESSAGE_MAP()


// LinkLostDatainfo 메시지 처리기입니다.


BOOL LinkLostDatainfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListLinkLostItem.SetExtendedStyle(m_ListLinkLostItem.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListLinkLostSpecID.SetExtendedStyle(m_ListLinkLostSpecID.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListLinkLostGenItemFile.SetExtendedStyle(m_ListLinkLostGenItemFile.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListLinkLostCrow.SetExtendedStyle(m_ListLinkLostCrow.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListNoUseSpecID.SetExtendedStyle(m_ListNoUseSpecID.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	char* szNameColumnItem[4] =
	{
		"Message", "Item Name", "Item Mid", "Item Sid"		
	};

	char* szNameColumnSpecID[4] =
	{
		"Message", "SpecID", "ItemName", "Item(Mid/Sid)"
	};

	char* szNameColumnGenItemFile[2] =
	{
		"Message", "GenFileName"
	};

	char* szNameColumnCrow[4] =
	{
		"Message", "CrowName", "Crow Mid", "Crow Sid"
	};

	const int WidthRateItCr[4] =
	{
		40, 30, 13, 13
	};

	const int WidthRateSpecId[4] =
	{
		40, 10, 30, 20
	};

	const int WidthRateGenItemFile[2] =
	{
		60, 40
	};

	RECT rectCtrl;
	m_ListLinkLostItem.GetClientRect(&rectCtrl);
	int InWidth(rectCtrl.right - rectCtrl.left);

	for(unsigned int _index(0); _index < 4; ++_index)
	{
	LV_COLUMN lvComlunm;
	lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
	lvComlunm.fmt = LVCFMT_CENTER;
	lvComlunm.pszText = szNameColumnItem[_index];
	lvComlunm.iSubItem = -1;
	lvComlunm.cx = (InWidth * WidthRateItCr[_index])/ 100;
	m_ListLinkLostItem.InsertColumn(_index, &lvComlunm);
	}

	m_ListLinkLostSpecID.GetClientRect(&rectCtrl);
	InWidth = rectCtrl.right - rectCtrl.left;
	for(unsigned int _index(0); _index < 4; ++_index)
	{
	LV_COLUMN lvComlunm;
	lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
	lvComlunm.fmt = LVCFMT_CENTER;
	lvComlunm.pszText = szNameColumnSpecID[_index];
	lvComlunm.iSubItem = -1;
	lvComlunm.cx = (InWidth * WidthRateSpecId[_index])/ 100;
	m_ListLinkLostSpecID.InsertColumn(_index, &lvComlunm);
	}

	m_ListLinkLostGenItemFile.GetClientRect(&rectCtrl);
	InWidth = rectCtrl.right - rectCtrl.left;
	for(unsigned int _index(0); _index < 2; ++_index)
	{
	LV_COLUMN lvComlunm;
	lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
	lvComlunm.fmt = LVCFMT_CENTER;
	lvComlunm.pszText = szNameColumnGenItemFile[_index];
	lvComlunm.iSubItem = -1;
	lvComlunm.cx = (InWidth * WidthRateGenItemFile[_index])/ 100;
	m_ListLinkLostGenItemFile.InsertColumn(_index, &lvComlunm);
	}

	m_ListLinkLostCrow.GetClientRect(&rectCtrl);
	InWidth = rectCtrl.right - rectCtrl.left;
	for(unsigned int _index(0); _index < 4; ++_index)
	{
	LV_COLUMN lvComlunm;
	lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
	lvComlunm.fmt = LVCFMT_CENTER;
	lvComlunm.pszText = szNameColumnCrow[_index];
	lvComlunm.iSubItem = -1;
	lvComlunm.cx = (InWidth * WidthRateItCr[_index])/ 100;
	m_ListLinkLostCrow.InsertColumn(_index, &lvComlunm);
	}

	m_ListNoUseSpecID.GetClientRect(&rectCtrl);
	char* _SpecID = "SpecID";
	LV_COLUMN lvComlunm;
	lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
	lvComlunm.fmt = LVCFMT_CENTER;
	lvComlunm.pszText = _SpecID;
	lvComlunm.iSubItem = -1;
	lvComlunm.cx = rectCtrl.right;
	m_ListNoUseSpecID.InsertColumn(0, &lvComlunm);

	UpDateItems();	
	return true;
}
//Column 값들의 초기 세팅, 현재 enum값이 아니라 지정된 숫자로 for문을 돌림

void LinkLostDatainfo::UpDateItems()
{
		m_ListLinkLostItem.DeleteAllItems();
		m_ListLinkLostSpecID.DeleteAllItems();
		m_ListLinkLostGenItemFile.DeleteAllItems();
		m_ListLinkLostCrow.DeleteAllItems();
		m_ListNoUseSpecID.DeleteAllItems();
		
		GenItem::GenItemManager::GetInstance().FindLinkLostData(m_LinkLostData);
		SetCtrlListItemCase(m_LinkLostData);			
		
		GenItem::GenItemManager::GetInstance().DeleverNoUseSpecID(m_NoUseSpecID);
		SetCtrlNoUseSpecID(m_NoUseSpecID);
}
//데이터 변경이 일어날때마다 column의 item값들을 변경할 수 있도록 추가한 함수
//LinkLost데이터를 출력하는 Dialog의 item들을 모두 갱신해서 출력하도록 한다

void LinkLostDatainfo::SetCtrlNoUseSpecID(const std::vector<int> &_vecNoUse)
{
	unsigned int _Size = _vecNoUse.size();
	CString Strtemp;
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		Strtemp.Format(_T("%d"), _vecNoUse[_index]);
		m_ListNoUseSpecID.InsertItem(0, Strtemp);
	}
}
//NoUseSpecID의 아이템을 갱신해서 출력하는 함수

void LinkLostDatainfo::SetCtrlListItemCase(const std::vector<GenItem::LinkLostData*> &_Data)
{
	unsigned int _Size = _Data.size();

	CountLinkLost[LINKLOST_ITEM] = 0;
	CountLinkLost[LINKLOST_SPECID] = 0;
	CountLinkLost[LINKLOST_GENITEMFILE] = 0;
	CountLinkLost[LINKLOST_CROW] = 0;

	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		switch(_Data[_index]->m_Type)
		{
			case GenItem::LinkLostData::ITEM_DATA:
				{					
					CString Strtemp;
					int InsertIndex = m_ListLinkLostItem.InsertItem(CountLinkLost[LINKLOST_ITEM], _Data[_index]->m_StateMessage.c_str());
					m_ListLinkLostItem.SetItem(InsertIndex, 1, LVIF_TEXT, _Data[_index]->m_ItemName.c_str(), 0, 0, 0, 0);
					Strtemp.Format(_T("%d"), (int)_Data[_index]->m_ItemID.wMainID);
					m_ListLinkLostItem.SetItem(InsertIndex, 2, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
					Strtemp.Format(_T("%d"), (int)_Data[_index]->m_ItemID.wSubID);
					m_ListLinkLostItem.SetItem(InsertIndex, 3, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

					CountLinkLost[LINKLOST_ITEM]++;
					break;
				}

			case GenItem::LinkLostData::SPECID_DATA:
				{
					CString Strtemp;
					int InsertIndex = m_ListLinkLostSpecID.InsertItem(CountLinkLost[LINKLOST_SPECID], _Data[_index]->m_StateMessage.c_str());
					Strtemp.Format(_T("%d"), _Data[_index]->m_SpecID);
					m_ListLinkLostSpecID.SetItem(InsertIndex, 1, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
					m_ListLinkLostSpecID.SetItem(InsertIndex, 2, LVIF_TEXT, _Data[_index]->m_ItemName.c_str(), 0, 0, 0, 0);
					Strtemp.Format(_T("%d / %d"), _Data[_index]->m_ItemID.wMainID, _Data[_index]->m_ItemID.wSubID);
					m_ListLinkLostSpecID.SetItem(InsertIndex, 3, LVIF_TEXT, Strtemp, 0, 0, 0, 0);		

					CountLinkLost[LINKLOST_SPECID]++;
					break;
				}

			case GenItem::LinkLostData::GENITEMFILE_DATA:
				{					
					int InsertIndex = m_ListLinkLostGenItemFile.InsertItem(CountLinkLost[LINKLOST_GENITEMFILE], _Data[_index]->m_StateMessage.c_str());
					m_ListLinkLostGenItemFile.SetItem(InsertIndex, 1, LVIF_TEXT, _Data[_index]->m_GenItemFileName.c_str(), 0, 0, 0, 0);

					CountLinkLost[LINKLOST_GENITEMFILE]++;
					break;
				}
				
			case GenItem::LinkLostData::CROW_DATA:
				{
					CString Strtemp;
					int InsertIndex = m_ListLinkLostCrow.InsertItem(CountLinkLost[LINKLOST_CROW], _Data[_index]->m_StateMessage.c_str());
					m_ListLinkLostCrow.SetItem(InsertIndex, 1, LVIF_TEXT, _Data[_index]->m_CrowName.c_str(), 0, 0, 0, 0);
					Strtemp.Format(_T("%d"), (int)_Data[_index]->m_CrowID.wMainID);
					m_ListLinkLostCrow.SetItem(InsertIndex, 2, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
					Strtemp.Format(_T("%d"), (int)_Data[_index]->m_CrowID.wSubID);
					m_ListLinkLostCrow.SetItem(InsertIndex, 3, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
					
					CountLinkLost[LINKLOST_CROW]++;
					break;
				}
		}
	}
}
//NoUseSpecID를 제외한 네종류의 데이터들의 아이템 갱신

int LinkLostDatainfo::FindIndexFromData(const std::vector<GenItem::LinkLostData*> &_LinkLostData, 
										LinkLostCount _Case, const GenItem::LinkLostData &_Source)
{
	switch(_Case)
	{
	case LINKLOST_ITEM:
		{
			if(_Source.m_ItemID.wMainID != 65535 && _Source.m_ItemID.wSubID != 65535)
			{
				unsigned int _Size = _LinkLostData.size();
				for(unsigned int _index = 0; _index < _Size; ++_index)
				{
					  if(_LinkLostData[_index]->m_ItemID == _Source.m_ItemID)
						  return _index;
				}
			}
			else if(_Source.m_ItemName != "" && _Source.m_ItemID.wMainID == 65535 && _Source.m_ItemID.wSubID == 65535)
			{
				unsigned int _Size = _LinkLostData.size();
				for(unsigned int _index = 0; _index < _Size; ++_index)
				{
					if(_LinkLostData[_index]->m_ItemName == _Source.m_ItemName)
						return _index;
				}
			}
			else
				return -1;

			return -1;
		}

	case LINKLOST_SPECID:
		{
			if(_Source.m_SpecID != NULL)
			{
				unsigned int _Size = _LinkLostData.size();
				for(unsigned int _index = 0; _index < _Size; ++_index)
				{
					if(_LinkLostData[_index]->m_SpecID == _Source.m_SpecID)
						return _index;
				}
			}
			else
				return -1;

			return -1;
		}

	case LINKLOST_GENITEMFILE:
		{
			if(_Source.m_GenItemFileName != "")
			{
				unsigned int _Size = _LinkLostData.size();
				for(unsigned int _index = 0; _index < _Size; ++_index)
				{
					if(_LinkLostData[_index]->m_GenItemFileName == _Source.m_GenItemFileName)
						return _index;
				}
			}
			else
				return -1;

			return -1;
		}

	case LINKLOST_CROW:
		{
			if(_Source.m_CrowID.wMainID != 65535 && _Source.m_CrowID.wSubID != 65535)
			{
				unsigned int _Size = _LinkLostData.size();
				for(unsigned int _index = 0; _index < _Size; ++_index)
				{
					if(_LinkLostData[_index]->m_CrowID == _Source.m_CrowID)
						return _index;
				}
			}
			else if(_Source.m_CrowName != "" && _Source.m_CrowID.wMainID == 65535 && _Source.m_CrowID.wSubID == 65535)
			{
				unsigned int _Size = _LinkLostData.size();
				for(unsigned int _index = 0; _index < _Size; ++_index)
				{
					if(_LinkLostData[_index]->m_CrowName == _Source.m_CrowName)
						return _index;
				}
			}
			else
				return -1;

			return -1;
		}
	}
}
//LinkLost출력용 Dialog에서 검색할 때 이용함, 해당 아이템의 인덱스를 반환함

void LinkLostDatainfo::OnBnClickedButtonLinklostItemFind()
{
	GenItem::LinkLostData _Datatemp;

	std::string _ID;
	_ID = GetWin_Text(this, IDC_EDIT_LINKLOST_ITEM_MID);
	if(_ID != "")
		_Datatemp.m_ItemID.wMainID = atoi(_ID.c_str());
	else
		_Datatemp.m_ItemID.wMainID = 65535;

	_ID = GetWin_Text(this, IDC_EDIT_LINKLOST_ITEM_SID);
	if(_ID != "")
		_Datatemp.m_ItemID.wSubID = atoi(_ID.c_str());
	else
		_Datatemp.m_ItemID.wSubID = 65535;
	
	_Datatemp.m_ItemName = GetWin_Text(this, IDC_EDIT_LINKLOST_ITEMNAME);
	
	int _index = FindIndexFromData(m_LinkLostData, LINKLOST_ITEM, _Datatemp);

	if(_index == -1) 
	 MessageBox("You Can't find it", "Find", MB_OK);

	else
	{	
		POSITION pos = m_ListLinkLostItem.GetFirstSelectedItemPosition();
		int Item = m_ListLinkLostItem.GetNextSelectedItem(pos);
		if(Item >=0)
			m_ListLinkLostItem.SetItemState(Item, 0, LVIS_SELECTED | LVIS_FOCUSED);

		m_ListLinkLostItem.SetSelectionMark(_index);
		m_ListLinkLostItem.SetItemState(_index, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
		m_ListLinkLostItem.EnsureVisible(_index, FALSE);
		m_ListLinkLostItem.SetFocus();
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void LinkLostDatainfo::OnBnClickedButtonLinklostSpecidFind()
{
	GenItem::LinkLostData _Datatemp;

	_Datatemp.m_SpecID = GetWin_Num_int(this, IDC_EDIT_LINKLOST_SPECID);	

	int _index = FindIndexFromData(m_LinkLostData, LINKLOST_SPECID, _Datatemp);

	if(_index == -1) 
	 MessageBox("You Can't find it", "Find", MB_OK); 

	else
	{	
		for(int _Num = 0; _Num < LINKLOST_SPECID; ++_Num)
		{
			_index -= CountLinkLost[_Num];
		}	

		POSITION pos = m_ListLinkLostSpecID.GetFirstSelectedItemPosition();
		int Item = m_ListLinkLostSpecID.GetNextSelectedItem(pos);
		if(Item >=0)
			m_ListLinkLostSpecID.SetItemState(Item, 0, LVIS_SELECTED | LVIS_FOCUSED);

		m_ListLinkLostSpecID.SetSelectionMark(_index);
		m_ListLinkLostSpecID.SetItemState(_index, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
		m_ListLinkLostSpecID.EnsureVisible(_index, FALSE);
		m_ListLinkLostSpecID.SetFocus();	
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void LinkLostDatainfo::OnBnClickedButtonLinklostGenitemfileFind()
{
	GenItem::LinkLostData _Datatemp;

	_Datatemp.m_GenItemFileName = GetWin_Text(this, IDC_EDIT_LINKLOST_GENITEMFILENAME);

	if(_Datatemp.m_GenItemFileName != "" && sc::string::FindWord(_Datatemp.m_GenItemFileName, ".genitem") == -1)
		_Datatemp.m_GenItemFileName.append(".genitem");

	int _index = FindIndexFromData(m_LinkLostData, LINKLOST_GENITEMFILE, _Datatemp);

	if(_index == -1) 
	{ MessageBox("You Can't find it", "Find", MB_OK); return; }

	else
	{
		for(int _Num = 0; _Num < LINKLOST_GENITEMFILE; ++_Num)
		{
			_index -= CountLinkLost[_Num];
		}
	
		POSITION pos = m_ListLinkLostGenItemFile.GetFirstSelectedItemPosition();
		int Item = m_ListLinkLostGenItemFile.GetNextSelectedItem(pos);
		if(Item >=0)
			m_ListLinkLostGenItemFile.SetItemState(Item, 0, LVIS_SELECTED | LVIS_FOCUSED);

		m_ListLinkLostGenItemFile.SetSelectionMark(_index);
		m_ListLinkLostGenItemFile.SetItemState(_index, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
		m_ListLinkLostGenItemFile.EnsureVisible(_index, FALSE);
		m_ListLinkLostGenItemFile.SetFocus();		
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void LinkLostDatainfo::OnBnClickedButtonLinklostCrowFind()
{
	GenItem::LinkLostData _Datatemp;

	_Datatemp.m_CrowID.wMainID = GetWin_Num_int(this, IDC_EDIT_LINKLOST_CROW_MID);
	_Datatemp.m_CrowID.wSubID = GetWin_Num_int(this, IDC_EDIT_LINKLOST_CROW_SID);
	_Datatemp.m_CrowName = GetWin_Text(this, IDC_EDIT_LINKLOST_CROWNAME);

	int _index = FindIndexFromData(m_LinkLostData, LINKLOST_CROW, _Datatemp);

	if(_index == -1) 
	 MessageBox("You Can't find it", "Find", MB_OK); 

	else
	{	
		for(int _Num = 0; _Num < LINKLOST_CROW; ++_Num)
		{
			_index -= CountLinkLost[_Num];
		}

		POSITION pos = m_ListLinkLostCrow.GetFirstSelectedItemPosition();
		int Item = m_ListLinkLostCrow.GetNextSelectedItem(pos);
		if(Item >=0)
			m_ListLinkLostCrow.SetItemState(Item, 0, LVIS_SELECTED | LVIS_FOCUSED);

		m_ListLinkLostCrow.SetSelectionMark(_index);
		m_ListLinkLostCrow.SetItemState(_index, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
		m_ListLinkLostCrow.EnsureVisible(_index, FALSE);
		m_ListLinkLostCrow.SetFocus();			
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
//find버튼의 이벤트를 통한 처리르 하는 함수들인데 일치하는 데이터의 아이템 인덱스를 반환 받으면
//해당 아이템의 인덱스를 찾아서 커서(파란색의 선택형태)를 이동시키고 선택을 진행시켜서 보여줌

void LinkLostDatainfo::OnBnClickedButtonReload()
{
	UpDateItems();// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

HRESULT LinkLostDatainfo::SaveExcelFile(CWnd* _pWnd)
{
	GenItem::GenItemManager::GetInstance().SaveExcelFile(_pWnd, m_LinkLostData, m_NoUseSpecID);
	MessageBox("Save Complete", "Excel Export", MB_OK);
	return S_OK;	
}
//엑셀파일 저장용 함수

void LinkLostDatainfo::OnBnClickedButtonLinklostCsv()
{
	SaveExcelFile(this);	//CSV -> Excel로 변경됨
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
