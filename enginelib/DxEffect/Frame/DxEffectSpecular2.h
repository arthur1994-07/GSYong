//  interface for the DxEffectSpecular2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFCTSPECULAR2_INCLUDED__)
#define __DXEFFCTSPECULAR2_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"

struct DxMeshes;

struct DXMATERIAL_SPEC2_OLD
{
	BOOL    bSpecUse;
	VOID*   pTemp;
	char    szTexture[MAX_PATH];
	char    szSpecTex[MAX_PATH];
};

struct DXMATERIAL_SPEC2
{
	BOOL				bSpecUse;
	TextureResource m_textureRes;
	char				szTexture[MAX_PATH];
	char				szSpecTex[MAX_PATH];

	DXMATERIAL_SPEC2 () :
		bSpecUse(FALSE)
	{
		memset( szTexture, 0, sizeof(char)*MAX_PATH );
		memset( szSpecTex, 0, sizeof(char)*MAX_PATH );
	}

    DXMATERIAL_SPEC2& operator= ( const DXMATERIAL_SPEC2& rvalue )
    {
        bSpecUse = rvalue.bSpecUse;
        m_textureRes = rvalue.m_textureRes;
        memcpy( szTexture, rvalue.szTexture, sizeof( char ) * MAX_PATH );
        memcpy( szSpecTex, rvalue.szSpecTex, sizeof( char ) * MAX_PATH );

        return *this;
    }

    void SaveFile( sc::SerialFile &SFile );
    void LoadFile( sc::SerialFile &SFile );
};

struct SPECULAR2_PROPERTY
{
	DWORD		m_dwColorOP;
	DWORD		m_dwMaterials;
	DWORD		m_dwTempMaterials;
};

class DxEffectSpecular2 : public DxEffectBase
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
	static	LPDIRECT3DSTATEBLOCK9		m_pSavedStateBlock;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawStateBlock;

public:
	void SetProperty ( SPECULAR2_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SPECULAR2_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			SPECULAR2_PROPERTY		m_Property;
		};

		struct
		{
			DWORD		m_dwColorOP;
			DWORD		m_dwMaterials;
			DWORD		m_dwTempMaterials;
		};
	};

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	LPDIRECT3DTEXTUREQ		m_pddsTexture;

protected:
	DXMATERIAL_SPEC2*		m_pTempMaterials;
	DXMATERIAL_SPEC2*		m_pMaterials;			// 매트리얼 정보

public:
	DXMATERIAL_SPEC2*		GetMaterials()	{ return m_pMaterials; }
	void					ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();
	
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	DxEffectSpecular2 ();
	virtual ~DxEffectSpecular2 ();

protected:
	virtual void SaveBuffer ( sc::SerialFile &SFile );
	virtual void LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff );
};

#endif // !defined(__DXEFFCTSPECULAR2_INCLUDED__)
