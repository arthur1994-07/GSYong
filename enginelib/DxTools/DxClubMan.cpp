#include "pch.h"
#include "../Common/StlFunctions.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "../../SigmaCore/File/SerialFile.h"
#include "../Common/gltexfile.h"

#include "./Texture/NSTexCompress.h"
#include "./TextureManager.h"

#include "DxClubMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxClubMan& DxClubMan::GetInstance()
{
	static DxClubMan Instance;
	return Instance;
}

const DWORD	DxClubMan::VERSION = 106;
//const DWORD	DxClubMan::CHAR_NUM = 100;
const DWORD	DxClubMan::FULL_WIDTH = 16;
const DWORD	DxClubMan::FULL_HEIGHT = 11;

const std::string	DxClubMan::m_strText = "Club_Tex.dat";
static const std::string strDEFAULTMARK = "Club_NoMark.bmp";
char DxClubMan::g_szPath[MAX_PATH]	= "";



//////////////////////////////////////////////////////////////////////////
DxClubMan::ID_DATA::ID_DATA( LPDIRECT3DDEVICEQ pd3dDevice )
	: m_nUnUsedCount(0)
	, m_dwVer(0)
	, m_pTexture(NULL)
{
	if ( !CreateClubTex( pd3dDevice ) )
	{
		// ���������� �� ������ ���߱� ������ ������ �ε��� �� �����ϵ��� �Ѵ�.
		m_nUnUsedCount = UNUSED_MAX_COUNT;
	}
}

DxClubMan::ID_DATA::~ID_DATA()
{
	if ( m_pTexture )
	{
		TextureManager::GetInstance().ReleaseTexture( m_pTexture, TEXTURE_UI );
	}
}

bool DxClubMan::ID_DATA::IsValidData( DWORD dwVer ) const
{
	if ( m_dwVer != dwVer )
		return false;	// ������ �ٸ���.

	return true;
}

BOOL DxClubMan::ID_DATA::CreateClubTex( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pTexture )
	{
		TextureManager::GetInstance().ReleaseTexture( m_pTexture, TEXTURE_UI );
	}

	UINT unWidth(FULL_WIDTH);
	UINT unHeight(FULL_HEIGHT);
	D3DFORMAT emFormat(D3DFMT_A8R8G8B8);
	TextureManager::GetInstance().CreateTexture
	(
		pd3dDevice,
		unWidth,
		unHeight, 
		1,
		0L, 
		emFormat,
		D3DPOOL_MANAGED,
		&m_pTexture,
		TEXTURE_RESIZE_NONE,
		TEXTURE_UI 
	);

	if ( m_pTexture )
		return TRUE;
	else
		return FALSE;
}

