#include "stdafx.h"

#include <boost/tr1/memory.hpp>

#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"
#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Define/DxVMDefine.h"
#include "./VisualMaterialFX.h"

#include "DxVMManager.h"

// ----------------------------------------------------------------------------
#include "../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


namespace NS_VM_MANAGER
{
    struct NAME_TYPE
    {
        TSTRING     m_strName;
        RENDER_TYPE m_emType;

    };

    bool operator < ( const NAME_TYPE& lhs, const NAME_TYPE& rhs )
    {
        if ( lhs.m_strName < rhs.m_strName )
            return true;
        else if ( lhs.m_strName > rhs.m_strName )
            return false;

        if ( lhs.m_emType < rhs.m_emType )
            return true;
        else if ( lhs.m_emType > rhs.m_emType )
            return false;

        return false;
    }

    typedef std::map<NAME_TYPE,std::tr1::shared_ptr<VisualMaterialFX>> MAP_VM_FX;
    typedef MAP_VM_FX::iterator                                     MAP_VM_FX_ITER;

    TSTRING                         g_strFullPath = _T("");
    LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_PIECE;
    LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_PIECE_COLOR;
    LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_PIECE_COLOR_LM;
    LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_SKIN;
    LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_SKIN_OBJECT;
    ic::TextureResourceInterface    g_textureResShininess;
    TSTRING				            g_strShininessTex = _T("Shininess.dds");	// 광텍도 텍스쳐
    MAP_VM_FX                       g_mapVMFX;
    BOOL                            g_bGame(TRUE);

