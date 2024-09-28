#pragma once

#include <map>
#include <string>

struct DxMeshes;
class DxTexEffBase;
class CMaterialSetting;

namespace sc
{
	class SerialFile;
};

#define TEXEFF_DEFAULT		0x1000
#define TEXEFF_OPAQUE		0x2000
#define TEXEFF_HARDALPHA	0x3000
#define TEXEFF_SOFTALPHA	0x4000
#define TEXEFF_SOFTALPHA_1	0x5000
#define TEXEFF_SOFTALPHA_2	0x6000
#define TEXEFF_DXT			0xf000

struct TEXEFF_PROPERTY
{
	DWORD dwFlag;

	TEXEFF_PROPERTY() :
		dwFlag(0L)
	{
	};
};

enum PIECE_LIGHTMAP_OPTION
{
	EMPLO_DONT_LM_SHADOW_RECEIVE	= 0x01,
	EMPLO_DONT_LM_SHADOW_CAST		= 0x02,
};

struct TextureData
{
    DWORD   m_dwTexType;			// DXT 
	DWORD	m_dwPieceLM_Option;		// PIECE_LIGHTMAP_OPTION

    TextureData( DWORD dwTexType, DWORD dwFlags )  // 0, 0
        : m_dwTexType(dwTexType)
        , m_dwPieceLM_Option(dwFlags)
    {
    }
};

class DxTextureEffMan
{
private:
    enum
    {
        VERSION = 0x102,
    };

	// Note : Texture List
public:
	typedef	std::map<std::string,TextureData>	MAPTEXLIST;
	typedef	MAPTEXLIST::iterator				MAPTEXLIST_ITER;
    typedef	MAPTEXLIST::const_iterator	    	MAPTEXLIST_CITER;
	typedef	std::map<std::string,DxTexEffBase*>	MAPTEXEFF;
	typedef	MAPTEXEFF::iterator					MAPTEXEFF_ITER;
    typedef	MAPTEXEFF::const_iterator			MAPTEXEFF_CITER;

protected:
	MAPTEXLIST	m_mapTexList;
	MAPTEXEFF	m_mapTexEff;
    LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	MAPTEXEFF*	GetMapTexEff()		{ return &m_mapTexEff; }

public:
	void InsertTexList( const char* szName );
	void InsertTextureEff( MAPTEXEFF& mapData, const char* szName, DxTexEffBase* pBase );	// Note : 기본 텍스쳐 추가.

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const D3DXMATRIX& matWorld, BOOL bRealPL, const CMaterialSetting* pMaterialSetting );
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const BOOL bShadow, const D3DXMATRIX& matWorld, BOOL bRealPL, const CMaterialSetting* pMaterialSetting );
	void RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const D3DXMATRIX& matWorld, const CMaterialSetting* pMaterialSetting, int nRenderOrder );

	void RenderWaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxMeshes* pMeshs, 
										const D3DXMATRIX& matWorld, 
										const CMaterialSetting* pMaterialSetting );

	// Deffered
public:
	void Render_Deffered_GPass( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxMeshes* pMeshs, 
								const D3DXMATRIX& matWorld, 
								const CMaterialSetting* pMaterialSetting, 
								DWORD dwPass, 
								BOOL bAlpha,
								BOOL bForwardPass );	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들..

	void Render_Deffered_GPass_Decal( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxMeshes* pMeshs, 
										const D3DXMATRIX& matWorld, 
										const CMaterialSetting* pMaterialSetting, 
										DWORD dwOrder,
										BOOL bForwardPass );	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들..

	void Render_Forward_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const D3DXMATRIX& matWorld, const CMaterialSetting* pMaterialSetting );

protected:
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh, const DWORD dwAttrib, const int nMETR, const BOOL bShadow );

protected:
	void MakeEffectMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs );

	// Note : Tool 에서 사용
public:
	int GetTextureListNUM() const		{ return (int)m_mapTexList.size(); } 
	BOOL IsGetTextureEFFDXT( DWORD i, LPCSTR& szName, DWORD& dwTexType, DWORD& dwFlagEff, TSTRING& strVM ) const;
	void DeleteTexEff( const char* pName, DxTexEffBase* pBase );
	DxTexEffBase* GetEffRoot( const char* szName );
    void ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice );
    BOOL SetTextureData( const TSTRING& strName, const TextureData& sTextureData );
    const TextureData* GetTextureData( const TSTRING& strName ) const;
	DWORD GetTexType( const TSTRING& strName ) const;
	void SetDXTFlag( const TSTRING& strName, DWORD dwFlag );
    BOOL IsVM2Side( const TSTRING& strName ) const;         // VM 을 사용하면서 2Side Rendering을 사용했는가~?

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

public:
	void SavePSF( sc::SerialFile& SFile );
    void LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bPiece );
	void LoadPSF_VER100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
    void LoadPSF_PREV_PIECE( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void CloneMapTexEff( LPDIRECT3DDEVICEQ pd3dDevice, MAPTEXEFF& mapSrc, MAPTEXEFF& mapDest );
    void Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxTextureEffMan* pSrc );

public:
	DxTextureEffMan();
	~DxTextureEffMan();
};
