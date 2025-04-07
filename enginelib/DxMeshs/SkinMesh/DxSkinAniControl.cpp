#include "pch.h"

#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool.hpp>
#include <algorithm>

#include "../../../RanThirdParty/PhysX_3_2/include/PxPhysicsAPI.h"

#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/Memory/ObjectPool.h"
#include "../../../SigmaCore/Math/Random.h"
#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxEffect/EffAni/DxEffAniPlayer.h"
#include "../../DxSound/DxSoundMan.h"
#include "../../Common/StlFunctions.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/EditMeshs.h"

#include "./DxAttBoneLinkThreadNeedData.h"
#include "./DxVehicleThreadNeedData.h"
#include "./DxSkeletonMaintain.h"
#include "./DxSkinAniControlData.h"
#include "./DxSkinAnimationManager.h"
#include "./DxSkinAnimation.h"
#include "./DxSkinDefine.h"
#include "./DxJiggleBoneColl.h"
#include "./DxJiggleBone.h"
#include "./DxClothColl.h"

#include "DxSkinAniControl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static boost::object_pool<ANIMCONTNODE>				g_poolANICONTNODE;	//(_T("ANIMCONTNODE"));
static boost::object_pool<DxSkinAniControl::MAIN>	g_poolAniActionStack;

bool DxSkinAniControl::m_bBIGHEAD = false;
bool DxSkinAniControl::m_bBIGHAND = false;
float DxSkinAniControl::m_fBIGHEAD = 4.0f;
float DxSkinAniControl::m_fBIGHAND = 2.0f;

extern BOOL g_bOBJECT100;
extern TSTRING	g_strPICK_BONE;
extern TSTRING	g_strJIGGLE_BONE;
extern TSTRING	g_strJIGGLE_BONE_PREV;
extern BOOL		g_bCHAR_EDIT_RUN;
extern float	g_fJIGGLE_BONE_SWING_Y;
extern float	g_fJIGGLE_BONE_SWING_Z;
extern D3DXVECTOR3	g_vJIGGLE_BONE_ROTATE;


struct ANIMCONTNODE_OPER
{
	bool operator() (ANIMCONTNODE* lvalue, ANIMCONTNODE* rvalue) const
	{
		return lvalue->m_pDxSkinAni->m_strCfgFile.compare(rvalue->m_pDxSkinAni->m_strCfgFile) < 0;
	}

    bool operator() (ANIMCONTNODE* lvalue, const std::string& rvalue) const
	{
		return lvalue->m_pDxSkinAni->m_strCfgFile.compare(rvalue) < 0;
	}

	//
	//mjeon.migration
	//
	// Debug_Iterator checks whether Predicate is commutative.	-> If A is TRUE, B MUST be FALSE;
	//

    bool operator() (const std::string& lvalue, ANIMCONTNODE* rvalue) const
	{
		return lvalue.compare(rvalue->m_pDxSkinAni->m_strCfgFile) < 0;
	}
};

//------------------------------------------------------------------------------------------------------------------------
//							A	N	I	M		C	O	N	T		N	O	D	E
//------------------------------------------------------------------------------------------------------------------------
ANIMCONTNODE::~ANIMCONTNODE()
{
	m_pDxSkinAni = NULL;
	wBackFrame = 0;
}

//void ANIMCONTNODE::SetSTime( int nSTime )
//{
//	if( m_pDxSkinAni && nSTime == -1 )
//	{
//		m_dwSTimeSKT = m_pDxSkinAni->m_dwSTime;
//	}
//	else
//	{
//		m_dwSTimeSKT = nSTime;
//	}
//}
//
//void ANIMCONTNODE::SetETime( int nETime )
//{
//	if( m_pDxSkinAni && nETime == -1 )	
//	{
//		m_dwETimeSKT = m_pDxSkinAni->m_dwETime;
//	}
//	else
//	{
//		m_dwETimeSKT = nETime;
//	}
//}

EMANI_MAINTYPE	ANIMCONTNODE::GetMType() const
{
	return m_pDxSkinAni->m_MainType;
}

EMANI_SUBTYPE	ANIMCONTNODE::GetSType() const
{ 
	return m_pDxSkinAni->m_SubType;
}

DWORD ANIMCONTNODE::GetTimeLength()
{
	return (m_pDxSkinAni->m_dwETime-m_pDxSkinAni->m_dwSTime);
}

void ANIMCONTNODE::Import_OnlySetting( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pName )
{
	if( !m_pDxSkinAni )
		return;

	// 복제
	m_pDxSkinAni->Import_OnlySetting( pd3dDevice, pName );
}


//--------------------------------------------------------------------------------------------------------------------
//								D	x		A	n	i		C	o	n	t	r	o	l
//--------------------------------------------------------------------------------------------------------------------
DxAniControl::DxAniControl()
	: m_bEndAnim(FALSE)
	, m_nPrevKeyTime(-1)
	, m_rAnimNode(NULL)
	, m_rAnimThreadLoad(NULL)
	, m_fTime(0.f)
	, m_bFirstAnimation(FALSE)
{
}

void DxAniControl::CleanUp()
{
	if ( m_rAnimNode )
	{
		m_rAnimNode->StopLoopSound();
	}

	m_nPrevKeyTime = -1;
	m_fTime = 0.0f;
	m_bEndAnim = FALSE;
	m_rAnimNode = NULL;
	m_rAnimThreadLoad = NULL;
}

void DxAniControl::FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, DxAniControl& sAniControlBACKUP )
{
	if( m_rAnimThreadLoad )
	{
		if( m_rAnimThreadLoad->m_pDxSkinAni )
		{
			if( m_rAnimThreadLoad->m_pDxSkinAni->IsValidateANI() )
			{
				sAniControlBACKUP.m_fTime = m_fTime;			// 이전 값을 셋팅한다.
				sAniControlBACKUP.m_rAnimNode = m_rAnimNode;	// 이전 값을 셋팅한다.
				sAniControlBACKUP.m_bEndAnim = m_bEndAnim;

				m_rAnimNode = m_rAnimThreadLoad;
				m_rAnimThreadLoad = NULL;

				m_bEndAnim = FALSE;
				m_nPrevKeyTime = -1;
				m_fTime = (float)m_rAnimNode->m_pDxSkinAni->m_dwSTime;
				m_rAnimNode->StartAnim ();
			}
		}
	}

	if( !m_rAnimNode )	return;

	m_fTime += fElapsedTime * UNITANIKEY_PERSEC;
	m_bEndAnim = m_rAnimNode->FrameMove ( m_fTime, bContinue, dwFlags );
//	if ( m_bEndAnim == true && dwFlags & EMANI_MOTIONBLENDING )
}

void DxAniControl::FrameMoveLayer( float fElapsedTime, BOOL bContinue, DWORD dwFlags )
{
	if( !m_rAnimNode )				return;
	if( !m_rAnimNode->m_pDxSkinAni	)	return;

	if( m_bEndAnim )
	{
        /*
            앞 애니가 정상적으로 끝났는데 굳이 블랜딩을 다시 해줌으로써 뒤 애니 앞 부분이
            앞 애니 끝부분과 블랜딩 되어서 튀게 되는 문제 수정            

        */
// 		m_fTime = (float)(m_rAnimNode->m_pDxSkinAni->m_dwETime - m_rAnimNode->m_pDxSkinAni->m_UNITTIME);
// 		m_rAnimNode->FrameMove ( m_fTime, bContinue, dwFlags );
	}
	else
	{
		m_fTime += fElapsedTime * UNITANIKEY_PERSEC;
		m_bEndAnim = m_rAnimNode->FrameMove ( m_fTime, bContinue, dwFlags );
	}

	m_rAnimNode->StopLoopSound();
}

BOOL DxAniControl::UpdateTime_PureThread( float fWeight, BOOL bFirst, EMBODY emBody )
{
	if( !m_rAnimNode )				return FALSE;
	if( !m_rAnimNode->m_pDxSkinAni )	return FALSE;

	if( !m_rAnimNode->m_pDxSkinAni->UpdateTime_PureThread( m_fTime, fWeight, bFirst, emBody ) )
	{
		return FALSE;
	}

	return TRUE;
}

void DxAniControl::UpdateMix_PureThread()
{
	if( !m_rAnimNode )				return;
	if( !m_rAnimNode->m_pDxSkinAni )	return;
	
	m_rAnimNode->m_pDxSkinAni->UpdateMix_PureThread( m_fTime );
}

BOOL DxAniControl::UpdateBoneBip01_PureThread( float fWeight, BOOL bFirst )
{
	if( !m_rAnimNode )				return FALSE;
	if( !m_rAnimNode->m_pDxSkinAni )	return FALSE;

	if( !m_rAnimNode->m_pDxSkinAni->UpdateBoneBip01_PureThread( m_fTime, fWeight, bFirst ) )
	{
		return FALSE;
	}

	return TRUE;
}

void DxAniControl::FrameMoveMIX( float fElapsedTime )
{
	if( !m_rAnimNode )	return;

	m_fTime += fElapsedTime * UNITANIKEY_PERSEC;
	if ( DWORD(m_fTime) > m_rAnimNode->m_pDxSkinAni->m_dwETime )
	{
		m_rAnimNode = NULL;
	}
}

void DxAniControl::ToStartTime( float fCurTime )
{
	if( m_rAnimNode )
	{
		m_bFirstAnimation = TRUE;
		m_bEndAnim = FALSE;
		m_nPrevKeyTime = -1;
		m_fTime = fCurTime;
	}
}

void DxAniControl::ToEndTime( float fCurTime )
{
	if( m_rAnimNode )
	{
		m_nPrevKeyTime = -1;
		m_fTime = fCurTime;
	}
}

BOOL DxAniControl::SetAnimation( ANIMCONTNODE* pNode, DxAniControl& sAniControlBACKUP, BOOL bSameAniRefresh )
{
	//if ( m_rAnimNode==pNode && bResetSkillAni == FALSE)	
	//	return FALSE;

	if( !pNode->m_pDxSkinAni )		
		return FALSE;

	if ( bSameAniRefresh )
	{
		sAniControlBACKUP.m_fTime = 0.f;		// 이전 값을 셋팅한다.
		sAniControlBACKUP.m_rAnimNode = NULL;	// 이전 값을 셋팅한다.
		sAniControlBACKUP.m_bEndAnim = TRUE;
	}
	else
	{
		if( m_rAnimNode==pNode )
			return FALSE;

		sAniControlBACKUP.m_fTime = m_fTime;			// 이전 값을 셋팅한다.
		sAniControlBACKUP.m_rAnimNode = m_rAnimNode;	// 이전 값을 셋팅한다.
		sAniControlBACKUP.m_bEndAnim = m_bEndAnim;
	}

	// 루프중인 사운드를 끈다.
	if ( m_rAnimNode )
		m_rAnimNode->StopLoopSound();

	// 애니메이션이 처음 동작 되는것이라고 셋팅한다.
	m_bFirstAnimation = TRUE;

	if( pNode->m_pDxSkinAni->IsValidateANI() )
	{
		m_rAnimNode = pNode;
		m_rAnimThreadLoad = NULL;

		m_bEndAnim = FALSE;
		m_nPrevKeyTime = -1;
		//m_fTime = (float)pNode->m_dwSTimeSKT;
		m_fTime = (float)pNode->m_pDxSkinAni->m_dwSTime;
		pNode->StartAnim ();
	}
	else
	{
		m_rAnimThreadLoad = pNode;

		//m_fTime = (float)pNode->m_dwSTimeSKT;
		m_fTime = (float)pNode->m_pDxSkinAni->m_dwSTime;
	}

	return TRUE;
}

void DxAniControl::SetMixAni( ANIMCONTNODE* pNode )
{
	m_rAnimNode = pNode;
	m_fTime = 0.0f;
}



void DxAniControl::UpdateBoneScale( float fWeight, BOOL bFirst )
{
	if( !m_rAnimNode )				return;
	if( !m_rAnimNode->m_pDxSkinAni )	return;
	if( fWeight < 0.f )				return;

	m_rAnimNode->m_pDxSkinAni->UpdateBoneScale( fWeight, bFirst, m_fTime );
}

void DxAniControl::Validate()	// 유효한지 체크한다.
{
	if( m_bEndAnim )
	{
		m_rAnimNode = NULL;
		m_rAnimThreadLoad = NULL;
		//m_rAnimNodeBACKUP = NULL;
	}
}

void DxAniControl::StopLoopSound()
{
	if ( m_rAnimNode )
	{
		m_rAnimNode->StopLoopSound();
	}
}

// 0 KeyFrame <-> 1 JiggleBone
void DxAniControl::GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const
{
	if ( !m_rAnimNode )
		return;

	if ( !m_rAnimNode->m_pDxSkinAni )
		return;

	m_rAnimNode->m_pDxSkinAni->GetKeyJiggleWeight_PureThread( afKeyJiggleWeight, dwKeyJiggleWeightNUM, dwKey );
}

// [AnimationMove] 애니메이션 움직임이 캐릭터에 영향을 받는 것인지 확인
BOOL DxAniControl::IsAnimationMove()
{
	if( !m_rAnimNode )
		return FALSE;

	if( !m_rAnimNode->m_pDxSkinAni )
		return FALSE;

	return m_rAnimNode->m_pDxSkinAni->IsAnimationMove();
}

