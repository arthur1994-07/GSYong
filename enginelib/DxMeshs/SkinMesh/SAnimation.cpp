#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxCommon9/DXUT_UTIL.h"
#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../DxTools/PrivateResource.h"
#include "../../DxTools/DxMethods.h"
#include "../../Common/stlfunctions.h"
#include "DxSkeletonManager.h"
#include "DxAniBoneScale.h"
#include "DxSkinDefine.h"
//#include "DxSkinAnimationManager.h"

#include "SAnimation.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//const TCHAR DxSkinAnimation::szFILETYPE[] = _T("AnimContainer");

#define DIV_1_65535	0.000015259021896f
#define DIV_1_32768 0.000030517578125f

void CompressionQuaternion( QUATCOMP& sOut, D3DXQUATERNION& vQuat )
{
	if( vQuat.w < 0.f )
	{
		vQuat.x = -vQuat.x;
		vQuat.y = -vQuat.y;
		vQuat.z = -vQuat.z;
		vQuat.w = -vQuat.w;
	}

	vQuat.x += 1.f;		// 0~2
	vQuat.y += 1.f;
	vQuat.z += 1.f;

	vQuat.x *= 0.5f;	// 0~1
	vQuat.y *= 0.5f;
	vQuat.z *= 0.5f;

	sOut.dwFirst = (DWORD)(vQuat.x * 0xffff) << 16;
	sOut.dwFirst += (DWORD)(vQuat.y * 0xffff);

	sOut.dwSecond = (DWORD)(vQuat.z * 0xffff) << 16;
	sOut.dwSecond += (DWORD)(vQuat.w * 0xffff);
}

void DecompressionQuaternion( D3DXQUATERNION& vOut, const QUATCOMP& sSrc )
{
	int nTemp = sSrc.dwFirst >> 16;
	nTemp -= 32767;
	vOut.x = (float)nTemp * DIV_1_32768;

	nTemp = sSrc.dwFirst & 0xffff;
	nTemp -= 32767;
	vOut.y = (float)nTemp * DIV_1_32768;

	nTemp = sSrc.dwSecond >> 16;
	nTemp -= 32767;
	vOut.z = (float)nTemp * DIV_1_32768;

	nTemp = sSrc.dwSecond & 0xffff;
	vOut.w = (float)nTemp * DIV_1_65535;
}


//SAttachBoneAnimInfo& SAttachBoneAnimInfo::operator = ( const SAttachBoneAnimInfo &value )
//{
//	bUseStrikeAni = value.bUseStrikeAni;
//	sBasicAni     = value.sBasicAni;
//
//
//	vecStrikeAni.clear();
//	vecStrikeAni.reserve( value.vecStrikeAni.size() );
//	for( DWORD i = 0; i < value.vecStrikeAni.size(); i++ )
//	{
//		SAttachBoneAnimBasicInfo sTempBasicAni = value.vecStrikeAni[i];
//		vecStrikeAni.push_back(sTempBasicAni);
//	}
//	return *this;
//}
//
//SANIMCONINFO::SANIMCONINFO() :
//	m_dwFlag(ACF_LOOP|ACF_NEWINFO),
//	m_dwSTime(0),
//	m_dwETime(0),
//	m_dwETimeOrig(0),
//	m_UNITTIME(0),
//	m_MainType(AN_GUARD_N),
//	m_SubType(AN_SUB_NONE),
//
//	m_wDivCount(0),
//	m_wStrikeCount(0),
//	m_pAniScale(NULL)
//{
//	//memset( m_Name, 0, sizeof(TCHAR)*ACF_SZNAME );
//	//memset( m_SkeletionName, 0, sizeof(TCHAR)*ACF_SZNAME );
//	memset ( m_wDivFrame, 0, sizeof(WORD)*ACF_DIV );
//	m_pAniScale = new DxAniScale;
//}
//
//SANIMCONINFO::~SANIMCONINFO ()
//{
//	CleanUp();
//
//	SAFE_DELETE( m_pAniScale );
//}
//
//void SANIMCONINFO::CleanUp()
//{
//	std::for_each ( m_listEffAni.begin(), m_listEffAni.end(), std_afunc::DeleteObject() );
//	m_listEffAni.clear();
//
//	m_pAniScale->CleanUp();
//
//	for( DWORD i = 0; i < PIECE_SIZE; i++ )
//	{
//		m_sAttBoneAnimInfo[i].Init();
//	}
//}
//
//SANIMCONINFO& SANIMCONINFO::operator = ( SANIMCONINFO &value )
//{
//	CleanUp();
//
//	m_Name = value.m_Name;
//	m_SkeletionName = value.m_SkeletionName;
//
//	m_dwFlag = value.m_dwFlag;					//	기타 속성.
//	m_dwSTime = value.m_dwSTime;					//	시작 시간.
//	m_dwETime = value.m_dwETime;
//	m_dwETimeOrig = value.m_dwETimeOrig;	//	끝 시간.
//	m_UNITTIME = value.m_UNITTIME;					//	키단위 시간.
//
//	m_MainType = (EMANI_MAINTYPE)value.m_MainType;
//	m_SubType = (EMANI_SUBTYPE)value.m_SubType;
//
//	m_wDivCount = value.m_wDivCount;				//	에니메이션 분할 키 갯수.
//	for ( DWORD i=0;i<m_wDivCount; ++i )
//	{
//		m_wDivFrame[i] = value.m_wDivFrame[i];
//	}
//
//	m_wStrikeCount = value.m_wStrikeCount;
//	for ( DWORD i=0;i<m_wStrikeCount; ++i )
//	{
//		m_sStrikeEff[i] = value.m_sStrikeEff[i];
//	}
//
//	m_ChaSoundData = value.m_ChaSoundData;
//
//	EFFANILIST_ITER iter = value.m_listEffAni.begin();
//	EFFANILIST_ITER iter_end = value.m_listEffAni.end();
//	for ( ; iter!=iter_end; ++iter )
//	{
//		DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance ( (*iter)->GetTypeID() );
//		if ( pEff )
//		{
//			pEff->SetProperty ( (*iter)->GetProperty() );
//			pEff->SetEffAniData ( (*iter) );
//			m_listEffAni.push_back ( pEff );
//		}
//	}
//
//	*m_pAniScale = *value.m_pAniScale;
//
//	for( DWORD i = 0; i < PIECE_SIZE; i++ )
//	{
//		m_sAttBoneAnimInfo[i] = value.m_sAttBoneAnimInfo[i];
//	}
//
//	return *this;
//}

SMatrixKey* Lower_bound ( SMatrixKey* pFirst, DWORD dwCount, DWORD dwTime )
{
	for (; 0 < dwCount; )
	{
		// divide and conquer, find half that contains answer
		DWORD dwCount2 = dwCount / 2;
		SMatrixKey *pMiddle = pFirst;
		pMiddle += dwCount2;

		// [shhan][2014.06.19]
		// if ( pMiddle->dwTime < dwTime ) 이전 if ( pMiddle->dwTime <= dwTime ) 이후
		// UINT가 160 일 때 160 값을 넘은 것으로 할 것인가~? -> if ( pMiddle->dwTime <= dwTime )
		// UINT가 160 일 때 161 값을 넘은 것으로 할 것인가~? -> if ( pMiddle->dwTime < dwTime )
		// if ( pMiddle->dwTime <= dwTime ) 게 맞는거 같다.
		if ( pMiddle->dwTime <= dwTime )
			pFirst = ++pMiddle, dwCount -= dwCount2 + 1;
		else
			dwCount = dwCount2;
	}

	//DWORD dwCount2(0);
	//SMatrixKey *pMiddle(NULL);
	//for (; 0 < dwCount; )
	//{
	//	// divide and conquer, find half that contains answer
	//	dwCount2 = dwCount/2;
	//	pMiddle = pFirst;
	//	pMiddle += dwCount2;

	//	if ( pMiddle->fTime < fTime )
	//		pFirst = ++pMiddle, dwCount -= dwCount2 + 1;
	//	else
	//		dwCount = dwCount2;
	//}

	return (pFirst);
}

SQuatPosKey* Lower_bound3312( SQuatPosKey* pFirst, DWORD dwCount, DWORD dwTime )
{
	for (; 0 < dwCount; )
	{
		// divide and conquer, find half that contains answer
		DWORD dwCount2 = dwCount / 2;
		SQuatPosKey *pMiddle = pFirst;
		pMiddle += dwCount2;

		// [shhan][2014.06.19]
		// if ( pMiddle->dwTime < dwTime ) 이전 if ( pMiddle->dwTime <= dwTime ) 이후
		// UINT가 160 일 때 160 값을 넘은 것으로 할 것인가~? -> if ( pMiddle->dwTime <= dwTime )
		// UINT가 160 일 때 161 값을 넘은 것으로 할 것인가~? -> if ( pMiddle->dwTime < dwTime )
		// if ( pMiddle->dwTime <= dwTime ) 게 맞는거 같다.
		if ( pMiddle->m_dwTime <= dwTime )
			pFirst = ++pMiddle, dwCount -= dwCount2 + 1;
		else
			dwCount = dwCount2;
	}

	return (pFirst);
}

const SMatrixKey* SAnimation::GetMapThisKey ( float fGlobalTime ) const
{
	//float fLerpValue=0.0f;
	//float fTime1=0.0f;
	//float fTime2=0.0f;
	//const SMatrixKey* pMatrixKey2=NULL;
	//const SMatrixKey* pMatrixKey3=NULL;

	//if ( m_pMatrixKeys )
	//{

	//	//	Note : f = fmod ( x, y )  : x = i*y + f ( 나머지 구하는 함수 )
	//	//		에니메이션 루프.
	//	//
	//	float fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_vecMatrixKeys[m_cMatrixKeys-1].dwTime) );

	//	MATRIXKEY_CITER iter3;
	//	if ( fTime==0 )
	//	{
	//		iter3 = m_vecMatrixKeys.begin();
	//	}
	//	else
	//	{
	//		SMatrixKey sKEY;
	//		sKEY.dwTime = DWORD(fTime);

	//		iter3 = std::lower_bound ( m_vecMatrixKeys.begin(), m_vecMatrixKeys.end(), sKEY );
	//		if ( iter3==m_vecMatrixKeys.end() )		iter3 = m_vecMatrixKeys.begin();
	//	}

	//	pMatrixKey3 = &(*iter3);
	//	if ( iter3==m_vecMatrixKeys.begin() )	return pMatrixKey3;

	//	MATRIXKEY_CITER iter2 = iter3--;
	//	pMatrixKey2 = &(*iter2);

	//	fTime1 = (float)pMatrixKey2->dwTime;
	//	fTime2 = (float)pMatrixKey3->dwTime;

	//	//	Note : 두 키 중에 가장 가까운 키를 선택.
	//	//
	//	if ( (fTime2 - fTime1) ==0 )	fLerpValue = 0;
	//	else							fLerpValue =  (fTime-fTime1)  / (fTime2-fTime1);

	//	if ( fLerpValue > 0.5 )			return pMatrixKey3;
	//	else							return pMatrixKey2;
	//}

	return NULL;
}

