#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <vector>
#include <map>
#include <string>

class DxPieceEdit;


//----------------------------------------------------------------------------------------------------------------------
//								D	x		P	i	e	c	e		C	o	n	t	a	i	n	e	r
//	Note : 
//----------------------------------------------------------------------------------------------------------------------
class DxPieceContainer
{
protected:
	static char		m_szPath[MAX_PATH];
	TSTRING			m_strPieceEditPath;

public:
	static void		SetPath( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	static char*	GetPath()				{ return m_szPath; }

	void		SetPieceEditPath( char* szPath )	{ m_strPieceEditPath = szPath; }
    const char* GetPieceEditPath()					{ return m_strPieceEditPath.c_str(); }

public:
    typedef std::tr1::unordered_map<std::string, DxPieceEdit*> MAPSTATICPIS;
	typedef MAPSTATICPIS::iterator	                           MAPSTATICPIS_ITER;
    typedef MAPSTATICPIS::value_type                           MAPSTATICPIS_VALUE;

	MAPSTATICPIS	m_mapStaticPis;

public:
	DxPieceEdit* LoadPiece ( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile );
	DxPieceEdit* ReLoadPiece ( LPDIRECT3DDEVICEQ pd3dDevice, class DxPiece* pPiece, const char *szFile );
	void ReleasePiece ( const char *szFile );
	DxPieceEdit* FindPiece ( const char *szFile );

public:
	void CleanUp();

public:
	DxPieceContainer();
	~DxPieceContainer();

public:

	//////////////////////////////////////////////////////////////////////////
	// [shhan][2014.10.31] 절대적 주의
	//						기존에는 GetInstance 만 썼지만,
	//						코드 내부에서 DxStaticAniFrame 값들을 WLD 나 PSF 가 같이 공유하면서 문제가 생겼다.
	//						WLD, PSF 서로 렌더할 때 셋팅이 변해야했다.
	//						그래서 따로 두는 방향으로 함.
	//						코드적 변경이 나름 적게 할 수 있는게 Instance 를 2개 만들어서 사용하게 되었다.


	// WLD 에서 사용
	static DxPieceContainer& GetInstance();

	// PSF 에서 사용하는건 이걸 호출해야 한다.
	static DxPieceContainer& GetInstancePSF();
};