#include "stdafx.h"
#include <fstream>
#include "ReportFiles.h"
#include "../Hash/CHash.h"


#include "../DebugInclude.h"


namespace REPORTFILES
{
    CString GetAppPath ()
    {
        CString _strAppPath;

        // Note : 실행파일의 경로를 찾아서 저장한다. - 반드시 CBasicWnd(CD3dApp) 이전에 수행해야 한다.
        //
        CString strCommandLine;

        TCHAR szPath[MAX_PATH] = {0};
        GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
        strCommandLine = szPath;

        if ( !strCommandLine.IsEmpty() )
        {
            DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
            if ( dwFind != -1 )
            {
                _strAppPath = strCommandLine.Left ( dwFind );
                if ( _strAppPath.IsEmpty() )	return CString();

                if ( _strAppPath.GetAt(0) == '"' )		_strAppPath = _strAppPath.Right ( _strAppPath.GetLength() - 1 );
            }
        }

        return _strAppPath;
    }

    const std::string g_strReport = "_filelist.lst";
    const std::string g_strReportExe = "reportfiles.exe";

    std::fstream g_file;

    // Hash 를 하기위한 오브젝트.
    HASH::CHash g_hashObj;

    void MakeFileList(const char* szPath, const std::vector<std::string>& BanFiles)
    {
        // MD5 해쉬를 위해서 설정한다.		
        // Set the algorithm
        g_hashObj.SetHashAlgorithm( HASH::MD5 ); // MD5 로 설정
        // Set the operation
        g_hashObj.SetHashOperation( HASH::FILE_HASH ); // 파일 해쉬로 설정
        // 스페이스 없는 대문자.
        g_hashObj.SetHashFormat( HASH::UPPERCASE_NOSPACES );

        CString strROOT = GetAppPath ();
        strROOT.MakeLower();
        std::string strRoot = strROOT;		

        std::string strSAVEFILE = strRoot + "\\" + g_strReport.c_str();
        g_file.open ( strSAVEFILE.c_str(), std::ios_base::out | std::ios_base::trunc );

        g_file << "source_path" << "	" << strRoot << std::endl;
		g_file << "fileVersion_Add" << "	" << "1" << std::endl;
        g_file << std::endl;
        g_file << "\\" << std::endl;
     
        PathRecurse(strRoot, strRoot, BanFiles);

        g_file.close();
    }

	void PathRecurse(
        const std::string strRootPath,
        const std::string strCurPath,
        const std::vector<std::string>& BanFiles)
    {
        CString strRoot = GetAppPath ();

        CFileFind finder;

        //	build a string with wildcards
        std::string strWildcard ( strCurPath );
        strWildcard += _T("\\*.*");

        //	start working for files
        BOOL bWorking = finder.FindFile( strWildcard.c_str() );

        while ( bWorking )
        {
            bWorking = finder.FindNextFile ();

            //	skip . and .. files; otherwise, we'd
            //	recur infinitely!
            if ( finder.IsDots() ) continue;

            //	if it's a directory, recursively search it
            if ( !finder.IsDirectory() )
            {
                CString strNAME = finder.GetFileName();
                strNAME.MakeLower ();
                CString strPATH = finder.GetFilePath();

                bool bBanFile = false;
                for (size_t i=0; i<BanFiles.size(); ++i)
                {
                    if (strNAME.GetString() == BanFiles[i])
                        bBanFile = true;
                }

                if (!bBanFile)
                {                
#ifdef CH_PARAM
                    // Set the file
                    g_hashObj.SetHashFile( strPATH );
                    // Hash the file
                    CString strHash = g_hashObj.DoHash();
                    g_file << "\t" << strNAME.GetString() << "\t" << strHash.GetString() << std::endl;
#else
                    g_file << "\t" << strNAME.GetString() << std::endl;
#endif
                }
            }
        }

        finder.Close();

        //	start working for files
        bWorking = finder.FindFile( strWildcard.c_str() );

        while ( bWorking )
        {
            bWorking = finder.FindNextFile ();

            //	skip . and .. files; otherwise, we'd
            //	recur infinitely!
            if ( finder.IsDots() ) continue;

            //	if it's a directory, recursively search it
            if ( finder.IsDirectory() )
            {
                CString strPATH = finder.GetFilePath();
                strPATH.MakeLower ();
                std::string strPath = strPATH;

                std::string strSubPath = std::string ( strPath.begin()+strRootPath.length(), strPath.end() );

                g_file << std::endl;
                g_file << strSubPath.c_str() << "\\" << std::endl;

                PathRecurse(strRootPath, strPath, BanFiles);
            }
        }

        finder.Close();
    }
};

