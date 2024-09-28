#include "pch.h"

#include "../../DxEffect/TexEff/DxTextureEffMan.h"
#include "../../DxEffect/Frame/DxEffectDefine.h"
#include "../../DxEffect/Frame/DxEffectTiling.h"
#include "../../DxEffect/Frame/DxEffectFrame.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxTools/Collision.h"
#include "../../DxTools/editmeshs.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/DxLandMan.h"
#include "../../DxCommon9/DXUTmisc.h"
#include "./dxreplacecontainer.h"
#include "./DxAnimationManager.h"
#include "./NSVERTEXNORTEX_FX.h"

#include "DxFrameMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DxFrameMesh::SetAnimationTimeZero()
{
	//	Note : ���ϸ��̼� ������Ʈ.
	DxAnimation* pAnimCur = pAnimHead;
	while ( pAnimCur != NULL )
	{
		pAnimCur->SetTime( 0.f );		// Note : Edit Mode
		pAnimCur = pAnimCur->pAnimNext;
	}

	// Note : Matrix �⺻ ������... 
	D3DXMATRIXA16 mCur;
	D3DXMatrixIdentity ( &mCur );
	UpdateFrames( m_pDxFrame, mCur );
}

HRESULT DxFrameMesh::FrameMovePiece ( float fTime, float fElapsedTime )
{
	HRESULT hr;

	//	Note : ���ϸ��̼� ������Ʈ.
	//
	DxAnimation* pAnimCur = pAnimHead;
	while ( pAnimCur != NULL )
	{
		pAnimCur->SetTime ( fCurTime );
		pAnimCur = pAnimCur->pAnimNext;
	}

	//	Note : ȿ�� ������Ʈ.
	//
	DxEffectBase* pEffCur = m_pEffectHead;
	while ( pEffCur != NULL )
	{
		hr = pEffCur->FrameMove ( fTime, fElapsedTime );
		if ( FAILED(hr) )	return hr;

		pEffCur = pEffCur->pEffectNext;
	}

	return S_OK;
}

HRESULT DxFrameMesh::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime = fTime;
	m_fElapsedTime = fElapsedTime,

	//	Note : ���� �ð� ������Ʈ.
	fCurTime += fElapsedTime * UNITANIKEY_PERSEC;
	if ( fCurTime > 1.0e15f )	fCurTime = 0;

	//	Note : ���ϸ��̼� ������Ʈ.
	DxAnimation* pAnimCur = pAnimHead;
	while ( pAnimCur != NULL )
	{
		pAnimCur->SetTime( fCurTime );
		pAnimCur = pAnimCur->pAnimNext;
	}

	//	Note : ȿ�� ������Ʈ.
	//
	DxEffectBase* pEffCur = m_pEffectHead;
	while ( pEffCur != NULL )
	{
		pEffCur->FrameMove ( fTime, fElapsedTime );
		pEffCur = pEffCur->pEffectNext;
	}

	if( m_pTextureEff )	m_pTextureEff->FrameMove( fElapsedTime );
	
	NSMaterialManager::SetTime( m_fTime );
	NSMaterialManager::FrameMove
	( 
		DxViewPort::GetInstance().GetMatView(), 
		DxViewPort::GetInstance().GetMatProj(), 
		DxSurfaceTex::GetInstance().m_pColorTex_1st, 
		DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
		DxSurfaceTex::GetInstance().m_pColorTex_3rd, 
		DxSurfaceTex::GetInstance().m_pColorTex_4th
	);

	return S_OK;
}

HRESULT DxFrameMesh::UpdateFrames ( DxFrame *pframeCur, D3DXMATRIX &matCur )
{
    HRESULT hr = S_OK;
    
	//	Note : pframeCur�� ��Ʈ������ ���. matCombined = matRot * matParent
    D3DXMatrixMultiply ( &pframeCur->matCombined, &pframeCur->matRot, &matCur );

	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	//
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
    while ( pframeChild != NULL )
    {
        hr = UpdateFrames ( pframeChild, pframeCur->matCombined );
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }

    return S_OK;
}

