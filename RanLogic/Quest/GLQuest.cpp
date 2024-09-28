#include "../pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "./GLQuest.h"
#include "./GLQuestMan.h"
#include "../GLogicDataMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SGENQUESTITEM& SGENQUESTITEM::operator= ( const SGENQUESTITEM& rvalue )
{
	sNID = rvalue.sNID;
	wNUM = rvalue.wNUM;
	fGEN_RATE = rvalue.fGEN_RATE;
	vecGEN_MOB = rvalue.vecGEN_MOB;

	return *this;
}

bool SGENQUESTITEM::Find ( DWORD dwMOB )
{
	bool bfound = std::binary_search ( vecGEN_MOB.begin(), vecGEN_MOB.end(), dwMOB );
	return bfound;
}

BOOL SGENQUESTITEM::GENMOB_PUSH ( DWORD dwID )
{
	if ( GENMOB_MAX <= vecGEN_MOB.size() )
		return S_FALSE;

	vecGEN_MOB.push_back ( dwID );
	return S_OK;
}


void SGENQUESTITEM::GENMOB_ERASE ( DWORD dwIndex )
{
	GASSERT ( vecGEN_MOB.size() > dwIndex && "GLQUEST_STEP::GENMOB_ERASE()" );
	if ( vecGEN_MOB.size() <= dwIndex )	return;

	DWQARRAY_ITER iter = vecGEN_MOB.begin()+dwIndex;
	vecGEN_MOB.erase ( iter, iter+1 );
}

bool SGENQUESTITEM::LOAD ( sc::BaseStream &SFile )
{
	DWORD dwVER(0);
	SFile >> dwVER;

	if ( dwVER==VERSION )
	{
		SFile >> sNID.dwID;
		SFile >> wNUM;
		SFile >> fGEN_RATE;
		SFile >> vecGEN_MOB;

		std::sort ( vecGEN_MOB.begin(), vecGEN_MOB.end() );
	}
	else
	{
		GASSERT(0&&"SGENQUESTITEM::LOAD() error VERSION");
	}

	return true;
}

bool SGENQUESTITEM::SAVE ( sc::SerialFile &SFile )
{
	SFile << (DWORD) VERSION;
	SFile << sNID.dwID;
	SFile << wNUM;
	SFile << fGEN_RATE;
	SFile << vecGEN_MOB;

	return true;
}

GLQUEST_STEP& GLQUEST_STEP::operator= ( const GLQUEST_STEP &rVALUE )
{
	m_strTITLE = rVALUE.m_strTITLE;
	m_strCOMMENT = rVALUE.m_strCOMMENT;

	m_strOBJ_NPCTALK = rVALUE.m_strOBJ_NPCTALK;
	m_dwNID_NPCTALK = rVALUE.m_dwNID_NPCTALK;

	m_strOBJ_MOBGEN_QITEM = rVALUE.m_strOBJ_MOBGEN_QITEM;
	m_vecMOBGEN_QITEM = rVALUE.m_vecMOBGEN_QITEM;
	
	m_dwNID_ITEMUSE = rVALUE.m_dwNID_ITEMUSE;
	m_dwNUM_ITEMUSE = rVALUE.m_dwNUM_ITEMUSE;
	m_strOBJ_ITEMUSE = rVALUE.m_strOBJ_ITEMUSE;

	m_dwNUM_LEVELUP = rVALUE.m_dwNUM_LEVELUP;
	m_strOBJ_LEVELUP = rVALUE.m_strOBJ_LEVELUP;

	m_dwType_CONFTING = rVALUE.m_dwType_CONFTING;
	m_bSCHOOL =  rVALUE.m_bSCHOOL;
	m_dwNUM_CONFTING = rVALUE.m_dwNUM_CONFTING;
	m_strOBJ_CONFTING = rVALUE.m_strOBJ_CONFTING;

	m_dwNID_ITEMGRINDING = rVALUE.m_dwNID_ITEMGRINDING;	
	m_dwNUM_ITEMGRINDING = rVALUE.m_dwNUM_ITEMGRINDING;	
	m_dwItemType = rVALUE.m_dwItemType;			
	m_bConsecution = rVALUE.m_bConsecution;
	m_strOBJ_ITEMGRINDING = rVALUE.m_strOBJ_ITEMGRINDING; 

	m_strOBJ_MOBKILL = rVALUE.m_strOBJ_MOBKILL;
	m_dwNID_MOBKILL = rVALUE.m_dwNID_MOBKILL;
	m_dwNUM_MOBKILL = rVALUE.m_dwNUM_MOBKILL;
	m_bIsPLAYERKILL = rVALUE.m_bIsPLAYERKILL;

	m_strOBJ_NPCGUARD = rVALUE.m_strOBJ_NPCGUARD;
	m_dwNID_NPCGUARD = rVALUE.m_dwNID_NPCGUARD;

	m_strOBJ_REACH_ZONE = rVALUE.m_strOBJ_REACH_ZONE;
	m_sMAPID_REACH = rVALUE.m_sMAPID_REACH;
	m_wPOSX_REACH = rVALUE.m_wPOSX_REACH;
	m_wPOSY_REACH = rVALUE.m_wPOSY_REACH;
	m_wRADIUS_REACH = rVALUE.m_wRADIUS_REACH;

	m_strOBJ_DEFENSE_ZONE = rVALUE.m_strOBJ_DEFENSE_ZONE;
	m_sMAPID_DEFENSE = rVALUE.m_sMAPID_DEFENSE;
	m_wPOSX_DEFENSE = rVALUE.m_wPOSX_DEFENSE;
	m_wPOSY_DEFENSE = rVALUE.m_wPOSY_DEFENSE;
	m_wRADIUS_DEFENSE = rVALUE.m_wRADIUS_DEFENSE;
	m_fDEFENSE_TIME = rVALUE.m_fDEFENSE_TIME;

	m_vecRESET_QITEM = rVALUE.m_vecRESET_QITEM;
	m_vecGIFT_QITEM = rVALUE.m_vecGIFT_QITEM;
	m_wLevel = rVALUE.m_wLevel;

	return *this;
}

bool GLQUEST_STEP::LOAD_0001 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	typedef std::vector<SITEMCUSTOM_103>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0002 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	typedef std::vector<SITEMCUSTOM_104>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0003 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	typedef std::vector<SITEMCUSTOM_105>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0004 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_106>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0005( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_106>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_wLevel;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0006( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_108>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_wLevel;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0007( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_109>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_wLevel;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0008( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_110>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_wLevel;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0009( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_111>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_wLevel;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}

	return true;
}

bool GLQUEST_STEP::LOAD_0010( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_112>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;
	
	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_wLevel;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}
	//SFile >> m_vecGIFT_QITEM;

	return true;
}


bool GLQUEST_STEP::LOAD_0011( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_112>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_strOBJ_NPCTALK;
	SFile >> m_dwNID_NPCTALK;

	SFile >> m_strOBJ_MOBGEN_QITEM;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}

	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;

	SFile >> m_strOBJ_NPCGUARD;
	SFile >> m_dwNID_NPCGUARD;

	SFile >> m_strOBJ_REACH_ZONE;
	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;

	SFile >> m_strOBJ_DEFENSE_ZONE;
	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;

	SFile >> m_wLevel;

	SFile >> m_vecRESET_QITEM;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}
	//SFile >> m_vecGIFT_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0012( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_112>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;
	
	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;


	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}
	//SFile >> m_vecGIFT_QITEM;
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0013( sc::BaseStream &SFile )
{
    typedef std::vector<SITEMCUSTOM_113> INVENQARRAY_OLD;
    typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

        DWORD dwSize(0);

    SFile >> m_strTITLE;
    SFile >> m_strCOMMENT;

    SFile >> m_dwNID_MOBKILL;
    SFile >> m_dwNUM_MOBKILL;
    SFile >> m_strOBJ_MOBKILL;

    DWORD dwVerMOBGENQITEM(0);
    SFile >> dwVerMOBGENQITEM;

    SFile >> dwSize;
    m_vecMOBGEN_QITEM.reserve (dwSize);
    for ( DWORD i=0; i<dwSize; ++i )
    {
        SGENQUESTITEM sGENQ;
        sGENQ.LOAD ( SFile );
        MOBGENITEM_PUSH ( sGENQ );
    }
    SFile >> m_strOBJ_MOBGEN_QITEM;

    SFile >> m_dwNID_NPCTALK;
    SFile >> m_strOBJ_NPCTALK;

    SFile >> m_sMAPID_REACH.dwID;
    SFile >> m_wPOSX_REACH;
    SFile >> m_wPOSY_REACH;
    SFile >> m_wRADIUS_REACH;
    SFile >> m_strOBJ_REACH_ZONE;

    SFile >> m_wLevel; 

    SFile >> m_dwNID_NPCGUARD;
    SFile >> m_strOBJ_NPCGUARD;

    SFile >> m_sMAPID_DEFENSE.dwID;
    SFile >> m_wPOSX_DEFENSE;
    SFile >> m_wPOSY_DEFENSE;
    SFile >> m_wRADIUS_DEFENSE;
    SFile >> m_fDEFENSE_TIME;
    SFile >> m_strOBJ_DEFENSE_ZONE;

    INVENQARRAY_OLD vecGIFT_QITEM;
    SFile >> vecGIFT_QITEM;

    m_vecGIFT_QITEM.clear();
    m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

    INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
    INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
    for ( ; iter!=iter_end; ++iter )
    {
        SITEMCUSTOM sCUSTOM;
        sCUSTOM.Assign ( (*iter) );
        m_vecGIFT_QITEM.push_back ( sCUSTOM );
    }
    SFile >> m_vecRESET_QITEM;

    SFile >> m_stepMoveMap.nidMAP.dwID;
    SFile >> m_stepMoveMap.wPosX;
    SFile >> m_stepMoveMap.wPosY;
    SFile >> m_stepMoveMap.dwGateID;

    return true;
}

