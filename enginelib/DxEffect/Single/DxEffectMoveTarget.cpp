#include "pch.h"
#include "../../../SigmaCore/Math/Random.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/EditMeshs.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingleMan.h"
#include "./DxEffectMoveTarget.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//	Note	:	디버그용,	
extern BOOL			g_bEFFtoTEX;
extern BOOL			g_bEffectAABBBox;

//---------------------------------------------------------------------[MOVETARGET_PROPERTY]
const DWORD	MOVETARGET_PROPERTY::VERSION	= 0x0101;
const char	MOVETARGET_PROPERTY::NAME[]	= "Move (Target)";

DxEffSingle* MOVETARGET_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffectMoveTarget *pEffect = new DxEffectMoveTarget;

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;

	//	Note : 시간 설정.			[공통설정]
	//
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	//
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_fObjectRadius = m_fObjectRadius;

	pEffect->m_vCustom_Velocity = m_vCustom_Velocity;
	pEffect->m_fRandom_Angel	= m_fRandom_Angel;
	pEffect->m_fGuided_Accel	= m_fGuided_Accel;

	StringCchCopy( pEffect->m_szFileName, MAX_PATH, m_szFileName );

	return pEffect;
}

HRESULT MOVETARGET_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : 부모 클레스의 정보.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : 형제, 자식 저장.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT MOVETARGET_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	if( dwVer == VERSION )
	{
		//	Note : 부모 클레스의 정보.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 읽기.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

HRESULT MOVETARGET_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

HRESULT MOVETARGET_PROPERTY::DeleteDeviceObjects ()
{
	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectMoveTarget::TYPEID	= EFFSINGLE_MOVETARGET;
const DWORD	DxEffectMoveTarget::FLAG	= NULL;
const char	DxEffectMoveTarget::NAME[]	= "Move (Target)";

//	Note :
//
DxEffectMoveTarget::DxEffectMoveTarget(void) :
	m_vGLocation(0,0,0),

	m_vCustom_Velocity(1.f, 0.5f, 0.f),
	m_fRandom_Angel(0.1f),
	m_fGuided_Accel(10.f),

	m_fElapsedTime(0.f),

	m_bGetInitPosDir(FALSE),

	m_vInitDir(0,0,0),

	m_SingleRender(FALSE),

	m_pd3dDevice(NULL)
{
	D3DXMatrixIdentity ( &m_matTrans );
}

DxEffectMoveTarget::~DxEffectMoveTarget(void)
{
}

HRESULT DxEffectMoveTarget::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK; 
}
HRESULT DxEffectMoveTarget::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

void DxEffectMoveTarget::OnInitDevice_STATIC()
{
}

void DxEffectMoveTarget::OnDeleteDevice_STATIC()
{
}

void DxEffectMoveTarget::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos = m_sData.m_vPos;

	if( vMax.x < vPos.x )	vMax.x = vPos.x;
	if( vMax.y < vPos.y )	vMax.y = vPos.y;
	if( vMax.z < vPos.z )	vMax.z = vPos.z;

	if( vMin.x > vPos.x )	vMin.x = vPos.x;
	if( vMin.y > vPos.y )	vMin.y = vPos.y;
	if( vMin.z > vPos.z )	vMin.z = vPos.z;

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
void DxEffectMoveTarget::CreateDxEffSingleGroup()
{
	if ( m_spSingleGroup.get() )
		return;

	if ( m_sPropGroupRes.Update() )	
	{	
		m_spSingleGroup.reset();						// 이전 데이터는 지워준다.
		m_spSingleGroup = m_sPropGroupRes.get()->NEWEFFGROUP ();
		if ( m_spSingleGroup.get() )
		{
			//	Note : Eff Group Create Dev.
			//
			m_spSingleGroup->Create ( m_pd3dDevice );
			m_spSingleGroup->m_matWorld = m_matTrans;
		}
	}
}

// DxEffSingleGroup 을 정리한다.
void DxEffectMoveTarget::DeleteDxEffSingleGroup()
{
	if ( !m_spSingleGroup.get() )
		return;

	m_spSingleGroup->CleanUp();
	m_spSingleGroup.reset();
}

HRESULT DxEffectMoveTarget::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	D3DXMatrixIdentity( &m_matTrans );

	// DxEffSingleGroup 을 정리한다.
	DeleteDxEffSingleGroup();

	// 이름이 있으면 작업함.
	if ( strlen(m_szFileName) )
	{
		m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_szFileName, TRUE, FALSE );

		// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
		CreateDxEffSingleGroup();
	}

	//	Note : 형제, 자식 호출
	DxEffSingle::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectMoveTarget::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->RestoreDeviceObjects ( pd3dDevice );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectMoveTarget::InvalidateDeviceObjects ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->InvalidateDeviceObjects ();

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffectMoveTarget::DeleteDeviceObjects ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->DeleteDeviceObjects ();

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffectMoveTarget::FinalCleanup ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->FinalCleanup();
	m_spSingleGroup.reset();
	//DxEffSinglePropGMan::GetInstance().DeleteEffectGProp ( m_szFileName );

	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

