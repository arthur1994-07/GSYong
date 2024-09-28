#include "pch.h"

#include "../../../SigmaCore/String/StringUtil.h"

#include "../../Common/StlFunctions.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../GlobalParam.h"
#include "../Char/DxEffcharData.h"
#include "./DxEffectParticleSys.h"
#include "./DxEffSinglePropGMan.h"
#include "./DxEffGroupPlayer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxEffGroupPlayer& DxEffGroupPlayer::GetInstance()
{
	static DxEffGroupPlayer Instance;
	return Instance;
}

DxEffGroupPlayer::DxEffGroupPlayer (void) :
	m_fTimer(0.0f),
	m_pd3dDevice(NULL)
{
}

DxEffGroupPlayer::~DxEffGroupPlayer (void)
{
}

D3DXVECTOR3 DxEffGroupPlayer::GetTargetIDPos ( STARGETID *pTargetID )
{
	D3DXVECTOR3 vPos;
	
	vPos.x = 0.f;
	vPos.y = 10.f;
	vPos.z = -50.f;

	return vPos;
}

// szFileName 가 m_mapESG 에 있다면, 기존것을 사용하면서 위치값변경.
// m_mapESG 에 없다면, NewEffGroup 를 사용하여 m_mapESG 에 등록.
HRESULT DxEffGroupPlayer::PassiveEffect ( const char* szFileName, const D3DXMATRIX &matStart, const STARGETID &sTargetID, BOOL bEdit /* = FALSE */ )
{
	if ( sTargetID.GaeaId==EMTARGET_NULL )	
		return E_FAIL;

	if ( !szFileName || szFileName[0]==NULL )
		return E_FAIL;

	std::string strFileName = szFileName;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	BOOL bExist = FALSE;
    SPDXEFFSINGLEGROUP spEffSingleGroup;
	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; ++iter )
	{
		spEffSingleGroup = (*iter).second;
		if ( spEffSingleGroup->m_strFileName==strFileName )
		{
			bExist = TRUE;
			if( bEdit )
			{
				spEffSingleGroup->m_matWorld = matStart;
				spEffSingleGroup->m_matTarget = matStart;
			}
		}
	}

	if ( !bExist )
	{
		NewEffGroup ( strFileName.c_str(), matStart, &sTargetID, FALSE, FALSE );
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeletePassiveEffect ( const STARGETID &sTargetID )
{
	if ( sTargetID.GaeaId==EMTARGET_NULL )	
		return E_FAIL;

	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; )
	{
		iter = m_mapESG.erase(iter);
	}

	// 로딩 List 에서 지워줌.
	{
		LISTEFFSINGLEGROUP_ITER iter = m_listESG_Loading.begin();
		for ( ; iter!=m_listESG_Loading.end(); )
		{
			DXEFFSINGLEGROUP_DATA& Data = (*iter);
			if ( Data.m_sTargetID == sTargetID )
			{
				iter = m_listESG_Loading.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeletePassiveEffect ( const char* szFileName, const STARGETID &sTargetID )
{
	if ( sTargetID.GaeaId==EMTARGET_NULL )	
		return E_FAIL;

	std::string strFileName = szFileName;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

    SPDXEFFSINGLEGROUP spEffSingleGroup;
	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; )
	{
		spEffSingleGroup = (*iter).second;
		if ( spEffSingleGroup->m_strFileName==strFileName )
		{
			iter = m_mapESG.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// 로딩 List 에서 지워줌.
	{
		LISTEFFSINGLEGROUP_ITER iter = m_listESG_Loading.begin();
		for ( ; iter!=m_listESG_Loading.end(); )
		{
			if ( ( (*iter).m_sTargetID == sTargetID ) && ( (*iter).m_strFileName == strFileName ) )
			{
				iter = m_listESG_Loading.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	return S_OK;
}

BOOL DxEffGroupPlayer::FindPassiveEffect ( const char* szFileName, const STARGETID &sTargetID )
{
	if ( sTargetID.GaeaId==EMTARGET_NULL )	return FALSE;

	std::string strFileName = szFileName;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

    SPDXEFFSINGLEGROUP spEffSingleGroup;
	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; ++iter )
	{
		spEffSingleGroup = (*iter).second;
		if ( spEffSingleGroup->m_strFileName==strFileName )
            return TRUE;
	}

	return FALSE;
}

HRESULT DxEffGroupPlayer::NewEffGroup ( DxEffSinglePropGroupResource spPropGroup, EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matStart )
{
	return NewEffGroup ( spPropGroup.get(), pTargetProp, matStart );
}

SPDXEFFSINGLEGROUP DxEffGroupPlayer::NewEffGroup ( DxEffSinglePropGroupResource spPropGroup, const D3DXMATRIX &matStart, const STARGETID* pTargetID )
{
	return NewEffGroup ( spPropGroup.get(), matStart, pTargetID );
}

HRESULT DxEffGroupPlayer::NewEffGroup ( EFF_PROPGROUP* pPropGroup, EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matStart )
{
	if ( matStart._11 > 100000.f || matStart._12 > 100000.f || matStart._13 > 100000.f || matStart._14 > 100000.f ||
		matStart._21 > 100000.f || matStart._22 > 100000.f || matStart._23 > 100000.f || matStart._24 > 100000.f ||
		matStart._31 > 100000.f || matStart._32 > 100000.f || matStart._33 > 100000.f || matStart._34 > 100000.f ||
		matStart._41 > 100000.f || matStart._42 > 100000.f || matStart._43 > 100000.f || matStart._44 > 100000.f ||
		matStart._11 < -100000.f || matStart._12 < -100000.f || matStart._13 < -100000.f || matStart._14 < -100000.f ||
		matStart._21 < -100000.f || matStart._22 < -100000.f || matStart._23 < -100000.f || matStart._24 < -100000.f ||
		matStart._31 < -100000.f || matStart._32 < -100000.f || matStart._33 < -100000.f || matStart._34 < -100000.f ||
		matStart._41 < -100000.f || matStart._42 < -100000.f || matStart._43 < -100000.f || matStart._44 < -100000.f)
	{
		return E_FAIL;

	}
	//	Note : Eff Group instance 생성.
	//
	SPDXEFFSINGLEGROUP spEffSingleGroup = pPropGroup->NEWEFFGROUP ( pTargetProp );
	if ( spEffSingleGroup.get() == NULL )	
		return E_FAIL;

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	//
	spEffSingleGroup->m_matWorld = matStart;
	spEffSingleGroup->m_matTarget = matStart;

	//	Note : Eff Group Create Dev.
	//
	spEffSingleGroup->Create ( m_pd3dDevice );

	//	Note : 리스트에 등록함.
	//
	m_mapESG.insert ( std::make_pair(STARGETID(), spEffSingleGroup) );

	return S_OK;
}

SPDXEFFSINGLEGROUP DxEffGroupPlayer::NewEffGroup ( EFF_PROPGROUP* pPropGroup, const D3DXMATRIX &matStart, const STARGETID* pTargetID )
{
	if ( matStart._11 > 100000.f || matStart._12 > 100000.f || matStart._13 > 100000.f || matStart._14 > 100000.f ||
		matStart._21 > 100000.f || matStart._22 > 100000.f || matStart._23 > 100000.f || matStart._24 > 100000.f ||
		matStart._31 > 100000.f || matStart._32 > 100000.f || matStart._33 > 100000.f || matStart._34 > 100000.f ||
		matStart._41 > 100000.f || matStart._42 > 100000.f || matStart._43 > 100000.f || matStart._44 > 100000.f ||
		matStart._11 < -100000.f || matStart._12 < -100000.f || matStart._13 < -100000.f || matStart._14 < -100000.f ||
		matStart._21 < -100000.f || matStart._22 < -100000.f || matStart._23 < -100000.f || matStart._24 < -100000.f ||
		matStart._31 < -100000.f || matStart._32 < -100000.f || matStart._33 < -100000.f || matStart._34 < -100000.f ||
		matStart._41 < -100000.f || matStart._42 < -100000.f || matStart._43 < -100000.f || matStart._44 < -100000.f)
	{
		return SPDXEFFSINGLEGROUP();
	}

	//	Note : Eff Group instance 생성.
	//
	SPDXEFFSINGLEGROUP spEffSingleGroup = pPropGroup->NEWEFFGROUP ();
	if ( spEffSingleGroup.get() == NULL )	
		return SPDXEFFSINGLEGROUP();

	//	Note : 타겟지정.
	//
	spEffSingleGroup->SetTargetID ( pTargetID );

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	//
	spEffSingleGroup->m_matWorld = matStart;
	spEffSingleGroup->m_matTarget = matStart;

	//	Note : Eff Group Create Dev.
	//
	spEffSingleGroup->Create ( m_pd3dDevice );

	//	Note : 리스트에 등록함.
	//
	STARGETID sTargetID;
	if ( pTargetID )	sTargetID = *pTargetID;
	m_mapESG.insert( std::make_pair(sTargetID,spEffSingleGroup) );

	return spEffSingleGroup;
}

// bBackUpCache2 아직은 ResourceManager 만들어져있지는 않다.
// 이 옵션이 되어 있으면 맵 이동시는 삭제 하지 않고, 캐릭터를 재 선택하여 플레이 할 경우만 삭제 하도록 한다.
SPDXEFFSINGLEGROUP DxEffGroupPlayer::NewEffGroup ( const char* szFileName, const D3DXMATRIX &matStart, const STARGETID* pTargetID, BOOL bThread, BOOL bBackUpCache2 )
{
	GASSERT(pTargetID);
	GASSERT(m_pd3dDevice);

	size_t nSize(0);
	StringCchLength( szFileName, MAX_PATH, &nSize );
	if ( nSize == 0 )
	{
		return SPDXEFFSINGLEGROUP();
	}

	// 일단은 무조건 BackUpCache를 하도록 한다.
	// 추후 ResourceManager에 Cache 기능이 업그레이드 되면 그때 수정하는 방향으로 한다.
	
	//	Note : Prop Group 을 가져옴.
	DxEffSinglePropGroupResource spPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( szFileName, bThread, bBackUpCache2 );
	if ( spPropGroup.IsValid() )
	{
		//	Note : Eff Group instance 생성.
		return NewEffGroup( spPropGroup, matStart, pTargetID );
	}
	
	if ( bThread )
	{
		DXEFFSINGLEGROUP_DATA sData;
		sData.m_sTargetID = *pTargetID;;
		sData.m_strFileName = szFileName;
		sData.m_matTrans = matStart;
		sData.m_sPropGroupRes = spPropGroup;
		m_listESG_Loading.push_back( sData );
	}

	return SPDXEFFSINGLEGROUP();
}

HRESULT DxEffGroupPlayer::NewEffBody ( const char* szFileName, const STARGETID* pTargetID, const D3DXVECTOR3 *pDir )
{
	GASSERT(pTargetID);

	DxSkinChar* pSkinChar = DxEffectMan::GetInstance().GetSkinChar ( *pTargetID );
	if ( !pSkinChar )	return E_FAIL;

	D3DXVECTOR3 vDIR_ORG(0.f,0.f,-1.f);
	float fdir_y = 0;
	if ( pDir )		fdir_y = DXGetThetaYFromDirection ( *pDir, vDIR_ORG );

	BOOL bOk = DxEffcharDataMan::GetInstance().PutEffect ( pSkinChar, szFileName, pDir, fdir_y );
	if ( !bOk )			return E_FAIL;

	return S_OK;
}

HRESULT DxEffGroupPlayer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	m_pd3dDevice = pd3dDevice;

	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::RestoreDeviceObjects ()
{
	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->RestoreDeviceObjects ( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::InvalidateDeviceObjects ()
{
	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeleteDeviceObjects ()
{
	RemoveAllEff ();

	return S_OK;
}

HRESULT DxEffGroupPlayer::RemoveAllEff ()
{
	m_mapESG.clear();
	m_listESG_Loading.clear();

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeleteEff(const std::string& FileName)
{
	if (FileName.empty())
        return S_FALSE;

	if (m_mapESG.empty())
        return S_FALSE;

	std::string strFileName(FileName);
    sc::string::lowerString(strFileName);

    SPDXEFFSINGLEGROUP spEffect;
    EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
        spEffect = iter->second;
		if (spEffect->m_strFileName == strFileName)
		{
			m_mapESG.erase ( iter );
			return S_OK;
		}
	}

    return S_OK;
}

HRESULT DxEffGroupPlayer::DeleteAllEff(const std::string& FileName)
{
	if (FileName.empty())
        return S_FALSE;

	if (m_mapESG.empty())
        return S_FALSE;

	std::string strFileName(FileName);
    sc::string::lowerString(strFileName);


	SPDXEFFSINGLEGROUP spEffect;
	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; )
	{
		spEffect = iter->second;
		if (spEffect->m_strFileName == strFileName)
		{
			iter = m_mapESG.erase( iter );
		}
        else
        {
            ++iter;
        }
	}

    return S_OK;
}

void DxEffGroupPlayer::NewAfterEff ( DxEffSingleGroup* pEffSG )
{
	if ( pEffSG->m_listEffSAfter.empty() )	return;

	STARGETID sTargetID = pEffSG->m_TargetID;

	if( !pEffSG->m_ResultTargetID.ISNULL() )
	{
		sTargetID = pEffSG->m_ResultTargetID;
	}

	D3DXVECTOR3 vTARPOS = DxEffectMan::GetInstance().GetCrowPos ( sTargetID );
	vTARPOS.y += 15.0f;

	float fTARDIR = DxEffectMan::GetInstance().GetCrowDir ( sTargetID );

	DxEffSingleGroup::AFTERLIST_ITER iter = pEffSG->m_listEffSAfter.begin();
	DxEffSingleGroup::AFTERLIST_ITER iter_end = pEffSG->m_listEffSAfter.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CString strTemp = (*iter).m_strEffect.c_str();
		if ( !((*iter).m_strEffect.empty()) )
		{
			switch ( (*iter).m_emType )
			{
			case EFFABODY:
				{
					DxSkinChar* pSkinChar = DxEffectMan::GetInstance().GetSkinChar ( sTargetID );
					if ( !pSkinChar )	continue;

					D3DXVECTOR3 vDir = vTARPOS - pEffSG->m_vStartPos;
					D3DXVec3Normalize ( &vDir, &vDir );

					DxEffcharDataMan::GetInstance().PutEffect
					(
						pSkinChar,
						(*iter).m_strEffect.c_str(),
						&vDir,
						fTARDIR
					);
				}
				break;

			case EFFASINGLE:
				{
					//	보이지 않는 타갯일 경우 타격 이팩트는 생략됨.
					if ( !DxEffectMan::GetInstance().IsCrowVisible(sTargetID) )
                        break;

					//	타겟의 위치.
					sTargetID.vPos = vTARPOS;

					D3DXMATRIX matEffect;


					D3DXVECTOR3 vDir = pEffSG->m_vStartPos - vTARPOS;

					D3DXVECTOR3 vDIR_ORG(1,0,0);
					float fdir_y = DXGetThetaYFromDirection ( vDir, vDIR_ORG );

					D3DXMatrixRotationY ( &matEffect, fdir_y );
					matEffect._41 = vTARPOS.x;
					matEffect._42 = vTARPOS.y;
					matEffect._43 = vTARPOS.z;

//					D3DXMatrixTranslation ( &matEffect, vTARPOS.x, vTARPOS.y, vTARPOS.z );

					NewEffGroup
					(
						(*iter).m_strEffect.c_str(),
						matEffect,
						&sTargetID,
						TRUE, 
						FALSE
					);
				}
				break;
			};
		}
	}

	pEffSG->m_listEffSAfter.clear();
}

void DxEffGroupPlayer::UpdateLoadingData()
{
	LISTEFFSINGLEGROUP_ITER iter = m_listESG_Loading.begin();
	for ( ; iter!=m_listESG_Loading.end(); )
	{
		if ( (*iter).m_sPropGroupRes.Update() )
		{
			// 죽지 않는 속성일 때만 렌더링을 한다.
			// 죽는 속성일 경우는 렌더링을 해도 제대로 된 위치가 아니라서 문제가 될 소지가 크다. 그래서 렌더링에서 제외한다.
			if ( (*iter).m_sPropGroupRes.get()->IsFlags( EFF_CFG_NEVERDIE ) )
			{
				NewEffGroup( (*iter).m_sPropGroupRes, (*iter).m_matTrans, &(*iter).m_sTargetID );
			}

			iter = m_listESG_Loading.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

HRESULT DxEffGroupPlayer::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTimer += fElapsedTime;

	UpdateLoadingData();

    SPDXEFFSINGLEGROUP spEffect;
	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; )
	{
        spEffect = iter->second;
		spEffect->FrameMove ( fTime, fElapsedTime );

		if ( spEffect->IsBeginAfterEffect() )
		{
			++iter;

			NewAfterEff ( spEffect.get() );
		}
		else if ( spEffect->m_dwAliveCount==0 )
		{
			iter = m_mapESG.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	//CDebugSet::ToView ( 1, 2, "PARTICLE SLEEP OBJ : %d", DxEffectParticleSys::GetParticlePool()->GetAmount() );

	return S_OK;
}

HRESULT	DxEffGroupPlayer::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return S_OK;

	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SPDXEFFSINGLEGROUP& spEffectGroup = iter->second;
		if ( spEffectGroup->isEnableRender() )
			spEffectGroup->Render ( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
	}

	return S_OK;
}

void DxEffGroupPlayer::RenderEnable(const STARGETID& sID, const bool bRenderEnable)
{
	EFFSGMAP_PAIR rangeID = m_mapESG.equal_range(sID);
	for ( EFFSGMAP_ITER iter = rangeID.first; iter != rangeID.second; ++iter )
	{
		SPDXEFFSINGLEGROUP& spEffectGroup = iter->second;
		spEffectGroup->SetEnableRender(bRenderEnable);
	}	
}