//	Note : MatrixKey 중에서 현제 시간에 맞는 에니메이션 키를 선택해 준다.
//
UINT SAnimation::GetThisKey ( float fGlobalTime ) const
{
	//// Note : 속도 때문에 이것으로 대처 했다.. 정확하지 않은 데이터가 나올 수도 있다.
	//UINT nKey(0);
	//if( m_pMatrixKeys )
	//{
	//	// 1번당.. 걸리는 시간을 구해야 한다.
	//	DWORD dwTime = m_pMatrixKeys[m_cMatrixKeys-1].dwTime+m_pMatrixKeys[1].dwTime;
	//	float fTime = (float)fmod ( fGlobalTime, static_cast<float>(dwTime) );
	//	float fDiv = (float)dwTime/(m_cMatrixKeys-1);
	//	return fTime/fDiv;

	//	//m_pMatrixKeys[1].fTime

	//	////nKey = (UINT)(fGlobalTime * m_fDivFullTime * m_fMatrixKeys);
	//	//float fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pMatrixKeys[m_cMatrixKeys-1].dwTime) );
	//	//nKey = (UINT)(fTime * m_fDivFullTime * m_fMatrixKeys);
	//}

	//return nKey;

	if ( m_pMatrixKeys )
	{
		//	Note : f = fmod ( x, y )  : x = i*y + f ( 나머지 구하는 함수 )
		//		에니메이션 루프.
		//
		float fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pMatrixKeys[m_cMatrixKeys-1].dwTime) );

		SMatrixKey* pFOUND =  Lower_bound ( m_pMatrixKeys, m_cMatrixKeys, (DWORD)fTime );
		if ( !pFOUND )	pFOUND = m_pMatrixKeys + (m_cMatrixKeys-1);

		return (pFOUND-m_pMatrixKeys);
	}

	return 0;

	//UINT iKey = 0;
	//UINT dwp2;
	//UINT dwp3;
	//float fTime1;
	//float fTime2;
	//float fLerpValue;
	//float fTime;

	//if ( m_pMatrixKeys )
	//{
	//	//	Note : f = fmod ( x, y )  : x = i*y + f ( 나머지 구하는 함수 )
	//	//		에니메이션 루프.
	//	//
	//	fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pMatrixKeys[m_cMatrixKeys-1].dwTime) );

	//	SMatrixKey* pFOUND =  Lower_bound ( m_pMatrixKeys, m_cMatrixKeys, (DWORD)fTime );
	//	if ( !pFOUND )	pFOUND = m_pMatrixKeys + (m_cMatrixKeys-1);

	//	dwp3 = (pFOUND-m_pMatrixKeys);
	//	if ( iKey > 0 )
	//	{
	//		dwp2= iKey - 1;
	//	}
	//	else  // when iKey == 0, then dwp2 == 0
	//	{
	//		dwp2 = iKey;
	//	}

	//	//	Note : 현제 fTime에 들어가는 시간대의 키를 선택.
	//	//
	//	//for ( iKey = 0 ; iKey < m_cMatrixKeys ; iKey++ )
	//	//{
	//	//	if ( (float)m_pMatrixKeys[iKey].dwTime > fTime )
	//	//	{
	//	//		dwp3 = iKey;

	//	//		if ( iKey > 0 )
	//	//		{
	//	//			dwp2= iKey - 1;
	//	//		}
	//	//		else  // when iKey == 0, then dwp2 == 0
	//	//		{
	//	//			dwp2 = iKey;
	//	//		}

	//	//		break;
	//	//	}
	//	//}

	//	fTime1 = (float)m_pMatrixKeys[dwp2].dwTime;
	//	fTime2 = (float)m_pMatrixKeys[dwp3].dwTime;

 //       
	//	//	Note : 두 키 중에 가장 가까운 키를 선택.
	//	//
	//	if ( (fTime2 - fTime1) ==0 )	fLerpValue = 0;
	//	else							fLerpValue =  (fTime-fTime1)  / (fTime2-fTime1);

	//	if ( fLerpValue > 0.5 )			iKey = dwp3;
	//	else							iKey = dwp2;
	//}

	//return iKey;
}

void SAnimation::SetMatrix ( UINT iKey )
{
	//	Note : 4byte * 16 개.. 맞나 ? ㅋ
	memcpy( m_pBoneToAnimate->matRot, m_pMatrixKeys[iKey].mat, 64 ); 
}


namespace
{
	BOOL ISDIFFERENT ( D3DXMATRIX *pmatA, D3DXMATRIX *pmatB )
	{
		for ( int i=0; i<4; ++i )
		for ( int j=0; j<4; ++j )
		{
			if ( fabs ( pmatA->m[i][j] - pmatB->m[i][j] ) > 0.01f )		return TRUE;
		}

		return FALSE;
	}
}

void SAnimation::SetMix_PureThread( float fGlobalTime )
{
	if ( m_pBoneToAnimate == NULL )	return;

	if ( m_pMatrixKeys )
	{
		//	Note : 에니메이션 키 설정.
		//
		UINT iKey = GetThisKey(fGlobalTime);
		const SMatrixKey* pMatrixKey = &m_pMatrixKeys[iKey];

		//const SMatrixKey* pMatrixKey = GetMapThisKey ( fGlobalTime );

		//	Note : 에니메이션 키 설정.
		if ( pMatrixKey )
		{
			//if ( ISDIFFERENT(&pMatrixKey->mat,&m_pBoneToAnimate->matRotOrig) )
			{
				D3DXMATRIX matInv;
				D3DXMatrixInverse ( &matInv, NULL, &m_pMatrixKeys[0].mat );
				D3DXMatrixMultiply ( &matInv, &pMatrixKey->mat, &matInv );

				D3DXMatrixMultiply ( &m_pBoneToAnimate->matRot, &matInv, &m_pBoneToAnimate->matRot );
			}
		}
	}
	else if( m_pQuatPosKeys )
	{
		//	Note : f = fmod ( x, y )  : x = i*y + f ( 나머지 구하는 함수 )
		//		에니메이션 루프.
		float fTime = (float)fmod ( fGlobalTime, static_cast<float>(m_pQuatPosKeys[m_cQuatPosKeys-1].m_dwTime) );

		SQuatPosKey* pFOUND =  Lower_bound3312( m_pQuatPosKeys, m_cQuatPosKeys, (DWORD)fGlobalTime );
		if ( !pFOUND )	pFOUND = m_pQuatPosKeys + (m_cQuatPosKeys-1);
		DWORD dwKey = pFOUND-m_pQuatPosKeys;
		if( dwKey >= m_cQuatPosKeys )	
		{
			dwKey = m_cQuatPosKeys-1;
		}

		// Note : 없는 애들만 셋팅한다.. MIX 만 하는 것임.
		if( !m_pQuatOrigRot )
		{
			m_pQuatOrigRot = new D3DXQUATERNION[m_cQuatPosKeys];

			D3DXQUATERNION vQuat;
			D3DXQUATERNION vQuatInv;
			for( DWORD i=0; i<m_cQuatPosKeys; ++i )
			{
				DecompressionQuaternion( vQuat, m_pQuatPosKeys[0].m_vQuatComp );
				D3DXQuaternionInverse( &vQuatInv, &vQuat );

				DecompressionQuaternion( vQuat, m_pQuatPosKeys[i].m_vQuatComp );
				D3DXQuaternionMultiply( &m_pQuatOrigRot[i], &vQuat, &vQuatInv );	// 실제 회전이다.
			}
		}

		m_pBoneToAnimate->m_dwFlag |= USE_ANI_ADD;
		m_pBoneToAnimate->m_vQuatMIX = m_pQuatOrigRot[dwKey];
	}
}

// 모션이 변경될 때만 블랜드(보간) 이 일어나던 것을
// 평상시 모션에도 보간이 일어나도록 변경, 향후 안정화 되면 삭제 예정
#define DEF_FIX_ANI_INTERPOLATION

