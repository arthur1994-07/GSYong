// RandomOptionTool.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "./RandomOptionTool.h"
#include "../RanLogic/RandomOption/RandomOption.h"

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
namespace RandomOptionTool
{
	HWND hWindow = 0;
	HWND hWindowMain = 0;
	HWND hWindowConvert = 0;
	WNDPROC procedureListValueOld = 0;
	WNDPROC procedureEditTypeOld = 0;
	WNDPROC procedureEditRateOld = 0;
	WNDPROC procedureEditLowOld = 0;
	WNDPROC procedureEditHighOld = 0;
	LPARAM paramLValueSelected = 0xffffffffL;

	RandomOption::Entity* pEntity = 0;
}
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	windowProcedureRandomOption(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK dialogueProcedureMain(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL);
LRESULT CALLBACK dialogueProcedureConvert(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL);

LRESULT CALLBACK listValueProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL);
LRESULT CALLBACK editTypeProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL);
LRESULT CALLBACK editRateProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL);
LRESULT CALLBACK editLowProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL);
LRESULT CALLBACK editHighProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RANDOMOPTIONTOOL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RANDOMOPTIONTOOL));

	// �⺻ �޽��� �����Դϴ�.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
//  ����:
//
//    Windows 95���� �߰��� 'RegisterClassEx' �Լ����� ����
//    �ش� �ڵ尡 Win32 �ý��۰� ȣȯ�ǵ���
//    �Ϸ��� ��쿡�� �� �Լ��� ����մϴ�. �� �Լ��� ȣ���ؾ�
//    �ش� ���� ���α׷��� �����
//    '�ùٸ� ������' ���� �������� ������ �� �ֽ��ϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= windowProcedureRandomOption;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RANDOMOPTIONTOOL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_RANDOMOPTIONTOOL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   RandomOptionTool::hWindow = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if ( RandomOptionTool::hWindow == 0 )
      return FALSE;

   RandomOptionTool::hWindowMain = ::CreateDialog(hInstance, MAKEINTRESOURCE(IDD_RANDOMOPTION_TOOL), RandomOptionTool::hWindow, (DLGPROC)dialogueProcedureMain);
   if ( RandomOptionTool::hWindowMain == 0 )
	   return FALSE;

   RandomOptionTool::hWindowConvert = ::CreateDialog(hInstance, MAKEINTRESOURCE(IDD_RANDOMOPTION_CONVERT), RandomOptionTool::hWindowMain, (DLGPROC)dialogueProcedureConvert);
   if ( RandomOptionTool::hWindowConvert == 0 )
	   return FALSE;

   ShowWindow(RandomOptionTool::hWindow, SW_HIDE);
   ShowWindow(RandomOptionTool::hWindowMain, SW_SHOW);
   ShowWindow(RandomOptionTool::hWindowConvert, SW_HIDE);

   UpdateWindow(RandomOptionTool::hWindow);
   UpdateWindow(RandomOptionTool::hWindowMain);
   UpdateWindow(RandomOptionTool::hWindowConvert);   

   {
	   const HWND _handleListEntity(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));

	   LVCOLUMN _column;
	   _column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	   _column.fmt = LVCFMT_CENTER;

	   _column.pszText  = TEXT("����");
	   _column.iSubItem = 0;
	   _column.cx = 196;
	   SendMessage(_handleListEntity, LVM_INSERTCOLUMN, 0, (LPARAM)&_column);

	   _column.pszText = TEXT("Ȯ��");
	   _column.iSubItem = 1;
	   _column.cx = 100;
	   SendMessage(_handleListEntity, LVM_INSERTCOLUMN, 1, (LPARAM)&_column);   

	   _column.pszText = TEXT("�ּҰ�");
	   _column.iSubItem = 2;
	   _column.cx = 100;
	   SendMessage(_handleListEntity, LVM_INSERTCOLUMN, 2, (LPARAM)&_column);   

	   _column.pszText = TEXT("�ִ밪");
	   _column.iSubItem = 3;
	   _column.cx = 100;
	   SendMessage(_handleListEntity, LVM_INSERTCOLUMN, 3, (LPARAM)&_column);

	   _column.pszText = TEXT("identification-key");
	   _column.iSubItem = 4;
	   _column.cx = 1;
	   SendMessage(_handleListEntity, LVM_INSERTCOLUMN, 4, (LPARAM)&_column);

	   SendMessage(_handleListEntity, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
   }

   {
	   const HWND _handleListValue(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));
	   const HWND _handleEditType(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_0_NAME));
	   const HWND _handleEditRate(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_1_RATE));
	   const HWND _handleEditLow(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_2_MINIMUM));
	   const HWND _handleEditHigh(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_3_MAXIMUM));

	   RandomOptionTool::procedureListValueOld = (WNDPROC)SetWindowLong(_handleListValue, GWL_WNDPROC, (LONG)listValueProcedureSubClassing);
	   RandomOptionTool::procedureEditTypeOld = (WNDPROC)SetWindowLong(_handleEditType, GWL_WNDPROC, (LONG)editTypeProcedureSubClassing);
	   RandomOptionTool::procedureEditRateOld = (WNDPROC)SetWindowLong(_handleEditRate, GWL_WNDPROC, (LONG)editRateProcedureSubClassing);
	   RandomOptionTool::procedureEditLowOld = (WNDPROC)SetWindowLong(_handleEditLow, GWL_WNDPROC, (LONG)editLowProcedureSubClassing);
	   RandomOptionTool::procedureEditHighOld = (WNDPROC)SetWindowLong(_handleEditHigh, GWL_WNDPROC, (LONG)editHighProcedureSubClassing);
   }

   return TRUE;
}

