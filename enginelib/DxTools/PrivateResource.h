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

	// SaveFileDec (szFileNameIn �ε� �� ��ȣȭ ���� �� szFileNameOut �� �����.) �� ���� ����
	// �߰� �Ȱ� szFileNameOut ������ ������ ���̴�.
	// ��� �� fclose( pFile ); �� �� ����� �Ѵ�.
	//
	// szFileName	 : �ε� �� ����.
	// szNewFileName : szFileNameIn �� ��ȣȭ ���� �� Save �� ���� �̸�. 
	// FILE			 : szNewFileName �� ��������.
	FILE* LoadFile_Decrypt_NewFile( const char* szFileName, const char* szNewFileName );

	// szFileName : �ε� �� ��ȣȭ ���� �� ����.
	bool LoadFileDec( const char* szFileName );

	// szFileNameIn	 : �ε� �� ����.
	// szFileNameOut : szFileNameIn �� ��ȣȭ ���� �� Save �� ���� �̸�. 
	// bDelete		 : szFileNameIn ������ ���� ���ΰ�~?
	bool SaveFileDec( const char* szFileNameIn, const char* szFileNameOut, bool bDelete );

	// szFileNameIn	 : �ε� �� ����.
	// szFileNameOut : szFileNameIn �� ��ȣȭ �� Save �� ���� �̸�. 
	// bDelete		 : szFileNameIn ������ ���� ���ΰ�~?
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