// [AnimationMove] 처음 애니메이션 시작인가~?
BOOL DxAniControl::IsFirstAnimation()
{
	return m_bFirstAnimation;
}

// [AnimationMove] 처음 애니메이션 동작을 작업하였으면 FALSE로 셋팅한다.
void DxAniControl::SetFirstAnimationFALSE()
{
	m_bFirstAnimation = FALSE;
}

// [AnimationMove] 처음 애니메이션 동작의 위치를 얻는다.
const D3DXVECTOR3* DxAniControl::GetFirstAnimationPos()
{
	if( !m_rAnimNode )
		return NULL;

	if( !m_rAnimNode->m_pDxSkinAni )
		return NULL;

	return m_rAnimNode->m_pDxSkinAni->GetStartPos_Bip01();
}




//--------------------------------------------------------------------------------------------------------------------
//						D	x		A	n	i		C	o	n	t	r	o	l		C	O	R	E
//--------------------------------------------------------------------------------------------------------------------
void DxAniControlCORE::CleanUp()
{
	m_sUpBodyAC.CleanUp();
	m_sDownBodyAC.CleanUp();
	m_sUpBodyAC_Layer.CleanUp();
	m_sDownBodyAC_Layer.CleanUp();
	m_sMixAC.CleanUp();
}

void DxAniControlCORE::FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, float fWeight )
{
	m_sUpBodyAC.FrameMove( fElapsedTime, bContinue, dwFlags, m_sUpBodyAC_Layer );
	m_sDownBodyAC.FrameMove( fElapsedTime, bContinue, dwFlags, m_sDownBodyAC_Layer );

	m_sUpBodyAC_Layer.FrameMoveLayer( fElapsedTime, bContinue, dwFlags );
	m_sDownBodyAC_Layer.FrameMoveLayer( fElapsedTime, bContinue, dwFlags );

	m_sUpBodyAC.Validate();

	if( fWeight >= 1.f )
	{
		m_sUpBodyAC_Layer.Validate();
		m_sDownBodyAC_Layer.Validate();
	}

	m_sMixAC.FrameMoveMIX( fElapsedTime );
}

void DxAniControlCORE::UpdateTime_PureThread( float fWeight )
{	
#ifdef USE_ANI_QUATERNION
	if( m_sUpBodyAC.m_rAnimNode )
	{
		// Note : 아직 로딩이 덜 되었을 경우 제작했을 때의 모습이 나와버려서.. 디폴트를 로딩해 놓는다.
		if( !m_sUpBodyAC.UpdateTime_PureThread( fWeight, TRUE, EMBODY_UPBODY ) )
		{
		//	m_sUpBodyAC.m_rAnimNode = m_sUpBodyAC.m_rAnimNodeBACKUP;
		//	m_sUpBodyAC.UpdateTime( fWeight, TRUE, EMBODY_UPBODY );
		}
		m_sUpBodyAC_Layer.UpdateTime_PureThread( 1.f-fWeight, FALSE, EMBODY_UPBODY );

		// Note : 아직 로딩이 덜 되었을 경우 제작했을 때의 모습이 나와버려서.. 디폴트를 로딩해 놓는다.
		if( !m_sDownBodyAC.UpdateTime_PureThread( fWeight, TRUE, EMBODY_DOWNBODY ) )
		{
		//	m_sDownBodyAC.m_rAnimNode = m_sDownBodyAC.m_rAnimNodeBACKUP;
		//	m_sDownBodyAC.UpdateTime( fWeight, TRUE, EMBODY_DOWNBODY );
		}
		m_sDownBodyAC_Layer.UpdateTime_PureThread( 1.f-fWeight, FALSE, EMBODY_DOWNBODY );
	}
	else
	{
		// Note : 아직 로딩이 덜 되었을 경우 제작했을 때의 모습이 나와버린다.
		if( !m_sDownBodyAC.UpdateTime_PureThread( fWeight, TRUE, EMBODY_DEFAULT ) )
		{
		//	m_sDownBodyAC.m_rAnimNode = m_sDownBodyAC.m_rAnimNodeBACKUP;
		//	m_sDownBodyAC.UpdateTime( fWeight, TRUE, EMBODY_DEFAULT );
		}
		m_sDownBodyAC_Layer.UpdateTime_PureThread( 1.f-fWeight, FALSE, EMBODY_DEFAULT );
	}

	if( fWeight >= 1.f )
	{
 		m_sUpBodyAC_Layer.CleanUp();
 		m_sDownBodyAC_Layer.CleanUp();
	}

#else
	//	Note : 에니메이션의 갱신.
	m_sUpBodyAC.UpdateTime_PureThread( 1.f, TRUE, EMBODY_DEFAULT );
	m_sDownBodyAC.UpdateTime_PureThread( 1.f, TRUE, EMBODY_DEFAULT );

#endif

	m_sUpBodyAC.UpdateBoneScale( fWeight, TRUE );
	m_sUpBodyAC_Layer.UpdateBoneScale( 1.f-fWeight, FALSE );

	m_sDownBodyAC.UpdateBoneScale( fWeight, TRUE );
	m_sDownBodyAC_Layer.UpdateBoneScale( 1.f-fWeight, FALSE );
}

void DxAniControlCORE::UpdateMix_PureThread()
{
	m_sMixAC.UpdateMix_PureThread();
}

void DxAniControlCORE::UpdateBoneBip01_PureThread( float fWeight )
{
	m_sUpBodyAC.UpdateBoneBip01_PureThread( fWeight, TRUE );
	//m_sDownBodyAC.UpdateBoneBip01( 1.f-fWeight, FALSE );
}

BOOL DxAniControlCORE::SetAnimation( ANIMCONTNODE* pNode, bool bResetSkillAni )
{
	if( !pNode )			return FALSE;
	if( !pNode->m_pDxSkinAni )	return FALSE;

	BOOL bReturn(FALSE);
	if( pNode->m_pDxSkinAni->m_dwFlag&ACF_UPBODY )
	{
		bReturn = m_sUpBodyAC.SetAnimation( pNode,  m_sUpBodyAC_Layer, bResetSkillAni );
	}
	else
	{
		bReturn = m_sDownBodyAC.SetAnimation( pNode,  m_sDownBodyAC_Layer, bResetSkillAni );
	}

	return bReturn;
}

BOOL DxAniControlCORE::SetAnimationEDIT( ANIMCONTNODE* pNode, BOOL bSameAniRefresh )
{
	if( !pNode )
		return FALSE;

	if( !pNode->m_pDxSkinAni )
		return FALSE;

	//m_sUpBodyAC.SetAnimation( pNode,  m_sUpBodyAC_Layer );
	return m_sDownBodyAC.SetAnimation( pNode,  m_sDownBodyAC_Layer, bSameAniRefresh );

	//return TRUE;
}

void DxAniControlCORE::SetMixAni( ANIMCONTNODE* pNode )
{
	m_sMixAC.SetMixAni( pNode );
}

void DxAniControlCORE::StopLoopSound()
{
	m_sUpBodyAC        .StopLoopSound();
	m_sDownBodyAC      .StopLoopSound();
	m_sUpBodyAC_Layer  .StopLoopSound();
	m_sDownBodyAC_Layer.StopLoopSound();
	m_sMixAC		   .StopLoopSound();
}

// 0 KeyFrame <-> 1 JiggleBone
void DxAniControlCORE::GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const
{
	m_sDownBodyAC.GetKeyJiggleWeight_PureThread( afKeyJiggleWeight, dwKeyJiggleWeightNUM, dwKey );
}





//////////////////////////////////////////////////////////////////////////
//							AnimationMove
//////////////////////////////////////////////////////////////////////////

// [AnimationMove] 애니메이션 움직임이 캐릭터에 영향을 받는 것인지 확인
BOOL DxAniControlCORE::IsAnimationMove()
{
	return m_sDownBodyAC.IsAnimationMove();
}

// [AnimationMove] 처음 애니메이션 시작인가~?
BOOL DxAniControlCORE::IsFirstAnimation()
{
	return m_sDownBodyAC.IsFirstAnimation();
}

// [AnimationMove] 처음 애니메이션 동작을 작업하였으면 FALSE로 셋팅한다.
void DxAniControlCORE::SetFirstAnimationFALSE()
{
	m_sDownBodyAC.SetFirstAnimationFALSE();
}

// [AnimationMove] 처음 애니메이션 동작의 위치를 얻는다.
const D3DXVECTOR3* DxAniControlCORE::GetFirstAnimationPos()
{
	return m_sDownBodyAC.GetFirstAnimationPos();
}

//////////////////////////////////////////////////////////////////////////








//--------------------------------------------------------------------------------------------------------------------
//						D	x		S	k	i	n		A	n	i		C	o	n	t	r	o	l
//--------------------------------------------------------------------------------------------------------------------
void DxSkinAniControl::SVEC::insert ( PANIMCONTNODE pValue )
{
	if ( m_dwSize >= ANI_MAX )
	{
		// [shhan][2014.10.22] Log 를 남긴다.
		TSTRING strName(_T(""));

		if ( pValue && pValue->m_pDxSkinAni )
		{
			strName += pValue->m_pDxSkinAni->m_strCfgFile;
		}

		for ( DWORD i=0; i<ANI_MAX; ++i )
		{
			if ( !m_pANI[i] )
				continue;

			if ( !m_pANI[i]->m_pDxSkinAni )
				continue;

			strName +=  TSTRING(", ") + m_pANI[i]->m_pDxSkinAni->m_strCfgFile;
		}
		CDebugSet::ToLogFile( strName );

		GASSERT( !(m_dwSize>=ANI_MAX) && _T("log를 참조해주세요.") );
		return;
	}

	m_pANI[m_dwSize] = pValue;
	m_dwSize++;
}

float DxSkinAniControl::GETSTARTTIME () const
{
	if (!m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)
        return 0.0f;
    else
	    return (float)m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->m_pDxSkinAni->m_dwSTime;
}

float DxSkinAniControl::GETENDTIME () const
{
	if (!m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)
        return 0.0f;
    else
	    return (float)m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->m_pDxSkinAni->m_dwETime;
}

DWORD DxSkinAniControl::GETUNITTIME () const
{
	if (!m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)
        return 160;
    else
	    return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->m_pDxSkinAni->m_UNITTIME;
}

BOOL DxSkinAniControl::IsCurAniConInfo(const EMANICONINFO emAniConInfo) const
{
	if ( m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode )
	{
		return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->m_pDxSkinAni->m_dwFlag & emAniConInfo;
	}
	return FALSE;
}

const DxSkinAnimation*	DxSkinAniControl::GETCURANIM () const
{ 
	return (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)?m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->m_pDxSkinAni:NULL; 
}

DxSkinAnimation*	DxSkinAniControl::GETCURANIM_none_const ()
{ 
	return (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)?m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->m_pDxSkinAni:NULL; 
}

const PANIMCONTNODE	DxSkinAniControl::GETCURANIMNODE ()	const
{ 
	return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode; 
}

//DxSkinAnimation*	DxSkinAniControl::GETCURANIM_UPBODY()
//{ 
//	return (m_sAniControlCORE.m_sUpBodyAC.m_rAnimNode) ? m_sAniControlCORE.m_sUpBodyAC.m_rAnimNode->m_pDxSkinAni:NULL; 
//}

EMANI_MAINTYPE DxSkinAniControl::GETCURMTYPE () const
{
	if (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)
        return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetMType();
    else	
	    return AN_NONE;
}

EMANI_SUBTYPE DxSkinAniControl::GETCURSTYPE () const
{
	if (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)
        return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetSType();
    else	
	    return AN_SUB_NONE;
}

float DxSkinAniControl::GETCURANITIME () const
{
	if (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)
		return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetTimeLength () / UNITANIKEY_PERSEC;
    else
	    return 4.0f;
}

DWORD DxSkinAniControl::GETCURKEYTIME () const
{
	return DWORD ( m_sAniControlCORE.m_sDownBodyAC.m_fTime / GETUNITTIME() );
}

BOOL DxSkinAniControl::ISENDANIM() const
{
	if( m_sAniControlCORE.m_sDownBodyAC.m_rAnimThreadLoad )
        return FALSE;
    else
	    return m_sAniControlCORE.m_sDownBodyAC.m_bEndAnim;
}
						
BOOL DxSkinAniControl::ISENDANIM( EMANICONINFO emBody ) const
{
	if( emBody & ACF_UPBODY )
        return m_sAniControlCORE.m_sDownBodyAC.m_bEndAnim;
	if( emBody & ACF_DOWNBODY )
        return m_sAniControlCORE.m_sUpBodyAC.m_bEndAnim;

	return FALSE;
}

BOOL DxSkinAniControl::IsAnimName( const TCHAR* szAnim ) const
{
	const DxSkinAnimation* pSkinAnimation = GETCURANIM();
	if ( !pSkinAnimation )
		return FALSE;

	if ( pSkinAnimation->m_strCfgFile == szAnim )
		return TRUE;

	return FALSE;
}