// dwVer - bTextureIN TRUE �� ��츸 m_dwVer �� ���� �����Ѵ�.
BOOL DxClubMan::ID_DATA::DrawClubTex( DWORD* pColor, BOOL bTextureIN, DWORD dwVer )
{
	if ( !pColor )
		return FALSE;

	if ( !m_pTexture )
		return FALSE;

	// [shhan][2015.01.29] �׷��� ����̹��� �����ȰŸ� ������ �߻��� �� �ִٶ�� ������ ����.
	//
	// http://www.intel.com/support/kr/graphics/sb/cs-029956.htm
	//
	// �׷��� ����̹�  
	//
	// LockRect Direct3D* ���� �߻�
	//
	//		�ַ�� 
	//
	//			�� ������ �ֽ� �׷��� ����̹��� �ذ�Ǿ����ϴ�. 
	//
	//			�׷��� ��Ʈ�ѷ��� ����� �� �ִ� �ֽ� �׷��� ����̹��� �ִ��� ���θ� Ȯ���Ϸ���  ���ڢ� ����̹� ������Ʈ ��ƿ��Ƽ, ���.
	//			�� �¶��� ��ƿ��Ƽ���� ����� ���ǵ� �׷��� ����̹��� ã�Ҵٴ� �޽����� ǥ�õǸ�, ����� ���ǵ� ��ǻ�� ������ü �׷��� ����̹��� �����������. 
	//
	//
	//		� ü��: 
	//
	//			Windows XP Professional x64 Edition*, Windows XP Professional*, Windows XP Home Edition*, Windows XP Tablet PC Edition*, Windows XP Media Center Edition*  
	//
	//
	//		���� ���:  
	//
	//			Intel�� G31 �ͽ������� Ĩ�� 
	//			Intel�� G33 �ͽ������� Ĩ�� 
	//			Intel�� G35 �ͽ������� Ĩ�� 
	//			Intel�� G41 �ͽ������� Ĩ�� 
	//			Intel�� G43 �ͽ������� Ĩ�� 
	//			Intel�� G45 �ͽ������� Ĩ�� 
	//			Intel�� Q33 �ͽ������� Ĩ�� 
	//			���ڢ� Q35 �ͽ������� Ĩ�� 
	//			Intel�� Q43 �ͽ������� Ĩ�� 
	//			���ڢ� Q45 �ͽ������� Ĩ�� 
	//			����� Intel�� 4 �ø��� �ͽ������� Ĩ�� ��ǰ�� 
	//			����� Intel�� 965 �ͽ������� Ĩ�� ��ǰ�� 
	//
	//			�ַ�� ID: CS-029956 
	//			������ ������: 2014�� 10�� 27�� 
	//			�ۼ���: 2008�� 12�� 07��



	// ��ü Lock ���� ��õ�
	DWORD			dwBPP = 32;
	D3DLOCKED_RECT	lrDst;
	HRESULT hr = m_pTexture->LockRect ( 0, &lrDst, NULL, 0 );
	if ( hr == D3D_OK )
	{
		DWORD* pDst       = (DWORD*)lrDst.pBits;

		for( DWORD y=0; y<HEIGHT; y++ )
		{
			for( DWORD x=0; x<WIDTH; x++ )
			{
				if ( bTextureIN )
				{
					m_dwVer = dwVer;
					pDst[x] = pColor[(y*WIDTH)+x];
				}
				else
				{
					pColor[(y*WIDTH)+x] = pDst[x];
				}
				
			}
			pDst = pDst+(lrDst.Pitch/(dwBPP/8));
		}

		m_pTexture->UnlockRect(0);
	}
	else
	{
		// �α� �����.
		CDebugSet::ToLogFile( std::string("m_pTexture->LockRect ( 0, &lrDst, NULL, 0 )") );

		// ��� ���Ѵٰ� �����ϰ� �����Ѵ�.
		TextureManager::GetInstance().ReleaseTexture( m_pTexture, TEXTURE_UI );

		return FALSE;	// intel Ĩ�� ���� �׷��ȵ���̹� ���Ǹ� ������ �߻��� �� ����.	// Ư�������� pDst[x] = pColor[(y*WIDTH)+x]; ���� ��� ������ BugTrap �� �ͼ� ��ȿ���˻� �߰���.
	}

	return TRUE;
}

void DxClubMan::ID_DATA::Save ( sc::SerialFile &SFile )
{
	SFile << m_dwVer;

	// m_pTexture ���� ���� ���´�.
	DWORD sClubMarkGet[WIDTH*HEIGHT];
	if ( !DrawClubTex( sClubMarkGet, FALSE, 0L ) )
	{
		// ���������� �� ������ ���߱� ������ ������ �ε��� �� �����ϵ��� �Ѵ�.
		m_nUnUsedCount = UNUSED_MAX_COUNT;
	}
	SFile.WriteBuffer( sClubMarkGet, sizeof(DWORD)*WIDTH*HEIGHT );

	SFile << m_nUnUsedCount;
}