void SetPosQuat( SQuatPosKey* pQuatPosKeys, DWORD cMatrixKeys, DxBoneTrans* m_pBoneToAnimate, float fGlobalTime, float fWeight, BOOL bFirst )
{
    if( !pQuatPosKeys )		return;

    //	Note : f = fmod ( x, y )  : x = i*y + f ( 나머지 구하는 함수 )
    //		에니메이션 루프.
    float fTime = (float)fmod ( fGlobalTime, static_cast<float>(pQuatPosKeys[cMatrixKeys-1].m_dwTime) );

#ifndef DEF_FIX_ANI_INTERPOLATION
    SQuatPosKey* pFOUND =  Lower_bound3312( pQuatPosKeys, cMatrixKeys, (DWORD)fTime );
    if ( !pFOUND )	pFOUND = pQuatPosKeys + (cMatrixKeys-1);
    DWORD dwKey = pFOUND-pQuatPosKeys;
    if( dwKey >= cMatrixKeys )
    {
        dwKey = cMatrixKeys-1;
    }
#else
    DWORD dwKey = 0;
    DWORD dwNext = 0;

    while (dwKey + 1 < cMatrixKeys)
    {
        dwNext = dwKey + 1;

        if (fTime < pQuatPosKeys[dwNext].m_dwTime)
        {
            break;
        }

        ++dwKey;
    }

    MIN_ASSERT(dwKey >= 0 && _T("키 설정 오류!!!"));    
    MIN_ASSERT(dwNext < cMatrixKeys && _T("키 설정 오류!!!"));
#endif

    // Note : Quaternion의.. 보간.

    // Note : Bone에 셋팅.~!
    if( bFirst )
    {
        m_pBoneToAnimate->m_pQuatPosCUR.m_vPos = pQuatPosKeys[dwKey].m_vPos;
        m_pBoneToAnimate->m_pQuatPosCUR.m_vScale = pQuatPosKeys[dwKey].m_vScale;

        //m_pBoneToAnimate->m_pQuatPosPREV.m_vQuat = pQuatPosKeys[dwKey].m_vQuat;
        DecompressionQuaternion( m_pBoneToAnimate->m_pQuatPosCUR.m_vQuat, pQuatPosKeys[dwKey].m_vQuatComp );

#ifdef DEF_FIX_ANI_INTERPOLATION
        static SQuatPos m_kQuatPosNext;

        if (dwNext != dwKey)
        {
            float fCur = float(pQuatPosKeys[dwKey].m_dwTime);
            float fNext = float(pQuatPosKeys[dwNext].m_dwTime);

            float t = (fTime - fCur) / (fNext - fCur);

            m_kQuatPosNext.m_vPos = pQuatPosKeys[dwNext].m_vPos;
            m_kQuatPosNext.m_vScale = pQuatPosKeys[dwNext].m_vScale;

            DecompressionQuaternion( m_kQuatPosNext.m_vQuat, pQuatPosKeys[dwNext].m_vQuatComp );

            D3DXVec3Lerp( &m_pBoneToAnimate->m_pQuatPosCUR.m_vPos, &m_pBoneToAnimate->m_pQuatPosCUR.m_vPos, &m_kQuatPosNext.m_vPos, t );
            D3DXVec3Lerp( &m_pBoneToAnimate->m_pQuatPosCUR.m_vScale, &m_pBoneToAnimate->m_pQuatPosCUR.m_vScale, &m_kQuatPosNext.m_vScale, t );
            D3DXQuaternionSlerp( &m_pBoneToAnimate->m_pQuatPosCUR.m_vQuat, &m_pBoneToAnimate->m_pQuatPosCUR.m_vQuat, &m_kQuatPosNext.m_vQuat, t );
        }
#endif
    }
    else
    {
        m_pBoneToAnimate->m_dwFlag |= USE_ANI_BLENDING;
        m_pBoneToAnimate->m_pQuatPosPREV.m_vPos = pQuatPosKeys[dwKey].m_vPos;
        m_pBoneToAnimate->m_pQuatPosPREV.m_vScale = pQuatPosKeys[dwKey].m_vScale;

        //m_pBoneToAnimate->m_pQuatPosPREV.m_vQuat = pQuatPosKeys[dwKey].m_vQuat;
        DecompressionQuaternion( m_pBoneToAnimate->m_pQuatPosPREV.m_vQuat, pQuatPosKeys[dwKey].m_vQuatComp );
    }
}

void SAnimation::SetTime_PureThread( float fGlobalTime, float fWeight, BOOL bFirst )
{
	int iKey;
	UINT dwp2;
	UINT dwp3;
	D3DXMATRIXA16 matResult;
	D3DXMATRIXA16 matTemp;
	float fTime1;
	float fTime2;
	float fLerpValue;
	D3DXVECTOR3 vScale;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION quat;
	BOOL bAnimate = false;
	float fTime;

	if( !m_pBoneToAnimate )	return;

	if ( m_pMatrixKeys )
	{
		//	Note : 에니메이션 키 설정.
		iKey = GetThisKey ( fGlobalTime );
		SetMatrix ( iKey );

		//	Note : 에니메이션 키 설정.
		//const SMatrixKey* pMatrixKey = GetMapThisKey ( fGlobalTime );
		//if ( pMatrixKey )	SetMatrix ( pMatrixKey );
	}
	else if( m_pQuatPosKeys )
	{
		SetPosQuat( m_pQuatPosKeys, m_cQuatPosKeys, m_pBoneToAnimate, fGlobalTime, fWeight, bFirst );
	}
	else
	{
		D3DXMatrixIdentity ( &matResult );

		// 최대 시간을 구해야 한다.
		// 변화가 없으면 셋팅이 안되므로 작업이 필요함.
		DWORD dwFullTime(0);
		if ( m_pScaleKeys )
		{
			dwFullTime = m_pScaleKeys[m_cScaleKeys-1].dwTime;
		}
		if ( m_pRotateKeys )
		{
			if ( dwFullTime < m_pRotateKeys[m_cRotateKeys-1].dwTime )
				dwFullTime = m_pRotateKeys[m_cRotateKeys-1].dwTime;
		}
		if ( m_pPositionKeys )
		{
			if ( dwFullTime < m_pPositionKeys[m_cPositionKeys-1].dwTime )
				dwFullTime = m_pPositionKeys[m_cPositionKeys-1].dwTime;
		}

		fTime = (float)fmod(fGlobalTime, static_cast<float>(dwFullTime) );

		if ( m_pScaleKeys )
		{
			dwp2 = dwp3 = 0;

			//for ( iKey = 0 ;iKey < m_cScaleKeys ; iKey++ )
			//{
			//	if ( (float)m_pScaleKeys[iKey].dwTime > fTime )
			//	{
			//		dwp3 = iKey;

			//		if (iKey > 0)
			//		{
			//			dwp2= iKey - 1;
			//		}
			//		else  // when iKey == 0, then dwp2 == 0
			//		{
			//			dwp2 = iKey;
			//		}

			//		break;
			//	}
			//}

			for ( iKey = m_cScaleKeys-1 ;iKey >= 0 ; iKey-- )
			{
				if ( (float)m_pScaleKeys[iKey].dwTime <= fTime )
				{
					dwp2 = iKey;

					if (iKey == m_cScaleKeys-1)
					{
						dwp3 = iKey;
					}
					else
					{
						dwp3 = iKey + 1;
					}

					break;
				}
			}
            
			fTime1 = (float)m_pScaleKeys[dwp2].dwTime;
			fTime2 = (float)m_pScaleKeys[dwp3].dwTime;

			if ( (fTime2 - fTime1) ==0 )
				fLerpValue = 0;
			else
				fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

			//	Note : 선형 보간.
			//
			D3DXVec3Lerp ( &vScale,
					&m_pScaleKeys[dwp2].vScale,
					&m_pScaleKeys[dwp3].vScale,
					fLerpValue );

			D3DXMatrixScaling ( &matTemp, vScale.x, vScale.y, vScale.z );
			D3DXMatrixMultiply ( &matResult, &matResult, &matTemp );

			bAnimate = true;
		}

		// check rot keys
		if ( m_pRotateKeys )
		{
			int i1 = 0;
			int i2 = 0;

			//for ( iKey = 0 ;iKey < m_cRotateKeys ; iKey++ )
			//{
			//	if ( (float)m_pRotateKeys[iKey].dwTime > fTime )
			//	{
			//		i1 = (iKey > 0) ? iKey - 1 : 0;
			//		i2 = iKey;
			//		break;
			//	}
			//}

			for ( iKey = m_cRotateKeys-1 ;iKey >= 0 ; iKey-- )
			{
				if ( (float)m_pRotateKeys[iKey].dwTime <= fTime )
				{
					i1 = iKey;

					if (iKey == m_cRotateKeys-1)
					{
						i2 = iKey;
					}
					else
					{
						i2 = iKey + 1;
					}

					break;
				}
			}

			fTime1 = (float)m_pRotateKeys[i1].dwTime;
			fTime2 = (float)m_pRotateKeys[i2].dwTime;

			if ( (fTime2 - fTime1) == 0 )
				fLerpValue = 0;
			else
				fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

//#define USE_SQUAD
#ifdef USE_SQUAD
			int i0 = i1 - 1;
			int i3 = i2 + 1;

			if ( i0 < 0 )
				i0 += m_cRotateKeys;

			if ( i3 >= (INT) m_cRotateKeys )
				i3 -= m_cRotateKeys;

			D3DXQUATERNION qA, qB, qC;
			D3DXQuaternionSquadSetup ( &qA, &qB, &qC,
				&m_pRotateKeys[i0].quatRotate, &m_pRotateKeys[i1].quatRotate, 
				&m_pRotateKeys[i2].quatRotate, &m_pRotateKeys[i3].quatRotate );

			D3DXQuaternionSquad ( &quat, &m_pRotateKeys[i1].quatRotate, &qA, &qB, &qC, fLerpValue );
#else
			D3DXQuaternionSlerp ( &quat, &m_pRotateKeys[i1].quatRotate, &m_pRotateKeys[i2].quatRotate, fLerpValue );
#endif

			quat.w = -quat.w;
			D3DXMatrixRotationQuaternion ( &matTemp, &quat );
			D3DXMatrixMultiply ( &matResult, &matResult, &matTemp );

			bAnimate = true;
		}

		if ( m_pPositionKeys )
		{
			dwp2=dwp3=0;

			//for ( iKey = 0 ;iKey < m_cPositionKeys ; iKey++ )
			//{
			//	if ( (float)m_pPositionKeys[iKey].dwTime > fTime )
			//	{
			//		dwp3 = iKey;

			//		if (iKey > 0)
			//		{
			//			dwp2= iKey - 1;
			//		}
			//		else  // when iKey == 0, then dwp2 == 0
			//		{
			//			dwp2 = iKey;
			//		}

			//		break;
			//	}
			//}

			for ( iKey = m_cPositionKeys-1 ;iKey >= 0 ; iKey-- )
			{
				if ( (float)m_pPositionKeys[iKey].dwTime <= fTime )
				{
					dwp2 = iKey;

					if (iKey == m_cPositionKeys-1)
					{
						dwp3 = iKey;
					}
					else
					{
						dwp3 = iKey + 1;
					}

					break;
				}
			}
            
			fTime1 = (float)m_pPositionKeys[dwp2].dwTime;
			fTime2 = (float)m_pPositionKeys[dwp3].dwTime;

			if ( (fTime2 - fTime1) ==0 )
				fLerpValue = 0;
			else
				fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);


			D3DXVec3Lerp ( (D3DXVECTOR3*)&vPos,
					&m_pPositionKeys[dwp2].vPos,
					&m_pPositionKeys[dwp3].vPos,
					fLerpValue );

			D3DXMatrixTranslation ( &matTemp, vPos.x, vPos.y, vPos.z );

			D3DXMatrixMultiply ( &matResult, &matResult, &matTemp );
			bAnimate = true;
		}
		else
		{
			D3DXMatrixTranslation ( &matTemp, m_pBoneToAnimate->matRotOrig._41, m_pBoneToAnimate->matRotOrig._42, m_pBoneToAnimate->matRotOrig._43 );

			D3DXMatrixMultiply ( &matResult, &matResult, &matTemp );
		}

		if (bAnimate)
		{
			m_pBoneToAnimate->matRot = matResult;
		}
	}
}


