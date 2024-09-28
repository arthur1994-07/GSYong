#include "stdafx.h"
#include "./RandomOptionTool.h"
#include "../RanLogic/RandomOption/RandomOption.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"

namespace RandomOptionTool
{
	HTREEITEM handleTreeItemBasic = 0;
	HTREEITEM handleTreeItem[RandomOption::GENERATE_NSIZE] = {0, 0};

	void setWindowText(const HWND _handleWindow, const unsigned int _indexComponent, const char* const _cString)
	{
		const HWND _handleDialogueList(::GetDlgItem(_handleWindow, _indexComponent));
		SetWindowText(_handleDialogueList, _cString);
	}
	void getWindowText(const HWND _handleWindow, const unsigned int _indexComponent, char* _cString, const unsigned int _length)
	{
		const HWND _handleDialogueEdit(::GetDlgItem(_handleWindow, _indexComponent));
		::GetWindowText(_handleDialogueEdit, _cString, _length);
	}
	
	void refreshList(void)
	{
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		const RandomOption::EntityListMap& _mapEntityList(_managerRandomOption.getMapEntityList());

		const HWND _handleWindow(RandomOptionTool::hWindowMain);

		char _cKeywordSearch[RandomOption::LENGTH_NAME] = { 0,};
		RandomOptionTool::getWindowText(_handleWindow, IDC_EDIT_SEARCH, _cKeywordSearch, RandomOption::LENGTH_NAME);
		const std::string _stringKeywordSearch(_cKeywordSearch);

		const HWND _handleDialogueListRandomOption(::GetDlgItem(_handleWindow, IDC_LIST_RANDOM_OPTION));

		const unsigned int _sizeRandomOption(::SendMessage(_handleDialogueListRandomOption, LB_GETCOUNT, 0, 0));
		for ( unsigned int _count(_sizeRandomOption); _count--; )
			::SendMessage(_handleDialogueListRandomOption, LB_DELETESTRING, 0, 0);

		for ( RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.begin()); _iteratorEntityList != _mapEntityList.end(); ++_iteratorEntityList )
		{
			const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
			for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
			{
				const RandomOption::Entity& _entity(*_iteratorEntity);
				const std::string _stringNameEntity(_entity.getName());

				if ( sc::string::FindWord(_stringNameEntity, _stringKeywordSearch) == unsigned int(-1) )
					continue;

				::SendMessage(_handleDialogueListRandomOption, LB_ADDSTRING, 0, (LPARAM)_entity.getName());
			}				
		}

		const HWND _handleListRandomOptionType(::GetDlgItem(_handleWindow, IDC_LIST_RANDOM_OPTION_VALUE));

		const unsigned int _sizeRandomOptionValue(::SendMessage(_handleListRandomOptionType, LB_GETCOUNT, 0, 0));
		for ( unsigned int _count(_sizeRandomOptionValue); _count--; )
			::SendMessage(_handleListRandomOptionType, LB_DELETESTRING, 0, 0);

		for ( unsigned int _index(0); _index < RandomOption::RANDOM_OPTION_NSIZE; ++_index )
			::SendMessage(_handleListRandomOptionType, LB_ADDSTRING, 0, (LPARAM)RandomOption::cNameValue[_index]);

