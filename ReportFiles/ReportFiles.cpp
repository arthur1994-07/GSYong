// ReportFiles.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "../SigmaCore/File/ReportFiles.h"
#include "../SigmaCore/File/ReportFilesXml.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Util/MinMessageBox.h"
#include "./ReportFiles.h"

#include "../SigmaCore/DebugInclude.h"

// ������ ���� ���α׷� ��ü�Դϴ�.

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC�� �ʱ�ȭ�մϴ�. �ʱ�ȭ���� ���� ��� ������ �μ��մϴ�.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���� �ڵ带 �ʿ信 ���� �����մϴ�.
		_tprintf(_T("�ɰ��� ����: MFC�� �ʱ�ȭ���� ���߽��ϴ�.\n"));
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
