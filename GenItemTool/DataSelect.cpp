// DataSelect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GenItemTool.h"
#include "DataSelect.h"
#include "../RanLogic/GenItemTable/GenItemManager.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"


// DataSelect ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DataSelect, CDialog)

DataSelect::DataSelect(CWnd* pParent /*=NULL*/)
: CDialog(DataSelect::IDD, pParent), m_FindCase(GenItem::NONE_NAME_WORD),m_CountSelectLine(0),
 m_CompleteWord("")
{

}

DataSelect::~DataSelect()
{
}

void DataSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FINDWORD, m_ListFindWord);
	DDX_Control(pDX, IDC_LIST_SELECT_DATA, m_ListSelectData);
}


BEGIN_MESSAGE_MAP(DataSelect, CDialog)
	ON_BN_CLICKED(ID_BUTTON_SELECT, &DataSelect::OnBnClickedButtonSelect)
	ON_BN_CLICKED(ID_BUTTON_DELETE, &DataSelect::OnBnClickedButtonDelete)
	ON_BN_CLICKED(ID_BUTTON_CLEAR, &DataSelect::OnBnClickedButtonClear)
	ON_EN_CHANGE(IDC_EDIT_INPUT_NAME, &DataSelect::OnEnChangeEditInputName)
	ON_BN_CLICKED(ID_BUTTON_COMPLETE, &DataSelect::OnBnClickedButtonComplete)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FINDWORD, &DataSelect::OnNMDblclkListFindword)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELECT_DATA, &DataSelect::OnNMDblclkListSelectData)
END_MESSAGE_MAP()


