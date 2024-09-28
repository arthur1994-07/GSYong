#include "pch.h"

#include "../DxLandMan.h"

#include "../../DxEffect/Frame/DxEffectRender.h"

#include "../../DxTools/Occlusion/NSOcclusionOctree.h"
#include "../../DxTools/Occlusion/DxOcclusionQuery.h"
#include "../../DxTools/EditMeshs.h"
#include "../../DxEffect/Frame/DxEffectDefine.h"
#include "../../DxEffect/Frame/DxEffectFrame.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../GlobalParam.h"
#include "../DxPiece\NSOctreeRenderList.h"

#include "DxOctree.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace	OctreeDebugInfo
{
	int			TotalNodesDrawn;
	int			EndNodeCount;
	int			AmountDxFrame;
	int			TodalDxFrameDrawn;
	int			FaceViewCount;
	int			VertexViewCount;
}

DWORD	DxOctree::m_MaxTriangles = 0;
DWORD	DxOctree::m_MaxSubdivisions = 0;
DWORD	DxOctree::m_CurrentSubdivision = 0;

LPDIRECT3DDEVICEQ	DxOctree::m_pd3dDevice = NULL;
DxFrameMesh*		DxOctree::m_pFrameMesh = NULL;

//	Note	:	디버그용,	SKY
BOOL			g_bDebugLine = FALSE;

DxOctree::DxOctree()
	: m_nID(-1)
	, m_pDxFrameHead(NULL),
	m_pDxAlphaMapFrameHead(NULL),
	m_bSubDivided(FALSE),
	m_SFile(NULL),
	m_pEffGlobalHead(NULL),
	m_vMin(D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX)),
	m_vMax(D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)),
	m_bLoaded(FALSE),
	m_DataAddress(0)

	, m_pd3dQuery( NULL )
	, m_bQueryTest( FALSE )
	, m_dwCullPixels( 1 )
{	
	memset( m_pOctreeNodes, 0, sizeof(DxOctree*)*8 );
}
 
DxOctree::~DxOctree()
{	
	m_bSubDivided = FALSE;
	for ( int i = 0; i < 8; i++ )
	{
		SAFE_DELETE( m_pOctreeNodes[i] );		
	}
	m_vMin = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMax = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	SAFE_DELETE ( m_pDxFrameHead );

}


HRESULT DxOctree::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

	m_SFile				= NULL;
	m_pEffGlobalHead	= NULL;
	m_pDxAlphaMapFrameHead = NULL;

	return	S_OK;
}

HRESULT DxOctree::DeleteDeviceObjects()
{
	m_bSubDivided = FALSE;
	for ( int i = 0; i < 8; i++ )
	{
		SAFE_DELETE( m_pOctreeNodes[i] );		
	}
	m_vMin = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMax = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	SAFE_DELETE ( m_pDxFrameHead );

	m_pFrameMesh = NULL;
	m_pDxAlphaMapFrameHead = NULL;

	m_MaxTriangles = 0;
	m_MaxSubdivisions = 0;
	m_CurrentSubdivision = 0;
	
	return S_OK;
}

void DxOctree::DrawFrame ( DxFrame *pframeCur, DxLandMan* pLandMan )
{
	//	Note : 원본 Render 전 이팩트
	//
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectPrev;
	while ( pEffectCur )
	{
		pEffectCur->Render ( pframeCur, m_pd3dDevice );
		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	if ( pframeCur->pEffect )
	{
		//	Note : 원본 대체 이팩트
		//
		pEffectCur = pframeCur->pEffect;
		while ( pEffectCur )
		{
			pEffectCur->Render ( pframeCur, m_pd3dDevice );
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}
	else
	{
		//	Note : 원본 Render
		//
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 메쉬 그리기.
			//
			DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					pmsMeshs->RenderOctree ( m_pd3dDevice );
				}
				pmsMeshs = pmsMeshs->m_pMeshNext;
			}
		}
	}

	//	Note : 원본 Render 후 이팩트
	//
	pEffectCur = pframeCur->pEffectNext;
	for ( ; pEffectCur; pEffectCur = pEffectCur->pLocalFrameEffNext )
	{
		if ( pEffectCur->GetFlag() & _EFF_AFTER )
		{
			pLandMan->GetAfterRender()->AfterRender ( 0, DXEFFECTREND(pEffectCur,pframeCur,pLandMan) );
		}
		else
		{
			pEffectCur->Render ( pframeCur, m_pd3dDevice, NULL, pLandMan );
		}
	}
}

