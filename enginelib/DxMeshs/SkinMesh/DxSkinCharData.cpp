#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxTools/DebugSet.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/RENDERPARAM.h"

#include "../../G-Logic/GLogic.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../Archive/Archive.h"

#include "./NSSkinCharCommon.h"
#include "./DxJiggleBoneColl.h"
#include "./DxJiggleBone.h"
#include "./DxFootColl.h"

#include "DxSkinCharData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxSkinCharDataContainer& DxSkinCharDataContainer::GetInstance()
{
	static DxSkinCharDataContainer Instance;
	return Instance;
}

//-----------------------------------------------------------------------------------[DxSkinCharData]
//
DxSkinCharData::DxSkinCharData(void) 
	: m_dwRef(0)
	, m_bWorldObj(FALSE)
	, m_fScale(1.0f)
	, m_vMax(7.f,20.f,7.f)
	, m_vMin(-7.f,0.f,-7.f)
	, m_fHeight(20.f)
	, m_fRadius(7.f)
	, m_fHeightYPos( 0.0f )
	, m_dwFlags(0L)
	, m_spSkinAniControlData(new DxSkinAniControlData)
{
	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		m_strPIECE[i] = _T("");
	}

	for ( DWORD i=0; i<EMPEACE_WEAPON_SIZE; ++i )
	{
		m_arrayfWeaponScale[i] = 1.f;
	}

	//for (int i=0; i<PIECE_SIZE; ++i)
	//	m_pAttBoneArray[i] = NULL;
}

DxSkinCharData::~DxSkinCharData(void)
{
	ClearAll ();
	CleanUp();
}

void DxSkinCharData::CleanUp()
{
	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		m_strPIECE[i] = _T("");
	}
}

HRESULT DxSkinCharData::ClearAll ()
{
	m_dwRef = 0;
	m_fScale = 1.0f;
	m_fHeightYPos = 0.0f;

	m_vMax = D3DXVECTOR3(7,20,7);
	m_vMin = D3DXVECTOR3(-7,0,-7);

	m_vecANIFILE.clear();

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_strPIECE[i] = _T("");

		////	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
		//if ( m_pAttBoneArray[i] ) 
		//{
		//	DxAttBoneLinkContainer::GetInstance().ReleaseLink ( m_pAttBoneArray[i]->m_szFileName );
		//}
		//m_pAttBoneArray[i] = NULL;
	}

	// [ Thread Work shhan ]
	//DxSkinAniControl::ClearAll ();

	m_spSkinAniControlData->CleanUp();

	return S_OK;
}