LRESULT CALLBACK listValueProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL)
{
	switch ( _message )
	{
	case WM_KEYDOWN:
		switch ( _paramW )
		{
		case VK_SPACE:
			break;
		case VK_RETURN:
		case VK_TAB:
			SetFocus(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_0_NAME));
			break;		
		}
		break;
	}

	return ::CallWindowProc(RandomOptionTool::procedureListValueOld, _handleWindow, _message, _paramW, _paramL);
}
LRESULT CALLBACK editTypeProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL)
{
	switch ( _message )
	{
	case WM_KEYDOWN:
		switch ( _paramW )
		{
		case VK_UP:
		case VK_DOWN:
			{
// 				char _cString[4][128];
// 				RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
// 				const HWND _handleEditType(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_0_NAME));
//
// 				const unsigned int _indexItem(ListView_GetNextItem(_handleListValue, -1, LVNI_ALL | LVNI_SELECTED));
// 				ListView_GetItemText(_handleListValue, _indexItem, 0, _cString[0], 128);
// 				ListView_GetItemText(_handleListValue, _indexItem, 1, _cString[1], 128);
// 				ListView_GetItemText(_handleListValue, _indexItem, 2, _cString[2], 128);
// 				ListView_GetItemText(_handleListValue, _indexItem, 3, _cString[3], 128);
			}
			break;
		case VK_SPACE:
			break;
		case VK_RETURN:
		case VK_TAB:
			::SetFocus(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_1_RATE));
			break;
		}
		break;
	}

	return ::CallWindowProc(RandomOptionTool::procedureEditTypeOld, _handleWindow, _message, _paramW, _paramL);
}
LRESULT CALLBACK editRateProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL)
{
	switch ( _message )
	{
	case WM_KEYDOWN:
		switch ( _paramW )
		{
		case VK_SPACE:
			break;
		case VK_RETURN:
		case VK_TAB:
			SetFocus(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_2_MINIMUM));
			break;
		}
		break;
	}

	return ::CallWindowProc(RandomOptionTool::procedureEditRateOld, _handleWindow, _message, _paramW, _paramL);
}
LRESULT CALLBACK editLowProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL)
{
	switch ( _message )
	{
	case WM_KEYDOWN:
		switch ( _paramW )
		{
		case VK_SPACE:
			break;
		case VK_RETURN:
		case VK_TAB:
			SetFocus(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_3_MAXIMUM));
			break;
		}
		break;
	}

	return ::CallWindowProc(RandomOptionTool::procedureEditLowOld, _handleWindow, _message, _paramW, _paramL);
}
LRESULT CALLBACK editHighProcedureSubClassing(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL)
{
	switch ( _message )
	{
	case WM_KEYDOWN:
		switch ( _paramW )
		{
		case VK_SPACE:
			break;
		case VK_RETURN:
		case VK_TAB:
			SetFocus(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));
			break;
		}
		break;
	}

	return ::CallWindowProc(RandomOptionTool::procedureEditHighOld, _handleWindow, _message, _paramW, _paramL);
}