DxSkinAniControl::DxSkinAniControl (void) 
    : m_pd3dDevice(NULL)
	, m_dwFlags(NULL)
	, m_fWeight(1.f)
	, m_bRandomStartFrame(true)
	, m_fScale(1.f)
	, m_scpJiggleBone(new DxJiggleBone)
	, m_scpJiggleBoneColl(new DxJiggleBoneColl)
	, m_nAnimationMove_PostWork(0)
	, m_vAniMovePos(0.f,0.f,0.f)
	, m_vAniMoveOffset(0.f,0.f,0.f)
	, m_vAniMoveOffsetStart(0.f,0.f,0.f)
{
	m_pEffAniPlayer					= new DxEffAniPlayer;
	m_pSkeleton						= new DxSkeletonMaintain;
	m_pSkeletonBackUp				= new DxSkeletonMaintain;
	m_pvecClothCollSpheres			= new std::vector<physx::PxClothCollisionSphere>;
	m_pvecClothCollSpheresBackUp	= new std::vector<physx::PxClothCollisionSphere>;
	m_pmatWorld						= new D3DXMATRIX;
	m_pmatWorldBackUp				= new D3DXMATRIX;

	// 초기값을 셋팅해줄 필요가 있다.
	// // [shhan][2015.04.22] rm #906, rm #1213 이유로 인해서...
	D3DXMatrixIdentity( const_cast<D3DXMATRIX*>( m_pmatWorld ) );

	//D3DXMatrixIdentity( &m_matAniMoveStartWorld );
}

DxSkinAniControl::~DxSkinAniControl (void)
{
	ClearAll ();

	SAFE_DELETE ( m_pEffAniPlayer );
	SAFE_DELETE ( m_pSkeleton );
	SAFE_DELETE ( m_pSkeletonBackUp );
	SAFE_DELETE ( m_pvecClothCollSpheres );
	SAFE_DELETE ( m_pvecClothCollSpheresBackUp );
	SAFE_DELETE ( m_pmatWorld );
	SAFE_DELETE ( m_pmatWorldBackUp );

	for ( DWORD i=0; i<m_vecReserveFunction.size(); ++i )
	{
		g_poolAniActionStack.destroy( m_vecReserveFunction[i] );
	}
	m_vecReserveFunction.clear();
}

void DxSkinAniControl::ClearAll ()
{
	m_pEffAniPlayer->RemoveAllEff();

	m_strSkeleton.clear();

	m_sAniControlCORE.CleanUp();

	for ( int i=0; i<AN_TYPE_SIZE; i++ )
		for ( int j=0; j<AN_SUB_00_SIZE; j++ )
			m_vecAniType[i][j].clear();

	//std::for_each ( m_vecAnim.begin(), m_vecAnim.end(), std_afunc::DeleteObject() );
	size_t dwSize = m_vecAnim.size();
	for( size_t i=0; i<dwSize; ++i )
	{
		g_poolANICONTNODE.destroy( m_vecAnim[i] );
		m_vecAnim[i] = NULL;
	}
	m_vecAnim.clear();
}

DWORD DxSkinAniControl::GETANIAMOUNT () const
{
	return (DWORD) m_vecAnim.size();
}

bool DxSkinAniControl::IsAni ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType ) const
{
    return GETANIAMOUNT(MType, SType) != 0;
}

PANIMCONTNODE DxSkinAniControl::GETANI_none_const ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwCount ) const
{
	if( m_vecAnim.empty() )
		return NULL;

	DWORD dwAmount = GETANIAMOUNT(MType,SType);
	if ( dwAmount==0 )
        return m_vecAnim[0];

	//	순서에 맞는 에니메이션 찾음.
	if ( m_vecAniType[MType][SType].size()<dwCount )
        dwCount = 0;

	return m_vecAniType[MType][SType].get(dwCount);
}

const ANIMCONTNODE* DxSkinAniControl::findanicontnode(const std::string& AnimNode) const
{
	VECNODE_CITER found = std::lower_bound(m_vecAnim.begin(), m_vecAnim.end(), AnimNode, ANIMCONTNODE_OPER());
	if (found == m_vecAnim.end())
        return NULL;
    
	if (!(*found)->m_pDxSkinAni->m_strCfgFile.compare(AnimNode))
        return (*found);
    else
	    return NULL;
}

ANIMCONTNODE* DxSkinAniControl::findanicontnode_none_const(const std::string& AnimNode)
{
	VECNODE_ITER found = std::lower_bound(m_vecAnim.begin(), m_vecAnim.end(), AnimNode, ANIMCONTNODE_OPER());
	if (found == m_vecAnim.end())
		return NULL;

	if (!(*found)->m_pDxSkinAni->m_strCfgFile.compare(AnimNode))
		return (*found);
	else
		return NULL;
}

const DxSkinAnimation*	DxSkinAniControl::findanicont ( const TCHAR* szAnimNode ) const
{
	const ANIMCONTNODE* pNode = findanicontnode ( szAnimNode );
	if ( pNode )
        return pNode->m_pDxSkinAni;
    else
	    return NULL;
}

DxSkinAnimation*	DxSkinAniControl::findanicont_none_const( const TCHAR* szAnimNode )
{
	ANIMCONTNODE* pNode = findanicontnode_none_const ( szAnimNode );
	if ( pNode )
		return pNode->m_pDxSkinAni;
	else
		return NULL;
}

const PANIMCONTNODE DxSkinAniControl::GETANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType ) const
{
	if( m_vecAnim.empty() )
        return NULL;

	DWORD dwAmount = GETANIAMOUNT(MType,SType);
	if ( dwAmount==0 )
        return m_vecAnim[0];
    else
	    return m_vecAniType[MType][SType].get(0);
}

DWORD DxSkinAniControl::GETANIAMOUNT ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType ) const
{
	if ( AN_TYPE_SIZE <= MType )
		return 0;
	if ( AN_SUB_00_SIZE <= SType )
		return 0;

	return m_vecAniType[MType][SType].size();
}

void DxSkinAniControl::InitAnimation( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinAnimation* pAnimCont )
{
	ANIMCONTNODE *pNewNode = g_poolANICONTNODE.construct( pAnimCont );
	//pNewNode->m_pDxSkinAni = pAnimCont;
	m_vecAnim.push_back ( pNewNode );
}


//BOOL DxSkinAniControl::ReleaseAnimation ( const TCHAR* szAnim )
//{
//	VECNODE_ITER found = std::lower_bound ( m_vecAnim.begin(), m_vecAnim.end(), szAnim, ANIMCONTNODE_OPER() );
//	if ( found==m_vecAnim.end() )	return NULL;
//
//	if ( !_tcscmp((*found)->m_pDxSkinAni->m_strCfgFile.c_str(),szAnim) )
//	{
//		g_poolANICONTNODE.releaseMem( (*found) );	// 메모리 풀에서 가져왔던 것을 되돌려 준다.
//		(*found) = NULL;
//		m_vecAnim.erase ( found );
//	}
//
//	return FALSE;
//}

BOOL DxSkinAniControl::LoadSkeletonForEdit ( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(szSkeleton&&"DxSkinAniControl::LoadSkeleton()");

	m_strSkeleton = szSkeleton;
	m_skeletonResThread = DxSkeletonManager::GetInstance().LoadSkeleton( m_strSkeleton, TRUE );
	
	////////////////////////////////////////////////////////////////////////////
	//// 팔벌리고라도 서 있도록 한다. 
	//// 이게 없으면 처음 캐릭터가 나왔지만 Bone 정보가 없게 된다.
	//m_pSkeleton->Import( m_skeletonResThread.get() );

	return TRUE;
}

BOOL DxSkinAniControl::LoadAnimationForEdit(const std::string& AnimName, LPDIRECT3DDEVICEQ pd3dDevice )
{
	const ANIMCONTNODE* pNode = findanicontnode(AnimName);
	if (pNode)
        return TRUE;

    // [Thread Work shhan ]

	TSTRING strBinFile	= sc::util::ChangeExtName( AnimName, _T("cfg") );
	BOOL bREPRESH = FALSE;

	DxSkinAnimationRes skinAniRes = DxSkinAnimationManager::GetInstance().LoadAnimContainerFromTool(
		strBinFile,
		AnimName,
		std::string(GetSkeletonName()),
		bREPRESH);

	if (!skinAniRes.IsValid())
		return FALSE;

 //  DxSkinAnimationRes skinAniRes = DxSkinAnimationManager::GetInstance().LoadAnimationCFG( AnimName );
	//if ( !skinAniRes.IsValid() )
 //       return FALSE;
    
	DxSkinAnimation* pSkinAnimation = skinAniRes.get();
	ANIMCONTNODE *pNewNode = g_poolANICONTNODE.construct( pSkinAnimation );
	//pNewNode->m_pDxSkinAni = skinAniRes.get();

	pNewNode->ChaSound = skinAniRes.get()->m_ChaSoundData;

	m_vecAnim.push_back ( pNewNode );
	std::sort ( m_vecAnim.begin(), m_vecAnim.end(), ANIMCONTNODE_OPER() );

	return TRUE;
}

BOOL DxSkinAniControl::ReleaseAnimationForEdit(const std::string& AnimName)
{
	VECNODE_ITER found = std::lower_bound ( m_vecAnim.begin(), m_vecAnim.end(), AnimName, ANIMCONTNODE_OPER() );
	if ( found==m_vecAnim.end() )
        return NULL;

	if ( !(*found)->m_pDxSkinAni->m_strCfgFile.compare(AnimName))
	{
		g_poolANICONTNODE.destroy( (*found) );	// 메모리 풀에서 가져왔던 것을 되돌려 준다.
		(*found) = NULL;
		m_vecAnim.erase ( found );
	}

	return FALSE;
}

void DxSkinAniControl::ClassifyAnimation ()
{
	for ( int i=0; i<AN_TYPE_SIZE; i++ )
	for ( int j=0; j<AN_SUB_00_SIZE; j++ )
	{
		m_vecAniType[i][j].clear ();
	}

	std::sort ( m_vecAnim.begin(), m_vecAnim.end(), ANIMCONTNODE_OPER() );

	for ( size_t n=0; n<m_vecAnim.size(); ++n )
	{
		int MType = m_vecAnim[n]->GetMType();
		int SType = m_vecAnim[n]->GetSType();
		m_vecAniType[MType][SType].insert(m_vecAnim[n]);
	}
}

void DxSkinAniControl::ReAttachBoneForEdit()
{
    m_skeletonResThread = DxSkeletonManager::GetInstance().LoadSkeleton(m_skeletonResThread.GetFileName(), FALSE);

	////////////////////////////////////////////////////////////////////////////
	//// 팔벌리고라도 서 있도록 한다. 
	//// 이게 없으면 처음 캐릭터가 나왔지만 Bone 정보가 없게 된다.
	//m_pSkeleton->Import( m_skeletonResThread.get() );

    BOOST_FOREACH(ANIMCONTNODE* pAniNode, m_vecAnim)
    {
        if (pAniNode->m_pDxSkinAni->GetAniListResource().IsValid())
        {
            pAniNode->m_pDxSkinAni->GetAniListResource().get()->ReAttachBoneForEdit(m_skeletonResThread.get());
        }
    }
}

//void DxSkinAniControl::Assign ( const DxSkinAniControl& value )
//{
//	//std::for_each ( m_vecAnim.begin(), m_vecAnim.end(), std_afunc::DeleteObject() );
//	size_t dwSize = m_vecAnim.size();
//	for( size_t i=0; i<dwSize; ++i )
//	{
//		g_poolANICONTNODE.releaseMem( m_vecAnim[i] );
//		m_vecAnim[i] = NULL;
//	}
//	m_vecAnim.clear();
//
//	size_t nSIZE = value.m_vecAnim.size();
//	for ( size_t n=0; n<nSIZE; n++ )
//	{
//		PANIMCONTNODE pNODE = value.m_vecAnim[n];
//
//		ANIMCONTNODE *pNewNode = g_poolANICONTNODE.getMem();
//		pNewNode->m_pDxSkinAni = pNODE->m_pDxSkinAni;
//		pNewNode->ChaSound = pNODE->m_pDxSkinAni->m_ChaSoundData;
//		m_vecAnim.push_back ( pNewNode );
//	}
//
//	memcpy ( m_vecAniType, value.m_vecAniType, sizeof(SVEC)*AN_TYPE_SIZE*AN_SUB_00_SIZE );
//}

const SBONESCALE* DxSkinAniControl::GetBoneScale( const int nIndex ) const
{
	if( static_cast<int>(m_deqBoneEditScale.size()) < nIndex ) return NULL;
	return &m_deqBoneEditScale[nIndex];
}

void DxSkinAniControl::AddBoneScale( const SBONESCALE& boneScale )
{
	DEQ_BONESCALE_ITER iter;
	for( iter = m_deqBoneEditScale.begin(); iter != m_deqBoneEditScale.end(); ++iter )
	{
		if( (*iter).strBoneName == boneScale.strBoneName )
		{
			(*iter).strBoneName = boneScale.strBoneName;
			(*iter).vScale	   = boneScale.vScale;
			return;
		}
	}

	m_deqBoneEditScale.push_back(boneScale);
}

