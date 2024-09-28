// by 경대
//
#pragma once

#include "./BaseStream.h"

namespace sc
{

class SerialMemory : public BaseStream
{
public:
    SerialMemory();
    SerialMemory(const char *_szZipFileName, const char *_szFileName, bool bencode = false);
    virtual ~SerialMemory();

protected:
	char		m_szZipFileName[_MAX_PATH];
	PBYTE		m_pBuffer;
	UINT		m_nBufSize;
	DWORD		m_dwOffSet;
	
protected:
	virtual void ReadFileType ();
	virtual size_t read(void *pbuffer, DWORD dwSize);

public:
	virtual void GetFileType( char *szFileType, int nStrLen, DWORD &FileID );
	virtual void SetEncode ( bool bencode )	{ m_bencode = bencode; }

    BOOL OpenFile(const std::string& ZipFileName, const std::string& FileName, bool bencode = false, BOOL bPack = FALSE);
	virtual BOOL IsOpen() { return m_IsFileOpened; }
	virtual void CloseFile( BOOL bRemind=TRUE );
	virtual DWORD GetDefaltOffSet() { return m_DefaultOffSet; }	//	기본 저장위치 옵셋을 가져옴.
	virtual BOOL SetOffSet(long _OffSet);				//	파일 저장 위치를 변경.
    virtual long GetfTell();								//	현제 저장위치를 가져옴.
	virtual DWORD GetFileSize()			{ return m_nBufSize; }
	virtual BOOL BeginBlock();
	virtual BOOL EndBlock();
	virtual DWORD ReadBlockSize();
    virtual BOOL ReadBuffer( void* pBuffer, DWORD Size );

    // ------- 불러오기.
	virtual BOOL operator >> (short &Value);
	virtual BOOL operator >> (int &Value);
	virtual BOOL operator >> (WORD &Value);
	virtual BOOL operator >> (DWORD &Value);
	virtual BOOL operator >> (UINT &Value);
	virtual BOOL operator >> (float &Value);
	virtual BOOL operator >> (double &Value);	
	virtual BOOL operator >> (BYTE &Value);
	virtual BOOL operator >> (char &Value);
	virtual BOOL operator >> (std::string &str);
	virtual BOOL operator >> (bool &Value);
	virtual BOOL operator >> (LONGLONG &Value );
};

} // namespace sc