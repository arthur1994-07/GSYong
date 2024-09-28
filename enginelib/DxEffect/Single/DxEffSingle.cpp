#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxCommon9/DXUTmisc.h"

#include "../Frame/DxEffectMan.h"

#include "./DxEffectParticleSys.h"
#include "./DxEffectGround.h"
#include "./DxEffectMesh.h"
#include "./DxEffectSequence.h"
#include "./DxEffectDecal.h"
#include "./DxEffectCamera.h"
#include "./DxEffectBlurSys.h"
#include "./DxEffectLighting.h"
#include "./DxEffectMoveRotate.h"
#include "./DxEffectSkinMesh.h"
#include "./DxEffectWave.h"
#include "./DxEffectPointLight.h"
#include "./DxEffectMoveTarget.h"
#include "./DxEffSingle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL	g_bEffectAABBBox	= FALSE;
BOOL	g_bEFFtoTEX			= FALSE;

extern BOOL	g_bEFFECT_TOOL;

//---------------------------------------------------------------------- [ DxEffSingle ]

BOOL DxEffSingle::IsReadyToDie ()
{
	return ( m_fGAge>=(m_fGBeginTime+m_fGLifeTime) );
}

void DxEffSingle::ReStartEff ()
{
	m_fGAge = 0.f;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;
	m_dwRunFlag &= ~EFF_PLY_RENDPASS;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffSingle::FrameMove ( float fTime, float fElapsedTime )
{
	GASSERT(m_rThisGroup);

	//	사운드 업데이트
	m_MovSound.Update ( m_rThisGroup->m_vGNowPos, m_fGBeginTime, m_fGAge, m_fGLifeTime );

	if ( m_pChild )
	{
		if ( !( m_pChild->m_dwRunFlag & EFF_PLY_END ) )		++m_rThisGroup->m_dwAliveCount;
		m_pChild->FrameMove ( fTime, fElapsedTime );
	}

	if ( m_pSibling )
	{
		if ( !( m_pSibling->m_dwRunFlag & EFF_PLY_END ) )	++m_rThisGroup->m_dwAliveCount;
		m_pSibling->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

HRESULT	DxEffSingle::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	GASSERT(0&&"DxEffSingle::Render() not defined!");

	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matComb, fAlpha );
	//if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha ); -- [ERROR CODE]

	return S_OK;
}

HRESULT DxEffSingle::OneTimeSceneInit ()
{
	if ( m_pChild )		m_pChild->OneTimeSceneInit ();
	if ( m_pSibling )	m_pSibling->OneTimeSceneInit ();

	return S_OK;
}

HRESULT DxEffSingle::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pChild )		m_pChild->InitDeviceObjects ( pd3dDevice );
	if ( m_pSibling )	m_pSibling->InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_rThisGroup->m_dwFlag & EFF_CFG_NEVERDIE )
	{
		if ( m_rThisGroup->m_fGBeginTime > m_fGBeginTime )	m_rThisGroup->m_fGBeginTime = m_fGBeginTime;
		if ( m_rThisGroup->m_fGLifeTime < m_fGLifeTime )	m_rThisGroup->m_fGLifeTime = m_fGLifeTime;
	}
	if ( m_bMoveObj )
	{
		m_rThisGroup->m_fGBeginTime = m_fGBeginTime;
		m_rThisGroup->m_fGLifeTime = m_fGLifeTime;
	}

	if ( m_pChild )		m_pChild->RestoreDeviceObjects ( pd3dDevice );
	if ( m_pSibling )	m_pSibling->RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingle::InvalidateDeviceObjects ()
{
	if ( m_pChild )		m_pChild->InvalidateDeviceObjects ();
	if ( m_pSibling )	m_pSibling->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffSingle::DeleteDeviceObjects ()
{
	if ( m_pChild )		m_pChild->DeleteDeviceObjects ();
	if ( m_pSibling )	m_pSibling->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffSingle::FinalCleanup ()
{
	if ( m_pChild )		m_pChild->FinalCleanup ();
	if ( m_pSibling )	m_pSibling->FinalCleanup ();

	PutSleepMemory( m_pChild );
	PutSleepMemory( m_pSibling );
//	SAFE_DELETE(m_pChild);
//	SAFE_DELETE(m_pSibling);

	return S_OK;
}

//void DxEffSingle::SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert )
//{
//	if( m_pChild )		m_pChild->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//	if( m_pSibling )	m_pSibling->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//}

void DxEffSingle::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if( m_pChild )		m_pChild->SetWorldBattleFlag( pWorldBattleFlag );
 	if( m_pSibling )	m_pSibling->SetWorldBattleFlag( pWorldBattleFlag );
}

//---------------------------------------------------------------------- [ DxEffSingleGroup ]

int DxEffSingleGroup::g_nEFF_COUNT = 0;

void DxEffSingleGroup::SetTargetID ( const STARGETID* pTargetID )
{
	if ( pTargetID )	m_TargetID = *pTargetID;
}

void DxEffSingleGroup::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = *m_pMax;	vMin = *m_pMin;
}

