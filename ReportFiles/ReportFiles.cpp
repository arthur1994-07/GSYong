// ReportFiles.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "../SigmaCore/File/ReportFiles.h"
#include "../SigmaCore/File/ReportFilesXml.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Util/MinMessageBox.h"
#include "./ReportFiles.h"

#include "../SigmaCore/DebugInclude.h"

// 유일한 응용 프로그램 개체입니다.

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("심각한 오류: MFC를 초기화하지 못했습니다.\n"));
		return nRetCode = 1;
	}

    std::string AppPath = sc::getAppPath();
    std::string FileName(AppPath);
    FileName.append("\\ReportFilter.xml");

    sc::ReportFilesXml XmlLoad;
    if (!XmlLoad.Load(FileName))
    {
        std::string ErrMsg(
            sc::string::format(
                "%1% file load failed.",
                FileName));
        sc::ErrorMessageBox(ErrMsg, false);
        return 0;
    }

	REPORTFILES::MakeFileList(AppPath.c_str(), XmlLoad.GetBanFiles());
	return nRetCode;
}