BOOL DxClubMan::ID_DATA::Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile )
{
	SFile >> m_dwVer;

	DWORD sClubMarkToTexture[WIDTH*HEIGHT];
	SFile.ReadBuffer( sClubMarkToTexture, sizeof(DWORD)*WIDTH*HEIGHT );

	SFile >> m_nUnUsedCount;

	// �ε�ø��� ���������� �ø���.
	++m_nUnUsedCount;
	if ( m_nUnUsedCount > UNUSED_MAX_COUNT )
	{
		return FALSE;
	}

	// m_pTexture �� ���� �ִ´�.
	if ( !DrawClubTex( sClubMarkToTexture, TRUE, m_dwVer ) )
	{
		TextureManager::GetInstance().ReleaseTexture( m_pTexture, TEXTURE_UI );
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
DxClubMan::ID_SERVER::ID_SERVER()
	: m_bChangeData(FALSE)
{
}

DxClubMan::ID_SERVER::~ID_SERVER()
{
	std::for_each ( m_mapData.begin(), m_mapData.end(), std_afunc::DeleteMapObject() );
	m_mapData.clear();
}

BOOL DxClubMan::ID_SERVER::IsExistClub( int nID ) const
{
	ID_DATA_MAP_CITER iter = m_mapData.find(nID);
	if (iter == m_mapData.end())
		return FALSE;	// ID �� ����.

	return TRUE;
}

bool DxClubMan::ID_SERVER::IsValidData( int nID, DWORD dwVer ) const
{
	ID_DATA_MAP_CITER iter = m_mapData.find(nID);
	if (iter == m_mapData.end())
		return false;	// ID �� ����.

	return iter->second->IsValidData( dwVer );
}

LPDIRECT3DTEXTUREQ DxClubMan::ID_SERVER::GetTexture( int nID ) const
{
	ID_DATA_MAP_CITER iter = m_mapData.find(nID);
	if (iter == m_mapData.end())
		return NULL;

	return (*iter).second->GetTexture();
}

void DxClubMan::ID_SERVER::CreateClub( LPDIRECT3DDEVICEQ pd3dDevice, int nID )
{
	m_bChangeData = TRUE;

	ID_DATA_MAP_ITER iter = m_mapData.find(nID);
	if ( iter == m_mapData.end() )
	{
		ID_DATA* pData = new ID_DATA( pd3dDevice );
		m_mapData.insert( std::make_pair( nID, pData ) );
	}
}

void DxClubMan::ID_SERVER::SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nID, DWORD dwVer, DWORD *pClubMark )
{
	ID_DATA_MAP_ITER iter = m_mapData.find(nID);

	if ( iter != m_mapData.end() )		// �̷��� �ִ� �Ŵ�.
	{
		if ( !iter->second->IsValidData( dwVer ) )			// ���� �ٸ���.
		{
			m_bChangeData = TRUE;

			iter->second->DrawClubTex( pClubMark, TRUE, dwVer );
		}
	}
	else	// ���� ID ������ ����.
	{
		m_bChangeData = TRUE;

		ID_DATA* pData = new ID_DATA( pd3dDevice );
		pData->DrawClubTex( pClubMark, TRUE, dwVer );
		m_mapData.insert( std::make_pair( nID, pData ) );
	}
}

void DxClubMan::ID_SERVER::Save ( sc::SerialFile &SFile ) const
{
	DWORD dwSize = static_cast< DWORD >( m_mapData.size() );
	SFile << dwSize;

	{
		ID_DATA_MAP_CITER iter = m_mapData.begin();

		for ( ; iter!=m_mapData.end(); ++iter )
		{
			SFile << iter->first;
			iter->second->Save( SFile );
		}
	}
}

void DxClubMan::ID_SERVER::Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile )
{
	std::string strName;

	DWORD dwCount = 0;
	SFile >> dwCount;

	ID_DATA* pData = NULL;
	DWORD dwID = 0;
	for ( DWORD i=0; i<dwCount; ++i )
	{		
        SFile >> dwID;

		pData = new ID_DATA( pd3dDevice );
		if ( pData->Load( pd3dDevice, SFile ) )
		{
			m_mapData.insert ( std::make_pair(dwID,pData) );
		}
		else
		{
			SAFE_DELETE( pData );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
DxClubMan::DxClubMan()
    : COL( 0 )
    , ROW( 0 )
    , UV_DIS( 0.f, 0.f )
{	
}

DxClubMan::~DxClubMan ()
{
	std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
	m_mapServer.clear();
}

BOOL DxClubMan::LoadClubMark ( const char* cName, LPDWORD& pColor )
{
	// ������ �ִ��� ã��
	std::string strPathname;

	if ( strDEFAULTMARK==cName )
	{
		strPathname = g_szPath;
		strPathname += cName;
	}
	else
	{
		strPathname = m_strREGISTPATH;
		strPathname += cName;
	}

	FILE* file = _fsopen ( strPathname.c_str(), "rb", _SH_DENYNO );
	if ( !file )	
		return FALSE;   // ������ ����.
    fclose(file);

	//	BMP, TGA ���� ã��
	BOOL	bBMP = TRUE;
	std::string	strSrcName = cName;
	std::transform ( strSrcName.begin(), strSrcName.end(), strSrcName.begin(), tolower );
	std::string	strExtName;
	std::string::size_type idx = strSrcName.find('.');

    //	�����̸��� ��ħǥ�� ������ ���� �ʴ�.
	if ( idx == std::string::npos )
		return FALSE;

    //	�����̸��� Ȯ���ڸ� �����´�.
	strExtName = strSrcName.substr ( idx+1 );
	idx = strExtName.find("bmp");
	if ( idx == std::string::npos ) // bmp �ƴϴ�.
	{
		bBMP = FALSE;
		idx = strExtName.find("tga");
		if ( idx == std::string::npos )
			return FALSE;	// bmp �� tga �� �ƴϴ�.
	}

	// ������ ��������
	if ( bBMP )
	{
		if ( !LoadBMPFile ( strPathname.c_str(), WIDTH, HEIGHT, m_sClubMark ) )	
			return FALSE;
	}
	else
	{
		if ( !LoadTGAFile ( strPathname.c_str(), WIDTH, HEIGHT, m_sClubMark ) )	
			return FALSE;
	}

	pColor = m_sClubMark;	// !!!!

	return TRUE;
}

bool DxClubMan::IsValidData(int nServer, int nID, DWORD dwVer)
{
	ID_SERVER_MAP_ITER iter_s = m_mapServer.find(nServer);
	if (iter_s == m_mapServer.end())
		return false;

	ID_SERVER* pServer = iter_s->second;
	return pServer->IsValidData( nID, dwVer );	
}

bool DxClubMan::SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer, DWORD *pClubMark )
{
	ID_SERVER_MAP_ITER iter_s = m_mapServer.find(nServer);

	if ( iter_s==m_mapServer.end() )
	{
		NewServerData ( nServer );
		iter_s = m_mapServer.find(nServer);
	}

    if ( iter_s==m_mapServer.end() )
        return false;

    ID_SERVER* pMapServer = iter_s->second;
	pMapServer->SetClubData( pd3dDevice, nID, dwVer, pClubMark );

	return true;
}

const clubmark::DXDATA& DxClubMan::GetClubData( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID )
{
	// �ϴ� �ʱ�ȭ�� �� ���� �ؾ��Ѵ�.
	// �׷��� ������ ������ �ȵ� ������ ���� Texture ���� ����ϰ� �Ǿ� ������ �ȴ�.
	m_sTextureData.pddsTexture = NULL;

	ID_SERVER_MAP_ITER iter_s = m_mapServer.find(nServer);
	if ( iter_s != m_mapServer.end() )
	{
        ID_SERVER* pServerMap = iter_s->second;

		if ( pServerMap->IsExistClub( nID ) )
		{
			m_sTextureData.pddsTexture = pServerMap->GetTexture( nID );
		}	
	}

	if ( !m_sTextureData.pddsTexture )
	{
		// �ؽ��İ� ���ٸ� ����Ʈ ���� ������ �ϵ��� �Ѵ�.
		m_sTextureData.pddsTexture = m_sTextureDefaultClubMark.GetTexture();
	}

	D3DXVECTOR2 vTex(0.5f/FULL_WIDTH,0.5f/FULL_HEIGHT);
	m_sTextureData.vTex_1_LU = vTex;
	m_sTextureData.vTex_2_RU = vTex;
	m_sTextureData.vTex_3_LD = vTex;
	m_sTextureData.vTex_4_RD = vTex;

	m_sTextureData.vTex_2_RU.x += UV_DIS.x;
	m_sTextureData.vTex_3_LD.y += UV_DIS.y;
	m_sTextureData.vTex_4_RD.x += UV_DIS.x;
	m_sTextureData.vTex_4_RD.y += UV_DIS.y;

	return m_sTextureData;
}

void DxClubMan::NewServerData ( int nServer )
{
	ID_SERVER_MAP_ITER iter = m_mapServer.find(nServer);
	if ( iter != m_mapServer.end() )
        return;

	ID_SERVER* pServer = new ID_SERVER;
	m_mapServer.insert(ID_SERVER_MAP_VALUE(nServer,pServer));
}

void DxClubMan::SaveData_Main() const
{
	//////////////////////////////////////////////////////////////////////////
	// ��θ� �����.
	std::string strPathname;
	strPathname = g_szPath;
	strPathname += m_strText.c_str();


	//////////////////////////////////////////////////////////////////////////
	// ������ Save �ϱ� ���� ����.
	sc::SerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_WRITE, strPathname.c_str() );
	if ( !bOPEN )	// ó���̶�� ������ ���� �� �ִ�.
	{
		int fh;

		// ���� �� �������� ��� ������ ���� ���� �����Ƿ� ���鵵�� �Ѵ�.
		CreateDirectory ( g_szPath, NULL );

		// ������ ���� ���鵵�� �Ѵ�.
		fh = _creat( strPathname.c_str(), _S_IREAD | _S_IWRITE );
		if( fh == -1 )
			perror( "Couldn't create data file" );
		else
		{
			printf( "Created data file.\n" );
			_close( fh );
		}

		BOOL bOPEN = SFile.OpenFile ( FOT_WRITE, strPathname.c_str() );
		if ( !bOPEN )
		{
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Save �� �Ѵ�.
	{
		SFile << (DWORD)VERSION;

		SFile.BeginBlock();
		{
			DWORD dwSize = static_cast< DWORD >( m_mapServer.size() );
			SFile << dwSize;

			ID_SERVER_MAP_CITER iter = m_mapServer.begin();
			for ( ; iter!=m_mapServer.end(); ++iter )
			{
				SFile << (*iter).first;
				(*iter).second->Save ( SFile );
			}
		}
		SFile.EndBlock();
	}

	SFile.CloseFile();
}

void DxClubMan::SaveData() const
{
	ID_SERVER_MAP_CITER iter_s = m_mapServer.begin();
	for ( ; iter_s!=m_mapServer.end(); ++iter_s )
	{
		//////////////////////////////////////////////////////////////////////////
		// �ϳ��� ����Ȱ��� �ִٸ� Save �۾��� �ϵ��� �Ѵ�.
		if ( (*iter_s).second->IsChangeData() )
		{
			SaveData_Main();
			return;
		}
	}
}

void DxClubMan::LoadData( LPDIRECT3DDEVICEQ pd3dDevice )
{
	std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
	m_mapServer.clear();

	std::string strPathname;
	strPathname = g_szPath;
	strPathname += m_strText.c_str();

	sc::SerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_READ, strPathname.c_str() );
	if ( !bOPEN )
	{
		return;
	}

	DWORD dwVer = 0;
	SFile >> dwVer;

	if ( dwVer==VERSION )
	{
		DWORD dwBufferSize(0);
		SFile >> dwBufferSize;

		// ���� �ٸ��ٸ� Save �� ���� ���༭ ���������� ���鵵�� �Ѵ�.
		// �̷��� ���ϸ� ������ ���� �� ������ ������ �ʴ� �̻� �ʿ� ������ �Ұ����� ���� �ִ�.
		if ( !SFile.IsSameRemindLoadSize( dwBufferSize ) || (dwBufferSize==0) ) 
		{
			SFile.CloseFile();

			// [shhan][2015.01.14] ���� Save���� ������ �߻����� ���� �����Ƿ�(���� ����Ȯ��,������ BugTrap�� ����) �� Save �� �ؼ� ���� ���ӽ� ������ ������ �Ѵ�.
			SaveData_Main();

			return;
		}


		DWORD dwCount = 0;
		SFile >> dwCount;

		DWORD dwServer = 0;
		ID_SERVER* pServer = NULL;
		for ( DWORD i=0; i<dwCount; ++i )
		{			
			SFile >> dwServer;
			pServer = new ID_SERVER;
			pServer->Load ( pd3dDevice, SFile );

			m_mapServer.insert ( ID_SERVER_MAP_VALUE(dwServer,pServer) );
		}
	}
	else
	{
		// �ε��۾��� ���� ���ƶ�.
		// ���� ���׷� ����(LockRect) ������ �ƴ� �� �ִ�. ���� ������.
		// �Ϻα׷���ī�忡�� ������ �Ǵ°� ����.
	}

	SFile.CloseFile();
}

//void DxClubMan::NewData()
//{
//	std::string strPathname;
//	strPathname = g_szPath;
//	strPathname += m_strText.c_str();
//
//	int fh;
//
//	fh = _creat( strPathname.c_str(), _S_IREAD | _S_IWRITE );
//	if( fh == -1 )
//		perror( "Couldn't create data file" );
//	else
//	{
//		printf( "Created data file.\n" );
//		_close( fh );
//	}
//
//	// �� �̹����� ����Ѵٰ� �˸���.
//	FILE* file = _fsopen ( strPathname.c_str(), "wb", _SH_DENYNO );
//	if ( !file )
//        return;
//
//	fwrite ( &VERSION, sizeof(DWORD), 1, file );
//
//	DWORD dwCount = 0;
//	fwrite ( &dwCount, sizeof(DWORD), 1, file );
//
//	fclose(file);
//}

BOOL DxClubMan::LoadBMPFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor )
{
	FILE* file=_fsopen(strPathname,"rb",_SH_DENYNO);
	if( !file )
        return FALSE;

	BITMAPFILEHEADER	bfHeader;
	fread(&bfHeader,sizeof(BITMAPFILEHEADER),1,file);

	DWORD dwBufferSize = bfHeader.bfSize-bfHeader.bfOffBits;
	if ( bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER) > sizeof(BITMAPINFO)+(dwWidth*dwHeight)*sizeof(RGBQUAD) )
        return FALSE;
	
	BITMAPINFO* pbmi=(BITMAPINFO*)new BYTE[bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)];
	fread(pbmi,bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER),1,file);

	DWORD	dwStage	= 1;
	DWORD	dwBPP	= pbmi->bmiHeader.biBitCount;
	if ( dwBPP != 24 )							goto _RETURN;
	if ( dwWidth != pbmi->bmiHeader.biWidth )	goto _RETURN;
	if ( dwHeight != pbmi->bmiHeader.biHeight )	goto _RETURN;
	
	DWORD dwOffset;
	DWORD b;
	DWORD g;
	DWORD r;
	DWORD a;

	for ( DWORD y=0;y<dwHeight;++y )
	{
		dwOffset = (dwHeight-y-1)*dwWidth;
		for ( DWORD x=0;x<dwWidth;++x )
		{
			b=getc(file);
			g=getc(file);
			r=getc(file);
			a=0xff;
			pColor[dwOffset+x]=(a<<24)+(r<<16)+(g<<8)+b;
		}
	}

	SAFE_DELETE_ARRAY ( pbmi );
	fclose(file);
	return TRUE;

