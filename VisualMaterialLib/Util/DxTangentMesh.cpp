#include "stdafx.h"

#include "../../InternalCommonLib/Interface/DebugSetInterface.h"

#include "../Manager/DxVMManager.h"
#include "../Define/DxVMDefine.h"

#include "DxTangentMesh.h"

// ----------------------------------------------------------------------------
#include "../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------

DxTangentMesh::DxTangentMesh()
    : m_pVB(NULL)
	, m_pIB(NULL)
    , m_pAttrib(NULL)
    , m_dwVert(0)
	, m_dwFace(0)
	, m_dwAttrib(0)
{
}

DxTangentMesh::~DxTangentMesh()
{
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_DELETE_ARRAY( m_pAttrib );
}

HRESULT DxTangentMesh::CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwFVF, DWORD dwVert, DWORD dwFace )
{
    LPD3DXMESH pTempMesh;
	D3DXCreateMeshFVF ( dwFace, dwVert, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pTempMesh );
	if ( !pTempMesh )
    {
        ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - CreateMesh가 만들어 지지 않았습니다." );
        return S_OK;
    }

	UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

	BYTE *pBuffer(NULL);
    BYTE *pBufferSRC(NULL);
    pVB->Lock( 0, 0, (VOID**)&pBufferSRC, D3DLOCK_READONLY );
	pTempMesh->LockVertexBuffer( 0L, (VOID**)&pBuffer );
	memcpy ( pBuffer, pBufferSRC, nSIZE*dwVert );
	pTempMesh->UnlockVertexBuffer();
    pVB->Unlock();

    pIB->Lock( 0, 0, (VOID**)&pBufferSRC, D3DLOCK_READONLY );
	pTempMesh->LockIndexBuffer ( 0L, (VOID**)&pBuffer );
    memcpy ( pBuffer, pBufferSRC, sizeof(WORD)*dwFace*3 );
	pTempMesh->UnlockIndexBuffer();
    pIB->Unlock();

    // 
    CreateMesh( pd3dDevice, pTempMesh );

    SAFE_RELEASE( pTempMesh );

    return S_OK;
}

HRESULT DxTangentMesh::CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh )
{
    LPD3DXMESH pTempMesh;
    if ( FAILED( pSrcMesh->CloneMeshFVF( pSrcMesh->GetOptions(), vm::VERTEXNORTEX2::FVF,
										pd3dDevice, &pTempMesh ) ) )
    {
        ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - CreateMesh가 만들어 지지 않았습니다." );
		return E_FAIL;
    }

    NS_VM_MANAGER::CreateTangentMesh( pd3dDevice, &pTempMesh );
    NS_VM_MANAGER::CompressTangentMesh( pd3dDevice, &pTempMesh );

    CreateVBIBATT( pd3dDevice, pTempMesh, sizeof(vm::VERTEXNORTANGENTTEX2_COMPRESS), vm::VERTEXNORTANGENTTEX2_COMPRESS::FVF );

    SAFE_RELEASE( pTempMesh );

    return S_OK;
};

HRESULT DxTangentMesh::CreateColorMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh )
{
    LPD3DXMESH pTempMesh;
    if ( FAILED( pSrcMesh->CloneMeshFVF( pSrcMesh->GetOptions(), vm::VERTEXNORCOLORTEX2::FVF,
										pd3dDevice, &pTempMesh ) ) )
    {
        ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - CreateMesh가 만들어 지지 않았습니다." );
		return E_FAIL;
    }

    NS_VM_MANAGER::CreateTangentColorMeshLM( pd3dDevice, &pTempMesh );
    NS_VM_MANAGER::CompressTangentColorMeshLM( pd3dDevice, &pTempMesh );

    CreateVBIBATT( pd3dDevice, pTempMesh, sizeof(vm::VERTEXNORCOLORTANGENTTEX2_COMPRESS), vm::VERTEXNORCOLORTANGENTTEX2_COMPRESS::FVF );

    SAFE_RELEASE( pTempMesh );

    return S_OK;
};

HRESULT DxTangentMesh::CreateSkinMeshObject( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh )
{
    LPD3DXMESH pTempMesh;
    if ( FAILED( pSrcMesh->CloneMeshFVF( pSrcMesh->GetOptions(), vm::VERTEX::FVF,
										pd3dDevice, &pTempMesh ) ) )
    {
        ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - CreateSkinMeshObject가 만들어 지지 않았습니다." );
		return E_FAIL;
    }

    NS_VM_MANAGER::CreateTangentSkinMeshObject( pd3dDevice, &pTempMesh );
    NS_VM_MANAGER::CompressTangentSkinMeshObject( pd3dDevice, &pTempMesh );

    CreateVBIBATT( pd3dDevice, pTempMesh, sizeof(vm::VERTEXNORTEXTANGENT_COMPRESS), vm::VERTEXNORTEXTANGENT_COMPRESS::FVF );

    SAFE_RELEASE( pTempMesh );

    return S_OK;
};

