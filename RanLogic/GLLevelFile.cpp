#include "pch.h"

#include "../EngineLib/NaviMesh/Collision/SimpleCollision.h"
#include "../EngineLib/NaviMesh/Collision/LayerCollision.h"
#include "../EngineLib/DxLand/DxLandEff.h"
#include "../EngineLib/G-Logic/GLOGIC.h"
#include "../EngineLib/GlobalParam.h"

#include "./Land/GLLandMark.h"
#include "./Quest/GLQuest.h"
#include "./GLogicDataMan.h"

#include "GLLevelFile.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace COMMENT
{
	std::string CDT_SIGN[EMSIGN_SIZE] =
	{
		"=",
		"<",
		">",
		"<=",
		">=",
		"항상참"
	};

	std::string CDT_SIGN_ID[EMSIGN_SIZE] =
	{
		"EMSIGN_SAME",
		"EMSIGN_HIGHER",
		"EMSIGN_LOWER",
		"EMSIGN_SAMEHIGHER",
		"EMSIGN_SAMELOWER",
		"EMSIGN_ALWAYS"
	};
};

VOID SLEVEL_REQUIRE::SaveCsvHead( std::fstream &SFile )
{
	SFile << "Entry Req m_bForceExit" << ",";
	SFile << "Entry Req bPartyMbr" << ",";

	SFile << "Entry Req signLevel" << ",";
	SFile << "Entry Req wLevel" << ",";
	SFile << "Entry Req wLevel2" << ",";

	SFile << "Entry Req sItemID_MID" << ",";
	SFile << "Entry Req sItemID_SID" << ",";

	char szTemp[256] = {0,};

	for( int i = 0; i < MAX_ITEM; ++i )
	{
		sprintf(szTemp, "Entry Req sItemID_MID_%d", i + 1 );

		SFile << szTemp << ",";

		sprintf(szTemp, "Entry Req sItemID_SID_%d", i + 1 );

		SFile << szTemp << ",";
	}

	SFile << "Entry Req sSkillID_MID" << ",";
	SFile << "Entry Req sSkillID_SID" << ",";

	SFile << "Entry Req sComQuestID_MID" << ",";
	SFile << "Entry Req sComQuestID_SID" << ",";
	SFile << "Entry Req sActQuestID_MID" << ",";
	SFile << "Entry Req sActQuestID_SID" << ",";

	SFile << "Entry Req signLiving" << ",";
	SFile << "Entry Req nLiving" << ",";
	SFile << "Entry Req signBright" << ",";
	SFile << "Entry Req nBright" << ",";
}

VOID SLEVEL_REQUIRE::SaveCsv( std::fstream &SFile )
{
	SFile << m_bForceExit << ",";
	SFile << m_bPartyMbr << ",";

	SFile << (DWORD)m_signLevel << ",";
	SFile << m_wLevel << ",";
	SFile << m_wLevel2 << ",";

	SFile << m_sItemID.wMainID << ",";
	SFile << m_sItemID.wSubID  << ",";

	for( int i = 0; i < MAX_ITEM; ++i )
	{
		SFile << m_sItemIDMulti[i].wMainID << ",";
		SFile << m_sItemIDMulti[i].wSubID  << ",";
	}
	
	SFile << m_sSkillID.wMainID << ",";
	SFile << m_sSkillID.wSubID << ",";

	SFile << m_sComQuestID.wMainID << ",";
	SFile << m_sComQuestID.wSubID << ",";
	SFile << m_sActQuestID.wMainID << ",";
	SFile << m_sActQuestID.wSubID << ",";

	SFile << (DWORD)m_signLiving << ",";
	SFile << m_nLiving << ",";
	SFile << (DWORD)m_signBright << ",";
	SFile << m_nBright << ",";
}

VOID SLEVEL_REQUIRE::LoadCsv( CStringArray &StrArray )
{
	int iCsvCur = 2;
	m_bForceExit = (0!=atol( StrArray[ iCsvCur++ ] ));
	m_bPartyMbr = (0!=atol( StrArray[ iCsvCur++ ] ));

	m_signLevel = (EMCDT_SIGN)atol( StrArray[ iCsvCur++ ] );
	m_wLevel  = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_wLevel2 = (WORD)atol( StrArray[ iCsvCur++ ] );

	m_sItemID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sItemID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );

	for( int i = 0; i < MAX_ITEM; ++i )
	{
		m_sItemIDMulti[i].wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
		m_sItemIDMulti[i].wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );
	}
	
	m_sSkillID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sSkillID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );

	m_sComQuestID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sComQuestID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sActQuestID.wMainID = (WORD)atol( StrArray[ iCsvCur++ ] );
	m_sActQuestID.wSubID  = (WORD)atol( StrArray[ iCsvCur++ ] );

	m_signLiving = (EMCDT_SIGN)atol( StrArray[ iCsvCur++ ] );
	m_nLiving	 = atol( StrArray[ iCsvCur++ ] );
	m_signBright = (EMCDT_SIGN)atol( StrArray[ iCsvCur++ ] );
	m_nBright = atol( StrArray[ iCsvCur++ ] );
}

