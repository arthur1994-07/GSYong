#include "pch.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../../RanThirdParty/DirectX/Include/dxfile.h"
#include "../Common/StlFunctions.h"
#include "../Common/ByteStream.h"
#include "../DxTools/NsLoadingLagTester.h"
#include "../DxTools/COLLISION.h"
#include "../DxTools/DxRenderStates.h"
#include "../DxTools/PrivateResource.h"

#include "./DxSimpleMeshMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------[DxSimMesh]
//
DxSimMesh::DxSimMesh () :
	m_dwRef(0),
	m_dwFlag(NULL),

	m_vCenter(0,0,0),
	m_fRadius(0),

	m_vMax(0,0,0),
	m_vMin(0,0,0),

	m_pd3dDevice(NULL),
	m_pLocalMesh(NULL),
	m_pBlendMesh(NULL),
	m_dwNumMaterials(0L),
	m_pMaterials(NULL),
	m_pTextureRes(NULL),
	m_emTexType(NULL),
	m_bUseMaterials(TRUE)
{
	m_pVertex[0] = NULL;
	m_pVertex[1] = NULL;
	m_pVertex[2] = NULL;

	m_pNormal[0] = NULL;
	m_pNormal[1] = NULL;
	m_pNormal[2] = NULL;
}

DxSimMesh::~DxSimMesh()
{
	Destroy();

    SAFE_DELETE_ARRAY ( m_pVertex[0] );
	SAFE_DELETE_ARRAY ( m_pVertex[1] );
	SAFE_DELETE_ARRAY ( m_pVertex[2] );

	SAFE_DELETE_ARRAY ( m_pNormal[0] );
	SAFE_DELETE_ARRAY ( m_pNormal[1] );
	SAFE_DELETE_ARRAY ( m_pNormal[2] );
}

HRESULT DxSimMesh::Create ( LPDIRECT3DDEVICEQ pd3dDevice,
						   const TSTRING& strName,
						   void* pBuf,
						   UINT bufSize, 
						   BOOL bTexThread,
						   TEXTURE_RESIZE uiResizeLevel,
						   TEXTURE_CATEGORY emCategory )
{
	char			szPath[MAX_PATH] = "";
	LPD3DXBUFFER	pAdjacencyBuffer = NULL;
	LPD3DXBUFFER	pMtrlBuffer = NULL;
	HRESULT			hr;
	DWORD			dwFVF( VERTEX::FVF );

	UINT nVERSIZE = D3DXGetFVFVertexSize ( VERTEX::FVF );

	m_pd3dDevice = pd3dDevice;
	m_strFileName = strName;

	{
		// Load the mesh
		if (FAILED(hr = D3DXLoadMeshFromXInMemory(
			pBuf,
			bufSize,
			D3DXMESH_MANAGED,
			pd3dDevice, 
			&pAdjacencyBuffer,
			&pMtrlBuffer,
			NULL,
			&m_dwNumMaterials,
			&m_pLocalMesh)))
		{
			std::string ErrorMsg(
				sc::string::format(
				"D3DXLoadMeshFromXInMemory() %1%, hr: %2%",
				strName,
				DXGetErrorString9(hr)));
			CDebugSet::ToLogFile(ErrorMsg);
			return hr;
		}

		// Optimize the mesh for performance
		if (FAILED(hr = m_pLocalMesh->OptimizeInplace(
			D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
			(DWORD*) pAdjacencyBuffer->GetBufferPointer(),
			NULL,
			NULL,
			NULL)))
		{
			SAFE_RELEASE( pAdjacencyBuffer );
			SAFE_RELEASE( pMtrlBuffer );
			std::string ErrorMsg(
				sc::string::format(
				"OptimizeInplace() %1%, hr: %2%",
				strName,
				DXGetErrorString9(hr)));
			CDebugSet::ToLogFile(ErrorMsg);
			return hr;
		}
	}

	// Set FVF
	SetFVF(dwFVF);

	// Get material info for the mesh
	// Get the array of materials out of the buffer
	if ( pMtrlBuffer && m_dwNumMaterials > 0 )
	{
		// Allocate memory for the materials and textures
		D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*) pMtrlBuffer->GetBufferPointer();
		if (d3dxMtrls)
		{
			m_pMaterials = new D3DXMATERIAL[m_dwNumMaterials];
			m_pTextureRes  = new TextureResource[m_dwNumMaterials];
			m_emTexType  = new TEXTURE_ALPHA[m_dwNumMaterials];

			// Copy each material and create its texture
			for (DWORD i=0; i<m_dwNumMaterials; ++i)
			{
				// Copy the material
				if (d3dxMtrls[i].pTextureFilename)
				{
					size_t nStrLen = strlen(d3dxMtrls[i].pTextureFilename)+1;
					m_pMaterials[i].pTextureFilename = new char[nStrLen];
					StringCchCopyA(m_pMaterials[i].pTextureFilename, nStrLen, d3dxMtrls[i].pTextureFilename);
				}
				else
				{
					m_pMaterials[i].pTextureFilename = NULL;
				}
				m_pMaterials[i].MatD3D			= d3dxMtrls[i].MatD3D;
				m_pMaterials[i].MatD3D.Ambient	= m_pMaterials[i].MatD3D.Diffuse;
				m_emTexType[i]					= EMTT_NORMAL;	// Default

				// Create a texture
				if ( d3dxMtrls[i].pTextureFilename )
				{
					m_pTextureRes[i] = TextureManager::GetInstance().LoadTexture(
						d3dxMtrls[i].pTextureFilename,
						false,
						TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
						EMMM_CHAR_MIPMAP,
						D3DFMT_UNKNOWN,
						TEXTURE_RESIZE_NONE,
						TEXTURE_SIMPLEMESH,
						bTexThread ? true : false,
						TRUE );

					if( m_pTextureRes[i].GetTexture() )
						m_emTexType[i] = m_pTextureRes[i].GetAlphaType();
				}
			}
		}
	}

	SAFE_RELEASE( pAdjacencyBuffer );
	SAFE_RELEASE( pMtrlBuffer );

	return S_OK;
}

