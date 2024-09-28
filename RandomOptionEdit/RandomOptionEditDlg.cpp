
// RandomOptionEditDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "./RandomOptionEdit.h"
#include "./RandomOptionEditDlg.h"
#include "./RandomOptionEditDefine.h"
#include "./RandomOptionEditFunction.h"
#include "./RandomOptionConvertDlg.h"
#include "../RanLogic/RandomOption/RandomOption.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"

#include "./RandomOptionEditDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CRandomOptionEditDlg::CRandomOptionEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRandomOptionEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRandomOptionEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_RANDOM_OPTION, m_RandomOptionListBox);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_SerchEditBox);
	DDX_Control(pDX, IDC_EDIT_RANDOM_OPTION_NAME, m_NameEditBox);
	DDX_Control(pDX, IDC_LIST_RANDOM_OPTION_ENTITY, m_RandomOptionData);
	DDX_Control(pDX, IDC_LIST_RANDOM_OPTION_VALUE, m_OptionValueListBox);
	DDX_Control(pDX, IDC_EDIT_0_NAME, m_TypeEditBox);
	DDX_Control(pDX, IDC_EDIT_1_RATE, m_RateEditBox);
	DDX_Control(pDX, IDC_EDIT_2_MINIMUM, m_LowEditBox);
	DDX_Control(pDX, IDC_EDIT_3_MAXIMUM, m_HighEditBox);
}

BEGIN_MESSAGE_MAP(CRandomOptionEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BUTTON_CSV_SAVE, OnBnClickedBtnCsvSave)
	ON_BN_CLICKED(IDC_BUTTON_CSV_LOAD, OnBnClickedBtnCsvLoad)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedBtnAddEntity)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_OLD, OnBnClickedBtnOldLoad)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedBtnRemoveEntity)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedBtnApplyEntity)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_VALUE, OnBnClickedBtnRemoveValue)
	ON_BN_CLICKED(IDC_BUTTON_ADD_VALUE, OnBnClickedBtnAddValue)
	ON_LBN_SELCHANGE(IDC_LIST_RANDOM_OPTION, OnBnSelchangeRandomOption)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RANDOM_OPTION_ENTITY, OnBnSelchangeEntity)
// 	ON_EN_CHANGE(IDC_EDIT_1_RATE, OnChangeValue)
// 	ON_EN_CHANGE(IDC_EDIT_2_MINIMUM, OnChangeValue)
// 	ON_EN_CHANGE(IDC_EDIT_3_MAXIMUM, OnChangeValue)
	
END_MESSAGE_MAP()


// CRandomOptionEditDlg 메시지 처리기

BOOL CRandomOptionEditDlg::OnInitDialog()
{
	if ( 0 == (RandomOptionEdit::hWindowMain = GetSafeHwnd()) )
		return FALSE;

	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);		
	SetIcon(m_hIcon, FALSE);	

	m_HighEditBox.SubclassDlgItem(IDC_EDIT_3_MAXIMUM, this);
	int nColumn = 0;
	// 종류, 확률, 최소, 최대;
	m_RandomOptionData.InsertColumn(nColumn++, "Type", LVCFMT_LEFT, 196);
	m_RandomOptionData.InsertColumn(nColumn++, "Probability", LVCFMT_LEFT, 100);
	m_RandomOptionData.InsertColumn(nColumn++, "Min", LVCFMT_LEFT, 100);
	m_RandomOptionData.InsertColumn(nColumn++, "Max", LVCFMT_LEFT, 100);
	m_RandomOptionData.InsertColumn(nColumn++, "identification-key", LVCFMT_LEFT, 1);

	m_OptionValueListBox.ResetContent();
	for ( unsigned int _index(0); _index < RandomOption::RANDOM_OPTION_NSIZE; ++_index )
		m_OptionValueListBox.AddString(RandomOption::cNameValue[_index]);

	m_RandomOptionData.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

void CRandomOptionEditDlg::OnOK(void)
{
}

