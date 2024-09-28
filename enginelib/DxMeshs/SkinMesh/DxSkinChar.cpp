#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/IrradianceVolume/NSLightMapAmbientColor.h"
#include "../../DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../DxTools/Texture/NSTexCompress.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxBackUpRendTarget.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/COLLISION.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxLand/DxLandMan.h"

#include "./NSChangeCPS.h"
#include "./NSBillboardLua.h"
#include "./NSSkinAniThread.h"
#include "./DxSkinAniControlThreadNeedData.h"
#include "./DxSkinAniControlData.h"
#include "./DxSkeletonMaintain.h"
#include "./DxSkinAniControl.h"
#include "./DxSkinMeshManager.h"
#include "./DxSkinAnimation.h"
#include "./SANIMCONINFO.h"
#include "./DxSkinDefine.h"
#include "./DxAniJiggleBlend.h"
#include "./DxJiggleBoneColl.h"
#include "./DxJiggleBone.h"
#include "./DxClothColl.h"
#include "./DxFootColl.h"
#include "./DxVehicle.h"

#include "DxSkinChar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD	DxSkinChar::g_dwLOD = 0;
DWORD	DxSkinChar::g_dwHIGHDRAW_NUM = 0;

BOOL     g_bAmbientTest_CHAREDIT(FALSE);
DWORD    g_dwAmbient_CHAREDIT(180);
extern BOOL     g_bOBJECT100;
extern BOOL		g_bCHAR_EDIT_RUN;
extern BOOL		g_bWIREFRAME_VIEW;

DxSkinChar::DxSkinChar(void)
    : m_fScale(1.0f)
	, m_fHeight(20.f)
	, m_bWorldObj(FALSE)
	, m_dwFlags(0L)
	, m_dwVertexNUM(0)
	, m_fHeightYPos( 0.0f )
    , m_bCharAmbientUP(FALSE)
	, m_bCompleted_PartLoad_FirstCreate(FALSE)
	, m_bAmbientColor(FALSE)
	, m_spSkinAniControlThread(new DxSkinAniControl)
	, m_spThreadNeedData(new DxSkinAniControlThreadNeedData(PIECE_SIZE))
	, m_bPhysX_Cloth_OnOff(FALSE)
	, m_bCompleted_PhysX_Cloth(FALSE)
{
	memset( m_szFileName, 0, FILE_LENGTH );

	for ( DWORD i=0; i<PIECE_SIZE; ++i )
	{
		m_pAttBoneArray[i] = NULL;
	}

	for ( DWORD i=0; i<EMPEACE_WEAPON_SIZE; ++i )
	{
		m_arrayfWeaponScale[i] = 1.f;
	}

	D3DXMatrixIdentity( &m_matWorld );
}

DxSkinChar::~DxSkinChar(void)
{
	m_bCompleted_PhysX_Cloth = FALSE;

	for ( DWORD i=0; i<PIECE_SIZE; ++i )
	{
		if ( !m_pAttBoneArray[i] )
			continue;

		m_pAttBoneArray[i]->ClearAll();
		SAFE_DELETE ( m_pAttBoneArray[i] );
	}
}
//
//HRESULT DxSkinChar::SetPiece( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag /* = 0x0 */, int nLevel/* =0 */, bool bCharacterData /* = FALSE */, BOOL bAttachSub /* = FALSE */, EMPIECECHAR* pOutPiecetype /* = NULL*/)
//{
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
//			for ( int i=0; i<PIECE_SIZE; ++i )
//			{
//				if ( m_pAttBoneArray[i]->GetFileName() )
//				{
//					if ( !_tcscmp(m_pAttBoneArray[i]->GetFileName(),szFile) )
//					{
//						m_pAttBoneArray[i]->ClearAll();
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
//		if ( m_pAttBoneArray[pAttBoneLink->m_emPieceCharType].GetFileName() )
//		{
//			m_pAttBoneArray[pAttBoneLink->m_emPieceCharType].ClearAll();
//		}
//
//		m_pAttBoneArray[pAttBoneLink->m_emPieceCharType].SetLink ( pAttBoneLink, pd3dDevice, bCharacterData, nLevel );
//
//		if( pAttBoneLink->m_bUseCPS )
//		{
//			SetPiece( pAttBoneLink->m_strPieceName.c_str(), pd3dDevice, dwFlag, nLevel, bCharacterData, TRUE );
//		}
//
//		if ( pOutPiecetype )
//		{
//			*pOutPiecetype = pAttBoneLink->m_emPieceCharType;
//		}
//
//	}
//	else
//	{
//		//	Note : 만약 처음 로드된 것을 무시하고 새로이 로드할때에는 원래 로드된걸 지워준다.
//		//
//		if ( dwFlag&FG_MUSTNEWLOAD )
//		{
//			for ( int i=0; i<PIECE_SIZE; ++i )
//			{
//				if ( m_PartArray[i].GetFileName() )
//				{
//					if ( !_tcscmp(m_PartArray[i].GetFileName(),szFile) )
//					{
//						m_PartArray[i].ReSetPart ();
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
//		if( !bAttachSub )
//		{
//			//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
//			//
//			if ( m_pAttBoneArray[pSkinPiece->m_emPieceCharType].GetFileName() )
//			{
//				m_pAttBoneArray[pSkinPiece->m_emPieceCharType].ClearAll();
//			}
//		}
//
//		//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
//		//
//		if ( m_PartArray[pSkinPiece->m_emPieceCharType].GetFileName() )
//		{
//			m_PartArray[pSkinPiece->m_emPieceCharType].ReSetPart ();
//		}
//		
//		// Note : 모자에 셋팅이 이상하게 되어서 이렇게 해야 한다.
//		if( pSkinPiece->m_emPieceCharType == PIECE_HAIR )
//		{
//			if( m_PartArray[PIECE_HEADGEAR].GetFileName() )
//			{
//				// 일단 정리
//				m_PartArray[PIECE_HEADGEAR].ReSetPart ();
//
//				// 데이터 셋팅
//				DxSkinPiece* pSkinPieceHG = DxSkinPieceContainer::GetInstance().LoadPiece( m_PartArray[PIECE_HEADGEAR].GetFileName(), pd3dDevice, TRUE );
//				if ( !pSkinPieceHG )	goto _RETURN;
//				m_PartArray[PIECE_HEADGEAR].SetPart( pSkinPieceHG, pd3dDevice, bCharacterData );
//			}
//		}
//		_RETURN:
//
//		//	Note : Piece 를 지정.
//		//
//		m_PartArray[pSkinPiece->m_emPieceCharType].SetPart ( pSkinPiece, pd3dDevice, bCharacterData );
//		m_PartArray[pSkinPiece->m_emPieceCharType].SetGrindLevel( nLevel );
//
//		if ( pOutPiecetype )
//		{
//			*pOutPiecetype = pSkinPiece->m_emPieceCharType;
//		}
//	}
//
//	// Note : 정점 데이터 재 수집
//	CollectVertexNUM();
//
//	return S_OK;
//}

//HRESULT DxSkinChar::SetPiece( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag, int nLevel, LONG lColorKey, BOOL bThread, DxSkinChar* pFaceCHF )
HRESULT DxSkinChar::SetPiece( const TCHAR* szFile, 
							 LPDIRECT3DDEVICEQ pd3dDevice, 
							 DWORD dwFlag, 
							 int nLevel, 
							 EMSCD_FLAGS emFlags, 
							 BOOL bAttachSub /* = FALSE */, 
							 EMPIECECHAR* pOutPiecetype,
							 const char* pSelfBodyEffect )
{
	if ( NULL == szFile )
		return E_FAIL;
	else
		//return SetPiece( std::string(szFile), pd3dDevice, dwFlag, nLevel, lColorKey, bThread, pFaceCHF );
		return SetPiece( std::string(szFile), pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );
}


