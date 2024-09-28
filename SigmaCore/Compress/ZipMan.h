#ifndef _SC_ZIP_MAN_H_
#define _SC_ZIP_MAN_H_

#pragma once

#include <string>
#include <map>
#include "../../=zlib/unzip.h"
#include "../Util/Lock.h"

typedef unz_file_pos UNZ_FILE_POS;

//! 게임내 사용하는 zip 파일 관리
//! Open Close 를 자주하지 않기 위해서 제작
//! 원래 CUnzipper 를 사용했다.
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
    MAP_ZIP   m_ZipData; //! Zip file, 열려있는 zip 파일 pointer
    
public:

    //! 미리 Zip 파일속의 위치와 압축정보를 읽어둔다.
    BOOL LOADFILE_RCC(const std::string& strZipName);

	//! 미리 Zip 파일속의 위치와 압축정보를 읽어둔다.
	//
	// fclose(pFile) 는 ZipMan 에서 해주므로 외부에서 할 필요는 없다. ( return TRUE 일 경우 )
	// return FALSE 일 경우는 fclose(pFile) 가 필요하다.
	//
	// 꼭 DeleteMemory 를 해주도록 하자. 
	// LOADFILE_RCC 랑은 다르다.
	//
	// strZipName	: m_mapZipPos, m_ZipData 에 insert 하기 위해 필요하다. (FullPath)
	// pFile		: 이걸 기준으로 unzFile 을 만든다.
	BOOL LOAD_Memory( const std::string& strZipName, FILE* pFile );

	// LOAD_Memory ~ DeleteMemory
	void DeleteMemory( const std::string& strZipName );

	// strZipName	: 압축된 zip 파일 경로 (FullPath)
	// FileName		: 압축파일 안에 들어있는 파일 이름.
	// cBuffer		: 압축 해제된 Data
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