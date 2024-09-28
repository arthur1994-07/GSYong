#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxTools/DebugSet.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/COLLISION.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxLand/DxLandMan.h"

#include "./DxSkinAniControlThreadNeedData.h"
#include "./DxVehicleThreadNeedData.h"
#include "./DxSkeletonMaintain.h"
#include "./NSSkinAniThread.h"
#include "./DxSkeletonMaintain.h"
#include "./DxSkinMeshManager.h"
#include "./DxSkinDefine.h"
#include "./DxJiggleBoneColl.h"
#include "./DxJiggleBone.h"
#include "./DxClothColl.h"
#include "./DxFootColl.h"

#include "DxVehicle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL	g_bCHAR_EDIT_RUN;

DWORD	DxVehicle::g_dwLOD = 0;
DWORD	DxVehicle::g_dwHIGHDRAW_NUM = 0;


DxVehicle::DxVehicle(void)
	: m_fScale(1.0f)
	, m_fHeight(20.f)
	, m_bWorldObj(FALSE)
	, m_dwVertexNUM(0)
	, m_bCharAmbientUP(FALSE)
	, m_bAmbientColor(FALSE)
	, m_vDiffuseColor(1.f,1.f,1.f)
	, m_spSkinAniControlThread(new DxSkinAniControl)
	, m_bCompleted_PartLoad_FirstCreate(FALSE)
	, m_bPhysX_Cloth_OnOff(FALSE)
	, m_bCompleted_PhysX_Cloth(FALSE)
{
	memset( m_szFileName, 0, FILE_LENGTH );

	for ( DWORD i=0; i<PART_V_SIZE; ++i )
	{
		m_p_V_AttBoneArray[i] = NULL;
	}

	D3DXMatrixIdentity( &m_matWorld );
	m_spThreadNeedData = std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>(new DxSkinAniControlThreadNeedData(PART_V_SIZE));
	m_spVehicleThreadNeedData = std::tr1::shared_ptr<DxVehicleThreadNeedData>(new DxVehicleThreadNeedData);
}

DxVehicle::~DxVehicle(void)
{
	m_bCompleted_PhysX_Cloth = FALSE;

	for ( DWORD i=0; i<PART_V_SIZE; ++i )
	{
		if ( !m_p_V_AttBoneArray[i] )
			continue;

		m_p_V_AttBoneArray[i]->ClearAll();
		SAFE_DELETE ( m_p_V_AttBoneArray[i] );
	}
}


HRESULT DxVehicle::ClearAll ()
{
	m_fScale = 1.0f;
	m_bWorldObj = FALSE;
	m_bCompleted_PhysX_Cloth = FALSE;

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		//	Note : 만약 이미 읽은 Vehicle Part Type 에 지정된 Piece 가 있다면 지워준다.
		//
		if ( m_V_PartsArray[i].GetFileName() )
		{
			m_V_PartsArray[i].ReSetPart ();
		}

		if ( m_p_V_AttBoneArray[i] && m_p_V_AttBoneArray[i]->GetFileName() )
		{
			m_p_V_AttBoneArray[i]->ClearAll();
		}
	}

	m_spSkinAniControlThread->ClearAll ();

	return S_OK;
}

PDXCHARPART DxVehicle::GetVehiclePart(int i)
{
    if (i<0 || i>= PART_V_SIZE)
        return NULL;
    else
        return &m_V_PartsArray[i];
}

PDXATTBONELINK DxVehicle::GetAttachBone(int i)
{
    if (i<0 || i>= PART_V_SIZE)
        return NULL;
    else
        return m_p_V_AttBoneArray[i];
}

void DxVehicle::ResetVehiclePart ( int i )
{
	GASSERT(i<PART_V_SIZE);
    if (i<0 || i>= PART_V_SIZE)
        return;

	//	Note : 이전에 올려진 Part 가 있다면 이를 내려준다.
	//
	if ( m_V_PartsArray[i].GetFileName() )
	{
		m_V_PartsArray[i].ReSetPart ();
	}

	if ( m_p_V_AttBoneArray[i] && m_p_V_AttBoneArray[i]->GetFileName() )
	{
		m_p_V_AttBoneArray[i]->ClearAll();
	}
}

void DxVehicle::ResetVehicle()
{
	ClearAll();

	//if ( m_szFileName[0]!='\0' )
	//	DxVehicleDataContainer::GetInstance().ReleaseVehicleData ( m_szFileName );
	//m_szFileName[0] = '\0';
}

