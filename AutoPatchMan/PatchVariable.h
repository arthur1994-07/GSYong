#pragma once

enum emFileList
{	
	FILENAME_SIZE	= 64,
	SUBPATH_SIZE	= 128,
	MD5_SIZE		= 33, // MD5�� 32�ڸ� + ���Ṯ�� 1 = 33

	VER_1_OFFSET	= 196,
	VER_2_OFFSET	= 232, // 4����Ʈ ���ķ� 229�� �ƴ� 3����Ʈ �þ 232
	VER_3_OFFSET	= 236
};

struct	SFILENODE_OLD
{
	// Version 1
	CHAR FileName[FILENAME_SIZE];
	CHAR SubPath[SUBPATH_SIZE];
	INT Ver;
	// Version 2
	CHAR szMD5[MD5_SIZE];

public:
	SFILENODE_OLD() :
	  Ver(0)
	{
		SecureZeroMemory( FileName, sizeof( FileName ) );
		SecureZeroMemory( SubPath, sizeof( SubPath ) );
		SecureZeroMemory( szMD5, sizeof( szMD5 ) );
	}
};

// SFILENODE ����ü ��ó��ġ ������Ʈ���� �ִ�. �� �κ� ������ �� �� Ȯ�� �� �Բ� ���� �ʿ�
struct	SFILENODE
{
	// Version 1
	CHAR FileName[FILENAME_SIZE];
	CHAR SubPath[SUBPATH_SIZE];
	INT Ver;
	// Version 2
	CHAR szMD5[MD5_SIZE];
	// Version 3
	INT IsPreDown;

public:
	SFILENODE() :
	  Ver(0), IsPreDown(0)
	{
		SecureZeroMemory( FileName, sizeof( FileName ) );
		SecureZeroMemory( SubPath, sizeof( SubPath ) );
		SecureZeroMemory( szMD5, sizeof( szMD5 ) );
	}

	void SetData(SFILENODE *pData)
	{
		::StringCchCopy(FileName, FILENAME_SIZE, pData->FileName);
		::StringCchCopy(SubPath, SUBPATH_SIZE, pData->SubPath);
		::StringCchCopy(szMD5, MD5_SIZE, pData->szMD5);

		Ver = pData->Ver;
		IsPreDown = pData->IsPreDown;
	}

	void SetData(SFILENODE &data)
	{
		::StringCchCopy(FileName, FILENAME_SIZE, data.FileName);
		::StringCchCopy(SubPath, SUBPATH_SIZE, data.SubPath);
		::StringCchCopy(szMD5, MD5_SIZE, data.szMD5);

		Ver = data.Ver;
		IsPreDown = data.IsPreDown;
	}

	void SetData(int nVer)
	{
		SecureZeroMemory( FileName, sizeof( FileName ) );
		SecureZeroMemory( SubPath, sizeof( SubPath ) );
		SecureZeroMemory( szMD5, sizeof( szMD5 ) );

		Ver = nVer;
		IsPreDown = 0;
	}

	void Clear()
	{
		SecureZeroMemory( FileName, sizeof( FileName ) );
		SecureZeroMemory( SubPath, sizeof( SubPath ) );
		SecureZeroMemory( szMD5, sizeof( szMD5 ) );

		Ver = 0;
		IsPreDown = 0;
	}
};

typedef	std::vector<SFILENODE*>		FILEVECTOR;
typedef	FILEVECTOR::iterator		FILEVECTOR_ITER;

typedef	std::map<std::string, SFILENODE*>	FILEMAP;
typedef	FILEMAP::iterator					FILEMAP_ITER;