HRESULT DxSkinCharData::SetPieceEDIT( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlag )
{
	char ext[_MAX_EXT] = {0};
	_splitpath( szFile, NULL, NULL, NULL, ext );

	CString strExt(ext);
	strExt.MakeUpper();

	// ���� �ε��� ������ Attach Bone Link �����̸�...
	if( strExt == _T(".ABL") )
	{
		//if ( dwFlag&FG_MUSTNEWLOAD )
		//{
		//	for ( int i=0; i<PART_V_SIZE; ++i )
		//	{
		//		if( m_strPIECE[i] == szFile )
		//		{
		//			m_strPIECE[i] = _T("");
		//		}
		//	}

		//	DxAttBoneLinkContainer::GetInstance().DeleteLink ( szFile );
		//}

		//	Note : Attach Bone�� �о�´�.
		//
		DxAttBoneLink* pAttBoneLink = DxAttBoneLinkContainer::GetInstance().LoadLink(std::string(szFile), emFlags );
		if (!pAttBoneLink)
			return E_FAIL;

		// [shhan][2015.06.08] Link �� �ƴ� Skin �� ���� �ؾ� �ϰ� ThreadLoading �̱� ������
		//						DxCharPart::SetPart_Detail Ÿ�ֿ̹��� Ȯ���ϴ� ������ ����Ǿ���.
		//
		//CString strFile1( m_spSkinAniControlData->GetSkeletonName() );
		//strFile1.MakeLower();

		//CString strFile2( pAttBoneLink->m_strBaseBoneName.c_str() );
		//strFile2.MakeLower();

		//if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
		//{
		//	CDebugSet::ToLogFile
		//	( 
		//		sc::string::format( _T("Base Bone : %1%, Error cps : %2%, Error Bone : %3% \n"), m_spSkinAniControlData->GetSkeletonName(), szFile, pAttBoneLink->m_strBaseBoneName.c_str() ) 
		//	);

		//	return E_DIFFERENT_BONEFILE;
		//}

		//	Note : Piece �� ����.
		//
		m_strPIECE[pAttBoneLink->m_emPieceCharType] = szFile;
	}
	else
	{
		////	Note : ���� ó�� �ε�� ���� �����ϰ� ������ �ε��Ҷ����� ���� �ε�Ȱ� �����ش�.
		////
		//if ( dwFlag&FG_MUSTNEWLOAD )
		//{
		//	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
		//	{
		//		if( m_strPIECE[i] == szFile )
		//		{
		//			m_strPIECE[i] = _T("");
		//		}
		//	}

		//	DxSkinPieceContainer::GetInstance().DeletePiece ( szFile );
		//}

		//	Note : Piece�� �о�´�.
		//
		DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( szFile, 0L );
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
		//	//if ( !pSkinPiece->m_pmcMesh )
		//	//	return E_FAIL;

		//	//if ( pSkinPiece->m_pmcMesh && pSkinPiece->m_pmcMesh->pSkinInfo )
		//	{
		//		CString strFile1( m_spSkinAniControlData->GetSkeletonName() );
		//		strFile1.MakeLower();

		//		CString strFile2( pSkinPiece->m_szSkeleton );
		//		strFile2.MakeLower();

		//		if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
		//		{
		//			CDebugSet::ToLogFile
		//			( 
		//				sc::string::format( _T("Base Bone : %1%, Error cps : %2%, Error Bone : %3% \n"), m_spSkinAniControlData->GetSkeletonName(), szFile, pSkinPiece->m_szSkeleton ) 
		//			);

		//			return E_DIFFERENT_BONEFILE;
		//		}
		//	}
		//}

		// 
		m_strPIECE[pSkinPiece->m_emType] = szFile;
	}

	return S_OK;
}

void DxSkinCharData::ResetPieceEDIT( LPDIRECT3DDEVICEQ pd3dDevice, int i )
{
	MIN_ASSERT(i<PIECE_SIZE);

	m_strPIECE[i] = _T("");
}

BOOL DxSkinCharData::LoadAnimationFromTool( const TCHAR* szNameCHF, const TCHAR* szNameCFG, const TCHAR* szNameXFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH /*=false*/ )
{
	NSSkinCharCommon::LoadAnimationFromTool( szNameCHF, szNameCFG, szNameXFile, pd3dDevice, bREPRESH, *m_spSkinAniControlData.get(), m_vecANIFILE );

	return TRUE;
}

void DxSkinCharData::ReleaseAnimationFromTool( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* szAnim )
{
	NSSkinCharCommon::ReleaseAnimationFromTool( pd3dDevice, szAnim, *m_spSkinAniControlData.get(), m_vecANIFILE );
}