bool GLQUEST_STEP::LOAD_0014( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_114> INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;

	m_vecGIFT_QITEM.clear();
	m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGIFT_QITEM.push_back ( sCUSTOM );
	}
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0015( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;


	SFile >> m_vecGIFT_QITEM;
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0016( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	SFile >> m_vecGIFT_QITEM;
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0017( sc::BaseStream &SFile )
{
    DWORD dwSize(0);

    SFile >> m_strTITLE;
    SFile >> m_strCOMMENT;

    SFile >> m_dwNUM_LEVELUP;
    SFile >> m_strOBJ_LEVELUP;

    SFile >> m_dwType_CONFTING;
    SFile >> m_bSCHOOL;
    SFile >> m_dwNUM_CONFTING; 
    SFile >> m_strOBJ_CONFTING;

    SFile >> m_dwNID_ITEMGRINDING;	
    SFile >> m_dwNUM_ITEMGRINDING;	
    SFile >> m_dwItemType;
    SFile >> m_bConsecution;
    SFile >> m_strOBJ_ITEMGRINDING; 

    SFile >> m_dwNID_ITEMUSE;
    SFile >> m_dwNUM_ITEMUSE;
    SFile >> m_strOBJ_ITEMUSE;

    SFile >> m_dwNID_MOBKILL;
    SFile >> m_dwNUM_MOBKILL;
    SFile >> m_strOBJ_MOBKILL;
    SFile >> m_bIsPLAYERKILL;

    DWORD dwVerMOBGENQITEM(0);
    SFile >> dwVerMOBGENQITEM;

    SFile >> dwSize;
    m_vecMOBGEN_QITEM.reserve (dwSize);
    for ( DWORD i=0; i<dwSize; ++i )
    {
        SGENQUESTITEM sGENQ;
        sGENQ.LOAD ( SFile );
        MOBGENITEM_PUSH ( sGENQ );
    }
    SFile >> m_strOBJ_MOBGEN_QITEM;

    SFile >> m_dwNID_NPCTALK;
    SFile >> m_strOBJ_NPCTALK;

    SFile >> m_sMAPID_REACH.dwID;
    SFile >> m_wPOSX_REACH;
    SFile >> m_wPOSY_REACH;
    SFile >> m_wRADIUS_REACH;
    SFile >> m_strOBJ_REACH_ZONE;

    SFile >> m_wLevel;	

    SFile >> m_dwNID_NPCGUARD;
    SFile >> m_strOBJ_NPCGUARD;

    SFile >> m_sMAPID_DEFENSE.dwID;
    SFile >> m_wPOSX_DEFENSE;
    SFile >> m_wPOSY_DEFENSE;
    SFile >> m_wRADIUS_DEFENSE;
    SFile >> m_fDEFENSE_TIME;
    SFile >> m_strOBJ_DEFENSE_ZONE;

    // 아이템 정보가 바뀌면 작업을 해야된다.
    typedef std::vector<SITEMCUSTOM_115> INVENQARRAY_OLD;
    typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

    INVENQARRAY_OLD vecGIFT_QITEM;
    SFile >> vecGIFT_QITEM;
    {
        m_vecGIFT_QITEM.clear();
        m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

        INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
        INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
        for ( ; iter!=iter_end; ++iter )
        {
            SITEMCUSTOM sCUSTOM;
            sCUSTOM.Assign ( (*iter) );
            m_vecGIFT_QITEM.push_back ( sCUSTOM );
        }
    }
    // 작업 끝
    SFile >> m_vecRESET_QITEM;

    SFile >> m_stepMoveMap.nidMAP.dwID;
    SFile >> m_stepMoveMap.wPosX;
    SFile >> m_stepMoveMap.wPosY;
    SFile >> m_stepMoveMap.dwGateID;

    return true;
}

bool GLQUEST_STEP::LOAD_0018( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector<SITEMCUSTOM_116> INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign ( (*iter) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}
	// 작업 끝
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0019( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector<SITEMCUSTOM_117> INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign ( (*iter) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}

	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0020( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector<SITEMCUSTOM_118> INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign ( (*iter) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}

	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0021( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_119 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}

	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0022( sc::BaseStream &SFile )
{
	DWORD dwSize( 0 );

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_120 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}


bool GLQUEST_STEP::LOAD_0023( sc::BaseStream &SFile )
{
	DWORD dwSize( 0 );

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;


	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_121 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0024 ( sc::BaseStream &SFile )
{
	DWORD dwSize( 0 );

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_122 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0025 ( sc::BaseStream &SFile )
{
	DWORD dwSize( 0 );

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_123 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGIFT_QITEM.clear();
		m_vecGIFT_QITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGIFT_QITEM.push_back ( sCUSTOM );
		}
	}
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD_0026 ( sc::BaseStream &SFile )	// 아이템 데이터가 변경되었다;
{
	DWORD dwSize( 0 );

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwNUM_LEVELUP;
	SFile >> m_strOBJ_LEVELUP;

	SFile >> m_dwType_CONFTING;
	SFile >> m_bSCHOOL;
	SFile >> m_dwNUM_CONFTING; 
	SFile >> m_strOBJ_CONFTING;

	SFile >> m_dwNID_ITEMGRINDING;	
	SFile >> m_dwNUM_ITEMGRINDING;	
	SFile >> m_dwItemType;
	SFile >> m_bConsecution;
	SFile >> m_strOBJ_ITEMGRINDING; 

	SFile >> m_dwNID_ITEMUSE;
	SFile >> m_dwNUM_ITEMUSE;
	SFile >> m_strOBJ_ITEMUSE;

	SFile >> m_dwNID_MOBKILL;
	SFile >> m_dwNUM_MOBKILL;
	SFile >> m_strOBJ_MOBKILL;
	SFile >> m_bIsPLAYERKILL;

	DWORD dwVerMOBGENQITEM(0);
	SFile >> dwVerMOBGENQITEM;

	SFile >> dwSize;
	m_vecMOBGEN_QITEM.reserve (dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SGENQUESTITEM sGENQ;
		sGENQ.LOAD ( SFile );
		MOBGENITEM_PUSH ( sGENQ );
	}
	SFile >> m_strOBJ_MOBGEN_QITEM;

	SFile >> m_dwNID_NPCTALK;
	SFile >> m_strOBJ_NPCTALK;

	SFile >> m_sMAPID_REACH.dwID;
	SFile >> m_wPOSX_REACH;
	SFile >> m_wPOSY_REACH;
	SFile >> m_wRADIUS_REACH;
	SFile >> m_strOBJ_REACH_ZONE;

	SFile >> m_wLevel;	

	SFile >> m_dwNID_NPCGUARD;
	SFile >> m_strOBJ_NPCGUARD;

	SFile >> m_sMAPID_DEFENSE.dwID;
	SFile >> m_wPOSX_DEFENSE;
	SFile >> m_wPOSY_DEFENSE;
	SFile >> m_wRADIUS_DEFENSE;
	SFile >> m_fDEFENSE_TIME;
	SFile >> m_strOBJ_DEFENSE_ZONE;

	SFile >> m_vecGIFT_QITEM;
	SFile >> m_vecRESET_QITEM;

	SFile >> m_stepMoveMap.nidMAP.dwID;
	SFile >> m_stepMoveMap.wPosX;
	SFile >> m_stepMoveMap.wPosY;
	SFile >> m_stepMoveMap.dwGateID;

	return true;
}

bool GLQUEST_STEP::LOAD ( sc::BaseStream &SFile )
{
	DWORD dwVer(0);
	DWORD dwSize(0);

	SFile >> dwVer;

    //////////////////////////////////////////////////////////////////////////
    // 이 경고를 보고 있다면 [퀘스트] 정보를 재대로 로드 할수 없다.
    //
    //이유는 다음과 같다.
    //	case VERSION:	최신 로드 함수의
    // 코드 후반부에 보면 다음과 같은 코드가 있는데
    // SFile >> m_vecGIFT_QITEM;
    //
    // m_vec의 원형을 보게 되면 SITEMCUSTOM아이템이 VECTOR로 되어있는걸 알 수 있다.
    // 즉 SITEMCUSTOM의 버전은 올라가게 되었는데. 
    // 퀘스트의 SITEMCUSTOM의 버전은 이전버전을 읽고있다는 말이다.
    // 
    // 해경 방법
    //퀘스트의 버전을 올려야 한다.
    // 
    // 더욱 열받는건 이 부분이 퀘스트에 2개나 있다.
    // 다른 부분도 찾아서 수정해야 한다.
    //
    
    // 그리고 다음 사람을 위해서 이 버전을 아이템 커스텀 버전과 맞춰놓아라.
    GASSERT( SITEMCUSTOM::VERSION == 0x0125 );

	switch ( dwVer )
	{
	case 0x0001:	LOAD_0001 ( SFile ); break;
	case 0x0002:	LOAD_0002 ( SFile ); break;
	case 0x0003:	LOAD_0003 ( SFile ); break;
	case 0x0004:	LOAD_0004 ( SFile ); break;
	case 0x0005:	LOAD_0005 ( SFile ); break;
	case 0x0006:	LOAD_0006 ( SFile ); break;
	case 0x0007:	LOAD_0007 ( SFile ); break;
	case 0x0008:	LOAD_0008 ( SFile ); break;
	case 0x0009:	LOAD_0009 ( SFile ); break;
	case 0x0010:	LOAD_0010 ( SFile ); break;
	case 0x0011:	LOAD_0011 ( SFile ); break;
	case 0x0012:	LOAD_0012 ( SFile ); break;
    case 0x0013:	LOAD_0013 ( SFile ); break;
	case 0x0014:	LOAD_0014 ( SFile ); break;
	case 0x0015:	LOAD_0015 ( SFile ); break;
    case 0x0016:	LOAD_0016 ( SFile ); break;
    case 0x0017:	LOAD_0017 ( SFile ); break;
	case 0x0018:	LOAD_0018 ( SFile ); break;
	case 0x0019:	LOAD_0019 ( SFile ); break;
	case 0x0020:	LOAD_0020 ( SFile ); break;
	case 0x0021:	LOAD_0021 ( SFile ); break;
	case 0x0022:	LOAD_0022 ( SFile ); break;
	case 0x0023:	LOAD_0023 ( SFile ); break;
	case 0x0024:	LOAD_0024 ( SFile ); break;
	case 0x0025:	LOAD_0025 ( SFile ); break;
	case VERSION:	LOAD_0026 ( SFile ); break;
	};

	return true;
}

bool GLQUEST_STEP::SAVE ( sc::SerialFile &SFile )
{
	SFile << (DWORD) VERSION;

	SFile << m_strTITLE;
	SFile << m_strCOMMENT;

	SFile << m_dwNUM_LEVELUP;
	SFile << m_strOBJ_LEVELUP;

	SFile << m_dwType_CONFTING;
	SFile << m_bSCHOOL;
	SFile << m_dwNUM_CONFTING; 
	SFile << m_strOBJ_CONFTING;

	SFile << m_dwNID_ITEMGRINDING;	
	SFile << m_dwNUM_ITEMGRINDING;	
	SFile << m_dwItemType;
	SFile << m_bConsecution;
	SFile << m_strOBJ_ITEMGRINDING; 

	SFile << m_dwNID_ITEMUSE;
	SFile << m_dwNUM_ITEMUSE;
	SFile << m_strOBJ_ITEMUSE;

	SFile << m_dwNID_MOBKILL;
	SFile << m_dwNUM_MOBKILL;
	SFile << m_strOBJ_MOBKILL;
	SFile << m_bIsPLAYERKILL;

	SFile << SGENQUESTITEM::VERSION;
	{
		SFile << (DWORD)m_vecMOBGEN_QITEM.size();
		GENMOBITEMARRAY_ITER iter = m_vecMOBGEN_QITEM.begin();
		GENMOBITEMARRAY_ITER iter_end = m_vecMOBGEN_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			(*iter).SAVE ( SFile );
		}
	}
	SFile << m_strOBJ_MOBGEN_QITEM;

	SFile << m_dwNID_NPCTALK;
	SFile << m_strOBJ_NPCTALK;
	
	SFile << m_sMAPID_REACH.dwID;
	SFile << m_wPOSX_REACH;
	SFile << m_wPOSY_REACH;
	SFile << m_wRADIUS_REACH;
	SFile << m_strOBJ_REACH_ZONE;

	SFile << m_wLevel;

	SFile << m_dwNID_NPCGUARD;
	SFile << m_strOBJ_NPCGUARD;

    SFile << m_sMAPID_DEFENSE.dwID;
	SFile << m_wPOSX_DEFENSE;
	SFile << m_wPOSY_DEFENSE;
	SFile << m_wRADIUS_DEFENSE;
	SFile << m_fDEFENSE_TIME;
	SFile << m_strOBJ_DEFENSE_ZONE;

	SFile << m_vecGIFT_QITEM;
	SFile << m_vecRESET_QITEM;

	SFile << m_stepMoveMap.nidMAP.dwID;
	SFile << m_stepMoveMap.wPosX;
	SFile << m_stepMoveMap.wPosY;
	SFile << m_stepMoveMap.dwGateID;

	return true;
}

void GLQUEST_STEP::RESETITEM_ERASE ( DWORD dwIndex )
{
	GASSERT ( m_vecRESET_QITEM.size() > dwIndex && "GLQUEST_STEP::RESETITEM_ERASE()" );
	if ( m_vecRESET_QITEM.size() <= dwIndex )	return;

	DWQARRAY_ITER iter = m_vecRESET_QITEM.begin()+dwIndex;
	m_vecRESET_QITEM.erase ( iter, iter+1 );
}

BOOL GLQUEST_STEP::MOBGENITEM_PUSH( SGENQUESTITEM& sITEM )
{
	if ( NEEDQITEM_MAX <= m_vecMOBGEN_QITEM.size() )
		return S_FALSE;

	m_vecMOBGEN_QITEM.push_back ( sITEM );
	return S_OK;
}

void GLQUEST_STEP::MOBGENITEM_ERASE ( DWORD dwIndex )
{
	GASSERT ( m_vecMOBGEN_QITEM.size() > dwIndex && "GLQUEST_STEP::MOBGENITEM_ERASE()" );
	if ( m_vecMOBGEN_QITEM.size() <= dwIndex )	return;

	GENMOBITEMARRAY_ITER iter = m_vecMOBGEN_QITEM.begin()+dwIndex;
	m_vecMOBGEN_QITEM.erase ( iter, iter+1 );
}

void GLQUEST_STEP::GIFTITEM_ERASE ( DWORD dwIndex )
{
	GASSERT ( m_vecGIFT_QITEM.size() > dwIndex && "GLQUEST_STEP::GIFTITEM_ERASE()" );
	if ( m_vecGIFT_QITEM.size() <= dwIndex )	return;

	INVENQARRAY_ITER iter = m_vecGIFT_QITEM.begin()+dwIndex;
	m_vecGIFT_QITEM.erase ( iter, iter+1 );
}

bool GLQUEST_START::LOAD_001( sc::BaseStream &SFile )
{
	SFile >> dwCLASS;
	SFile >> wSCHOOL;

	SFile >> nidITEM.dwID;
	SFile >> nidSKILL.dwID;

	SFile >> nidMAP.dwID;
	SFile >> wPosX;
	SFile >> wPosY;
	SFile >> wSIZE;

	SFile >> wLEVEL;

	return true;
}

bool GLQUEST_START::LOAD_002( sc::BaseStream &SFile )
{
	SFile >> dwCLASS;
	SFile >> wSCHOOL;

	SFile >> nidITEM.dwID;
	SFile >> nidSKILL.dwID;

	SFile >> nidMAP.dwID;
	SFile >> wPosX;
	SFile >> wPosY;
	SFile >> wSIZE;

	SFile >> wLEVEL;

	return true;
}

bool GLQUEST_START::LOAD_003( sc::BaseStream &SFile )
{
	SFile >> dwCLASS;
	SFile >> wSCHOOL;

	SFile >> wLEVEL;

	SFile >> nidSKILL.dwID;
	SFile >> nidITEM.dwID;

	SFile >> nidMAP.dwID;
	SFile >> wSIZE;
	SFile >> wPosX;
	SFile >> wPosY;

/*
	// 클래스 조건 자동 체크 기능...필요할때만 사용

	//	남자 클래스
	//	남자 일반 클래스
	DWORD dwManClass = GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_M | GLCC_SPIRIT_M;
	DWORD dwWomanClass = GLCC_FIGHTER_W | GLCC_ARMS_W | GLCC_ARCHER_W | GLCC_SPIRIT_W;
	DWORD dwManClassEx = GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_M | GLCC_SPIRIT_M | GLCC_EXTREME_M;
	DWORD dwWomanClassEx = GLCC_FIGHTER_W | GLCC_ARMS_W | GLCC_ARCHER_W | GLCC_SPIRIT_W | GLCC_EXTREME_W;

	// 모든 클래스
	if ( dwCLASS == GLCC_ALL_NEWSEX )
	{
		dwCLASS = GLCC_ALL_SCIENTIST;			
	}
	else if ( dwCLASS == dwManClass + dwWomanClass )
	{
		dwCLASS += GLCC_SCIENTIST_M + GLCC_SCIENTIST_W;
	}
	// 남자, 여자, 일반부서
	else if ( dwCLASS == dwManClass || dwCLASS == dwManClassEx ) 
	{
		dwCLASS += GLCC_SCIENTIST_M;
	}
	else if ( dwCLASS == dwWomanClass  || dwCLASS == dwWomanClassEx ) 
	{
		dwCLASS += GLCC_SCIENTIST_W;
	}
*/
	return true;

}

bool GLQUEST_START::LOAD_004( sc::BaseStream &SFile )
{
	SFile >>  dwCLASS;
	SFile >>  wSCHOOL;
	SFile >>  wLEVEL;
	SFile >>  nidSKILL.dwID;
	SFile >>  nidITEM.dwID;	
	SFile >>  nidMAP.dwID;
	SFile >>  wSIZE;
	SFile >>  wPosX;
	SFile >>  wPosY;

	SFile >>  nJOIN;
// 	SFile >> dwCLASS;
// 	SFile >> wSCHOOL;
// 
// 	SFile >> nidITEM.dwID;
// 	SFile >> nidSKILL.dwID;
// 
// 	SFile >> nidMAP.dwID;
// 	SFile >> wPosX;
// 	SFile >> wPosY;
// 	SFile >> wSIZE;
// 
// 	SFile >> wLEVEL;
// 	SFile >> nJOIN;


// 	nidSKILL.dwID = 4294967295;
// 	nidITEM.dwID = 4294967295;	
// 	wLEVEL = 65535;

	return true;
}

bool GLQUEST_START::LOAD ( sc::BaseStream &SFile )
{
	DWORD dwVERSION(0), dwSIZE(0);

	SFile >> dwVERSION;
	SFile >> dwSIZE;

	switch ( dwVERSION )
	{
	case VERSION:	LOAD_004( SFile ); break;
	case 0x0003:	LOAD_003( SFile ); break;
	case 0x0002:	LOAD_002( SFile ); break;
	case 0x0001:	LOAD_001( SFile ); break;
	default:
		{
			MessageBox ( NULL, "GLQUEST_START::LOAD(), unknown data version.", "ERROR", MB_OK );
			SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
			break;
		}
	}
	
	return true;
}

bool GLQUEST_START::SAVE ( sc::SerialFile &SFile )
{
	SFile << (DWORD) VERSION;
	SFile.BeginBlock ();
	{
		SFile << dwCLASS;
		SFile << wSCHOOL;

		SFile << wLEVEL;

		SFile << nidSKILL.dwID;
		SFile << nidITEM.dwID;	
		
		SFile << nidMAP.dwID;
		SFile << wSIZE;
		SFile << wPosX;
		SFile << wPosY;

		SFile << nJOIN;
	}
	SFile.EndBlock ();

	return true;
}

GLQUEST::GLQUEST()
    : m_dwFlags(0)
    , m_dwQuestArea(0)
    , m_dwEDITVER(1)
    , m_strTITLE("퀘스트 제목")
    , m_strCOMMENT("퀘스트에 대한 설명 부분.")
    , m_dwBeginMoney(0)
    , m_dwBeginPartyMemNum(0)
    , m_dwLimitTime(0)
    , m_dwLimitPartyMemNum(0)
	, m_bTimeLimiorKeep(false)
	, m_bPastOnlyOnOline(false)
    , m_bNonDie(false)
    , m_lnGiftEXP(0)
    , m_dwGiftMONEY(0)
    , m_dwGiftELEMENT(0)
    , m_dwGiftLIFEPOINT(0)
    , m_dwGiftDisPK(0)
    , m_dwGiftSKILLPOINT(0)
    , m_dwGiftSTATSPOINT(0)
    , m_dwGiftQUEST(UINT_MAX)
{
}

GLQUEST::~GLQUEST()
{
}

GLQUEST& GLQUEST::operator= (const GLQUEST &rVALUE)
{
	m_sNID = rVALUE.m_sNID;
	m_dwFlags = rVALUE.m_dwFlags;
	m_dwEDITVER = rVALUE.m_dwEDITVER;

    m_dwQuestArea = rVALUE.m_dwQuestArea;

	m_strTITLE = rVALUE.m_strTITLE;
	m_strCOMMENT = rVALUE.m_strCOMMENT;

	m_sSTARTOPT = rVALUE.m_sSTARTOPT;

	m_sAttendanceOption = rVALUE.m_sAttendanceOption;

	//	최초 부여 조건.
	m_dwBeginMoney = rVALUE.m_dwBeginMoney;
	m_dwBeginPartyMemNum = rVALUE.m_dwBeginPartyMemNum;

	//	진행 조건.
	m_dwLimitTime = rVALUE.m_dwLimitTime;
	m_bTimeLimiorKeep = rVALUE.m_bTimeLimiorKeep;
	m_bPastOnlyOnOline = rVALUE.m_bPastOnlyOnOline;
	m_dwLimitPartyMemNum = rVALUE.m_dwLimitPartyMemNum;
	m_bNonDie = rVALUE.m_bNonDie;

	m_vecProgStep = rVALUE.m_vecProgStep;

	//	완료시 보상.
	m_lnGiftEXP = rVALUE.m_lnGiftEXP; //경험치
	m_dwGiftMONEY = rVALUE.m_dwGiftMONEY;//돈
	m_dwGiftELEMENT = rVALUE.m_dwGiftELEMENT;//
	m_dwGiftLIFEPOINT = rVALUE.m_dwGiftLIFEPOINT;//생활점수
	m_dwGiftDisPK = rVALUE.m_dwGiftDisPK;//
	m_dwGiftSKILLPOINT = rVALUE.m_dwGiftSKILLPOINT;//스킬포인트
	m_dwGiftSTATSPOINT = rVALUE.m_dwGiftSTATSPOINT;//스텟

	m_vecGiftITEM = rVALUE.m_vecGiftITEM;
	m_vecGiftSKILL = rVALUE.m_vecGiftSKILL;
	m_dwGiftQUEST = rVALUE.m_dwGiftQUEST;

    m_ProgressEvent = rVALUE.m_ProgressEvent;

	return *this;
}

const char* GLQUEST::GetTITLE() const
{ 
    return GetTitleStr().c_str();
}

const std::string& GLQUEST::GetTitleStr() const 
{
    if (GLOBAL_PARAM::bShowContentID)
    {
        static std::string TempStr;
        TempStr = sc::string::format("%1%/%2% %3% %4%", m_sNID.wMainID, m_sNID.wSubID, m_sNID.dwID, m_strTITLE);
        return TempStr;
    }

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d", m_sNID.wMainID, m_sNID.wSubID);
		return CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
	}
	else
	{	
		return m_strTITLE; 
	}
}

const char* GLQUEST::GetCOMMENT() const
{
	return GetCommentStr().c_str();
}

const std::string& GLQUEST::GetCommentStr() const
{
	if (GLOBAL_PARAM::bShowContentID)
	{
		static std::string TempStr;
		TempStr = sc::string::format("%1%/%2% %3% %4%", m_sNID.wMainID, m_sNID.wSubID, m_sNID.dwID, m_strCOMMENT);
		return TempStr;
	}

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QD_%03d_%03d", m_sNID.wMainID, m_sNID.wSubID);
		return CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
	}
	else
	{	
		return m_strCOMMENT; 
	}
}

bool GLQUEST::LOAD_0001 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_103>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	SFile >> m_vecGiftSKILL;

	return true;
}

bool GLQUEST::LOAD_0002 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_104>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	SFile >> m_vecGiftSKILL;

	return true;
}

bool GLQUEST::LOAD_0003 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_105>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	SFile >> m_vecGiftSKILL;

	return true;
}