BOOL SLEVEL_REQUIRE::SAVE ( sc::SerialFile &SFile )
{
	SFile << DWORD(VERSION);
	SFile.BeginBlock();
	{
		SFile << (DWORD)m_signLevel;
		SFile << m_wLevel;
		SFile << m_wLevel2;

		SFile << m_sActQuestID.dwID;
		SFile << m_sComQuestID.dwID;

		SFile << m_sSkillID.dwID;
		SFile << m_sItemID.dwID;

		for( int i = 0; i < MAX_ITEM; ++i )
			SFile << m_sItemIDMulti[i].dwID;

		SFile << (DWORD)m_signBright;
		SFile << m_nBright;
		SFile << (DWORD)m_signLiving;
		SFile << m_nLiving;

		SFile << m_bPartyMbr;
		SFile << m_bForceExit;
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL SLEVEL_REQUIRE::LOAD_0101 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;

	SFile >> m_bPartyMbr;

	SFile >> dwData;		m_signLevel = (EMCDT_SIGN) dwData;
	SFile >> m_wLevel;
	m_wLevel2 = 0;
	SFile >> m_sItemID.dwID;
	SFile >> m_sSkillID.dwID;

	SFile >> m_sComQuestID.dwID;
	SFile >> m_sActQuestID.dwID;

	SFile >> dwData;		m_signLiving = (EMCDT_SIGN) dwData;
	SFile >> m_nLiving;
	SFile >> dwData;		m_signBright = (EMCDT_SIGN) dwData;
	SFile >> m_nBright;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_REQUIRE::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_REQUIRE::LOAD_0102 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;

	SFile >> m_bPartyMbr;

	SFile >> dwData;		m_signLevel = (EMCDT_SIGN) dwData;
	SFile >> m_wLevel;
	SFile >> m_wLevel2;
	SFile >> m_sItemID.dwID;
	SFile >> m_sSkillID.dwID;

	SFile >> m_sComQuestID.dwID;
	SFile >> m_sActQuestID.dwID;

	SFile >> dwData;		m_signLiving = (EMCDT_SIGN) dwData;
	SFile >> m_nLiving;
	SFile >> dwData;		m_signBright = (EMCDT_SIGN) dwData;
	SFile >> m_nBright;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_REQUIRE::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_REQUIRE::LOAD_0103 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;

	SFile >> dwData;		m_signLevel = (EMCDT_SIGN) dwData;
	SFile >> m_wLevel;
	SFile >> m_wLevel2;

	SFile >> m_sActQuestID.dwID;
	SFile >> m_sComQuestID.dwID;

	SFile >> m_sSkillID.dwID;
	SFile >> m_sItemID.dwID;

	SFile >> dwData;		m_signBright = (EMCDT_SIGN) dwData;
	SFile >> m_nBright;
	SFile >> dwData;		m_signLiving = (EMCDT_SIGN) dwData;
	SFile >> m_nLiving;

	SFile >> m_bPartyMbr;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_REQUIRE::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_REQUIRE::LOAD_0104 ( sc::BaseStream &SFile, DWORD dwSize )
{ 
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;

	SFile >> dwData;		m_signLevel = (EMCDT_SIGN) dwData;
	SFile >> m_wLevel;
	SFile >> m_wLevel2;

	SFile >> m_sActQuestID.dwID;
	SFile >> m_sComQuestID.dwID;

	SFile >> m_sSkillID.dwID;
	SFile >> m_sItemID.dwID;

	SFile >> dwData;		m_signBright = (EMCDT_SIGN) dwData;
	SFile >> m_nBright;
	SFile >> dwData;		m_signLiving = (EMCDT_SIGN) dwData;
	SFile >> m_nLiving;

	SFile >> m_bPartyMbr;
	SFile >> m_bForceExit;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_REQUIRE::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_REQUIRE::LOAD_0105 ( sc::BaseStream &SFile, DWORD dwSize )
{ 
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;

	SFile >> dwData;		m_signLevel = (EMCDT_SIGN) dwData;
	SFile >> m_wLevel;
	SFile >> m_wLevel2;

	SFile >> m_sActQuestID.dwID;
	SFile >> m_sComQuestID.dwID;

	SFile >> m_sSkillID.dwID;
	SFile >> m_sItemID.dwID;

	for( int i = 0; i < MAX_ITEM; ++i )
		SFile >> m_sItemIDMulti[i].dwID;

	SFile >> dwData;		m_signBright = (EMCDT_SIGN) dwData;
	SFile >> m_nBright;
	SFile >> dwData;		m_signLiving = (EMCDT_SIGN) dwData;
	SFile >> m_nLiving;

	SFile >> m_bPartyMbr;
	SFile >> m_bForceExit;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_REQUIRE::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_REQUIRE::LOAD ( sc::BaseStream &SFile )
{
	DWORD dwVersion, dwSize;
	SFile >> dwVersion;
	SFile >> dwSize;

	switch ( dwVersion )
	{
	case 0x0101:	LOAD_0101(SFile, dwSize);	break;
	case 0x0102:	LOAD_0102(SFile, dwSize);	break;
	case 0x0103:	LOAD_0103(SFile, dwSize);	break;
	case 0x0104:	LOAD_0104(SFile, dwSize);	break;
	case VERSION:	LOAD_0105(SFile, dwSize);	break;
	default:
		{
			GASSERT(0&&"SLEVEL_REQUIRE::LOAD 알수 없는 버전의 data입니다.");
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		}
		break;
	};

	return TRUE;
}

template<class TYPE>
bool compare ( TYPE lvalue, EMCDT_SIGN emSign, TYPE rvalue )
{
	switch ( emSign )
	{
	case EMSIGN_ALWAYS:
		return true;

	case EMSIGN_SAME:		//	=
		return lvalue == rvalue;

	case EMSIGN_HIGHER:		//	<
		return lvalue < rvalue;

	case EMSIGN_LOWER:		//	>
		return lvalue > rvalue;

	case EMSIGN_SAMEHIGHER:	//	<=
		return lvalue <= rvalue;

	case EMSIGN_SAMELOWER:	//	>=
		return lvalue >= rvalue;
	};

	return false;
}

BOOL SLEVEL_REQUIRE::IsCheckItem() const
{
	std::vector<SNATIVEID> vItemID;

	if( m_sItemID != SNATIVEID(false))
	{
		vItemID.push_back(m_sItemID);
	}

	for( int i = 0; i < MAX_ITEM; ++i )
	{
		if( m_sItemIDMulti[i] != SNATIVEID(false))
		{
			vItemID.push_back(m_sItemIDMulti[i]);
		}
	}

	if( vItemID.size() > 0 ) return TRUE;

	return FALSE;
}

BOOL SLEVEL_REQUIRE::IsHaveItem( GLCHARLOGIC * pCHARLOGIC, std::vector<SNATIVEID>& vList ) const
{
	std::vector<SNATIVEID> vItemID;

	if( m_sItemID != SNATIVEID(false))
	{
		vItemID.push_back(m_sItemID);
		vList.push_back(m_sItemID);
	}

	for( int i = 0; i < MAX_ITEM; ++i )
	{
		if( m_sItemIDMulti[i] != SNATIVEID(false))
		{
			vItemID.push_back(m_sItemIDMulti[i]);
			vList.push_back(m_sItemIDMulti[i]);
		}
	}

	std::vector<SNATIVEID>::iterator iter = vItemID.begin();

	SINVENITEM* pInvenItem = NULL;

	BOOL bIsHave = FALSE;

	while( iter != vItemID.end() )
	{
		pInvenItem = pCHARLOGIC->m_cInventory.FindItem ( *(iter) );

		if( pInvenItem != NULL || pCHARLOGIC->ISHAVEITEM( *(iter), 1 ) )
		{
			bIsHave = TRUE;

			break;
		}

		++iter;
	}

	if( vItemID.size() > 0 && !bIsHave ) return FALSE;

	return TRUE;
}

EMREQFAIL SLEVEL_REQUIRE::ISCOMPLETE ( GLCHARLOGIC * pCHARLOGIC ) const
{
	//m_bPartyMbr;

	bool bcomplete(false);

	if ( m_signLevel == EMSIGN_FROMTO )
	{
		if (m_wLevel > pCHARLOGIC->m_wLevel || m_wLevel2 < pCHARLOGIC->m_wLevel)
            return EMREQUIRE_LEVEL;
	}
    else
    {
		if (!compare(m_wLevel, m_signLevel, pCHARLOGIC->m_wLevel))
            return EMREQUIRE_LEVEL;
	}

	std::vector<SNATIVEID> vItemID;
	
	if( m_sItemID != SNATIVEID(false))
	{
		vItemID.push_back(m_sItemID);
	}

	for( int i = 0; i < MAX_ITEM; ++i )
	{
		if( m_sItemIDMulti[i] != SNATIVEID(false))
		{
			vItemID.push_back(m_sItemIDMulti[i]);
		}
	}

	std::vector<SNATIVEID>::iterator iter = vItemID.begin();

	SINVENITEM* pInvenItem = NULL;

	BOOL bIsHave = FALSE;

	while( iter != vItemID.end() )
	{
		pInvenItem = pCHARLOGIC->m_cInventory.FindItem ( *(iter) );

		if( pInvenItem != NULL || pCHARLOGIC->ISHAVEITEM( *(iter), 1 ) )
		{
			bIsHave = TRUE;

			break;
		}

		++iter;
	}

	if( vItemID.size() > 0 && !bIsHave ) return EMREQUIRE_ITEM;

	/*
	if ( m_sItemID!=SNATIVEID(false) )
	{
		bcomplete = false;
		SINVENITEM* pInvenItem = pCHARLOGIC->m_cInventory.FindItem ( m_sItemID );
		if ( pInvenItem )
		{
			bcomplete = true;
		}
		else
		{
			for ( int i=0; i<SLOT_TSIZE; ++i )
			{
				EMSLOT emSLOT = (EMSLOT) i;
				if ( pCHARLOGIC->VALID_SLOT_ITEM(emSLOT) )
				{
					const SITEMCUSTOM& sITEM = pCHARLOGIC->GET_SLOT_ITEM ( emSLOT );
					if ( sITEM.GetNativeID() == m_sItemID )
                        bcomplete = true;
					break;
				}
			}
		}

		if ( !bcomplete )
            return EMREQUIRE_ITEM;
	}
	*/

	if ( m_sSkillID!=SNATIVEID(false) )
	{
		SKILL_MAP_ITER iter = pCHARLOGIC->m_ExpSkills.find ( m_sSkillID.dwID );
		if ( pCHARLOGIC->m_ExpSkills.end() == iter )
            return EMREQUIRE_SKILL;
	}

	if ( m_sComQuestID!=SNATIVEID(false) )
	{
		GLQUESTPROG *pPROG = pCHARLOGIC->m_cQuestPlay.FindEnd ( m_sComQuestID.dwID );
		if ( !pPROG || !pPROG->m_bCOMPLETE )
            return EMREQUIRE_QUEST_COM;
	}

	if ( m_sActQuestID!=SNATIVEID(false) )
	{
		GLQUESTPROG *pPROG = pCHARLOGIC->m_cQuestPlay.FindProc ( m_sActQuestID.dwID );
		if ( !pPROG )
            return EMREQUIRE_QUEST_ACT;
	}

	if ( ! compare ( m_nLiving, m_signLiving, pCHARLOGIC->m_nLiving ) )
        return EMREQUIRE_LIVING;
	if ( ! compare ( m_nBright, m_signBright, pCHARLOGIC->m_nBright ) )
        return EMREQUIRE_BRIGHT;



	return EMREQUIRE_COMPLETE;
}

BOOL SLEVEL_HEAD::SAVE ( sc::SerialFile &SFile )
{
	SFile << DWORD(VERSION);
	SFile.BeginBlock();
	{
		SFile << m_strMapName;
		SFile << m_strWldFile;
		SFile << m_strNaviBypassFile;
		SFile << (DWORD)m_eDivision;
		SFile << (DWORD)m_emBright;
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL SLEVEL_HEAD::LOAD_0101 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;
	SFile >> m_strWldFile;
	SFile >> m_strMapName;
	SFile >> dwData;			m_emBright = (EMBRIGHT) dwData;

	SFile >> dwData;			m_eDivision = (EMDIVISION) dwData;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_HEAD::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_HEAD::LOAD_0102 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;
	SFile >> m_strMapName;
	SFile >> m_strWldFile;
	SFile >> dwData;			m_eDivision = (EMDIVISION) dwData;
	SFile >> dwData;			m_emBright = (EMBRIGHT) dwData;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_HEAD::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_HEAD::LOAD_0103 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	DWORD dwData;
	SFile >> m_strMapName;
	SFile >> m_strWldFile;
	SFile >> m_strNaviBypassFile;
	SFile >> dwData;			m_eDivision = (EMDIVISION) dwData;
	SFile >> dwData;			m_emBright = (EMBRIGHT) dwData;

	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_HEAD::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_HEAD::LOAD ( sc::BaseStream &SFile )
{
	DWORD dwVersion;
	SFile >> dwVersion;

	DWORD dwSize;
	SFile >> dwSize;

	switch ( dwVersion )
	{
	case 0x0101:
        LOAD_0101(SFile, dwSize);
        break;
	case 0x0102:
        LOAD_0102(SFile, dwSize);
        break;
	case VERSION:
		LOAD_0103(SFile, dwSize);
		break;
	default:
		{
			GASSERT(0&&"SLEVEL_HEAD::LOAD 알수 없는 버전의 data입니다.");
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		}
		break;
	};

	return TRUE;
}

void SLEVEL_HEAD::Assign ( const SLEVEL_HEAD_100 &sOldData )
{
	m_strWldFile = sOldData.m_szWldFile;
	m_strMapName = sOldData.m_szMapName;

	m_eDivision = sOldData.m_eDivision;
}


BOOL SLEVEL_ETC_FUNC::SAVE ( sc::SerialFile &SFile )
{
	SFile << DWORD(VERSION);
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( &m_bUseFunction, sizeof(m_bUseFunction) );

		// Save Camera Control
		if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
		{
			SFile << m_sControlCam.fUp;
			SFile << m_sControlCam.fDown;
			SFile << m_sControlCam.fLeft;
			SFile << m_sControlCam.fRight;
			SFile << m_sControlCam.fFar;
			SFile << m_sControlCam.fNear;
			SFile << m_sControlCam.vCamPos.x;			
			SFile << m_sControlCam.vCamPos.y;
			SFile << m_sControlCam.vCamPos.z;
		}

		// Save Limit Time
		if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
		{
			SFile << m_sLimitTime.nLimitMinute;
			SFile << m_sLimitTime.nTellTermMinute;
		}

		// Save Land Effect
		if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
		{
			SFile << static_cast<int>(m_vecLandEffect.size());
			for( size_t i = 0; i < m_vecLandEffect.size(); i++ )
			{
				SLANDEFFECT landEffect = m_vecLandEffect[i];
				SFile << (int)landEffect.emLandEffectType;
				SFile << landEffect.fValue;
				SFile << landEffect.vMinPos.x;
                SFile << landEffect.vMinPos.y;
				SFile << landEffect.vMinPos.z;
				SFile << landEffect.vMaxPos.x;
                SFile << landEffect.vMaxPos.y;
				SFile << landEffect.vMaxPos.z;
				SFile << landEffect.bCircleCheck;
				SFile << landEffect.fLength;
			}
		}

		// Save Re-Entry Limit Time
		if( m_bUseFunction[EMETCFUNC_REENTRYLIMIT] )
		{
			SFile << m_dwReEntryTime;
		}
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD_0100 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	//		SFile.ReadBuffer( &m_bUseFunction, sizeof(m_bUseFunction) );
	SFile >> m_bUseFunction[EMETCFUNC_LIMITTIME];
	SFile >> m_bUseFunction[EMETCFUNC_CONTROLCAM];
	SFile >> m_bUseFunction[EMETCFUNC_LANDEFFECT];
	SFile >> m_bUseFunction[EMETCFUNC_DIFFICULTY];
	SFile >> m_bUseFunction[EMETCFUNC_AROUNDREGEN];

	// Load Limit Time
	if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
	{
		SFile >> m_sLimitTime.nLimitMinute;
		SFile >> m_sLimitTime.nTellTermMinute;
	}

	// Load Camera Control
	if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
	{
		SFile >> m_sControlCam.fUp;
		SFile >> m_sControlCam.fDown;
		SFile >> m_sControlCam.fLeft;
		SFile >> m_sControlCam.fRight;
		SFile >> m_sControlCam.fFar;
		SFile >> m_sControlCam.fNear;
		SFile >> m_sControlCam.vCamPos.x;			
		SFile >> m_sControlCam.vCamPos.y;
		SFile >> m_sControlCam.vCamPos.z;
	}


	m_vecLandEffect.clear();
	// Load Land Effect
	if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
	{
		int iSize, EffectType;
		SFile >> iSize;
		for( int i = 0; i < iSize; i++ )
		{
			SLANDEFFECT landEffect;
			SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
			SFile >> landEffect.fValue;
			SFile >> landEffect.vMinPos.x;
			SFile >> landEffect.vMinPos.z;
			SFile >> landEffect.vMaxPos.x;
			SFile >> landEffect.vMaxPos.z;

			D3DXVECTOR3 vMin, vMax;
			vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            vMin.y = 0.0f;
            vMin.z = min( landEffect.vMinPos.z, landEffect.vMaxPos.z );

			vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
			vMax.y = 0.0f;
            vMax.z = max( landEffect.vMinPos.z, landEffect.vMaxPos.z );

			landEffect.vMinPos = vMin;
			landEffect.vMaxPos = vMax;


			m_vecLandEffect.push_back(landEffect);
		}
	}


	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD_0101 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	SFile >> m_bUseFunction[ EMETCFUNC_LIMITTIME ];
	SFile >> m_bUseFunction[ EMETCFUNC_CONTROLCAM ];
	SFile >> m_bUseFunction[ EMETCFUNC_LANDEFFECT ];
	SFile >> m_bUseFunction[ EMETCFUNC_DIFFICULTY ];
	SFile >> m_bUseFunction[ EMETCFUNC_AROUNDREGEN ];
	SFile >> m_bUseFunction[ EMETCFUNC_MOBCHASE ];

	// Load Limit Time
	if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
	{
		SFile >> m_sLimitTime.nLimitMinute;
		SFile >> m_sLimitTime.nTellTermMinute;
	}

	// Load Camera Control
	if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
	{
		SFile >> m_sControlCam.fUp;
		SFile >> m_sControlCam.fDown;
		SFile >> m_sControlCam.fLeft;
		SFile >> m_sControlCam.fRight;
		SFile >> m_sControlCam.fFar;
		SFile >> m_sControlCam.fNear;
		SFile >> m_sControlCam.vCamPos.x;			
		SFile >> m_sControlCam.vCamPos.y;
		SFile >> m_sControlCam.vCamPos.z;
	}


	m_vecLandEffect.clear();
	// Load Land Effect
	if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
	{
		int iSize, EffectType;
		SFile >> iSize;
		for( int i = 0; i < iSize; i++ )
		{
            SLANDEFFECT landEffect;
            SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
            SFile >> landEffect.fValue;
            SFile >> landEffect.vMinPos.x;
            SFile >> landEffect.vMinPos.z;
            SFile >> landEffect.vMaxPos.x;
            SFile >> landEffect.vMaxPos.z;

            D3DXVECTOR3 vMin, vMax;
            vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            vMin.y = 0.0f;
            vMin.z = min( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            vMax.y = 0.0f;
            vMax.z = max( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            landEffect.vMinPos = vMin;
            landEffect.vMaxPos = vMax;

			m_vecLandEffect.push_back(landEffect);
		}
	}


	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD_0102 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	SFile >> m_bUseFunction[ EMETCFUNC_LIMITTIME ];
	SFile >> m_bUseFunction[ EMETCFUNC_CONTROLCAM ];
	SFile >> m_bUseFunction[ EMETCFUNC_LANDEFFECT ];
	SFile >> m_bUseFunction[ EMETCFUNC_DIFFICULTY ];
	SFile >> m_bUseFunction[ EMETCFUNC_AROUNDREGEN ];
	SFile >> m_bUseFunction[ EMETCFUNC_MOBCHASE ];
	SFile >> m_bUseFunction[ EMETCFUNC_REGENONCE ];


	// Load Limit Time
	if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
	{
		SFile >> m_sLimitTime.nLimitMinute;
		SFile >> m_sLimitTime.nTellTermMinute;
	}

	// Load Camera Control
	if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
	{
		SFile >> m_sControlCam.fUp;
		SFile >> m_sControlCam.fDown;
		SFile >> m_sControlCam.fLeft;
		SFile >> m_sControlCam.fRight;
		SFile >> m_sControlCam.fFar;
		SFile >> m_sControlCam.fNear;
		SFile >> m_sControlCam.vCamPos.x;			
		SFile >> m_sControlCam.vCamPos.y;
		SFile >> m_sControlCam.vCamPos.z;
	}


	m_vecLandEffect.clear();
	// Load Land Effect
	if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
	{
		int iSize, EffectType;
		SFile >> iSize;
		for( int i = 0; i < iSize; i++ )
		{
            SLANDEFFECT landEffect;
            SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
            SFile >> landEffect.fValue;
            SFile >> landEffect.vMinPos.x;
            SFile >> landEffect.vMinPos.z;
            SFile >> landEffect.vMaxPos.x;
            SFile >> landEffect.vMaxPos.z;

            D3DXVECTOR3 vMin, vMax;
            vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            vMin.y = 0.0f;
            vMin.z = min( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            vMax.y = 0.0f;
            vMax.z = max( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            landEffect.vMinPos = vMin;
            landEffect.vMaxPos = vMax;

			m_vecLandEffect.push_back(landEffect);
		}
	}


	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD_0103 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	SFile >> m_bUseFunction[ EMETCFUNC_LIMITTIME ];
	SFile >> m_bUseFunction[ EMETCFUNC_CONTROLCAM ];
	SFile >> m_bUseFunction[ EMETCFUNC_LANDEFFECT ];
	SFile >> m_bUseFunction[ EMETCFUNC_DIFFICULTY ];
	SFile >> m_bUseFunction[ EMETCFUNC_AROUNDREGEN ];
	SFile >> m_bUseFunction[ EMETCFUNC_MOBCHASE ];
	SFile >> m_bUseFunction[ EMETCFUNC_REGENONCE ];

	// Load Camera Control
	if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
	{
		SFile >> m_sControlCam.fUp;
		SFile >> m_sControlCam.fDown;
		SFile >> m_sControlCam.fLeft;
		SFile >> m_sControlCam.fRight;
		SFile >> m_sControlCam.fFar;
		SFile >> m_sControlCam.fNear;
		SFile >> m_sControlCam.vCamPos.x;			
		SFile >> m_sControlCam.vCamPos.y;
		SFile >> m_sControlCam.vCamPos.z;
	}

	// Load Limit Time
	if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
	{
		SFile >> m_sLimitTime.nLimitMinute;
		SFile >> m_sLimitTime.nTellTermMinute;
	}


	m_vecLandEffect.clear();
	// Load Land Effect
	if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
	{
		int iSize, EffectType;
		SFile >> iSize;
		for( int i = 0; i < iSize; i++ )
		{
            SLANDEFFECT landEffect;
            SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
            SFile >> landEffect.fValue;
            SFile >> landEffect.vMinPos.x;
            SFile >> landEffect.vMinPos.z;
            SFile >> landEffect.vMaxPos.x;
            SFile >> landEffect.vMaxPos.z;

            D3DXVECTOR3 vMin, vMax;
            vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            vMin.y = 0.0f;
            vMin.z = min( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            vMax.y = 0.0f;
            vMax.z = max( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            landEffect.vMinPos = vMin;
            landEffect.vMaxPos = vMax;

			m_vecLandEffect.push_back(landEffect);
		}
	}


	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD_0104 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	SFile >> m_bUseFunction[ EMETCFUNC_LIMITTIME ];
	SFile >> m_bUseFunction[ EMETCFUNC_CONTROLCAM ];
	SFile >> m_bUseFunction[ EMETCFUNC_LANDEFFECT ];
	SFile >> m_bUseFunction[ EMETCFUNC_DIFFICULTY ];
	SFile >> m_bUseFunction[ EMETCFUNC_AROUNDREGEN ];
	SFile >> m_bUseFunction[ EMETCFUNC_MOBCHASE ];
	SFile >> m_bUseFunction[ EMETCFUNC_REGENONCE ];

	// Load Camera Control
	if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
	{
		SFile >> m_sControlCam.fUp;
		SFile >> m_sControlCam.fDown;
		SFile >> m_sControlCam.fLeft;
		SFile >> m_sControlCam.fRight;
		SFile >> m_sControlCam.fFar;
		SFile >> m_sControlCam.fNear;
		SFile >> m_sControlCam.vCamPos.x;			
		SFile >> m_sControlCam.vCamPos.y;
		SFile >> m_sControlCam.vCamPos.z;
	}

	// Load Limit Time
	if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
	{
		SFile >> m_sLimitTime.nLimitMinute;
		SFile >> m_sLimitTime.nTellTermMinute;
	}


	m_vecLandEffect.clear();
	// Load Land Effect
	if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
	{
		int iSize, EffectType;
		SFile >> iSize;
		for( int i = 0; i < iSize; i++ )
		{
			SLANDEFFECT landEffect;
			SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
			SFile >> landEffect.fValue;
			SFile >> landEffect.vMinPos.x;
			SFile >> landEffect.vMinPos.z;
			SFile >> landEffect.vMaxPos.x;
			SFile >> landEffect.vMaxPos.z;
			SFile >> landEffect.bCircleCheck;
			SFile >> landEffect.fLength;

			if( !landEffect.bCircleCheck )
			{
                D3DXVECTOR3 vMin, vMax;
                vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
                vMin.y = 0.0f;
                vMin.z = min( landEffect.vMinPos.z, landEffect.vMaxPos.z );

                vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
                vMax.y = 0.0f;
                vMax.z = max( landEffect.vMinPos.z, landEffect.vMaxPos.z );

				landEffect.vMinPos = vMin;
				landEffect.vMaxPos = vMax;
			}


			m_vecLandEffect.push_back(landEffect);
		}
	}


	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}


BOOL SLEVEL_ETC_FUNC::LOAD_0105 ( sc::BaseStream &SFile, DWORD dwSize )
{
	DWORD dwRead = SFile.GetfTell();

	SFile >> m_bUseFunction[ EMETCFUNC_LIMITTIME ];
	SFile >> m_bUseFunction[ EMETCFUNC_CONTROLCAM ];
	SFile >> m_bUseFunction[ EMETCFUNC_LANDEFFECT ];
	SFile >> m_bUseFunction[ EMETCFUNC_DIFFICULTY ];
	SFile >> m_bUseFunction[ EMETCFUNC_AROUNDREGEN ];
	SFile >> m_bUseFunction[ EMETCFUNC_MOBCHASE ];
	SFile >> m_bUseFunction[ EMETCFUNC_REGENONCE ];
	SFile >> m_bUseFunction[ EMETCFUNC_REENTRYLIMIT ];

	// Load Camera Control
	if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
	{
		SFile >> m_sControlCam.fUp;
		SFile >> m_sControlCam.fDown;
		SFile >> m_sControlCam.fLeft;
		SFile >> m_sControlCam.fRight;
		SFile >> m_sControlCam.fFar;
		SFile >> m_sControlCam.fNear;
		SFile >> m_sControlCam.vCamPos.x;			
		SFile >> m_sControlCam.vCamPos.y;
		SFile >> m_sControlCam.vCamPos.z;
	}

	// Load Limit Time
	if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
	{
		SFile >> m_sLimitTime.nLimitMinute;
		SFile >> m_sLimitTime.nTellTermMinute;
	}


	m_vecLandEffect.clear();
	// Load Land Effect
	if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
	{
		int iSize, EffectType;
		SFile >> iSize;
		for( int i = 0; i < iSize; i++ )
		{
            SLANDEFFECT landEffect;
			SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
			SFile >> landEffect.fValue;
			SFile >> landEffect.vMinPos.x;
			SFile >> landEffect.vMinPos.z;
			SFile >> landEffect.vMaxPos.x;
			SFile >> landEffect.vMaxPos.z;
			SFile >> landEffect.bCircleCheck;
			SFile >> landEffect.fLength;

			if( !landEffect.bCircleCheck )
			{
                D3DXVECTOR3 vMin, vMax;
                vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
                vMin.y = 0.0f;
                vMin.z = min( landEffect.vMinPos.z, landEffect.vMaxPos.z );

                vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
                vMax.y = 0.0f;
                vMax.z = max( landEffect.vMinPos.z, landEffect.vMaxPos.z );

				landEffect.vMinPos = vMin;
				landEffect.vMaxPos = vMax;
			}


			m_vecLandEffect.push_back(landEffect);
		}
	}

	// Load Re-Entry Limit Time
	if( m_bUseFunction[EMETCFUNC_REENTRYLIMIT] )
	{
		SFile >> m_dwReEntryTime;
	}


	dwRead = SFile.GetfTell() - dwRead;
	GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");

	return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD_0106 ( sc::BaseStream &SFile, DWORD dwSize )
{
    DWORD dwRead = SFile.GetfTell();

    SFile >> m_bUseFunction[ EMETCFUNC_LIMITTIME ];
    SFile >> m_bUseFunction[ EMETCFUNC_CONTROLCAM ];
    SFile >> m_bUseFunction[ EMETCFUNC_LANDEFFECT ];
    SFile >> m_bUseFunction[ EMETCFUNC_DIFFICULTY ];
    SFile >> m_bUseFunction[ EMETCFUNC_AROUNDREGEN ];
    SFile >> m_bUseFunction[ EMETCFUNC_MOBCHASE ];
    SFile >> m_bUseFunction[ EMETCFUNC_REGENONCE ];
    SFile >> m_bUseFunction[ EMETCFUNC_REENTRYLIMIT ];

    // Load Camera Control
    if( m_bUseFunction[EMETCFUNC_CONTROLCAM] ) 
    {
        SFile >> m_sControlCam.fUp;
        SFile >> m_sControlCam.fDown;
        SFile >> m_sControlCam.fLeft;
        SFile >> m_sControlCam.fRight;
        SFile >> m_sControlCam.fFar;
        SFile >> m_sControlCam.fNear;
        SFile >> m_sControlCam.vCamPos.x;			
        SFile >> m_sControlCam.vCamPos.y;
        SFile >> m_sControlCam.vCamPos.z;
    }

    // Load Limit Time
    if( m_bUseFunction[EMETCFUNC_LIMITTIME] ) 
    {
        SFile >> m_sLimitTime.nLimitMinute;
        SFile >> m_sLimitTime.nTellTermMinute;
    }


    m_vecLandEffect.clear();
    // Load Land Effect
    if( m_bUseFunction[EMETCFUNC_LANDEFFECT] )
    {
        int iSize, EffectType;
        SFile >> iSize;
        for( int i = 0; i < iSize; i++ )
        {
            SLANDEFFECT landEffect;
            SFile >> EffectType; landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)EffectType;
            SFile >> landEffect.fValue;
            SFile >> landEffect.vMinPos.x;
            SFile >> landEffect.vMinPos.y;  // 높이값 추가됨;
            SFile >> landEffect.vMinPos.z;
            SFile >> landEffect.vMaxPos.x;
            SFile >> landEffect.vMaxPos.y;  // 높이값 추가됨;
            SFile >> landEffect.vMaxPos.z;
            SFile >> landEffect.bCircleCheck;
            SFile >> landEffect.fLength;

            //위치정보 x,y,z 모두 저장되게 바뀜으로써 필요 없어짐;
            //if( !landEffect.bCircleCheck )
            //{
            //    D3DXVECTOR3 vMin, vMax;
            //    vMin.x = min( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            //    vMin.y = min( landEffect.vMinPos.y, landEffect.vMaxPos.y );
            //    vMin.z = min( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            //    vMax.x = max( landEffect.vMinPos.x, landEffect.vMaxPos.x );
            //    vMax.y = max( landEffect.vMinPos.y, landEffect.vMaxPos.y );
            //    vMax.z = max( landEffect.vMinPos.z, landEffect.vMaxPos.z );

            //    landEffect.vMinPos = vMin;
            //    landEffect.vMaxPos = vMax;
            //}

            m_vecLandEffect.push_back(landEffect);
        }
    }

    // Load Re-Entry Limit Time
    if( m_bUseFunction[EMETCFUNC_REENTRYLIMIT] )
    {
        SFile >> m_dwReEntryTime;
    }

    dwRead = SFile.GetfTell() - dwRead;
    GASSERT(dwRead==dwSize&&"SLEVEL_ETC_FUNC::LOAD 저장된 data량과 로드량이 차이가 납니다.");

    return TRUE;
}

BOOL SLEVEL_ETC_FUNC::LOAD ( sc::BaseStream &SFile )
{
	DWORD dwVersion, dwSize;
	SFile >> dwVersion;
	SFile >> dwSize;

    this->m_dwVersion = dwVersion;

	switch ( dwVersion )
	{
	case 0x0100:
        LOAD_0100(SFile, dwSize);
        break;
	case 0x0101:
        LOAD_0101(SFile, dwSize);
        break;
	case 0x0102:
        LOAD_0102(SFile, dwSize);
        break;
	case 0x0103:
        LOAD_0103(SFile, dwSize);
        break;
	case 0x0104:
        LOAD_0104(SFile, dwSize);
        break;
	case 0x0105:
        LOAD_0105(SFile, dwSize);
        break;
    case VERSION:
        LOAD_0106(SFile, dwSize);
        break;
	default:
		{
			GASSERT(0&&"SLEVEL_ETC_FUNC::LOAD 알수 없는 버전의 data입니다.");
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		}
		break;
	};	

	return TRUE;
}










//////////////////////////////////////////////////////////////////////////
//						GLLevelFile::LayerEffect
GLLevelFile::LayerEffect::LayerEffect( BOOL bActiveClientRender )
	: m_bActiveClientRender(bActiveClientRender)
	, m_spLandEffList(new DXLANDEFF_CAPSULE<DXLANDEFF>)
	, m_spLandEffTree(new DXLANDEFF_CAPSULE<OBJAABBNode>)
{
}

GLLevelFile::LayerEffect::~LayerEffect()
{
}

void GLLevelFile::LayerEffect::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		pCur->InitDeviceObjects ( pd3dDevice );
		pCur = pCur->m_pNext;
	}
}

void GLLevelFile::LayerEffect::RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		pCur->RestoreDeviceObjects ( pd3dDevice );
		pCur = pCur->m_pNext;
	}
}

void GLLevelFile::LayerEffect::InvalidateDeviceObjects()
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		pCur->InvalidateDeviceObjects ();
		pCur = pCur->m_pNext;
	}
}

void GLLevelFile::LayerEffect::DeleteDeviceObjects()
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		pCur->DeleteDeviceObjects ();
		pCur = pCur->m_pNext;
	}
}

PLANDEFF GLLevelFile::LayerEffect::GetLandEffHEAD() const
{
	return m_spLandEffList->m_pData;
}

PLANDEFF GLLevelFile::LayerEffect::FindLandEff ( const char* szName ) const
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )
			return pCur;

		pCur = pCur->m_pNext;
	}

	return NULL;
}

void GLLevelFile::LayerEffect::AddLandEff ( PLANDEFF pLandEff )
{
	pLandEff->m_pNext = m_spLandEffList->m_pData;
	m_spLandEffList->m_pData = pLandEff;
}

void GLLevelFile::LayerEffect::DelLandEff ( PLANDEFF pLandEff )
{
	PLANDEFF pCur, pBack;

	if ( m_spLandEffList->m_pData==pLandEff )
	{
		pCur = m_spLandEffList->m_pData;
		m_spLandEffList->m_pData = m_spLandEffList->m_pData->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		//m_dwNumLandEff--;
		return;
	}

	pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		if ( pCur==pLandEff )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			//m_dwNumLandEff--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

void GLLevelFile::LayerEffect::SetObjRotate( const D3DXMATRIX& matRotate )
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		D3DXMatrixMultiply( &pCur->m_matWorld, &pCur->m_matWorld, &matRotate );
		pCur = pCur->m_pNext;
	}
	BuildSingleEffTree();
}