HRESULT DxFrameMesh::Render_Independence( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, D3DMATERIAL9* pMaterial, LPDIRECT3DTEXTUREQ pTexture )
{
	HRESULT hr;

	//	Note : ���� Render;
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����;
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���;
		//
		DxMeshes* pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				for ( DWORD i=0; i<pmsMeshs->m_cMaterials; i++ )
				{
					if ( pmsMeshs->m_exMaterials[i].MtrlType != MATERIAL_NORMAL )
						continue;

					//	Note : ���͸���, �ؽ��� ����;
					//  Material�� ������ ��쿡�� ������ Material�� ����Ѵ�;
					if( pMaterial )
						pd3dDevice->SetMaterial( pMaterial );
					else
						pd3dDevice->SetMaterial( &pmsMeshs->m_rgMaterials[i] );

					if( pTexture )
					{
						pd3dDevice->SetTexture( 0,
							pTexture );
					}
					else
					{
						pd3dDevice->SetTexture( 0,
							pmsMeshs->m_pTextureRes[i].GetTexture() );
					}

					pmsMeshs->m_pLocalMesh->DrawSubset( i );
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���;
	//
	DxFrame* pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = Render_Independence( pd3dDevice, pframeChild, pMaterial, pTexture );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX *pWorld, const CLIPVOLUME *pCV  )
{
	if ( !m_pDxFrame )	return S_OK;

	HRESULT hr;

	////	Note : ���� ����.
	////
	//if ( pWorld )
	//{
	//	m_pDxFrame->matRot = *pWorld;
	//}
	//else
	//{
	//	D3DXMATRIX Mat;
	//	D3DXMatrixIdentity ( &Mat );
	//	m_pDxFrame->matRot = Mat;
	//}

	//	Note : ��Ʈ���� ���.
	//
	D3DXMATRIXA16 mCur;
	D3DXMatrixIdentity ( &mCur );
	if ( pWorld )
	{
		mCur = *pWorld;
	}

	hr = UpdateFrames ( m_pDxFrame, mCur );
	if (FAILED(hr))		return hr;

	//	Note : �޽� �׸��� ����.
	//
	hr = DrawFrames ( pd3dDevice, m_pDxFrame, pCV, mCur );
	if (FAILED(hr))		return hr;

	DrawFrames_RenderAlpha( pd3dDevice, m_pDxFrame, pCV, mCur );	// HardAlpha, SoftAlpha �Ѵ� �������ȴ�.
	DrawFrames_Decal1( pd3dDevice, m_pDxFrame, pCV, mCur );
	DrawFrames_Decal2( pd3dDevice, m_pDxFrame, pCV, mCur );
	DrawFrames_Decal3( pd3dDevice, m_pDxFrame, pCV, mCur );
	RenderWaterCustomSoftAlpha( pd3dDevice, m_pDxFrame, pCV, mCur );

	////	Note : Alpha �޽� �׸��� ����.
	////
	//hr = DrawAlphaMapFrames ( pd3dDevice, pCV );
	//if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxFrameMesh::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX *pWorld, const CLIPVOLUME *pCV  )
{
	if ( !m_pDxFrame )	return S_OK;

	HRESULT hr;

	//	Note : ġȯ ���� �׸��� ����.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_NULL;
	hr = DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, pCV, emReflect, fDistance );
	if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxFrameMesh::Render_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV  )
{
	HRESULT	hr;

	if ( !m_pDxFrame )	return S_OK;

	//	Note : �޽� �׸��� ����.
	//
	hr = DrawFrames_NOEFF ( pd3dDevice, m_pDxFrame, pCV );
	if (FAILED(hr))		return hr;

	//	Note : Alpha �޽� �׸��� ����.
	//
	hr = DrawAlphaMapFrames ( pd3dDevice, pCV );
	if (FAILED(hr))		return hr;

	//	Note : ġȯ ���� �׸��� ����.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	hr = DxReplaceContainer::DrawRePlacePiece ( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, pCV, DxReplaceContainer::EM_REFLECT_OLD, fDistance );
	if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxFrameMesh::Render_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )
{
	// FrameMesh ���� �ݻ縦 ���� �ʹٸ�, Reflect �� ���ڵ�������Ѵ�.

	//HRESULT	hr;

	//if ( !m_pDxFrame )	return S_OK;

	////	Note : �޽� �׸��� ����.
	////
	//hr = DrawFrames_Reflect ( pd3dDevice, m_pDxFrame, pCV );
	//if (FAILED(hr))		return hr;

	////	Note : Alpha �޽� �׸��� ����.
	////
	//hr = DrawAlphaMapFrames ( pd3dDevice, pCV, TRUE );
	//if (FAILED(hr))		return hr;

	////	Note : ġȯ ���� �׸��� ����.
	//float fDistance = DxViewPort::GetInstance().GetDistance();
	//fDistance = fDistance*fDistance;
	//hr = DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, pCV, DxReplaceContainer::EM_REFLECT, fDistance );
	//if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxFrameMesh::RenderLightMapChecker( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur, const CLIPVOLUME *pCV )
{
	if ( !m_pDxFrame )	
		return S_OK;

	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	
		return S_OK;

	if( pframeCur->bPieceUSE )		
		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	
		return S_OK;

	if ( pframeCur->pEffect )	
	{

	}
	else
	{
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : Ʈ������ ����.
			//
			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
			if (FAILED(hr))
				return hr;

			NSLightMapFX::SetWorld( pframeCur->matCombined, 1.f );

			//	Note : �޽� �׸���.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					//if ( !pmsMeshs->RenderLightMapChecker( pd3dDevice, pframeCur->pmsMeshs->m_wwLightMapUserUV_TextureSize ) )
					{
						pmsMeshs->Render( pd3dDevice );
						pmsMeshs->RenderAlpha ( pd3dDevice, FALSE );
					}

/*					DxFrame* pFrameSRC(NULL);
					if ( pFrameMeshSRC )
					{
						pFrameSRC = pFrameMeshSRC->FindFrame( pframeCur->szName );
					}

					if ( pFrameSRC && pFrameSRC->pmsMeshs )
					{
						if ( !pmsMeshs->RenderLightMapChecker( pd3dDevice, pFrameSRC->pmsMeshs->m_wwLightMapUserUV_TextureSize ) )
						{
							pmsMeshs->Render( pd3dDevice );
							pmsMeshs->RenderAlpha ( pd3dDevice, FALSE );
						}
					}
					else
					{
						if ( !pmsMeshs->RenderLightMapCheckerInexact( pd3dDevice ) )
						{
							pmsMeshs->Render( pd3dDevice );
							pmsMeshs->RenderAlpha ( pd3dDevice, FALSE );
						}
					}	*/				
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}
		}

		//	Note : �ڽ� ������ �׸���.
		//
		pframeChild = pframeCur->pframeFirstChild;
		while ( pframeChild != NULL )
		{
			hr = RenderLightMapChecker ( pd3dDevice, pframeChild, pCV );
			if (FAILED(hr))
				return hr;

			pframeChild = pframeChild->pframeSibling;
		}
	}

	return S_OK;
}

