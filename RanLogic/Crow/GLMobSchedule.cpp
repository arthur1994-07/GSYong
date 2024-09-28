#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../EngineLib/DxTools/Collision.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "./GLCrowData.h"
#include "./GLCrowDataMan.h"
//#include "./GLLandMan.h"
#include "./GLMobSchedule.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace COMMENT
{
	std::string MOBACTIONS[EMACTION_SIZE] =
	{
		"EMACTION_MOVE",
		"EMACTION_ACTSTAY",
	};
};

const DWORD	SMOBACTION::VERSION = 0x0100;

//-------------------------------------------------------------------------[GLMobSchedule]
//
BOOL GLMobSchedule::m_bRENDACT = FALSE;

GLMobSchedule::GLMobSchedule(void) :
m_fReGenTimeMin(4.0f)
, m_fReGenTimeMax(4.0f)
, m_bRendGenPos(TRUE)

, m_dwPC_REGEN_GATEID(UINT_MAX)

, m_pNext(NULL)
, m_dwVolatFlags(NULL)
, m_fTimer(0.0f)
, m_bALive(FALSE)
, m_bGroub(FALSE)
, m_dwGlobID(UINT_MAX)

, m_dwRendGenIndex(0)
	
, m_vMax(0,0,0)
, m_vMin(0,0,0)
, m_nRegenMin(0)
, m_nRegenHour(0)
, m_strGroupName("")
, m_bLeaderMonster(FALSE)
, m_bEachregen(FALSE)
, m_bGroupRegen( FALSE )
, m_wRendGenPosNum( EM_MAXRENDDISTRANGE )
, m_wRendGenPosDist( EM_MINDISTFACTOR )

