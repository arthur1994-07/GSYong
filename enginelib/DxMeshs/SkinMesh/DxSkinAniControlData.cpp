#include "pch.h"

#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../Archive/ArchiveType.h"
#include "../../Archive/Archive.h"
#include "./DxSkinAnimation.h"
#include "./DxSkeletonManager.h"
#include "./DxJiggleBoneColl.h"
#include "./DxJiggleBone.h"
#include "./DxClothColl.h"
#include "./DxFootColl.h"

#include "DxSkinAniControlData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxSkinAniControlData::DxSkinAniControlData()
{
	m_pJiggleBone = new DxJiggleBone;
	m_pJiggleBoneColl = new DxJiggleBoneColl;
	m_pClothColl = new DxClothColl;
	m_smpFootColl = std::tr1::shared_ptr<DxFootColl>( new DxFootColl );
}

DxSkinAniControlData::~DxSkinAniControlData()
{
	SAFE_DELETE( m_pJiggleBone );
	SAFE_DELETE( m_pJiggleBoneColl );
	SAFE_DELETE( m_pClothColl );

	CleanUp();
}

void DxSkinAniControlData::CleanUp()
{
	m_skinAniResVec.clear();

	m_smpFootColl->CleanUp();

	m_deqBoneEditScale.clear();
}

// CreateSkinAni_EDIT 과는 다르게 vecAniFile 값이 변화가 생긴다.
void DxSkinAniControlData::CreateSkinAni( IN OUT VEC_TSTRING& vecAniFile, BOOL bTool_NoRefresh )
{
	VEC_TSTRING_ITER iter = vecAniFile.begin();
	for( ; iter!=vecAniFile.end(); )
	{
		DxSkinAnimationRes skinAniRes = DxSkinAnimationManager::GetInstance().LoadAnimationCFG( (*iter) );
		if( skinAniRes.IsValid() )
		{
			m_skinAniResVec.push_back( skinAniRes );
			++iter;

			// bTool_NoRefresh = FALSE 라면 무조건 bin 파일은 재 생성하도록 한다.
			if( !bTool_NoRefresh )
			{
				HRESULT hr = skinAniRes.get()->CreateAnimationDataFromTool( !bTool_NoRefresh );
				if ( FAILED(hr) )
				{
					std::string ErrorMsg( sc::string::format( "Animation X-File Load failed : %1%",	skinAniRes.get()->m_strCfgFile ));
					CDebugSet::ToLogFile( ErrorMsg );
				}
			}
		}
		else
		{
			// 외부에서 cfg파일을 지웠을 수도 있다.
			// 그럼 가지고 있는 애니메이션 이름정보를 없애도록 하자.
			iter = vecAniFile.erase( iter );
		}

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//
	}
}

void DxSkinAniControlData::CreateSkinAni_EDIT( const VEC_TSTRING& vecAniFile )
{
	for (DWORD i=0; i<(DWORD)vecAniFile.size(); ++i )
	{
		DxSkinAnimationRes skinAniRes = DxSkinAnimationManager::GetInstance().LoadAnimationCFG( vecAniFile[i] );
		if( skinAniRes.IsValid() )
		{
			m_skinAniResVec.push_back( skinAniRes );
		}

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//
	}

	//for ( size_t n=0; n<m_skinAniResVec.size(); ++n )
	//{
	//	int MType = m_skinAniResVec[n].get()->m_MainType;
	//	int SType = m_skinAniResVec[n].get()->m_SubType;
	//	m_vecAniType[MType][SType].push_back( m_skinAniResVec[n] );
	//}
}

