#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"

//#include "../../EngineBaseLib/Common/stlfunctions.h"
//#include "../../CoreCommon/String/StringUtils.h"
//#include "../DxEffect/EffAni/DxEffAni.h"
//#include "../DxCommon/DXUT_UTIL.h"
#include "../../DxTools/DxMethods.h"
#include "./DxSkinAnimationManager.h"
#include "./DxSkeletonManager.h"
#include "./DxAnimationList.h"
#include "./DxAniBoneScale.h"
#include "./SAnimation.h"

#include "DxSkinAnimation.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


DxSkinAnimation::DxSkinAnimation() 
{
}

DxSkinAnimation::~DxSkinAnimation()
{
}

//! xFile에서 에니메이션 데이터를 로딩.
HRESULT DxSkinAnimation::CreateAnimationDataFromTool( BOOL bRefesh )
{
	HRESULT hr = S_OK;
	//MIN_ASSERT(m_skeletonRes.IsValid());
	if( !m_skeletonRes.IsValid() )
	{
		TSTRING strError = m_strSkeletion;
		strError += _T(" - 본이 준비가 되지 않았습니다.");

		AfxMessageBox( strError.c_str() );
		return E_FAIL;
	}

// 	if( bRefesh )
// 	{
// 		m_dxAniListRes = DxAnimationListManager::GetInstance().LoadAnimList_XFile(
// 			*this, m_skeletonRes.get() );
// 
// 		if( !m_dxAniListRes.IsValid() )
// 			return E_FAIL;
// 	}
// 	else
// 	{
// 		std::wstring strBinFile = sc::util::ChangeExtName( m_strXFile, _T("bin") );
// 		m_dxAniListRes = DxAnimationListManager::GetInstance().LoadAnimList_BIN( 
// 			strBinFile.c_str(), m_skeletonRes.GetFileName(), false );
// 
// 		if( !m_dxAniListRes.IsValid() )
// 			return E_FAIL;
// 	}
	if( bRefesh)
	{
		DxAnimationListManager::GetInstance().DeleteXFile(m_strXFile);
		DxAnimationListManager::GetInstance().DeleteData(m_strBinFile);

		// bin 을 새로 만들기 위해 추가함.
		m_dwFlag |= ACF_NEWINFO;
	}

	m_dxAniListRes = DxAnimationListManager::GetInstance().LoadAnimList_XFile(
		*this, m_skeletonRes.get() );

	if( m_dxAniListRes.IsValid() )
		return S_OK;

	TSTRING strBinFile = sc::util::ChangeExtName( m_strXFile, _T("bin") );
	m_dxAniListRes = DxAnimationListManager::GetInstance().LoadAnimList_BIN( 
		strBinFile.c_str(), m_skeletonRes.GetFileName(), false );

	if( m_dxAniListRes.IsValid() )
		return S_OK;


	return E_FAIL;
}

void DxSkinAnimation::SetAnimationList( BOOL bThread )
{
	if( !m_skeletonRes.IsValid() )
		return;

	// 툴이면 매번 무조건...
// 	if( bTool )
// 	{
// 		m_dxAniListRes = DxAnimationListManager::GetInstance().LoadAnimList_BIN( m_strBinFile,
// 			m_skeletonRes.GetFileName(), true );
// 	}
// 	else
// 	{
		// 게임에선 한번 로딩하면 재로딩 안한다  툴에서는 부적함 한듯
		if( !m_dxAniListRes.IsValid() )
		{
			m_dxAniListRes = DxAnimationListManager::GetInstance().LoadAnimList_BIN( m_strBinFile,
				m_skeletonRes.GetFileName(), bThread?true:false );
		}
//	}
}


BOOL DxSkinAnimation::IsValidateANI()
{
	if ( !m_skeletonRes.IsValid() )		
		return FALSE;

	if ( !m_dxAniListRes.IsValid() )	
		return FALSE;

	return TRUE;
}

bool DxSkinAnimation::UpdateTime_PureThread( float fCurTime,
								float fWeight,
								BOOL bFirst,
								EMBODY emBody )
{
	if ( !m_skeletonRes.IsValid() )	
		return false;

	if( !m_dxAniListRes.Update() )
		return false;

	m_dxAniListRes.get()->UpdateTime_PureThread( fCurTime, fWeight, bFirst, emBody );
	return true;
}