HRESULT DxMeshes::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		if ( m_exMaterials[i].MtrlType != MATERIAL_NORMAL )		continue;

		//	Note : ���͸���, �ؽ��� ����.
		pd3dDevice->SetMaterial( &m_rgMaterials[i] );
		pd3dDevice->SetTexture( 0, m_pTextureRes[i].GetTexture() );

        NSVERTEXNORTEX_FX::Render( pd3dDevice, m_pLocalMesh, i );
		//m_pLocalMesh->DrawSubset( i );
	}

	return S_OK;
}

HRESULT DxMeshes::RenderAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow )
{
	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		DXMTRLTYPE emMETR = m_exMaterials[i].MtrlType;
		if ( emMETR==MATERIAL_NORMAL )			continue;

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().SetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().SetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().SetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().SetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
				break;
			};
		}

		//	Note : ���͸���, �ؽ��� ����.
		pd3dDevice->SetMaterial( &m_rgMaterials[i] );
		pd3dDevice->SetTexture( 0, m_pTextureRes[i].GetTexture() );
	
		//m_pLocalMesh->DrawSubset( i );
        NSVERTEXNORTEX_FX::Render( pd3dDevice, m_pLocalMesh, i );

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().ReSetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().ReSetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().ReSetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().ReSetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
				break;
			};
		}
	}

	return S_OK;
}

