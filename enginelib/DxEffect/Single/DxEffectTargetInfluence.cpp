#include "pch.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "./DxEffSingleMan.h"
#include "./DxEffectTargetInfluence.h"

#include "../../../SigmaCore/DebugInclude.h"

const DWORD	TARGETINFLUENCE_PROPERTY::VERSION	= 0x0100;
const char	TARGETINFLUENCE_PROPERTY::NAME[]	= "타겟 영향 설정";

DxEffSingle* TARGETINFLUENCE_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffectTargetInfluence* pEffect = new DxEffectTargetInfluence;
	// TransPorm Set.		[공통설정];
	pEffect->m_matLocal = m_matLocal;
	// 시간 설정.			[공통설정];
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	// 날아가는 오브젝트	[공통설정];
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFLAG = m_dwFLAG;
	return pEffect;
}

HRESULT TARGETINFLUENCE_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	if ( dwVer == VERSION )
	{
		// 부모 클레스의 정보;
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );

		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		// 이팩트의 Property 를 저장;
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		// 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행;
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}
	
	// Device 자원을 생성;
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	// 형제, 자식 읽기;
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, FALSE );

	return S_OK;
}

HRESULT TARGETINFLUENCE_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	// DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨;
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	// 부모 클레스의 정보;
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );

	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	// 이팩트의 Property 를 저장;
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	EFF_PROPERTY::SaveFile ( SFile );
	return S_OK;
}

HRESULT TARGETINFLUENCE_PROPERTY::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache)
{
	// 형제, 자식;
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );
	return S_OK;
}

HRESULT TARGETINFLUENCE_PROPERTY::DeleteDeviceObjects()
{	
	// 형제, 자식;
	EFF_PROPERTY::DeleteDeviceObjects ();
	return S_OK;
}

const DWORD	DxEffectTargetInfluence::TYPEID		= EFFSINGLE_TARGETINFLUENCE;
const DWORD	DxEffectTargetInfluence::FLAG		= NULL;
const char	DxEffectTargetInfluence::NAME[]		= "타겟 영향 설정";

DxEffectTargetInfluence::DxEffectTargetInfluence(void)
: m_dwFLAG(0)
, m_pd3dDevice(NULL)
{
	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectTargetInfluence::~DxEffectTargetInfluence(void)
{

}

HRESULT DxEffectTargetInfluence::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK; 
}

HRESULT DxEffectTargetInfluence::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}
void DxEffectTargetInfluence::OnInitDevice_STATIC(){}
void DxEffectTargetInfluence::OnDeleteDevice_STATIC(){}

HRESULT DxEffectTargetInfluence::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	// 형제, 자식 호출;
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );
	return S_OK;
}

HRESULT DxEffectTargetInfluence::InvalidateDeviceObjects ()
{
	// 형제, 자식 호출;
	DxEffSingle::InvalidateDeviceObjects ();
	return S_OK;
}

HRESULT DxEffectTargetInfluence::FrameMove ( float fTime, float fElapsedTime )
{
	// Eff의 나이를 계산;
	m_fGAge += fElapsedTime;

	// 반복할 때 시간 및 여러 값을 초기화 한다;
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

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	// 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴;
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		
			m_dwRunFlag |= EFF_PLY_PLAY;
		else
			goto _RETURN;
	}

	// 종료 시점이 되었는지 검사;
	if ( IsReadyToDie() )
		m_dwRunFlag |= EFF_PLY_END;

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );
	return S_OK;
} 

HRESULT DxEffectTargetInfluence::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	D3DXMATRIX matWorld;		D3DXMatrixIdentity( &matWorld );
	
	if ( m_dwFLAG & USEPARENTROTATE )
	{
		D3DXMatrixMultiply_RM2( m_rThisGroup->m_matTarget, matWorld );
		matWorld._41 = matComb._41;
		matWorld._42 = matComb._42;
		matWorld._43 = matComb._43;
	}

	// 형제, 자식 노드 렌더;
	m_matWorld = matWorld;
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );
	return S_OK;
}

HRESULT DxEffectTargetInfluence::FinalCleanup()
{
	// 형제 자식 노드;
	DxEffSingle::FinalCleanup ();
	return S_OK;
}

void DxEffectTargetInfluence::ReStartEff ()
{
	m_fGAge			= 0.f;
	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

void DxEffectTargetInfluence::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}