void GLLevelFile::LayerEffect::BuildSingleEffTree()
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while( pCur )
	{
		pCur->ReSetAABBBox();
		pCur = pCur->m_pNext;
	}

	COLLISION::MakeAABBTree ( m_spLandEffTree->m_pData, m_spLandEffList->m_pData );
}

void GLLevelFile::LayerEffect::SetActiveLayer()
{
	m_bActiveClientRender = TRUE;
}

void GLLevelFile::LayerEffect::SetDeActiveLayer()
{
	m_bActiveClientRender = FALSE;
}

void GLLevelFile::LayerEffect::SetWorldBattleFlag_LandEff( const TCHAR* pWorldBattleFlag )
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		pCur->SetWorldBattleFlag( pWorldBattleFlag );
		pCur = pCur->m_pNext;
	}
}

void GLLevelFile::LayerEffect::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
{
	if ( !m_bActiveClientRender )
		return;

	//	Note : 단독 이펙트 ( single effect ) 랜더링.
	//
	//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_spLandEffTree->m_pData, FALSE );
}

void GLLevelFile::LayerEffect::RenderEffectName( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PLANDEFF pCur = m_spLandEffList->m_pData;
	while ( pCur )
	{
		pCur->RenderName( pd3dDevice );
		pCur = pCur->m_pNext;
	}
}

