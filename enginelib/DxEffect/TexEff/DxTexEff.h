#pragma once

#include "../../DxTools/DxVertexFVF.h"

namespace sc
{
    class SerialFile;
} // namespace sc

class DxTangentMesh;

#define DEF_TEXEFF_DIFFUSE		    0x0001
#define DEF_TEXEFF_FLOWUV		    0x0002
#define DEF_TEXEFF_ROTATE		    0x0004
#define DEF_TEXEFF_SPECULAR		    0x0008
#define DEF_TEXEFF_VISUAL_MATERIAL	0x0010

#define DEF_TEXEFF_ALL			    0x00ff

struct DxTexEffType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxTexEffType *pNextType;

	DxTexEffType() :
		TYPEID(0xffffffff),
		pNextType(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxTexEffType ()
	{
		SAFE_DELETE(pNextType);
	}
};

class DxTexEffBase
{
    // static
public:
    static DxTexEffBase* NEW_TEXEFF_STATIC( DWORD dwType );

public:
	virtual DWORD GetTypeID()				{ return 0xffffffff; }
	virtual const char* GetName()			{ return "[0]_정의되지않은_[UnDefined]"; }

    virtual void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{}	
	virtual void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{}	

	virtual void FrameMove( const float fElapsedTime )	{}	

    virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib ) {}
    virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, 
                        LPD3DXATTRIBUTERANGE pAttrib, const DWORD dwAttrib, VERTEX* pVertSrc, DWORD dwVert ) {}

	// Note : Octree 메쉬에서의 개개의 VB를 사용하지 않고 공용 VB 사용하려고 이렇게 만들어 놓았다.
	virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces, VERTEXNORCOLORTEX* pVertSrc )	{}

    virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                            DxTangentMesh* pTangentMesh, 
                            const D3DXMATRIX& matWorld, 
                            LPDIRECT3DTEXTUREQ pTextureDay, 
                            LPDIRECT3DTEXTUREQ pTextureNight,
                            LPDIRECT3DTEXTUREQ pTextureAvgDay,
                            LPDIRECT3DTEXTUREQ pTextureAvgNight,
                            const D3DXVECTOR2& vLightMapUV_Offset,
                            float fAlpha )        {}

    virtual void RenderColorLM( LPDIRECT3DDEVICEQ pd3dDevice, 
                                DxTangentMesh* pTangentMesh, 
                                const D3DXMATRIX& matWorld, 
                                LPDIRECT3DTEXTUREQ pTextureDay, 
                                LPDIRECT3DTEXTUREQ pTextureNight,
                                LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                const D3DXVECTOR2& vLightMapUV_Offset,
                                float fAlpha )        {}

    virtual void RenderColor( LPDIRECT3DDEVICEQ pd3dDevice, DxTangentMesh* pTangentMesh, const DWORD dwAttrib, BOOL bRealPL, const D3DXMATRIX& matWorld, float fAlpha )  {}
    virtual void ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )   {}
    virtual BOOL IsVM2Side() const                      { return FALSE; }   // VM 을 사용하면서 2Side Rendering을 사용했는가~?
    virtual void GetFileNameVM( TSTRING& strVM ) const  {}                  // VM 을 사용한다면 사용하는 VM 파일 이름은~?

	virtual void GetProperty( PBYTE &pProp ) { pProp=NULL; }
	virtual void SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp ) {}

	virtual void SavePSF( sc::SerialFile& SFile ) {}
	virtual BOOL LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile ) { return FALSE; }

public:
	DxTexEffBase* m_pNext;

public:
	DxTexEffBase();
	virtual ~DxTexEffBase();
};