BOOL DataSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListFindWord.SetExtendedStyle(m_ListFindWord.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListSelectData.SetExtendedStyle(m_ListSelectData.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	char* szNameColumn[2] =
	{
		"Name", "MID / SID"
	};

	const int WidthRate[2] =
	{
		100, 0//65, 35
	};

	RECT rectCtrl;
	m_ListFindWord.GetClientRect(&rectCtrl);
	int InWidth(rectCtrl.right - rectCtrl.left);
	for(unsigned int _index(0); _index < 1; ++_index)
	{
		LV_COLUMN lvComlunm;
		lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvComlunm.fmt = LVCFMT_CENTER;
		lvComlunm.pszText = szNameColumn[_index];
		lvComlunm.iSubItem = -1;
		lvComlunm.cx = (InWidth * WidthRate[_index])/ 100;
		m_ListFindWord.InsertColumn(_index, &lvComlunm);
	}

	m_ListSelectData.GetClientRect(&rectCtrl);
	InWidth = rectCtrl.right - rectCtrl.left;
	for(unsigned int _index(0); _index < 1; ++_index)
	{
		LV_COLUMN lvComlunm;
		lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvComlunm.fmt = LVCFMT_CENTER;
		lvComlunm.pszText = szNameColumn[_index];
		lvComlunm.iSubItem = -1;
		lvComlunm.cx = (InWidth * WidthRate[_index])/ 100;
		m_ListSelectData.InsertColumn(_index, &lvComlunm);
	}

	if(m_CompleteWord != "")
	{
		std::string _Origin = m_CompleteWord;
		char* _strTemp = const_cast<char*>(_Origin.c_str());
		char* _Name(NULL);
		_Name = strtok(_strTemp, ";");

		for(; _Name != NULL; _Name = strtok(NULL, ";"))
		{	
			if(_Name != NULL)
			{
				std::string _temp = _Name;
				m_VecSelectName.push_back(_temp);
			}
		}
	}

	unsigned int _NameSize = m_VecSelectName.size();
	for(unsigned int _index = 0; _index < _NameSize; ++_index)
	{
		m_ListSelectData.InsertItem(_index, m_VecSelectName[_index].c_str());
	}

	return true;
}

// DataSelect �޽��� ó�����Դϴ�.

void DataSelect::OnBnClickedButtonSelect()
{
	POSITION _pos = m_ListFindWord.GetFirstSelectedItemPosition();	
	
	for(int _SetItem = m_ListFindWord.GetNextSelectedItem(_pos) ;_SetItem != -1; )
	{		
		std::string _SelectName = m_ListFindWord.GetItemText(_SetItem, 0);

		if(!(_SelectName == ""))
		{
			m_ListSelectData.InsertItem(m_CountSelectLine, _SelectName.c_str(), 0);
			m_VecSelectName.push_back(_SelectName);
			++m_CountSelectLine;			
		}

		_SetItem = m_ListFindWord.GetNextSelectedItem(_pos);
	}

	m_CountSelectLine = 0;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void DataSelect::OnBnClickedButtonDelete()
{
	POSITION _pos = m_ListSelectData.GetFirstSelectedItemPosition();	

	for(int _SetItem = m_ListSelectData.GetNextSelectedItem(_pos); _SetItem != -1; )
	{
		std::string _SelectName = m_ListSelectData.GetItemText(_SetItem, 0);		
		
		if(_SelectName != "")
		{
			std::vector<std::string> tempobj;
			tempobj.swap(m_VecSelectName);
			unsigned int _Size = tempobj.size();
			for(unsigned int _index = 0 ; _index < _Size ; ++_index)
			{					
				if(tempobj[_index] != _SelectName)		
					m_VecSelectName.push_back(tempobj[_index]);
				else
					continue;
			}
		}

		_SetItem = m_ListSelectData.GetNextSelectedItem(_pos);
	}

	m_ListSelectData.DeleteAllItems();
	unsigned int _Size = m_VecSelectName.size();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{	
		m_ListSelectData.InsertItem(m_CountSelectLine, m_VecSelectName[_index].c_str(), 0);
		++m_CountSelectLine;
	}

	m_CountSelectLine = 0;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void DataSelect::OnBnClickedButtonClear()
{
	m_ListSelectData.DeleteAllItems();
	m_VecSelectName.clear();
	std::vector<std::string> tempobj;
	tempobj.swap(m_VecSelectName);

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void DataSelect::OnEnChangeEditInputName()
{
	UpDateItems();
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void DataSelect::OnBnClickedButtonComplete()
{
	unsigned int _Size = m_VecSelectName.size();
	m_CompleteWord.clear();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		m_CompleteWord.append(m_VecSelectName[_index]);
		m_CompleteWord.append(_T(";"));
	}
	
	SendMessage(WM_CLOSE, 0, 0);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void DataSelect::UpDateItems()
{
	m_VecDataName.clear();
	std::vector<std::string> tempobj;
	tempobj.swap(m_VecDataName);

	m_ListFindWord.DeleteAllItems();
	std::string _Name = GetWin_Text(this, IDC_EDIT_INPUT_NAME);

	if(_Name != "")
		GenItem::GenItemManager::GetInstance().FindDatafromWord(m_VecDataName, _Name, m_FindCase);

	unsigned int _Size = m_VecDataName.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		int InsertIndex =  m_ListFindWord.InsertItem(_index, m_VecDataName[_index].c_str());
	}	
}

void DataSelect::OnNMDblclkListFindword(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION _pos = m_ListFindWord.GetFirstSelectedItemPosition();		
	int _SetItem = m_ListFindWord.GetNextSelectedItem(_pos);

	if(_SetItem == -1) return;
		
	std::string _SelectName = m_ListFindWord.GetItemText(_SetItem, 0);

	if(_SelectName != "")
	{
		m_ListSelectData.InsertItem(m_CountSelectLine, _SelectName.c_str(), 0);
		m_VecSelectName.push_back(_SelectName);
		++m_CountSelectLine;			
	}	

	m_CountSelectLine = 0;
	*pResult = 0;
}

void DataSelect::OnNMDblclkListSelectData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION _pos = m_ListSelectData.GetFirstSelectedItemPosition();		
	int _SetItem = m_ListSelectData.GetNextSelectedItem(_pos);
	if(_SetItem == -1) return;
	
	std::string _SelectName = m_ListSelectData.GetItemText(_SetItem, 0);		

	if(_SelectName != "")
	{
		std::vector<std::string> tempobj;
		tempobj.swap(m_VecSelectName);
		unsigned int _Size = tempobj.size();
		for(unsigned int _index = 0 ; _index < _Size ; ++_index)
		{					
			if(tempobj[_index] != _SelectName)		
				m_VecSelectName.push_back(tempobj[_index]);
			else
				continue;
		}
	}	

	m_ListSelectData.DeleteAllItems();
	unsigned int _Size = m_VecSelectName.size();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{	
		m_ListSelectData.InsertItem(m_CountSelectLine, m_VecSelectName[_index].c_str(), 0);
		++m_CountSelectLine;
	}

	m_CountSelectLine = 0;
	*pResult = 0;
}
