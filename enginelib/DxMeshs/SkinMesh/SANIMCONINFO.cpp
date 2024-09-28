#include "pch.h"
#include <algorithm>

#include "../../../SigmaCore/Util/Util.h"

#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../Common/StlFunctions.h"
#include "../../Common/ByteStream.h"
#include "../../DxTools/DxMethods.h"
#include "../../G-Logic/GLogic.h"
#include "./DxSkinAnimationManager.h"
#include "./DxAniJiggleBlend.h"

#include "SANIMCONINFO.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


SAttachBoneAnimInfo& SAttachBoneAnimInfo::operator = ( const SAttachBoneAnimInfo &value )
{
	bUseStrikeAni = value.bUseStrikeAni;
	sBasicAni     = value.sBasicAni;


	vecStrikeAni.clear();
	vecStrikeAni.reserve( value.vecStrikeAni.size() );
	for( DWORD i = 0; i < value.vecStrikeAni.size(); i++ )
	{
		SAttachBoneAnimBasicInfo sTempBasicAni = value.vecStrikeAni[i];
		vecStrikeAni.push_back(sTempBasicAni);
	}
	return *this;
}

SANIMCONINFO::SANIMCONINFO()
	: m_dwFlag(ACF_LOOP|ACF_NEWINFO)
	, m_dwSTime(0)
	, m_dwETime(0)
	, m_dwETimeOrig(0)
	, m_UNITTIME(INT_MAX)
	, m_MainType(AN_GUARD_N)
	, m_SubType(AN_SUB_NONE)
	, m_wDivCount(0)
	, m_wStrikeCount(0)
	, m_vAnimationMoveOffset(0.f,0.f,0.f)

	, m_scpAniJiggleBlend(new DxAniJiggleBlend)
{
	::memset ( m_wDivFrame, 0, sizeof(WORD)*ACF_DIV );
    m_pAniScale = new DxAniScale;
}

SANIMCONINFO::~SANIMCONINFO()
{
	CleanUp();

    SAFE_DELETE( m_pAniScale );
}

void SANIMCONINFO::CleanUp()
{
	std::for_each( m_listEffAni.begin(), m_listEffAni.end(), std_afunc::DeleteObject() );
	m_listEffAni.clear();

    m_pAniScale->CleanUp();

    for( DWORD i = 0; i < PIECE_SIZE; i++ )
	{
		m_sAttBoneAnimInfo[i].Init();
	}
}

BOOL SANIMCONINFO::IsSameNameCFG( const TCHAR* szNameCFG )
{
	if( m_strCfgFile == szNameCFG )
		return TRUE;
	else
		return FALSE;
}
// 
// SANIMCONINFO& SANIMCONINFO::operator = ( SANIMCONINFO &value )
// {
// 	CleanUp();
// 
// 	m_strCfgFile	= value.m_strCfgFile;
// 	m_strXFile		= value.m_strXFile;
// 	m_strBinFile	= value.m_strBinFile;
// 	m_strSkeletion	= value.m_strSkeletion;
// 
// 	m_dwFlag = value.m_dwFlag;				//	기타 속성.
// 	m_dwSTime = value.m_dwSTime;			//	시작 시간.
// 	m_dwETime = value.m_dwETime;
// 	m_dwETimeOrig = value.m_dwETimeOrig;	//	끝 시간.
// 	m_UNITTIME = value.m_UNITTIME;			//	키단위 시간.
// 
// 	m_MainType = (EMANI_MAINTYPE)value.m_MainType;
// 	m_SubType = (EMANI_SUBTYPE)value.m_SubType;
// 
// 	m_wDivCount = value.m_wDivCount;		//	에니메이션 분할 키 개수.
// 	for ( DWORD i=0;i<m_wDivCount; ++i )
// 	{
// 		m_wDivFrame[i] = value.m_wDivFrame[i];
// 	}
// 
// 	m_wStrikeCount = value.m_wStrikeCount;
// 	for ( DWORD i=0;i<m_wStrikeCount; ++i )
// 	{
// 		m_sStrikeEff[i] = value.m_sStrikeEff[i];
// 	}
// 
// 	m_ChaSoundData = value.m_ChaSoundData;
// 
// 	EFFANILIST_ITER iter = value.m_listEffAni.begin();
// 	EFFANILIST_ITER iter_end = value.m_listEffAni.end();
// 	for ( ; iter!=iter_end; ++iter )
// 	{
// 		DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance ( (*iter)->GetTypeID() );
// 		if ( pEff )
// 		{
// 			pEff->SetProperty ( (*iter)->GetProperty() );
// 			pEff->SetEffAniData ( (*iter) );
// 			m_listEffAni.push_back ( pEff );
// 		}
// 	}
// 
// 	*m_pAniScale = *value.m_pAniScale;
// 
// 	return *this;
// }