HRESULT DxVehicle::SetPart( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag, int nLevel, EMSCD_FLAGS emFlags, bool bCharacterData, BOOL bAttachSub )
{
	std::string strFile(szFile);

	char ext[_MAX_EXT] = {0};
	_splitpath( strFile.c_str(), NULL, NULL, NULL, ext );

	CString strExt(ext);
	strExt.MakeUpper();

	// 만일 로드한 파일이 Attach Bone Link 파일이면...
	if( strExt == _T(".ABL") )
	{
		////	Note : 만약 처음 로드된 것을 무시하고 새로이 로드할때에는 원래 로드된걸 지워준다.
		////
		//if ( dwFlag&FG_MUSTNEWLOAD )
		//{
		//	for ( int i=0; i<PART_V_SIZE; ++i )
		//	{
		//		if ( m_p_V_AttBoneArray[i] && m_p_V_AttBoneArray[i]->GetFileName() )
		//		{
		//			if ( !_tcscmp(m_p_V_AttBoneArray[i]->GetFileName(),strFile.c_str()) )
		//			{
		//				m_p_V_AttBoneArray[i]->ClearAll();
		//			}
		//		}
		//	}

		//	DxAttBoneLinkContainer::GetInstance().DeleteLink ( strFile.c_str() );
		//}

		//	Note : Attach Bone을 읽어온다.
		//
		DxAttBoneLink* pAttBoneLink = DxAttBoneLinkContainer::GetInstance().LoadLink( strFile, emFlags );
		if (!pAttBoneLink)
			return E_FAIL;

		// [shhan][2015.06.08] Link 가 아닌 Skin 일 때만 해야 하고 ThreadLoading 이기 때문에
		//						DxCharPart::SetPart_Detail 타이밍에서 확인하는 것으로 변경되었다.
		//
		//// [shhan][2014.11.26] Bone 정보가 다를 경우 절대 Part 를 넣으면 안된다.
		////						DxSkeletonMaintain::FindBone_Index 에서 NULL 이 리턴되면서 문제가 생길 수 있다.
		////
		////if ( dwFlag&FG_BONEFILE_CHECK )
		//{
		//	CString strFile1( pAttBoneLink->m_strBaseBoneName.c_str() );
		//	strFile1.MakeLower();

		//	CString strFile2( m_spSkinAniControlThread->GetSkeletonName() );
		//	strFile2.MakeLower();

		//	if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
		//	{
		//		CDebugSet::ToLogFile
		//		( 
		//			sc::string::format( _T("Base Bone : %1%, Error cps : %2%, Error Bone : %3% \n"), m_spSkinAniControlThread->GetSkeletonName(), strFile, pAttBoneLink->m_strBaseBoneName.c_str() ) 
		//		);

		//		return E_DIFFERENT_BONEFILE;
		//	}
		//}

		//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
		//
		if ( m_p_V_AttBoneArray[pAttBoneLink->m_emPieceCharType] )
		{
			if ( m_p_V_AttBoneArray[pAttBoneLink->m_emPieceCharType]->GetFileName() )
			{
				m_p_V_AttBoneArray[pAttBoneLink->m_emPieceCharType]->ClearAll();
			}

			m_p_V_AttBoneArray[pAttBoneLink->m_emPieceCharType]->SetLink( pAttBoneLink, pd3dDevice, m_bCharAmbientUP, nLevel );
		}

		if( pAttBoneLink->m_bUseCPS )
		{
			SetPart( pAttBoneLink->m_strPieceName.c_str(), pd3dDevice, dwFlag, nLevel, emFlags, m_bCharAmbientUP, TRUE );
		}
	}
	else
	{
		////	Note : 만약 처음 로드된 것을 무시하고 새로이 로드할때에는 원래 로드된걸 지워준다.
		//if ( dwFlag&FG_MUSTNEWLOAD )
		//{
		//	for ( int i=0; i<PART_V_SIZE; ++i )
		//	{
		//		if ( m_V_PartsArray[i].GetFileName() )
		//		{
		//			if ( ! strFile.compare( m_V_PartsArray[i].GetFileName() ) )
		//			{
		//				m_V_PartsArray[i].ReSetPart();
		//			}
		//		}
		//	}

		//	DxSkinPieceContainer::GetInstance().DeletePiece( strFile );
		//}

		if ( strFile.empty() )
			return E_FAIL;

		//	Note : Piece를 읽어온다.
		DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( strFile, emFlags );
		if ( !pSkinPiece )
			return E_FAIL;

		// [shhan][2015.06.08] Link 가 아닌 Skin 일 때만 해야 하고 ThreadLoading 이기 때문에
		//						DxCharPart::SetPart_Detail 타이밍에서 확인하는 것으로 변경되었다.
		//
		//// [shhan][2014.11.26] Bone 정보가 다를 경우 절대 Part 를 넣으면 안된다.
		////						DxSkeletonMaintain::FindBone_Index 에서 NULL 이 리턴되면서 문제가 생길 수 있다.
		////
		////if ( dwFlag&FG_BONEFILE_CHECK )
		//{
		//	CString strFile1( pSkinPiece->m_szSkeleton );
		//	strFile1.MakeLower();

		//	CString strFile2( m_spSkinAniControlThread->GetSkeletonName() );
		//	strFile2.MakeLower();

		//	if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
		//	{
		//		CDebugSet::ToLogFile
		//		( 
		//			sc::string::format( _T("Base Bone : %1%, Error cps : %2%, Error Bone : %3% \n"), m_spSkinAniControlThread->GetSkeletonName(), strFile, pSkinPiece->m_szSkeleton ) 
		//		);

		//		return E_DIFFERENT_BONEFILE;
		//	}
		//}

		////	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
		////
		//if ( m_V_PartsArray[pSkinPiece->m_emV_PartType].GetFileName() )
		//{
		//	m_V_PartsArray[pSkinPiece->m_emV_PartType].ReSetPart();
		//}

		// cps 가 변경되면 같은 포지션의 AttBone 에 셋팅된 것을 지워줘야 한다.
		// 하지만 AttBone에서도 cps 를 강제 셋팅하는게 있기 때문에 AttBone일 경우는 AttBone 을 지워주진 않는다.
		if( !bAttachSub )
		{
			//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
			//
			if ( m_p_V_AttBoneArray[pSkinPiece->m_emV_PartType] && m_p_V_AttBoneArray[pSkinPiece->m_emV_PartType]->GetFileName() )
			{
				m_p_V_AttBoneArray[pSkinPiece->m_emV_PartType]->ClearAll();
			}
		}

		//	Note : Piece 를 지정.
		//
		m_V_PartsArray[pSkinPiece->m_emV_PartType].SetPart( this, pSkinPiece, pd3dDevice, m_bCharAmbientUP, NULL );
		m_V_PartsArray[pSkinPiece->m_emV_PartType].SetGrindLevel( nLevel );
		//m_V_PartsArray[pSkinPiece->m_emType].SetColorKey( lColorKey );
	}

	// 변경 파츠의 PhysX 정보 재 셋팅을 해야 한다라고 셋팅한다.
	m_bCompleted_PhysX_Cloth = FALSE;


	// [ Thread Work shhan ]
	// return S_OK;

//	char ext[_MAX_EXT] = {0};
//	_splitpath( szFile, NULL, NULL, NULL, ext );
//
//	CString strExt(ext);
//	strExt.MakeUpper();
//
//	// 만일 로드한 파일이 Attach Bone Link 파일이면...
//	if( strExt == _T(".ABL") )
//	{
//		//	Note : 만약 처음 로드된 것을 무시하고 새로이 로드할때에는 원래 로드된걸 지워준다.
//		//
//		if ( dwFlag&FG_MUSTNEWLOAD )
//		{
//			for ( int i=0; i<PART_V_SIZE; ++i )
//			{
//				if ( m_p_V_AttBoneArray[i].GetFileName() )
//				{
//					if ( !_tcscmp(m_p_V_AttBoneArray[i].GetFileName(),szFile) )
//					{
//						m_p_V_AttBoneArray[i].ClearAll();
//					}
//				}
//			}
//
//			DxAttBoneLinkContainer::GetInstance().DeleteLink ( szFile );
//		}
//
//		//	Note : Attach Bone을 읽어온다.
//		//
//        DxAttBoneLink* pAttBoneLink = DxAttBoneLinkContainer::GetInstance().LoadLink(std::string(szFile), pd3dDevice, TRUE);
//		if (!pAttBoneLink)
//            return E_FAIL;
//
//		if ( dwFlag&FG_BONEFILE_CHECK )
//		{
//			if ( _tcscmp(pAttBoneLink->m_strBaseBoneName.c_str(),m_strSkeleton.c_str()) )	return E_DIFFERENT_BONEFILE;
//		}
//
//		//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
//		//
//		if ( m_p_V_AttBoneArray[pAttBoneLink->m_emPieceCharType].GetFileName() )
//		{
//			m_p_V_AttBoneArray[pAttBoneLink->m_emPieceCharType].ClearAll();
//		}
//
//		m_p_V_AttBoneArray[pAttBoneLink->m_emPieceCharType].SetLink ( pAttBoneLink, pd3dDevice, bCharacterData );
//
//		if( pAttBoneLink->m_bUseCPS )
//		{
//			SetPart( pAttBoneLink->m_strPieceName.c_str(), pd3dDevice, dwFlag, nLevel, bCharacterData, TRUE );
//		}
//
//	}else{
//
//		//	Note : 만약 처음 로드된 것을 무시하고 새로이 로드할때에는 원래 로드된걸 지워준다.
//		//
//		if ( dwFlag&FG_MUSTNEWLOAD )
//		{
//			for ( int i=0; i<PART_V_SIZE; ++i )
//			{
//				if ( m_V_PartsArray[i].GetFileName() )
//				{
//					if ( !_tcscmp(m_V_PartsArray[i].GetFileName(),szFile) )
//					{
//						m_V_PartsArray[i].ReSetPart ();
//					}
//				}
//			}
//
//			DxSkinPieceContainer::GetInstance().DeletePiece ( szFile );
//		}
//
//		//	Note : Piece를 읽어온다.
//		//
//		DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( szFile, pd3dDevice, TRUE );
//		if ( !pSkinPiece )	return E_FAIL;
//
//		if ( dwFlag&FG_BONEFILE_CHECK )
//		{
//			if ( _tcscmp(pSkinPiece->m_szSkeleton,m_strSkeleton.c_str()) )	return E_DIFFERENT_BONEFILE;
//		}
//
//
//		//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
//		//
//		if ( m_V_PartsArray[pSkinPiece->m_emV_PartType].GetFileName() )
//		{
//			m_V_PartsArray[pSkinPiece->m_emV_PartType].ReSetPart ();
//		}
//
//		//// Note : 모자에 셋팅이 이상하게 되어서 이렇게 해야 한다.
//		if( pSkinPiece->m_emV_PartType == PART_V_BODY )
//		{
//			if( m_V_PartsArray[PART_V_BODY].GetFileName() )
//			{
//				// 일단 정리
//				m_V_PartsArray[PART_V_BODY].ReSetPart ();
//
//				// 데이터 셋팅
//				DxSkinPiece* pSkinPieceHG = DxSkinPieceContainer::GetInstance().LoadPiece( m_V_PartsArray[PART_V_BODY].GetFileName(), pd3dDevice, TRUE );
//				if ( !pSkinPieceHG )	goto _RETURN;
//				m_V_PartsArray[PART_V_BODY].SetPart( pSkinPieceHG, pd3dDevice, bCharacterData );
//			}
//		}
//		_RETURN:
//
//		//	Note : Piece 를 지정.
//		//
//		m_V_PartsArray[pSkinPiece->m_emV_PartType].SetPart ( pSkinPiece, pd3dDevice, bCharacterData );
//		m_V_PartsArray[pSkinPiece->m_emV_PartType].SetGrindLevel( nLevel );
//	}
//
//
//// Note : 정점 데이터 재 수집
//CollectVertexNUM();

return S_OK;
}

