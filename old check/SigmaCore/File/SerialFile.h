// JDH, 2003.01.16
#ifndef SC_SERIALFILE_H_
#define SC_SERIALFILE_H_

#pragma once

#include <string>
#include "./BaseStream.h"

namespace sc
{

class SerialFile : public BaseStream
{
public:
    SerialFile();
    SerialFile(FOPENTYPE _Type, const char *_szFileName, bool bencode=false);
    virtual ~SerialFile();
    
protected:
	FILE* m_FileStream;
	
protected:
    BOOL NewFile(const char* pName);    
	virtual void ReadFileType();
	virtual void WriteFileType();    
    virtual size_t read(void *pbuffer, DWORD dwSize);
    virtual size_t write(const void* pbuffer, DWORD dwSize);    

public:

    virtual BOOL OpenFile(const FOPENTYPE _Type, const char* _szFileName, bool bencode=false);
    virtual BOOL ReadBuffer(void* pBuffer, DWORD Size);
	virtual void SetFileType(const char *szFileType, DWORD FileID);
	virtual void GetFileType(char* szFileType, int nStrLen, DWORD& FileID);
	virtual void SetEncode(bool bencode) { m_bencode = bencode; }	
	virtual BOOL IsOpen() { return m_IsFileOpened; }
	virtual void CloseFile( BOOL bRemind=TRUE );
	virtual const char* GetFileName() { return m_szFileName; }
	virtual DWORD GetFileSize();
	virtual DWORD GetDefaltOffSet() { return m_DefaultOffSet; }	//	기본 저장위치 옵셋을 가져옴.

    //!	파일 저장 위치를 변경.
    virtual BOOL SetOffSet(long _OffSet);
    //!	현재 저장위치를 가져옴.
    virtual long GetfTell();

	virtual BOOL IsSameRemindLoadSize( int nSize );				// 현재 로드 가능한 사이즈가 nSize 와 같은지 확인한다.

	virtual BOOL BeginBlock();
	virtual BOOL EndBlock();
	virtual DWORD ReadBlockSize();
    virtual BOOL WriteBuffer(const void* pBuffer, DWORD Size);

// ------- 저장하기.
public:
	virtual BOOL operator << (short Value);
	virtual BOOL operator << (int Value);
	virtual BOOL operator << (WORD Value);
	virtual BOOL operator << (DWORD Value);
	virtual BOOL operator << (UINT Value);
	virtual BOOL operator << (float Value);
	virtual BOOL operator << (double Value);	
	virtual BOOL operator << (BYTE Value);
	virtual BOOL operator << (char Value);
	virtual BOOL operator << (bool Value);
	virtual BOOL operator << (const std::string& str);
	virtual BOOL operator << (LONGLONG Value);
    virtual BOOL operator << (const D3DVECTOR& Value);
    virtual BOOL operator << (const D3DCOLORVALUE& Value);
    virtual BOOL operator << (const D3DBLEND& Value);

    template<typename TYPE>
    BOOL operator << (const std::vector<TYPE> &vecVALUE )
    {
        BOOL bOK(FALSE);
        DWORD dwVecSize = (DWORD) vecVALUE.size();
        bOK = operator << (dwVecSize );
        if (!bOK)
            return FALSE;

        if (vecVALUE.empty())
            return TRUE;
        return WriteBuffer(&(vecVALUE[0]), DWORD(sizeof(TYPE)*vecVALUE.size()));
    }

    // ------- 불러오기.
public:
	virtual BOOL operator >> (short& Value);
	virtual BOOL operator >> (int& Value);
	virtual BOOL operator >> (WORD& Value);
	virtual BOOL operator >> (DWORD& Value);
	virtual BOOL operator >> (UINT& Value);
	virtual BOOL operator >> (float& Value);
	virtual BOOL operator >> (double& Value);
	virtual BOOL operator >> (BYTE& Value);
	virtual BOOL operator >> (char& Value);
	virtual BOOL operator >> (std::string& str);
	virtual BOOL operator >> (bool& Value);
	virtual BOOL operator >> (LONGLONG& Value);
    virtual BOOL operator >> (D3DVECTOR &Value);
    virtual BOOL operator >> (D3DCOLORVALUE &Value);
    virtual BOOL operator >> (D3DBLEND &Value);

    template<typename TYPE>
    BOOL operator >> (std::vector<TYPE>& vecVALUE)
    {
        BOOL bOK(FALSE);
        DWORD dwSize(0);

        vecVALUE.clear();

        bOK = operator >> (dwSize);
        if (!bOK)
            return FALSE;

        if (dwSize==0)
            return TRUE;

        vecVALUE.reserve(dwSize);
        for (DWORD i=0; i<dwSize; ++i)
        {
            TYPE tVALUE;
            bOK = ReadBuffer(&tVALUE, DWORD(sizeof(TYPE)));
            if (!bOK)
                return FALSE;
            vecVALUE.push_back(tVALUE);
        }
        return TRUE;
    }
}; // class SerialFile

} // namespcae sc
#endif // SC_SERIALFILE_H_