// [shhan][2014.12.08] chf 에 있는 cfg 에 들어있는 bin 파일들을 Thread 로 하든 아니든 파일 로딩을 하도록 한다.
//						목적 : 자신의 캐릭터가 애니메이션이 없어서 알맞은 동작을 못하는 문제 해결.
void DxSkinAniControlData::LoadBIN_HaveToAnimationForMyChar( BOOL bAniThread )
{
	for( DWORD i=0; i < m_skinAniResVec.size(); ++ i )
	{
		switch ( m_skinAniResVec[i].get()->m_MainType )
		{
		case AN_GUARD_N:
		case AN_PLACID:
		case AN_WALK:
		case AN_RUN:
		case AN_ATTACK:
		case AN_SHOCK:
		case AN_DIE:
			DxAnimationListManager::GetInstance().LoadAnimList_BIN( m_skinAniResVec[i].get()->m_strBinFile, m_skeletonRes.GetFileName(), bAniThread ? true : false );
			break;

		default:
			break;
		};
	}
}

// [shhan][2014.12.08] 속도는 느려도 나의 캐릭터 최대 40번이다. 그냥 쓰도록 한다.
void DxSkinAniControlData::LoadBIN_HaveToAnimationForMyChar( EMANI_MAINTYPE emANIMTYPE, EMANI_SUBTYPE emANISTYPE, BOOL bAniThread )
{
	for( DWORD i=0; i < m_skinAniResVec.size(); ++ i )
	{
		if ( m_skinAniResVec[i].get()->m_MainType == emANIMTYPE && m_skinAniResVec[i].get()->m_SubType == emANISTYPE )
		{
			DxAnimationListManager::GetInstance().LoadAnimList_BIN( m_skinAniResVec[i].get()->m_strBinFile, m_skeletonRes.GetFileName(), bAniThread ? true : false );
			return;
		}
	}
}

DxSkinAnimation* DxSkinAniControlData::FindAnimation( const TCHAR* szNameCFG )
{
	for( DWORD i=0; i < m_skinAniResVec.size(); ++ i )
	{
		if( m_skinAniResVec[i].get()->IsSameNameCFG( szNameCFG ) )
		{
			return m_skinAniResVec[i].get();
		}
	}
	return NULL;
}

BOOL DxSkinAniControlData::LoadSkeleton ( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice )
{
	MIN_ASSERT(szSkeleton&&"DxSkinAniControlData::LoadSkeleton()");

	m_strSkeleton = szSkeleton;
	m_skeletonRes = DxSkeletonManager::GetInstance().LoadSkeleton( m_strSkeleton, TRUE );
	
	return TRUE;
}

// BoneScale
const SBONESCALE* DxSkinAniControlData::GetBoneScale( const int nIndex ) const
{
	if( static_cast<int>(m_deqBoneEditScale.size()) < nIndex ) return NULL;
	return &m_deqBoneEditScale[nIndex];
}

void DxSkinAniControlData::AddBoneScale( const SBONESCALE& boneScale )
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

bool DxSkinAniControlData::EraseBoneScale( const int nIndex )
{
	if( static_cast<int>(m_deqBoneEditScale.size()) < nIndex )
		return FALSE;

	m_deqBoneEditScale.erase( m_deqBoneEditScale.begin() + nIndex );

	return TRUE;
}

void DxSkinAniControlData::UpdateBoneScaleList()
{
	if( !m_skeletonRes.IsValid() )	
		return;

	DEQ_BONESCALE_ITER iter = m_deqBoneEditScale.begin();
	for( ; iter != m_deqBoneEditScale.end(); ++iter )
	{
		DxBoneTrans *pBoneCur = m_skeletonRes.get()->FindBone_t( _T(iter->strBoneName.c_str()) );
		if( pBoneCur )
		{
			// [ Thread Work shhan ]
			pBoneCur->vBoneScale = iter->vScale;
			//pBoneCur->m_vEditScale = iter->vScale;
		}
	}
}

