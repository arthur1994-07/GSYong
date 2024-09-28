#pragma once

#include <vector>
#include <string>
#include "../../SigmaCore/gassert.h"

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/CoreCommon.h"

namespace se
{
	class SerializeData
	{
	public:
		SerializeData() : m_pData( NULL ), m_dwSize( 0 ), m_dwCurPos( 0 )	{}
		SerializeData( BYTE* pData, const DWORD dwSize ) : 
			m_pData		( pData ),
			m_dwSize	( dwSize ),
			m_dwCurPos	( 0 )
		{}

		inline DWORD	GetRemainedSize()	const				{ GASSERT( m_dwSize > m_dwCurPos );		return m_dwSize - m_dwCurPos; }

	protected:
		BYTE*		m_pData;
		DWORD		m_dwSize;
		DWORD		m_dwCurPos;
	};

	class SerializeWritableData : public SerializeData
	{
	public:
		SerializeWritableData( BYTE* pData, const DWORD dwSize ) : SerializeData( pData, dwSize )
		{}

		inline BOOL			Write( const void* pSrc, const DWORD dwSize )
		{
			if( dwSize == 0 || GetRemainedSize() < dwSize )
				return FALSE;

			memcpy( m_pData + m_dwCurPos, pSrc, dwSize );
			m_dwCurPos += dwSize;
			return TRUE;
		}

		template<typename T>
		inline SerializeWritableData&		operator << ( T val )		
		{
			Write( &val, sizeof( T ) );
			return (*this);
		}
	};

class ByteStream : public sc::BaseStream
{
public:
	typedef std::vector<BYTE>			      BVECT;
	typedef std::vector<BYTE>::iterator	      BVECT_IT;
    typedef std::vector<BYTE>::const_iterator BVECT_CIT;

protected:
	BVECT m_Buffer;
	DWORD m_dwIter;

public:
	virtual BOOL operator << (short Value);
	virtual BOOL operator << (int Value);
	virtual BOOL operator << (WORD Value);
	virtual BOOL operator << (DWORD Value);
	virtual BOOL operator << (UINT Value);

	virtual BOOL operator << (float Value);
	virtual BOOL operator << (double Value);

	virtual BOOL operator << (const D3DVECTOR& Value);
	virtual BOOL operator << (const D3DCOLORVALUE& Value);
	virtual BOOL operator << (const D3DBLEND& Value);

	virtual BOOL operator << (BYTE Value);
	virtual BOOL operator << (char Value);
    virtual BOOL operator << (wchar_t Value);
	virtual BOOL operator << (const std::string& str);
    virtual BOOL operator << (const std::wstring& str);

	virtual BOOL operator << (bool Value);

	virtual BOOL operator << (LONGLONG Value);

	template<typename TYPE>
	BOOL operator << (const std::vector<TYPE>& vecVALUE)
	{
		BOOL bOK(FALSE);
		bOK = operator << ( (DWORD)vecVALUE.size() );
		if ( !bOK )					return FALSE;

		if ( vecVALUE.empty() )		return TRUE;
		return WriteBuffer ( &(vecVALUE[0]), DWORD(sizeof(TYPE)*vecVALUE.size()) );
	}

	virtual BOOL WriteBuffer (const void* pBuffer, DWORD Size);

private:
    //! 주의:이미 존재하는 버퍼를 덮어쓴다. 뒷쪽에 더해지는 것이 아니다.
    BOOL WriteBufferVector(const std::vector<BYTE>& vBuffer);

public:
	virtual BOOL operator >> (short& Value);
	virtual BOOL operator >> (int& Value);
	virtual BOOL operator >> (WORD& Value);
	virtual BOOL operator >> (DWORD& Value);
	virtual BOOL operator >> (UINT& Value);

	virtual BOOL operator >> (float& Value);
	virtual BOOL operator >> (double& Value);

	virtual BOOL operator >> (D3DVECTOR& Value);
	virtual BOOL operator >> (D3DCOLORVALUE& Value);
	virtual BOOL operator >> (D3DBLEND& Value);

	virtual BOOL operator >> (BYTE& Value);
	virtual BOOL operator >> (char& Value);
    virtual BOOL operator >> (wchar_t& Value);
	virtual BOOL operator >> (std::string& str);
    virtual BOOL operator >> (std::wstring& str);

	virtual BOOL operator >> (bool &Value);

	virtual BOOL operator >> (LONGLONG &Value);

	template<typename TYPE>
	BOOL operator >> (std::vector<TYPE>& vecVALUE)
	{
		BOOL bOK(FALSE);
		DWORD dwSize(0);

		vecVALUE.clear();

		bOK = operator >> ( dwSize );
		if ( !bOK )					return FALSE;

		if ( dwSize==0 )			return TRUE;

		vecVALUE.reserve(dwSize);
		for ( DWORD i=0; i<dwSize; ++i )
		{
			TYPE tVALUE;
			bOK = ReadBuffer ( &tVALUE, DWORD(sizeof(TYPE)) );
			if ( !bOK )				return FALSE;

			vecVALUE.push_back ( tVALUE );
		}

		return TRUE;
	}

	virtual BOOL ReadBuffer(void* pBuffer, DWORD Size);

public:
	virtual BOOL SetOffSet(long _OffSet);
	virtual long GetfTell();

public:
	BOOL IsEmpty() const { return m_Buffer.empty(); }
	void GetBuffer(LPBYTE& pBuff, DWORD& dwSize);

	void ResetIter() { m_dwIter=0; }
	void ClearBuffer();
	virtual void ReadFileType() OVERRIDE;
	virtual void GetFileType(TCHAR *szFileType, int nStrLen, DWORD &FileID) OVERRIDE;

public:
	ByteStream( const DWORD dwReserve = 2048 );
    ByteStream(const LPBYTE pBuff, const DWORD dwSize);
    ByteStream(const std::vector<BYTE>& vBuffer);
	virtual ~ByteStream();
};

} // namespace se