, m_dwFlags(0L)
, m_fScale(1.f)
, m_fActivityArea(0)
, m_fInfluenceY(0.0f)
{
	ZeroMemory( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	GLMobSchedule::UseAffineMatrix ();
}

GLMobSchedule::~GLMobSchedule(void)
{
}

BOOL GLMobSchedule::Load ( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;
	SFile >> dwVer;
	dwSize = SFile.ReadBlockSize ();

	switch ( dwVer )
	{
	case 0x0100:	Load_0100( SFile );	break;
	case 0x0101:	Load_0101( SFile );	break;
	case 0x0102:	Load_0102( SFile );	break;
	case 0x0103:	Load_0103( SFile );	break;
	case 0x0104:	Load_0104( SFile ); break;
	case 0x0105:	Load_0105( SFile ); break;
	case 0x0106:	Load_0106( SFile ); break;
	case 0x0107:	Load_0107( SFile ); break;
	case 0x0108:	Load_0108( SFile );	break;
	case 0x0109:	Load_0109( SFile );	break;
    case 0x0110:	Load_0110( SFile );	break;
	case 0x0111:	Load_0111( SFile ); break;
	case 0x0112:	Load_0112( SFile ); break;
	case 0x0113:	Load_0113( SFile ); break;
	default:
	{
		MessageBox ( NULL, "GLMobSchedule::Load() Unknown data version.", "ERROR", MB_OK );
		break;
	}
	}	

	//	박스의 크기를 계산.
	CalculateBox ();

	if ( m_bRendGenPos )
		std::random_shuffle ( m_vectorRendGenPos.begin(), m_vectorRendGenPos.end() );

	// 랜덤 시간 리젠 기능 추가되면서 추가됨;
	if ( dwVer < 0x0111 )
	{
		m_fReGenTimeMax = m_fReGenTimeMin;
	}
	return TRUE;
}

BOOL GLMobSchedule::Load_0100( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;

	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTimeMin;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	return TRUE;
}
BOOL GLMobSchedule::Load_0101( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;

	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTimeMin;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0102( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTimeMin;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0103( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTimeMin;


	SFile >> m_nRegenMin;
	SFile >> m_nRegenHour;

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0104( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTimeMin;

	SFile >> m_nRegenMin;
	SFile >> m_nRegenHour;

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0105( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTimeMin;


	SFile >> m_nRegenMin;
	SFile >> m_nRegenHour;

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	if( m_strGroupName == "" )
		m_bLeaderMonster = FALSE;

	SFile >> m_bRendGenPos;
	SFile >> m_wRendGenPosNum;
	SFile >> m_wRendGenPosDist;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0106( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTimeMin;


	SFile >> m_nRegenMin;
	SFile >> m_nRegenHour;

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bLeaderMonster;
	SFile >> m_bGroupRegen;
	SFile >> m_strGroupName;

	if( m_strGroupName == "" )
		m_bLeaderMonster = FALSE;

	SFile >> m_bRendGenPos;
	SFile >> m_wRendGenPosNum;
	SFile >> m_wRendGenPosDist;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}
	
	return TRUE;
}

BOOL GLMobSchedule::Load_0107( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;
	
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );

	SFile >> m_fReGenTimeMin;
	SFile >> m_nRegenHour;
	SFile >> m_nRegenMin;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_bGroupRegen;
	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	if( m_strGroupName == "" )
		m_bLeaderMonster = FALSE;

	SFile >> m_bRendGenPos;
	SFile >> m_wRendGenPosNum;
	SFile >> m_wRendGenPosDist;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;
	
	return TRUE;
}

BOOL GLMobSchedule::Load_0108( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;

	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );

	SFile >> m_fReGenTimeMin;
	SFile >> m_nRegenHour;
	SFile >> m_nRegenMin;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_bGroupRegen;
	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	if( m_strGroupName == "" )
		m_bLeaderMonster = FALSE;

	SFile >> m_bRendGenPos;
	SFile >> m_wRendGenPosNum;
	SFile >> m_wRendGenPosDist;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_dwFlags;

	return TRUE;
}


BOOL GLMobSchedule::Load_0109( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;

	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );

	SFile >> m_fReGenTimeMin;
	SFile >> m_nRegenHour;
	SFile >> m_nRegenMin;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_bEachregen;
	SFile >> m_bGroupRegen;
	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	if( m_strGroupName == "" )
		m_bLeaderMonster = FALSE;

	SFile >> m_bRendGenPos;
	SFile >> m_wRendGenPosNum;
	SFile >> m_wRendGenPosDist;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_dwFlags;

	return TRUE;
}

BOOL GLMobSchedule::Load_0110( sc::BaseStream &SFile )
{
    DWORD dwVer, dwSize;

    SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );
    SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );

    SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );

    SFile >> m_fReGenTimeMin;
    SFile >> m_nRegenHour;
    SFile >> m_nRegenMin;

    SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
    DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


    BOOL bExist;
    SMOBACTION sMobAction;

    SFile >> bExist;
    while ( bExist )
    {
        SFile >> dwVer;
        SFile >> dwSize;
        SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
        m_sMobActList.ADDTAIL ( sMobAction );

        SFile >> bExist;
    }

    SFile >> m_bEachregen;
    SFile >> m_bGroupRegen;
    SFile >> m_bLeaderMonster;
    SFile >> m_strGroupName;

    if( m_strGroupName == "" )
        m_bLeaderMonster = FALSE;

    SFile >> m_bRendGenPos;
    SFile >> m_wRendGenPosNum;
    SFile >> m_wRendGenPosDist;

    DWORD dwNum = 0;
    SFile >> dwNum;
    m_vectorRendGenPos.clear();

    D3DXVECTOR3 vPos;
    for ( DWORD i=0; i<dwNum; ++i )
    {
        SFile >> vPos;
        m_vectorRendGenPos.push_back ( vPos );
    }

    SFile >> m_dwPC_REGEN_GATEID;
    SFile >> m_strBUSLOCATION;

    SFile >> m_dwFlags;
    SFile >> m_fScale;

    return TRUE;
}

BOOL GLMobSchedule::Load_0111( sc::BaseStream &SFile )
{
	DWORD dwVer, dwSize;

	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );

	SFile >> m_fReGenTimeMin;
	SFile >> m_fReGenTimeMax;
	SFile >> m_nRegenHour;
	SFile >> m_nRegenMin;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_bEachregen;
	SFile >> m_bGroupRegen;
	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	if( m_strGroupName == "" )
		m_bLeaderMonster = FALSE;

	SFile >> m_bRendGenPos;
	SFile >> m_wRendGenPosNum;
	SFile >> m_wRendGenPosDist;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_dwFlags;
	SFile >> m_fScale;

	return TRUE;
}