BOOL CRandomOptionEditDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				const HWND hFocus(::GetFocus());
				if ( hFocus == m_RateEditBox.GetSafeHwnd())
				{
					::SetFocus(::GetDlgItem(RandomOptionEdit::hWindowMain, IDC_EDIT_2_MINIMUM));
					this->OnChangeValue();
					return TRUE;
				}
				else if ( hFocus == m_LowEditBox.GetSafeHwnd())
				{
					::SetFocus(::GetDlgItem(RandomOptionEdit::hWindowMain, IDC_EDIT_3_MAXIMUM));
					this->OnChangeValue();
					return TRUE;
				}
				else if ( hFocus == m_HighEditBox.GetSafeHwnd())
				{
					::SetFocus(::GetDlgItem(RandomOptionEdit::hWindowMain, IDC_EDIT_1_RATE));
					this->OnChangeValue();
					return TRUE;
				}
				else if ( hFocus == m_SerchEditBox.GetSafeHwnd() )
				{
					this->UpdateRandomOptionList();
					return TRUE;
				}
			}
		}
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CRandomOptionEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRandomOptionEditDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRandomOptionEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRandomOptionEditDlg::OnBnClickedBtnLoad(void)
{
	if ( RandomOption::LoadFile(RandomOption::LOAD_TOOL) == E_FAIL )
		::MessageBox(0, TEXT("loadFile() : 불러오기에 실패 하였습니다;"), TEXT("실패"), MB_OK);

	m_RandomOptionListBox.ResetContent();
	UpdateRandomOptionList();
}

void CRandomOptionEditDlg::OnBnClickedBtnOldLoad(void)
{
	CRandomOptionConvertDlg ConverDlg;
	if ( ConverDlg.DoModal() == IDOK )
	{
		RandomOptionEdit::hWindowConvert = 0;
	}
}

void CRandomOptionEditDlg::OnBnClickedBtnAddEntity(void)
{
	char _cName[RandomOption::LENGTH_NAME] = { 0,};
	CString strText;
	m_NameEditBox.GetWindowText(strText);
	strcpy(_cName, strText.GetString());
	const std::string _stringName(_cName);
	if ( _stringName.size() == 0 )
	{
		::MessageBox(0, TEXT("추가 할 아이템의 이름을 입력 해 주십시요;"), TEXT("실패"), MB_OK);
		return;
	}

	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());

	if ( _managerRandomOption.find(_stringName.c_str()) == 0 )
	{
		_managerRandomOption.create(_stringName.c_str());
		m_RandomOptionListBox.AddString(_stringName.c_str());
	}
	else
		::MessageBox(0, TEXT("중복 된 이름 입니다;"), _stringName.c_str(), MB_OK);
}

void CRandomOptionEditDlg::OnBnClickedBtnRemoveEntity(void)
{
	const unsigned int _sizeSelected(m_RandomOptionListBox.GetSelCount());
	if ( _sizeSelected == 0 )
	{
		::MessageBox(0, TEXT("삭제 할 아이템들을 선택 해 주십시요;"), TEXT("실패"), MB_OK);
		return;
	}

	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	unsigned int* _pIDItems(new unsigned int[_sizeSelected]);
	m_RandomOptionListBox.GetSelItems(_sizeSelected, (LPINT)_pIDItems);
	for ( unsigned int _index(_sizeSelected); _index--; )
	{
		char _cName[RandomOption::LENGTH_NAME];
		m_RandomOptionListBox.GetText(_pIDItems[_index], _cName);
		_managerRandomOption.remove(_cName);
	}

	delete[] _pIDItems;

	this->UpdateRandomOptionList();
}

void CRandomOptionEditDlg::OnBnClickedBtnApplyEntity(void)
{
	const unsigned int _sizeSelected(m_RandomOptionListBox.GetSelCount());
	if ( _sizeSelected == 0 )
	{
		::MessageBox(0, TEXT("이름을 변경 할 아이템들을 선택 해 주십시요;"), TEXT("실패"), MB_OK);
		return;
	}

	const unsigned int _indexList(m_RandomOptionListBox.GetCurSel());

	char _cNameTarget[RandomOption::LENGTH_NAME];
	char _cNameSource[RandomOption::LENGTH_NAME];
	m_RandomOptionListBox.GetText(m_RandomOptionListBox.GetCurSel(), _cNameTarget);
	CString strText;
	m_NameEditBox.GetWindowText(strText);
	strcpy(_cNameSource, strText.GetString());

	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	RandomOption::Entity* const _pEntity(_managerRandomOption.rename(_cNameTarget, _cNameSource));
	if ( _pEntity == 0 )
		::MessageBox(0, TEXT("선택된 아이템이 없거나, 유효한 이름이 아닙니다;"), TEXT("오류"), MB_OK);
	else if ( std::string(_pEntity->getName()) == std::string(_cNameTarget) )
		::MessageBox(0, TEXT("중복 된 이름 입니다;"), TEXT("오류"), MB_OK);
	else
		this->UpdateRandomOptionList();
}

