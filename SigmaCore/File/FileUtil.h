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

        //! ������ ������ �ִ��� �˻��Ѵ�.
        bool isExist( const wchar_t* szFullPath );
        bool isExist( const std::wstring& strFullPath );
        bool isExist( const char* szFullPath );
        bool isExist( const std::string& strFullPath );

        //! ������ ������ ���̸����� �����Ѵ�.
        //! ����:���� �̸��� ����Ǹ� ������ ���ϵ����
        //! ��� path �� ����ȴ�.
        bool rename( const std::wstring& strOldPath, const std::wstring& strNewPath );

        //! �ڵ����� �����̸��� �ٲ۴�.
        //! ��Ģ:Ȯ���� ���� �����̸��� ���ڰ� �ϳ��� �����Ѵ�.
        //! ex) a.txt -> a1.txt -> a2.txt -> a3.txt	
        bool autoRename(const std::wstring& strFullPath);
        bool autoRename(const std::string& strFullPath);

        //! from ���� to �� ������ copy �Ѵ�.
		bool copyFile( const std::string& fromFullPath, const std::string& toFullPath, BOOL bOverwrite=TRUE );
        bool copyFile(const std::wstring& fromFullPath, const std::wstring& toFullPath, BOOL bOverwrite=TRUE );
        bool copyFile(const CStringW& fromFullPath, const CStringW& toFullPath, BOOL bOverwrite=TRUE);

        //! ���丮�� �����Ѵ�.
        //! �̹� ���丮�� ������ ����
        //! �����Ϸ��� ���丮 �̸��� ���� �̸���
        //! ������ �ִٸ� ����
        bool createDirectory( const wchar_t* szPath );
        bool createDirectory( const char* szPath );
        bool createDirectory( const std::wstring& Path );
        bool createDirectory( const std::string& Path );

        bool isDirectory( const wchar_t* szPath );
        bool isDirectory( const char* szPath );
        bool isDirectory( const std::wstring& Path );
        bool isDirectory( const std::string& Path );

        //! �����̳� ���丮�� �����
        bool remove( const wchar_t* szFullPath );
        bool remove( const char* szFullPath );
        bool remove( const std::wstring& Path );
        bool remove( const std::string& Path );

        //! �ش� ���丮�� ��� ������ �����.
        void deleteFiles(const std::string& Path);

        //! ���� Ȯ���ڸ� �����ش�.
        //! test.txt -> txt
        std::wstring getFileExtW(const std::wstring& strFullPath);
        std::string  getFileExt(const std::string& strFullPath);

        //! ������ Ȯ���ڸ� strNewExt �� ������ ���ϸ��� �����ش�.
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

        //! ������ ������ Write Ÿ���� ���Ѵ�.
        bool getFileLastWriteTime( const std::wstring& FileName, __time64_t& WriteTime );

        //! ������ ������ Access Ÿ���� ���Ѵ�.
        bool getFileLastAccessTime( const std::wstring& FileName, __time64_t& AccessTime );

        //! ������ Create Ÿ���� ���Ѵ�.
        bool getFileCreateTime( const std::wstring& FileName, __time64_t& CreateTime );

        //! ������ Create / Access / Write Ÿ���� ���Ѵ�.
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
        //! RootPath �Ʒ��� �ִ� ���ϵ��� ����Ʈ�� �ۼ��Ѵ�.
        void makeFileList(const char* szRootPath, std::vector<PatchFileA>& vecFile );

        //! http://msdn.microsoft.com/en-us/library/bb773559(v=vs.85)
        CString GetFileOnly(LPCTSTR Path);
        CString GetFileExt(LPCTSTR Path);
        CString GetFileName(LPCTSTR Path);

    } // namespace file
} // namespace sc
#endif // _FILE_SYSTEM_UTIL_H_