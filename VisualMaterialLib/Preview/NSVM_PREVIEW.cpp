#include "stdafx.h"

#include "../../InternalCommonLib/Interface/DxInputDeviceInterface.h"
#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"
#include "../../InternalCommonLib/Interface/XFileRenderInterface.h"
#include "../Command_EX/DxVMCommand_EX.h"
#include "../Util/NSNearPointLight.h"
#include "../HLSL/DxVMCommandFX.h"
#include "../Edit/EditMeshs.h"
#include "../DxVisualMaterial.h"

#include "NSVM_PREVIEW.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



namespace NSVM_PREVIEW
{
    LPDIRECT3DDEVICEQ                       g_pd3dDevice(NULL);
    LPD3DXMESH                              g_pBox(NULL);
    LPD3DXMESH                              g_pPlane(NULL);
    LPD3DXMESH                              g_pSphere(NULL);
    LPD3DXMESH                              g_pCylinder(NULL);
    LPD3DXMESH                              g_pCustomMesh(NULL);
    UINT                                    g_nPreviewRenderStyle(MODEL_PLANE);
    D3DXVECTOR3                             g_vLightPosition;
    LPDIRECT3DVERTEXDECLARATION9	        g_pDCRT(NULL);
    vm::VEC_TEXTURE_RESOURCE                g_vecTextureResource;
    vm::VEC_CUBE_TEXTURE_RESOURCE           g_vecCubeTextureResource;
    vm::VEC_PIXEL_VECTOR                    g_vecPixelScalar;
    vm::VEC_PIXEL_VECTOR                    g_vecPixelVector;
    std::tr1::shared_ptr<DxVMCommandFX>        g_spVMCommandFX(new DxVMCommandFX);
    TSTRING				                    g_strShininessTex = _T("Shininess.dds");	// 광텍도 텍스쳐
    ic::TextureResourceInterface            g_textureResShininess;
    BOOL                                    g_bGridToggle(TRUE);
    std::auto_ptr<ic::XFileRenderInterface> g_spXFileRender;
    float                                   g_fTime(0.f);
    D3DXVECTOR4                             g_vUserColor1(0.5f,0.5f,0.5f,1.f);
    D3DXVECTOR4                             g_vUserColor2(0.5f,0.5f,0.5f,1.f);
    D3DXVECTOR3                             g_vLightDirection( 1.f, -1.f, 1.f );


    UINT g_nSlices = 20;
    UINT g_nStacks = 20;

    float g_fSphereRadius = 10.0f;
    float g_fCylinderRadius1 = 7.0f;
    float g_fCylinderRadius2 = 7.0f;
    float g_fCylinderLength = 20.0f;


    void Reset()
    {
        if ( g_spXFileRender.get() )
        {
            g_spXFileRender.get()->CleanUpVM();
        }
    }

    //--------------------------------------------------------------------------------------------------- 소스를 가져온다.
    const std::string& GetSourceString()
    {
        return g_spVMCommandFX.get()->GetSourceString_Piece();
    }

    const std::string& GetSourceString_Simple()
    {
        //return g_spVMCommandFX.get()->GetSourceString_Simple();
		//return g_spVMCommandFX.get()->GetSourceString_Piece();
		//return g_spVMCommandFX.get()->GetSourceString_PieceColor();
		//return g_spVMCommandFX.get()->GetSourceString_PieceColorLM();
		//return g_spVMCommandFX.get()->GetSourceString_SKIN_VS();
		//return g_spVMCommandFX.get()->GetSourceString_SkinObject();
        return g_spVMCommandFX.get()->GetSourceString_Simple();
    }

