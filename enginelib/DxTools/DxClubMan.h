#if !defined(_DXCLUBMAN_H__INCLUDED_)
#define _DXCLUBMAN_H__INCLUDED_

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <list>
#include <algorithm>
#include "./TextureManager.h"

namespace sc {
	class SerialFile;
}

namespace clubmark
{

	struct DXDATA
	{
		LPDIRECT3DTEXTUREQ pddsTexture;
		D3DXVECTOR2 vTex_1_LU;
		D3DXVECTOR2 vTex_2_RU;
		D3DXVECTOR2 vTex_3_LD;
		D3DXVECTOR2 vTex_4_RD;

		DXDATA()
			: pddsTexture( NULL )
			, vTex_1_LU( 0, 0 )
			, vTex_2_RU( 0, 0 )
			, vTex_3_LD( 0, 0 )
			, vTex_4_RD( 0, 0 )
		{
		}
	};

}

class DxClubMan
{
public:
	static DxClubMan& GetInstance();

protected:
	const static DWORD VERSION;
	//const static DWORD CHAR_NUM;
	const static DWORD FULL_WIDTH;
	const static DWORD FULL_HEIGHT;

	enum
	{
		WIDTH = 16,
		HEIGHT = 11,

		UNUSED_MAX_COUNT = 100,		// ���� Ƚ���̻� ������ �ʴ´ٸ� �����ϱ� ���� ������.
	};

protected:
	class ID_DATA
	{
	private:
		int					m_nUnUsedCount;		// ���� Ƚ���̻� ������ �ʴ´ٸ� �����ϱ� ���� ������. <UNUSED_MAX_COUNT>
		DWORD				m_dwVer;
		LPDIRECT3DTEXTUREQ	m_pTexture;			// ��ǻ�� ������ Texture �� ���� �ȵ� �� �� �ִ�.

	public:
		bool				IsValidData( DWORD dwVer ) const;
		LPDIRECT3DTEXTUREQ	GetTexture() const						{ return m_dwVer == 0 ? NULL : m_pTexture; }	// Version �� 0 �̶�� ���� Texture �� Data �� ��� �ȵ� �����̴�.

		// dwVer - bTextureIN TRUE �� ��츸 m_dwVer �� ���� �����Ѵ�.
		BOOL DrawClubTex ( DWORD* pColor, BOOL bTextureIN, DWORD dwVer );    // Tex�� ������ ����� �׸���.

		void Save ( sc::SerialFile &SFile );
		BOOL Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile );

	private:
		BOOL CreateClubTex( LPDIRECT3DDEVICEQ pd3dDevice );

	public:
		ID_DATA( LPDIRECT3DDEVICEQ pd3dDevice );
		~ID_DATA();
	};

	typedef std::map<DWORD, ID_DATA*>   ID_DATA_MAP;		// Ŭ�����̵�, Ŭ�� ����
	typedef ID_DATA_MAP::iterator       ID_DATA_MAP_ITER;
	typedef ID_DATA_MAP::const_iterator ID_DATA_MAP_CITER;

protected:
	class ID_SERVER
	{
	private:
		BOOL		m_bChangeData;
		ID_DATA_MAP m_mapData;

	public:
		BOOL				IsChangeData() const						{ return m_bChangeData;	}
		BOOL				IsExistClub( int nID ) const;
		bool				IsValidData( int nID, DWORD dwVer ) const;
		LPDIRECT3DTEXTUREQ	GetTexture( int nID ) const;

		void CreateClub( LPDIRECT3DDEVICEQ pd3dDevice, int nID );
		void SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nID, DWORD dwVer, DWORD *pClubMark );

		void Save ( sc::SerialFile &SFile ) const;
		void Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile );

	public:
		ID_SERVER();
		~ID_SERVER ();
	};

	typedef std::map<DWORD, ID_SERVER*>	  ID_SERVER_MAP;		// ������ȣ, Ŭ�� ������
	typedef ID_SERVER_MAP::iterator		  ID_SERVER_MAP_ITER;
	typedef ID_SERVER_MAP::const_iterator ID_SERVER_MAP_CITER;
	typedef ID_SERVER_MAP::value_type	  ID_SERVER_MAP_VALUE;

protected:
	static char g_szPath[MAX_PATH];
	std::string m_strREGISTPATH;

	DWORD m_sClubMark[WIDTH*HEIGHT];	    // ���ο� Ŭ�� ��ũ �迭

	const static std::string m_strText;		// ����
	//std::string m_strBaseName;	            // �̹��� �⺻
	//std::string m_strExtName;	            // �̹��� Ȯ����

	DWORD COL;
	DWORD ROW;
	D3DXVECTOR2	UV_DIS;

	ID_SERVER_MAP m_mapServer;

	//DWORD				m_sDefaultClubMark[WIDTH*HEIGHT];	// ����Ʈ Ŭ�� ��ũ �迭
	TextureResource		m_sTextureDefaultClubMark;

public:
	clubmark::DXDATA m_sTextureData;	// GetClubData() �޼ҵ忡�� ���ϰ��� �ֱ� ���ؼ� ������.

protected:
	DxClubMan();

public:
	virtual ~DxClubMan();

public:
	static void SetPath( char* szPath ) { StringCchCopy( g_szPath, MAX_PATH, szPath ); }
	static char* GetPath() { return g_szPath; }

	void SetRegistPath ( std::string strPath ) { m_strREGISTPATH = strPath; }

public:
	//	Note : �űԷ� ����� Ŭ����ũ�� �̹����� ���Ͽ��� �о����.
	BOOL LoadClubMark ( const char* szFileName, LPDWORD& pMarkColor );

	//	Note : Ŭ�� ��ũ�� �ؽ��� ������ ��ȿ���� �˻�.
	bool IsValidData ( int nServer, int nID, DWORD dwVer );

	//	Note : Ŭ�� ��ũ�� �ؽ�ó ������ ����.
	bool SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer, DWORD *pClubMark );

	//	Note : Ŭ�� ��ũ�� �ؽ��� ������ ��������.
	const clubmark::DXDATA& GetClubData( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID );

protected:
	void NewServerData ( int nServer );

	void SaveData() const;
	void SaveData_Main() const;
	void LoadData( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	BOOL LoadBMPFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor );
	BOOL LoadTGAFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(_DXCLUBMAN_H__INCLUDED_)