void DxEffectMoveTarget::ReStartEff ()
{
	m_fGAge			= 0.f;
	m_dwRunFlag		&= ~EFF_PLY_PLAY;
	m_dwRunFlag		&= ~EFF_PLY_DONE;
	m_dwRunFlag		&= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectMoveTarget::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;
	m_fElapsedTime = fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge			= 0.f;
			m_dwRunFlag		&= ~EFF_PLY_PLAY;
			m_dwRunFlag		&= ~EFF_PLY_DONE;
			m_dwRunFlag		&= ~EFF_PLY_END;
		}
	}

	// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
	CreateDxEffSingleGroup();

	if ( m_spSingleGroup.get() && m_SingleRender )
	{
		m_spSingleGroup->FrameMove ( fTime, fElapsedTime );
	}

	if ( m_dwRunFlag & EFF_PLY_RENDPASS )
	{
		if ( m_spSingleGroup.get() )
		{
			if ( m_spSingleGroup->m_dwAliveCount == 0 )
			{
				m_dwRunFlag |= EFF_PLY_END;		

				m_spSingleGroup->InvalidateDeviceObjects ();
				m_spSingleGroup->DeleteDeviceObjects ();
				m_spSingleGroup->FinalCleanup();
				m_spSingleGroup.reset();
			}
		}
		else	m_dwRunFlag |= EFF_PLY_END;
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	m_sData.FrameMove ( fElapsedTime );

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )
	{
		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_TIMEOVER) )		// 시간 종료에 의해 추가 Effect 생성
		{
			if ( m_spSingleGroup.get() )
                m_SingleRender = TRUE;
		}

		m_dwRunFlag |= EFF_PLY_RENDPASS;
	}

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectMoveTarget::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld = matComb;

	if ( m_spSingleGroup.get() && m_SingleRender )
	{
		m_spSingleGroup->m_matWorld = matWorld;
		m_spSingleGroup->m_matWorld._41 = m_sData.m_vPos.x;
		m_spSingleGroup->m_matWorld._42 = m_sData.m_vPos.y;
		m_spSingleGroup->m_matWorld._43 = m_sData.m_vPos.z;

		m_spSingleGroup->Render ( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
	}

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	//	Render
	if ( m_dwFlag & USE_S_TARGET )	// 목표한 위치 있음
	{
		if ( !Render_Target ( matWorld ) )	goto _RETURN;
	}
	else if ( m_dwFlag & USE_S_DIRECT )		Render_Direct ( matWorld );

	matWorld._41 = m_sData.m_vPos.x;
	matWorld._42 = m_sData.m_vPos.y;
	matWorld._43 = m_sData.m_vPos.z;

	// EFFECT TOOl 에서만 위치를 표시 나게 함.
	if( g_bEffectAABBBox )
	{
		if( !g_bEFFtoTEX )
		{
			EDITMESHS::RENDERSPHERE ( pd3dDevice, m_sData.m_vPos, m_fObjectRadius );

			if ( m_dwFlag & USE_S_TARGET )		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_rThisGroup->m_TargetID.vPos, m_fObjectRadius );
		}
	}

_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 노드.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	return S_OK;
}

