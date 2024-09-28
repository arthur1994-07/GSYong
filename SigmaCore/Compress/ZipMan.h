#ifndef _SC_ZIP_MAN_H_
#define _SC_ZIP_MAN_H_

#pragma once

#include <string>
#include <map>
#include "../../=zlib/unzip.h"
#include "../Util/Lock.h"

typedef unz_file_pos UNZ_FILE_POS;

//! ���ӳ� ����ϴ� zip ���� ����
//! Open Close �� �������� �ʱ� ���ؼ� ����
//! ���� CUnzipper �� ����ߴ�.
//! 2011-08-17 jgkim
class ZipMan : public sc::DefaultLock
{
public:
    enum 
    { 
        MAX_COMMENT = 255,
        BUFFERSIZE  = 20480,
    };

    struct UZ_FileInfo
    {
        char szFileName[MAX_PATH + 1];
        char szComment[MAX_COMMENT + 1];

        DWORD dwVersion;  
        DWORD dwVersionNeeded;
        DWORD dwFlags;	 
        DWORD dwCompressionMethod; 
        DWORD dwDosDate;	
        DWORD dwCRC;   
        DWORD dwCompressedSize; 
        DWORD dwUncompressedSize;
        DWORD dwInternalAttrib; 
        DWORD dwExternalAttrib; 
        bool bFolder;

        UZ_FileInfo()
        {
            memset( szFileName, 0, sizeof(char)*(MAX_PATH+1) );
            memset( szComment, 0, sizeof(char)*(MAX_COMMENT+1) );
        };
    };

    struct ZIP_POS_INFO
    {
        UNZ_FILE_POS FilePos;
        UZ_FileInfo  FileInfo;
    };

    typedef std::map<std::string, ZIP_POS_INFO> MAPZIPPOS;
    typedef MAPZIPPOS::iterator                 MAPZIPPOS_ITER;
    typedef MAPZIPPOS::const_iterator           MAPZIPPOS_CITER;
    typedef MAPZIPPOS::value_type               MAPZIPPOS_VALUE;

    typedef std::map<std::string, unzFile> MAP_ZIP;
    typedef MAP_ZIP::iterator              MAP_ZIP_ITER;
    typedef MAP_ZIP::const_iterator        MAP_ZIP_CITER;
    typedef MAP_ZIP::value_type            MAP_ZIP_VALUE;

	typedef std::vector<std::string>				VECTOR_FILE_NAME;
	typedef VECTOR_FILE_NAME::const_iterator	VECTOR_FILE_NAME_CITER;
	typedef VECTOR_FILE_NAME::iterator			VECTOR_FILE_NAME_ITER;

	typedef std::map<std::string, VECTOR_FILE_NAME>	MAP_FILE_LIST;
	typedef MAP_FILE_LIST::const_iterator		MAP_FILE_LIST_CITER;
	typedef MAP_FILE_LIST::iterator					MAP_FILE_LIST_ITER;
	typedef MAP_FILE_LIST::value_type			MAP_FILE_LIST_VALUE;

public:
    static ZipMan& GetInstance();

private:
    ZipMan();
    ~ZipMan();

private:
	MAP_FILE_LIST m_mapFileList; //! file list, sorted by Extension;
    MAPZIPPOS m_mapZipPos; //! file, zip position
    MAP_ZIP   m_ZipData; //! Zip file, �����ִ� zip ���� pointer
    
public:

    //! �̸� Zip ���ϼ��� ��ġ�� ���������� �о�д�.
    BOOL LOADFILE_RCC(const std::string& strZipName);

	//! �̸� Zip ���ϼ��� ��ġ�� ���������� �о�д�.
	//
	// fclose(pFile) �� ZipMan ���� ���ֹǷ� �ܺο��� �� �ʿ�� ����. ( return TRUE �� ��� )
	// return FALSE �� ���� fclose(pFile) �� �ʿ��ϴ�.
	//
	// �� DeleteMemory �� ���ֵ��� ����. 
	// LOADFILE_RCC ���� �ٸ���.
	//
	// strZipName	: m_mapZipPos, m_ZipData �� insert �ϱ� ���� �ʿ��ϴ�. (FullPath)
	// pFile		: �̰� �������� unzFile �� �����.
	BOOL LOAD_Memory( const std::string& strZipName, FILE* pFile );

	// LOAD_Memory ~ DeleteMemory
	void DeleteMemory( const std::string& strZipName );

	// strZipName	: ����� zip ���� ��� (FullPath)
	// FileName		: �������� �ȿ� ����ִ� ���� �̸�.
	// cBuffer		: ���� ������ Data
	// return		: cBuffer Size
    UINT UnzipToMemory(const std::string& ZipFileName, const std::string& FileName, PBYTE& cBuffer);

	const VECTOR_FILE_NAME* const getFileListByExtension(const std::string& _stringExtension);


private:	
    int GetFileCount(unzFile pZipFile);
    BOOL GOTOFILEPOS(unzFile pZipFile, std::string& strFileName, ZIP_POS_INFO& ZipPosInfo);
    BOOL GetFileInfo(unzFile pZipFile, int nFile, UZ_FileInfo& info);
    BOOL GetFileInfo(unzFile pZipFile, UZ_FileInfo& info);
    BOOL GotoFile(unzFile pZipFile, int nFile);
    BOOL GotoFirstFile(unzFile pZipFile, LPCTSTR szExt=NULL);
    BOOL GotoNextFile(unzFile pZipFile, LPCTSTR szExt=NULL);
    BOOL IsEncrypted(DWORD dwFlags);
};

#endif