BOOL GLMobSchedule::Load_0112( sc::BaseStream &SFile )
{
	Load_0111(SFile);

	// 행동반경(활동영역)
	SFile >> m_fActivityArea;

	return TRUE;
}

BOOL GLMobSchedule::Load_0113( sc::BaseStream &SFile )
{
	Load_0112(SFile);

	SFile >> m_fInfluenceY;

	return TRUE;
}


BOOL GLMobSchedule::Save ( sc::SerialFile &SFile )
{
	SFile << VERSION;
	
	SFile.BeginBlock();

	SFile.WriteBuffer ( &m_CrowID, sizeof(SNATIVEID) );
	SFile.WriteBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );

	SFile.WriteBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile << m_fReGenTimeMin;
	SFile << m_fReGenTimeMax;
	SFile << m_nRegenHour;
	SFile << m_nRegenMin;

	SFile.WriteBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );

	sc::SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	if ( pCur )	SFile << (BOOL) TRUE;
	else		SFile << (BOOL) FALSE;

	while ( pCur )
	{
		SMOBACTION &sMobAction = pCur->Data;

		SFile << SMOBACTION::VERSION;
		SFile << (DWORD) sizeof(SMOBACTION);
		SFile.WriteBuffer ( &sMobAction, sizeof(SMOBACTION) );

		if ( pCur->pNext )	SFile << (BOOL) TRUE;
		else				SFile << (BOOL) FALSE;

		pCur = pCur->pNext;
	}


	SFile << m_bEachregen;
	SFile << m_bGroupRegen;
	SFile << m_bLeaderMonster;
	SFile << m_strGroupName;

	SFile << m_bRendGenPos;
	SFile << m_wRendGenPosNum;
	SFile << m_wRendGenPosDist;

	SFile << static_cast<DWORD> ( m_vectorRendGenPos.size() );
	VEC3ARRAY_ITER iter = m_vectorRendGenPos.begin ();
	VEC3ARRAY_ITER iter_end = m_vectorRendGenPos.end ();
	for ( ; iter!=iter_end; ++iter )
	{
		SFile <<  (*iter);
	}

	SFile << m_dwPC_REGEN_GATEID;
	SFile << m_strBUSLOCATION;

	SFile << m_dwFlags;
    SFile << m_fScale;

	SFile << m_fActivityArea;
	SFile << m_fInfluenceY;	

	SFile.EndBlock();

	return TRUE;
}

BOOL GLMobSchedule::GenerateRendGenPos ( NavigationMesh* pNaviMesh )
{
	m_vectorRendGenPos.clear();

	BOOL bCol = FALSE;
	D3DXVECTOR3 vStart;
	DWORD		dwStartID = 0;
	pNaviMesh->IsCollision
	(
		m_pAffineParts->vTrans + D3DXVECTOR3(0,+5,0), m_pAffineParts->vTrans - D3DXVECTOR3(0,+5,0),
		vStart, dwStartID, bCol
	);
	if ( !bCol )		return FALSE;

	m_vectorRendGenPos.push_back ( vStart );

	for ( int i=0; i<EM_MAXRENDPOS; ++i )
	for ( int j = 0; j<m_wRendGenPosNum; j++ )
	{
		float fDist = (float)j * (float)m_wRendGenPosDist;
		fDist += EM_RENDDISTSTART;
		D3DXVECTOR3 vRendPos = m_pAffineParts->vTrans;
		D3DXVECTOR3 vDir(1,0,0);

		D3DXMATRIX matY;
		D3DXMatrixRotationY ( &matY, D3DX_PI*2.0f / EM_MAXRENDPOS * i );
		D3DXVec3TransformCoord ( &vDir, &vDir, &matY );

		vRendPos += vDir * fDist;
		DWORD dwEndID = 0;

		D3DXVECTOR3 vEndPos;
		pNaviMesh->IsCollision
		(
			vRendPos + D3DXVECTOR3(0,+15,0), vRendPos - D3DXVECTOR3(0,+15,0),
			vEndPos, dwEndID, bCol
		);

		if ( bCol )
		{
			bCol = pNaviMesh->LineOfSightTest ( dwStartID, vStart, dwEndID, vEndPos );
			if ( bCol )
			{
				m_vectorRendGenPos.push_back ( vRendPos );
			}
		}
	}

	std::random_shuffle ( m_vectorRendGenPos.begin(), m_vectorRendGenPos.end() );

	return TRUE;
}