//void DxVehicle::UpdatePassengerPos()
//{
//	if( !m_spSkinAniControlThread->IsValid_Skeleton() )
//		return;
//
//	D3DXMATRIX matScale, matRotate, matTrans, matCombinedScaleInv;
//
//	for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
//	{
//		if( m_sPassengerData[i].strBoneName.empty() ) 
//			continue;
//
//		D3DXMatrixScaling(&matScale, m_fScale, m_fScale, m_fScale);
//
//		D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_sPassengerData[i].vRotate.y),
//													D3DXToRadian(m_sPassengerData[i].vRotate.x), 
//													D3DXToRadian(m_sPassengerData[i].vRotate.z) );
//
//		D3DXMatrixTranslation( &matTrans, m_sPassengerData[i].vPos.x, m_sPassengerData[i].vPos.y, m_sPassengerData[i].vPos.z );
//
//		const DxBoneTransMaintain* pBoneTran = m_spSkinAniControlThread->GetSkeletonMaintain()->FindBone( m_sPassengerData[i].strBoneName.c_str() );
//
//		D3DXMatrixIdentity(&matCombinedScaleInv);
//		matCombinedScaleInv._11 /= m_fScale;
//		matCombinedScaleInv._22 /= m_fScale;
//		matCombinedScaleInv._33 /= m_fScale;
//
//		if( pBoneTran ) 
//			m_sPassengerData[i].matWorld = matRotate * matTrans * matCombinedScaleInv * pBoneTran->m_matCombined;
//	}
//}

void DxVehicle::SetVehiclePartData ( DxVehicleData* pVehiclePartData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP /* = FALSE */, EMSCD_FLAGS emFlags )
{
	// [shhan][2014.09.18]
	// [CRM #3013] 변신 기능으로 인해 DxSkinChar가 유지 되면서 DxSkinCharData 인자값이 다른것들이 넘어 올 수 있다. 그러면서 재 셋팅.
	//				타 쓰레드에서 참조하고 있을 수 있으므로 m_spSkinAniControlThread->ClearAll, m_spSkinAniControlThread->SetCharData 등.. 작업을 직접하지 말고 새로 생성 후 하자.
	//				형상이 바뀌는 경우는 캐릭터가 잠깐 안보이더라도 전부 정리를 해주는 방향으로 한다.
	{
		m_spSkinAniControlThread = std::tr1::shared_ptr<DxSkinAniControl>(new DxSkinAniControl);
		m_spThreadNeedData = std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>(new DxSkinAniControlThreadNeedData(PART_V_SIZE));

		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			//	Note : 만약 이미 읽은 Vehicle Part Type 에 지정된 Piece 가 있다면 지워준다.
			//
			if ( m_V_PartsArray[i].GetFileName() )
			{
				m_V_PartsArray[i].ReSetPart ();
			}

			if ( m_p_V_AttBoneArray[i] && m_p_V_AttBoneArray[i]->GetFileName() )
			{
				m_p_V_AttBoneArray[i]->ClearAll();
			}
		}
	}


	//TSTRING strFileName = pVehiclePartData->GetFileName();
	//if ( strFileName.empty() )
	//	return;

	BOOL bJiggleBone(FALSE);

	StringCchCopy( m_szFileName, FILE_LENGTH, pVehiclePartData->GetFileName() );

	m_smpFootColl = pVehiclePartData->GetFootColl();

	//EMANI_MAINTYPE MType = m_spSkinAniControlThread->GETCURMTYPE();
	//EMANI_SUBTYPE SType = m_spSkinAniControlThread->GETCURSTYPE();
	//if ( MType == AN_NONE )	MType = AN_GUARD_N;

	m_spSkinAniControlThread->ClearAll ();

	//float fCurTime = m_spSkinAniControlThread->GETCURTIME();

	m_bCharAmbientUP = bAmbientUP;
	m_fScale = pVehiclePartData->m_fScale;
	m_vMax = pVehiclePartData->m_vMax;
	m_vMin = pVehiclePartData->m_vMin;
	m_fHeight = pVehiclePartData->m_fHeight;


	m_bWorldObj = pVehiclePartData->m_bWorldObj;

	m_spSkinAniControlThread->SetCharData ( pVehiclePartData->GetSkinAniControlData(), pd3dDevice, m_fScale, m_szFileName );

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		DxSkinPieceBase* pSkinPieceBase = pVehiclePartData->GetVehiclePart( pd3dDevice, i, emFlags );
		if ( !pSkinPieceBase )
		{
			m_V_PartsArray[i].ReSetPart ();
			if ( m_p_V_AttBoneArray[i] )
			{
				m_p_V_AttBoneArray[i]->ClearAll();
			}
		}

		// [shhan][2015.03.25] 헤어 Color 를 초기화 해주도록 한다. ( CharEdit 에서 꼭 필요한 기능임 )
		//						해주지 않으면 이전 Color 가 남아있어서 cps 에서 셋팅한 값이 안나온다.
		m_V_PartsArray[i].SetHairColor( 0L, FALSE );
		m_V_PartsArray[i].SetSubColor( 0L, FALSE );
	}

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		DxSkinPieceBase* pSkinPieceBase = pVehiclePartData->GetVehiclePart( pd3dDevice, i, emFlags );
		if ( !pSkinPieceBase )
		{
			continue;
		}

		// 알맞는 Slot 찾기
		// cps 로 chf 를 만들어서 셋팅 후 그 cps의 셋팅 부위를 변경하고, chf 를 변경 안하면 변경된 셋팅부위가 아닌 이전 셋팅부위에 설정되는 문제가 발생함.
		// 그걸 보완하기 위해 추가함.
		DWORD dwSlot = pSkinPieceBase->GetVehicleSlot();

		if ( pSkinPieceBase->m_emSkinPieceType == DxSkinPieceBase::SKIN_PIECE )
		{
			m_V_PartsArray[dwSlot].SetPart ( this, MIN_BOOST::polymorphic_downcast<DxSkinPiece*>( pSkinPieceBase ), pd3dDevice, bAmbientUP, NULL );
		}

		if ( pSkinPieceBase->m_emSkinPieceType == DxSkinPieceBase::SKIN_ATT_BONE_LINK )
		{
			// [ Thread Work shhan ]
			//m_p_V_AttBoneArray[i].SetLink ( pCharData->GetAttachBone(i), pd3dDevice, bAmbientUP, bUserColor, static_cast<EMPIECECHAR>(i) );
			DxAttBoneLink* pNew = new DxAttBoneLink;
			pNew->SetLink ( MIN_BOOST::polymorphic_downcast<DxAttBoneLink*>( pSkinPieceBase ), pd3dDevice, bAmbientUP, NULL );
			SAFE_DELETE ( m_p_V_AttBoneArray[dwSlot] );
			m_p_V_AttBoneArray[dwSlot] = pNew;

			if( pNew->m_bUseCPS )
			{
				SetPart( pNew->m_strPieceName.c_str(), pd3dDevice, 0L, 0L, emFlags, FALSE, TRUE );
			}
		}
	}

	m_spVehicleThreadNeedData->Import( pVehiclePartData );

	// 변경 파츠의 PhysX 정보 재 셋팅을 해야 한다라고 셋팅한다.
	m_bCompleted_PhysX_Cloth = FALSE;
}

