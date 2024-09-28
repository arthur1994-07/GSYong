#include "pch.h"
#include "./ReferChar.h"

#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Skill/GLSkillChangeList.h"
#include "../../RanLogicClient/Char/GLCharClient.h"

#include "../GLGaeaClient.h"

//#include "../RanLogic/Character/GLCharData.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ReferChar
{
	const std::string g_strFileName = "ReferChar.db3";
	const int g_nVersion = 1;
	const int g_nGarbageMax = 1000;

	CClient::CClient( GLGaeaClient* pGaeaClient )
		: m_pGaeaClient( pGaeaClient )

		, m_fDelay( 0.0f )
		, m_bComplete( false )
		, m_nReferServer( 0 )
	{
		std::string strPath( sc::getAppPath() );
		strPath.append( m_pGaeaClient->GetSubPath()->Cache() );

		if( !sc::file::isExist( strPath ) )
			sc::file::createDirectory( strPath );

		strPath.append( g_strFileName );

		SQLiteOpen( strPath );
	}

	CClient::~CClient()
	{
		SQLiteClose();
	}

	void CClient::NotifyPublic( bool bPublic )
	{
		SNET_MSG_REFER_CHAR_PUBLIC sNetMsg;
		sNetMsg.m_bPublic = bPublic;
		m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
	}

	bool CClient::Request( int nServer, const std::string& strName )
	{
		if( m_fDelay > 0.0f )
		{
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::RED,
				ID2GAMEINTEXT( "REFER_CHAR_REQUEST_DELAY" ) );

			return false;
		}

		m_fDelay = GLCONST_CHAR::fReferCharDelay;
		m_bComplete = false;
		m_nReferServer = nServer;
		StringCchCopy( m_sData.m_szName, CHAR_SZNAME, strName.c_str() );

		SNET_MSG_REFER_CHAR_REQ sNetMsg;
		sNetMsg.SetTargetName( strName.c_str() );
		m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );

		//상대가 정보공개인지 온라인인지 확인하기  위해 주석
		//if( SQLiteSelectData( m_nReferServer, m_sData ) )
		//{
		//	m_bComplete = true;
		//	m_pInterface->UiRefresh( REFERCHAR_WINDOW );
		//}

		return true;
	}

	bool CClient::Select( int nServer )
	{
		GLCHARLOGIC sData;
		SCharInfoAddition sDataAddition;

		if( !SQLiteSelectData( nServer, sData, sDataAddition ) )
			return false;

		m_bComplete = true;
		m_nReferServer = nServer;
		m_sData = sData;
		m_sDataAddition = sDataAddition;

		return true;
	}

	bool CClient::Refresh()
	{
		if( m_fDelay > 0.0f )
			return false;

		if( _tcslen( m_sData.m_szName ) <= 0 )
			return false;

		m_fDelay = GLCONST_CHAR::fReferCharDelay;

		SNET_MSG_REFER_CHAR_REQ sNetMsg;
		sNetMsg.SetTargetName( m_sData.m_szName );
		m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );

		return true;
	}

	void CClient::Update( FLOAT fElapsedTime )
	{
		if( m_fDelay > 0.0f )
			m_fDelay -= fElapsedTime;
	}

	void CClient::GetNameList( int nServer, NameList& vecList )
	{
		SQLiteSelectNameList( nServer, vecList );
	}

	LuaTable CClient::GetNameListForWidget( int nServer )
	{
		NameList vecList;
		GetNameList( nServer, vecList );

		LuaTable tbDataList( GLWidgetScript::GetInstance().GetLuaState() );

		std::vector< std::string >::iterator iter = vecList.begin();
		for( int i = 1; iter != vecList.end(); ++i, ++iter )
			tbDataList.set( i, iter->c_str() );

		return tbDataList;
	}

	GLCHARLOGIC* CClient::GetCompleteData()
	{
		if( m_bComplete )
			return &m_sData;

		return NULL;
	}

	SCharInfoAddition* CClient::GetCompleteDataAddition()
	{
		if( m_bComplete )
			return &m_sDataAddition;

		return NULL;
	}

	void CClient::ReceiveMsgFb( NET_MSG_GENERIC* pMsg )
	{
		SNET_MSG_REFER_CHAR_FB* pNetMsg = (SNET_MSG_REFER_CHAR_FB*)pMsg;

		if( _tcscmp( m_sData.m_szName, pNetMsg->m_szTargetName ) != 0 )
			return;

		switch( pNetMsg->m_eResult )
		{
		case EReferSuccess:
			{
				Select( m_pGaeaClient->GetConnServerGroup() );

				for( WORD i=0; i<SLOT_TSIZE; ++i )
					m_sData.m_PutOnItems[ i ] = SITEMCUSTOM();

				m_sData.m_ExpSkills.clear();
			}
			break;

		case EReferLogout:
			{
				std::string strText = sc::string::format(
					ID2GAMEINTEXT( "REFER_CHAR_LOGOUT" ),
					pNetMsg->m_szTargetName );
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, strText.c_str() );
			}
			break;

		case EReferPrivate:
			{
				std::string strText = sc::string::format(
					ID2GAMEINTEXT( "REFER_CHAR_PRIVATE" ),
					pNetMsg->m_szTargetName );
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, strText.c_str() );
			}
			break;
		}
	}

	void CClient::ReceiveMsgPuton( NET_MSG_GENERIC* pMsg )
	{
		SNET_MSG_REFER_CHAR_PUTON* pNetMsg = (SNET_MSG_REFER_CHAR_PUTON*)pMsg;

		if( _tcscmp( m_sData.m_szName, pNetMsg->m_szTargetName ) != 0 )
			return;

		m_sData.m_dwGlobalID = pNetMsg->m_dwTargetGaeaId;
		for( WORD i=0; i<pNetMsg->m_nNum; ++i )
		{
			const SPutonItem& sPuton = pNetMsg->m_sPuton[ i ];

			if( sPuton.m_nSlot < SLOT_TSIZE )
				m_sData.m_PutOnItems[ sPuton.m_nSlot ] = sPuton.m_sItem;
		}
	}

	void CClient::ReceiveMsgSkill( NET_MSG_GENERIC* pMsg )
	{
		SNET_MSG_REFER_CHAR_SKILL* pNetMsg = (SNET_MSG_REFER_CHAR_SKILL*)pMsg;

		if( _tcscmp( m_sData.m_szName, pNetMsg->m_szTargetName ) != 0 )
			return;

		for( WORD i=0; i<pNetMsg->m_nNum; ++i )
		{
			const SCHARSKILL& sSkill = pNetMsg->m_sSkill[ i ];

			m_sData.m_ExpSkills.insert(
				std::make_pair( sSkill.sNativeID.dwID, sSkill ) );
		}
	}

	void CClient::ReceiveMsgInfo( NET_MSG_GENERIC* pMsg )
	{
		SNET_MSG_REFER_CHAR_INFO* pNetMsg = (SNET_MSG_REFER_CHAR_INFO*)pMsg;

		if( _tcscmp( m_sData.m_szName, pNetMsg->m_szTargetName ) != 0 )
			return;

		StringCchCopy( m_sData.m_szTitle, EM_TITLEID_MAXLENGTH, pNetMsg->m_sInfo.m_szTitle );

		m_sData.m_emClass = pNetMsg->m_sInfo.m_emClass;
		m_sData.m_wSchool = pNetMsg->m_sInfo.m_wSchool;
		m_sData.m_wSex = pNetMsg->m_sInfo.m_wSex;
		m_sData.m_wHair = pNetMsg->m_sInfo.m_wHair;
		m_sData.m_wHairColor = pNetMsg->m_sInfo.m_wHairColor;
		m_sData.m_wFace = pNetMsg->m_sInfo.m_wFace;

		m_sData.m_wLevel = pNetMsg->m_sInfo.m_wLevel;

		m_sData.m_sStats = pNetMsg->m_sInfo.m_sStats;

		m_sData.m_powerAttack = pNetMsg->m_sInfo.m_powerAttack;
		m_sData.m_powerDefence = pNetMsg->m_sInfo.m_powerDefnece;
		m_sData.m_powerAttribute[SKILL::EMAPPLY_MELEE] = pNetMsg->m_sInfo.m_powerAttribute[SKILL::EMAPPLY_MELEE];
		m_sData.m_powerAttribute[SKILL::EMAPPLY_RANGE] = pNetMsg->m_sInfo.m_powerAttribute[SKILL::EMAPPLY_RANGE];
		m_sData.m_powerAttribute[SKILL::EMAPPLY_MAGIC] = pNetMsg->m_sInfo.m_powerAttribute[SKILL::EMAPPLY_MAGIC];

		m_sData.m_sHP.nMax = pNetMsg->m_sInfo.m_nHP;
		m_sData.m_sMP.nMax = pNetMsg->m_sInfo.m_nMP;
		m_sData.m_sSP.nMax = pNetMsg->m_sInfo.m_nSP;

		m_sData.m_sHP.TO_FULL();
		m_sData.m_sMP.TO_FULL();
		m_sData.m_sSP.TO_FULL();

		m_sDataAddition = pNetMsg->m_sInfoAddition;

		SQLiteDelete( m_nReferServer, m_sData.m_szName );
		SQLiteInsert( m_nReferServer, m_sData, m_sDataAddition );

		m_sData.SET_VALIDATION_EXPSKILL_MAP();

		m_bComplete = true;	

		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_ReferChar_Open, "-s", m_sData.m_szName);
	}

	LuaTable	CClient::GetInfoData()
	{
		LuaTable tbStatList( GLWidgetScript::GetInstance().GetLuaState() );
	
		m_sData.INIT_DATA( FALSE, FALSE );
		m_sData.UPDATE_DATA( 0.0f, 0.0f, TRUE );

		tbStatList.set( 1, m_sData.m_emClass );
		tbStatList.set( 2, m_sData.m_wSchool );
		tbStatList.set( 3, m_sData.m_wSex );
		tbStatList.set( 4, m_sData.m_wHair );
		tbStatList.set( 5, m_sData.m_wHairColor );
		tbStatList.set( 6, m_sData.m_wFace );
		tbStatList.set( 7, m_sData.m_wLevel );	

		tbStatList.set( 8, m_sData.m_sSUMSTATS.wPow );
		tbStatList.set( 9, m_sData.m_sSUMSTATS.wDex );
		tbStatList.set( 10, m_sData.m_sSUMSTATS.wSpi );
		tbStatList.set( 11, m_sData.m_sSUMSTATS.wStr );
		tbStatList.set( 12, m_sData.m_sSUMSTATS.wSta );

		tbStatList.set( 13, m_sData.m_gdDAMAGE_BASIC.dwLow );
		tbStatList.set( 14, m_sData.m_gdDAMAGE_BASIC.dwHigh );
		tbStatList.set( 15, m_sData.GETDEFENSE() );
		tbStatList.set( 16, m_sData.m_sumPowerAttribute[ SKILL::EMAPPLY_MELEE ] );	
		tbStatList.set( 17, m_sData.m_sumPowerAttribute[ SKILL::EMAPPLY_RANGE ] );	
		tbStatList.set( 18, m_sData.m_sumPowerAttribute[ SKILL::EMAPPLY_MAGIC ] );

		tbStatList.set( 19, m_sData.m_nSUM_HIT );	
		tbStatList.set( 20, m_sData.m_nSUM_AVOID );	

		tbStatList.set( 21, m_sData.m_sHP.nMax );	
		tbStatList.set( 22, m_sData.m_sMP.nMax );	
		tbStatList.set( 23, m_sData.m_sSP.nMax );

// 		tbStatList.set( 24, m_sDataAddition.m_fCritical );
// 		tbStatList.set( 25, m_sDataAddition.m_fCrushingBlow );
// 		tbStatList.set( 26, m_sDataAddition.m_fMoveSpeed );
// 		tbStatList.set( 27, m_sDataAddition.m_fAtkSpeed );
// 		tbStatList.set( 28, m_sDataAddition.m_fExpRate );
// 		tbStatList.set( 29, m_sDataAddition.m_fINCR_HP_TOTAL );
// 		tbStatList.set( 30, m_sDataAddition.m_fINCR_MP_TOTAL );
// 		tbStatList.set( 31, m_sDataAddition.m_fINCR_SP_TOTAL );
// 		tbStatList.set( 32, m_sDataAddition.m_nReceive_CP_Inc );
// 	
// 		tbStatList.set( 33, m_sDataAddition.m_nPotion_HP_Inc );
// 		tbStatList.set( 34, m_sDataAddition.m_nPotion_MP_Inc );
// 		tbStatList.set( 35, m_sDataAddition.m_nPotion_SP_Inc );
// 
// 		tbStatList.set( 36, m_sData.m_sSUMRESIST.nFire );
// 		tbStatList.set( 37, m_sData.m_sSUMRESIST.nPoison );
// 		tbStatList.set( 38, m_sData.m_sSUMRESIST.nIce );
// 		tbStatList.set( 39, m_sData.m_sSUMRESIST.nSpirit );
// 		tbStatList.set( 40, m_sData.m_sSUMRESIST.nElectric );
// 
// 		tbStatList.set( 41, m_sData.m_nBright );
// 		tbStatList.set( 42, m_sData.m_nLiving );
// 		tbStatList.set( 43, m_sData.m_nContributionPoint );

		return tbStatList;
	
	}

	LuaTable CClient::GetCharInfoDetailAddonInfo()
	{
		// 캐릭터 정보 디테일 가산 옵션 정보를 묶어줌;
		m_sData.INIT_DATA( FALSE, FALSE );
		m_sData.UPDATE_DATA( 0.0f, 0.0f, TRUE );
		
		LuaTable tbDetailInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbDetailInfo.set("CriticalRate",	m_sDataAddition.m_fCritical );
		tbDetailInfo.set("CrushingBlow",	m_sDataAddition.m_fCrushingBlow );
		tbDetailInfo.set("MoveSpeed",		m_sDataAddition.m_fMoveSpeed );
		tbDetailInfo.set("AtkSpeed",		m_sDataAddition.m_fAtkSpeed );
		tbDetailInfo.set("ExpRate",			m_sDataAddition.m_fExpRate );
		tbDetailInfo.set("IncrHpTotal",		m_sDataAddition.m_fINCR_HP_TOTAL );
		tbDetailInfo.set("IncrMpTotal",		m_sDataAddition.m_fINCR_MP_TOTAL );
		tbDetailInfo.set("IncrSpTotal",		m_sDataAddition.m_fINCR_SP_TOTAL );
		tbDetailInfo.set("ReceiveCP",		m_sDataAddition.m_nReceive_CP_Inc );
		tbDetailInfo.set("PotionHpInc",		m_sDataAddition.m_nPotion_HP_Inc );
		tbDetailInfo.set("PotionMpInc",		m_sDataAddition.m_nPotion_MP_Inc );
		tbDetailInfo.set("PotionSpInc",		m_sDataAddition.m_nPotion_SP_Inc );
											
		tbDetailInfo.set("ResistFire",	m_sData.m_sSUMRESIST.nFire );
		tbDetailInfo.set("ResistCold",	m_sData.m_sSUMRESIST.nPoison );
		tbDetailInfo.set("ResistElec",	m_sData.m_sSUMRESIST.nIce );
		tbDetailInfo.set("ResistPisn",	m_sData.m_sSUMRESIST.nSpirit );
		tbDetailInfo.set("ResistSprt",	m_sData.m_sSUMRESIST.nElectric );

		tbDetailInfo.set("IncR_Ignore_BossDamage",		m_sData.m_sSUMITEM.fIncR_Ignore_BossDamage );
		tbDetailInfo.set("Inc_BossFixedDamage",			m_sData.m_sSUMITEM.nInc_BossFixedDamage );
		tbDetailInfo.set("IncR_Critical_EmergencyMob",	m_sData.m_sSUMITEM.fIncR_Critical_EmergencyMob );
		tbDetailInfo.set("IncR_Critical_EmergencyBoss",	m_sData.m_sSUMITEM.fIncR_Critical_EmergencyBoss );
		tbDetailInfo.set("Dec_Damage_EmergencyState",	m_sData.m_sSUMITEM.fDecR_Damage_EmergencyState );
		tbDetailInfo.set("IncR_ExpAddEffect",			m_sData.m_sSUMITEM.fIncR_ExpAddEffect );
		tbDetailInfo.set("IncR_DropGenMoney",			m_sData.m_sSUMITEM.fIncR_DropGenMoney );
		tbDetailInfo.set("DecR_Damage_MeleeSkill",		m_sData.m_sSUMITEM.fDecR_Damage_MeleeSkill );
		tbDetailInfo.set("DecR_Damage_RangeSkill",		m_sData.m_sSUMITEM.fDecR_Damage_RangeSkill );
		tbDetailInfo.set("DecR_Damage_MagicSkill",		m_sData.m_sSUMITEM.fDecR_Damage_MagicSkill );
		tbDetailInfo.set("Inc_HP_SoloPlay",				m_sData.m_sSUMITEM.nInc_HP_SoloPlay );
		tbDetailInfo.set("Inc_HP_PartyPlay",			m_sData.m_sSUMITEM.nInc_HP_PartyPlay );
		tbDetailInfo.set("IncR_AtkSpeed_SoloPlay",		m_sData.m_sSUMITEM.fIncR_AtkSpeed_SoloPlay );
		tbDetailInfo.set("IncR_AtkSpeed_PartyPlay",		m_sData.m_sSUMITEM.fIncR_AtkSpeed_PartyPlay );

		tbDetailInfo.set("GetIncR_CriticalDamage",			m_sData.m_sSUMITEM.fIncR_CriticalDamage );
		tbDetailInfo.set("GetIncR_CrushingBlow",			m_sData.m_sSUMITEM.fIncR_CrushingBlowDamage );
		tbDetailInfo.set("GetIncR_LifePerKill",				m_sData.m_sSUMITEM.fIncR_LifePerKill );
		tbDetailInfo.set("GetIncR_MPPerKill",				m_sData.m_sSUMITEM.fIncR_MPPerKill );
		tbDetailInfo.set("GetIncR_SPPerKill",				m_sData.m_sSUMITEM.fIncR_SPPerKill );
		tbDetailInfo.set("GetIncR_HPMPSPPerKill",			m_sData.m_sSUMITEM.fIncR_HPMPSPPerKill );
		tbDetailInfo.set("GetIncR_AllMPRatioToAttack",		m_sData.m_sSUMITEM.fIncR_AllMPRatioToAttack );
		tbDetailInfo.set("GetIncR_AllSPRatioToHp",			m_sData.m_sSUMITEM.fIncR_AllSPRatioToHp );
		tbDetailInfo.set("GetIncR_AllHitRatioToMelee",		m_sData.m_sSUMITEM.fIncR_AllHitRatioToMelee );
		tbDetailInfo.set("GetIncR_AllHitRatioToRange",		m_sData.m_sSUMITEM.fIncR_AllHitRatioToRange );
		tbDetailInfo.set("GetIncR_AllHitRatioToMagic",		m_sData.m_sSUMITEM.fIncR_AllHitRatioToMagic );
															
		return tbDetailInfo;								
	}														

	void CClient::GetSkillIndexRange(EMCHARCLASS _Class, int &_begin, int &_End)
	{
		switch( _Class )
		{
			case GLCC_FIGHTER_M:
			case GLCC_FIGHTER_W:
				{
					_begin = EMSKILL_FIGHTER_01;
					_End = EMSKILL_FIGHTER_04;
					break;
				}
			case GLCC_ARMS_M:
			case GLCC_ARMS_W:
				{
					_begin = EMSKILL_ARMS_01;
					_End = EMSKILL_ARMS_04;
					break;
				}
			case GLCC_ARCHER_M:
			case GLCC_ARCHER_W:
				{
					_begin = EMSKILL_ARCHER_01;
					_End = EMSKILL_ARCHER_04;
					break;
				}
			case GLCC_SPIRIT_M:
			case GLCC_SPIRIT_W:
				{
					_begin = EMSKILL_SPIRIT_01;
					_End = EMSKILL_SPIRIT_04;
					break;	
				}
			case GLCC_EXTREME_M:
			case GLCC_EXTREME_W:
				{
					_begin = EMSKILL_EXTREME_01;
					_End = EMSKILL_EXTREME_04;
					break;
				}				
			case GLCC_SCIENTIST_M:
			case GLCC_SCIENTIST_W:
				{
					_begin = EMSKILL_SCIENTIST_01;
					_End = EMSKILL_SCIENTIST_04;		
					break;
				}				
			case GLCC_ASSASSIN_M:
			case GLCC_ASSASSIN_W:
				{
					_begin = EMSKILL_ASSASSIN_01;
					_End = EMSKILL_ASSASSIN_04;	
					break;	  
				}				
			case GLCC_TRICKER_M:
			case GLCC_TRICKER_W:
				{
					_begin = EMSKILL_TRICKER_01;
					_End = EMSKILL_TRICKER_04;		
					break;
				}				
			case GLCC_ETC_M:
			case GLCC_ETC_W:
				{
					_begin = EMSKILL_ETC_01;
					_End = EMSKILL_ETC_04;
					break;
				}		
			case GLCC_ACTOR_M:
			case GLCC_ACTOR_W:
				{
					_begin = EMSKILL_ACTOR_01;
					_End = EMSKILL_ACTOR_08;
					break;
				}	
			}
	}

	LuaTable CClient::GetSkillData( int nPage )
	{
		LuaTable tbSkillList( GLWidgetScript::GetInstance().GetLuaState() );

		int nBegin = 0;
		int nEnd = 0;
		GetSkillIndexRange( m_sData.m_emClass, nBegin, nEnd );	

		int nSelectPage = nBegin + nPage;
		if( nSelectPage > nEnd )
			return tbSkillList;

		WORD nClass = 0;
		WORD nIndex = 0;
		GLSkillMan::GetInstance().GetMaxSkill( nClass, nIndex );

		std::vector< GLSKILL* >	vecSkill; // 스킬 정렬을 위한 백터 

		int nTableIndex = 1;
		for( WORD i = 0; i < nIndex; ++i )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( nSelectPage, i );
			if( NULL == pSkill )
				continue;

			SCHARSKILL* pCharSkill = m_sData.GETLEARNED_SKILL( pSkill->GetId() );
			if( true == pSkill->m_sBASIC.bLearnView && NULL == pCharSkill )
				continue;

			vecSkill.push_back( pSkill );
		}

		if( vecSkill.empty() == true )
			return NULL;

		//------ GLCharacer 의 스킬 정령을 그대로 가져옴 --- 관련해서 차후 수정이 필요 // 

		class CCompareSkill
		{
		public :
			bool operator()( const GLSKILL* pLValue, const GLSKILL* pRValue )
			{
				if( pLValue->m_sBASIC.dwGRADE < pRValue->m_sBASIC.dwGRADE )
				{
					return true;
				}
				else if( pLValue->m_sBASIC.dwGRADE == pRValue->m_sBASIC.dwGRADE )
				{
					if( pLValue->m_sBASIC.sNATIVEID.wSubID == pRValue->m_sBASIC.sNATIVEID.wSubID )
						return true;
				}

				return false;
			}
		};

		std::sort( vecSkill.begin(), vecSkill.end(), CCompareSkill() );

		BOOL bChange = false;
		DWORD dwTableID = 0;
		std::tr1::shared_ptr<GLCharClient> spCharClient = m_pGaeaClient->GetChar(m_sData.m_dwGlobalID);
		if (NULL != spCharClient.get())
		{
			bChange = RF_DISGUISE( spCharClient->m_EffSkillVarSet ).IsOn();
			dwTableID = RF_DISGUISE( spCharClient->m_EffSkillVarSet ).GetTableID() - 1;
		}

		std::vector< GLSKILL* >::iterator  iter = vecSkill.begin();
		for( ; iter != vecSkill.end(); ++iter )
		{
			PGLSKILL pSkill = (*iter);
			if( pSkill == NULL)
				continue;

			WORD wLevel = 0;
			SCHARSKILL* pCharSkill = m_sData.GETLEARNED_SKILL( pSkill->GetId() );
			if (pCharSkill)
			{
				wLevel = pCharSkill->wLevel + 1;
				if( SKILL::MAX_LEVEL <= wLevel )
					wLevel = SKILL::MAX_LEVEL - 1;
			}

			if (true == bChange)
			{
				GLSKILL* pOrgSkill = pSkill;

				SCHARSKILL pTempCharSkill;

				pSkill = GSCL_INST.GetChangeSkill((UINT)dwTableID, pSkill, pTempCharSkill);
				if (pSkill && pOrgSkill != pSkill && pTempCharSkill.sNativeID != NATIVEID_NULL())
				{
					if ( pCharSkill )
						m_sData.LEARN_CHANGE_SKILL(pOrgSkill, pCharSkill->wLevel);
				}

				if (NULL == pSkill)
					pSkill = pOrgSkill;
			}

			LuaTable tbSkill( GLWidgetScript::GetInstance().GetLuaState() );
			tbSkill.set( 1, pSkill->GetId().dwID );
			tbSkill.set( 2, wLevel );
			tbSkill.set( 3, pSkill->m_sBASIC.dwGRADE );
			tbSkill.set( 4, pSkill->GetName() );

			tbSkillList.set( nTableIndex++, tbSkill );
		}

		return tbSkillList;
	}
	
	LuaTable CClient::GetSkillTabPoint()
	{
		LuaTable tbSkilltabPoint( GLWidgetScript::GetInstance().GetLuaState() );
		int nPoint = 0;

		int nBegin = 0;
		int nEnd = 0;

		GetSkillIndexRange( m_sData.m_emClass, nBegin, nEnd );	

		for(int Count = 1; nBegin <= nEnd; ++nBegin, ++Count)
		{	
			int nSelectPage = nBegin;		

			WORD nClass = 0;
			WORD nIndex = 0;
			GLSkillMan::GetInstance().GetMaxSkill( nClass, nIndex );

			for( int i=0; i<nIndex; ++i )
			{
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( nSelectPage, i );
				if(pSkill)
				{
					SKILL_MAP_ITER it = m_sData.m_ExpSkills.find( pSkill->GetId().dwID );				
					
					if( it == m_sData.m_ExpSkills.end() )
						continue;

					const SCHARSKILL& sSkill = it->second;		
					for( WORD i=0; i<=sSkill.wLevel; ++i )
						nPoint += pSkill->m_sLEARN.sLVL_STEP[ i ].dwSKP;	
				}					
			}		
			tbSkilltabPoint.set(Count, nPoint);
			nPoint = 0;
		}
		
		return tbSkilltabPoint;
	}

	void CClient::ReqReferChar( const char* szTarName )
	{
		Request(m_pGaeaClient->GetConnServerGroup(), szTarName);
	}

	LuaTable CClient::GetPutOnItem( EMSLOT emSlot )
	{
		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, m_sData.m_PutOnItems[ emSlot ].Mid() );
		tbItem.set( 2,	m_sData.m_PutOnItems[ emSlot ].Sid() );
		tbItem.set( 3, m_sData.m_PutOnItems[ emSlot ].GetNativeID().IsValidNativeID() );

		return tbItem;
	}

	LuaTable CClient::GetPutOnItems()
	{
		LuaTable tbItems( GLWidgetScript::GetInstance().GetLuaState() );

		for( int i = 0; i < SLOT_NSIZE_S_2; ++i )
			tbItems.set( i + 1, GetPutOnItem( EMSLOT( i ) ) );

		return tbItems;
	}

	void CClient::SQLiteOpen( const std::string& strPath )
	{
		if( m_cSQLite.Open( strPath.c_str() ) != SQLITE_OK )
		{
			sc::writeLogError(
				sc::string::format(
					"[ReferChar] Open %1%", m_cSQLite.GetLastError() ) );
			return;
		}

		SQLiteVersion();
		SQLiteGarbage();
		SQLiteCreate();
	}

	void CClient::SQLiteClose()
	{
		m_cSQLite.Close();
	}

	void CClient::SQLiteVersion()
	{
		if( !m_cSQLite.IsOpen() )
			return;

		const char* szQuery = "SELECT Num FROM Version;";

		SQLite::TablePtr pTable( m_cSQLite.QuerySQL2( szQuery ) );
		if( !pTable.m_pTable )
			return;

		SQLite::Table& cTable = pTable();
		int nCount = cTable.GetRowCount();
		if( nCount <= 0 )
			return;

		int nVersion = cTable.GetValue< int >( 0 );
		if( nVersion == g_nVersion )
			return;

		// Version
		{
			const char* szQuery = "DROP TABLE IF EXISTS Version;";
			m_cSQLite.ExecuteSQL( szQuery );
		}

		// Info
		{
			const char* szQuery = "DROP TABLE IF EXISTS Info;";
			m_cSQLite.ExecuteSQL( szQuery );
		}

		// Puton
		{
			const char* szQuery = "DROP TABLE IF EXISTS Puton;";
			m_cSQLite.ExecuteSQL( szQuery );
		}

		// Skill
		{
			const char* szQuery = "DROP TABLE IF EXISTS Skill;";
			m_cSQLite.ExecuteSQL( szQuery );
		}
	}

	void CClient::SQLiteGarbage()
	{
		if( !m_cSQLite.IsOpen() )
			return;

		const char* szQuery = "\
			SELECT Server, NameHash \
			FROM Info \
			ORDER BY DateTime DESC \
			LIMIT %1% OFFSET %2%;";

		std::string strQuery = sc::string::format( szQuery, g_nGarbageMax, EViewMax );

		SQLite::TablePtr pTable( m_cSQLite.QuerySQL2( strQuery.c_str() ) );
		if( !pTable.m_pTable )
			return;

		SQLite::Table& cTable = pTable();
		int nCount = cTable.GetRowCount();
		if( nCount <= 0 )
			return;

		for( int i=0; i<nCount; ++i, cTable.GoNext() )
		{
			SQLiteDelete(
				cTable.GetValue< int >( 0 ),
				cTable.GetValue< size_t >( 1 ) );
		}
	}

	void CClient::SQLiteCreate()
	{
		if( !m_cSQLite.IsOpen() )
			return;

		SQLiteCreateVersion();
		SQLiteCreateInfo();
		SQLiteCreatePuton();
		SQLiteCreateSkill();
	}

	void CClient::SQLiteCreateVersion()
	{
		// Create
		{
			const char* szQuery = "\
				CREATE TABLE IF NOT EXISTS Version( \
					Num		INTEGER		NOT NULL, \
					PRIMARY KEY( Num ) );";

			if( m_cSQLite.ExecuteSQL( szQuery ) != SQLITE_OK )
			{
				sc::writeLogError(
					sc::string::format(
						"[ReferChar] CREATE TABLE Version %1%", m_cSQLite.GetLastError() ) );
			}
		}

		// Select
		{
			const char* szQuery = "SELECT Num FROM Version;";

			SQLite::TablePtr pTable( m_cSQLite.QuerySQL2( szQuery ) );
			if( pTable.m_pTable )
			{
				SQLite::Table& cTable = pTable();
				int nCount = cTable.GetRowCount();
				if( nCount > 0 )
					return;
			}
		}

		// Insert
		{
			const char* szQuery = "INSERT INTO Version( Num ) VALUES( %1% );";

			std::string strQuery = sc::string::format( szQuery, g_nVersion );

			if( m_cSQLite.ExecuteSQL( strQuery.c_str() ) != SQLITE_OK )
			{
				sc::writeLogError(
					sc::string::format(
						"[ReferChar] INSERT INTO Version %1%", m_cSQLite.GetLastError() ) );
			}
		}
	}

	void CClient::SQLiteCreateInfo()
	{
		const char* szQuery = "\
			CREATE TABLE IF NOT EXISTS Info( \
				Server		TINYINT		NOT NULL, \
				NameHash	INTEGER		NOT NULL, \
				Name		TEXT		NOT NULL, \
				DateTime	DATETIME	NOT NULL, \
				Title		TEXT		NOT NULL, \
				Class		SMALLINT	NOT NULL, \
				School		SMALLINT	NOT NULL, \
				Sex			SMALLINT	NOT NULL, \
				Hair		SMALLINT	NOT NULL, \
				HairColor	SMALLINT	NOT NULL, \
				Face		SMALLINT	NOT NULL, \
				Level		SMALLINT	NOT NULL, \
				StatPow		SMALLINT	NOT NULL, \
				StatStr		SMALLINT	NOT NULL, \
				StatSpi		SMALLINT	NOT NULL, \
				StatDex		SMALLINT	NOT NULL, \
				StatInt		SMALLINT	NOT NULL, \
				StatSta		SMALLINT	NOT NULL, \
				AP			SMALLINT	NOT NULL, \
				DP			SMALLINT	NOT NULL, \
				PA			SMALLINT	NOT NULL, \
				SA			SMALLINT	NOT NULL, \
				MA			SMALLINT	NOT NULL, \
				HP			SMALLINT	NOT NULL, \
				MP			SMALLINT	NOT NULL, \
				SP			SMALLINT	NOT NULL, \
				Critical		FLOAT		NOT NULL, \
				CrushingBlow	FLOAT		NOT NULL, \
				MoveSpeed		FLOAT		NOT NULL, \
				AtkSpeed		FLOAT		NOT NULL, \
				ExpRate			FLOAT		NOT NULL, \
				IncR_HP			FLOAT		NOT NULL, \
				IncR_MP			FLOAT		NOT NULL, \
				IncR_SP			FLOAT		NOT NULL, \
				Receive_CP		INTEGER		NOT NULL, \
				Potion_HP		INTEGER		NOT NULL, \
				Potion_MP		INTEGER		NOT NULL, \
				Potion_SP		INTEGER		NOT NULL, \
				PRIMARY KEY( Server, NameHash ) ); \
			CREATE INDEX IF NOT EXISTS Idx_Info_1 ON Info( Server ); \
			CREATE INDEX IF NOT EXISTS Idx_Info_2 ON Info( DateTime );";

		if( m_cSQLite.ExecuteSQL( szQuery ) != SQLITE_OK )
		{
			sc::writeLogError(
				sc::string::format(
					"[ReferChar] CREATE TABLE Info %1%", m_cSQLite.GetLastError() ) );
		}
	}

	void CClient::SQLiteCreatePuton()
	{
		const char* szQuery = "\
			CREATE TABLE IF NOT EXISTS Puton( \
				Server			TINYINT		NOT NULL, \
				NameHash		INTEGER		NOT NULL, \
				Slot			TINYINT		NOT NULL, \
				NativeMID		SMALLINT	NOT NULL, \
				NativeSID		SMALLINT	NOT NULL, \
				DisguiseMID		SMALLINT	NOT NULL, \
				DisguiseSID		SMALLINT	NOT NULL, \
				DAMAGE			TINYINT		NOT NULL, \
				DEFENSE			TINYINT		NOT NULL, \
				RESIST_FIRE		TINYINT		NOT NULL, \
				RESIST_ICE		TINYINT		NOT NULL, \
				RESIST_ELEC		TINYINT		NOT NULL, \
				RESIST_POISON	TINYINT		NOT NULL, \
				RESIST_SPIRIT	TINYINT		NOT NULL, \
				OptTYPE1		TINYINT		NOT NULL, \
				OptTYPE2		TINYINT		NOT NULL, \
				OptTYPE3		TINYINT		NOT NULL, \
				OptTYPE4		TINYINT		NOT NULL, \
				OptVALUE1		SMALLINT	NOT NULL, \
				OptVALUE2		SMALLINT	NOT NULL, \
				OptVALUE3		SMALLINT	NOT NULL, \
				OptVALUE4		SMALLINT	NOT NULL, \
				MainColor		INTEGER		NOT NULL, \
				SubColor		INTEGER		NOT NULL, \
				BasicAttackDamage		INTEGER		NOT NULL, \
				BasicDefence			SMALLINT	NOT NULL, \
				ItemLinkSkillMID		SMALLINT	NOT NULL, \
				ItemLinkSkillSID		SMALLINT	NOT NULL, \
				ItemLinkSkillLevel		SMALLINT	NOT NULL, \
				ItemLinkSkillTarget		SMALLINT	NOT NULL, \
				ItemLinkSkillOccurRate	FLOAT		NOT NULL, \
				ItemAddonType1			SMALLINT	NOT NULL, \
				ItemAddonValue1			INTEGER		NOT NULL, \
				ItemAddonType2			SMALLINT	NOT NULL, \
				ItemAddonValue2			INTEGER		NOT NULL, \
				ItemAddonType3			SMALLINT	NOT NULL, \
				ItemAddonValue3			INTEGER		NOT NULL, \
				ItemAddonType4			SMALLINT	NOT NULL, \
				ItemAddonValue4			INTEGER		NOT NULL, \
				ItemAddonType5			SMALLINT	NOT NULL, \
				ItemAddonValue5			INTEGER		NOT NULL, \
				ItemAddonType6			SMALLINT	NOT NULL, \
				ItemAddonValue6			INTEGER		NOT NULL, \
				PRIMARY KEY( Server, NameHash, Slot ) ); \
			CREATE INDEX IF NOT EXISTS Idx_Puton_1 ON Puton( Server, NameHash );";

		if( m_cSQLite.ExecuteSQL( szQuery ) != SQLITE_OK )
		{
			sc::writeLogError(
				sc::string::format(
					"[ReferChar] CREATE TABLE Puton %1%", m_cSQLite.GetLastError() ) );
		}
	}

	void CClient::SQLiteCreateSkill()
	{
		const char* szQuery = "\
			CREATE TABLE IF NOT EXISTS Skill( \
				Server		TINYINT		NOT NULL, \
				NameHash	INTEGER		NOT NULL, \
				NativeMID	SMALLINT	NOT NULL, \
				NativeSID	SMALLINT	NOT NULL, \
				Level		SMALLINT	NOT NULL, \
				PRIMARY KEY( Server, NameHash, NativeMID, NativeSID ) ); \
			CREATE INDEX IF NOT EXISTS Idx_Skill_1 ON Skill( Server, NameHash );";

		if( m_cSQLite.ExecuteSQL( szQuery ) != SQLITE_OK )
		{
			sc::writeLogError(
				sc::string::format(
					"[ReferChar] CREATE TABLE Skill %1%", m_cSQLite.GetLastError() ) );
		}
	}

	void CClient::SQLiteInsert( int nServer, const GLCHARLOGIC& sData, const SCharInfoAddition& sDataAddition )
	{
		if( !m_cSQLite.IsOpen() )
			return;

		SQLiteInsertInfo( nServer, sData, sDataAddition );
		SQLiteInsertPuton( nServer, sData );
		SQLiteInsertSkill( nServer, sData );
	}

	void CClient::SQLiteInsertInfo( int nServer, const GLCHARLOGIC& sData, const SCharInfoAddition& sDataAddition  )
	{
		const char* szQuery = "\
			INSERT INTO Info( \
				Server, NameHash, Name, DateTime, \
				Title, Class, School, Sex, Hair, HairColor, Face, Level, \
				StatPow, StatStr, StatSpi, StatDex, StatInt, StatSta, \
				AP, DP, PA, SA, MA, \
				HP, MP, SP, \
				Critical, CrushingBlow, MoveSpeed, AtkSpeed, ExpRate, \
				IncR_HP, IncR_MP, IncR_SP, Receive_CP, \
				Potion_HP, Potion_MP, Potion_SP ) \
			VALUES( \
				%1%, %2%, '%3%', '%4%', \
				'%5%', %6%, %7%, %8%, %9%, %10%, %11%, %12%, \
				%13%, %14%, %15%, %16%, %17%, %18%, \
				%19%, %20%, %21%, %22%, %23%, \
				%24%, %25%, %26%, \
				%27%, %28%, %29%, %30%, %31%, \
				%32%, %33%, %34%, %35%, \
				%36%, %37%, %38% );";

		time_t ltime;
		time( &ltime );
		struct tm sLocalTime;
		localtime_s( &sLocalTime, &ltime );
		char szDateTime[ MAX_PATH ] = { 0 };
		_tcsftime( szDateTime, MAX_PATH, "%Y-%m-%d %H:%M:%S", &sLocalTime );

		const SCHARSTATS& sStats = sData.m_sStats;

		std::string strQuery = sc::string::format(
			szQuery,
			nServer, m_hashString( sData.m_szName ), sData.m_szName, szDateTime,
			sData.m_szTitle, sData.m_emClass, sData.m_wSchool, sData.m_wSex, sData.m_wHair, sData.m_wHairColor, sData.m_wFace, sData.m_wLevel,
			sStats.wPow, sStats.wStr, sStats.wSpi, sStats.wDex, sStats.wInt, sStats.wSta,
			sData.m_powerAttack, sData.m_powerDefence, sData.m_powerAttribute[SKILL::EMAPPLY_MELEE], sData.m_powerAttribute[SKILL::EMAPPLY_RANGE], sData.m_powerAttribute[SKILL::EMAPPLY_MAGIC],
			sData.m_sHP.nNow, sData.m_sMP.nNow, sData.m_sSP.nNow,
			sDataAddition.m_fCritical, sDataAddition.m_fCrushingBlow, sDataAddition.m_fMoveSpeed, sDataAddition.m_fAtkSpeed, sDataAddition.m_fExpRate,
			sDataAddition.m_fINCR_HP_TOTAL, sDataAddition.m_fINCR_MP_TOTAL, sDataAddition.m_fINCR_SP_TOTAL, sDataAddition.m_nReceive_CP_Inc,
			sDataAddition.m_nPotion_HP_Inc, sDataAddition.m_nPotion_MP_Inc, sDataAddition.m_nPotion_SP_Inc );

		if( m_cSQLite.ExecuteSQL( strQuery.c_str() ) != SQLITE_OK )
		{
			sc::writeLogError(
				sc::string::format(
					"[ReferChar] INSERT INTO Info %1%", m_cSQLite.GetLastError() ) );
		}
	}

	void CClient::SQLiteInsertPuton( int nServer, const GLCHARLOGIC& sData )
	{
		const char* szQuery = "\
			INSERT INTO Puton( \
				Server, NameHash, Slot, \
				NativeMID, NativeSID, DisguiseMID, DisguiseSID, \
				DAMAGE, DEFENSE, \
				RESIST_FIRE, RESIST_ICE, RESIST_ELEC, RESIST_POISON, RESIST_SPIRIT, \
				OptTYPE1, OptTYPE2, OptTYPE3, OptTYPE4, \
				OptVALUE1, OptVALUE2, OptVALUE3, OptVALUE4, \
				MainColor, SubColor, \
				BasicAttackDamage, \
				BasicDefence, \
				ItemLinkSkillMID, \
				ItemLinkSkillSID, \
				ItemLinkSkillLevel, \
				ItemLinkSkillTarget, \
				ItemLinkSkillOccurRate, \
				ItemAddonType1, \
				ItemAddonValue1, \
				ItemAddonType2, \
				ItemAddonValue2, \
				ItemAddonType3, \
				ItemAddonValue3, \
				ItemAddonType4, \
				ItemAddonValue4, \
				ItemAddonType5, \
				ItemAddonValue5, \
				ItemAddonType6, \
				ItemAddonValue6 \
				) \
			VALUES( \
				%1%, %2%, %3%, \
				%4%, %5%, %6%, %7%, \
				%8%, %9%, \
				%10%, %11%, %12%, %13%, %14%, \
				%15%, %16%, %17%, %18%, \
				%19%, %20%, %21%, %22%, \
				%23%, %24%, \
				%25%, \
				%26%, \
				%27%, \
				%28%, \
				%29%, \
				%30%, \
				%31%, \
				%32%, \
				%33%, \
				%34%, \
				%35%, \
				%36%, \
				%37%, \
				%38%, \
				%39%, \
				%40%, \
				%41%, \
				%42%, \
				%43% \
				);";

		size_t nNameHash = m_hashString( sData.m_szName );

		for( WORD i=0; i<SLOT_TSIZE; ++i )
		{
			const SITEMCUSTOM& sItem = sData.m_PutOnItems[ i ];

			if( sItem.GetNativeID() == NATIVEID_NULL() )
				continue;

			std::string strQuery = sc::string::format(
				szQuery,
				nServer, nNameHash, i,
				sItem.GetNativeID().wMainID, sItem.GetNativeID().wSubID, sItem.nidDISGUISE.wMainID, sItem.nidDISGUISE.wSubID,
				(WORD)sItem.cDAMAGE, (WORD)sItem.cDEFENSE,
				(WORD)sItem.cRESIST_FIRE, (WORD)sItem.cRESIST_ICE, (WORD)sItem.cRESIST_ELEC, (WORD)sItem.cRESIST_POISON, (WORD)sItem.cRESIST_SPIRIT,
				(WORD)sItem.GETOptTYPE1(), (WORD)sItem.GETOptTYPE2(), (WORD)sItem.GETOptTYPE3(), (WORD)sItem.GETOptTYPE4(),
				sItem.randomOption.getValue(0), sItem.randomOption.getValue(1), sItem.randomOption.getValue(2), sItem.randomOption.getValue(3),
				sItem.dwMainColor, sItem.dwSubColor,
				sItem.GetBasicAttackDamage().GetData(),
				sItem.GetBasicDefence(),
				sItem.GetLinkSkillID().Mid(),
				sItem.GetLinkSkillID().Sid(),
				sItem.GetLinkSkillLevel(),
				sItem.GetLinkSkillTarget(),
				sItem.GetLinkSkillOccurRate(),
				static_cast< WORD >( sItem.sAddonInfo.m_sADDON[0].emTYPE ),
				sItem.sAddonInfo.m_sADDON[0].nVALUE,
				static_cast< WORD >( sItem.sAddonInfo.m_sADDON[1].emTYPE ),
				sItem.sAddonInfo.m_sADDON[1].nVALUE,
				static_cast< WORD >( sItem.sAddonInfo.m_sADDON[2].emTYPE ),
				sItem.sAddonInfo.m_sADDON[2].nVALUE,
				static_cast< WORD >( sItem.sAddonInfo.m_sADDON[3].emTYPE ),
				sItem.sAddonInfo.m_sADDON[3].nVALUE,
				static_cast< WORD >( sItem.sAddonInfo.m_sADDON[4].emTYPE ),
				sItem.sAddonInfo.m_sADDON[4].nVALUE,
				static_cast< WORD >( sItem.sAddonInfo.m_sADDON[5].emTYPE ),
				sItem.sAddonInfo.m_sADDON[5].nVALUE );


			if( m_cSQLite.ExecuteSQL( strQuery.c_str() ) != SQLITE_OK )
			{
				sc::writeLogError(
					sc::string::format(
						"[ReferChar] INSERT INTO Puton %1%", m_cSQLite.GetLastError() ) );
			}
		}
	}

	void CClient::SQLiteInsertSkill( int nServer, const GLCHARLOGIC& sData )
	{
		const char* szQuery = "\
			INSERT INTO Skill( \
				Server, NameHash, \
				NativeMID, NativeSID, Level ) \
			VALUES( \
				%1%, %2%, \
				%3%, %4%, %5% );";

		size_t nNameHash = m_hashString( sData.m_szName );

		BOOST_FOREACH( const SKILL_MAP::value_type& it, sData.m_ExpSkills )
		{
			const SCHARSKILL& sSkill = it.second;

			std::string strQuery = sc::string::format(
				szQuery,
				nServer, nNameHash,
				sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID, sSkill.wLevel );

			if( m_cSQLite.ExecuteSQL( strQuery.c_str() ) != SQLITE_OK )
			{
				sc::writeLogError(
					sc::string::format(
						"[ReferChar] INSERT INTO Skill %1%", m_cSQLite.GetLastError() ) );
			}
		}
	}

	void CClient::SQLiteDelete( int nServer, const std::string& strName )
	{
		SQLiteDelete( nServer, m_hashString( strName ) );
	}

	void CClient::SQLiteDelete( int nServer, size_t nNameHash )
	{
		if( !m_cSQLite.IsOpen() )
			return;

		const char* szQuery = "\
			DELETE FROM %1% \
			WHERE Server = %2% AND NameHash = %3%;";

		std::string strQuery = sc::string::format( szQuery, "Info", nServer, nNameHash );
		m_cSQLite.ExecuteSQL( strQuery.c_str() );

		strQuery = sc::string::format( szQuery, "Puton", nServer, nNameHash );
		m_cSQLite.ExecuteSQL( strQuery.c_str() );

		strQuery = sc::string::format( szQuery, "Skill", nServer, nNameHash );
		m_cSQLite.ExecuteSQL( strQuery.c_str() );
	}

	void CClient::SQLiteSelectNameList( int nServer, NameList& vecList )
	{
		if( !m_cSQLite.IsOpen() )
			return;

		const char* szQuery = "\
			SELECT Name \
			FROM Info \
			WHERE Server = %1% \
			ORDER BY DateTime DESC \
			LIMIT %2%;";

		std::string strQuery = sc::string::format( szQuery, nServer, EViewMax );

		SQLite::TablePtr pTable( m_cSQLite.QuerySQL2( strQuery.c_str() ) );
		if( !pTable.m_pTable )
			return;

		SQLite::Table& cTable = pTable();
        int nCount = cTable.GetRowCount();
        if( nCount <= 0 )
            return;

		for( int i=0; i<nCount; ++i, cTable.GoNext() )
			vecList.push_back( cTable.GetValue< std::string >( 0 ) );
	}

	bool CClient::SQLiteSelectData( int nServer, GLCHARLOGIC& sData, SCharInfoAddition& sDataAddition )
	{
		if( !m_cSQLite.IsOpen() )
			return false;

		if( !SQLiteSelectDataInfo( nServer, sData, sDataAddition ) )
			return false;

		SQLiteSelectDataPuton( nServer, sData );
		SQLiteSelectDataSkill( nServer, sData );

		return true;
	}

	bool CClient::SQLiteSelectDataInfo( int nServer, GLCHARLOGIC& sData, SCharInfoAddition& sDataAddition )
	{
		const char* szQuery = "\
			SELECT \
				Title, Class, School, Sex, Hair, HairColor, Face, Level, \
				StatPow, StatStr, StatSpi, StatDex, StatInt, StatSta, \
				AP, DP, PA, SA, MA, \
				HP, MP, SP, \
				Critical, CrushingBlow, MoveSpeed, AtkSpeed, ExpRate, \
				IncR_HP, IncR_MP, IncR_SP, Receive_CP, \
				Potion_HP, Potion_MP, Potion_SP \
			FROM Info \
			WHERE Server = %1% AND NameHash = %2%;";

		std::string strQuery = sc::string::format(
			szQuery,
			nServer, m_hashString( sData.m_szName ) );

		SQLite::TablePtr pTable( m_cSQLite.QuerySQL2( strQuery.c_str() ) );
		if( !pTable.m_pTable )
			return false;

		SQLite::Table& cTable = pTable();
		int nCount = cTable.GetRowCount();
		if( nCount <= 0 )
			return false;

		int nCol = 0;

		std::string strTitle = cTable.GetValue< std::string >( nCol++ );
		if( strTitle.empty() )
			memset( sData.m_szTitle, 0, sizeof( char ) * EM_TITLEID_MAXLENGTH );
		else
			StringCchCopy( sData.m_szTitle, EM_TITLEID_MAXLENGTH, strTitle.c_str() );

		sData.m_emClass = EMCHARCLASS( cTable.GetValue< WORD >( nCol++ ) );
		sData.m_wSchool = cTable.GetValue< WORD >( nCol++ );
		sData.m_wSex = cTable.GetValue< WORD >( nCol++ );
		sData.m_wHair = cTable.GetValue< WORD >( nCol++ );
		sData.m_wHairColor = cTable.GetValue< WORD >( nCol++ );
		sData.m_wFace = cTable.GetValue< WORD >( nCol++ );
		sData.m_wLevel = cTable.GetValue< WORD >( nCol++ );
		sData.m_sStats.wPow = cTable.GetValue< WORD >( nCol++ );
		sData.m_sStats.wStr = cTable.GetValue< WORD >( nCol++ );
		sData.m_sStats.wSpi = cTable.GetValue< WORD >( nCol++ );
		sData.m_sStats.wDex = cTable.GetValue< WORD >( nCol++ );
		sData.m_sStats.wInt = cTable.GetValue< WORD >( nCol++ );
		sData.m_sStats.wSta = cTable.GetValue< WORD >( nCol++ );
		sData.m_powerAttack = cTable.GetValue< int >( nCol++ );
		sData.m_powerDefence = cTable.GetValue< int >( nCol++ );
		sData.m_powerAttribute[SKILL::EMAPPLY_MELEE] = cTable.GetValue< int >( nCol++ );
		sData.m_powerAttribute[SKILL::EMAPPLY_RANGE] = cTable.GetValue< int >( nCol++ );
		sData.m_powerAttribute[SKILL::EMAPPLY_MAGIC] = cTable.GetValue< int >( nCol++ );
		sData.m_sHP.nNow = cTable.GetValue< int >( nCol++ );
		sData.m_sMP.nNow = cTable.GetValue< int >( nCol++ );
		sData.m_sSP.nNow = cTable.GetValue< int >( nCol++ );

		sDataAddition.m_fCritical = cTable.GetValue< float >( nCol++ );
		sDataAddition.m_fCrushingBlow = cTable.GetValue< float >( nCol++ );

		sDataAddition.m_fMoveSpeed = cTable.GetValue< float >( nCol++ );
		sDataAddition.m_fAtkSpeed = cTable.GetValue< float >( nCol++ );

		sDataAddition.m_fExpRate = cTable.GetValue< float >( nCol++ );

		sDataAddition.m_fINCR_HP_TOTAL = cTable.GetValue< float >( nCol++ );
		sDataAddition.m_fINCR_MP_TOTAL = cTable.GetValue< float >( nCol++ );
		sDataAddition.m_fINCR_SP_TOTAL = cTable.GetValue< float >( nCol++ );
		sDataAddition.m_nReceive_CP_Inc = cTable.GetValue< int >( nCol++ );

		sDataAddition.m_nPotion_HP_Inc = cTable.GetValue< int >( nCol++ );
		sDataAddition.m_nPotion_MP_Inc = cTable.GetValue< int >( nCol++ );
		sDataAddition.m_nPotion_SP_Inc = cTable.GetValue< int >( nCol++ );

		sData.m_sHP.TO_FULL();
		sData.m_sMP.TO_FULL();
		sData.m_sSP.TO_FULL();

		return true;
	}

	void CClient::SQLiteSelectDataPuton( int nServer, GLCHARLOGIC& sData )
	{
		for( WORD i=0; i<SLOT_TSIZE; ++i )
			sData.m_PutOnItems[ i ] = SITEMCUSTOM();

		const char* szQuery = "\
			SELECT \
				Slot, \
				NativeMID, NativeSID, DisguiseMID, DisguiseSID, \
				DAMAGE, DEFENSE, \
				RESIST_FIRE, RESIST_ICE, RESIST_ELEC, RESIST_POISON, RESIST_SPIRIT, \
				OptTYPE1, OptTYPE2, OptTYPE3, OptTYPE4, \
				OptVALUE1, OptVALUE2, OptVALUE3, OptVALUE4, \
				MainColor, SubColor, \
				BasicAttackDamage, \
				BasicDefence, \
				ItemLinkSkillMID, \
				ItemLinkSkillSID, \
				ItemLinkSkillLevel, \
				ItemLinkSkillTarget, \
				ItemLinkSkillOccurRate, \
				ItemAddonType1, \
				ItemAddonValue1, \
				ItemAddonType2, \
				ItemAddonValue2, \
				ItemAddonType3, \
				ItemAddonValue3, \
				ItemAddonType4, \
				ItemAddonValue4, \
				ItemAddonType5, \
				ItemAddonValue5, \
				ItemAddonType6, \
				ItemAddonValue6 \
			FROM Puton \
			WHERE Server = %1% AND NameHash = %2%;";

		std::string strQuery = sc::string::format(
			szQuery,
			nServer, m_hashString( sData.m_szName ) );

		SQLite::TablePtr pTable( m_cSQLite.QuerySQL2( strQuery.c_str() ) );
		if( !pTable.m_pTable )
			return;

		SQLite::Table& cTable = pTable();
		int nCount = cTable.GetRowCount();
		if( nCount <= 0 )
			return;

		for( int i=0; i<nCount; ++i, cTable.GoNext() )
		{
			int nCol = 0;

			WORD nSlot = cTable.GetValue< WORD >( nCol++ );
			if( nSlot >= SLOT_TSIZE )
				continue;

            SNATIVEID sNativeID;
            sNativeID.wMainID = cTable.GetValue< WORD >( nCol++ );
            sNativeID.wSubID = cTable.GetValue< WORD >( nCol++ );

			SITEMCUSTOM sItem( sNativeID );

			sItem.nidDISGUISE.wMainID = cTable.GetValue< WORD >( nCol++ );
			sItem.nidDISGUISE.wSubID = cTable.GetValue< WORD >( nCol++ );
			sItem.cDAMAGE = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.cDEFENSE = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.cRESIST_FIRE = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.cRESIST_ICE = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.cRESIST_ELEC = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.cRESIST_POISON = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.cRESIST_SPIRIT = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.randomOption.option[0].cOptType = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.randomOption.option[1].cOptType = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.randomOption.option[2].cOptType = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.randomOption.option[3].cOptType = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.randomOption.option[4].cOptType = (BYTE)cTable.GetValue< WORD >( nCol++ );
			sItem.randomOption.option[0].nOptValue = cTable.GetValue< short >( nCol++ );
			sItem.randomOption.option[1].nOptValue = cTable.GetValue< short >( nCol++ );
			sItem.randomOption.option[2].nOptValue = cTable.GetValue< short >( nCol++ );
			sItem.randomOption.option[3].nOptValue = cTable.GetValue< short >( nCol++ );
			sItem.randomOption.option[4].nOptValue = cTable.GetValue< short >( nCol++ );
			sItem.dwMainColor = cTable.GetValue< DWORD >( nCol++ );
			sItem.dwSubColor = cTable.GetValue< DWORD >( nCol++ );
			GLPADATA sBasicAttackDamage;
			sBasicAttackDamage.dwData = cTable.GetValue< DWORD >( nCol++ );
			sItem.SetBasicAttackDamage( sBasicAttackDamage );
			sItem.SetBasicDefence( cTable.GetValue< WORD >( nCol++ ) );
			sItem.SetLinkSkillID( SNATIVEID( cTable.GetValue< WORD >( nCol++ ), cTable.GetValue< WORD >( nCol++ ) ) );
			sItem.SetLinkSkillLevel( cTable.GetValue< WORD >( nCol++ ) );
			sItem.SetLinkSkillTarget( static_cast< EMITEM_SKILL_LINK >( cTable.GetValue< WORD >( nCol++ ) ) );
			sItem.SetLinkSkillOccurRate( cTable.GetValue< float >( nCol++ ) );
			sItem.sAddonInfo.m_sADDON[0].emTYPE = static_cast< EMITEM_ADDON >( cTable.GetValue< WORD >( nCol++ ) );
			sItem.sAddonInfo.m_sADDON[0].nVALUE = cTable.GetValue< int >( nCol++ );
			sItem.sAddonInfo.m_sADDON[1].emTYPE = static_cast< EMITEM_ADDON >( cTable.GetValue< WORD >( nCol++ ) );
			sItem.sAddonInfo.m_sADDON[1].nVALUE = cTable.GetValue< int >( nCol++ );
			sItem.sAddonInfo.m_sADDON[2].emTYPE = static_cast< EMITEM_ADDON >( cTable.GetValue< WORD >( nCol++ ) );
			sItem.sAddonInfo.m_sADDON[2].nVALUE = cTable.GetValue< int >( nCol++ );
			sItem.sAddonInfo.m_sADDON[3].emTYPE = static_cast< EMITEM_ADDON >( cTable.GetValue< WORD >( nCol++ ) );
			sItem.sAddonInfo.m_sADDON[3].nVALUE = cTable.GetValue< int >( nCol++ );
			sItem.sAddonInfo.m_sADDON[4].emTYPE = static_cast< EMITEM_ADDON >( cTable.GetValue< WORD >( nCol++ ) );
			sItem.sAddonInfo.m_sADDON[4].nVALUE = cTable.GetValue< int >( nCol++ );
			sItem.sAddonInfo.m_sADDON[5].emTYPE = static_cast< EMITEM_ADDON >( cTable.GetValue< WORD >( nCol++ ) );
			sItem.sAddonInfo.m_sADDON[5].nVALUE = cTable.GetValue< int >( nCol++ );

			sData.m_PutOnItems[ nSlot ] = sItem;
		}
	}

	void CClient::SQLiteSelectDataSkill( int nServer, GLCHARLOGIC& sData )
	{
		sData.m_ExpSkills.clear();

		const char* szQuery = "\
			SELECT NativeMID, NativeSID, Level \
			FROM Skill \
			WHERE Server = %1% AND NameHash = %2%;";

		std::string strQuery = sc::string::format(
			szQuery,
			nServer, m_hashString( sData.m_szName ) );

		SQLite::TablePtr pTable( m_cSQLite.QuerySQL2( strQuery.c_str() ) );
		if( !pTable.m_pTable )
			return;

		SQLite::Table& cTable = pTable();
		int nCount = cTable.GetRowCount();
		if( nCount <= 0 )
			return;

		for( int i=0; i<nCount; ++i, cTable.GoNext() )
		{
			int nCol = 0;

			SCHARSKILL sSkill;
			sSkill.sNativeID.wMainID = cTable.GetValue< WORD >( nCol++ );
			sSkill.sNativeID.wSubID = cTable.GetValue< WORD >( nCol++ );
			sSkill.wLevel = cTable.GetValue< WORD >( nCol++ );

			sData.m_ExpSkills.insert(
				std::make_pair( sSkill.sNativeID.dwID, sSkill ) );
		}

		m_sData.SET_VALIDATION_EXPSKILL_MAP();
	}
}
