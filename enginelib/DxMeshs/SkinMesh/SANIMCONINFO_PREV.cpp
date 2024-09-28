#include "pch.h"
#include <algorithm>
#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../DxTools/DxMethods.h"
#include "../../G-Logic/GLogic.h"
#include "../../Common/StlFunctions.h"
#include "./DxSkinAnimationManager.h"
//#include "./DxAniBoneScale.h"
#include "SANIMCONINFO.h"

//#include "../CoreCommon/String/ByteStream.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SANIMCONINFO_PREV::SANIMCONINFO_PREV() :
	m_dwFlag(ACF_LOOP|ACF_NEWINFO),
	m_dwSTime(0),
	m_dwETime(0),
	m_dwETimeOrig(0),
	m_UNITTIME(0),
	m_MainType(AN_GUARD_N),
	m_SubType(AN_SUB_NONE),

	m_wDivCount(0),
	m_wStrikeCount(0),
	m_pAniScale(NULL)
{
	//memset( m_Name, 0, sizeof(TCHAR)*ACF_SZNAME );
	//memset( m_SkeletionName, 0, sizeof(TCHAR)*ACF_SZNAME );
	memset ( m_wDivFrame, 0, sizeof(WORD)*ACF_DIV );
	m_pAniScale = new DxAniScale;
}

SANIMCONINFO_PREV::~SANIMCONINFO_PREV ()
{
	CleanUp();

	SAFE_DELETE( m_pAniScale );
}

void SANIMCONINFO_PREV::CleanUp()
{
	std::for_each ( m_listEffAni.begin(), m_listEffAni.end(), std_afunc::DeleteObject() );
	m_listEffAni.clear();

	if ( m_pAniScale )
	{
		m_pAniScale->CleanUp();
	}

	for( DWORD i = 0; i < PIECE_SIZE; i++ )
	{
		m_sAttBoneAnimInfo[i].Init();
	}
}

BOOL SANIMCONINFO_PREV::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SANIMCONINFO_101 OldAnimInfo;
	SFile.ReadBuffer ( &OldAnimInfo, sizeof(OldAnimInfo) );
	
    char szName[ACF_SZNAME] = {0};
	StringCchCopy(szName, ACF_SZNAME, OldAnimInfo.m_szName);
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	StringCchCopy(szSkeletonName, ACF_SZNAME, OldAnimInfo.m_szSkeletion );
    m_SkeletionName = szSkeletonName;

    m_dwFlag = OldAnimInfo.m_dwFlag;
	m_dwSTime = OldAnimInfo.m_dwSTime;
	m_dwETime = OldAnimInfo.m_dwETime;
	m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
	m_UNITTIME = OldAnimInfo.m_UNITTIME;

	m_MainType = OldAnimInfo.m_MainType;
	m_SubType = OldAnimInfo.m_SubType;

	m_wDivCount = OldAnimInfo.m_wDivCount;
	memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );		

	return TRUE;
}

BOOL SANIMCONINFO_PREV::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SANIMCONINFO_102 OldAnimInfo;
	SFile.ReadBuffer ( &OldAnimInfo, sizeof(OldAnimInfo) );
	
    char szName[ACF_SZNAME] = {0};
	StringCchCopy( szName, ACF_SZNAME, OldAnimInfo.m_szName );
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	StringCchCopy(szSkeletonName, ACF_SZNAME, OldAnimInfo.m_szSkeletion);
    m_SkeletionName = szSkeletonName;

    m_dwFlag = OldAnimInfo.m_dwFlag;
	m_dwSTime = OldAnimInfo.m_dwSTime;
	m_dwETime = OldAnimInfo.m_dwETime;
	m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
	m_UNITTIME = OldAnimInfo.m_UNITTIME;

	m_MainType = OldAnimInfo.m_MainType;
	m_SubType = OldAnimInfo.m_SubType;

	m_wDivCount = OldAnimInfo.m_wDivCount;
	memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );

	m_ChaSoundData = OldAnimInfo.m_ChaSoundData;

	return TRUE;
}

BOOL SANIMCONINFO_PREV::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SANIMCONINFO_103 OldAnimInfo;
	SFile.ReadBuffer ( &OldAnimInfo, sizeof(OldAnimInfo) );

    char szName[ACF_SZNAME] = {0};
	StringCchCopy(szName, ACF_SZNAME, OldAnimInfo.m_szName );
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	StringCchCopy(szSkeletonName, ACF_SZNAME, OldAnimInfo.m_szSkeletion );
    m_SkeletionName = szSkeletonName;

	m_dwFlag = OldAnimInfo.m_dwFlag;
	m_dwSTime = OldAnimInfo.m_dwSTime;
	m_dwETime = OldAnimInfo.m_dwETime;
	m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
	m_UNITTIME = OldAnimInfo.m_UNITTIME;

	m_MainType = OldAnimInfo.m_MainType;
	m_SubType = OldAnimInfo.m_SubType;

	m_wDivCount = OldAnimInfo.m_wDivCount;
	memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );

	m_wStrikeCount = 0;

	m_ChaSoundData = OldAnimInfo.m_ChaSoundData;

	return TRUE;
}

