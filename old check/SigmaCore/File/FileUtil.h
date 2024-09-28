#ifndef _FILE_SYSTEM_UTIL_H_
#define _FILE_SYSTEM_UTIL_H_

#pragma once

#include <string>
#include <vector>

namespace sc {
    namespace file {

    //CString strASUserPath;		///< /<document>/<user>/RanAS
    //CString strASLogPath;		///< /<document>/<user>/RanAS/log
    //CString strASTempPath;		///< /<document>/<user>/RanAS/temp
    //CString strASScreenShotPath; ///< /<document>/<user>/RanAS/screenshot

        //! 폴더나 파일이 있는지 검사한다.
        bool isExist( const wchar_t* szFullPath );
        bool isExist( const std::wstring& strFullPath );
        bool isExist( const char* szFullPath );
        bool isExist( const std::string& strFullPath );

        //! 폴더나 파일을 새이름으로 변경한다.
        //! 주의:폴더 이름이 변경되면 하위의 파일들까지
        //! 모두 path 가 변경된다.
        bool rename( const std::wstring& strOldPath, const std::wstring& strNewPath );

        //! 자동으로 파일이름을 바꾼다.
        //! 규칙:확장자 앞의 파일이름에 숫자가 하나씩 증가한다.
        //! ex) a.txt -> a1.txt -> a2.txt -> a3.txt	
        bool autoRename(const std::wstring& strFullPath);
        bool autoRename(const std::string& strFullPath);

        //! from 에서 to 로 파일을 copy 한다.
		bool copyFile( const std::string& fromFullPath, const std::string& toFullPath, BOOL bOverwrite=TRUE );
        bool copyFile(const std::wstring& fromFullPath, const std::wstring& toFullPath, BOOL bOverwrite=TRUE );
        bool copyFile(const CStringW& fromFullPath, const CStringW& toFullPath, BOOL bOverwrite=TRUE);

        //! 디렉토리를 생성한다.
        //! 이미 디렉토리가 있으면 성공
        //! 생성하려는 디렉토리 이름과 같은 이름의
        //! 파일이 있다면 실패
        bool createDirectory( const wchar_t* szPath );
        bool createDirectory( const char* szPath );
        bool createDirectory( const std::wstring& Path );
        bool createDirectory( const std::string& Path );

        bool isDirectory( const wchar_t* szPath );
        bool isDirectory( const char* szPath );
        bool isDirectory( const std::wstring& Path );
        bool isDirectory( const std::string& Path );

        //! 파일이나 디렉토리를 지운다
        bool remove( const wchar_t* szFullPath );
        bool remove( const char* szFullPath );
        bool remove( const std::wstring& Path );
        bool remove( const std::string& Path );

        //! 해당 디렉토리의 모든 파일을 지운다.
        void deleteFiles(const std::string& Path);

        //! 파일 확장자를 돌려준다.
        //! test.txt -> txt
        std::wstring getFileExtW(const std::wstring& strFullPath);
        std::string  getFileExt(const std::string& strFullPath);

        //! 파일의 확장자를 strNewExt 로 변경한 파일명을 돌려준다.
        //! changeFileExt( test.txt, avi ) -> test.avi
        std::wstring changeFileExtW(const std::wstring& strFileName, const std::wstring& strNewExt);
        std::string  changeFileExt(const std::string& strFileName, const std::string& strNewExt);

        //CString& getUserFolder();
        //const CString& getLogFolder();
        //const CString& getTempFolder();
        const CString& getScreenShotFolder();
        //void makeASFolder();

        //! Fast getting file size function
        //! MS Windows version
        //! \param szFileName File Name (FULL PATH)
        //! \return File size
        __int64 getFileSize64( const TCHAR * szFileName );

        //! Fast getting file size function
        //! C++ version
        //! \param szFileName File Name (FULL PATH)
        //! \return File size
        int getFileSizeCPP(const char* szFileName);

        //! Fast getting file size function
        //! MFC version
        //! \param szFileName File Name (FULL PATH)
        //! \return File Size
        ULONGLONG getFileSizeMFC( const TCHAR * szFileName );

        //! 파일의 마지막 Write 타임을 구한다.
        bool getFileLastWriteTime( const std::wstring& FileName, __time64_t& WriteTime );

        //! 파일의 마지막 Access 타임을 구한다.
        bool getFileLastAccessTime( const std::wstring& FileName, __time64_t& AccessTime );

        //! 파일의 Create 타임을 구한다.
        bool getFileCreateTime( const std::wstring& FileName, __time64_t& CreateTime );

        //! 파일의 Create / Access / Write 타임을 구한다.
        bool getFileTime( const std::wstring& FileName, __time64_t& CreateTime, __time64_t& AccessTime, __time64_t& WriteTime );

        enum EM_PATCH_FOLDER_TYPE {
            FOLDER = 1,
            ZIP_FILE = 2,
        };	

        struct PatchFileW
        {
            EM_PATCH_FOLDER_TYPE emType;
            std::wstring strFolder;
            std::wstring strFile;
            std::wstring strExt;
        };

        struct PatchFileA
        {
            EM_PATCH_FOLDER_TYPE emType;
            std::string strFolder;
            std::string strFile;
            std::string strExt;
        };

        //int nPatchFolderIndex;
        //! RootPath 아래에 있는 파일들의 리스트를 작성한다.
        void makeFileList(const char* szRootPath, std::vector<PatchFileA>& vecFile );

        //! http://msdn.microsoft.com/en-us/library/bb773559(v=vs.85)
        CString GetFileOnly(LPCTSTR Path);
        CString GetFileExt(LPCTSTR Path);
        CString GetFileName(LPCTSTR Path);

    } // namespace file
} // namespace sc
#endif // _FILE_SYSTEM_UTIL_H_