// [shhan][2015.08.27] AttachBone 부위에 대한 룰
// 문제점 : 인벤의 벨트, attachbone 부위의 상체
//			인벤의 상체, attachbone 부위의 상체로 셋팅하면 둘중 하나는 사라져야 함.
// 
// 해결방법이 아닌 문제가 안생기게 하기위한 조치
// 1. 인벤의 벨트, attachbone 부위의 상체
// 2. 망토 attachbone 의 부위는 cape. 망토는 src_cps 를 상체로 씀. cps 상체는 인벤의 상체.
//   결국 인벤의 상체는, cps 상 상체, attachbone 상 cape 로 셋팅되야 함.
// 3. 어떤 셋팅은 인벤의 상체, cps 상체에 attachbone 모자로 되어있는게 있다고 함.
//   결국 인벤의 상체는, cps 상 상체, attachbone 상 모자로도 셋팅되야 함.
// 4. 인벤의 모자, cps 모자는 새로운 부위를 만들어줘야 맞는거 같음.
//
// 추후 개발시 생각 할 점.
// 1. 인벤이랑 cps 의 부위랑 유효성 검사를 꼭 하게 해야함.
// 2. cps랑 attbone 은 부위가 같아야 헤깔릴 염려가 없음.
//
//
//HRESULT DxSkinChar::SetPiece( const std::string& strFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag, int nLevel, LONG lColorKey, BOOL bThread, DxSkinChar* pFaceCHF )
HRESULT DxSkinChar::SetPiece( const std::string& strFile, 
							 LPDIRECT3DDEVICEQ pd3dDevice, 
							 DWORD dwFlag, 
							 int nLevel, 
							 EMSCD_FLAGS emFlags, 
							 BOOL bAttachSub /* = FALSE */, 
							 EMPIECECHAR* pOutPiecetype,
							 const char* pSelfBodyEffect,
							 const EMPIECECHAR* pCheckPieceType )
{
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
		//	for ( int i=0; i<PIECE_SIZE; ++i )
		//	{
		//		if ( m_pAttBoneArray[i] && m_pAttBoneArray[i]->GetFileName() )
		//		{
		//			if ( !_tcscmp(m_pAttBoneArray[i]->GetFileName(),strFile.c_str()) )
		//			{
		//				m_pAttBoneArray[i]->ClearAll();
		//				SAFE_DELETE ( m_pAttBoneArray[i] );
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
		if ( m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] && m_pAttBoneArray[pAttBoneLink->m_emPieceCharType]->GetFileName() )
		{
			m_pAttBoneArray[pAttBoneLink->m_emPieceCharType]->ClearAll();
			SAFE_DELETE ( m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] );
		}

		// 타입이 다르다면 작동되지 않도록 한다.
		if ( pCheckPieceType )
		{
			if ( *pCheckPieceType != pAttBoneLink->m_emPieceCharType )
				return E_FAIL;
		}
		
		// bAttachSub TRUE 라면, 작업되면 안된다.
		// 첫 SetPiece 일 경우만 (bAttachSub FALSE) NSChangeCPS::GetNewCPS 을 동작되도록 한다.
		if ( !pCheckPieceType )
		{
			if ( GetBoneName() && pAttBoneLink->GetBaseBoneName() )
			{
				CString strFile1( GetBoneName() );
				strFile1.MakeLower();

				CString strFile2( pAttBoneLink->GetBaseBoneName() );
				strFile2.MakeLower();

				// Bone 이 다르다면 NSChangeCPS 를 체크해야 함.
				if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
				{
					const TCHAR* pNewCPS = NSChangeCPS::GetNewCPS( strFile.c_str() );
					if ( pNewCPS )
					{
						EMPIECECHAR emPieceChar = static_cast<EMPIECECHAR>( pAttBoneLink->m_emPieceCharType );
						return SetPiece( pNewCPS, pd3dDevice, dwFlag, nLevel, emFlags, FALSE, pOutPiecetype, pSelfBodyEffect, &emPieceChar );
					}
					else
					{
						return E_FAIL;
					}
				}
			}
		}

		DxAttBoneLink* pNew = new DxAttBoneLink;
		pNew->SetLink( pAttBoneLink, pd3dDevice, m_bCharAmbientUP, nLevel, pSelfBodyEffect );
		SAFE_DELETE ( m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] );
		m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] = pNew;

		if( pAttBoneLink->m_bUseCPS )
		{
			SetPiece( pAttBoneLink->m_strPieceName.c_str(), pd3dDevice, dwFlag, nLevel, emFlags, TRUE, NULL, pSelfBodyEffect );
		}

		if ( pOutPiecetype )
		{
			*pOutPiecetype = static_cast<EMPIECECHAR>( pAttBoneLink->m_emPieceCharType );
		}
	}
	else
	{
		////	Note : 만약 처음 로드된 것을 무시하고 새로이 로드할때에는 원래 로드된걸 지워준다.
		//if ( dwFlag&FG_MUSTNEWLOAD )
		//{
		//	for ( int i=0; i<PIECE_SIZE; ++i )
		//	{
		//		if ( m_PartArray[i].GetFileName() )
		//		{
		//			if ( ! strFile.compare( m_PartArray[i].GetFileName() ) )
		//			{
		//				m_PartArray[i].ReSetPart();
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

		// cps 가 변경되면 같은 포지션의 AttBone 에 셋팅된 것을 지워줘야 한다.
		// 하지만 AttBone에서도 cps 를 강제 셋팅하는게 있기 때문에 AttBone일 경우는 AttBone 을 지워주진 않는다.
		if( !bAttachSub )
		{
			//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
			//
			if ( m_pAttBoneArray[pSkinPiece->m_emType] && m_pAttBoneArray[pSkinPiece->m_emType]->GetFileName() )
			{
				m_pAttBoneArray[pSkinPiece->m_emType]->ClearAll();
				SAFE_DELETE ( m_pAttBoneArray[pSkinPiece->m_emType] );
			}
		}

		// 타입이 다르다면 작동되지 않도록 한다.
		if ( pCheckPieceType )
		{
			if ( *pCheckPieceType != pSkinPiece->m_emType )
				return E_FAIL;
		}

		// bAttachSub TRUE 라면, 작업되면 안된다.
		// 첫 SetPiece 일 경우만 (bAttachSub FALSE) NSChangeCPS::GetNewCPS 을 동작되도록 한다.
		if( !bAttachSub )
		{
			if ( GetBoneName() &&  pSkinPiece->GetBoneName() )
			{
				CString strFile1( GetBoneName() );
				strFile1.MakeLower();

				CString strFile2( pSkinPiece->GetBoneName() );
				strFile2.MakeLower();

				// Bone 이 다르다면 NSChangeCPS 를 체크해야 함.
				if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
				{
					const TCHAR* pNewCPS = NSChangeCPS::GetNewCPS( strFile.c_str() );
					if ( pNewCPS )
					{
						return SetPiece( pNewCPS, pd3dDevice, dwFlag, nLevel, emFlags, FALSE, pOutPiecetype, pSelfBodyEffect, &pSkinPiece->m_emType );
					}
					else
					{
						if ( !pSkinPiece->m_skinMeshRes.get() )
							return E_FAIL;

						SMeshContainer* pmcMesh = pSkinPiece->m_skinMeshRes.get()->FindMeshContainer ( pSkinPiece->m_szMeshName );
						if ( !pmcMesh )
							return E_FAIL;

						// SkinMesh 라면 Bone 다를경우 셋팅이 불가능 함.
						if( pmcMesh->pSkinInfo )
							return E_FAIL;
					}
				}
			}
		}

		////	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
		////
		//if ( m_PartArray[pSkinPiece->m_emType].GetFileName() )
		//{
		//	m_PartArray[pSkinPiece->m_emType].ReSetPart();
		//}

		//// Note : 모자는 HAIR가 바뀌어야 하기때문에 셋팅이 PIECE_HAIR 로 되어있다.
		//if( pSkinPiece->m_emType == PIECE_HAIR )
		//{
		//	if( m_PartArray[PIECE_HEADGEAR].IsMeshContainer() )
		//	{
		//		// 일단 정리
		//		m_PartArray[PIECE_HEADGEAR].ReSetPart();

		//		// 데이터 셋팅
		//		DxSkinPiece* pSkinPieceHG = DxSkinPieceContainer::GetInstance().LoadPiece( m_PartArray[PIECE_HEADGEAR].GetFileName(), pd3dDevice, bThread );
		//		if ( !pSkinPieceHG )
		//			goto _RETURN;
		//		m_PartArray[PIECE_HEADGEAR].SetPart( pSkinPieceHG, pd3dDevice, bAmbientUP , TRUE, PIECE_HEADGEAR );
		//	}
		//}

//_RETURN:

		//	Note : Piece 를 지정.
		//
		m_PartArray[pSkinPiece->m_emType].SetPart ( this, pSkinPiece, pd3dDevice, m_bCharAmbientUP, pSelfBodyEffect );
		m_PartArray[pSkinPiece->m_emType].SetGrindLevel( nLevel );
		//m_PartArray[pSkinPiece->m_emType].SetColorKey( lColorKey );

		// [ Thread Work shhan ]
		//// HEAD일 경우만 강제로 셋팅하도록 한다.
		//SetSexFaceID( pSkinPiece->m_emType );

		//if( pFaceCHF )
		//{
		//	//pFaceCHF->SetPiece( strFile, pd3dDevice, dwFlag, nLevel, lColorKey, bThread, NULL );
		//	pFaceCHF->SetPiece( strFile, pd3dDevice, dwFlag, nLevel, bThread, NULL );
		//}

		if( pSkinPiece->m_emType == PIECE_HEADGEAR && pSkinPiece->IsHatHairOption( HAT_HAIR_NEW ) && !pSkinPiece->GetAddHairCps().empty() )
		{
			// HAT_HAIR_NEW - 새로운 헤어가 나와야 한다.
			SetPiece( pSkinPiece->GetAddHairCps().c_str(), pd3dDevice, dwFlag, nLevel, emFlags, FALSE, NULL, pSelfBodyEffect );
		}

		if ( pOutPiecetype )
		{
			*pOutPiecetype = pSkinPiece->m_emType;
		}
	}

	// 변경 파츠의 PhysX 정보 재 셋팅을 해야 한다라고 셋팅한다.
	m_bCompleted_PhysX_Cloth = FALSE;

	return S_OK;
}

HRESULT DxSkinChar::SetPieceHiddenWeapon( 
							 const std::string& strFile,
							 LPDIRECT3DDEVICEQ pd3dDevice,
							 int nLevel, 
							 EMSCD_FLAGS emFlags,
							 OUT EMPIECECHAR& emType,
							 const char* pSelfBodyEffect /*NULL*/ )
{
	if ( strFile.empty() )
		return E_FAIL;

	//	Note : Piece를 읽어온다.
	DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( strFile, EMSCD_ALL_THREAD );
	if ( !pSkinPiece )
		return E_FAIL;

	emType = pSkinPiece->m_emType;
	if ( emType != PIECE_RHAND_HIDDEN && emType != PIECE_LHAND_HIDDEN )
		return E_FAIL;

	// cps 가 변경되면 같은 포지션의 AttBone 에 셋팅된 것을 지워줘야 한다.
	// 하지만 AttBone에서도 cps 를 강제 셋팅하는게 있기 때문에 AttBone일 경우는 AttBone 을 지워주진 않는다.
	if ( m_pAttBoneArray[emType] && m_pAttBoneArray[emType]->GetFileName() )
	{
		m_pAttBoneArray[emType]->ClearAll();
		SAFE_DELETE ( m_pAttBoneArray[emType] );
	}

	if ( GetBoneName() &&  pSkinPiece->GetBoneName() )
	{
		CString strFile1( GetBoneName() );
		strFile1.MakeLower();

		CString strFile2( pSkinPiece->GetBoneName() );
		strFile2.MakeLower();

		// Bone 이 다르다면 NSChangeCPS 를 체크해야 함.
		if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
		{
			const TCHAR* pNewCPS = NSChangeCPS::GetNewCPS( strFile.c_str() );
			if ( pNewCPS )
			{
				return SetPieceHiddenWeapon( pNewCPS, pd3dDevice, nLevel, emFlags, emType, pSelfBodyEffect );
			}
			else
			{
				if ( !pSkinPiece->m_skinMeshRes.get() )
					return E_FAIL;

				SMeshContainer* pmcMesh = pSkinPiece->m_skinMeshRes.get()->FindMeshContainer ( pSkinPiece->m_szMeshName );
				if ( !pmcMesh )
					return E_FAIL;

				// SkinMesh 라면 Bone 다를경우 셋팅이 불가능 함.
				if( pmcMesh->pSkinInfo )
					return E_FAIL;
			}
		}
	}

	// 동일한 Parts 라면 FALSE 가 나올 수 있다.
	// 다른 Parts 라면 TRUE 가 나오면서 m_bCompleted_PhysX_Cloth FALSE 를 해준다.
	if ( m_PartArray[emType].SetPart ( this, pSkinPiece, pd3dDevice, m_bCharAmbientUP, pSelfBodyEffect ) )
	{
		// 변경 파츠의 PhysX 정보 재 셋팅을 해야 한다라고 셋팅한다.
		m_bCompleted_PhysX_Cloth = FALSE;
	}

	m_PartArray[emType].SetGrindLevel( nLevel );

	return S_OK;
}

