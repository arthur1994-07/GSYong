#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"
#include "../G-Logic/GLPeriod.h"
#include "../DxTools/DxMethods.h"
#include "../DxTools/EditMeshs.h"
#include "../DxTools/DxViewPort.h"

#include "./DxLandEff.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL					g_bWORLD_TOOL;

const DWORD DXLANDEFF::VERSION = 0x0103;
DWORD DXLANDEFF::SIZE = sizeof(char)*MAX_PATH +
						sizeof(D3DXMATRIX) + 
						sizeof(char)*DXLANDEFF::LANDEFF_MAXSZ;

float DXLANDEFF::m_fTime = 0.0f;
float DXLANDEFF::m_fElapsedTime = 0.0f;

DXLANDEFF::DXLANDEFF(void)
    : m_vMax(0,0,0)
	, m_vMin(0,0,0)
    , m_fColorPowerDay(1.f)
    , m_fColorPowerNight(1.f)
	//, m_pPropGroup(NULL)
	, m_pNext(NULL)
    , m_bRender(true)
{
	StringCchCopy( m_szName, LANDEFF_MAXSZ, "이름없음" );

	D3DXMatrixIdentity( &m_matWorld );
	memset( m_szFileName, 0, sizeof(char)*MAX_PATH );
}

DXLANDEFF::~DXLANDEFF(void)
{
	SAFE_DELETE(m_pNext);
}

HRESULT DXLANDEFF::SetEffect ( char* szName, const D3DXMATRIX &matWorld, const char* szFile, float fColorPowerDay, float fColorPowerNight, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	GASSERT(szName);
	GASSERT(szFile);

	StringCchCopy( m_szName, LANDEFF_MAXSZ, szName );
	StringCchCopy( m_szFileName, MAX_PATH, szFile );

    m_fColorPowerDay    = fColorPowerDay;
    m_fColorPowerNight  = fColorPowerNight;

	m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( szFile, FALSE, FALSE );
	if ( !m_sPropGroupRes.IsValid() )	
		return E_FAIL;

	m_vMax = m_sPropGroupRes.get()->m_vMax;
	m_vMin = m_sPropGroupRes.get()->m_vMin;
	m_matWorld = matWorld;
	COLLISION::TransformAABB ( m_vMax, m_vMin, matWorld );

	if ( m_spSingleGroup.get() )
	{
		hr = PlaySingleGroup ( pd3dDevice );
		if (FAILED(hr) )	return hr;
	}

	return S_OK;
}

void DXLANDEFF::ReSetAABBBox()
{
	if( !m_sPropGroupRes.IsValid() )	return;

	m_vMax = m_sPropGroupRes.get()->m_vMax;
	m_vMin = m_sPropGroupRes.get()->m_vMin;
	COLLISION::TransformAABB( m_vMax, m_vMin, m_matWorld );
}

HRESULT DXLANDEFF::Load_0100 ( sc::BaseStream	&SFile )
{
	HRESULT hr = S_OK;

	DXAFFINEPARTS	m_sAffineParts;

	SFile.ReadBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
	SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
	SFile.ReadBuffer ( &m_sAffineParts, sizeof(DXAFFINEPARTS) );

	D3DXMatrixCompX ( m_matWorld, m_sAffineParts );

	// Max, Min 때문에 ThreadLoading 을 사용 못하고 있다.
	m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName, FALSE, FALSE );
	if ( m_sPropGroupRes.IsValid() )
	{
		m_vMax = m_sPropGroupRes.get()->m_vMax;
		m_vMin = m_sPropGroupRes.get()->m_vMin;
		COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );
	}
	else
	{
		CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] 단독 이팩트의 설정파일을 읽어오는데 실패하였습니다.",
			m_szName, m_szFileName );

		hr = E_FAIL;
	}

	return hr;
}

HRESULT DXLANDEFF::Load_0101 ( sc::BaseStream	&SFile )
{	
	HRESULT hr = S_OK;

	SFile.ReadBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
	SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
	SFile.ReadBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

	// Max, Min 때문에 ThreadLoading 을 사용 못하고 있다.
	m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName, FALSE, FALSE );
	if ( m_sPropGroupRes.IsValid() )
	{
		m_vMax = m_sPropGroupRes.get()->m_vMax;
		m_vMin = m_sPropGroupRes.get()->m_vMin;
		COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );
	}
	else
	{
		CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] 단독 이팩트의 설정파일을 읽어오는데 실패하였습니다.",
			m_szName, m_szFileName );

		hr = E_FAIL;
	}

	return hr;
}

HRESULT DXLANDEFF::Load_0102 ( sc::BaseStream	&SFile )
{
	HRESULT hr = S_OK;

	SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
	SFile.ReadBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
	SFile.ReadBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

	// Max, Min 때문에 ThreadLoading 을 사용 못하고 있다.
	m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName, FALSE, FALSE );
	if ( m_sPropGroupRes.IsValid() )
	{
		m_vMax = m_sPropGroupRes.get()->m_vMax;
		m_vMin = m_sPropGroupRes.get()->m_vMin;
		COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );
	}
	else
	{
		CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] 단독 이팩트의 설정파일을 읽어오는데 실패하였습니다.",
			m_szName, m_szFileName );

		hr = E_FAIL;
	}
	
	return hr;
}