_RETURN:
	SAFE_DELETE_ARRAY ( pbmi );
	fclose(file);
	return FALSE;
}

BOOL DxClubMan::LoadTGAFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor )
{
	FILE* file = _fsopen( strPathname, "rb", _SH_DENYNO );
	if( !file )
        return FALSE;

	struct TargaHeader
	{
		BYTE	idLength;		
		BYTE	ColorMapType;	
		BYTE	ImageType;		
		WORD	ColorMapFirst;	
		BYTE	ColorMapBits;	
		WORD	FirstX;			
		WORD	FirstY;			
		WORD	Width;			
		WORD	Height;			
		BYTE	Bits;			
		BYTE	Descriptor;	
	}tga;

	fread( &tga, sizeof(TargaHeader), 1, file );

	DWORD dwBPP = tga.Bits;
	if ( dwBPP!=32 && dwBPP!=24 )	goto _RETURN;
	if ( dwWidth != tga.Width )		goto _RETURN;
	if ( dwHeight != tga.Height )	goto _RETURN;

	DWORD dwOffset;
	DWORD PacketInfo;
	DWORD PacketType;
	DWORD PixelCount;
	DWORD dwB;
	DWORD dwG;
	DWORD dwR;
	DWORD dwA;
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;

	for( DWORD y=0; y<dwHeight; y++ )
	{
		dwOffset = y*dwWidth;					//Origin in lower left-hand corner

		if( 0 == ( tga.Descriptor & 0x0010 ) )	//Origin in upper left-hand corner
			dwOffset = (dwHeight-y-1)*dwWidth;

		for( DWORD x=0; x<dwWidth; x )
		{
			//�̹��� Ÿ���� 10�� ���
			if( tga.ImageType == 10 )
			{
				PacketInfo = getc( file );
				PacketType = 0x80 & PacketInfo;
				PixelCount = ( 0x007f & PacketInfo ) + 1;

				if( PacketType )				//for the run length packet
				{
					dwB = getc( file );
					dwG = getc( file );
					dwR = getc( file );
					dwA = 0xff;
					if( dwBPP == 32 )
						dwA = getc( file );

					while( PixelCount-- )		//�ݺ��� Į�� ����
					{
						pColor[dwOffset+x] = (dwA<<24)+(dwR<<16)+(dwG<<8)+dwB;
						x++;
					}
				}
				else							//for the raw packet
				{
					while( PixelCount-- )		
					{
						b = getc( file );
						g = getc( file );
						r = getc( file );
						a = 0xff;
						if( dwBPP == 32 )
							a = getc( file );

						pColor[dwOffset+x] = (a<<24)+(r<<16)+(g<<8)+b;
						x++;
					}
				}
			}
			//�̹��� Ÿ���� 2�� ���
			else
			{
				b = getc( file );
				g = getc( file );
				r = getc( file );
				a = 0xff;
				if( dwBPP == 32 )
					a = getc( file );

				pColor[dwOffset+x] = (a<<24)+(r<<16)+(g<<8)+b;
				x++;
			}
		}
	}

	fclose( file );
	return TRUE;

_RETURN:
	fclose( file );
	return FALSE;
}