HRESULT DxSkinChar::SetPieceGrindLevel ( int nIndex, int nLevel )
{
	if ( PIECE_SAVE_SIZE <= nIndex )	return E_FAIL;

	m_PartArray[nIndex].SetGrindLevel(nLevel);

	return S_OK;
}

void DxSkinChar::SetHairColor( WORD wColor )
{
	WORD wR, wG, wB;
	wR = wColor >> 10;
	wG = wColor >> 5;
	wG = wG&0x1f;
	wB = wColor&0x1f;
	
	wR = (WORD)( wR * 8.225806f );	//<< 3;
	wG = (WORD)( wG * 8.225806f );	//<< 3;
	wB = (WORD)( wB * 8.225806f );	//<< 3;

	DWORD dwColor = 0xff000000 + (wR<<16) + (wG<<8) + wB;

	m_PartArray[PIECE_HAIR].SetHairColor( dwColor );
	m_PartArray[PIECE_HAIR_4_HAT].SetHairColor( dwColor );
}

WORD DxSkinChar::GetHairColor()
{
	DWORD dwColor = m_PartArray[PIECE_HAIR].GetHairColor();

	// 각각 분할
	WORD wR, wG, wB;
	wR = (WORD)((dwColor&0xff0000)>>16);
	wG = (WORD)((dwColor&0xff00)>>8);
	wB = (WORD)(dwColor&0xff);

	// 압축시
	wR = wR >> 3;
	wG = wG >> 3;
	wB = wB >> 3;
	return (wR<<10) + (wG<<5) + wB;
}

HRESULT DxSkinChar::ClearAll ()
{
	m_fScale = 1.0f;
	m_bWorldObj = FALSE;
	m_dwFlags = 0L;
	m_fHeightYPos = 0.0f;
	m_bCompleted_PhysX_Cloth = FALSE;

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
		//
		if ( m_PartArray[i].GetFileName() )
		{
			m_PartArray[i].ReSetPart ();
		}

		if ( m_pAttBoneArray[i] && m_pAttBoneArray[i]->GetFileName() )
		{
			m_pAttBoneArray[i]->ClearAll();
		}

		SAFE_DELETE( m_pAttBoneArray[i] );
	}

	m_spSkinAniControlThread->ClearAll ();

	return S_OK;
}

PDXATTBONELINK DxSkinChar::GetAttachBone ( int i )
{
	if( i < 0 || i >= PIECE_SIZE )
		return NULL;

	if ( !m_pAttBoneArray[i] )
		return NULL;

	return m_pAttBoneArray[i];
}

PDXCHARPART DxSkinChar::GetPiece ( int i )
{
	if( i < 0 || i >= PIECE_SIZE )
		return NULL;

	return &m_PartArray[i];
}

void DxSkinChar::ResetPiece ( int i )
{
	if( i < 0 || i >= PIECE_SIZE )
		return;

	// 이전 값이 Hair 를 변경하는 것이었다면 되돌려야 한다.
	if ( i == PIECE_HEADGEAR && m_PartArray[i].IsHatHairOption( HAT_HAIR_NEW ) && !m_PartArray[i].GetAddHairCps().empty() )
	{
		ResetPiece ( PIECE_HAIR_4_HAT );
	}

	//	Note : 이전에 올려진 Part 가 있다면 이를 내려준다.
	//
	if ( m_PartArray[i].GetFileName() )
	{
		m_PartArray[i].ReSetPart ();
	}

	if ( m_pAttBoneArray[i] && m_pAttBoneArray[i]->GetFileName() )
	{
		m_pAttBoneArray[i]->ClearAll();
	}

	SAFE_DELETE ( m_pAttBoneArray[i] );
}

void DxSkinChar::SetPartRend ( EMPIECECHAR emPIECE, BOOL bRender )
{
	if( emPIECE < 0 || emPIECE >= PIECE_SIZE )
		return;

	m_PartArray[emPIECE].SetRender ( bRender );

	if ( m_pAttBoneArray[emPIECE] )
	{
		m_pAttBoneArray[emPIECE]->SetRender ( bRender );
	}
}

void DxSkinChar::SetCharData ( DxSkinCharData* pCharData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP /*= FALSE*/, EMSCD_FLAGS emFlags, BOOL bMaintainAnim )
{
	// [shhan][2014.09.18]
	// [CRM #3013] 변신 기능으로 인해 DxSkinChar가 유지 되면서 DxSkinCharData 인자값이 다른것들이 넘어 올 수 있다. 그러면서 재 셋팅.
	//				타 쓰레드에서 참조하고 있을 수 있으므로 m_spSkinAniControlThread->ClearAll, m_spSkinAniControlThread->SetCharData 등.. 작업을 직접하지 말고 새로 생성 후 하자.
	//				형상이 바뀌는 경우는 캐릭터가 잠깐 안보이더라도 전부 정리를 해주는 방향으로 한다.
	{
		if ( bMaintainAnim == FALSE )
			m_spSkinAniControlThread = std::tr1::shared_ptr<DxSkinAniControl>(new DxSkinAniControl);

		m_spThreadNeedData = std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>(new DxSkinAniControlThreadNeedData(PIECE_SIZE));

		for ( int i=0; i<PIECE_SIZE; ++i )
		{
			//	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
			//
			if ( m_PartArray[i].GetFileName() )
			{
				m_PartArray[i].ReSetPart ();
			}

			if ( m_pAttBoneArray[i] && m_pAttBoneArray[i]->GetFileName() )
			{
				m_pAttBoneArray[i]->ClearAll();
			}

			SAFE_DELETE( m_pAttBoneArray[i] );
		}
	}
	




	//if ( pCharData->GetFileName().empty() )
	//	return;

	//BOOL bUserColor(FALSE);
	BOOL bSetBaseCPS(FALSE);
	BOOL bMeshThread(FALSE);
	BOOL bJiggleBone(FALSE);
	//if( emFlags & EMSCD_USERCOLOR )
	//{
	//	bUserColor = TRUE;
	//}
	if( emFlags & EMSCD_SET_BASE_CPS )
	{
		bSetBaseCPS = TRUE;
	}
	if( emFlags & EMSCD_MESH_THREAD )
	{
		bMeshThread = TRUE;
	}
	//if( emFlags & EMSCD_GAME_PLAYER )
	//{
	//	m_bGamePlayer = TRUE;
	//}
	//if( emFlags & EMSCD_JIGGLE_BONE )
	//{
	//	bJiggleBone = TRUE;
	//}

	bJiggleBone = TRUE;		// 지글본 무조건 활성화

	//if( dwFlags & EMSCD_GAME_PLAYER )
	//{
	//	m_bGamePlayer = TRUE;
	//}

	StringCchCopy( m_szFileName, FILE_LENGTH, pCharData->GetFileName().c_str() );

	if ( bMaintainAnim == FALSE )
		m_spSkinAniControlThread->ClearAll ();

	//	EMANI_MAINTYPE MType = m_spSkinAniControlThread->GETCURMTYPE();
	//	EMANI_SUBTYPE SType = m_spSkinAniControlThread->GETCURSTYPE();
	//	if ( MType == AN_NONE )	MType = AN_GUARD_N;
	EMANI_MAINTYPE MType = AN_GUARD_N;
	EMANI_SUBTYPE  SType = AN_SUB_NONE;

	//float fCurTime = m_spSkinAniControlThread->GETCURTIME();

	// Billboard.
	m_spBillboard = NSBillboard::pushBillboard( pCharData->GetFileName().c_str() );

	m_fScale = pCharData->m_fScale;
	m_vMax = pCharData->m_vMax;
	m_vMin = pCharData->m_vMin;
	m_fHeight = pCharData->m_fHeight;
	m_fHeightYPos = pCharData->m_fHeightYPos;

	m_bWorldObj = pCharData->m_bWorldObj;
	m_dwFlags = pCharData->m_dwFlags;

    m_bCharAmbientUP = bAmbientUP;

	m_smpFootColl	= pCharData->GetFootColl();				// DxFootColl

	//sBillboard.Import( pCharData->m_sBillboardData );

	m_spSkinAniControlThread->SetCharData ( pCharData->GetSkinAniControlData(), pd3dDevice, m_fScale, m_szFileName, bMaintainAnim);

	for ( DWORD i=0; i<EMPEACE_WEAPON_SIZE; ++i )
	{
		m_arrayfWeaponScale[i] = pCharData->m_arrayfWeaponScale[i];
	}

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		DxSkinPieceBase* pSkinPieceBase = pCharData->GetPiece( pd3dDevice, i, emFlags );
		if ( !pSkinPieceBase )
		{
			m_PartArray[i].ReSetPart ();
			if ( m_pAttBoneArray[i] )
			{
				m_pAttBoneArray[i]->ClearAll();
				SAFE_DELETE ( m_pAttBoneArray[i] );
			}
		}

		// [shhan][2015.03.25] 헤어 Color 를 초기화 해주도록 한다. ( CharEdit 에서 꼭 필요한 기능임 )
		//						해주지 않으면 이전 Color 가 남아있어서 cps 에서 셋팅한 값이 안나온다.
		m_PartArray[i].SetHairColor( 0L, FALSE );
		m_PartArray[i].SetSubColor( 0L, FALSE );
	}

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//

		DxSkinPieceBase* pSkinPieceBase = pCharData->GetPiece( pd3dDevice, i, emFlags );
		if ( !pSkinPieceBase )
		{
			continue;
		}

		// 알맞는 Slot 찾기
		// cps 로 chf 를 만들어서 셋팅 후 그 cps의 셋팅 부위를 변경하고, chf 를 변경 안하면 변경된 셋팅부위가 아닌 이전 셋팅부위에 설정되는 문제가 발생함.
		// 그걸 보완하기 위해 추가함.
		DWORD dwSlot = pSkinPieceBase->GetSlot();

		if ( pSkinPieceBase->m_emSkinPieceType == DxSkinPieceBase::SKIN_PIECE )
		{
			// [shhan][2015.06.02]
			// chf 상 셋팅시 맞았지만. cps 그 후에 변경해버리면, 알맞지 않은 bone 이 셋팅과 연결이 되어 문제가 된다.
			// 이 때는 그냥 셋팅을 안하는게 맞다.
			CString strFile1( pSkinPieceBase->GetBoneName() );
			strFile1.MakeLower();

			CString strFile2( pCharData->GetSkeletonName() );
			strFile2.MakeLower();

			m_PartArray[dwSlot].SetPart ( this, MIN_BOOST::polymorphic_downcast<DxSkinPiece*>( pSkinPieceBase ), pd3dDevice, bAmbientUP, NULL );

			// CharEdit 에서 조각파일 셋팅해도 보이지 않아서 추가함.
			// 이걸 안하면 아마 이러한 모자를 chf 에 저장해 놓은 상태도 문제가 생길 수 있다.
			if ( dwSlot == PIECE_HEADGEAR && m_PartArray[dwSlot].IsHatHairOption(HAT_HAIR_NEW) && !m_PartArray[dwSlot].GetAddHairCps().empty() )
			{
				SetPiece( m_PartArray[dwSlot].GetAddHairCps().c_str(), pd3dDevice, 0L, 0, emFlags, FALSE, NULL, NULL );
			}
		}

		if ( pSkinPieceBase->m_emSkinPieceType == DxSkinPieceBase::SKIN_ATT_BONE_LINK )
		{
			// [ Thread Work shhan ]
			//m_pAttBoneArray[dwSlot]->SetLink ( pCharData->GetAttachBone(dwSlot), pd3dDevice, bAmbientUP, bUserColor, static_cast<EMPIECECHAR>(dwSlot) );
			DxAttBoneLink* pNew = new DxAttBoneLink;
			pNew->SetLink ( MIN_BOOST::polymorphic_downcast<DxAttBoneLink*>( pSkinPieceBase ), pd3dDevice, bAmbientUP, 0 );
			SAFE_DELETE ( m_pAttBoneArray[dwSlot] );
			m_pAttBoneArray[dwSlot] = pNew;

			// [shhan][2014.12.12] CharEdit 가 아닌 Game 상에서 NPC 나 몹에서 m_bUseCPS TRUE 가 정상 출력되지 않아서 수정함.
			if( pNew->m_bUseCPS )
			{
				SetPiece( pNew->m_strPieceName.c_str(), pd3dDevice, 0L, 0, emFlags, TRUE, NULL, NULL );
			}

			// [shhan][2014.12.12] 생각해보니 밑에 주석의 이유가 밝혀졌다. 
			//						Game 상 주요 캐릭터들은 SetPiece 로 인해 m_bUseCPS 에 맞게 작동되면서 제대로 나왔었고 
			//						일반 몹이나 NPC 는 정상적으로 안나왔었고 툴 또한 안나왔었다.
			//// [shhan][2014.07.18] CharEdit 상에서 m_bUseCPS 셋팅된게 안나오는 문제가 있어서 작업함.
			////						Game 에서는 잘 나온다. 왜 잘되는지 확인이 필요하다.
			//if ( g_bCHAR_EDIT_RUN )
			//{
			//	if( pNew->m_bUseCPS )
			//	{
			//		SetPiece( pNew->m_strPieceName.c_str(), pd3dDevice, 0L, 0, emFlags, TRUE, NULL, NULL );
			//	}
			//}
		}
	}

	// 변경 파츠의 PhysX 정보 재 셋팅을 해야 한다라고 셋팅한다.
	m_bCompleted_PhysX_Cloth = FALSE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//const DxSkinAniControlData::VEC_ANIMATION& vecANI = pCharData->GetAniList();
	//size_t nSize = vecANI.size();
	//for ( size_t n=0; n<nSize; ++n )
	//{
	//	m_spSkinAniControlThread->InitAnimation( pd3dDevice, vecANI[n].get() );
	//}

	//m_spSkinAniControlThread->ClassifyAnimation ();

 //   //---------------------------------------------------//
 //   NSLoadingDirect::FrameMoveRender( FALSE );
 //   //---------------------------------------------------//

	////	Note : 초기 에니메이션 설정.
	////
	//m_spSkinAniControlThread->SETCURTIME ( fCurTime );
	//m_spSkinAniControlThread->SELECTANI ( MType, SType );

	//if ( !GETCURANIMNODE() )
	//{
	//	m_spSkinAniControlThread->DEFAULTANI();
	//	//m_spSkinAniControlThread->SELECTANI ( AN_PLACID, SType );
	//}

	//if ( !GETCURANIMNODE() )
	//{
 //       std::string ErrorMsg(
 //           sc::string::format(
 //               "current animation node null point error [%1%] [M %2% S %3%]",
 //               pCharData->GetFileName(),
 //               MType,
 //               SType));
	//	CDebugSet::ToLogFile(ErrorMsg);

	//	if (m_vecAnim.size())
 //           DEFAULTANI();
	//}

	//// [ Thread Work shhan ]

	//m_spSkinAniControlThread->SetBoneScaleList(pCharData->GetBoneScaleList());

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
}