void DxOctree::DrawFrame_NOEFF ( DxFrame *pframeCur )
{
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectNext;

	for ( ; pEffectCur; pEffectCur = pEffectCur->pLocalFrameEffNext )
	{
		if ( pEffectCur->GetFlag() & _EFF_AFTER )		return;
	}

	if ( pframeCur->pEffect )
	{
		//	Note : 원본 대체 이팩트
		//
		pEffectCur = pframeCur->pEffect;
		while ( pEffectCur )
		{
			if ( pEffectCur->GetTypeID() == DEF_EFFECT_SHADOW )		{}
			else													pEffectCur->Render ( pframeCur, m_pd3dDevice );

			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}
	else
	{
		//	Note : 원본 Render
		//
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 메쉬 그리기.
			//
			DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					pmsMeshs->RenderOctree ( m_pd3dDevice );
				}
				pmsMeshs = pmsMeshs->m_pMeshNext;
			}
		}
	}
}

void DxOctree::DrawFrame_Query ( DxFrame *pframeCur )
{
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectNext;

	for ( ; pEffectCur; pEffectCur = pEffectCur->pLocalFrameEffNext )
	{
		if ( pEffectCur->GetFlag() & _EFF_AFTER )		return;
	}

	//	Note : 원본 Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pDxOctreeMesh )
			{
				pmsMeshs->RenderOctree ( m_pd3dDevice );
			}
			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}
}

void DxOctree::Render ( const CLIPVOLUME &cv, sc::SerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, DxLandMan* pLandMan )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if ( BoxIsInTheFrustum(cv) )
	{
		if ( !m_bLoaded )
            DynamicLoad ( SFile, pd3dDevice, pEffGlobalHead );
		
		if ( m_bSubDivided )
		{
			if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
		}
		else
		{
			if ( !NSOcclusionOctree::Visible( m_nID ) )
			{
				return;
			}

			DxFrame	*pDxFrameCur = m_pDxFrameHead;
			if ( pDxFrameCur )
                OctreeDebugInfo::TotalNodesDrawn++;
			if ( g_bDebugLine && pDxFrameCur )
                RenderDebugBox();

			while ( pDxFrameCur )
			{
				DrawFrame ( pDxFrameCur, pLandMan );

				OctreeDebugInfo::TodalDxFrameDrawn++;
				pDxFrameCur = pDxFrameCur->pframeSibling;
			}
		}
	}
}

void DxOctree::Render_NOEFF ( const CLIPVOLUME &cv, sc::SerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if ( !BoxIsInTheFrustum(cv) )
        return;

	if ( !m_bLoaded )
		DynamicLoad ( SFile, pd3dDevice, pEffGlobalHead );
	
	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);					
	}
	else
	{
		DxFrame	*pDxFrameCur = m_pDxFrameHead;
		if ( pDxFrameCur )
            OctreeDebugInfo::TotalNodesDrawn++;
		if ( g_bDebugLine && pDxFrameCur )
            RenderDebugBox();

		while ( pDxFrameCur )
		{
			DrawFrame_NOEFF ( pDxFrameCur );

			OctreeDebugInfo::TodalDxFrameDrawn++;
			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}
}