HRESULT DXLANDEFF::Load_0103 ( sc::BaseStream	&SFile )
{
	HRESULT hr = S_OK;

    SFile >> m_fColorPowerDay;
    SFile >> m_fColorPowerNight;

	SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
	SFile.ReadBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
	SFile.ReadBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

	// Max, Min 때문에 ThreadLoading 을 사용 못하고 있다.
	m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName, FALSE, FALSE );
	if ( m_sPropGroupRes.IsValid() )
	{
		m_vMax = m_sPropGroupRes.get()->m_vMax;
		m_vMin = m_sPropGroupRes.get()->m_vMin;
		COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );
	}
	else
	{
		CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] 단독 이팩트의 설정파일을 읽어오는데 실패하였습니다.",
			m_szName, m_szFileName );

		hr = E_FAIL;
	}
	
	return hr;
}

HRESULT DXLANDEFF::Load ( sc::BaseStream &SFile )
{
	HRESULT hr = S_OK;
	DWORD dwVer, dwSize;
	
	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:
        hr = Load_0100(SFile);	
        break;

	case 0x0101:	
        hr = Load_0101(SFile);	
        break;

    case 0x0102:
        hr = Load_0102(SFile);	
        break;

	case VERSION:	
        hr = Load_0103(SFile);	
        break;

	default:
		{
			CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] 단독 이팩트의 버전이 틀려 로드가 생략되었습니다.",
				m_szName, m_szFileName );

			SFile.SetOffSet ( SFile.GetfTell() + dwSize );

			hr = E_FAIL;
		}
		break;
	};

	return hr;
}

HRESULT DXLANDEFF::Save ( sc::SerialFile	&SFile )
{
	SFile << VERSION;
	SFile << SIZE;		// 변한다면 수정해야함.

    SFile << m_fColorPowerDay;
    SFile << m_fColorPowerNight;

	SFile.WriteBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
	SFile.WriteBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
	SFile.WriteBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

	return S_OK;
}

HRESULT DXLANDEFF::PlaySingleGroup ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	if ( m_sPropGroupRes.Update() )
	{
		m_spSingleGroup.reset();

		//	Note : Eff Group instance 생성.
		//
		m_spSingleGroup = m_sPropGroupRes.get()->NEWEFFGROUP ();
		if ( m_spSingleGroup.get() == NULL )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		hr = m_spSingleGroup->Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

		//	Note : Eff Group의 매트릭스 트랜스폼 지정.
		//
		m_spSingleGroup->m_matWorld = DXLANDEFF::m_matWorld;

		//	Note : 사용되지 않음. (이전 단계에서 그리기 검사 완료하기 때문. )
		//
		//m_spSingleGroup->m_vMax = m_vMax;
		//m_spSingleGroup->m_vMin = m_vMin;
	}

	return S_OK;
}

HRESULT DXLANDEFF::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )
{
	HRESULT hr;

    if (m_bRender == false)
    {
        return S_OK;
    }

	if ( m_spSingleGroup.get() == NULL && m_sPropGroupRes.Update() )
	{
		hr = PlaySingleGroup ( pd3dDevice );
		if (FAILED(hr) )	return hr;
	}

	if ( m_spSingleGroup.get() )
	{
		// 기본적으로 렌더하고 반사쪽에서도 렌더를 해야 하는데, 그때 FrameMove를 해줄 필요는 없다.
		if ( m_spSingleGroup->GetCount() != DxEffSingleGroup::g_nEFF_COUNT )
		{
			m_spSingleGroup->SetCount( DxEffSingleGroup::g_nEFF_COUNT );
			hr = m_spSingleGroup->FrameMove ( m_fTime, m_fElapsedTime );
			if ( FAILED(hr) )	return hr;
		}

	//	EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );

		//	Note : Eff Group의 매트릭스 트랜스폼 지정.
		m_spSingleGroup->m_matWorld = DXLANDEFF::m_matWorld;

        float fColorPower = m_fColorPowerDay * GLPeriod::GetInstance().GetBlendFact() +
                            m_fColorPowerNight * (1.f-GLPeriod::GetInstance().GetBlendFact());

		hr = m_spSingleGroup->Render ( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), fColorPower );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

void DXLANDEFF::RenderName( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 vOut;
	D3DXVECTOR3 vSrc( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXVec3Project( &vOut, &vSrc, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(), &matIdentity );
	CDebugSet::ToPos( vOut.x, vOut.y, "%s", m_szName );
}

HRESULT DXLANDEFF::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_spSingleGroup.get() )
	{
		return m_spSingleGroup->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDEFF::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_spSingleGroup.get() )
	{
		return m_spSingleGroup->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDEFF::InvalidateDeviceObjects ()
{
	if ( m_spSingleGroup.get() )
	{
		return m_spSingleGroup->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DXLANDEFF::DeleteDeviceObjects()
{
	if ( m_spSingleGroup.get() )
	{
		return m_spSingleGroup->DeleteDeviceObjects ();
	}

	m_spSingleGroup.reset();

	return S_OK;
}

BOOL DXLANDEFF::IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor ) const
{
	return COLLISION::IsCollisionLineToAABB( vP1, vP2, m_vMax, m_vMin );
}

////! 승리한 클럽의 국기를 맵에 셋팅한다.
//void DXLANDEFF::SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert )
//{
//	if ( !m_spSingleGroup.get() )
//		return;
//
//	m_spSingleGroup->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//}

void DXLANDEFF::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if ( !m_spSingleGroup.get() )
		return;

	m_spSingleGroup->SetWorldBattleFlag( pWorldBattleFlag );
}