bool GLQUEST::LOAD_0004 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_106>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	SFile >> m_vecGiftSKILL;

	return true;
}

bool GLQUEST::LOAD_0005 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_106>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	return true;
}

bool GLQUEST::LOAD_0006 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_108>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	return true;
}

bool GLQUEST::LOAD_0007 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);
	typedef std::vector<SITEMCUSTOM_108>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	return true;
}

bool GLQUEST::LOAD_0008 ( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_109>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	//SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	return true;
}

bool GLQUEST::LOAD_0009 ( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_110>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	//SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	return true;
}

bool GLQUEST::LOAD_0010 ( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_111>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	//SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	return true;
}

bool GLQUEST::LOAD_0011 ( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_112>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	//SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	return true;
}

bool GLQUEST::LOAD_0012 ( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_112>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_dwEDITVER;
	SFile >> m_dwFlags;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginMoney;
	SFile >> m_dwBeginPartyMemNum;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_dwLimitPartyMemNum;
	SFile >> m_bNonDie;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftMONEY;
	SFile >> m_dwGiftELEMENT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}

	//SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;
	SFile >> m_dwGiftQUEST;

	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0013 ( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_112>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	DWORD dwGiftEXP;
	SFile >> dwGiftEXP;	
	m_lnGiftEXP = (LONGLONG)dwGiftEXP;
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}
	//SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}


bool GLQUEST::LOAD_0014 ( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_112>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;
	INVENQARRAY_OLD vecGiftITEM;
	SFile >> vecGiftITEM;
	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGiftITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGiftITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGiftITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}
	//SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0015 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;
	SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0016( sc::BaseStream &SFile )
{
    typedef std::vector<SITEMCUSTOM_113>	INVENQARRAY_OLD;
    typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

    DWORD dwSize(0);

    SFile >> m_dwEDITVER;

    SFile >> m_strTITLE;
    SFile >> m_strCOMMENT;

    SFile >> m_dwFlags;

    SFile >> m_dwQuestArea;

    m_sSTARTOPT.LOAD ( SFile );

    //	최초 부여 조건.
    SFile >> m_dwBeginPartyMemNum;
    SFile >> m_dwBeginMoney;

    //	진행 조건.
    SFile >> m_dwLimitTime;
    SFile >> m_bNonDie;
    SFile >> m_dwLimitPartyMemNum;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        GLQUEST_STEP sSTEP;
        sSTEP.LOAD ( SFile );
        m_vecProgStep.push_back ( sSTEP );
    }

    //	완료시 보상.
    SFile >> m_dwGiftMONEY;
    SFile >> m_lnGiftEXP;	
    SFile >> m_dwGiftSKILLPOINT;
    SFile >> m_dwGiftSTATSPOINT;
    SFile >> m_dwGiftLIFEPOINT;
    SFile >> m_dwGiftDisPK;
    SFile >> m_dwGiftELEMENT;




    SFile >> m_dwGiftQUEST;

    INVENQARRAY_OLD vecGIFT_QITEM;
    SFile >> vecGIFT_QITEM;

    m_vecGiftITEM.clear();
    m_vecGiftITEM.reserve ( vecGIFT_QITEM.size() );

    INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
    INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
    for ( ; iter!=iter_end; ++iter )
    {
        SITEMCUSTOM sCUSTOM;
        sCUSTOM.Assign ( (*iter) );
        m_vecGiftITEM.push_back ( sCUSTOM );
    }

    SFile >> m_vecGiftSKILL;


    SFile >> m_ProgressEvent.bUseProgressEvent;

    if( m_ProgressEvent.bUseProgressEvent )
    {
        SFile >> m_ProgressEvent.dwStartProgress;
        SFile >> m_ProgressEvent.dwEndProgress;
        SFile >> m_ProgressEvent.dwFailProgress;

        SFile >> m_ProgressEvent.dwfailType;

        SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.startMap.wPosX;
        SFile >> m_ProgressEvent.startMap.wPosY;
        SFile >> m_ProgressEvent.startMap.dwGateID;

        SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.endMap.wPosX;
        SFile >> m_ProgressEvent.endMap.wPosY;
        SFile >> m_ProgressEvent.endMap.dwGateID;

        SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.failMap.wPosX;
        SFile >> m_ProgressEvent.failMap.wPosY;
        SFile >> m_ProgressEvent.failMap.dwGateID;
    }

    return true;
}

bool GLQUEST::LOAD_0017( sc::BaseStream &SFile )
{
	typedef std::vector<SITEMCUSTOM_114>	INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator		INVENQARRAY_OLD_ITER;

    DWORD dwSize(0);

    SFile >> m_dwEDITVER;

    SFile >> m_strTITLE;
    SFile >> m_strCOMMENT;

    SFile >> m_dwFlags;

    SFile >> m_dwQuestArea;

    m_sSTARTOPT.LOAD ( SFile );

    //	최초 부여 조건.
    SFile >> m_dwBeginPartyMemNum;
    SFile >> m_dwBeginMoney;

    //	진행 조건.
    SFile >> m_dwLimitTime;
    SFile >> m_bNonDie;
    SFile >> m_dwLimitPartyMemNum;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        GLQUEST_STEP sSTEP;
        sSTEP.LOAD ( SFile );
        m_vecProgStep.push_back ( sSTEP );
    }

    //	완료시 보상.
    SFile >> m_dwGiftMONEY;
    SFile >> m_lnGiftEXP;	
    SFile >> m_dwGiftSKILLPOINT;
    SFile >> m_dwGiftSTATSPOINT;
    SFile >> m_dwGiftLIFEPOINT;
    SFile >> m_dwGiftDisPK;
    SFile >> m_dwGiftELEMENT;

    SFile >> m_dwGiftQUEST;

	INVENQARRAY_OLD vecGIFT_ITEM;
	SFile >> vecGIFT_ITEM;

	m_vecGiftITEM.clear();
	m_vecGiftITEM.reserve ( vecGIFT_ITEM.size() );

	INVENQARRAY_OLD_ITER iter = vecGIFT_ITEM.begin();
	INVENQARRAY_OLD_ITER iter_end = vecGIFT_ITEM.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.Assign ( (*iter) );
		m_vecGiftITEM.push_back ( sCUSTOM );
	}


    SFile >> m_vecGiftSKILL;


    SFile >> m_ProgressEvent.bUseProgressEvent;

    if( m_ProgressEvent.bUseProgressEvent )
    {
        SFile >> m_ProgressEvent.dwStartProgress;
        SFile >> m_ProgressEvent.dwEndProgress;
        SFile >> m_ProgressEvent.dwFailProgress;

        SFile >> m_ProgressEvent.dwfailType;

        SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.startMap.wPosX;
        SFile >> m_ProgressEvent.startMap.wPosY;
        SFile >> m_ProgressEvent.startMap.dwGateID;

        SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.endMap.wPosX;
        SFile >> m_ProgressEvent.endMap.wPosY;
        SFile >> m_ProgressEvent.endMap.dwGateID;

        SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.failMap.wPosX;
        SFile >> m_ProgressEvent.failMap.wPosY;
        SFile >> m_ProgressEvent.failMap.dwGateID;
    }

    return true;
}

bool GLQUEST::LOAD_0018 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;	
	SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0019 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;
	SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0020 ( sc::BaseStream &SFile )
{
    DWORD dwSize(0);

    SFile >> m_dwEDITVER;

    SFile >> m_strTITLE;
    SFile >> m_strCOMMENT;

    SFile >> m_dwFlags;

    SFile >> m_dwQuestArea;

    m_sSTARTOPT.LOAD ( SFile );
    m_sAttendanceOption.LOAD ( SFile );

    //	최초 부여 조건.
    SFile >> m_dwBeginPartyMemNum;
    SFile >> m_dwBeginMoney;

    //	진행 조건.
    SFile >> m_dwLimitTime;
    SFile >> m_bTimeLimiorKeep;
    SFile >> m_bPastOnlyOnOline;
    SFile >> m_bNonDie;
    SFile >> m_dwLimitPartyMemNum;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        GLQUEST_STEP sSTEP;
        sSTEP.LOAD ( SFile );
        m_vecProgStep.push_back ( sSTEP );
    }

    //	완료시 보상.
    SFile >> m_dwGiftMONEY;
    SFile >> m_lnGiftEXP;	
    SFile >> m_dwGiftSKILLPOINT;
    SFile >> m_dwGiftSTATSPOINT;
    SFile >> m_dwGiftLIFEPOINT;
    SFile >> m_dwGiftDisPK;
    SFile >> m_dwGiftELEMENT;

    SFile >> m_dwGiftQUEST;

    // 아이템 정보가 바뀌면 작업을 해야된다.
    typedef std::vector<SITEMCUSTOM_115> INVENQARRAY_OLD;
    typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

    INVENQARRAY_OLD vecGIFT_QITEM;
    SFile >> vecGIFT_QITEM;
    {
        m_vecGiftITEM.clear();
        m_vecGiftITEM.reserve ( vecGIFT_QITEM.size() );

        INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
        INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
        for ( ; iter!=iter_end; ++iter )
        {
            SITEMCUSTOM sCUSTOM;
            sCUSTOM.Assign ( (*iter) );
            m_vecGiftITEM.push_back ( sCUSTOM );
        }
    }
    // 작업 끝
    SFile >> m_vecGiftSKILL;


    SFile >> m_ProgressEvent.bUseProgressEvent;

    if( m_ProgressEvent.bUseProgressEvent )
    {
        SFile >> m_ProgressEvent.dwStartProgress;
        SFile >> m_ProgressEvent.dwEndProgress;
        SFile >> m_ProgressEvent.dwFailProgress;

        SFile >> m_ProgressEvent.dwfailType;

        SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.startMap.wPosX;
        SFile >> m_ProgressEvent.startMap.wPosY;
        SFile >> m_ProgressEvent.startMap.dwGateID;

        SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.endMap.wPosX;
        SFile >> m_ProgressEvent.endMap.wPosY;
        SFile >> m_ProgressEvent.endMap.dwGateID;

        SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.failMap.wPosX;
        SFile >> m_ProgressEvent.failMap.wPosY;
        SFile >> m_ProgressEvent.failMap.dwGateID;
    }

    return true;
}

bool GLQUEST::LOAD_0021 ( sc::BaseStream &SFile )
{
    DWORD dwSize(0);

    SFile >> m_dwEDITVER;

    SFile >> m_strTITLE;
    SFile >> m_strCOMMENT;

    SFile >> m_dwFlags;

    SFile >> m_dwQuestArea;

    m_sSTARTOPT.LOAD ( SFile );
    m_sAttendanceOption.LOAD ( SFile );

    //	최초 부여 조건.
    SFile >> m_dwBeginPartyMemNum;
    SFile >> m_dwBeginMoney;

    //	진행 조건.
    SFile >> m_dwLimitTime;
    SFile >> m_bTimeLimiorKeep;
    SFile >> m_bPastOnlyOnOline;
    SFile >> m_bNonDie;
    SFile >> m_dwLimitPartyMemNum;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        GLQUEST_STEP sSTEP;
        sSTEP.LOAD ( SFile );
        m_vecProgStep.push_back ( sSTEP );
    }

    //	완료시 보상.
    SFile >> m_dwGiftMONEY;
    SFile >> m_lnGiftEXP;	
    SFile >> m_dwGiftSKILLPOINT;
    SFile >> m_dwGiftSTATSPOINT;
    SFile >> m_dwGiftLIFEPOINT;
    SFile >> m_dwGiftDisPK;
    SFile >> m_dwGiftELEMENT;

    SFile >> m_dwGiftQUEST;

    // 아이템 정보가 바뀌면 작업을 해야된다.
    typedef std::vector<SITEMCUSTOM_116> INVENQARRAY_OLD;
    typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

    INVENQARRAY_OLD vecGIFT_QITEM;
    SFile >> vecGIFT_QITEM;
    {
        m_vecGiftITEM.clear();
        m_vecGiftITEM.reserve ( vecGIFT_QITEM.size() );

        INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
        INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
        for ( ; iter!=iter_end; ++iter )
        {
            SITEMCUSTOM sCUSTOM;
            sCUSTOM.Assign ( (*iter) );
            m_vecGiftITEM.push_back ( sCUSTOM );
        }
    }
    // 작업 끝
    SFile >> m_vecGiftSKILL;


    SFile >> m_ProgressEvent.bUseProgressEvent;

    if( m_ProgressEvent.bUseProgressEvent )
    {
        SFile >> m_ProgressEvent.dwStartProgress;
        SFile >> m_ProgressEvent.dwEndProgress;
        SFile >> m_ProgressEvent.dwFailProgress;

        SFile >> m_ProgressEvent.dwfailType;

        SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.startMap.wPosX;
        SFile >> m_ProgressEvent.startMap.wPosY;
        SFile >> m_ProgressEvent.startMap.dwGateID;

        SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.endMap.wPosX;
        SFile >> m_ProgressEvent.endMap.wPosY;
        SFile >> m_ProgressEvent.endMap.dwGateID;

        SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
        SFile >> m_ProgressEvent.failMap.wPosX;
        SFile >> m_ProgressEvent.failMap.wPosY;
        SFile >> m_ProgressEvent.failMap.dwGateID;
    }

    return true;
}

bool GLQUEST::LOAD_0022 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector<SITEMCUSTOM_117> INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGiftITEM.clear();
		m_vecGiftITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign ( (*iter) );
			m_vecGiftITEM.push_back ( sCUSTOM );
		}
	}
	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0023 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	
	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector<SITEMCUSTOM_118> INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator  INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGiftITEM.clear();
		m_vecGiftITEM.reserve ( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign ( (*iter) );
			m_vecGiftITEM.push_back ( sCUSTOM );
		}
	}
	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0024 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_119 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGiftITEM.clear();
		m_vecGiftITEM.reserve( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGiftITEM.push_back( sCUSTOM );
		}
	}

	SFile >> m_vecGiftSKILL;


	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0025( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_120 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGiftITEM.clear();
		m_vecGiftITEM.reserve( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGiftITEM.push_back( sCUSTOM );
		}
	}

	SFile >> m_vecGiftSKILL;

	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0026( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_121 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGiftITEM.clear();
		m_vecGiftITEM.reserve( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGiftITEM.push_back( sCUSTOM );
		}
	}

	SFile >> m_vecGiftSKILL;

	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0027 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_122 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGiftITEM.clear();
		m_vecGiftITEM.reserve( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGiftITEM.push_back( sCUSTOM );
		}
	}

	SFile >> m_vecGiftSKILL;
	SFile >> m_vecGiftSKILL;

	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}