void DxVehicle::SetRadiusHeight( DxVehicleData* pVehiclePartData )
{
	m_vMin = pVehiclePartData->m_vMin;
	m_vMax = pVehiclePartData->m_vMax;
	m_fHeight = pVehiclePartData->m_fHeight;
}

void DxVehicle::ResetStrikeAttachBone()
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if (!pAnimContainer)
        return;

	const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PART_V_SIZE; ++i )
	{	
		if ( !m_p_V_AttBoneArray[i] )
			continue;

		if( m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			continue;
			
		m_p_V_AttBoneArray[i]->m_nOldAttSTEP = -1;
	}
}

void DxVehicle::UpdateStrikeAttachBone( const DWORD dwThisKey, const int nattSTEP )
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if( !pAnimContainer ) return;
	if ( nattSTEP >= pAnimContainer->m_wStrikeCount ) return;

	const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PART_V_SIZE; ++i )
	{	
		if ( !m_p_V_AttBoneArray[i] )
			continue;

		if( m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() )
            continue;

		PDXATTBONE pDxAttBone = m_p_V_AttBoneArray[i]->GetAttBone();
		if( !pDxAttBone )
            continue;
		if( !pAttachBoneAnimInfo[i].bUseStrikeAni )
            continue;
		if( static_cast<int>(pAttachBoneAnimInfo[i].vecStrikeAni.size()) < nattSTEP )
            continue;

		{
			SAttachBoneAnimBasicInfo sAttBasicInfo = pAttachBoneAnimInfo[i].vecStrikeAni[nattSTEP];

			DWORD dwStrikeKey = pAnimContainer->m_sStrikeEff[nattSTEP].m_dwFrame + (sAttBasicInfo.nStartAnim * m_spSkinAniControlThread->GETUNITTIME());

			if ( dwThisKey>=dwStrikeKey && m_p_V_AttBoneArray[i]->m_nOldAttSTEP != nattSTEP )
			{
				m_p_V_AttBoneArray[i]->m_nOldAttSTEP = nattSTEP;
				// 애니메이션 update
				m_p_V_AttBoneArray[i]->GetAttBone()->TOSTARTTIME();
				m_p_V_AttBoneArray[i]->SELECTANI( sAttBasicInfo.emAttAniType );			
			}
            else if ( m_p_V_AttBoneArray[i]->m_nOldAttSTEP == -1 || m_p_V_AttBoneArray[i]->GetAttBone()->ISENDANIM() )
			{
				// STAY 여야 할때			
				if( sAttBasicInfo.emAttAniType != AN_ATTBONE_STAY )
				{
					m_p_V_AttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );
				}
			}

			if( m_p_V_AttBoneArray[i]->GetAttBone()->ISENDANIM() )
			{
				if( sAttBasicInfo.emAttAniType != AN_ATTBONE_STAY )
				{
					m_p_V_AttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );
				}
			}

		}
	}
}

void DxVehicle::UpdateNonStrikeAttachBone()
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if( !pAnimContainer ) return;

	const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		if ( !m_p_V_AttBoneArray[i] )
			continue;

		// 프레임이 끝나면 무조건 STAY로
		if( m_p_V_AttBoneArray[i]->GetAttBone()->ISENDANIM() )
		{
			m_p_V_AttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );			
		}

		if( m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			continue;

		PDXATTBONE pDxAttBone = m_p_V_AttBoneArray[i]->GetAttBone();
		if( !pDxAttBone ) continue;
		if( pAttachBoneAnimInfo[i].bUseStrikeAni ) continue;

		SAttachBoneAnimBasicInfo sAttBasicInfo = pAttachBoneAnimInfo[i].sBasicAni;

		// 애니메이션을 바꾸어야 할때
		if( m_spSkinAniControlThread->GETCURKEYTIME() >= static_cast<DWORD>(sAttBasicInfo.nStartAnim) )
		{
			// 애니메이션 update
			if( sAttBasicInfo.emAttAniType != pDxAttBone->GETCURMTYPE() )
			{
				m_p_V_AttBoneArray[i]->SELECTANI( sAttBasicInfo.emAttAniType );
			}
		}
        else
        {
			// STAY 여야 할때			
			if( sAttBasicInfo.emAttAniType != AN_ATTBONE_STAY )
			{
				m_p_V_AttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );
			}
		}		
	}

}

void DxVehicle::SET_ATTBONE_CURTIME_ALL_FOR_EDIT( float fCurTime )
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if( !pAnimContainer ) 
		return;

	const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		if ( !m_p_V_AttBoneArray[i] )
			continue;

		if( m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			continue;

		PDXATTBONE pDxAttBone = m_p_V_AttBoneArray[i]->GetAttBone();
		if( !pDxAttBone ) continue;

		SAttachBoneAnimBasicInfo sAttBasicInfo = pAttachBoneAnimInfo[i].sBasicAni;

		// 애니메이션을 바꾸어야 할때
		if( m_spSkinAniControlThread->GETCURTIME() >= sAttBasicInfo.nStartAnim * m_spSkinAniControlThread->GETUNITTIME() )
		{
			pDxAttBone->SETCURTIME_ALL_FOR_EDIT( fCurTime - sAttBasicInfo.nStartAnim * m_spSkinAniControlThread->GETUNITTIME() );
		}else{
			pDxAttBone->SETCURTIME_ALL_FOR_EDIT( fCurTime );
		}
	}
}


HRESULT DxVehicle::FrameMove( const float fTime, const float fElapsedTime, BOOL bNoneOverFrame, const D3DXMATRIX &matRot, const BOOL bContinue/* =TRUE */, BOOL const bFreeze/* =FALSE */)
{
	if ( bFreeze )		m_spSkinAniControlThread->FrameMove_AniTime ( 0.0f, bContinue );
	else				m_spSkinAniControlThread->FrameMove_AniTime ( fElapsedTime, bContinue );

	UpdateNonStrikeAttachBone();

	m_spSkinAniControlThread->FrameMove_EffAniPlayer( this, m_sSkinEffData, fTime, fElapsedTime, bFreeze );

	// Note : Edge Color 디폴트 셋팅.
	m_EffKeepPlayer.FrameMove( fElapsedTime, m_sSkinEffData );

	//UpdatePassengerPos();

	//////////////////////////////////////////////////////////////////////////
	// Render 쪽에 있었던것들.
	D3DXMatrixIdentity( &m_matWorld );
	m_matWorld._11 = m_fScale;
	m_matWorld._22 = m_fScale;
	m_matWorld._33 = m_fScale;
	D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matRot );

	// TAG_ProcessAnimationThread_1
	// 등록만 한다.
	NSSkinAniThread::AddAniControl_Vehicle( m_spSkinAniControlThread, m_spThreadNeedData, m_spVehicleThreadNeedData, m_matWorld, bNoneOverFrame );
	//if ( !m_spSkinAniControlThread->Render( m_spSkinAniControlThread->GetWorld(), m_afKeyJiggleWeight, PART_V_SIZE ) )
	//	return S_OK;
	//////////////////////////////////////////////////////////////////////////


	m_sSkinEffData.Reset();		// Data Reset.
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].FrameMove ( fTime, fElapsedTime, m_spSkinAniControlThread, bNoneOverFrame );

		if ( m_p_V_AttBoneArray[i] )
		{
			m_p_V_AttBoneArray[i]->FrameMove( fTime, fElapsedTime, bNoneOverFrame, m_spSkinAniControlThread, m_spThreadNeedData->m_arrayKeyJiggleWeight[i], bContinue, bFreeze );
		}
	}

	// 이전 키등록을 제일 마지막에 해야한다.
	m_spSkinAniControlThread->SetPrevKeyFrame();

	return S_OK;
}

