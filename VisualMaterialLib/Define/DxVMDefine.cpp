#include "stdafx.h"

#include "DxVMDefine.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



namespace NSVISUAL_MATERIAL
{
    float       TITLE_CORE_WIDTH        = 10.f;
    float       TITLE_CORE_HEIGHT       = 1.4f;
    float       TITLE_COMMAND_WIDTH     = 5.f;
    float       TITLE_COMMAND_HEIGHT    = 1.4f;
    float       TITLE_HEIGHT_SMALL      = 0.1f;
    float       MOUSE_MOVE_VEL          = 0.3f;
    TSTRING     g_strFont               = _T("Arial");

    // Note : Add Command
    TSTRING     g_strVisualMaterialCommand[VMC_SIZE] =
    {
        _T("Core"),             // VMC_CORE
        _T("TextureSample"),    // VMC_TEXTURESAMPLE
        _T("Multiply"),         // VMC_MULTIPLY
        _T("Sine"),             // VMC_SINE
        _T("Constant"),         // VMC_CONSTANT
        _T("Time"),             // VMC_TIME
        _T("Constant2Vector"),  // VMC_CONSTANT2VECTOR
        _T("Constant3Vector"),  // VMC_CONSTANT3VECTOR
        _T("Constant4Vector"),  // VMC_CONSTANT4VECTOR
        _T("Add"),              // VMC_ADD
        _T("ReflectionVector"), // VMC_REFLECTION_VECTOR
        _T("Clamp"),            // VMC_CLAMP
        _T("Subtract"),         // VMC_SUBTRACT
        _T("LinearInterpolate"),// VMC_LINEAR_INTERPOLATE
        _T("Fresnel"),          // VMC_FRESNEL
        _T("Panner"),           // VMC_PANNER
        _T("Rotator"),          // VMC_ROTATOR
        _T("TextureCoordinate"),// VMC_TEXTURE_COORDINATE
        _T("VectorParameter"),  // VMC_VECTOR_PARAMETER
		_T("TextureRefract"),	// VMC_TEXTURE_REFRACT
    };

    TSTRING     g_strBlendMode[VM_BLENDMODE_SIZE] = { _T("Opaque"), _T("Masked"), _T("Translucent"), _T("Additive"), _T("Modulate") };
    TSTRING     g_strTwoSideMode[VM_TWOSIDEDMODE_SIZE] = { _T("Base"), _T("Two Sided"), _T("Two Sided Seperate Pass") };

    // Edit
    int          g_nSelectCommandVM = -1;
};


VM_CAN_SELECT_INDEX::VM_CAN_SELECT_INDEX()
    : m_emMode(CAN_SELECT_ZERO)
    , m_dwID(-1)
    , m_nIndexBox(-1)
{
}

