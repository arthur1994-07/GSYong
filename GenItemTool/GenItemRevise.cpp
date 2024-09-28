// GenItemRevise.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GenItemTool.h"
#include "GenItemRevise.h"
#include "./DataSelect.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"


// GenItemRevise 대화 상자입니다.

IMPLEMENT_DYNAMIC(GenItemRevise, CDialog)

GenItemRevise::GenItemRevise(CWnd* pParent /*=NULL*/)
	: CDialog(GenItemRevise::IDD, pParent), m_FileName(), m_CheckSkillFact(false)

{

}

GenItemRevise::~GenItemRevise()
{
	m_EditData.erase();
}

void GenItemRevise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO_GENITEMFILE, m_ListGenItemFileData);
	DDX_Control(pDX, IDC_LIST_SPECID_ITEM, m_ListSpecIDItem);
	DDX_Control(pDX, IDC_LIST_INFO_GENITEMFILE_ANNOTATION, m_Listannotation);
}

BEGIN_MESSAGE_MAP(GenItemRevise, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_INFO_GENITEMFILE, &GenItemRevise::OnNMDblclkListInfoGenitemfile)
	ON_BN_CLICKED(IDC_BUTTON_REVISE_INPUT_GENRATE, &GenItemRevise::OnBnClickedButtonReviseInputGenrate)
	ON_BN_CLICKED(IDC_BUTTON_REVISE_INPUT_GENNUM, &GenItemRevise::OnBnClickedButtonReviseInputGennum)
	ON_BN_CLICKED(IDC_BUTTON_REVISE_INPUT_GENDATA, &GenItemRevise::OnBnClickedButtonReviseInputGendata)

	ON_BN_CLICKED(IDC_CHECK_REVISE_SKILLFACT, &GenItemRevise::OnBnClickedCheckReviseSkillfact)
	ON_BN_CLICKED(ID_REVISE_DELETE, &GenItemRevise::OnBnClickedReviseDelete)
	ON_BN_CLICKED(ID_REVISE_CLEAR, &GenItemRevise::OnBnClickedReviseClear)
	ON_BN_CLICKED(ID_REVISE_SAVE, &GenItemRevise::OnBnClickedReviseSave)
	ON_BN_CLICKED(IDC_BUTTON_REVISE_FIND, &GenItemRevise::OnBnClickedButtonReviseFind)
	ON_BN_CLICKED(IDC_BUTTON_REVISE_SEARCH_ITEM, &GenItemRevise::OnBnClickedButtonReviseSearchItem)		
	ON_EN_CHANGE(IDC_EDIT_SPECID_SINGLE, &GenItemRevise::OnEnChangeEditSpecidSingle)

	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_INFO_GENITEMFILE, &GenItemRevise::OnNMCustomdrawListInfoGenitemfile)
	ON_BN_CLICKED(IDC_BUTTON_REVISE_INPUT_ANNOTATION, &GenItemRevise::OnBnClickedButtonReviseInputAnnotation)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_INFO_GENITEMFILE_ANNOTATION, &GenItemRevise::OnNMDblclkListInfoGenitemfileAnnotation)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_INFO_GENITEMFILE_ANNOTATION, &GenItemRevise::OnNMCustomdrawListInfoGenitemfileAnnotation)
	ON_NOTIFY(NM_CLICK, IDC_LIST_INFO_GENITEMFILE, &GenItemRevise::OnNMClickListInfoGenitemfile)
	ON_NOTIFY(NM_CLICK, IDC_LIST_INFO_GENITEMFILE_ANNOTATION, &GenItemRevise::OnNMClickListInfoGenitemfileAnnotation)
END_MESSAGE_MAP()