//DxSkinAnimation::DxSkinAnimation() :
//	m_dwRefCount(0),
//	m_pSkeleton(NULL),
//
//	m_bLOAD(false),
//	m_bVALID(false)
//{
//}
//
//DxSkinAnimation::~DxSkinAnimation()
//{
//	std::for_each( m_listAnimation.begin(), m_listAnimation.end(), std_afunc::DeleteObject() );
//	m_listAnimation.clear();
//
//	std::for_each( m_listAniUPBODY.begin(), m_listAniUPBODY.end(), std_afunc::DeleteObject() );
//	m_listAniUPBODY.clear();
//}
//
////	Note : xFile에서 에니메이션 데이터를 로딩.
////
//HRESULT SAnimContainer::CreateAnimationData ( SANIMCONINFO &sAnimInfo, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH /*=false*/ )
//{
//	bool bTHREAD = true;
//
//	HRESULT hr = S_OK;
//	SANIMCONINFO::operator = ( sAnimInfo );
//
//	if ( !bREPRESH && (m_dwFlag&ACF_NEWINFO)==NULL )
//	{
//		if ( bTHREAD && !(m_MainType==AN_GUARD_N||m_MainType==AN_PLACID) )
//		{
//			//	스레드를 사용하여 나중에 필요시에 읽음.
//			m_bLOAD = false;
//			m_bVALID = false;
//			
//			return S_OK;
//		}
//		else
//		{
//			m_bLOAD = true;
//			m_bVALID = true;
//
//			CString strBinFile;
//			STRUTIL::ChangeExt(m_Name.c_str(), ".x", strBinFile, ".bin");
//
//			TSTRING strPath;
//			strPath += DxSkinAniMan::GetInstance().GetPath ();
//			strPath += strBinFile.GetString();
//
//			hr = LoadAnimFromFile ( strPath );
//			if ( SUCCEEDED(hr) )
//                return hr;
//		}
//
//		return E_FAIL;
//	}
//
//	//	x 파일에서 직접 읽어오는 경우, 스레드 사용하지 않음.
//	m_bLOAD = true;
//	m_bVALID = true;
//
//	std::for_each( m_listAnimation.begin(), m_listAnimation.end(), std_afunc::DeleteObject() );
//	m_listAnimation.clear();
//
//	std::for_each( m_listAniUPBODY.begin(), m_listAniUPBODY.end(), std_afunc::DeleteObject() );
//	m_listAniUPBODY.clear();
//
//	LPDIRECTXFILE pxofapi = NULL;
//	LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
//	LPDIRECTXFILEDATA pxofobjCur = NULL;
//	int cchFileName;
//
//    m_pSkeleton = DxBoneCollector::GetInstance().Load(m_SkeletionName, pd3dDevice);
//	if (!m_pSkeleton)
//        return E_FAIL;
//
//    CString strEncryptFile;
//    CPrivateResource cResource( CPrivateResource::ETypeSkin );
//
//	//	Note : xFile 열기.
//	//
//	cchFileName = m_Name.length();
//	if (cchFileName < 2)
//	{
//		hr = E_FAIL;
//		goto e_Exit;
//	}
//    
//	//	xFile 개체를 만듬.
//	hr = DirectXFileCreate(&pxofapi);
//	if (FAILED(hr))	goto e_Exit;
//    
//    //	xFile 템플리트 등록.
//	hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
//		D3DRM_XTEMPLATE_BYTES);
//	if (FAILED(hr))
//		goto e_Exit;
//
//    TCHAR szPath[MAX_PATH] = "";
//	StringCchCopy(szPath, MAX_PATH, DxSkinAniMan::GetInstance().GetPath());
//	StringCchCat(szPath, MAX_PATH, m_Name.c_str());
//
//	STRUTIL::ChangeExt( szPath, strEncryptFile, ".mxf" );
//
//	if ( cResource.LoadFileDec( strEncryptFile ) )
//	{
//		D3DXF_FILELOADMEMORY sData;
//		sData.lpMemory = (LPCVOID)cResource.GetData();
//		sData.dSize = cResource.GetSize();
//
//		hr = pxofapi->CreateEnumObject((LPVOID)&sData,
//			DXFILELOAD_FROMMEMORY,
//			&pxofenum);
//
//		if (FAILED(hr))
//			goto e_Exit;
//	}
//	else
//	{
//		//	실제 xFile을 열음.
//		hr = pxofapi->CreateEnumObject((LPVOID)szPath,
//			DXFILELOAD_FROMFILE,
//			&pxofenum);
//		if (FAILED(hr))	goto e_Exit;
//	}
//
//	//	Note : 최상위 오브젝트를 읽어옴.
//	//
//	while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
//	{
//		const GUID *type;
//
//		hr = pxofobjCur->GetType(&type);
//		if (FAILED(hr))	goto e_Exit;
//
//		//	Note : 에니메이션 오브젝트.
//		//
//		if ( *type == TID_D3DRMAnimationSet )
//		{
//			LoadAnimationSet ( pxofobjCur );
//		}
//		else if ( *type == TID_D3DRMAnimation )
//		{
//			LoadAnimation ( pxofobjCur );
//		}
//
//		GXRELEASE(pxofobjCur);
//        
//		if (FAILED(hr))	goto e_Exit;
//	}
//
//
//	if ( m_dwFlag & ACF_NEWINFO )
//	{
//		m_dwFlag &= ~ACF_NEWINFO;
//	}
//
//	SaveToFile ();
//
//e_Exit:
//    GXRELEASE(pxofobjCur);
//    GXRELEASE(pxofenum);
//    GXRELEASE(pxofapi);
//    
//    return hr;
//}
//
////	Note : 에니메이션을 읽어옴.
////
////
//HRESULT DxSkinAnimation::LoadAnimation ( LPDIRECTXFILEDATA pxofobjCur )
//{
//	HRESULT hr = S_OK;
//	SRotateKeyXFile *pFileRotateKey;
//	SScaleKeyXFile *pFileScaleKey;
//	SPositionKeyXFile *pFilePosKey;
//	SMatrixKeyXFile *pFileMatrixKey;
//	SAnimation *pAnimationCur;
//	LPDIRECTXFILEDATA pxofobjChild = NULL;
//	LPDIRECTXFILEOBJECT pxofChild = NULL;
//	LPDIRECTXFILEDATAREFERENCE pxofobjChildRef = NULL;
//	const GUID *type;
//	DWORD dwSize;
//	PBYTE pData;
//	DWORD dwKeyType;
//	DWORD cKeys;
//	DWORD iKey;
//	DWORD cchName;
//	TCHAR *szBoneName;
//    
//	//	Note : 새로운 Animation 만듬.
//	//
//	pAnimationCur = new SAnimation();
//	if (pAnimationCur == NULL)
//	{
//		hr = E_OUTOFMEMORY;
//		goto e_Exit;
//	}
//    
//	//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
//	//		QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
//	//
//	while ( SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)) )
//	{
//        //	Note : Object가 "FileDataReference"일때.
//		//
//		hr = pxofChild->QueryInterface ( IID_IDirectXFileDataReference, (LPVOID *)&pxofobjChildRef );
//		if (SUCCEEDED(hr))
//		{
//			hr = pxofobjChildRef->Resolve (&pxofobjChild);
//			if (SUCCEEDED(hr))
//			{
//				hr = pxofobjChild->GetType(&type);
//				if (FAILED(hr))
//					goto e_Exit;
//                
//				//	Note : 에니에이션에 연결된 프레임을 지정.
//				//
//				if ( TID_D3DRMFrame == *type )
//				{
//					if ( pAnimationCur->m_pBoneToAnimate != NULL )
//					{
//						hr = E_INVALIDARG;
//						goto e_Exit;
//					}
//                    
//					//	Note : 연결된 프레임의 이름을 가져옴.
//					//
//					hr = pxofobjChild->GetName ( NULL, &cchName );
//					if (FAILED(hr))
//						goto e_Exit;
//                    
//					if (cchName == 0)
//					{
//						hr = E_INVALIDARG;
//						goto e_Exit;
//					}
//                    
//					szBoneName = (TCHAR*)_alloca(cchName);
//					if (szBoneName == NULL)
//					{
//						hr = E_OUTOFMEMORY;
//						goto e_Exit;
//					}
//                    
//					hr = pxofobjChild->GetName ( szBoneName, &cchName );
//					if (FAILED(hr))
//						goto e_Exit;
//                    
//					//	NEED : 연결된 "BONE" 을 지정.
//					//
//					pAnimationCur->m_pBoneToAnimate = m_pSkeleton->FindBone ( szBoneName );
//					if ( !pAnimationCur->m_pBoneToAnimate )
//					{
//						hr = E_INVALIDARG;
//						goto e_Exit;
//					}
//
//					//	Note : 연결된 "BONE" 이름 저장.
//					//
//					pAnimationCur->m_strBone = szBoneName;
//				}
//                
//				GXRELEASE(pxofobjChild);
//
//			}	//	Resolve()
//            
//			GXRELEASE(pxofobjChildRef);
//
//		}	// IID_IDirectXFileDataReference ?
//		else
//		{
//            //	Note : Object가 "FileData"일 경우.
//			//
//			hr = pxofChild->QueryInterface(IID_IDirectXFileData,
//				(LPVOID *)&pxofobjChild);
//			if (SUCCEEDED(hr))
//			{
//				hr = pxofobjChild->GetType(&type);
//				if (FAILED(hr))
//				{
//					goto e_Exit;
//				}
//                
//				if ( TID_D3DRMAnimationKey == *type )
//				{
//					hr = pxofobjChild->GetData ( NULL, &dwSize, (PVOID*)&pData );
//					if (FAILED(hr))
//					{
//						goto e_Exit;
//					}
//                    
//					dwKeyType = ((DWORD*)pData)[0];
//					cKeys = ((DWORD*)pData)[1];
//
//					DWORD dwEndTime = 0;
//					DWORD dwUnitKey = UINT_MAX;
//					
//					//	Note : 쿼터니언 회전 키.
//					//
//					if ( dwKeyType == 0 )
//					{
//						if ( pAnimationCur->m_pRotateKeys != NULL )
//						{
//							hr = E_INVALIDARG;
//							goto e_Exit;
//						}
//                        
//						//	Note : 회전키 생성.
//						//
//						pAnimationCur->m_pRotateKeys = new SRotateKey[cKeys];
//						if ( pAnimationCur->m_pRotateKeys == NULL )
//						{
//							hr = E_OUTOFMEMORY;
//							goto e_Exit;
//						}
//                        
//						//	회전키의 갯수.
//                        pAnimationCur->m_cRotateKeys = cKeys;
//                        
//						//	NOTE : 쿼터니언 값.
//						//
//                        pFileRotateKey =  (SRotateKeyXFile*)(pData + (sizeof(DWORD) * 2));
//                        for (iKey = 0;iKey < cKeys; iKey++)
//                        {
//							//	현제 키의 시간.
//                            pAnimationCur->m_pRotateKeys[iKey].dwTime = pFileRotateKey->dwTime;
//
//                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.x = pFileRotateKey->x;
//                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.y = pFileRotateKey->y;
//                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.z = pFileRotateKey->z;
//                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.w = pFileRotateKey->w;
//                            
//							if ( iKey!=0 )
//							{
//								DWORD dxTime = pAnimationCur->m_pRotateKeys[iKey].dwTime - pAnimationCur->m_pRotateKeys[iKey-1].dwTime;
//								if (  dxTime < dwUnitKey )
//									dwUnitKey = dxTime;
//							}
//
//                            pFileRotateKey += 1;
//                        }
//
//						dwEndTime = pAnimationCur->m_pRotateKeys[cKeys-1].dwTime;
//                    }
//					//	Note : 스케일 키.
//					//
//                    else if (dwKeyType == 1)
//                    {
//                        if (pAnimationCur->m_pScaleKeys != NULL)
//                        {
//                            hr = E_INVALIDARG;
//                            goto e_Exit;
//                        }
//                
//						//	Note : 스케일키 생성.
//						//
//                        pAnimationCur->m_pScaleKeys = new SScaleKey[cKeys];
//                        if (pAnimationCur->m_pScaleKeys == NULL)
//                        {
//                            hr = E_OUTOFMEMORY;
//                            goto e_Exit;
//                        }
//                        
//						//	스케일키 갯수.
//                        pAnimationCur->m_cScaleKeys = cKeys;
//                        
//						//	Note : 스케일키 값.
//						//
//                        pFileScaleKey =  (SScaleKeyXFile*)(pData + (sizeof(DWORD) * 2));
//                        for (iKey = 0;iKey < cKeys; iKey++)
//                        {
//							//	현제 키의 시간.
//                            pAnimationCur->m_pScaleKeys[iKey].dwTime = pFileScaleKey->dwTime;
//                            
//							pAnimationCur->m_pScaleKeys[iKey].vScale = pFileScaleKey->vScale;
//
//							if ( iKey!=0 )
//							{
//								DWORD dxTime = pAnimationCur->m_pScaleKeys[iKey].dwTime - pAnimationCur->m_pScaleKeys[iKey-1].dwTime;
//								if (  dxTime < dwUnitKey )
//									dwUnitKey = dxTime;
//							}
//                            
//                            pFileScaleKey += 1;
//                        }
//
//						dwEndTime = pAnimationCur->m_pScaleKeys[cKeys-1].dwTime;
//                    }
//					//	Note : 이동 키.
//					//
//                    else if (dwKeyType == 2)
//                    {
//                        if (pAnimationCur->m_pPositionKeys != NULL)
//                        {
//                            hr = E_INVALIDARG;
//                            goto e_Exit;
//                        }
//                        
//						//	Note : 이동 키 생성.
//						//
//                        pAnimationCur->m_pPositionKeys = new SPositionKey[cKeys];
//                        if (pAnimationCur->m_pPositionKeys == NULL)
//                        {
//                            hr = E_OUTOFMEMORY;
//                            goto e_Exit;
//                        }
//                        
//						//	이동키 갯수.
//                        pAnimationCur->m_cPositionKeys = cKeys;
//                        
//						//	Note : 이동키 값.
//						//
//                        pFilePosKey =  (SPositionKeyXFile*)(pData + (sizeof(DWORD) * 2));
//                        for (iKey = 0;iKey < cKeys; iKey++)
//                        {
//							//	현제 키의 시간.
//                            pAnimationCur->m_pPositionKeys[iKey].dwTime = pFilePosKey->dwTime;
//                            
//							pAnimationCur->m_pPositionKeys[iKey].vPos = pFilePosKey->vPos;
//
//							if ( iKey!=0 )
//							{
//								DWORD dxTime = pAnimationCur->m_pPositionKeys[iKey].dwTime - pAnimationCur->m_pPositionKeys[iKey-1].dwTime;
//								if (  dxTime < dwUnitKey )
//									dwUnitKey = dxTime;
//							}
//                            
//                            pFilePosKey += 1;
//                        }
//
//						dwEndTime = pAnimationCur->m_pPositionKeys[cKeys-1].dwTime;
//                    }
//					//	Note : 트렌스폼 키.
//					//
//                    else if (dwKeyType == 4)
//                    {
//						if ( pAnimationCur->m_pMatrixKeys!=NULL )
//                        {
//                            hr = E_INVALIDARG;
//                            goto e_Exit;
//                        }
//                        
//						//	Note : 트렌스폼 키 생성.
//						//
//						pAnimationCur->m_pMatrixKeys = new SMatrixKey[cKeys];
//						if (pAnimationCur->m_pMatrixKeys == NULL)
//						{
//							hr = E_OUTOFMEMORY;
//							goto e_Exit;
//						}
//
//						//	트렌스폼 키 갯수.
//                        pAnimationCur->m_cMatrixKeys = cKeys;
//                        
//						//	Note : 트렌스폼 키 값.
//						//
//                        pFileMatrixKey =  (SMatrixKeyXFile*)(pData + (sizeof(DWORD) * 2));
//                        for (iKey = 0;iKey < cKeys; iKey++)
//                        {
//							//	현제 프레임의 시간.
//                            pAnimationCur->m_pMatrixKeys[iKey].dwTime = pFileMatrixKey->dwTime;
//                            
//							pAnimationCur->m_pMatrixKeys[iKey].mat = pFileMatrixKey->mat;
//                            
//							if ( iKey!=0 )
//							{
//								DWORD dwTime = pAnimationCur->m_pMatrixKeys[iKey].dwTime - pAnimationCur->m_pMatrixKeys[iKey-1].dwTime;
//								if (  dwTime < dwUnitKey )
//									dwUnitKey = dwTime;
//							}
//
//                            pFileMatrixKey += 1;
//                        }
//
//						// Optimize 
//						pAnimationCur->m_fDivFullTime = 1.f/pAnimationCur->m_pMatrixKeys[cKeys-1].dwTime;
//						pAnimationCur->m_fMatrixKeys = (float)pAnimationCur->m_cMatrixKeys;
//
//						dwEndTime = pAnimationCur->m_pMatrixKeys[cKeys-1].dwTime;
//
//						// Convert Quaternion
//						pAnimationCur->ConvertMatToQuat();
//                    }
//                    else
//                    {
//                        hr = E_INVALIDARG;
//                        goto e_Exit;
//                    }
//					
//					m_dwETimeOrig = dwEndTime;
//					m_UNITTIME = dwUnitKey;
//
//					if ( m_dwFlag&ACF_NEWINFO || m_dwETime>m_dwETimeOrig )
//					{
//						m_dwETime = m_dwETimeOrig;
//						if ( m_dwSTime > m_dwETime )	m_dwSTime = m_dwETime;
//					}
//                }
//                
//                GXRELEASE(pxofobjChild);
//            }
//        }
//        
//        GXRELEASE(pxofChild);
//    }
//
//	//	Note : 에니메이션 리스트에 등록.
//	switch( pAnimationCur->m_pBoneToAnimate->m_emBody )
//	{
//	case EMBODY_DEFAULT:
//		m_listAnimation.push_back ( pAnimationCur );
//		break;
//	case EMBODY_UPBODY:
//		m_listAniUPBODY.push_back ( pAnimationCur );
//		break;
//	};	
//    
//e_Exit:
//    GXRELEASE(pxofobjChild);
//    GXRELEASE(pxofChild);
//    GXRELEASE(pxofobjChildRef);
//    return hr;
//}
//
////	Note : AnimationSet 를 읽어와 구성함.
////
//HRESULT DxSkinAnimation::LoadAnimationSet ( LPDIRECTXFILEDATA pxofobjCur )
//{
//    const GUID *type;
//    HRESULT hr = S_OK;
//    LPDIRECTXFILEDATA pxofobjChild = NULL;
//    LPDIRECTXFILEOBJECT pxofChild = NULL;
//    
//   
//	//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
//	//		QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
//	//
//    while (SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)))
//    {
//        // Query the child for it's FileData
//        hr = pxofChild->QueryInterface ( IID_IDirectXFileData, (LPVOID *)&pxofobjChild );
//        
//		if (SUCCEEDED(hr))
//        {
//            hr = pxofobjChild->GetType(&type);
//            if (FAILED(hr))
//                goto e_Exit;
//            
//            if( TID_D3DRMAnimation == *type )
//            {
//                hr = LoadAnimation ( pxofobjChild );
//                if (FAILED(hr))
//                    goto e_Exit;
//            }
//            
//            GXRELEASE(pxofobjChild);
//        }
//        
//        GXRELEASE(pxofChild);
//    }
//    
//e_Exit:
//    GXRELEASE(pxofobjChild);
//    GXRELEASE(pxofChild);
//    return hr;
//}
//
//BOOL DxSkinAnimation::IsValidateANI()
//{
//	if ( !m_pSkeleton )		return FALSE;
//
//	if ( !m_bLOAD )		// 로드 요청을 안 했다면 요청을 한다.
//	{
//		m_bLOAD = true;
//		DxSkinAnimationManager::GetInstance().AddBinFIleLoad(m_Name, m_pSkeleton);
//		return FALSE;
//	}
//
//	if( m_bVALID )	return TRUE;
//	else			return FALSE;
//}
//
//bool DxSkinAnimation::UpdateTime ( float fCurTime, float fWeight, BOOL bFirst, EMBODY emBody )
//{
//	if ( !m_pSkeleton )		return false;
//
//	if ( !m_bLOAD )
//	{
//		//	로드 요청.
//		m_bLOAD = true;
//		DxSkinAnimationManager::GetInstance().AddBinFIleLoad(m_Name, m_pSkeleton);
//		return false;
//	}
//
//	if ( m_bVALID )
//	{
//		//std::for_each(m_listAnimation.begin(),m_listAnimation.end(),std::bind2nd(std::mem_fun(&SAnimation::SetTime),fCurTime,fWeight,bFirst));
//
//		SANILIST_ITER iter;
//		switch( emBody )
//		{
//		case EMBODY_DEFAULT:
//			iter = m_listAniUPBODY.begin();
//			for( ; iter!=m_listAniUPBODY.end(); ++iter )
//			{
//				(*iter)->SetTime( fCurTime, fWeight, bFirst );
//			}
//
//			iter = m_listAnimation.begin();
//			for( ; iter!=m_listAnimation.end(); ++iter )
//			{
//				(*iter)->SetTime( fCurTime, fWeight, bFirst );
//			}
//
//			break;
//
//		case EMBODY_UPBODY:
//			iter = m_listAniUPBODY.begin();
//			for( ; iter!=m_listAniUPBODY.end(); ++iter )
//			{
//				(*iter)->SetTime( fCurTime, fWeight, bFirst );
//			}
//			break;
//
//		case EMBODY_DOWNBODY:
//			iter = m_listAnimation.begin();
//			for( ; iter!=m_listAnimation.end(); ++iter )
//			{
//				(*iter)->SetTime( fCurTime, fWeight, bFirst );
//			}
//			break;
//		};
//		
//		return true;
//	}
//
//	return false;
//}
//
//bool DxSkinAnimation::UpdateMix ( float fCurMixTime )
//{
//	if ( !m_pSkeleton )		return false;
//
//	if ( !m_bLOAD )
//	{
//		//	로드 요청.
//		m_bLOAD = true;
//		DxSkinAnimationManager::GetInstance().AddBinFIleLoad(m_Name, m_pSkeleton);
//		return false;
//	}
//
//	if ( m_bVALID )
//	{
//		std::for_each(m_listAniUPBODY.begin(),m_listAniUPBODY.end(),std::bind2nd(std::mem_fun(&SAnimation::SetMix),fCurMixTime));
//		std::for_each(m_listAnimation.begin(),m_listAnimation.end(),std::bind2nd(std::mem_fun(&SAnimation::SetMix),fCurMixTime));
//
//		//SANILIST_ITER iter = m_listAnimation.begin();
//		//for( ; iter!=m_listAnimation.end(); ++iter )
//		//{
//		//	(*iter)->SetTime( fCurTime, fWeight, bFirst );
//		//}
//		return true;
//	}
//
//	return false;
//}
//
//void DxSkinAnimation::UpdateBoneScale( float fWeight, BOOL bFirst, float fTime )
//{
//	m_pAniScale->UpdateBoneScale( m_pSkeleton, fWeight, bFirst, fTime, (float)m_dwSTime, (float)m_dwETime, (float)m_UNITTIME );
//}

