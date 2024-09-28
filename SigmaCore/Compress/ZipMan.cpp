#include "stdafx.h"
#include "../../=ZLib/iowin32.h"
#include "../String/StringUtil.h"
#include "../Log/LogMan.h"
#include "../gassert.h"

#include "./ZipMan.h"


//#include "../DebugInclude.h"


ZipMan& ZipMan::GetInstance()
{
    static ZipMan Instance;
    return Instance;
}

ZipMan::ZipMan()
{
}

ZipMan::~ZipMan()
{
    for (MAP_ZIP_ITER iter=m_ZipData.begin(); iter!=m_ZipData.end(); ++iter)
    {
        unzFile pZipFile = iter->second;
        unzCloseCurrentFile(pZipFile);
        unzClose(pZipFile);
    }    
}

//! �̸� Zip ���ϼ��� ��ġ�� ���������� �о�д�.
//
// fclose(pFile) �� ZipMan ���� ���ֹǷ� �ܺο��� �� �ʿ�� ����. ( return TRUE �� ��� )
// return FALSE �� ���� fclose(pFile) �� �ʿ��ϴ�.
//
// �� DeleteMemory �� ���ֵ��� ����. 
// LOADFILE_RCC ���� �ٸ���.
//
// strZipName	: m_mapZipPos, m_ZipData �� insert �ϱ� ���� �ʿ��ϴ�.
// pFile		: �̰� �������� unzFile �� �����.
BOOL ZipMan::LOAD_Memory( const std::string& strZipName, FILE* pFile )
{
	lockOn();

	unzFile pZipfile = unzOpenMemory( pFile );
	if (!pZipfile)
	{
		lockOff();
		return FALSE;
	}

	if (unzGoToFirstFile(pZipfile) == UNZ_OK)
	{
		do
		{
			ZIP_POS_INFO info;
			if (UNZ_OK != unzGetFilePos(pZipfile, &info.FilePos))
			{
				lockOff();
				return FALSE;
			}

			if (!GetFileInfo(pZipfile, info.FileInfo))
			{
				lockOff();
				return FALSE;
			}

			const std::string strFileName(info.FileInfo.szFileName);
			std::string strFullFileName(strZipName + strFileName);
			sc::string::lowerString(strFullFileName);
			m_mapZipPos.insert(MAPZIPPOS_VALUE(strFullFileName, info));

			char* pExt(strrchr(info.FileInfo.szFileName, '.'));
			++pExt;
			const std::string stringExtension(pExt);
			MAP_FILE_LIST_ITER iteratorFileList(m_mapFileList.find(stringExtension));
			if ( iteratorFileList == m_mapFileList.end() )
			{				
				m_mapFileList.insert(MAP_FILE_LIST_VALUE(stringExtension, VECTOR_FILE_NAME()));
				iteratorFileList = m_mapFileList.find(stringExtension);
			}
			VECTOR_FILE_NAME& fileNameVector(iteratorFileList->second);
			fileNameVector.push_back(strFileName);
		}
		while (unzGoToNextFile(pZipfile) == UNZ_OK);
		unzCloseCurrentFile(pZipfile);
	}

	m_ZipData.insert(MAP_ZIP_VALUE(strZipName, pZipfile));
	lockOff();
	return TRUE;
}

// LOAD_Memory ~ DeleteMemory
void ZipMan::DeleteMemory( const std::string& strZipName )
{
	lockOn();

	// ������ �ִ� ���� �����Ѵ�.
	MAP_ZIP_ITER iter = m_ZipData.find( strZipName );
	if ( iter != m_ZipData.end() )
	{
		unzFile pZipFile = iter->second;
		unzCloseCurrentFile(pZipFile);
		unzClose(pZipFile);

		m_ZipData.erase( iter );
	}

	lockOff();
}

//! �̸� Zip ���ϼ��� ��ġ�� ���������� �о�д�.
BOOL ZipMan::LOADFILE_RCC(const std::string& strZipName )
{
    lockOn();

    unzFile pZipfile = unzOpen(strZipName.c_str());
    if (!pZipfile)
    {
        lockOff();
        return FALSE;
    }

    if (unzGoToFirstFile(pZipfile) == UNZ_OK)
    {
        do
        {
            ZIP_POS_INFO info;
            if (UNZ_OK != unzGetFilePos(pZipfile, &info.FilePos))
            {
                lockOff();
                return FALSE;
            }
                      
            if (!GetFileInfo(pZipfile, info.FileInfo))
            {
                lockOff();
                return FALSE;
            }

            //UNZIPKEY unzKey;
            //unzKey.strZipName = strZipName;
            //unzKey.strFileName = info.szFileName;

			const std::string strFileName(info.FileInfo.szFileName);
            std::string strFullFileName(strZipName + strFileName);
            sc::string::lowerString(strFullFileName);
            m_mapZipPos.insert(MAPZIPPOS_VALUE(strFullFileName, info));

			char* pExt = NULL;
			if ( NULL != (pExt = strrchr(info.FileInfo.szFileName, '.')) )
			{
				++pExt;
				const std::string stringExtension(pExt);
				MAP_FILE_LIST_ITER iteratorFileList(m_mapFileList.find(stringExtension));
				if ( iteratorFileList == m_mapFileList.end() )
				{				
					m_mapFileList.insert(MAP_FILE_LIST_VALUE(stringExtension, VECTOR_FILE_NAME()));
					iteratorFileList = m_mapFileList.find(stringExtension);
				}
				VECTOR_FILE_NAME& fileNameVector(iteratorFileList->second);
				fileNameVector.push_back(strFileName);
			}
			else
			{
				sc::writeLogError( sc::string::format( "[%1%] Extension does not exist, If a File doesn't have extension in Zip, It won't load.", info.FileInfo.szFileName ) );
			}
        }
        while (unzGoToNextFile(pZipfile) == UNZ_OK);
        unzCloseCurrentFile(pZipfile);
    }
    
    m_ZipData.insert(MAP_ZIP_VALUE(strZipName, pZipfile));
    lockOff();
    return TRUE;
}