// 렌더시 필수 요소가 있는가~?
BOOL DxVehicle::IsValidForRender()
{
	// Thread 작업되면서 Bone이 없을 수 있다.
	if ( !m_spSkinAniControlThread->GetSkeletonMaintain()->IsActiveBone() )
		return FALSE;

	// [shhan][2014.10.02] 바이크 같은 경우는 애니메이션이 없더라도 렌더링을 해주도록 하자.
	//						팔 벌리는 그런게 있는게 아니니깐...
	//
	//// 캐릭터 툴이 아닐 때..
	//if ( !g_bCHAR_EDIT_RUN ) 
	//{
	//	// 기본 애니메이션이 있을 때 렌더링 한다.
	//	if ( !m_spSkinAniControlThread->IsValid_Skeleton_DefaultAni() )
	//		return FALSE;
	//}

	return TRUE;
}


HRESULT DxVehicle::RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, 
							   BOOL bPointLightRT,
							   float fAlpha,
							   const BOOL bShadow/* =FALSE */,
							   const BOOL bEff/* =TRUE */, 
							   const BOOL bReverse /* = FALSE */, 
							   const BOOL bBoardRender /* = FALSE */,
							   BOOL bFogON /* = TRUE */ )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return S_OK;

	g_dwLOD = 0;
#ifdef USE_SKINMESH_LOD
	// Note : 그림자일 경우는 저급으로 뿌린다.
	if( bShadow )
	{
		g_dwLOD = 1;
	}
	else if( g_dwHIGHDRAW_NUM > 10 )
	{
		// Note : 한계가 넘었을 경우 최소한 보장해 줘야 하는 애들을 위함
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 30.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+30.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}
	else
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 150.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+150.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}

	if( g_dwLOD==0 )
	{
		++g_dwHIGHDRAW_NUM;
	}