HRESULT DxSimMesh::SetBlendMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxSimMesh* pSimMesh_0, DxSimMesh* pSimMesh_1 )
{
	if ( !m_pLocalMesh )
		return S_OK;

	if ( !pSimMesh_0->m_pLocalMesh )
		return S_OK;

	if ( !pSimMesh_1->m_pLocalMesh )
		return S_OK;

	// Morph 작업이 되어 있어서 또 작업할 필요가 없다.
	if ( m_pBlendMesh )
		return S_OK;

	m_pLocalMesh->CloneMeshFVF( m_pLocalMesh->GetOptions(), m_pLocalMesh->GetFVF(), pd3dDevice, &m_pBlendMesh );

	HRESULT hr(S_OK);

	DWORD dwVERTICES[3] = { 0, 0, 0 };
	LPD3DXMESH pLOCALMESH[3] = { NULL, NULL, NULL };

	UINT nVERSIZE = D3DXGetFVFVertexSize ( VERTEX::FVF );

	for ( DWORD i=0; i<3; ++i )
	{
		if ( i==0 )
		{
			pLOCALMESH[i] = m_pLocalMesh;
		}
		else if ( i==1 )
		{
			pLOCALMESH[i] = pSimMesh_0->m_pLocalMesh;
		}
		else if ( i==2 )
		{
			pLOCALMESH[i] = pSimMesh_1->m_pLocalMesh;
		}

		dwVERTICES[i] = pLOCALMESH[i]->GetNumVertices();
		m_pVertex[i] = new D3DXVECTOR3[dwVERTICES[i]];

		BYTE *pBuffer(NULL);
		pLOCALMESH[i]->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
		for ( DWORD n=0; n<dwVERTICES[i]; ++n )
		{
			m_pVertex[i][n] = *(D3DXVECTOR3*)( pBuffer + nVERSIZE*n );
		}
		pLOCALMESH[i]->UnlockVertexBuffer ();

		//if ( dwFVF&D3DFVF_NORMAL )
		{
			m_pNormal[i] = new D3DXVECTOR3[dwVERTICES[i]];

			BYTE *pBuffer(NULL);
			pLOCALMESH[i]->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
			for ( DWORD n=0; n<dwVERTICES[i]; ++n )
			{
				m_pNormal[i][n] = *(D3DXVECTOR3*)( pBuffer + nVERSIZE*n + sizeof(D3DXVECTOR3) );
			}
			pLOCALMESH[i]->UnlockVertexBuffer ();
		}
	}

	// 점 갯수가 안 맞을 경우에
	if ( dwVERTICES[0]!=dwVERTICES[1] || dwVERTICES[0]!=dwVERTICES[2] )
	{
		ReleaseBlendMesh();

		MessageBox ( NULL, "Mesh file vertex not same number.", m_strFileName.c_str(), MB_OK );
	}

	return S_OK;
}

void DxSimMesh::ReleaseBlendMesh()
{
	if ( !m_pBlendMesh )
		return;

	SAFE_RELEASE( m_pBlendMesh );

	SAFE_DELETE_ARRAY ( m_pVertex[0] );
	SAFE_DELETE_ARRAY ( m_pVertex[1] );
	SAFE_DELETE_ARRAY ( m_pVertex[2] );

	SAFE_DELETE_ARRAY ( m_pNormal[0] );
	SAFE_DELETE_ARRAY ( m_pNormal[1] );
	SAFE_DELETE_ARRAY ( m_pNormal[2] );
}