void DxSkinChar::SetRadiusHeight( DxSkinCharData* pCharData )
{
	m_vMin = pCharData->m_vMin;
	m_vMax = pCharData->m_vMax;
	m_fHeight = pCharData->m_fHeight;
}

void DxSkinChar::ResetStrikeAttachBone()
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if( !pAnimContainer )
		return;

	//SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PIECE_SIZE; ++i )
	{	
		if( !m_pAttBoneArray[i] )
			continue;

		if( m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			continue;
		
		m_pAttBoneArray[i]->m_nOldAttSTEP = -1;
	}
}

void DxSkinChar::UpdateStrikeAttachBone( const DWORD dwThisKey, const int nattSTEP )
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if( !pAnimContainer )
		return;

	if ( nattSTEP >= pAnimContainer->m_wStrikeCount )
		return;

	const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PIECE_SIZE; ++i )
	{	
		if( !m_pAttBoneArray[i] )
			continue;

		if( m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			continue;

		PDXATTBONE pDxAttBone = m_pAttBoneArray[i]->GetAttBone();
		if( !pDxAttBone ) 
			continue;

		if( !pAttachBoneAnimInfo[i].bUseStrikeAni ) 
			continue;

		if( static_cast<int>(pAttachBoneAnimInfo[i].vecStrikeAni.size()) < nattSTEP ) 
			continue;

		{
			SAttachBoneAnimBasicInfo sAttBasicInfo = pAttachBoneAnimInfo[i].vecStrikeAni[nattSTEP];

			DWORD dwStrikeKey = pAnimContainer->m_sStrikeEff[nattSTEP].m_dwFrame + (sAttBasicInfo.nStartAnim * m_spSkinAniControlThread->GETUNITTIME());

			if ( dwThisKey>=dwStrikeKey && m_pAttBoneArray[i]->m_nOldAttSTEP != nattSTEP )
			{
				m_pAttBoneArray[i]->m_nOldAttSTEP = nattSTEP;
				// 애니메이션 update
				m_pAttBoneArray[i]->GetAttBone()->TOSTARTTIME();
				m_pAttBoneArray[i]->SELECTANI( sAttBasicInfo.emAttAniType );			
			}
			else if( m_pAttBoneArray[i]->m_nOldAttSTEP == -1	
					  || m_pAttBoneArray[i]->GetAttBone()->ISENDANIM() )
			{
				// STAY 여야 할때			
				if( sAttBasicInfo.emAttAniType != AN_ATTBONE_STAY )
				{
					m_pAttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );
				}
			}

			if( m_pAttBoneArray[i]->GetAttBone()->ISENDANIM() )
			{
				if( sAttBasicInfo.emAttAniType != AN_ATTBONE_STAY )
				{
					m_pAttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );
				}
			}

		}
	}
}

void DxSkinChar::UpdateNonStrikeAttachBone()
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if( !pAnimContainer ) return;

	const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PIECE_SIZE; ++i )
	{	
		if( !m_pAttBoneArray[i] )
			continue;

		// 프레임이 끝나면 무조건 STAY로
		if( m_pAttBoneArray[i]->GetAttBone()->ISENDANIM() )
		{
			m_pAttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );			
		}


		if( m_pAttBoneArray[i]->m_strAttBoneName.empty() ) continue;
		PDXATTBONE pDxAttBone = m_pAttBoneArray[i]->GetAttBone();
		if( !pDxAttBone ) continue;
		if( pAttachBoneAnimInfo[i].bUseStrikeAni ) continue;

		SAttachBoneAnimBasicInfo sAttBasicInfo = pAttachBoneAnimInfo[i].sBasicAni;

		// 애니메이션을 바꾸어야 할때
		if( m_spSkinAniControlThread->GETCURKEYTIME() >= static_cast<DWORD>(sAttBasicInfo.nStartAnim) )
		{
			// 애니메이션 update
			if( sAttBasicInfo.emAttAniType != pDxAttBone->GETCURMTYPE() )
			{
				m_pAttBoneArray[i]->SELECTANI( sAttBasicInfo.emAttAniType );
			}
		}
		else
		{
			// STAY 여야 할때			
			if( sAttBasicInfo.emAttAniType != AN_ATTBONE_STAY )
			{
				m_pAttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );
			}
		}		
	}

}

void DxSkinChar::SET_ATTBONE_CURTIME_ALL_FOR_EDIT( float fCurTime )
{
	const DxSkinAnimation* pAnimContainer = m_spSkinAniControlThread->GETCURANIM();
	if( !pAnimContainer ) 
		return;

	const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimContainer->GetAttachBoneAnimInfo();
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		if( !m_pAttBoneArray[i] )
			continue;

		if( m_pAttBoneArray[i]->m_strAttBoneName.empty() ) continue;
		PDXATTBONE pDxAttBone = m_pAttBoneArray[i]->GetAttBone();
		if( !pDxAttBone ) continue;

		SAttachBoneAnimBasicInfo sAttBasicInfo = pAttachBoneAnimInfo[i].sBasicAni;

		// 애니메이션을 바꾸어야 할때
		if( m_spSkinAniControlThread->GETCURTIME() >= sAttBasicInfo.nStartAnim * m_spSkinAniControlThread->GETUNITTIME() )
		{
			pDxAttBone->SETCURTIME_ALL_FOR_EDIT( fCurTime - sAttBasicInfo.nStartAnim * m_spSkinAniControlThread->GETUNITTIME() );
		}
		else
		{
			pDxAttBone->SETCURTIME_ALL_FOR_EDIT( fCurTime );
		}
	}
}

void DxSkinChar::SetPart_Detail()
{
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].SetPart_Detail();
	}
}

BOOL DxSkinChar::IsUseWorldColor()
{
	if ( m_dwFlags & EMSCD2_WORLD_COLOR )
		return TRUE;
	else
		return FALSE;
}

