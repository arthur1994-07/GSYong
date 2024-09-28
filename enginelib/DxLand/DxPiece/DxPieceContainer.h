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
	// [shhan][2014.10.31] ������ ����
	//						�������� GetInstance �� ������,
	//						�ڵ� ���ο��� DxStaticAniFrame ������ WLD �� PSF �� ���� �����ϸ鼭 ������ �����.
	//						WLD, PSF ���� ������ �� ������ ���ؾ��ߴ�.
	//						�׷��� ���� �δ� �������� ��.
	//						�ڵ��� ������ ���� ���� �� �� �ִ°� Instance �� 2�� ���� ����ϰ� �Ǿ���.


	// WLD ���� ���
	static DxPieceContainer& GetInstance();

	// PSF ���� ����ϴ°� �̰� ȣ���ؾ� �Ѵ�.
	static DxPieceContainer& GetInstancePSF();
};