    void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
    {
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

        // Object
        {
            // 36 byte
            D3DVERTEXELEMENT9 VertexDecl[] =
            {        
                { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		        { 0, 12, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		        { 0, 20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
                { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
                { 0, 32, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },

                D3DDECL_END()
            };
            pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_PIECE );
        }

        // Object
        {
            // 36 byte
            D3DVERTEXELEMENT9 VertexDecl[] =
            {        
                { 0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		        { 0, 12, D3DDECLTYPE_SHORT4,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		        { 0, 20, D3DDECLTYPE_SHORT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
                { 0, 24, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
                { 0, 28, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },

                D3DDECL_END()
            };
            pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_PIECE_COLOR );
        }

        // Object
        {
            // 40 byte
            D3DVERTEXELEMENT9 VertexDecl[] =
            {        
                { 0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		        { 0, 12, D3DDECLTYPE_SHORT4,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		        { 0, 20, D3DDECLTYPE_SHORT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
                { 0, 24, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
                { 0, 28, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
                { 0, 36, D3DDECLTYPE_SHORT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },

                D3DDECL_END()
            };
            pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_PIECE_COLOR_LM );
        }

        // Skin VS
        {
            // 40 byte
            D3DVERTEXELEMENT9 VertexDecl_SKIN_VS[] =
            {        
                { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	            { 0, 12, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	            { 0, 20, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
                { 0, 28, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
	            { 0, 36, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

                D3DDECL_END()
            };
            pd3dDevice->CreateVertexDeclaration( VertexDecl_SKIN_VS,  &g_pDCRT_SKIN );
        }

        // Object
        {
            // 28 byte
            D3DVERTEXELEMENT9 VertexDecl[] =
            {        
                { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		        { 0, 12, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		        { 0, 20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
                { 0, 24, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },

                D3DDECL_END()
            };
            pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_SKIN_OBJECT );
        }


        MAP_VM_FX_ITER iter = g_mapVMFX.begin();
        for ( ; iter!=g_mapVMFX.end(); ++iter )
        {
            (*iter).second->OnResetDevice( pd3dDevice );
        }
    }

    void OnLostDevice()
    {
        SAFE_RELEASE( g_pDCRT_PIECE );
        SAFE_RELEASE( g_pDCRT_PIECE_COLOR );
        SAFE_RELEASE( g_pDCRT_PIECE_COLOR_LM );
        SAFE_RELEASE( g_pDCRT_SKIN );
        SAFE_RELEASE( g_pDCRT_SKIN_OBJECT );

        g_textureResShininess.Clear();

        MAP_VM_FX_ITER iter = g_mapVMFX.begin();
        for ( ; iter!=g_mapVMFX.end(); ++iter )
        {
            (*iter).second->OnLostDevice();
        }
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

        D3DXVECTOR3 vVector;
        vm::VERTEXNORTANGENTTEX2* pbVertSrc;
		vm::VERTEXNORTANGENTTEX2_COMPRESS* pbVertDestShader;
		(*ppMesh)->LockVertexBuffer( 0, (VOID**)&pbVertSrc );
        pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
		for( DWORD i=0; i<(*ppMesh)->GetNumVertices(); ++i )
		{
			pbVertDestShader[i].vPos = pbVertSrc[i].vPos;

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vNor );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vNor[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vNor[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vNor[2] = static_cast<short>( vVector.z * 32767.f );

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vTan );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vTan[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vTan[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vTan[2] = static_cast<short>( vVector.z * 32767.f );

            pbVertDestShader[i].vTex0 = pbVertSrc[i].vTex[0];
            pbVertDestShader[i].vTex1[0] = static_cast<short>( pbVertSrc[i].vTex[1].x * 32767.f );
            pbVertDestShader[i].vTex1[1] = static_cast<short>( pbVertSrc[i].vTex[1].y * 32767.f );
		}
        pTempMesh->UnlockVertexBuffer();
		(*ppMesh)->UnlockVertexBuffer();

        SAFE_RELEASE( (*ppMesh) );
        (*ppMesh) = pTempMesh;
    }

    void CreateTangentColorMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        LPD3DXMESH pMeshTangent;
        HRESULT hr = vm::CreateTagentColorMeshLM( pd3dDevice, (*ppMesh), &pMeshTangent );
		if( FAILED(hr) )
		{
			return;
		}
		if( !pMeshTangent )		return;

		LPD3DXMESH pTemp = (*ppMesh);
		(*ppMesh) = pMeshTangent;
		SAFE_RELEASE( pTemp );
    }

    void CompressTangentColorMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        ID3DXMesh* pTempMesh = NULL;
        (*ppMesh)->CloneMeshFVF( (*ppMesh)->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE1(1), pd3dDevice, &pTempMesh );	
        if ( !pTempMesh )
            return;

        D3DXVECTOR3 vVector;
        vm::VERTEXNORCOLORTANGENTTEX2* pbVertSrc;
		vm::VERTEXNORCOLORTANGENTTEX2_COMPRESS* pbVertDestShader;
		(*ppMesh)->LockVertexBuffer( 0, (VOID**)&pbVertSrc );
        pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
		for( DWORD i=0; i<(*ppMesh)->GetNumVertices(); ++i )
		{
			pbVertDestShader[i].vPos = pbVertSrc[i].vPos;

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vNor );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vNor[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vNor[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vNor[2] = static_cast<short>( vVector.z * 32767.f );

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vTan );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vTan[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vTan[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vTan[2] = static_cast<short>( vVector.z * 32767.f );

            pbVertDestShader[i].cColor = pbVertSrc[i].cColor;

            pbVertDestShader[i].vTex0 = pbVertSrc[i].vTex[0];
            pbVertDestShader[i].vTex1[0] = static_cast<short>( pbVertSrc[i].vTex[1].x * 32767.f );
            pbVertDestShader[i].vTex1[1] = static_cast<short>( pbVertSrc[i].vTex[1].y * 32767.f );
		}
        pTempMesh->UnlockVertexBuffer();
		(*ppMesh)->UnlockVertexBuffer();

        SAFE_RELEASE( (*ppMesh) );
        (*ppMesh) = pTempMesh;
    }

    void CreateTangentColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        LPD3DXMESH pMeshTangent;
        HRESULT hr = vm::CreateTagentColorMesh( pd3dDevice, (*ppMesh), &pMeshTangent );
		if( FAILED(hr) )
		{
			return;
		}
		if( !pMeshTangent )		return;

		LPD3DXMESH pTemp = (*ppMesh);
		(*ppMesh) = pMeshTangent;
		SAFE_RELEASE( pTemp );
    }

    void CompressTangentColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh, BOOL bHaveDiffuse )
    {
        ID3DXMesh* pTempMesh = NULL;
        (*ppMesh)->CloneMeshFVF( (*ppMesh)->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1, pd3dDevice, &pTempMesh );	
        if ( !pTempMesh )
            return;

        D3DXVECTOR3 vVector;
        vm::VERTEXNORCOLORTEXTANGENT* pbVertSrc;
		vm::VERTEXNORCOLORTEXTANGENT_COMPRESS* pbVertDestShader;
		(*ppMesh)->LockVertexBuffer( 0, (VOID**)&pbVertSrc );
        pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
		for( DWORD i=0; i<(*ppMesh)->GetNumVertices(); ++i )
		{
			pbVertDestShader[i].vPos = pbVertSrc[i].vPos;

            if ( bHaveDiffuse )
            {
                pbVertDestShader[i].cColor = pbVertSrc[i].cColor;
            }
            else
            {
                pbVertDestShader[i].cColor = 0xffffffff;
            }

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vNor );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vNor[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vNor[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vNor[2] = static_cast<short>( vVector.z * 32767.f );

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vTan );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vTan[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vTan[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vTan[2] = static_cast<short>( vVector.z * 32767.f );

            pbVertDestShader[i].vTex = pbVertSrc[i].vTex;
		}
        pTempMesh->UnlockVertexBuffer();
		(*ppMesh)->UnlockVertexBuffer();

        SAFE_RELEASE( (*ppMesh) );
        (*ppMesh) = pTempMesh;
    }

    void CreateTangentSkinMeshObject( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        LPD3DXMESH pMeshTangent;
        HRESULT hr = vm::CreateTagentSkinMeshObject( pd3dDevice, (*ppMesh), &pMeshTangent );
		if( FAILED(hr) )
		{
			return;
		}
		if( !pMeshTangent )		return;

		LPD3DXMESH pTemp = (*ppMesh);
		(*ppMesh) = pMeshTangent;
		SAFE_RELEASE( pTemp );
    }

    void CompressTangentSkinMeshObject( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        ID3DXMesh* pTempMesh = NULL;
        (*ppMesh)->CloneMeshFVF( (*ppMesh)->GetOptions(), D3DFVF_XYZ|D3DFVF_TEX2, pd3dDevice, &pTempMesh );	
        if ( !pTempMesh )
            return;

        D3DXVECTOR3 vVector;
        vm::VERTEXNORTEXTANGENT* pbVertSrc;
		vm::VERTEXNORTEXTANGENT_COMPRESS* pbVertDestShader;
		(*ppMesh)->LockVertexBuffer( 0, (VOID**)&pbVertSrc );
        pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
		for( DWORD i=0; i<(*ppMesh)->GetNumVertices(); ++i )
		{
			pbVertDestShader[i].vPos = pbVertSrc[i].vPos;

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vNor );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vNor[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vNor[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vNor[2] = static_cast<short>( vVector.z * 32767.f );

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vTan );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vTan[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vTan[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vTan[2] = static_cast<short>( vVector.z * 32767.f );

            pbVertDestShader[i].vTex[0] = static_cast<short>( pbVertSrc[i].vTex.x * 32767.f );
			pbVertDestShader[i].vTex[1] = static_cast<short>( pbVertSrc[i].vTex.y * 32767.f );
		}
        pTempMesh->UnlockVertexBuffer();
		(*ppMesh)->UnlockVertexBuffer();

        SAFE_RELEASE( (*ppMesh) );
        (*ppMesh) = pTempMesh;
    }

    void CreateTangentSkinMeshVS( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        LPD3DXMESH pMeshTangent;
        HRESULT hr = vm::CreateTagentSkinMeshVS( pd3dDevice, (*ppMesh), &pMeshTangent );
		if( FAILED(hr) )
		{
			return;
		}
		if( !pMeshTangent )		return;

		LPD3DXMESH pTemp = (*ppMesh);
		(*ppMesh) = pMeshTangent;
		SAFE_RELEASE( pTemp );
    }

    void CompressTangentSkinMeshVS( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh )
    {
        ID3DXMesh* pTempMesh = NULL;
        (*ppMesh)->CloneMeshFVF( (*ppMesh)->GetOptions(), D3DFVF_XYZ|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE1(3), pd3dDevice, &pTempMesh );	
        if ( !pTempMesh )
            return;

        D3DXVECTOR3 vVector;
		vm::VERTEXNORTEXTANGENT_VS* pbVertSrc;
		vm::VERTEXNORTEXTANGENT_VS_COMPRESS* pbVertDestShader;
		(*ppMesh)->LockVertexBuffer( 0, (VOID**)&pbVertSrc );
        pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
		for( DWORD i=0; i<(*ppMesh)->GetNumVertices(); ++i )
		{
			pbVertDestShader[i].vPos = pbVertSrc[i].vPos;

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vNor );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vNor[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vNor[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vNor[2] = static_cast<short>( vVector.z * 32767.f );

			D3DXVec3Normalize( &vVector, &pbVertSrc[i].vTan );	// 노멀라이즈를 해준다.
			pbVertDestShader[i].vTan[0] = static_cast<short>( vVector.x * 32767.f );
			pbVertDestShader[i].vTan[1] = static_cast<short>( vVector.y * 32767.f );
			pbVertDestShader[i].vTan[2] = static_cast<short>( vVector.z * 32767.f );

			pbVertDestShader[i].vTex[0] = static_cast<short>( pbVertSrc[i].vTex.x * 32767.f );
			pbVertDestShader[i].vTex[1] = static_cast<short>( pbVertSrc[i].vTex.y * 32767.f );

            pbVertDestShader[i].vWeight[0] = static_cast<short>( pbVertSrc[i].vWeight.x * 32767.f );
            pbVertDestShader[i].vWeight[1] = static_cast<short>( pbVertSrc[i].vWeight.y * 32767.f );
            pbVertDestShader[i].vWeight[2] = static_cast<short>( pbVertSrc[i].vWeight.z * 32767.f );
            pbVertDestShader[i].vWeight[3] = static_cast<short>( pbVertSrc[i].vWeight.w * 32767.f );

            pbVertDestShader[i].vIndies[0] = static_cast<byte>( pbVertSrc[i].vIndies.x );
            pbVertDestShader[i].vIndies[1] = static_cast<byte>( pbVertSrc[i].vIndies.y );
            pbVertDestShader[i].vIndies[2] = static_cast<byte>( pbVertSrc[i].vIndies.z );
            pbVertDestShader[i].vIndies[3] = static_cast<byte>( pbVertSrc[i].vIndies.w );
		}
        pTempMesh->UnlockVertexBuffer();
		(*ppMesh)->UnlockVertexBuffer();

        SAFE_RELEASE( (*ppMesh) );
        (*ppMesh) = pTempMesh;
    }

    std::tr1::shared_ptr<VisualMaterialFX> LoadFile( LPDIRECT3DDEVICEQ pd3dDevice, TSTRING strName, RENDER_TYPE emRenderTYPE )
    {
        TCHAR szPathName[MAX_PATH] = _T("");
        StringCchCopy( szPathName, MAX_PATH, g_strFullPath.c_str() );
	    StringCchCat( szPathName, MAX_PATH, strName.c_str() );

        std::tr1::shared_ptr<VisualMaterialFX> sVMFX(new VisualMaterialFX);

        std::auto_ptr<ic::SerialFileInterface> apSFile = ic::g_funcSerialFile();
        ic::SerialFileInterface* pSFile = apSFile.get();
	    if ( !pSFile->OpenFile( ic::FOT_READ, szPathName ) )
        {
            pSFile->CloseFile();
            return sVMFX;
        }

        if ( sVMFX->Load( pd3dDevice, strName.c_str(), pSFile, emRenderTYPE ) != S_OK )
		{
			pSFile->CloseFile();
			return sVMFX;
		}

        pSFile->CloseFile();

        // Game이 아닐 경우만 시간을 기록해놓는다.
        if ( !g_bGame )
        {
            // 시간 기록해 놓기
            _WIN32_FILE_ATTRIBUTE_DATA sFileData;
	        if( !GetFileAttributesEx( szPathName, GetFileExInfoStandard, &sFileData ) )
		        return sVMFX;

            sVMFX->m_cLastWriteTime = sFileData.ftLastWriteTime;
        }

        return sVMFX;
    }

    std::tr1::shared_ptr<VisualMaterialFX> Load( LPDIRECT3DDEVICEQ pd3dDevice, TSTRING strName, RENDER_TYPE emRenderTYPE )
    {
        if ( strName.empty() )
            return std::tr1::shared_ptr<VisualMaterialFX>(new VisualMaterialFX);

        NAME_TYPE sNameType;
        sNameType.m_strName = strName;
        sNameType.m_emType = emRenderTYPE;

        MAP_VM_FX_ITER iter = g_mapVMFX.find( sNameType );
        if ( iter != g_mapVMFX.end() )
        {
            return (*iter).second;
        }

        std::tr1::shared_ptr<VisualMaterialFX> spVMFX = LoadFile( pd3dDevice, sNameType.m_strName, emRenderTYPE );
        g_mapVMFX.insert( std::make_pair( sNameType, spVMFX ) );

        return spVMFX;
    }

    BOOL NeedReload( float fElapsedTime )
    {
        // 이 함수에 들어온다는 것은 게임이 아닌 Tool 이다.
        g_bGame = FALSE;

    	const FLOAT s_fDelaySec = 0.1f;
		static FLOAT s_fAccuSec = 0.0f;

		s_fAccuSec += fElapsedTime;
		if( s_fAccuSec < s_fDelaySec )
			return FALSE;

		s_fAccuSec = 0.0f;

        MAP_VM_FX_ITER iter = g_mapVMFX.begin();
        for ( ; iter != g_mapVMFX.end(); ++iter )
        {
            TCHAR szPathName[MAX_PATH] = _T("");
            StringCchCopy( szPathName, MAX_PATH, g_strFullPath.c_str() );
	        StringCchCat( szPathName, MAX_PATH, (*iter).first.m_strName.c_str() );

            _WIN32_FILE_ATTRIBUTE_DATA sFileData;
		    if( !GetFileAttributesEx( szPathName, GetFileExInfoStandard, &sFileData ) )
			    continue;

            // 변경된것 확인
            if( (*iter).second->m_cLastWriteTime != sFileData.ftLastWriteTime )
            {
			    return TRUE;
            }
        }

        return FALSE;
    }

    // Tool 에서만 쓰이는 함수임.
    void ClearUpVMFX( BOOL bReleaseTexture )
    {
        // 변경된 텍스쳐를 로딩할 수 있도록 리소스 관리자에서 날려버린다
        if ( !bReleaseTexture )
        {
            MAP_VM_FX_ITER iter = g_mapVMFX.begin();
            for ( ; iter != g_mapVMFX.end(); ++iter )
            {
                (*iter).second->SetTexturePointerNULL();
            }
        }

        g_mapVMFX.clear();
    }
};
