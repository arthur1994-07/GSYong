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
//	// ���� �ε��� ������ Attach Bone Link �����̸�...
//	if( strExt == _T(".ABL") )
//	{
//		//	Note : ���� ó�� �ε�� ���� �����ϰ� ������ �ε��Ҷ����� ���� �ε�Ȱ� �����ش�.
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
//		//	Note : Attach Bone�� �о�´�.
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
//		//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
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
//		//	Note : ���� ó�� �ε�� ���� �����ϰ� ������ �ε��Ҷ����� ���� �ε�Ȱ� �����ش�.
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
//		//	Note : Piece�� �о�´�.
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
//			//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
//			//
//			if ( m_pAttBoneArray[pSkinPiece->m_emPieceCharType].GetFileName() )
//			{
//				m_pAttBoneArray[pSkinPiece->m_emPieceCharType].ClearAll();
//			}
//		}
//
//		//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
//		//
//		if ( m_PartArray[pSkinPiece->m_emPieceCharType].GetFileName() )
//		{
//			m_PartArray[pSkinPiece->m_emPieceCharType].ReSetPart ();
//		}
//		
//		// Note : ���ڿ� ������ �̻��ϰ� �Ǿ �̷��� �ؾ� �Ѵ�.
//		if( pSkinPiece->m_emPieceCharType == PIECE_HAIR )
//		{
//			if( m_PartArray[PIECE_HEADGEAR].GetFileName() )
//			{
//				// �ϴ� ����
//				m_PartArray[PIECE_HEADGEAR].ReSetPart ();
//
//				// ������ ����
//				DxSkinPiece* pSkinPieceHG = DxSkinPieceContainer::GetInstance().LoadPiece( m_PartArray[PIECE_HEADGEAR].GetFileName(), pd3dDevice, TRUE );
//				if ( !pSkinPieceHG )	goto _RETURN;
//				m_PartArray[PIECE_HEADGEAR].SetPart( pSkinPieceHG, pd3dDevice, bCharacterData );
//			}
//		}
//		_RETURN:
//
//		//	Note : Piece �� ����.
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
//	// Note : ���� ������ �� ����
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