SANIMCONINFO_MSG SANIMCONINFO::LoadFile( void* pBuf,UINT bufSize, LPDIRECT3DDEVICEQ pd3dDevice )
{
	se::ByteStream stream( (LPBYTE)pBuf,bufSize);
	stream.ReadFileType();
	return LoadFile(stream,pd3dDevice);
}


BOOL SANIMCONINFO::LoadFile( const TCHAR *szFileName, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive )
{
	TCHAR szFilePath[MAX_PATH] = _T("");
	::StringCchCopy( szFilePath, MAX_PATH, DxSkinAnimationManager::GetInstance().GetPath().c_str() );
	::StringCchCat( szFilePath, MAX_PATH, szFileName );

    /* 아카이브 기능 추가로 변경 된 코드, 향후 안정화되면 삭제 예정
	std::auto_ptr<sc::BaseStream> pBStream( 
		GLOGIC::openfile_basestream(
			GLOGIC::bENGLIB_ZIPFILE, 
			GLOGIC::strANIMATION_ZIPFILE.c_str(),
			szFilePath, 
			szFileName ) );
	if ( !pBStream.get() )
	{
		return FALSE;
	}

    SANIMCONINFO_MSG sMsg = LoadFile(*pBStream);
    */

    SPBaseStream spFile;

    if (Archive::CreateFileBaseStream(szFilePath, szFileName, GLOGIC::strANIMATION_ZIPFILE.c_str(), pArchive, spFile) == false)
    {
        return FALSE;
    }

    SANIMCONINFO_MSG sMsg = LoadFile(*spFile);

	switch ( sMsg )
	{
	case SANIMCONINFO_MSG_OK:
		return TRUE;

	case SANIMCONINFO_MSG_FALSE:
		return FALSE;

	case SANIMCONINFO_MSG_UNKNOWN_FILE_VERSION:
		MessageBox(NULL, "SANIMCONINFO::LoadFile() unknown file version. %s", szFileName, MB_OK);
		return FALSE;

	default:
		return FALSE;
	};
}