BOOL GenItemRevise::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListGenItemFileData.SetExtendedStyle(m_ListGenItemFileData.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_ListSpecIDItem.SetExtendedStyle(m_ListSpecIDItem.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_Listannotation.SetExtendedStyle(m_Listannotation.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	char* szNameColumnGenFile[3] = 
	{
		"Type", "Value", "Rate"
	};

	char* szNameColumnSpecID[2] = 
	{
		"Item Name", "Item ID"
	};

	char* szNameColumnAnnotation[3] = 
	{
		"Type", "SpecID", "Annotation"
	};

	const int WidthRateColumnGenFile[3] = 
	{
		30, 40, 30
	};

	const int WidthRateColumnSpecID[2] =
	{
		70, 30
	};

	const int WidthRateColumnAnnotation[3] = 
	{
		15, 15, 70
	};
	
	RECT rectCtrl;
	m_ListGenItemFileData.GetClientRect(&rectCtrl);

	const long InWidth(rectCtrl.right - rectCtrl.left);
	for(unsigned int _index(0); _index < 3; ++_index)
	{
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szNameColumnGenFile[_index];
		lvColumn.iSubItem = -1;
		lvColumn.cx = (InWidth * WidthRateColumnGenFile[_index] ) / 100;
		m_ListGenItemFileData.InsertColumn(_index, &lvColumn);
	}

	m_ListSpecIDItem.GetClientRect(&rectCtrl);

	const long Inwidth(rectCtrl.right - rectCtrl.left);
	for(unsigned int _index(0); _index < 2; ++_index)
	{
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szNameColumnSpecID[_index];
		lvColumn.iSubItem = -1;
		lvColumn.cx = (Inwidth * WidthRateColumnSpecID[_index] ) / 100;
		m_ListSpecIDItem.InsertColumn(_index, &lvColumn);
	}

	m_Listannotation.GetClientRect(&rectCtrl);
	const long Wid(rectCtrl.right - rectCtrl.left);
	for(unsigned int _index(0); _index < 3; ++_index)
	{
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szNameColumnAnnotation[_index];
		lvColumn.iSubItem = -1;
		lvColumn.cx = (InWidth * WidthRateColumnAnnotation[_index] ) / 100;
		m_Listannotation.InsertColumn(_index, &lvColumn);
	}

	GetDlgItem(IDC_STATIC_REVISE_FILENAME)->SetWindowText(m_FileName.c_str());
	GetDataOriginal();
	UpdateGenDataItems();
	UpdateAnnotation();
	DlgUseCheck();
	return TRUE;
}

void GenItemRevise::UpdateGenDataItems()
{
	m_ListGenItemFileData.DeleteAllItems();
	CString _strtemp;
	int Count(0);

	if(m_FileName == "") return;

	int InsertIndex = m_ListGenItemFileData.InsertItem(Count, _T("FileRate"));	
	++Count;

	if(m_EditData.m_FileGenRate.m_StrName != "")
	{	
		InsertIndex = m_ListGenItemFileData.InsertItem(Count, m_EditData.m_FileGenRate.m_StrName.c_str());
		_strtemp.Format(_T("%G"), m_EditData.m_FileGenRate.m_Rate);
		m_ListGenItemFileData.SetItem(InsertIndex, 1, LVIF_TEXT, _strtemp, 0, 0, 0, 0 );
		++Count;
	}

	InsertIndex = m_ListGenItemFileData.InsertItem(Count, _T("Generate Number"));	
	m_ListGenItemFileData.SetItem(InsertIndex, 1, LVIF_TEXT, _T("[Max: 5]"), 0, 0, 0, 0);
	m_ListGenItemFileData.SetItem(InsertIndex, 2, LVIF_TEXT, _T("Sum = 100%"), 0, 0, 0, 0);
	++Count;

	std::list<GenItem::GenItemEditData::NumRate*>::iterator _iterBegin(m_EditData.m_listNumRate.begin());
	std::list<GenItem::GenItemEditData::NumRate*>::iterator _iterEnd(m_EditData.m_listNumRate.end());

	for(; _iterBegin != _iterEnd; ++_iterBegin, ++Count)
	{
		InsertIndex = m_ListGenItemFileData.InsertItem(Count, (*_iterBegin)->m_StrName.c_str());
		_strtemp.Format(_T("%d"), (*_iterBegin)->m_Num);
		m_ListGenItemFileData.SetItem(InsertIndex, 1, LVIF_TEXT, _strtemp, 0, 0, 0, 0);
		_strtemp.Format(_T("%G"), (*_iterBegin)->m_NumRate);
		m_ListGenItemFileData.SetItem(InsertIndex, 2, LVIF_TEXT, _strtemp, 0, 0, 0, 0);		
	}
	
	InsertIndex = m_ListGenItemFileData.InsertItem(Count, _T("Kind of Generate"));
	m_ListGenItemFileData.SetItem(InsertIndex, 1, LVIF_TEXT, _T("[Max: 30]"), 0, 0, 0, 0);
	m_ListGenItemFileData.SetItem(InsertIndex, 2, LVIF_TEXT, _T("Sum = 100%"), 0, 0, 0, 0);
	++Count;

	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorBegin(m_EditData.m_listSpIDnRate.begin());
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorEnd(m_EditData.m_listSpIDnRate.end());
	for(; _iteratorBegin != _iteratorEnd; ++_iteratorBegin, ++Count)
	{
		InsertIndex = m_ListGenItemFileData.InsertItem(Count, (*_iteratorBegin)->m_StrName.c_str());

		if((*_iteratorBegin)->m_Type == GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC)
		{		
			_strtemp.Format(_T("%d"), (*_iteratorBegin)->m_SpecID);
			m_ListGenItemFileData.SetItem(InsertIndex, 1, LVIF_TEXT, _strtemp, 0, 0, 0, 0);
			_strtemp.Format(_T("%G"), (*_iteratorBegin)->m_Rate);
			m_ListGenItemFileData.SetItem(InsertIndex, 2, LVIF_TEXT, _strtemp, 0, 0, 0, 0);
		}
		else if((*_iteratorBegin)->m_Type == GenItem::GenItemEditData::SPIDNRATE_SKILLFACT)
		{
			_strtemp.Format(_T("%d (%d / %d)"), (*_iteratorBegin)->m_SpecID, 
				(*_iteratorBegin)->m_SkillID.wMainID, (*_iteratorBegin)->m_SkillID.wSubID);
			m_ListGenItemFileData.SetItem(InsertIndex, 1, LVIF_TEXT, _strtemp, 0, 0, 0, 0);
			_strtemp.Format(_T("%G"), (*_iteratorBegin)->m_Rate);
			m_ListGenItemFileData.SetItem(InsertIndex, 2, LVIF_TEXT, _strtemp, 0, 0, 0, 0);
		}
		else
			continue;
	}	
}

void GenItemRevise::UpdateItemdataItems()
{
	m_ListSpecIDItem.DeleteAllItems();
	unsigned int _Size = m_vecItInfo.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		int InsertIndex = m_ListSpecIDItem.InsertItem(_index, m_vecItInfo[_index].m_ItemName.c_str());
		CString strtmp;
		strtmp.Format(_T("%d / %d"), m_vecItInfo[_index].m_ItemID.wMainID, m_vecItInfo[_index].m_ItemID.wSubID);
		m_ListSpecIDItem.SetItem(InsertIndex, 1, LVIF_TEXT, strtmp, 0, 0, 0, 0);
	}
}

void GenItemRevise::DlgUseCheck()
{
	GetDlgItem(IDC_EDIT_REVISE_SKILLID_MID)->EnableWindow(m_CheckSkillFact);	
	GetDlgItem(IDC_EDIT_REVISE_SKILLID_SID)->EnableWindow(m_CheckSkillFact);	
}

void GenItemRevise::GetDataOriginal()
{
	if(m_FileName == "" || m_FileName == ".genitem") return;	

	if(m_FileName != "" && sc::string::FindWord(m_FileName, ".genitem") == -1)
		m_FileName.append(".genitem");

	GenItem::GenItemManager::GetInstance().FindGenItemFileData(m_FileName, m_EditData);
}
// GenItemRevise 메시지 처리기입니다.

void GenItemRevise::FindClickData(GenItem::GenItemEditData::GenFileEditType &_dataType, int &_Line, int _SetItem)
{
	int _tempLine = _SetItem - 1; //임의로 넣어준 값들은 빼줌 3개
	int _SizeNumRate = m_EditData.m_listNumRate.size();
	int _SizeSpIDnRate = m_EditData.m_listSpIDnRate.size();

	if(_SetItem == 0 || _SetItem == 2 || _SetItem == 3 + _SizeNumRate) 
	{
		_dataType = GenItem::GenItemEditData::NONE;
		_Line = -1;
		return;
	}

	if(_tempLine == 0)
	{
		_dataType = GenItem::GenItemEditData::FILEGENRATE;
		_Line = 1;
	}
	else if(_tempLine - 1 <= _SizeNumRate + 1)
	{
		_Line = _tempLine - 2;
		_dataType = GenItem::GenItemEditData::NUMRATE;
				
	}
	else if(_tempLine - 2 < _SizeNumRate + _SizeSpIDnRate + 1)
	{
		_Line = _tempLine - _SizeNumRate - 3;

		std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorBegin(m_EditData.m_listSpIDnRate.begin());
		std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorEnd(m_EditData.m_listSpIDnRate.end());
		for(int Count(0); _iteratorBegin != _iteratorEnd; ++_iteratorBegin, ++Count)
		{
			if(Count == _Line)
			{
				_dataType = (*_iteratorBegin)->m_Type;
				break;
			}
		}
	}
	else
	{
		_dataType = GenItem::GenItemEditData::NONE;
		_Line = -1;
	}
}
//여기 적힌 숫자들은 칼럼에 들어가는 임의의 아이템 값이 한 항목당 한줄이라는 조건하에서 성립함
//만약 더 많은 아이템을 삽입한다면 숫자는 변경 해주어야함

GenItem::GenItemEditData::NumRate* GenItemRevise::GetNumRateFromList(int _Line)
{
	std::list<GenItem::GenItemEditData::NumRate*>::iterator _iterBegin(m_EditData.m_listNumRate.begin());
	std::list<GenItem::GenItemEditData::NumRate*>::iterator _iterEnd(m_EditData.m_listNumRate.end());

	for(int Count(0); _iterBegin != _iterEnd; ++_iterBegin, ++Count)
	{
		if(Count == _Line)
			return (*_iterBegin);
	}
}

GenItem::GenItemEditData::SpIDnRate* GenItemRevise::GetSpIDnRate(int _Line)
{
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorBegin(m_EditData.m_listSpIDnRate.begin());
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorEnd(m_EditData.m_listSpIDnRate.end());

	for(int Count(0); _iteratorBegin != _iteratorEnd; ++_iteratorBegin, ++Count)
	{
		if(Count == _Line)
			return (*_iteratorBegin);
	}
}

void GenItemRevise::OnNMDblclkListInfoGenitemfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION _pos = m_ListGenItemFileData.GetFirstSelectedItemPosition();	
	int _SetItem = m_ListGenItemFileData.GetNextSelectedItem(_pos);

	TypeNLine _TNL;
	FindClickData(_TNL.m_Type, _TNL.m_Selection, _SetItem);
			
	switch(_TNL.m_Type)
	{
		case GenItem::GenItemEditData::NUMRATE:
			{
				std::list<GenItem::GenItemEditData::NumRate*>::iterator _begin = m_EditData.m_listNumRate.begin();
				std::list<GenItem::GenItemEditData::NumRate*>::iterator _end = m_EditData.m_listNumRate.end();

				for(int count(0);_begin != _end; ++_begin, ++count)
				{
					if(count == _TNL.m_Selection)
					{
						GenItem::GenItemEditData::NumRate* _temp = *_begin;
						delete _temp;
						m_EditData.m_listNumRate.erase(_begin);
						break;
					}
				}
				m_TNL.m_Type = GenItem::GenItemEditData::NONE;
				m_TNL.m_Selection = -1;					
				break;
			}
		case GenItem::GenItemEditData::SPIDNRATE_SKILLFACT:
		case GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC:
			{
				std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _begin = m_EditData.m_listSpIDnRate.begin();
				std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _end = m_EditData.m_listSpIDnRate.end();

				for(int count(0); _begin != _end; ++_begin, ++count)
				{
					if(count == _TNL.m_Selection)
					{
						GenItem::GenItemEditData::SpIDnRate* _temp = *_begin;
						delete _temp;
						m_EditData.m_listSpIDnRate.erase(_begin);
						break;
					}
				}
				m_TNL.m_Type = GenItem::GenItemEditData::NONE;
				m_TNL.m_Selection = -1;					
				break;					
			}
		default:
			break;
	}	
	

	m_TNL.m_Selection = -1;
	m_TNL.m_Type = GenItem::GenItemEditData::NONE;
	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Input"));
	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Input"));
	UpdateGenDataItems();
	UpdateAnnotation();

	*pResult = 0;
}