void GLLevelFile::LayerEffect::Save( sc::SerialFile &SFile )
{
	SFile << static_cast<DWORD>( VERSION );

	SFile << BOOL ( m_spLandEffList->m_pData!=NULL );

	SFile.BeginBlock();
	{
		PLANDEFF pCur = m_spLandEffList->m_pData;
		while ( pCur )
		{
			pCur->Save ( SFile );

			pCur = pCur->m_pNext;
			SFile << BOOL ( pCur!=NULL );
		}
	}
	SFile.EndBlock();
}

void GLLevelFile::LayerEffect::Load( sc::BaseStream &SFile, BOOL bSkip )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;

	if ( dwVer == 0x101 )
	{
		BOOL bLOAD(FALSE);
		SFile >> bLOAD;

		SFile >> dwBufferSize;

		if ( bSkip )
		{
			SFile.SetOffSet ( SFile.GetfTell() + dwBufferSize );
		}
		else
		{
			if ( bLOAD )
			{
				while ( bLOAD )
				{
					PLANDEFF pLandEff = new DXLANDEFF;
					pLandEff->Load ( SFile );
					AddLandEff ( pLandEff );

					SFile >> bLOAD;
				}

				BuildSingleEffTree ();
			}
		}
	}
	else if ( dwVer == 0x100 )
	{
		SFile >> dwBufferSize;

		if ( bSkip )
		{
			SFile.SetOffSet ( SFile.GetfTell() + dwBufferSize );
		}
		else
		{
			BOOL bLOAD(FALSE);
			SFile >> bLOAD;

			if ( bLOAD )
			{
				while ( bLOAD )
				{
					PLANDEFF pLandEff = new DXLANDEFF;
					pLandEff->Load ( SFile );
					AddLandEff ( pLandEff );

					SFile >> bLOAD;
				}

				BuildSingleEffTree ();
			}
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBufferSize );
	}
	
}