int ZipMan::GetFileCount(unzFile pZipFile)
{
    if (!pZipFile)
        return 0;
    unz_global_info info;
    if (unzGetGlobalInfo(pZipFile, &info) == UNZ_OK)
        return (int)info.number_entry;
    else
        return 0;
}

BOOL ZipMan::GOTOFILEPOS(unzFile pZipFile, std::string& strFileName, ZIP_POS_INFO& ZipPosInfo)
{
    sc::string::lowerString(strFileName);
    MAPZIPPOS_ITER iter = m_mapZipPos.find( strFileName );
    if (iter == m_mapZipPos.end())
        return FALSE;

    ZipPosInfo = iter->second;    
    if (UNZ_OK != unzGoToFilePos(pZipFile, &ZipPosInfo.FilePos))
        return FALSE;
    else
        return TRUE;
}

BOOL ZipMan::GetFileInfo(unzFile pZipFile, UZ_FileInfo& info)
{
    if (!pZipFile)
        return FALSE;

    unz_file_info uzfi;

    SecureZeroMemory(&info, sizeof(info));
    SecureZeroMemory(&uzfi, sizeof(uzfi));

    if (UNZ_OK != unzGetCurrentFileInfo(pZipFile, &uzfi, info.szFileName, MAX_PATH, NULL, 0, info.szComment, MAX_COMMENT))
        return FALSE;

    // copy across
    info.dwVersion = uzfi.version;	
    info.dwVersionNeeded = uzfi.version_needed;
    info.dwFlags = uzfi.flag;	
    info.dwCompressionMethod = uzfi.compression_method; 
    info.dwDosDate = uzfi.dosDate;  
    info.dwCRC = uzfi.crc;	 
    info.dwCompressedSize = uzfi.compressed_size; 
    info.dwUncompressedSize = uzfi.uncompressed_size;
    info.dwInternalAttrib = uzfi.internal_fa; 
    info.dwExternalAttrib = uzfi.external_fa; 

    // replace filename forward slashes with backslashes
    int nLen = lstrlen(info.szFileName);

    while (nLen--)
    {
        if (info.szFileName[nLen] == '/')
            info.szFileName[nLen] = '\\';
    }

    // is it a folder?
    info.bFolder = (info.szFileName[lstrlen(info.szFileName) - 1] == '\\');

    return TRUE;
}

BOOL ZipMan::GetFileInfo(unzFile pZipFile, int nFile, UZ_FileInfo& info)
{
    if (!pZipFile)
        return FALSE;

    if (!GotoFile(pZipFile, nFile))
        return FALSE;

    return GetFileInfo(pZipFile, info);
}

BOOL ZipMan::GotoNextFile(unzFile pZipFile, LPCTSTR szExt)
{
    if (!pZipFile)
        return FALSE;

    if (!szExt || !lstrlen(szExt))
        return (unzGoToNextFile(pZipFile) == UNZ_OK);

    // else
    UZ_FileInfo info;

    while (unzGoToNextFile(pZipFile) == UNZ_OK)
    {
        if (!GetFileInfo(pZipFile, info))
            return FALSE;

        // test extension
        char* pExt = strrchr(info.szFileName, '.');

        if (pExt)
        {
            pExt++;

            if (lstrcmpi(szExt, pExt) == 0)
                return TRUE;
        }
    }

    return FALSE;
}

BOOL ZipMan::GotoFirstFile(unzFile pZipFile, LPCTSTR szExt)
{
    if (!pZipFile)
        return FALSE;

    if (!szExt || !lstrlen(szExt))
        return (unzGoToFirstFile(pZipFile) == UNZ_OK);

    // else
    if (unzGoToFirstFile(pZipFile) == UNZ_OK)
    {
        UZ_FileInfo info;

        if (!GetFileInfo(pZipFile, info))
            return FALSE;

        // test extension
        char* pExt = strrchr(info.szFileName, '.');

        if (pExt)
        {
            pExt++;

            if (lstrcmpi(szExt, pExt) == 0)
                return TRUE;
        }

        return GotoNextFile(pZipFile, szExt);
    }

    return FALSE;
}