void CRandomOptionEditDlg::OnBnClickedBtnSave(void)
{
	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	if ( _managerRandomOption.saveFile() == false )
		::MessageBox(0, TEXT("saveFile() : 저장에 싱패 하였습니다;"), TEXT("실패"), MB_OK);
	else
		::MessageBox(0, TEXT("saveFile() : 저장에 성공 하였습니다;"), TEXT("성공"), MB_OK);
}

void CRandomOptionEditDlg::OnBnClickedBtnCsvSave(void)
{
	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	if ( _managerRandomOption.saveCsv(this) == false )
		::MessageBox(0, TEXT("saveCsv() : 저장에 실패 하였습니다;"), TEXT("실패"), MB_OK);
	else
		::MessageBox(0, TEXT("saveCsv() : 저장에 성공 하였습니다;"), TEXT("성공"), MB_OK);
}

void CRandomOptionEditDlg::OnBnClickedBtnCsvLoad(void)
{
	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	if ( _managerRandomOption.loadCsv(this) == false )
		::MessageBox(0, TEXT("loadCsv() : 읽기에 실패 하였습니다;"), TEXT("실패"), MB_OK);
	else
		::MessageBox(0, TEXT("loadCsv() : 읽기에 성공 하였습니다;"), TEXT("성공"), MB_OK);

	m_RandomOptionListBox.ResetContent();
	UpdateRandomOptionList();
}

void CRandomOptionEditDlg::OnBnClickedBtnRemoveValue(void)
{
	const HWND _handleWindow(RandomOptionEdit::hWindowMain);
	const HWND _handleListEntity(::GetDlgItem(_handleWindow, IDC_LIST_RANDOM_OPTION));
	const unsigned int _indexList(m_RandomOptionListBox.GetCurSel());
	
	char _cName[RandomOption::LENGTH_NAME];
	m_RandomOptionListBox.GetText(m_RandomOptionListBox.GetCurSel(), _cName);

	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	RandomOption::EntityListMap& _mapEntityList(_managerRandomOption.getMapEntityList());

	RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
	if ( _pEntity == 0 )
		return;

	const HWND _handleListValue(::GetDlgItem(RandomOptionEdit::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));

	int _indexItemPrevious(-1);
	while ( true )
	{
		const int _indexItem(ListView_GetNextItem(_handleListValue, _indexItemPrevious, LVNI_ALL | LVNI_SELECTED));
		if ( _indexItem == -1 )
			break;

		char _cKeyIdentification[RandomOption::LENGTH_NAME];
		ListView_GetItemText(_handleListValue, _indexItem, 4, _cKeyIdentification, RandomOption::LENGTH_NAME);

		if ( _cKeyIdentification[0] != 0 )
		{
			const LPARAM _paramL(LPARAM(::atoi(_cKeyIdentification)));
			const RandomOptionEdit::TYPE _type(RandomOptionEdit::TYPE(_paramL & 0xff000000L));
			const RandomOption::RANDOM_OPTION _typeRandomOption(RandomOption::RANDOM_OPTION((_paramL >> 8) & 0xffL));
			const unsigned int _indexValue(_paramL & 0xffL);
			switch ( _type )
			{
			case RandomOptionEdit::TYPE_INFORMATION_BASIC:
				break;
			case RandomOptionEdit::TYPE_TYPE_DROP:
			case RandomOptionEdit::TYPE_TYPE_REMODEL:
				{
					const RandomOption::GENERATE _typeGenerate(_type == RandomOptionEdit::TYPE_TYPE_DROP ? RandomOption::GENERATE_DROP : RandomOption::GENERATE_REMODEL);
					switch ( _paramL & 0x00ff0000L )
					{					
					case RandomOptionEdit::TYPE_INFORMATION_TYPE:
						_pEntity->removeValueBunch(_typeGenerate, _typeRandomOption);
						break;
					case RandomOptionEdit::TYPE_INFORMATION_VALUE:
						_pEntity->removeValue(_typeGenerate, _typeRandomOption, _indexValue);
						break;
					}
				}
				break;
			}
		}

		_indexItemPrevious = _indexItem;
	}
	this->UpdateRandomOptionData();
}

