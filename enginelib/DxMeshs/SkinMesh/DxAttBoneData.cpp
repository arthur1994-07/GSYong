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
#include "../../DxMeshs/SkinMesh/DxSkinAniControl.h"
#include "../../DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../DxMeshs/SkinMesh/DxJiggleBone.h"
#include "../../DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../Archive/Archive.h"

#include "./NSSkinCharCommon.h"

#include "DxAttBoneData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxAttBoneDataContainer& DxAttBoneDataContainer::GetInstance()
{
	static DxAttBoneDataContainer Instance;
	return Instance;
}


//-----------------------------------------------------------------------------------[DxAttBoneData]
//
DxAttBoneData::DxAttBoneData(void)
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

	//for ( int i=0; i<ATTBONE_SIZE; ++i )
	//{
	//	m_pAttBoneArray[i] = NULL;
	//}
	for ( int i=0; i<ATTBONE_SIZE; i++ )
	{
		m_strATTBONE[i] = _T("");
	}
}

DxAttBoneData::~DxAttBoneData(void)
{
	ClearAll ();
}

HRESULT DxAttBoneData::ClearAll ()
{
	m_dwRef = 0;
	m_fScale = 1.0f;

	m_vMax = D3DXVECTOR3(7,20,7);
	m_vMin = D3DXVECTOR3(-7,0,-7);

	m_vecANIFILE.clear();

	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_strATTBONE[i] = _T("");

		////	Note : 만약 이미 읽은 Piece Type 에 지정된 Piece 가 있다면 지워준다.
		////
		//if ( m_pAttBoneArray[i] )
		//{
		//	DxSkinPieceContainer::GetInstance().ReleasePiece ( m_pAttBoneArray[i]->m_szFileName );
		//}
		//m_pAttBoneArray[i] = NULL;
	}

	// [ Thread Work shhan ]
	//DxSkinAniControl::ClearAll ();

	m_spSkinAniControlData->CleanUp();

	return S_OK;
}

HRESULT DxAttBoneData::SetAttBonePiece ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlag )
{
	////	Note : 만약 처음 로드된 것을 무시하고 새로이 로드할때에는 원래 로드된걸 지워준다.
	////
	//if ( dwFlag&FG_MUSTNEWLOAD )
	//{
	//	for ( int i=0; i<ATTBONE_SIZE; ++i )
	//	{
	//		if( m_strATTBONE[i] == szFile )
	//		{
	//			m_strATTBONE[i] = _T("");
	//		}
	//	}

	//	DxSkinPieceContainer::GetInstance().DeletePiece ( szFile );
	//}

	//	Note : Piece를 읽어온다.
	//
	DxSkinPiece* pAttBone = DxSkinPieceContainer::GetInstance().LoadPiece( szFile, emFlags );
	if ( !pAttBone )	return E_FAIL;
	if ( !pAttBone->m_emPieceType == PIECETYPE_ATTACHBONE ) return E_FAIL;

	// [shhan][2015.06.08] Link 가 아닌 Skin 일 때만 해야 하고 ThreadLoading 이기 때문에
	//						DxCharPart::SetPart_Detail 타이밍에서 확인하는 것으로 변경되었다.
	//
	//// [shhan][2014.11.26] Bone 정보가 다를 경우 절대 Part 를 넣으면 안된다.
	////						DxSkeletonMaintain::FindBone_Index 에서 NULL 이 리턴되면서 문제가 생길 수 있다.
	////
	////if ( dwFlag&FG_BONEFILE_CHECK )
	//{
	//	//if ( pAttBone->m_pmcMesh && pAttBone->m_pmcMesh->pSkinInfo )
	//	{
	//		CString strFile1( m_spSkinAniControlData->GetSkeletonName() );
	//		strFile1.MakeLower();

	//		CString strFile2( pAttBone->m_szSkeleton );
	//		strFile2.MakeLower();

	//		if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
	//		{
	//			CDebugSet::ToLogFile
	//			( 
	//				sc::string::format( _T("Base Bone : %1%, Error cps : %2%, Error Bone : %3% \n"), m_spSkinAniControlData->GetSkeletonName(), szFile, pAttBone->m_szSkeleton )
	//			);

	//			return E_DIFFERENT_BONEFILE;
	//		}
	//	}
	//}

	////	Note : 만약 이미 읽은 AttBone Type 에 지정된 Piece 가 있다면 지워준다.
	////
	//if ( m_pAttBoneArray[pAttBone->m_emAttBoneType] )
	//{
	//	DxSkinPieceContainer::GetInstance().ReleasePiece ( szFile );
	//}

	//	Note : Piece 를 지정.
	//
	// 
	m_strATTBONE[pAttBone->m_emAttBoneType] = szFile;

	return S_OK;
}