HRESULT DxMeshes::RenderTriangle ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD OldFillMode;
	pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	DWORD OldLighting;
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	//	Note : ���͸���, �ؽ��� ����.
	//
	D3DMATERIALQ WhiteMaterial;
	memset(&WhiteMaterial, 0, sizeof(D3DMATERIALQ));
	WhiteMaterial.Diffuse.r = 1.0f;
	WhiteMaterial.Diffuse.g = 1.0f;
	WhiteMaterial.Diffuse.b = 1.0f;
	WhiteMaterial.Diffuse.a = 1.0f;
	WhiteMaterial.Ambient = WhiteMaterial.Specular = WhiteMaterial.Diffuse;

	pd3dDevice->SetMaterial( &WhiteMaterial );
	pd3dDevice->SetTexture( 0, NULL );
	
	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		m_pLocalMesh->DrawSubset( i );
	}

	pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );

	{
		DWORD OldFillMode;
		pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		DWORD dwD3dColorOld;
		pd3dDevice->GetRenderState( D3DRS_AMBIENT, &dwD3dColorOld );

		DWORD dwD3dColor = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,0);
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColor );

		D3DLIGHTQ light, lightOld;
		pd3dDevice->GetLight ( 0, &lightOld );

		D3DUtil_InitLight ( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
		pd3dDevice->SetLight ( 0, &light );

		//	Note : ���͸���, �ؽ��� ����.
		//
		D3DMATERIALQ RedMaterial;
		memset(&RedMaterial, 0, sizeof(D3DMATERIALQ));
		RedMaterial.Diffuse.r = 1.0f;
		RedMaterial.Diffuse.g = 0.0f;
		RedMaterial.Diffuse.b = 0.0f;
		RedMaterial.Specular = RedMaterial.Diffuse;

		pd3dDevice->SetMaterial( &RedMaterial );
		pd3dDevice->SetTexture( 0, NULL );

		for ( DWORD i=0; i<m_cMaterials; i++ )
		{
			if ( m_exMaterials[i].bSubSetTriRend )
				m_pLocalMesh->DrawSubset( i );
		}

		pd3dDevice->SetMaterial( &WhiteMaterial );
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColorOld );
		pd3dDevice->SetLight ( 0, &lightOld );
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return S_OK;

	//	Note : 
	//
	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )	return S_OK;

	////	Note : ���� Render �� ����Ʈ
	////
	DxEffectBase* pEffectCur;
	//pEffectCur = pframeCur->pEffectPrev;
	//while ( pEffectCur )
	//{
	//	pEffectCur->Render ( pframeCur, pd3dDevice );
	//	pEffectCur = pEffectCur->pLocalFrameEffNext;
	//}

	if ( pframeCur->pEffect )
	{
		//	Note : ���� ��ü ����Ʈ
		//
		pEffectCur = pframeCur->pEffect;
		while ( pEffectCur )
		{
			pEffectCur->Render ( pframeCur, pd3dDevice, this );
			pEffectCur->RenderMAT ( pframeCur, pd3dDevice, this, NULL, FALSE, FALSE );
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}
	else
	{
		//	Note : ���� Render
		//
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : Ʈ������ ����.
			//
			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
			if (FAILED(hr))
				return hr;

			//	Note : �޽� �׸���.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	
					{
						NSVERTEXNORTEX_FX::SetWorld( pframeCur->matCombined, 1.f );

						m_pTextureEff->Render( pd3dDevice, pmsMeshs, pframeCur->matCombined, TRUE, m_scpMaterialSetting.get() );
					}
					else
					{
						pmsMeshs->Render( pd3dDevice );
						pmsMeshs->RenderAlpha( pd3dDevice, FALSE );
					}		
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}
		}

		//	Note : �ڽ� ������ �׸���.
		//
		pframeChild = pframeCur->pframeFirstChild;
		while ( pframeChild != NULL )
		{
			hr = DrawFrames ( pd3dDevice, pframeChild, pCV, matWorld );
			if (FAILED(hr))
				return hr;

			pframeChild = pframeChild->pframeSibling;
		}
	}

	////	Note : ���� Render �� ����Ʈ
	////
	//std::vector<DxEffectTiling *> vecEffectTiling;
	//vecEffectTiling.reserve(10);
	//pEffectCur = pframeCur->pEffectNext;
	//while ( pEffectCur )
	//{
	//	if( pEffectCur->GetTypeID() != DEF_EFFECT_TILING )
	//	{
	//		pEffectCur->Render ( pframeCur, pd3dDevice, this );
	//	}else{
	//		vecEffectTiling.push_back( (DxEffectTiling *)pEffectCur );
	//	}
	//	pEffectCur = pEffectCur->pLocalFrameEffNext;
	//}

	//sort( vecEffectTiling.begin(), vecEffectTiling.end(), std::mem_fun(&DxEffectTiling::SortRenderPriority) );

	//std::vector<DxEffectTiling *>::iterator iter = vecEffectTiling.begin();
	//for( ; iter != vecEffectTiling.end(); ++iter )
	//{
	//	(* iter)->Render( pframeCur, pd3dDevice, this, NULL, m_pTextureEff );
	//}

	

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return S_OK;

	//	Note : 
	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : ���� Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				if( m_pTextureEff )	
				{
					m_pTextureEff->RenderAlpha( pd3dDevice, pmsMeshs, FALSE, pframeCur->matCombined, TRUE, m_scpMaterialSetting.get() );
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFrames_RenderAlpha( pd3dDevice, pframeChild, pCV, matWorld );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Decal1( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return S_OK;

	//	Note : 
	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : ���� Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				if( m_pTextureEff )	
				{
					if ( m_scpMaterialSetting.get() )
					{
						m_pTextureEff->RenderDecal( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_scpMaterialSetting.get(), 1 );
					}
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFrames_Decal1( pd3dDevice, pframeChild, pCV, matWorld );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Decal2( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return S_OK;

	//	Note : 
	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : ���� Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				if( m_pTextureEff )	
				{
					if ( m_scpMaterialSetting.get() )
					{
						m_pTextureEff->RenderDecal( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_scpMaterialSetting.get(), 2 );
					}
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFrames_Decal2( pd3dDevice, pframeChild, pCV, matWorld );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Decal3( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return S_OK;

	//	Note : 
	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : ���� Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				if( m_pTextureEff )	
				{
					if ( m_scpMaterialSetting.get() )
					{
						m_pTextureEff->RenderDecal( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_scpMaterialSetting.get(), 3 );
					}
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFrames_Decal3( pd3dDevice, pframeChild, pCV, matWorld );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::RenderWaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return S_OK;

	//	Note : 
	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : ���� Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				if( m_pTextureEff )	
				{
					if ( m_scpMaterialSetting.get() )
					{
						m_pTextureEff->RenderWaterCustomSoftAlpha( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_scpMaterialSetting.get() );
					}
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = RenderWaterCustomSoftAlpha( pd3dDevice, pframeChild, pCV, matWorld );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	return S_OK;

	if ( pframeCur->pEffect )	{}
	else
	{
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : Ʈ������ ����.
			//
			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
			if (FAILED(hr))
				return hr;

			//	Note : �޽� �׸���.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	m_pTextureEff->Render( pd3dDevice, pmsMeshs, pframeCur->matCombined, TRUE, m_scpMaterialSetting.get() );
					else				pmsMeshs->Render( pd3dDevice );
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}
		}

		//	Note : �ڽ� ������ �׸���.
		//
		pframeChild = pframeCur->pframeFirstChild;
		while ( pframeChild != NULL )
		{
			hr = DrawFrames_NOEFF ( pd3dDevice, pframeChild, pCV );
			if (FAILED(hr))
				return hr;

			pframeChild = pframeChild->pframeSibling;
		}
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : ġȯ ������ ������.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	return S_OK;

	if ( pframeCur->pEffect )	{}
	else
	{
		if ( pframeCur->pmsMeshs != NULL )
		{
			D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

			D3DXMATRIX matWorld, matScale;
			matWorld = pframeCur->matCombined;
			matWorld._42 -= (vCenter.y*2.f);
			D3DXMatrixMultiply_MIRROR( matWorld );

			DWORD	dwCullMode;
			pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

			//	Note : Ʈ������ ����.
			//
			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
			if (FAILED(hr))
				return hr;

			//	Note : �޽� �׸���.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	m_pTextureEff->Render( pd3dDevice, pmsMeshs, pframeCur->matCombined, TRUE, m_scpMaterialSetting.get() );
					else				pmsMeshs->Render( pd3dDevice );
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
		}

		//	Note : �ڽ� ������ �׸���.
		//
		pframeChild = pframeCur->pframeFirstChild;
		while ( pframeChild != NULL )
		{
			hr = DrawFrames_Reflect ( pd3dDevice, pframeChild, pCV );
			if (FAILED(hr))
				return hr;

			pframeChild = pframeChild->pframeSibling;
		}
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawAlphaMapFrames ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, BOOL bRefelct )
{
	//DrawFrames ���� �Ű� ����.

	//HRESULT hr = S_OK;

	//DxMeshes* pmsMeshs;
	//DxFrame *pframeCur = m_pDxAlphaMapFrameHead;
	//while ( pframeCur )
	//{
	//	// Note : �������� Piece�� ��� �Ѹ��� ����.
	//	if( pframeCur->pRePlacePiece || pframeCur->bPieceUSE )
	//	{
	//		pframeCur = pframeCur->pNextAlphaMapFrm;
	//		continue;
	//	}

	//	//// Note : ������ �̸���  [Mesh]�� �� �̸����� �������� ã�� �� Piece�� ��� �Ѹ��� ����.
	//	//std::string strFrameName = pframeCur->szName;
	//	//UINT nSize = strFrameName.size();
	//	//nSize = nSize - 6;
	//	//strFrameName.resize( nSize );
	//	//DxFrame *pFindFrame = FindFrame( strFrameName.c_str() );
	//	//if( pFindFrame )
	//	//{
	//	//	if( pFindFrame->pRePlacePiece || pFindFrame->bPieceUSE )
	//	//	{
	//	//		pframeCur = pframeCur->pNextAlphaMapFrm;
	//	//		continue;
	//	//	}
	//	//}

	//	//	Note : ������ ����... Render ( , D3DXMATRIX *pWorld, ) ���� �����ε�.
	//	//
	//	if ( !pframeCur->BoxIsInTheFrustum ( &m_pDxFrame->matRot, *pCV, pd3dDevice ) )
	//	{
	//		pframeCur = pframeCur->pNextAlphaMapFrm;
	//		continue;
	//	}

	//	//	Note : ���� Render
	//	//
	//	if ( pframeCur->pEffect )	{}
	//	else if ( pframeCur->pmsMeshs != NULL )
	//	{
	//		DWORD	dwCullMode;
	//		D3DXMATRIX matWorld;
	//		if ( bRefelct )
	//		{
	//			D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

	//			D3DXMATRIX matScale;
	//			matWorld = pframeCur->matCombined;
	//			matWorld._42 -= (vCenter.y*2.f);
	//			D3DXMatrixMultiply_MIRROR( matWorld );
	//			
	//			pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
	//			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
	//		}
	//		else
	//		{
	//			matWorld = pframeCur->matCombined;
	//		}

	//		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
	//		if (FAILED(hr))
	//			return hr;

	//		//	Note : �޽� �׸���.
	//		//
	//		pmsMeshs = pframeCur->pmsMeshs;
	//		while ( pmsMeshs != NULL )
	//		{
	//			if ( pmsMeshs->m_pLocalMesh )
	//			{
	//				if( m_pTextureEff )	m_pTextureEff->RenderAlpha( pd3dDevice, pmsMeshs, FALSE, pframeCur->matCombined, TRUE );
	//				else				pmsMeshs->RenderAlpha( pd3dDevice, FALSE );
	//			}

	//			pmsMeshs = pmsMeshs->m_pMeshNext;
	//		}

	//		if ( bRefelct )
	//		{
	//			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
	//		}
	//	}

	//	pframeCur = pframeCur->pNextAlphaMapFrm;
	//}

	////	Note : �ڽ��������� �׸��� �ʴ´�.
	////		- ������ ���� DxMesh �� Alpha ������ ��� �����Ƿ�...

	return S_OK;
}

HRESULT DxFrameMesh::DrawSelectedFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, const D3DXMATRIX* pmatWorld )
{
	HRESULT hr;
	if ( !m_pCollDetectFrame )	return S_OK;

	DWORD dwZFunc;
	pd3dDevice->GetRenderState( D3DRS_ZFUNC,	&dwZFunc );

	//	Note : ������ ����... Render ( , D3DXMATRIX *pWorld, ) ���� �����ε�.
	//
	if ( !m_pCollDetectFrame->BoxIsInTheFrustum ( pmatWorld, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : ���� Render
	//
	if ( m_pCollDetectFrame->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &m_pCollDetectFrame->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		DxMeshes *pmsMeshs = m_pCollDetectFrame->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				pmsMeshs->RenderTriangle ( pd3dDevice );
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZFUNC,	dwZFunc );

	return S_OK;
}

HRESULT DxFrameMesh::DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV )
{
	HRESULT hr;

    //	Note : ���� Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		DxMeshes* pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
            	for ( DWORD i=0; i<pmsMeshs->m_cMaterials; i++ )
                {
	                pmsMeshs->m_pLocalMesh->DrawSubset( i );
                }
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	DxFrame* pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawWireFrame ( pd3dDevice, pframeChild, pCV );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

void DxFrameMesh::DrawSelectedMesh1 ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, DxFrame* pCurFrame )
{
	if ( !pCurFrame ) return;

	//	Note : ������ ����... Render ( , D3DXMATRIX *pWorld, ) ���� �����ε�.
	//
	
//	if ( !pCurFrame->BoxIsInTheFrustum ( NULL, *pCV, pd3dDevice ) )	return;

	//	Note : ���� Render
	//
	if ( pCurFrame->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		pd3dDevice->SetTransform ( D3DTS_WORLD, &pCurFrame->matCombined );
		
		//	Note : �޽� �׸���.
		//
		DxMeshes *pmsMeshs = pCurFrame->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				DWORD OldFillMode;
				pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
				pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

				DWORD dwD3dColorOld;
				pd3dDevice->GetRenderState( D3DRS_AMBIENT, &dwD3dColorOld );

				DWORD dwD3dColor = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,0);
				pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColor );

				D3DLIGHTQ light, lightOld;
				pd3dDevice->GetLight ( 0, &lightOld );

				D3DUtil_InitLight ( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
				pd3dDevice->SetLight ( 0, &light );

				//	Note : ���͸���, �ؽ��� ����.
				//
				D3DMATERIALQ RedMaterial;
				memset(&RedMaterial, 0, sizeof(D3DMATERIALQ));
				RedMaterial.Diffuse.r = 1.0f;
				RedMaterial.Diffuse.g = 0.0f;
				RedMaterial.Diffuse.b = 0.0f;
				RedMaterial.Specular = RedMaterial.Diffuse;

				pd3dDevice->SetMaterial( &RedMaterial );
				pd3dDevice->SetTexture( 0, NULL );

				for ( DWORD i=0; i<pmsMeshs->m_cMaterials; i++ )
				{
					if ( pmsMeshs->m_exMaterials[i].bSubSetTriRend )
						pmsMeshs->m_pLocalMesh->DrawSubset( i );
				}

				pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );
				pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColorOld );
				pd3dDevice->SetLight ( 0, &lightOld );
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	DrawSelectedMesh1 (  pd3dDevice, pCV, pCurFrame->pframeFirstChild );
	DrawSelectedMesh1 (  pd3dDevice, pCV, pCurFrame->pframeSibling );
}

HRESULT DxFrameMesh::DrawSelectedMesh ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )
{
	if ( !m_pDxFrame )	return S_OK;

	DWORD dwZFunc;
	pd3dDevice->GetRenderState( D3DRS_ZFUNC,	&dwZFunc );

	DrawSelectedMesh1( pd3dDevice, pCV, m_pDxFrame );

	pd3dDevice->SetRenderState( D3DRS_ZFUNC,	dwZFunc );

	return S_OK;
}

HRESULT DxFrameMesh::DrawSelectedPoint ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vVert )
{
	//	-----------------------------[�׽�Ʈ��]--------------------------------
	//
	static DWORD BOXVERTEX = D3DFVF_XYZ;

	DWORD OldFillMode;
	pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	DWORD OldLighting;
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	//	Note : ���͸���, �ؽ��� ����.
	//
	D3DMATERIALQ WhiteMaterial;
	memset(&WhiteMaterial, 0, sizeof(D3DMATERIALQ));
	WhiteMaterial.Diffuse.r = 1.0f;
	WhiteMaterial.Diffuse.g = 1.0f;
	WhiteMaterial.Diffuse.b = 1.0f;
	WhiteMaterial.Specular = WhiteMaterial.Diffuse;

	pd3dDevice->SetMaterial( &WhiteMaterial );
	pd3dDevice->SetTexture( 0, NULL );

	//	Note : Set the world matrix
	//
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation ( &matWorld, vVert.x, vVert.y, vVert.z );

	pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );

	pd3dDevice->SetFVF(BOXVERTEX);
	pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, EDITMESHS::dwNumSphereVertex, EDITMESHS::dwNumSphereFaces,
		EDITMESHS::SphereIndex, D3DFMT_INDEX16, EDITMESHS::SphereVertex, D3DXGetFVFVertexSize(BOXVERTEX) );

	pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );

	return S_OK;
}