bool DxSkinAniControl::EraseBoneScale( const int nIndex )
{
	if( static_cast<int>(m_deqBoneEditScale.size()) < nIndex )
        return FALSE;

	m_deqBoneEditScale.erase( m_deqBoneEditScale.begin() + nIndex );

	return TRUE;
}



// Thread 와는 상관없는 곳.
// 여기서는 Ani Thread 가 동작되지 않는다.
void DxSkinAniControl::SwapBone( const D3DXMATRIX& matWorld )
{
	// World Matrix
	{
		const D3DXMATRIX* rTemp = m_pmatWorld;
		m_pmatWorld = m_pmatWorldBackUp;
		m_pmatWorldBackUp = const_cast<D3DXMATRIX*>( rTemp );
	}

	// DxSkeletonMaintain
	{
		const DxSkeletonMaintain* rTemp = m_pSkeleton;
		m_pSkeleton = m_pSkeletonBackUp;
		m_pSkeletonBackUp = const_cast<DxSkeletonMaintain*>( rTemp );
	}

	// physx::PxClothCollisionSphere
	{
		const std::vector<physx::PxClothCollisionSphere>* rTemp = m_pvecClothCollSpheres;
		m_pvecClothCollSpheres = m_pvecClothCollSpheresBackUp;
		m_pvecClothCollSpheresBackUp = const_cast<std::vector<physx::PxClothCollisionSphere>*>( rTemp );
	}

	// 본이 변경될일은 없다.
	// 첫 Bone 생성 타임을 이걸로 잡아도 됨.
	// 첫 Bone 생성 타임 때 기초적인 작업들이 필요함.
	// 여기서는 Ani Thread 가 동작되지 않기 때문에 m_skeletonResThread.get() 을 사용해도 된다.
	if ( m_pSkeleton->IsActiveBone() && !m_pSkeletonBackUp->IsActiveBone() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		if ( ResetBone_PureThread() )
		{
			m_skeletonResThread.get()->UpdateBones_PureThread( m_skeletonResThread.get()->m_skeletonPart.pBoneRoot, matIdentity, 1.f, 1.f );
		}

		m_scpJiggleBone->Import ( m_spSkinAniControlData->GetJiggleBone(), m_skeletonResThread.get(), m_fScale );			// DxJiggleBone
		m_scpJiggleBoneColl->Import ( m_spSkinAniControlData->GetJiggleBoneColl(), m_skeletonResThread.get(), m_fScale );	// DxJiggleBoneColl
	}

	// DxSkeletonMaintain 스왑 후 필요작업.
	// m_pActor->setGlobalPose( pose ); 변경된다.
	m_scpJiggleBone->FrameMove_MainThread( *m_pSkeleton, matWorld );
	m_scpJiggleBoneColl->FrameMove( *m_pSkeleton );

	// 이전키 얻는다.

	// 이전까지 모았던 m_sAniControlCORE 관련 함수들을 실행시킨다.
	PlayReserveFuction();
}

void DxSkinAniControl::TOSTARTTIME()
{ 
	m_vecReserveFunction.push_back( g_poolAniActionStack.construct(0) );
}

void DxSkinAniControl::TOENDTIME()												
{ 
	m_vecReserveFunction.push_back( g_poolAniActionStack.construct(1) ); 
}

void DxSkinAniControl::DOSHOCKMIX()												
{ 
	// [shhan][2016.09.23] DOSHOCKMIX (2) 가 존재하면 추가하지 않는다.
	for ( DWORD i=0; i<m_vecReserveFunction.size(); ++i )
	{
		if ( m_vecReserveFunction[i]->m_cIndex == 2 )
		{
			return;
		}
	}

	m_vecReserveFunction.push_back( g_poolAniActionStack.construct(2) ); 
}

void DxSkinAniControl::SetPosition( const D3DXVECTOR3& vPos )						
{
	DxSkinAniControl::MAIN* pData = g_poolAniActionStack.construct(3);
	pData->m_vPos = vPos;
	m_vecReserveFunction.push_back( pData ); 
}

void DxSkinAniControl::DEFAULTANI()												
{ 
	// [shhan][2016.09.23] 애니메이션이 변경되면 이전 것을 지워준다.
	for ( DWORD i=0; i<m_vecReserveFunction.size(); ++i )
	{
		g_poolAniActionStack.destroy( m_vecReserveFunction[i] );
	}
	m_vecReserveFunction.clear();

	m_vecReserveFunction.push_back( g_poolAniActionStack.construct(4) ); 
}

void DxSkinAniControl::SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect )	
{ 
	// [shhan][2016.09.23] 애니메이션이 변경되면 이전 것을 지워준다.
	for ( DWORD i=0; i<m_vecReserveFunction.size(); ++i )
	{
		g_poolAniActionStack.destroy( m_vecReserveFunction[i] );
	}
	m_vecReserveFunction.clear();

	DxSkinAniControl::MAIN* pData = g_poolAniActionStack.construct(5);
	pData->m_cMType = static_cast<BYTE>(MType);
	pData->m_cSType = static_cast<BYTE>(SType);
	pData->m_cAniIndex = static_cast<BYTE>(dwSelect);
	pData->m_dwFlags = dwFlags;
	m_vecReserveFunction.push_back( pData ); 
}

void DxSkinAniControl::FrameMove_AniTime( float fElapsedTime, BOOL bContinue )	
{ 
	DxSkinAniControl::MAIN* pData = g_poolAniActionStack.construct(6);
	pData->m_fElapsedTime = fElapsedTime;
	pData->m_dwFlags = bContinue ? 1 : 0;
	m_vecReserveFunction.push_back( pData ); 
}

// ANIMCONTNODE 를 바탕으로 M,S 기준 몇번째 Index 에 있는지 찾는다.
DWORD DxSkinAniControl::GetAniIndex( ANIMCONTNODE* pNode ) const
{
	DWORD dwAmount = GETANIAMOUNT( pNode->GetMType(), pNode->GetSType() );
	if ( dwAmount==0 )
		return -1;

	const SVEC& sVec = m_vecAniType[pNode->GetMType()][pNode->GetSType()];

	//	순서에 맞는 에니메이션 찾음.
	for ( DWORD i=0; i<sVec.size(); ++i )
	{
		if ( sVec.get(i) == pNode )
			return i;
	}

	return -1;
}

void DxSkinAniControl::SELECTANI( const TCHAR* szAnim, DWORD dwFlags )
{
	ANIMCONTNODE* pNode = findanicontnode_none_const ( szAnim );
	if( !pNode )
		return;

	DWORD dwIndex = GetAniIndex( pNode );
	if ( dwIndex == -1 )
		return;

	SELECTANI( pNode->GetMType(), pNode->GetSType(), dwFlags, dwIndex );
}

void DxSkinAniControl::PlayReserveFuction()
{
	for ( DWORD i=0; i<m_vecReserveFunction.size(); ++i )
	{
		switch( m_vecReserveFunction[i]->m_cIndex )
		{
		case 0:
			re_TOSTARTTIME ();
			break;
		case 1:
			re_TOENDTIME ();
			break;
		case 2:
			re_DOSHOCKMIX();
			break;
		case 3:
			re_SetPosition ( m_vecReserveFunction[i]->m_vPos );
			break;
		case 4:
			re_DEFAULTANI();
			break;
		case 5:
			re_SELECTANI( static_cast<EMANI_MAINTYPE>(m_vecReserveFunction[i]->m_cMType), 
							static_cast<EMANI_SUBTYPE>(m_vecReserveFunction[i]->m_cSType), 
							m_vecReserveFunction[i]->m_dwFlags, 
							m_vecReserveFunction[i]->m_cAniIndex );
			break;
		case 6:
			re_FrameMove_AniTime( m_vecReserveFunction[i]->m_fElapsedTime, m_vecReserveFunction[i]->m_dwFlags ? TRUE : FALSE );
			break;
		};
	}

	for ( DWORD i=0; i<m_vecReserveFunction.size(); ++i )
	{
		g_poolAniActionStack.destroy( m_vecReserveFunction[i] );
	}
	m_vecReserveFunction.clear();
}

// [shhan][2014.12.18] GLCharClient 에서 FrameMove 를 호출 하지 않으면서 PlayReserveFuction 불려지지 않아 ClearReserveFuction 를 강제로 호출하도록 함.
//						m_vecReserveFunction.clear(); 가 되지않아 메모리 문제가 생겼었음.
void DxSkinAniControl::ClearReserveFuction()
{
	for ( DWORD i=0; i<m_vecReserveFunction.size(); ++i )
	{
		g_poolAniActionStack.destroy( m_vecReserveFunction[i] );
	}
	m_vecReserveFunction.clear();
}


//
//HRESULT DxSkinAniControl::Render( const D3DXMATRIX &matRot, const std::string& BoneName, const D3DXQUATERNION& qBoneROT )
//{
//	if( !m_skeletonResThread.IsValid() )	return E_FAIL;
//	if( !RENDERPARAM::bCALCULATE_BONE )	return S_OK;
//
//    DxSkeletonPart* pSkeletonPart = GetSkeletonPart();
//    if( !pSkeletonPart )
//		return E_FAIL;
//
//	//	Note : 에니메이션에 모든 본의 에니메이션 키가 없을 때를 대비하여
//	//		키를 모두 리샛함.
//	//
//	DxSkinAniControl::ResetBone ();
//
//	//	Note : 현제 프레임에 맞는 에니메이션 키를 설정.
//	m_sAniControlCORE.UpdateTime( m_fWeight, m_vecAnim[0] );
//
//	m_sAniControlCORE.UpdateMix();
//
//	// Note : 원하는 본 매트릭스를 수정한다.
//	DxBoneTrans *pBoneCur = m_skeletonResThread.get()->FindBone_t( BoneName );
//	if ( pBoneCur )
//	{
//		D3DXQuaternionMultiply( &pBoneCur->m_pQuatPosCUR.m_vQuat, &qBoneROT, &pBoneCur->m_pQuatPosCUR.m_vQuat );
//	}
//
//	//	Note : 계층 메트릭스 업데이트.
//	//DxSkeleton::g_fWeight = m_fWeight;
//	m_skeletonResThread.get()->UpdateBones( pSkeletonPart->pBoneRoot, matRot, m_fWeight, 1.f );
//
//	return S_OK;
//}
//
//HRESULT DxSkinAniControl::Render( const D3DXMATRIX &matRot, const std::string& BoneName, const D3DXMATRIX& matBoneROT )
//{
//	if( !m_skeletonResThread.IsValid() )	return E_FAIL;
//	if( !RENDERPARAM::bCALCULATE_BONE )	return S_OK;
//
//    DxSkeletonPart* pSkeletonPart = GetSkeletonPart();
//    if( !pSkeletonPart )
//		return E_FAIL;
//
//	//	Note : 에니메이션에 모든 본의 에니메이션 키가 없을 때를 대비하여
//	//		키를 모두 리샛함.
//	//
//	DxSkinAniControl::ResetBone ();
//
//	//	Note : 현제 프레임에 맞는 에니메이션 키를 설정.
//	m_sAniControlCORE.UpdateTime( m_fWeight, m_vecAnim[0] );
//
//	m_sAniControlCORE.UpdateMix();
//
//	// Note : 원하는 본 매트릭스를 수정한다.
//	DxBoneTrans *pBoneCur = m_skeletonResThread.get()->FindBone_t( BoneName );
//	if ( pBoneCur )
//	{
//		D3DXMatrixMultiply( &pBoneCur->matRot, &matBoneROT, &pBoneCur->matRot );
//	}
//
//	//	Note : 계층 메트릭스 업데이트.
//	//DxSkeleton::g_fWeight = m_fWeight;
//	m_skeletonResThread.get()->UpdateBones( pSkeletonPart->pBoneRoot, matRot, m_fWeight, 1.f );
//
//	return S_OK;
//}

//const LPD3DXMATRIX DxSkinAniControl::GetRHandMatrix()
//{
//	if ( !m_skeletonResThread.IsValid() )
//        return NULL;
//
//	// Note : 매트릭스를 얻어 놓는다.
//	DxBoneTrans *pBoneCur = NULL;
//	pBoneCur = m_skeletonResThread.get()->FindBone_t( _T("Bip01_R_Hand") );
//	if ( pBoneCur )
//        return &pBoneCur->matCombined;
//	else
//        return NULL;
//}
//
//const LPD3DXMATRIX DxSkinAniControl::GetLHandMatrix()
//{
//	if ( !m_skeletonResThread.IsValid() )	
//        return NULL;
//
//	// Note : 매트릭스를 얻어 놓는다.
//	DxBoneTrans *pBoneCur = NULL;
//	pBoneCur = m_skeletonResThread.get()->FindBone( _T("Bip01_L_Hand") );
//	if ( pBoneCur )
//        return &pBoneCur->matCombined;
//	else
//        return NULL;
//}

DxSkeletonPart* DxSkinAniControl::GetSkeletonPart_PureThread() const
{
	if( !m_skeletonResThread.IsValid() )
		return NULL;

	return &m_skeletonResThread.get()->m_skeletonPart;
}