// 목표한 위치 있음
BOOL	DxEffectMoveTarget::Render_Target ( D3DXMATRIX &matWorld )
{
	//	Note : 초기 위치, 방향을 얻어라.
	if ( !m_bGetInitPosDir )
	{
		D3DXVECTOR3	vDir;
		float		fPower;

		m_sData.m_vPos.x = matWorld._41;
		m_sData.m_vPos.y = matWorld._42;
		m_sData.m_vPos.z = matWorld._43;

		//	목표 지점을 지났는지 체크 하기 위하여 이것을 쓴다.
		m_vInitDir = m_sData.m_vPos - m_rThisGroup->m_TargetID.vPos;
		D3DXVec3Normalize ( &m_vInitDir, &m_vInitDir );

		////	쏴야 하는 각도를 생각해서 방향을 정해준다.
		////	해 보았는데. 위치도 잘 안 맞고, 힘이 쎄고 가까울 경우, 높이 올라가고, 힘이 적고 멀 경우 도달하지 못한다.
		//m_vDir = DXPHYSICAL::COMPUTE_DIR_V ( m_vPos, m_rThisGroup->m_TargetID.vPos, m_fVelocity );

		//	일정각도를 간단히 정해 준후.. 쏘아올리는 힘을 정한다.
		vDir = DXPHYSICAL::COMPUTE_DIR_H ( m_sData.m_vPos, m_rThisGroup->m_TargetID.vPos, 0.3f );
		fPower = DXPHYSICAL::COMPUTE_POWER ( m_sData.m_vPos, m_rThisGroup->m_TargetID.vPos, vDir );

		vDir = vDir * fPower;

		if ( m_dwFlag & USE_S_RANDOM )
		{
			vDir.x = vDir.x + (sc::Random::RANDOM_NUM()*m_fRandom_Angel);
			vDir.y = vDir.y + (sc::Random::RANDOM_NUM()*m_fRandom_Angel);
			vDir.z = vDir.z + (sc::Random::RANDOM_NUM()*m_fRandom_Angel);
		}

		// 위치, 속도, 반지름, 마찰력, 탄성도
		m_sData.InitData ( m_sData.m_vPos, vDir, m_fObjectRadius, 0.8f, 0.8f );

		//	초기 위치와 방향을 정했으니 다시 이 곳에 안 오도록 한다.
		m_bGetInitPosDir = TRUE;
	}

	if ( (m_dwFlag&USE_E_CRASH) || ( (m_dwFlag&USE_A_EFF)&&(m_dwFlag&USE_A_CRASH) ) )
	{
		// 목표 위치를 통과했는지 체크해라.
		if ( DXPHYSICAL::CHECK_PASS ( m_vInitDir, m_sData.m_vPos, m_rThisGroup->m_TargetID.vPos ) )
		{
			// 목표 위치에 도달하면 끝내라.
			if ( m_dwFlag&USE_E_CRASH )	m_dwRunFlag |= EFF_PLY_RENDPASS;

			// 목표 위치에 도달하면 추가 Effect 생성
			if ( (m_dwFlag&USE_A_EFF)&&(m_dwFlag&USE_A_CRASH) )
			{
				if ( m_spSingleGroup.get() )
                    m_SingleRender = TRUE;
			}
		}
	}

	//	처음 운동
	if ( m_dwFlag&USE_S_GUIDED )	m_sData.Render ( m_rThisGroup->m_TargetID.vPos, m_fGuided_Accel );
	else							m_sData.Render ();

	//  최고점에서 추가 효과 생성
	if ( m_sData.IsCheckBestHigh() )
	{
		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_HIGH) )		//충돌에 의해 추가 Effect 생성
		{
			if ( m_spSingleGroup.get() )
                m_SingleRender = TRUE;
		}
	}

	//	충돌
	if ( m_dwFlag & USE_GROUND_CRASH )		Crash(0);

	//	움직일 수 없을 때의 작동
	if ( m_sData.IsCheckDontMove() )
	{
		if (m_dwFlag&USE_E_DONTMOVE)	m_dwRunFlag |= EFF_PLY_RENDPASS;

		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_DONTMOVE) )
		{
			if ( m_spSingleGroup.get() )
                m_SingleRender = TRUE;
		}
	}

	return TRUE;
}