void DxAttBoneData::ResetAttBonePiece ( LPDIRECT3DDEVICEQ pd3dDevice, int i )
{
	//GASSERT(i<ATTBONE_SIZE);

	//PDXSKINPIECE pAttBone = GetAttBonePiece (i);

	////	Note : 이전에 올려진 Piece 가 있다면 이를 내려준다.
	////
	//if ( pAttBone )
	//{
	//	DxSkinPieceContainer::GetInstance().ReleasePiece ( pAttBone->m_szFileName );
	//	m_pAttBoneArray[i] = NULL;
	//}

	MIN_ASSERT(i<ATTBONE_SIZE);

	m_strATTBONE[i] = _T("");
}

BOOL DxAttBoneData::LoadAnimationFromTool( const TCHAR* szNameCHF, const TCHAR* szNameCFG, const TCHAR* szNameXFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH /*=false*/ )
{
	NSSkinCharCommon::LoadAnimationFromTool( szNameCHF, szNameCFG, szNameXFile, pd3dDevice, bREPRESH, *m_spSkinAniControlData.get(), m_vecANIFILE );

	return TRUE;
}

void DxAttBoneData::ReleaseAnimationFromTool( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* szAnim )
{
	NSSkinCharCommon::ReleaseAnimationFromTool( pd3dDevice, szAnim, *m_spSkinAniControlData.get(), m_vecANIFILE );
}

PDXSKINPIECE DxAttBoneData::GetAttBonePiece ( LPDIRECT3DDEVICEQ pd3dDevice, int i, EMSCD_FLAGS emFlags )
{
	if( i >= ATTBONE_SIZE )
	{
		return NULL;
	}

	return DxSkinPieceContainer::GetInstance().LoadPiece( m_strATTBONE[i], emFlags );
}

const TSTRING& DxAttBoneData::GetAttBoneName( int i )
{
	MIN_ASSERT(i<ATTBONE_SIZE);

	return m_strATTBONE[i];
}

BOOL DxAttBoneData::SaveFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive )
{
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxAttBoneDataContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );

    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return FALSE;
    }

    sc::SerialFile& SFile = *spFile;

	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		return FALSE;

	SFile << VERSION;

	SFile << ATTBONE_SIZE;
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		if( m_strATTBONE[i].empty() )
		{
			SFile << BOOL(FALSE);
			continue;
		}
		else
		{
			SFile << BOOL(TRUE);
		}

		SFile << m_strATTBONE[i];
	}

	SFile << TSTRING( m_spSkinAniControlData->GetSkeletonName() );

	//	Note :에니메이션 쓰기.
	//
	SFile << static_cast<DWORD>( m_vecANIFILE.size() );

	for ( size_t n=0; n<m_vecANIFILE.size(); ++n )
	{
		SFile << m_vecANIFILE[n];
	}

	SFile << m_fScale;

	// Note : 추가.
	SFile << (BOOL)TRUE;
	SFile << m_fHeight;
	SFile << m_fRadius;

	//  Note : CharEditor에서 수정한 본 스케일
	SFile << (DWORD)m_spSkinAniControlData->GetBoneScaleList().size();

	SBONESCALE boneScale;
	for( size_t i = 0; i < m_spSkinAniControlData->GetBoneScaleList().size(); i++ )
	{
		SFile << m_spSkinAniControlData->GetBoneScaleList()[i].vScale;
		SFile << m_spSkinAniControlData->GetBoneScaleList()[i].strBoneName;
	}

	//	Note : 경계 박스 계산.
	//
	SFile << (BOOL) TRUE;
	//CalculateBoundingBox ( pd3dDevice, m_vMax, m_vMin );

	SFile << m_vMax;
	SFile << m_vMin;

	// Note : 추가.
	SFile << (BOOL)TRUE;
	SFile << m_bWorldObj;

	//	Note : 이펙트등 다른 추가 계체가 들어갈 것을 대비해서 설정 이펙트가 들어가게 되면
	//		TRUE 값을 저장하게 된다.
	//
	SFile << (BOOL)FALSE;

	//	Note : 스킨 케릭터 이를 설정.
	StringCchCopy( m_szFileName, FILE_LENGTH, szFile );

	return TRUE;
}