//HRESULT DxSkinCharData::SetPiece( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread, DWORD dwFlag /* = 0x0 */, BOOL bAttachSub /* = FALSE */ )
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
//		if ( dwFlag&FG_MUSTNEWLOAD )
//		{
//			for ( int i=0; i<PIECE_SIZE; ++i )
//			{
//				if ( m_pAttBoneArray[i] && m_pAttBoneArray[i]->m_szFileName )
//				{
//					if ( !strcmp(m_pAttBoneArray[i]->m_szFileName,szFile) )
//					{
//						m_pAttBoneArray[i] = NULL;
//					}
//				}
//			}
//
//			DxAttBoneLinkContainer::GetInstance().DeleteLink ( szFile );
//		}
//
//		// Attach Bone�� �о�´�.
//        DxAttBoneLink* pAttBoneLink = DxAttBoneLinkContainer::GetInstance().LoadLink(std::string(szFile), pd3dDevice, bThread);
//		if (!pAttBoneLink)
//            return E_FAIL;
//
//		CString strFile1( m_spSkinAniControlData->GetSkeletonName() );
//		strFile1.MakeLower();
//
//		CString strFile2( pAttBoneLink->m_strBaseBoneName.c_str() );
//		strFile2.MakeLower();
//
//		if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
//		{
//            std::string ErrorMsg(
//                sc::string::format(
//                    "Incorrect bone %1% %2%",
//                    strFile1.GetString(),
//                    strFile2.GetString()));
//			CDebugSet::ToLogFile(ErrorMsg);
//			return E_DIFFERENT_BONEFILE;
//		}
//
//		if ( m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] )
//		{
//			DxAttBoneLinkContainer::GetInstance().ReleaseLink ( szFile );
//		}
//
//		//	Note : Piece �� ����.
//		//
//		m_pAttBoneArray[pAttBoneLink->m_emPieceCharType] = pAttBoneLink;
//
//		if( pAttBoneLink->m_bUseCPS )
//		{
//			SetPiece( pAttBoneLink->m_strPieceName.c_str(), pd3dDevice, bThread, dwFlag, TRUE );
//		}
//	}
//	else
//	{
//		//	Note : Piece�� �о�´�.
//		//
//		DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( szFile, pd3dDevice, bThread );
//		if ( !pSkinPiece )	return E_FAIL;
//		if ( pSkinPiece->m_emPieceType != PIECETYPE_CHARACTER ) return E_FAIL;
//
//		if ( dwFlag&FG_BONEFILE_CHECK )
//		{
//			if ( pSkinPiece->m_pmcMesh->pSkinInfo )
//			{
//				CString strFile1( m_spSkinAniControlData->GetSkeletonName() );
//				strFile1.MakeLower();
//
//				CString strFile2( pSkinPiece->m_szSkeleton );
//				strFile2.MakeLower();
//
//				if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
//				{
//                    std::string ErrorMsg(
//                        sc::string::format(
//                            "Incorrect bonee %1% %2%",
//                            strFile1.GetString(),
//                            strFile2.GetString()));
//					CDebugSet::ToLogFile(ErrorMsg);
//					return E_DIFFERENT_BONEFILE;
//				}
//			}
//		}
//
//		if( !bAttachSub )
//		{
//			if ( m_pAttBoneArray[pSkinPiece->m_emPieceCharType] )
//			{
//				DxAttBoneLinkContainer::GetInstance().ReleaseLink ( szFile );
//			}
//
//			m_pAttBoneArray[pSkinPiece->m_emPieceCharType] = NULL;
//		}
//	}
//
//	return S_OK;
//}
//
//void DxSkinCharData::ResetPiece ( LPDIRECT3DDEVICEQ pd3dDevice, int i )
//{
//	GASSERT(i<PIECE_SIZE);
//
//	PDXSKINPIECE pSkinPiece = GetPiece ( pd3dDevice, i);
//	
//	PDXATTBONELINK pAttBoneLink = GetAttachBone(i);
//	if( pAttBoneLink )
//	{
//		DxAttBoneLinkContainer::GetInstance().ReleaseLink ( pAttBoneLink->m_szFileName );
//		m_pAttBoneArray[i] = NULL;
//	}
//}

DxSkinPieceBase* DxSkinCharData::GetPiece( LPDIRECT3DDEVICEQ pd3dDevice, int i, EMSCD_FLAGS emFlags )
{
	if( i >= PIECE_SAVE_SIZE && i != PIECE_LHAND_HIDDEN && i != PIECE_RHAND_HIDDEN )
	{
		return NULL;
	}

	char ext[_MAX_EXT] = {0};
	_splitpath( m_strPIECE[i].c_str(), NULL, NULL, NULL, ext );

	CString strExt(ext);
	strExt.MakeUpper();

	// ���� �ε��� ������ Attach Bone Link �����̸�...
	if( strExt == _T(".ABL") )
	{
		return DxAttBoneLinkContainer::GetInstance().LoadLink( m_strPIECE[i], emFlags );
	}
	else
	{
		return DxSkinPieceContainer::GetInstance().LoadPiece( m_strPIECE[i], emFlags );
	}
}

void DxSkinCharData::SaveBinFile( const TSTRING& strNameCFG )
{
	DxSkinAnimationRes skinAniRes = DxSkinAnimationManager::GetInstance().LoadAnimationCFG( strNameCFG );
	if( skinAniRes.IsValid() )
	{
		skinAniRes.get()->CreateAnimationDataFromTool( TRUE );
	}
}

void DxSkinCharData::SaveBinFileAll()
{
	for ( DWORD i=0; i<(DWORD)m_vecANIFILE.size(); ++i )
	{
		SaveBinFile( m_vecANIFILE[i] );
	}
}

