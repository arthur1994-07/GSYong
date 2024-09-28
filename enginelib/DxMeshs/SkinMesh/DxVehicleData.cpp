#include "pch.h"

#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/File/FileUtil.h"
#include "../../../SigmaCore/File/FileFind.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/RENDERPARAM.h"

#include "../../G-Logic/GLogic.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../DxMeshs/SkinMesh/DxJiggleBone.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../Archive/ArchiveType.h"
#include "../../Archive/Archive.h"

#include "./NSSkinCharCommon.h"
#include "./DxFootColl.h"
#include ".\dxvehicledata.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxVehicleDataContainer& DxVehicleDataContainer::GetInstance()
{
	static DxVehicleDataContainer Instance;
	return Instance;
}


//-----------------------------------------------------------------------------------[DxVehicleData]
//
DxVehicleData::DxVehicleData(void)
	: m_dwRef(0)
	, m_bWorldObj(FALSE)
	, m_fScale(1.0f)
	, m_vMax(7.f,20.f,7.f)
	, m_vMin(-7.f,0.f,-7.f)
	, m_fHeight(20.f)
	, m_fRadius(7.f)
	, m_spSkinAniControlData(new DxSkinAniControlData)
{
	memset( m_szFileName, 0, FILE_LENGTH );

	//for ( int i=0; i<PART_V_SIZE; ++i )
	//{
	//	m_V_PartsArray[i] = NULL;
	//}

	//for ( int i=0; i<PART_V_SIZE; ++i )
	//{
	//	m_V_AttBoneArray[i] = NULL;
	//}

	for ( int i=0; i<PART_V_SIZE; i++ )
	{
		m_strVehicle[i] = _T("");
	}

	for ( int i=0; i<MAX_PASSENGER_COUNT; ++i )
	{
		m_sPassengerData[i].Init();
	}
}

DxVehicleData::~DxVehicleData(void)
{
	ClearAll ();
}