HRESULT DxSkinChar::FrameMoveCore( const float fTime, 
								  const float fElapsedTime,
								  const BOOL bContinue, 
								  BOOL const bFreeze, 
								  const BOOL bAttackMode, 
								  const BOOL bBoardRender,
								  BOOL bNoneOverFrame )
	{
	if ( bFreeze )	
		m_spSkinAniControlThread->FrameMove_AniTime ( 0.0f, bContinue );
	else
		m_spSkinAniControlThread->FrameMove_AniTime ( fElapsedTime, bContinue );

	UpdateNonStrikeAttachBone();

	m_sSkinEffData.Reset();		// Data Reset.

	m_PartArray[PIECE_RHAND].SetAttackMode ( bAttackMode );
	m_PartArray[PIECE_LHAND].SetAttackMode ( bAttackMode );

	m_PartArray[PIECE_RHAND_HIDDEN].SetAttackMode ( bAttackMode );
	m_PartArray[PIECE_LHAND_HIDDEN].SetAttackMode ( bAttackMode );

	if ( m_pAttBoneArray[PIECE_RHAND] )
	{
		m_pAttBoneArray[PIECE_RHAND]->SetAttackMode ( bAttackMode );
	}
	if ( m_pAttBoneArray[PIECE_LHAND] )
	{
		m_pAttBoneArray[PIECE_LHAND]->SetAttackMode ( bAttackMode );
	}

	if ( m_pAttBoneArray[PIECE_RHAND_HIDDEN] )
	{
		m_pAttBoneArray[PIECE_RHAND_HIDDEN]->SetAttackMode ( bAttackMode );
	}
	if ( m_pAttBoneArray[PIECE_LHAND_HIDDEN] )
	{
		m_pAttBoneArray[PIECE_LHAND_HIDDEN]->SetAttackMode ( bAttackMode );
	}

	// 헤드기어에도 Overlay 를 넣고 싶으나, 기존에는 PIECE_HAIR 로 셋팅을 하여서 문제가 발생.
	// PIECE_HEADGEAR 셋팅 하고, 헤드기어의 옵션에서 헤어 Rendering On/Off 를 할 수 있도록 작업 함.
	if ( m_PartArray[PIECE_HEADGEAR].IsHatHairOption( HAT_HAIR_BASE ) )
	{
		m_PartArray[PIECE_HAIR].SetRender( TRUE );
	}
	else
	{
		m_PartArray[PIECE_HAIR].SetRender( FALSE );
	}

	m_spSkinAniControlThread->FrameMove_EffAniPlayer( this, m_sSkinEffData, fTime, fElapsedTime, bFreeze );

	// Note : Edge Color 디폴트 셋팅.
	m_EffKeepPlayer.FrameMove( fElapsedTime, m_sSkinEffData );


	if( bBoardRender )
	{
		m_PartArray[PIECE_RHAND].FrameMove ( fTime, fElapsedTime, m_spSkinAniControlThread, bNoneOverFrame );
		m_PartArray[PIECE_LHAND].FrameMove ( fTime, fElapsedTime, m_spSkinAniControlThread, bNoneOverFrame );
		m_PartArray[PIECE_RHAND_HIDDEN].FrameMove ( fTime, fElapsedTime, m_spSkinAniControlThread, bNoneOverFrame );
		m_PartArray[PIECE_LHAND_HIDDEN].FrameMove ( fTime, fElapsedTime, m_spSkinAniControlThread, bNoneOverFrame );
	}
	else
	{
		for( int i = 0; i < PIECE_SIZE; ++i )
		{
			m_PartArray[i].FrameMove ( fTime, fElapsedTime, m_spSkinAniControlThread, bNoneOverFrame );

			if ( m_pAttBoneArray[i] )
			{
				m_pAttBoneArray[i]->FrameMove( fTime, fElapsedTime, bNoneOverFrame, m_spSkinAniControlThread, m_spThreadNeedData->m_arrayKeyJiggleWeight[i], bContinue, bFreeze );
			}
		}
	}


	return S_OK;
}

HRESULT DxSkinChar::FrameMove( const float fTime, 
							  const float fElapsedTime,
							  BOOL bNoneOverFrame,
							  const D3DXMATRIX &matRot, 
							  const BOOL bContinue/* =TRUE */, 
							  BOOL const bFreeze/* =FALSE */, 
							  const BOOL bAttackMode/* =TRUE */, 
							  const BOOL bBoardRender /* = FALSE */ )
{
	FrameMoveCore
	( 
		fTime, 
		fElapsedTime,
		bContinue,
		bFreeze,
		bAttackMode,
		bBoardRender,
		bNoneOverFrame
	);

	//////////////////////////////////////////////////////////////////////////
	// Render 에서 옴.
	D3DXMatrixIdentity( &m_matWorld );
	m_matWorld._11 = m_fScale;
	m_matWorld._22 = m_fScale;
	m_matWorld._33 = m_fScale;
	D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matRot );

	// 본 Scale 을 느리면서 다리 위치가 땅에 파묻힐 수 있어서 Offset 값을 작업한다.
	m_matWorld._42 += m_fHeightYPos;

	// 혹시 SafeMode 이면서 무기가 EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT 에 붙어야 할 경우를 찾는다.
	m_PartArray[PIECE_RHAND].GetMoveParentBoneIDForSafeModeWeaponSlot( m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot );

	// TAG_ProcessAnimationThread_1
	// 등록만 한다.
	NSSkinAniThread::AddAniControl( m_spSkinAniControlThread, m_spThreadNeedData, m_matWorld, bNoneOverFrame );

	// 이전 키등록을 제일 마지막에 해야한다.
	m_spSkinAniControlThread->SetPrevKeyFrame();

	return S_OK;
}

HRESULT DxSkinChar::FrameMoveCharOnVehicle( const float fTime, 
										  const float fElapsedTime,
										  const BOOL bContinue,
										  BOOL const bFreeze,
										  const BOOL bAttackMode,
										  DxVehicle* pVehicle,
										  DWORD dwVehiclePassengerID, 
										  BOOL bNoneOverFrame )
{
	FrameMoveCore
	( 
		fTime, 
		fElapsedTime,
		bContinue,
		bFreeze,
		bAttackMode,
		FALSE,
		bNoneOverFrame
	);

	// 혹시 SafeMode 이면서 무기가 EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT 에 붙어야 할 경우를 찾는다.
	m_PartArray[PIECE_RHAND].GetMoveParentBoneIDForSafeModeWeaponSlot( m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot );

	// TAG_ProcessAnimationThread_1
	// 등록만 한다.
	NSSkinAniThread::AddAniControl_CharOnVehicle( m_spSkinAniControlThread, m_spThreadNeedData, pVehicle->m_spVehicleThreadNeedData, dwVehiclePassengerID, bNoneOverFrame );

	// 이전 키등록을 제일 마지막에 해야한다.
	m_spSkinAniControlThread->SetPrevKeyFrame();

	return S_OK;
}

void DxSkinChar::ClearReserveAction()
{
	m_spSkinAniControlThread->ClearReserveFuction();	
}

// 렌더시 필수 요소가 있는가~?
BOOL DxSkinChar::IsValidForRender()
{
	// Thread 작업되면서 Bone이 없을 수 있다.
	if ( !m_spSkinAniControlThread->GetSkeletonMaintain()->IsActiveBone() )
		return FALSE;

	// 캐릭터 툴이 아닐 때..
	if ( !g_bCHAR_EDIT_RUN ) 
	{
		// 기본 애니메이션이 있을 때 렌더링 한다.
		if ( !m_spSkinAniControlThread->IsValid_Skeleton_DefaultAni() )
			return FALSE;
	}

	return TRUE;
}

