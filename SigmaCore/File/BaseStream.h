#ifndef _SC_BASE_STREAM_H_
#define _SC_BASE_STREAM_H_
#pragma warning(disable: 4235)
#include <string>
#include <vector>

// struct D3DVECTOR;
// struct D3DCOLORVALUE;
// enum D3DBLEND;
// 위 구조체 정의가 필요해서 include 했다. DirectX 를 사용하는건 아니다.
/*
#ifndef D3DVECTOR_DEFINED
typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif
#ifndef D3DCOLORVALUE_DEFINED
typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;
#define D3DCOLORVALUE_DEFINED
#endif
*/
#include "../../../RanThirdParty/DirectX/Include/d3d9types.h"

#include "../gassert.h"
#include "../Encrypt/Rijndael.h"

#define CALLERROR MIN_ASSERT("0&&sc::BaseStream::operator<<()는 호출되지 않습니다.")

enum FOPENTYPE
{
    FOT_READ,
    FOT_WRITE
};

#define FILETYPESIZE	128

enum EMSFILE_BLOCK
{
    EMBLOCK_SIZE = 100,
};

namespace sc
{

class BaseStream
{
public:
    BaseStream();
    virtual ~BaseStream();

protected:
    BOOL		m_IsFileOpened;
    FOPENTYPE	m_OpenType;
    bool		m_bencode;
    INT			m_nVersion;

    char		m_szFileName[_MAX_FNAME];
    char		m_szFileType[FILETYPESIZE];
    DWORD		m_FileID;

    DWORD		m_DefaultOffSet;
    sc::CRijndael m_oRijndael;

    long	m_nMarkPos[EMBLOCK_SIZE];		// 마킹 되는 것이 30개가 넘어 버리면 문제가 생긴다.
    DWORD	m_dwBlockMarkPOS;

protected:
    virtual void ReadFileType (){CALLERROR;}
    virtual void WriteFileType (){CALLERROR;}
    virtual size_t read ( void *pbuffer, DWORD dwSize ){CALLERROR; return UINT_MAX;}
    virtual size_t write ( const void* pbuffer, DWORD dwSize ){return UINT_MAX;}

public:
    virtual void SetFileType ( const char *szFileType, DWORD FileID ){CALLERROR;}
    virtual void GetFileType ( char *szFileType, int nStrLen, DWORD &FileID ){CALLERROR;}
    virtual void SetEncode ( bool bencode ){CALLERROR;}

    virtual BOOL IsOpen (){CALLERROR; return true;}
    virtual void CloseFile ( BOOL bRemind=TRUE ){CALLERROR;}
    virtual const char* GetFileName() { return m_szFileName; }
	virtual DWORD GetFileSize() {CALLERROR; return 0;}

    virtual DWORD GetDefaltOffSet (){CALLERROR; return UINT_MAX;}		//	기본 저장위치 옵셋을 가져옴.

	virtual BOOL IsSameRemindLoadSize( int nSize )	{CALLERROR; return TRUE;}	// 현재 로드 가능한 사이즈가 nSize 와 같은지 확인한다.

    virtual BOOL BeginBlock(){CALLERROR; return true;};
    virtual BOOL EndBlock(){CALLERROR; return true;};

    virtual DWORD ReadBlockSize (){CALLERROR; return UINT_MAX;}

    // ------- 저장하기.
    virtual BOOL operator << (short Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (int Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (WORD Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (DWORD Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (UINT Value) { CALLERROR; return TRUE;}

    virtual BOOL operator << (float Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (double Value) { CALLERROR; return TRUE;}

    virtual BOOL operator << (const D3DVECTOR& Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (const D3DCOLORVALUE& Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (const D3DBLEND& Value) { CALLERROR; return TRUE;}

    virtual BOOL operator << (BYTE Value) { CALLERROR; return TRUE;}
    virtual BOOL operator << (char Value) { CALLERROR; return TRUE;}

    virtual BOOL operator << (bool Value) { CALLERROR; return TRUE;}

    virtual BOOL operator << (const std::string &str) { CALLERROR;return TRUE;}

    virtual BOOL operator << (LONGLONG Value) { CALLERROR; return TRUE;}

    virtual BOOL WriteBuffer(const void* pBuffer, DWORD Size ){CALLERROR;return TRUE;}

    // ------- 불러오기.
    virtual BOOL operator >> (short& Value) = 0;
    virtual BOOL operator >> (int& Value) = 0;
    virtual BOOL operator >> (WORD& Value) = 0;
    virtual BOOL operator >> (DWORD& Value) = 0;
    virtual BOOL operator >> (UINT& Value) = 0;

    virtual BOOL operator >> (float& Value) = 0;
    virtual BOOL operator >> (double& Value) = 0;

    virtual BOOL operator >> (D3DVECTOR& Value) = 0;
    virtual BOOL operator >> (D3DCOLORVALUE& Value) = 0;
    virtual BOOL operator >> (D3DBLEND& Value) = 0;

    virtual BOOL operator >> (BYTE& Value) = 0;
    virtual BOOL operator >> (char& Value) = 0;

    virtual BOOL operator >> (bool& Value) = 0;

    virtual BOOL operator >> (std::string& str) = 0;

    virtual BOOL operator >> (LONGLONG& Value) = 0;

    virtual BOOL ReadBuffer(void* pBuffer, DWORD Size) = 0;

    virtual BOOL SetOffSet(long _OffSet) = 0;
    virtual long GetfTell() = 0;

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
}; // class BaseStream

} // namespace sc

#endif // _SC_BASE_STREAM_H_