		RandomOptionTool::paramLValueSelected = 0xffffffffL;
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "");

		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_0_NAME, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_1_RATE, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_2_MINIMUM, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_3_MAXIMUM, "");
	}
	void changeValue(void)
	{
		const HWND _handleWindow(RandomOptionTool::hWindowMain);
		const LPARAM _paramLValueSelected(RandomOptionTool::paramLValueSelected);
		if ( _paramLValueSelected == LPARAM(0xffffffffL) )
			return;

		const HWND _handleListValue(::GetDlgItem(_handleWindow, IDC_LIST_RANDOM_OPTION));
		const unsigned int _indexList(SendMessage(_handleListValue, LB_GETCURSEL, 0, 0));

		char _cName[RandomOption::LENGTH_NAME];
		SendMessage(_handleListValue, LB_GETTEXT, _indexList, (LPARAM)_cName);

		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		RandomOption::EntityListMap& _mapEntityList(_managerRandomOption.getMapEntityList());

		RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
		if ( _pEntity == 0 )
			return;

		const RandomOptionTool::TYPE _type(RandomOptionTool::TYPE(_paramLValueSelected & 0xff000000L));
		switch ( _type )
		{
		case RandomOptionTool::TYPE_INFORMATION_BASIC:
			break;
		case RandomOptionTool::TYPE_TYPE_DROP:
		case RandomOptionTool::TYPE_TYPE_REMODEL:
			{
				char _cType[RandomOption::LENGTH_NAME] = {0, };
				char _cRate[RandomOption::LENGTH_NAME] = {0, };
				char _cMinimum[RandomOption::LENGTH_NAME] = {0, };
				char _cMaximum[RandomOption::LENGTH_NAME] = {0, };

				RandomOptionTool::getWindowText(_handleWindow, IDC_EDIT_0_NAME, _cType, RandomOption::LENGTH_NAME);
				RandomOptionTool::getWindowText(_handleWindow, IDC_EDIT_1_RATE, _cRate, RandomOption::LENGTH_NAME);
				RandomOptionTool::getWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, _cMinimum, RandomOption::LENGTH_NAME);
				RandomOptionTool::getWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, _cMaximum, RandomOption::LENGTH_NAME);

				const RandomOption::RANDOM_OPTION _typeRandomOption(RandomOption::RANDOM_OPTION((_paramLValueSelected >> 8) & 0xffL));
				const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

				RandomOption::Information _information;
				_information.bValueInteger = _bValueInteger;				
				if ( _bValueInteger == true )
				{
					_information.valueIntegerLow = ::atoi(_cMinimum);
					_information.valueIntegerHigh = ::atoi(_cMaximum);
				}
				else
				{
					_information.valueFloatLow = ::atof(_cMinimum);
					_information.valueFloatHigh = ::atof(_cMaximum);
				}
				
				const RandomOption::GENERATE _typeGenerate(_type == RandomOptionTool::TYPE_TYPE_DROP ? RandomOption::GENERATE_DROP : RandomOption::GENERATE_REMODEL);
				switch ( _paramLValueSelected & 0x00ff0000L )
				{					
				case RandomOptionTool::TYPE_INFORMATION_TYPE:
					break;
				case RandomOptionTool::TYPE_INFORMATION_VALUE:
					{
						const HWND _handleListValue(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));
						const unsigned int _indexValue(_paramLValueSelected & 0xffL);
						_information.fRateChoose = ::atof(_cRate) * 0.01f;

						const int _indexItem(ListView_GetNextItem(_handleListValue, -1, LVNI_ALL | LVNI_SELECTED));
						if ( _indexItem == -1 )
							break;
						else if ( _pEntity->setInformation(_typeGenerate, _typeRandomOption, _indexValue, _information) == true )
						{
							CString _stringOut;

							ListView_SetItemText(_handleListValue, _indexItem, 1, _cRate);
							if ( _bValueInteger == true )
							{
								ListView_SetItemText(_handleListValue, _indexItem, 2, _cMinimum);
								ListView_SetItemText(_handleListValue, _indexItem, 3, _cMaximum);
							}
							else
							{								
								_stringOut.Format(TEXT("%s%%"), _cMinimum);
								ListView_SetItemText(_handleListValue, _indexItem, 2, _stringOut.GetBuffer());
								_stringOut.Format(TEXT("%s%%"), _cMaximum);
								ListView_SetItemText(_handleListValue, _indexItem, 3, _stringOut.GetBuffer());
							}


							RandomOption::Information _informationReference;
							_pEntity->getInformation(_typeGenerate, _typeRandomOption, _informationReference);

							const unsigned int _indexValueBunch(_indexItem - _indexValue - 1);
							ListView_SetItemText(_handleListValue, _indexValueBunch, 0, _cType);
							_stringOut.Format(TEXT("%.0f%%"), _informationReference.fRateChoose);
							ListView_SetItemText(_handleListValue, _indexValueBunch, 1, _stringOut.GetBuffer());
							if ( _bValueInteger == true )
							{
								_stringOut.Format(TEXT("%d"), _informationReference.valueIntegerLow);
								ListView_SetItemText(_handleListValue, _indexValueBunch, 2, _stringOut.GetBuffer());
								_stringOut.Format(TEXT("%d"), _informationReference.valueIntegerHigh);
								ListView_SetItemText(_handleListValue, _indexValueBunch, 3, _stringOut.GetBuffer());
							}
							else
							{
								_stringOut.Format(TEXT("%.2f%%"), _informationReference.valueFloatLow);
								ListView_SetItemText(_handleListValue, _indexValueBunch, 2, _stringOut.GetBuffer());
								_stringOut.Format(TEXT("%.2f%%"), _informationReference.valueFloatHigh);
								ListView_SetItemText(_handleListValue, _indexValueBunch, 3, _stringOut.GetBuffer());
							}
						}
					}
					break;
				}
			}
			break;
		}
	}
	void loadFile(void)
	{
		if ( RandomOption::LoadFile(RandomOption::LOAD_TOOL) == E_FAIL )
		{
			::MessageBox(0, TEXT("loadFile() : 불러오기에 실패 하였습니다;"), TEXT("실패"), MB_OK);
			return;
		}
		
		RandomOptionTool::refreshList();
	}
	void loadFileOld(const bool _bConvert)
	{
		if ( RandomOption::LoadFileOld(_bConvert) == E_FAIL )
		{
			::MessageBox(0, TEXT("loadFileOld() : 불러오기에 실패 하였습니다;"), TEXT("실패"), MB_OK);
			return;
		}

		RandomOptionTool::refreshList();
	}
	void saveFile(void)
	{
		/*
		BROWSEINFO bi;

		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = GetSafeHwnd();
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		TCHAR szPath[MAX_PATH] = {0};
		SHGetPathFromIDList(pidl, szPath);

		if (_tchdir(szPath) != 0)
		{
		MessageBox(NULL, _T("잘못되었거나 접근할 수 없는 경로입니다."),
		_T("오류"), MB_OK);
		return;
		}
		 */
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		if ( _managerRandomOption.saveFile() == false )
			::MessageBox(0, TEXT("saveFile() : 저장에 싱패 하였습니다;"), TEXT("실패"), MB_OK);
		else
			::MessageBox(0, TEXT("saveFile() : 저장에 성공 하였습니다;"), TEXT("성공"), MB_OK);
	}
	void selectItemList(const char* const _cName)
	{
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
		if ( _pEntity == 0 )
			return;

		const HWND _handleWindow(RandomOptionTool::hWindowMain);

		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_RANDOM_OPTION_NAME, _cName);		

		const HWND _handleListEntity(::GetDlgItem(_handleWindow, IDC_LIST_RANDOM_OPTION_ENTITY));

		::SendMessage(_handleListEntity, LVM_DELETEALLITEMS, 0, 0);				

		CString _string;
		LVITEM _item;

		const float _fRateGenerate(_pEntity->getRateRandomOptionGenerate());
		const float _fRateDestroy(_pEntity->getRateRandomOptionDestroy());
		const float _fRateMoneySpend(_pEntity->getRateMoneySpend());				

		unsigned int _indexItem(0);

		_string.Format("%s", _cName);
		_item.mask = LVIF_TEXT | LVIF_PARAM;
		_item.state = 0;
		_item.stateMask = 0;
		_item.iImage = -1;
		_item.iSubItem = 0;
		_item.iItem = _indexItem++;
		_item.lParam = RandomOptionTool::TYPE_INFORMATION_BASIC;
		_item.pszText = _string.GetBuffer();
		SendMessage(_handleListEntity, LVM_INSERTITEM, 0, (LPARAM)&_item);

		_string.Format("발생확률:%.0f%%", _fRateGenerate);
		_item.mask = LVIF_TEXT;
		_item.iSubItem = 1;
		_item.pszText = _string.GetBuffer();
		SendMessage(_handleListEntity, LVM_SETITEM, 1, (LPARAM)&_item);

		_string.Format("파괴확률:%.0f%%", _fRateDestroy);
		_item.iSubItem = 2;
		_item.pszText = _string.GetBuffer();
		SendMessage(_handleListEntity, LVM_SETITEM, 2, (LPARAM)&_item);

		_string.Format("소비금액:%.0f%%", _fRateMoneySpend);
		_item.iSubItem = 3;
		_item.pszText = _string.GetBuffer();
		SendMessage(_handleListEntity, LVM_SETITEM, 3, (LPARAM)&_item);

		_string.Format("%d", _item.lParam);
		_item.iSubItem = 4;
		_item.pszText = _string.GetBuffer();
		SendMessage(_handleListEntity, LVM_SETITEM, 4, (LPARAM)&_item);

		for ( unsigned int _indexGenerate(0); _indexGenerate < RandomOption::GENERATE_NSIZE; ++_indexGenerate )
		{
			const RandomOption::ValueBunchVector& _vectorValueBunch(*_pEntity->getVectorValueBunch(RandomOption::GENERATE(_indexGenerate)));
			const unsigned int _sizeValueBunch(_vectorValueBunch.size());

			_string.Format("[%s : %d개]", RandomOption::cNameGenerate[_indexGenerate], _sizeValueBunch);
			_item.mask = LVIF_TEXT;
			_item.iSubItem = 0;
			_item.iItem = _indexItem++;
			_item.pszText = _string.GetBuffer();
			SendMessage(_handleListEntity, LVM_INSERTITEM, 0, (LPARAM)&_item);

			_string.Format("-------------------------");
			_item.iSubItem = 1;
			_item.pszText = _string.GetBuffer();
			SendMessage(_handleListEntity, LVM_SETITEM, 1, (LPARAM)&_item);
			_item.iSubItem = 2;			
			SendMessage(_handleListEntity, LVM_SETITEM, 2, (LPARAM)&_item);
			_item.iSubItem = 3;			
			SendMessage(_handleListEntity, LVM_SETITEM, 3, (LPARAM)&_item);
			_item.iSubItem = 4;			
			SendMessage(_handleListEntity, LVM_SETITEM, 4, (LPARAM)&_item);


			for ( unsigned int _indexValueBunch(0); _indexValueBunch < _sizeValueBunch; ++_indexValueBunch )
			{
				const RandomOption::ValueBunch& _valueBunch(_vectorValueBunch[_indexValueBunch]);

				const float _fRateChoose(_valueBunch.getRateChoose());
				const RandomOption::RANDOM_OPTION _typeRandomOption(_valueBunch.getTypeRandomOption());
				const RandomOption::ValueVector& _vectorValue(_valueBunch.getValueVector());

				const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

				_indexGenerate == RandomOption::GENERATE_DROP ? RandomOptionTool::TYPE_TYPE_DROP : RandomOptionTool::TYPE_TYPE_REMODEL;

				_string.Format("%s", RandomOption::cNameValue[_typeRandomOption]);
				_item.mask = LVIF_TEXT | LVIF_PARAM;
				_item.iSubItem = 0;
				_item.iItem = _indexItem++;
				switch ( _indexGenerate )
				{
				case RandomOption::GENERATE_DROP:
					_item.lParam = RandomOptionTool::TYPE_TYPE_DROP;
					break;
				case RandomOption::GENERATE_REMODEL:
					_item.lParam = RandomOptionTool::TYPE_TYPE_REMODEL;
					break;
				}
				_item.lParam |= (RandomOptionTool::TYPE_INFORMATION_TYPE | (_typeRandomOption << 8));
				_item.pszText = _string.GetBuffer();
				SendMessage(_handleListEntity, LVM_INSERTITEM, 0, (LPARAM)&_item);

				_string.Format("%.0f%%", _fRateChoose);
				_item.mask = LVIF_TEXT;
				_item.iSubItem = 1;
				_item.pszText = _string.GetBuffer();
				SendMessage(_handleListEntity, LVM_SETITEM, 1, (LPARAM)&_item);

				if ( _bValueInteger == true )
				{
					const int _valueIntegerHigh(_valueBunch.getValueIntegerHigh());
					const int _valueIntegerLow(_valueBunch.getValueIntegerLow());						

					_string.Format("%d", _valueIntegerLow);
					_item.iSubItem = 2;
					_item.pszText = _string.GetBuffer();
					SendMessage(_handleListEntity, LVM_SETITEM, 2, (LPARAM)&_item);

					_string.Format("%d", _valueIntegerHigh);
					_item.iSubItem = 3;
					_item.pszText = _string.GetBuffer();
					SendMessage(_handleListEntity, LVM_SETITEM, 3, (LPARAM)&_item);
				}
				else
				{
					const float _fValueFloatHigh(_valueBunch.getValueFloatHigh());
					const float _fValueFloatLow(_valueBunch.getValueFloatLow());

					_string.Format("%.2f%%", _fValueFloatLow);
					_item.iSubItem = 2;
					_item.pszText = _string.GetBuffer();
					SendMessage(_handleListEntity, LVM_SETITEM, 2, (LPARAM)&_item);

					_string.Format("%.2f%%", _fValueFloatHigh);
					_item.iSubItem = 3;
					_item.pszText = _string.GetBuffer();
					SendMessage(_handleListEntity, LVM_SETITEM, 3, (LPARAM)&_item);
				}
				_string.Format("%d", _item.lParam);
				_item.iSubItem = 4;
				_item.pszText = _string.GetBuffer();
				SendMessage(_handleListEntity, LVM_SETITEM, 4, (LPARAM)&_item);

				const unsigned int _sizeValue(_vectorValue.size());
				for ( unsigned int _indexValue(0); _indexValue < _sizeValue; ++_indexValue )
				{
					const RandomOption::Value& _value(_vectorValue[_indexValue]);
					const float _fRateChoose(_value.getRateChoose());

					_string.Format("");
					_item.mask = LVIF_TEXT | LVIF_PARAM;
					_item.iSubItem = 0;
					_item.iItem = _indexItem++;
					switch ( _indexGenerate )
					{
					case RandomOption::GENERATE_DROP:
						_item.lParam = RandomOptionTool::TYPE_TYPE_DROP;
						break;
					case RandomOption::GENERATE_REMODEL:
						_item.lParam = RandomOptionTool::TYPE_TYPE_REMODEL;
						break;
					}
					_item.lParam |= (RandomOptionTool::TYPE_INFORMATION_VALUE | (_typeRandomOption << 8) | _indexValue);
					_item.pszText = _string.GetBuffer();
					SendMessage(_handleListEntity, LVM_INSERTITEM, 0, (LPARAM)&_item);

					_string.Format("%.0f", _fRateChoose * 100.0f);
					_item.mask = LVIF_TEXT;
					_item.iSubItem = 1;
					_item.pszText = _string.GetBuffer();
					SendMessage(_handleListEntity, LVM_SETITEM, 1, (LPARAM)&_item);


					int _valueIntegerHigh, _valueIntegerLow;
					float _fValueFloatHigh, _fValueFloatLow;
					if ( _bValueInteger == true )
					{
						_valueIntegerHigh = _value.getValueIntegerHigh();
						_valueIntegerLow = _value.getValueIntegerLow();

						_string.Format("%d", _valueIntegerLow);
						_item.iSubItem = 2;
						_item.pszText = _string.GetBuffer();
						SendMessage(_handleListEntity, LVM_SETITEM, 2, (LPARAM)&_item);

						_string.Format("%d", _valueIntegerHigh);
						_item.iSubItem = 3;
						_item.pszText = _string.GetBuffer();
						SendMessage(_handleListEntity, LVM_SETITEM, 3, (LPARAM)&_item);
					}
					else
					{
						_fValueFloatHigh = _value.getValueFloatHigh();
						_fValueFloatLow = _value.getValueFloatLow();

						_string.Format("%.2f%%", _fValueFloatLow);
						_item.iSubItem = 2;
						_item.pszText = _string.GetBuffer();
						SendMessage(_handleListEntity, LVM_SETITEM, 2, (LPARAM)&_item);

						_string.Format("%.2f%%", _fValueFloatHigh);
						_item.iSubItem = 3;
						_item.pszText = _string.GetBuffer();
						SendMessage(_handleListEntity, LVM_SETITEM, 3, (LPARAM)&_item);
					}

					_string.Format("%d", _item.lParam);
					_item.iSubItem = 4;
					_item.pszText = _string.GetBuffer();
					SendMessage(_handleListEntity, LVM_SETITEM, 4, (LPARAM)&_item);
				}
			}
		}

		RandomOptionTool::paramLValueSelected = 0xffffffffL;
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "");

		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_0_NAME, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_1_RATE, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_2_MINIMUM, "");
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_3_MAXIMUM, "");
	}

	void addRandomOptionEntity(void)
	{
		const HWND _handleDialogueEditName(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_RANDOM_OPTION_NAME));
		char _cName[RandomOption::LENGTH_NAME] = { 0,};
		::GetWindowText(_handleDialogueEditName, _cName, RandomOption::LENGTH_NAME);
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

			const HWND _handleDialogueList(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION));
			::SendMessage(_handleDialogueList, LB_ADDSTRING, 0, (LPARAM)_stringName.c_str());
