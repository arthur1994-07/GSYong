#include "pch.h"

#include "../../DxEffect/TexEff/DxTextureEffMan.h"

#include "DxFrameMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//							GeometryPass
HRESULT DxFrameMesh::Render_Deffered_GeometryPass( LPDIRECT3DDEVICEQ pd3dDevice, 
												  const D3DXMATRIX *pWorld, 
												  const CLIPVOLUME *pCV,
												  BOOL bForwardPass )	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들..
{
	m_vecframeCur.clear();

	if ( !m_pDxFrame )	
		return S_OK;

	HRESULT hr;

	//	Note : 메트릭스 계산.
	//
	D3DXMATRIXA16 mCur;
	D3DXMatrixIdentity ( &mCur );
	if ( pWorld )
	{
		mCur = *pWorld;
	}

	hr = UpdateFrames ( m_pDxFrame, mCur );
	if (FAILED(hr))		return hr;

	//	Note : 메쉬 그리기 수행.
	DrawFrames_Deffered_GPass( pd3dDevice, m_pDxFrame, pCV, mCur, 0, &m_vecframeCur, bForwardPass );
	DrawFrames_Deffered_GPass_Decal( pd3dDevice, m_vecframeCur, 1, bForwardPass );
	DrawFrames_Deffered_GPass_Decal( pd3dDevice, m_vecframeCur, 2, bForwardPass );
	DrawFrames_Deffered_GPass_Decal( pd3dDevice, m_vecframeCur, 3, bForwardPass );

	return S_OK;
}

HRESULT DxFrameMesh::RenderSoftAlpha_Deffered_GeometryPass( LPDIRECT3DDEVICEQ pd3dDevice, 
														   BOOL bInverseOrder )
{
	//	Note : 메쉬 그리기 수행.
	//
	DrawFrames_Deffered_GPass_SoftAlpha( pd3dDevice, m_vecframeCur, bInverseOrder, FALSE );

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Deffered_GPass( LPDIRECT3DDEVICEQ pd3dDevice, 
											   DxFrame *pframeCur, 
											   const CLIPVOLUME *pCV, 
											   const D3DXMATRIX& matWorld, 
											   DWORD dwPass, 
											   std::vector<DxFrame*>* pvecframeCur,
											   BOOL bForwardPass )	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들..
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 내비게이션 메시 그리지 않음 검사
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )
		return S_OK;

	//	Note : 
	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )
		return S_OK;

	//	Note : 원본 Render
	if ( pframeCur->pmsMeshs != NULL )
	{
		// 
		if ( pvecframeCur )
		{
			pvecframeCur->push_back( pframeCur );
		}

		//	Note : 메쉬 그리기.
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				if( m_pTextureEff )	
				{
					m_pTextureEff->Render_Deffered_GPass( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_scpMaterialSetting.get(), dwPass, FALSE, bForwardPass );
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : 자식 프레임 그리기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DrawFrames_Deffered_GPass ( pd3dDevice, pframeChild, pCV, matWorld, dwPass, pvecframeCur, bForwardPass );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Deffered_GPass_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
														 const std::vector<DxFrame*>& vecframeCur, 
														 BOOL bInverseOrder,
														 BOOL bForwardPass )	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들..)
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	for ( DWORD i=0; i<vecframeCur.size(); ++i )
	{
		//	Note : 원본 Render
		const DxFrame* pframeCur(NULL);
		if ( bInverseOrder )
		{
			pframeCur = vecframeCur[static_cast<int>(vecframeCur.size())-1-i];
		}
		else
		{
			pframeCur = vecframeCur[i];
		}

		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 메쉬 그리기.
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	
					{
						if ( m_scpMaterialSetting.get() )
						{
							m_pTextureEff->Render_Deffered_GPass( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_scpMaterialSetting.get(), 0, TRUE, bForwardPass );
						}
					}
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}
		}
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Deffered_GPass_Decal( LPDIRECT3DDEVICEQ pd3dDevice, 
													 const std::vector<DxFrame*>& vecframeCur,
													 DWORD dwOrder, 
													 BOOL bForwardPass )	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들.. )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	for ( DWORD i=0; i<vecframeCur.size(); ++i )
	{
		//	Note : 원본 Render
		const DxFrame* pframeCur = vecframeCur[i];
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 메쉬 그리기.
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	
					{
						if ( m_scpMaterialSetting.get() )
						{
							m_pTextureEff->Render_Deffered_GPass_Decal( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_scpMaterialSetting.get(), dwOrder, bForwardPass );
						}
					}
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}
		}
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//						MaterialPass
HRESULT DxFrameMesh::Render_Deffered_MaterialPass( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX *pWorld, const CLIPVOLUME *pCV )
{
	//if ( !m_pDxFrame )	return S_OK;

	////	Note : 메트릭스 계산.
	////
	//D3DXMATRIXA16 mCur;
	//D3DXMatrixIdentity ( &mCur );
	//if ( pWorld )
	//{
	//	mCur = *pWorld;
	//}

	return S_OK;
}

HRESULT DxFrameMesh::Render_DecalPass2_WaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	Render_Deffered_GeometryPass( pd3dDevice, &matIdentity, pCV, TRUE );
	DrawFrames_Deffered_GPass_SoftAlpha( pd3dDevice, m_vecframeCur, FALSE, TRUE );

	return S_OK;
}

//
////////////////////////////////////////////////////////////////////////////
////							ForwardPass
//HRESULT DxFrameMesh::Render_Forward_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX *pWorld, const CLIPVOLUME *pCV )
//{
//	if ( !m_pDxFrame )
//		return S_OK;
//
//	D3DXMATRIXA16 mCur;
//	D3DXMatrixIdentity ( &mCur );
//	if ( pWorld )
//	{
//		mCur = *pWorld;
//	}
//
//	DWORD dwAlphaBlendEnable;
//	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
//
//	DrawFrames_Forward_SoftAlpha( pd3dDevice, m_pDxFrame, pCV, mCur );
//
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
//
//	return S_OK;
//}
//
//HRESULT DxFrameMesh::DrawFrames_Forward_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld )
//{
//	HRESULT hr = S_OK;
//	DxMeshes *pmsMeshs;
//	DxFrame *pframeChild;
//
//	//	Note : 내비게이션 메시 그리지 않음 검사
//	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )
//		return S_OK;
//
//	//	Note : 
//	if ( !pframeCur->BoxIsInTheFrustum ( &matWorld, *pCV, pd3dDevice ) )
//		return S_OK;
//
//	//	Note : 원본 Render
//	//
//	if ( pframeCur->pmsMeshs != NULL )
//	{
//		//	Note : 메쉬 그리기.
//		//
//		pmsMeshs = pframeCur->pmsMeshs;
//		while ( pmsMeshs != NULL )
//		{
//			if ( pmsMeshs->m_pLocalMesh )
//			{
//				if( m_pTextureEff )	
//				{
//					m_pTextureEff->Render_Forward_SoftAlpha( pd3dDevice, pmsMeshs, pframeCur->matCombined, m_spMaterialSetting.get() );
//				}
//			}
//
//			pmsMeshs = pmsMeshs->m_pMeshNext;
//		}
//	}
//
//	//	Note : 자식 프레임 그리기.
//	//
//	pframeChild = pframeCur->pframeFirstChild;
//	while ( pframeChild != NULL )
//	{
//		hr = DrawFrames_Forward_SoftAlpha( pd3dDevice, pframeChild, pCV, matWorld );
//		if (FAILED(hr))
//			return hr;
//
//		pframeChild = pframeChild->pframeSibling;
//	}
//
//	return S_OK;
//}