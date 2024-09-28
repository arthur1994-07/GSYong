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

// CreateSkinAni_EDIT ���� �ٸ��� vecAniFile ���� ��ȭ�� �����.
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

			// bTool_NoRefresh = FALSE ��� ������ bin ������ �� �����ϵ��� �Ѵ�.
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
			// �ܺο��� cfg������ ������ ���� �ִ�.
			// �׷� ������ �ִ� �ִϸ��̼� �̸������� ���ֵ��� ����.
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

// [shhan][2014.12.08] chf �� �ִ� cfg �� ����ִ� bin ���ϵ��� Thread �� �ϵ� �ƴϵ� ���� �ε��� �ϵ��� �Ѵ�.
//						���� : �ڽ��� ĳ���Ͱ� �ִϸ��̼��� ��� �˸��� ������ ���ϴ� ���� �ذ�.
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

// [shhan][2014.12.08] �ӵ��� ������ ���� ĳ���� �ִ� 40���̴�. �׳� ������ �Ѵ�.
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
	// �ϴ� �������� �����ֵ����Ѵ�.
	{
		SAFE_DELETE( m_pJiggleBone );
		SAFE_DELETE( m_pJiggleBoneColl );
		SAFE_DELETE( m_pClothColl );
		m_pJiggleBone = new DxJiggleBone;
		m_pJiggleBoneColl = new DxJiggleBoneColl;
		m_pClothColl = new DxClothColl;
		m_smpFootColl = std::tr1::shared_ptr<DxFootColl>( new DxFootColl );
	}

	// ������ bsf ������ �ε� �غ���.
	// ������ bsf ���� ����ϵ��� �Ѵ�.
	// ������ ������� ������ bsf ������ ���� ���� �ִ�.
	{
		CString strSkeletonBSF;	// �ҹ��ڷ� ����
		STRUTIL::ChangeExt( m_strSkeleton.c_str(), strSkeletonBSF, ".bsf" );
		strSkeletonBSF.MakeLower();

		std::string PathName(DxSkeletonManager::GetInstance().GetPath());
		PathName.append(strSkeletonBSF.GetString());

		SPSerialFile spFileBSF;
		if ( Archive::CreateSerialFile(pArchive, spFileBSF) == true )
		{
			sc::SerialFile& SFileBSF = *spFileBSF;

			// ������ ���� ��츸 �ε��۾��� ���ֵ��� �Ѵ�.
			if ( SFileBSF.OpenFile ( FOT_READ, PathName.c_str() ) )
			{
				DWORD dwVer, dwBufferSize;
				SFileBSF >> dwVer;
				SFileBSF >> dwBufferSize;

				LOAD_BSF_Ver100( SFileBSF );

				// cache�� �� �ʿ䰡 ��� ������ ���� ���ش�.`
				SFileBSF.CloseFile();
			}
		}
	}

	return TRUE;
}

BOOL DxSkinAniControlData::SaveFile_BSF( boost::function<void (const TCHAR*)> pArchive )
{
	CString strSkeletonBSF;	// �ҹ��ڷ� ����
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
		// Note : �߰�.
		SFile << (BOOL)TRUE;
		m_smpFootColl->Save( SFile );

		// Note : �߰�.
		SFile << (BOOL)TRUE;
		m_pJiggleBone->Save( SFile );

		// Note : �߰�.
		SFile << (BOOL)TRUE;
		m_pJiggleBoneColl->Save( SFile );

		// Note : �߰�.
		SFile << (BOOL)TRUE;
		m_pClothColl->Save( SFile );

		//	Note : ����Ʈ�� �ٸ� �߰� ��ü�� �� ���� ����ؼ� ���� ����Ʈ�� ���� �Ǹ�
		//		TRUE ���� �����ϰ� �ȴ�.
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

	// Note : ����
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