void SAnimation::ConvertMatToQuat( DxBoneTrans*	pBoneToAnimateTemp )
{
#ifdef USE_ANI_QUATERNION

	D3DXQUATERNION	vQuat;

	if( !m_pQuatPosKeys && m_pMatrixKeys )
	{
		m_cQuatPosKeys = m_cMatrixKeys;
		m_pQuatPosKeys = new SQuatPosKey[m_cQuatPosKeys];
		for ( DWORD i=0; i<m_cQuatPosKeys; ++i )
		{
			m_pQuatPosKeys[i].m_dwTime = m_pMatrixKeys[i].dwTime;
			D3DXMatrixToSQT(
				m_pQuatPosKeys[i].m_vScale,
				vQuat,
				m_pQuatPosKeys[i].m_vPos,
				m_pMatrixKeys[i].mat );

			if( pBoneToAnimateTemp )
			{
				m_pQuatPosKeys[i].m_vPos.x = pBoneToAnimateTemp->matRotOrig._41;
				m_pQuatPosKeys[i].m_vPos.y = pBoneToAnimateTemp->matRotOrig._42;
				m_pQuatPosKeys[i].m_vPos.z = pBoneToAnimateTemp->matRotOrig._43;
			}

			CompressionQuaternion( m_pQuatPosKeys[i].m_vQuatComp, vQuat );	// 사원수 압축
		}

		if ( pBoneToAnimateTemp && pBoneToAnimateTemp->m_dwFlag & USE_ANI_INVERSE_SCALE_ROOT )
		{
			if ( pBoneToAnimateTemp->m_dwFlag & USE_ANI_INVERSE_SCALE_ROOT )
			{
				for ( UINT i=0; i<m_cQuatPosKeys; ++i )
				{
					if ( m_pQuatPosKeys[i].m_vScale.x < 0.f )
					{
						m_pQuatPosKeys[i].m_vScale = -m_pQuatPosKeys[i].m_vScale;
					}
				}
			}
			//else if ( m_pBoneToAnimate->m_dwFlag & USE_ANI_INVERSE_SCALE_CHILE )
			//{
			//	for ( UINT i=0; i<m_cQuatPosKeys; ++i )
			//	{
			//		if ( m_pQuatPosKeys[i].m_vPos.x < 0.f )
			//		{
			//			m_pQuatPosKeys[i].m_vPos = -m_pQuatPosKeys[i].m_vPos;
			//		}
			//	}
			//}
		}

		// Note : Matrix는 사용하지 않을 것이다. 데이터의 용량과도 상관이 있다.
		m_cMatrixKeys = 0L;
		SAFE_DELETE_ARRAY( m_pMatrixKeys );
	}

#endif
}

