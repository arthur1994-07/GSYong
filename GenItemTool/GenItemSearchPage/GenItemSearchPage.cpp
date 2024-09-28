#include "stdafx.h"

#include "../DataSelect.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../GenItemRevise.h"
#include "../GenItemTool.h"
#include "../SheetWithTab.h"
#include "./GenItemSearchPage.h"
#include "../RanLogic/GenItemTable/GenItemManager.h"

#include "../SigmaCore/String/StringUtil.h"




// CGenItemSearchPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CGenItemSearchPage, CPropertyPage)
CGenItemSearchPage::CGenItemSearchPage(LOGFONT logfont)
: CPropertyPage(CGenItemSearchPage::IDD)
, m_pFont(new CFont())
{	
	for(int i = 0; i < 5; ++i)
		m_CheckRange[i] = false;

	m_pFont->CreateFontIndirect(&logfont);
}
CGenItemSearchPage::~CGenItemSearchPage(void)
{	
	SAFE_DELETE (m_pFont);	
}

void CGenItemSearchPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GENITEM, m_listGenItem);
	DDX_Control(pDX, IDC_EDIT_CROW_NAME, m_EditCrowName);
	DDX_Control(pDX, IDC_EDIT_ITEM_NAME, m_EditItemName);
	DDX_Control(pDX, IDC_EDIT_GENITEMFILENAME, m_EditGenItemFileName);
}

BEGIN_MESSAGE_MAP(CGenItemSearchPage, CPropertyPage)	
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CGenItemSearchPage::OnBnClickedButtonSearch)	
	ON_BN_CLICKED(IDC_BUTTON_FINDWORD_CROWNAME, &CGenItemSearchPage::OnBnClickedButtonFindwordCrowname)
	ON_BN_CLICKED(IDC_BUTTON_FINDWORD_ITEMNAME, &CGenItemSearchPage::OnBnClickedButtonFindwordItemname)
	ON_BN_CLICKED(IDC_BUTTON_FINDWORD_GENITEMNAME, &CGenItemSearchPage::OnBnClickedButtonFindwordGenitemname)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_GENITEM, &CGenItemSearchPage::OnNMCustomDrawListGenItemData)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_GENITEM, &CGenItemSearchPage::OnLvnColumnclickListGenitem)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GENITEM, &CGenItemSearchPage::OnNMDblclkGenItemList)
END_MESSAGE_MAP()