SANIMCONINFO_MSG SANIMCONINFO::LoadFile( sc::BaseStream& stream, LPDIRECT3DDEVICEQ pd3dDevice )
{
	TCHAR szFileType[FILETYPESIZE] = _T("");
	DWORD dwVersion;
	stream.GetFileType( szFileType, FILETYPESIZE, dwVersion );

	if( dwVersion<=0x0110 )
	{
		SANIMCONINFO_PREV sPrev;
		SANIMCONINFO_MSG sMsg = sPrev.LoadFile( stream, dwVersion, pd3dDevice );
        if ( sMsg != SANIMCONINFO_MSG_OK )
		{
			return sMsg;
		}

		m_strCfgFile	= sc::util::ChangeExtName( sPrev.m_Name, _T("cfg") );
		m_strXFile		= sc::util::ChangeExtName( sPrev.m_Name, _T("x") );
		m_strBinFile	= sc::util::ChangeExtName( sPrev.m_Name, _T("bin") );
		m_strSkeletion	= sPrev.m_SkeletionName;

		m_dwFlag		= sPrev.m_dwFlag;					//! 기타 속성.
		m_dwSTime		= sPrev.m_dwSTime;					//! 시작 시간.
		m_dwETime		= sPrev.m_dwETime;					//! 끝 시간.
		m_dwETimeOrig	= sPrev.m_dwETimeOrig;				//! 끝 시간.
		m_UNITTIME		= sPrev.m_UNITTIME;					//! 키단위 시간.

		m_MainType		= sPrev.m_MainType;
		m_SubType		= sPrev.m_SubType;

		m_wStrikeCount	= sPrev.m_wStrikeCount;
		for ( DWORD i=0; i<m_wStrikeCount; ++i )
		{
			m_sStrikeEff[i] = sPrev.m_sStrikeEff[i];
		}

		m_wDivCount		= sPrev.m_wDivCount;
		for ( DWORD i=0; i<m_wDivCount; ++i )
		{
			m_wDivFrame[i] = sPrev.m_wDivFrame[i];
		}
		
		m_listEffAni	= sPrev.m_listEffAni;
		sPrev.m_listEffAni.clear();

		SAFE_DELETE( m_pAniScale );
		m_pAniScale		= sPrev.m_pAniScale;
		sPrev.m_pAniScale = NULL;

		m_ChaSoundData	= sPrev.m_ChaSoundData;

		for ( DWORD i=0; i<PIECE_SAVE_SIZE; ++i )
		{
			m_sAttBoneAnimInfo[i] = sPrev.m_sAttBoneAnimInfo[i];
		}

		// [2012.11.23] 캐릭터 edit 어테치본 보완, 개선 < 그래픽팀 요청 >
		//				애니메이션 구분 적용대상 : 공격, Skill-A,B,C,D,E,F,G
		//				타격시점에서 -3frame 부터 attack0001 애니메이션이 발동 되도록 수정
		switch ( m_MainType )
		{
		case AN_ATTACK:
		case AN_SKILL:
		case AN_SKILL_B:
		case AN_SKILL_C:
		case AN_SKILL_D:
		case AN_SKILL_E:
		case AN_SKILL_F:
		case AN_SKILL_G:
		case AN_SKILL_H:
			{
				for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
				{
					m_sAttBoneAnimInfo[i].Init();

					SAttachBoneAnimBasicInfo sTempBasicAni;

					for( DWORD j = 0; j < m_wStrikeCount; j++ )
					{
						sTempBasicAni.nStartAnim = -3;
						sTempBasicAni.emAttAniType = AN_ATTBONE_ATTACK_001;
						m_sAttBoneAnimInfo[i].bUseStrikeAni = true;
						m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
					}
				}
			}
			break;
		};
	}
	else if( dwVersion==0x0111 )
	{
		stream >> m_strCfgFile;
		stream >> m_strXFile;
		stream >> m_strBinFile;
		stream >> m_strSkeletion;

		stream >> m_dwFlag;
		stream >> m_dwSTime;
		stream >> m_dwETime;
		stream >> m_dwETimeOrig;
		stream >> m_UNITTIME;

		DWORD	dwType;
		stream >> dwType;
		m_MainType = (EMANI_MAINTYPE)dwType;
		stream >> dwType;
		m_SubType = (EMANI_SUBTYPE)dwType;

		stream >> m_wDivCount;
		stream.ReadBuffer( m_wDivFrame, sizeof(WORD)*ACF_DIV );

		stream >> m_wStrikeCount;
		stream.ReadBuffer( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

		stream.ReadBuffer( &m_ChaSoundData, sizeof(SChaSoundData) );

		DWORD	dwSize;
		stream >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD	dwType;
			stream >> dwType;
			DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance( dwType );
			if ( pEff )
			{
				pEff->LoadFile( stream, pd3dDevice );
				m_listEffAni.push_back( pEff );
			}
		}

		m_pAniScale->Load( stream );

		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T('['), _T('_') );
		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T(']') );

		// [2012.11.23] 캐릭터 edit 어테치본 보완, 개선 < 그래픽팀 요청 >
		//				애니메이션 구분 적용대상 : 공격, Skill-A,B,C,D,E,F,G
		//				타격시점에서 -3frame 부터 attack0001 애니메이션이 발동 되도록 수정
		switch ( m_MainType )
		{
		case AN_ATTACK:
		case AN_SKILL:
		case AN_SKILL_B:
		case AN_SKILL_C:
		case AN_SKILL_D:
		case AN_SKILL_E:
		case AN_SKILL_F:
		case AN_SKILL_G:
		case AN_SKILL_H:
			{
				for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
				{
					m_sAttBoneAnimInfo[i].Init();

					SAttachBoneAnimBasicInfo sTempBasicAni;

					for( DWORD j = 0; j < m_wStrikeCount; j++ )
					{
						sTempBasicAni.nStartAnim = -3;
						sTempBasicAni.emAttAniType = AN_ATTBONE_ATTACK_001;
						m_sAttBoneAnimInfo[i].bUseStrikeAni = true;
						m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
					}
				}
			}
			break;

			// 이 부분은 0x0111 버전에서 Save, Load 가 잘 못 셋팅된 것이 있어서 추가되어야 한다.
		default:
			{
				for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
				{
					m_sAttBoneAnimInfo[i].Init();
					m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim = 0;
					m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType = AN_ATTBONE_STAY;
				}
			}
			break;
		};
	}
	else if( dwVersion==0x0112 )
	{
		stream >> m_strCfgFile;
		stream >> m_strXFile;
		stream >> m_strBinFile;
		stream >> m_strSkeletion;

		stream >> m_dwFlag;
		stream >> m_dwSTime;
		stream >> m_dwETime;
		stream >> m_dwETimeOrig;
		stream >> m_UNITTIME;

		DWORD	dwType;
		stream >> dwType;
		m_MainType = (EMANI_MAINTYPE)dwType;
		stream >> dwType;
		m_SubType = (EMANI_SUBTYPE)dwType;

		stream >> m_wDivCount;
		stream.ReadBuffer( m_wDivFrame, sizeof(WORD)*ACF_DIV );

		stream >> m_wStrikeCount;
		stream.ReadBuffer( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

		stream.ReadBuffer( &m_ChaSoundData, sizeof(SChaSoundData) );

		DWORD	dwSize;
		stream >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD	dwType;
			stream >> dwType;
			DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance( dwType );
			if ( pEff )
			{
				pEff->LoadFile( stream, pd3dDevice );
				m_listEffAni.push_back( pEff );
			}
		}

		m_pAniScale->Load( stream );

		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T('['), _T('_') );
		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T(']') );

		for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
		{
			m_sAttBoneAnimInfo[i].Init();

			if ( i < PIECE_SAVE_SIZE_OLD_SANIMCONINFO_0x0113 )
			{
				stream >> m_sAttBoneAnimInfo[i].bUseStrikeAni;

				if( !m_sAttBoneAnimInfo[i].bUseStrikeAni )
				{	
					stream >> m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim;
					stream >> dwType;
					m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
				}
				else
				{
					SAttachBoneAnimBasicInfo sTempBasicAni;

					for( DWORD j = 0; j < m_wStrikeCount; j++ )
					{
						stream >>  sTempBasicAni.nStartAnim;
						stream >> dwType;
						sTempBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
						m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
					}
				}
			}
		}
	}
	else if( dwVersion==0x0113 )
	{
		stream >> m_strCfgFile;
		stream >> m_strXFile;
		stream >> m_strBinFile;
		stream >> m_strSkeletion;

		stream >> m_dwFlag;
		stream >> m_dwSTime;
		stream >> m_dwETime;
		stream >> m_dwETimeOrig;
		stream >> m_UNITTIME;

		DWORD	dwType;
		stream >> dwType;
		m_MainType = (EMANI_MAINTYPE)dwType;
		stream >> dwType;
		m_SubType = (EMANI_SUBTYPE)dwType;

		stream >> m_wDivCount;
		stream.ReadBuffer( m_wDivFrame, sizeof(WORD)*ACF_DIV );

		stream >> m_wStrikeCount;
		stream.ReadBuffer( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

		stream.ReadBuffer( &m_ChaSoundData, sizeof(SChaSoundData) );

		DWORD	dwSize;
		stream >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD	dwType;
			stream >> dwType;
			DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance( dwType );
			if ( pEff )
			{
				pEff->LoadFile( stream, pd3dDevice );
				m_listEffAni.push_back( pEff );
			}
		}

		m_pAniScale->Load( stream );

		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T('['), _T('_') );
		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T(']') );

		for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
		{
			m_sAttBoneAnimInfo[i].Init();

			if ( i < PIECE_SAVE_SIZE_OLD_SANIMCONINFO_0x0113 )
			{
				stream >> m_sAttBoneAnimInfo[i].bUseStrikeAni;

				if( !m_sAttBoneAnimInfo[i].bUseStrikeAni )
				{	
					stream >> m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim;
					stream >> dwType;
					m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
				}
				else
				{
					SAttachBoneAnimBasicInfo sTempBasicAni;

					for( DWORD j = 0; j < m_wStrikeCount; j++ )
					{
						stream >>  sTempBasicAni.nStartAnim;
						stream >> dwType;
						sTempBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
						m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
					}
				}
			}
		}

		m_scpAniJiggleBlend->Load( stream );
	}
	else if( dwVersion==0x0114 )
	{
		stream >> m_strCfgFile;
		stream >> m_strXFile;
		stream >> m_strBinFile;
		stream >> m_strSkeletion;

		stream >> m_dwFlag;
		stream >> m_dwSTime;
		stream >> m_dwETime;
		stream >> m_dwETimeOrig;
		stream >> m_UNITTIME;

		DWORD	dwType;
		stream >> dwType;
		m_MainType = (EMANI_MAINTYPE)dwType;
		stream >> dwType;
		m_SubType = (EMANI_SUBTYPE)dwType;

		stream >> m_wDivCount;
		stream.ReadBuffer( m_wDivFrame, sizeof(WORD)*ACF_DIV );

		stream >> m_wStrikeCount;
		stream.ReadBuffer( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

		stream.ReadBuffer( &m_ChaSoundData, sizeof(SChaSoundData) );

		DWORD	dwSize;
		stream >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD	dwType;
			stream >> dwType;
			DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance( dwType );
			if ( pEff )
			{
				pEff->LoadFile( stream, pd3dDevice );
				m_listEffAni.push_back( pEff );
			}
		}

		m_pAniScale->Load( stream );

		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T('['), _T('_') );
		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T(']') );

		DWORD dwPieceSaveSize;
		stream >> dwPieceSaveSize;
		for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
		{
			m_sAttBoneAnimInfo[i].Init();

			if ( i < dwPieceSaveSize )
			{
				stream >> m_sAttBoneAnimInfo[i].bUseStrikeAni;

				if( !m_sAttBoneAnimInfo[i].bUseStrikeAni )
				{	
					stream >> m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim;
					stream >> dwType;
					m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
				}
				else
				{
					SAttachBoneAnimBasicInfo sTempBasicAni;

					for( DWORD j = 0; j < m_wStrikeCount; j++ )
					{
						stream >>  sTempBasicAni.nStartAnim;
						stream >> dwType;
						sTempBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
						m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
					}
				}
			}
		}

		m_scpAniJiggleBlend->Load( stream );
	}
	else if( dwVersion==0x0115 )
	{
		stream >> m_strCfgFile;
		stream >> m_strXFile;
		stream >> m_strBinFile;
		stream >> m_strSkeletion;

		stream >> m_dwFlag;
		stream >> m_dwSTime;
		stream >> m_dwETime;
		stream >> m_dwETimeOrig;
		stream >> m_UNITTIME;

		DWORD	dwType;
		stream >> dwType;
		m_MainType = (EMANI_MAINTYPE)dwType;
		stream >> dwType;
		m_SubType = (EMANI_SUBTYPE)dwType;

		stream >> m_wDivCount;
		stream.ReadBuffer( m_wDivFrame, sizeof(WORD)*ACF_DIV );

		stream >> m_wStrikeCount;
		stream.ReadBuffer( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

		stream.ReadBuffer( &m_ChaSoundData, sizeof(SChaSoundData) );

		DWORD	dwSize;
		stream >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD	dwType;
			stream >> dwType;
			DxEffAniData* pEff = DxEffAniMan::GetInstance().CreateEffInstance( dwType );
			if ( pEff )
			{
				pEff->LoadFile( stream, pd3dDevice );
				m_listEffAni.push_back( pEff );
			}
		}

		m_pAniScale->Load( stream );

		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T('['), _T('_') );
		m_strCfgFile = sc::util::GetChangeTCHAR( m_strCfgFile, _T(']') );

		DWORD dwPieceSaveSize;
		stream >> dwPieceSaveSize;
		for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
		{
			m_sAttBoneAnimInfo[i].Init();

			if ( i < dwPieceSaveSize )
			{
				stream >> m_sAttBoneAnimInfo[i].bUseStrikeAni;

				if( !m_sAttBoneAnimInfo[i].bUseStrikeAni )
				{	
					stream >> m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim;
					stream >> dwType;
					m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
				}
				else
				{
					SAttachBoneAnimBasicInfo sTempBasicAni;

					for( DWORD j = 0; j < m_wStrikeCount; j++ )
					{
						stream >>  sTempBasicAni.nStartAnim;
						stream >> dwType;
						sTempBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
						m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
					}
				}
			}
		}

		m_scpAniJiggleBlend->Load( stream );

		stream >> m_vAnimationMoveOffset;
	}
	else
	{
		//MIN_ASSERT(false && _T("SANIMCONINFO::LoadFile() unknown file version.") );
		return SANIMCONINFO_MSG_UNKNOWN_FILE_VERSION;
	}

	if ( m_strSkeletion.empty() )
	{
		AfxMessageBox( sc::string::format( "%1를 삭제해주시기 바랍니다.", m_strCfgFile ).c_str() );
		return SANIMCONINFO_MSG_FALSE;
	}

	m_dwFlag = m_dwFlag & ~ACF_NEWINFO;

    // [ Thread Work shhan ]

	// cache를 할 필요가 없어서 강제로 셋팅 해준다.
	stream.CloseFile();


	// [shhan][2014.11.17] Data 정보가 이상하게 들어가 있는게 있어서 보완작업함.
	if ( m_MainType < 0 || m_MainType >= AN_TYPE_SIZE || m_SubType < 0 || m_SubType >= AN_SUB_00_SIZE )
	{
		// [shhan][2014.10.22] Log 를 남긴다.
		TSTRING strName(m_strCfgFile);

		strName += _T(" - m_MainType, m_SubType 값이 정상이 아닙니다.");

		CDebugSet::ToLogFile( strName );

		GASSERT( 0 && _T("m_MainType, m_SubType 값이 정상이 아닙니다. log를 참조해주세요.") );

		m_MainType = AN_GUARD_N;
		m_SubType = AN_SUB_NONE;
	}

	return SANIMCONINFO_MSG_OK;
}