HRESULT DxVehicleData::ClearAll ()
{
	m_dwRef = 0;
	m_fScale = 1.0f;

	m_vMax = D3DXVECTOR3(7,20,7);
	m_vMin = D3DXVECTOR3(-7,0,-7);

	m_vecANIFILE.clear();

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
		m_strVehicle[i] = _T("");
	}

	//for ( int i=0; i<PART_V_SIZE; ++i )
	//{
	//	//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
	//	//
	//	if ( m_V_PartsArray[i] )
	//	{
	//		DxSkinPieceContainer::GetInstance().ReleasePiece ( m_V_PartsArray[i]->m_szFileName );
	//	}
	//	m_V_PartsArray[i] = NULL;

	//	if ( m_V_AttBoneArray[i] ) 
	//	{
	//		DxAttBoneLinkContainer::GetInstance().ReleaseLink ( m_V_AttBoneArray[i]->m_szFileName );
	//	}
	//	m_V_AttBoneArray[i] = NULL;
	//}

	//DxSkinAniControl::ClearAll ();

	m_spSkinAniControlData->CleanUp();

	return S_OK;
}
//
//HRESULT DxVehicleData::SetVehiclePart ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bAniThread, DWORD dwFlag, BOOL bAttachSub /* = FALSE */ )
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
//			for ( int i=0; i<PART_V_SIZE; ++i )
//			{
//				if ( m_V_AttBoneArray[i] && m_V_AttBoneArray[i]->m_szFileName )
//				{
//					if ( !strcmp(m_V_AttBoneArray[i]->m_szFileName,szFile) )
//					{
//						m_V_AttBoneArray[i] = NULL;
//					}
//				}
//			}
//
//			DxAttBoneLinkContainer::GetInstance().DeleteLink ( szFile );
//		}
//
//		//	Note : Attach Bone�� �о�´�.
//		//
//        DxAttBoneLink* pAttBoneLink = DxAttBoneLinkContainer::GetInstance().LoadLink(std::string(szFile), pd3dDevice, bMeshThread, bAniThread);
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
//                    "Incorrect bone %1% %2%", strFile1.GetString(), strFile2.GetString()));
//			CDebugSet::ToLogFile(ErrorMsg);
//			return E_DIFFERENT_BONEFILE;
//		}
//
//		//	Note : ���� �̹� ���� Piece Type �� ������ Piece �� �ִٸ� �����ش�.
//		//
//		if ( m_V_AttBoneArray[pAttBoneLink->m_emPieceCharType] )
//		{
//			DxSkinPieceContainer::GetInstance().ReleasePiece ( szFile );
//			m_V_AttBoneArray[pAttBoneLink->m_emPieceCharType] = NULL;
//		}
//		if ( m_V_AttBoneArray[pAttBoneLink->m_emPieceCharType] )
//		{
//			DxAttBoneLinkContainer::GetInstance().ReleaseLink ( szFile );
//		}
//
//		//	Note : Piece �� ����.
//		//
//		m_V_AttBoneArray[pAttBoneLink->m_emPieceCharType] = pAttBoneLink;
//
//		if( pAttBoneLink->m_bUseCPS )
//		{
//			SetVehiclePart( pAttBoneLink->m_strPieceName.c_str(), pd3dDevice, bMeshThread, bAniThread, dwFlag, TRUE );
//		}
//
//	}else{
//
//		//	Note : ���� ó�� �ε�� ���� �����ϰ� ������ �ε��Ҷ����� ���� �ε�Ȱ� �����ش�.
//		//
//		if ( dwFlag&FG_MUSTNEWLOAD )
//		{
//			for ( int i=0; i<PART_V_SIZE; ++i )
//			{
//				if ( m_V_PartsArray[i] && m_V_PartsArray[i]->m_szFileName )
//				{
//					if ( !strcmp(m_V_PartsArray[i]->m_szFileName,szFile) )
//					{
//						m_V_PartsArray[i] = NULL;
//					}
//				}
//			}
//
//			DxSkinPieceContainer::GetInstance().DeletePiece ( szFile );
//		}
//
//		//	Note : Piece�� �о�´�.
//		//
//		DxSkinPiece* pVehicle = DxSkinPieceContainer::GetInstance().LoadPiece( szFile, pd3dDevice, bMeshThread );
//		if ( !pVehicle )	return E_FAIL;
//		if ( !pVehicle->m_emPieceType == PIECETYPE_ATTACHBONE ) return E_FAIL;
//
//		if ( dwFlag&FG_BONEFILE_CHECK )
//		{
//			if ( pVehicle->m_pmcMesh->pSkinInfo )
//			{
//				CString strFile1( m_spSkinAniControlData->GetSkeletonName() );
//				strFile1.MakeLower();
//
//				CString strFile2( pVehicle->m_szSkeleton );
//				strFile2.MakeLower();
//
//				if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
//				{
//                    std::string ErrorMsg(
//                        sc::string::format(
//                            "Incorrect bone %1% %2%", strFile1.GetString(), strFile2.GetString()));
//					CDebugSet::ToLogFile(ErrorMsg);
//					return E_DIFFERENT_BONEFILE;
//				}
//			}
//		}
//
//		if( !bAttachSub )
//		{
//			if ( m_V_AttBoneArray[pVehicle->m_emV_PartType] )
//			{
//				DxAttBoneLinkContainer::GetInstance().ReleaseLink ( szFile );
//			}
//
//			m_V_AttBoneArray[pVehicle->m_emV_PartType] = NULL;
//		}
//
//		//	Note : ���� �̹� ���� Vehicle Type �� ������ Piece �� �ִٸ� �����ش�.
//		//
//		if ( m_V_PartsArray[pVehicle->m_emV_PartType] )
//		{
//			DxSkinPieceContainer::GetInstance().ReleasePiece ( szFile );
//		}
//
//		//	Note : Piece �� ����.
//		//
//		m_V_PartsArray[pVehicle->m_emV_PartType] = pVehicle;
//	}
//
//	return S_OK;
//}