void CRandomOptionEditDlg::OnBnClickedBtnAddValue(void)
{
}

void CRandomOptionEditDlg::OnBnSelchangeRandomOption(void)
{
	char _cName[RandomOption::LENGTH_NAME];
	
	m_RandomOptionListBox.GetText(m_RandomOptionListBox.GetCurSel(), _cName);
	m_NameEditBox.SetWindowText(_cName);
	this->UpdateRandomOptionData();
}

void CRandomOptionEditDlg::OnBnSelchangeEntity(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString strText;
	m_RandomOptionListBox.GetText(m_RandomOptionListBox.GetCurSel(), strText);
	
	const int nItem(m_RandomOptionData.GetNextItem(-1, LVIS_SELECTED));
	RandomOptionEdit::paramLValueSelected = 0xffffffffL;
	const DWORD dwLParam(m_RandomOptionData.GetItemData(nItem));
	switch ( dwLParam & 0xff000000L )
	{
	case RandomOptionEdit::TYPE_INFORMATION_BASIC:
		RandomOptionEdit::editInformationBasic(strText.GetString(), dwLParam);
		break;
	case RandomOptionEdit::TYPE_TYPE_DROP:
		RandomOptionEdit::editTypeValueDrop(strText.GetString(), dwLParam);
		break;
	case RandomOptionEdit::TYPE_TYPE_REMODEL:
		RandomOptionEdit::editTypeValueRemodel(strText.GetString(), dwLParam);
		break;
	default:							
		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_STATIC_0_NAME, TEXT(""));
		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_STATIC_1_RATE, TEXT(""));
		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_STATIC_2_MINIMUM, TEXT(""));
		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_STATIC_3_MAXIMUM, TEXT(""));

		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_EDIT_0_NAME, TEXT(""));
		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_EDIT_1_RATE, TEXT(""));
		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_EDIT_2_MINIMUM, TEXT(""));
		RandomOptionEdit::setWindowText(RandomOptionEdit::hWindowMain, IDC_EDIT_3_MAXIMUM, TEXT(""));
		break;
	}
	*pResult = 0;
}

