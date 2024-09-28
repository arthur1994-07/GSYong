#ifndef SC_SERIALARCHIVEFILE_H_
#define SC_SERIALARCHIVEFILE_H_

#include "SerialFile.h"

namespace sc
{

/**
	아카이브 기능 수행을 위해
    파일의 시리얼 기능을 사용 한다.

    우리는 파일을 쓰거나 읽을때 파일명이 있는지를 체크하여
    파일명일떄 해당 파일이 있다면 추출 한다.
	
 */
class SerialArchiveFile
    : public SerialFile
{
public:
    SerialArchiveFile(boost::function<void (const TCHAR*)> pArchive, const TCHAR* pFile = 0);    
    ~SerialArchiveFile();

    virtual size_t read(void *pbuffer, DWORD dwSize);
    virtual size_t write(const void* pbuffer, DWORD dwSize);

    virtual void Tokenizer(const TCHAR* pStream, int nSize);
    virtual void OnArchive(const TCHAR* pArchiveFile);

    virtual BOOL SetOffSet(long _OffSet) { if (m_OpenType == FOT_READ) return SerialFile::SetOffSet(_OffSet); else return TRUE; }
    virtual long GetfTell() { if (m_OpenType == FOT_READ) return SerialFile::GetfTell(); else return m_streaming; }
    virtual BOOL OpenFile(const FOPENTYPE _Type, const char* _szFileName, bool bencode=false);
    virtual BOOL BeginBlock();
    virtual BOOL EndBlock();
    virtual BOOL WriteBuffer(const void* pBuffer, DWORD Size) { write(pBuffer, Size); return TRUE; }
    virtual BOOL ReadBuffer(void* pBuffer, DWORD Size) { read(pBuffer, Size); return TRUE; }

private:
    boost::function<void (const TCHAR*)> m_pArchive;
    long m_streaming;
};

} // namespcae sc

#endif