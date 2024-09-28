#include "pch.h"

#include "../../DxTools/DxMethods.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingleMan.h"

#include "./dxeffectmesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//------------------------------- 구동 클래스(CMeshEffect)시작 ----------------------------------//
const DWORD DxEffectMesh::TYPEID = EFFSINGLE_MESH;
const DWORD DxEffectMesh::FLAG = NULL;
const char DxEffectMesh::NAME[] = "메쉬효과";

LPDIRECT3DSTATEBLOCK9		DxEffectMesh::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectMesh::m_pDrawRenderStats = NULL;

DxEffectMesh::DxEffectMesh(void):
	m_fElapsedTime(0.f),
	m_vDistance( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ),
	m_fTimeSum(0.f),
	m_iAllSprite(1),
	m_iNowSprite(0),
	m_bSequenceRoop(FALSE)
{
	m_fSizeStart = 0.f;
	m_fAlphaStart = 0.f;

	m_fAngle			= 0.0f;
	m_nDelta			= 0;
	m_nMaterialDelta	= 0;
	m_vDistance			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_nBlend			= 1;
	m_vTexVel			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vTexVelSum		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_dwVertices		= 0;

	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectMesh::~DxEffectMesh(void)
{
	CleanUp ();
}

//Note: CreateDivece와 ReleaseDevice는 모두 DxEffSingleMan에서
//      InvalidateDeviceObjects()와 RestoreDeviceObjects()에서 호출하는 부분이다.
HRESULT DxEffectMesh::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
	
		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}


HRESULT DxEffectMesh::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectMesh::OnInitDevice_STATIC()
{
}

void DxEffectMesh::OnDeleteDevice_STATIC()
{
}

void DxEffectMesh::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	if( !m_SimMeshRes.Update() )
		return;

	D3DXVECTOR3 vMaxMesh, vMinMesh;
	m_SimMeshRes.get()->GetBoundBox( vMaxMesh, vMinMesh );

	D3DXMatrixAABBTransform( vMaxMesh, vMinMesh, m_matWorld );

	if( vMax.x < vMaxMesh.x )	vMax.x = vMaxMesh.x;
	if( vMax.y < vMaxMesh.y )	vMax.y = vMaxMesh.y;
	if( vMax.z < vMaxMesh.z )	vMax.z = vMaxMesh.z;

	if( vMin.x > vMinMesh.x )	vMin.x = vMinMesh.x;
	if( vMin.y > vMinMesh.y )	vMin.y = vMinMesh.y;
	if( vMin.z > vMinMesh.z )	vMin.z = vMinMesh.z;

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectMesh::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );	

	return S_OK;
}