HRESULT DxSkinChar::RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, 
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

	//UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&m_matWorld._41), pd3dDevice);    

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
		for( DWORD i=0; i<PIECE_SIZE; ++i )
		{
			if( !m_PartArray[i].IsCompletedLINK() )
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
		nStart = PIECE_VEHICLE;
		nEnd = -1;
		nAdd = -1;
	}
	else
	{
		nStart = 0;	
		nEnd = PIECE_SIZE;
		nAdd = 1;
	}

	int i = nStart;


	// Note : 조명의 변화는 받지 않는다.
	if( m_bWorldObj )
	{
		DWORD	dwNormalizeNormals;
		pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			DxFogMan::GetInstance().GetFogColor() );

		if( bBoardRender )
		{
			{
				//m_PartArray[PIECE_RHAND].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
				//m_PartArray[PIECE_LHAND].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
				//m_PartArray[PIECE_RHAND_HIDDEN].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
				//m_PartArray[PIECE_LHAND_HIDDEN].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );

				m_spSkinAniControlThread->Render_EffAniPlayer ( pd3dDevice, this, fAlpha );
			}
			pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );


			//	Note : Glow 
			{
				m_PartArray[PIECE_RHAND].RenderGlow( pd3dDevice );
				m_PartArray[PIECE_LHAND].RenderGlow( pd3dDevice );
				m_PartArray[PIECE_RHAND_HIDDEN].RenderGlow( pd3dDevice );
				m_PartArray[PIECE_LHAND_HIDDEN].RenderGlow( pd3dDevice );
			}

			//	Note : Single Eff
			{
				m_PartArray[PIECE_RHAND].RenderEff( pd3dDevice, m_fScale, NULL );
				m_PartArray[PIECE_LHAND].RenderEff( pd3dDevice, m_fScale, NULL );
				m_PartArray[PIECE_RHAND_HIDDEN].RenderEff( pd3dDevice, m_fScale, NULL );
				m_PartArray[PIECE_LHAND_HIDDEN].RenderEff( pd3dDevice, m_fScale, NULL );
			}
		}
		else
		{
			{
				i = nStart;
				while ( i != nEnd )
				{					
					m_PartArray[i].Render ( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), m_arrayfWeaponScale, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), m_sSkinInfo, bShadow, bEff );
					if ( m_pAttBoneArray[i] )
					{
						m_pAttBoneArray[i]->RenderFixed ( pd3dDevice, 1.f, bShadow, bEff, bReverse, m_spThreadNeedData->m_arrayKeyJiggleWeight[i] );
					}
					i += nAdd;
				}

				//i = nStart;
				//while ( i != nEnd )
				//{
				//	m_PartArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
				//	i += nAdd;

				//}

				m_spSkinAniControlThread->Render_EffAniPlayer ( pd3dDevice, this, fAlpha );
			}
			pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );


			//	Note : Glow 
			for ( int i=0; i<PIECE_SIZE; ++i )
			{
				m_PartArray[i].RenderGlow( pd3dDevice );
			}

			//	Note : Single Eff
			for ( int i=0; i<PIECE_SIZE; ++i )
			{
				m_PartArray[i].RenderEff( pd3dDevice, m_fScale, NULL );
			}
		}

		return S_OK;
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
		if ( g_bAmbientTest_CHAREDIT )
		{
			sDestLight00.Ambient.r = static_cast<float>(g_dwAmbient_CHAREDIT)/255.f;
			sDestLight00.Ambient.g = static_cast<float>(g_dwAmbient_CHAREDIT)/255.f;
			sDestLight00.Ambient.b = static_cast<float>(g_dwAmbient_CHAREDIT)/255.f;

		}
		else if ( m_bCharAmbientUP )
		{
			sDestLight00.Ambient.r = 180.f/255.f;
			sDestLight00.Ambient.g = 180.f/255.f;
			sDestLight00.Ambient.b = 180.f/255.f;
		}
		else
		{
			sDestLight00.Ambient.r += sSrcLight00.Diffuse.r * 0.2f;
			sDestLight00.Ambient.g += sSrcLight00.Diffuse.g * 0.2f;
			sDestLight00.Ambient.b += sSrcLight00.Diffuse.b * 0.2f;
			sDestLight00.Ambient.r += sSrcLight00.Ambient.r * 0.2f;
			sDestLight00.Ambient.g += sSrcLight00.Ambient.g * 0.2f;
			sDestLight00.Ambient.b += sSrcLight00.Ambient.b * 0.2f;
		}
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

	if ( bPointLightRT )
	{
		D3DXVECTOR3 vCharPos( m_spSkinAniControlThread->GetWorld()._41, m_spSkinAniControlThread->GetWorld()._42, m_spSkinAniControlThread->GetWorld()._43 );
		DxLightMan::GetInstance()->SetCharPLight( pd3dDevice, vCharPos, FALSE );	//m_bAmbientColor );
	}

	DWORD	dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );

	if ( !g_bWIREFRAME_VIEW && !bFogON )
	{
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	}

	DWORD	dwNormalizeNormals;
	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );


	if( bBoardRender )
	{
		//	m_PartArray[PIECE_RHAND].Render ( pd3dDevice, m_skeletonResThread.get(), m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), bShadow, bEff );
		//	m_PartArray[PIECE_LHAND].Render ( pd3dDevice, m_skeletonResThread.get(), m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), bShadow, bEff );

		//m_PartArray[PIECE_RHAND].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
		//m_PartArray[PIECE_LHAND].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
		//m_PartArray[PIECE_RHAND_HIDDEN].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
		//m_PartArray[PIECE_LHAND_HIDDEN].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );

		m_spSkinAniControlThread->Render_EffAniPlayer ( pd3dDevice, this, fAlpha );
		m_EffKeepPlayer.Render( pd3dDevice );

		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );

		//	Note : Glow 
		m_PartArray[PIECE_RHAND].RenderGlow( pd3dDevice );
		m_PartArray[PIECE_LHAND].RenderGlow( pd3dDevice );
		m_PartArray[PIECE_RHAND_HIDDEN].RenderGlow( pd3dDevice );
		m_PartArray[PIECE_LHAND_HIDDEN].RenderGlow( pd3dDevice );

		//	Note : Single Eff
		{
			m_PartArray[PIECE_RHAND].RenderEff( pd3dDevice, m_fScale, NULL );
			m_PartArray[PIECE_LHAND].RenderEff( pd3dDevice, m_fScale, NULL );
			m_PartArray[PIECE_RHAND_HIDDEN].RenderEff( pd3dDevice, m_fScale, NULL );
			m_PartArray[PIECE_LHAND_HIDDEN].RenderEff( pd3dDevice, m_fScale, NULL );
		}
	}else{
		{			
			switch( m_sSkinEffData.m_bNewHeadDRAW )
			{
			case TRUE:

				i = nStart;
				while ( i != nEnd )
				{
					m_PartArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), m_arrayfWeaponScale, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), m_sSkinInfo, bShadow, bEff );
					if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
						m_pAttBoneArray[i]->RenderFixed ( pd3dDevice, 1.f, bShadow, bEff, bReverse, m_spThreadNeedData->m_arrayKeyJiggleWeight[i] );
					i += nAdd;
				}	
				break;

			case FALSE:
				i = nStart;
				while ( i != nEnd )
				{
					m_PartArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), m_arrayfWeaponScale, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), m_sSkinInfo, bShadow, bEff );
					if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
						m_pAttBoneArray[i]->RenderFixed ( pd3dDevice, 1.f, bShadow, bEff, bReverse, m_spThreadNeedData->m_arrayKeyJiggleWeight[i] );
					i += nAdd;
				}
				break;
			};
			
			//for ( int i=0; i<PIECE_SIZE; ++i )
			//{
			//	m_PartArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
			//}
			m_spSkinAniControlThread->Render_EffAniPlayer ( pd3dDevice, this, fAlpha );
			m_EffKeepPlayer.Render( pd3dDevice );

			pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
		}

		//	Note : Glow 
		for ( int i=0; i<PIECE_SIZE; ++i )
		{
			m_PartArray[i].RenderGlow( pd3dDevice );
		}

		//	Note : Single Eff
		for ( int i=0; i<PIECE_SIZE; ++i )
		{
			m_PartArray[i].RenderEff( pd3dDevice, m_fScale, NULL );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	pd3dDevice->SetLight ( 0, &sSrcLight00 );

	pd3dDevice->LightEnable( 1, FALSE );
	pd3dDevice->LightEnable( 2, FALSE );
	pd3dDevice->LightEnable( 3, FALSE );
	pd3dDevice->LightEnable( 4, FALSE );
	pd3dDevice->LightEnable( 5, FALSE );

	//if ( m_bAmbientColor )
	//{
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwColorOP_0 );
	//}

	return S_OK;
}
//
//HRESULT DxSkinChar::RenderSKIN( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& m_spSkinAniControlThread->GetWorld() )
//{
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
//		AvailableVB_InitVB();
//
//		switch( m_sSkinEffData.m_bNewHeadDRAW )
//		{
//		case TRUE:
//			for ( int i=1; i<PIECE_SIZE; ++i )
//			{
//				m_PartArray[i].Render( pd3dDevice, m_skeletonResThread.get(), m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), FALSE, TRUE );
//			}
//			break;
//
//		case FALSE:
//			for ( int i=0; i<PIECE_SIZE; ++i )
//			{
//				m_PartArray[i].Render( pd3dDevice, m_skeletonResThread.get(), m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), FALSE, TRUE );
//			}
//			break;
//		};
//		
//		for ( int i=0; i<PIECE_SIZE; ++i )
//		{
//			m_PartArray[i].RenderGhosting ( pd3dDevice, this, m_spSkinAniControlThread->GetWorld() );
//		}
//		m_pEffAniPlayer->Render( pd3dDevice, this, m_spSkinAniControlThread->GetWorld(), 1.f );
//		m_EffKeepPlayer.Render( pd3dDevice );
//
//		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
//	}
//
//	//	Note : Glow 
//	for ( int i=0; i<PIECE_SIZE; ++i )
//	{
//		m_PartArray[i].RenderGlow( pd3dDevice, m_skeletonResThread.get() );
//	}
//
//	//	Note : Single Eff
//	for ( int i=0; i<PIECE_SIZE; ++i )
//	{
//		m_PartArray[i].RenderEff( pd3dDevice, m_skeletonResThread.get(), m_fScale, NULL );
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

HRESULT DxSkinChar::RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj, const BOOL bDay )
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

		for ( int i=0; i<PIECE_SIZE; ++i )
		{
			m_PartArray[i].RenderShadowForMaterial( pd3dDevice, matViewProj, FALSE );

			if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			{
				m_pAttBoneArray[i]->RenderShadow( pd3dDevice, m_matWorld, matViewProj, bDay );
			}
		}

		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
	}
	else	// 하드웨어 성능이 무지 나쁜경우.
	{
		for ( int i=0; i<PIECE_SIZE; ++i )
		{
			m_PartArray[i].RenderShadow( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), fHeight );

			if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			{
				m_pAttBoneArray[i]->RenderShadow( pd3dDevice, m_matWorld, matViewProj, bDay );
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	return S_OK;
}

HRESULT DxSkinChar::RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj )
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

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].RenderShadowForMaterial( pd3dDevice, matViewProj, TRUE );

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->RenderShadowForMaterialSystem( pd3dDevice, matViewProj );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	return S_OK;
}

BOOL DxSkinChar::RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex )
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

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		if ( !m_PartArray[i].RenderMaterialGhosting( pd3dDevice, m_sSkinEffData, m_sSkinInfo, fAlpha, unGhostingIndex ) )
			return FALSE;

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			if ( !m_pAttBoneArray[i]->RenderMaterialGhosting( pd3dDevice, fAlpha, unGhostingIndex ) )
				return FALSE;
		}
	}

	return TRUE;
}

HRESULT DxSkinChar::RenderReflect( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
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

	// 일정 이상 사양이 되야만 반사 렌더링 되도록 작업함.

	//////////////////////////////////////////////////////////////////////////
	//								Opaque
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].RenderMaterialOpaque
		(
			pd3dDevice, 
			*m_spSkinAniControlThread->GetSkeletonMaintain(),
			m_arrayfWeaponScale,
			m_sSkinEffData, 
			fAlpha, 
			m_sSkinInfo, 
			NULL,
			m_spSkinAniControlThread->GetWorld(), 
			m_fScale,
			m_spSkinAniControlThread->GetClothColl(),
			m_spSkinAniControlThread->GetClothCollSphere(),
			1.f,
			FALSE
		);

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->RenderMaterialOpaque
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


	//////////////////////////////////////////////////////////////////////////
	//							Hard Alpha
	DWORD dwAlphaTestEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].RenderMaterialHardAlpha( pd3dDevice, fAlpha, m_sSkinInfo );

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_pAttBoneArray[i]->RenderMaterialHardAlpha( pd3dDevice, fAlpha );
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );


	//////////////////////////////////////////////////////////////////////////
	//							Soft Alpha
	DWORD dwAlphaBlendEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].RenderMaterialSoftAlpha( pd3dDevice, fAlpha, m_sSkinInfo );

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_pAttBoneArray[i]->RenderMaterialSoftAlpha( pd3dDevice, fAlpha );
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );

	return S_OK;
}

void DxSkinChar::RenderBone( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return;
	}
	m_spSkinAniControlThread->RenderBone( pd3dDevice );
}