bool GLQUEST::LOAD_0028 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	// 아이템 정보가 바뀌면 작업을 해야된다.
	typedef std::vector< SITEMCUSTOM_123 > INVENQARRAY_OLD;
	typedef INVENQARRAY_OLD::iterator INVENQARRAY_OLD_ITER;

	INVENQARRAY_OLD vecGIFT_QITEM;
	SFile >> vecGIFT_QITEM;
	{
		m_vecGiftITEM.clear();
		m_vecGiftITEM.reserve( vecGIFT_QITEM.size() );

		INVENQARRAY_OLD_ITER iter = vecGIFT_QITEM.begin();
		INVENQARRAY_OLD_ITER iter_end = vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEMCUSTOM sCUSTOM;
			sCUSTOM.Assign( ( *iter ) );
			m_vecGiftITEM.push_back( sCUSTOM );
		}
	}

	SFile >> m_vecGiftSKILL;
	SFile >> m_vecGiftSKILL;

	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

bool GLQUEST::LOAD_0029 ( sc::BaseStream &SFile )
{
	DWORD dwSize(0);

	SFile >> m_dwEDITVER;

	SFile >> m_strTITLE;
	SFile >> m_strCOMMENT;

	SFile >> m_dwFlags;

	SFile >> m_dwQuestArea;

	m_sSTARTOPT.LOAD ( SFile );
	m_sAttendanceOption.LOAD ( SFile );

	//	최초 부여 조건.
	SFile >> m_dwBeginPartyMemNum;
	SFile >> m_dwBeginMoney;

	//	진행 조건.
	SFile >> m_dwLimitTime;
	SFile >> m_bTimeLimiorKeep;
	SFile >> m_bPastOnlyOnOline;
	SFile >> m_bNonDie;
	SFile >> m_dwLimitPartyMemNum;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		GLQUEST_STEP sSTEP;
		sSTEP.LOAD ( SFile );
		m_vecProgStep.push_back ( sSTEP );
	}

	//	완료시 보상.
	SFile >> m_dwGiftMONEY;
	SFile >> m_lnGiftEXP;	
	SFile >> m_dwGiftSKILLPOINT;
	SFile >> m_dwGiftSTATSPOINT;
	SFile >> m_dwGiftLIFEPOINT;
	SFile >> m_dwGiftDisPK;
	SFile >> m_dwGiftELEMENT;

	SFile >> m_dwGiftQUEST;

	SFile >> m_vecGiftITEM;
	SFile >> m_vecGiftSKILL;

	SFile >> m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile >> m_ProgressEvent.dwStartProgress;
		SFile >> m_ProgressEvent.dwEndProgress;
		SFile >> m_ProgressEvent.dwFailProgress;

		SFile >> m_ProgressEvent.dwfailType;

		SFile >> m_ProgressEvent.startMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.startMap.wPosX;
		SFile >> m_ProgressEvent.startMap.wPosY;
		SFile >> m_ProgressEvent.startMap.dwGateID;

		SFile >> m_ProgressEvent.endMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.endMap.wPosX;
		SFile >> m_ProgressEvent.endMap.wPosY;
		SFile >> m_ProgressEvent.endMap.dwGateID;

		SFile >> m_ProgressEvent.failMap.nidMAP.dwID;
		SFile >> m_ProgressEvent.failMap.wPosX;
		SFile >> m_ProgressEvent.failMap.wPosY;
		SFile >> m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}


bool GLQUEST::LOAD(const std::string& FileName )
{
	std::string strLIST_FILE(GLQuestMan::GetInstance().GetPath());
    strLIST_FILE.append(FileName);

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseLogicZipFile(), 
            GLogicData::GetInstance().GetQuestZipFile(),
            strLIST_FILE.c_str(), 
            FileName, 
            true,
            GLOGIC::UseLogicPackFile()));

	if (!pBStream.get())
    {
        sc::writeLogError(
            sc::string::format(
                "GLQUEST::LOAD %1%", strLIST_FILE));
		return false;
    }

	sc::BaseStream& SFile = *pBStream;

	DWORD dwVer(0);
	SFile >> dwVer;


    //////////////////////////////////////////////////////////////////////////
    // 이 경고를 보고 있다면 [퀘스트] 정보를 재대로 로드 할수 없다.
    //
    //이유는 다음과 같다.
    //	case VERSION:	최신 로드 함수의
    // 코드 후반부에 보면 다음과 같은 코드가 있는데
    // SFile >> m_vecGIFT_QITEM;
    //
    // m_vec의 원형을 보게 되면 SITEMCUSTOM아이템이 VECTOR로 되어있는걸 알 수 있다.
    // 즉 SITEMCUSTOM의 버전은 올라가게 되었는데. 
    // 퀘스트의 SITEMCUSTOM의 버전은 이전버전을 읽고있다는 말이다.
    // 
    // 해경 방법
    //퀘스트의 버전을 올려야 한다.
    // 
    // 더욱 열받는건 이 부분이 퀘스트에 2개나 있다.
    // 다른 부분도 찾아서 수정해야 한다.
    //

    // 그리고 다음 사람을 위해서 이 버전을 아이템 커스텀 버전과 맞춰놓아라.
    GASSERT( SITEMCUSTOM::VERSION == 0x0125 );

	switch (dwVer)
	{
	case 0x0001:	LOAD_0001(SFile);	break;
	case 0x0002:	LOAD_0002(SFile);	break;
	case 0x0003:	LOAD_0003(SFile);	break;
	case 0x0004:	LOAD_0004(SFile);	break;
	case 0x0005:	LOAD_0005(SFile);	break;
	case 0x0006:	LOAD_0006(SFile);	break;
	case 0x0007:	LOAD_0007(SFile);	break;
	case 0x0008:	LOAD_0008(SFile);	break;
	case 0x0009:	LOAD_0009(SFile);	break;
	case 0x0010:	LOAD_0010(SFile);	break;
	case 0x0011:	LOAD_0011(SFile);	break;
	case 0x0012:	LOAD_0012(SFile);	break;
	case 0x0013:	LOAD_0013(SFile);	break;
	case 0x0014:	LOAD_0014(SFile);	break;
    case 0x0015:    LOAD_0015(SFile);   break;
    case 0x0016:    LOAD_0016(SFile);   break;
	case 0x0017:	LOAD_0017(SFile);	break;
	case 0x0018:	LOAD_0018(SFile);	break;
    case 0x0019:	LOAD_0019(SFile);	break;
    case 0x0020:	LOAD_0020(SFile);	break;
	case 0x0021:	LOAD_0021(SFile);	break;
	case 0x0022:	LOAD_0022(SFile);	break;
	case 0x0023:	LOAD_0023(SFile);	break;
	case 0x0024:	LOAD_0024(SFile);	break;
	case 0x0025:	LOAD_0025(SFile);	break;
	case 0x0026:	LOAD_0026(SFile);	break;
	case 0x0027:	LOAD_0027(SFile);	break;
	case 0x0028:	LOAD_0028(SFile);	break;
	case VERSION:	LOAD_0029(SFile);	break;
	
	default:
		GASSERT(0&&"알수 없는 버전의 퀘스트 파일입니다.");
		return false;
	};

	return true;
}

namespace QUEST
{
	int iCsvCur = 0;
}

void GLQUEST::LoadCsv( CStringArray &StrArray, CStringArray &StrArrayProgress )
{
	QUEST::iCsvCur = 0;
	LoadCsvInfoDate( StrArray );
	LoadCsvBaseData( StrArray, QUEST::iCsvCur );
	// Quest csv load는 데이터 일괄 삭제 이후 새로 초기화하는 방법이아니다;
	// 아래 함수는 그렇게 구현했다...;
	// 특정 데이터는 그대로 유지하고 일부만 갱신하는 용도로 csv를 사용하는거같다;
	// LoadCsvProgress( StrArrayProgress, QUEST::iCsvCur );
}

void GLQUEST::LoadCsvInfoDate( CStringArray &StrArray )
{
	return;
}

void GLQUEST::LoadCsvBaseData( CStringArray &StrArray, INT& iCsvCur )
{
    iCsvCur = 2; // Note : InfoData에 정보가 있는 관계로 2부터 읽어 들어야 한다. by luxes.

    m_dwBeginMoney = (DWORD) atoi( StrArray[iCsvCur++] );
	m_dwLimitPartyMemNum = (DWORD) atoi( StrArray[iCsvCur++] );
	m_dwBeginPartyMemNum = (DWORD) atoi( StrArray[iCsvCur++] );

    m_dwFlags = (DWORD)atoi( StrArray[iCsvCur++] ); // 퀘스트 수행 특성

    m_dwQuestArea = (DWORD)atoi( StrArray[iCsvCur++] );

    m_dwLimitTime = (DWORD) atoi( StrArray[iCsvCur++] ); // 제한시간
    m_sSTARTOPT.dwCLASS = (DWORD) atoi( StrArray[iCsvCur++] ); // 클래스조건

    int nSchool = atoi( StrArray[iCsvCur++] ); // 학원 조건 

    if ( nSchool == 0 ) m_sSTARTOPT.wSCHOOL = 10; // 전체
    else if ( nSchool == 1 ) m_sSTARTOPT.wSCHOOL = 0; // 성문
    else if ( nSchool == 2 ) m_sSTARTOPT.wSCHOOL = 1; // 현암
    else if ( nSchool == 3 ) m_sSTARTOPT.wSCHOOL = 2; // 봉황

    // 퀘스트 조건
    m_sSTARTOPT.nidITEM.wMainID = (WORD)atoi( StrArray[iCsvCur++] );
    m_sSTARTOPT.nidITEM.wSubID = (WORD)atoi( StrArray[iCsvCur++] );
    m_sSTARTOPT.nidSKILL.wMainID = (WORD)atoi( StrArray[iCsvCur++] );
    m_sSTARTOPT.nidSKILL.wSubID = (WORD)atoi( StrArray[iCsvCur++] );
    m_sSTARTOPT.wLEVEL = (WORD)atoi( StrArray[iCsvCur++] );

    m_lnGiftEXP = (LONGLONG)_atoi64( StrArray[iCsvCur++] );
    m_dwGiftDisPK = (DWORD)atoi( StrArray[iCsvCur++] ); 
    m_dwGiftMONEY = (DWORD)atoi( StrArray[iCsvCur++] );
    m_dwGiftSKILLPOINT = (DWORD)atoi( StrArray[iCsvCur++] );
    m_dwGiftELEMENT = (DWORD)atoi( StrArray[iCsvCur++] );
    m_dwGiftSTATSPOINT = (DWORD)atoi( StrArray[iCsvCur++] );
    m_dwGiftLIFEPOINT = (DWORD)atoi( StrArray[iCsvCur++] );
    m_dwGiftQUEST = (DWORD)atoi( StrArray[iCsvCur++] );

    // 보상 아이템
    const int nItemMAX = 10;
    const int nSkillMAX = 6;

    m_vecGiftITEM.clear();	

	int iTempCsvCur = 23;
	for( int i=0; i<nItemMAX; ++i )
	{
		if( StrArray[(iTempCsvCur)+(i*10)] == " " ) break;

		WORD wMID = (WORD)atoi( StrArray[(iTempCsvCur)+(i*10) ] );	iCsvCur++;
		WORD wSID = (WORD)atoi( StrArray[(iTempCsvCur+1)+(i*10) ] );	iCsvCur++;

		SITEMCUSTOM sItemCustom( SNATIVEID(wMID,wSID) );
		sItemCustom.cDAMAGE = (BYTE) atoi( StrArray[(iTempCsvCur+2)+(i*10) ] );			iCsvCur++;
		sItemCustom.cDEFENSE = (BYTE)atoi( StrArray[(iTempCsvCur+3)+(i*10) ] );			iCsvCur++;
		sItemCustom.wTurnNum = (WORD)atoi( StrArray[(iTempCsvCur+4)+(i*10) ] );			iCsvCur++;
		sItemCustom.cRESIST_ELEC = (BYTE)atoi( StrArray[(iTempCsvCur+5)+(i*10) ] );		iCsvCur++;
		sItemCustom.cRESIST_FIRE = (BYTE)atoi( StrArray[(iTempCsvCur+6)+(i*10) ] );		iCsvCur++;
		sItemCustom.cRESIST_ICE = (BYTE)atoi( StrArray[(iTempCsvCur+7)+(i*10) ] );		iCsvCur++;
		sItemCustom.cRESIST_POISON = (BYTE)atoi( StrArray[(iTempCsvCur+8)+(i*10) ] );	iCsvCur++;
		sItemCustom.cRESIST_SPIRIT = (BYTE)atoi( StrArray[(iTempCsvCur+9)+(i*10) ] );	iCsvCur++;

		m_vecGiftITEM.push_back(sItemCustom);
	}

	SNATIVEID sNativeID;

	m_vecGiftSKILL.clear();

	iTempCsvCur = 123;
	for( int i=0; i<nSkillMAX; ++i )
	{
		if (StrArray[(iTempCsvCur)+(i*2)] == " " ) break;

		sNativeID.wMainID = (WORD)atoi( StrArray[(iTempCsvCur)+(i*2) ] );  iCsvCur++;
		sNativeID.wSubID = (WORD)atoi( StrArray[(iTempCsvCur+1)+(i*2) ] ); iCsvCur++;
		m_vecGiftSKILL.push_back( sNativeID.dwID );
	}
	
	iCsvCur = 135;
}

#define GetCsvInt atoi(StrArray[iCsvCur++])
#define GetCsvFlt atof(StrArray[iCsvCur++])

void GLQUEST::LoadCsvProgress( CStringArray &StrArray, INT& iCsvCur )
{
	const int nStepMAX(30), nValidStepSize(GetCsvInt), nOffset_AtStep(136);
	for ( INT i = 0; i < nValidStepSize; ++i )
	{
		GLQUEST_STEP sQuestStep;
		iCsvCur = nOffset_AtStep + (i * 136);
		sQuestStep.m_dwNID_NPCTALK = SNATIVEID(GetCsvInt, GetCsvInt).dwID;
		sQuestStep.m_dwNID_MOBKILL = SNATIVEID(GetCsvInt, GetCsvInt).dwID;
		sQuestStep.m_dwNUM_MOBKILL = GetCsvInt;
		sQuestStep.m_sMAPID_REACH =  SNATIVEID(GetCsvInt, GetCsvInt);
		sQuestStep.m_wPOSX_REACH = GetCsvInt;
		sQuestStep.m_wPOSY_REACH = GetCsvInt;
		sQuestStep.m_wRADIUS_REACH = GetCsvInt;

		const INT nOffset_AtQItem(iCsvCur);
		const INT nQItemSize(GetCsvInt);
		for ( INT j = 0; j < nQItemSize; ++j )	// 125, GLQUEST_STEP::NEEDQITEM_MAX;
		{
			iCsvCur = nOffset_AtQItem + (j * (25));
			SGENQUESTITEM sQITEM;
			sQITEM.sNID = SNATIVEID(GetCsvInt, GetCsvInt);
			sQITEM.wNUM = GetCsvInt;
			sQITEM.fGEN_RATE = GetCsvFlt;
			const INT nGenMobSize(GetCsvInt);
			for ( INT k = 0; k < nGenMobSize; ++k )	// 20,  SGENQUESTITEM::GENMOB_MAX
				sQITEM.GENMOB_PUSH(SNATIVEID(GetCsvInt, GetCsvInt).dwID);

			sQuestStep.m_vecMOBGEN_QITEM.push_back(sQITEM);
		}
		m_vecProgStep.push_back(sQuestStep);
	}

	iCsvCur = nOffset_AtStep + (nStepMAX*136);

	return;
}

