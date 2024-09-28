================================================================================
              MFC 라이브러리 : MMPC 프로젝트 개요
================================================================================

응용 프로그램 마법사를 사용하여 MMPC 응용 프로그램을 만듭니다. 이 응용 
프로그램에서는 기본적인 MFC 사용 밥법과 응용 프로그램 작성 방법을 설명합니다.

이 파일에는 MMPC 응용 프로그램을 구성하는 각 파일에 대한 개요가 포함되어
있습니다.

MMPC.vcproj
    응용 프로그램 마법사를 사용하여 생성된 VC++ 프로젝트의 주 프로젝트 파일입니다.
    이 파일에는 파일을 생성한 Visual C++ 버전 정보 및 응용 프로그램 마법사에서 선택한 플랫폼,
    구성, 프로젝트 기능 등의 정보가 포함됩니다.

MMPC.h
    응용 프로그램의 주 헤더 파일입니다. 이 파일에는 다른 프로젝트에 관련된 Resource.h와 같은
    특정 헤더가 포함되며 CMMPCApp 응용 프로그램 클래스가 선언됩니다.

MMPC.cpp
    CMMPCApp 응용 프로그램 클래스를 포함하는 주 응용 프로그램의 소스 파일입니다.

MMPC.rc
    프로그램에서 사용하는 모든 Microsoft Windows 리소스가 나열된 파일입니다.
    이 파일에는 RES 하위 디렉터리에 저장된 아이콘, 비트맵 및 커서가 포함되며
    Microsoft Visual C++에서 직접 이 파일을 편집할 수도 있습니다. 사용자의 프로젝트
    리소스는 1042에 들어 있습니다.

res\MMPC.ico
    응용 프로그램의 아이콘으로 사용되는 아이콘 파일입니다.
    이 아이콘은 주 리소스 파일인 MMPC.rc에 포함됩니다.

res\MMPC.rc2
    Microsoft Visual C++에서 편집할 수 없는 리소스가 포함된 파일입니다.
    리소스 편집기에서 편집할 수 없는 모든 리소스는 이 파일에 포함되어 있습니다.

/////////////////////////////////////////////////////////////////////////////

응용 프로그램 마법사에서는 단일 대화 상자 클래스를 만듭니다.
MMPCDlg.h, MMPCDlg.cpp - 대화 상자
    CMMPCDlg 클래스를 포함하는 파일입니다.  이 클래스에는 응용 프로그램의
    주 대화 상자에 대한 동작이 정의됩니다. 대화 상자의 템플릿은 MMPC.rc에
    있으며 Microsoft Visual C++에서 편집할 수 있습니다.
/////////////////////////////////////////////////////////////////////////////

기타 기능

ActiveX 컨트롤
    응용 프로그램에서 ActiveX 컨트롤을 사용할 수 있습니다.
/////////////////////////////////////////////////////////////////////////////

기타 표준 파일

StdAfx.h, StdAfx.cpp
    미리 컴파일된 헤더 파일(PCH) MMPC.pch 및 미리 컴파일된
    형식 파일 StdAfx.obj를 빌드할 때 사용되는 파일입니다.

Resource.h
    새로운 리소스 ID를 정의하는 표준 헤더 파일입니다.
    Microsoft Visual C++에서 이 파일을 읽고 업데이트합니다.

/////////////////////////////////////////////////////////////////////////////

기타 정보

응용 프로그램 마법사에서는 "TODO:"를 사용하여 추가하거나 사용자 지정해야 하는
소스 코드를 나타냅니다.

응용 프로그램에서 공유 DLL에 MFC를 사용하고 응용 프로그램의 언어가 운영 체제의
언어와 다른 경우 Microsoft Visual C++ CD-ROM의 Win\System 디렉터리에 있는 
해당 지역의 리소스인 MFC70XXX.DLL을 컴퓨터의 system 또는 system32 디렉터리에 
복사한 다음 MFCLOC.DLL로 이름을 바꾸어야 합니다. "XXX"는 해당 언어를 나타내는
약어입니다. 예를 들어 MFC70DEU.DLL에는 독일어로 변환된 리소스가 포함됩니다.
이런 작업을 하지 않으면 응용 프로그램의 일부 UI 요소가 운영 체제의 언어로
남아 있게 됩니다.

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