HRESULT	DxMeshes::RenderOctree( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha )
{
	if ( !m_pDxOctreeMesh->m_pVB )
		return S_OK;

	if ( !m_pDxOctreeMesh->m_pIB )
		return S_OK;

	OctreeDebugInfo::FaceViewCount += m_pDxOctreeMesh->m_dwNumFaces;
	OctreeDebugInfo::VertexViewCount += m_pDxOctreeMesh->m_dwNumVertices;

	HRESULT hr;
	hr = pd3dDevice->SetStreamSource ( 0, m_pDxOctreeMesh->m_pVB, 0, D3DXGetFVFVertexSize(m_pDxOctreeMesh->m_dwFVF) );
	if ( FAILED ( hr ) ) return E_FAIL;
	
	hr = pd3dDevice->SetFVF ( m_pDxOctreeMesh->m_dwFVF );
	if ( FAILED ( hr ) ) return E_FAIL;

	hr = pd3dDevice->SetIndices ( m_pDxOctreeMesh->m_pIB );
	if ( FAILED ( hr ) ) return E_FAIL;

	D3DMATERIALQ sMaterial;
	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		if ( m_exMaterials[i].MtrlType != MATERIAL_NORMAL )		continue;

		//	Note : ���͸���, �ؽ��� ����.
		pd3dDevice->SetTexture( 0, m_pTextureRes[i].GetTexture() );

		switch( g_emMeshMeterial )
		{
		case EM_MESH_MATERIAL:
			if( fAlpha == 1.f )
			{
				pd3dDevice->SetMaterial( &m_rgMaterials[i] );
			}
			else
			{
				sMaterial = m_rgMaterials[i];
				sMaterial.Diffuse.a = fAlpha;
				sMaterial.Ambient.a = fAlpha;
				sMaterial.Specular.a = fAlpha;
				sMaterial.Emissive.a = fAlpha;
				pd3dDevice->SetMaterial( &sMaterial );
			}
			break;
		case EM_MESH_MATERIAL_RED:
			sMaterial = m_rgMaterials[i];
			sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
			sMaterial.Ambient.g = sMaterial.Diffuse.g = DxMeshes::g_fAlpha;
			sMaterial.Ambient.b = sMaterial.Diffuse.b = DxMeshes::g_fAlpha;
			pd3dDevice->SetMaterial( &sMaterial );
			break;
		};

		m_pDxOctreeMesh->DrawSubset( i, pd3dDevice );
	}

	return S_OK;
}