BOOL SANIMCONINFO::SaveFile( const TCHAR *szNameCFG, const TCHAR *szXFile )
{
	TCHAR szFilePath[MAX_PATH] = _T("");
	::StringCchCopy( szFilePath, MAX_PATH, DxSkinAnimationManager::GetInstance().GetPath().c_str() );
	::StringCchCat( szFilePath, MAX_PATH, szNameCFG );

	sc::SerialFile SFile;
	SFile.SetFileType( _T("SANIMCONINFO"), SANIMCONINFO::VERSION );
	if ( !SFile.OpenFile( FOT_WRITE, szFilePath ) )	return FALSE;

	m_strCfgFile	= szNameCFG;
	m_strBinFile	= sc::util::ChangeExtName( szXFile, _T("bin") );	//strBinFile;
	m_strXFile		= szXFile;								//sc::util::ChangeExtName( strBinFile, _T("x") );

	SFile << m_strCfgFile;
	SFile << m_strXFile;
	SFile << m_strBinFile;
	SFile << m_strSkeletion;

	SFile << m_dwFlag;
	SFile << m_dwSTime;
	SFile << m_dwETime;
	SFile << m_dwETimeOrig;
	SFile << m_UNITTIME;

	SFile << (DWORD)m_MainType;
	SFile << (DWORD)m_SubType;

	SFile << m_wDivCount;
	SFile.WriteBuffer( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	SFile << m_wStrikeCount;
	SFile.WriteBuffer( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SFile.WriteBuffer( &m_ChaSoundData, sizeof(SChaSoundData) );

	SFile << (DWORD) m_listEffAni.size();
	EFFANILIST_ITER iter = m_listEffAni.begin();
	EFFANILIST_ITER iter_end = m_listEffAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SFile << (DWORD) (*iter)->GetTypeID();
		(*iter)->SaveFile( SFile );
	}

	m_pAniScale->Save( SFile );

	SFile << static_cast<DWORD>(PIECE_SAVE_SIZE);

	DWORD dwType;
	for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
	{
		SFile << m_sAttBoneAnimInfo[i].bUseStrikeAni;
		if( !m_sAttBoneAnimInfo[i].bUseStrikeAni )
		{	
			SFile << m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim;
			dwType = (DWORD)m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType;
			SFile << dwType;
		}
		else
		{
			for( DWORD j = 0; j < m_wStrikeCount; j++ )
			{
				SFile << m_sAttBoneAnimInfo[i].vecStrikeAni[j].nStartAnim;
				dwType = (DWORD)m_sAttBoneAnimInfo[i].vecStrikeAni[j].emAttAniType;
				SFile << dwType;
			}
		}
	}

	m_scpAniJiggleBlend->Save( SFile );

	SFile << m_vAnimationMoveOffset;

	return TRUE;
}

void SANIMCONINFO::InitEffAniList( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFANILIST_ITER iter = m_listEffAni.begin();
	EFFANILIST_ITER iter_end = m_listEffAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InitDeviceObjects ( pd3dDevice );
	}
}