// [shhan][2015.08.27] AttachBone ������ ���� ��
// ������ : �κ��� ��Ʈ, attachbone ������ ��ü
//			�κ��� ��ü, attachbone ������ ��ü�� �����ϸ� ���� �ϳ��� ������� ��.
// 
// �ذ����� �ƴ� ������ �Ȼ���� �ϱ����� ��ġ
// 1. �κ��� ��Ʈ, attachbone ������ ��ü
// 2. ���� attachbone �� ������ cape. ����� src_cps �� ��ü�� ��. cps ��ü�� �κ��� ��ü.
//   �ᱹ �κ��� ��ü��, cps �� ��ü, attachbone �� cape �� ���õǾ� ��.
// 3. � ������ �κ��� ��ü, cps ��ü�� attachbone ���ڷ� �Ǿ��ִ°� �ִٰ� ��.
//   �ᱹ �κ��� ��ü��, cps �� ��ü, attachbone �� ���ڷε� ���õǾ� ��.
// 4. �κ��� ����, cps ���ڴ� ���ο� ������ �������� �´°� ����.
//
// ���� ���߽� ���� �� ��.
// 1. �κ��̶� cps �� ������ ��ȿ�� �˻縦 �� �ϰ� �ؾ���.
// 2. cps�� attbone �� ������ ���ƾ� ��� ������ ����.
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

	// ���� �ε��� ������ Attach Bone Link �����̸�...
	if( strExt == _T(".ABL") )
	{
		////	Note : ���� ó�� �ε�� ���� �����ϰ� ������ �ε��Ҷ����� ���� �ε�Ȱ� �����ش�.
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

		//	Note : Attach Bone�� �о�´�.
		//
        DxAttBoneLink* pAttBoneLink = DxAttBoneLinkContainer::GetInstance().LoadLink( strFile, emFlags );
		if (!pAttBoneLink)
            return E_FAIL;

		// [shhan][2015.06.08] Link �� �ƴ� Skin �� ���� �ؾ� �ϰ� ThreadLoading �̱� ������
		//						DxCharPart::SetPart_Detail Ÿ�ֿ̹��� Ȯ���ϴ� ������ ����Ǿ���.
		//
		//// [shhan][2014.11.26] Bone ������ �ٸ� ��� ���� Part �� ������ �ȵȴ�.
		////						DxSkeletonMaintain::FindBone_Index ���� NULL �� ���ϵǸ鼭 ������ ���� �� �ִ�.
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

		//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
		//
		if ( m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] && m_pAttBoneArray[pAttBoneLink->m_emPieceCharType]->GetFileName() )
		{
			m_pAttBoneArray[pAttBoneLink->m_emPieceCharType]->ClearAll();
			SAFE_DELETE ( m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] );
		}

		// Ÿ���� �ٸ��ٸ� �۵����� �ʵ��� �Ѵ�.
		if ( pCheckPieceType )
		{
			if ( *pCheckPieceType != pAttBoneLink->m_emPieceCharType )
				return E_FAIL;
		}
		
		// bAttachSub TRUE ���, �۾��Ǹ� �ȵȴ�.
		// ù SetPiece �� ��츸 (bAttachSub FALSE) NSChangeCPS::GetNewCPS �� ���۵ǵ��� �Ѵ�.
		if ( !pCheckPieceType )
		{
			if ( GetBoneName() && pAttBoneLink->GetBaseBoneName() )
			{
				CString strFile1( GetBoneName() );
				strFile1.MakeLower();

				CString strFile2( pAttBoneLink->GetBaseBoneName() );
				strFile2.MakeLower();

				// Bone �� �ٸ��ٸ� NSChangeCPS �� üũ�ؾ� ��.
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
		////	Note : ���� ó�� �ε�� ���� �����ϰ� ������ �ε��Ҷ����� ���� �ε�Ȱ� �����ش�.
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

		//	Note : Piece�� �о�´�.
		DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( strFile, emFlags );
		if ( !pSkinPiece )
			return E_FAIL;

		// [shhan][2015.06.08] Link �� �ƴ� Skin �� ���� �ؾ� �ϰ� ThreadLoading �̱� ������
		//						DxCharPart::SetPart_Detail Ÿ�ֿ̹��� Ȯ���ϴ� ������ ����Ǿ���.
		//
		//// [shhan][2014.11.26] Bone ������ �ٸ� ��� ���� Part �� ������ �ȵȴ�.
		////						DxSkeletonMaintain::FindBone_Index ���� NULL �� ���ϵǸ鼭 ������ ���� �� �ִ�.
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

		// cps �� ����Ǹ� ���� �������� AttBone �� ���õ� ���� ������� �Ѵ�.
		// ������ AttBone������ cps �� ���� �����ϴ°� �ֱ� ������ AttBone�� ���� AttBone �� �������� �ʴ´�.
		if( !bAttachSub )
		{
			//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
			//
			if ( m_pAttBoneArray[pSkinPiece->m_emType] && m_pAttBoneArray[pSkinPiece->m_emType]->GetFileName() )
			{
				m_pAttBoneArray[pSkinPiece->m_emType]->ClearAll();
				SAFE_DELETE ( m_pAttBoneArray[pSkinPiece->m_emType] );
			}
		}

		// Ÿ���� �ٸ��ٸ� �۵����� �ʵ��� �Ѵ�.
		if ( pCheckPieceType )
		{
			if ( *pCheckPieceType != pSkinPiece->m_emType )
				return E_FAIL;
		}

		// bAttachSub TRUE ���, �۾��Ǹ� �ȵȴ�.
		// ù SetPiece �� ��츸 (bAttachSub FALSE) NSChangeCPS::GetNewCPS �� ���۵ǵ��� �Ѵ�.
		if( !bAttachSub )
		{
			if ( GetBoneName() &&  pSkinPiece->GetBoneName() )
			{
				CString strFile1( GetBoneName() );
				strFile1.MakeLower();

				CString strFile2( pSkinPiece->GetBoneName() );
				strFile2.MakeLower();

				// Bone �� �ٸ��ٸ� NSChangeCPS �� üũ�ؾ� ��.
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

						// SkinMesh ��� Bone �ٸ���� ������ �Ұ��� ��.
						if( pmcMesh->pSkinInfo )
							return E_FAIL;
					}
				}
			}
		}

		////	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
		////
		//if ( m_PartArray[pSkinPiece->m_emType].GetFileName() )
		//{
		//	m_PartArray[pSkinPiece->m_emType].ReSetPart();
		//}

		//// Note : ���ڴ� HAIR�� �ٲ��� �ϱ⶧���� ������ PIECE_HAIR �� �Ǿ��ִ�.
		//if( pSkinPiece->m_emType == PIECE_HAIR )
		//{
		//	if( m_PartArray[PIECE_HEADGEAR].IsMeshContainer() )
		//	{
		//		// �ϴ� ����
		//		m_PartArray[PIECE_HEADGEAR].ReSetPart();

		//		// ������ ����
		//		DxSkinPiece* pSkinPieceHG = DxSkinPieceContainer::GetInstance().LoadPiece( m_PartArray[PIECE_HEADGEAR].GetFileName(), pd3dDevice, bThread );
		//		if ( !pSkinPieceHG )
		//			goto _RETURN;
		//		m_PartArray[PIECE_HEADGEAR].SetPart( pSkinPieceHG, pd3dDevice, bAmbientUP , TRUE, PIECE_HEADGEAR );
		//	}
		//}