BOOL DxSkinCharData::SaveFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive)
{
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxSkinCharDataContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );

    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return FALSE;
    }

    sc::SerialFile& SFile = *spFile;

	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		
        return FALSE;

	SFile << VERSION;

	SFile << PIECE_SAVE_SIZE;
	for ( int i=0; i<PIECE_SAVE_SIZE; ++i )
	{
		//if( m_pAttBoneArray[i] != NULL )
		//{
		//	SFile << BOOL(m_pAttBoneArray[i]!=NULL);
		//	if ( !m_pAttBoneArray[i] )	continue;

		//	std::string strPieceFile = m_pAttBoneArray[i]->m_szFileName;
		//	SFile << strPieceFile;
		//}
		//else
		{
			if( m_strPIECE[i].empty() )
			{
				SFile << BOOL(FALSE);
				continue;
			}
			else
			{
				SFile << BOOL(TRUE);
			}

			SFile << m_strPIECE[i];
		}
	}

	SFile << TSTRING( m_spSkinAniControlData->GetSkeletonName() );

	//	Note :���ϸ��̼� ����.
	//
	SFile << (DWORD)m_vecANIFILE.size();
	
	for ( size_t n=0; n<m_vecANIFILE.size(); ++n )
	{
		SFile << m_vecANIFILE[n];
	}

	SFile << m_fScale;
	SFile << m_fHeightYPos;

	// Note : �߰�.
	SFile << (BOOL)TRUE;
	SFile << m_fHeight;
	SFile << m_fRadius;

	//  Note : CharEditor���� ������ �� ������
	SFile << (DWORD)m_spSkinAniControlData->GetBoneScaleList().size();

	SBONESCALE boneScale;
	for( size_t i = 0; i < m_spSkinAniControlData->GetBoneScaleList().size(); i++ )
	{
		SFile << m_spSkinAniControlData->GetBoneScaleList()[i].vScale;
		SFile << m_spSkinAniControlData->GetBoneScaleList()[i].strBoneName;
	}

	//	Note : ��� �ڽ� ���.
	//
	SFile << (BOOL) TRUE;
	//CalculateBoundingBox ( pd3dDevice, m_vMax, m_vMin );

	SFile << m_vMax;
	SFile << m_vMin;

	// Note : �߰�.
	SFile << (BOOL)TRUE;
	SFile << m_bWorldObj;

	// Note : �߰�.
	SFile << (BOOL)TRUE;
	SFile << m_dwFlags;

	// Note : �߰�.
	SFile << (BOOL)TRUE;
	SFile << EMPEACE_WEAPON_SIZE;
	for ( DWORD i=0; i<EMPEACE_WEAPON_SIZE; ++i )
	{
		SFile << m_arrayfWeaponScale[i];
	}

	//	Note : ����Ʈ�� �ٸ� �߰� ��ü�� �� ���� ����ؼ� ���� ����Ʈ�� ���� �Ǹ�
	//		TRUE ���� �����ϰ� �ȴ�.
	//
	SFile << (BOOL)FALSE;

	//	Note : ��Ų �ɸ��� �̸� ����.
	m_strFileName = szFile;

	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver108( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SAVE_SIZE )		
		return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	//
	DWORD dwAniNum(0);
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		TSTRING strANIFILE;
		SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );



	SFile >> m_fScale;
	SFile >> m_fHeightYPos;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )	
	{
		SFile >> m_fHeight;
		SFile >> m_fRadius;
	}
	else			return TRUE;

	//  Note : CharEditor���� ������ �� ������
	DWORD dwBoneScaleSize;
	SFile >> dwBoneScaleSize;

	SBONESCALE boneScale;
	for( size_t i = 0; i < dwBoneScaleSize; i++ )
	{
		SFile >> boneScale.vScale;
		SFile >> boneScale.strBoneName;

		m_spSkinAniControlData->AddBoneScale( boneScale );
	}

	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_bWorldObj;
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_dwFlags;
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		DWORD dwSize(0L);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			if ( i<EMPEACE_WEAPON_SIZE )
			{
				SFile >> m_arrayfWeaponScale[i];
			}
			else
			{
				float fScale(0.f);
				SFile >> fScale;
			}
		}
	}
	else			return TRUE;

	// Note : ����
	SFile >> bExist;

	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver107( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SAVE_SIZE )		
        return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	//
	DWORD dwAniNum(0);
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		TSTRING strANIFILE;
		SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );



	SFile >> m_fScale;
	SFile >> m_fHeightYPos;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )	
	{
		SFile >> m_fHeight;
		SFile >> m_fRadius;
	}
	else			return TRUE;

	//  Note : CharEditor���� ������ �� ������
	DWORD dwBoneScaleSize;
	SFile >> dwBoneScaleSize;

	SBONESCALE boneScale;
	for( size_t i = 0; i < dwBoneScaleSize; i++ )
	{
		SFile >> boneScale.vScale;
		SFile >> boneScale.strBoneName;

		m_spSkinAniControlData->AddBoneScale( boneScale );
	}

	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_bWorldObj;
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_dwFlags;
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_spSkinAniControlData->ErrorLoadJiggleBone( SFile );
	}
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_spSkinAniControlData->LoadPrevFootColl( SFile );
	}
	else			return TRUE;

	// Note : ����
	SFile >> bExist;

	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver106( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	//
	DWORD dwAniNum(0);
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		TSTRING strANIFILE;
		SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );


	SFile >> m_fScale;
	SFile >> m_fHeightYPos;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )	
	{
		SFile >> m_fHeight;
		SFile >> m_fRadius;
	}
	else			return TRUE;

	//  Note : CharEditor���� ������ �� ������
	DWORD dwBoneScaleSize;
	SFile >> dwBoneScaleSize;

	SBONESCALE boneScale;
	for( size_t i = 0; i < dwBoneScaleSize; i++ )
	{
		SFile >> boneScale.vScale;
		SFile >> boneScale.strBoneName;

		m_spSkinAniControlData->AddBoneScale( boneScale );
	}

	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_bWorldObj;
	else			return TRUE;

	// Note : ����
	SFile >> bExist;

	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver105( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	//
	DWORD dwAniNum(0);
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		TSTRING strANIFILE;
		SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );


	SFile >> m_fScale;
	SFile >> m_fHeightYPos;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )	
	{
		SFile >> m_fHeight;
		SFile >> m_fRadius;
	}
	else			return TRUE;

	//  Note : CharEditor���� ������ �� ������
	std::vector<SBONESCALE> temp;
	SFile >> temp;

	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_bWorldObj;
	else			return TRUE;

	// Note : ����
	SFile >> bExist;

	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver104( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	//
	DWORD dwAniNum(0);
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		TSTRING strANIFILE;
		SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );


	SFile >> m_fScale;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )	
	{
		SFile >> m_fHeight;
		SFile >> m_fRadius;
	}
	else			return TRUE;

	//  Note : CharEditor���� ������ �� ������
	std::vector<SBONESCALE> temp;
	SFile >> temp;

	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_bWorldObj;
	else			return TRUE;

	// Note : ����
	SFile >> bExist;

	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver103( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;

	SFile >> m_fScale;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	//
	DWORD dwAniNum(0);
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		TSTRING strANIFILE;
		SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );


	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_bWorldObj;
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	
	{
		SFile >> m_fHeight;
		SFile >> m_fRadius;
	}
	else			return TRUE;

	// Note : ����
	SFile >> bExist;

	//  Note : CharEditor���� ������ �� ������
	std::vector<SBONESCALE> temp;
	SFile >> temp;


	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;

	SFile >> m_fScale;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	//
	DWORD dwAniNum(0);
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		TSTRING strANIFILE;
		SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );


	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )	SFile >> m_bWorldObj;
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )	
	{
		SFile >> m_fHeight;
		SFile >> m_fRadius;
	}
	else			return TRUE;

	// Note : ����
	SFile >> bExist;

	return TRUE;
}