HRESULT DxClubMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD	dwWidth = WIDTH+1;
	DWORD	dwHeight = HEIGHT+1;

	COL = FULL_WIDTH/dwWidth;
	ROW = FULL_HEIGHT/dwHeight;

	UV_DIS = D3DXVECTOR2 ( (float)(WIDTH)/FULL_WIDTH, (float)(HEIGHT)/FULL_HEIGHT );
	
	//LPDWORD pMarkColor(NULL);
	//if ( LoadClubMark ( strDEFAULTMARK.c_str(), pMarkColor ) )
	//{
	//	// ������ ���� �۾��ϵ��� ����.
	//	memcpy(m_sDefaultClubMark, pMarkColor, sizeof(DWORD)*WIDTH*HEIGHT);
	//}
	
	m_sTextureDefaultClubMark = TextureManager::GetInstance().LoadTexture(
		strDEFAULTMARK.c_str(),
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_UI,
		false,
		TRUE );

	LoadData ( pd3dDevice );	// �ε�

	//// �׽�Ʈ													TEST
	//LPDWORD	pClubMark = NULL;//[WIDTH*HEIGHT];

	//for ( DWORD i=0; i<100; ++i )
	//{
	//	if ( SetClubMark ( "1.bmp", pClubMark ) )		// ����
	//	{
	//		// ������ sClubMark �����͸� ��������.
	//	}
	//	// sClubMark �������� �޴´�.
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+0, 100, pClubMark );		// �� ������ ó�� ���� ����� �ž�

	//	SetClubMark ( "2.bmp", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+1, 100, pClubMark );

	//	SetClubMark ( "3.bmp", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+2, 100, pClubMark );

	//	SetClubMark ( "4.bmp", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+3, 100, pClubMark );

	//	SetClubMark ( "4.tga", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+4, 100, pClubMark );
	//}

	//SaveData();

	/*
	if ( IsValidData ( 1, 0, 0 ) )
	{
	}

	LPDWORD	pClubMark = NULL;//[WIDTH*HEIGHT];

	for ( DWORD i=0; i<1000; ++i )
	{
		GetClubData ( pd3dDevice, 1, i, 1 );
	}

	for ( DWORD i=0; i<1000; ++i )
	{
		GetClubData ( pd3dDevice, 2, i, 1 );
	}
	*/

	return S_OK;
}

