================================================================================
              MFC ���̺귯�� : MMPC ������Ʈ ����
================================================================================

���� ���α׷� �����縦 ����Ͽ� MMPC ���� ���α׷��� ����ϴ�. �� ���� 
���α׷������� �⺻���� MFC ��� ����� ���� ���α׷� �ۼ� ����� �����մϴ�.

�� ���Ͽ��� MMPC ���� ���α׷��� �����ϴ� �� ���Ͽ� ���� ���䰡 ���ԵǾ�
�ֽ��ϴ�.

MMPC.vcproj
    ���� ���α׷� �����縦 ����Ͽ� ������ VC++ ������Ʈ�� �� ������Ʈ �����Դϴ�.
    �� ���Ͽ��� ������ ������ Visual C++ ���� ���� �� ���� ���α׷� �����翡�� ������ �÷���,
    ����, ������Ʈ ��� ���� ������ ���Ե˴ϴ�.

MMPC.h
    ���� ���α׷��� �� ��� �����Դϴ�. �� ���Ͽ��� �ٸ� ������Ʈ�� ���õ� Resource.h�� ����
    Ư�� ����� ���ԵǸ� CMMPCApp ���� ���α׷� Ŭ������ ����˴ϴ�.

MMPC.cpp
    CMMPCApp ���� ���α׷� Ŭ������ �����ϴ� �� ���� ���α׷��� �ҽ� �����Դϴ�.

MMPC.rc
    ���α׷����� ����ϴ� ��� Microsoft Windows ���ҽ��� ������ �����Դϴ�.
    �� ���Ͽ��� RES ���� ���͸��� ����� ������, ��Ʈ�� �� Ŀ���� ���ԵǸ�
    Microsoft Visual C++���� ���� �� ������ ������ ���� �ֽ��ϴ�. ������� ������Ʈ
    ���ҽ��� 1042�� ��� �ֽ��ϴ�.

res\MMPC.ico
    ���� ���α׷��� ���������� ���Ǵ� ������ �����Դϴ�.
    �� �������� �� ���ҽ� ������ MMPC.rc�� ���Ե˴ϴ�.

res\MMPC.rc2
    Microsoft Visual C++���� ������ �� ���� ���ҽ��� ���Ե� �����Դϴ�.
    ���ҽ� �����⿡�� ������ �� ���� ��� ���ҽ��� �� ���Ͽ� ���ԵǾ� �ֽ��ϴ�.

/////////////////////////////////////////////////////////////////////////////

���� ���α׷� �����翡���� ���� ��ȭ ���� Ŭ������ ����ϴ�.
MMPCDlg.h, MMPCDlg.cpp - ��ȭ ����
    CMMPCDlg Ŭ������ �����ϴ� �����Դϴ�.  �� Ŭ�������� ���� ���α׷���
    �� ��ȭ ���ڿ� ���� ������ ���ǵ˴ϴ�. ��ȭ ������ ���ø��� MMPC.rc��
    ������ Microsoft Visual C++���� ������ �� �ֽ��ϴ�.
/////////////////////////////////////////////////////////////////////////////

��Ÿ ���

ActiveX ��Ʈ��
    ���� ���α׷����� ActiveX ��Ʈ���� ����� �� �ֽ��ϴ�.
/////////////////////////////////////////////////////////////////////////////

��Ÿ ǥ�� ����

StdAfx.h, StdAfx.cpp
    �̸� �����ϵ� ��� ����(PCH) MMPC.pch �� �̸� �����ϵ�
    ���� ���� StdAfx.obj�� ������ �� ���Ǵ� �����Դϴ�.

Resource.h
    ���ο� ���ҽ� ID�� �����ϴ� ǥ�� ��� �����Դϴ�.
    Microsoft Visual C++���� �� ������ �а� ������Ʈ�մϴ�.

/////////////////////////////////////////////////////////////////////////////

��Ÿ ����

���� ���α׷� �����翡���� "TODO:"�� ����Ͽ� �߰��ϰų� ����� �����ؾ� �ϴ�
�ҽ� �ڵ带 ��Ÿ���ϴ�.

���� ���α׷����� ���� DLL�� MFC�� ����ϰ� ���� ���α׷��� �� � ü����
���� �ٸ� ��� Microsoft Visual C++ CD-ROM�� Win\System ���͸��� �ִ� 
�ش� ������ ���ҽ��� MFC70XXX.DLL�� ��ǻ���� system �Ǵ� system32 ���͸��� 
������ ���� MFCLOC.DLL�� �̸��� �ٲپ�� �մϴ�. "XXX"�� �ش� �� ��Ÿ����
����Դϴ�. ���� ��� MFC70DEU.DLL���� ���Ͼ�� ��ȯ�� ���ҽ��� ���Ե˴ϴ�.
�̷� �۾��� ���� ������ ���� ���α׷��� �Ϻ� UI ��Ұ� � ü���� ����
���� �ְ� �˴ϴ�.

/////////////////////////////////////////////////////////////////////////////


=========================================================================================
MFC Library: MMPC Project Overview
= ... MMPC.vcproj
The main project file for a VC++ project generated using the Application Wizard.
This file contains information about the version of Visual C++ that generated the file, the platform selected by the Application Wizard, the configuration, and project features.

MMPC.h
The main header file for the application. This file includes specific headers, such as Resource.h, that are specific to other projects, and declares the CMMPCApp application class.

MMPC.cpp
The source file for the main application that contains the CMMPCApp application class.

MMPC.rc
This file lists all the Microsoft Windows resources used by the program.
This file contains icons, bitmaps, and cursors stored in the RES subdirectory,
and you can also edit this file directly in Microsoft Visual C++. Your project

resources are located in 1042.

res\MMPC.ico
This is an icon file used as the application's icon.
This icon is included in the main resource file, MMPC.rc.

res\MMPC.rc2
File containing resources that cannot be edited in Microsoft Visual C++.
All resources that cannot be edited in the resource editor are contained in this file.

////////////////////////////////////////////////////////////////////////////////////////////

The Application Wizard creates a single dialog class.

MMPClg.h, MMPCDlg.cpp - Dialog
Files containing the CMMPCDlg class. This class defines the behavior for the application's
main dialog box. The template for the dialog box is in MMPC.rc
and can be edited in Microsoft Visual C++. //////////////////////////////////////////////////////////////////////////////////

Other Features

ActiveX Controls

You can use ActiveX controls in your applications.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Other Standard Files

StdAfx.h, StdAfx.cpp
These are the files used to build the precompiled header file (PCH) MMPC.pch and the precompiled
format file StdAfx.obj.

Resource.h
A standard header file that defines new resource IDs.
Microsoft Visual C++ reads and updates this file.

//////////////////////////////////////////////////////////////////////////////////////////////////////

Other Information

The Application Wizard uses "TODO:" to indicate source code that you need to add or customize.

If your application uses MFC in a shared DLL and the language of your application is different from the language of the operating system, you must copy the local resource, MFC70XXX.DLL, located in the Win\System directory on the Microsoft Visual C++ CD-ROM, to the system or system32 directory on your computer, and then rename it MFCLOC.DLL. "XXX" is an abbreviation for the language. For example, MFC70DEU.DLL contains resources translated to German.
If you do not do this, some UI elements of your application will remain in the language of the operating system.

//////////////////////////////////////////////////////////////////////////////////////////