BOOL DxSkinCharData::LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwPieceSize;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	char szPIECE[PIECE_SIZE_OLD][MAX_PATH] = {0};
    // V597 The compiler could delete the 'memset' function call, which is used to flush 'szPIECE' buffer. The RtlSecureZeroMemory() function should be used to erase the private data.
	memset ( szPIECE, 0, sizeof(char)*PIECE_SIZE_OLD*MAX_PATH );
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : ������ �б�.
	char szSkeleton[MAX_PATH] = "";
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*MAX_PATH );
	m_spSkinAniControlData->LoadSkeleton ( szSkeleton, pd3dDevice );

	//	Note :���ϸ��̼� �б�.
	char szAniName[ACF_SZNAME] = "";

	DWORD dwAniNum;
	SFile >> dwAniNum;

	m_vecANIFILE.clear();
	m_vecANIFILE.reserve ( dwAniNum );

	for ( DWORD i=0; i<dwAniNum; i++ )
	{
		SFile.ReadBuffer ( szAniName, sizeof(char)*ACF_SZNAME );

		TSTRING strANIFILE(szAniName);
		//SFile >> strANIFILE;
		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );
	}

	// [2010.10.06] ���ڱ� MobNpcEdit���� �ִϸ��̼� ������ �̻��ϰ� ���ͼ� �̷��� sort�� �ʿ��ϴ�.
	std::sort ( m_vecANIFILE.begin(), m_vecANIFILE.end() ); //, ANIMCONTNODE_OPER() );


	//	Note : �ٿ�� �ڽ� ���� �ε�.
	//
	BOOL bBoundBox(FALSE);
	SFile >> bBoundBox;
	if ( bBoundBox )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else
	{
		m_vMax = D3DXVECTOR3(7.f,20.f,7.f);
		m_vMin = D3DXVECTOR3(-7.f,0.f,-7.f);
	}

	return TRUE;
}