BOOL DxEffSingleGroup::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	return COLLISION::IsWithInPoint ( vDivMax, vDivMin, vPos );
}

void DxEffSingleGroup::MovePos( const D3DXVECTOR3 &vTarget )
{
	if ( m_dwFlag & EFF_CFG_AVERAGE )			// 일정한 속도로 이동.
	{
		m_vGDelta.x = (vTarget.x-m_matWorld._41) / m_fGLifeTime;	// 지금것은 따라가는 것
		m_vGDelta.y = (vTarget.y-m_matWorld._42) / m_fGLifeTime;
		m_vGDelta.z = (vTarget.z-m_matWorld._43) / m_fGLifeTime;

		m_vGNowPos.x = m_matWorld._41 + m_vGDelta.x*(m_fGAge-m_fGBeginTime);
		m_vGNowPos.y = m_matWorld._42 + m_vGDelta.y*(m_fGAge-m_fGBeginTime);
		m_vGNowPos.z = m_matWorld._43 + m_vGDelta.z*(m_fGAge-m_fGBeginTime);
	}
	else if ( m_dwFlag & EFF_CFG_SLOWFAST )		// 점점 빨라짐.
	{
		float fRate = m_fGAge/m_fGLifeTime;
		fRate = powf(fRate,3);

		m_vGDelta = vTarget - m_vStartPos;

		m_vGNowPos = m_vStartPos + m_vGDelta*fRate;
	}
	else if ( m_dwFlag & EFF_CFG_FASTSLOW )		// 점점 느려짐.
	{
		float fRate = (m_fGLifeTime-m_fGAge)/m_fGLifeTime;
		fRate = powf(fRate,3);
		fRate = 1.f - fRate;

		m_vGDelta = vTarget - m_vStartPos;

		m_vGNowPos = m_vStartPos + m_vGDelta*fRate;
	}
}