HRESULT DxTangentMesh::CreateColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwFVF, DWORD dwVert, DWORD dwFace )
{
    LPD3DXMESH pTempMesh;
	D3DXCreateMeshFVF ( dwFace, dwVert, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pTempMesh );
	if ( !pTempMesh )
    {
        ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - CreateColorMesh가 만들어 지지 않았습니다." );
        return S_OK;
    }

	UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

	BYTE *pBuffer(NULL);
    BYTE *pBufferSRC(NULL);
    if( FAILED( pVB->Lock( 0, 0, (VOID**)&pBufferSRC, 0 ) ) )
	{
		SAFE_RELEASE( pTempMesh );
		return E_FAIL;
	}

	pTempMesh->LockVertexBuffer( 0L, (VOID**)&pBuffer );
	memcpy ( pBuffer, pBufferSRC, nSIZE*dwVert );
	pTempMesh->UnlockVertexBuffer();
    pVB->Unlock();

    if( FAILED( pIB->Lock( 0, 0, (VOID**)&pBufferSRC, 0 ) ) )
	{
		SAFE_RELEASE( pTempMesh );
		return E_FAIL;
	}

	pTempMesh->LockIndexBuffer ( 0L, (VOID**)&pBuffer );
    memcpy ( pBuffer, pBufferSRC, sizeof(WORD)*dwFace*3 );
	pTempMesh->UnlockIndexBuffer();
    pIB->Unlock();

    // 
    CreateColorMesh( pd3dDevice, pTempMesh );

    SAFE_RELEASE( pTempMesh );

    return S_OK;
}

HRESULT DxTangentMesh::CreateColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVertices, DWORD dwFVF, DWORD dwVert, DWORD dwFace )
{
    LPD3DXMESH pTempMesh;
	D3DXCreateMeshFVF ( dwFace, dwVert, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pTempMesh );
	if ( !pTempMesh )
    {
        ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - CreateColorMesh가 만들어 지지 않았습니다." );
        return S_OK;
    }

	UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

	BYTE *pBuffer(NULL);
	pTempMesh->LockVertexBuffer( 0L, (VOID**)&pBuffer );
	memcpy ( pBuffer, pVertices, nSIZE*dwVert );
	pTempMesh->UnlockVertexBuffer();

    WORD* pIndices;
	pTempMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;
	pTempMesh->UnlockIndexBuffer();

    // 
    CreateColorMesh( pd3dDevice, pTempMesh );

    SAFE_RELEASE( pTempMesh );

    return S_OK;
}

HRESULT DxTangentMesh::CreateColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh )
{
    BOOL bHaveDiffuse(FALSE);
    if ( pSrcMesh->GetFVF() & D3DFVF_DIFFUSE )
        bHaveDiffuse = TRUE;

    LPD3DXMESH pTempMesh;
    if ( FAILED( pSrcMesh->CloneMeshFVF( pSrcMesh->GetOptions(), vm::VERTEXNORCOLORTEX::FVF,
										pd3dDevice, &pTempMesh ) ) )
    {
        ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - CreateColorMesh가 만들어 지지 않았습니다." );
		return E_FAIL;
    }

    NS_VM_MANAGER::CreateTangentColorMesh( pd3dDevice, &pTempMesh );
    NS_VM_MANAGER::CompressTangentColorMesh( pd3dDevice, &pTempMesh, bHaveDiffuse );

    CreateVBIBATT( pd3dDevice, pTempMesh, sizeof(vm::VERTEXNORCOLORTEXTANGENT_COMPRESS), vm::VERTEXNORCOLORTEXTANGENT_COMPRESS::FVF );

    SAFE_RELEASE( pTempMesh );

    return S_OK;
};

HRESULT DxTangentMesh::CreateVBIBATT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh, DWORD dwVertexSize, DWORD dwFVF )
{
    HRESULT hr;
	{
		m_dwVert = pSrcMesh->GetNumVertices();

		SAFE_RELEASE( m_pVB );
		hr = pd3dDevice->CreateVertexBuffer ( m_dwVert*dwVertexSize, D3DUSAGE_DYNAMIC,
										dwFVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
		if( FAILED(hr) )
		{
            ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - IB가 만들어 지지 않았습니다." );
			return E_FAIL;
		}

		vm::VERTEX *pSrc, *Dest;
		hr = pSrcMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
		if ( FAILED(hr) )
		{
			return E_FAIL;
		}

		hr = m_pVB->Lock( 0, 0, (VOID**)&Dest, 0L );
		if ( FAILED(hr) )
		{
			pSrcMesh->UnlockVertexBuffer();
			return E_FAIL;
		}

		memcpy( Dest, pSrc, m_dwVert*dwVertexSize );
		m_pVB->Unlock();
		pSrcMesh->UnlockVertexBuffer();
	}

	{
		m_dwFace = pSrcMesh->GetNumFaces();

		SAFE_RELEASE( m_pIB );
		hr = pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L,
										D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
		if ( FAILED(hr) )
		{
            ic::DebugSetInterface::GetInstance()->ToLogFile( "DxTangentMesh - IB가 만들어 지지 않았습니다." );
			return E_FAIL;
		}

		WORD *pSrc, *Dest;
		hr = pSrcMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
		if ( FAILED(hr) )
		{
			return E_FAIL;
		}

		hr = m_pIB->Lock( 0, 0, (VOID**)&Dest, 0L );
		if ( FAILED(hr) )
		{
			pSrcMesh->UnlockIndexBuffer();
			return E_FAIL;
		}

		memcpy( Dest, pSrc, m_dwFace*3*sizeof(WORD) );
		m_pIB->Unlock();
		pSrcMesh->UnlockIndexBuffer();
	}

	{
		pSrcMesh->GetAttributeTable( NULL, &m_dwAttrib );

		SAFE_DELETE_ARRAY( m_pAttrib );
		m_pAttrib = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
		pSrcMesh->GetAttributeTable( m_pAttrib, &m_dwAttrib );
	}

    return S_OK;
};