HRESULT DxClubMan::DeleteDeviceObjects()
{
	SaveData();	// ���̺�

	std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
	m_mapServer.clear();

	return S_OK;
}

HRESULT DxClubMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )		// �׽�Ʈ�� �ڵ�. ^^
{
	//struct	VERTEX_RHW
	//{
	//	D3DXVECTOR4	vPos;
	//	D3DXVECTOR2 vTex;
	//};

	//VERTEX_RHW	sVerties[4];

	//sVerties[0].vPos = D3DXVECTOR4 ( 0.f,	0.f, 1.f, 1.f );
	//sVerties[1].vPos = D3DXVECTOR4 ( 16.f,	0.f, 1.f, 1.f );
	//sVerties[2].vPos = D3DXVECTOR4 ( 0.f,	11.f, 1.f, 1.f );
	//sVerties[3].vPos = D3DXVECTOR4 ( 16.f,	11.f, 1.f, 1.f );

	//DXDATA sData = GetClubData ( pd3dDevice, 0, 0, 100 );

	//sVerties[0].vTex = sData.vTex_1_LU;
	//sVerties[1].vTex = sData.vTex_2_RU;
	//sVerties[2].vTex = sData.vTex_3_LD;
	//sVerties[3].vTex = sData.vTex_4_RD;

	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState ( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );

	//pd3dDevice->SetFVF ( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	//pd3dDevice->SetTexture ( 0, sData.pddsTexture );
	//pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sVerties, sizeof(VERTEX_RHW) );

	return S_OK;
}