BOOL DxSkinAniControlData::LoadFile_BSF( boost::function<void (const TCHAR*)> pArchive )
{
	// 일단 기존것을 지워주도록한다.
	{
		SAFE_DELETE( m_pJiggleBone );
		SAFE_DELETE( m_pJiggleBoneColl );
		SAFE_DELETE( m_pClothColl );
		m_pJiggleBone = new DxJiggleBone;
		m_pJiggleBoneColl = new DxJiggleBoneColl;
		m_pClothColl = new DxClothColl;
		m_smpFootColl = std::tr1::shared_ptr<DxFootColl>( new DxFootColl );
	}

	// 무조건 bsf 파일을 로딩 해본다.
	// 있으면 bsf 꺼를 사용하도록 한다.
	// 이전에 만들어진 파일은 bsf 파일이 없을 수도 있다.
	{
		CString strSkeletonBSF;	// 소문자로 변경
		STRUTIL::ChangeExt( m_strSkeleton.c_str(), strSkeletonBSF, ".bsf" );
		strSkeletonBSF.MakeLower();

		std::string PathName(DxSkeletonManager::GetInstance().GetPath());
		PathName.append(strSkeletonBSF.GetString());

		SPSerialFile spFileBSF;
		if ( Archive::CreateSerialFile(pArchive, spFileBSF) == true )
		{
			sc::SerialFile& SFileBSF = *spFileBSF;

			// 파일이 잇을 경우만 로딩작업을 해주도록 한다.
			if ( SFileBSF.OpenFile ( FOT_READ, PathName.c_str() ) )
			{
				DWORD dwVer, dwBufferSize;
				SFileBSF >> dwVer;
				SFileBSF >> dwBufferSize;

				LOAD_BSF_Ver100( SFileBSF );

				// cache를 할 필요가 없어서 강제로 셋팅 해준다.`
				SFileBSF.CloseFile();
			}
		}
	}

	return TRUE;
}

BOOL DxSkinAniControlData::SaveFile_BSF( boost::function<void (const TCHAR*)> pArchive )
{
	CString strSkeletonBSF;	// 소문자로 변경
	STRUTIL::ChangeExt( m_strSkeleton.c_str(), strSkeletonBSF, ".bsf" );

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxSkeletonManager::GetInstance().GetPath().c_str() );
	StringCchCat( szPathName, MAX_PATH, strSkeletonBSF.GetString() );

	SPSerialFile spFile;

	if (Archive::CreateSerialFile(pArchive, spFile) == false)
	{
		return FALSE;
	}

	sc::SerialFile& SFile = *spFile;

	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		
		return FALSE;

	SFile << VERSION;
	SFile.BeginBlock();
	{
		// Note : 추가.
		SFile << (BOOL)TRUE;
		m_smpFootColl->Save( SFile );

		// Note : 추가.
		SFile << (BOOL)TRUE;
		m_pJiggleBone->Save( SFile );

		// Note : 추가.
		SFile << (BOOL)TRUE;
		m_pJiggleBoneColl->Save( SFile );

		// Note : 추가.
		SFile << (BOOL)TRUE;
		m_pClothColl->Save( SFile );

		//	Note : 이펙트등 다른 추가 계체가 들어갈 것을 대비해서 설정 이펙트가 들어가게 되면
		//		TRUE 값을 저장하게 된다.
		//
		SFile << (BOOL)FALSE;
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL DxSkinAniControlData::LOAD_BSF_Ver100( sc::BaseStream &SFile )
{
	m_strSkeleton;

	BOOL bExist;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_smpFootColl->Load( SFile );
	}
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_pJiggleBone->Load( SFile );
	}
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_pJiggleBoneColl->Load( SFile, m_skeletonRes.get() );
	}
	else			return TRUE;

	bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_pClothColl->Load( SFile, m_skeletonRes.get() );
	}
	else			return TRUE;

	// Note : 덤프
	SFile >> bExist;

	return TRUE;
}

void DxSkinAniControlData::ErrorLoadJiggleBone( sc::BaseStream &SFile )
{
	m_pJiggleBone->Load( SFile );
	m_pJiggleBone->CleanUp();
}

void DxSkinAniControlData::LoadPrevFootColl( sc::BaseStream &SFile )
{
	m_smpFootColl->Load( SFile );
}