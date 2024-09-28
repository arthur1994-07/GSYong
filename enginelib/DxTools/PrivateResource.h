#pragma once

class CPrivateResource
{
public:
    enum
    {
        ETypeTexture,
        ETypeSkin,
		ETypeWidgetScript,
    };

private:
    BYTE m_eType;
    BYTE* m_pBuffer;
	int m_nSize;

    int m_nHeaderSize;
    int m_nVersion;
    BYTE m_nXorValue;
    BYTE m_nDiffValue;
	BYTE m_nDiffValue2;

public:
	CPrivateResource( BYTE eType );
	~CPrivateResource();

public:
    void Reset( bool bDelete = true );

public:

	// SaveFileDec (szFileNameIn 로드 후 암호화 해제 후 szFileNameOut 로 만든다.) 과 거의 같고
	// 추가 된건 szFileNameOut 파일을 리턴할 뿐이다.
	// 사용 후 fclose( pFile ); 를 꼭 해줘야 한다.
	//
	// szFileName	 : 로드 할 파일.
	// szNewFileName : szFileNameIn 을 암호화 해제 후 Save 될 파일 이름. 
	// FILE			 : szNewFileName 의 파일정보.
	FILE* LoadFile_Decrypt_NewFile( const char* szFileName, const char* szNewFileName );

	// szFileName : 로드 후 암호화 해제 할 파일.
	bool LoadFileDec( const char* szFileName );

	// szFileNameIn	 : 로드 할 파일.
	// szFileNameOut : szFileNameIn 을 암호화 해제 후 Save 될 파일 이름. 
	// bDelete		 : szFileNameIn 파일을 지울 것인가~?
	bool SaveFileDec( const char* szFileNameIn, const char* szFileNameOut, bool bDelete );

	// szFileNameIn	 : 로드 할 파일.
	// szFileNameOut : szFileNameIn 을 암호화 후 Save 될 파일 이름. 
	// bDelete		 : szFileNameIn 파일을 지울 것인가~?
    bool SaveFileEnc( const char* szFileNameIn, const char* szFileNameOut, bool bDelete );


private:
    bool Decrypt( FILE* pFile );
	bool DecryptToFile( FILE* pFile, const char* szFileNameIn, const char* szFileNameOut );
    bool EncryptToFile( FILE* pFile, const char* szFileNameIn, const char* szFileNameOut );

	bool DecryptBuffer( FILE* pFile );
    bool EncryptBuffer();

    int GetFileType( const char* szFileName );

public:
	BYTE* GetData() { return m_pBuffer; }
	int	GetSize() { return m_nSize; }
};