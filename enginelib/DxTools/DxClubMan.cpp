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
		// 정상적으로 값 셋팅을 못했기 때문에 다음에 로딩할 때 삭제하도록 한다.
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
		return false;	// 버젼이 다르다.

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

// dwVer - bTextureIN TRUE 일 경우만 m_dwVer 에 값을 셋팅한다.
BOOL DxClubMan::ID_DATA::DrawClubTex( DWORD* pColor, BOOL bTextureIN, DWORD dwVer )
{
	if ( !pColor )
		return FALSE;

	if ( !m_pTexture )
		return FALSE;

	// [shhan][2015.01.29] 그래픽 드라이버가 오래된거면 오류가 발생할 수 있다라는 문서가 있음.
	//
	// http://www.intel.com/support/kr/graphics/sb/cs-029956.htm
	//
	// 그래픽 드라이버  
	//
	// LockRect Direct3D* 오류 발생
	//
	//		솔루션 
	//
	//			이 문제는 최신 그래픽 드라이버로 해결되었습니다. 
	//
	//			그래픽 컨트롤러에 사용할 수 있는 최신 그래픽 드라이버가 있는지 여부를 확인하려면  인텔® 드라이버 업데이트 유틸리티, 사용.
	//			이 온라인 유틸리티에서 사용자 정의된 그래픽 드라이버를 찾았다는 메시지가 표시되면, 사용자 정의된 컴퓨터 제조업체 그래픽 드라이버를 사용할페이지. 
	//
	//
	//		운영 체제: 
	//
	//			Windows XP Professional x64 Edition*, Windows XP Professional*, Windows XP Home Edition*, Windows XP Tablet PC Edition*, Windows XP Media Center Edition*  
	//
	//
	//		적용 대상:  
	//
	//			Intel® G31 익스프레스 칩셋 
	//			Intel® G33 익스프레스 칩셋 
	//			Intel® G35 익스프레스 칩셋 
	//			Intel® G41 익스프레스 칩셋 
	//			Intel® G43 익스프레스 칩셋 
	//			Intel® G45 익스프레스 칩셋 
	//			Intel® Q33 익스프레스 칩셋 
	//			인텔® Q35 익스프레스 칩셋 
	//			Intel® Q43 익스프레스 칩셋 
	//			인텔® Q45 익스프레스 칩셋 
	//			모바일 Intel® 4 시리즈 익스프레스 칩셋 제품군 
	//			모바일 Intel® 965 익스프레스 칩셋 제품군 
	//
	//			솔루션 ID: CS-029956 
	//			마지막 수정일: 2014년 10월 27일 
	//			작성일: 2008년 12월 07일



	// 전체 Lock 으로 재시도
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
		// 로그 남긴다.
		CDebugSet::ToLogFile( std::string("m_pTexture->LockRect ( 0, &lrDst, NULL, 0 )") );

		// 사용 못한다고 생각하고 삭제한다.
		TextureManager::GetInstance().ReleaseTexture( m_pTexture, TEXTURE_UI );

		return FALSE;	// intel 칩의 구형 그래픽드라이버 사용되면 오류가 발생할 수 있음.	// 특정유저가 pDst[x] = pColor[(y*WIDTH)+x]; 에서 계속 뻑나는 BugTrap 이 와서 유효성검사 추가함.
	}

	return TRUE;
}