BOOL SANIMCONINFO_PREV::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SANIMCONINFO_104 OldAnimInfo;
	SFile.ReadBuffer ( &OldAnimInfo, sizeof(SANIMCONINFO_104) );

    char szName[ACF_SZNAME] = {0};
	StringCchCopy( szName, ACF_SZNAME, OldAnimInfo.m_szName );
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	StringCchCopy(szSkeletonName, ACF_SZNAME, OldAnimInfo.m_szSkeletion);
    m_SkeletionName = szSkeletonName;

	m_dwFlag = OldAnimInfo.m_dwFlag;
	m_dwSTime = OldAnimInfo.m_dwSTime;
	m_dwETime = OldAnimInfo.m_dwETime;
	m_dwETimeOrig = OldAnimInfo.m_dwETimeOrig;
	m_UNITTIME = OldAnimInfo.m_UNITTIME;

	m_MainType = OldAnimInfo.m_MainType;
	m_SubType = OldAnimInfo.m_SubType;

	m_wDivCount = OldAnimInfo.m_wDivCount;
	memcpy ( m_wDivFrame, OldAnimInfo.m_wDivFrame, sizeof(WORD)*ACF_DIV );

	m_wStrikeCount = OldAnimInfo.m_wStrikeCount;
	memcpy ( m_sStrikeEff, OldAnimInfo.m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	m_ChaSoundData = OldAnimInfo.m_ChaSoundData;

	return TRUE;
}

BOOL SANIMCONINFO_PREV::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SFile.ReadBuffer ( this, sizeof(SANIMCONINFO) );
    char szName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szName, sizeof(char)*ACF_SZNAME);
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szSkeletonName, sizeof(char)*ACF_SZNAME);
    m_SkeletionName = szSkeletonName;

	SFile >> m_dwFlag;
	SFile >> m_dwSTime;
	SFile >> m_dwETime;
	SFile >> m_dwETimeOrig;
	SFile >> m_UNITTIME;

	DWORD	dwType;
	SFile >> dwType;
	m_MainType = (EMANI_MAINTYPE)dwType;
	SFile >> dwType;
	m_SubType = (EMANI_SUBTYPE)dwType;

	SFile >> m_wDivCount;
	SFile.ReadBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	SFile >> m_wStrikeCount;
	SFile.ReadBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SChaSoundData_103 ChaSoundData;
	SFile.ReadBuffer ( &ChaSoundData, sizeof(SChaSoundData_103) );
	m_ChaSoundData = ChaSoundData;		

	DWORD	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD	dwType;
		SFile >> dwType;
		DxEffAniData*	pEff = DxEffAniMan::GetInstance().CreateEffInstance ( dwType );
		if ( pEff )
		{
			pEff->LoadFile ( SFile, pd3dDevice );
			m_listEffAni.push_back ( pEff );
		}
	}

	return TRUE;
}

BOOL SANIMCONINFO_PREV::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
    char szName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szName, sizeof(char)*ACF_SZNAME);
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szSkeletonName, sizeof(char)*ACF_SZNAME);
    m_SkeletionName = szSkeletonName;

	SFile >> m_dwFlag;
	SFile >> m_dwSTime;
	SFile >> m_dwETime;
	SFile >> m_dwETimeOrig;
	SFile >> m_UNITTIME;

	DWORD	dwType;
	SFile >> dwType;
	m_MainType = (EMANI_MAINTYPE)dwType;
	SFile >> dwType;
	m_SubType = (EMANI_SUBTYPE)dwType;

	SFile >> m_wDivCount;
	SFile.ReadBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	SFile >> m_wStrikeCount;
	SFile.ReadBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SFile.ReadBuffer ( &m_ChaSoundData, sizeof(SChaSoundData) );

	DWORD	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD	dwType;
		SFile >> dwType;
		DxEffAniData*	pEff = DxEffAniMan::GetInstance().CreateEffInstance ( dwType );
		if ( pEff )
		{
			pEff->LoadFile ( SFile, pd3dDevice );
			m_listEffAni.push_back ( pEff );
		}
	}

	return TRUE;
}