void DxOctree::RenderAlpha ( const CLIPVOLUME &cv, BOOL bShadow )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if(!BoxIsInTheFrustum(cv))
        return;
	
	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->RenderAlpha (cv,bShadow);
	}
	else
	{
		if ( !NSOcclusionOctree::Visible( m_nID ) )
		{
			return;
		}

		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = m_pDxAlphaMapFrameHead;

		if ( pDxFrameCur )
			OctreeDebugInfo::TotalNodesDrawn++;
		if ( g_bDebugLine && pDxFrameCur )
			RenderDebugBox();

		while ( pDxFrameCur )
		{
			if ( pDxFrameCur->pEffect )	{}
			else
			{
				//	Note : 메쉬 그리기.
				//
				pmsMeshs = pDxFrameCur->pmsMeshs;
				while ( pmsMeshs != NULL )
				{
					if ( pmsMeshs->m_pDxOctreeMesh )
					{
						pmsMeshs->RenderOctreeAlphaMap ( m_pd3dDevice, bShadow );
					}
					pmsMeshs = pmsMeshs->m_pMeshNext;
				}
			}

			OctreeDebugInfo::TodalDxFrameDrawn++;
			pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
		}
	}
}

BOOL	DxOctree::RenderDebugBox(BOOL	bIsAnimationBox)
{
	if ( bIsAnimationBox )
	{
		D3DMATERIALQ mtrl;
		D3DUtil_InitMaterial( mtrl, 0.0f, 0.0f, 1.0f, 1.f );
		m_pd3dDevice->SetMaterial( &mtrl );
		m_pd3dDevice->SetTexture ( 0, NULL );
	}
	else
	{
		D3DMATERIALQ mtrl;
		D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 1.0f, 1.f );
		m_pd3dDevice->SetMaterial( &mtrl );
		m_pd3dDevice->SetTexture ( 0, NULL );
	}

	EDITMESHS::RENDERAABB ( m_pd3dDevice, m_vMax, m_vMin );
	
	return TRUE;
}

void DxOctree::Render_Query( const CLIPVOLUME &cv, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if(!BoxIsInTheFrustum(cv))
		return;

	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->Render_Query (cv,pd3dDevice);
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->Render_Query (cv,pd3dDevice);
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->Render_Query (cv,pd3dDevice);
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->Render_Query (cv,pd3dDevice);
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->Render_Query (cv,pd3dDevice);
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->Render_Query (cv,pd3dDevice);
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->Render_Query (cv,pd3dDevice);
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->Render_Query (cv,pd3dDevice);
	}
	else
	{
		if ( !m_pd3dQuery )
			return;

		if ( NSOcclusionExceptIndex::IsHaveExceptIndexOctree( m_nID ) )
			return;

		if( !m_bQueryTest )
		{
			// 1차검사 안되었으면 검사하고 플래그를 바꾼다.
			m_pd3dQuery->Issue( D3DISSUE_BEGIN );
			{
				DxFrame	*pDxFrameCur = m_pDxFrameHead;
				while ( pDxFrameCur )
				{
					DrawFrame_Query ( pDxFrameCur );
					pDxFrameCur = pDxFrameCur->pframeSibling;
				}
			}
			m_pd3dQuery->Issue( D3DISSUE_END );

			m_bQueryTest = TRUE;
		}
	}
}

void DxOctree::SetLoadState ( BOOL bLoaded )
{
	m_bLoaded = bLoaded;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		시작
BOOL DxOctree::IsHaveDrawFrame ( DxFrame *pframeCur )
{
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectNext;

	for ( ; pEffectCur; pEffectCur = pEffectCur->pLocalFrameEffNext )
	{
		if ( pEffectCur->GetFlag() & _EFF_AFTER )
			return FALSE;
	}

	//	Note : 원본 Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pDxOctreeMesh )
			{
				return TRUE;
			}
			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	return FALSE;
}

VOID DxOctree::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->CreateQuery( pd3dDevice );
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->CreateQuery( pd3dDevice );
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->CreateQuery( pd3dDevice );
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->CreateQuery( pd3dDevice );
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->CreateQuery( pd3dDevice );
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->CreateQuery( pd3dDevice );
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->CreateQuery( pd3dDevice );
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->CreateQuery( pd3dDevice );
	}
	else
	{
		BOOL bDraw(FALSE);
		DxFrame	*pDxFrameCur = m_pDxFrameHead;
		while ( pDxFrameCur )
		{
			if ( IsHaveDrawFrame ( pDxFrameCur ) )
			{
				bDraw = TRUE;
			}
			pDxFrameCur = pDxFrameCur->pframeSibling;
		}

		// 드로우 할것이 없으면 Query를 만든다.
		if ( !bDraw )
			return;

		SAFE_RELEASE( m_pd3dQuery );
		m_bQueryTest = FALSE;
		m_dwCullPixels = 1;

		pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pd3dQuery );
	}
}