#endif

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 다 로딩되었는지 확인한다.
		BOOL bEnableNotYet(FALSE);
		for( DWORD i=0; i<PART_V_SIZE; ++i )
		{
			if( !m_V_PartsArray[i].IsCompletedLINK() )
			{
				bEnableNotYet = TRUE;
			}
		}

		if( !bEnableNotYet )
		{
			m_bCompleted_PartLoad_FirstCreate = TRUE;
		}
	}

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return S_OK;
	}


	int nStart=0, nEnd = 0, nAdd = 0;

	// 피스 순서를 거꾸로 그린다. ( 펫 전용 )
	if ( bReverse )
	{
		nStart = PART_V_SIZE;
		nEnd = -1;
		nAdd = -1;
	}
	else
	{
		nStart = 0;	
		nEnd = PART_V_SIZE;
		nAdd = 1;
	}

	int i = nStart;


	// Note : 조명의 변화는 받지 않는다.
	if( m_bWorldObj )
	{
		DWORD	dwNormalizeNormals;
		pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		{
			SKIN_SPEC_SSS_INFO sSkinInfo;

			int i = nStart;
			while ( i != nEnd )
			{
				m_V_PartsArray[i].Render ( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), NULL, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, bShadow, bEff );

				if ( m_p_V_AttBoneArray[i] )
				{
					m_p_V_AttBoneArray[i]->RenderFixed ( pd3dDevice, 1.f, bShadow, bEff, bReverse, m_spThreadNeedData->m_arrayKeyJiggleWeight[i] );
				}
				i += nAdd;
			}

			//i = nStart;
			//while ( i != nEnd )
			//{
			//	m_V_PartsArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
			//	i += nAdd;

			//}

			m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, fAlpha );
		}
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );


		//	Note : Glow 
		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			m_V_PartsArray[i].RenderGlow( pd3dDevice );
		}

		//	Note : Single Eff
		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			m_V_PartsArray[i].RenderEff( pd3dDevice, m_fScale, NULL );
		}

		return TRUE;
	}

	D3DLIGHTQ	sSrcLight00;
	D3DLIGHTQ	sDestLight00;

	pd3dDevice->GetLight ( 0, &sSrcLight00 );

	sDestLight00 = sSrcLight00;

	DWORD dwColorOP_0(0L);

	//if ( m_bAmbientColor )
	//{
	//	float fHalf(0.5f);
	//	sDestLight00.Diffuse.r = sSrcLight00.Diffuse.r * m_vDiffuseColor.x * fHalf;
	//	sDestLight00.Diffuse.g = sSrcLight00.Diffuse.g * m_vDiffuseColor.y * fHalf;
	//	sDestLight00.Diffuse.b = sSrcLight00.Diffuse.b * m_vDiffuseColor.z * fHalf;
	//	sDestLight00.Ambient.r = m_vDiffuseColor.x * fHalf;
	//	sDestLight00.Ambient.g = m_vDiffuseColor.y * fHalf;
	//	sDestLight00.Ambient.b = m_vDiffuseColor.z * fHalf;

	//	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwColorOP_0 );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
	//}
	//else
	{
		sDestLight00.Ambient.r += sSrcLight00.Diffuse.r * 0.2f;
		sDestLight00.Ambient.g += sSrcLight00.Diffuse.g * 0.2f;
		sDestLight00.Ambient.b += sSrcLight00.Diffuse.b * 0.2f;
		sDestLight00.Ambient.r += sSrcLight00.Ambient.r * 0.2f;
		sDestLight00.Ambient.g += sSrcLight00.Ambient.g * 0.2f;
		sDestLight00.Ambient.b += sSrcLight00.Ambient.b * 0.2f;
		sDestLight00.Diffuse.r = sSrcLight00.Diffuse.r * 0.6f;
		sDestLight00.Diffuse.g = sSrcLight00.Diffuse.g * 0.6f;
		sDestLight00.Diffuse.b = sSrcLight00.Diffuse.b * 0.6f;
	}

	//sDestLight01.Ambient.r += sSrcLight01.Diffuse.r * 0.15f;
	//sDestLight01.Ambient.g += sSrcLight01.Diffuse.g * 0.15f;
	//sDestLight01.Ambient.b += sSrcLight01.Diffuse.b * 0.15f;
	//sDestLight01.Diffuse.r = sSrcLight01.Diffuse.r * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	//sDestLight01.Diffuse.g = sSrcLight01.Diffuse.g * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	//sDestLight01.Diffuse.b = sSrcLight01.Diffuse.b * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.

	//sDestLight02.Ambient.r += sSrcLight02.Diffuse.r * 0.15f;
	//sDestLight02.Ambient.g += sSrcLight02.Diffuse.g * 0.15f;
	//sDestLight02.Ambient.b += sSrcLight02.Diffuse.b * 0.15f;
	//sDestLight02.Diffuse.r = sSrcLight02.Diffuse.r * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	//sDestLight02.Diffuse.g = sSrcLight02.Diffuse.g * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	//sDestLight02.Diffuse.b = sSrcLight02.Diffuse.b * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.

	pd3dDevice->SetLight ( 0, &sDestLight00 );

	pd3dDevice->LightEnable( 1, FALSE );
	pd3dDevice->LightEnable( 2, FALSE );
	pd3dDevice->LightEnable( 3, FALSE );
	pd3dDevice->LightEnable( 4, FALSE );
	pd3dDevice->LightEnable( 5, FALSE );
	pd3dDevice->LightEnable( 6, FALSE );
	pd3dDevice->LightEnable( 7, FALSE );

	D3DXVECTOR3 vCharPos( m_spSkinAniControlThread->GetWorld()._41, m_spSkinAniControlThread->GetWorld()._42, m_spSkinAniControlThread->GetWorld()._43 );
	DxLightMan::GetInstance()->SetCharPLight( pd3dDevice, vCharPos, FALSE );	//m_bAmbientColor );

	DWORD	dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD	dwNormalizeNormals;
	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );


	{
		{			
			SKIN_SPEC_SSS_INFO sSkinInfo;

			switch( m_sSkinEffData.m_bNewHeadDRAW )
			{
			case TRUE:

				i = nStart;
				while ( i != nEnd )
				{
					m_V_PartsArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), NULL, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, bShadow, bEff );
					if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
						m_p_V_AttBoneArray[i]->RenderFixed ( pd3dDevice, 1.f, bShadow, bEff, bReverse, m_spThreadNeedData->m_arrayKeyJiggleWeight[i] );
					i += nAdd;
				}	
				break;

			case FALSE:
				i = nStart;
				while ( i != nEnd )
				{
					m_V_PartsArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), NULL, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, bShadow, bEff );
					if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
						m_p_V_AttBoneArray[i]->RenderFixed ( pd3dDevice, 1.f, bShadow, bEff, bReverse, m_spThreadNeedData->m_arrayKeyJiggleWeight[i] );
					i += nAdd;
				}
				break;
			};

			//for ( int i=0; i<PART_V_SIZE; ++i )
			//{
			//	m_V_PartsArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
			//}
			m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, fAlpha );
			m_EffKeepPlayer.Render( pd3dDevice );

			pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
		}

		//	Note : Glow 
		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			m_V_PartsArray[i].RenderGlow( pd3dDevice );
		}

		//	Note : Single Eff
		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			m_V_PartsArray[i].RenderEff( pd3dDevice, m_fScale, NULL );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	pd3dDevice->SetLight ( 0, &sSrcLight00 );

	pd3dDevice->LightEnable( 1, FALSE );
	pd3dDevice->LightEnable( 2, FALSE );
	pd3dDevice->LightEnable( 3, FALSE );
	pd3dDevice->LightEnable( 4, FALSE );
	pd3dDevice->LightEnable( 5, FALSE );



	return TRUE;

	//EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );

	//float	fDirectDiffuseFact = 0.6f;	//0.7f;	// Item 때문에 색을 낮춘다.
	//float	fDirectAmbientFact = 0.25f;	//0.3f;	// Item 때문에 색을 낮춘다.

	////	낮 일 경우와. 지형그림자가 켜져 있을 경우만 <지형에 따라 캐릭터를 어둡게> 한다.
	//if ( GLPeriod::GetInstance().IsOffLight() && DxEffectMan::GetInstance().GetSSDetail() )
	//{
	//	DxLandMan*	pLandMan = DxEffectMan::GetInstance().GetLandMan();
	//	if ( pLandMan )
	//	{
	//		D3DXVECTOR3	vDirect;
	//		vDirect = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
	//		vDirect.x *= -10000.f;
	//		vDirect.y *= -10000.f;
	//		vDirect.z *= -10000.f;

	//		BOOL		bCollision;
	//		D3DXVECTOR3	vCollision;
	//		D3DXVECTOR3	vPoint1(m_spSkinAniControlThread->GetWorld()._41+vDirect.x,m_spSkinAniControlThread->GetWorld()._42+8.f+vDirect.y,m_spSkinAniControlThread->GetWorld()._43+vDirect.z);
	//		D3DXVECTOR3	vPoint2(m_spSkinAniControlThread->GetWorld()._41,m_spSkinAniControlThread->GetWorld()._42+8.f,m_spSkinAniControlThread->GetWorld()._43);
	//		LPDXFRAME	pDxFrame = NULL;

	//		pLandMan->IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame );

	//		if ( bCollision )	// 무언가에 닿았다면... 어둡게 만든다.
	//		{
	//			fDirectDiffuseFact *= 0.7f;
	//			fDirectAmbientFact *= 0.7f;
	//		}
	//	}
	//}
}
//
//HRESULT DxVehicle::RenderSKIN( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& m_spSkinAniControlThread->GetWorld() )
//{
//	// Thread 작업되면서 Bone이 없을 수 있다.
//	if ( !m_spSkinAniControlThread->GetSkeletonMaintain()->IsActiveBone() )
//		return S_OK;
//
//	D3DLIGHTQ	sSrcLight00;
//	D3DLIGHTQ	sDestLight00;
//
//	pd3dDevice->GetLight ( 0, &sSrcLight00 );
//
//	sDestLight00 = sSrcLight00;
//
//	sDestLight00.Ambient.r += sSrcLight00.Diffuse.r * 0.2f;
//	sDestLight00.Ambient.g += sSrcLight00.Diffuse.g * 0.2f;
//	sDestLight00.Ambient.b += sSrcLight00.Diffuse.b * 0.2f;
//	sDestLight00.Ambient.r += sSrcLight00.Ambient.r * 0.2f;
//	sDestLight00.Ambient.g += sSrcLight00.Ambient.g * 0.2f;
//	sDestLight00.Ambient.b += sSrcLight00.Ambient.b * 0.2f;
//	sDestLight00.Diffuse.r = sSrcLight00.Diffuse.r * 0.6f;
//	sDestLight00.Diffuse.g = sSrcLight00.Diffuse.g * 0.6f;
//	sDestLight00.Diffuse.b = sSrcLight00.Diffuse.b * 0.6f;
//
//	pd3dDevice->SetLight ( 0, &sDestLight00 );
//
//	pd3dDevice->LightEnable( 1, FALSE );
//	pd3dDevice->LightEnable( 2, FALSE );
//	pd3dDevice->LightEnable( 3, FALSE );
//	pd3dDevice->LightEnable( 4, FALSE );
//	pd3dDevice->LightEnable( 5, FALSE );
//	pd3dDevice->LightEnable( 6, FALSE );
//	pd3dDevice->LightEnable( 7, FALSE );
//
//	D3DXVECTOR3 vCharPos( m_spSkinAniControlThread->GetWorld()._41, m_spSkinAniControlThread->GetWorld()._42, m_spSkinAniControlThread->GetWorld()._43 );
//	DxLightMan::GetInstance()->SetCharPLight( pd3dDevice, vCharPos, m_bAmbientColor );
//
//	DWORD	dwFogEnable;
//	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
//
//	{
//		DWORD	dwNormalizeNormals;
//		pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
//		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
//
//
//		SKIN_SPEC_SSS_INFO sSkinInfo;
//
//		switch( m_sSkinEffData.m_bNewHeadDRAW )
//		{
//		case TRUE:
//			for ( int i=1; i<PART_V_SIZE; ++i )
//			{
//				m_V_PartsArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, FALSE, TRUE );
//			}
//			break;
//
//		case FALSE:
//			for ( int i=0; i<PART_V_SIZE; ++i )
//			{
//				m_V_PartsArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, FALSE, TRUE );
//			}
//			break;
//		};
//
//		//for ( int i=0; i<PART_V_SIZE; ++i )
//		//{
//		//	m_V_PartsArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
//		//}
//		m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, m_spSkinAniControlThread->GetWorld(), 1.f );
//		m_EffKeepPlayer.Render( pd3dDevice );
//
//		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
//	}
//
//	//	Note : Glow 
//	for ( int i=0; i<PART_V_SIZE; ++i )
//	{
//		m_V_PartsArray[i].RenderGlow( pd3dDevice );
//	}
//
//	//	Note : Single Eff
//	for ( int i=0; i<PART_V_SIZE; ++i )
//	{
//		m_V_PartsArray[i].RenderEff( pd3dDevice, m_fScale, NULL );
//	}
//
//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
//
//	pd3dDevice->SetLight ( 0, &sSrcLight00 );
//
//	pd3dDevice->LightEnable( 1, FALSE );
//	pd3dDevice->LightEnable( 2, FALSE );
//	pd3dDevice->LightEnable( 3, FALSE );
//	pd3dDevice->LightEnable( 4, FALSE );
//	pd3dDevice->LightEnable( 5, FALSE );
//
//	return S_OK;
//}