HRESULT DxSimMesh::GetBoundSphere ( D3DXVECTOR3 &vCenter, float &fRadius )
{
	HRESULT hr = S_OK;

	if ( !m_pLocalMesh )
		return S_OK;
	
	if ( m_dwFlag&EM_BOUNDSPHERE )
	{
		 vCenter = m_vCenter;
		 fRadius = m_fRadius;

		return S_OK;
	}

	DWORD dwFVF = m_pLocalMesh->GetFVF ();
	DWORD dwVertices = m_pLocalMesh->GetNumVertices ();

	BYTE *pbData;
	hr = m_pLocalMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
	if ( FAILED(hr) )	return hr;

	hr = D3DXComputeBoundingSphere ( (D3DXVECTOR3*)pbData, dwVertices, dwFVF, &m_vCenter, &m_fRadius );
	if ( FAILED(hr) )
	{
		m_pLocalMesh->UnlockVertexBuffer ();
		return hr;
	}

	m_pLocalMesh->UnlockVertexBuffer ();

	m_dwFlag |= EM_BOUNDSPHERE;
	vCenter = m_vCenter;
	fRadius = m_fRadius;

	 return S_OK;
}

HRESULT DxSimMesh::GetBoundBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr = S_OK;

	if ( !m_pLocalMesh )
		return S_OK;
	
	if ( m_dwFlag&EM_BOUNDBOX )
	{
		vMax = m_vMax;
		vMin = m_vMin;

		return S_OK;
	}

	DWORD dwFVF = m_pLocalMesh->GetFVF ();
	DWORD dwStride = D3DXGetFVFVertexSize ( dwFVF );
	DWORD dwVertices = m_pLocalMesh->GetNumVertices ();

	BYTE *pbData;
	hr = m_pLocalMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
	if ( FAILED(hr) )	return hr;

	D3DXMATRIX matIdent;
	D3DXMatrixIdentity ( &matIdent );
	hr = COLLISION::CalculateBoundingBox ( &matIdent, vMax, vMin, pbData, dwVertices, dwFVF );
	if ( FAILED(hr) )
	{
		m_pLocalMesh->UnlockVertexBuffer ();
		return hr;
	}

	m_pLocalMesh->UnlockVertexBuffer ();

	m_dwFlag |= EM_BOUNDBOX;
	m_vMax = vMax;
	m_vMin = vMin;

	 return S_OK;
}

HRESULT DxSimMesh::SetFVF ( DWORD dwFVF )
{
	if ( !m_pd3dDevice )	return E_FAIL;
	if ( !m_pLocalMesh )	return E_FAIL;
	if ( m_pLocalMesh->GetFVF() == dwFVF )	return S_OK;

	LPD3DXMESH pTempLocalMesh  = NULL;
	if( FAILED( m_pLocalMesh->CloneMeshFVF ( D3DXMESH_MANAGED, dwFVF, m_pd3dDevice,
											&pTempLocalMesh ) ) )
	{
		SAFE_RELEASE( pTempLocalMesh );
		return E_FAIL;
	}

	SAFE_RELEASE( m_pLocalMesh );

	if ( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

	//// Compute normals in case the meshes have them
	//if ( dwFVF & D3DFVF_NORMAL )
	//{
	//	if( m_pLocalMesh )
	//	{
	//		D3DXComputeNormals ( m_pLocalMesh, NULL );
	//	}
	//}

	return S_OK;
}

HRESULT DxSimMesh::Destroy()
{
	for ( UINT i=0; i<m_dwNumMaterials; ++i )
	{
		m_pTextureRes[i].Clear();
		SAFE_DELETE_ARRAY( m_pMaterials[i].pTextureFilename );
	}

	SAFE_DELETE_ARRAY( m_pTextureRes );
	SAFE_DELETE_ARRAY( m_pMaterials );
	SAFE_DELETE_ARRAY( m_emTexType );

	SAFE_RELEASE( m_pLocalMesh );
	SAFE_RELEASE( m_pBlendMesh );

	m_dwNumMaterials = 0L;

	return S_OK;
}

HRESULT DxSimMesh::FrameMove ( float fTime )
{
	if ( !m_pBlendMesh ) 
		return S_OK;

	float	fWeight1, fWeight2, fWeight3;

	if ( fTime >= 0.f )
	{
		fWeight1 = fabsf ( fTime );
		fWeight2 = 1.f - fabsf ( fTime );
		fWeight3 = 0.f;
	}
	if ( fTime < 0.f )
	{
		fWeight1 = 0.f;
		fWeight2 = 1.f - fabsf ( fTime );
		fWeight3 = fabsf ( fTime );
	}

	DWORD	dwFVF		= m_pBlendMesh->GetFVF();
	DWORD	dwVertices	= m_pBlendMesh->GetNumVertices();
	UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF );

	D3DXVECTOR3*	pVecter(NULL);
	D3DXVECTOR3*	pNormal(NULL);
	BYTE*			pBuffer(NULL);
	m_pBlendMesh->LockVertexBuffer ( 0L, (VOID**)&pBuffer );

    if ( dwFVF & D3DFVF_NORMAL )
    {
        for ( DWORD i=0; i<dwVertices; ++i )
	    {
 		    pVecter = (D3DXVECTOR3*)( pBuffer );
		    *pVecter = m_pVertex[0][i]*fWeight1 + m_pVertex[1][i]*fWeight2 + m_pVertex[2][i]*fWeight3;

            pNormal = (D3DXVECTOR3*)( pBuffer + sizeof(D3DXVECTOR3) );
			*pNormal = *pVecter;

            pBuffer += nVERSIZE;
	    }
    }
    else
    {
        for ( DWORD i=0; i<dwVertices; ++i )
	    {
 		    pVecter = (D3DXVECTOR3*)( pBuffer );
		    *pVecter = m_pVertex[0][i]*fWeight1 + m_pVertex[1][i]*fWeight2 + m_pVertex[2][i]*fWeight3;

            pBuffer += nVERSIZE;
	    }
    }

	m_pBlendMesh->UnlockVertexBuffer ();

	return S_OK;
}