HRESULT	DxMeshes::RenderOctreeAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow, const float fAlpha )
{
	if ( !m_pDxOctreeMesh->m_pVB )
		return S_OK;

	if ( !m_pDxOctreeMesh->m_pIB )

		return S_OK;
	OctreeDebugInfo::FaceViewCount += m_pDxOctreeMesh->m_dwNumFaces;
	OctreeDebugInfo::VertexViewCount += m_pDxOctreeMesh->m_dwNumVertices;

	HRESULT hr;
	hr = pd3dDevice->SetStreamSource ( 0, m_pDxOctreeMesh->m_pVB, 0, D3DXGetFVFVertexSize(m_pDxOctreeMesh->m_dwFVF) );
	if ( FAILED ( hr ) ) return E_FAIL;
	
	hr = pd3dDevice->SetFVF ( m_pDxOctreeMesh->m_dwFVF );
	if ( FAILED ( hr ) ) return E_FAIL;

	hr = pd3dDevice->SetIndices ( m_pDxOctreeMesh->m_pIB );
	if ( FAILED ( hr ) ) return E_FAIL;


	D3DMATERIALQ sMaterial;
	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		DXMTRLTYPE emMETR = m_exMaterials[i].MtrlType;
		if ( emMETR == MATERIAL_NORMAL )		continue;

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().SetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().SetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().SetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().SetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
				break;
			default:
				DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
			};
		}

		//	Note : ���͸���, �ؽ��� ����.
		pd3dDevice->SetTexture( 0, m_pTextureRes[i].GetTexture() );

		switch( g_emMeshMeterial )
		{
		case EM_MESH_MATERIAL:
			if( fAlpha == 1.f )
			{
				pd3dDevice->SetMaterial( &m_rgMaterials[i] );
			}
			else
			{
				sMaterial = m_rgMaterials[i];
				sMaterial.Diffuse.a = fAlpha;
				sMaterial.Ambient.a = fAlpha;
				sMaterial.Specular.a = fAlpha;
				sMaterial.Emissive.a = fAlpha;
				pd3dDevice->SetMaterial( &sMaterial );
			}
			break;
		case EM_MESH_MATERIAL_RED:
			sMaterial = m_rgMaterials[i];
			sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
			sMaterial.Ambient.g = sMaterial.Diffuse.g = DxMeshes::g_fAlpha;
			sMaterial.Ambient.b = sMaterial.Diffuse.b = DxMeshes::g_fAlpha;
			pd3dDevice->SetMaterial( &sMaterial );
			break;
		};

		m_pDxOctreeMesh->DrawSubset( i, pd3dDevice );

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().ReSetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().ReSetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().ReSetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().ReSetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
				break;
			default:
				DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
			};
		}
	}

	return S_OK;
}