namespace
{
	void RESIZEBOX ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const D3DXVECTOR3& vPos )
	{
		if ( vMax.x < vPos.x )	vMax.x = vPos.x;
		if ( vMax.y < vPos.y )	vMax.y = vPos.y;
		if ( vMax.z < vPos.z )	vMax.z = vPos.z;

		if ( vMin.x > vPos.x )	vMin.x = vPos.x;
		if ( vMin.y > vPos.y )	vMin.y = vPos.y;
		if ( vMin.z > vPos.z )	vMin.z = vPos.z;
	}
};
void GLMobSchedule::CalculateBox ()
{
	m_vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	m_vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	D3DXVECTOR3 vPos;

	vPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);
	RESIZEBOX ( m_vMax, m_vMin, vPos );

	vPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);
	RESIZEBOX ( m_vMax, m_vMin, vPos );

	sc::SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		switch ( pCur->Data.emAction )
		{
		case EMACTION_MOVE:
			{
				vPos = pCur->Data.vPos + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);
				RESIZEBOX ( m_vMax, m_vMin, vPos );

				vPos = pCur->Data.vPos + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);
				RESIZEBOX ( m_vMax, m_vMin, vPos );
			}
			break;
		};
	}
}

BOOL GLMobSchedule::IsCollision ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 )
{
	if ( !COLLISION::IsCollisionLineToAABB(vPoint1,vPoint2,m_vMax,m_vMin) )	return FALSE;

	D3DXVECTOR3 vMax = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);
	D3DXVECTOR3 vMin = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);

	BOOL bCol = COLLISION::IsCollisionLineToAABB(vPoint1,vPoint2,vMax,vMin);
	if ( bCol )		return TRUE;

	sc::SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		switch ( pCur->Data.emAction )
		{
		case EMACTION_MOVE:
			{
				vMax = pCur->Data.vPos + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);
				vMin = pCur->Data.vPos + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);

				bCol = COLLISION::IsCollisionLineToAABB(vPoint1,vPoint2,vMax,vMin);
				if ( bCol )		return TRUE;
			}
			break;
		};
	}

	return FALSE;
}

const D3DXVECTOR3 GLMobSchedule::m_vHIGHER = D3DXVECTOR3(0,4,0);
const float GLMobSchedule::m_fSPHERE_S1 = 8.0f;
const float GLMobSchedule::m_fSPHERE_S2 = 4.0f;
const float GLMobSchedule::m_fSPHERE_S3 = 2.0f;

void GLMobSchedule::operator= ( const GLMobSchedule &MobSch )
{
	DXAFFINEMATRIX::SetAffineValue ( MobSch.m_pAffineParts );

	StringCchCopy ( m_szName, EM_MAXSZNAME, MobSch.m_szName );
	m_CrowID = MobSch.m_CrowID;
	m_fReGenTimeMin = MobSch.m_fReGenTimeMin;
	m_fReGenTimeMax = MobSch.m_fReGenTimeMax;

	m_nRegenMin  = MobSch.m_nRegenMin;
	m_nRegenHour = MobSch.m_nRegenHour;
	memcpy( m_bDayOfWeek, MobSch.m_bDayOfWeek, sizeof(m_bDayOfWeek) );

	//	Note : 휘발성 자료는 리샛됨.
	m_dwVolatFlags = NULL;
	m_fTimer	   = 0.0f;
	m_bALive	   = FALSE;
	m_bGroub	   = FALSE;
	m_dwGlobID	   = UINT_MAX;
	m_RegenTime	   = 0;

	m_bLeaderMonster = MobSch.m_bLeaderMonster;
	m_bEachregen = MobSch.m_bEachregen;
	m_bGroupRegen = MobSch.m_bGroupRegen;
	m_strGroupName	 = MobSch.m_strGroupName;

	m_sMobActList = MobSch.m_sMobActList;

	m_dwPC_REGEN_GATEID = MobSch.m_dwPC_REGEN_GATEID;

	m_bLeaderMonster = MobSch.m_bLeaderMonster;
	m_strGroupName	 = MobSch.m_strGroupName;

	m_bRendGenPos = MobSch.m_bRendGenPos;
	m_wRendGenPosNum = MobSch.m_wRendGenPosNum;
	m_wRendGenPosDist = MobSch.m_wRendGenPosDist;
	m_vectorRendGenPos = MobSch.m_vectorRendGenPos;

	m_dwFlags = MobSch.m_dwFlags;

    m_fScale = MobSch.m_fScale;

	m_fActivityArea = MobSch.m_fActivityArea;
	m_fInfluenceY = MobSch.m_fInfluenceY;

	CalculateBox ();
}