HRESULT DxSimMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture, D3DMATERIALQ* pMaterials, BOOL bBlendMesh )
{
	if ( m_pBlendMesh && bBlendMesh )
	{
		return Render ( pd3dDevice, pTexture, pMaterials, m_pBlendMesh );
	}
	else
	{
		return Render ( pd3dDevice, pTexture, pMaterials, m_pLocalMesh );
	}
}

HRESULT DxSimMesh::RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials )
{
	return RenderItem ( pd3dDevice, matWld, pMaterials, m_pLocalMesh );
}

HRESULT DxSimMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture, D3DMATERIALQ* pMaterials, LPD3DXMESH	pLocalMesh )
{
	if ( NULL == pLocalMesh )		
		return E_FAIL;

	for ( DWORD i=0; i<m_dwNumMaterials; ++i )
	{
		if ( m_bUseMaterials )
		{
			if ( pMaterials )
			{
				D3DMATERIALQ	matMaterial;

				matMaterial.Diffuse.a = m_pMaterials[i].MatD3D.Diffuse.a * pMaterials->Diffuse.a;
				matMaterial.Diffuse.r = m_pMaterials[i].MatD3D.Diffuse.r * pMaterials->Diffuse.r;
				matMaterial.Diffuse.g = m_pMaterials[i].MatD3D.Diffuse.g * pMaterials->Diffuse.g;
				matMaterial.Diffuse.b = m_pMaterials[i].MatD3D.Diffuse.b * pMaterials->Diffuse.b;

				matMaterial.Ambient.a = m_pMaterials[i].MatD3D.Ambient.a * pMaterials->Ambient.a;
				matMaterial.Ambient.r = m_pMaterials[i].MatD3D.Ambient.r * pMaterials->Ambient.r;
				matMaterial.Ambient.g = m_pMaterials[i].MatD3D.Ambient.g * pMaterials->Ambient.g;
				matMaterial.Ambient.b = m_pMaterials[i].MatD3D.Ambient.b * pMaterials->Ambient.b;

				matMaterial.Emissive.a = m_pMaterials[i].MatD3D.Emissive.a * pMaterials->Emissive.a;
				matMaterial.Emissive.r = m_pMaterials[i].MatD3D.Emissive.r * pMaterials->Emissive.r;
				matMaterial.Emissive.g = m_pMaterials[i].MatD3D.Emissive.g * pMaterials->Emissive.g;
				matMaterial.Emissive.b = m_pMaterials[i].MatD3D.Emissive.b * pMaterials->Emissive.b;

				matMaterial.Specular.a = m_pMaterials[i].MatD3D.Specular.a * pMaterials->Specular.a;
				matMaterial.Specular.r = m_pMaterials[i].MatD3D.Specular.r * pMaterials->Specular.r;
				matMaterial.Specular.g = m_pMaterials[i].MatD3D.Specular.g * pMaterials->Specular.g;
				matMaterial.Specular.b = m_pMaterials[i].MatD3D.Specular.b * pMaterials->Specular.b;

				matMaterial.Power = m_pMaterials[i].MatD3D.Power;

				pd3dDevice->SetMaterial ( &matMaterial );
			}
			else
			{
				pd3dDevice->SetMaterial ( &m_pMaterials[i].MatD3D );
			}

			if ( pTexture )	pd3dDevice->SetTexture ( 0, pTexture );
			else			pd3dDevice->SetTexture ( 0, m_pTextureRes[i].GetTexture() );
		}

		DWORD dwColorOP, dwAlphaOP;
		if ( !pTexture && !m_pTextureRes[i].GetTexture() )
		{
			pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP, &dwColorOP );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );

			pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAOP, &dwAlphaOP );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
		}

        if( m_pMaterials[i].pTextureFilename && 
			m_pMaterials[i].pTextureFilename[0]!='\0' && 
			!pTexture && 
			!m_pTextureRes[i].GetTexture() )	
		{
		}
		else
		{
			pLocalMesh->DrawSubset(i);
		}

		if ( !pTexture && !m_pTextureRes[i].GetTexture() )
		{
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, dwColorOP );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, dwAlphaOP );
		}

        if ( m_bUseMaterials && pMaterials )
        {
            D3DMATERIALQ	matMaterial;

            D3DUtil_InitMaterial( matMaterial, 1.f, 1.f, 1.f, 1.f );
            pd3dDevice->SetMaterial ( &matMaterial );
        }        
	}

	return S_OK;
}

