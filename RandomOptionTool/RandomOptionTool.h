#pragma once

#include "resource.h"
#include "./RandomOptionToolDefine.h"

namespace RandomOptionTool
{
	extern HWND hWindow;
	extern HWND hWindowMain; // handle tool-dialogue;
	extern HWND hWindowConvert;
	extern WNDPROC procedureListValueOld;
	extern WNDPROC procedureEditTypeOld;
	extern WNDPROC procedureEditRateOld;
	extern WNDPROC procedureEditLowOld;
	extern WNDPROC procedureEditHighOld;
	extern LPARAM paramLValueSelected;

	void setWindowText(const HWND _hWindow, const unsigned int _indexComponent, const char* const _cString);
	void getWindowText(const HWND _hWindow, const unsigned int _indexComponent, char* _cString, const unsigned int _length);

	void refreshList(void);
	void changeValue(void);

	void saveFile(void);
	void loadFile(void);
	void loadFileOld(const bool _bConvert = true);

	void selectItemList(const char* const _cName);
	void addRandomOptionEntity(void);
	void renameRandomOptionEntity(void);
	void removeRandomOptionEntity(void);
	void removeRandomOptionValue(void);

	void editInformationBasic(const char* const _cName, const unsigned int _paramL);
	void editTypeValueDrop(const char* const _cName, const unsigned int _paramL);
	void editTypeValueRemodel(const char* const _cName, const unsigned int _paramL);
}