bool GLQUEST::SAVE ( const char *szFile )
{
	std::string strLIST_FILE = std::string(GLQuestMan::GetInstance().GetPath()) + szFile;

	sc::SerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_WRITE, strLIST_FILE.c_str(), true );
	if ( !bOPEN )					
		return false;

	SFile << (DWORD) VERSION;

	SFile << m_dwEDITVER;

	SFile << m_strTITLE;
	SFile << m_strCOMMENT;

	SFile << m_dwFlags;

    SFile << m_dwQuestArea;

	m_sSTARTOPT.SAVE ( SFile );
	m_sAttendanceOption.SAVE( SFile );

	//	최초 부여 조건.
	SFile << m_dwBeginPartyMemNum;
	SFile << m_dwBeginMoney;

	//	진행 조건.
	SFile << m_dwLimitTime;
	SFile << m_bTimeLimiorKeep;
	SFile << m_bPastOnlyOnOline;
	SFile << m_bNonDie;
	SFile << m_dwLimitPartyMemNum;


	SFile << (DWORD) m_vecProgStep.size();
	VECQSTEP_ITER iter = m_vecProgStep.begin();
	VECQSTEP_ITER iter_end = m_vecProgStep.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).SAVE ( SFile );
	}

	//	완료시 보상.
	SFile << m_dwGiftMONEY;
	SFile << m_lnGiftEXP;
	SFile << m_dwGiftSKILLPOINT;
	SFile << m_dwGiftSTATSPOINT;
	SFile << m_dwGiftLIFEPOINT;
	SFile << m_dwGiftDisPK;
	SFile << m_dwGiftELEMENT;

	SFile << m_dwGiftQUEST;
	SFile << m_vecGiftITEM;
	SFile << m_vecGiftSKILL;

	SFile << m_ProgressEvent.bUseProgressEvent;

	if( m_ProgressEvent.bUseProgressEvent )
	{
		SFile << m_ProgressEvent.dwStartProgress;
		SFile << m_ProgressEvent.dwEndProgress;
		SFile << m_ProgressEvent.dwFailProgress;

		SFile << m_ProgressEvent.dwfailType;

		SFile << m_ProgressEvent.startMap.nidMAP.dwID;
		SFile << m_ProgressEvent.startMap.wPosX;
		SFile << m_ProgressEvent.startMap.wPosY;
		SFile << m_ProgressEvent.startMap.dwGateID;

		SFile << m_ProgressEvent.endMap.nidMAP.dwID;
		SFile << m_ProgressEvent.endMap.wPosX;
		SFile << m_ProgressEvent.endMap.wPosY;
		SFile << m_ProgressEvent.endMap.dwGateID;

		SFile << m_ProgressEvent.failMap.nidMAP.dwID;
		SFile << m_ProgressEvent.failMap.wPosX;
		SFile << m_ProgressEvent.failMap.wPosY;
		SFile << m_ProgressEvent.failMap.dwGateID;
	}

	return true;
}

void GLQUEST::SaveCsvHead( std::fstream &SFile, CString strFileName )
{
	SaveCsvInfoHead( SFile, strFileName );
	SaveCsvBaseHead( SFile );
	SaveCsvProgressBaseHead( SFile );	
}

void GLQUEST::SaveCsv( std::fstream &SFile, CString strFileName )
{
	//SaveCsvInfoHead( SFile, strFileName );
	//SaveCsvBaseHead( SFile );
	//SaveCsvProgressBaseHead( SFile );	
	std::string strTemp;

	strTemp = strFileName.GetString();
	STRUTIL::OutputStrCsv( SFile, strTemp );

	SaveCsvInfoData( SFile );
	SaveCsvBaseData( SFile );
	SaveCsvProgressBaseData( SFile );

	//SaveCsvProgress( SFile );
}

void GLQUEST::SaveCsvInfoHead( std::fstream &SFile,  CString strFileName)
{
	//std::string strTemp;

	//strTemp = strFileName.GetString();
	//STRUTIL::OutputStrCsv( SFile, strTemp );
	SFile << "   " << ",";
	SFile << "Quest_ID" << ",";
}

void GLQUEST::SaveCsvBaseHead( std::fstream &SFile )
{
	SFile << "ConditionMoney" << ",";
	SFile << "MaximumPlayer" << ",";
	SFile << "MinimumPlayer" << ",";
	SFile << "BasicActionCondition" << ",";
    SFile << "QuestArea" << ",";
	SFile << "ConditionTime" << ",";
	SFile << "UsedCharClass" << ",";
	SFile << "UsedSchool" << ",";	
	
	SFile << "ConditionItemMid" << ",";
	SFile << "ConditionItemSid" << ",";
	SFile << "ConditionSkillMid" << ",";
	SFile << "ConditionSkillSid" << ",";
	SFile << "ConditionLevel" << ",";

	SFile << "CompensationExp" << ",";
	SFile << "CompensationPK" << ",";
	SFile << "CompensationMoney" << ",";
	SFile << "CompensationSkillPoint" << ",";
	SFile << "CompensationAttribute" << ",";
	SFile << "CompensationStatsPoint" << ",";
	SFile << "CompensationLiving" << ",";
	SFile << "ConnectionNextQuest";	// 20
	
	CString strTemp;
	const int nItemMAX = 10;
	const int nSkillMAX = 6;

	for ( int i = 0 ; i < nItemMAX; ++i ) // 100
	{
		SFile << ",";
		strTemp.Format( "%d" , i );
		SFile << "CompensationItemMid" << strTemp << ",";
		SFile << "CompensationItemSid" << strTemp << ",";
		SFile << "InchantDamage" << strTemp << ",";
		SFile << "InchantDefense" << strTemp << ",";
		SFile << "TrunNum" << strTemp << ",";
		SFile << "ResistElectric" << strTemp << ",";
		SFile << "ResistFire" << strTemp << ",";
		SFile << "ResistIce" << strTemp << ",";
		SFile << "ResistPoison" << strTemp << ",";
		SFile << "ResistSpirit" << strTemp;
	}	

	for ( int i = 0; i < nSkillMAX; ++ i ) // 12
	{
		SFile << ",";
		strTemp.Format( "%d", i );
		SFile << "CompensationSkillMid" << strTemp << ",";
		SFile << "CompensationSkillSid" << strTemp;
	}

    SFile << ",";
	//SFile << std::endl;
}

void GLQUEST::SaveCsvInfoData( std::fstream &SFile )
{
	//SFile << " " << "," ;
	SFile << m_sNID.dwID << ",";
}

void GLQUEST::SaveCsvBaseData( std::fstream &SFile )
{
	SFile << m_dwBeginMoney << ",";
	SFile << m_dwLimitPartyMemNum << ",";
	SFile << m_dwBeginPartyMemNum << ",";
	SFile << m_dwFlags << ",";
    SFile << m_dwQuestArea << ",";
	SFile << m_dwLimitTime << ",";
	SFile << m_sSTARTOPT.dwCLASS << ",";
	
	
	int nSchool = 0; // 학원 조건 
	if ( m_sSTARTOPT.wSCHOOL == 10 ) nSchool = 0; // 전체
	else if ( m_sSTARTOPT.wSCHOOL == 0 ) nSchool = 1; // 성문
	else if ( m_sSTARTOPT.wSCHOOL == 1 ) nSchool = 2; // 현암
	else if ( m_sSTARTOPT.wSCHOOL == 2 ) nSchool = 3; // 봉황

	// 퀘스트 조건
    SFile << nSchool << ",";
	SFile << m_sSTARTOPT.nidITEM.wMainID << ",";
	SFile << m_sSTARTOPT.nidITEM.wSubID << ",";
	SFile << m_sSTARTOPT.nidSKILL.wMainID << ",";
	SFile << m_sSTARTOPT.nidSKILL.wSubID << ",";
	SFile << m_sSTARTOPT.wLEVEL << ",";

	// 보상
	SFile << m_lnGiftEXP << ",";
	SFile << m_dwGiftDisPK << ",";
	SFile << m_dwGiftMONEY << ",";
	SFile << m_dwGiftSKILLPOINT << ",";
	SFile << m_dwGiftELEMENT << ",";
	SFile << m_dwGiftSTATSPOINT << ",";	
	SFile << m_dwGiftLIFEPOINT << ",";	
	int nTemp = (int) m_dwGiftQUEST;
	SFile << nTemp;


	// 보상 아이템
	int iGiftItem = (int)m_vecGiftITEM.size();
	const int nItemMAX = 10;
	const int nSkillMAX = 6;

	for( int i=0; i<nItemMAX; ++i )
	{
		SFile << ",";
		if ( i < iGiftItem )
		{
			SFile << m_vecGiftITEM[i].GetNativeID().wMainID << ",";
			SFile << m_vecGiftITEM[i].GetNativeID().wSubID << ",";

			SFile << (int)m_vecGiftITEM[i].cDAMAGE << ",";
			SFile << (int)m_vecGiftITEM[i].cDEFENSE << ",";
			SFile << m_vecGiftITEM[i].wTurnNum << ",";
	        
			SFile << (int)m_vecGiftITEM[i].cRESIST_ELEC << ",";
			SFile << (int)m_vecGiftITEM[i].cRESIST_FIRE << ",";
			SFile << (int)m_vecGiftITEM[i].cRESIST_ICE << ",";
			SFile << (int)m_vecGiftITEM[i].cRESIST_POISON << ",";
			SFile << (int)m_vecGiftITEM[i].cRESIST_SPIRIT;
		}
		else
		{
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " " << ",";
			SFile << " ";
		}
	}

	// 보상 스킬
	int iGiftSkill = (int)m_vecGiftSKILL.size();
	SNATIVEID sNativeID;

	for( int i=0; i<nSkillMAX; ++i )
	{
		SFile << ",";
		if ( i < iGiftSkill ) 
		{
			sNativeID = m_vecGiftSKILL[i];
			SFile << sNativeID.wMainID << ",";
			SFile << sNativeID.wSubID;
		}
		else
		{
			SFile << " " << ",";
			SFile << " ";
		}
	}

    SFile << ",";
	//SFile << std::endl;
}

void GLQUEST::SaveCsvProgress( std::fstream &SFile )
{
	SaveCsvProgressBaseHead( SFile );	
	SaveCsvProgressBaseData( SFile );
}

void GLQUEST::SaveCsvProgressBaseHead( std::fstream &SFile )
{
	int iProgStep = (int)m_vecProgStep.size();
	const int nStepMAX = 30;

	SFile << "ProgStepCnt";

	CString strTemp;

	for ( int i = 0; i < nStepMAX; ++i ) 
	{
		//if ( i < iProgStep )
		{
			strTemp.Format("%d",i);
			SFile << ",";
			SFile << "NPC_MID" << strTemp << ",";
			SFile << "NPC_SID" << strTemp << ",";
			SFile << "MOB_MID" << strTemp << ",";
			SFile << "MOB_SID" << strTemp << ",";
			SFile << "MOB_COUNT" << strTemp << ",";
			SFile << "MAP_MID" << strTemp << ",";
			SFile << "MAP_SID" << strTemp << ",";
            SFile << "ReachPosX" << strTemp << ",";
            SFile << "ReachPosY" << strTemp << ",";
			SFile << "MAP_RADIUS" << strTemp << ",";
			SFile << "TAKEITEM_SIZE" << strTemp;

			for ( INT j = 0; j < GLQUEST_STEP::NEEDQITEM_MAX; ++j )
			{
				strTemp.Format("%d", j);
				SFile << ",TakeItemMID_" << strTemp << ",TakeItemSID_" << strTemp;
				SFile << ",TakeItemCount_" << strTemp;
				SFile << ",GenRate_" << strTemp;
				SFile << ",GenMobCount_" << strTemp;
				for ( INT k = 0; k < SGENQUESTITEM::GENMOB_MAX; ++k )
				{
					strTemp.Format("%d", k);
					SFile << ",ItemGenMobMID_" << strTemp << ",ItemGenMobSID_" << strTemp;
				}
			}
		}
	}

    SFile << ",";
	SFile << std::endl;
}

void GLQUEST::SaveCsvProgressBaseData( std::fstream &SFile )
{
	int iProgStep = (int)m_vecProgStep.size();
	const int nStepMAX = 30;

	SFile << iProgStep;

	CString strTemp;

	for ( int i = 0; i < nStepMAX; ++i ) 
	{
		if ( i < iProgStep )
		{
			SNATIVEID sNpcNID(m_vecProgStep[i].m_dwNID_NPCTALK)
				, sMobNID(m_vecProgStep[i].m_dwNID_MOBKILL)
				, sMapNID(m_vecProgStep[i].m_sMAPID_REACH);

			strTemp.Format("%d",i);
			SFile << ",";
			SFile << sNpcNID.Mid() << "," << sNpcNID.Sid() << ",";
			SFile << sMobNID.Mid() << "," << sMobNID.Sid() << ",";
			SFile << m_vecProgStep[i].m_dwNUM_MOBKILL << ",";
			SFile << sMapNID.Mid() << "," << sMapNID.Sid() << ",";

            if(m_vecProgStep[i].IsNEED_REACHZONE())
            {
                SFile << m_vecProgStep[i].m_wPOSX_REACH << ",";
                SFile << m_vecProgStep[i].m_wPOSY_REACH << ",";
            }
            else
            {
                SFile << " " << ",";
                SFile << " " << ",";
            }

			SFile << m_vecProgStep[i].m_wRADIUS_REACH << ",";

			const INT nQItemSize(m_vecProgStep[i].m_vecMOBGEN_QITEM.size());
			SFile << nQItemSize;

			for ( INT j = 0; j < GLQUEST_STEP::NEEDQITEM_MAX; ++j )
			{
				if ( j < nQItemSize )
				{
					const SGENQUESTITEM& sQITEM = m_vecProgStep[i].m_vecMOBGEN_QITEM[j];
					SFile << "," << sQITEM.sNID.Mid();
					SFile << "," << sQITEM.sNID.Sid();
					SFile << "," << sQITEM.wNUM;
					SFile << "," << sQITEM.fGEN_RATE;
					SFile << "," << (INT)sQITEM.vecGEN_MOB.size();
					for ( INT k = 0; k < SGENQUESTITEM::GENMOB_MAX; ++k )
					{
						if ( k < (INT)sQITEM.vecGEN_MOB.size() )
						{
							SNATIVEID sNID(sQITEM.vecGEN_MOB[k]);
							SFile << "," << sNID.Mid() << "," << sNID.Sid();
						}
						else
							SFile << ", , ";
					}
				}
				else
				{
					SFile << ", , , , , ";
					for ( INT k = 0; k < SGENQUESTITEM::GENMOB_MAX; ++k )
						SFile << ", , ";
				}
				//m_vecProgStep[i].m_vecMOBGEN_QITEM
			}
		}
		else
		{
			SFile << ", , , , , , , , , , , ";
			for ( INT j = 0; j < GLQUEST_STEP::NEEDQITEM_MAX; ++j )
			{
				SFile << ", , , , , ";
				for ( INT k = 0; k < SGENQUESTITEM::GENMOB_MAX; ++k )
					SFile << ", , ";
			}
		}
	}

    SFile << ",";
	SFile << std::endl;
}

void GLQUEST::ExportText(std::fstream& SFile)
{	
    SFile << QUEST::TextExportBegin << std::endl;
    SFile << QUEST::TextExportQuestId << m_sNID.dwID << std::endl;
	ExportTextSubJect(SFile);
	ExportTextProgress(SFile);
    SFile << QUEST::TextExportEnd << std::endl;
}

void GLQUEST::ExportTextSubJect(std::fstream& SFile)
{	
    SFile << QUEST::TextExportSubject << std::endl;
	SFile << m_strTITLE << std::endl;
	SFile << QUEST::TextExportSubjectEnd << std::endl;

	SFile << QUEST::TextExportDesc << std::endl;
	SFile << m_strCOMMENT << std::endl;
	SFile << QUEST::TextExportDescEnd << std::endl;

}

void GLQUEST::ExportTextProgress( std::fstream &SFile )
{
	int iProgStep = (int) m_vecProgStep.size();	
	for (int i=0; i<iProgStep; ++i)
	{
        const GLQUEST_STEP& QuestStep = m_vecProgStep[i]; 

        SFile << QUEST::TextExportProgress << std::endl;
		SFile << QuestStep.m_strTITLE << std::endl;
        SFile << QUEST::TextExportProgressEnd << std::endl;

		SFile << QUEST::TextExportDesc << std::endl;
		SFile << QuestStep.m_strCOMMENT << std::endl;
		SFile << QUEST::TextExportDescEnd << std::endl;

        SFile << QUEST::TextExportNpcTalk << std::endl;
        SFile << QuestStep.m_strOBJ_NPCTALK << std::endl;
        SFile << QUEST::TextExportNpcTalkEnd << std::endl;

		SFile << QUEST::TextExportLevelUp << std::endl;
		SFile << QuestStep.m_strOBJ_LEVELUP << std::endl;
		SFile << QUEST::TextExportLevelUpEnd << std::endl;

		SFile << QUEST::TextExportConfting << std::endl;
		SFile << QuestStep.m_strOBJ_CONFTING << std::endl;
		SFile << QUEST::TextExportConftingEnd << std::endl;

        SFile << QUEST::TextExportItemGrinding << std::endl;
        SFile << QuestStep.m_strOBJ_ITEMGRINDING << std::endl;
        SFile << QUEST::TextExportItemGrindingEnd << std::endl;

		SFile << QUEST::TextExportItemUse << std::endl;
		SFile << QuestStep.m_strOBJ_ITEMUSE << std::endl;
		SFile << QUEST::TextExportItemUseEnd << std::endl;

		SFile << QUEST::TextExportKillMob << std::endl;
		SFile << QuestStep.m_strOBJ_MOBKILL << std::endl;
		SFile << QUEST::TextExportKillMobEnd << std::endl;

        SFile << QUEST::TextExportProtectNpc << std::endl;
        SFile << QuestStep.m_strOBJ_NPCGUARD << std::endl;
        SFile << QUEST::TextExportProtectNpcEnd << std::endl;

        SFile << QUEST::TextExportMovePos << std::endl;
        SFile << QuestStep.m_strOBJ_REACH_ZONE << std::endl;
        SFile << QUEST::TextExportMovePosEnd << std::endl;

        SFile << QUEST::TextExportGetItem << std::endl;
        SFile << QuestStep.m_strOBJ_MOBGEN_QITEM << std::endl;
        SFile << QUEST::TextExportGetItemEnd << std::endl;

        SFile << QUEST::TextExportProtectPos << std::endl;
        SFile << QuestStep.m_strOBJ_DEFENSE_ZONE << std::endl;
        SFile << QUEST::TextExportProtectPosEnd << std::endl;
	}
}