HRESULT GLMobSchedule::RenderGenPos ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if (!m_bRendGenPos)
        return E_FAIL;

	DWORD dwColor = 0xffffffff;		// White
	const PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData( m_CrowID );
	if (!pCrow)
        return FALSE;

	if ( pCrow->m_emCrow == CROW_MATERIAL )
        dwColor = 0xffff0000;
	else
        dwColor = 0xffffffff;

	VEC3ARRAY_ITER iter = m_vectorRendGenPos.begin();
	VEC3ARRAY_ITER iter_end = m_vectorRendGenPos.end();
	for ( ; iter!=iter_end; ++iter )
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, (*iter), m_fSPHERE_S2, NULL, dwColor );
	}

	return S_OK;
}

HRESULT GLMobSchedule::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;
	DWORD dwColor = 0xffffffff;		// White

	if ( !COLLISION::IsCollisionVolume(cv,m_vMax,m_vMin) )
        return FALSE;


	const PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData( m_CrowID );
	if ( !pCrow )
        return FALSE;

	if ( pCrow->m_emCrow == CROW_MATERIAL )
        dwColor = 0xffff0000;
	else
        dwColor = 0xffffffff;


	D3DXVECTOR3 vOldPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER;
	EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos, m_fSPHERE_S1, NULL, dwColor );

	sc::SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		switch ( pCur->Data.emAction )
		{
		case EMACTION_MOVE:
			{
				EDITMESHS::RENDERLINE ( pd3dDevice, vOldPos, pCur->Data.vPos + m_vHIGHER, dwColor );

				vOldPos = pCur->Data.vPos + m_vHIGHER;
				EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos, m_fSPHERE_S2, NULL, dwColor );
			}
			break;

		case EMACTION_ACTSTAY:
			{
				EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos+m_vHIGHER, m_fSPHERE_S3, NULL, dwColor );
			}
			break;
		};
	}

	return S_OK;
}

void GLMobSchedule::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	D3DXVec3TransformCoord( &DXAFFINEMATRIX::m_pAffineParts->vTrans, &DXAFFINEMATRIX::m_pAffineParts->vTrans, &matRotate );

	sc::SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		D3DXVec3TransformCoord( &pCur->Data.vPos, &pCur->Data.vPos, &matRotate );
	}

	VEC3ARRAY_ITER iter = m_vectorRendGenPos.begin();
	VEC3ARRAY_ITER iter_end = m_vectorRendGenPos.end();
	for ( ; iter!=iter_end; ++iter )
	{
		D3DXVec3TransformCoord( &(*iter), &(*iter), &matRotate );
	}

	CalculateBox();
}

const bool GLMobSchedule::GetUseRegenTimer ( )
{
	// Min, Max 값이 모두 0 이라면 사용하지 않는다;
	if( m_fReGenTimeMin == 0.0f && m_fReGenTimeMax == 0.0f )
		return false;

	return true;
}

const bool GLMobSchedule::GetUseRegenTimeStamp ( )
{
	// RegenTimer 사용중이라면 사용하지 않는다;
	if( GetUseRegenTimer() )
		return false;

	// 날짜가 설정되어 있지 않으면 사용하지 않는다;
	if( !m_bDayOfWeek[0] && !m_bDayOfWeek[1] && !m_bDayOfWeek[2] &&
		!m_bDayOfWeek[3] && !m_bDayOfWeek[4] && !m_bDayOfWeek[5] && !m_bDayOfWeek[6] )
		return false;

	return true;
}

void GLMobSchedule::SetGenTime()
{
	m_RegenTime = CTime::GetCurrentTime();
}