// 0 KeyFrame <-> 1 JiggleBone
void SANIMCONINFO::GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const
{
	m_scpAniJiggleBlend->GetKeyJiggleWeight_PureThread( afKeyJiggleWeight, dwKeyJiggleWeightNUM, dwKey );
}

BOOL SANIMCONINFO::IsAnimationMove()
{ 
	return (m_dwFlag & ACF_MOVE) ? TRUE : FALSE; 
}





typedef boost::unordered_map< TSTRING, SANIMCONINFO* > AnimConfigMap;
typedef AnimConfigMap::iterator AnimConfigMapIter;

static AnimConfigMap s_mapAnimConfig;

const SANIMCONINFO* CAnimConfigCache::Load( const TSTRING& strFileName )
{
    if( strFileName.empty() )
        return NULL;

    AnimConfigMapIter it = s_mapAnimConfig.find( strFileName );
    if( it != s_mapAnimConfig.end() )
        return it->second;

    SANIMCONINFO* pAnimConfig = new SANIMCONINFO;
    if( !pAnimConfig->LoadFile( strFileName.c_str() ) )
    {
        SAFE_DELETE( pAnimConfig );
        return NULL;
    }

    s_mapAnimConfig.insert( std::make_pair( strFileName, pAnimConfig ) );

    return pAnimConfig;
}

VOID CAnimConfigCache::Release()
{
    std::for_each( s_mapAnimConfig.begin(), s_mapAnimConfig.end(), std_afunc::DeleteMapObject() );
    s_mapAnimConfig.clear();
}