//			RandomOptionTool::refreshList();
		}
		else
			::MessageBox(0, TEXT("중복 된 이름 입니다;"), _stringName.c_str(), MB_OK);
	}
	void renameRandomOptionEntity(void)
	{
		const HWND _handleDialogueList(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION));
		const HWND _handleDialogueEdit(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_RANDOM_OPTION_NAME));
		const unsigned int _sizeSelected(SendMessage(_handleDialogueList, LB_GETSELCOUNT, 0, 0));
		if ( _sizeSelected == 0 )
		{
			::MessageBox(0, TEXT("이름을 변경 할 아이템들을 선택 해 주십시요;"), TEXT("실패"), MB_OK);
			return;
		}

		const unsigned int _indexList(SendMessage(_handleDialogueList, LB_GETCURSEL, 0, 0));

		char _cNameTarget[RandomOption::LENGTH_NAME];
		char _cNameSource[RandomOption::LENGTH_NAME];
		SendMessage(_handleDialogueList, LB_GETTEXT, _indexList, (LPARAM)_cNameTarget);		
		GetWindowText(_handleDialogueEdit, _cNameSource, RandomOption::LENGTH_NAME);

		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		RandomOption::Entity* const _pEntity(_managerRandomOption.rename(_cNameTarget, _cNameSource));
		if ( _pEntity == 0 )
			::MessageBox(0, TEXT("선택된 아이템이 없거나, 유효한 이름이 아닙니다;"), TEXT("오류"), MB_OK);
		else if ( std::string(_pEntity->getName()) == std::string(_cNameTarget) )
			::MessageBox(0, TEXT("중복 된 이름 입니다;"), TEXT("오류"), MB_OK);
		else
			RandomOptionTool::refreshList();
	}
	void removeRandomOptionEntity(void)
	{
		const HWND _handleListRandomOption(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION));
		const unsigned int _sizeSelected(SendMessage(_handleListRandomOption, LB_GETSELCOUNT, 0, 0));
		if ( _sizeSelected == 0 )
		{
			::MessageBox(0, TEXT("삭제 할 아이템들을 선택 해 주십시요;"), TEXT("실패"), MB_OK);
			return;
		}

		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());

		unsigned int* _pIDItems(new unsigned int[_sizeSelected]);
		SendMessage(_handleListRandomOption, LB_GETSELITEMS, (WPARAM)_sizeSelected, (LPARAM)_pIDItems);
		for ( unsigned int _index(_sizeSelected); _index--; )
		{
			char _cName[RandomOption::LENGTH_NAME];
			SendMessage(_handleListRandomOption, LB_GETTEXT, _pIDItems[_index], (LPARAM)_cName);
			_managerRandomOption.remove(_cName);
		}

		delete[] _pIDItems;

		RandomOptionTool::refreshList();
	}
	void removeRandomOptionValue(void)
	{
		const HWND _handleWindow(RandomOptionTool::hWindowMain);

		const HWND _handleListEntity(::GetDlgItem(_handleWindow, IDC_LIST_RANDOM_OPTION));
		const unsigned int _indexList(SendMessage(_handleListEntity, LB_GETCURSEL, 0, 0));

		char _cName[RandomOption::LENGTH_NAME];
		SendMessage(_handleListEntity, LB_GETTEXT, _indexList, (LPARAM)_cName);

		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		RandomOption::EntityListMap& _mapEntityList(_managerRandomOption.getMapEntityList());

		RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
		if ( _pEntity == 0 )
			return;

		const HWND _handleListValue(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));

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
				const RandomOptionTool::TYPE _type(RandomOptionTool::TYPE(_paramL & 0xff000000L));
				const RandomOption::RANDOM_OPTION _typeRandomOption(RandomOption::RANDOM_OPTION((_paramL >> 8) & 0xffL));
				const unsigned int _indexValue(_paramL & 0xffL);
				switch ( _type )
				{
				case RandomOptionTool::TYPE_INFORMATION_BASIC:
					break;
				case RandomOptionTool::TYPE_TYPE_DROP:
				case RandomOptionTool::TYPE_TYPE_REMODEL:
					{
						const RandomOption::GENERATE _typeGenerate(_type == RandomOptionTool::TYPE_TYPE_DROP ? RandomOption::GENERATE_DROP : RandomOption::GENERATE_REMODEL);
						switch ( _paramL & 0x00ff0000L )
						{					
						case RandomOptionTool::TYPE_INFORMATION_TYPE:
							_pEntity->removeValueBunch(_typeGenerate, _typeRandomOption);
							break;
						case RandomOptionTool::TYPE_INFORMATION_VALUE:
							_pEntity->removeValue(_typeGenerate, _typeRandomOption, _indexValue);
							break;
						}
					}
					break;
				}
			}

			_indexItemPrevious = _indexItem;
		}