//void DxVehicleData::ResetVehiclePart ( LPDIRECT3DDEVICEQ pd3dDevice, int i )
//{
//	GASSERT(i<PART_V_SIZE);
//
//	PDXSKINPIECE pVehicle = GetVehiclePart (i);
//
//	//	Note : ������ �÷��� Piece �� �ִٸ� �̸� �����ش�.
//	//
//	if ( pVehicle )
//	{
//		DxSkinPieceContainer::GetInstance().ReleasePiece ( pVehicle->m_szFileName );
//	}
//	m_V_PartsArray[i] = NULL;
//
//	PDXATTBONELINK pAttBoneLink = GetAttachBone(i);
//	if( pAttBoneLink )
//	{
//		DxAttBoneLinkContainer::GetInstance().ReleaseLink ( pAttBoneLink->m_szFileName );
//	}
//	m_V_AttBoneArray[i] = NULL;
//}

HRESULT DxVehicleData::SetPieceEDIT( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlag )
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
		//		if( m_strVehicle[i] == szFile )
		//		{
		//			m_strVehicle[i] = _T("");
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
		m_strVehicle[pAttBoneLink->m_emPieceCharType] = szFile;

		//if( pAttBoneLink->m_bUseCPS )
		//{
		//	SetPieceEDIT( pAttBoneLink->m_strPieceName.c_str(), pd3dDevice, emFlags, dwFlag );
		//}

	}
	else
	{

		////	Note : ���� ó�� �ε�� ���� �����ϰ� ������ �ε��Ҷ����� ���� �ε�Ȱ� �����ش�.
		////
		//if ( dwFlag&FG_MUSTNEWLOAD )
		//{
		//	for ( int i=0; i<PART_V_SIZE; ++i )
		//	{
		//		if( m_strVehicle[i] == szFile )
		//		{
		//			m_strVehicle[i] = _T("");
		//		}
		//	}

		//	DxSkinPieceContainer::GetInstance().DeletePiece ( szFile );
		//}

		//	Note : Piece�� �о�´�.
		//
		DxSkinPiece* pVehicle = DxSkinPieceContainer::GetInstance().LoadPiece( szFile, emFlags );
		if ( !pVehicle )	return E_FAIL;
		if ( !pVehicle->m_emPieceType == PIECETYPE_ATTACHBONE ) 
			return E_FAIL;

		// [shhan][2015.06.08] Link �� �ƴ� Skin �� ���� �ؾ� �ϰ� ThreadLoading �̱� ������
		//						DxCharPart::SetPart_Detail Ÿ�ֿ̹��� Ȯ���ϴ� ������ ����Ǿ���.
		//
		////if ( dwFlag&FG_BONEFILE_CHECK )
		//{
		//	//if ( pVehicle->m_pmcMesh && pVehicle->m_pmcMesh->pSkinInfo )
		//	{
		//		CString strFile1( m_spSkinAniControlData->GetSkeletonName() );
		//		strFile1.MakeLower();

		//		CString strFile2( pVehicle->m_szSkeleton );
		//		strFile2.MakeLower();

		//		if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
		//		{
		//			CDebugSet::ToLogFile
		//			( 
		//				sc::string::format( _T("Base Bone : %1%, Error cps : %2%, Error Bone : %3% \n"), m_spSkinAniControlData->GetSkeletonName(), szFile, pVehicle->m_szSkeleton ) 
		//			);

		//			return E_DIFFERENT_BONEFILE;
		//		}
		//	}
		//}

		//	Note : Piece �� ����.
		//
		m_strVehicle[pVehicle->m_emV_PartType] = szFile;
	}

	return S_OK;
}

void DxVehicleData::ResetPieceEDIT( int i )
{
	MIN_ASSERT(i<PART_V_SIZE);

	m_strVehicle[i] = _T("");
}

BOOL DxVehicleData::LoadAnimationFromTool( const TCHAR* szNameCHF, const TCHAR* szNameCFG, const TCHAR* szNameXFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH /*=false*/ )
{
	NSSkinCharCommon::LoadAnimationFromTool( szNameCHF, szNameCFG, szNameXFile, pd3dDevice, bREPRESH, *m_spSkinAniControlData.get(), m_vecANIFILE );

	return TRUE;
}