// 목표한 방향으로..
void	DxEffectMoveTarget::Render_Direct ( D3DXMATRIX &matWorld )
{
	//	Note : 초기 위치, 방향을 얻어라.
	if ( !m_bGetInitPosDir )
	{
		D3DXVECTOR3 vPos;
		vPos.x = matWorld._41;
		vPos.y = matWorld._42;
		vPos.z = matWorld._43;

		D3DXMATRIX	matRotate;
		matRotate._11 = m_rThisGroup->m_matWorld._11;
		matRotate._12 = m_rThisGroup->m_matWorld._12;
		matRotate._13 = m_rThisGroup->m_matWorld._13;
		matRotate._14 = m_rThisGroup->m_matWorld._14;

		matRotate._21 = m_rThisGroup->m_matWorld._21;
		matRotate._22 = m_rThisGroup->m_matWorld._22;
		matRotate._23 = m_rThisGroup->m_matWorld._23;
		matRotate._24 = m_rThisGroup->m_matWorld._24;

		matRotate._31 = m_rThisGroup->m_matWorld._31;
		matRotate._32 = m_rThisGroup->m_matWorld._32;
		matRotate._33 = m_rThisGroup->m_matWorld._33;
		matRotate._34 = m_rThisGroup->m_matWorld._34;

		matRotate._41 = 0.f;
		matRotate._42 = 0.f;
		matRotate._43 = 0.f;
		matRotate._44 = m_rThisGroup->m_matWorld._34;

		if ( m_dwFlag & USE_S_RANDOM )
		{
			m_vCustom_Velocity.x = m_vCustom_Velocity.x + (sc::Random::RANDOM_NUM()*m_fRandom_Angel);
			m_vCustom_Velocity.y = m_vCustom_Velocity.y + (sc::Random::RANDOM_NUM()*m_fRandom_Angel);
			m_vCustom_Velocity.z = m_vCustom_Velocity.z + (sc::Random::RANDOM_NUM()*m_fRandom_Angel);
		}

		D3DXVec3TransformNormal ( &m_vCustom_Velocity, &m_vCustom_Velocity, &matRotate );	// 캐릭터에 맞게 회전 시킨다.

		//D3DXVec3Normalize ( &m_vCustom_Dir, &m_vCustom_Dir );

		//m_vCustom_Dir = m_vCustom_Dir * m_fCustom_Vel;

		m_sData.InitData ( vPos, m_vCustom_Velocity, m_fObjectRadius, 0.8f, 0.8f );

		//	초기 위치와 방향을 정했으니 다시 이 곳에 안 오도록 한다.
		m_bGetInitPosDir = TRUE;
	}

	//	처음 운동
	m_sData.Render ();

	//  최고점에서 추가 효과 생성
	if ( m_sData.IsCheckBestHigh() )
	{
		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_HIGH) )		//충돌에 의해 추가 Effect 생성
		{
			if ( m_spSingleGroup.get() )
                m_SingleRender = TRUE;
		}
	}

	//	충돌
	if ( m_dwFlag & USE_GROUND_CRASH )		Crash (0);

	//	움직일 수 없을 때의 작동
	if ( m_sData.IsCheckDontMove() )
	{
		if (m_dwFlag&USE_E_DONTMOVE)	m_dwRunFlag |= EFF_PLY_RENDPASS;

		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_DONTMOVE) )
		{
			if ( m_spSingleGroup.get() )
                m_SingleRender = TRUE;
		}
	}
}

void	DxEffectMoveTarget::Crash ( int nColl )
{
//	if ( m_sData.IsCheckDontMove() )	return;

	CDebugSet::ToView ( 10, "%f, %f, %f", m_sData.m_vPos.x, m_sData.m_vPos.y, m_sData.m_vPos.z );

	// 구멍에 뚫려서 지하로 떨어진 것이다.. ㅋ
	if ( m_sData.m_vPos.y < -100000.f )
	{
		m_sData.SetDontMove();
		return;
	}

	boost::shared_ptr<DxLandMan> spLandMan = DxEffSingleMan::GetInstance().GetLandMan();
	if ( spLandMan )
	{
		BOOL		bCollision;
		D3DXVECTOR3	vCollision;
		D3DXVECTOR3	vReflectNormal;
		LPDXFRAME	pDxFrame = NULL;

		D3DXVECTOR3 vPos_1  = m_sData.m_vPos;
		D3DXVECTOR3 vPos_2  = m_sData.m_vPos_Prev;
		D3DXVECTOR3 vDir	= vPos_1 - vPos_2;
		D3DXVec3Normalize ( &vDir, &vDir );
		D3DXVECTOR3 vVector = (vDir*m_sData.m_fRadius*10.f);	// 각도가 낮으면 낮을수록 잘 안나온다..
		vPos_1 += vVector;

		D3DXVECTOR3 vLeft, vRight;
		D3DXVECTOR3 vUp	= D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		D3DXVec3Cross ( &vLeft, &vUp, &vDir );
		D3DXVec3Cross ( &vRight, &vLeft, &vDir );

		const char* szName = NULL;
		spLandMan->IsCollisionEX ( vPos_2, vPos_1, vCollision, bCollision, pDxFrame, szName, &vReflectNormal );

		if ( bCollision )
		{
			if ( m_sData.IsReflect ( vCollision, vReflectNormal, vVector, nColl ) )
			{
				if (m_dwFlag&USE_E_CRASH)	m_dwRunFlag |= EFF_PLY_RENDPASS;

				if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_CRASH) )		//충돌에 의해 추가 Effect 생성
				{
					if ( m_spSingleGroup.get() )
                        m_SingleRender = TRUE;
				}

				// 만약 충돌이 있다면 또 다시 충돌이 생기는 것을 체크 하기 위함.
				++nColl;
				Crash (nColl);		// 재귀호출이다.  계속 충돌이 생긴다면 ?? 문제 생길텐데.음.
			}
		}
	}
}