HRESULT DxSimMesh::RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials, LPD3DXMESH pLocalMesh )
{
	if ( NULL == pLocalMesh )
        return E_FAIL;

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWld );

	for ( DWORD i=0; i<m_dwNumMaterials; ++i )
	{
		if ( m_bUseMaterials )
		{
			if ( !pMaterials )
                pd3dDevice->SetMaterial ( &m_pMaterials[i].MatD3D );
			else
                pd3dDevice->SetMaterial ( pMaterials );

			if( !m_pTextureRes[i].IsValid() )
			{
                if( m_pTextureRes[i].GetTexture() )
					m_emTexType[i] = m_pTextureRes[i].GetAlphaType();
			}

			pd3dDevice->SetTexture ( 0, m_pTextureRes[i].GetTexture() );
		}

		if ( m_bUseMaterials )
		{
			if( (m_emTexType[i]==EMTT_ALPHA_HARD) || (m_emTexType[i]==EMTT_ALPHA_SOFT) ||
				(m_emTexType[i]==EMTT_ALPHA_SOFT01) || (m_emTexType[i]==EMTT_ALPHA_SOFT02))
			{
				DxRenderStates::GetInstance().SetAlphaMap( pd3dDevice );
			}
		}

		pLocalMesh->DrawSubset(i);

		if ( m_bUseMaterials )
		{
			if( (m_emTexType[i]==EMTT_ALPHA_HARD) || (m_emTexType[i]==EMTT_ALPHA_SOFT) ||
				(m_emTexType[i]==EMTT_ALPHA_SOFT01) || (m_emTexType[i]==EMTT_ALPHA_SOFT02))
			{
				DxRenderStates::GetInstance().ReSetAlphaMap( pd3dDevice );
			}
		}
	}

	return S_OK;
}

void DxSimMesh::SaveFile()
{
	CString strNewName;
	std::string strFullName( DxSimpleMeshMan::GetInstance().GetPath() );
	STRUTIL::ChangeExt( m_strFileName.c_str(), strNewName, _T(".mobj") );
	strFullName += strNewName.GetString();

	sc::SerialFile	SFile;
	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFullName.c_str() );
	if ( !bOpened )
		return;

	SFile << static_cast<DWORD>(0x100);
	SFile.BeginBlock();
	{
		DWORD dwFVF = m_pLocalMesh->GetFVF ();
		DWORD dwVertices = m_pLocalMesh->GetNumVertices ();
		DWORD dwFaceNUM = m_pLocalMesh->GetNumFaces();

		SFile << dwFVF;
		SFile << dwVertices;
		SFile << dwFaceNUM;

		VERTEX *pbData(NULL);
		HRESULT hr = m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pbData );
		if ( FAILED(hr) )
		{
			SFile.EndBlock();
			return;
		}

		WORD *pIndexBuffer(NULL);
		m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			m_pLocalMesh->UnlockVertexBuffer ();
			SFile.EndBlock();
			return;
		}

		SFile.WriteBuffer( pbData, sizeof(VERTEX)*dwVertices );
		SFile.WriteBuffer( pIndexBuffer, sizeof(WORD)*3*dwFaceNUM );

		m_pLocalMesh->UnlockIndexBuffer();
		m_pLocalMesh->UnlockVertexBuffer ();

		// 텍스쳐는 하나만 로딩하도록 한다.
		SFile << m_dwNumMaterials;

		for ( DWORD i=0; i<m_dwNumMaterials; ++i )
		{
			if ( m_pMaterials[i].pTextureFilename )
			{
				SFile << std::string( m_pMaterials[i].pTextureFilename );
			}
			else
			{
				SFile << std::string( "" );
			}
		}
	}
	SFile.EndBlock();
}

