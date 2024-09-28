
#pragma once

namespace RandomOptionEdit
{
	extern HWND		hWindowMain;
	extern HWND		hWindowConvert;
	extern LPARAM	paramLValueSelected;

	extern WNDPROC procedureEditTypeOld;
	extern WNDPROC procedureEditRateOld;
	extern WNDPROC procedureEditLowOld;
	extern WNDPROC procedureEditHighOld;
	
	extern SUBCLASSPROC subprocedureEditLowOld;

	void editInformationBasic(const char* const _cName, const unsigned int _paramL);
	void editTypeValueDrop(const char* const _cName, const unsigned int _paramL);
	void editTypeValueRemodel(const char* const _cName, const unsigned int _paramL);

	void setWindowText(const HWND _handleWindow, const unsigned int _indexComponent, const char* const _cString);
};