void DxVehicleData::ReleaseAnimationFromTool( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* szAnim )
{
	NSSkinCharCommon::ReleaseAnimationFromTool( pd3dDevice, szAnim, *m_spSkinAniControlData.get(), m_vecANIFILE );
}

DxSkinPieceBase* DxVehicleData::GetVehiclePart ( LPDIRECT3DDEVICEQ pd3dDevice, int i, EMSCD_FLAGS emFlags )
{
	if( i >= PART_V_SIZE )
	{
		return NULL;
	}

	char ext[_MAX_EXT] = {0};
	_splitpath( m_strVehicle[i].c_str(), NULL, NULL, NULL, ext );

	CString strExt(ext);
	strExt.MakeUpper();

	// ���� �ε��� ������ Attach Bone Link �����̸�...
	if( strExt == _T(".ABL") )
	{
		return DxAttBoneLinkContainer::GetInstance().LoadLink( m_strVehicle[i], emFlags );
	}
	else
	{
		return DxSkinPieceContainer::GetInstance().LoadPiece( m_strVehicle[i], emFlags );
	}
}

const TSTRING& DxVehicleData::GetVehicleName( int i )
{
	MIN_ASSERT(i<PART_V_SIZE);

	return m_strVehicle[i];
}

BOOL DxVehicleData::SaveFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive )
{
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxVehicleDataContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );

    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return FALSE;
    }

    sc::SerialFile& SFile = *spFile;

	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		return FALSE;

	SFile << VERSION;

	SFile << PART_V_SIZE;
	for ( int i=0; i<PART_V_SIZE; ++i )
	{	
		if( m_strVehicle[i].empty() )
		{
			SFile << BOOL(FALSE);
			continue;
		}
		else
		{
			SFile << BOOL(TRUE);
		}

		SFile << m_strVehicle[i];
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

	//	Note : ����Ʈ�� �ٸ� �߰� ��ü�� �� ���� ����ؼ� ���� ����Ʈ�� ���� �Ǹ�
	//		TRUE ���� �����ϰ� �ȴ�.
	//
	SFile << (BOOL)FALSE;

	for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		SFile.WriteBuffer ( m_sPassengerData[i].vPos, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer ( m_sPassengerData[i].vRotate, sizeof(D3DXVECTOR3) );
		SFile << m_sPassengerData[i].strBoneName;
	}

	//	Note : ��Ų �ɸ��� �̸� ����.
	StringCchCopy( m_szFileName, FILE_LENGTH, szFile );

	return TRUE;
}

BOOL DxVehicleData::LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVehicleSize;

	SFile >> dwVehicleSize;
	if ( dwVehicleSize > PART_V_SIZE )		return FALSE;

	//	Note : ������ �ε�.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwVehicleSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strVehicle[i].clear();
			continue;
		}

		SFile >> m_strVehicle[i];
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

	for ( DWORD i=0; i<dwAniNum; ++i )
	{
		std::string strANIFILE;
		SFile >> strANIFILE;

		strANIFILE = sc::util::ChangeExtName( strANIFILE, _T("cfg") );

		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T('['), _T('_') );
		strANIFILE = sc::util::GetChangeTCHAR( strANIFILE, _T(']') );

		m_vecANIFILE.push_back ( strANIFILE.c_str() );

	}

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

	for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		SFile.ReadBuffer ( m_sPassengerData[i].vPos, sizeof(D3DXVECTOR3) );
		SFile.ReadBuffer ( m_sPassengerData[i].vRotate, sizeof(D3DXVECTOR3) );
		SFile >> m_sPassengerData[i].strBoneName;
	}

	return TRUE;
}

BOOL DxVehicleData::LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( LOAD_Ver100( SFile, pd3dDevice ) == FALSE )
		return FALSE;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if ( bExist )
	{
		m_spSkinAniControlData->LoadPrevFootColl( SFile );
	}

	return TRUE;
}

BOOL DxVehicleData::LOAD_Ver102( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( LOAD_Ver101( SFile, pd3dDevice ) == FALSE )
		return FALSE;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_spSkinAniControlData->ErrorLoadJiggleBone( SFile );
	}

	return TRUE;
}