//BOOL DxSimMesh::IsCollision( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd ) const
//{
//	if( !m_pLocalMesh )		return FALSE;
//
//	D3DXVECTOR3 vFromPt = vStart;
//
//	DWORD dwFVF = m_pLocalMesh->GetFVF ();
//	UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF );
//
//	BOOL bUSE(FALSE);
//	D3DXVECTOR3 vCollison;
//	BYTE *pBuffer(NULL);
//	D3DXVECTOR3* pVecter[3];
//	WORD wIndex[3];
//	WORD *pIndexBuffer(NULL);
//	DWORD dwFaceNUM = m_pLocalMesh->GetNumFaces();
//	m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
//	m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndexBuffer );
//	for ( DWORD n=0; n<dwFaceNUM; ++n )
//	{
//		wIndex[0] = pIndexBuffer[n*3+0];
//		wIndex[1] = pIndexBuffer[n*3+1];
//		wIndex[2] = pIndexBuffer[n*3+2];
//
//		pVecter[0] = (D3DXVECTOR3*)( pBuffer + nVERSIZE*wIndex[0] );
//		pVecter[1] = (D3DXVECTOR3*)( pBuffer + nVERSIZE*wIndex[1] );
//		pVecter[2] = (D3DXVECTOR3*)( pBuffer + nVERSIZE*wIndex[2] );
//
//		//m_pVertex[0][wIndex[1]];
//		//m_pVertex[0][wIndex[2]];
//
//		if( COLLISION::IsLineTriangleCollision
//			( 
//				pVecter[0], 
//				pVecter[1], 
//				pVecter[2],
//				&vFromPt, 
//				&vEnd,
//				&vCollison,
//				NULL,
//				EMCC_CULL_NONE,
//				TRUE
//			)
//		)
//		{
//			vEnd = vCollison;
//			bUSE = TRUE;
//		}
//	}
//	m_pLocalMesh->UnlockVertexBuffer();
//	m_pLocalMesh->UnlockIndexBuffer();
//
//	return bUSE;
//}

//-----------------------------------------------------------------------------[DxSimpleMeshMan]
//
DxSimpleMeshMan& DxSimpleMeshMan::GetInstance()
{
	static DxSimpleMeshMan Instance;
	return Instance;
}

DxSimpleMeshMan::DxSimpleMeshMan(void) :
	m_pd3dDevice(NULL)
{
}

void DxSimpleMeshMan::SetPath(const TSTRING& path)
{ 
	m_path = path; 
	m_sFileTree.CreateTree( path );
}
//
//void DxSimpleMeshMan::ConvertFileName( char* szOut, int nStrLen, const char* szFile, DWORD dwFVF )
//{
//	char sz0 = '0';
//	char sz1 = '0';
//	char sz2 = '0';
//	char sz3 = '0';
//
//	if ( dwFVF&D3DFVF_XYZ )		sz0 += 1;
//	if ( dwFVF&D3DFVF_NORMAL )	sz1 += 1;
//	if ( dwFVF&D3DFVF_DIFFUSE )	sz2 += 1;
//	if ( dwFVF&D3DFVF_TEX1 )	sz3 += 1;
//
//	std::string szString = "";
//	szString += sz0;
//	szString += sz1;
//	szString += sz2;
//	szString += sz3;
//
//	StringCchCopy( szOut, nStrLen, szFile );
//	StringCchCat( szOut, nStrLen, "." );
//	StringCchCat( szOut, nStrLen, szString.c_str() );
//}