void GLLevelFile::LayerEffect::LoadOLD( sc::BaseStream &SFile, BOOL bExceptEffect )
{
	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	if ( bLOAD )
	{
		while ( bLOAD )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( SFile );

			if ( !bExceptEffect )
			{
				AddLandEff ( pLandEff );
			}

			SFile >> bLOAD;
		}

		BuildSingleEffTree ();
	}
}

void GLLevelFile::LayerEffect::LoadOLD_2( sc::BaseStream &SFile, BOOL bSkip )
{
	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	DWORD dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( bSkip )
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}
	else
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDEFF pLandEff = new DXLANDEFF;
				pLandEff->Load ( SFile );
				AddLandEff ( pLandEff );

				SFile >> bLOAD;
			}

			BuildSingleEffTree ();
		}
	}
}
























//////////////////////////////////////////////////////////////////////////
//							GLLevelFile
const char*		GLLevelFile::FILE_EXT		= "glmap";
char			GLLevelFile::szPATH[MAX_PATH] = "";


GLLevelFile::GLLevelFile()
	: m_dwFileVer(0)
	, m_bPeaceZone(false)
	, m_bPKZone(false)
	, m_bFreePK(false)
	, m_bPKMap(false)
	, m_bItemDrop(false)
	, m_bPetActivity(false)
	, m_bDECEXP(false)
	, m_bVehicleActivity(false)
	, m_bClubBattleZone(false)
	, m_bLunchBoxForbid(false)
	, m_bCPReset(false)
	, m_bPartyMap(false)
	, m_bUIMapSelect(false)
	, m_bUIMapInfo(false)
	, m_bExpeditionMap(false)
	, m_bCullByObjectMap(false)
	, m_bChangeSchool(true)

	//, m_dwNumLandEff(0)
	//, m_spLandEffList(new DXLANDEFF_CAPSULE<DXLANDEFF>)
	//, m_spLandEffTree(new DXLANDEFF_CAPSULE<OBJAABBNode>)
	, m_emInstanceType(EMINSTANCE_NORMAL)
	, m_bPartyInstantMap(FALSE)
	, m_dwInstantHostID(GAEAID_NULL)
	, m_bClubPkRecord(false)
	, m_pLandGateMan(NULL)
{
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );	// 기본 Effect
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );	// Layer 1 번 Effect

	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );	// 기본 Effect
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );	// Layer 1 번 Effect
}