BOOL DxVehicleData::LOAD_Ver103( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( LOAD_Ver100( SFile, pd3dDevice ) == FALSE )
		return FALSE;

	return TRUE;
}

// ��Ų ������ ����.
// BOOL DxSkinDataDummy::LoadFile () ������ ���� ������ �ε��� ���ϹǷ� �� �ε�/���̺� �κ��� �ٲ��
// �����ؼ� ���� ������ �ʿ��ϴ�.
BOOL DxVehicleData::LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, boost::function<void (const TCHAR*)> pArchive )
{
	m_spSkinAniControlData->CleanUp();

    std::string PathName(DxVehicleDataContainer::GetInstance().GetPath());
	PathName.append(FileName);

	SPBaseStream spFile;

	if (Archive::CreateFileBaseStream(PathName.c_str(), FileName.c_str(), GLOGIC::strSKINOBJ_ZIPFILE.c_str(), pArchive, spFile) == false)
	{
		return FALSE;
	}

	sc::BaseStream& SFile = *spFile;




	//std::auto_ptr<sc::BaseStream> pBStream(
 //       GLOGIC::openfile_basestream(
 //           GLOGIC::UseEngineZipFile(), 
	//	    GLOGIC::strSKINOBJ_ZIPFILE, 
	//	    PathName.c_str(), 
	//	    FileName));

	//if (!pBStream.get())
	//	return FALSE;

	//sc::BaseStream& SFile = *pBStream;

	// Note : Init Data
	m_fHeight = 20.f;
	m_fRadius = 7.f;

	DWORD dwVer;
	SFile >> dwVer;

	switch ( dwVer )
	{
	case 0x0100:
		LOAD_Ver100( SFile, pd3dDevice );
		break;
	case 0x0101:
		LOAD_Ver101( SFile, pd3dDevice );
		break;
	case 0x0102:
		LOAD_Ver102( SFile, pd3dDevice );
		break;
	case 0x0103:
		LOAD_Ver103( SFile, pd3dDevice );
		break;
	default:
		return FALSE;
	};


	//	Note : ������ �б�.
	//
	if ( !m_spSkinAniControlData->IsValidSkeleton() )		
        return FALSE;

	// Bone Setting File Loading
	m_spSkinAniControlData->LoadFile_BSF( pArchive );

	DWORD i;
	for ( i=0; i<PART_V_SIZE; ++i )
	{
		if ( !m_strVehicle[i].empty() )
		{
			// �븻���� �Ұ��� �� ��� �븻���� ������� �ʴ� �ǽ� ������ �ε��Ѵ�.
			if( !RENDERPARAM::bUseNormalMap || !DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader() )
			{
				int idx = static_cast<int> (m_strVehicle[i].find( "_n." ));
				if( idx!=std::string::npos )	
				{

					char szVehicleName[_MAX_FNAME]={0,};
					char fname[_MAX_FNAME]={0,};
					char ext[_MAX_EXT]={0,};
					_splitpath( m_strVehicle[i].c_str(), NULL, NULL, fname, ext );
					strncpy( szVehicleName, fname, strlen(fname)-2);
					strcat( szVehicleName, ext );					

					if( SetPieceEDIT ( szVehicleName, pd3dDevice, emFlags, 0L ) != E_FAIL ) 
						continue;
				}
			}
			// �������� �Ұ��� �� ��� �������̸� ������� �ʴ� �ǽ� ������ �ε��Ѵ�.
			if( !DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader() )
			{
				int idx = static_cast<int> (m_strVehicle[i].find( "_o." ));
				if( idx!=std::string::npos )	
				{

					char szVehicleName[_MAX_FNAME]={0,};
					char fname[_MAX_FNAME]={0,};
					char ext[_MAX_EXT]={0,};
					_splitpath( m_strVehicle[i].c_str(), NULL, NULL, fname, ext );
					strncpy( szVehicleName, fname, strlen(fname)-2);
					strcat( szVehicleName, ext );					

					if( SetPieceEDIT ( szVehicleName, pd3dDevice, emFlags, 0L ) != E_FAIL ) 
						continue;
				}
			}				
			// ��Ż ���̴� �Ұ��� �� ��� ��Ż ���̴��� ������� �ʴ� �ǽ� ������ �ε��Ѵ�.
			if( !DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader() )
			{
				int idx = static_cast<int> (m_strVehicle[i].find( "_t." ));
				if( idx!=std::string::npos )	
				{

					char szVehicleName[_MAX_FNAME]={0,};
					char fname[_MAX_FNAME]={0,};
					char ext[_MAX_EXT]={0,};
					_splitpath( m_strVehicle[i].c_str(), NULL, NULL, fname, ext );
					strncpy( szVehicleName, fname, strlen(fname)-2);
					strcat( szVehicleName, ext );					

					if( SetPieceEDIT ( szVehicleName, pd3dDevice, emFlags, 0L ) != E_FAIL ) 
						continue;
				}
			}				
			SetPieceEDIT ( m_strVehicle[i].c_str(), pd3dDevice, emFlags, 0L );
		}		
	}

	//for ( i=0; i<(DWORD)m_vecANIFILE.size(); ++i )
	//{
	//	DxSkinAniControl::LoadAnimation ( m_vecANIFILE[i].c_str(), pd3dDevice );

	//	//// Represh ��� ������ bin ������ �� �����ϵ��� �Ѵ�.
	//	//if( bREPRESH )
	//	//{
	//	//	HRESULT hr = skinAniRes.get()->CreateAnimationDataFromTool( bREPRESH );
	//	//	if ( FAILED(hr) )
	//	//	{
	//	//		std::string ErrorMsg( sc::string::format( "Animation X-File Load failed : %1%",	skinAniRes.get()->m_strCfgFile ));
	//	//		CDebugSet::ToLogFile( ErrorMsg );
	//	//	}
	//	//}
	//}

	//DxSkinAniControl::ClassifyAnimation ();

	BOOL bAniThread(FALSE);
	if ( emFlags & EMSCD_ANI_THREAD )
	{
		bAniThread = TRUE;
	}

	m_spSkinAniControlData->CreateSkinAni( m_vecANIFILE, bAniThread );

	//for ( size_t n=0; n<m_spSkinAniControlData->m_skinAniResVec.size(); ++n )
	//{
	//	int MType = m_spSkinAniControlData->m_skinAniResVec[n].get()->m_MainType;
	//	int SType = m_spSkinAniControlData->m_skinAniResVec[n].get()->m_SubType;
	//	m_spSkinAniControlData->m_vecAniType[MType][SType].push_back( m_spSkinAniControlData->m_skinAniResVec[n] );
	//}

	//// �ʱ� ���ϸ��̼� ����.
	//DxSkinAniControl::SELECTANI ( AN_GUARD_N );

	//if ( !DxSkinAniControl::GETCURANIMNODE() )
	//{
 //       std::string ErrorMsg(
 //           sc::string::format(
 //               "select ani fail %1%", FileName));
	//	CDebugSet::ToLogFile(ErrorMsg);
	//}

	//// Char Editor���� ������ �� ������ �� ����
	//DxSkinAniControl::SetBoneScaleList( m_deqBoneEditScale );

	// ��Ų �ɸ��� �̸� ����.
	StringCchCopy(m_szFileName, FILE_LENGTH, FileName.c_str());
	return TRUE;
}