HRESULT SAnimation::SaveToFile ( sc::SerialFile &SFile )
{
	SFile << VERSION;

	SFile << m_strBone;

	SFile << m_cRotateKeys;
	if ( m_cRotateKeys>0 )
	{
		SFile.WriteBuffer ( m_pRotateKeys, sizeof(SRotateKey)*m_cRotateKeys );
	}

	// 쓰레기값
	DWORD dwGabageData = static_cast<DWORD> (m_strBone.length());
	SFile << dwGabageData;

	SFile << m_cPositionKeys;
	if ( m_cPositionKeys>0 )
	{
		SFile.WriteBuffer ( m_pPositionKeys, sizeof(SPositionKey)*m_cPositionKeys );
	}



	SFile << m_cQuatPosKeys;
	if ( m_cQuatPosKeys>0 )
	{
		SFile.WriteBuffer ( m_pQuatPosKeys, sizeof(SQuatPosKey)*m_cQuatPosKeys );
	}

	SFile << m_cMatrixKeys;
	if ( m_cMatrixKeys>0 )
	{
		SFile.WriteBuffer ( m_pMatrixKeys, sizeof(SMatrixKey)*m_cMatrixKeys );
	}

	// 쓰레기값
	dwGabageData = static_cast<DWORD> (m_strBone.length() + VERSION);
	SFile << dwGabageData;

	SFile << m_cScaleKeys;
	if ( m_cScaleKeys>0 )
	{
		SFile.WriteBuffer ( m_pScaleKeys, sizeof(SScaleKey)*m_cScaleKeys );
	}	

	return S_OK;
}

HRESULT SAnimation::LoadFromFile ( sc::BaseStream &SFile, const DxSkeleton* pSkeleton )
{
//	GASSERT(pSkeleton&&"SAnimation::LoadFromFile()");

	DWORD dwVER(0);
	SFile >> dwVER;

	switch ( dwVER )
	{
	case 0x0100:	Load_0100( SFile, pSkeleton );	break;
	case 0x0101:	Load_0101( SFile, pSkeleton );	break;
	case 0x0102:	Load_0102( SFile, pSkeleton );	break;
	case VERSION:	Load( SFile, pSkeleton );	break;
	default:
		{
			GASSERT(0&&"SAnimation::LoadFromFile() 알수 없는 버전입니다.");	
			return E_FAIL;
		}
		break;
	};


	return S_OK;
}

HRESULT SAnimation::Load_0100( sc::BaseStream &SFile, const DxSkeleton* pSkeleton )
{
	SFile >> m_cPositionKeys;
	if ( m_cPositionKeys>0 )
	{
		m_pPositionKeys = new SPositionKey[m_cPositionKeys];
		SFile.ReadBuffer ( m_pPositionKeys, sizeof(SPositionKey)*m_cPositionKeys );
	}

	SFile >> m_cRotateKeys;
	if ( m_cRotateKeys>0 )
	{
		m_pRotateKeys = new SRotateKey[m_cRotateKeys];
		SFile.ReadBuffer ( m_pRotateKeys, sizeof(SRotateKey)*m_cRotateKeys );
	}

	SFile >> m_cScaleKeys;
	if ( m_cScaleKeys>0 )
	{
		m_pScaleKeys = new SScaleKey[m_cScaleKeys];
		SFile.ReadBuffer ( m_pScaleKeys, sizeof(SScaleKey)*m_cScaleKeys );
	}

	SFile >> m_cMatrixKeys;
	if ( m_cMatrixKeys>0 )
	{
		m_pMatrixKeys = new SMatrixKey[m_cMatrixKeys];
		SFile.ReadBuffer ( m_pMatrixKeys, sizeof(SMatrixKey)*m_cMatrixKeys );

		// Optimize 
		m_fDivFullTime = 1.f/m_pMatrixKeys[m_cMatrixKeys-1].dwTime;
		m_fMatrixKeys = (float)m_cMatrixKeys;
	}

	SFile >> m_strBone;

	//	적용될 BONE 과 연결.
	m_pBoneToAnimate = pSkeleton->FindBone_t( m_strBone.c_str() );

	// 본이 없음.
	if( !m_pBoneToAnimate )
	{
		return E_FAIL;
	}

    // Convert Quaternion
	switch( m_pBoneToAnimate->m_emBody )
	{
	case EMBODY_ROOT:
		ConvertMatToQuat( NULL );
		break;
	case EMBODY_DOWNBODY:
	case EMBODY_UPBODY:
		ConvertMatToQuat( m_pBoneToAnimate );
		break;
	};

	return S_OK;
}

