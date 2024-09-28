// DxEffectGlow.h: interface for the DxEffectGlow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTGLOW_H__INCLUDED_)
#define AFX_DXEFFECTGLOW_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../DxTools/TextureManager.h"
#include "./DxEffectFrame.h"

struct DxFrame;

struct DXUSERMATERIAL_OLD
{
public:
	BOOL				bGlowUse;
	BOOL				bColorUse;
	D3DCOLOR			cColor;
    VOID*               pTemp;
	char				szTexture[MAX_PATH];
	char				szGlowTex[MAX_PATH];
};

struct DXUSERMATERIAL
{
public:
	BOOL				bGlowUse;
	BOOL				bColorUse;
	D3DCOLOR			cColor;
	char				szTexture[MAX_PATH];
	char				szGlowTex[MAX_PATH];
    TextureResource		textureRes;

private:
    LPDIRECT3DTEXTUREQ	pTexture;

public:
	DXUSERMATERIAL () :
		bGlowUse(FALSE),
		bColorUse(FALSE),
		cColor(0xffffffff),
		pTexture(NULL)
	{
		memset( szTexture, 0, sizeof(char)*MAX_PATH );
		memset( szGlowTex, 0, sizeof(char)*MAX_PATH );
	}

public:
    void SaveFile( sc::SerialFile &SFile );
    void LoadFile( sc::SerialFile &SFile );

    void OnDestroyDevice() { textureRes.Clear(); }

    bool LoadTexture( DWORD mipMap, bool bBackgroundLoading );
    void UpdateTexture() { textureRes.Update(); }

    void SetTexture( LPDIRECT3DTEXTUREQ pTexture_ ) { pTexture = pTexture_; }
    LPDIRECT3DTEXTUREQ GetTexture() { return _tcslen( szGlowTex ) ? textureRes.GetTexture() : pTexture; }
};

#define		USEDAY		0x0001
#define		USENIGHT	0x0002

struct GLOW_PROPERTY
{
	DWORD		m_dwFlag;

	DWORD		m_dwMaterials;			// 매트리얼 전체 갯수
	DWORD		m_dwFrameMaterials;
};

class DxEffectGlow : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );
	
protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_SAVE;

	//	Note : 속성.
	//
protected:
	union
	{
		GLOW_PROPERTY	m_Property;

		struct
		{
			DWORD		m_dwFlag;

			DWORD		m_dwMaterials;			// 매트리얼 전체 갯수
			DWORD		m_dwFrameMaterials;
		};
	};

public:
	void SetProperty ( GLOW_PROPERTY& Property )
	{
		m_Property = Property;
	}
	GLOW_PROPERTY& GetProperty () { return m_Property; }

protected:
	float				m_fTime;

	LPDIRECT3DTEXTUREQ			m_pTempTexture;		// Release 해 줄 필요가 없다.

protected:
	DXUSERMATERIAL*		m_pFrameMaterials;
	DXUSERMATERIAL*		m_pMaterials;			// 매트리얼 정보

public:
	DXUSERMATERIAL*		GetMaterials()	{ return m_pMaterials; }
	void				ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	HRESULT DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );

public:
	DxEffectGlow();
	virtual ~DxEffectGlow();

protected:
	virtual void SaveBuffer ( sc::SerialFile &SFile );
	virtual void LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff );
};

#endif // !defined(AFX_DXEFFECTGLOW_H__INCLUDED_)