//HRESULT DxVehicleData::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
//{
//	D3DXVECTOR3 vLastMax(-FLT_MAX,-FLT_MAX,-FLT_MAX), vLastMin(FLT_MAX,FLT_MAX,FLT_MAX);
//	D3DXVECTOR3 vMax, vMin;
//
//	for ( int i=0; i<PART_V_SIZE; ++i )
//	{
//		if ( !m_V_PartsArray[i] )
//		{
//			continue;//	������ �Ϻκ� �������� �����Ƿ� ������ ������. ( �������� �Ǵ�. )
//		}
//
//		HRESULT hr = m_V_PartsArray[i]->CalculateBoundingBox ( pd3dDevice, vMax, vMin );
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

void DxVehicleData::SetRadiusHeight()
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

void DxVehicleData::RenderAABBBox( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !strlen( m_szFileName ) )	return;

	EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );
}

boost::shared_ptr<DxSkinAniControlData>	DxVehicleData::GetSkinAniControlData()	const
{
	return m_spSkinAniControlData;
}

std::tr1::shared_ptr< DxFootColl > DxVehicleData::GetFootColl() const
{
	return m_spSkinAniControlData->GetFootColl();
}



//--------------------------------------------------------------------------[DxVehicleDataContainer]
//
DxVehicleDataContainer::DxVehicleDataContainer ()
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxVehicleDataContainer::~DxVehicleDataContainer ()
{
}