GLLevelFile::~GLLevelFile(void)
{
	/*m_sLevelHead.;
	m_sLevelRequire;
	m_sLevelAxisInfo;
	m_sLevelEtcFunc;*/
    clearGateMan();
}

GLLevelFile& GLLevelFile::operator = (const GLLevelFile &rvalue )
{
	m_strFileName		= rvalue.m_strFileName;

	m_dwFileVer			= rvalue.m_dwFileVer;
	m_mapID				= rvalue.m_mapID;
//	m_bPeaceZone		= rvalue.m_bPeaceZone;
//	m_bPKZone				= rvalue.m_bPKZone;
	m_bFreePK				= rvalue.m_bFreePK;
	m_bPKMap				= rvalue.m_bPKMap;
	m_bItemDrop			= rvalue.m_bItemDrop; 
	m_bPetActivity		= rvalue.m_bPetActivity;
	m_bDECEXP				= rvalue.m_bDECEXP;
	m_bVehicleActivity	= rvalue.m_bVehicleActivity;
	m_bClubBattleZone	= rvalue.m_bClubBattleZone;
	m_bLunchBoxForbid	= rvalue.m_bLunchBoxForbid;
	m_bCPReset			= rvalue.m_bCPReset;
	m_bPartyMap			= rvalue.m_bPartyMap;
	m_bUIMapSelect		= rvalue.m_bUIMapSelect;
	m_bUIMapInfo			= rvalue.m_bUIMapInfo;
	m_bExpeditionMap	= rvalue.m_bExpeditionMap;	
	m_bCullByObjectMap	= rvalue.m_bCullByObjectMap;
	m_emInstanceType	= rvalue.m_emInstanceType;

	m_sLevelHead		= rvalue.m_sLevelHead;
	m_sLevelRequire		= rvalue.m_sLevelRequire;
	m_sLevelAxisInfo	= rvalue.m_sLevelAxisInfo;
	m_sLevelEtcFunc		= rvalue.m_sLevelEtcFunc;
	m_bChangeSchool		= rvalue.m_bChangeSchool;

    clearGateMan();	
	for ( LandGateManContainerConstIterator it(rvalue.m_LandGateManContainer.begin()); it != rvalue.m_LandGateManContainer.end(); ++it )
	{
		const DxLandGateMan* pLandGateMan = *it;
		DxLandGateMan* pNewLandGateMan(new DxLandGateMan);
		*pNewLandGateMan = *pLandGateMan;
		m_LandGateManContainer.push_back(pNewLandGateMan);
		if ( pLandGateMan == rvalue.m_pLandGateMan )
			m_pLandGateMan = pNewLandGateMan;
	}    

	//m_dwNumLandEff		= rvalue.m_dwNumLandEff;
	//m_spLandEffList		= rvalue.m_spLandEffList;
	//m_spLandEffTree		= rvalue.m_spLandEffTree;

	m_vecLayerEffect	= rvalue.m_vecLayerEffect;
	m_vecLayerCollision = rvalue.m_vecLayerCollision;

	//m_vecLANDMARK_DISP	= rvalue.m_vecLANDMARK_DISP;

	m_sLevelEtcFunc		= rvalue.m_sLevelEtcFunc;

//	m_bInstantMap		= rvalue.m_bInstantMap;

    m_bClubPkRecord = rvalue.m_bClubPkRecord;

	return *this;
}

void GLLevelFile::clearGateMan()
{
	m_pLandGateMan = NULL;
	for ( LandGateManContainerIterator it(m_LandGateManContainer.begin()); it != m_LandGateManContainer.end(); ++it )
	{
		DxLandGateMan* pLandGateMan = *it; 
		SAFE_DELETE(pLandGateMan);
	}
	m_LandGateManContainer.clear();
}