void DxSkinChar::RenderAniEff( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// 로딩이 다 되지 않았다면 렌더하면 안된다. 모든 정보가 불안한 상태이다.
		return;
	}

	D3DLIGHTQ	sSrcLight00;
	D3DLIGHTQ	sDestLight00;

	pd3dDevice->GetLight ( 0, &sSrcLight00 );

	sDestLight00 = sSrcLight00;

	sDestLight00.Ambient.r += sSrcLight00.Diffuse.r * 0.2f;
	sDestLight00.Ambient.g += sSrcLight00.Diffuse.g * 0.2f;
	sDestLight00.Ambient.b += sSrcLight00.Diffuse.b * 0.2f;
	sDestLight00.Ambient.r += sSrcLight00.Ambient.r * 0.2f;
	sDestLight00.Ambient.g += sSrcLight00.Ambient.g * 0.2f;
	sDestLight00.Ambient.b += sSrcLight00.Ambient.b * 0.2f;
	sDestLight00.Diffuse.r = sSrcLight00.Diffuse.r * 0.6f;
	sDestLight00.Diffuse.g = sSrcLight00.Diffuse.g * 0.6f;
	sDestLight00.Diffuse.b = sSrcLight00.Diffuse.b * 0.6f;

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
	DxLightMan::GetInstance()->SetCharPLight( pd3dDevice, vCharPos, FALSE );

	DWORD	dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD	dwNormalizeNormals;
	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

	m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, 1.f );
	m_EffKeepPlayer.Render( pd3dDevice );


	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	pd3dDevice->SetLight ( 0, &sSrcLight00 );

	pd3dDevice->LightEnable( 1, FALSE );
	pd3dDevice->LightEnable( 2, FALSE );
	pd3dDevice->LightEnable( 3, FALSE );
	pd3dDevice->LightEnable( 4, FALSE );
	pd3dDevice->LightEnable( 5, FALSE );

}

BOOL DxSkinChar::RenderBillboard( const D3DXMATRIX& matTrans, float fBillboardAlpha )
{
	if ( !m_spBillboard.get() )
		return FALSE;

	D3DXVECTOR3 vCharPos( matTrans._41, matTrans._42, matTrans._43 );
	m_spBillboard->Render( vCharPos, fBillboardAlpha );

	return TRUE;
}

HRESULT DxSkinChar::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : m_PartArray, m_EffAniPlayer 은 다른 경로에서 초기화를 해 준다.

	return S_OK;
}

HRESULT DxSkinChar::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].RestoreDeviceObjects ( pd3dDevice );

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->RestoreDeviceObjects( pd3dDevice );
		}
	}

	m_spSkinAniControlThread->RestoreDeviceObjects();

	return S_OK;
}

HRESULT DxSkinChar::InvalidateDeviceObjects ()
{
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].InvalidateDeviceObjects ();

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->InvalidateDeviceObjects();
		}
	}

	m_spSkinAniControlThread->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT DxSkinChar::DeleteDeviceObjects ()
{
	m_spSkinAniControlThread->DeleteDeviceObjects();

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].DeleteDeviceObjects ();

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->DeleteDeviceObjects();
		}
	}

	//m_pEffAniPlayer->DeleteDeviceObjects ();

	//m_spJiggleBone->CleanUp();
	//m_spJiggleBoneColl->CleanUp();

	return S_OK;
}

void DxSkinChar::ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].ReloadVMFX( pd3dDevice );
	}
}

void DxSkinChar::GetStaticVertexData( std::vector<VERTEXNORCOLORTEX4>& vecVertices, const D3DXMATRIX& matWorld )
{
	////	Note : Skin Animation.
	//m_spSkinAniControlThread->FrameMove( 0.f );
	//m_spSkinAniControlThread->CalculateBone_PureThread_HARD( matWorld );	// 굳이 돌릴 필요는 없다.

	//for ( int i=0; i<PIECE_SIZE; ++i )
	//{
	//	m_PartArray[i].GetStaticVertexData( vecVertices, m_spSkinAniControlThread->GetSkeletonForEdit() );
	//}
}

//------------------------------------------------------------
//				E	f	f		K	e	e	p
void DxSkinChar::CreateEff( DxEffKeepDataMain* pEff )
{
	m_EffKeepPlayer.CreateEff( pEff );
}

void DxSkinChar::DeleteEff( DxEffKeepDataMain* pEff )
{
	
}

void DxSkinChar::Reset()
{
	m_EffKeepPlayer.Reset();
}


//void DxSkinChar::SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert )
//{
//	for ( DWORD i=0; i<PIECE_SAVE_SIZE; ++i )
//	{
//		m_PartArray[i].SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//	}
//}

void DxSkinChar::UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    //for (DWORD i = 0; i < PIECE_SIZE; ++i)
    //{
    //    m_PartArray[i].UpdateLOD(vPos, pd3dDevice);
    //}
}

void DxSkinChar::SetInteractiveScale( const float fScale )
{
    m_fScale *= fScale;
    m_vMax *= fScale;
    m_vMin *= fScale;
    m_fHeight *= fScale;
    m_fHeightYPos *= fScale;
}

void DxSkinChar::SetPosOffset( const D3DXVECTOR3& vPosOffset )
{
	//////////////////////////////////////////////////////////////////////////
	// 
	if ( vPosOffset.x == 0.f && vPosOffset.y == 0.f && vPosOffset.z == 0.f)
		return;

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].SetPosOffset( vPosOffset );

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->SetPosOffset( vPosOffset );
		}
	}
}

void DxSkinChar::RenderMaterialOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	//UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&m_matWorld._41), pd3dDevice);

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
		for( DWORD i=0; i<PIECE_SIZE; ++i )
		{
			if( !m_PartArray[i].IsCompletedLINK() )
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

	// 
	SetDiffuseColor_NSMaterialSkinManager();
	SetLookatLength_NSMaterialSkinManager();
	SetPosition_NSMaterialSkinManager();

	int nStart=0, nEnd = 0, nAdd = 0;
	{
		nStart = 0;	
		nEnd = PIECE_SIZE;
		nAdd = 1;
	}

	int i = nStart;

	i = nStart;
	while ( i != nEnd )
	{
		m_PartArray[i].RenderMaterialOpaque
		( 
			pd3dDevice, 
			*m_spSkinAniControlThread->GetSkeletonMaintain(),
			m_arrayfWeaponScale,
			m_sSkinEffData, 
			fAlpha, 
			m_sSkinInfo, 
			NULL, 
			m_spSkinAniControlThread->GetWorld(), 
			m_fScale, 
			m_spSkinAniControlThread->GetClothColl(),
			m_spSkinAniControlThread->GetClothCollSphere(),
			1.f,
			TRUE
		);

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->RenderMaterialOpaque
			(
				pd3dDevice, 
				fAlpha,
				FALSE, 
				TRUE,
				m_spSkinAniControlThread->GetClothColl(),
				m_spSkinAniControlThread->GetClothCollSphere(),
				m_fScale
			);
		}
		i += nAdd;
	}
}

void DxSkinChar::RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
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

	//DWORD dwAlphaREF(0L);
	DWORD dwAlphaTestEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
	//pd3dDevice->GetRenderState( D3DRS_ALPHAREF,			&dwAlphaREF );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x40 );

	int nStart=0, nEnd = 0, nAdd = 0;

	{
		nStart = 0;	
		nEnd = PIECE_SIZE;
		nAdd = 1;
	}

	int i = nStart;

	i = nStart;
	while ( i != nEnd )
	{
		m_PartArray[i].RenderMaterialHardAlpha( pd3dDevice, fAlpha, m_sSkinInfo );
		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_pAttBoneArray[i]->RenderMaterialHardAlpha( pd3dDevice, fAlpha );
		i += nAdd;
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			dwAlphaREF );
}

void DxSkinChar::RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
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
	SetPosition_NSMaterialSkinManager();;

	DWORD dwAlphaBlendEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	int nStart=0, nEnd = 0, nAdd = 0;
	{
		nStart = 0;	
		nEnd = PIECE_SIZE;
		nAdd = 1;
	}

	int i = nStart;

	i = nStart;
	while ( i != nEnd )
	{
		m_PartArray[i].RenderMaterialSoftAlpha( pd3dDevice, fAlpha, m_sSkinInfo );
		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_pAttBoneArray[i]->RenderMaterialSoftAlpha( pd3dDevice, fAlpha );
		i += nAdd;
	}

	// 되돌림.
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
}

void DxSkinChar::RenderMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
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
	SetPosition_NSMaterialSkinManager();;

	DWORD dwAlphaBlendEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	//	Note : Single Eff
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].RenderEff( pd3dDevice, m_fScale, NULL );
		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
			m_pAttBoneArray[i]->RenderEff( pd3dDevice, fAlpha );
	}

	m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, fAlpha );


	int nStart=0, nEnd = 0, nAdd = 0;
	{
		nStart = 0;	
		nEnd = PIECE_SIZE;
		nAdd = 1;
	}

	//////////////////////////////////////////////////////////////////////////
	//	Note : Add
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	int i = nStart;
	while ( i != nEnd )
	{
		m_PartArray[i].RenderMaterialAdd( pd3dDevice );
		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->RenderMaterialAdd( pd3dDevice );
		}
		i += nAdd;
	}
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
	



	//////////////////////////////////////////////////////////////////////////
	//	Note : Glow 
	i = nStart;
	while ( i != nEnd )
	{
		m_PartArray[i].RenderMaterialGlow( pd3dDevice );
		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->RenderMaterialGlow( pd3dDevice );
		}
		i += nAdd;
	}

	// 되돌림.
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
}

//////////////////////////////////////////////////////////////////////////
void DxSkinChar::GhostingON( BOOL bGhosting, float fLifeTime )
{
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].GhostingON( bGhosting, fLifeTime );

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->GhostingON( bGhosting, fLifeTime );
		}
	}
}

void DxSkinChar::SetDiffuseColor_NSMaterialSkinManager()
{
	NSMaterialSkinManager::SetDiffuseColor( m_vDiffuseColor, m_vPointLightColor );
}

void DxSkinChar::SetLookatLength_NSMaterialSkinManager()
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

void DxSkinChar::SetPosition_NSMaterialSkinManager()
{
	const D3DXMATRIX& matWorld = m_spSkinAniControlThread->GetWorld();
	D3DXVECTOR3 vPos( matWorld._41, matWorld._42, matWorld._43 );

	NSMaterialSkinManager::SetPosition_NSMaterialSkinManager( vPos );
}