BOOL SANIMCONINFO_PREV::LoadFile_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SFile.ReadBuffer ( this, sizeof(SANIMCONINFO) );
    char szName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szName, sizeof(char)*ACF_SZNAME);
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szSkeletonName, sizeof(char)*ACF_SZNAME);
    m_SkeletionName = szSkeletonName;

	SFile >> m_dwFlag;
	SFile >> m_dwSTime;
	SFile >> m_dwETime;
	SFile >> m_dwETimeOrig;
	SFile >> m_UNITTIME;

	DWORD	dwType;
	SFile >> dwType;
	m_MainType = (EMANI_MAINTYPE)dwType;
	SFile >> dwType;
	m_SubType = (EMANI_SUBTYPE)dwType;

	SFile >> m_wDivCount;
	SFile.ReadBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	SFile >> m_wStrikeCount;
	SFile.ReadBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SFile.ReadBuffer ( &m_ChaSoundData, sizeof(SChaSoundData) );

	DWORD	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD	dwType;
		SFile >> dwType;
		DxEffAniData*	pEff = DxEffAniMan::GetInstance().CreateEffInstance ( dwType );
		if ( pEff )
		{
			pEff->LoadFile ( SFile, pd3dDevice );
			m_listEffAni.push_back ( pEff );
		}
	}

	m_pAniScale->Load( SFile );

	return TRUE;
}

BOOL SANIMCONINFO_PREV::LoadFile_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SFile.ReadBuffer ( this, sizeof(SANIMCONINFO) );
    char szName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szName, sizeof(char)*ACF_SZNAME);
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szSkeletonName, sizeof(char)*ACF_SZNAME);
    m_SkeletionName = szSkeletonName;

	SFile >> m_dwFlag;
	SFile >> m_UNITTIME;
	SFile >> m_dwSTime;
	SFile >> m_dwETime;
	SFile >> m_dwETimeOrig;


	DWORD	dwType;
	SFile >> dwType;
	m_MainType = (EMANI_MAINTYPE)dwType;
	SFile >> dwType;
	m_SubType = (EMANI_SUBTYPE)dwType;

	SFile >> m_wStrikeCount;
	SFile.ReadBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SFile >> m_wDivCount;
	SFile.ReadBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	DWORD	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD	dwType;
		SFile >> dwType;
		DxEffAniData*	pEff = DxEffAniMan::GetInstance().CreateEffInstance ( dwType );
		if ( pEff )
		{
			pEff->LoadFile ( SFile, pd3dDevice );
			m_listEffAni.push_back ( pEff );
		}
	}

	m_pAniScale->Load( SFile );

	SFile.ReadBuffer ( &m_ChaSoundData, sizeof(SChaSoundData) );

	return TRUE;
}


BOOL SANIMCONINFO_PREV::LoadFile_0109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SFile.ReadBuffer ( this, sizeof(SANIMCONINFO) );
    char szName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szName, sizeof(char)*ACF_SZNAME);
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szSkeletonName, sizeof(char)*ACF_SZNAME);
    m_SkeletionName = szSkeletonName;

	SFile >> m_dwFlag;
	SFile >> m_UNITTIME;
	SFile >> m_dwSTime;
	SFile >> m_dwETime;
	SFile >> m_dwETimeOrig;


	DWORD	dwType;
	SFile >> dwType;
	m_MainType = (EMANI_MAINTYPE)dwType;
	SFile >> dwType;
	m_SubType = (EMANI_SUBTYPE)dwType;

	SFile >> m_wStrikeCount;
	SFile.ReadBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SFile >> m_wDivCount;
	SFile.ReadBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	DWORD	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD	dwType;
		SFile >> dwType;
		DxEffAniData*	pEff = DxEffAniMan::GetInstance().CreateEffInstance ( dwType );
		if ( pEff )
		{
			pEff->LoadFile ( SFile, pd3dDevice );
			m_listEffAni.push_back ( pEff );
		}
	}

	m_pAniScale->Load( SFile );

	SFile.ReadBuffer ( &m_ChaSoundData, sizeof(SChaSoundData) );

	for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
	{
		m_sAttBoneAnimInfo[i].Init();

		if ( i < PIECE_SAVE_SIZE_OLD_SANIMCONINFO_0x0113 )
		{
			SFile >> m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim;
			SFile >> dwSize;
			m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType = (EMANI_MAINTYPE)dwSize;
		}
	}

	return TRUE;
}