namespace vm
{
    HRESULT CreateTagentMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut )
    {
	    HRESULT hr = S_OK;
	    ID3DXMesh* pTempMesh = NULL;
	    ID3DXMesh* pNewMesh = NULL;

	    // Start clean
	    (*ppMeshOut) = NULL;

	    // Create a new vertex declaration to hold all the required data
	    D3DVERTEXELEMENT9 VertexDecl[] =
	    {
		    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
            { 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
		    { 0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            { 0, 44, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
            
		    D3DDECL_END()
	    };
	    // Clone mesh to the new vertex format
	    hr = pMeshIn->CloneMesh( pMeshIn->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );	
        
	    // Compute tangents, which are required for normal mapping
	    hr = D3DXComputeTangentFrameEx( pTempMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
									    D3DX_DEFAULT, 0, D3DDECLUSAGE_NORMAL, 0,
									    0, NULL, -1, 0, -1, &pNewMesh, NULL );
	    if( FAILED(hr) )
		    goto e_Exit;

		// D3DXComputeTangentFrameEx 가 되면, 내가 SetAttributeTable 으로 변경된 것이 무효화 되고 이전 원본 데이터가 씌워진다.
		// 그걸 복원하도록 한다. ( mc_ks_weapon_01_sawtooth.cps )
		{
			DWORD dwAttrib(0);
			pTempMesh->GetAttributeTable( NULL, &dwAttrib );
			D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[ dwAttrib ];
			pTempMesh->GetAttributeTable( pAttrib, &dwAttrib );

			pNewMesh->SetAttributeTable( pAttrib, dwAttrib );
			SAFE_DELETE_ARRAY( pAttrib );
		}

	    // Copy the mesh and return
	    (*ppMeshOut) = pNewMesh;
	    pNewMesh = NULL;
	    hr = S_OK;

    e_Exit:
	    SAFE_RELEASE( pNewMesh );
	    SAFE_RELEASE( pTempMesh );
	    return S_OK;
    }

    HRESULT CreateTagentColorMeshLM( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut )
    {
	    HRESULT hr = S_OK;
	    ID3DXMesh* pTempMesh = NULL;
	    ID3DXMesh* pNewMesh = NULL;

	    // Start clean
	    (*ppMeshOut) = NULL;

	    // Create a new vertex declaration to hold all the required data
	    D3DVERTEXELEMENT9 VertexDecl[] =
	    {
		    { 0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		    { 0, 12, D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
            { 0, 24, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
            { 0, 28, D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
		    { 0, 40, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            { 0, 48, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
            
		    D3DDECL_END()
	    };
	    // Clone mesh to the new vertex format
	    hr = pMeshIn->CloneMesh( pMeshIn->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );	
        
	    // Compute tangents, which are required for normal mapping
	    hr = D3DXComputeTangentFrameEx( pTempMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
									    D3DX_DEFAULT, 0, D3DDECLUSAGE_NORMAL, 0,
									    0, NULL, -1, 0, -1, &pNewMesh, NULL );
	    if( FAILED(hr) )
		    goto e_Exit;

		// D3DXComputeTangentFrameEx 가 되면, 내가 SetAttributeTable 으로 변경된 것이 무효화 되고 이전 원본 데이터가 씌워진다.
		// 그걸 복원하도록 한다. ( mc_ks_weapon_01_sawtooth.cps )
		{
			DWORD dwAttrib(0);
			pTempMesh->GetAttributeTable( NULL, &dwAttrib );
			D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[ dwAttrib ];
			pTempMesh->GetAttributeTable( pAttrib, &dwAttrib );

			pNewMesh->SetAttributeTable( pAttrib, dwAttrib );
			SAFE_DELETE_ARRAY( pAttrib );
		}

	    // Copy the mesh and return
	    (*ppMeshOut) = pNewMesh;
	    pNewMesh = NULL;
	    hr = S_OK;

    e_Exit:
	    SAFE_RELEASE( pNewMesh );
	    SAFE_RELEASE( pTempMesh );
	    return S_OK;
    }

    HRESULT CreateTagentSkinMeshObject( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut )
    {
	    HRESULT hr = S_OK;
	    ID3DXMesh* pTempMesh = NULL;
	    ID3DXMesh* pNewMesh = NULL;

	    // Start clean
	    (*ppMeshOut) = NULL;

	    // Create a new vertex declaration to hold all the required data
	    D3DVERTEXELEMENT9 VertexDecl[] =
	    {
		    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		    { 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
            
		    D3DDECL_END()
	    };
	    // Clone mesh to the new vertex format
	    hr = pMeshIn->CloneMesh( pMeshIn->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );	
        
	    // Compute tangents, which are required for normal mapping
	    hr = D3DXComputeTangentFrameEx( pTempMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
									    D3DX_DEFAULT, 0, D3DDECLUSAGE_NORMAL, 0,
									    0, NULL, -1, 0, -1, &pNewMesh, NULL );
	    if( FAILED(hr) )
		    goto e_Exit;

		// D3DXComputeTangentFrameEx 가 되면, 내가 SetAttributeTable 으로 변경된 것이 무효화 되고 이전 원본 데이터가 씌워진다.
		// 그걸 복원하도록 한다. ( mc_ks_weapon_01_sawtooth.cps )
		{
			DWORD dwAttrib(0);
			pTempMesh->GetAttributeTable( NULL, &dwAttrib );
			D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[ dwAttrib ];
			pTempMesh->GetAttributeTable( pAttrib, &dwAttrib );

			pNewMesh->SetAttributeTable( pAttrib, dwAttrib );
			SAFE_DELETE_ARRAY( pAttrib );
		}

	    // Copy the mesh and return
	    (*ppMeshOut) = pNewMesh;
	    pNewMesh = NULL;
	    hr = S_OK;

    e_Exit:
	    SAFE_RELEASE( pNewMesh );
	    SAFE_RELEASE( pTempMesh );
	    return S_OK;
    }


    HRESULT CreateTagentColorMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut )
    {
	    HRESULT hr = S_OK;
	    ID3DXMesh* pTempMesh = NULL;
	    ID3DXMesh* pNewMesh = NULL;

	    // Start clean
	    (*ppMeshOut) = NULL;

	    // Create a new vertex declaration to hold all the required data
	    D3DVERTEXELEMENT9 VertexDecl[] =
	    {
		    { 0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		    { 0, 12, D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
            { 0, 24, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
		    { 0, 28, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		    { 0, 36, D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
            
		    D3DDECL_END()
	    };
	    // Clone mesh to the new vertex format
	    hr = pMeshIn->CloneMesh( pMeshIn->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );	
        
	    // Compute tangents, which are required for normal mapping
	    hr = D3DXComputeTangentFrameEx( pTempMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
									    D3DX_DEFAULT, 0, D3DDECLUSAGE_NORMAL, 0,
									    0, NULL, -1, 0, -1, &pNewMesh, NULL );
	    if( FAILED(hr) )
		    goto e_Exit;

		// D3DXComputeTangentFrameEx 가 되면, 내가 SetAttributeTable 으로 변경된 것이 무효화 되고 이전 원본 데이터가 씌워진다.
		// 그걸 복원하도록 한다. ( mc_ks_weapon_01_sawtooth.cps )
		{
			DWORD dwAttrib(0);
			pTempMesh->GetAttributeTable( NULL, &dwAttrib );
			D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[ dwAttrib ];
			pTempMesh->GetAttributeTable( pAttrib, &dwAttrib );

			pNewMesh->SetAttributeTable( pAttrib, dwAttrib );
			SAFE_DELETE_ARRAY( pAttrib );
		}

	    // Copy the mesh and return
	    (*ppMeshOut) = pNewMesh;
	    pNewMesh = NULL;
	    hr = S_OK;

    e_Exit:
	    SAFE_RELEASE( pNewMesh );
	    SAFE_RELEASE( pTempMesh );
	    return S_OK;
    }

    HRESULT CreateTagentSkinMeshVS( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut )
    {
        HRESULT hr = S_OK;
	    ID3DXMesh* pTempMesh = NULL;
	    ID3DXMesh* pNewMesh = NULL;

	    // Start clean
	    (*ppMeshOut) = NULL;

        // [기억하자]
        // D3DFVF_LASTBETA_UBYTE4 가 있다면 Blend의 마지막 위치에 Index를 넣겠다는 것이다.
        // D3DFVF_XYZB1 -> byte vIndices[4];
        // D3DFVF_XYZB2 -> float vWeight[1], byte vIndices[4]; -> 가중치가 2개 같지만 한개만 사용
        // D3DFVF_XYZB3 -> float vWeight[2], byte vIndices[4]; -> 가중치가 3개 같지만 2개만 사용
        // D3DFVF_XYZB4 -> float vWeight[3], byte vIndices[4]; -> 가중치가 4개 같지만 3개만 사용
        // D3DFVF_XYZB5 -> float vWeight[4], byte vIndices[4]; -> 가중치가 5개 같지만 4개만 사용
        //
        //{
        //    DWORD dwPosMASK = pMeshIn->GetFVF() & D3DFVF_POSITION_MASK;
        //    if ( dwPosMASK == D3DFVF_XYZB1 )
        //    {
        //        struct SDATA
        //        {
        //            D3DXVECTOR3 vPos;
        //            byte vIndices[4];
        //            D3DXVECTOR3 vNor;
        //            D3DXVECTOR2 vTex;
        //        };

		      //  SDATA* sVertices(NULL);
		      //  pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		      //  for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		      //  {
        //            int a=0;
		      //  }
        //        pMeshIn->UnlockVertexBuffer();
        //    }
        //    else if ( dwPosMASK == D3DFVF_XYZB2 )
        //    {
        //        struct SDATA
        //        {
        //            D3DXVECTOR3 vPos;
        //            float vWeight;
        //            byte vIndices[4];
        //            D3DXVECTOR3 vNor;
        //            D3DXVECTOR2 vTex;
        //        };

		      //  SDATA* sVertices(NULL);
		      //  pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		      //  for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		      //  {
        //            int a=0;
		      //  }
        //        pMeshIn->UnlockVertexBuffer();
        //    }
        //    else if ( dwPosMASK == D3DFVF_XYZB3 )
        //    {
        //        struct SDATA
        //        {
        //            D3DXVECTOR3 vPos;
        //            float vWeight[2];
        //            byte vIndices[4];
        //            D3DXVECTOR3 vNor;
        //            D3DXVECTOR2 vTex;
        //        };

		      //  SDATA* sVertices(NULL);
		      //  pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		      //  for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		      //  {
        //            int a=0;
		      //  }
        //        pMeshIn->UnlockVertexBuffer();
        //    }
        //    else if ( dwPosMASK == D3DFVF_XYZB4 )
        //    {
        //        struct SDATA
        //        {
        //            D3DXVECTOR3 vPos;
        //            float vWeight[3];
        //            byte vIndices[4];
        //            D3DXVECTOR3 vNor;
        //            D3DXVECTOR2 vTex;
        //        };

		      //  SDATA* sVertices(NULL);
		      //  pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		      //  for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		      //  {
        //            int a=0;
		      //  }
        //        pMeshIn->UnlockVertexBuffer();
        //    }
        //    else if ( dwPosMASK == D3DFVF_XYZB5 )
        //    {
        //        struct SDATA
        //        {
        //            D3DXVECTOR3 vPos;
        //            float vWeight[4];
        //            byte vIndices[4];
        //            D3DXVECTOR3 vNor;
        //            D3DXVECTOR2 vTex;
        //        };

		      //  SDATA* sVertices(NULL);
		      //  pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		      //  for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		      //  {
        //            int a=0;
		      //  }
        //        pMeshIn->UnlockVertexBuffer();
        //    }
        //}

        // Create a new vertex declaration to hold all the required data
        D3DVERTEXELEMENT9 VertexDecl[] =
        {
            { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
            { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
            { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            { 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
            { 0, 44, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
            { 0, 60, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES,  0 },
            
            D3DDECL_END()
        };

        // Clone mesh to the new vertex format
        SAFE_RELEASE( pTempMesh );
        hr = pMeshIn->CloneMesh( pMeshIn->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );
        
        // 도대체 왜 BlendIndices가 적용이 안되는지 모르겠다.
        // 그래서 이렇게 넣고 있다.
		// 
		// [2013.06.03][shhan] 혹시 D3DDECLUSAGE_BLENDINDICES 가 D3DDECLTYPE_UBYTE4 를 안 써서 그런것일 수도 있겠다.
		//						나중에 Test 해보자.
        {
            VERTEXNORTEXTANGENT_VS* pVertices(NULL);
            pTempMesh->LockVertexBuffer( 0, (VOID**)&pVertices );
            {
                DWORD dwPosMASK = pMeshIn->GetFVF() & D3DFVF_POSITION_MASK;
                if ( dwPosMASK == D3DFVF_XYZB1 )
                {
                    struct SDATA
                    {
                        D3DXVECTOR3 vPos;
                        byte vIndices[4];
                        D3DXVECTOR3 vNor;
                        D3DXVECTOR2 vTex;
                    };

		            SDATA* sVertices(NULL);
		            pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		            for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		            {
                        pVertices[i].vIndies.x = sVertices[i].vIndices[0];
                        pVertices[i].vIndies.y = sVertices[i].vIndices[1];
                        pVertices[i].vIndies.z = sVertices[i].vIndices[2];
                        pVertices[i].vIndies.w = sVertices[i].vIndices[3];
		            }
                    pMeshIn->UnlockVertexBuffer();
                }
                else if ( dwPosMASK == D3DFVF_XYZB2 )
                {
                    struct SDATA
                    {
                        D3DXVECTOR3 vPos;
                        float vWeight;
                        byte vIndices[4];
                        D3DXVECTOR3 vNor;
                        D3DXVECTOR2 vTex;
                    };

		            SDATA* sVertices(NULL);
		            pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		            for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		            {
                        pVertices[i].vIndies.x = sVertices[i].vIndices[0];
                        pVertices[i].vIndies.y = sVertices[i].vIndices[1];
                        pVertices[i].vIndies.z = sVertices[i].vIndices[2];
                        pVertices[i].vIndies.w = sVertices[i].vIndices[3];
		            }
                    pMeshIn->UnlockVertexBuffer();
                }
                else if ( dwPosMASK == D3DFVF_XYZB3 )
                {
                    struct SDATA
                    {
                        D3DXVECTOR3 vPos;
                        float vWeight[2];
                        byte vIndices[4];
                        D3DXVECTOR3 vNor;
                        D3DXVECTOR2 vTex;
                    };

		            SDATA* sVertices(NULL);
		            pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		            for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		            {
                        pVertices[i].vIndies.x = sVertices[i].vIndices[0];
                        pVertices[i].vIndies.y = sVertices[i].vIndices[1];
                        pVertices[i].vIndies.z = sVertices[i].vIndices[2];
                        pVertices[i].vIndies.w = sVertices[i].vIndices[3];
		            }
                    pMeshIn->UnlockVertexBuffer();
                }
                else if ( dwPosMASK == D3DFVF_XYZB4 )
                {
                    struct SDATA
                    {
                        D3DXVECTOR3 vPos;
                        float vWeight[3];
                        byte vIndices[4];
                        D3DXVECTOR3 vNor;
                        D3DXVECTOR2 vTex;
                    };

		            SDATA* sVertices(NULL);
		            pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		            for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		            {
                        pVertices[i].vIndies.x = sVertices[i].vIndices[0];
                        pVertices[i].vIndies.y = sVertices[i].vIndices[1];
                        pVertices[i].vIndies.z = sVertices[i].vIndices[2];
                        pVertices[i].vIndies.w = sVertices[i].vIndices[3];
		            }
                    pMeshIn->UnlockVertexBuffer();
                }
                else if ( dwPosMASK == D3DFVF_XYZB5 )
                {
                    struct SDATA
                    {
                        D3DXVECTOR3 vPos;
                        float vWeight[4];
                        byte vIndices[4];
                        D3DXVECTOR3 vNor;
                        D3DXVECTOR2 vTex;
                    };

		            SDATA* sVertices(NULL);
		            pMeshIn->LockVertexBuffer( 0, (VOID**)&sVertices );
		            for( DWORD i=0; i<pMeshIn->GetNumVertices(); ++i )
		            {
                        pVertices[i].vIndies.x = sVertices[i].vIndices[0];
                        pVertices[i].vIndies.y = sVertices[i].vIndices[1];
                        pVertices[i].vIndies.z = sVertices[i].vIndices[2];
                        pVertices[i].vIndies.w = sVertices[i].vIndices[3];
		            }
                    pMeshIn->UnlockVertexBuffer();
                }
            }
            pTempMesh->UnlockVertexBuffer();
        }

	    // Compute tangents, which are required for normal mapping
	    hr = D3DXComputeTangentFrameEx( pTempMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
									    D3DX_DEFAULT, 0, D3DDECLUSAGE_NORMAL, 0,
									    0, NULL, -1, 0, -1, &pNewMesh, NULL );
	    if( FAILED(hr) )
		    goto e_Exit;

		// D3DXComputeTangentFrameEx 가 되면, 내가 SetAttributeTable 으로 변경된 것이 무효화 되고 이전 원본 데이터가 씌워진다.
		// 그걸 복원하도록 한다. ( mc_ks_weapon_01_sawtooth.cps ) ( mc_ks_weapon_01_sawtooth.cps )
		{
			DWORD dwAttrib(0);
			pTempMesh->GetAttributeTable( NULL, &dwAttrib );
			D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[ dwAttrib ];
			pTempMesh->GetAttributeTable( pAttrib, &dwAttrib );

			pNewMesh->SetAttributeTable( pAttrib, dwAttrib );
			SAFE_DELETE_ARRAY( pAttrib );
		}

	    // Copy the mesh and return
	    (*ppMeshOut) = pNewMesh;
	    pNewMesh = NULL;
	    hr = S_OK;

    e_Exit:
	    SAFE_RELEASE( pNewMesh );
	    SAFE_RELEASE( pTempMesh );
	    return S_OK;
    }
};