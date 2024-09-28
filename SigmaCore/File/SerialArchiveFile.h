#ifndef SC_SERIALARCHIVEFILE_H_
#define SC_SERIALARCHIVEFILE_H_

#include "SerialFile.h"

namespace sc
{

/**
	��ī�̺� ��� ������ ����
    ������ �ø��� ����� ��� �Ѵ�.

    �츮�� ������ ���ų� ������ ���ϸ��� �ִ����� üũ�Ͽ�
    ���ϸ��ϋ� �ش� ������ �ִٸ� ���� �Ѵ�.
	
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