//	Note : ��Ų ������ ����.
//		BOOL DxSkinDataDummy::LoadFile () ������ ���� ������ �ε��� ���ϹǷ� �� �ε�/���̺� �κ��� �ٲ��
//		�����ؼ� ���� ������ �ʿ��ϴ�.
//
BOOL DxSkinCharData::LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, bool bAniDirectLoad, boost::function<void (const TCHAR*)> pArchive)
{
	//////////////////////////////////////////////////////////////////////////
	// CleanUp
	{
		for ( DWORD i=0; i<EMPEACE_WEAPON_SIZE; ++i )
		{
			m_arrayfWeaponScale[i] = 1.f;
		}
	}
	// CleanUp
	//////////////////////////////////////////////////////////////////////////

	m_spSkinAniControlData->CleanUp();

    std::string PathName(DxSkinCharDataContainer::GetInstance().GetPath());
	PathName.append(FileName);

    /* ��ī�̺� ��� �߰��� ���� �� �ڵ�, ���� ����ȭ�Ǹ� ���� ����
	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseEngineZipFile(), 
            GLOGIC::strSKINOBJ_ZIPFILE, 
            PathName.c_str(), 
            FileName));

	if (!pBStream.get())
		return FALSE;

	sc::BaseStream& SFile = *pBStream;
    */

    SPBaseStream spFile;

    if (Archive::CreateFileBaseStream(PathName.c_str(), FileName.c_str(), GLOGIC::strSKINOBJ_ZIPFILE.c_str(), pArchive, spFile) == false)
    {
        return FALSE;
    }

    sc::BaseStream& SFile = *spFile;

	// Note : Init Data
	m_fHeight = 20.f;
	m_fRadius = 7.f;
	m_fHeightYPos = 0.0f;
	m_bWorldObj = FALSE;
	m_dwFlags = 0L;

	DWORD dwVer;
	SFile >> dwVer;


	//////////////////////////////////////////////////////////////////////////
	//						�� �ض�.
	//////////////////////////////////////////////////////////////////////////
	//	Note : ��Ų ������ ����.
	//		BOOL DxSkinDataDummy::LoadFile () ������ ���� ������ �ε��� ���ϹǷ� �� �ε�/���̺� �κ��� �ٲ��
	//		�����ؼ� ���� ������ �ʿ��ϴ�.
	//////////////////////////////////////////////////////////////////////////
	switch ( dwVer )
	{
	case 0x0100:
		LOAD_Ver100( SFile, pd3dDevice );
		break;
	case 0x0101:
	case 0x0102:
		LOAD_Ver101( SFile, pd3dDevice );
		break;
	case 0x0103:
		LOAD_Ver103( SFile, pd3dDevice );
		break;
	case 0x0104:
		LOAD_Ver104( SFile, pd3dDevice );
		break;
	case 0x0105:
		LOAD_Ver105( SFile, pd3dDevice );
		break;
	case 0x0106:
		LOAD_Ver106( SFile, pd3dDevice );
		break;
    case 0x0107:
		LOAD_Ver107( SFile, pd3dDevice );
		break;
	case VERSION:
		LOAD_Ver108( SFile, pd3dDevice );
		break;
	default:
		return FALSE;
	};

	if ( pArchive != NULL )
	{
		for ( VEC_TSTRING_ITER iter = m_vecANIFILE.begin(); iter != m_vecANIFILE.end(); ++iter )		
			pArchive(iter->c_str());
	}

	//	Note : ������ �б�.
	//
	if ( !m_spSkinAniControlData->IsValidSkeleton() )
        return FALSE;

	// Bone Setting File Loading
	m_spSkinAniControlData->LoadFile_BSF( pArchive );

	m_spSkinAniControlData->CreateSkinAni( m_vecANIFILE, !bAniDirectLoad );

	//for ( size_t n=0; n<m_spSkinAniControlData->m_skinAniResVec.size(); ++n )
	//{
	//	int MType = m_spSkinAniControlData->m_skinAniResVec[n].get()->m_MainType;
	//	int SType = m_spSkinAniControlData->m_skinAniResVec[n].get()->m_SubType;
	//	m_spSkinAniControlData->m_vecAniType[MType][SType].push_back( m_spSkinAniControlData->m_skinAniResVec[n] );
	//}

	// [ Thread Work shhan ]

	//// Char Editor���� ������ �� ������ �� ����
	//DxSkinAniControl::SetBoneScaleList( m_deqBoneEditScale );

	//	Note : ��Ų �ɸ��� �̸� ����.
	m_strFileName = FileName;

	// [ Thread Work shhan ]

	// cache�� �� �ʿ䰡 ��� ������ ���� ���ش�.
	SFile.CloseFile();

	return TRUE;
}