void GLQUEST::ImportText(sc::CStringFile& strFile)
{
	ImportTextSubject(strFile);
	ImportTextProgress(strFile);
}

void GLQUEST::ImportTextSubject(sc::CStringFile& strFile)
{
	CString strLine;	
    // ------------------------------------------------------------------------
	strFile.GetNextLine(strLine);			
	if (strLine != QUEST::TextExportSubject.c_str())
        return;
	m_strTITLE = "";	
	strFile.GetNextLine(strLine);	
	while (strLine != QUEST::TextExportSubjectEnd.c_str())
	{
		m_strTITLE += strLine;		
		strFile.GetNextLine(strLine);		
		if (strLine != QUEST::TextExportSubjectEnd.c_str())
            m_strTITLE += "\r\n";
	}

    // ------------------------------------------------------------------------
	strFile.GetNextLine(strLine);
	if (strLine != QUEST::TextExportDesc.c_str())
        return;
	m_strCOMMENT = "";	
	strFile.GetNextLine(strLine);
	while (strLine != QUEST::TextExportDescEnd.c_str())
    {		
		m_strCOMMENT += strLine;		
		strFile.GetNextLine(strLine);
		if (strLine != QUEST::TextExportDescEnd.c_str())
            m_strCOMMENT += "\r\n";
	}    
}

void GLQUEST::ImportTextProgress( sc::CStringFile& strFile )
{
	CString strLine;
	CString strError;
	
	strFile.GetNextLine( strLine );
	
	int index = 0;
	int nStep = (int)m_vecProgStep.size();


    while (strLine != QUEST::TextExportEnd.c_str())
	{
		if ( index >= nStep )
		{
			strError.Format("ID=%d overfull progress step", m_sNID.dwID);
			AfxMessageBox(strError,MB_OK );
			return;
		}

        GLQUEST_STEP& QuestStep = m_vecProgStep[index];

        // --------------------------------------------------------------------
        if (strLine != QUEST::TextExportProgress.c_str())
            return;
		strFile.GetNextLine(strLine);
		QuestStep.m_strTITLE = "";
        while (strLine != QUEST::TextExportProgressEnd.c_str())
		{
			QuestStep.m_strTITLE += strLine;
			strFile.GetNextLine(strLine);
			if (strLine != QUEST::TextExportProgressEnd.c_str())
                QuestStep.m_strTITLE += "\r\n";
		}
		strFile.GetNextLine(strLine); 

        // --------------------------------------------------------------------
		if (strLine != QUEST::TextExportDesc.c_str())
            return;
		strFile.GetNextLine(strLine); 
		QuestStep.m_strCOMMENT = "";
		while (strLine != QUEST::TextExportDescEnd.c_str())
		{
			QuestStep.m_strCOMMENT += strLine;
			strFile.GetNextLine(strLine);
			if (strLine != QUEST::TextExportDescEnd.c_str())
                QuestStep.m_strCOMMENT += "\r\n";
		} 
		strFile.GetNextLine(strLine);

        // ------------------------------------------------------------------------
        if (strLine != QUEST::TextExportNpcTalk.c_str())
            return;
        strFile.GetNextLine(strLine); 
        QuestStep.m_strOBJ_NPCTALK = "";
        while (strLine != QUEST::TextExportNpcTalkEnd.c_str())
        {
            QuestStep.m_strOBJ_NPCTALK += strLine;
            strFile.GetNextLine(strLine);
            if (strLine != QUEST::TextExportNpcTalkEnd.c_str())
                QuestStep.m_strOBJ_NPCTALK += "\r\n";
        } 
        strFile.GetNextLine(strLine);

        // ------------------------------------------------------------------------
        if (strLine != QUEST::TextExportKillMob.c_str())
            return;
        strFile.GetNextLine(strLine); 
        QuestStep.m_strOBJ_MOBKILL = "";
        while (strLine != QUEST::TextExportKillMobEnd.c_str())
        {
            QuestStep.m_strOBJ_MOBKILL += strLine;
            strFile.GetNextLine(strLine);
            if (strLine != QUEST::TextExportKillMobEnd.c_str())
                QuestStep.m_strOBJ_MOBKILL += "\r\n";
        } 
        strFile.GetNextLine(strLine);
		// ------------------------------------------------------------------------
		if (strLine != QUEST::TextExportLevelUp.c_str())
			return;
		strFile.GetNextLine(strLine); 
		QuestStep.m_strOBJ_LEVELUP = "";
		while (strLine != QUEST::TextExportLevelUpEnd.c_str())
		{
			QuestStep.m_strOBJ_LEVELUP += strLine;
			strFile.GetNextLine(strLine);
			if (strLine != QUEST::TextExportLevelUpEnd.c_str())
				QuestStep.m_strOBJ_LEVELUP += "\r\n";
		} 
		strFile.GetNextLine(strLine);
		// ------------------------------------------------------------------------
		if (strLine != QUEST::TextExportConfting.c_str())
			return;
		strFile.GetNextLine(strLine); 
		QuestStep.m_strOBJ_CONFTING = "";
		while (strLine != QUEST::TextExportConftingEnd.c_str())
		{
			QuestStep.m_strOBJ_CONFTING += strLine;
			strFile.GetNextLine(strLine);
			if (strLine != QUEST::TextExportConftingEnd.c_str())
				QuestStep.m_strOBJ_CONFTING += "\r\n";
		} 
		strFile.GetNextLine(strLine);
		// ------------------------------------------------------------------------
		if (strLine != QUEST::TextExportItemGrinding.c_str())
			return;
		strFile.GetNextLine(strLine); 
		QuestStep.m_strOBJ_ITEMGRINDING = "";
		while (strLine != QUEST::TextExportItemGrindingEnd.c_str())
		{
			QuestStep.m_strOBJ_ITEMGRINDING += strLine;
			strFile.GetNextLine(strLine);
			if (strLine != QUEST::TextExportItemGrindingEnd.c_str())
				QuestStep.m_strOBJ_ITEMGRINDING += "\r\n";
		} 
		strFile.GetNextLine(strLine);

		// ------------------------------------------------------------------------
		if (strLine != QUEST::TextExportItemUse.c_str())
			return;
		strFile.GetNextLine(strLine); 
		QuestStep.m_strOBJ_ITEMUSE = "";
		while (strLine != QUEST::TextExportItemUseEnd.c_str())
		{
			QuestStep.m_strOBJ_ITEMUSE += strLine;
			strFile.GetNextLine(strLine);
			if (strLine != QUEST::TextExportItemUseEnd.c_str())
				QuestStep.m_strOBJ_ITEMUSE += "\r\n";
		} 
		strFile.GetNextLine(strLine);

        // ------------------------------------------------------------------------
        if (strLine != QUEST::TextExportProtectNpc.c_str())
            return;
        strFile.GetNextLine(strLine); 
        QuestStep.m_strOBJ_NPCGUARD = "";
        while (strLine != QUEST::TextExportProtectNpcEnd.c_str())
        {
            QuestStep.m_strOBJ_NPCGUARD += strLine;
            strFile.GetNextLine(strLine);
            if (strLine != QUEST::TextExportProtectNpcEnd.c_str())
                QuestStep.m_strOBJ_NPCGUARD += "\r\n";
        } 
        strFile.GetNextLine(strLine);

        // ------------------------------------------------------------------------
        if (strLine != QUEST::TextExportMovePos.c_str())
            return;
        strFile.GetNextLine(strLine); 
        QuestStep.m_strOBJ_REACH_ZONE = "";
        while (strLine != QUEST::TextExportMovePosEnd.c_str())
        {
            QuestStep.m_strOBJ_REACH_ZONE += strLine;
            strFile.GetNextLine(strLine);
            if (strLine != QUEST::TextExportMovePosEnd.c_str())
                QuestStep.m_strOBJ_REACH_ZONE += "\r\n";
        } 
        strFile.GetNextLine(strLine);

        // ------------------------------------------------------------------------
        if (strLine != QUEST::TextExportGetItem.c_str())
            return;
        strFile.GetNextLine(strLine); 
        QuestStep.m_strOBJ_MOBGEN_QITEM = "";
        while (strLine != QUEST::TextExportGetItemEnd.c_str())
        {
            QuestStep.m_strOBJ_MOBGEN_QITEM += strLine;
            strFile.GetNextLine(strLine);
            if (strLine != QUEST::TextExportGetItemEnd.c_str())
                QuestStep.m_strOBJ_MOBGEN_QITEM += "\r\n";
        } 
        strFile.GetNextLine(strLine);

        // ------------------------------------------------------------------------
        if (strLine != QUEST::TextExportProtectPos.c_str())
            return;
        strFile.GetNextLine(strLine); 
        QuestStep.m_strOBJ_DEFENSE_ZONE = "";
        while (strLine != QUEST::TextExportProtectPosEnd.c_str())
        {
            QuestStep.m_strOBJ_DEFENSE_ZONE += strLine;
            strFile.GetNextLine(strLine);
            if (strLine != QUEST::TextExportProtectPosEnd.c_str())
                QuestStep.m_strOBJ_DEFENSE_ZONE += "\r\n";
        } 
        strFile.GetNextLine(strLine);

        // ------------------------------------------------------------------------
		++index;
	}
}


GLQUEST_STEP* GLQUEST::GetSTEP ( DWORD dwSTEP )
{
	if (dwSTEP >= m_vecProgStep.size())
        return NULL;
    else
	    return &(m_vecProgStep[dwSTEP]);
}

DWORD GLQUEST::ToUpStep(DWORD dwSTEP)
{
	if (m_vecProgStep.empty())
        return 0;
	if (dwSTEP >= m_vecProgStep.size())
        return UINT_MAX;
	if (dwSTEP < 1)
        return dwSTEP;

	GLQUEST_STEP sSTEP_A, sSTEP_B;

	sSTEP_A = m_vecProgStep[dwSTEP-1];
	sSTEP_B = m_vecProgStep[dwSTEP];

	m_vecProgStep[dwSTEP-1] = sSTEP_B;
	m_vecProgStep[dwSTEP] = sSTEP_A;

	return dwSTEP-1;
}

DWORD GLQUEST::ToDownStep ( DWORD dwSTEP )
{
	if ( m_vecProgStep.empty() )				return 0;
	if ( (dwSTEP+1) >= m_vecProgStep.size() )	return DWORD(m_vecProgStep.size()-1);

	GLQUEST_STEP sSTEP_A, sSTEP_B;

	sSTEP_A = m_vecProgStep[dwSTEP+1];
	sSTEP_B = m_vecProgStep[dwSTEP];

	m_vecProgStep[dwSTEP+1] = sSTEP_B;
	m_vecProgStep[dwSTEP] = sSTEP_A;

	return dwSTEP+1;
}

GLQUESTPROG_STEP& GLQUESTPROG_STEP::operator= ( const GLQUESTPROG_STEP &rVALUE )
{
	 m_bTALK = rVALUE.m_bTALK;
	 m_bQITEM = rVALUE.m_bQITEM;

	 m_bLEVELUP_ALL = rVALUE.m_bLEVELUP_ALL;
	 m_dwNUM_LEVELUP = rVALUE.m_dwNUM_LEVELUP;

	 m_bCONFTING_ALL = rVALUE.m_bCONFTING_ALL;
	 m_dwNUM_CONFTING = rVALUE.m_dwNUM_CONFTING;

	 m_bITEMGRINDING_ALL = rVALUE.m_bITEMGRINDING_ALL;
	 m_dwNUM_ITEMGRINDING = rVALUE.m_dwNUM_ITEMGRINDING;

	 m_bITEMUSE_ALL = rVALUE.m_bITEMUSE_ALL;
	 m_dwNUM_ITEMUSE = rVALUE.m_dwNUM_ITEMUSE;

	 m_bMOBKILL_ALL = rVALUE.m_bMOBKILL_ALL;
	 m_dwNUM_MOBKILL = rVALUE.m_dwNUM_MOBKILL;
	 m_bNPCGUARD_DEATH = rVALUE.m_bNPCGUARD_DEATH;

	 m_bREACH_ZONE = rVALUE.m_bREACH_ZONE;
	
	 m_bDEFENSE_ZONE_BEGIN = rVALUE.m_bDEFENSE_ZONE_BEGIN;
	 m_bDEFENSE_ZONE_COMPLETE = rVALUE.m_bDEFENSE_ZONE_COMPLETE;
	 m_fDEFENSE_ZONE_TIMER = rVALUE.m_fDEFENSE_ZONE_TIMER;

	 m_bLevel = rVALUE.m_bLevel;

	 return *this;
}

DWORD GLQUESTPROG_STEP::GetDataSize()
{
	DWORD		dwDataSize = 0;
	dwDataSize += sizeof( DWORD );		//VERSION;

	dwDataSize += sizeof( m_bLEVELUP_ALL );
	dwDataSize += sizeof( m_dwNUM_LEVELUP );

	dwDataSize += sizeof( m_bCONFTING_ALL);
	dwDataSize += sizeof( m_dwNUM_CONFTING);

	dwDataSize += sizeof( m_bITEMGRINDING_ALL );
	dwDataSize += sizeof( m_dwNUM_ITEMGRINDING );

	dwDataSize += sizeof( m_bITEMUSE_ALL );
	dwDataSize += sizeof( m_dwNUM_ITEMUSE );

	dwDataSize += sizeof( m_bMOBKILL_ALL );
	dwDataSize += sizeof( m_dwNUM_MOBKILL );

	dwDataSize += sizeof( m_bQITEM );
	dwDataSize += sizeof( m_bTALK );
	dwDataSize += sizeof( m_bREACH_ZONE );

	dwDataSize += sizeof( m_bLevel );

	dwDataSize += sizeof( m_bNPCGUARD_DEATH );

	dwDataSize += sizeof( m_bDEFENSE_ZONE_BEGIN );
	dwDataSize += sizeof( m_bDEFENSE_ZONE_COMPLETE );
	dwDataSize += sizeof( m_fDEFENSE_ZONE_TIMER );

	return dwDataSize;
}

BOOL GLQUESTPROG_STEP::GetData( se::SerializeWritableData& data )
{
	data << (DWORD)VERSION
		 << m_bLEVELUP_ALL
		 << m_dwNUM_LEVELUP
		 << m_bCONFTING_ALL
	     << m_dwNUM_CONFTING
		 << m_bITEMGRINDING_ALL
		 << m_dwNUM_ITEMGRINDING
		 << m_bITEMUSE_ALL
		 << m_dwNUM_ITEMUSE
		 << m_bMOBKILL_ALL
		 << m_dwNUM_MOBKILL
		 << m_bQITEM
		 << m_bTALK
		 << m_bREACH_ZONE
		 << m_bLevel
		 << m_bNPCGUARD_DEATH
		 << m_bDEFENSE_ZONE_BEGIN
		 << m_bDEFENSE_ZONE_COMPLETE
		 << m_fDEFENSE_ZONE_TIMER;

	return TRUE;
}

/**
 * 함수 수정시 연관성 있는 GetDataSize도 수정필요!!!
 */