BOOL DxAttBoneData::LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwAttBoneSize;

	SFile >> dwAttBoneSize;
	if ( dwAttBoneSize > ATTBONE_SIZE )		return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwAttBoneSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strATTBONE[i].clear();
			continue;
		}

		SFile >> m_strATTBONE[i];
	}

	//	Note : 본파일 읽기.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :에니메이션 읽기.
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

	//  Note : CharEditor에서 수정한 본 스케일
	DWORD dwBoneScaleSize;
	SFile >> dwBoneScaleSize;

	SBONESCALE boneScale;
	for( size_t i = 0; i < dwBoneScaleSize; i++ )
	{
		SFile >> boneScale.vScale;
		SFile >> boneScale.strBoneName;

		m_spSkinAniControlData->AddBoneScale( boneScale );
	}

	//	Note : 바운드 박스 정보 로드.
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

	// Note : 덤프
	SFile >> bExist;

	return TRUE;
}

BOOL DxAttBoneData::LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwAttBoneSize;

	SFile >> dwAttBoneSize;
	if ( dwAttBoneSize > ATTBONE_SIZE )		return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwAttBoneSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strATTBONE[i].clear();
			continue;
		}

		SFile >> m_strATTBONE[i];
	}

	//	Note : 본파일 읽기.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :에니메이션 읽기.
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

	//  Note : CharEditor에서 수정한 본 스케일
	DWORD dwBoneScaleSize;
	SFile >> dwBoneScaleSize;

	SBONESCALE boneScale;
	for( size_t i = 0; i < dwBoneScaleSize; i++ )
	{
		SFile >> boneScale.vScale;
		SFile >> boneScale.strBoneName;

		m_spSkinAniControlData->AddBoneScale( boneScale );
	}

	//	Note : 바운드 박스 정보 로드.
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
	if( bExist )
	{
		m_spSkinAniControlData->ErrorLoadJiggleBone( SFile );
	}
	else			return TRUE;

	// Note : 덤프
	SFile >> bExist;

	return TRUE;
}

BOOL DxAttBoneData::LOAD_Ver102( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwAttBoneSize;

	SFile >> dwAttBoneSize;
	if ( dwAttBoneSize > ATTBONE_SIZE )		return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwAttBoneSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strATTBONE[i].clear();
			continue;
		}

		SFile >> m_strATTBONE[i];
	}

	//	Note : 본파일 읽기.
	//
	TSTRING strSkeletonName;
	SFile >> strSkeletonName;
	m_spSkinAniControlData->LoadSkeleton ( strSkeletonName.c_str(), pd3dDevice );

	//	Note :에니메이션 읽기.
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

	//  Note : CharEditor에서 수정한 본 스케일
	DWORD dwBoneScaleSize;
	SFile >> dwBoneScaleSize;

	SBONESCALE boneScale;
	for( size_t i = 0; i < dwBoneScaleSize; i++ )
	{
		SFile >> boneScale.vScale;
		SFile >> boneScale.strBoneName;

		m_spSkinAniControlData->AddBoneScale( boneScale );
	}

	//	Note : 바운드 박스 정보 로드.
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

	// Note : 덤프
	SFile >> bExist;

	return TRUE;
}