HRESULT DxEffSingleGroup::FrameMove ( float fTime, float fElapsedTime )
{
	m_dwAliveCount = 0;

	// [shhan][2015.06.19] 2fps -> 0.9fps 으로 일단 변경됨.
	//						변경이유는 CD3DApplication::FrameMove3DEnvironment 쪽 Sleep(1000) 으로 인해 Effect 들이 지워지지 않고 33배가 중첩되어 버린다. 
	//						1초당 1개씩이지만 시간은 0.03 간게 되어버러서 삭제가 30배 후에 됨.
	//						1fps 으로 하면 엇박자가 생길꺼 같아서 0.9 로 함.
	//
	////Note : <이전주석> 2 fps  극악 속도, 처음 로딩시 많이 발생. 
	////
	if ( fElapsedTime > 1.1f )		fElapsedTime = 0.03f;

	m_fElapsedTime = fElapsedTime;

	if ( m_pEffSingleRoot )
	{
		//	효과 그룹 속성에 따라 하는 행동 변화	- 위치 지정
		//
		if ( m_dwFlag & EFF_CFG_TARGETPOS )			// 방향
		{
			//	m_TargetID	: 목표 위치
			//	m_pmatLocal : 발산 위치

			//	Note : Eff의 나이를 계산.
			//
			m_fGAge += fElapsedTime;

			if ( m_fGAge > m_fGBeginTime && m_fGAge < m_fGBeginTime + m_fGLifeTime )
			{
				MovePos( m_TargetID.vPos );
			}
			else
			{
				m_vGNowPos.x = m_matWorld._41;
				m_vGNowPos.y = m_matWorld._42;
				m_vGNowPos.z = m_matWorld._43;
			}
		}
		else if ( m_dwFlag & EFF_CFG_TAGERT_CAMERA_MOVE )			//  EFFECT가 타겟 카메라까지 이동.
		{
			D3DXVECTOR3 vScreenPos( DxViewPort::GetInstance().GetFromPt() + DxViewPort::GetInstance().GetLookDir() * 6.f );

			//	DxViewPort::GetPosition	: 목표 위치
			//	m_pmatLocal				: 발산 위치

			//	Note : Eff의 나이를 계산.
			//
			m_fGAge += fElapsedTime;

			if ( m_fGAge > m_fGBeginTime && m_fGAge < m_fGBeginTime + m_fGLifeTime )
			{
				MovePos( vScreenPos );
			}
			else
			{
				m_vGNowPos.x = m_matWorld._41;
				m_vGNowPos.y = m_matWorld._42;
				m_vGNowPos.z = m_matWorld._43;
			}
		}
		else if ( m_dwFlag & EFF_CFG_TARGETID )		// ID
		{
			D3DXVECTOR3	vPos = DxEffectMan::GetInstance().GetCrowPos ( m_TargetID );
			m_vGNowPos = vPos;
			// m_matTarget = DxEffectMan::GetInstance().GetCrowTransMatrix(m_TargetID);
		}
		else if ( m_dwFlag & EFF_CFG_TARGETLINE )
		{
			m_vGNowPos = m_TargetID.vPos;
		}
		else
		{
			m_vGNowPos.x = m_matWorld._41;
			m_vGNowPos.y = m_matWorld._42;
			m_vGNowPos.z = m_matWorld._43;
		}

		if ( !m_TargetID.ISNULL() && !(m_dwFlag & ( EFF_CFG_TARGETPOS|EFF_CFG_TAGERT_CAMERA_MOVE)) )
		{
			D3DXVECTOR3	vPos = DxEffectMan::GetInstance().GetCrowPos ( m_TargetID );
			m_vGNowPos = vPos;
			m_matTarget = DxEffectMan::GetInstance().GetCrowTransMatrix(m_TargetID);
		}


		if ( !( m_pEffSingleRoot->m_dwRunFlag & EFF_PLY_END ) )		++m_dwAliveCount;
		m_pEffSingleRoot->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

HRESULT	DxEffSingleGroup::Render(LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vScale, float fAlpha )
{
	if ( !m_pEffSingleRoot )		
		return S_OK;

	if ( !m_bOut_Particle )			
		return S_OK;

	D3DMATERIALQ	pMaterial;
	D3DUtil_InitMaterial ( pMaterial, 1.0f, 1.0f, 1.0f, 1.0f );
	pd3dDevice->SetMaterial ( &pMaterial );

	D3DXMATRIX matTrans;

	if ( m_bOut_Direct )
	{

	}

	if ( m_dwFlag & EFF_CFG_TARGETPOS )			// 방향
	{
		D3DXMatrixIdentity ( &matTrans );
		matTrans._41 = m_vGNowPos.x;
		matTrans._42 = m_vGNowPos.y;
		matTrans._43 = m_vGNowPos.z;
	}
	else if ( m_dwFlag & EFF_CFG_TAGERT_CAMERA_MOVE )			//  EFFECT가 타겟 카메라까지 이동.
	{
		matTrans = DxViewPort::GetInstance().GetMatView();
		matTrans._41 = m_vGNowPos.x;
		matTrans._42 = m_vGNowPos.y;
		matTrans._43 = m_vGNowPos.z;
	}
	else if ( m_dwFlag & EFF_CFG_TARGETID )		// ID
	{
		matTrans = m_matWorld;				// 회전 매트릭스가 들어있다.
		matTrans._41 = m_vGNowPos.x;
		matTrans._42 = m_vGNowPos.y;
		matTrans._43 = m_vGNowPos.z;
	}
	//else if ( m_dwFlag & EFF_CFG_TARGETLINE )
	//{
	//	matTrans = m_matWorld;				// 회전 매트릭스가 들어있다.
	//}
	else
	{
		matTrans = m_matWorld;
	}

	matTrans._42 += m_vLocal.y;


	D3DXVECTOR3 vMin = *m_pMin, vMax = *m_pMax;
	CLIPVOLUME &cCV = DxViewPort::GetInstance().GetClipVolume();

	D3DXMATRIX matWorld;
	D3DXMatrixScaling( &matWorld, vScale.x, vScale.y, vScale.z );
	matWorld *= matTrans;

	//	Note : AABB 박스 계산.
	//
	COLLISION::TransformAABB ( vMax, vMin, matWorld );

	BOOL bCOL = COLLISION::IsCollisionVolume ( cCV, vMax, vMin );
	if ( !bCOL )	
		return S_FALSE;

	//EDITMESHS::RENDERAABB ( pd3dDevice, vMax, vMin );

	m_pEffSingleRoot->Render ( pd3dDevice, matWorld, fAlpha );

	
	if( !g_bEFFtoTEX )
	{
		// Note : 효과툴인가 ?, 
		if( g_bEFFECT_TOOL )
		{
			if(m_dwFlag&EFF_CFG_AUTOAABBBOX)
			{
				// 자동 AABB Box가 설정 되어 있는가 ?
				m_pEffSingleRoot->CheckAABBBox( *m_pMax, *m_pMin );
			}
		}

		// AABB 박스의 표시
		if( g_bEffectAABBBox )	EDITMESHS::RENDERAABB( pd3dDevice, *m_pMax, *m_pMin );
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::OneTimeSceneInit ()
{
	m_vStartPos.x = m_matWorld._41;
	m_vStartPos.y = m_matWorld._42;
	m_vStartPos.z = m_matWorld._43;

	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->OneTimeSceneInit ();
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::InvalidateDeviceObjects ()
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::DeleteDeviceObjects ()
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->DeleteDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::FinalCleanup ()
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->FinalCleanup ();
	}

	PutSleepMemory( m_pEffSingleRoot );
	//SAFE_DELETE(m_pEffSingleRoot);

	return S_OK;
}

void DxEffSingleGroup::ReStartEff ()
{
	m_fGAge = 0.f;

	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->ReStartEff ();
	}
}

void DxEffSingleGroup::PutSleepMemory( PDXEFFSINGLE& pSingle )
{
    SAFE_DELETE( pSingle );
}

////! 승리한 클럽의 국기를 맵에 셋팅한다.
//void DxEffSingleGroup::SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert )
//{
//	if ( m_pEffSingleRoot )
//	{
//		m_pEffSingleRoot->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//	}
//}

void DxEffSingleGroup::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->SetWorldBattleFlag( pWorldBattleFlag );
	}
}

void DxEffSingle::PutSleepMemory( PDXEFFSINGLE& pSingle )
{
    SAFE_DELETE( pSingle );
}