BOOL ZipMan::GotoFile(unzFile pZipFile, int nFile)
{
    if (!pZipFile)
        return FALSE;

    if (nFile < 0 || nFile >= GetFileCount(pZipFile))
        return FALSE;

    GotoFirstFile(pZipFile);

    while (nFile--)
    {
        if (!GotoNextFile(pZipFile))
            return FALSE;
    }

    return TRUE;
}

BOOL ZipMan::IsEncrypted(DWORD dwFlags)
{
    return ( dwFlags & (WORD)1) != 0;
}

// szFileName�� full pathname, ���ڵ� ����
UINT ZipMan::UnzipToMemory(const std::string& ZipFileName, const std::string& FileName, PBYTE& cBuffer)
{
    lockOn();

    // �̹� �����ϴ��� üũ
    MAP_ZIP_ITER iter = m_ZipData.find(ZipFileName);
    if (iter == m_ZipData.end())
    {
        lockOff();
        return 0;
    }

    unzFile pZipFile = iter->second;
    if (GetFileCount(pZipFile) == 0)
    {
        lockOff();
        return FALSE;
    }

    UINT nRetB(0);

    std::string strFileName(ZipFileName);
    strFileName += FileName;

    ZIP_POS_INFO ZipPosInfo;
    if (GOTOFILEPOS(pZipFile, strFileName, ZipPosInfo)) // GotoFilePos�� �̵�
    {
//      UZ_FileInfo info;
//      GetFileInfo(pZipFile, info);

        // if the item is a folder then simply return 'TRUE'
        //if (info.szFileName[lstrlen(info.szFileName) - 1] == '\\')
        //	continue;

        if (IsEncrypted(ZipPosInfo.FileInfo.dwFlags)) // MEMO : ��ȣȭ �� �����̸�...
        {
			// ��ȣ�� �����ؼ� Ǭ��.
			// ��ȣ ���� ��ġ�� CTexEncryptDlg::RunEncryptWidgetScriptFolder
			std::string strPass(FileName);
			for ( DWORD i=0; i<strPass.size(); ++i )
			{
				strPass[i] = strPass[i] + 0x54;
				strPass[i] = strPass[i] ^ 0xa4;
				strPass[i] = strPass[i] + 0x17;
			}
			std::stringstream sstrPass;
			sstrPass << strPass.c_str();
			sstrPass << ZipPosInfo.FileInfo.dwUncompressedSize;

			if (unzOpenCurrentFilePassword(pZipFile, sstrPass.str().c_str()) != UNZ_OK)
			{
				if (unzOpenCurrentFilePassword(pZipFile, "tkqwlf") != UNZ_OK)
				{
					lockOff();
					return UINT_MAX;
				}
			}
        }
        else
        {
            if (unzOpenCurrentFile(pZipFile) != UNZ_OK)
            {
                lockOff();
                return UINT_MAX;
            }
        }

        // read the file and output
        UINT nRet(0);
        char pBuffer[BUFFERSIZE] = "";
        cBuffer = new BYTE[ZipPosInfo.FileInfo.dwUncompressedSize]; // ȣ���ϴ� �κп��� �޸� ����

        do
        {
            nRet = unzReadCurrentFile(pZipFile, pBuffer, BUFFERSIZE);

            if (nRet > 0)
            {
				// Ȥ�� �� ���輺�� ���.
				// �ڲ� memcpy �� crash �� �ͼ� �߰��غ�.
				if ( (nRetB+nRet) > ZipPosInfo.FileInfo.dwUncompressedSize )
				{
					GASSERT( 0 && " (nRetB+nRet) > ZipPosInfo.FileInfo.dwUncompressedSize" );
					sc::writeLogError( sc::string::format( "memcpy error - [%1%] [%2%] [%3%] [%4%]", ZipFileName, FileName, nRetB+nRet, ZipPosInfo.FileInfo.dwUncompressedSize ) );

					unzCloseCurrentFile(pZipFile);
					lockOff();

					SAFE_DELETE_ARRAY ( cBuffer );

					return 0;
				}

                memcpy( cBuffer+nRetB, pBuffer, nRet );
                nRetB += nRet; // ���� ���� ũ��
            }
        }
        while (nRet > 0);

        unzCloseCurrentFile(pZipFile);
    }

    lockOff();
    return nRetB;
}

const ZipMan::VECTOR_FILE_NAME* const ZipMan::getFileListByExtension(const std::string& _stringExtension)
{
	MAP_FILE_LIST_CITER iteratorFileList(m_mapFileList.find(_stringExtension));
	if ( iteratorFileList == m_mapFileList.end() )
		return NULL;

	return &iteratorFileList->second;
}