//		RandomOptionTool::refreshList();

//		::SendMessage(_handleListEntity, LB_SETCURSEL, _indexList, 0);
		RandomOptionTool::selectItemList(_cName);
	}
	void editInformationBasic(const char* const _cName, const unsigned int _paramL)
	{
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		const RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
		if ( _pEntity == 0 )
			return;

		const HWND _handleWindow(RandomOptionTool::hWindowMain);
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_0_NAME, TEXT("이름 :"));
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_1_RATE, TEXT("선택확률 :"));
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_2_MINIMUM, TEXT("파괴확률 :"));
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_3_MAXIMUM, TEXT("소비금액 :"));

		CString _stringOut;
		_stringOut.Format("%s", _cName);
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, _stringOut.GetBuffer());

		_stringOut.Format("%.0f", _pEntity->getRateRandomOptionGenerate());
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, _stringOut.GetBuffer());

		_stringOut.Format("%.0f", _pEntity->getRateRandomOptionDestroy());
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, _stringOut.GetBuffer());

		_stringOut.Format("%.0f", _pEntity->getRateMoneySpend());
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, _stringOut.GetBuffer());

		RandomOptionTool::paramLValueSelected = _paramL;
	}	
	void editTypeValue(const RandomOption::GENERATE _typeGenerate, const char* const _cName, const unsigned int _paramL)
	{
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		const HWND _handleWindow(RandomOptionTool::hWindowMain);

		const RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
		if ( _pEntity == 0 )
		{
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
			return;
		}

		const RandomOption::RANDOM_OPTION _typeRandomOption(RandomOption::RANDOM_OPTION((_paramL >> 8) & 0xffL));
		if ( _typeRandomOption >= RandomOption::RANDOM_OPTION_NSIZE )
		{
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
			return;
		}
		
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_1_RATE, TEXT("확률 :"));
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_2_MINIMUM, TEXT("최소 :"));
		RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_3_MAXIMUM, TEXT("최대 :"));

		CString _stringOut;
		_stringOut.Format("%s", RandomOption::cNameValue[_typeRandomOption]);
		RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, _stringOut.GetBuffer());

		RandomOption::Information _informationReference;
		switch ( _paramL & 0x00ff0000L )
		{
		case RandomOptionTool::TYPE_INFORMATION_TYPE:
			{
				RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_0_NAME, TEXT("종류 :"));

				if ( _pEntity->getInformation(_typeGenerate, _typeRandomOption, _informationReference) == false )
				{
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
					return;
				}

				_stringOut.Format("%.0f", _informationReference.fRateChoose);
				RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, _stringOut.GetBuffer());
			}
			break;
		case RandomOptionTool::TYPE_INFORMATION_VALUE:
			{
				const unsigned int _indexValue(_paramL & 0xffL);
				RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_0_NAME, TEXT("종류(값) :"));

				if ( _pEntity->getInformation(_typeGenerate, _typeRandomOption, _indexValue, _informationReference) == false )
				{
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
					RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
					return;
				}

				_stringOut.Format("%.0f", _informationReference.fRateChoose * 100.0f);
				RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, _stringOut.GetBuffer());
			}
			break;
		}		

		

		if ( _informationReference.bValueInteger == true )
		{
			_stringOut.Format("%d", _informationReference.valueIntegerLow);
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, _stringOut.GetBuffer());

			_stringOut.Format("%d", _informationReference.valueIntegerHigh);
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, _stringOut.GetBuffer());
		}
		else
		{
			_stringOut.Format("%.2f", _informationReference.valueFloatLow);
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, _stringOut.GetBuffer());

			_stringOut.Format("%.2f", _informationReference.valueFloatHigh);
			RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, _stringOut.GetBuffer());
		}

		RandomOptionTool::paramLValueSelected = _paramL;
	}	
	void editTypeValueDrop(const char* const _cName, const unsigned int _paramL)
	{
		RandomOptionTool::editTypeValue(RandomOption::GENERATE_DROP, _cName, _paramL);
	}
	void editTypeValueRemodel(const char* const _cName, const unsigned int _paramL)
	{
		RandomOptionTool::editTypeValue(RandomOption::GENERATE_REMODEL, _cName, _paramL);
	}
}