HRESULT SAnimation::Load_0101( sc::BaseStream &SFile, const DxSkeleton* pSkeleton )
{
	SFile >> m_cPositionKeys;
	if ( m_cPositionKeys>0 )
	{
		m_pPositionKeys = new SPositionKey[m_cPositionKeys];
		SFile.ReadBuffer ( m_pPositionKeys, sizeof(SPositionKey)*m_cPositionKeys );
	}

	SFile >> m_cRotateKeys;
	if ( m_cRotateKeys>0 )
	{
		m_pRotateKeys = new SRotateKey[m_cRotateKeys];
		SFile.ReadBuffer ( m_pRotateKeys, sizeof(SRotateKey)*m_cRotateKeys );
	}

	SFile >> m_cScaleKeys;
	if ( m_cScaleKeys>0 )
	{
		m_pScaleKeys = new SScaleKey[m_cScaleKeys];
		SFile.ReadBuffer ( m_pScaleKeys, sizeof(SScaleKey)*m_cScaleKeys );
	}

	SFile >> m_cMatrixKeys;
	if ( m_cMatrixKeys>0 )
	{
		m_pMatrixKeys = new SMatrixKey[m_cMatrixKeys];
		SFile.ReadBuffer ( m_pMatrixKeys, sizeof(SMatrixKey)*m_cMatrixKeys );

		// Optimize 
		m_fDivFullTime = 1.f/m_pMatrixKeys[m_cMatrixKeys-1].dwTime;
		m_fMatrixKeys = (float)m_cMatrixKeys;
	}

	SFile >> m_cQuatPosKeys;
	if ( m_cQuatPosKeys>0 )
	{
		m_pQuatPosKeys = new SQuatPosKey[m_cQuatPosKeys];
		SFile.ReadBuffer ( m_pQuatPosKeys, sizeof(SQuatPosKey)*m_cQuatPosKeys );
	}

	SFile >> m_strBone;

	//	적용될 BONE 과 연결.
	m_pBoneToAnimate = pSkeleton->FindBone_t( m_strBone.c_str() );

	// 본이 없음.
	if( !m_pBoneToAnimate )
	{
		return E_FAIL;
	}

	if ( m_pBoneToAnimate->m_dwFlag & USE_ANI_INVERSE_SCALE_ROOT )
	{
		for ( UINT i=0; i<m_cQuatPosKeys; ++i )
		{
			if ( m_pQuatPosKeys[i].m_vScale.x < 0.f )
			{
				m_pQuatPosKeys[i].m_vScale = -m_pQuatPosKeys[i].m_vScale;
			}
		}
	}
	//else if ( m_pBoneToAnimate->m_dwFlag & USE_ANI_INVERSE_SCALE_CHILE )
	//{
	//	for ( UINT i=0; i<m_cQuatPosKeys; ++i )
	//	{
	//		if ( m_pQuatPosKeys[i].m_vPos.x < 0.f )
	//		{
	//			m_pQuatPosKeys[i].m_vPos = -m_pQuatPosKeys[i].m_vPos;
	//		}
	//	}
	//}

	// Convert Quaternion
	switch( m_pBoneToAnimate->m_emBody )
	{
	case EMBODY_ROOT:
		ConvertMatToQuat( NULL );
		break;
	case EMBODY_DOWNBODY:
	case EMBODY_UPBODY:
		ConvertMatToQuat( m_pBoneToAnimate );
		break;
	};

	return S_OK;
}

HRESULT SAnimation::Load_0102( sc::BaseStream &SFile, const DxSkeleton* pSkeleton )
{

	SFile >> m_strBone;

	SFile >> m_cRotateKeys;
	if ( m_cRotateKeys>0 )
	{
		m_pRotateKeys = new SRotateKey[m_cRotateKeys];
		SFile.ReadBuffer ( m_pRotateKeys, sizeof(SRotateKey)*m_cRotateKeys );
	}
	
	SFile >> m_cPositionKeys;
	if ( m_cPositionKeys>0 )
	{
		m_pPositionKeys = new SPositionKey[m_cPositionKeys];
		SFile.ReadBuffer ( m_pPositionKeys, sizeof(SPositionKey)*m_cPositionKeys );
	}

	DWORD dwGabageValue;
	SFile >> dwGabageValue;

	SFile >> m_cQuatPosKeys;
	if ( m_cQuatPosKeys>0 )
	{
		m_pQuatPosKeys = new SQuatPosKey[m_cQuatPosKeys];
		SFile.ReadBuffer ( m_pQuatPosKeys, sizeof(SQuatPosKey)*m_cQuatPosKeys );
	}

	SFile >> dwGabageValue;

	SFile >> m_cMatrixKeys;
	if ( m_cMatrixKeys>0 )
	{
		m_pMatrixKeys = new SMatrixKey[m_cMatrixKeys];
		SFile.ReadBuffer ( m_pMatrixKeys, sizeof(SMatrixKey)*m_cMatrixKeys );

		// Optimize 
		m_fDivFullTime = 1.f/m_pMatrixKeys[m_cMatrixKeys-1].dwTime;
		m_fMatrixKeys = (float)m_cMatrixKeys;
	}

	SFile >> m_cScaleKeys;
	if ( m_cScaleKeys>0 )
	{
		m_pScaleKeys = new SScaleKey[m_cScaleKeys];
		SFile.ReadBuffer ( m_pScaleKeys, sizeof(SScaleKey)*m_cScaleKeys );
	}

	//	적용될 BONE 과 연결.
	m_pBoneToAnimate = pSkeleton->FindBone_t( m_strBone.c_str() );

	// 본이 없음.
	if( !m_pBoneToAnimate )
	{
		return E_FAIL;
	}

	if ( m_pBoneToAnimate->m_dwFlag & USE_ANI_INVERSE_SCALE_ROOT )
	{
		for ( UINT i=0; i<m_cQuatPosKeys; ++i )
		{
			if ( m_pQuatPosKeys[i].m_vScale.x < 0.f )
			{
				m_pQuatPosKeys[i].m_vScale = -m_pQuatPosKeys[i].m_vScale;
			}
		}
	}
	//else if ( m_pBoneToAnimate->m_dwFlag & USE_ANI_INVERSE_SCALE_CHILE )
	//{
	//	for ( UINT i=0; i<m_cQuatPosKeys; ++i )
	//	{
	//		if ( m_pQuatPosKeys[i].m_vPos.x < 0.f )
	//		{
	//			m_pQuatPosKeys[i].m_vPos = -m_pQuatPosKeys[i].m_vPos;
	//		}
	//	}
	//}

	// Convert Quaternion
	switch( m_pBoneToAnimate->m_emBody )
	{
	case EMBODY_ROOT:
		ConvertMatToQuat( NULL );
		break;
	case EMBODY_DOWNBODY:
	case EMBODY_UPBODY:
		ConvertMatToQuat( m_pBoneToAnimate );
		break;
	};

	return S_OK;
}

HRESULT SAnimation::Load( sc::BaseStream &SFile, const DxSkeleton* pSkeleton )
{

	SFile >> m_strBone;

	SFile >> m_cRotateKeys;
	if ( m_cRotateKeys>0 )
	{
		m_pRotateKeys = new SRotateKey[m_cRotateKeys];
		SFile.ReadBuffer ( m_pRotateKeys, sizeof(SRotateKey)*m_cRotateKeys );
	}

	DWORD dwGabageValue;
	SFile >> dwGabageValue;
	
	SFile >> m_cPositionKeys;
	if ( m_cPositionKeys>0 )
	{
		m_pPositionKeys = new SPositionKey[m_cPositionKeys];
		SFile.ReadBuffer ( m_pPositionKeys, sizeof(SPositionKey)*m_cPositionKeys );
	}

	SFile >> m_cQuatPosKeys;
	if ( m_cQuatPosKeys>0 )
	{
		m_pQuatPosKeys = new SQuatPosKey[m_cQuatPosKeys];
		SFile.ReadBuffer ( m_pQuatPosKeys, sizeof(SQuatPosKey)*m_cQuatPosKeys );
	}

	SFile >> m_cMatrixKeys;
	if ( m_cMatrixKeys>0 )
	{
		m_pMatrixKeys = new SMatrixKey[m_cMatrixKeys];
		SFile.ReadBuffer ( m_pMatrixKeys, sizeof(SMatrixKey)*m_cMatrixKeys );

		// Optimize 
		m_fDivFullTime = 1.f/m_pMatrixKeys[m_cMatrixKeys-1].dwTime;
		m_fMatrixKeys = (float)m_cMatrixKeys;
	}

	SFile >> dwGabageValue;

	SFile >> m_cScaleKeys;
	if ( m_cScaleKeys>0 )
	{
		m_pScaleKeys = new SScaleKey[m_cScaleKeys];
		SFile.ReadBuffer ( m_pScaleKeys, sizeof(SScaleKey)*m_cScaleKeys );
	}

	//	적용될 BONE 과 연결.
	m_pBoneToAnimate = pSkeleton->FindBone_t( m_strBone.c_str() );

	// 본이 없음.
	if( !m_pBoneToAnimate )
	{
		return E_FAIL;
	}

	if ( m_pBoneToAnimate->m_dwFlag & USE_ANI_INVERSE_SCALE_ROOT )
	{
		for ( UINT i=0; i<m_cQuatPosKeys; ++i )
		{
			if ( m_pQuatPosKeys[i].m_vScale.x < 0.f )
			{
				m_pQuatPosKeys[i].m_vScale = -m_pQuatPosKeys[i].m_vScale;
			}
		}
	}
	//else if ( m_pBoneToAnimate->m_dwFlag & USE_ANI_INVERSE_SCALE_CHILE )
	//{
	//	for ( UINT i=0; i<m_cQuatPosKeys; ++i )
	//	{
	//		if ( m_pQuatPosKeys[i].m_vPos.x < 0.f )
	//		{
	//			m_pQuatPosKeys[i].m_vPos = -m_pQuatPosKeys[i].m_vPos;
	//		}
	//	}
	//}

	// Convert Quaternion
	switch( m_pBoneToAnimate->m_emBody )
	{
	case EMBODY_ROOT:
		ConvertMatToQuat( NULL );
		break;
	case EMBODY_DOWNBODY:
	case EMBODY_UPBODY:
		ConvertMatToQuat( m_pBoneToAnimate );
		break;
	};

	return S_OK;
}

