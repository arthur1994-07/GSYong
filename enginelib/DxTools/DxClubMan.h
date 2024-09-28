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

		UNUSED_MAX_COUNT = 100,		// 일정 횟수이상 사용되지 않는다면 삭제하기 위해 존재함.
	};

protected:
	class ID_DATA
	{
	private:
		int					m_nUnUsedCount;		// 일정 횟수이상 사용되지 않는다면 삭제하기 위해 존재함. <UNUSED_MAX_COUNT>
		DWORD				m_dwVer;
		LPDIRECT3DTEXTUREQ	m_pTexture;			// 컴퓨터 사정상 Texture 가 생성 안될 수 도 있다.

	public:
		bool				IsValidData( DWORD dwVer ) const;
		LPDIRECT3DTEXTUREQ	GetTexture() const						{ return m_dwVer == 0 ? NULL : m_pTexture; }	// Version 이 0 이라면 아직 Texture 에 Data 가 기록 안된 상태이다.

		// dwVer - bTextureIN TRUE 일 경우만 m_dwVer 에 값을 셋팅한다.
		BOOL DrawClubTex ( DWORD* pColor, BOOL bTextureIN, DWORD dwVer );    // Tex에 정해진 모양을 그린다.

		void Save ( sc::SerialFile &SFile );
		BOOL Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile );

	private:
		BOOL CreateClubTex( LPDIRECT3DDEVICEQ pd3dDevice );

	public:
		ID_DATA( LPDIRECT3DDEVICEQ pd3dDevice );
		~ID_DATA();
	};

	typedef std::map<DWORD, ID_DATA*>   ID_DATA_MAP;		// 클럽아이디, 클럽 정보
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

	typedef std::map<DWORD, ID_SERVER*>	  ID_SERVER_MAP;		// 서버번호, 클럽 데이터
	typedef ID_SERVER_MAP::iterator		  ID_SERVER_MAP_ITER;
	typedef ID_SERVER_MAP::const_iterator ID_SERVER_MAP_CITER;
	typedef ID_SERVER_MAP::value_type	  ID_SERVER_MAP_VALUE;

protected:
	static char g_szPath[MAX_PATH];
	std::string m_strREGISTPATH;

	DWORD m_sClubMark[WIDTH*HEIGHT];	    // 새로운 클럽 마크 배열

	const static std::string m_strText;		// 문서
	//std::string m_strBaseName;	            // 이미지 기본
	//std::string m_strExtName;	            // 이미지 확장자

	DWORD COL;
	DWORD ROW;
	D3DXVECTOR2	UV_DIS;

	ID_SERVER_MAP m_mapServer;

	//DWORD				m_sDefaultClubMark[WIDTH*HEIGHT];	// 디폴트 클럽 마크 배열
	TextureResource		m_sTextureDefaultClubMark;

public:
	clubmark::DXDATA m_sTextureData;	// GetClubData() 메소드에서 리턴값을 주기 위해서 쓰였다.

protected:
	DxClubMan();

public:
	virtual ~DxClubMan();

public:
	static void SetPath( char* szPath ) { StringCchCopy( g_szPath, MAX_PATH, szPath ); }
	static char* GetPath() { return g_szPath; }

	void SetRegistPath ( std::string strPath ) { m_strREGISTPATH = strPath; }

public:
	//	Note : 신규로 등록할 클럽마크의 이미지를 파일에서 읽어오기.
	BOOL LoadClubMark ( const char* szFileName, LPDWORD& pMarkColor );

	//	Note : 클럽 마크의 텍스쳐 정보의 유효성을 검사.
	bool IsValidData ( int nServer, int nID, DWORD dwVer );

	//	Note : 클럽 마크의 텍스처 정보를 설정.
	bool SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer, DWORD *pClubMark );

	//	Note : 클럽 마크의 텍스쳐 정보를 가져오기.
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