HRESULT DxEffectMesh::InvalidateDeviceObjects()
{
	m_textureResColl.Clear();

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectMesh::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fAlpha	= m_fAlphaStart;
	m_fHeight	= m_fHeightStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if( m_dwFlag & USESCALE )	m_fSize = m_fSizeStart;
	else						m_fSize = 1.f;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectMesh::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff의 나이를 계산.
	//
	m_fElapsedTime	= fElapsedTime; 
	m_fGAge			+= fElapsedTime;
	m_fTimeSum		+= fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_fAlpha	= m_fAlphaStart;
			m_fHeight	= m_fHeightStart;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;

			if( m_dwFlag & USESCALE )	m_fSize = m_fSizeStart;
			else						m_fSize = 1.f;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	//if ( !m_pMesh || !m_pMesh->m_pLocalMesh )	goto _RETURN;

	//	Note: 회전 부분 이다.
	m_fAngle += fElapsedTime*m_fRotationAngle;
	if( m_fAngle > 2*D3DX_PI ) m_fAngle -= 2*D3DX_PI;

	//	Note : 사이즈 적용
	//
	if( m_dwFlag & USESCALE )
	{
		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fSizeTime1)
			m_fSize += m_fSizeDelta1*fElapsedTime;
		else if ( m_fGAge >= m_fSizeTime1 && m_fGAge < m_fSizeTime2)	
			m_fSize += m_fSizeDelta2*fElapsedTime;
		else														
			m_fSize += m_fSizeDelta3*fElapsedTime;

		if ( m_fSize < 0.f )		m_fSize = 0.f;
	}

	//	Note : 사이즈 조정 XYZ 각각 제어 가능
	//
	if( m_dwFlag & USESIZEXYZ )
	{
		m_vSizeXYZEnd.x = m_fSize*(1.f+m_vSizeXYZ.x*((m_fGAge-m_fGBeginTime)/m_fGLifeTime));
		m_vSizeXYZEnd.y = m_fSize*(1.f+m_vSizeXYZ.y*((m_fGAge-m_fGBeginTime)/m_fGLifeTime));
		m_vSizeXYZEnd.z = m_fSize*(1.f+m_vSizeXYZ.z*((m_fGAge-m_fGBeginTime)/m_fGLifeTime));
	}
	else
	{
		m_vSizeXYZEnd = D3DXVECTOR3 ( m_fSize, m_fSize, m_fSize );
	}

	//	Note : 컬러, 알파 조정 
	//
	{
		float fDelta;

		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fAlphaTime1)
			m_fAlpha += m_fAlphaDelta1*fElapsedTime;
		else if ( m_fGAge >= m_fAlphaTime1 && m_fGAge < m_fAlphaTime2)	
			m_fAlpha += m_fAlphaDelta2*fElapsedTime;
		else														
			m_fAlpha += m_fAlphaDelta3*fElapsedTime;

		if ( m_fAlpha < 0.f )		m_fAlpha = 0.f;
		if ( m_fAlpha > 1.f )		m_fAlpha = 1.f;

		fDelta = m_fGAge / (m_fGBeginTime + m_fGLifeTime);
		m_cColorVar = m_clrStart + fDelta*( m_clrEnd	 - m_clrStart );
		m_cColorVar.a = m_fAlpha;
	}

	//	Note : 텍스쳐 회전 적용
	//
	if ( m_dwFlag & USETEXROTATE )
	{
	}

	//	Note : 텍스쳐 이동 적용
	//
	if ( m_dwFlag & USETEXMOVE )
	{
		m_vTexVelSum.x += m_vTexVel.x*fElapsedTime;
		m_vTexVelSum.y += m_vTexVel.y*fElapsedTime;
		//if ( m_vTexVelSum.x > INT_MAX )		m_vTexVelSum.x = 0.f;	// 값이 넘어서지 않도록
		//if ( m_vTexVelSum.y > INT_MAX )		m_vTexVelSum.y = 0.f;	// 값이 넘어서지 않도록
		//if ( m_vTexVelSum.z > INT_MAX )		m_vTexVelSum.z = 0.f;	// 값이 넘어서지 않도록
	}

	//	Note : 텍스쳐 스케일 적용
	//
	if ( m_dwFlag & USETEXSCALE )
	{
		m_fTexScale		= m_fTexScaleDelta*fElapsedTime;
	}

	if ( m_dwFlag & USEHEIGHT_MESH )
	{
		m_fHeight += m_fHeightDelta*fElapsedTime;
	}

	//m_cColorVar = m_cColorStart;
	//m_cColorVar.a = m_fAlpha;	

	if ( m_dwFlag & USESEQUENCE )
	{
		if ( m_dwFlag & USESEQUENCELOOP )
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				if ( m_bSequenceRoop )	m_iNowSprite += 1;
				else					m_iNowSprite -= 1;
				m_fTimeSum = 0.0f;
			}

			if ( m_iNowSprite >= m_iAllSprite )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite -= 2;
			}
			else if ( m_iNowSprite < 0 )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite += 2;
			}
		}
		else
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				m_iNowSprite += 1;
				m_fTimeSum = 0.0f;
			}
			if ( m_iNowSprite >= m_iAllSprite )		m_iNowSprite = 0;
		}

		m_pSequenceUV[0].x = m_iNowSprite / (float)m_nCol;
		m_pSequenceUV[0].y = (m_iNowSprite / m_nCol) / (float)m_nRow;

		m_pSequenceUV[1].x = (m_iNowSprite+1) / (float)m_nCol;
		m_pSequenceUV[1].y = (m_iNowSprite / m_nCol) / (float)m_nRow;

		m_pSequenceUV[2].x = m_iNowSprite / (float)m_nCol;
		m_pSequenceUV[2].y = (m_iNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

		m_pSequenceUV[3].x = (m_iNowSprite+1) / (float)m_nCol;
		m_pSequenceUV[3].y = (m_iNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;
	}

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );
	return S_OK;
}