DxSimMeshResource DxSimpleMeshMan::Load (
    const TSTRING& strName,
	BOOL bMeshThread,
    BOOL bTexThread,
    TEXTURE_RESIZE uiResizeLevel,
    TEXTURE_CATEGORY emCategory )
{
	if ( strName.empty() )
		return DxSimMeshResource();

	CString strFileName(strName.c_str());

	// mxf 가 존재 한다면 그것으로 이름을 바꾼다.
	{
		CString strEncName;
		STRUTIL::ChangeExt( strFileName, strEncName, _T( ".mxf" ) );

		if ( ExistFile( TSTRING(strEncName) ) )
		{
			strFileName = strEncName;
		}
	}

	SIMPLEMESH_ADD_DATA sAddData(bTexThread,uiResizeLevel,emCategory);
	return LoadData( GetPath() + strFileName.GetString(), false, sAddData, bMeshThread?true:false );


	//HRESULT hr;
	//if( !szFile )
	//{
	//	CDebugSet::ToListView( "DxSimpleMeshMan::Load() -- szFile -- NULL" );
	//	return NULL;
	//}

	//char szConvertName[MAX_PATH] = "";
	//ConvertFileName( szConvertName, MAX_PATH, szFile, dwFVF );

	//DxSimMesh* pMesh = NULL;
	//pMesh = Find ( szConvertName );
	//if ( pMesh ) 
	//{
	//	pMesh->m_dwRef++;
 //       //pMesh->SetRemindData( m_pd3dDevice, bTexThread, uiResizeLevel, emCategory );
	//	return pMesh;
	//}

	//pMesh = new DxSimMesh;
	//hr = pMesh->Create(
 //       pd3dDevice,
 //       dwFVF,
 //       szFile,
 //       "",
 //       "",
 //       bTexThread,
	//	uiResizeLevel,
 //       emCategory );
	//if ( FAILED(hr) )
	//{
	//	SAFE_DELETE(pMesh);
	//	CDebugSet::ToListView( "DxSimpleMeshMan::Load() -- Create() -- NULL" );
	//	return NULL;
	//}

	////	Note : 리스트에 삽입.
	////
	//m_MeshMap.insert ( std::make_pair(szConvertName,pMesh) );

	//pMesh->m_dwRef++;
	//return pMesh;
}

void DxSimpleMeshMan::ProcessCompleteData(
	SP_QUEUE_DATA_VECTOR& dataVec,
	NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
	DWORD dwPrevTick, 
	DWORD nWorkTime )
{
	SP_QUEUE_DATA_VECTOR::iterator it = dataVec.begin();
	while( it != dataVec.end() )
	{
		DWORD dwTick( timeGetTime() );

		const SP_QUEUE_DATA& spResData = *it;
		++it;

		MIN_ASSERT(!spResData->m_fileName.empty());
		MIN_ASSERT(spResData->m_spRawData != NULL);
		MIN_ASSERT(m_pd3dDevice);

		SP_COMPLETED_DATA spSimMesh(new DxSimMesh);
		spSimMesh->Create
		( 
			m_pd3dDevice, 
			spResData->m_fileName,
			spResData->m_spRawData.get(),
			spResData->m_rawDataSize, 
			spResData->m_addData.m_bTexThread,
			spResData->m_addData.m_uiResizeLevel,
			spResData->m_addData.m_emCategory
		);

		pCompletedDataVec->push_back( std::make_pair(spResData->m_fileName,spSimMesh) );

#ifdef _RELEASED
		if ( NSLoadingLagTester::g_DxSimpleMeshMan_ProcessCompleteData < timeGetTime() - dwTick )
		{
			NSLoadingLagTester::g_DxSimpleMeshMan_ProcessCompleteData = timeGetTime() - dwTick;
			NSLoadingLagTester::g_strDxSimpleMeshMan_ProcessCompleteData = spResData->m_fileName;
		}
#endif

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxSimpleMeshMan Load %1% - %2%", spResData->m_fileName, dwTick ) );

		if ( timeGetTime() - dwPrevTick > nWorkTime )
		{
			break;
		}
	}

	if( !dataVec.empty() )
		dataVec.erase( dataVec.begin(), it );
}