BOOL SANIMCONINFO_PREV::LoadFile_0110 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SFile.ReadBuffer ( this, sizeof(SANIMCONINFO) );
    char szName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szName, sizeof(char)*ACF_SZNAME);
    m_Name = szName;

    char szSkeletonName[ACF_SZNAME] = {0};
	SFile.ReadBuffer(szSkeletonName, sizeof(char)*ACF_SZNAME);
    m_SkeletionName = szSkeletonName;

	SFile >> m_dwFlag;
	SFile >> m_UNITTIME;
	SFile >> m_dwSTime;
	SFile >> m_dwETime;
	SFile >> m_dwETimeOrig;


	DWORD	dwType;
	SFile >> dwType;
	m_MainType = (EMANI_MAINTYPE)dwType;
	SFile >> dwType;
	m_SubType = (EMANI_SUBTYPE)dwType;

	SFile >> m_wStrikeCount;
	SFile.ReadBuffer ( m_sStrikeEff, sizeof(SANIMSTRIKE)*ACF_STRIKE );

	SFile >> m_wDivCount;
	SFile.ReadBuffer ( m_wDivFrame, sizeof(WORD)*ACF_DIV );

	DWORD	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD	dwType;
		SFile >> dwType;
		DxEffAniData*	pEff = DxEffAniMan::GetInstance().CreateEffInstance ( dwType );
		if ( pEff )
		{
			pEff->LoadFile ( SFile, pd3dDevice );
			m_listEffAni.push_back ( pEff );
		}
	}

	m_pAniScale->Load( SFile );

	SFile.ReadBuffer ( &m_ChaSoundData, sizeof(SChaSoundData) );

	for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
	{
		m_sAttBoneAnimInfo[i].Init();

		if ( i < PIECE_SAVE_SIZE_OLD_SANIMCONINFO_0x0113 )
		{
			SFile >> m_sAttBoneAnimInfo[i].bUseStrikeAni;

			if( !m_sAttBoneAnimInfo[i].bUseStrikeAni )
			{	
				SFile >> m_sAttBoneAnimInfo[i].sBasicAni.nStartAnim;
				SFile >> dwType;
				m_sAttBoneAnimInfo[i].sBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
			}else{
				SAttachBoneAnimBasicInfo sTempBasicAni;

				for( DWORD j = 0; j < m_wStrikeCount; j++ )
				{
					SFile >>  sTempBasicAni.nStartAnim;
					SFile >> dwType;
					sTempBasicAni.emAttAniType = (EMANI_MAINTYPE)dwType;
					m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
				}
			}
		}
	}

	return TRUE;
}

SANIMCONINFO_MSG SANIMCONINFO_PREV::LoadFile( sc::BaseStream& stream, DWORD dwVersion, LPDIRECT3DDEVICEQ pd3dDevice )
{
	CleanUp();

 //   std::string FilePath(DxSkinAnimationManager::GetInstance().GetPath());
	//FilePath.append(FileName);

	//std::auto_ptr<sc::BaseStream> pBStream(
 //       GLOGIC::openfile_basestream(
 //           GLOGIC::UseEngineZipFile(), 
 //           GLOGIC::strANIMATION_ZIPFILE,
 //           FilePath.c_str(), 
 //           FileName));

	//if (!pBStream.get())
	//	return FALSE;

	//sc::BaseStream& SFile = *pBStream;

	//char szFileType[FILETYPESIZE] = {0};
	//DWORD dwVersion;
	//stream.GetFileType( szFileType, FILETYPESIZE, dwVersion );

	switch (dwVersion)
	{
	case 0x0101:	LoadFile_0101( stream, pd3dDevice );	break;
	case 0x0102:	LoadFile_0102( stream, pd3dDevice );	break;
	case 0x0103:	LoadFile_0103( stream, pd3dDevice );	break;
	case 0x0104:	LoadFile_0104( stream, pd3dDevice );	break;
	case 0x0105:	LoadFile_0105( stream, pd3dDevice );	break;
	case 0x0106:	LoadFile_0106( stream, pd3dDevice );	break;
	case 0x0107:	LoadFile_0107( stream, pd3dDevice );	break;
	case 0x0108:	LoadFile_0108( stream, pd3dDevice );	break;
	case 0x0109:	LoadFile_0109( stream, pd3dDevice );	break;
	case 0x0110:	LoadFile_0110( stream, pd3dDevice );	break;		
	default:
		{
			//MessageBox(NULL, "SANIMCONINFO::LoadFile() unknown file version.", FileName.c_str(), MB_OK);
			return SANIMCONINFO_MSG_UNKNOWN_FILE_VERSION;
		}
		break;
	};

	m_dwFlag = m_dwFlag & ~ACF_NEWINFO;

	return SANIMCONINFO_MSG_OK;
}