BOOL CGenItemSearchPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont(this, m_pFont, CDF_TOPLEFT);
	m_listGenItem.SetExtendedStyle(m_listGenItem.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	char* szNameColumn[CGenItemSearchPage::ELEMENT_NSIZE] = 
	{
		"SpecID",		
		"ItemMID", "ItemSID", "ItemName", "SpecRate",
		"CrowMID", "CrowSID", "CrowName",
		"GenItem", "Num",
		"GenRate", "SumRate", 
		"Skill"
	};

	const int widthRateColumn[CGenItemSearchPage::ELEMENT_NSIZE] = 
	{
		5,		
		5, 5, 13, 9,
		5, 5, 13,
		10, 4,
		9, 9, 
		7
	};

	RECT rectCtrl;
	m_listGenItem.GetClientRect ( &rectCtrl );

	const long lnWidth(rectCtrl.right - rectCtrl.left);
	for ( unsigned int _index(0); _index < CGenItemSearchPage::ELEMENT_NSIZE; ++_index )
	{
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szNameColumn[_index];
		lvColumn.iSubItem = -1;
		lvColumn.cx = (lnWidth * widthRateColumn[_index] ) / 100;
		m_listGenItem.InsertColumn(_index, &lvColumn);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

bool CGenItemSearchPage::CheckSameWord(const std::string &_str)
{
	std::vector<std::string>_vecStr;
	std::string _CopyStr = _str;
	char* _Origin = const_cast<char*>(_CopyStr.c_str());
	char* _Seperatething(NULL);
	_Seperatething = strtok(_Origin, ";");

	for(; _Seperatething != NULL; _Seperatething = strtok(NULL, ";"))
	{
		if(_Seperatething != NULL)
		{		
			std::string _temp = _Seperatething;
			_vecStr.push_back(_temp);
		}
	}

	unsigned int _Size = _vecStr.size();
	for(unsigned int i = 0; i < _Size; ++i)
	{
		for(unsigned int j = 0; j < _Size; ++j)
		{
			if(i == j) continue;

			if(_vecStr[i] == _vecStr[j])
				return false;
		}		
	}
	
	return true;
}

void CGenItemSearchPage::UpdateItems(void)
{	
	m_listGenItem.DeleteAllItems();

	unsigned int _Size = m_vecGenItemData.size();
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		CString Strtemp;	

		Strtemp.Format(_T("%d"), m_vecGenItemData[_index]->nSpecID);
		int InsertIndex = m_listGenItem.InsertItem(_index, Strtemp);
		
		Strtemp.Format(_T("%d"), (int)m_vecGenItemData[_index]->itemID.wMainID);
		m_listGenItem.SetItem(InsertIndex, ITEM_MAINID, LVIF_TEXT, Strtemp, 0, 0, 0, 0);		

		Strtemp.Format(_T("%d"), (int)m_vecGenItemData[_index]->itemID.wSubID);
		m_listGenItem.SetItem(InsertIndex, ITEM_SUBID, LVIF_TEXT, Strtemp, 0, 0, 0, 0);		
		
		m_listGenItem.SetItem(InsertIndex, ITEM_NAME, LVIF_TEXT, m_vecGenItemData[_index]->stringItemName.c_str(), 0, 0, 0, 0);
		
		Strtemp.Format(_T("%g"), m_vecGenItemData[_index]->rateSpec * 100);
		Strtemp.Append(_T(" %"));
		m_listGenItem.SetItem(InsertIndex, RATE_SPEC, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

		Strtemp.Format(_T("%d"), (int)m_vecGenItemData[_index]->crowID.wMainID);		
		m_listGenItem.SetItem(InsertIndex, CROW_MAINID, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

		Strtemp.Format(_T("%d"), (int)m_vecGenItemData[_index]->crowID.wSubID);
		m_listGenItem.SetItem(InsertIndex, CROW_SUBID, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

		m_listGenItem.SetItem(InsertIndex, CROW_NAME, LVIF_TEXT, m_vecGenItemData[_index]->stringCrowName.c_str(), 0, 0, 0, 0);

		m_listGenItem.SetItem(InsertIndex, GENITEM_FILE_NAME, LVIF_TEXT, m_vecGenItemData[_index]->stringGenItemFileName.c_str(), 0, 0, 0, 0);

		Strtemp.Format(_T("%d"), m_vecGenItemData[_index]->sizeGenNum);
		m_listGenItem.SetItem(InsertIndex, GEN_NUM, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

		Strtemp.Format(_T("%g"), m_vecGenItemData[_index]->rateGen * 100);
		Strtemp.Append(_T(" %"));
		m_listGenItem.SetItem(InsertIndex, RATE_GEN, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

		Strtemp.Format(_T("%g"), m_vecGenItemData[_index]->rateTotal * 100);
		Strtemp.Append(_T(" %"));
		m_listGenItem.SetItem(InsertIndex, RATE_TOTAL, LVIF_TEXT, Strtemp, 0, 0, 0, 0);		
			
		Strtemp.Format(_T("%d / %d"), m_vecGenItemData[_index]->skillID.wMainID, m_vecGenItemData[_index]->skillID.wSubID);
		m_listGenItem.SetItem(InsertIndex, SKILL_NATIVEID, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
		
	}
}

void CGenItemSearchPage::OnBnClickedButtonSearch()
{	
	if(!GetSearchWord()) return;
	if(!GenItem::GenItemManager::GetInstance().FindData(m_SearChdataInfo))
	{
		MessageBox("It Can't find Unless you input a 'Right' SearchWord", "Search", MB_OK);
		return;
	}
	GenItem::GenItemManager::GetInstance().DeleverSearchData(m_vecGenItemData);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	UpdateItems();	

	if(m_SearChdataInfo.m_CrowID_MID == "" && m_SearChdataInfo.m_CrowID_SID == "" &&
		m_SearChdataInfo.m_CrowName == "" && m_SearChdataInfo.m_ItemID_MID == "" &&
		m_SearChdataInfo.m_GenItemFileName == "" && m_SearChdataInfo.m_ItemID_MID == ""&&
		m_SearChdataInfo.m_ItemID_SID == "" && m_SearChdataInfo.m_ItemName == "" &&
		m_SearChdataInfo.m_SpecID == "")
	{
		MessageBox("It Can't find Unless you input a SearchWord", "Search", MB_OK);
		return;
	}

	if(m_vecGenItemData.size() != 0)
	{
		std::string _SearchNum( sc::string::format(	"Result: %1%",m_vecGenItemData.size()));
		MessageBox(_SearchNum.c_str(), "Search", MB_OK);
	}
	else
	{
		MessageBox("You Can't find it", "Search", MB_OK);
	}

	m_SearChdataInfo.clear();	
}

bool CGenItemSearchPage::GetSearchWord()
{
	m_SearChdataInfo.m_CrowName = GetWin_Text(this, IDC_EDIT_CROW_NAME).GetString();
	if(!CheckSameWord(m_SearChdataInfo.m_CrowName))
	{
		MessageBox("Same Word in CrowName_box", "Search", MB_OK);
		return false;
	}

	m_SearChdataInfo.m_CrowID_MID = GetWin_Text(this, IDC_EDIT_CROW_MID).GetString();
	if(!CheckSameWord(m_SearChdataInfo.m_CrowID_MID))
	{
		MessageBox("Same Word in Crow_MID_box", "Search", MB_OK);
		return false;
	}

	m_SearChdataInfo.m_CrowID_SID = GetWin_Text(this, IDC_EDIT_CROW_SID).GetString();	
	if(!CheckSameWord(m_SearChdataInfo.m_CrowID_SID))
	{
		MessageBox("Same Word in Crow_SID_box", "Search", MB_OK);
		return false;
	}

	GenItem::GenItemManager::GetInstance().SeperateStringID(m_SearChdataInfo.m_vecCrowID, 
						m_SearChdataInfo.m_CrowID_MID, m_SearChdataInfo.m_CrowID_SID, ";");

	m_SearChdataInfo.m_ItemName = GetWin_Text(this, IDC_EDIT_ITEM_NAME).GetString();
	if(!CheckSameWord(m_SearChdataInfo.m_ItemName))
	{
		MessageBox("Same Word in ItemName_box", "Search", MB_OK);
		return false;
	}

	m_SearChdataInfo.m_ItemID_MID = GetWin_Text(this, IDC_EDIT_ITEM_MID).GetString();
	if(!CheckSameWord(m_SearChdataInfo.m_ItemID_MID))
	{
		MessageBox("Same Word in Item_MID_box", "Search", MB_OK);
		return false;
	}

	m_SearChdataInfo.m_ItemID_SID = GetWin_Text(this, IDC_EDIT_ITEM_SID).GetString();	
	if(!CheckSameWord(m_SearChdataInfo.m_ItemID_SID))
	{
		MessageBox("Same Word in Item_SID_box", "Search", MB_OK);
		return false;
	}

	GenItem::GenItemManager::GetInstance().SeperateStringID(m_SearChdataInfo.m_vecItemID, 
						m_SearChdataInfo.m_ItemID_MID, m_SearChdataInfo.m_ItemID_SID, ";");

	m_SearChdataInfo.m_SpecID = GetWin_Text(this, IDC_EDIT_GEN_SPEC_ID).GetString();
	if(!CheckSameWord(m_SearChdataInfo.m_SpecID))
	{
		MessageBox("Same Word in SpecID_box", "Search", MB_OK);
		return false;
	}

	m_SearChdataInfo.m_GenItemFileName = GetWin_Text(this, IDC_EDIT_GENITEMFILENAME).GetString();
	if(!CheckSameWord(m_SearChdataInfo.m_GenItemFileName))
	{
		MessageBox("Same Word in GenItemFileName_box", "Search", MB_OK);
		return false;
	}

	return true;
}

void CGenItemSearchPage::OnBnClickedButtonFindwordCrowname()
{	
	DataSelect Dlg;
	Dlg.SetCase(GenItem::CROW_NAME_WORD);	
	Dlg.SetCompleteStr(GetWin_Text(this, IDC_EDIT_CROW_NAME).GetString());
	Dlg.DoModal();

	std::string _CompleteWord = Dlg.GetCompleteStr();
	SetWin_Text(this, IDC_EDIT_CROW_NAME, _CompleteWord.c_str());
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CGenItemSearchPage::OnBnClickedButtonFindwordItemname()
{
	DataSelect Dlg;
	Dlg.SetCase(GenItem::ITEM_NAME_WORD);
	Dlg.SetCompleteStr(GetWin_Text(this, IDC_EDIT_ITEM_NAME).GetString());	
	Dlg.DoModal();

	std::string _CompleteWord = Dlg.GetCompleteStr();
	SetWin_Text(this, IDC_EDIT_ITEM_NAME, _CompleteWord.c_str());
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CGenItemSearchPage::OnBnClickedButtonFindwordGenitemname()
{
	DataSelect Dlg;
	Dlg.SetCase(GenItem::GENITEMFILE_NAME_WORD);
	Dlg.SetCompleteStr(GetWin_Text(this, IDC_EDIT_GENITEMFILENAME).GetString());	
	Dlg.DoModal();

	std::string _CompleteWord = Dlg.GetCompleteStr();
	SetWin_Text(this, IDC_EDIT_GENITEMFILENAME, _CompleteWord.c_str());
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CGenItemSearchPage::OnNMCustomDrawListGenItemData(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;
	*pResult = 0;
	int nRow = 0;
	int nCol = 0;
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
				strCol = m_listGenItem.GetItemText(pLVCD->nmcd.dwItemSpec, 12);

				if(strCol != "65535 / 65535")
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
//Skill ID가 존재할 시에 파란색으로 출력 색깔을 변경해주는 함수

void CGenItemSearchPage::OnLvnColumnclickListGenitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int _NowColumn = pNMLV->iSubItem;
	switch(_NowColumn)
	{
		case SPEC_ID:
			{
				if(m_CheckRange[0])
				{
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);
				
					if(tempobj.size() == 0) return;

					for(int _Max = GLItemMan::SPECID_NUM; _Max > 0; --_Max)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->nSpecID == _Max)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}

					m_CheckRange[0] = false;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 0)
							continue;
						m_CheckRange[i] = false;
					}
				}
				else
				{
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(int _Max = 1; _Max < GLItemMan::SPECID_NUM; ++_Max)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->nSpecID == _Max)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}
					m_CheckRange[0] = true;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 0)
							continue;
						m_CheckRange[i] = false;
					}
				}

				break;
			}

		case ITEM_MAINID:
			{
				if(m_CheckRange[1])
				{
					size_t _MaxMid(GLItemMan::GetInstance().GetMaxMid());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(size_t _Mid = _MaxMid; _Mid > 0; --_Mid) //MID의 Max, GLItemMan.h 참조
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->itemID.wMainID == _Mid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}	
					m_CheckRange[1] = false;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 1)
							continue;
						m_CheckRange[i] = false;
					}
				}
				else
				{
					size_t _MaxMid(GLItemMan::GetInstance().GetMaxMid());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(size_t _Mid = 0; _Mid < _MaxMid; ++_Mid) //MID의 Max, GLItemMan.h 참조
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->itemID.wMainID == _Mid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}	
					m_CheckRange[1] = true;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 1)
							continue;
						m_CheckRange[i] = false;
					}
				}
				break;
			}

		case ITEM_SUBID:
			{
				if(m_CheckRange[2])
				{				
					size_t  _MaxSid(GLItemMan::GetInstance().GetMaxSid());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(size_t _Sid = _MaxSid; _Sid > 0; --_Sid)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->itemID.wSubID == _Sid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}
					m_CheckRange[2] = false;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 2)
							continue;
						m_CheckRange[i] = false;
					}
				}
				else
				{
					size_t  _MaxSid(GLItemMan::GetInstance().GetMaxSid());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(size_t _Sid = 0; _Sid < _MaxSid; ++_Sid)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->itemID.wSubID == _Sid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}
					m_CheckRange[2] = true;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 2)
							continue;
						m_CheckRange[i] = false;
					}
				}
				break;
			}

		case CROW_MAINID:
			{
				if(m_CheckRange[3])
				{				
					size_t _CrowMaxMid(GLCrowDataMan::GetInstance().GetCrowMaxMID());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;
					
					for(size_t _Mid = _CrowMaxMid; _Mid > 0; --_Mid)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->crowID.wMainID == _Mid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}
					m_CheckRange[3] = false;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 3)
							continue;
						m_CheckRange[i] = false;
					}
				}
				else
				{
					size_t _CrowMaxMid(GLCrowDataMan::GetInstance().GetCrowMaxMID());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(size_t _Mid = 0; _Mid < _CrowMaxMid; ++_Mid)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->crowID.wMainID == _Mid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}
					m_CheckRange[3] = true;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 3)
							continue;
						m_CheckRange[i] = false;
					}
				}
				break;
			}

		case CROW_SUBID:
			{
				if(m_CheckRange[4])
				{				
					size_t _CrowMaxSid(GLCrowDataMan::GetInstance().GetCrowMaxSID());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(size_t _Sid = _CrowMaxSid; _Sid > 0; --_Sid)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->crowID.wSubID == _Sid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}
					m_CheckRange[4] = false;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 4)
							continue;
						m_CheckRange[i] = false;
					}
				}
				else
				{
					size_t _CrowMaxSid(GLCrowDataMan::GetInstance().GetCrowMaxSID());
					std::vector<GenItem::GenItemData*> tempobj;
					tempobj.swap(m_vecGenItemData);

					if(tempobj.size() == 0) return;

					for(size_t _Sid = 0; _Sid < _CrowMaxSid; ++_Sid)
					{
						unsigned int _Size = tempobj.size();
						for(unsigned int _index = 0; _index < _Size; ++_index)
						{
							if(tempobj[_index]->crowID.wSubID == _Sid)
								m_vecGenItemData.push_back(tempobj[_index]);
						}
					}
					m_CheckRange[4] = true;

					for(int i = 0; i < 5; ++i)
					{
						if(i == 4)
							continue;
						m_CheckRange[i] = false;
					}
				}
				break;
			}

		default:
			return;
	}
	
	UpdateItems();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

//Column 클릭시 정렬해서 출력용 함수

void CGenItemSearchPage::OnNMDblclkGenItemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION _pos = m_listGenItem.GetFirstSelectedItemPosition();		
	int _SetItem = m_listGenItem.GetNextSelectedItem(_pos);

	std::string _strtmp = m_listGenItem.GetItemText(_SetItem, GENITEM_FILE_NAME);
	GenItemRevise::GetInstance().Clear();
	GenItemRevise::GetInstance().SetFileName(_strtmp);

	if(GenItemRevise::GetInstance().GetSafeHwnd() == NULL)
	{
		GenItemRevise::GetInstance().Create(IDD_DIALOG_REVISE);
	}	
	else
	{
		GenItemRevise::GetInstance().DestroyWindow();
		GenItemRevise::GetInstance().Create(IDD_DIALOG_REVISE);
	}
	GenItemRevise::GetInstance().ShowWindow(SW_SHOW);
}