const bool GLMobSchedule::IsRegen ( )
{
	// Timer 를 이용해 리젠할 경우;
	if ( GetUseRegenTimer() )
	{		
		if ( m_fTimer >= 0.0f )
			return false;

		return true;
	}	
	// 리젠타임을 사용할 경우;
	else if( GetUseRegenTimeStamp() )
	{
		CTime curTime = CTime::GetCurrentTime();
		
		for( int i = 1; i <= 7; i++ )
		{
			if( curTime.GetDayOfWeek() == i && !m_bDayOfWeek[i-1] ) 
			{
				m_RegenTime = 0;
				return false;
			}
		}
		if( curTime.GetHour() != m_nRegenHour || curTime.GetMinute() != m_nRegenMin )
		{
			m_RegenTime = 0;
			return false;
		}

		if( m_RegenTime.GetDayOfWeek() == curTime.GetDayOfWeek() && 
			m_RegenTime.GetHour() == curTime.GetHour() &&
			m_RegenTime.GetMinute() == curTime.GetMinute() )
			return false;

		return true;
	}
	
	// 아무것도 설정되어 있지 않을경우 첫번째 Drop 일 경우에만 Drop 한다;
	if ( UINT_MAX == m_dwGlobID )
	{
		return true;
	}

	// 이미 소환 되었을 경우 리젠을 하지 않는다;
	return false;
}

void GLMobSchedule::SetRegenTimer ( float fRegenTimeMin, float fRegenTimeMax )
{
	m_fReGenTimeMin = fRegenTimeMin;
	m_fReGenTimeMax = fRegenTimeMax;
}

void GLMobSchedule::AddPosition ( const D3DXVECTOR3& sPopsition )
{
	m_vectorRendGenPos.push_back( sPopsition );
}

void GLMobSchedule::SetRotation ( float fRotate )
{
	m_pAffineParts->vRotate.x = fRotate * D3DX_PI / 180.0f;
}

void GLMobSchedule::SetCrowID ( const SNATIVEID& sCrowID )
{
	m_CrowID = sCrowID;
}

void GLMobSchedule::SetScale ( float fScale )
{
	m_fScale = fScale;
}

void GLMobSchedule::ModifierSchList( D3DXVECTOR3* pOffset, NavigationMesh* pNaviMesh )
{
	D3DXVECTOR3 vStart;
	DWORD dwStartID = 0;
	BOOL bCol = FALSE;

	pNaviMesh->IsCollision
	(
		m_pAffineParts->vTrans + D3DXVECTOR3(0,+5,0), m_pAffineParts->vTrans - D3DXVECTOR3(0,+5,0),
		vStart, dwStartID, bCol
	);
	if( !bCol )
		return;

	sc::SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for( ; pCur; pCur = pCur->pNext )
	{
		if( pCur->Data.emAction != EMACTION_MOVE ) continue;

		pCur->Data.vPos -= *pOffset;

		D3DXVECTOR3 vEndPos;
		DWORD dwEndID = 0;
		bCol = FALSE;

		pNaviMesh->IsCollision
		(
			pCur->Data.vPos + D3DXVECTOR3(0,+15,0), pCur->Data.vPos - D3DXVECTOR3(0,+15,0),
			vEndPos, dwEndID, bCol
		);
		if( bCol )
			pCur->Data.vPos = vEndPos;
	}

	CalculateBox();
}

void GLMobSchedule::ResetTimer()
{
	m_fTimer = static_cast<float>(
		sc::Random::getInstance().RandomNumber(m_fReGenTimeMin, m_fReGenTimeMax));
}

GLMOBGROUB& GLMOBGROUB::operator= ( const GLMOBGROUB& value )
{
	m_fTimer = value.m_fTimer;
	m_dwRendGenIndex = value.m_dwRendGenIndex;

	m_vecSchedule = value.m_vecSchedule;

	return *this;
}

void GLMOBGROUB::ADD ( GLMobSchedule* pSch )
{
	m_vecSchedule.push_back ( pSch );
}

void GLMOBGROUB::DEL ( GLMobSchedule* pSch )
{
	VECSCHEDULE_ITER pos = std::find ( m_vecSchedule.begin(), m_vecSchedule.end(), pSch );
	if ( pos!=m_vecSchedule.end() )
	{
		m_vecSchedule.erase ( pos );
	}
}