BOOL DxMeshes::RenderLightMapChecker( LPDIRECT3DDEVICEQ pd3dDevice, WORDWORD wwLightMapUserUV_TextureSize )
{
	return FALSE;

	//D3DXVECTOR2 vMax(-FLT_MAX,-FLT_MAX);
	//D3DXVECTOR2 vMin(FLT_MAX,FLT_MAX);
	//DWORD dwVerticesNUM = m_pLocalMesh->GetNumVertices();
	//VERTEX*	pVertices;
	//m_pLocalMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
	//for ( DWORD i=0; i<dwVerticesNUM; ++i )
	//{
	//	if ( vMax.x < pVertices[i].vTex.x )
	//		vMax.x = pVertices[i].vTex.x;
	//	if ( vMin.x > pVertices[i].vTex.x )
	//		vMin.x = pVertices[i].vTex.x;
	//	if ( vMax.y < pVertices[i].vTex.y )
	//		vMax.y = pVertices[i].vTex.y;
	//	if ( vMin.y > pVertices[i].vTex.y )
	//		vMin.y = pVertices[i].vTex.y;
	//	
	//}
	//m_pLocalMesh->UnlockVertexBuffer();

	//D3DXVECTOR2 vMultiply(vMax - vMin);
	//vMultiply.x = 1.f / vMultiply.x;
	//vMultiply.y = 1.f / vMultiply.y;

	//WORDWORD wwUV_Mutiply;
	//wwUV_Mutiply.wA = static_cast<WORD>( wwLightMapUserUV_TextureSize.wA * vMultiply.x );
	//wwUV_Mutiply.wB = static_cast<WORD>( wwLightMapUserUV_TextureSize.wB * vMultiply.y );

	//NSLightMapFX::SetLightMapUV_Multiply( wwUV_Mutiply );

	//return NSLightMapFX::RenderVERTEX( pd3dDevice, m_pLocalMesh, m_cMaterials );
}

BOOL DxMeshes::RenderLightMapCheckerInexact( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 vDir;
	vDir = m_vMax - m_vMin;

	int nIndex = 0;
	if ( vDir.x >= vDir.y && vDir.x >= vDir.z )
	{
		if ( vDir.y >= vDir.z )	// x,y
			nIndex = 0;
		else					// x,z
			nIndex = 1;
	}
	else if ( vDir.y >= vDir.x && vDir.y >= vDir.z )
	{
		if ( vDir.x >= vDir.z )	// x,y
			nIndex = 0;
		else					// y,z
			nIndex = 2;
	}
	else
	{
		if ( vDir.x >= vDir.y )	// x,z
			nIndex = 1;
		else					// y,z
			nIndex = 2;
	}
	
	NSLightMapFX::SetLightMapUV_Multiply( WORDWORD(1,1) );
	return NSLightMapFX::RenderInexact( pd3dDevice, m_pLocalMesh, m_cMaterials, nIndex );
}