const D3DXMATRIX& DxSkinChar::GetSkeletonRootMatrix() const
{
	return m_spSkinAniControlThread->GetWorld();
}



//////////////////////////////////////////////////////////////////////////
// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수

// 이곳은 매 프레임 들어오도록 한다.
void DxSkinChar::SetPhysX_Cloth( BOOL bOnOff )
{
	// 셋팅값도 같도 정보도 변하지 않았다면 내부로 들어갈 필요가 없다.
	if ( m_bPhysX_Cloth_OnOff == bOnOff && m_bCompleted_PhysX_Cloth )
		return;

	for ( DWORD i=0; i<PIECE_SIZE; ++i )
	{
		if ( !m_PartArray[i].SetPhysX_Cloth( bOnOff ) )
			return;	// 작업 완료 안되고 실패.

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			if ( !m_pAttBoneArray[i]->SetPhysX_Cloth( bOnOff ) )
				return;	// 작업 완료 안되고 실패.
		}
	}

	// 다 작업되면 값 셋팅.
	m_bPhysX_Cloth_OnOff = bOnOff;
	m_bCompleted_PhysX_Cloth = TRUE;
}

// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
//////////////////////////////////////////////////////////////////////////

void DxSkinChar::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	for ( DWORD i=0; i<PIECE_SIZE; ++i )
	{
		m_PartArray[i].SetWorldBattleFlag( pWorldBattleFlag );

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			m_pAttBoneArray[i]->SetWorldBattleFlag( pWorldBattleFlag );
		}
	}
}

const TCHAR* DxSkinChar::GetBoneName() const
{
	if ( !m_spSkinAniControlThread.get() )
		return NULL;
		
	return m_spSkinAniControlThread->GetSkeletonName();
}

void DxSkinChar::CreateChfBillboard( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bScreenRender, const TCHAR* szNameCHF_bSaveBillboard )
{
	BILLBOARD_CHF sBillboard( *NSBillboard::GetBillboardChfData(), _T("") );

	UINT uiTexWidth( sBillboard.m_dwTexSizeX );
	UINT uiTexHeight( sBillboard.m_dwTexSizeY );

	float fWidth( sBillboard.m_fWidth );
	float fHeight( sBillboard.m_fHeight );

	D3DXVECTOR3 vUp(0.f,1.f,0.f);
	D3DXVECTOR3 vCross;
	D3DXVec3Cross( &vCross, &DxViewPort::GetInstance().GetLookDir_Y0(), &vUp );
	vCross *= sBillboard.m_fRightOffset;

	D3DXVECTOR3 vLookatPt( 0.f, fHeight*0.5f, 0.f );
	D3DXVECTOR3 vFromPt = vLookatPt - (DxViewPort::GetInstance().GetLookDir_Y0()*200.f);
	vLookatPt.y	+= sBillboard.m_fUpOffset;
	vFromPt.y	+= sBillboard.m_fUpOffset;
	vLookatPt	+= vCross;
	vFromPt		+= vCross;

	// Note : View Port를 설정한다.
	D3DXMATRIX matView;
	D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
	D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );

	// Note : Projection를 설정한다.
	D3DXMATRIX matProj;
	float fNearPlane = 5.f;
	float fFarPlane = 1000.f;
	D3DXMatrixOrthoLH( &matProj, fWidth, fHeight, fNearPlane, fFarPlane );

	//DxViewPort::GetInstance().SetViewTrans( vFromPt, vLookatPt, vUpVec );
	//DxViewPort::GetInstance().SetProjectionOrthoLH( fWidth, fHeight, fNearPlane, fFarPlane );

	NSMaterialSkinManager::FrameMove( matView, matProj );

	// Note : SetRenderState
	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	// Note : 백업과 셋팅
	D3DXMATRIX matViewPREV, matProjPREV;
	pd3dDevice->GetTransform( D3DTS_VIEW,		&matViewPREV );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjPREV );
	pd3dDevice->SetTransform( D3DTS_VIEW,		&matView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DFORMAT emFormat( D3DFMT_A8R8G8B8 );
	IDirect3DTexture9*	pTexture(NULL);
	LPDIRECT3DSURFACEQ	pSurface(NULL);
	LPDIRECT3DSURFACEQ	pZBuffer(NULL);
	TextureManager::GetInstance().CreateTexture
	(
		pd3dDevice,
		uiTexWidth, 
		uiTexHeight, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		emFormat,
		D3DPOOL_DEFAULT,
		&pTexture,
		TEXTURE_RESIZE_NONE,
		TEXTURE_CATEGORY_SIZE 
	);
	pTexture->GetSurfaceLevel( 0, &pSurface );

	pd3dDevice->CreateDepthStencilSurface
	( 
		uiTexWidth,
		uiTexHeight,
		DxSurfaceTex::GetInstance().GetZFormat(),
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&pZBuffer,
		NULL 
	);

	// 
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice, DxBackupTarget::ALL );

		pd3dDevice->SetRenderTarget( 0, pSurface );
		pd3dDevice->SetDepthStencilSurface( pZBuffer );
		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x0L, 1.0f, 0L);

		pd3dDevice->BeginScene();
		{
			//// Note : FrameMove
			//D3DXMATRIX matIdentity;
			//D3DXMatrixIdentity( &matIdentity );
			//FrameMove( 0.f, 0.f, FALSE, matIdentity );
			//SELECTANI( AN_GUARD_N, AN_SUB_NONE );

			////////////////////////////////////////////////////////////////////////////
			//// NSSkinAniThread
			//NSSkinAniThread::CalcluateSkinAniControlForCharEditer( 0.f );

			//SetPhysX_Cloth( FALSE );

			RenderMaterialOpaque( pd3dDevice, 1.f  );
			RenderMaterialHardAlpha( pd3dDevice, 1.f );
			RenderMaterialSoftAlpha( pd3dDevice, 1.f );
		}
		pd3dDevice->EndScene();

		sBackupTarget.Restore();


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// Billboard 를 파일로 저장한다.
		//////////////////////////////////////////////////////////////////////////
		if ( szNameCHF_bSaveBillboard )
		{
			// 폴더가 없을 수도 있으므로 만들도록 한다.
			CreateDirectory ( NSBillboard::GetTexturePath(), NULL );

			// Note : 텍스쳐를 압축한다.
			LPDIRECT3DBASETEXTURE9 pTexRGB(NULL);
			NSTexCompress::Compress
			(
				pd3dDevice,
				uiTexWidth,
				uiTexHeight,
				0,
				1,
				D3DFMT_A8R8G8B8,
				pTexture,
				pTexRGB
			);

			LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
			NSTexCompress::Compress
			(
				pd3dDevice,
				uiTexWidth,
				uiTexHeight,
				0,
				1,
				D3DFMT_DXT1,
				pTexRGB,
				pTexNew
			);

			// Note : 버퍼에 그려진 것을 텍스쳐로 뽑는다.
			TSTRING strTexture = sc::util::GetName_ExtErase( szNameCHF_bSaveBillboard );
			strTexture += _T("_blb.dds");
			strTexture = NSBillboard::GetTexturePath() + strTexture;

			D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_DDS, pTexNew, NULL );

			SAFE_RELEASE( pTexRGB );
			SAFE_RELEASE( pTexNew );
		}
		//////////////////////////////////////////////////////////////////////////
		// Billboard 를 파일로 저장한다.
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 메인 화면에 생성한 billboard 를 렌더링 하도록 한다.
	//////////////////////////////////////////////////////////////////////////
	if( bScreenRender )
	{
		pd3dDevice->BeginScene();

		DWORD dwZEnable(FALSE);
		DWORD dwZWriteEnable(FALSE);
		pd3dDevice->GetRenderState( D3DRS_ZENABLE,		&dwZEnable );
		pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE, &dwZWriteEnable );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		DWORD dwColorOP_1(0L);
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP_1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		VERTEXRHW sVertex[4];
		sVertex[0].vPos = D3DXVECTOR4( 0.f,									0.f,								1.f, 1.f );
		sVertex[1].vPos = D3DXVECTOR4( static_cast<float>( uiTexWidth ),	0.f,								1.f, 1.f );
		sVertex[2].vPos = D3DXVECTOR4( 0.f,									static_cast<float>( uiTexHeight ),	1.f, 1.f );
		sVertex[3].vPos = D3DXVECTOR4( static_cast<float>( uiTexWidth ),	static_cast<float>( uiTexHeight ),	1.f, 1.f );
		sVertex[0].vTex = D3DXVECTOR2( 0.f, 0.f );
		sVertex[1].vTex = D3DXVECTOR2( 1.f, 0.f );
		sVertex[2].vTex = D3DXVECTOR2( 0.f, 1.f );
		sVertex[3].vTex = D3DXVECTOR2( 1.f, 1.f );

		pd3dDevice->SetFVF( VERTEXRHW::FVF );
		pd3dDevice->SetTexture( 0, pTexture );
		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertex, sizeof(VERTEXRHW) );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP_1 );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,				dwZEnable );

		pd3dDevice->EndScene();
	}
	//////////////////////////////////////////////////////////////////////////
	// 메인 화면에 생성한 billboard 를 렌더링 하도록 한다.
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	// Note : 롤백
	pd3dDevice->SetTransform( D3DTS_VIEW,		&matViewPREV );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjPREV );



	SAFE_RELEASE( pTexture );
	SAFE_RELEASE( pSurface );
	SAFE_RELEASE( pZBuffer );
}





//////////////////////////////////////////////////////////////////////////
//						AnimationMove
//////////////////////////////////////////////////////////////////////////

// 애니메이션 움직임이 캐릭터에 영향을 받는 것인지 확인
BOOL DxSkinChar::IsAnimationMove()
{
	return m_spSkinAniControlThread->IsAnimationMove();
}

// 캐릭터가 애니메이션으로 얼마만큼 움직였는지 확인
const D3DXVECTOR3& DxSkinChar::GetAnimationMovePos()
{
	return m_spSkinAniControlThread->GetAnimationMovePos();
}

//// AnimationMove 의 시작 Matrix
//const D3DXMATRIX& DxSkinChar::GetAnimationMoveStartMatrix()
//{
//	return m_spSkinAniControlThread->GetAnimationMoveStartMatrix();
//}

D3DXVECTOR3 DxSkinChar::GetAnimationMoveOffsetFull_EDIT()
{
	return m_spSkinAniControlThread->GetAnimationMoveOffsetFull_EDIT();
}

//////////////////////////////////////////////////////////////////////////	