void GLLevelFile::ResetData()
{	
	m_dwFileVer	= 0;

	m_mapID.reset();
	m_bPeaceZone = false;
	m_bPKZone = false;
	m_bFreePK = false;
	m_bPKMap = false;
	m_bItemDrop = false;
	m_bPetActivity = false;
	m_bDECEXP = false;
	m_bVehicleActivity = false;
	m_bClubBattleZone = false;
	m_bLunchBoxForbid = false;
	m_bPartyMap = false;
	m_bUIMapSelect = false;
	m_bUIMapInfo = false;
	m_bExpeditionMap = false;
	m_bCPReset = false;
	m_bCullByObjectMap	= false;
	m_bChangeSchool = true;

	//m_dwNumLandEff = 0;
	//m_spLandEffList.reset();
	//m_spLandEffTree.reset();

	//m_pLandMarkTree = NULL;

	m_emInstanceType = EMINSTANCE_NORMAL;
	m_bPartyInstantMap = FALSE;

	m_dwInstantHostID = GAEAID_NULL;

    m_bClubPkRecord = false;

	//m_vecLANDMARK_DISP.clear();

    clearGateMan();

	m_sLevelHead		= SLEVEL_HEAD();
	m_sLevelRequire		= SLEVEL_REQUIRE();
	m_sLevelAxisInfo	= GLMapAxisInfo();
	m_sLevelEtcFunc		= SLEVEL_ETC_FUNC();
}

void GLLevelFile::SetFileName ( const char* szFile )
{
	m_strFileName = szFile;
}
const DWORD GLLevelFile::GetFileVer () const
{
	return m_dwFileVer;
}

const char* GLLevelFile::GetFileName () const
{
	return m_strFileName.c_str();
}

void GLLevelFile::SetInstantMapData( const DWORD dwHostID, const GLPartyID& PartyID )
{
	m_emInstanceType	   = EMINSTANCE_OLD_INSTANCE;
    if ( PartyID.isValidParty() == false )
	{
		m_dwInstantHostID  = dwHostID;
		m_bPartyInstantMap = FALSE;
	}
    else
    {
		m_dwInstantHostID  = PartyID.GetID();
		m_bPartyInstantMap = TRUE;
	}
	
}

const char* GLLevelFile::GetWldFileName () const
{
	return m_sLevelHead.m_strWldFile.c_str();
}

const char* GLLevelFile::GetNaviBypassFileName () const
{
	return m_sLevelHead.m_strNaviBypassFile.c_str();
}

BOOL GLLevelFile::IsEmptyNaviBypassFileName () const
{
	return m_sLevelHead.m_strNaviBypassFile.empty() ? TRUE : FALSE;
}

const EMBRIGHT GLLevelFile::GETBRIGHT () const
{
	return m_sLevelHead.m_emBright;
}

void GLLevelFile::SetLandTreeDiv ( EMDIVISION eDivision )
{
	m_sLevelHead.m_eDivision = eDivision;
}

EMDIVISION GLLevelFile::GetLandTreeDiv () const
{ 
	return m_sLevelHead.m_eDivision;
}

// 공격가능 거리가 되고, LayerSimpleCollision 에 닿지 않으면 TRUE
// 하나라도 문제가 되면 FALSE
BOOL GLLevelFile::IsAbleAttack_Range_LayerSimpleCollision( float fTakeAbleDis, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset )
{
	float fDist = D3DXVec3Length( &D3DXVECTOR3(vStart-vEnd) );
	if( fDist > fTakeAbleDis )
		return FALSE;

	// 공격 가능거리라면, LayerSimpleCollision에 걸리는지 보자. 
	// 걸리면 공격이 불가능함.
	if ( IsLayerSimpleCollision( vStart, vEnd, fOffset ) )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//							LayerCollision
BOOL GLLevelFile::IsLayerSimpleCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) const
{
	for ( DWORD i=0; i<m_vecLayerCollision.size(); ++i )
	{
		if ( m_vecLayerCollision[i]->IsCollision( vStart, vEnd, fOffset ) )
			return TRUE;
	}
	return FALSE;
}

const LayerCollision* GLLevelFile::GetLayerSimpleCollisionList( size_t unLayer ) const
{
	if ( unLayer >= m_vecLayerCollision.size() )
		return NULL;

	return m_vecLayerCollision[unLayer].get();
}

const SimpleCollisionBase* GLLevelFile::FindLayerSimpleCollision( size_t unLayer, const char* szName ) const
{
	if ( unLayer >= m_vecLayerCollision.size() )
		return NULL;

	return m_vecLayerCollision[unLayer]->FindCollision( szName );
}

void GLLevelFile::AddLayerSimpleCollision ( size_t unLayer, boost::shared_ptr<SimpleCollisionBase> spNew )
{
	if ( unLayer >= m_vecLayerCollision.size() )
		return;

	m_vecLayerCollision[unLayer]->AddCollision( spNew );
}

void GLLevelFile::ChangeLayerSimpleCollision ( size_t unLayer, int nIndex, boost::shared_ptr<SimpleCollisionBase> spNew )
{
	if ( unLayer >= m_vecLayerCollision.size() )
		return;

	m_vecLayerCollision[unLayer]->ChangeCollision( nIndex, spNew );
}

void GLLevelFile::DelLayerSimpleCollision ( size_t unLayer, DWORD dwIndex )
{
	if ( unLayer >= m_vecLayerCollision.size() )
		return;

	m_vecLayerCollision[unLayer]->DelCollision( dwIndex );
}

//////////////////////////////////////////////////////////////////////////
//							PLANDEFF
PLANDEFF GLLevelFile::GetLandEffList( size_t unLayer )
{
	if ( unLayer >= m_vecLayerEffect.size() )
		return NULL;

	return m_vecLayerEffect[unLayer]->GetLandEffHEAD();
}

PLANDEFF GLLevelFile::FindLandEff ( size_t unLayer, char* szName )
{
	if ( unLayer >= m_vecLayerEffect.size() )
		return NULL;

	return m_vecLayerEffect[unLayer]->FindLandEff( szName );
}

void GLLevelFile::AddLandEff ( size_t unLayer, PLANDEFF pLandEff )
{
	if ( unLayer >= m_vecLayerEffect.size() )
		return;

	m_vecLayerEffect[unLayer]->AddLandEff( pLandEff );
}

void GLLevelFile::DelLandEff ( size_t unLayer, PLANDEFF pLandEff )
{
	GASSERT(pLandEff);

	if ( unLayer >= m_vecLayerEffect.size() )
		return;

	m_vecLayerEffect[unLayer]->DelLandEff( pLandEff );
}

void GLLevelFile::BuildSingleEffTree ( size_t unLayer )
{
	if ( unLayer >= m_vecLayerEffect.size() )
		return;

	m_vecLayerEffect[unLayer]->BuildSingleEffTree();
}

void GLLevelFile::AddLandMark ( PLANDMARK pLandMark )
{
	m_vecLandMark.push_back( pLandMark );

	//BuildLandMarkTree();
}

void GLLevelFile::DelLandMark ( PLANDMARK pLandMark )
{
	GASSERT(pLandMark);

	VECLANDMARK_ITER iter = m_vecLandMark.begin();
	for ( ; iter!=m_vecLandMark.end(); )
	{
		if ( (*iter) == pLandMark )
		{
			iter = m_vecLandMark.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	//BuildLandMarkTree();
}

PLANDMARK GLLevelFile::FindLandMark ( char* szName )
{
	VECLANDMARK_ITER iter = m_vecLandMark.begin();
	for ( ; iter!=m_vecLandMark.end(); ++iter )
	{
		if ( (*iter)->m_strMARK == szName )
		{
			return (*iter);
		}
	}

	return NULL;
}

//void GLLevelFile::BuildLandMarkTree ()
//{
//	PLANDMARK pPrev(NULL);
//	for ( DWORD i=0; i<m_vecLandMark.size(); ++i )
//	{
//		m_vecLandMark[i]->m_dwID = i;
//		m_vecLandMark[i]->m_pNext = pPrev;
//		pPrev = m_vecLandMark[i];
//	}
//
//	COLLISION::MakeAABBTree ( m_pLandMarkTree, m_vecLandMark[m_vecLandMark.size()-1] );
//}

void GLLevelFile::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	// Note : 이펙트
	//PLANDEFF pCur = m_spLandEffList->m_pData;
	//while ( pCur )
	//{
	//	D3DXMatrixMultiply( &pCur->m_matWorld, &pCur->m_matWorld, &matRotate );
	//	pCur = pCur->m_pNext;
	//}
	//BuildSingleEffTree();

	for ( DWORD i =0; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->SetObjRotate( matRotate );
	}

	for ( DWORD i =0; i<m_vecLayerCollision.size(); ++i )
	{
		m_vecLayerCollision[i]->SetObjRotate( D3DX_PI*0.5f );
	}

	// Note : Mark
	D3DXMATRIX matWorld;
	for ( DWORD i=0; i<m_vecLandMark.size(); ++i )
	{
		matWorld._41 = m_vecLandMark[i]->m_pAffineParts->vTrans.x;
		matWorld._42 = m_vecLandMark[i]->m_pAffineParts->vTrans.y;
		matWorld._43 = m_vecLandMark[i]->m_pAffineParts->vTrans.z;
		D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );	// 위치
		m_vecLandMark[i]->m_pAffineParts->vTrans.x = matWorld._41;
		m_vecLandMark[i]->m_pAffineParts->vTrans.y = matWorld._42;
		m_vecLandMark[i]->m_pAffineParts->vTrans.z = matWorld._43;
	}
	//BuildLandMarkTree();

	// Note : 게이트
	m_pLandGateMan->SetObjRotate90();
}