// [shhan][2014.12.08] chf �� �ִ� cfg �� ����ִ� bin ���ϵ��� Thread �� �ϵ� �ƴϵ� ���� �ε��� �ϵ��� �Ѵ�.
//						���� : �ڽ��� ĳ���Ͱ� �ִϸ��̼��� ��� �˸��� ������ ���ϴ� ���� �ذ�.
void DxSkinCharData::LoadBIN_HaveToAnimationForMyChar( BOOL bAniThread )
{
	m_spSkinAniControlData->LoadBIN_HaveToAnimationForMyChar( bAniThread );
}

void DxSkinCharData::LoadBIN_HaveToAnimationForMyChar( EMANI_MAINTYPE emANIMTYPE, EMANI_SUBTYPE emANISTYPE, BOOL bAniThread )
{
	m_spSkinAniControlData->LoadBIN_HaveToAnimationForMyChar( emANIMTYPE, emANISTYPE, bAniThread );
}

//HRESULT DxSkinCharData::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
//{
//	D3DXVECTOR3 vLastMax(-FLT_MAX,-FLT_MAX,-FLT_MAX), vLastMin(FLT_MAX,FLT_MAX,FLT_MAX);
//	D3DXVECTOR3 vMax, vMin;
//
//	for ( int i=0; i<PIECE_SAVE_SIZE; ++i )
//	{
//		if ( !m_pPieceArray[i] )
//		{
//			continue;//	������ �Ϻκ� �������� �����Ƿ� ������ ������. ( �������� �Ǵ�. )
//		}
//		
//		HRESULT hr = m_pPieceArray[i]->CalculateBoundingBox ( pd3dDevice, vMax, vMin );
//		
//		if ( FAILED(hr) )
//		{
//			continue;//	������ �Ϻκ� �������� �����Ƿ� ������ ������. ( �������� �Ǵ�. )
//		}
//
//		if ( vLastMax.x < vMax.x )	vLastMax.x = vMax.x;
//		if ( vLastMax.y < vMax.y )	vLastMax.y = vMax.y;
//		if ( vLastMax.z < vMax.z )	vLastMax.z = vMax.z;
//
//		if ( vLastMin.x > vMin.x )	vLastMin.x = vMin.x;
//		if ( vLastMin.y > vMin.y )	vLastMin.y = vMin.y;
//		if ( vLastMin.z > vMin.z )	vLastMin.z = vMin.z;
//	}
//
//	_vMax = vLastMax;
//	_vMin = vLastMin;
//
//	return S_OK;
//}