//_RETURN:

		//	Note : Piece �� ����.
		//
		m_PartArray[pSkinPiece->m_emType].SetPart ( this, pSkinPiece, pd3dDevice, m_bCharAmbientUP, pSelfBodyEffect );
		m_PartArray[pSkinPiece->m_emType].SetGrindLevel( nLevel );
		//m_PartArray[pSkinPiece->m_emType].SetColorKey( lColorKey );

		// [ Thread Work shhan ]
		//// HEAD�� ��츸 ������ �����ϵ��� �Ѵ�.
		//SetSexFaceID( pSkinPiece->m_emType );

		//if( pFaceCHF )
		//{
		//	//pFaceCHF->SetPiece( strFile, pd3dDevice, dwFlag, nLevel, lColorKey, bThread, NULL );
		//	pFaceCHF->SetPiece( strFile, pd3dDevice, dwFlag, nLevel, bThread, NULL );
		//}

		if( pSkinPiece->m_emType == PIECE_HEADGEAR && pSkinPiece->IsHatHairOption( HAT_HAIR_NEW ) && !pSkinPiece->GetAddHairCps().empty() )
		{
			// HAT_HAIR_NEW - ���ο� �� ���;� �Ѵ�.
			SetPiece( pSkinPiece->GetAddHairCps().c_str(), pd3dDevice, dwFlag, nLevel, emFlags, FALSE, NULL, pSelfBodyEffect );
		}

		if ( pOutPiecetype )
		{
			*pOutPiecetype = pSkinPiece->m_emType;
		}
	}

	// ���� ������ PhysX ���� �� ������ �ؾ� �Ѵٶ�� �����Ѵ�.
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

	//	Note : Piece�� �о�´�.
	DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( strFile, EMSCD_ALL_THREAD );
	if ( !pSkinPiece )
		return E_FAIL;

	emType = pSkinPiece->m_emType;
	if ( emType != PIECE_RHAND_HIDDEN && emType != PIECE_LHAND_HIDDEN )
		return E_FAIL;

	// cps �� ����Ǹ� ���� �������� AttBone �� ���õ� ���� ������� �Ѵ�.
	// ������ AttBone������ cps �� ���� �����ϴ°� �ֱ� ������ AttBone�� ���� AttBone �� �������� �ʴ´�.
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

		// Bone �� �ٸ��ٸ� NSChangeCPS �� üũ�ؾ� ��.
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

				// SkinMesh ��� Bone �ٸ���� ������ �Ұ��� ��.
				if( pmcMesh->pSkinInfo )
					return E_FAIL;
			}
		}
	}

	// ������ Parts ��� FALSE �� ���� �� �ִ�.
	// �ٸ� Parts ��� TRUE �� �����鼭 m_bCompleted_PhysX_Cloth FALSE �� ���ش�.
	if ( m_PartArray[emType].SetPart ( this, pSkinPiece, pd3dDevice, m_bCharAmbientUP, pSelfBodyEffect ) )
	{
		// ���� ������ PhysX ���� �� ������ �ؾ� �Ѵٶ�� �����Ѵ�.
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

	// ���� ����
	WORD wR, wG, wB;
	wR = (WORD)((dwColor&0xff0000)>>16);
	wG = (WORD)((dwColor&0xff00)>>8);
	wB = (WORD)(dwColor&0xff);

	// �����
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
		//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
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

	// ���� ���� Hair �� �����ϴ� ���̾��ٸ� �ǵ����� �Ѵ�.
	if ( i == PIECE_HEADGEAR && m_PartArray[i].IsHatHairOption( HAT_HAIR_NEW ) && !m_PartArray[i].GetAddHairCps().empty() )
	{
		ResetPiece ( PIECE_HAIR_4_HAT );
	}

	//	Note : ������ �÷��� Part �� �ִٸ� �̸� �����ش�.
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
	// [CRM #3013] ���� ������� ���� DxSkinChar�� ���� �Ǹ鼭 DxSkinCharData ���ڰ��� �ٸ��͵��� �Ѿ� �� �� �ִ�. �׷��鼭 �� ����.
	//				Ÿ �����忡�� �����ϰ� ���� �� �����Ƿ� m_spSkinAniControlThread->ClearAll, m_spSkinAniControlThread->SetCharData ��.. �۾��� �������� ���� ���� ���� �� ����.
	//				������ �ٲ�� ���� ĳ���Ͱ� ��� �Ⱥ��̴��� ���� ������ ���ִ� �������� �Ѵ�.
	{
		if ( bMaintainAnim == FALSE )
			m_spSkinAniControlThread = std::tr1::shared_ptr<DxSkinAniControl>(new DxSkinAniControl);

		m_spThreadNeedData = std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>(new DxSkinAniControlThreadNeedData(PIECE_SIZE));

		for ( int i=0; i<PIECE_SIZE; ++i )
		{
			//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
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

	bJiggleBone = TRUE;		// ���ۺ� ������ Ȱ��ȭ

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

		// [shhan][2015.03.25] ��� Color �� �ʱ�ȭ ���ֵ��� �Ѵ�. ( CharEdit ���� �� �ʿ��� ����� )
		//						������ ������ ���� Color �� �����־ cps ���� ������ ���� �ȳ��´�.
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

		// �˸´� Slot ã��
		// cps �� chf �� ���� ���� �� �� cps�� ���� ������ �����ϰ�, chf �� ���� ���ϸ� ����� ���ú����� �ƴ� ���� ���ú����� �����Ǵ� ������ �߻���.
		// �װ� �����ϱ� ���� �߰���.
		DWORD dwSlot = pSkinPieceBase->GetSlot();

		if ( pSkinPieceBase->m_emSkinPieceType == DxSkinPieceBase::SKIN_PIECE )
		{
			// [shhan][2015.06.02]
			// chf �� ���ý� �¾�����. cps �� �Ŀ� �����ع�����, �˸��� ���� bone �� ���ð� ������ �Ǿ� ������ �ȴ�.
			// �� ���� �׳� ������ ���ϴ°� �´�.
			CString strFile1( pSkinPieceBase->GetBoneName() );
			strFile1.MakeLower();

			CString strFile2( pCharData->GetSkeletonName() );
			strFile2.MakeLower();

			m_PartArray[dwSlot].SetPart ( this, MIN_BOOST::polymorphic_downcast<DxSkinPiece*>( pSkinPieceBase ), pd3dDevice, bAmbientUP, NULL );

			// CharEdit ���� �������� �����ص� ������ �ʾƼ� �߰���.
			// �̰� ���ϸ� �Ƹ� �̷��� ���ڸ� chf �� ������ ���� ���µ� ������ ���� �� �ִ�.
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

			// [shhan][2014.12.12] CharEdit �� �ƴ� Game �󿡼� NPC �� ������ m_bUseCPS TRUE �� ���� ��µ��� �ʾƼ� ������.
			if( pNew->m_bUseCPS )
			{
				SetPiece( pNew->m_strPieceName.c_str(), pd3dDevice, 0L, 0, emFlags, TRUE, NULL, NULL );
			}

			// [shhan][2014.12.12] �����غ��� �ؿ� �ּ��� ������ ��������. 
			//						Game �� �ֿ� ĳ���͵��� SetPiece �� ���� m_bUseCPS �� �°� �۵��Ǹ鼭 ����� ���Ծ��� 
			//						�Ϲ� ���̳� NPC �� ���������� �ȳ��Ծ��� �� ���� �ȳ��Ծ���.
			//// [shhan][2014.07.18] CharEdit �󿡼� m_bUseCPS ���õȰ� �ȳ����� ������ �־ �۾���.
			////						Game ������ �� ���´�. �� �ߵǴ��� Ȯ���� �ʿ��ϴ�.
			//if ( g_bCHAR_EDIT_RUN )
			//{
			//	if( pNew->m_bUseCPS )
			//	{
			//		SetPiece( pNew->m_strPieceName.c_str(), pd3dDevice, 0L, 0, emFlags, TRUE, NULL, NULL );
			//	}
			//}
		}
	}

	// ���� ������ PhysX ���� �� ������ �ؾ� �Ѵٶ�� �����Ѵ�.
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

	////	Note : �ʱ� ���ϸ��̼� ����.
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
				// �ִϸ��̼� update
				m_pAttBoneArray[i]->GetAttBone()->TOSTARTTIME();
				m_pAttBoneArray[i]->SELECTANI( sAttBasicInfo.emAttAniType );			
			}
			else if( m_pAttBoneArray[i]->m_nOldAttSTEP == -1	
					  || m_pAttBoneArray[i]->GetAttBone()->ISENDANIM() )
			{
				// STAY ���� �Ҷ�			
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

		// �������� ������ ������ STAY��
		if( m_pAttBoneArray[i]->GetAttBone()->ISENDANIM() )
		{
			m_pAttBoneArray[i]->SELECTANI( AN_ATTBONE_STAY );			
		}


		if( m_pAttBoneArray[i]->m_strAttBoneName.empty() ) continue;
		PDXATTBONE pDxAttBone = m_pAttBoneArray[i]->GetAttBone();
		if( !pDxAttBone ) continue;
		if( pAttachBoneAnimInfo[i].bUseStrikeAni ) continue;

		SAttachBoneAnimBasicInfo sAttBasicInfo = pAttachBoneAnimInfo[i].sBasicAni;

		// �ִϸ��̼��� �ٲپ�� �Ҷ�
		if( m_spSkinAniControlThread->GETCURKEYTIME() >= static_cast<DWORD>(sAttBasicInfo.nStartAnim) )
		{
			// �ִϸ��̼� update
			if( sAttBasicInfo.emAttAniType != pDxAttBone->GETCURMTYPE() )
			{
				m_pAttBoneArray[i]->SELECTANI( sAttBasicInfo.emAttAniType );
			}
		}
		else
		{
			// STAY ���� �Ҷ�			
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

		// �ִϸ��̼��� �ٲپ�� �Ҷ�
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

	// ������ Overlay �� �ְ� ������, �������� PIECE_HAIR �� ������ �Ͽ��� ������ �߻�.
	// PIECE_HEADGEAR ���� �ϰ�, ������� �ɼǿ��� ��� Rendering On/Off �� �� �� �ֵ��� �۾� ��.
	if ( m_PartArray[PIECE_HEADGEAR].IsHatHairOption( HAT_HAIR_BASE ) )
	{
		m_PartArray[PIECE_HAIR].SetRender( TRUE );
	}
	else
	{
		m_PartArray[PIECE_HAIR].SetRender( FALSE );
	}

	m_spSkinAniControlThread->FrameMove_EffAniPlayer( this, m_sSkinEffData, fTime, fElapsedTime, bFreeze );

	// Note : Edge Color ����Ʈ ����.
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
	// Render ���� ��.
	D3DXMatrixIdentity( &m_matWorld );
	m_matWorld._11 = m_fScale;
	m_matWorld._22 = m_fScale;
	m_matWorld._33 = m_fScale;
	D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matRot );

	// �� Scale �� �����鼭 �ٸ� ��ġ�� ���� �Ĺ��� �� �־ Offset ���� �۾��Ѵ�.
	m_matWorld._42 += m_fHeightYPos;

	// Ȥ�� SafeMode �̸鼭 ���Ⱑ EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT �� �پ�� �� ��츦 ã�´�.
	m_PartArray[PIECE_RHAND].GetMoveParentBoneIDForSafeModeWeaponSlot( m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot );

	// TAG_ProcessAnimationThread_1
	// ��ϸ� �Ѵ�.
	NSSkinAniThread::AddAniControl( m_spSkinAniControlThread, m_spThreadNeedData, m_matWorld, bNoneOverFrame );

	// ���� Ű����� ���� �������� �ؾ��Ѵ�.
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

	// Ȥ�� SafeMode �̸鼭 ���Ⱑ EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT �� �پ�� �� ��츦 ã�´�.
	m_PartArray[PIECE_RHAND].GetMoveParentBoneIDForSafeModeWeaponSlot( m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot );

	// TAG_ProcessAnimationThread_1
	// ��ϸ� �Ѵ�.
	NSSkinAniThread::AddAniControl_CharOnVehicle( m_spSkinAniControlThread, m_spThreadNeedData, pVehicle->m_spVehicleThreadNeedData, dwVehiclePassengerID, bNoneOverFrame );

	// ���� Ű����� ���� �������� �ؾ��Ѵ�.
	m_spSkinAniControlThread->SetPrevKeyFrame();

	return S_OK;
}

void DxSkinChar::ClearReserveAction()
{
	m_spSkinAniControlThread->ClearReserveFuction();	
}

// ������ �ʼ� ��Ұ� �ִ°�~?
BOOL DxSkinChar::IsValidForRender()
{
	// Thread �۾��Ǹ鼭 Bone�� ���� �� �ִ�.
	if ( !m_spSkinAniControlThread->GetSkeletonMaintain()->IsActiveBone() )
		return FALSE;

	// ĳ���� ���� �ƴ� ��..
	if ( !g_bCHAR_EDIT_RUN ) 
	{
		// �⺻ �ִϸ��̼��� ���� �� ������ �Ѵ�.
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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return S_OK;

	//UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&m_matWorld._41), pd3dDevice);    

	g_dwLOD = 0;
#ifdef USE_SKINMESH_LOD
	// Note : �׸����� ���� �������� �Ѹ���.
	if( bShadow )
	{
		g_dwLOD = 1;
	}
	else if( g_dwHIGHDRAW_NUM > 10 )
	{
		// Note : �Ѱ谡 �Ѿ��� ��� �ּ��� ������ ��� �ϴ� �ֵ��� ����
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
		if( fLength > fLengthSRC )	// �Ÿ��� (ī�޶�Ÿ�+30.f) �̻��� ��� LOD ������ �ٲ۴�.
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
		if( fLength > fLengthSRC )	// �Ÿ��� (ī�޶�Ÿ�+150.f) �̻��� ��� LOD ������ �ٲ۴�.
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
		// �� �ε��Ǿ����� Ȯ���Ѵ�.
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
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
		return S_OK;
	}
	
	int nStart=0, nEnd = 0, nAdd = 0;
    
	// �ǽ� ������ �Ųٷ� �׸���. ( �� ���� )
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


	// Note : ������ ��ȭ�� ���� �ʴ´�.
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
	//sDestLight01.Diffuse.r = sSrcLight01.Diffuse.r * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight01.Diffuse.g = sSrcLight01.Diffuse.g * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight01.Diffuse.b = sSrcLight01.Diffuse.b * 0.75f;	//0.9f;	// Item ������ ���� �����.

	//sDestLight02.Ambient.r += sSrcLight02.Diffuse.r * 0.15f;
	//sDestLight02.Ambient.g += sSrcLight02.Diffuse.g * 0.15f;
	//sDestLight02.Ambient.b += sSrcLight02.Diffuse.b * 0.15f;
	//sDestLight02.Diffuse.r = sSrcLight02.Diffuse.r * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight02.Diffuse.g = sSrcLight02.Diffuse.g * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight02.Diffuse.b = sSrcLight02.Diffuse.b * 0.75f;	//0.9f;	// Item ������ ���� �����.

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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return S_OK;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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

	if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )	// ĳ���Ͱ� Material System ���� ���۵� ���� �̷��� ����.
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
	else	// �ϵ���� ������ ���� ���۰��.
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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return S_OK;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return FALSE;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return S_OK;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
		return S_OK;
	}

	// 
	SetDiffuseColor_NSMaterialSkinManager();

	// ���� �̻� ����� �Ǿ߸� �ݻ� ������ �ǵ��� �۾���.

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
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
		return;
	}
	m_spSkinAniControlThread->RenderBone( pd3dDevice );
}