void CRandomOptionEditDlg::OnChangeValue()
{
	const LPARAM _paramLValueSelected(RandomOptionEdit::paramLValueSelected);
	if ( _paramLValueSelected == LPARAM(0xffffffffL) )
		return;

	CString strText;
	m_RandomOptionListBox.GetText(m_RandomOptionListBox.GetCurSel(), strText);

	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	RandomOption::EntityListMap& _mapEntityList(_managerRandomOption.getMapEntityList());

	RandomOption::Entity* const _pEntity(_managerRandomOption.find(strText.GetString()));
	if ( _pEntity == NULL )
		return;

	const RandomOptionEdit::TYPE _type(RandomOptionEdit::TYPE(_paramLValueSelected & 0xff000000L));
	switch ( _type )
	{
	case RandomOptionEdit::TYPE_INFORMATION_BASIC:
		break;
	case RandomOptionEdit::TYPE_TYPE_DROP:
	case RandomOptionEdit::TYPE_TYPE_REMODEL:
		{
			CString _strType, _strRate, _strMinimum, _strMaximum;
			m_TypeEditBox.GetWindowText(_strType);
			m_RateEditBox.GetWindowText(_strRate);
			m_LowEditBox.GetWindowText(_strMinimum);
			m_HighEditBox.GetWindowText(_strMaximum);

			const RandomOption::RANDOM_OPTION _typeRandomOption(RandomOption::RANDOM_OPTION((_paramLValueSelected >> 8) & 0xffL));
			const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

			RandomOption::Information _information;
			_information.bValueInteger = _bValueInteger;				
			if ( _bValueInteger == true )
			{
				_information.valueIntegerLow = ::atoi(_strMinimum.GetString());
				_information.valueIntegerHigh = ::atoi(_strMaximum.GetString());
			}
			else
			{
				_information.valueFloatLow = (float)::atof(_strMinimum.GetString());
				_information.valueFloatHigh = (float)::atof(_strMaximum.GetString());
			}

			const RandomOption::GENERATE _typeGenerate(_type == RandomOptionEdit::TYPE_TYPE_DROP ? RandomOption::GENERATE_DROP : RandomOption::GENERATE_REMODEL);
			switch ( _paramLValueSelected & 0x00ff0000L )
			{					
			case RandomOptionEdit::TYPE_INFORMATION_TYPE:
				break;
			case RandomOptionEdit::TYPE_INFORMATION_VALUE:
				{
					const HWND _handleListValue(::GetDlgItem(RandomOptionEdit::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));
					const unsigned int _indexValue(_paramLValueSelected & 0xffL);
					_information.fRateChoose = ::atof(_strRate.GetString()) * 0.01f;

					const int _indexItem(ListView_GetNextItem(_handleListValue, -1, LVNI_ALL | LVNI_SELECTED));
					if ( _indexItem == -1 )
						break;
					else if ( _pEntity->setInformation(_typeGenerate, _typeRandomOption, _indexValue, _information) == true )
					{
						CString _stringOut;

						ListView_SetItemText(_handleListValue, _indexItem, 1, (char*)_strRate.GetString());
						if ( _bValueInteger == true )
						{
							ListView_SetItemText(_handleListValue, _indexItem, 2, (char*)_strMinimum.GetString());
							ListView_SetItemText(_handleListValue, _indexItem, 3, (char*)_strMaximum.GetString());
						}
						else
						{								
							_stringOut.Format(TEXT("%s%%"), _strMinimum.GetString());
							ListView_SetItemText(_handleListValue, _indexItem, 2, (char*)_stringOut.GetBuffer());
							_stringOut.Format(TEXT("%s%%"), _strMaximum.GetString());
							ListView_SetItemText(_handleListValue, _indexItem, 3, (char*)_stringOut.GetBuffer());
						}


						RandomOption::Information _informationReference;
						_pEntity->getInformation(_typeGenerate, _typeRandomOption, _informationReference);

						const unsigned int _indexValueBunch(_indexItem - _indexValue - 1);
						ListView_SetItemText(_handleListValue, _indexValueBunch, 0, (char*)_strType.GetString());
						_stringOut.Format(TEXT("%.0f%%"), _informationReference.fRateChoose);
						ListView_SetItemText(_handleListValue, _indexValueBunch, 1, (char*)_stringOut.GetBuffer());
						if ( _bValueInteger == true )
						{
							_stringOut.Format(TEXT("%d"), _informationReference.valueIntegerLow);
							ListView_SetItemText(_handleListValue, _indexValueBunch, 2, (char*)_stringOut.GetBuffer());
							_stringOut.Format(TEXT("%d"), _informationReference.valueIntegerHigh);
							ListView_SetItemText(_handleListValue, _indexValueBunch, 3, (char*)_stringOut.GetBuffer());
						}
						else
						{
							_stringOut.Format(TEXT("%.4f%%"), _informationReference.valueFloatLow);
							ListView_SetItemText(_handleListValue, _indexValueBunch, 2, (char*)_stringOut.GetBuffer());
							_stringOut.Format(TEXT("%.4f%%"), _informationReference.valueFloatHigh);
							ListView_SetItemText(_handleListValue, _indexValueBunch, 3, (char*)_stringOut.GetBuffer());
						}
					}
				}
				break;
			}
		}
		break;
	}
}

void CRandomOptionEditDlg::UpdateRandomOptionList()
{
	m_RandomOptionListBox.ResetContent();
	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	const RandomOption::EntityListMap& _mapEntityList(_managerRandomOption.getMapEntityList());

	CString _strSerchText;
	m_SerchEditBox.GetWindowText(_strSerchText);
	for ( RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.begin()); _iteratorEntityList != _mapEntityList.end(); ++_iteratorEntityList )
	{
		const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
		for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
		{
			const RandomOption::Entity& _entity(*_iteratorEntity);
			const std::string _stringNameEntity(_entity.getName());

			if ( sc::string::FindWord(_stringNameEntity, _strSerchText.GetString()) == unsigned int(-1) )
				continue;

			m_RandomOptionListBox.AddString(_entity.getName());
		}				
	}
}