HRESULT DxVehicle::RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj, const BOOL bDay )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return S_OK;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return S_OK;
	}

	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	float fHeight=0.f;
	if( !bDay )
	{
		fHeight = -m_matWorld._42;
	}

	if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )	// 캐릭터가 Material System 으로 동작될 때는 이렇게 쓴다.
	{
		DWORD dwAlphaTestEnable(0L);
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );			

		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			m_V_PartsArray[i].RenderShadowForMaterial( pd3dDevice, matViewProj, FALSE );

			if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			{
				m_p_V_AttBoneArray[i]->RenderShadow( pd3dDevice, m_matWorld, matViewProj, bDay );
			}
		}

		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
	}
	else	// 하드웨어 성능이 무지 나쁜경우.
	{
		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			m_V_PartsArray[i].RenderShadow( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), fHeight );

			if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			{
				m_p_V_AttBoneArray[i]->RenderShadow( pd3dDevice, m_matWorld, matViewProj, bDay );
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	return S_OK;
}

HRESULT DxVehicle::RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return S_OK;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return S_OK;
	}

	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD dwAlphaTestEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );	

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].RenderShadowForMaterial( pd3dDevice, matViewProj, TRUE );

		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_p_V_AttBoneArray[i]->RenderShadowForMaterialSystem( pd3dDevice, matViewProj );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	return S_OK;
}

BOOL DxVehicle::RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return FALSE;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return FALSE;
	}

	// 
	SetDiffuseColor_NSMaterialSkinManager();

	SKIN_SPEC_SSS_INFO sSkinInfo;

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		if ( !m_V_PartsArray[i].RenderMaterialGhosting( pd3dDevice, m_sSkinEffData, sSkinInfo, fAlpha, unGhostingIndex ) )
			return FALSE;

		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			if ( !m_p_V_AttBoneArray[i]->RenderMaterialGhosting( pd3dDevice, fAlpha, unGhostingIndex ) )
				return FALSE;
		}
	}

	return TRUE;
}

HRESULT DxVehicle::RenderReflect( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return S_OK;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return S_OK;
	}

	// 
	SetDiffuseColor_NSMaterialSkinManager();

	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	SKIN_SPEC_SSS_INFO sSkinInfo;

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].RenderMaterialOpaque
		(
			pd3dDevice, 
			*m_spSkinAniControlThread->GetSkeletonMaintain(), 
			NULL,
			m_sSkinEffData, 
			fAlpha, 
			sSkinInfo, 
			NULL, 
			m_spSkinAniControlThread->GetWorld(), 
			m_fScale, 
			m_spSkinAniControlThread->GetClothColl(),
			m_spSkinAniControlThread->GetClothCollSphere(),
			1.f,
			FALSE
		);

		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_p_V_AttBoneArray[i]->RenderMaterialOpaque
			( 
				pd3dDevice,
				fAlpha, 
				FALSE, 
				FALSE, 
				m_spSkinAniControlThread->GetClothColl(), 
				m_spSkinAniControlThread->GetClothCollSphere(),
				m_fScale
			);
		}
	}

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].RenderMaterialHardAlpha( pd3dDevice, fAlpha, sSkinInfo );

		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_p_V_AttBoneArray[i]->RenderMaterialHardAlpha( pd3dDevice, fAlpha );
	}

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].RenderMaterialSoftAlpha( pd3dDevice, fAlpha, sSkinInfo );

		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_p_V_AttBoneArray[i]->RenderMaterialSoftAlpha( pd3dDevice, fAlpha );
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	return S_OK;
}

void DxVehicle::RenderBone( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return;
	}

	m_spSkinAniControlThread->RenderBone( pd3dDevice );
}

void DxVehicle::RenderMaterialOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	g_dwLOD = 0;
#ifdef USE_SKINMESH_LOD
	// Note : 그림자일 경우는 저급으로 뿌린다.
	if( bShadow )
	{
		g_dwLOD = 1;
	}
	else if( g_dwHIGHDRAW_NUM > 10 )
	{
		// Note : 한계가 넘었을 경우 최소한 보장해 줘야 하는 애들을 위함
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 30.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+30.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}
	else
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 150.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+150.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}

	if( g_dwLOD==0 )
	{
		++g_dwHIGHDRAW_NUM;
	}
#endif

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 다 로딩되었는지 확인한다.
		BOOL bEnableNotYet(FALSE);
		for( DWORD i=0; i<PART_V_SIZE; ++i )
		{
			if( !m_V_PartsArray[i].IsCompletedLINK() )
			{
				bEnableNotYet = TRUE;
			}
		}

		if( !bEnableNotYet )
		{
			m_bCompleted_PartLoad_FirstCreate = TRUE;
		}
	}

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return;
	}


	int nStart=0, nEnd = 0, nAdd = 0;

	nStart = 0;	
	nEnd = PART_V_SIZE;
	nAdd = 1;

	int i = nStart;


	// 
	SetDiffuseColor_NSMaterialSkinManager();
	SetLookatLength_NSMaterialSkinManager();
	SetPosition_NSMaterialSkinManager();


		
	SKIN_SPEC_SSS_INFO sSkinInfo;

	i = nStart;
	while ( i != nEnd )
	{
		m_V_PartsArray[i].RenderMaterialOpaque
		( 
			pd3dDevice, 
			*m_spSkinAniControlThread->GetSkeletonMaintain(), 
			NULL,
			m_sSkinEffData, 
			fAlpha, 
			sSkinInfo,
			NULL,
			m_spSkinAniControlThread->GetWorld(), 
			m_fScale, 
			NULL, 
			m_spSkinAniControlThread->GetClothCollSphere(),
			1.f,
			TRUE
		);
		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_p_V_AttBoneArray[i]->RenderMaterialOpaque( pd3dDevice, fAlpha, FALSE, TRUE, NULL, m_spSkinAniControlThread->GetClothCollSphere(), m_fScale );
		i += nAdd;
	}
}

void DxVehicle::RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return;
	}

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	DWORD dwAlphaTestEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );


	int nStart=0, nEnd = 0, nAdd = 0;

	nStart = 0;	
	nEnd = PART_V_SIZE;
	nAdd = 1;

	int i = nStart;

	SKIN_SPEC_SSS_INFO sSkinInfo;

	i = nStart;
	while ( i != nEnd )
	{
		m_V_PartsArray[i].RenderMaterialHardAlpha( pd3dDevice, fAlpha, sSkinInfo );
		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_p_V_AttBoneArray[i]->RenderMaterialHardAlpha( pd3dDevice, fAlpha );
		i += nAdd;
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
}

