#include "pch.h"

#include <algorithm>

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../../Common/StlFunctions.h"

#include "../../DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../DxMeshs/SkinMesh/DxAttBone.h"
#include "../../DxMeshs/SkinMesh/DxVehicle.h"

#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../DxEffect/EffAni/DxEffAniGhosting.h"
#include "../../DxEffect/EffAni/DxEffAniFaceOff.h"

#include "./DxEffAniPlayer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxEffAniPlayer::DxEffAniPlayer (void) :
	m_fTimer(0.0f),
	m_pd3dDevice(NULL)
{
}

DxEffAniPlayer::~DxEffAniPlayer (void)
{
	RemoveAllEff ();

	m_setLoadAnimation.clear();
}

HRESULT DxEffAniPlayer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	m_pd3dDevice = pd3dDevice;

	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InitDeviceObjects( pd3dDevice );
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InitDeviceObjects( pd3dDevice );
	}

	BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
	{
		iter.m_pEffAni->InitDeviceObjects( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffAniPlayer::RestoreDeviceObjects ()
{
	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->RestoreDeviceObjects( m_pd3dDevice );
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->RestoreDeviceObjects( m_pd3dDevice );
	}

	BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
	{
		iter.m_pEffAni->RestoreDeviceObjects( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffAniPlayer::InvalidateDeviceObjects ()
{
	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InvalidateDeviceObjects ();
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InvalidateDeviceObjects();
	}

	BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
	{
		iter.m_pEffAni->InvalidateDeviceObjects();
	}

	return S_OK;
}

HRESULT DxEffAniPlayer::DeleteDeviceObjects ()
{
	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->DeleteDeviceObjects ();
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->DeleteDeviceObjects();
	}

	BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
	{
		iter.m_pEffAni->DeleteDeviceObjects();
	}

	return S_OK;
}

void	DxEffAniPlayer::ResetEff ( DxSkinAnimation* pAnimContainer )
{
	SANIMCONINFO::EFFANILIST_ITER iter = pAnimContainer->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimContainer->m_listEffAni.end();

	for ( ; iter!=iter_end; ++iter )
	{
		if ( (*iter)->GetTypeID() == EMEFFANI_GHOSTING )
		{
			((DxEffAniData_Ghosting*)(*iter))->SetCreated( FALSE );
		}
		if ( (*iter)->GetTypeID() == EMEFFANI_FACEOFF )
		{
			((DxEffAniData_FaceOff*)(*iter))->SetCreated( FALSE );
		}
	}

	DeleteEffSingleEffLoop( pAnimContainer );
	CheckCreateEff( pAnimContainer );
}

void DxEffAniPlayer::CheckCreateEff( DxSkinAnimation* pAnimContainer )		// 처음 로딩시 셋팅하는 것.
{
	if( !pAnimContainer )
        return;

	// 지속 Effect 처리를 위해서 이렇게 해야한다.
	{
		DeleteEffSingleEffLoop( pAnimContainer );

		// Effect들을 생성한다.
		SANIMCONINFO::EFFANILIST_ITER iter = pAnimContainer->m_listEffAni.begin();
		SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimContainer->m_listEffAni.end();

		for ( ; iter!=iter_end; ++iter )
		{
			if ( (*iter)->IsSingleEffLoop() )
			{
				CreateEffSingleEffLoop( (*iter), pAnimContainer );	// SingleEff Loop
			}
		}
	}

	// 작업된 DxSkinAnimation* 라면 다시 하지 않는다.
	SET_PSSKINANIMATION_ITER iter_Ani = m_setLoadAnimation.find(pAnimContainer);
	if (iter_Ani != m_setLoadAnimation.end())
	{
		return;
	}

	// 새로 넣어준다.
	m_setLoadAnimation.insert(pAnimContainer);

	// Effect들을 생성한다.
	SANIMCONINFO::EFFANILIST_ITER iter = pAnimContainer->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimContainer->m_listEffAni.end();

	for ( ; iter!=iter_end; ++iter )
	{
		if ( (*iter)->GetTypeID() != EMEFFANI_SINGLE )
		{
			CreateEff( (*iter), pAnimContainer );	// 동작 하도록 함.
		}
	}
}

void	DxEffAniPlayer::CheckCreateEff( DxSkinAnimation* pAnimContainer, int nPrevKeyTime, DWORD dwCurKeyTime )	// 매 프레임 셋팅.
{
	if ( !pAnimContainer )
        return;

	SANIMCONINFO::EFFANILIST_ITER iter = pAnimContainer->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimContainer->m_listEffAni.end();

	int nCount = 0;
	for ( ; iter!=iter_end; ++iter, ++nCount )
	{
		if ( (*iter)->CheckEff( dwCurKeyTime, nPrevKeyTime ) )
		{
			CreateEff( (*iter), pAnimContainer );	// 동작 하도록 함.
		}
	}
}

void	DxEffAniPlayer::CreateEff ( DxEffAniData* pEff, DxSkinAnimation* pAnimContainer )		// 복제를 해서 이펙트를 생성한다.
{
	// Loop 의 경우는 CheckCreateEff( DxSkinAnimation* pAnimContainer ) 에서 작업된다.
	if ( pEff->IsSingleEffLoop() )
		return;

	DxEffAni*	pNewEff = pEff->NEWOBJ( pAnimContainer );

	m_listAni.push_back ( pNewEff );
}

void	DxEffAniPlayer::CreateEffSingleEffLoop( DxEffAniData* pEff, DxSkinAnimation* pAnimContainer )	// SingleEff Loop
{
	BOOL bExist(FALSE);
	VEC_EFFANI_ITER iter = m_vecAniLoop.begin();
	for ( ; iter!=m_vecAniLoop.end(); ++iter )
	{
		if ( (*iter).m_pEffData == pEff )
		{
			// 같은게 있다면 다시 작업하지는 않는다.
			return;
		}
	}

	DxEffAni*	pNewEff = pEff->NEWOBJ( pAnimContainer );

	NEVERDIE_EFFANI sData;
	sData.m_pSkinAnimation = pAnimContainer;
	sData.m_pEffData = pEff;
	sData.m_pEffAni = pNewEff;
	m_vecAniLoop.push_back( sData );
}

void	DxEffAniPlayer::DeleteEffSingleEffLoop( DxSkinAnimation* pAnimContainer )	// SingleEff Loop
{
	VEC_EFFANI_ITER iter = m_vecAniLoop.begin();
	for ( ; iter!=m_vecAniLoop.end(); ++iter )
	{
		if ( (*iter).m_pSkinAnimation != pAnimContainer )
		{
			(*iter).m_pEffAni->SetNeverDieErase();
		}
	}
}

void	DxEffAniPlayer::DeleteEffSingleEffLoop()	// SingleEff Loop
{
	BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
	{
		SAFE_DELETE ( iter.m_pEffAni );
	}
	m_vecAniLoop.clear();
}

HRESULT DxEffAniPlayer::RemoveAllEff ()
{
	std::for_each ( m_listAni.begin(), m_listAni.end(), std_afunc::DeleteObject() );
	m_listAni.clear();

	DeleteEffSingleEffLoop();

	return S_OK;
}

HRESULT DxEffAniPlayer::DeleteEff ( const TCHAR* szFile )
{
	//if ( !szFile )						return S_FALSE;
	//if ( m_listAni.empty() )	return S_FALSE;

	//EFFLIST_ITER iter = m_listAni.begin();
	//EFFLIST_ITER iter_end = m_listAni.end();
	//for ( ; iter!=iter_end; ++iter )
	//{
	//	if ( !strcmp ( (*iter)->m_szFileName, szFile ) )
	//	{
	//		SAFE_DELETE ( (*iter) );
	//		m_listAni.erase ( iter );
	//		return TRUE;
	//	}
	//}

	return S_OK;
}

HRESULT DxEffAniPlayer::FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, const DxSkinAnimation* pAnimContainer, float fTime, float fElapsedTime, SKINEFFDATA& sSKINEFFDATA )
{
	m_fTimer += fElapsedTime;

	sSKINEFFDATA.m_bNewHeadDRAW = FALSE;

	DxEffAni* pEffAni = NULL;
	for ( EFFLIST_ITER iter = m_listAni.begin(); iter!=m_listAni.end(); )
	{
        pEffAni = *iter;

		pEffAni->CheckFrameMove( pAnimContainer );		// 애니메이션이 같은지 체크한다.!!
		if( pEffAni->IsHeadDrawChild() )
		{
			sSKINEFFDATA.m_bNewHeadDRAW = TRUE;
		}

		pEffAni->FrameMove ( pSkinCharBase, fTime, fElapsedTime );

		if ( pEffAni->IsEnd() )
		{
			SAFE_DELETE( pEffAni );
			iter = m_listAni.erase( iter );
		}
        else
        {
            ++iter;
        }
	}

	for ( VEC_EFFANI_ITER iter = m_vecAniLoop.begin(); iter!=m_vecAniLoop.end(); )
	{
		pEffAni = (*iter).m_pEffAni;

		pEffAni->FrameMove ( pSkinCharBase, fTime, fElapsedTime );

		if ( pEffAni->IsEnd() )
		{
			SAFE_DELETE( pEffAni );
			iter = m_vecAniLoop.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	return S_OK;
}

HRESULT	DxEffAniPlayer::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlpha )
{
	{
		EFFLIST_ITER iter = m_listAni.begin();
		for ( ; iter!=m_listAni.end(); )
		{
			if ( (*iter)->Render ( pd3dDevice, pSkinChar, matCurPos, fAlpha ) == S_OK )
			{
				++iter;
			}
			else
			{
				// 내부에서 정상이 아니면 Msg 를 기록하는데 계속 기록하는 문제가 있어서 수정함.
				iter = m_listAni.erase( iter );
			}
		}
	}

	{
		VEC_EFFANI_ITER iter = m_vecAniLoop.begin();
		for ( ; iter!=m_vecAniLoop.end(); )
		{
			if ( (*iter).m_pEffAni->Render ( pd3dDevice, pSkinChar, matCurPos, fAlpha ) == S_OK )
			{
				++iter;
			}
			else
			{
				// 내부에서 정상이 아니면 Msg 를 기록하는데 계속 기록하는 문제가 있어서 수정함.
				iter = m_vecAniLoop.erase( iter );
			}
		}
	}

	//BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
	//{
	//	iter.m_pEffAni->Render ( pd3dDevice, pSkinChar, matCurPos, fAlpha );
	//}

	return S_OK;
}
//
//HRESULT	DxEffAniPlayer::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	EFFLIST_ITER iter = m_listAni.begin();
//	EFFLIST_ITER iter_end = m_listAni.end();
//	for ( ; iter!=iter_end; ++iter )
//	{
//		(*iter)->Render ( pd3dDevice, pAttBone, matCurPos, fAlpha );
//	}
//
//	BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
//	{
//		iter.m_pEffAni->Render ( pd3dDevice, pAttBone, matCurPos, fAlpha );
//	}
//
//	return S_OK;
//}
//
//HRESULT	DxEffAniPlayer::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	EFFLIST_ITER iter = m_listAni.begin();
//	EFFLIST_ITER iter_end = m_listAni.end();
//	for ( ; iter!=iter_end; ++iter )
//	{
//		(*iter)->Render ( pd3dDevice, pVehicle, matCurPos, fAlpha );
//	}
//
//	BOOST_FOREACH( VEC_EFFANI::value_type& iter, m_vecAniLoop )
//	{
//		iter.m_pEffAni->Render ( pd3dDevice, pVehicle, matCurPos, fAlpha );
//	}
//
//	return S_OK;
//}
//
//