void CRandomOptionEditDlg::UpdateRandomOptionData()
{
	m_RandomOptionData.DeleteAllItems();
	CString strText;
	m_NameEditBox.GetWindowText(strText);
	RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
	RandomOption::Entity* const _pEntity(_managerRandomOption.find(strText.GetString()));
	if ( _pEntity == 0 )
		return;

	const float _fRateGenerate(_pEntity->getRateRandomOptionGenerate());
	const float _fRateDestroy(_pEntity->getRateRandomOptionDestroy());
	const float _fRateMoneySpend(_pEntity->getRateMoneySpend());
	unsigned int _indexItem(0);
	LVITEM _item;
	_item.mask = LVIF_TEXT | LVIF_PARAM;
	_item.state = 0;
	_item.stateMask = 0;
	_item.iImage = -1;
	_item.iSubItem = 0;
	_item.iItem = _indexItem;
	_item.lParam = RandomOptionEdit::TYPE_INFORMATION_BASIC;
	_item.pszText = (char*)strText.GetString();

	m_RandomOptionData.InsertItem(&_item);
	strText.Format("발생확률:%.0f%%", _fRateGenerate);
	m_RandomOptionData.SetItemText(_indexItem, 1, strText);
	strText.Format("파괴확률:%.0f%%", _fRateDestroy);
	m_RandomOptionData.SetItemText(_indexItem, 2, strText);
	strText.Format("소비금액:%.0f%%", _fRateMoneySpend);
	m_RandomOptionData.SetItemText(_indexItem, 3, strText);
	strText.Format("%d", _item.lParam);
	m_RandomOptionData.SetItemText(_indexItem++, 4, strText);

	
	for ( unsigned int _indexGenerate(0); _indexGenerate < RandomOption::GENERATE_NSIZE; ++_indexGenerate )
	{
		const RandomOption::ValueBunchVector& _vectorValueBunch(*_pEntity->getVectorValueBunch(RandomOption::GENERATE(_indexGenerate)));
		const unsigned int _sizeValueBunch(_vectorValueBunch.size());

		strText.Format("[%s : %d개]", RandomOption::cNameGenerate[_indexGenerate], _sizeValueBunch);
		m_RandomOptionData.InsertItem(_indexItem, strText.GetString());

		strText.Format("-------------------------");
		m_RandomOptionData.SetItemText(_indexItem, 1, strText);
		m_RandomOptionData.SetItemText(_indexItem, 2, strText);
		m_RandomOptionData.SetItemText(_indexItem, 3, strText);
		m_RandomOptionData.SetItemText(_indexItem++, 4, strText);

		for ( unsigned int _indexValueBunch(0); _indexValueBunch < _sizeValueBunch; ++_indexValueBunch )
		{
			const RandomOption::ValueBunch& _valueBunch(_vectorValueBunch[_indexValueBunch]);

			const float _fRateChoose(_valueBunch.getRateChoose());
			const RandomOption::RANDOM_OPTION _typeRandomOption(_valueBunch.getTypeRandomOption());
			const RandomOption::ValueVector& _vectorValue(_valueBunch.getValueVector());

			const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

			_indexGenerate == RandomOption::GENERATE_DROP ? RandomOptionEdit::TYPE_TYPE_DROP : RandomOptionEdit::TYPE_TYPE_REMODEL;

			strText.Format("%s", RandomOption::cNameValue[_typeRandomOption]);
			_item.mask = LVIF_TEXT | LVIF_PARAM;
			_item.iSubItem = 0;
			_item.iItem = _indexItem;
			switch ( _indexGenerate )
			{
			case RandomOption::GENERATE_DROP:
				_item.lParam = RandomOptionEdit::TYPE_TYPE_DROP;
				break;
			case RandomOption::GENERATE_REMODEL:
				_item.lParam = RandomOptionEdit::TYPE_TYPE_REMODEL;
				break;
			}
			_item.lParam |= (RandomOptionEdit::TYPE_INFORMATION_TYPE | (_typeRandomOption << 8));
			_item.pszText = strText.GetBuffer();
			m_RandomOptionData.InsertItem(&_item);

			strText.Format("%.0f%%", _fRateChoose);
			_item.mask = LVIF_TEXT;
			_item.iSubItem = 1;
			_item.pszText = strText.GetBuffer();
			m_RandomOptionData.SetItem(&_item);
			//_indexItem++;
			if ( _bValueInteger == true )
			{
				const int _valueIntegerHigh(_valueBunch.getValueIntegerHigh());
				const int _valueIntegerLow(_valueBunch.getValueIntegerLow());						

				strText.Format("%d", _valueIntegerLow);
				_item.iSubItem = 2;
				_item.pszText = strText.GetBuffer();
				m_RandomOptionData.SetItem(&_item);

				strText.Format("%d", _valueIntegerHigh);
				_item.iSubItem = 3;
				_item.pszText = strText.GetBuffer();
				m_RandomOptionData.SetItem(&_item);
			}
			else
			{
				const float _fValueFloatHigh(_valueBunch.getValueFloatHigh());
				const float _fValueFloatLow(_valueBunch.getValueFloatLow());

				strText.Format("%.4f%%", _fValueFloatLow);
				_item.iSubItem = 2;
				_item.pszText = strText.GetBuffer();
				m_RandomOptionData.SetItem(&_item);

				strText.Format("%.4f%%", _fValueFloatHigh);
				_item.iSubItem = 3;
				_item.pszText = strText.GetBuffer();
				m_RandomOptionData.SetItem(&_item);
			}
			strText.Format("%d", _item.lParam);
			_item.iSubItem = 4;
			_item.pszText = strText.GetBuffer();
			m_RandomOptionData.SetItem(&_item);
			_indexItem++;
 
			const unsigned int _sizeValue(_vectorValue.size());
			for ( unsigned int _indexValue(0); _indexValue < _sizeValue; ++_indexValue )
			{
				const RandomOption::Value& _value(_vectorValue[_indexValue]);
				const float _fRateChoose(_value.getRateChoose());

				strText.Format("");
				_item.mask = LVIF_TEXT | LVIF_PARAM;
				_item.iSubItem = 0;
				_item.iItem = _indexItem++;
				switch ( _indexGenerate )
				{
				case RandomOption::GENERATE_DROP:
					_item.lParam = RandomOptionEdit::TYPE_TYPE_DROP;
					break;
				case RandomOption::GENERATE_REMODEL:
					_item.lParam = RandomOptionEdit::TYPE_TYPE_REMODEL;
					break;
				}
				_item.lParam |= (RandomOptionEdit::TYPE_INFORMATION_VALUE | (_typeRandomOption << 8) | _indexValue);
				_item.pszText = strText.GetBuffer();
				m_RandomOptionData.InsertItem(&_item);

				strText.Format("%.0f", _fRateChoose * 100.0f);
				_item.mask = LVIF_TEXT;
				_item.iSubItem = 1;
				_item.pszText = strText.GetBuffer();
				m_RandomOptionData.SetItem(&_item);


				int _valueIntegerHigh, _valueIntegerLow;
				float _fValueFloatHigh, _fValueFloatLow;
				if ( _bValueInteger == true )
				{
					_valueIntegerHigh = _value.getValueIntegerHigh();
					_valueIntegerLow = _value.getValueIntegerLow();

					strText.Format("%d", _valueIntegerLow);
					_item.iSubItem = 2;
					_item.pszText = strText.GetBuffer();
					m_RandomOptionData.SetItem(&_item);
					
					strText.Format("%d", _valueIntegerHigh);
					_item.iSubItem = 3;
					_item.pszText = strText.GetBuffer();
					m_RandomOptionData.SetItem(&_item);
				}
				else
				{
					_fValueFloatHigh = _value.getValueFloatHigh();
					_fValueFloatLow = _value.getValueFloatLow();

					strText.Format("%.4f%%", _fValueFloatLow);
					_item.iSubItem = 2;
					_item.pszText = strText.GetBuffer();
					m_RandomOptionData.SetItem(&_item);

					strText.Format("%.4f%%", _fValueFloatHigh);
					_item.iSubItem = 3;
					_item.pszText = strText.GetBuffer();
					m_RandomOptionData.SetItem(&_item);
				}

				strText.Format("%d", _item.lParam);
				_item.iSubItem = 4;
				_item.pszText = strText.GetBuffer();
 				m_RandomOptionData.SetItem(&_item);
			}
		}
	}
}