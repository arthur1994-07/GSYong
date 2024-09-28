// VersionManagerDoc.cpp : CVersionManagerDoc Ŭ������ ����
//

#include "stdafx.h"

#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/File/Excel/MinExcel.h"
#include "../SigmaCore/File/TxtFile.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Util/MinMessageBox.h"
#include "../SigmaCore/Util/SystemInfo.h"

#include "VersionManager.h"
#include "VersionManagerDoc.h"
#include "DlgCompress.h"
#include "DlgFTP.h"
#include "CMinFtp.h"
#include "CCfg.h"
#include "DlgFileCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVersionManagerDoc

IMPLEMENT_DYNCREATE(CVersionManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CVersionManagerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_FTP, OnFtp)
    ON_COMMAND(ID_CHECK, OnCheck)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
END_MESSAGE_MAP()


// CVersionManagerDoc ����/�Ҹ�

CVersionManagerDoc::CVersionManagerDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.	
}

CVersionManagerDoc::~CVersionManagerDoc()
{
}

BOOL CVersionManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CVersionManagerDoc serialization

void CVersionManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CVersionManagerDoc ����

#ifdef _DEBUG
void CVersionManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVersionManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVersionManagerDoc ���

void CVersionManagerDoc::GetAppPath()
{	
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !m_strFullPath.IsEmpty() )
			if ( m_strFullPath.GetAt(0) == '"' )
				m_strFullPath = m_strFullPath.Right ( m_strFullPath.GetLength() - 1 );
		}
	}
}

void CVersionManagerDoc::OnFileOpen()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.	
	char szFilters[]=
      "Ran Version List File (*.lst)|*.lst|All Files (*.*)|*.*||";

	CFileDialog DlgFile(TRUE, "lst", "*.lst", OFN_ENABLESIZING, szFilters, NULL);
	    
    GetAppPath();
    DlgFile.m_pOFN->lpstrInitialDir = m_strFullPath;
	
	if (DlgFile.DoModal() == IDOK ) 
	{
		POSITION pos = DlgFile.GetStartPosition();
		CString strPathName = DlgFile.GetNextPathName(pos);

		CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();

        CDlgCompress DlgCompress(pFrame, strPathName);
		DlgCompress.DoModal();

        pFrame->m_pLeft->InitListData();
        pFrame->m_pRight->InitListData();
	}
}

void CVersionManagerDoc::OnFtp()
{
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    TRACE("OnFtp \n");
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CDlgFTP DlgFtp(pFrame);
    DlgFtp.DoModal();   
}

void CVersionManagerDoc::OnCheck()
{
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    TRACE("OnCheck \n");
    CDlgFileCheck DlgCheck;
    DlgCheck.DoModal();
}

void CVersionManagerDoc::OnFileSave()
{
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    TRACE("OnFileSave \n");    

    // DB ���� ��ü ���ϸ���Ʈ�� �����´�    
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    IVersionManagerDb* pDb = pFrame->GetDbMan();
    IVersionManConfig* pConfig = pFrame->GetConfig();

    std::vector<FullFileList> vFullFileList; 
    pDb->GetAllFileList(vFullFileList);
    
    std::string AppPath = sc::getAppPath();
    AppPath.append("\\filelist.bin");
    // ���ϸ���Ʈ�� �����Ѵ�.
    CFile tmpFile;
    if (!tmpFile.Open(AppPath.c_str(), CFile::modeCreate | CFile::modeWrite))
    {
        std::string ErrMsg(
            sc::string::format(
                "%1% %2% file create failed.",
                tmpFile.GetFilePath().GetString(),
                tmpFile.GetFileName().GetString()));
        sc::ErrorMessageBox(ErrMsg, false);
        return;
    }

    RanFileList sTemp;
    SecureZeroMemory( &sTemp, sizeof(RanFileList) );

    int nTotalCount = (int) vFullFileList.size(); // ��ü ���ϰ���
    
	// ù��° ���ڵ�� ���ϸ���Ʈ �����̴�.
#ifdef CHINAPARAM
    sTemp.nVersion = 2;
#else
	sTemp.nVersion = 3;
#endif
    tmpFile.Write(&sTemp, sizeof(RanFileList));

    // �ι�° ���ڵ�� ��ü ���ϰ����̴�.
    sTemp.nVersion = nTotalCount;
    tmpFile.Write(&sTemp, sizeof(RanFileList));

    sc::TxtFile TxtList(sc::getAppPath()+"\\filelist.csv", true);

    // �ι�° ���ڵ� ���� ���������� ���ϰ�ο� ���ϸ��� �����Ѵ�.
    for (int i=0; i<(int) vFullFileList.size(); i++)
	{	
        SecureZeroMemory( &sTemp, sizeof(RanFileList) );
        sTemp.nVersion = vFullFileList[i].nVersion;
		::StringCchCopy(
			sTemp.szFileName,
			FILENAME_SIZE,
			vFullFileList[i].strFileName.GetString());

        ::StringCchCopy(
			sTemp.szSubPath,
			SUBPATH_SIZE,
			vFullFileList[i].strFolder.GetString());
#ifdef CHINAPARAM
		::StringCchCopy(
			sTemp.szMD5,
			MD5_SIZE,
			vFullFileList[i].strMD5.GetString());
#endif

		sTemp.nIsPreDown = vFullFileList[i].nIsPreDown;

        tmpFile.Write(&sTemp, sizeof(RanFileList));

        std::string TxtWrite(
            sc::string::format(
                "%1%, %2%, %3%, %4%",
                sTemp.szSubPath,
                sTemp.szFileName,
                sTemp.nVersion,
				sTemp.nIsPreDown));
        TxtList.write(TxtWrite);
    }

    tmpFile.Close();
    
    // ���ϸ���Ʈ�� �����ؼ� ����Ѵ�.  
     
    //CString strFullPath;
    //CString strCommandLine;

	//TCHAR szPath[MAX_PATH] = {0};
	//GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	//strCommandLine = szPath;

    //if ( !strCommandLine.IsEmpty() )
    //{
    //    DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
    //    if ( dwFind != -1 )
    //    {
    //        strFullPath = strCommandLine.Left ( dwFind );
    //
    //        if ( !strFullPath.IsEmpty() )
    //        if ( strFullPath.GetAt(0) == '"' )
    //        strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
    //    }
    //}  

    //CString ListPath;
    //ListPath.Format("%s\\filelist.bin", strFullPath);

    CString TargetPath;
    TargetPath.Format("%s\\", pConfig->GetTargetPath());

    if (CAB_UTIL_MIN::MinMakeCab(AppPath.c_str(), TargetPath.GetString()) == TRUE)
    {
        std::string Msg(
            sc::string::format(
                "%1% %2% ���ϸ���Ʈ ������ �Ϸ��߽��ϴ�",
                AppPath,
                TargetPath.GetString()));
        pFrame->MessageBox(Msg.c_str());
    }
    else
    {
        std::string Msg(
            sc::string::format(
            "%1% %2% ���ϸ���Ʈ ���࿡ �����Ͽ����ϴ�",
            AppPath,
            TargetPath.GetString()));
        pFrame->MessageBox(Msg.c_str());      
    }    
}