void SAnimation::ReAttachBoneForEdit(DxSkeleton* pSkeleton)
{
    if (pSkeleton == 0)
    {
        return;
    }

    //	적용될 BONE 과 연결.
    m_pBoneToAnimate = pSkeleton->FindBone_t( m_strBone.c_str() );

    // 본이 없음.
    if( !m_pBoneToAnimate )
    {
        return ;
    }

    // Convert Quaternion
    switch( m_pBoneToAnimate->m_emBody )
    {
    case EMBODY_ROOT:
        ConvertMatToQuat( NULL );
        break;
    case EMBODY_DOWNBODY:
    case EMBODY_UPBODY:
        ConvertMatToQuat( m_pBoneToAnimate );
        break;
    };
}

//HRESULT DxSkinAnimation::SaveToFile ()
//{
//	CString strBinFile;
//	STRUTIL::ChangeExt(m_Name.c_str(), ".x", strBinFile, ".bin");
//
//	TSTRING strPath;
//	strPath += DxSkinAnimationManager::GetInstance().GetPath ();
//	strPath += strBinFile.GetString();
//
//	sc::SerialFile SFile;
//	SFile.SetFileType ( szFILETYPE, VERSION );
//
//	if ( !SFile.OpenFile ( FOT_WRITE, strPath.c_str() ) )		return E_FAIL;
//
//	SFile << (DWORD)m_listAniUPBODY.size();
//	
//	SANILIST_ITER iter = m_listAniUPBODY.begin();
//	SANILIST_ITER iter_end = m_listAniUPBODY.end();
//	for ( ; iter!=iter_end; ++iter )
//	{
//		(*iter)->SaveToFile(SFile);
//	}
//
//	// 쓰레기값
//	DWORD dwGabageData = m_listAniUPBODY.size() + m_listAnimation.size() + VERSION;
//	SFile << dwGabageData;
//
//	SFile << (DWORD)m_listAnimation.size();
//	
//	iter = m_listAnimation.begin();
//	iter_end = m_listAnimation.end();
//	for ( ; iter!=iter_end; ++iter )
//	{
//		(*iter)->SaveToFile(SFile);
//	}
//
//	return S_OK;
//}
//
//HRESULT DxSkinAnimation::SetInfo ( SANIMCONINFO &sAnimInfo, LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	//	Note : INFO 저장.
//	SANIMCONINFO::operator = ( sAnimInfo );
//
//	m_pSkeleton = DxSkeletonManager::GetInstance().Load(m_SkeletionName, pd3dDevice);
//	if (!m_pSkeleton)
//        return E_FAIL;
//
//	return S_OK;
//}
//
//HRESULT DxSkinAnimation::LoadAnimFromFile ( const TSTRING &strFilePath )
//{
//	sc::SerialFile SFile;
//	if ( !SFile.OpenFile ( FOT_READ, strFilePath.c_str() ) )		return E_FAIL;
//
//	TCHAR szFileType[FILETYPESIZE] = "";
//	DWORD dwVersion;
//	SFile.GetFileType( szFileType, FILETYPESIZE, dwVersion );
//
//	switch ( dwVersion )
//	{
//	case 0x0100:		Load_0100( SFile );	break;
//	case 0x0101:		Load_0101( SFile );	break;
//	case 0x0102:		Load_0102( SFile );	break;
//	case VERSION:		Load( SFile );		break;
//	default:			return E_FAIL;
//	}
//
//	return S_OK;
//}
//
//HRESULT  DxSkinAnimation::Load_0100( sc::SerialFile &SFile )
//{
//	SANILIST listANI;
//
//	DWORD dwNumAni(0);
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, m_pSkeleton );
//
//		listANI.push_back ( pAnimation );
//	}
//
//	//	로드한 값을 설정.
//	std::for_each(m_listAnimation.begin(),m_listAnimation.end(),std_afunc::DeleteObject());
//	m_listAnimation = listANI;
//
//
//	std::for_each(m_listAniUPBODY.begin(),m_listAniUPBODY.end(),std_afunc::DeleteObject());
//	m_listAniUPBODY.clear();
//
//	m_bVALID = true;
//
//	return S_OK;
//}
//HRESULT  DxSkinAnimation::Load_0101( sc::SerialFile &SFile )
//{
//	SANILIST listANI;
//
//	DWORD dwNumAni(0);
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, m_pSkeleton );
//
//		listANI.push_back ( pAnimation );
//	}
//
//	//	로드한 값을 설정.
//	std::for_each(m_listAnimation.begin(),m_listAnimation.end(),std_afunc::DeleteObject());
//	m_listAnimation = listANI;
//
//
//	std::for_each(m_listAniUPBODY.begin(),m_listAniUPBODY.end(),std_afunc::DeleteObject());
//	m_listAniUPBODY.clear();
//
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, m_pSkeleton );
//
//		m_listAniUPBODY.push_back ( pAnimation );
//	}
//	
//	//	data 유효성 on.
//	m_bVALID = true;
//
//	return S_OK;
//}
//
//HRESULT  DxSkinAnimation::Load_0102( sc::SerialFile &SFile )
//{
//	SANILIST listANI;
//	DWORD dwNumAni(0);
//
//	std::for_each(m_listAniUPBODY.begin(),m_listAniUPBODY.end(),std_afunc::DeleteObject());
//	m_listAniUPBODY.clear();
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, m_pSkeleton );
//
//		m_listAniUPBODY.push_back ( pAnimation );
//	}
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, m_pSkeleton );
//
//		listANI.push_back ( pAnimation );
//	}
//
//	//	로드한 값을 설정.
//	std::for_each(m_listAnimation.begin(),m_listAnimation.end(),std_afunc::DeleteObject());
//	m_listAnimation = listANI;
//	
//	//	data 유효성 on.
//	m_bVALID = true;
//
//	return S_OK;
//}
//
//HRESULT  DxSkinAnimation::Load( sc::SerialFile &SFile )
//{
//	SANILIST listANI;
//	DWORD dwNumAni(0);
//
//	std::for_each(m_listAniUPBODY.begin(),m_listAniUPBODY.end(),std_afunc::DeleteObject());
//	m_listAniUPBODY.clear();
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, m_pSkeleton );
//
//		m_listAniUPBODY.push_back ( pAnimation );
//	}
//
//	DWORD dwGarbage;
//	SFile >> dwGarbage;
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, m_pSkeleton );
//
//		listANI.push_back ( pAnimation );
//	}
//
//	//	로드한 값을 설정.
//	std::for_each(m_listAnimation.begin(),m_listAnimation.end(),std_afunc::DeleteObject());
//	m_listAnimation = listANI;
//	
//	//	data 유효성 on.
//	m_bVALID = true;
//
//	return S_OK;
//}
//
//void DxSkinAnimation::SetAnimFromList ( SANILIST &listANI, SANILIST &listANIUPBODY )
//{
//	//	종전 정보 삭제.
//	std::for_each(m_listAnimation.begin(),m_listAnimation.end(),std_afunc::DeleteObject());
//	std::for_each(m_listAniUPBODY.begin(),m_listAniUPBODY.end(),std_afunc::DeleteObject());
//	m_listAniUPBODY.clear();
//
//	//	로드한 값을 설정.
//	m_listAnimation = listANI;
//	m_listAniUPBODY = listANIUPBODY;
//	
//	//	data 유효성 on.
//	m_bVALID = true;
//}
//
//HRESULT DxSkinAnimation::LoadAnimFromFile ( const TSTRING &strFilePath, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY )
//{
//	sc::SerialFile SFile;
//	if( strFilePath.empty() )										return E_FAIL;
//	if ( !SFile.OpenFile ( FOT_READ, strFilePath.c_str() ) )		return E_FAIL;
//
//	TCHAR szFileType[FILETYPESIZE] = _T("");
//	DWORD dwVersion;
//	SFile.GetFileType( szFileType, FILETYPESIZE, dwVersion );
//
//	switch ( dwVersion )
//	{
//	case 0x0100:		Load_0100( SFile, pSkeleton, listANI, listANIUPBODY );	break;
//	case 0x0101:		Load_0101( SFile, pSkeleton, listANI, listANIUPBODY );	break;
//	case 0x0102:		Load_0102( SFile, pSkeleton, listANI, listANIUPBODY );	break;
//	case VERSION:		Load( SFile, pSkeleton, listANI, listANIUPBODY );		break;
//	default:			return E_FAIL;
//	}
//	
//	return S_OK;
//}
//
//HRESULT  DxSkinAnimation::Load_0100( sc::SerialFile &SFile, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY )
//{
//	DWORD dwNumAni(0);
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, pSkeleton );
//		listANI.push_back ( pAnimation );
//	}
//
//	return S_OK;
//}
//
//HRESULT  DxSkinAnimation::Load_0101( sc::SerialFile &SFile, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY )
//{
//
//	DWORD dwNumAni(0);
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, pSkeleton );
//		listANI.push_back ( pAnimation );
//	}
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, pSkeleton );
//		listANIUPBODY.push_back ( pAnimation );
//	}
//
//	return S_OK;
//}
//
//HRESULT  DxSkinAnimation::Load_0102( sc::SerialFile &SFile, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY )
//{
//	DWORD dwNumAni(0);
//	
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, pSkeleton );
//		listANIUPBODY.push_back ( pAnimation );
//	}
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, pSkeleton );
//		listANI.push_back ( pAnimation );
//	}
//
//	return S_OK;
//}
//
//HRESULT  DxSkinAnimation::Load( sc::SerialFile &SFile, const DxSkeleton* pSkeleton, SANILIST &listANI, SANILIST &listANIUPBODY )
//{
//	DWORD dwNumAni(0);
//	
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, pSkeleton );
//		listANIUPBODY.push_back ( pAnimation );
//	}
//
//	DWORD dwGarbage;
//	SFile >> dwGarbage;
//
//	SFile >> dwNumAni;
//	for ( DWORD i=0; i<dwNumAni; ++i )
//	{
//		SAnimation *pAnimation = new SAnimation;
//		pAnimation->LoadFromFile(SFile, pSkeleton );
//		listANI.push_back ( pAnimation );
//	}
//
//	return S_OK;
//}



//	Note : 에니메이션을 읽어옴.