BOOL GLQUESTPROG_STEP::SET_BYBUFFER ( se::ByteStream &ByteStream ) const
{
	ByteStream << (DWORD) VERSION;

	ByteStream << m_bLEVELUP_ALL;
	ByteStream << m_dwNUM_LEVELUP;

	ByteStream << m_bCONFTING_ALL;
	ByteStream << m_dwNUM_CONFTING;

	ByteStream << m_bITEMGRINDING_ALL;
	ByteStream << m_dwNUM_ITEMGRINDING;

	ByteStream << m_bITEMUSE_ALL;	
	ByteStream << m_dwNUM_ITEMUSE;

	ByteStream << m_bMOBKILL_ALL;
	ByteStream << m_dwNUM_MOBKILL;

	ByteStream << m_bQITEM;
	ByteStream << m_bTALK;
	ByteStream << m_bREACH_ZONE;

	ByteStream << m_bLevel;

	ByteStream << m_bNPCGUARD_DEATH;

	ByteStream << m_bDEFENSE_ZONE_BEGIN;
	ByteStream << m_bDEFENSE_ZONE_COMPLETE;
	ByteStream << m_fDEFENSE_ZONE_TIMER;

	return TRUE;
}

BOOL GLQUESTPROG_STEP::GET_BYBUFFER ( se::ByteStream &ByteStream )
{
	DWORD dwVer(0);

	ByteStream >> dwVer;

	switch ( dwVer )
	{
	case 0x0001:	LOAD_0001( ByteStream ); break;
	case 0x0002:	LOAD_0002( ByteStream ); break;
	case 0x0003:	LOAD_0003( ByteStream ); break;
	case VERSION:	LOAD_0004( ByteStream ); break;
	};

	return TRUE;
}

BOOL GLQUESTPROG_STEP::LOAD_0001( se::ByteStream &ByteStream )
{
	ByteStream >> m_bTALK;
	ByteStream >> m_bQITEM;

	ByteStream >> m_bMOBKILL_ALL;
	ByteStream >> m_dwNUM_MOBKILL;
	ByteStream >> m_bNPCGUARD_DEATH;

	ByteStream >> m_bREACH_ZONE;
	
	ByteStream >> m_bDEFENSE_ZONE_BEGIN;
	ByteStream >> m_bDEFENSE_ZONE_COMPLETE;
	ByteStream >> m_fDEFENSE_ZONE_TIMER;

	return TRUE;
}

BOOL GLQUESTPROG_STEP::LOAD_0002( se::ByteStream &ByteStream )
{
	ByteStream >> m_bTALK;
	ByteStream >> m_bQITEM;

	ByteStream >> m_bMOBKILL_ALL;
	ByteStream >> m_dwNUM_MOBKILL;
	ByteStream >> m_bNPCGUARD_DEATH;

	ByteStream >> m_bREACH_ZONE;
	
	ByteStream >> m_bDEFENSE_ZONE_BEGIN;
	ByteStream >> m_bDEFENSE_ZONE_COMPLETE;
	ByteStream >> m_fDEFENSE_ZONE_TIMER;

	ByteStream >> m_bLevel;
	return TRUE;
}

BOOL GLQUESTPROG_STEP::LOAD_0003( se::ByteStream &ByteStream )
{
	ByteStream >> m_bMOBKILL_ALL;
	ByteStream >> m_dwNUM_MOBKILL;
	
	ByteStream >> m_bQITEM;
	ByteStream >> m_bTALK;
	ByteStream >> m_bREACH_ZONE;

	ByteStream >> m_bLevel;

	ByteStream >> m_bNPCGUARD_DEATH;

	ByteStream >> m_bDEFENSE_ZONE_BEGIN;
	ByteStream >> m_bDEFENSE_ZONE_COMPLETE;
	ByteStream >> m_fDEFENSE_ZONE_TIMER;

	return TRUE;
}

BOOL GLQUESTPROG_STEP::LOAD_0004( se::ByteStream &ByteStream )
{
	ByteStream >> m_bLEVELUP_ALL;
	ByteStream >> m_dwNUM_LEVELUP;

	ByteStream >> m_bCONFTING_ALL;
	ByteStream >> m_dwNUM_CONFTING;

	ByteStream >> m_bITEMGRINDING_ALL;
	ByteStream >> m_dwNUM_ITEMGRINDING;

	ByteStream >> m_bITEMUSE_ALL;
	ByteStream >> m_dwNUM_ITEMUSE;

	ByteStream >> m_bMOBKILL_ALL;
	ByteStream >> m_dwNUM_MOBKILL;

	ByteStream >> m_bQITEM;
	ByteStream >> m_bTALK;
	ByteStream >> m_bREACH_ZONE;

	ByteStream >> m_bLevel;

	ByteStream >> m_bNPCGUARD_DEATH;

	ByteStream >> m_bDEFENSE_ZONE_BEGIN;
	ByteStream >> m_bDEFENSE_ZONE_COMPLETE;
	ByteStream >> m_fDEFENSE_ZONE_TIMER;

	return TRUE;
}

bool GLQUESTPROG_STEP::BEGIN_PROG ( const GLQUEST_STEP &sQSTEP )
{
	m_bTALK = !sQSTEP.IsNEED_NPCTALK();
	
	m_bQITEM = !sQSTEP.IsNEED_QITEM();

	m_bMOBKILL_ALL = !sQSTEP.IsNEED_MOBKILL();

	if (m_bMOBKILL_ALL)
	{
		m_bMOBKILL_ALL = !sQSTEP.IsNEED_PLAYERKILL();
	}

	m_dwNUM_MOBKILL = 0;

	m_bITEMUSE_ALL = !sQSTEP.IsNEED_ITEMUSE();

	m_dwNUM_ITEMUSE = 0;

	m_bITEMGRINDING_ALL = !sQSTEP.IsNEED_ITEMGRINDING();
	m_dwNUM_ITEMGRINDING = 0;

	m_bLEVELUP_ALL = !sQSTEP.IsNEED_LEVELUP();
	m_dwNUM_LEVELUP = 0;

	m_bCONFTING_ALL= !sQSTEP.IsNEED_CONFTING();
	m_dwNUM_CONFTING = 0;

	m_bNPCGUARD_DEATH = false;

	m_bREACH_ZONE = !sQSTEP.IsNEED_REACHZONE();

	m_bDEFENSE_ZONE_BEGIN = !sQSTEP.IsNEED_DEFENSEZONE();
	m_bDEFENSE_ZONE_COMPLETE = !sQSTEP.IsNEED_DEFENSEZONE();
	m_fDEFENSE_ZONE_TIMER = 0.0f;

	m_bLevel = !sQSTEP.IsNEED_LEVEL();

	return true;
}

void GLQUESTPROG_STEP::RESET ()
{
	m_bTALK = (true);
	m_bQITEM = (true);

	m_bLEVELUP_ALL = (true);
	m_dwNUM_LEVELUP = 0;

	m_bCONFTING_ALL = (true);
	m_dwNUM_CONFTING = (UINT_MAX);

	m_bITEMGRINDING_ALL = (true);
	m_dwNUM_ITEMGRINDING = (UINT_MAX);

	m_bITEMUSE_ALL = (true);
	m_dwNUM_ITEMUSE = (UINT_MAX);

	m_bMOBKILL_ALL = (true);
	m_dwNUM_MOBKILL = (UINT_MAX);
	m_bNPCGUARD_DEATH = (false);

	m_bREACH_ZONE = (true);

	m_bDEFENSE_ZONE_BEGIN = (true);
	m_bDEFENSE_ZONE_COMPLETE = (true);
	m_fDEFENSE_ZONE_TIMER = (FLT_MAX);

	m_bLevel = true;
}

bool GLQUESTPROG_STEP::IsCOMPLETE ()
{
	if ( !m_bTALK )
		return false;

	if ( !m_bQITEM )
		return false;

	if ( !m_bMOBKILL_ALL )
		return false;

	if ( m_bNPCGUARD_DEATH )
		return false;

	if ( !m_bREACH_ZONE )
		return false;
	
	if ( !m_bDEFENSE_ZONE_COMPLETE )
		return false;

	if ( !m_bLevel )
		return false;

	if ( !m_bITEMUSE_ALL )
		return false;

	if ( !m_bITEMGRINDING_ALL )
		return false;

	if ( !m_bCONFTING_ALL )
		return false;

	if ( !m_bLEVELUP_ALL )
		return false;

	return true;
}

std::string GLQUESTPROG_STEP::LogIsCOMPLETE()
{
	return sc::string::format("!bTALK:%1%, !bQITEM:%2%, !bMOBKILL_ALL:%3%, bNPCGUARD_DEATH:%4%, !bREACH_ZONE:%5%, !bDEFENSE_ZONE_COMPLETE:%6%, !bLevel:%7%, !bITEMUSE_ALL:%8%, !bITEMGRINDING_ALL:%9%, !bCONFTING_ALL:%10%, !bLEVELUP_ALL:%11%", m_bTALK, m_bQITEM, m_bMOBKILL_ALL, m_bNPCGUARD_DEATH, m_bREACH_ZONE, m_bDEFENSE_ZONE_COMPLETE, m_bLevel, m_bITEMUSE_ALL, m_bITEMGRINDING_ALL, m_bCONFTING_ALL, m_bLEVELUP_ALL );
}

GLQUESTPROG& GLQUESTPROG::operator= ( const GLQUESTPROG &rVALUE )
{
	m_sNID = rVALUE.m_sNID;
	m_dwEDITVER = rVALUE.m_dwEDITVER;

	m_dwFlags = rVALUE.m_dwFlags;
	m_bCOMPLETE = rVALUE.m_bCOMPLETE;
	m_dwCOUNT = rVALUE.m_dwCOUNT;
	m_fLAPSTIME = rVALUE.m_fLAPSTIME;
	
	m_sINVENTORY.Assign ( rVALUE.m_sINVENTORY );

	m_dwSTEP = rVALUE.m_dwSTEP;

	m_sSTEP_PROG = rVALUE.m_sSTEP_PROG;

	m_tStartTime = rVALUE.m_tStartTime;

	m_tEndTime = rVALUE.m_tEndTime;

	m_vectQuestStart = rVALUE.m_vectQuestStart;


	return *this;
}

DWORD GLQUESTPROG::GetDataSize()
{
	DWORD		dwDataSize = 0;
	dwDataSize += sizeof( DWORD );			// VERSION;

	dwDataSize += sizeof( m_sNID.dwID );
	dwDataSize += sizeof( m_dwEDITVER );

	dwDataSize += sizeof( m_dwFlags );

	dwDataSize += sizeof( m_bCOMPLETE );
	dwDataSize += sizeof( m_fLAPSTIME );

	dwDataSize += sizeof( m_dwCOUNT );

	dwDataSize += sizeof( m_tStartTime );

	dwDataSize += sizeof( m_tEndTime );

	//dwDataSize += sizeof(m_vectQuestStart) + sizeof(LONGLONG) * m_vectQuestStart.capacity();

	dwDataSize += sizeof(DWORD) + sizeof(LONGLONG) * m_vectQuestStart.size();

	// 아이템이 테이블로 빠지면서 퀘스트 바이너리에는 아이템 넣지 않는다.
	//dwDataSize += m_sINVENTORY.GetItemDataSize();

	dwDataSize += sizeof( m_dwSTEP );
	dwDataSize += m_sSTEP_PROG.GetDataSize();

	return dwDataSize;
} //GLQUESTPROG::GetDataSize

BOOL GLQUESTPROG::GetData( se::SerializeWritableData& data )
{
	data << (DWORD)VERSION
		 << m_sNID.dwID
		 << m_dwEDITVER
		 << m_dwFlags
		 << m_bCOMPLETE
		 << m_fLAPSTIME
		 << m_dwCOUNT
		 << m_tStartTime
		 << m_tEndTime;

	DWORD dwSize = (DWORD) m_vectQuestStart.size();
	data << dwSize;

	if (m_vectQuestStart.empty() == false)
	{
		data.Write(&m_vectQuestStart[0], 
			sizeof(LONGLONG) * m_vectQuestStart.size());
	}

	// 아이템이 테이블로 빠지면서 퀘스트 바이너리에는 아이템 넣지 않는다.
	//m_sINVENTORY.GetItemData( data );

	data << m_dwSTEP;
	m_sSTEP_PROG.GetData( data );

	return TRUE;
}

/**
 * 함수 수정시 연관성 있는 GetDataSize도 수정필요!!!
 */
BOOL GLQUESTPROG::SET_BYBUFFER ( se::ByteStream &ByteStream ) const
{
	ByteStream << (DWORD) VERSION;
	
	ByteStream << m_sNID.dwID;
	ByteStream << m_dwEDITVER;

	ByteStream << m_dwFlags;

	ByteStream << m_bCOMPLETE;
	ByteStream << m_fLAPSTIME;

	ByteStream << m_dwCOUNT;

	ByteStream << m_tStartTime;

	ByteStream << m_tEndTime;

	ByteStream << m_vectQuestStart;

	// 아이템이 테이블로 빠지면서 퀘스트 바이너리에는 아이템 넣지 않는다.
	//m_sINVENTORY.GETITEM_BYBUFFER ( ByteStream );

	ByteStream << m_dwSTEP;
	m_sSTEP_PROG.SET_BYBUFFER ( ByteStream );	

	return TRUE;
}

BOOL GLQUESTPROG::GET_BYBUFFER ( se::ByteStream &ByteStream )
{
	DWORD dwVer(0);
	ByteStream >> dwVer;

	switch ( dwVer )
	{
	case 0x0001:	LOAD_001( ByteStream ); break;
	case 0x0002:	LOAD_002( ByteStream ); break;
	case 0x0003:	LOAD_003( ByteStream ); break;
	case 0x0004:	LOAD_004( ByteStream ); break;
	case VERSION:	LOAD_005( ByteStream ); break;
	default:	break;
	}

	return TRUE;
}

BOOL GLQUESTPROG::LOAD_001 ( se::ByteStream &ByteStream )
{
	ByteStream >> m_sNID.dwID;
	ByteStream >> m_dwEDITVER;

	ByteStream >> m_dwFlags;
	ByteStream >> m_dwCOUNT;

	ByteStream >> m_bCOMPLETE;
	ByteStream >> m_fLAPSTIME;
	
	ByteStream >> m_dwSTEP;
	m_sSTEP_PROG.GET_BYBUFFER ( ByteStream );

	m_sINVENTORY.SETITEM_BYBUFFER ( ByteStream );

	return TRUE;
}

BOOL GLQUESTPROG::LOAD_002 ( se::ByteStream &ByteStream )
{
	ByteStream >> m_sNID.dwID;
	ByteStream >> m_dwEDITVER;

	ByteStream >> m_dwFlags;
	ByteStream >> m_dwCOUNT;

	ByteStream >> m_bCOMPLETE;
	ByteStream >> m_fLAPSTIME;
	
	ByteStream >> m_dwSTEP;
	
	ByteStream >> m_tStartTime;

	m_sSTEP_PROG.GET_BYBUFFER ( ByteStream );

	m_sINVENTORY.SETITEM_BYBUFFER ( ByteStream );

	return TRUE;
}


BOOL GLQUESTPROG::LOAD_003 ( se::ByteStream &ByteStream )
{
	ByteStream >> m_sNID.dwID;
	ByteStream >> m_dwEDITVER;

	ByteStream >> m_dwFlags;

	ByteStream >> m_bCOMPLETE;
	ByteStream >> m_fLAPSTIME;

	ByteStream >> m_dwCOUNT;

	ByteStream >> m_tStartTime;

	m_sINVENTORY.SETITEM_BYBUFFER ( ByteStream );
	
	ByteStream >> m_dwSTEP;

	m_sSTEP_PROG.GET_BYBUFFER ( ByteStream );

	return TRUE;
}

BOOL GLQUESTPROG::LOAD_004 ( se::ByteStream &ByteStream )
{
	ByteStream >> m_sNID.dwID;
	ByteStream >> m_dwEDITVER;

	ByteStream >> m_dwFlags;

	ByteStream >> m_bCOMPLETE;
	ByteStream >> m_fLAPSTIME;

	ByteStream >> m_dwCOUNT;

	ByteStream >> m_tStartTime;

	ByteStream >> m_tEndTime;

	ByteStream >> m_vectQuestStart;

	m_sINVENTORY.SETITEM_BYBUFFER ( ByteStream );

	ByteStream >> m_dwSTEP;

	m_sSTEP_PROG.GET_BYBUFFER ( ByteStream );

	return TRUE;
}

BOOL GLQUESTPROG::LOAD_005( se::ByteStream &ByteStream )
{
	ByteStream >> m_sNID.dwID;
	ByteStream >> m_dwEDITVER;

	ByteStream >> m_dwFlags;

	ByteStream >> m_bCOMPLETE;
	ByteStream >> m_fLAPSTIME;

	ByteStream >> m_dwCOUNT;

	ByteStream >> m_tStartTime;

	ByteStream >> m_tEndTime;

	ByteStream >> m_vectQuestStart;

	ByteStream >> m_dwSTEP;

	m_sSTEP_PROG.GET_BYBUFFER ( ByteStream );

	return TRUE;
}