void DxSkinChar::RenderAniEff( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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
	//sDestLight01.Diffuse.r = sSrcLight01.Diffuse.r * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight01.Diffuse.g = sSrcLight01.Diffuse.g * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight01.Diffuse.b = sSrcLight01.Diffuse.b * 0.75f;	//0.9f;	// Item ������ ���� �����.

	//sDestLight02.Ambient.r += sSrcLight02.Diffuse.r * 0.15f;
	//sDestLight02.Ambient.g += sSrcLight02.Diffuse.g * 0.15f;
	//sDestLight02.Ambient.b += sSrcLight02.Diffuse.b * 0.15f;
	//sDestLight02.Diffuse.r = sSrcLight02.Diffuse.r * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight02.Diffuse.g = sSrcLight02.Diffuse.g * 0.75f;	//0.9f;	// Item ������ ���� �����.
	//sDestLight02.Diffuse.b = sSrcLight02.Diffuse.b * 0.75f;	//0.9f;	// Item ������ ���� �����.

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
	// Note : m_PartArray, m_EffAniPlayer �� �ٸ� ��ο��� �ʱ�ȭ�� �� �ش�.

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
	//m_spSkinAniControlThread->CalculateBone_PureThread_HARD( matWorld );	// ���� ���� �ʿ�� ����.

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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	//UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&m_matWorld._41), pd3dDevice);

	g_dwLOD = 0;
#ifdef USE_SKINMESH_LOD
	// Note : �׸����� ���� �������� �Ѹ���.
	if( bShadow )
	{
		g_dwLOD = 1;
	}
	else if( g_dwHIGHDRAW_NUM > 10 )
	{
		// Note : �Ѱ谡 �Ѿ��� ��� �ּ��� ������ ��� �ϴ� �ֵ��� ����
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
		if( fLength > fLengthSRC )	// �Ÿ��� (ī�޶�Ÿ�+30.f) �̻��� ��� LOD ������ �ٲ۴�.
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
		if( fLength > fLengthSRC )	// �Ÿ��� (ī�޶�Ÿ�+150.f) �̻��� ��� LOD ������ �ٲ۴�.
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
		// �� �ε��Ǿ����� Ȯ���Ѵ�.
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
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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

	// �ǵ���.
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
}

void DxSkinChar::RenderMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// ������ �ʼ� ��Ұ� �ִ°�~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	if( !m_bCompleted_PartLoad_FirstCreate )
	{
		// �ε��� �� ���� �ʾҴٸ� �����ϸ� �ȵȴ�. ��� ������ �Ҿ��� �����̴�.
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

	// �ǵ���.
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
	// SelfShadow �� �Ÿ��� �Ѱ� ������ Diffuse(CameraSpecular) �� ��� ������ ������ �ִ�.
	// �׷��� �� �ָ� ������ Diffuse(CameraSpecular) ���� �ٿ��ֱ� ���� �����Ѵ�.
	//
	// �������� 1 �ּ��� 0
	// 50 ������ �� �������� ���ش�.
	// 200 �� ������ ���� Diffuse(CameraSpecular) �� ������� �ʴ´�.
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
// �ܺ� �ɼǿ� ���� ON/OFF �� ���� ���� Data ��ȭ�� ȣ�� �Լ� �� ����

// �̰��� �� ������ �������� �Ѵ�.
void DxSkinChar::SetPhysX_Cloth( BOOL bOnOff )
{
	// ���ð��� ���� ������ ������ �ʾҴٸ� ���η� �� �ʿ䰡 ����.
	if ( m_bPhysX_Cloth_OnOff == bOnOff && m_bCompleted_PhysX_Cloth )
		return;

	for ( DWORD i=0; i<PIECE_SIZE; ++i )
	{
		if ( !m_PartArray[i].SetPhysX_Cloth( bOnOff ) )
			return;	// �۾� �Ϸ� �ȵǰ� ����.

		if( m_pAttBoneArray[i] && !m_pAttBoneArray[i]->m_strAttBoneName.empty() ) 
		{
			if ( !m_pAttBoneArray[i]->SetPhysX_Cloth( bOnOff ) )
				return;	// �۾� �Ϸ� �ȵǰ� ����.
		}
	}

	// �� �۾��Ǹ� �� ����.
	m_bPhysX_Cloth_OnOff = bOnOff;
	m_bCompleted_PhysX_Cloth = TRUE;
}

// �ܺ� �ɼǿ� ���� ON/OFF �� ���� ���� Data ��ȭ�� ȣ�� �Լ� �� ����
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

	// Note : View Port�� �����Ѵ�.
	D3DXMATRIX matView;
	D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
	D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );

	// Note : Projection�� �����Ѵ�.
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

	// Note : ����� ����
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
		// Billboard �� ���Ϸ� �����Ѵ�.
		//////////////////////////////////////////////////////////////////////////
		if ( szNameCHF_bSaveBillboard )
		{
			// ������ ���� ���� �����Ƿ� ���鵵�� �Ѵ�.
			CreateDirectory ( NSBillboard::GetTexturePath(), NULL );

			// Note : �ؽ��ĸ� �����Ѵ�.
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

			// Note : ���ۿ� �׷��� ���� �ؽ��ķ� �̴´�.
			TSTRING strTexture = sc::util::GetName_ExtErase( szNameCHF_bSaveBillboard );
			strTexture += _T("_blb.dds");
			strTexture = NSBillboard::GetTexturePath() + strTexture;

			D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_DDS, pTexNew, NULL );

			SAFE_RELEASE( pTexRGB );
			SAFE_RELEASE( pTexNew );
		}
		//////////////////////////////////////////////////////////////////////////
		// Billboard �� ���Ϸ� �����Ѵ�.
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// ���� ȭ�鿡 ������ billboard �� ������ �ϵ��� �Ѵ�.
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
	// ���� ȭ�鿡 ������ billboard �� ������ �ϵ��� �Ѵ�.
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	// Note : �ѹ�
	pd3dDevice->SetTransform( D3DTS_VIEW,		&matViewPREV );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjPREV );



	SAFE_RELEASE( pTexture );
	SAFE_RELEASE( pSurface );
	SAFE_RELEASE( pZBuffer );
}





//////////////////////////////////////////////////////////////////////////
//						AnimationMove
//////////////////////////////////////////////////////////////////////////

// �ִϸ��̼� �������� ĳ���Ϳ� ������ �޴� ������ Ȯ��
BOOL DxSkinChar::IsAnimationMove()
{
	return m_spSkinAniControlThread->IsAnimationMove();
}

// ĳ���Ͱ� �ִϸ��̼����� �󸶸�ŭ ���������� Ȯ��
const D3DXVECTOR3& DxSkinChar::GetAnimationMovePos()
{
	return m_spSkinAniControlThread->GetAnimationMovePos();
}

//// AnimationMove �� ���� Matrix
//const D3DXMATRIX& DxSkinChar::GetAnimationMoveStartMatrix()
//{
//	return m_spSkinAniControlThread->GetAnimationMoveStartMatrix();
//}

D3DXVECTOR3 DxSkinChar::GetAnimationMoveOffsetFull_EDIT()
{
	return m_spSkinAniControlThread->GetAnimationMoveOffsetFull_EDIT();
}

//////////////////////////////////////////////////////////////////////////	