HRESULT DxVehicleDataContainer::CleanUp ()
{
	VEHICLEDATAMAP_ITER iter = m_mapVehicleData.begin ();
	VEHICLEDATAMAP_ITER iterEnd = m_mapVehicleData.end ();

	for ( ; iter!=iterEnd; iter++ )
	{
		delete (*iter).second;
	}

	m_mapVehicleData.clear ();

	return S_OK;
}

void DxVehicleDataContainer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
}

DxVehicleData* DxVehicleDataContainer::FindVehicleData ( const char* szFile )
{
	VEHICLEDATAMAP_ITER iter = m_mapVehicleData.find ( std::string(szFile) );
	if ( iter != m_mapVehicleData.end() )	return iter->second;

	return NULL;
}

BOOL DxVehicleDataContainer::DeleteVehicleData ( const char* szFile )
{
	VEHICLEDATAMAP_ITER iter = m_mapVehicleData.find ( std::string(szFile) );
	if ( iter == m_mapVehicleData.end() )	return FALSE;

	delete (*iter).second;
	m_mapVehicleData.erase ( iter );

	return TRUE;
}

void DxVehicleDataContainer::ReleaseVehicleData ( const char* szFile )
{
	//	Note : ���� ���� ��ä �ı� �����󿡼� �ļ����϶� ���� ���� ���ɼ��� ����.
	if ( m_mapVehicleData.empty() )		return;

	VEHICLEDATAMAP_ITER iter = m_mapVehicleData.find ( std::string(szFile) );
	if ( iter==m_mapVehicleData.end() )	return;

	iter->second->m_dwRef--;
	if ( iter->second->m_dwRef == 0 )
	{
		delete iter->second;
		m_mapVehicleData.erase ( iter );

	}


	return;
}

DxVehicleData* DxVehicleDataContainer::LoadVehicleData( const char* szFile, EMSCD_FLAGS emFlags )
{
	DxVehicleData* pVehicleData;

	pVehicleData = FindVehicleData( szFile );
	if ( pVehicleData )
	{
		pVehicleData->m_dwRef++;
		return pVehicleData;
	}

	pVehicleData = new DxVehicleData;
	BOOL bOk = pVehicleData->LoadFile( szFile, m_pd3dDevice, emFlags );
	if ( !bOk )
	{
		SAFE_DELETE(pVehicleData);
		return NULL;
	}
	pVehicleData->m_dwRef++;

	CDebugSet::ToListView ( "DxVehicleData::LoadVehicleData, %s", szFile );

	//	Note : Ʈ���� �ޱ�.
	//
	m_mapVehicleData[std::string(pVehicleData->GetFileName())] = pVehicleData;

	DxVehicleData* pTestVehicleData = FindVehicleData( szFile );

	return pTestVehicleData ;
}

//
static void FindDontHaveTexture( const TCHAR* FileName, const TCHAR* pExt )
{
	TSTRING strName = FileName;
	std::transform( strName.begin(), strName.end(), strName.begin(), tolower );

	strName = sc::file::getFileExt( strName );
	if ( strName != pExt )
		return;

	DxVehicleDataContainer::GetInstance().LoadVehicleData( FileName, 0L );
}

// �ε��� ���� �غ����� �Ѵ�.
void DxVehicleDataContainer::OnFindDontHaveTexture()
{
	sc::CFileFindTree	m_sFileTree;
	m_sFileTree.CreateTree( m_szPath, FALSE );
	m_sFileTree.SomeWorkSameExt( FindDontHaveTexture, "vps" );
}