void DxSkinCharData::SetRadiusHeight()
{
	m_vMax = D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_vMin = D3DXVECTOR3( 0.f, 0.f, 0.f );

	// Note : ���� ����
	m_vMax.y = m_fHeight;

	// Note : ������ ����
	m_vMax.x = m_fRadius;
	m_vMax.z = m_fRadius;
	m_vMin.x = -m_fRadius;
	m_vMin.z = -m_fRadius;
}

void DxSkinCharData::RenderAABBBox(LPDIRECT3DDEVICEQ pd3dDevice)
{
	if (!m_strFileName.empty())
	    EDITMESHS::RENDERAABB(pd3dDevice, m_vMax, m_vMin);
}

boost::shared_ptr<DxSkinAniControlData>	DxSkinCharData::GetSkinAniControlData()	const
{
	return m_spSkinAniControlData;
}

const TCHAR* DxSkinCharData::GetSkeletonName() const
{
	return m_spSkinAniControlData->GetSkeletonName();
}

std::tr1::shared_ptr< DxFootColl > DxSkinCharData::GetFootColl() const
{
	return m_spSkinAniControlData->GetFootColl();
}




//--------------------------------------------------------------------------[DxSkinCharDataContainer]
//
DxSkinCharDataContainer::DxSkinCharDataContainer ()
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxSkinCharDataContainer::~DxSkinCharDataContainer ()
{
}

HRESULT DxSkinCharDataContainer::CleanUp ()
{
	SKINCHARDATAMAP_ITER iter = m_mapSkinCharData.begin ();
	SKINCHARDATAMAP_ITER iterEnd = m_mapSkinCharData.end ();
	
	for ( ; iter!=iterEnd; ++iter)
		delete (*iter).second;

	m_mapSkinCharData.clear();
	return S_OK;
}

DxSkinCharData* DxSkinCharDataContainer::FindData(const std::string& FileName)
{
	SKINCHARDATAMAP_ITER iter = m_mapSkinCharData.find(FileName);
	if (iter != m_mapSkinCharData.end())
        return iter->second;
    else
	    return NULL;
}

BOOL DxSkinCharDataContainer::DeleteData(const std::string& FileName)
{
	SKINCHARDATAMAP_ITER iter = m_mapSkinCharData.find(FileName);
	if (iter == m_mapSkinCharData.end())
        return FALSE;

    DxSkinCharData* pData = iter->second;
	delete pData;
    pData = NULL;
	m_mapSkinCharData.erase(iter);
	return TRUE;
}

void DxSkinCharDataContainer::ReleaseData(const std::string& FileName)
{
	DxSkinCharData* pRes = FindData(FileName);
	if (pRes)
	{
        if (pRes->GetRef() <= 1)
		    DeleteData(FileName);
        else
	        pRes->ReleaseRef();
    }
}

DxSkinCharData* DxSkinCharDataContainer::LoadData(
    const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxSkinCharData* pCharData = FindData(FileName);
	if (pCharData)
	{
		pCharData->AddRef();
		return pCharData;
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	pCharData = new DxSkinCharData;
	BOOL bOk = pCharData->LoadFile(FileName, pd3dDevice);
	if (!bOk)
	{
		delete pCharData;
		return NULL;
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    pCharData->AddRef();
	CDebugSet::ToListView("DxSkinCharData::LoadFile, %s", FileName.c_str());

	// Ʈ���� �ޱ�
	m_mapSkinCharData.insert(SKINCHARDATAMAP_VALUE(pCharData->GetFileName(), pCharData));

	return pCharData;
}

void DxSkinCharDataContainer::CreateChfBillboard( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* FileName )
{
	//DxSkinCharData* pSkinCharData = LoadData( FileName, pd3dDevice );
	//if ( !pSkinCharData )
	//	return;

	//DxSkinChar sSkinChar;
	//sSkinChar.SetCharData( pSkinCharData, pd3dDevice, FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	//sSkinChar.CreateChfBillboard( pd3dDevice );
}

void DxSkinCharDataContainer::Archive( boost::function<void (const TCHAR*)> fun )
{
    BOOST_FOREACH(SKINCHARDATAMAP_VALUE& it, m_mapSkinCharData)
    {
        fun( (m_szPath + it.first).c_str() );
    }
}