HRESULT DxEffectMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;

	// 지면 Tex를 얻어오고자 할 때.
	if( (m_dwFlag&USEGROUNDTEX) && (!m_textureResColl.GetTexture()) )	// 딱 한번만 체크한다.
	{
		boost::shared_ptr<DxLandMan> spLandMan = DxEffSingleMan::GetInstance().GetLandMan();
		if ( spLandMan )
		{
			BOOL		bColl;
			D3DXVECTOR3	vColl;
			LPDXFRAME	pDxFrame = NULL;
			const char* szName = NULL;

			D3DXVECTOR3 vStart(matComb._41,matComb._42,matComb._43);
			D3DXVECTOR3 vEnd=vStart;
			vStart.y += 0.01f;
			vEnd.y -= 1000.f;
			spLandMan->IsCollisionEX ( vStart, vEnd, vColl, bColl, pDxFrame, szName );
			if ( bColl && szName )
			{
				m_szCollTex = szName;

                m_textureResColl = TextureManager::GetInstance().LoadTexture(
                    szName,
                    false,
                    TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                    0,
                    D3DFMT_UNKNOWN,
                    TEXTURE_RESIZE_NONE,
                    TEXTURE_LAND,
                    true,
					FALSE );
			}
		}
	}

	float fTime;
	
	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	D3DXVECTOR3	vStartPos;
	D3DXQUATERNION qRotate;
	D3DXMATRIX matWorld, matRotate, matTrans, matScale, matChild;

	matTrans = m_matLocal;
	matTrans._41 *= m_rThisGroup->m_fOut_Scale;
	matTrans._42 *= m_rThisGroup->m_fOut_Scale;
	matTrans._43 *= m_rThisGroup->m_fOut_Scale;
	matWorld	= matTrans;
	vStartPos	= D3DXVECTOR3 (matTrans._41,matTrans._42,matTrans._43);

	//	Note : 센터로 이동
	//
	if ( m_dwFlag & USEGOTOCENTER )
	{
		fTime = m_fGBeginTime+m_fGLifeTime-m_fGAge;

		vDelta = vStartPos/m_fGLifeTime;

		matTrans._41 = vDelta.x*fTime;
		matTrans._42 = vDelta.y*fTime;
		matTrans._43 = vDelta.z*fTime;

		D3DXMatrixMultiply ( &matChild, &matTrans, &matComb );

		if ( (m_dwFlag & USEDIRECTION) && !(m_rThisGroup->m_dwFlag & EFF_CFG_NONE) )	// 목표없음
		{
			DxBillboardLookAtDir ( matTrans, &vDelta, &vStartPos );//m_pAffineParts->vTrans );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else if ( m_dwFlag & USEDIRECTION )	// 방향, ID, 계속,
		{
			//D3DXVec3TransformCoord ( &vStartPos, &vStartPos, &matComb );
			DxBillboardLookAtDir ( matTrans, &m_rThisGroup->m_TargetID.vPos, &vStartPos );//m_pAffineParts->vTrans );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else
		{
			matWorld = matChild;
		}
	}
	else
	{
		D3DXMatrixMultiply ( &matChild, &matTrans, &matComb );

		if ( (m_dwFlag & USEDIRECTION) && m_bMoveObj )
		{
			DxBillboardLookAtDir ( matTrans, &m_rThisGroup->m_vGDelta, &vDelta );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else if ( m_dwFlag & USEDIRECTION )
		{
			DxBillboardLookAtDir ( matTrans, &m_rThisGroup->m_vGDelta );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else if ( m_rThisGroup->m_bOut_Direct )
		{
			matTrans = m_rThisGroup->m_matOutRotate;
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else
		{
			matWorld = matChild;
		}
	} 

	//	Note : 외부 스케일이 적용 받는다.
	//	Note : 이 부분에 스케일링 부분이 와야 한다.
	if( m_dwFlag & USESCALE )
	{
		float fSize = m_fSize * m_rThisGroup->m_fOut_Scale;
		if ( fSize < 0.00001f )				goto _RETURN;

		D3DXMatrixScaling ( &matScale, fSize, fSize, fSize );
		D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld );
	}
	else
	{
		D3DXVECTOR3	vSize;
		vSize.x = m_vSizeXYZEnd.x * m_rThisGroup->m_fOut_Scale;
		vSize.y = m_vSizeXYZEnd.y * m_rThisGroup->m_fOut_Scale;
		vSize.z = m_vSizeXYZEnd.z * m_rThisGroup->m_fOut_Scale;

		if ( vSize.x < 0.001f )				goto _RETURN;
		if ( vSize.y < 0.001f )				goto _RETURN;
		if ( vSize.z < 0.001f )				goto _RETURN;

		D3DXMatrixScaling ( &matScale, vSize.x, vSize.y, vSize.z );
		D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld );
	}

	//	Note: 외부에서 방향을 주고 이동도 줬다.
	if( m_rThisGroup->m_bAutoMove )
	{
		float		fPer	= (m_fGAge-m_fGBeginTime)/m_fGLifeTime;
		matWorld._41 = m_rThisGroup->m_vStartPos.x + (m_rThisGroup->m_vDirect.x*fPer);
		matWorld._42 = m_rThisGroup->m_vStartPos.y + (m_rThisGroup->m_vDirect.y*fPer);
		matWorld._43 = m_rThisGroup->m_vStartPos.z + (m_rThisGroup->m_vDirect.z*fPer);
	}

	//	Note: 회전 플래그 
	if( m_dwFlag & USEROTATE )
	{
		D3DXVECTOR3 vNormal = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3	vTarget;
		D3DXVec3Normalize( &vTarget, &m_rThisGroup->m_vGDelta );

		switch( m_nRotationType )
		{
			case 0: //	Note: 목표물의 방행을 중심으로 회전
					D3DXQuaternionRotationAxis( &qRotate, &vTarget, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );	
					break;
						
			case 1:	//	Note: 목표물 방향의 각 노말 벡터를 중심으로 회전					
					vNormal.x = vTarget.x ;
					vNormal.z = vTarget.z ;
					if( vTarget.y == 0.0f ) vTarget.y = 1.0f;
					D3DXVec3Cross( &vNormal, &vTarget, &vNormal );
					D3DXQuaternionRotationAxis( &qRotate, &vNormal, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );		
					break;

			case 2:	
					vNormal.x = vTarget.x ;
					vNormal.y = vTarget.y ;
					if( vTarget.z == 0.0f ) vTarget.z = 1.0f;
					D3DXVec3Cross( &vNormal, &vTarget, &vNormal );
					D3DXQuaternionRotationAxis( &qRotate, &vNormal, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );		
					break;

			case 3:
					vNormal.y = vTarget.x ;
					vNormal.z = vTarget.z ;
					if( vTarget.x == 0.0f ) vTarget.x = 1.0f;
					D3DXVec3Cross( &vNormal, &vTarget, &vNormal );
					D3DXQuaternionRotationAxis( &qRotate, &vNormal, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );						
					break;

			case 4: 
					D3DXMatrixRotationX( &matRotate, m_fAngle );
					break;

			case 5:
					D3DXMatrixRotationY( &matRotate, m_fAngle );
					break;

			case 6:
					D3DXMatrixRotationZ( &matRotate, m_fAngle );
					break;
		}	

		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );	// 회전이 방향성 보다 먼저다. !!
		D3DXMatrixMultiply_RM2( matRotate, matWorld );	// 회전이 방향성 보다 먼저다. !!
	}

	if ( m_dwFlag & USEHEIGHT_MESH )
	{
		matWorld._42 += m_fHeight;
	}
	


	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;
	
	if ( !m_SimMeshRes.Update() )
		goto _RETURN;

	if ( m_dwFlag & USEBLENDMESH )
	{
		if ( !m_SimMeshBlendRes[0].Update() )
			goto _RETURN;
		if ( !m_SimMeshBlendRes[1].Update() )
			goto _RETURN;

		m_SimMeshRes.get()->SetBlendMesh( pd3dDevice, m_SimMeshBlendRes[0].get(), m_SimMeshBlendRes[1].get() );
	}
	//if ( !m_pMesh || !m_pMesh->m_pLocalMesh )	goto _RETURN;



	m_pSavedRenderStats->Capture();
	m_pDrawRenderStats->Apply();

	DWORD dwFogColor(0L);
	pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR, &dwFogColor );


	if( m_dwFlag&USECULLNONE )	pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 0 :	// Src+Dest (ADD)
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_ADD );
		break;
	case 1:		// Src-Dest (SUBTRACT)
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_SUBTRACT );
		break;
	case 2:		// Dest-Src (REVSUBTRACT)
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_REVSUBTRACT );
		break;
	case 3:		// Color MIN
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_MIN );
		break;
	case 4:		// Color MAX
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_MAX );
		break;
	case 5:		// Src+Dest (ALPHA)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_ADD );
		break;
	case 6:		// Src (MY)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		TRUE );
		break;
	}

	//	Note : 빛의 세기 정도
	//
	switch ( m_nPower )
	{
	case 0 :
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		break;
	case 1:
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
		break;
	case 2:
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );
		break;
	}


	D3DMATERIALQ	sMaterial;
	sMaterial.Diffuse.r = 1.f;
	sMaterial.Diffuse.g = 1.f;
	sMaterial.Diffuse.b = 1.f;
	sMaterial.Diffuse.a = 1.f;

	sMaterial.Ambient.r = 1.f;
	sMaterial.Ambient.g = 1.f;
	sMaterial.Ambient.b = 1.f;
	sMaterial.Ambient.a = 1.f;

	sMaterial.Specular.r = 1.f;
	sMaterial.Specular.g = 1.f;
	sMaterial.Specular.b = 1.f;
	sMaterial.Specular.a = 1.f;

	sMaterial.Emissive.r = 0.f;
	sMaterial.Emissive.g = 0.f;
	sMaterial.Emissive.b = 0.f;
	sMaterial.Emissive.a = 0.f;

	sMaterial.Power = 1.f;

	if( m_dwFlag&USENORMAL2 )
	{
		sMaterial.Diffuse.r = m_cColorVar.r;
		sMaterial.Diffuse.g = m_cColorVar.g;
		sMaterial.Diffuse.b = m_cColorVar.b;
		sMaterial.Diffuse.a = m_cColorVar.a*fAlpha;

		sMaterial.Ambient.r = m_cColorVar.r;
		sMaterial.Ambient.g = m_cColorVar.g;
		sMaterial.Ambient.b = m_cColorVar.b;
		sMaterial.Ambient.a = m_cColorVar.a*fAlpha;

		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	TRUE );
	}
	else
	{
		D3DCOLOR	dwColor = ((DWORD)(m_cColorVar.a*255.f*fAlpha)<<24) +
								((DWORD)(m_cColorVar.r*255.f)<<16) +
								((DWORD)(m_cColorVar.g*255.f)<<8) +
								(DWORD)(m_cColorVar.b*255.f);

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,	dwColor );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
	}


	//	Note: 메트릭스의 스케일과 머티리얼 보간 부분만 해주면 된다.
	//		  나머진 DxEffectGroup 부분에서 이동에 관한 부분을 메트릭스로 가져오기에 상관 없다.
	if( m_SimMeshRes.get()->GetLocalMesh() ) 
	{
		if ( m_dwFlag & USETEXMOVE )
		{
			D3DXMATRIX matTexture;
			D3DXMatrixIdentity( &matTexture );
			matTexture._31 = m_vTexVelSum.x;
			matTexture._32 = m_vTexVelSum.y;
			pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTexture );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
		}

		if ( m_dwFlag & USETEXSCALE )
		{

		}

		if( m_dwFlag & USEBLENDMESH )
		{
			float fFrame = cosf ( ((m_fGAge-m_fGBeginTime)/m_fGLifeTime)*D3DX_PI*m_fMorphRoopNum );
			m_SimMeshRes.get()->FrameMove ( fFrame );
		}

		if ( m_dwFlag & USESEQUENCE )
		{
			D3DXMATRIX matTexture;
			D3DXMatrixIdentity( &matTexture );
			matTexture._11 /= m_nCol;
			matTexture._22 /= m_nRow;
			matTexture._31 = static_cast<float>(m_iNowSprite)/m_nCol;
			matTexture._32 = static_cast<float>(m_iNowSprite/m_nCol)/m_nRow;
			pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTexture );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
		}

		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_matWorld = matWorld;

		if( m_textureResColl.Update() )
            m_SimMeshRes.get()->Render( pd3dDevice, m_textureResColl.GetTexture(), &sMaterial, m_dwFlag & USEBLENDMESH );
		else if( m_textureRes.GetTexture() )
            m_SimMeshRes.get()->Render( pd3dDevice, m_textureRes.GetTexture(), &sMaterial, m_dwFlag & USEBLENDMESH );
		else
			m_SimMeshRes.get()->Render( pd3dDevice, NULL, &sMaterial, m_dwFlag & USEBLENDMESH );

		//	Note : 추가 잔상을 만들기 위해서 ^^
		//
		if ( m_dwFlag & USEBLUR )
		{
			D3DXVECTOR3	vNormalize = m_rThisGroup->m_vGDelta;
			D3DXVec3Normalize ( &vNormalize, &vNormalize );
			vNormalize *= 20.f;

			for ( int	i=0; i<m_nBlurObject; i++ )
			{
				matWorld._41 -= vNormalize.x / m_nBlurObject;
				matWorld._42 -= vNormalize.y / m_nBlurObject;
				matWorld._43 -= vNormalize.z / m_nBlurObject;
				pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
				m_SimMeshRes.get()->Render( pd3dDevice, NULL, &sMaterial, m_dwFlag & USEBLENDMESH );
			}
		}

		if ( m_dwFlag & USETEXMOVE || m_dwFlag & USESEQUENCE )
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		}
	}

	pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR, dwFogColor );

	m_pSavedRenderStats->Apply();


_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 노드.
	//
	if( matWorld._44!=1.f)
	{
		float fSize = 1.f/matWorld._44;
		matWorld._11 = matWorld._11*fSize;	matWorld._12 = matWorld._12*fSize;	matWorld._13 = matWorld._13*fSize;
		matWorld._21 = matWorld._21*fSize;	matWorld._22 = matWorld._22*fSize;	matWorld._23 = matWorld._23*fSize;
		matWorld._31 = matWorld._31*fSize;	matWorld._32 = matWorld._32*fSize;	matWorld._33 = matWorld._33*fSize;
		matWorld._41 = matWorld._41*fSize;	matWorld._42 = matWorld._42*fSize;	matWorld._43 = matWorld._43*fSize;
		matWorld._44 = 1.f;
	}
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	return S_OK;	
}


HRESULT DxEffectMesh::FinalCleanup ()
{
	
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