bool GLQUESTPROG::START ( GLQUEST* pQUEST, GLQUESTPROG *pOLD_PROG )
{
	GASSERT(pQUEST&&"GLQUESTPROG::START");
	if ( !pQUEST )		return false;

	//	이전 시도 횟수를 가져온다.
	DWORD dwOLD_COUNT(0);
	if ( pOLD_PROG )
	{
		dwOLD_COUNT = pOLD_PROG->m_dwCOUNT;
		m_vectQuestStart.clear();

		// 기숙사 및 반복 퀘스트 문제로 인해서 접속 불가 문제 처리
		// 퀘스트 반복할때마다 시간정보 누적되지 않도록 처리. 차후에 리뉴얼 출석부 사용할 경우 고려 필요
		//m_vectQuestStart = pOLD_PROG->m_vectQuestStart;
		//dwOLD_COUNT = static_cast<DWORD>(m_vectQuestStart.size());
	}

	//	퀘스트 기본 정보 설정.
	m_sNID = pQUEST->m_sNID;
	m_dwEDITVER = pQUEST->m_dwEDITVER;

	//	퀘스트 정보 읽기를 요청.
	ReqREADING ();

	//	진행 시도 횟수 설정.
	m_dwCOUNT = dwOLD_COUNT + 1;

	m_bCOMPLETE = false;
	m_fLAPSTIME = 0.0f;

	// 퀘스트 시작시간 설정
	CTime cTime = CTime::GetCurrentTime();
	m_tStartTime = cTime.GetTime();

	//	현제 진행 퀘스트 STEP.
	m_dwSTEP = 0;

	//	퀘스트 초기 스텝 설정.
	GLQUEST_STEP *pQSTEP = pQUEST->GetSTEP ( m_dwSTEP );
	GASSERT(pQSTEP&&"pQUEST->GetSTEP");
	if ( !pQSTEP )		return false;

	//	현제 진행 정보.
	bool bOK = m_sSTEP_PROG.BEGIN_PROG ( *pQSTEP );
	if ( !bOK )			return false;

	//	인벤토리 리셋.
	m_sINVENTORY.DeleteItemAll ();

	return true;
}

bool GLQUESTPROG::DoPROGRESS( bool &_bModifyInven, GLQUEST_PROGRESS_MAP &stepMoveMap, std::vector< SINVENITEM >& vecDeleted, bool& bInserted )
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find( m_sNID.dwID );

	if ( !pQUEST )
		return false;
	
	_bModifyInven = false;
	bInserted = false;

	DWORD dwNum = pQUEST->GetSTEPNUM();

	//	현제 진행 부분이 마지막 step일때.
	if ( m_dwSTEP == ( dwNum - 1 ) )
		return false;

	if ( m_sSTEP_PROG.IsCOMPLETE() )
	{
		GLQUEST_STEP* pSTEP = pQUEST->GetSTEP( m_dwSTEP );
		GASSERT( pSTEP && "pQUEST->GetSTEP" );
		if ( !pSTEP )
			return false;

		//	퀘스트 인벤토리 정리.
		//
		{
			DWQARRAY_ITER iter = pSTEP->m_vecRESET_QITEM.begin();
			DWQARRAY_ITER iter_end = pSTEP->m_vecRESET_QITEM.end();
			for ( ; iter!=iter_end; ++iter )
			{
				_bModifyInven = true;

				SNATIVEID sNID = SNATIVEID( *iter );
				SINVENITEM* pINVEN = m_sINVENTORY.FindItem(sNID);
				if ( !pINVEN )
					continue;

				vecDeleted.push_back( *pINVEN );

				m_sINVENTORY.DeleteItem( pINVEN->wPosX, pINVEN->wPosY );
			}
		}

		//	퀘스트 인벤토리에 아이템 삽입.
		//
		{
			INVENQARRAY_ITER iter = pSTEP->m_vecGIFT_QITEM.begin();
			INVENQARRAY_ITER iter_end = pSTEP->m_vecGIFT_QITEM.end();
			for ( ; iter!=iter_end; ++iter )
			{
				_bModifyInven = true;

				SITEMCUSTOM sCUSTOM( ( *iter ) );
				sc::SeqUniqueGUID::Instance()->getGUID( sCUSTOM.guid );
				sCUSTOM.GenerateBasicStat( false );
				sCUSTOM.GenerateLinkSkill();
				sCUSTOM.GenerateAddOption();
				sCUSTOM.nidDISGUISE.wMainID = m_sNID.wMainID;
				sCUSTOM.nidDISGUISE.wSubID = m_sNID.wSubID;

				if ( m_sINVENTORY.InsertItem( sCUSTOM ) )
				{
					bInserted = true;
				}
			}
		}

		if( pSTEP->m_stepMoveMap.nidMAP.wMainID  != UINT_MAX &&
			pSTEP->m_stepMoveMap.nidMAP.wSubID   != UINT_MAX &&
			(pSTEP->m_stepMoveMap.wPosX != UINT_MAX &&
			 pSTEP->m_stepMoveMap.wPosY != UINT_MAX ||
			 pSTEP->m_stepMoveMap.dwGateID != 0 ) )
		{
			stepMoveMap = pSTEP->m_stepMoveMap;
		}

		m_dwSTEP++;
		pSTEP = pQUEST->GetSTEP ( m_dwSTEP );
		GASSERT(pSTEP&&"pQUEST->GetSTEP");
		if ( !pSTEP )			return false;

		m_sSTEP_PROG.BEGIN_PROG ( *pSTEP );

		//	퀘스트 정보 읽기를 요청.
		ReqREADING ();

		return true;
	}

	return false;
}

bool GLQUESTPROG::DoCOMPLETE( std::vector< SINVENITEM >& vecDeleted, bool bTraceQuest /*= false*/ )
{
	if ( bTraceQuest )
		sc::writeLogError( sc::string::format( "[QUEST TRACE] DoCOMPLETE : %1%", LogCheckCOMPLETE() ) );

	if ( CheckCOMPLETE() )
	{
		m_dwFlags = NULL;

		//	퀘스트 정보 읽기를 요청.
		ReqREADING ();

		m_bCOMPLETE = true;
		m_fLAPSTIME = 0.0f; //퀘스트 완료후 진행시간을 초기화?

		// 퀘스트 종료시간
		CTime cTime = CTime::GetCurrentTime();
		m_tEndTime = cTime.GetTime();
		
		// 기숙사 및 반복 퀘스트 문제로 인해서 접속 불가 문제 처리
		// 퀘스트 반복할때마다 시간정보 누적되지 않도록 처리. 차후에 리뉴얼 출석부 사용할 경우 고려 필요
		m_vectQuestStart.clear();

		// [shhan][2015.12.14] 동작되지도 않는데 쓸데 없는 push_back 으로 인해 메모리 단편화가 생기지 못하게 한다.
		//m_vectQuestStart.push_back(m_tStartTime);

		m_dwSTEP = 0;
		m_sSTEP_PROG.RESET();

		GLInventory::CELL_MAP_CITER iter = m_sINVENTORY.GetItemList()->begin();
		GLInventory::CELL_MAP_CITER iter_end = m_sINVENTORY.GetItemList()->end();
		for ( ; iter != iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = ( *iter ).second;
			if ( pInvenItem )
			{
				vecDeleted.push_back( *pInvenItem );
			}
		}

		m_sINVENTORY.DeleteItemAll();

		return true;
	}

	return false;
}

bool GLQUESTPROG::CheckCOMPLETE (bool bChangeSchool)
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( m_sNID.dwID );
	if ( !pQUEST )
		return false;
	
	DWORD dwNum = pQUEST->GetSTEPNUM();

	if ( pQUEST->m_bTimeLimiorKeep  )
		return CheckTIMEOVER() || m_bCOMPLETE;
	//퀘스트 완료후 CheckTIMEOVER() 가 호출되면 
	// m_fLAPSTIME 값이 0 기 때문에 m_bCOMPLETE와 Or연산

	if( bChangeSchool )
	{
		if( m_dwSTEP == 0 )
		{
			return m_sSTEP_PROG.IsCOMPLETE();
		}
		else
		{
			if ( m_dwSTEP==(dwNum-1) )
			{
				return m_sSTEP_PROG.IsCOMPLETE();
			}
		}
	}
	else
	{
		if ( m_dwSTEP==(dwNum-1) )
		{
			return m_sSTEP_PROG.IsCOMPLETE ();
		}
	}

	return false;
}


std::string GLQUESTPROG::LogCheckCOMPLETE ()
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( m_sNID.dwID );
	if ( !pQUEST )
		return sc::string::format("LogCheckCOMPLETE Find ERROR. QuestID %1%", m_sNID.dwID);
	
	DWORD dwNum = pQUEST->GetSTEPNUM();

	if ( pQUEST->m_bTimeLimiorKeep  )
	{
		if( CheckTIMEOVER() || m_bCOMPLETE )
			return sc::string::format("LogCheckCOMPLETE m_bTimeLimiorKeep Ture ERROR. QuestID %1%", m_sNID.dwID);
		else
			return sc::string::format("LogCheckCOMPLETE m_bTimeLimiorKeep False ERROR. QuestID %1%", m_sNID.dwID);
	}

	return m_sSTEP_PROG.LogIsCOMPLETE();
}

void GLQUESTPROG::DoGIVEUP ()
{
	m_bCOMPLETE = false;
	m_dwSTEP = 0;
	m_sSTEP_PROG.RESET();

	m_dwFlags = NULL;
	m_fLAPSTIME = 0.0f;
	m_sINVENTORY.DeleteItemAll();
}

// 반드시 완료된 퀘스트 인지 먼저 확인하고 해당 변경해야함.
void GLQUESTPROG::ChangeCompletedQuestID(DWORD dwQuestID)
{
	m_sNID.dwID = dwQuestID;
}

// 반드시 완료된 퀘스트 인지 먼저 확인하고 해당 변경해야함.
void GLQUESTPROG::ResetAndConvertProgressQuest(DWORD dwQuestID)
{
	m_sNID.dwID = dwQuestID;

	//	퀘스트 정보 읽기를 요청.
	ReqREADING ();
	
	m_bCOMPLETE = false;
	m_fLAPSTIME = 0.0f;

	// 퀘스트 시작시간 설정
	CTime cTime = CTime::GetCurrentTime();
	m_tStartTime = cTime.GetTime();
		
	m_dwSTEP = 0;
	m_sSTEP_PROG.RESET();

	//	인벤토리 리셋.
	m_sINVENTORY.DeleteItemAll ();
}


bool GLQUESTPROG::CheckTIMEOVER ()
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( m_sNID.dwID );

	if ( !pQUEST )
		return false;

	if ( pQUEST->m_dwLimitTime==0 )
		return false;

	return ( pQUEST->m_dwLimitTime*60 <= m_fLAPSTIME );
}

bool GLQUESTPROG::IsLastStep()
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( m_sNID.dwID );
	GASSERT(pQUEST);

	DWORD dwNum = pQUEST->GetSTEPNUM();

	return m_dwSTEP == (dwNum-1);
}


bool GLQUESTPROG::IsNonDIE ()
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( m_sNID.dwID );
	if ( !pQUEST )
		return false;

	return pQUEST->IsNonDIE();
}

// 임시로 설정값 사용 차후 정리 필요
int GLQUESTPROG::ConvertQuestGroup(WORD wBeforeSchool, DWORD dwClass, WORD wAfterSchool, DWORD &dwID)
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find( m_sNID.dwID);

	if ( !pQUEST )
		return -1;

	int nQuestID = GLQuestMan::GetInstance().GetConvertQuestGroup(m_sNID.dwID, wBeforeSchool, dwClass, wAfterSchool);
	dwID = (DWORD)nQuestID;
	// 그룹으로 매칭된퀘스트가 존재하는지 확인
	if( nQuestID == 65535 )
	{
		return 65535; // EMSCHOOL_CHANGE_DISGARD 
	}
	else if( nQuestID != -1 )
	{
		if( CheckCOMPLETE(true) )
		{
			// 완료 상태이면 매핑된 퀘스트 번호만 바꿔준다
			ChangeCompletedQuestID(nQuestID);
		}
		else
		{
			// 진행중인 상태라면 퀘스트 변호를 바꿔주고 진행정보를 초기화 한다.
			ResetAndConvertProgressQuest(nQuestID);
		}

		return 0; // EMSCHOOL_CHANGE_EXIST 
	}
	
	dwID = 0; // 로그용
	return -1; // EMSCHOOL_CHANGE_NOT_EXIST 
}

HRESULT GLQUESTPROG::FrameMove ( float fTime, float fElapsedTime )
{
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( m_sNID.dwID );
	if ( !pQUEST )
		return E_FAIL;

	CTime cTime = CTime::GetCurrentTime();

	if ( pQUEST->m_dwLimitTime>0 )
	{
		//클라이언트 On/Off 퀘스트 시간 증가
		if ( !pQUEST->m_bPastOnlyOnOline )
		{
			//지나간시간 = 현재시간 - 시작시간
			__time64_t	tCurTime = cTime.GetTime();
			m_fLAPSTIME = (float) tCurTime - m_tStartTime;
		}
		else
		{
			m_fLAPSTIME += 60.0f;
		}

		if ( m_fLAPSTIME < 0.0f )
			m_fLAPSTIME = 0.0f;

		//	초과되었을 경우.
		if ( m_fLAPSTIME >= pQUEST->m_dwLimitTime*60 )
			m_fLAPSTIME = float(pQUEST->m_dwLimitTime*60);
	}

	return S_OK;
}


namespace COMMENT
{
	std::string QUEST_PROGRESS[EMQP_SIZE] =
	{
		"이벤트 없음",
		"캐릭터 사망",
		"시작지점으로 이동"
		"지정된 위치로 이동"
	};

    int QUEST_AREA_NUM = 1;
    VEC_QUEST_AREA QUEST_AREA_NAME;

	std::string QUEST_ATTENDANCE_TYPE[EMATTENDANCE_TYPE_SIZE] =
	{
		"없음",
		"특정 시간동안 접속을 유지",
		"특정 지점에서 특정 횟수만큼 유저를 처치",
		"특정 몹을 특정 횟수로 처치",
		"특정 레벨을 달성" ,
		"특정 아이템을 수집",
		"특정 아이템을 특정 횟수로 사용",
		"특정 맵(혹은 특정 지점)으로 이동",
		"아이템을 특정 횟수만큼 연마",
		"다른파티와 학원 대련을 특정 횟수만큼 실시"
	};
};

bool GLQUEST_ATTENDANCE_OPTION::LOAD_0001( sc::BaseStream &SFile )
{
	SFile >> bAdditionalQuest;
	SFile >> nType;
	SFile >> wMinLevel;
	SFile >> wMaxLevel;
	SFile >> dwDayofWeek;
	SFile >> wStartYear;
 	SFile >> WStarMonth;
 	SFile >> WstarDay; 
 	SFile >> wEndYear; 
 	SFile >> WEndMonth;
 	SFile >> WEndDay;
	return true;
}

bool GLQUEST_ATTENDANCE_OPTION::LOAD( sc::BaseStream &SFile )
{
	DWORD dwVERSION(0), dwSIZE(0);

	SFile >> dwVERSION;
	SFile >> dwSIZE;

	switch ( dwVERSION )
	{
	case VERSION:	LOAD_0001 ( SFile ); break;
	default:
		{
			MessageBox ( NULL, "GLQUEST_ATTENDANCE_OPTION::LOAD(), unknown data version.", "ERROR", MB_OK );
			SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
			break;
		}
	};

	return true;
}

bool GLQUEST_ATTENDANCE_OPTION::SAVE( sc::SerialFile &SFile )
{
	SFile << (DWORD) VERSION;
	SFile.BeginBlock (); // 현재 위치에 저장 사이즈를 저장한다
	{
		SFile << bAdditionalQuest;
		SFile << nType;
		SFile << wMinLevel;
		SFile << wMaxLevel;
		SFile << dwDayofWeek;
		SFile << wStartYear;
		SFile << WStarMonth;
		SFile << WstarDay; 
		SFile << wEndYear; 
		SFile << WEndMonth;
		SFile << WEndDay;
	}
	SFile.EndBlock ();

	return true;
}