//
//DxSimMesh*	DxSimpleMeshMan::BlendLoad (
//    LPDIRECT3DDEVICEQ pd3dDevice,
//    char** szFile,
//    DWORD dwFVF )
//{
//	HRESULT hr;
//	if( !szFile )
//	{
//		CDebugSet::ToListView( "DxSimpleMeshMan::BlendLoad() -- szFile -- NULL" );
//		return NULL;
//	}
//
//	char szConvertName[MAX_PATH] = "";
//	ConvertFileName( szConvertName, MAX_PATH, szFile[0], dwFVF );
//
//	DxSimMesh* pMesh = NULL;
//	pMesh = Find ( szConvertName, TRUE );
//	if ( pMesh )
//	{
//		pMesh->m_dwRef++;
// //       pMesh->SetRemindData( m_pd3dDevice, TRUE );
//		return pMesh;
//	}
//
//	pMesh = new DxSimMesh;
//	hr = pMesh->Create(
//        pd3dDevice,
//        dwFVF,
//        szFile[0],
//        szFile[1],
//        szFile[2] );
//	if (FAILED(hr))
//	{
//        std::string ErrorMsg(
//            sc::string::format(
//                "DxSimpleMeshMan::BlendLoad() -- %1%, %2%, %3%",
//                szFile[0],
//                szFile[1],
//                szFile[2]));
//		CDebugSet::ToLogFile(ErrorMsg);
//		SAFE_DELETE(pMesh);
//		return NULL;
//	}
//
//	//	Note : 리스트에 삽입.
//	//
//	m_BlendMeshMap.insert ( std::make_pair(szConvertName,pMesh) );
//
//	pMesh->m_dwRef++;
//	return pMesh;
//}
//
//HRESULT DxSimpleMeshMan::Release ( const char* szFile, DWORD dwFVF, BOOL bMorph )
//{
//	GASSERT(szFile);
//
//	char szConvertName[MAX_PATH] = "";
//	ConvertFileName( szConvertName, MAX_PATH, szFile, dwFVF );
//
//	if ( bMorph )
//	{
//		if ( m_BlendMeshMap.empty() )	return S_OK;
//
//		SIMPLEMAP_ITER iter = m_BlendMeshMap.find ( szConvertName );
//
//		if ( iter!=m_BlendMeshMap.end() )
//		{
//			DxSimMesh *pMesh = (*iter).second;
//
//			if ( --pMesh->m_dwRef == 0 )
//			{
//				SAFE_DELETE(pMesh);
//				m_BlendMeshMap.erase ( iter );
//			}
//		}
//	}
//	else
//	{
//		if ( m_MeshMap.empty() )	return S_OK;
//
//		SIMPLEMAP_ITER iter = m_MeshMap.find ( szConvertName );
//
//		if ( iter!=m_MeshMap.end() )
//		{
//			DxSimMesh *pMesh = (*iter).second;
//
//			if ( --pMesh->m_dwRef == 0 )
//			{
//				SAFE_DELETE(pMesh);
//				m_MeshMap.erase ( iter );
//			}
//		}
//	}
//
//	return S_OK;
//}
//
//HRESULT DxSimpleMeshMan::Delete ( const char* szFile, DWORD dwFVF, BOOL bMorph )
//{
//	GASSERT(szFile);
//
//	char szConvertName[MAX_PATH] = "";
//	ConvertFileName( szConvertName, MAX_PATH, szFile, dwFVF );
//
//	if ( bMorph )
//	{
//		SIMPLEMAP_ITER iter = m_BlendMeshMap.find ( szConvertName );
//		if ( iter!=m_BlendMeshMap.end() )
//		{
//			DxSimMesh *pMesh = (*iter).second;
//			{
//				SAFE_DELETE(pMesh);
//				m_BlendMeshMap.erase ( iter );
//			}
//		}
//	}
//	else
//	{
//		SIMPLEMAP_ITER iter = m_MeshMap.find ( szConvertName );
//		if ( iter!=m_MeshMap.end() )
//		{
//			DxSimMesh *pMesh = (*iter).second;
//			{
//				SAFE_DELETE(pMesh);
//				m_MeshMap.erase ( iter );
//			}
//		}
//	}
//
//	return S_OK;
//}
//
//DxSimMesh* DxSimpleMeshMan::Find ( const char* szFile, BOOL bMorph )
//{
//	GASSERT(szFile);
//
//	if ( bMorph )
//	{
//		SIMPLEMAP_ITER iter = m_BlendMeshMap.find ( szFile );
//		if ( iter!=m_BlendMeshMap.end() )	return (*iter).second;
//	}
//	else
//	{
//		SIMPLEMAP_ITER iter = m_MeshMap.find ( szFile );
//		if ( iter!=m_MeshMap.end() )		return (*iter).second;
//	}
//
//	return NULL;
//}

HRESULT DxSimpleMeshMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}
//
//HRESULT DxSimpleMeshMan::DeleteDeviceObjects ()
//{
//	//std::for_each ( m_BlendMeshMap.begin(), m_BlendMeshMap.end(), std_afunc::DeleteMapObject() );
//	//std::for_each ( m_MeshMap.begin(), m_MeshMap.end(), std_afunc::DeleteMapObject() );
//
//	//m_BlendMeshMap.clear();
//	//m_MeshMap.clear();
//
//	return S_OK;
//}
//
//HRESULT DxSimpleMeshMan::CleanUp ()
//{
//	DeleteDeviceObjects ();
//	
//	return S_OK;
//}
//

BOOL DxSimpleMeshMan::ExistFile( const TSTRING& strName )
{
	TSTRING strFullPath = m_sFileTree.FindPathName( strName );
	if ( strFullPath.empty() )
		return FALSE;
	else
		return TRUE;
}