void DxSkinAniControl::StopLoopSound()
{
	m_sAniControlCORE.StopLoopSound();
}

BOOL DxSkinAniControl::ResetBone_PureThread()
{	
	if( !m_skeletonResThread.IsValid() )
		return FALSE;

	DxSkeletonPart* pSkeletonPart = &m_skeletonResThread.get()->m_skeletonPart;
	if( !pSkeletonPart )
		return FALSE;

	if ( !pSkeletonPart->pBoneRoot )
		return FALSE;

	pSkeletonPart->pBoneRoot->ResetBone_PureThread(); 

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
HRESULT DxSkinAniControl::RestoreDeviceObjects ()
{
	m_pEffAniPlayer->RestoreDeviceObjects();

	return S_OK;
}

HRESULT DxSkinAniControl::InvalidateDeviceObjects ()
{
	m_pEffAniPlayer->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT DxSkinAniControl::DeleteDeviceObjects ()
{
	StopLoopSound();

	m_pEffAniPlayer->DeleteDeviceObjects ();

	//m_scpJiggleBone->CleanUp();
	//m_scpJiggleBoneColl->CleanUp();

	return S_OK;
}

void DxSkinAniControl::SetCharData ( boost::shared_ptr<DxSkinAniControlData> spCharData, LPDIRECT3DDEVICEQ pd3dDevice, float fScale, const TCHAR* rFileName, BOOL bMaintainAniminfo )
{
	m_pd3dDevice = pd3dDevice;
	m_fScale = fScale;

	// [shhan][2017.02.15] 첫 애니메이션이 특정 모션일 경우 랜덤으로 시작되도록 하기 위한 변수, 초기화.
	m_bRandomStartFrame = true;

	m_pEffAniPlayer->InitDeviceObjects( pd3dDevice );

	//	Note : 에니메이션 복제.
	//
	m_strSkeleton = spCharData->GetSkeletonName();
	if ( m_strSkeleton.empty() )
		return;

	m_skeletonResThread = DxSkeletonManager::GetInstance().LoadSkeleton( m_strSkeleton, TRUE );
	if ( !m_skeletonResThread.IsValid() )
		return;

	////////////////////////////////////////////////////////////////////////////
	//// 팔벌리고라도 서 있도록 한다. 
	//// 이게 없으면 처음 캐릭터가 나왔지만 Bone 정보가 없게 된다.
	//m_pSkeleton->Import( m_skeletonResThread.get() );

	m_spSkinAniControlData = spCharData;

	////if ( bJiggleBone )
	//{
	//	m_spJiggleBone->Import ( pCharData->GetJiggleBone(), m_skeletonResThread.get(), fScale );			// DxJiggleBone
	//	m_spJiggleBoneColl->Import ( pCharData->GetJiggleBoneColl(), m_skeletonResThread.get(), fScale );	// DxJiggleBoneColl
	//}

	//
	if ( bMaintainAniminfo == FALSE )
	{
		const DxSkinAniControlData::VEC_ANIMATION& vecANI = spCharData->GetAniList();
		size_t nSize = vecANI.size();
		for ( size_t n=0; n<nSize; ++n )
		{
			DxSkinAniControl::InitAnimation( pd3dDevice, vecANI[n].get() );
		}

		DxSkinAniControl::ClassifyAnimation ();

		// 팔 벌리고 서 있는걸 막는다.
		// if ( bDefaultAnim == TRUE )
		DxSkinAniControl::re_DEFAULTANI();
	}

	////---------------------------------------------------//
	//NSLoadingDirect::FrameMoveRender( FALSE );
	////---------------------------------------------------//

	//float fCurTime = GETCURTIME();
	////EMANI_MAINTYPE MType = GETCURMTYPE();
	////EMANI_SUBTYPE SType = GETCURSTYPE();
	////if ( MType == AN_NONE )
	////	MType = AN_GUARD_N;

	//EMANI_MAINTYPE MType = AN_GUARD_N;
	//EMANI_SUBTYPE  SType = AN_SUB_NONE;

	////	Note : 초기 에니메이션 설정.
	////
	//DxSkinAniControl::SETCURTIME ( fCurTime );
	//DxSkinAniControl::SELECTANI ( MType, SType );

	//if ( !GETCURANIMNODE() )
	//{
	//	DxSkinAniControl::DEFAULTANI();
	//}

	//if ( !GETCURANIMNODE() )
	//{
	//	std::string ErrorMsg(
	//		sc::string::format(
	//		"current animation node null point error [%1%] [M %2% S %3%]",
	//		rFileName,
	//		MType,
	//		SType));
	//	CDebugSet::ToLogFile(ErrorMsg);

	//	if (m_vecAnim.size())
	//		DEFAULTANI();
	//}

	// [ Thread Work shhan ]

	DxSkinAniControl::SetBoneScaleList(spCharData->GetBoneScaleList());
}

void DxSkinAniControl::FrameMove_EffAniPlayer( DxSkinCharEffAniBase* pSkinCharBase, SKINEFFDATA& sSkinEffData, float fTime, float fElapsedTime, BOOL bFreeze )
{
	m_pEffAniPlayer->FrameMove ( pSkinCharBase, GETCURANIM(), fTime, fElapsedTime, sSkinEffData );

	if ( !bFreeze && m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode )
	{
		m_pEffAniPlayer->CheckCreateEff ( m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->m_pDxSkinAni, GetPrevKeyFrame(), GETCURKEYTIME() );
	}
}

void DxSkinAniControl::Render_EffAniPlayer( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, float fAlpha )
{
	m_pEffAniPlayer->Render ( pd3dDevice, pSkinChar, *m_pmatWorld, fAlpha );
}
//
//void DxSkinAniControl::Render_EffAniPlayer( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIXA16& matLocalRot, float fAlpha )
//{
//	m_pEffAniPlayer->Render ( pd3dDevice, pAttBone, matLocalRot, fAlpha );
//}
//
//void DxSkinAniControl::Render_EffAniPlayer( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIXA16& matLocalRot, float fAlpha )
//{
//	m_pEffAniPlayer->Render ( pd3dDevice, pVehicle, matLocalRot, fAlpha );
//}

BOOL DxSkinAniControl::IsValid_Skeleton() const
{
	if( !m_skeletonResThread.IsValid() )
		return FALSE;

	return TRUE;
}

BOOL DxSkinAniControl::IsValid_Skeleton_DefaultAni() const
{
	if( m_vecAnim.empty() )
		return FALSE;

	if( !m_vecAnim[0]->m_pDxSkinAni )
		return FALSE;

	if ( !m_vecAnim[0]->m_pDxSkinAni->IsValidateANI() )
	{
		// 로딩하라고 호출이 안되었을 수도 있으니 로딩을 해본다.
		// 로비 같은 경우는 특정 애니메이션을 시작으로 쓰고 있어서 0 번 애니가 로딩 안 되어있다.
		m_vecAnim[0]->m_pDxSkinAni->SetAnimationList( TRUE );
		return FALSE;
	}

	return TRUE;
}

void DxSkinAniControl::RenderBone( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_skeletonResThread.IsValid() )
		return;

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

	m_skeletonResThread.get()->EditLineSphere( pd3dDevice );

	if ( !g_strJIGGLE_BONE.empty() )
	{
		DxBoneTrans* pBoneTrans = m_skeletonResThread.get()->FindBone_t( g_strJIGGLE_BONE );
		if ( pBoneTrans )
		{
			DxBoneTrans* pBoneTransPrev = m_skeletonResThread.get()->FindBone_t( g_strJIGGLE_BONE_PREV );
			if ( pBoneTransPrev )
			{
				D3DXVECTOR3 vPos;
				D3DXVECTOR3 vDir;
				vPos.x = pBoneTrans->matCombined._41;
				vPos.y = pBoneTrans->matCombined._42;
				vPos.z = pBoneTrans->matCombined._43;

				EDITMESHS::RENDERSPHERE( pd3dDevice, vPos, 0.003f*DxViewPort::GetInstance().GetDistance(), NULL, 0xff0000 );

				//D3DXVec3TransformNormal( &vDir, &g_vJIGGLE_BONE_DIR, &matCombined );

				D3DXMATRIX matCur;
				D3DXVECTOR3 vRotate = D3DXVECTOR3( g_vJIGGLE_BONE_ROTATE.x*D3DX_PI/180.f,
					g_vJIGGLE_BONE_ROTATE.y*D3DX_PI/180.f, 
					g_vJIGGLE_BONE_ROTATE.z*D3DX_PI/180.f );
				D3DXMatrixRotationYawPitchRoll( &matCur, vRotate.y, vRotate.x, vRotate.z );

				D3DXVECTOR3 vDirX(1.f,0.f,0.f);
				D3DXVECTOR3 vDirY(0.f,1.f,0.f);
				D3DXVECTOR3 vDirZ(0.f,0.f,1.f);
				D3DXVec3TransformNormal( &vDirX, &vDirX, &matCur );
				D3DXVec3TransformNormal( &vDirY, &vDirY, &matCur );
				D3DXVec3TransformNormal( &vDirZ, &vDirZ, &matCur );

				D3DXMATRIX matScale;
				D3DXMATRIX matCombined;
				D3DXMatrixScaling( &matScale, pBoneTrans->m_pQuatPosORIG.m_vScale.x, pBoneTrans->m_pQuatPosORIG.m_vScale.y, pBoneTrans->m_pQuatPosORIG.m_vScale.z );
				D3DXMatrixRotationQuaternion( &matCombined, &pBoneTrans->m_pQuatPosORIG.m_vQuat );
				D3DXMatrixMultiply( &matCombined, &matScale, &matCombined );
				matCombined._41 = pBoneTrans->m_pQuatPosORIG.m_vPos.x;
				matCombined._42 = pBoneTrans->m_pQuatPosORIG.m_vPos.y;
				matCombined._43 = pBoneTrans->m_pQuatPosORIG.m_vPos.z;
				D3DXMatrixMultiply( &matCombined, &matCombined, & pBoneTransPrev->matCombined );
				D3DXVec3TransformNormal( &vDirX, &vDirX, &matCombined );
				D3DXVec3TransformNormal( &vDirY, &vDirY, &matCombined );
				D3DXVec3TransformNormal( &vDirZ, &vDirZ, &matCombined );

				EDITMESHS::RENDERCONE( vPos, vDirX, vDirY, vDirZ, 1.f, D3DX_PI/180.f*g_fJIGGLE_BONE_SWING_Y, D3DX_PI/180.f*g_fJIGGLE_BONE_SWING_Z, 0xff0000 );

				vDir = vPos + vDirX*10.f;
				EDITMESHS::RENDERLINE( pd3dDevice, vPos, vDir, 0xff0000 );

				vDir = D3DXVECTOR3(1.f,0.f,0.f);
				D3DXVec3TransformNormal( &vDir, &vDir, &pBoneTrans->matCombined );
				EDITMESHS::RENDERCIRCLE( vPos, vDir, 1.f, 0xff0000 );

				vDir = D3DXVECTOR3(0.f,1.f,0.f);
				D3DXVec3TransformNormal( &vDir, &vDir, &pBoneTrans->matCombined );
				EDITMESHS::RENDERCIRCLE( vPos, vDir, 1.f, 0x00ff00 );

				vDir = D3DXVECTOR3(0.f,0.f,1.f);
				D3DXVec3TransformNormal( &vDir, &vDir, &pBoneTrans->matCombined );
				EDITMESHS::RENDERCIRCLE( vPos, vDir, 1.f, 0x0000ff );
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

	// Note : 본을 클릭하기 위함.
	if( DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT)&DXKEY_UP )
	{
		// Note : 픽킹을 하여 체크 된 Bone Name을 얻는다.
		//			현재 픽킹 범위를 넓혀 놓았다.
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
		if ( bTargetted )
		{
			m_skeletonResThread.get()->IsCollision( vFromPt, vTargetPt, g_strPICK_BONE, 0.006f*DxViewPort::GetInstance().GetDistance() );
			CDebugSet::ToView( 11, _T("Select Bone : %s"), g_strPICK_BONE.c_str() );
		}
	}

	// JiggleBone
	{
		m_scpJiggleBone->RenderEdit( pd3dDevice );		// DxJiggleBone
		m_scpJiggleBoneColl->RenderEdit( pd3dDevice );	// DxJiggleBone
	}

	// Clorh
	{
		m_spSkinAniControlData->GetClothColl()->RenderEdit( pd3dDevice, m_skeletonResThread.get() );		// Cloth
	}

	if( m_skeletonResThread.IsValid() && !g_bOBJECT100 )	
	{
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		m_skeletonResThread.get()->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
	}
}

const DxClothColl* DxSkinAniControl::GetClothColl() const
{ 
	return m_spSkinAniControlData->GetClothColl(); 
}

void DxSkinAniControl::SELECTANI_EDIT( const TCHAR* szAnim, BOOL bSameAniRefresh, DWORD dwFlag )
{
	ANIMCONTNODE* pNode = findanicontnode_none_const ( szAnim );
	if( !pNode )	return;

	// bin 파일 로딩작업을 한다. 이미 로딩되어있으면 추가로 하진 않는다.

	pNode->m_pDxSkinAni->SetAnimationList( FALSE );


	// [shhan][2014.12.24] 사운드와 효과에서 본과 bin 정보가 없어도 셋팅이 되야한다.
	//if( pNode->m_pDxSkinAni->IsValidateANI() )
	{
		if( pNode->ChaSound.m_PlayFrameCount != pNode->m_pDxSkinAni->m_ChaSoundData.m_PlayFrameCount )
		{
			pNode->ChaSound = pNode->m_pDxSkinAni->m_ChaSoundData;
		}
		//m_pEffAniPlayer->CheckCreateEff( pNode->m_pDxSkinAni, m_bGamePlayer );
		m_pEffAniPlayer->CheckCreateEff( pNode->m_pDxSkinAni );	//, m_bGamePlayer );
	}

	m_dwFlags = dwFlag;
	//pNode->SetSTime( -1 );
	//pNode->SetETime( -1 );
	if( m_sAniControlCORE.SetAnimationEDIT( pNode, bSameAniRefresh ) )
	{
		//m_fWeight = 1.f;
		m_fWeight = 0.f;
		//m_dwFlags = dwFlags;
	}
}

void DxSkinAniControl::SETCURTIME_ALL_FOR_EDIT( float fCurTime )
{
	m_sAniControlCORE.m_sUpBodyAC.m_fTime = fCurTime;
	m_sAniControlCORE.m_sDownBodyAC.m_fTime = fCurTime;
}

void DxSkinAniControl::SETCURKEYTIME_FOR_EDIT ( DWORD dwCurKeyTime )
{
	m_sAniControlCORE.m_sDownBodyAC.m_fTime = (float)(dwCurKeyTime * GETUNITTIME());
}
//
//void DxSkinAniControl::SETANIUPBODY( const TCHAR* szAnim )
//{
//	//	순서에 맞는 에니메이션 찾기.
//	PANIMCONTNODE pFoundAnimNode = findanicontnode_none_const( szAnim );
//	if( !pFoundAnimNode )
//	{
//		return;
//	}
//
//	// bin 파일 로딩작업을 한다. 이미 로딩되어있으면 추가로 하진 않는다.
//	pFoundAnimNode->m_pDxSkinAni->SetAnimationList( TRUE );
//	if( pFoundAnimNode->m_pDxSkinAni->IsValidateANI() )
//	{
//		if( pFoundAnimNode->ChaSound.m_PlayFrameCount != pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData.m_PlayFrameCount )
//		{
//			pFoundAnimNode->ChaSound = pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData;
//		}
//		m_pEffAniPlayer->CheckCreateEff( pFoundAnimNode->m_pDxSkinAni );	//, m_bGamePlayer );
//	}
//
//	//if( pFoundAnimNode->m_pDxSkinAni )
//	//{
//	//	// [ Thread Work shhan ]
//
//	//	////	if( pFoundAnimNode->pAnimCont->m_dwFlag&ACF_UPBODY )
//	//	////	{
//	//	//// Note : 이 값이 안들어가 있다...
//	//	////		다른 좋은곳이 있으면 옮기자.
//	//	//pFoundAnimNode->m_dwETimeSKT = pFoundAnimNode->m_pDxSkinAni->m_dwETime;
//
//	//	if( m_sAniControlCORE.SetAnimation( m_pd3dDevice, pFoundAnimNode, ACF_UPBODY ) )
//	//	{
//	//		//	m_fWeight = 0.f;
//	//		//	m_dwFlags = dwFlags;
//	//	}
//	//	//	}
//	//}
//}
//
//void DxSkinAniControl::SETANIUPBODY( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType )
//{
//	//	순서에 맞는 에니메이션 찾기.
//	PANIMCONTNODE pFoundAnimNode = GETANI ( MType, SType );	//, dwSelect );
//	if( !pFoundAnimNode )
//	{
//		return;
//	}
//
//	// bin 파일 로딩작업을 한다. 이미 로딩되어있으면 추가로 하진 않는다.
//	pFoundAnimNode->m_pDxSkinAni->SetAnimationList( TRUE );
//	if( pFoundAnimNode->m_pDxSkinAni->IsValidateANI() )
//	{
//		if( pFoundAnimNode->ChaSound.m_PlayFrameCount != pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData.m_PlayFrameCount )
//		{
//			pFoundAnimNode->ChaSound = pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData;
//		}
//		m_pEffAniPlayer->CheckCreateEff( pFoundAnimNode->m_pDxSkinAni );	//, m_bGamePlayer );
//	}
//
//	//if( pFoundAnimNode->m_pDxSkinAni )
//	//{
//	//	// [ Thread Work shhan ]
//
//	//	////	if( pFoundAnimNode->pAnimCont->m_dwFlag&ACF_UPBODY )
//	//	////	{
//	//	//// Note : 이 값이 안들어가 있다...
//	//	////		다른 좋은곳이 있으면 옮기자.
//	//	//pFoundAnimNode->m_dwETimeSKT = pFoundAnimNode->m_pDxSkinAni->m_dwETime;
//
//	//	if( m_sAniControlCORE.SetAnimation( m_pd3dDevice, pFoundAnimNode, ACF_UPBODY ) )
//	//	{
//	//		//	m_fWeight = 0.f;
//	//		//	m_dwFlags = dwFlags;
//	//	}
//	//	//	}
//	//}
//}
//



//////////////////////////////////////////////////////////////////////////

// 이것도 MainThread 에서 호출 된다.
void ANIMCONTNODE::StartAnim ()
{
	wBackFrame = WORD(m_pDxSkinAni->m_dwSTime);
	
	// 사운드 플레이 프레임 리셋
	ChaSound.PlayReset ( m_pDxSkinAni->m_dwSTime );
}

// 이건 Main Thread 에서 호출된다.
BOOL ANIMCONTNODE::FrameMove ( float &fCurTime, BOOL bContinue, DWORD dwFlag )
{
	BOOL bEndAnim = FALSE;
	
	//	Note : 에니메이션의 '시작지점' 체크.
	//
	if ( fCurTime < m_pDxSkinAni->m_dwSTime )
	{
		wBackFrame = WORD(m_pDxSkinAni->m_dwSTime);
		fCurTime = (float) m_pDxSkinAni->m_dwSTime;
	}

	////	Note : 에니메이션의 '종료지점' 체크.
	//if ( fCurTime > (m_pDxSkinAni->m_dwETime - (m_pDxSkinAni->m_UNITTIME*2)) )
	//{
	//	//	Note : 에니메이션 루프가 아닐때만 중단 결정.
	//	if ( !(m_pDxSkinAni->m_dwFlag&ACF_LOOP) )	bEndAnim = TRUE;
	//}
	if ( fCurTime > m_pDxSkinAni->m_dwETime )
	{
		//	Note : 에니메이션 루프가 아닐때만 중단 결정.
		if ( !(m_pDxSkinAni->m_dwFlag&ACF_LOOP&& !(dwFlag & EMANI_IGNORELOOP) ) )
			bEndAnim = TRUE;

		if ( dwFlag & EMANI_ENDFREEZE )
		{
			fCurTime = (float) m_pDxSkinAni->m_dwETime;
			fCurTime -= 1;
		}
		else
		{
			wBackFrame = WORD(m_pDxSkinAni->m_dwSTime);

			if( bEndAnim )
			{
				fCurTime = (float)(m_pDxSkinAni->m_dwETime);
				fCurTime -= 1;
			}
			else
			{
				// 사운드 플레이 프레임 리셋
				ChaSound.PlayReset ( m_pDxSkinAni->m_dwSTime );
				fCurTime = (float) m_pDxSkinAni->m_dwSTime;
			}			
		}
	}
	    

	//	Note : '!bContinue' 이고 중단 지점이 지정되어 있을때, 중단 시점에 도달했는지 체크후에 중단 시킴.
	//
	if ( !bContinue && !(m_pDxSkinAni->m_dwFlag&ACF_LOOP) )
	{
		for ( WORD i=0; i<m_pDxSkinAni->m_wDivCount; i++ )
		{
			const WORD &wThisTime = m_pDxSkinAni->m_wDivFrame[i];
			if ( wBackFrame < wThisTime && wThisTime <WORD(fCurTime) )
			{
				bEndAnim = TRUE;
			}
		}
	}

	wBackFrame = WORD(fCurTime);

	//	캐릭터 사운드 뿌리기
	ChaSound.PlayChaSound ( fCurTime, vPos );

	return bEndAnim;
}

// 이것도 MainThread 에서 호출 된다.
void ANIMCONTNODE::StopLoopSound()
{
	ChaSound.StopLoopSound();
}










//////////////////////////////////////////////////////////////////////////
//				딜레이되어 사용되는 함수들.
//////////////////////////////////////////////////////////////////////////
void DxSkinAniControl::re_TOSTARTTIME ()
{
	m_sAniControlCORE.m_sDownBodyAC.ToStartTime( GETSTARTTIME() );
}

void DxSkinAniControl::re_TOENDTIME ()
{
	m_sAniControlCORE.m_sDownBodyAC.ToEndTime( GETENDTIME() - GETUNITTIME() );
}

void DxSkinAniControl::re_DOSHOCKMIX ()
{
	if ( ISSHOCKMIX() )	return;

	DWORD dwAmount = GETANIAMOUNT(AN_SHOCK_MIX,AN_SUB_NONE);
	if ( dwAmount==0 )	return;

	//srand(::GetTickCount());
	DWORD dwSelect = rand()%dwAmount;

	//	순서에 맞는 에니메이션 찾기.
	PANIMCONTNODE pFoundAnimNode = GETANI_none_const( AN_SHOCK_MIX, AN_SUB_NONE, dwSelect );

	// Note : 이 값이 안들어가 있다...
	//		다른 좋은곳이 있으면 옮기자.
	if( pFoundAnimNode )
	{
		// bin 파일 로딩작업을 한다. 이미 로딩되어있으면 추가로 하진 않는다.
		pFoundAnimNode->m_pDxSkinAni->SetAnimationList( TRUE );

		// [shhan][2014.12.24] 사운드와 효과에서 본과 bin 정보가 없어도 셋팅이 되야한다.
		//if( pFoundAnimNode->m_pDxSkinAni->IsValidateANI() )
		{
			if( pFoundAnimNode->ChaSound.m_PlayFrameCount != pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData.m_PlayFrameCount )
			{
				pFoundAnimNode->ChaSound = pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData;
			}
			m_pEffAniPlayer->CheckCreateEff( pFoundAnimNode->m_pDxSkinAni );	//, m_bGamePlayer );
		}

		// [ Thread Work shhan ]

		//if( pFoundAnimNode->m_pDxSkinAni )
		//{
		//	pFoundAnimNode->m_dwETimeSKT = pFoundAnimNode->m_pDxSkinAni->m_dwETime;
		//}
	}

	// Note : Setting
	m_sAniControlCORE.SetMixAni( pFoundAnimNode );
}

void DxSkinAniControl::re_SELECTANI ( const TCHAR* szAnim, DWORD dwFlags )
{
	ANIMCONTNODE* pNode = findanicontnode_none_const ( szAnim );
	if( !pNode )
		return;	

	// bin 파일 로딩작업을 한다. 이미 로딩되어있으면 추가로 하진 않는다.
	pNode->m_pDxSkinAni->SetAnimationList( TRUE );

	// [shhan][2014.12.24] 사운드와 효과에서 본과 bin 정보가 없어도 셋팅이 되야한다.
	//if( pNode->m_pDxSkinAni->IsValidateANI() )
	{
		if( pNode->ChaSound.m_PlayFrameCount != pNode->m_pDxSkinAni->m_ChaSoundData.m_PlayFrameCount )
		{
			pNode->ChaSound = pNode->m_pDxSkinAni->m_ChaSoundData;
		}
		m_pEffAniPlayer->CheckCreateEff( pNode->m_pDxSkinAni );	//, m_bGamePlayer );
	}

	//// 이전 애니메이션이 AnimationMove 셋팅인지 확인
	//// 그렇다면 블렌딩은 작동되지 않는다.
	float fWeight(0.f);
	//if( m_sAniControlCORE.IsAnimationMove() )
	//{
	//	fWeight = 1.f;
	//}

	//pNode->SetSTime( -1 );
	//pNode->SetETime( -1 );
	if( m_sAniControlCORE.SetAnimation( pNode ) )
	{
		m_fWeight = fWeight;
		m_dwFlags = dwFlags;
	}

	return;
}

void DxSkinAniControl::re_DEFAULTANI()													
{
	if (m_vecAnim.empty())
	{
		std::string ErrorMsg(
			sc::string::format(
			"DxSkinAniControl::DEFAULTANI m_vecAnim.empty. %1%",
			m_strSkeleton));
		CDebugSet::ToLogFile(ErrorMsg);
		m_sAniControlCORE.CleanUp();
	}
	else
	{
		// bin 파일 로딩작업을 한다. 이미 로딩되어있으면 추가로 하진 않는다.
		m_vecAnim[0]->m_pDxSkinAni->SetAnimationList( TRUE );

		// [shhan][2014.12.24] 사운드와 효과에서 본과 bin 정보가 없어도 셋팅이 되야한다.
		//if( m_vecAnim[0]->m_pDxSkinAni->IsValidateANI() )
		{
			if( m_vecAnim[0]->ChaSound.m_PlayFrameCount != m_vecAnim[0]->m_pDxSkinAni->m_ChaSoundData.m_PlayFrameCount )
			{
				m_vecAnim[0]->ChaSound = m_vecAnim[0]->m_pDxSkinAni->m_ChaSoundData;
			}
			m_pEffAniPlayer->CheckCreateEff( m_vecAnim[0]->m_pDxSkinAni );	//, m_bGamePlayer );
		}

		m_sAniControlCORE.SetAnimation( m_vecAnim[0] );
	}


	//if( m_vecAnim.empty() )	
	//{
	//       std::string ErrorMsg(
	//           sc::string::format(
	//               "DxSkinAniControl::DEFAULTANI() - %1% - m_vecAnim.empty ", m_strSkeleton));
	//	CDebugSet::ToLogFile(ErrorMsg);
	//	m_sAniControlCORE.CleanUp();
	//}
	//else
	//{
	//	m_sAniControlCORE.SetAnimation( m_vecAnim[0] );
	//}
}

void DxSkinAniControl::re_SELECTANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect )
{
	//	Note : 갯수가 맞는지 점검.	
	if ( !GETANIAMOUNT(MType,SType) )
		SType = AN_SUB_NONE;

	DWORD dwAmount = GETANIAMOUNT(MType,SType);	

	if ( dwSelect >= dwAmount || dwAmount==0 )
	{
		m_sAniControlCORE.m_sDownBodyAC.m_bEndAnim = TRUE;	//	현재 에니메이션 바로 종료되게 함.

		//	현재 선택된 에니메이션이 없을때 오류를 방지 하기 위해서 기본 에니 선택.
		if ( m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode==NULL )
		{
			if ( !m_vecAnim.empty() )
			{
				m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode = m_vecAnim[0];
				MType = m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetMType();
				SType = m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetSType();
			}
			else
			{
				// 애니메이션이 없다.
				return;
			}
		}
		else
		{
			MType = m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetMType();
			SType = m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetSType();
		}
	}

	//	순서에 맞는 에니메이션 찾기.
	PANIMCONTNODE pFoundAnimNode = GETANI_none_const( MType, SType, dwSelect );
	if( !pFoundAnimNode )	
		return;

	// bin 파일 로딩작업을 한다. 이미 로딩되어있으면 추가로 하진 않는다.
	pFoundAnimNode->m_pDxSkinAni->SetAnimationList( TRUE );

	// [shhan][2014.12.24] 사운드와 효과에서 본과 bin 정보가 없어도 셋팅이 되야한다.
	//if( pFoundAnimNode->m_pDxSkinAni->IsValidateANI() )
	{
		if( pFoundAnimNode->ChaSound.m_PlayFrameCount != pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData.m_PlayFrameCount )
		{
			pFoundAnimNode->ChaSound = pFoundAnimNode->m_pDxSkinAni->m_ChaSoundData;
		}
		m_pEffAniPlayer->CheckCreateEff( pFoundAnimNode->m_pDxSkinAni );	//, m_bGamePlayer );
	}

	// [ Thread Work shhan ]

	//// 이전 애니메이션이 AnimationMove 셋팅인지 확인
	//// 그렇다면 블렌딩은 작동되지 않는다.
	float fWeight(0.f);
	//if( m_sAniControlCORE.IsAnimationMove() )
	//{
	//	fWeight = 1.f;
	//}

	//pFoundAnimNode->SetSTime( nSTime );
	//pFoundAnimNode->SetETime( nETime );
	if( m_sAniControlCORE.SetAnimation( pFoundAnimNode ) )
	{
		m_fWeight = fWeight;
		m_dwFlags = dwFlags;

		// [shhan][2017.02.15] 첫 애니메이션이 특정 모션일 경우 랜덤으로 시작되도록 하기 위한 변수 체크
		// CRM#1338 - 캐릭터및 몬스터의 애니메이션 시작프레임 변경
		if ( !(dwFlags & EMANI_ONLYZEROFRAMESTART) && 
			( MType==AN_GUARD_N || MType==AN_PLACID || MType==AN_WALK || MType==AN_RUN || MType==AN_GUARD_L) && 
			m_bRandomStartFrame )
		{
			m_bRandomStartFrame = false;
			float fEndTime = (float)((GETENDTIME()-GETSTARTTIME())*sc::Random::RANDOM_POS() + GETSTARTTIME());
			SETCURTIME ( fEndTime );
		}
	}

	return;
}

void DxSkinAniControl::re_FrameMove_AniTime( float fElapsedTime, BOOL bContinue )
{
	m_fWeight += fElapsedTime*7.f;	// 
	if( m_fWeight > 1.f )	m_fWeight = 1.f;

	if( m_dwFlags&EMANI_ENDFREEZE )
	{
		// 죽으면 fElapsedTime 가 0.f 가 온다... 
		// 그러면서 모션블렌딩이 안 일어나고, 안 일어나면 이전애니메이션이 화면에 나오게 된다.
		m_fWeight = 1.f; 
	}

	//	Note: 시간 경과 반영.
	m_sAniControlCORE.FrameMove( fElapsedTime, bContinue, m_dwFlags, m_fWeight );
}





//////////////////////////////////////////////////////////////////////////
//					Thread 관련 함수들
//////////////////////////////////////////////////////////////////////////

// TAG_CalculateAnimationThread_2
// 다른 Thread 상에서 호출된다.
BOOL DxSkinAniControl::CalculateBone_PureThread( const D3DXMATRIXA16& matLocalRot, 
												float arrayfKeyJiggleWeight[], 
												DWORD dwPartSize,
												WORDWORD wwMoveParentBoneIDForSafeModeWeaponSlot )	// SafeMode 에서 WeaponSlot 위치이동. Link 가 아닌 Skin 만을 위함
{
	if( !m_skeletonResThread.IsValid() )
		return FALSE;

	//	Note : Skin Animation.
	CalculateBone_PureThread_HARD( matLocalRot );

	//	Note : 특정 Bone 부터는 특정한 곳에 붙도록 한다.
	//			마술부 - 채찍, 마술상자 같이 스킨 되어 있는것도 손이 아닌 다른 착용위치에 붙어서 움직이도록 하기 위함.
	if ( wwMoveParentBoneIDForSafeModeWeaponSlot.dwData != 0xffffffff )
	{
		DxBoneTrans* pBoneTrans_Move = m_skeletonResThread.get()->FindBone_Index_Bone( wwMoveParentBoneIDForSafeModeWeaponSlot.wA );
		const DxBoneTrans* pBoneTrans_Parent = m_skeletonResThread.get()->FindBone_Index_Bone_const( wwMoveParentBoneIDForSafeModeWeaponSlot.wB );
		if ( pBoneTrans_Move && pBoneTrans_Parent )
		{
			m_skeletonResThread.get()->UpdateBones_PureThread( pBoneTrans_Move, pBoneTrans_Parent->matCombined, m_fWeight, 1.f );
		}
	}

	// Main 에서만 가중치를 계산하도록 한다.
	// 0 KeyFrame <-> 1 JiggleBone
	GetKeyJiggleWeight_PureThread( arrayfKeyJiggleWeight, dwPartSize+1 );

	// m_vecBoneTransJointData[i].m_rBoneTransJoint->matCombined 값이 변경된다.
	m_scpJiggleBone->FrameMove_PureThread( m_skeletonResThread.get(), arrayfKeyJiggleWeight[dwPartSize] );

	// 실제 사용하는 정보를 Import 한다.
	m_pSkeletonBackUp->Import_PureThread( m_skeletonResThread.get() );

	// 공틍으로 쓰이는 Data 임으로 m_pClothColl 의 값은 변경시키지 않는다.
	// 천 시뮬레이션 충돌정보 Update.
	m_spSkinAniControlData->GetClothColl()->FrameMove( *m_pvecClothCollSpheresBackUp, m_pSkeletonBackUp, matLocalRot, m_fScale );

	// 백업한다.
	*m_pmatWorldBackUp = matLocalRot;

	return TRUE;
}

BOOL DxSkinAniControl::CalculateBoneVehicle_PureThread( const std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleThreadNeedData, 
													   const D3DXMATRIXA16& matLocalRot,
													   float arrayfKeyJiggleWeight[], 
													   DWORD dwPartSize )
{
	if( !m_skeletonResThread.IsValid() )
		return FALSE;

	//	Note : Skin Animation.
	CalculateBone_PureThread_HARD( matLocalRot );

	// Main 에서만 가중치를 계산하도록 한다.
	// 0 KeyFrame <-> 1 JiggleBone
	GetKeyJiggleWeight_PureThread( arrayfKeyJiggleWeight, dwPartSize+1 );

	// m_vecBoneTransJointData[i].m_rBoneTransJoint->matCombined 값이 변경된다.
	m_scpJiggleBone->FrameMove_PureThread( m_skeletonResThread.get(), arrayfKeyJiggleWeight[dwPartSize] );

	// 실제 사용하는 정보를 Import 한다.
	m_pSkeletonBackUp->Import_PureThread( m_skeletonResThread.get() );

	// Passenger 위치를 Update 한다.
	spVehicleThreadNeedData->UpdatePassengerPos( GetSkeletonMaintainBackUp(), GetScale() );

	// 공틍으로 쓰이는 Data 임으로 m_pClothColl 의 값은 변경시키지 않는다.
	// 천 시뮬레이션 충돌정보 Update.
	m_spSkinAniControlData->GetClothColl()->FrameMove( *m_pvecClothCollSpheresBackUp, m_pSkeletonBackUp, matLocalRot, m_fScale );

	// 백업한다.
	*m_pmatWorldBackUp = matLocalRot;

	return TRUE;
}

// TAG_CalculateAnimationThread_2
// 다른 Thread 상에서 호출된다.
BOOL DxSkinAniControl::CalculateBoneAttBone_PureThread( D3DXMATRIX& matLocalRotOUT,
													   const std::tr1::shared_ptr<DxSkinAniControl>& spLinkSkinAniControl, 
													   const std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData, 
													   float fKeyJiggleWeight,
													   float fScale )
{
	if( !m_skeletonResThread.IsValid() )
		return FALSE;

	// Matrix를 구하자.
	// m_skeletonResThread.get() 을 쓰면 안된다 공용으로 쓰는거기 때문에 다른 캐릭터의 본에 AttBone 이 붙을 수 있다.
	// GetSkeletonMaintainBackUp 값은 미리 다 다 계산된 값이라서 사용해도 된다. 
	spThreadNeedData->ComputeMatrix( matLocalRotOUT, spLinkSkinAniControl->GetSkeletonMaintainBackUp() );

	// Scale 변경
	D3DXMATRIX matScale;
	D3DXMatrixIdentity( &matScale );
	//matScale._11 = spLinkSkinAniControl->GetScale() * fScale;
	//matScale._22 = spLinkSkinAniControl->GetScale() * fScale;
	//matScale._33 = spLinkSkinAniControl->GetScale() * fScale;
	matScale._11 = fScale;
	matScale._22 = fScale;
	matScale._33 = fScale;
	D3DXMatrixMultiply( &matLocalRotOUT, &matScale, &matLocalRotOUT );

	//	Note : Skin Animation.
	CalculateBone_PureThread_HARD( matLocalRotOUT );

	// m_vecBoneTransJointData[i].m_rBoneTransJoint->matCombined 값이 변경된다.
	m_scpJiggleBone->FrameMove_PureThread( m_skeletonResThread.get(), fKeyJiggleWeight );

	// 실제 사용하는 정보를 Import 한다.
	m_pSkeletonBackUp->Import_PureThread( m_skeletonResThread.get() );

	// 공틍으로 쓰이는 Data 임으로 m_pClothColl 의 값은 변경시키지 않는다.
	// 천 시뮬레이션 충돌정보 Update.
	m_spSkinAniControlData->GetClothColl()->FrameMove( *m_pvecClothCollSpheresBackUp, m_pSkeletonBackUp, matLocalRotOUT, m_fScale );

	// 백업한다.
	*m_pmatWorldBackUp = matLocalRotOUT;

	return TRUE;
}

// TAG_CalculateAnimationThread_2
// 다른 Thread 상에서 호출된다.
BOOL DxSkinAniControl::CalculateBoneCharOnVehicle_PureThread( D3DXMATRIX& matLocalRotOUT,
															   float arrayfKeyJiggleWeight[], 
															   DWORD dwPartSize,
															   const std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleThreadNeedData, 
															   DWORD dwPassengerID,
															   WORDWORD wwMoveParentBoneIDForSafeModeWeaponSlot )	// SafeMode 에서 WeaponSlot 위치이동. Link 가 아닌 Skin 만을 위함
{
	if( !m_skeletonResThread.IsValid() )
		return FALSE;

	// Matrix를 구하자.
	// m_skeletonResThread.get() 을 쓰면 안된다 공용으로 쓰는거기 때문에 다른 Vehicle의 본에 붙을 수 있다.
	// GetSkeletonMaintainBackUp 값은 미리 다 다 계산된 값이라서 사용해도 된다. 
	const D3DXMATRIX* pMatrix = spVehicleThreadNeedData->GetPassengerMatrix( dwPassengerID );
	if ( pMatrix )
	{
		matLocalRotOUT = *pMatrix;
	}
	else
	{
		D3DXMatrixIdentity( &matLocalRotOUT );
	}

	//	Note : Skin Animation.
	CalculateBone_PureThread_HARD( matLocalRotOUT );

	//	Note : 특정 Bone 부터는 특정한 곳에 붙도록 한다.
	//			마술부 - 채찍, 마술상자 같이 스킨 되어 있는것도 손이 아닌 다른 착용위치에 붙어서 움직이도록 하기 위함.
	if ( wwMoveParentBoneIDForSafeModeWeaponSlot.dwData != 0xffffffff )
	{
		DxBoneTrans* pBoneTrans_Move = m_skeletonResThread.get()->FindBone_Index_Bone( wwMoveParentBoneIDForSafeModeWeaponSlot.wA );
		const DxBoneTrans* pBoneTrans_Parent = m_skeletonResThread.get()->FindBone_Index_Bone_const( wwMoveParentBoneIDForSafeModeWeaponSlot.wB );
		if ( pBoneTrans_Move && pBoneTrans_Parent )
		{
			m_skeletonResThread.get()->UpdateBones_PureThread( pBoneTrans_Move, pBoneTrans_Parent->matCombined, m_fWeight, 1.f );
		}
	}

	// Main 에서만 가중치를 계산하도록 한다.
	// 0 KeyFrame <-> 1 JiggleBone
	GetKeyJiggleWeight_PureThread( arrayfKeyJiggleWeight, dwPartSize+1 );

	// m_vecBoneTransJointData[i].m_rBoneTransJoint->matCombined 값이 변경된다.
	m_scpJiggleBone->FrameMove_PureThread( m_skeletonResThread.get(), arrayfKeyJiggleWeight[dwPartSize] );

	// 실제 사용하는 정보를 Import 한다.
	m_pSkeletonBackUp->Import_PureThread( m_skeletonResThread.get() );

	// 공틍으로 쓰이는 Data 임으로 m_pClothColl 의 값은 변경시키지 않는다.
	// 천 시뮬레이션 충돌정보 Update.
	m_spSkinAniControlData->GetClothColl()->FrameMove( *m_pvecClothCollSpheresBackUp, m_pSkeletonBackUp, matLocalRotOUT, m_fScale );

	// 백업한다.
	*m_pmatWorldBackUp = matLocalRotOUT;

	return TRUE;
}

// TAG_CalculateAnimationThread_2_1
// Lock 이 필요하다.
HRESULT DxSkinAniControl::CalculateBone_PureThread_HARD( const D3DXMATRIX &matRot )
{
	if( !m_skeletonResThread.IsValid() )	
		return E_FAIL;

	if( !RENDERPARAM::bCALCULATE_BONE )	
		return S_OK;

	DxSkeletonPart* pSkeletonPart = GetSkeletonPart_PureThread();
	if( !pSkeletonPart )
		return E_FAIL;

	////	Note : 에니메이션에 모든 본의 에니메이션 키가 없을 때를 대비하여
	////		키를 모두 리샛함.
	////
	//DxSkinAniControl::ResetBone ();

	// Note : 상체애니메이션을 사용한다.
	//		< 목표 > Bone의 BIp01 Matrix값을 얻오는 것.
	if( m_sAniControlCORE.m_sUpBodyAC.m_rAnimNode )
	{
		ResetBone_PureThread();

		// Note : 
		m_sAniControlCORE.UpdateBoneBip01_PureThread( 1.f );

		// Note : Bip01 쪽 회전값을 얻어온다.
		m_skeletonResThread.get()->GetMatrixBone_Bip01_Spine_UpBody( pSkeletonPart, pSkeletonPart->pBoneRoot, matRot );
	}
	else
	{
		//	Note : 에니메이션에 모든 본의 에니메이션 키가 없을 때를 대비하여
		//		키를 모두 리샛함.
		//
		ResetBone_PureThread();
	}

	m_sAniControlCORE.UpdateTime_PureThread( m_fWeight );
	m_sAniControlCORE.UpdateMix_PureThread();

	if ( m_bBIGHEAD )
	{
		DxBoneTrans *pBoneCur = m_skeletonResThread.get()->FindBone_t( _T("Bip01_Head") );
		if ( pBoneCur )
		{
#ifdef USE_ANI_QUATERNION
			pBoneCur->m_pQuatPosCUR.m_vScale.x *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosCUR.m_vScale.y *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosCUR.m_vScale.z *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosPREV.m_vScale.x *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosPREV.m_vScale.y *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosPREV.m_vScale.z *= m_fBIGHEAD;
#else
			float fScale = 1.f/m_fBIGHEAD;
			pBoneCur->matRot._41 *= fScale;
			pBoneCur->matRot._42 *= fScale;
			pBoneCur->matRot._43 *= fScale;
			pBoneCur->matRot._44 *= fScale;
#endif
		}
	}

	if ( m_bBIGHAND )
	{
		DxBoneTrans *pBoneCur = NULL;

		pBoneCur = m_skeletonResThread.get()->FindBone_t( _T("Bip01_R_Hand") );
		if ( pBoneCur )
		{
#ifdef USE_ANI_QUATERNION
			float fScale = m_fBIGHAND/2.f;
			pBoneCur->m_pQuatPosCUR.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.z *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.z *= fScale;
#else
			float fScale = 2.f/m_fBIGHAND;
			pBoneCur->matRot._41 *= fScale;
			pBoneCur->matRot._42 *= fScale;
			pBoneCur->matRot._43 *= fScale;
			pBoneCur->matRot._44 *= fScale;
#endif
		}

		pBoneCur = m_skeletonResThread.get()->FindBone_t( _T("Bip01_L_Hand") );
		if ( pBoneCur )
		{
#ifdef USE_ANI_QUATERNION
			float fScale = m_fBIGHAND/2.f;
			pBoneCur->m_pQuatPosCUR.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.z *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.z *= fScale;
#else
			float fScale = 2.f/m_fBIGHAND;
			pBoneCur->matRot._41 *= fScale;
			pBoneCur->matRot._42 *= fScale;
			pBoneCur->matRot._43 *= fScale;
			pBoneCur->matRot._44 *= fScale;
#endif
		}
	}

	// [AnimationMove]
	AnimationMove_Update();

	// 본 스케일을 업데이트 한다.
	UpdateBoneScaleList_PureThread();

	//	Note : 계층 메트릭스 업데이트.
	//DxSkeleton::g_fWeight = m_fWeight;
	m_skeletonResThread.get()->UpdateBones_PureThread( pSkeletonPart->pBoneRoot, matRot, m_fWeight, 1.f );

	// 여기는 다른 Thread 임.

	return S_OK;
}

// TAG_CalculateAnimationThread_2_2
// 0 KeyFrame <-> 1 JiggleBone
void DxSkinAniControl::GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM ) const
{
	// 초기화를 해준다.
	for ( DWORD i=0; i<dwKeyJiggleWeightNUM; ++i )
	{
		afKeyJiggleWeight[i] = 1.f;
	}

	m_sAniControlCORE.GetKeyJiggleWeight_PureThread( afKeyJiggleWeight, dwKeyJiggleWeightNUM, GETCURKEYTIME() );
}

void DxSkinAniControl::UpdateBoneScaleList_PureThread()
{
	if( !m_skeletonResThread.IsValid() )	
		return;

	DEQ_BONESCALE_ITER iter = m_deqBoneEditScale.begin();
	for( ; iter != m_deqBoneEditScale.end(); ++iter )
	{
		DxBoneTrans *pBoneCur = m_skeletonResThread.get()->FindBone_t( _T(iter->strBoneName.c_str()) );
		if( pBoneCur )
		{
			// [ Thread Work shhan ]
			pBoneCur->vBoneScale = iter->vScale;
			//pBoneCur->m_vEditScale = iter->vScale;
		}
	}
}








//////////////////////////////////////////////////////////////////////////
//						AnimationMove
//////////////////////////////////////////////////////////////////////////

// [AnimationMove] 애니메이션 움직임이 캐릭터에 영향을 받는 것인지 확인
BOOL DxSkinAniControl::IsAnimationMove()
{
	return m_sAniControlCORE.IsAnimationMove();
}

// [AnimationMove] 캐릭터가 애니메이션으로 얼마만큼 움직였는지 확인
const D3DXVECTOR3& DxSkinAniControl::GetAnimationMovePos()
{
	return m_vAniMoveOffset; 
}

//// AnimationMove 의 시작 위치.
//const D3DXMATRIX& DxSkinAniControl::GetAnimationMoveStartMatrix()
//{
//	return m_matAniMoveStartWorld;
//}

void DxSkinAniControl::AnimationMove_Update()
{
	if( g_bCHAR_EDIT_RUN )
		return;

	if ( m_sAniControlCORE.IsAnimationMove() )
	{
		// Bip01 이동을 강제로 막아준다.
		DxBoneTrans* pBoneTrans = m_skeletonResThread.get()->FindBone_t( _T("Bip01") );
		if( pBoneTrans )
		{
			if( m_sAniControlCORE.IsFirstAnimation() )
			{
				// ACF_MOVE 라면 ACF_MOVE 종료 후 특정 작업을 하기 위해 셋팅됨.
				m_nAnimationMove_PostWork = 2;

				// [AnimationMove] 처음 애니메이션 동작을 작업하였으면 FALSE로 셋팅한다.
				m_sAniControlCORE.SetFirstAnimationFALSE();

				// 처음 애니메이션의 Bip01 위치점
				const D3DXVECTOR3* pPos = m_sAniControlCORE.GetFirstAnimationPos();
				if( pPos )
				{
					m_vAniMoveOffsetStart = *pPos;
				}
				else
				{
					m_vAniMoveOffsetStart = D3DXVECTOR3(0.f,0.f,0.f);
				}

				// 위치를 얻어온다.
				m_vAniMovePos = pBoneTrans->m_pQuatPosCUR.m_vPos;
				m_vAniMoveOffset = pBoneTrans->m_pQuatPosCUR.m_vPos - m_vAniMoveOffsetStart;
				m_vAniMoveOffset.y = 0.f;
			}
			else
			{
				// 이동된 위치를 얻는다.
				m_vAniMoveOffset = pBoneTrans->m_pQuatPosCUR.m_vPos - m_vAniMovePos;
				m_vAniMoveOffset.y = 0.f;

				// 위치를 백업한다.
				m_vAniMovePos = pBoneTrans->m_pQuatPosCUR.m_vPos;
			}

			pBoneTrans->m_pQuatPosCUR.m_vPos.x = m_vAniMoveOffsetStart.x;
			pBoneTrans->m_pQuatPosCUR.m_vPos.z = m_vAniMoveOffsetStart.z;
			pBoneTrans->m_pQuatPosPREV.m_vPos.x = m_vAniMoveOffsetStart.x;
			pBoneTrans->m_pQuatPosPREV.m_vPos.z = m_vAniMoveOffsetStart.z;
		}
	}
	else
	{
		if( m_sAniControlCORE.IsFirstAnimation() )
		{
			--m_nAnimationMove_PostWork;

			// [AnimationMove] 처음 애니메이션 동작을 작업하였으면 FALSE로 셋팅한다.
			m_sAniControlCORE.SetFirstAnimationFALSE();
		}

		if ( m_nAnimationMove_PostWork == 1 )
		{
			DxBoneTrans* pBoneTrans = m_skeletonResThread.get()->FindBone_t( _T("Bip01") );
			if( pBoneTrans )
			{
				pBoneTrans->m_pQuatPosPREV.m_vPos.x = m_vAniMoveOffsetStart.x;
				pBoneTrans->m_pQuatPosPREV.m_vPos.z = m_vAniMoveOffsetStart.z;
			}
		}
	}
}

D3DXVECTOR3 DxSkinAniControl::GetAnimationMoveOffsetFull_EDIT()
{
	D3DXVECTOR3 vPos(0.f,0.f,0.f);

	if ( m_sAniControlCORE.IsAnimationMove() )
	{
		const DxBoneTransMaintain* pBoneTranMaintain = m_pSkeleton->FindBone( _T("Bip01") );
		if( pBoneTranMaintain )
		{
			vPos.x = pBoneTranMaintain->m_matCombined._41;
			vPos.y = pBoneTranMaintain->m_matCombined._42;
			vPos.z = pBoneTranMaintain->m_matCombined._43;

			// 처음 애니메이션의 Bip01 위치점
			D3DXVECTOR3 vAniMoveOffsetStart( 0.f, 0.f, 0.f	 );
			const D3DXVECTOR3* pPos = m_sAniControlCORE.GetFirstAnimationPos();
			if( pPos )
			{
				vAniMoveOffsetStart = *pPos;
			}
			else
			{
				vAniMoveOffsetStart = D3DXVECTOR3(0.f,0.f,0.f);
			}

			// 위치를 얻어온다.
			vPos -= vAniMoveOffsetStart;
			vPos.y = 0.f;
		}
	}

	return vPos;
}

//////////////////////////////////////////////////////////////////////////