    void MakeMesh_VERTEX( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        ID3DXMesh* pTempMesh = NULL;
        (*ppMesh)->CloneMeshFVF( (*ppMesh)->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1, pd3dDevice, &pTempMesh );	
        if ( !pTempMesh )
            return;

        float fTemp(0.f);
		vm::VERTEX* pbVertDestShader;
        pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
		for( DWORD i=0; i<(*ppMesh)->GetNumVertices(); ++i )
		{
            if ( *ppMesh == g_pSphere )
            {
                if ( pbVertDestShader[i].vNor.z >= 0 )
                {
                    pbVertDestShader[i].vTex.x = ( 1.0f - ( asinf( pbVertDestShader[i].vNor.x ) / D3DX_PI + 0.5f ) * 0.5f );
                    pbVertDestShader[i].vTex.y  = ( -asinf( pbVertDestShader[i].vNor.y ) / D3DX_PI + 0.5f );
                }
                else
                {
                    pbVertDestShader[i].vTex.x = ( asinf( pbVertDestShader[i].vNor.x ) / D3DX_PI + 0.5f ) * 0.5f;
                    pbVertDestShader[i].vTex.y = ( -asinf( pbVertDestShader[i].vNor.y ) / D3DX_PI + 0.5f );
                }
            }
            else if ( *ppMesh == g_pCylinder )
            {
                float fHalfCylinderLength = g_fCylinderLength * 0.5f;

                if ( pbVertDestShader[i].vNor.x >= 0 )
                {
                    pbVertDestShader[i].vTex.x = ( 1.0f - ( asinf( pbVertDestShader[i].vNor.y ) / D3DX_PI + 0.5f ) * 0.5f );
                    pbVertDestShader[i].vTex.y = ( pbVertDestShader[i].vPos.z / fHalfCylinderLength * 0.5f + 0.5f );
                }
                else
                {
                    pbVertDestShader[i].vTex.x = ( asinf( pbVertDestShader[i].vNor.y ) / D3DX_PI + 0.5f ) * 0.5f;
                    pbVertDestShader[i].vTex.y = ( pbVertDestShader[i].vPos.z / fHalfCylinderLength * 0.5f + 0.5f );
                }
            }
		}
        pTempMesh->UnlockVertexBuffer();

        SAFE_RELEASE( (*ppMesh) );
        (*ppMesh) = pTempMesh;
    }

    void CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        LPD3DXMESH pMeshTangent;
        HRESULT hr = vm::CreateTagentMesh( pd3dDevice, (*ppMesh), &pMeshTangent );
		if( FAILED(hr) )
		{
			return;
		}
		if( !pMeshTangent )		return;