DxLandGateMan* GLLevelFile::GetLandGateMan( UINT nLayer )
{
	if ( nLayer >= m_LandGateManContainer.size() )
		return NULL;

	return m_LandGateManContainer[nLayer];
}

BOOL GLLevelFile::GetUseLevelEtcFunc ( const EMETCFUNC emEtcFunc)
{
	// 시간 제한과 재입장 제어는 인던이 아닌경우 사용이 불가능하다.
	if( (emEtcFunc == EMETCFUNC_REENTRYLIMIT || emEtcFunc == EMETCFUNC_LIMITTIME) && !IsInstantMap() )
        return FALSE;

	return m_sLevelEtcFunc.m_bUseFunction[emEtcFunc]; 
}

HRESULT GLLevelFile::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	//PLANDEFF pCur = m_spLandEffList->m_pData;
	//while ( pCur )
	//{
	//	pCur->InitDeviceObjects ( pd3dDevice );

	//	pCur = pCur->m_pNext;
	//}

	for ( DWORD i =0; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->InitDeviceObjects( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLLevelFile::RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	//PLANDEFF pCur = m_spLandEffList->m_pData;
	//while ( pCur )
	//{
	//	pCur->RestoreDeviceObjects ( pd3dDevice );

	//	pCur = pCur->m_pNext;
	//}

	for ( DWORD i =0; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->RestoreDeviceObjects( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLLevelFile::InvalidateDeviceObjects()
{
	//PLANDEFF pCur = m_spLandEffList->m_pData;
	//while ( pCur )
	//{
	//	pCur->InvalidateDeviceObjects ();

	//	pCur = pCur->m_pNext;
	//}

	for ( DWORD i =0; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->InvalidateDeviceObjects();
	}

	return S_OK;
}

HRESULT GLLevelFile::DeleteDeviceObjects()
{
	//PLANDEFF pCur = m_spLandEffList->m_pData;
	//while ( pCur )
	//{
	//	pCur->DeleteDeviceObjects ();
	//
	//	pCur = pCur->m_pNext;
	//}

	for ( DWORD i =0; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->DeleteDeviceObjects();
	}

	//m_dwNumLandEff = 0;
	//m_spLandEffList.reset();
	//m_spLandEffTree.reset();

	std::for_each ( m_vecLandMark.begin(), m_vecLandMark.end(), std_afunc::DeleteObject() );
	//SAFE_DELETE(m_pLandMarkTree);

	return S_OK;
}

HRESULT GLLevelFile::FrameMove ( float fTime, float fElapsedTime )
{
	DXLANDEFF::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT GLLevelFile::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	for ( DWORD i=0; i<m_vecLandMark.size(); ++i )
	{
		m_vecLandMark[i]->RenderEdit( pd3dDevice, &cv );
	}
	//COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandMarkTree, FALSE );

	return S_OK;
}

HRESULT GLLevelFile::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
{
    if( !GLOBAL_PARAM::IsValue( GP_EFFECT ) )
		return S_OK;

	//	Note : 단독 이펙트 ( single effect ) 랜더링.
	//
//	COLLISION::dwRendAABB = 0;
//	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_spLandEffTree->m_pData, FALSE );
	for ( DWORD i =0; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->Render_EFF( pd3dDevice, cv );
	}

	return S_OK;
}

////! 승리한 클럽의 국기를 맵에 셋팅한다.
//void GLLevelFile::SetFlagWinnerClubCountry( DWORD dwGuidanceID, const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert )
//{
//	//PLANDEFF pCur = m_spLandEffList;
//	//while ( pCur )
//	//{
//	//	// 선도 관리 구역이 같을 경우만 깃발 모양을 바꿔준다.
//	//	DWORD dwGuidanceID_OUT(0);
//	//	if ( GLogicData::GetInstance().wbGetGuidanceID( pCur->GetEffName(), dwGuidanceID_OUT ) )
//	//	{
//	//		if ( dwGuidanceID == dwGuidanceID_OUT )
//	//		{
//	//			//pCur->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//	//		}
//	//	}
//	//	pCur = pCur->m_pNext;
//	//}
//}

void GLLevelFile::SetWorldBattleFlag_LandEff( const TCHAR* pWorldBattleFlag )
{
	//PLANDEFF pCur = m_spLandEffList->m_pData;
	//while ( pCur )
	//{
	//	pCur->SetWorldBattleFlag( pWorldBattleFlag );
	//	pCur = pCur->m_pNext;
	//}

	for ( DWORD i =0; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->SetWorldBattleFlag_LandEff( pWorldBattleFlag );
	}
}

// [shhan][2015.10.02] m_vecLayerEffect 의 0 번째만 사용되도록 함.
void GLLevelFile::SetLandEffect( DWORD dwGateID, bool bEnable )
{
    if (m_pLandGateMan == 0)
    {
        return;
    }

    DxLandGate* pLandGate = m_pLandGateMan->FindLandGate(dwGateID);
    if (pLandGate == 0)
    {
        return;
    }

	if ( m_vecLayerEffect.empty() )
		return;

	PLANDEFF pCur = m_vecLayerEffect[0]->FindLandEff( pLandGate->GetEffName() );
	if ( pCur )
	{
		pCur->m_bRender = bEnable;     
	}

	//TSTRING strEffName = pLandGate->GetEffName();

    //PLANDEFF pCur = m_spLandEffList->m_pData;

    //while (pCur)
    //{
    //    TSTRING strCurEffName = pCur->GetEffectName();

    //    if (strEffName == strCurEffName)
    //    {
    //        pCur->m_bRender = bEnable;            
    //    }

    //    pCur = pCur->m_pNext;
    //}
}

void GLLevelFile::SetActiveLayer( size_t unLayer )
{
	size_t unLayerUSED = unLayer+1;
	if ( unLayerUSED < m_vecLayerEffect.size() )
	{
		m_vecLayerEffect[unLayerUSED]->SetActiveLayer();
	}

	if ( unLayerUSED < m_vecLayerCollision.size() )
	{
		m_vecLayerCollision[unLayerUSED]->SetActiveLayer();
	}
}

void GLLevelFile::SetDeActiveLayer( size_t unLayer )
{
	size_t unLayerUSED = unLayer+1;
	if ( unLayerUSED < m_vecLayerEffect.size() )
	{
		m_vecLayerEffect[unLayerUSED]->SetDeActiveLayer();
	}

	if ( unLayerUSED < m_vecLayerCollision.size() )
	{
		m_vecLayerCollision[unLayerUSED]->SetDeActiveLayer();
	}
}

void GLLevelFile::SetDeActiveAllLayer()
{
	for ( DWORD i=1; i<m_vecLayerEffect.size(); ++i )
	{
		m_vecLayerEffect[i]->SetDeActiveLayer();
	}

	for ( DWORD i=1; i<m_vecLayerCollision.size(); ++i )
	{
		m_vecLayerCollision[i]->SetDeActiveLayer();
	}
}

BOOL GLLevelFile::IsExistLayerObjectForServer( size_t unLayer )
{
	size_t unLayerUSED = unLayer+1;
	if ( unLayerUSED < m_vecLayerEffect.size() )
	{
		if ( m_vecLayerEffect[unLayerUSED]->IsExistEffectForServer() )
			return TRUE;
	}

	if ( unLayerUSED < m_vecLayerCollision.size() )
	{
		if ( m_vecLayerCollision[unLayerUSED]->IsExistSimpleCollision() )
			return TRUE;
	}

	return FALSE;
}

void GLLevelFile::RenderEffectName( LPDIRECT3DDEVICEQ pd3dDevice, size_t unLayer )
{
	if ( unLayer >= m_vecLayerEffect.size() )
		return;

	m_vecLayerEffect[unLayer]->RenderEffectName( pd3dDevice );
}