void DxVehicle::RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return;
	}

	// 
	SetDiffuseColor_NSMaterialSkinManager();
	SetLookatLength_NSMaterialSkinManager();
	SetPosition_NSMaterialSkinManager();


	DWORD dwAlphaBlendEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	int nStart=0, nEnd = 0, nAdd = 0;

	nStart = 0;	
	nEnd = PART_V_SIZE;
	nAdd = 1;

	int i = nStart;

	SKIN_SPEC_SSS_INFO sSkinInfo;

	i = nStart;
	while ( i != nEnd )
	{
		m_V_PartsArray[i].RenderMaterialSoftAlpha( pd3dDevice, fAlpha, sSkinInfo );
		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_p_V_AttBoneArray[i]->RenderMaterialSoftAlpha( pd3dDevice, fAlpha );
		i += nAdd;
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );

	// 되돌림.
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
}

void DxVehicle::RenderMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return;
	}

	// 
	SetDiffuseColor_NSMaterialSkinManager();
	SetLookatLength_NSMaterialSkinManager();
	SetPosition_NSMaterialSkinManager();





	DWORD dwAlphaBlendEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	int nStart=0, nEnd = 0, nAdd = 0;
	{
		nStart = 0;	
		nEnd = PART_V_SIZE;
		nAdd = 1;
	}

	//	Note : Single Eff
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].RenderEff( pd3dDevice, m_fScale, NULL );
		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_p_V_AttBoneArray[i]->RenderEff( pd3dDevice, fAlpha );
	}

	m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, fAlpha );



	//////////////////////////////////////////////////////////////////////////
	//	Note : Add
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	int i = nStart;
	while ( i != nEnd )
	{
		m_V_PartsArray[i].RenderMaterialAdd( pd3dDevice );
		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_p_V_AttBoneArray[i]->RenderMaterialAdd( pd3dDevice );
		}
		i += nAdd;
	}
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );



	//////////////////////////////////////////////////////////////////////////
	//	Note : Glow 
	i = nStart;
	while ( i != nEnd )
	{
		m_V_PartsArray[i].RenderMaterialGlow( pd3dDevice );
		if( m_p_V_AttBoneArray[i] && !m_p_V_AttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_p_V_AttBoneArray[i]->RenderMaterialGlow( pd3dDevice );
		}
		i += nAdd;
	}

	// 되돌림.
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
}

HRESULT DxVehicle::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : m_V_PartsArray, m_EffAniPlayer 은 다른 경로에서 초기화를 해 준다.

	return S_OK;
}

HRESULT DxVehicle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].RestoreDeviceObjects ( pd3dDevice );

		if ( m_p_V_AttBoneArray[i] )
		{
			m_p_V_AttBoneArray[i]->RestoreDeviceObjects ( pd3dDevice );
		}
	}

	m_spSkinAniControlThread->RestoreDeviceObjects ();

	return S_OK;
}

HRESULT DxVehicle::InvalidateDeviceObjects ()
{
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].InvalidateDeviceObjects ();

		if ( m_p_V_AttBoneArray[i] )
		{
			m_p_V_AttBoneArray[i]->InvalidateDeviceObjects ();
		}		
	}

	m_spSkinAniControlThread->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxVehicle::DeleteDeviceObjects ()
{
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].DeleteDeviceObjects ();

		if ( m_p_V_AttBoneArray[i] )
		{
			m_p_V_AttBoneArray[i]->DeleteDeviceObjects ();
		}		
	}

	m_spSkinAniControlThread->DeleteDeviceObjects ();

	return S_OK;
}

//------------------------------------------------------------
//				E	f	f		K	e	e	p
void DxVehicle::CreateEff( DxEffKeepDataMain* pEff )
{
	m_EffKeepPlayer.CreateEff( pEff );
}

void DxVehicle::DeleteEff( DxEffKeepDataMain* pEff )
{

}

void DxVehicle::Reset()
{
	m_EffKeepPlayer.Reset();
}

//////////////////////////////////////////////////////////////////////////
void DxVehicle::GhostingON( BOOL bGhosting, float fLifeTime )
{
	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].GhostingON ( bGhosting, fLifeTime );

		if ( m_p_V_AttBoneArray[i] )
		{
			m_p_V_AttBoneArray[i]->GhostingON ( bGhosting, fLifeTime );
		}		
	}
}

const SPassengerData* DxVehicle::GetPassengerData( int i ) const
{
	if ( i >= MAX_PASSENGER_COUNT )
		return NULL;

	return &m_spVehicleThreadNeedData->m_sPassengerData[i];
}

BOOL DxVehicle::IsValidPassengerMatrixData() const
{
	return m_spVehicleThreadNeedData->m_bUpdataPassenger;
}

void DxVehicle::SetDiffuseColor_NSMaterialSkinManager()
{
	NSMaterialSkinManager::SetDiffuseColor( m_vDiffuseColor, m_vPointLightColor );
}

void DxVehicle::SetLookatLength_NSMaterialSkinManager()
{
	const D3DXMATRIX& matWorld = m_spSkinAniControlThread->GetWorld();
	D3DXVECTOR3 vPos( matWorld._41, matWorld._42, matWorld._43 );

	D3DXVECTOR3 vDir = DxViewPort::GetInstance().GetLookatPt() - vPos;
	float fLength = D3DXVec3Length( &vDir );

	//////////////////////////////////////////////////////////////////////////
	// SelfShadow 의 거리상 한계 때문에 Diffuse(CameraSpecular) 등 밝게 나오는 문제가 있다.
	// 그래서 좀 멀리 있으면 Diffuse(CameraSpecular) 값을 줄여주기 위해 셋팅한다.
	//
	// 가까울수록 1 멀수록 0
	// 50 까지는 잘 나오도록 해준다.
	// 200 이 넘으면 변형 Diffuse(CameraSpecular) 를 사용하지 않는다.
	fLength = (200.f - fLength) / 150.f;
	if ( fLength > 1.f )
		fLength = 1.f;
	if ( fLength < 0.f )
		fLength = 0.f;

	NSMaterialSkinManager::SetColorPower_LooakLength( fLength );
}

void DxVehicle::SetPosition_NSMaterialSkinManager()
{
	const D3DXMATRIX& matWorld = m_spSkinAniControlThread->GetWorld();
	D3DXVECTOR3 vPos( matWorld._41, matWorld._42, matWorld._43 );

	NSMaterialSkinManager::SetPosition_NSMaterialSkinManager( vPos );
}



//////////////////////////////////////////////////////////////////////////
// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수

// 이곳은 매 프레임 들어오도록 한다.
void DxVehicle::SetPhysX_Cloth( BOOL bOnOff )
{
	// 셋팅값도 같도 정보도 변하지 않았다면 내부로 들어갈 필요가 없다.
	if ( m_bPhysX_Cloth_OnOff == bOnOff && m_bCompleted_PhysX_Cloth )
		return;

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_V_PartsArray[i].SetPhysX_Cloth( bOnOff );

		if ( m_p_V_AttBoneArray[i] )
		{
			if ( !m_p_V_AttBoneArray[i]->SetPhysX_Cloth( bOnOff ) )
				return;	// 작업 완료 안되고 실패.
		}		
	}

	// 다 작업되면 값 셋팅.
	m_bPhysX_Cloth_OnOff = bOnOff;
	m_bCompleted_PhysX_Cloth = TRUE;
}

// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
//////////////////////////////////////////////////////////////////////////



const TCHAR* DxVehicle::GetBoneName() const
{
	if ( !m_spSkinAniControlThread.get() )
		return NULL;

	return m_spSkinAniControlThread->GetSkeletonName();
}