		LPD3DXMESH pTemp = (*ppMesh);
		(*ppMesh) = pMeshTangent;
		SAFE_RELEASE( pTemp );
    }

    void CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        ID3DXMesh* pTempMesh = NULL;
        (*ppMesh)->CloneMeshFVF( (*ppMesh)->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE1(1), pd3dDevice, &pTempMesh );	
        if ( !pTempMesh )
            return;

        float fTemp(0.f);
		vm::VERTEXNORTANGENTTEX2* pbVertSrc;
		vm::VERTEXNORTANGENTTEX2_COMPRESS* pbVertDestShader;
		(*ppMesh)->LockVertexBuffer( 0, (VOID**)&pbVertSrc );
        pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
		for( DWORD i=0; i<(*ppMesh)->GetNumVertices(); ++i )
		{
			pbVertDestShader[i].vPos = pbVertSrc[i].vPos;

			D3DXVec3Normalize( &pbVertSrc[i].vNor, &pbVertSrc[i].vNor );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vNor[0] = static_cast<short>( pbVertSrc[i].vNor.x * 32767.f );
			pbVertDestShader[i].vNor[1] = static_cast<short>( pbVertSrc[i].vNor.y * 32767.f );
			pbVertDestShader[i].vNor[2] = static_cast<short>( pbVertSrc[i].vNor.z * 32767.f );

			D3DXVec3Normalize( &pbVertSrc[i].vTan, &pbVertSrc[i].vTan );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vTan[0] = static_cast<short>( pbVertSrc[i].vTan.x * 32767.f );
			pbVertDestShader[i].vTan[1] = static_cast<short>( pbVertSrc[i].vTan.y * 32767.f );
			pbVertDestShader[i].vTan[2] = static_cast<short>( pbVertSrc[i].vTan.z * 32767.f );

            pbVertDestShader[i].vTex0 = pbVertSrc[i].vTex[0];
            pbVertDestShader[i].vTex1[0] = static_cast<short>( pbVertSrc[i].vTex[1].x * 32767.f );
            pbVertDestShader[i].vTex1[1] = static_cast<short>( pbVertSrc[i].vTex[2].x * 32767.f );
		}
        pTempMesh->UnlockVertexBuffer();
		(*ppMesh)->UnlockVertexBuffer();

        SAFE_RELEASE( (*ppMesh) );
        (*ppMesh) = pTempMesh;
    }

    void CreatePlane( LPDIRECT3DDEVICEQ pd3dDevice, float fWidth, float fHeight )
    {
        SAFE_RELEASE( g_pPlane );
        D3DXCreateMeshFVF( 2, 4, D3DXMESH_MANAGED, vm::VERTEX::FVF, pd3dDevice, &g_pPlane );


        vm::VERTEX* pVertices;
	    g_pPlane->LockVertexBuffer ( 0L, (VOID**)&pVertices );
        {
            pVertices[0].vPos = D3DXVECTOR3( -fWidth,  fHeight,   0.f );
            pVertices[1].vPos = D3DXVECTOR3( fWidth,   fHeight,   0.f );
            pVertices[2].vPos = D3DXVECTOR3( -fWidth,  -fHeight,  0.f );
            pVertices[3].vPos = D3DXVECTOR3( fWidth,   -fHeight,  0.f );

            pVertices[0].vNor = D3DXVECTOR3( 0.f, 0.f, -1.f );
            pVertices[1].vNor = D3DXVECTOR3( 0.f, 0.f, -1.f );
            pVertices[2].vNor = D3DXVECTOR3( 0.f, 0.f, -1.f );
            pVertices[3].vNor = D3DXVECTOR3( 0.f, 0.f, -1.f );

            pVertices[0].vTex = D3DXVECTOR2( 0.f, 0.f );
            pVertices[1].vTex = D3DXVECTOR2( 1.f, 0.f );
            pVertices[2].vTex = D3DXVECTOR2( 0.f, 1.f );
            pVertices[3].vTex = D3DXVECTOR2( 1.f, 1.f );
        }
	    g_pPlane->UnlockVertexBuffer();


	    WORD* pIndices;
	    g_pPlane->LockIndexBuffer ( 0L, (VOID**)&pIndices );
		pIndices[0] = 0;
        pIndices[1] = 1;
        pIndices[2] = 2;
        pIndices[3] = 1;
        pIndices[4] = 3;
        pIndices[5] = 2;
	    g_pPlane->UnlockIndexBuffer();
    }

    void CreateBox( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vSize )
    {
        SAFE_RELEASE( g_pBox );
        D3DXCreateMeshFVF( 12, 24, D3DXMESH_MANAGED, vm::VERTEX::FVF, pd3dDevice, &g_pBox );


        vm::VERTEX* pVertices;
	    g_pBox->LockVertexBuffer ( 0L, (VOID**)&pVertices );
        {
            int nIndex(0);
            // 상
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  vSize.y,    vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   vSize.y,    vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  vSize.y,    -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   vSize.y,    -vSize.z );

            // 하
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  -vSize.y,   -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   -vSize.y,   -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  -vSize.y,   vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   -vSize.y,   vSize.z );

            // 옆1
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  vSize.y,     vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,   vSize.y,    -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  -vSize.y,    vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,   -vSize.y,   -vSize.z );

            // 옆2
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,  vSize.y,      -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   vSize.y,     vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,  -vSize.y,     -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   -vSize.y,    vSize.z );

            // 옆3
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  vSize.y,     -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   vSize.y,     -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,  -vSize.y,    -vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,   -vSize.y,    -vSize.z );

            // 옆4
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,  vSize.y,      vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,   vSize.y,    vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( vSize.x,  -vSize.y,     vSize.z );
            pVertices[nIndex++].vPos = D3DXVECTOR3( -vSize.x,   -vSize.y,   vSize.z );

            pVertices[0].vNor = pVertices[1].vNor = pVertices[2].vNor = pVertices[3].vNor = D3DXVECTOR3( 0.f, 1.f, 0.f );
            pVertices[4].vNor = pVertices[5].vNor = pVertices[6].vNor = pVertices[7].vNor = D3DXVECTOR3( 0.f, -1.f, 0.f );
            pVertices[8].vNor = pVertices[9].vNor = pVertices[10].vNor = pVertices[11].vNor = D3DXVECTOR3( -1.f, 0.f, 0.f );
            pVertices[12].vNor = pVertices[13].vNor = pVertices[14].vNor = pVertices[15].vNor = D3DXVECTOR3( 1.f, 0.f, 0.f );
            pVertices[16].vNor = pVertices[17].vNor = pVertices[18].vNor = pVertices[19].vNor = D3DXVECTOR3( 0.f, 0.f, -1.f );
            pVertices[20].vNor = pVertices[21].vNor = pVertices[22].vNor = pVertices[23].vNor = D3DXVECTOR3( 0.f, 0.f, 1.f );

            pVertices[0].vTex = pVertices[4].vTex = pVertices[8].vTex = pVertices[12].vTex = pVertices[16].vTex = pVertices[20].vTex = D3DXVECTOR2( 0.f, 0.f );
            pVertices[1].vTex = pVertices[5].vTex = pVertices[9].vTex = pVertices[13].vTex = pVertices[17].vTex = pVertices[21].vTex = D3DXVECTOR2( 1.f, 0.f );
            pVertices[2].vTex = pVertices[6].vTex = pVertices[10].vTex = pVertices[14].vTex = pVertices[18].vTex = pVertices[22].vTex = D3DXVECTOR2( 0.f, 1.f );
            pVertices[3].vTex = pVertices[7].vTex = pVertices[11].vTex = pVertices[15].vTex = pVertices[19].vTex = pVertices[23].vTex = D3DXVECTOR2( 1.f, 1.f );
        }
	    g_pBox->UnlockVertexBuffer();


	    WORD* pIndices;
	    g_pBox->LockIndexBuffer ( 0L, (VOID**)&pIndices );
        {
            int nIndex(0);
		    pIndices[nIndex++] = 0;
            pIndices[nIndex++] = 1;
            pIndices[nIndex++] = 2;
            pIndices[nIndex++] = 1;
            pIndices[nIndex++] = 3;
            pIndices[nIndex++] = 2;

            pIndices[nIndex++] = 0+4;
            pIndices[nIndex++] = 1+4;
            pIndices[nIndex++] = 2+4;
            pIndices[nIndex++] = 1+4;
            pIndices[nIndex++] = 3+4;
            pIndices[nIndex++] = 2+4;

            pIndices[nIndex++] = 0+8;
            pIndices[nIndex++] = 1+8;
            pIndices[nIndex++] = 2+8;
            pIndices[nIndex++] = 1+8;
            pIndices[nIndex++] = 3+8;
            pIndices[nIndex++] = 2+8;

            pIndices[nIndex++] = 0+12;
            pIndices[nIndex++] = 1+12;
            pIndices[nIndex++] = 2+12;
            pIndices[nIndex++] = 1+12;
            pIndices[nIndex++] = 3+12;
            pIndices[nIndex++] = 2+12;

            pIndices[nIndex++] = 0+16;
            pIndices[nIndex++] = 1+16;
            pIndices[nIndex++] = 2+16;
            pIndices[nIndex++] = 1+16;
            pIndices[nIndex++] = 3+16;
            pIndices[nIndex++] = 2+16;

            pIndices[nIndex++] = 0+20;
            pIndices[nIndex++] = 1+20;
            pIndices[nIndex++] = 2+20;
            pIndices[nIndex++] = 1+20;
            pIndices[nIndex++] = 3+20;
            pIndices[nIndex++] = 2+20;
        }
	    g_pBox->UnlockIndexBuffer();
    }

    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        HRESULT hr(S_OK);

        g_pd3dDevice = pd3dDevice;

        D3DXCreateSphere( pd3dDevice, g_fSphereRadius, g_nSlices, g_nStacks, &g_pSphere, NULL );
        D3DXCreateCylinder( pd3dDevice, g_fCylinderRadius1, g_fCylinderRadius2
            , g_fCylinderLength, g_nSlices, g_nStacks, &g_pCylinder, NULL );
        CreatePlane( pd3dDevice, 10.f, 10.f );
        CreateBox( pd3dDevice, D3DXVECTOR3(10.f,10.f,10.f) );

        MakeMesh_VERTEX( pd3dDevice, &g_pSphere );
        MakeMesh_VERTEX( pd3dDevice, &g_pCylinder );

        CreateTangentMesh( pd3dDevice, &g_pBox );
        CreateTangentMesh( pd3dDevice, &g_pPlane );
        CreateTangentMesh( pd3dDevice, &g_pSphere );
        CreateTangentMesh( pd3dDevice, &g_pCylinder );

        CompressTangentMesh( pd3dDevice, &g_pBox );
        CompressTangentMesh( pd3dDevice, &g_pPlane );
        CompressTangentMesh( pd3dDevice, &g_pSphere );
        CompressTangentMesh( pd3dDevice, &g_pCylinder );

        const D3DVERTEXELEMENT9 VertexDecl[] =
        {
		    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		    { 0, 12, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		    { 0, 20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
            { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
            { 0, 32, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
    		        
            D3DDECL_END()
        };

	    pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT );

        g_spVMCommandFX.get()->OnResetDevice();

        g_textureResShininess = ic::TextureManagerInterface::GetInstance()->LoadTexture(
            g_strShininessTex,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_VISUALMATERIAL,
			false,
			FALSE );

        g_spXFileRender = ic::g_funcXFileRender();

        return hr;
    }

    HRESULT OnLostDevice()
    {
        HRESULT hr(S_OK);

        SAFE_RELEASE( g_pBox );
        SAFE_RELEASE( g_pPlane );
        SAFE_RELEASE( g_pSphere );
        SAFE_RELEASE( g_pCylinder );

        SAFE_RELEASE( g_pDCRT );

        g_spVMCommandFX.get()->OnLostDevice();

        g_textureResShininess.Clear();

        return hr;
    }

    void LightRotation( const D3DXVECTOR3& vRotate )
    {
       D3DXVECTOR3 vCameraDir = g_vLightDirection;

	    //	Note : 현재의 회전값.
	    //
	    float Length, LengthXZ;
	    float thetaX, thetaY;

	    Length = D3DXVec3Length ( &vCameraDir );
	    if ( Length == 0 )
		    Length = 0.001f;

	    thetaX = (float) asinf ( vCameraDir.y / Length );

	    //	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	    //
	    LengthXZ = (float) sqrtf ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	    if ( LengthXZ == 0 )
		    LengthXZ = 0.001f;

	    thetaY = (float) acosf ( vCameraDir.x / LengthXZ );
	    if ( vCameraDir.z>0 )
		    thetaY = -thetaY;

	    //	Note : 회전 적용.
	    //
	    thetaX += vRotate.x;
	    thetaY += vRotate.y;

	    ////	Note : 회전 제한 값.
	    ////
	    //if ( m_dwFlag & DXVP_LIMITDIR )
	    //{
	    //	if ( m_fHIGHXDIR < thetaX )
	    //		thetaX = m_fHIGHXDIR;
	    //	else if ( m_fLOWXDIR > thetaX )
	    //		thetaX = m_fLOWXDIR;
	    //}
	    ////	Note : 연산 특성상 "LengthXZ==0" 이 되지 않도록 제한할 필요가 있다.
	    ////
	    //else
	    //{
		    const static float fLIMIT = D3DX_PI/2.0f - 0.001f;
		    if ( fLIMIT < thetaX )
			    thetaX = fLIMIT;
		    else if ( -fLIMIT > thetaX )
			    thetaX = -fLIMIT;
	    //}

	    //	Note : 카메라 방향 단위 백터 계산.
	    //
	    //		Dir = { 1, 0, 0 } 의 초기값에서 회전 시킴.
	    //
	    D3DXVECTOR3 Dir;

	    //	Note : 피치회전.
	    //
	    Dir.y = sinf(thetaX);

	    Dir.x = 1-Dir.y*Dir.y;
	    Dir.x = ( Dir.x < 0 ) ? 0 : sqrtf(Dir.x);

	    Dir.z = (float) (-Dir.x*sinf(thetaY) );
	    Dir.x = (float) ( Dir.x*cosf(thetaY) );

        g_vLightDirection = Dir*Length;
    }

    void UpdateLight( BOOL bTarPosWin )
    {
        // 카메라 작동
        if ( bTarPosWin )
	    {
            // L 키를 누른 상태에서만 동작된다.
            if( ic::DxInputDeviceInterface::GetInstance()->GetKeyState(DIK_L) & ic::DXKEY_DOWNED )
	        {
                // Note : 초기화
                int         dx(0), dy(0), dz(0);
                D3DXVECTOR3 vRotate(0.f,0.f,0.f);
                ic::DxInputDeviceInterface::GetInstance()->GetMouseMove ( dx, dy, dz );
                DWORD dwLButton = ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT );

                if ( dwLButton&ic::DXKEY_DRAG )    
	            {
                    vRotate.x += 0.01f*(dy);
			        vRotate.y -= 0.01f*(dx);
	            }

                LightRotation( vRotate );
	        }
        }
    };

    void LoadCustomMesh( const TSTRING& strName )
    {
        if ( !g_spXFileRender.get() )
            return;

        g_spXFileRender.get()->CleanUpVM();

//        TSTRING strTextureNameLwr( strName );
//        sc::string::lowerString( strTextureNameLwr );

		g_spXFileRender.get()->SetFile( _T(""), strName.c_str() );
		g_spXFileRender.get()->SetFVF( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );
		g_spXFileRender.get()->CreateVM( g_pd3dDevice );
        g_spXFileRender.get()->CreateTangentMesh( g_pd3dDevice );
        g_spXFileRender.get()->CompressTangentMesh( g_pd3dDevice );
    }

    HRESULT OnFrameMove( BOOL bTarPosWin, float fElapsedTime )
    {
        HRESULT hr(S_OK);

        g_fTime += fElapsedTime;
        UpdateLight( bTarPosWin );

        return hr;
    }    

    HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, BOOL bGlow )
    {
        HRESULT hr(S_OK);

		DWORD dwIndex(1);
		if ( bGlow )
		{
			dwIndex = 2;
		}

        UINT numPasses;
        hr = pFX->Begin( &numPasses, 0 ); 
        if( dwIndex < numPasses )
        {
	        hr = pFX->BeginPass( dwIndex );
            {
                if ( g_spXFileRender.get() && g_spXFileRender.get()->ExistXFile() )
                {
                    g_spXFileRender.get()->RenderVisualMaterial( pd3dDevice, pFX, g_pDCRT, matView, matProj );
                }
                else
                {
                    switch ( g_nPreviewRenderStyle )
                    {
                    case MODEL_CYLINDER:
                        DrawMeshFX( pd3dDevice, g_pCylinder );
                        break;

                    case MODEL_CUBE:
                        DrawMeshFX( pd3dDevice, g_pBox );
                        break;

                    case MODEL_SPHERE:
                        DrawMeshFX( pd3dDevice, g_pSphere );
                        break;

                    case MODEL_PLANE:
                        DrawMeshFX( pd3dDevice, g_pPlane );
                        break;
                    };
                }
            }
	        hr = pFX->EndPass();
        }
        hr = pFX->End();

        return hr;
    }

    HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bActivePL )
    {
        HRESULT hr(S_OK);

        // Note : SetRenderState()
        pd3dDevice->SetRenderState( D3DRS_LIGHTING, 	    TRUE );
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    TRUE );
        pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    TRUE );
        pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

        // 기본적으로 깔리는 사각 Wireframe
        if ( g_bGridToggle )
            vm::EDITMESHS::RENDERPLANE( pd3dDevice, D3DXVECTOR3(0,0,0), D3DXVECTOR3(100,0,100), D3DXVECTOR3(10,0,10) );

        ID3DXEffect* pFX = g_spVMCommandFX.get()->GetFX();
        if ( !pFX )
            return S_OK;

        // Note : SetTextureStageState()
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

        D3DLIGHT9 sLight;
        sLight.Type = D3DLIGHT_DIRECTIONAL;
        sLight.Diffuse.r = sLight.Diffuse.g = sLight.Diffuse.b = sLight.Diffuse.a = 1.f;
        sLight.Specular.r = sLight.Specular.g = sLight.Specular.b = sLight.Specular.a = 1.f;
        sLight.Ambient.r = sLight.Ambient.g = sLight.Ambient.b = sLight.Ambient.a = 0.2f;
        sLight.Direction = g_vLightDirection;
        sLight.Range = 10.f;
        pd3dDevice->SetLight( 0, &sLight );
        pd3dDevice->LightEnable ( 0, FALSE );


        D3DXVECTOR3 vLightDir3;
        D3DXVECTOR4 vLightDir;
        D3DXVECTOR3 vHalfDir3;
        D3DXVECTOR4 vHalfDir4;
        D3DXVECTOR4 vCameraFrom4;
        D3DXMATRIX matView, matProj;
        {
            pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
            pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

            D3DXMATRIX matWV, matWVP;
		    D3DXMatrixMultiply( &matWV, &matIdentity, &matView );
		    D3DXMatrixMultiply( &matWVP, &matWV, &matProj );

		    pFX->SetMatrixTranspose( "g_matWorld",	&matIdentity );
		    pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );

            D3DXVECTOR4 vTime(g_fTime,0.f,0.f,0.f);
            D3DXVECTOR4 vUserColor1(0.f,0.f,0.f,0.f);
            D3DXVECTOR4 vUserColor2(0.f,0.f,0.f,0.f);
            D3DXVECTOR4 vLightDiffuse(1.f,1.f,1.f,1.f);
            D3DXVECTOR4 vLightAmbient(0.2f,0.2f,0.2f,0.f);
            D3DXVECTOR4 vLightSpecular(1.f,1.f,1.f,1.f);
            vLightDir3.x = -g_vLightDirection.x;
            vLightDir3.y = -g_vLightDirection.y;
            vLightDir3.z = -g_vLightDirection.z;
            D3DXVec3Normalize( &vLightDir3, &vLightDir3 );
            vLightDir.x = vLightDir3.x;
            vLightDir.y = vLightDir3.y;
            vLightDir.z = vLightDir3.z;
            vLightDir.w = 1.f;

            vCameraFrom4.x = vFromPt.x;
            vCameraFrom4.y = vFromPt.y;
            vCameraFrom4.z = vFromPt.z;
            vCameraFrom4.w = 1.f;

            //// Half Vector 구하기.
            //{
            //    D3DXVECTOR3 vCameraDir = vFromPt - vLookatPt;
            //    D3DXVec3Normalize( &vCameraDir, &vCameraDir );
            //    vHalfDir3 = vCameraDir + vLightDir3;
            //    D3DXVec3Normalize( &vHalfDir3, &vHalfDir3 );
            //    vHalfDir4.x = vHalfDir3.x;
            //    vHalfDir4.y = vHalfDir3.y;
            //    vHalfDir4.z = vHalfDir3.z;
            //    vHalfDir4.w = 1.f;
            //}

			pFX->SetVector( "g_vLightDiffuse", &vLightDiffuse );
			pFX->SetVector( "g_vLightAmbient", &vLightAmbient );
            pFX->SetVector( "g_vLightSpecular", &vLightSpecular );
            pFX->SetVector( "g_vLightDir", &vLightDir );
            pFX->SetVector( "g_vCameraFrom", &vCameraFrom4 );
            pFX->SetVector( "g_fTime", &vTime );
            pFX->SetVector( "g_vUserColor1", &g_vUserColor1 );
            pFX->SetVector( "g_vUserColor2", &g_vUserColor2 );
            //pFX->SetVector( "g_vHalfDir", &vHalfDir4 );

            pFX->SetTexture( "ShininessTex", g_textureResShininess.GetTexture() );

            // Fog
		    D3DXVECTOR4 vFog
		    (
                1.f, 
			    1.f,
                1.f,
                1.f
		    );
		    pFX->SetVector( "g_vFog", &vFog );

            vm::MAP_PARAMETER_NAME_DATA mapParamaterNameData;

            // Texture__0 ... 설정
            {
                char c1='0';
                std::string strTextureORIG = "Texture_";
                std::string strTexture = "";
                for ( size_t i=0; i<g_vecTextureResource.size(); ++i )
                {
                    strTexture = strTextureORIG + static_cast<char>( c1 + i );
                    pFX->SetTexture( strTexture.c_str(), g_vecTextureResource[i].m_textureRes.GetTexture() );
                }
            }

            // TextureCube_0 ... 설정
            {
                char c1='0';
                std::string strTextureCubeORIG = "CubeTexture_";
                std::string strTextureCube = "";
                for ( size_t i=0; i<g_vecCubeTextureResource.size(); ++i )
                {
                    strTextureCube = strTextureCubeORIG + static_cast<char>( c1 + i );
                    pFX->SetTexture( strTextureCube.c_str(), g_vecCubeTextureResource[i].m_textureRes.GetCubeTexture() );
                }
            }

            // g_fUniformPixelScalar_0 ... 설정
            {
                char c1='0';
                std::string strUniformPixelScalarORIG = "g_fUniformPixelScalar_";
                std::string strUniformPixelScalar = "";
                for ( size_t i=0; i<g_vecPixelScalar.size(); ++i )
                {
                    g_vecPixelScalar[i]->FrameMove( g_fTime );

                    strUniformPixelScalar = strUniformPixelScalarORIG + static_cast<char>( c1 + i );
                    pFX->SetFloat( strUniformPixelScalar.c_str(), fabsf( g_vecPixelScalar[i]->GetPixelVector( mapParamaterNameData ).x ) );
                }
            }

            // g_vUniformPixelVector_0 ... 설정
            {
                char c1='0';
                std::string strParameterORIG = "g_vUniformPixelVector_";
                std::string strParameter = "";
                for ( size_t i=0; i<g_vecPixelVector.size(); ++i )
                {
                    g_vecPixelVector[i]->FrameMove( g_fTime );

                    strParameter = strParameterORIG + static_cast<char>( c1 + i );
                    pFX->SetVector( strParameter.c_str(), &g_vecPixelVector[i]->GetPixelVector( mapParamaterNameData ) );
                }
            }
        }

        Render( pd3dDevice, pFX, matView, matProj, FALSE );

        if ( g_spVMCommandFX.get()->GetTwoSideSeperatePass() )
        {
            DWORD dwCullMode;
            pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
            pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

            vLightDir.x = vLightDir3.x;
            vLightDir.y = vLightDir3.y;
            vLightDir.z = vLightDir3.z;
            vLightDir.w = -1.f;

            //// Half Vector 구하기.
            //{
            //    D3DXVECTOR3 vCameraDir = vFromPt - vLookatPt;
            //    D3DXVec3Normalize( &vCameraDir, &vCameraDir );
            //    vHalfDir3 = vCameraDir + vLightDir3;
            //    D3DXVec3Normalize( &vHalfDir3, &vHalfDir3 );
            //    vHalfDir4.x = vHalfDir3.x;
            //    vHalfDir4.y = vHalfDir3.y;
            //    vHalfDir4.z = vHalfDir3.z;
            //    vHalfDir4.w = 1.f;
            //}

            pFX->SetVector( "g_vLightDir", &vLightDir );
            //pFX->SetVector( "g_vHalfDir", &vCameraFrom4 );

            Render( pd3dDevice, pFX, matView, matProj, FALSE );

            pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode );
        }

        hr = pd3dDevice->SetVertexShader(NULL); 
        hr = pd3dDevice->SetPixelShader(NULL);

        return hr;
    }

	HRESULT Render_StarGlow( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT hr(S_OK);
		ID3DXEffect* pFX = g_spVMCommandFX.get()->GetFX();
		if ( !pFX )
			return S_OK;

		D3DXMATRIX matView, matProj;
		pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
		pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

		if ( g_spVMCommandFX.get()->IsActiveStarGlow() )
		{
			Render( pd3dDevice, pFX, matView, matProj, TRUE );
		}

		hr = pd3dDevice->SetVertexShader(NULL); 
		hr = pd3dDevice->SetPixelShader(NULL);

		return S_OK;
	}

    void DrawMeshFX( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh )
    {
        pd3dDevice->SetFVF( 0 );
        pd3dDevice->SetVertexDeclaration( g_pDCRT );

        LPDIRECT3DVERTEXBUFFER9 pVB(NULL);
        LPDIRECT3DINDEXBUFFER9 pIB(NULL);

        pMesh->GetVertexBuffer( &pVB );
        pMesh->GetIndexBuffer( &pIB );
        pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(vm::VERTEXNORTANGENTTEX2_COMPRESS) );
        pd3dDevice->SetIndices( pIB );
        pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pMesh->GetNumVertices(), 0, pMesh->GetNumFaces() );

        SAFE_RELEASE( pVB );
        SAFE_RELEASE( pIB );
    }
};