bool DxSkinAnimation::UpdateMix_PureThread( float fCurMixTime )
{
	if ( !m_skeletonRes.IsValid() )	return false;

	if( !m_dxAniListRes.Update() )
		return false;

	m_dxAniListRes.get()->UpdateMix_PureThread( fCurMixTime );
	return true;
}

bool DxSkinAnimation::UpdateBoneBip01_PureThread( float fCurTime, float fWeight, BOOL bFirst )
{
	if ( !m_skeletonRes.IsValid() )	return false;

	if( !m_dxAniListRes.Update() )
		return false;

	m_dxAniListRes.get()->UpdateBoneBip01_PureThread( fCurTime, fWeight, bFirst );
	return true;
}

void DxSkinAnimation::UpdateBoneScale( float fWeight, BOOL bFirst,	float fTime )
{
	m_pAniScale->UpdateBoneScale(
		m_skeletonRes.get(),
		fWeight,
		bFirst,
		fTime,
		(float) m_dwSTime,
		(float) m_dwETime,
		(float) m_UNITTIME );
}/*

HRESULT DxSkinAnimation::SaveToBinFile()
{
	if( !m_dxAniListRes.Update() )
		return E_FAIL;

	return m_dxAniListRes.get()->SaveToFile( m_strXFile.c_str() );
}*/

void DxSkinAnimation::LoadSkeleton( BOOL bPreLoad )
{
	if( m_skeletonRes.IsValid() )
		return;

	if ( bPreLoad )
		m_skeletonRes = DxSkeletonManager::GetInstance().LoadSkeletonAndBackUp( m_strSkeletion );
	else
		m_skeletonRes = DxSkeletonManager::GetInstance().LoadSkeleton( m_strSkeletion, TRUE );
}


void DxSkinAnimation::ChangeXfile( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pNameXFILE )	// Tool 에서만 쓰인다.
{
	m_strXFile		= pNameXFILE;
	m_strBinFile	= sc::util::ChangeExtName( pNameXFILE, _T("bin") );

	if( !m_skeletonRes.IsValid() )
	{
		m_dxAniListRes.Clear();
		return;
	}

	m_dxAniListRes = DxAnimationListManager::GetInstance().LoadAnimList_BIN( 
		m_strBinFile.c_str(), m_skeletonRes.GetFileName(), false );
}

//void DxSkinAnimData::ChangeMap( LPDIRECT3DDEVICEQ pd3dDevice )
//{
// 	EFFANILIST_ITER iter = m_listEffAni.begin();
// 	EFFANILIST_ITER iter_end = m_listEffAni.end();
// 	for ( ; iter!=iter_end; ++iter )
//	{
//		(*iter)->InitDeviceObjects ( pd3dDevice );
//	}
//}

const D3DXVECTOR3* DxSkinAnimation::GetStartPos_Bip01()
{
	if( !m_dxAniListRes.Update() )
		return NULL;

	return m_dxAniListRes.get()->GetStartPos_Bip01();
}

void DxSkinAnimation::Import_OnlySetting( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pName )
{
	SANIMCONINFO sSrcData;
	sSrcData.LoadFile( pName, pd3dDevice );

	SANIMCONINFO::CleanUp();

	m_dwFlag = sSrcData.m_dwFlag;					//	기타 속성.

	m_MainType = sSrcData.m_MainType;
	m_SubType = sSrcData.m_SubType;

	m_wDivCount = sSrcData.m_wDivCount;		//	에니메이션 분할 키 개수.
	for ( DWORD i=0;i<m_wDivCount; ++i )
	{
		m_wDivFrame[i] = sSrcData.m_wDivFrame[i];
	}

	m_wStrikeCount = sSrcData.m_wStrikeCount;
	for ( DWORD i=0;i<m_wStrikeCount; ++i )
	{
		m_sStrikeEff[i] = sSrcData.m_sStrikeEff[i];
	}

	m_ChaSoundData = sSrcData.m_ChaSoundData;

	EFFANILIST_ITER iter = sSrcData.m_listEffAni.begin();
	EFFANILIST_ITER iter_end = sSrcData.m_listEffAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance ( (*iter)->GetTypeID() );
		if ( pEff )
		{
			pEff->SetProperty ( (*iter)->GetProperty() );
			pEff->SetEffAniData ( (*iter) );
			m_listEffAni.push_back ( pEff );
		}
	}

	SAFE_DELETE( m_pAniScale );
	m_pAniScale = new DxAniScale;
	*m_pAniScale = *sSrcData.m_pAniScale;
}