void GenItemRevise::OnBnClickedButtonReviseInputGenrate()
{
	if(m_TNL.m_Selection == 1 && m_TNL.m_Type == GenItem::GenItemEditData::FILEGENRATE)
	{	
		float _Generate = GetWin_Num_float(this, IDC_EDIT_REVISE_GENRATE);
		if(_Generate < 0) return;
		m_EditData.m_FileGenRate.m_Rate = _Generate;
		UpdateGenDataItems();
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedButtonReviseInputGennum()
{
	if(m_TNL.m_Type == GenItem::GenItemEditData::NUMRATE)
	{			
		int _Num = GetWin_Num_int(this, IDC_EDIT_REVISE_GENNUM_NUM);
		float _Rate = GetWin_Num_float(this, IDC_EDIT_REVISE_GENNUM_RATE);
	
		if(_Num <= 0 || _Rate <= 0) 
		{
			MessageBox("Wrong Input Value", "Edit", MB_OK);
			return;
		}

		GenItem::GenItemEditData::NumRate* tempPoint = GetNumRateFromList(m_TNL.m_Selection);
		tempPoint->m_Num = _Num;
		tempPoint->m_NumRate = _Rate;				
		
		m_TNL.m_Type = GenItem::GenItemEditData::NONE;
		m_TNL.m_Selection = -1;	
		UpdateGenDataItems();
	}
	else
	{
		
		int _Num = GetWin_Num_int(this, IDC_EDIT_REVISE_GENNUM_NUM);
		float _Rate = GetWin_Num_float(this, IDC_EDIT_REVISE_GENNUM_RATE);

		if(_Num <= 0 || _Rate <= 0) 
		{
			MessageBox("Wrong Input Value", "Edit", MB_OK);
			return;
		}

		GenItem::GenItemEditData::NumRate* _ListNumRate = new GenItem::GenItemEditData::NumRate;
		_ListNumRate->m_Num = _Num;
		_ListNumRate->m_NumRate = _Rate;
		_ListNumRate->m_StrName = "GenNum";
		_ListNumRate->m_Type = GenItem::GenItemEditData::NUMRATE;
		m_EditData.m_listNumRate.push_back(_ListNumRate);

		m_TNL.m_Type = GenItem::GenItemEditData::NONE;
		m_TNL.m_Selection = -1;	
		UpdateGenDataItems();
	}

	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Input"));
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedButtonReviseInputGendata()
{
	if(m_TNL.m_Type == GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC && m_CheckSkillFact == false)
	{
		int _Num = GetWin_Num_int(this, IDC_EDIT_REVISE_GENDATA_SPECID);
		float _Rate = GetWin_Num_float(this, IDC_EDIT_REVISE_GENDATA_RATE);
		
		if(_Num <= 0 || _Rate < 0) 
		{
			MessageBox("Wrong Input Value", "Edit", MB_OK);
			return;
		}

		GenItem::GenItemEditData::SpIDnRate* tempPoint = GetSpIDnRate(m_TNL.m_Selection);	
		tempPoint->m_Rate = _Rate;
		tempPoint->m_SpecID = _Num;

		m_TNL.m_Type = GenItem::GenItemEditData::NONE;
		m_TNL.m_Selection = -1;	
		UpdateGenDataItems();
		UpdateAnnotation();
	}
	else if(m_TNL.m_Type != GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC && m_CheckSkillFact == false)
	{		
		int _Num = GetWin_Num_int(this, IDC_EDIT_REVISE_GENDATA_SPECID);
		float _Rate = GetWin_Num_float(this, IDC_EDIT_REVISE_GENDATA_RATE);
		
		if(_Num <= 0 || _Rate < 0) 
		{
			MessageBox("Wrong Input Value", "Edit", MB_OK);
			return;
		}

		GenItem::GenItemEditData::SpIDnRate* _ListSpIDnRate = new GenItem::GenItemEditData::SpIDnRate;
		_ListSpIDnRate->m_SpecID = _Num;
		_ListSpIDnRate->m_Rate = _Rate;		
		_ListSpIDnRate->m_Type = GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC;
		_ListSpIDnRate->m_StrName = "ITEMSPEC";
	
		m_EditData.m_listSpIDnRate.push_back(_ListSpIDnRate);

		m_TNL.m_Type = GenItem::GenItemEditData::NONE;
		m_TNL.m_Selection = -1;	
		UpdateGenDataItems();
		UpdateAnnotation();
	}
	else if(m_TNL.m_Type == GenItem::GenItemEditData::SPIDNRATE_SKILLFACT && m_CheckSkillFact == true)
	{
		int _Num = GetWin_Num_int(this, IDC_EDIT_REVISE_GENDATA_SPECID);
		float _Rate = GetWin_Num_float(this, IDC_EDIT_REVISE_GENDATA_RATE);
		int _Skill_MID = GetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_MID);
		int _Skill_SID = GetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_SID);

		if(_Num <= 0 || _Rate < 0 || _Skill_MID < 0 || _Skill_SID < 0) 
		{
			MessageBox("Wrong Input Value", "Edit", MB_OK);
			return;
		}
		
		GenItem::GenItemEditData::SpIDnRate* _ListSpIDnRate = GetSpIDnRate(m_TNL.m_Selection);
		_ListSpIDnRate->m_SpecID = _Num;
		_ListSpIDnRate->m_Rate = _Rate;
		_ListSpIDnRate->m_SkillID.wMainID = _Skill_MID;
		_ListSpIDnRate->m_SkillID.wSubID = _Skill_SID;

		m_TNL.m_Type = GenItem::GenItemEditData::NONE;
		m_TNL.m_Selection = -1;	
		UpdateGenDataItems();
		UpdateAnnotation();
	}
	else if(m_TNL.m_Type != GenItem::GenItemEditData::SPIDNRATE_SKILLFACT && m_CheckSkillFact == true)
	{
		int _Num = GetWin_Num_int(this, IDC_EDIT_REVISE_GENDATA_SPECID);
		float _Rate = GetWin_Num_float(this, IDC_EDIT_REVISE_GENDATA_RATE);
		int _Skill_MID = GetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_MID);
		int _Skill_SID = GetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_SID);

		if(_Num <= 0 || _Rate < 0 || _Skill_MID < 0 || _Skill_SID < 0) 
		{
			MessageBox("Wrong Input Value", "Edit", MB_OK);
			return;
		}

		GenItem::GenItemEditData::SpIDnRate* _ListSpIDnRate = new GenItem::GenItemEditData::SpIDnRate;
		_ListSpIDnRate->m_SpecID = _Num;
		_ListSpIDnRate->m_Rate = _Rate;		
		_ListSpIDnRate->m_SkillID.wMainID = _Skill_MID;
		_ListSpIDnRate->m_SkillID.wSubID = _Skill_SID;
		_ListSpIDnRate->m_Type = GenItem::GenItemEditData::SPIDNRATE_SKILLFACT;
		_ListSpIDnRate->m_StrName = "SKILLFACT";

		m_EditData.m_listSpIDnRate.push_back(_ListSpIDnRate);

		m_TNL.m_Type = GenItem::GenItemEditData::NONE;
		m_TNL.m_Selection = -1;	
		UpdateGenDataItems();
		UpdateAnnotation();
	}

	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Input"));
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedCheckReviseSkillfact()
{
	CheckStateSkillFact();
	DlgUseCheck();
	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Input"));
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedReviseDelete()
{
	POSITION _pos = m_ListGenItemFileData.GetFirstSelectedItemPosition();	
	int _SetItem = m_ListGenItemFileData.GetNextSelectedItem(_pos);

	std::vector<TypeNLine>_vecTNL;
	TypeNLine _TNL;

	for(;_SetItem != -1;)
	{		
		FindClickData(_TNL.m_Type, _TNL.m_Selection, _SetItem);
		_vecTNL.push_back(_TNL);
		_SetItem = m_ListGenItemFileData.GetNextSelectedItem(_pos);
	}

	int _Size = (int)_vecTNL.size();
	for(int _index(_Size - 1); _index >= 0; --_index)
	{
		if(_index < 0) continue;

		switch(_vecTNL[_index].m_Type)
		{
			case GenItem::GenItemEditData::NUMRATE:
				{
					std::list<GenItem::GenItemEditData::NumRate*>::iterator _begin = m_EditData.m_listNumRate.begin();
					std::list<GenItem::GenItemEditData::NumRate*>::iterator _end = m_EditData.m_listNumRate.end();
									
					for(int count(0);_begin != _end; ++_begin, ++count)
					{
						if(count == _vecTNL[_index].m_Selection)
						{
							GenItem::GenItemEditData::NumRate* _temp = *_begin;
							delete _temp;
							m_EditData.m_listNumRate.erase(_begin);
							break;
						}
					}
					m_TNL.m_Type = GenItem::GenItemEditData::NONE;
					m_TNL.m_Selection = -1;					
					break;
				}
			case GenItem::GenItemEditData::SPIDNRATE_SKILLFACT:
			case GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC:
				{
					std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _begin = m_EditData.m_listSpIDnRate.begin();
					std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _end = m_EditData.m_listSpIDnRate.end();
									
					for(int count(0); _begin != _end; ++_begin, ++count)
					{
						if(count == _vecTNL[_index].m_Selection)
						{
							GenItem::GenItemEditData::SpIDnRate* _temp = *_begin;
							delete _temp;
							m_EditData.m_listSpIDnRate.erase(_begin);
							break;
						}
					}
					m_TNL.m_Type = GenItem::GenItemEditData::NONE;
					m_TNL.m_Selection = -1;					
					break;					
				}
			default:
				break;
		}	
	}	

	m_TNL.m_Selection = -1;
	m_TNL.m_Type = GenItem::GenItemEditData::NONE;
	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Input"));
	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Input"));
	UpdateGenDataItems();
	UpdateAnnotation();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedReviseClear()
{
	m_EditData.erase();
	m_TNL.m_Selection = -1;
	m_TNL.m_Type = GenItem::GenItemEditData::NONE;
	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Input"));
	GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Input"));
	UpdateGenDataItems();
	UpdateAnnotation();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedReviseSave()
{
	GenItem::GenItemManager::GetInstance().SaveData(m_EditData, m_FileName);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedButtonReviseFind()
{	
	DataSelect Dlg;
	Dlg.SetCase(GenItem::ITEM_NAME_WORD);
	Dlg.SetCompleteStr(GetWin_Text(this, IDC_EDIT_REVISE_ITEMNAME).GetString());	
	Dlg.DoModal();

	std::string _CompleteWord = Dlg.GetCompleteStr();
	SetWin_Text(this, IDC_EDIT_REVISE_ITEMNAME, _CompleteWord.c_str());
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnBnClickedButtonReviseSearchItem()
{
	std::string _strtmpMid = GetWin_Text(this, IDC_EDIT_ITEMID_MID).GetString();
	std::string _strtmpSid = GetWin_Text(this, IDC_EDIT_ITEMID_SID).GetString();	
	std::string _CompleteWord = GetWin_Text(this, IDC_EDIT_REVISE_ITEMNAME).GetString();
	
	
	if(_strtmpMid != "" && _strtmpSid != "")
	{
		SNATIVEID _ID;
		char* _str = const_cast<char*>(_strtmpMid.c_str());
		_ID.wMainID = atoi(_str);
		_str = const_cast<char*>(_strtmpSid.c_str());
		_ID.wSubID = atoi(_str);

		GenItem::GenItemManager::GetInstance().GetItemInfo(m_ItemInfo, _T(""), _ID.wMainID, _ID.wSubID);
	}
	else if(_CompleteWord != "")
	{		
		char* _StrTemp = const_cast<char*>(_CompleteWord.c_str());
		char* _ItemName(NULL);
		_ItemName = strtok(_StrTemp, ";");	
		_CompleteWord = _ItemName;
		
		GenItem::GenItemManager::GetInstance().GetItemInfo(m_ItemInfo, _CompleteWord, 65535, 65535);
	}
	else
	{
		MessageBox("Wrong Input Value", "Search", MB_OK);
		return;
	}

	CString _strtmp;
	_strtmp.Format(_T("%d"), m_ItemInfo.m_SpecID);
	SetWin_Text(this, IDC_EDIT_RESULT, _strtmp);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnEnChangeEditSpecidSingle()
{
	std::string _strtmp = GetWin_Text(this, IDC_EDIT_SPECID_SINGLE).GetString();	

	std::vector<GenItem::ItemInfo> tempobj;
	tempobj.swap(m_vecItInfo);

	int _Size = _strtmp.size();
	for(int _index(0); _index < _Size; ++_index)
	{
		if(!isdigit(_strtmp[_index]))
		{
			MessageBox("Wrong Input SpecID box", "SpecID", MB_OK);
			return;
		}
	}

	char* _str = const_cast<char*>(_strtmp.c_str());
	GenItem::SpecIDInfo* _tempSpinfo;
	GenItem::GenItemManager::GetInstance().GetSpecIDInfo(_tempSpinfo, atoi(_str));

	_Size = _tempSpinfo->m_ItemIndex.size();
	for(int _index(0); _index < _Size; ++_index)
	{
		GenItem::ItemInfo _Ittmp;
		GenItem::GenItemManager::GetInstance().GetItemInfo(_Ittmp, _tempSpinfo->m_ItemIndex[_index]);
		m_vecItInfo.push_back(_Ittmp);
	}

	UpdateItemdataItems();
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnNMCustomdrawListInfoGenitemfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;	
	CString strCol = _T("");

	switch(pLVCD->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			{
				*pResult = CDRF_NOTIFYITEMDRAW;
				return;
			}
		case CDDS_ITEMPREPAINT:
			{
				*pResult = CDRF_NOTIFYSUBITEMDRAW;
				return;
			}
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
			{
				COLORREF crText;
				strCol = m_ListGenItemFileData.GetItemText(pLVCD->nmcd.dwItemSpec, 0);					
				if(strCol == "SKILLFACT")
				{
					crText = RGB(0, 0, 255);					
				}			
				else
				{
					crText = RGB(0, 0, 0);					
				}		

				pLVCD->clrText = crText;

				*pResult = CDRF_DODEFAULT;
				return;
		}
	}
}

void GenItemRevise::UpdateAnnotation()
{		
	m_Listannotation.DeleteAllItems();
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorBegin(m_EditData.m_listSpIDnRate.begin());
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorEnd(m_EditData.m_listSpIDnRate.end());
	CString _strtmp("");
	for(int count(0);_iteratorBegin != _iteratorEnd; ++_iteratorBegin, ++count)
	{
		int insertIndex = m_Listannotation.InsertItem(count, (*_iteratorBegin)->m_StrName.c_str());
		_strtmp.Format(_T("%d"), (*_iteratorBegin)->m_SpecID);
		m_Listannotation.SetItem(insertIndex, 1, LVIF_TEXT, _strtmp, 0, 0, 0, 0);
		m_Listannotation.SetItem(insertIndex, 2, LVIF_TEXT, (*_iteratorBegin)->m_StrAnnotation.c_str(), 0, 0, 0, 0);
	}
}

void GenItemRevise::OnBnClickedButtonReviseInputAnnotation()
{
	if(m_TNL.m_Type != GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC && m_TNL.m_Type != GenItem::GenItemEditData::SPIDNRATE_SKILLFACT)
		return;

	std::string _Annotation = GetWin_Text(this, IDC_EDIT_REVISE_ANNOTATION).GetString();
	GenItem::GenItemEditData::SpIDnRate* tempPoint = GetSpIDnRate(m_TNL.m_Selection);
	tempPoint->m_StrAnnotation = _Annotation;

	m_TNL.m_Type = GenItem::GenItemEditData::NONE;
	m_TNL.m_Selection = -1;		
	UpdateAnnotation();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void GenItemRevise::OnNMDblclkListInfoGenitemfileAnnotation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorBegin(m_EditData.m_listSpIDnRate.begin());
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorEnd(m_EditData.m_listSpIDnRate.end());
	POSITION _pos = m_Listannotation.GetFirstSelectedItemPosition();		
	int _SetItem = m_Listannotation.GetNextSelectedItem(_pos);
	for(int count(0); _iteratorBegin != _iteratorEnd; ++_iteratorBegin, ++count)
	{
		if(count == _SetItem)
		{
			(*_iteratorBegin)->m_StrAnnotation = "";
			break;			
		}
	}
	UpdateAnnotation();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void GenItemRevise::OnNMCustomdrawListInfoGenitemfileAnnotation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;	
	CString strCol = _T("");

	switch(pLVCD->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			{
				*pResult = CDRF_NOTIFYITEMDRAW;
				return;
			}
		case CDDS_ITEMPREPAINT:
			{
				*pResult = CDRF_NOTIFYSUBITEMDRAW;
				return;
			}
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
			{
				COLORREF crText;
				strCol = m_Listannotation.GetItemText(pLVCD->nmcd.dwItemSpec, 0);				
				
				if(strCol == "SKILLFACT")
				{
					crText = RGB(0, 0, 255);	
				}
				else
				{
					crText = RGB(0, 0, 0);
				}
			
				pLVCD->clrText = crText;

				*pResult = CDRF_DODEFAULT;
				return;
			}
	}
}

void GenItemRevise::OnNMClickListInfoGenitemfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION _pos = m_ListGenItemFileData.GetFirstSelectedItemPosition();		
	int _SetItem = m_ListGenItemFileData.GetNextSelectedItem(_pos);
	if(_SetItem == -1) return;	

	FindClickData(m_TNL.m_Type, m_TNL.m_Selection, _SetItem);

	if(m_TNL.m_Selection == -1) return;

	switch(m_TNL.m_Type)
	{
	case GenItem::GenItemEditData::FILEGENRATE:
		{
			SetWin_Num_float(this, IDC_EDIT_REVISE_GENRATE, m_EditData.m_FileGenRate.m_Rate);				
			break;
		}
	case GenItem::GenItemEditData::NUMRATE:
		{
			GenItem::GenItemEditData::NumRate* tempPoint = GetNumRateFromList(m_TNL.m_Selection);
			SetWin_Num_int(this, IDC_EDIT_REVISE_GENNUM_NUM, tempPoint->m_Num);
			SetWin_Num_float(this, IDC_EDIT_REVISE_GENNUM_RATE, tempPoint->m_NumRate);	
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Edit"));
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Input"));
			break;
		}
	case GenItem::GenItemEditData::SPIDNRATE_ITEMSPEC:
		{
			GenItem::GenItemEditData::SpIDnRate* tempPoint = GetSpIDnRate(m_TNL.m_Selection);
			SetWin_Num_int(this, IDC_EDIT_REVISE_GENDATA_SPECID, tempPoint->m_SpecID);
			SetWin_Num_float(this, IDC_EDIT_REVISE_GENDATA_RATE, tempPoint->m_Rate);
			SetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_MID, 65535);
			SetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_SID, 65535);				
			CheckDlgButton(IDC_CHECK_REVISE_SKILLFACT, BST_UNCHECKED);	
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Input"));
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Edit"));
			m_CheckSkillFact = false;			
			DlgUseCheck();
			break;
		}
	case GenItem::GenItemEditData::SPIDNRATE_SKILLFACT:
		{
			GenItem::GenItemEditData::SpIDnRate* tempPoint = GetSpIDnRate(m_TNL.m_Selection);
			SetWin_Num_int(this, IDC_EDIT_REVISE_GENDATA_SPECID, tempPoint->m_SpecID);
			SetWin_Num_float(this, IDC_EDIT_REVISE_GENDATA_RATE, tempPoint->m_Rate);
			SetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_MID, (int)tempPoint->m_SkillID.wMainID);
			SetWin_Num_int(this, IDC_EDIT_REVISE_SKILLID_SID, (int)tempPoint->m_SkillID.wSubID);			
			CheckDlgButton(IDC_CHECK_REVISE_SKILLFACT, BST_CHECKED);	
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Input"));
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Edit"));
			m_CheckSkillFact = true;	
			DlgUseCheck();
			break;
		}
	case GenItem::GenItemEditData::NONE:
		{
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENNUM)->SetWindowText(_T("Input"));
			GetDlgItem(IDC_BUTTON_REVISE_INPUT_GENDATA)->SetWindowText(_T("Input"));
			m_TNL.m_Type = GenItem::GenItemEditData::NONE;
			break;
		}
	}

	*pResult = 0;
}

void GenItemRevise::OnNMClickListInfoGenitemfileAnnotation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorBegin(m_EditData.m_listSpIDnRate.begin());
	std::list<GenItem::GenItemEditData::SpIDnRate*>::iterator _iteratorEnd(m_EditData.m_listSpIDnRate.end());
	POSITION _pos = m_Listannotation.GetFirstSelectedItemPosition();		
	int _SetItem = m_Listannotation.GetNextSelectedItem(_pos);
	for(int count(0); _iteratorBegin != _iteratorEnd; ++_iteratorBegin, ++count)
	{
		if(count == _SetItem)
		{
			m_TNL.m_Type = (*_iteratorBegin)->m_Type;
			m_TNL.m_Selection = _SetItem;
		}
	}

	SetWin_Text(this, IDC_EDIT_REVISE_ANNOTATION, m_Listannotation.GetItemText(_SetItem, 2));

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