void DxClubMan::ID_DATA::Save ( sc::SerialFile &SFile )
{
	SFile << m_dwVer;

	// m_pTexture 에서 값을 얻어온다.
	DWORD sClubMarkGet[WIDTH*HEIGHT];
	if ( !DrawClubTex( sClubMarkGet, FALSE, 0L ) )
	{
		// 정상적으로 값 셋팅을 못했기 때문에 다음에 로딩할 때 삭제하도록 한다.
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

	// 로드시마다 지속적으로 올린다.
	++m_nUnUsedCount;
	if ( m_nUnUsedCount > UNUSED_MAX_COUNT )
	{
		return FALSE;
	}

	// m_pTexture 에 값을 넣는다.
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
		return FALSE;	// ID 가 없다.

	return TRUE;
}

bool DxClubMan::ID_SERVER::IsValidData( int nID, DWORD dwVer ) const
{
	ID_DATA_MAP_CITER iter = m_mapData.find(nID);
	if (iter == m_mapData.end())
		return false;	// ID 가 없다.

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

	if ( iter != m_mapData.end() )		// 이러면 있는 거다.
	{
		if ( !iter->second->IsValidData( dwVer ) )			// 버젼 다르다.
		{
			m_bChangeData = TRUE;

			iter->second->DrawClubTex( pClubMark, TRUE, dwVer );
		}
	}
	else	// 현재 ID 정보가 없다.
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
	// 파일이 있는지 찾기
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
		return FALSE;   // 파일이 없다.
    fclose(file);

	//	BMP, TGA 인지 찾기
	BOOL	bBMP = TRUE;
	std::string	strSrcName = cName;
	std::transform ( strSrcName.begin(), strSrcName.end(), strSrcName.begin(), tolower );
	std::string	strExtName;
	std::string::size_type idx = strSrcName.find('.');

    //	파일이름이 마침표를 가지고 있지 않다.
	if ( idx == std::string::npos )
		return FALSE;

    //	파일이름을 확장자를 가져온다.
	strExtName = strSrcName.substr ( idx+1 );
	idx = strExtName.find("bmp");
	if ( idx == std::string::npos ) // bmp 아니다.
	{
		bBMP = FALSE;
		idx = strExtName.find("tga");
		if ( idx == std::string::npos )
			return FALSE;	// bmp 도 tga 도 아니다.
	}

	// 데이터 가져오기
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
	// 일단 초기화를 해 놓고 해야한다.
	// 그렇지 않으면 셋팅이 안된 유저는 이전 Texture 값을 사용하게 되어 문제가 된다.
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
		// 텍스쳐가 없다면 디폴트 값을 렌더링 하도록 한다.
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
	// 경로를 만든다.
	std::string strPathname;
	strPathname = g_szPath;
	strPathname += m_strText.c_str();


	//////////////////////////////////////////////////////////////////////////
	// 파일을 Save 하기 위해 연다.
	sc::SerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_WRITE, strPathname.c_str() );
	if ( !bOPEN )	// 처음이라면 파일이 없을 수 있다.
	{
		int fh;

		// 정말 비 정상적인 경우 폴더가 없을 수도 있으므로 만들도록 한다.
		CreateDirectory ( g_szPath, NULL );

		// 파일을 새로 만들도록 한다.
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
	// Save 를 한다.
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
		// 하나라도 변경된것이 있다면 Save 작업을 하도록 한다.
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

		// 만약 다르다면 Save 를 새로 해줘서 정상파일을 만들도록 한다.
		// 이렇게 안하면 유저가 직접 이 파일을 지우지 않는 이상 맵에 접속이 불가능할 수도 있다.
		if ( !SFile.IsSameRemindLoadSize( dwBufferSize ) || (dwBufferSize==0) ) 
		{
			SFile.CloseFile();

			// [shhan][2015.01.14] 이전 Save에서 문제가 발생했을 수도 있으므로(거의 낮은확률,하지만 BugTrap은 왔음) 재 Save 를 해서 다음 접속시 문제가 없도록 한다.
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
		// 로딩작업을 하지 말아라.
		// 이전 버그로 인해(LockRect) 정상이 아닐 수 있다. 새로 만들어라.
		// 일부그래픽카드에서 문제가 되는거 같음.
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
//	// 이 이미지를 사용한다고 알린다.
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
			//이미지 타입이 10인 경우
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

					while( PixelCount-- )		//반복된 칼라 정보
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
			//이미지 타입이 2인 경우
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
	//	// 성공될 때만 작업하도록 하자.
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

	LoadData ( pd3dDevice );	// 로딩

	//// 테스트													TEST
	//LPDWORD	pClubMark = NULL;//[WIDTH*HEIGHT];

	//for ( DWORD i=0; i<100; ++i )
	//{
	//	if ( SetClubMark ( "1.bmp", pClubMark ) )		// 성공
	//	{
	//		// 서버로 sClubMark 데이터를 보내겠지.
	//	}
	//	// sClubMark 서버에서 받는다.
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+0, 100, pClubMark );		// 이 문장은 처음 봐서 만드는 거야

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
	SaveData();	// 세이브

	std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
	m_mapServer.clear();

	return S_OK;
}

HRESULT DxClubMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )		// 테스트용 코드. ^^
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