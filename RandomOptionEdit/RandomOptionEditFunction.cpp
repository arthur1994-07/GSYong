
#include "stdafx.h"
#include "./RandomOptionEdit.h"
#include "./RandomOptionEditFunction.h"
#include "./RandomOptionEditDefine.h"
#include "../RanLogic/RandomOption/RandomOption.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"

namespace RandomOptionEdit
{
	HWND		hWindowMain = 0;
	HWND		hWindowConvert = 0;
	LPARAM		paramLValueSelected = 0xffffffffL;

	WNDPROC procedureEditTypeOld = 0;
	WNDPROC procedureEditRateOld = 0;
	WNDPROC procedureEditLowOld = 0;
	WNDPROC procedureEditHighOld = 0;

	SUBCLASSPROC subprocedureEditLowOld = 0;

	void editInformationBasic(const char* const _cName, const unsigned int _paramL)
	{
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		const RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
		if ( _pEntity == 0 )
			return;


		const HWND _handleWindow(RandomOptionEdit::hWindowMain);
		RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_0_NAME, TEXT("이름 :"));
		RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_1_RATE, TEXT("선택확률 :"));
		RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_2_MINIMUM, TEXT("파괴확률 :"));
		RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_3_MAXIMUM, TEXT("소비금액 :"));

		CString _stringOut;
		_stringOut.Format("%s", _cName);
		RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_0_NAME, _stringOut.GetBuffer());

		_stringOut.Format("%.0f", _pEntity->getRateRandomOptionGenerate());
		RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_1_RATE, _stringOut.GetBuffer());

		_stringOut.Format("%.0f", _pEntity->getRateRandomOptionDestroy());
		RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, _stringOut.GetBuffer());

		_stringOut.Format("%.0f", _pEntity->getRateMoneySpend());
		RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, _stringOut.GetBuffer());

		RandomOptionEdit::paramLValueSelected = _paramL;
	}	

	void editTypeValue(const RandomOption::GENERATE _typeGenerate, const char* const _cName, const unsigned int _paramL)
	{
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		const HWND _handleWindow(RandomOptionEdit::hWindowMain);

		const RandomOption::Entity* const _pEntity(_managerRandomOption.find(_cName));
		if ( _pEntity == 0 )
		{
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
			return;
		}

		const RandomOption::RANDOM_OPTION _typeRandomOption(RandomOption::RANDOM_OPTION((_paramL >> 8) & 0xffL));
		if ( _typeRandomOption >= RandomOption::RANDOM_OPTION_NSIZE )
		{
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
			return;
		}

		RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_1_RATE, TEXT("확률 :"));
		RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_2_MINIMUM, TEXT("최소 :"));
		RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_3_MAXIMUM, TEXT("최대 :"));

		CString _stringOut;
		_stringOut.Format("%s", RandomOption::cNameValue[_typeRandomOption]);
		RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_0_NAME, _stringOut.GetBuffer());

		RandomOption::Information _informationReference;
		switch ( _paramL & 0x00ff0000L )
		{
		case RandomOptionEdit::TYPE_INFORMATION_TYPE:
			{
				RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_0_NAME, TEXT("종류 :"));

				if ( _pEntity->getInformation(_typeGenerate, _typeRandomOption, _informationReference) == false )
				{
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
					return;
				}

				_stringOut.Format("%.0f", _informationReference.fRateChoose);
				RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_1_RATE, _stringOut.GetBuffer());
			}
			break;
		case RandomOptionEdit::TYPE_INFORMATION_VALUE:
			{
				const unsigned int _indexValue(_paramL & 0xffL);
				RandomOptionEdit::setWindowText(_handleWindow, IDC_STATIC_0_NAME, TEXT("종류(값) :"));

				if ( _pEntity->getInformation(_typeGenerate, _typeRandomOption, _indexValue, _informationReference) == false )
				{
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_0_NAME, "error");
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_1_RATE, "error");
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, "error");
					RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, "error");
					return;
				}

				_stringOut.Format("%.0f", _informationReference.fRateChoose * 100.0f);
				RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_1_RATE, _stringOut.GetBuffer());
			}
			break;
		}		



		if ( _informationReference.bValueInteger == true )
		{
			_stringOut.Format("%d", _informationReference.valueIntegerLow);
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, _stringOut.GetBuffer());

			_stringOut.Format("%d", _informationReference.valueIntegerHigh);
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, _stringOut.GetBuffer());
		}
		else
		{
			_stringOut.Format("%.4f", _informationReference.valueFloatLow);
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, _stringOut.GetBuffer());

			_stringOut.Format("%.4f", _informationReference.valueFloatHigh);
			RandomOptionEdit::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, _stringOut.GetBuffer());
		}

		RandomOptionEdit::paramLValueSelected = _paramL;
	}

	void editTypeValueDrop(const char* const _cName, const unsigned int _paramL)
	{
		RandomOptionEdit::editTypeValue(RandomOption::GENERATE_DROP, _cName, _paramL);
	}

	void editTypeValueRemodel(const char* const _cName, const unsigned int _paramL)
	{
		RandomOptionEdit::editTypeValue(RandomOption::GENERATE_REMODEL, _cName, _paramL);
	}

	void setWindowText(const HWND _handleWindow, const unsigned int _indexComponent, const char* const _cString)
	{
		const HWND _handleDialogueList(::GetDlgItem(_handleWindow, _indexComponent));
		SetWindowText(_handleDialogueList, _cString);
	}
};