LRESULT CALLBACK dialogueProcedureConvert(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL)
{
	switch ( _message )
	{
	case WM_INITDIALOG:			
		return TRUE;
	case WM_COMMAND:
		switch ( LOWORD(_paramW) )
		{			
		case ID_BUTTON_CONVERT_NO:
			RandomOptionTool::loadFileOld(false);
			::ShowWindow(RandomOptionTool::hWindowConvert, SW_HIDE);
			break;
		case ID_BUTTON_CONVERT:
			RandomOptionTool::loadFileOld(true);
			::ShowWindow(RandomOptionTool::hWindowConvert, SW_HIDE);
			break;
		}
		break;
	case WM_CLOSE:
		::ShowWindow(RandomOptionTool::hWindowConvert, SW_HIDE);
		break;
	}
	return FALSE;
}
LRESULT CALLBACK dialogueProcedureMain(const HWND _handleWindow, const unsigned int _message, const WPARAM _paramW, const LPARAM _paramL)
{
	switch ( _message )
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_NOTIFY:
		{
			const HWND _handleListEntity(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));
			LPNMHDR _hdr = (LPNMHDR)_paramL;
			const LPNMLISTVIEW _listView = LPNMLISTVIEW(_paramL);
			const LPNMTREEVIEW _treeview = LPNMTREEVIEW(_paramL);
						
			if ( _hdr->hwndFrom == _handleListEntity )
			{				
				switch ( _hdr->code )
				{
				case LVN_ITEMCHANGED:
					{
						if ( (_listView->uChanged != LVIF_STATE) || (_listView->uNewState != (LVIS_SELECTED | LVIS_FOCUSED)) )
							break;

						const HWND _handleListValue(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION));
						const unsigned int _indexList(SendMessage(_handleListValue, LB_GETCURSEL, 0, 0));

						char _cName[RandomOption::LENGTH_NAME];
						SendMessage(_handleListValue, LB_GETTEXT, _indexList, (LPARAM)_cName);

						RandomOptionTool::paramLValueSelected = 0xffffffffL;
						switch ( _listView->lParam & 0xff000000L )
						{
						case RandomOptionTool::TYPE_INFORMATION_BASIC:
							RandomOptionTool::editInformationBasic(_cName, _listView->lParam);
							break;
						case RandomOptionTool::TYPE_TYPE_DROP:
							RandomOptionTool::editTypeValueDrop(_cName, _listView->lParam);
							break;
						case RandomOptionTool::TYPE_TYPE_REMODEL:
							
							RandomOptionTool::editTypeValueRemodel(_cName, _listView->lParam);
							break;
						default:							
							RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_0_NAME, TEXT(""));
							RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_1_RATE, TEXT(""));
							RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_2_MINIMUM, TEXT(""));
							RandomOptionTool::setWindowText(_handleWindow, IDC_STATIC_3_MAXIMUM, TEXT(""));

							RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_0_NAME, TEXT(""));
							RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_1_RATE, TEXT(""));
							RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_2_MINIMUM, TEXT(""));
							RandomOptionTool::setWindowText(_handleWindow, IDC_EDIT_3_MAXIMUM, TEXT(""));
							break;
						}
					}
					break;
				}
			}
		}
		break;
	case WM_COMMAND:
		switch ( LOWORD(_paramW) )
		{			
		case IDC_BUTTON_SAVE:
			RandomOptionTool::saveFile();
			break;
		case IDC_BUTTON_LOAD:
			RandomOptionTool::loadFile();
			break;
		case IDC_BUTTON_LOAD_OLD:
			::ShowWindow(RandomOptionTool::hWindowConvert, SW_SHOW);
			break;
		case IDC_LIST_RANDOM_OPTION:
			switch ( HIWORD(_paramW) )
			{
			case LBN_SELCHANGE:
				{
					const HWND _handleList(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION));
					const unsigned int _indexList(SendMessage(_handleList, LB_GETCURSEL, 0, 0));

					char _cName[RandomOption::LENGTH_NAME];
					SendMessage(_handleList, LB_GETTEXT, _indexList, (LPARAM)_cName);
					RandomOptionTool::selectItemList(_cName);
				}
				break;
			}
			break;
		case IDC_EDIT_1_RATE:
		case IDC_EDIT_2_MINIMUM:
		case IDC_EDIT_3_MAXIMUM:
			{
				switch ( HIWORD(_paramW) )
				{
				case EN_CHANGE:
					RandomOptionTool::changeValue();
					break;
				}
			}
			break;
		case IDC_EDIT_SEARCH:
			switch ( HIWORD(_paramW) )
			{
			case EN_CHANGE:
				RandomOptionTool::refreshList();
				break;
			}
			break;
		case IDC_EDIT_RANDOM_OPTION_NAME:
			switch ( HIWORD(_paramW) )
			{
			case EN_CHANGE:				
				break;
			}
			break;
		case IDC_BUTTON_ADD:
			RandomOptionTool::addRandomOptionEntity();
			break;
		case IDC_BUTTON_APPLY:
			RandomOptionTool::renameRandomOptionEntity();
			break;
		case IDC_BUTTON_REMOVE:
			RandomOptionTool::removeRandomOptionEntity();
			break;
		case IDC_BUTTON_ADD_VALUE:
			break;
		case IDC_BUTTON_REMOVE_VALUE:
			RandomOptionTool::removeRandomOptionValue();
			break;
		}
		return TRUE;
	case WM_CLOSE:
		::PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK windowProcedureRandomOption(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �޴��� ���� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;	
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		{
			const HWND _handleListValue(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_LIST_RANDOM_OPTION_ENTITY));
			const HWND _handleEditName(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_0_NAME));
			const HWND _handleEditRate(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_1_RATE));
			const HWND _handleEditLow(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_2_MINIMUM));
			const HWND _handleEditHigh(::GetDlgItem(RandomOptionTool::hWindowMain, IDC_EDIT_3_MAXIMUM));
			SetWindowLong(_handleEditName, GWL_WNDPROC, (LONG)RandomOptionTool::procedureListValueOld);
			SetWindowLong(_handleListValue, GWL_WNDPROC, (LONG)RandomOptionTool::procedureEditTypeOld);
			SetWindowLong(_handleEditRate, GWL_WNDPROC, (LONG)RandomOptionTool::procedureEditRateOld);
			SetWindowLong(_handleEditLow, GWL_WNDPROC, (LONG)RandomOptionTool::procedureEditLowOld);
			SetWindowLong(_handleEditHigh, GWL_WNDPROC, (LONG)RandomOptionTool::procedureEditHighOld);
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