VOID DxOctree::DestroyQuery()
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->DestroyQuery();
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->DestroyQuery();
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->DestroyQuery();
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->DestroyQuery();
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->DestroyQuery();
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->DestroyQuery();
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->DestroyQuery();
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->DestroyQuery();
	}
	else
	{
		SAFE_RELEASE( m_pd3dQuery );
		m_bQueryTest = FALSE;
		m_dwCullPixels = 1;
	}
}

HRESULT DxOctree::GetDataQuery( DWORD& dwCullPixels )
{
	return m_pd3dQuery->GetData( (void*)&dwCullPixels, sizeof( DWORD ), 0 );
}

void DxOctree::ResetOcclusionQuery()
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->ResetOcclusionQuery();
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->ResetOcclusionQuery();
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->ResetOcclusionQuery();
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->ResetOcclusionQuery();
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->ResetOcclusionQuery();
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->ResetOcclusionQuery();
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->ResetOcclusionQuery();
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->ResetOcclusionQuery();
	}
	else
	{
		m_dwCullPixels = 0;
		m_bQueryTest = FALSE;
	}
}

void DxOctree::VisibleOcclusionQueryTest( const CLIPVOLUME &cv, LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if(!BoxIsInTheFrustum(cv))
		return;

	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->VisibleOcclusionQueryTest (cv,pd3dDevice,vecOctreeIndex);
	}
	else
	{
		if ( !m_pd3dQuery || !m_bQueryTest )
		{
			m_dwCullPixels = 0;
			return;
		}

		DWORD dwCullingDelay = timeGetTime();

		while ( GetDataQuery( m_dwCullPixels ) == S_FALSE )
		{
			//DxFrame	*pDxFrameCur = m_pDxFrameHead;
			//while ( pDxFrameCur )
			//{
			//	DrawFrame_Query ( pDxFrameCur );
			//	pDxFrameCur = pDxFrameCur->pframeSibling;
			//}

			//pDxFrameCur = m_pDxAlphaMapFrameHead;
			//while ( pDxFrameCur )
			//{
			//	DrawFrame_Query ( pDxFrameCur );
			//	pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
			//}

			D3DXVECTOR3 VertBox[8];
			VertBox[0] = D3DXVECTOR3( 100.f,	100.f,	100.f );
			VertBox[1] = D3DXVECTOR3( 100.f,	100.f,	0.f );
			VertBox[2] = D3DXVECTOR3( 0.f,		100.f,	100.f );
			VertBox[3] = D3DXVECTOR3( 0.f,		100.f,	0.f );
			VertBox[4] = D3DXVECTOR3( 100.f,	0.f,	100.f );
			VertBox[5] = D3DXVECTOR3( 100.f,	0.f,	0.f );
			VertBox[6] = D3DXVECTOR3( 0.f,		0.f,	100.f );
			VertBox[7] = D3DXVECTOR3( 0.f,		0.f,	0.f );
			pd3dDevice->DrawPrimitiveUP ( D3DPT_POINTLIST, 8, VertBox, 12 );
		}

		// 딜레이를 누적한다.
		DxOcclusionQuery::g_dwCullingDelay += timeGetTime() - dwCullingDelay;

		// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
		m_bQueryTest = FALSE;

		if ( m_dwCullPixels != 0 )
		{
			vecOctreeIndex.push_back( m_nID );
		}
	}	
}
//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		끝
//--------------------------------------------------------------------------------------------------------------------------------------