// 스킨 파일을 읽음.
// BOOL DxSkinDataDummy::LoadFile () 에서도 같은 구조로 로딩을 행하므로 이 로드/새이브 부분이 바뀔시
// 병행해서 같이 수정이 필요하다.
BOOL DxAttBoneData::LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlags, boost::function<void (const TCHAR*)> pArchive )
{
	m_spSkinAniControlData->CleanUp();

    if (FileName.empty())
        return FALSE;

    std::string PathName(DxAttBoneDataContainer::GetInstance().GetPath());
	PathName.append(FileName);

    /* 아카이브 기능 추가로 변경 된 코드, 향후 안정화되면 삭제 예정
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
	default:
		return FALSE;
	};


	//	
	if ( !m_spSkinAniControlData->IsValidSkeleton() )
        return FALSE;

	// Bone Setting File Loading
	m_spSkinAniControlData->LoadFile_BSF( pArchive );

	for ( DWORD i=0; i<ATTBONE_SIZE; ++i)
	{
		if (!m_strATTBONE[i].empty())
		{
			// 노말맵이 불가능 할 경우 노말맵을 사용하지 않는 피스 파일을 로드한다.
			if (!RENDERPARAM::bUseNormalMap || !DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader())
			{
				size_t idx = m_strATTBONE[i].find("_n.");
				if (idx != std::string::npos)
				{
					char szAttBoneName[_MAX_FNAME]={0};
					char fname[_MAX_FNAME]={0};
					char ext[_MAX_EXT]={0};
					_splitpath(m_strATTBONE[i].c_str(), NULL, NULL, fname, ext);
					strncpy(szAttBoneName, fname, strlen(fname)-2);
					strcat(szAttBoneName, ext);

					if (SetAttBonePiece(szAttBoneName, pd3dDevice, emFlags, dwFlags) != E_FAIL)
                        continue;
				}
			}
			// 오버레이 불가능 할 경우 오버레이를 사용하지 않는 피스 파일을 로드한다.
			if (!DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader())
			{
				size_t idx = m_strATTBONE[i].find("_o.");
				if (idx != std::string::npos)
				{
					char szAttBoneName[_MAX_FNAME]={0};
					char fname[_MAX_FNAME]={0};
					char ext[_MAX_EXT]={0};
					_splitpath(m_strATTBONE[i].c_str(), NULL, NULL, fname, ext);
					strncpy(szAttBoneName, fname, strlen(fname)-2);
					strcat(szAttBoneName, ext);

					if (SetAttBonePiece(szAttBoneName, pd3dDevice, emFlags, dwFlags) != E_FAIL)
                        continue;
				}
			}				
			// 토탈 셰이더 불가능 할 경우 토탈 셰이더를 사용하지 않는 피스 파일을 로드한다.
			if (!DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader())
			{
				size_t idx = m_strATTBONE[i].find("_t.");
				if (idx != std::string::npos)	
				{
					char szAttBoneName[_MAX_FNAME]={0};
					char fname[_MAX_FNAME]={0};
					char ext[_MAX_EXT]={0};
					_splitpath(m_strATTBONE[i].c_str(), NULL, NULL, fname, ext);
					strncpy(szAttBoneName, fname, strlen(fname)-2);
					strcat(szAttBoneName, ext);

					if (SetAttBonePiece(szAttBoneName, pd3dDevice, emFlags, dwFlags) != E_FAIL)
                        continue;
				}
			}				

			TSTRING strATTBONE(m_strATTBONE[i].c_str());
			SetAttBonePiece ( strATTBONE.c_str(), pd3dDevice, emFlags, dwFlags );
		}		
	}

	//for ( i=0; i<(DWORD)m_vecANIFILE.size(); ++i )
	//{
	//	DxSkinAniControl::LoadAnimation ( m_vecANIFILE[i].c_str(), pd3dDevice, bREPRESH );
	//}

	//DxSkinAniControl::ClassifyAnimation ();

	//// 초기 에니메이션 설정.
	//DxSkinAniControl::SELECTANI(AN_GUARD_N);

	//if (!DxSkinAniControl::GETCURANIMNODE())
	//{
 //       std::string ErrorMsg(
 //           sc::string::format(
 //               "select ani fail %1%", FileName));
	//	CDebugSet::ToLogFile(ErrorMsg);
	//}

	//// Char Editor에서 수정한 본 스케일 값 수정
	//DxSkinAniControl::SetBoneScaleList( m_deqBoneEditScale );

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


	// 스킨 케릭터 이를 설정.
	StringCchCopy(m_szFileName, FILE_LENGTH, FileName.c_str());

	return TRUE;
}

//HRESULT DxAttBoneData::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
//{
//	D3DXVECTOR3 vLastMax(-FLT_MAX,-FLT_MAX,-FLT_MAX), vLastMin(FLT_MAX,FLT_MAX,FLT_MAX);
//	D3DXVECTOR3 vMax, vMin;
//
//	for ( int i=0; i<ATTBONE_SIZE; ++i )
//	{
//		if ( !m_pAttBoneArray[i] )
//		{
//			continue;//	조각이 일부분 없을수도 있으므로 오류는 무시함. ( 정상으로 판단. )
//		}
//
//		HRESULT hr = m_pAttBoneArray[i]->CalculateBoundingBox ( pd3dDevice, vMax, vMin );
//
//		if ( FAILED(hr) )
//		{
//			continue;//	조각이 일부분 없을수도 있으므로 오류는 무시함. ( 정상으로 판단. )
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

void DxAttBoneData::SetRadiusHeight()
{
	m_vMax = D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_vMin = D3DXVECTOR3( 0.f, 0.f, 0.f );

	// Note : 높이 수정
	m_vMax.y = m_fHeight;

	// Note : 반지름 수정
	m_vMax.x = m_fRadius;
	m_vMax.z = m_fRadius;
	m_vMin.x = -m_fRadius;
	m_vMin.z = -m_fRadius;
}

void DxAttBoneData::RenderAABBBox( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !strlen( m_szFileName ) )	return;

	EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );
}

boost::shared_ptr<DxSkinAniControlData>	DxAttBoneData::GetSkinAniControlData()	const
{
	return m_spSkinAniControlData;
}




//--------------------------------------------------------------------------[DxAttBoneDataContainer]
//
DxAttBoneDataContainer::DxAttBoneDataContainer ()
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxAttBoneDataContainer::~DxAttBoneDataContainer ()
{
}

HRESULT DxAttBoneDataContainer::CleanUp ()
{
	ATTBONEDATAMAP_ITER iter = m_mapAttBoneData.begin ();
	ATTBONEDATAMAP_ITER iterEnd = m_mapAttBoneData.end ();

	for ( ; iter!=iterEnd; iter++ )
	{
		delete (*iter).second;
	}

	m_mapAttBoneData.clear ();

	return S_OK;
}

void DxAttBoneDataContainer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
}

DxAttBoneData* DxAttBoneDataContainer::FindAttBoneData(const std::string& FileName)
{
	ATTBONEDATAMAP_ITER iter = m_mapAttBoneData.find(FileName);
	if (iter != m_mapAttBoneData.end())
        return iter->second;
    else
	    return NULL;
}

BOOL DxAttBoneDataContainer::DeleteAttBoneData ( const char* szFile )
{
	ATTBONEDATAMAP_ITER iter = m_mapAttBoneData.find ( std::string(szFile) );
	if ( iter == m_mapAttBoneData.end() )	return FALSE;

	delete (*iter).second;
	m_mapAttBoneData.erase ( iter );

	return TRUE;
}

void DxAttBoneDataContainer::ReleaseAttBoneData ( const char* szFile )
{
	//	Note : 만약 전역 객채 파괴 순서상에서 후순위일때 에러 유발 가능성을 막음.
	if ( m_mapAttBoneData.empty() )		return;

	ATTBONEDATAMAP_ITER iter = m_mapAttBoneData.find ( std::string(szFile) );
	if ( iter==m_mapAttBoneData.end() )	return;

	iter->second->m_dwRef--;
	if ( iter->second->m_dwRef == 0 )
	{
		delete iter->second;
		m_mapAttBoneData.erase ( iter );
	}


	return;
}

DxAttBoneData* DxAttBoneDataContainer::LoadAttBoneData(const std::string& FileName, EMSCD_FLAGS emFlags )
{
	DxAttBoneData* pAttBoneData;

	pAttBoneData = FindAttBoneData(FileName);
	if (pAttBoneData)
	{
		pAttBoneData->m_dwRef++;
		return pAttBoneData;
	}

	pAttBoneData = new DxAttBoneData;
	BOOL bOk = pAttBoneData->LoadFile(FileName, m_pd3dDevice, emFlags, 0L );
	if (!bOk)
	{
		SAFE_DELETE(pAttBoneData);
		return NULL;
	}
	pAttBoneData->m_dwRef++;

	CDebugSet::ToListView("DxAttBoneData::LoadAttBoneData, %s", FileName.c_str());

	// 트리에 달기
	m_mapAttBoneData[std::string(pAttBoneData->GetFileName())] = pAttBoneData;
	DxAttBoneData* pTestAttBoneData = FindAttBoneData(FileName);
	return pTestAttBoneData ;
}

//
static void FindDontHaveTexture( const TCHAR* FileName, const TCHAR* pExt )
{
	TSTRING strName = FileName;
	std::transform( strName.begin(), strName.end(), strName.begin(), tolower );

	strName = sc::file::getFileExt( strName );
	if ( strName != pExt )
		return;

	DxAttBoneDataContainer::GetInstance().LoadAttBoneData( FileName, 0L );
}

// 로딩을 전부 해보도록 한다.
void DxAttBoneDataContainer::OnFindDontHaveTexture()
{
	sc::CFileFindTree	m_sFileTree;
	m_sFileTree.CreateTree( m_szPath, FALSE );
	m_sFileTree.SomeWorkSameExt( FindDontHaveTexture, "abf" );
}