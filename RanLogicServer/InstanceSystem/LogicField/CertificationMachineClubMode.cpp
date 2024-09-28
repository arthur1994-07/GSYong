#include "pch.h"

#include "../../../enginelib/GUInterface/GameTextControl.h"
#include "../../../enginelib/GUInterface/UITextControl.h"

#include "../../Database/DBAction/DbActionLogInstantDungeon.h"
#include "../../Land/GLLandMan.h"
#include "../../FieldServer/GLGaeaServer.h"

#include "./CertificationMachineClubMode.h"

namespace InstanceSystem
{

	const bool CALLBACK CCertificationMachineClubMode::OnInitialize ( )
	{
		return m_sCertificationMachineDungoen.OnInitialize( true );
	}


	const bool CALLBACK CCertificationMachineClubMode::OnUpdate (
		const float _fElapsedTime )
	{
		return true;
	}


	void CALLBACK CCertificationMachineClubMode::OnTimer (
		const DWORD _dwTimerHandle,
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3,
		const DWORD _dwParam4 )
	{
		m_sCertificationMachineDungoen.OnTimer(
			_dwTimerHandle,
			_dwParam1,
			_dwParam2,
			_dwParam3,
			_dwParam4 );

		/*switch ( _dwParam1 )
		{
		case CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_1:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 1;

				PrintSimpleXMLMsgAll(
					3.f,
					sXMLString,
					NS_UITEXTCOLOR::BLUE );

				RegistTimer( 5, CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_2 );
			}	
			break;

		case CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_2:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 2;

				PrintSimpleXMLMsgAll(
					5.f,
					sXMLString,
					NS_UITEXTCOLOR::BLUE );

				RegistTimer( 5, CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_3 );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_HELP_NOTIFY_3:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 3;

				PrintSimpleXMLMsgAll(
					5.f,
					sXMLString,
					NS_UITEXTCOLOR::BLUE );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CREATE_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 14;

				PrintSimpleXMLMsgAll(
					3.f,
					sXMLString,
					NS_UITEXTCOLOR::RED );

				PrintChatXMLMsgAll(
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_CAPTURE_NOTIFY:
			{
				{
					InstanceSystem::InstanceXmlString sXMLString;
					sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
					sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
					sXMLString.index = 15;
					sXMLString.XmlParamVector.push_back( GetClubName( _dwParam2 ) );

					PrintSimpleXMLMsgAll(
						3.f,
						sXMLString,
						NS_UITEXTCOLOR::RED );
				}

				{
					InstanceSystem::InstanceXmlString sXMLString;
					sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
					sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
					sXMLString.index = 16;

					PrintChatXMLMsgFaction(
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwParam2 ),
						sXMLString,
						NS_UITEXTCOLOR::YELLOW );
				}
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_ACCELERATOR_RECHARGE_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 13;
				sXMLString.XmlParamVector.push_back( sc::string::format( "%1%", _dwParam2 ) );

				PrintSimpleXMLMsgAll(
					3.f,
					sXMLString,
					NS_UITEXTCOLOR::RED );

				PrintChatXMLMsgAll(
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFY_SUCCESS_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 5;
				sXMLString.XmlParamVector.push_back( GetClubName( _dwParam2 ) );
				sXMLString.XmlParamVector.push_back( GetActorName( Faction::GLActorID( CROW_NPC, _dwParam3 ) ) );

				PrintSimpleXMLMsgAll(
					5.f,
					sXMLString,
					NS_UITEXTCOLOR::YELLOW );
	
				sXMLString.index = 6;

				PrintChatXMLMsgFaction(
					Faction::FactionID( Faction::EMFACTION_TEAM, _dwParam2 ),
					sXMLString,
					NS_UITEXTCOLOR::YELLOW );
				
				if ( _dwParam4 >= 1 )
				{
					InstanceSystem::InstanceXmlString sXMLString;
					sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
					sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
					sXMLString.index = 8;

					PrintChatXMLMsgFaction(
						Faction::FactionID( Faction::EMFACTION_TEAM, _dwParam2 ),
						sXMLString,
						NS_UITEXTCOLOR::YELLOW );
				}
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_DUPLICATE_FAIL_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 28;

				PrintChatXMLMsg(
					_dwParam2,
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 9;
				sXMLString.XmlParamVector.push_back( GetActorName( Faction::GLActorID( CROW_NPC, _dwParam3 ) ) );
				sXMLString.XmlParamVector.push_back( sc::string::format( "%1%", _dwParam4 ) );

				PrintChatXMLMsgFaction(
					Faction::FactionID( Faction::EMFACTION_TEAM, _dwParam2 ),
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 10;
				sXMLString.XmlParamVector.push_back( GetClubName( _dwParam2 ) );
				sXMLString.XmlParamVector.push_back( GetActorName( Faction::GLActorID( CROW_NPC, _dwParam3 ) ) );

				PrintSimpleXMLMsgAll(
					5.f,
					sXMLString,
					NS_UITEXTCOLOR::RED );

				PrintChatXMLMsgFaction(
					Faction::FactionID( Faction::EMFACTION_TEAM, _dwParam2 ),
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_FAIL_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 23;

				PrintChatXMLMsg(
					_dwParam2,
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_CERTIFICATION_MACHINE_CAPTURED_FAIL_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 23;

				PrintChatXMLMsg(
					_dwParam2,
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_APPROACHED_GOAL_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 4;
				sXMLString.XmlParamVector.push_back( GetClubName( _dwParam2 ) );

				PrintSimpleXMLMsgAll(
					5.f,
					sXMLString,
					NS_UITEXTCOLOR::RED );

				PrintChatXMLMsgAll(
					sXMLString,
					NS_UITEXTCOLOR::RED );
			}
			break;

		case CertificationMachineMode::EMTIMER_ID_EXIT_GAME_NOTIFY:
			{
				InstanceSystem::InstanceXmlString sXMLString;
				sXMLString.emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT;
				sXMLString.strXmlKey = "PVP_GUIDANCE_SYSTEM_MESSAGE";
				sXMLString.index = 12;
				sXMLString.XmlParamVector.push_back( sc::string::format( "%1%", _dwParam2 ) );

				PrintSimpleXMLMsgAll(
					3.f,
					sXMLString,
					NS_UITEXTCOLOR::YELLOW );

				PrintChatXMLMsgAll(
					sXMLString,
					NS_UITEXTCOLOR::YELLOW );
			}
			break;
		}*/
	}


	void CALLBACK CCertificationMachineClubMode::OnActorTriggerIn (
		const InstanceActorTrigger& _sActorTrigger,
		const ACTOR_VECTOR& _sInnerActorVector )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnActorTriggerOut (
		const InstanceActorTrigger& _sActorTrigger,
		const ACTOR_VECTOR& _sOuterActorVector )
	{

	}


	const bool CALLBACK CCertificationMachineClubMode::OnRequestJoin (
		const DBNUM _dwDBNum,
		const EMAUTHORITY _emAuthority )
	{
		if ( false == m_sCertificationMachineDungoen.IsPlay() )
			return true;

		if ( false == m_sCertificationMachineDungoen.IsInitialize() )
			return true;

		return false;
	}


	const bool CALLBACK CCertificationMachineClubMode::OnRequestMoveGate (
		const DBNUM _dwDBNum,
		const SNATIVEID& _sMapID,
		const SNATIVEID& _sMapTarget,
		const DWORD _dwGateID,
		const DWORD _dwGateIDTarget )
	{
		if ( _sMapTarget.Mid() == 22 )
		{
			SetPositionField( Faction::GLActorID( CROW_PC, _dwDBNum ), _sMapTarget, D3DXVECTOR3( 0.f, 0.f, 0.f ) );

			return false;
		}

		return true;
	}


	void CALLBACK CCertificationMachineClubMode::OnOnline (
		const DBNUM _dwDBNum )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnOffline (
		const DBNUM _dwDBNum,
		const bool _bReserveMember )
	{
		OnOut( _dwDBNum );
	}


	const bool CALLBACK CCertificationMachineClubMode::OnJoin (
		const DBNUM _dwDBNum,
		GLLandMan* _pLand,
		IN OUT D3DXVECTOR3& _vAdjustPos )
	{
		const DWORD dwClubID = GetClubID( _dwDBNum );
		if ( CLUB_NULL == dwClubID )
			return false;

		bool bReturn = m_sCertificationMachineDungoen.OnJoin( _dwDBNum, dwClubID );

		// 캐릭터의 학원에 따라 복귀위치를 다르게한다;
		const DWORD dwSchool = GetSchool( _dwDBNum );
		switch ( dwSchool )
		{
		case SCHOOL_SM:
			SetReturnPosition( Faction::GLActorID( CROW_PC, _dwDBNum ), SNATIVEID( 2, 0 ), 6 );
			break;

		case SCHOOL_HA:
			SetReturnPosition( Faction::GLActorID( CROW_PC, _dwDBNum ), SNATIVEID( 5, 0 ), 6 );
			break;

		case SCHOOL_BH:
			SetReturnPosition( Faction::GLActorID( CROW_PC, _dwDBNum ), SNATIVEID( 8, 0 ), 6 );
			break;

		default:
			SetReturnPosition( Faction::GLActorID( CROW_PC, _dwDBNum ), SNATIVEID( 22, 0 ), 7 );
			break;
		}
	}


	void CALLBACK CCertificationMachineClubMode::OnOut (
		const DBNUM _dwDBNum )
	{
		GLChar* pChar = GetChar( _dwDBNum );
		if ( NULL == pChar )
			return;

		const DWORD dwClubID = GetClubID( _dwDBNum );
		if ( CLUB_NULL == dwClubID )
			return;

		m_sCertificationMachineDungoen.OnOut( _dwDBNum );
	}


	void CALLBACK CCertificationMachineClubMode::OnMapEnter (
		const DBNUM _dwDBNum,
		const SNATIVEID& _sBaseMapID )
	{
		if ( NULL == m_pLandManager )
			return;

		const DWORD dwClubID = GetClubID( _dwDBNum );
		if ( CLUB_NULL == dwClubID )
			return;

		m_sCertificationMachineDungoen.OnEnterMap( _dwDBNum, _sBaseMapID );
	}


	void CALLBACK CCertificationMachineClubMode::OnPartyChangeMaster (
		const GLPartyID& _sPartyID,
		const bool _bExpedition,
		const DBNUM _dwNewMasterDBNum,
		const bool _bInSameInstanceNewMaster,
		const DBNUM _dwOldMasterDBNum,
		const bool _bInSameInstanceOldMaster )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnPartyJoin (
		const GLPartyID& _sPartyID,
		const bool _bExpedition,
		const DBNUM _dwJoinerDBNum,
		const bool _bInSameInstance )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnPartyOut (
		const GLPartyID& _sPartyID,
		const bool _bExpedition,
		const DBNUM _dwOuterDBNum,
		const bool _bInSameInstance )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnClickTrigger (
		const DWORD _emNpcType,
		const DWORD _dwNpcID,
		const DWORD _emClickActorType,
		const DWORD _dwClickActorID )
	{
		m_sCertificationMachineDungoen.OnStartCertification( _dwNpcID, _dwClickActorID );
	}


	void CALLBACK CCertificationMachineClubMode::OnRetryProc (
		const DWORD _dwChaNum,
		bool _bAccept,
		const DWORD _dwResultFlag )
	{

	}


	const bool CALLBACK CCertificationMachineClubMode::OnCompleteRetryProc (
		const DWORD _emFactionType,
		const DWORD _dwFactionID,
		bool _bSuccess,
		const DWORD _dwResultFlag )
	{
		return true;
	}


	const bool CALLBACK CCertificationMachineClubMode::OnAllDie (
		const DWORD _emFactionType,
		const DWORD _dwFactionID )
	{
		return true;
	}


	const bool CALLBACK CCertificationMachineClubMode::OnDie (
		const DWORD _emDieActorType,
		const DWORD _dwDieActorID,
		const DWORD _emKillActorType,
		const DWORD _dwKillActorID )
	{
		return m_sCertificationMachineDungoen.OnDie(
			Faction::GLActorID( _emDieActorType, _dwDieActorID ),
			Faction::GLActorID( _emKillActorType, _dwKillActorID ) );
	}


	const bool CALLBACK CCertificationMachineClubMode::OnReceiveDamage (
		const DWORD _emDamagedActorType,
		const DWORD _dwDamagedActorID,
		const DWORD _emAttackActorType,
		const DWORD _dwAttackActorID,
		DWORD& _dwDamage,
		const DWORD _dwDamageFlag )
	{
		return true;
	}


	const bool CALLBACK CCertificationMachineClubMode::OnReceiveHeal (
		const DWORD _emActorType,
		const DWORD _dwActorID,
		const DWORD _emReActorType,
		const DWORD _dwReActorID,
		DWORD& _dwHeal,
		const DWORD _dwHealFlag )
	{
		return true;
	}


	const bool CALLBACK CCertificationMachineClubMode::OnUseItem (
		const DWORD _dwActorID,
		const DWORD _dwItemMID,
		const DWORD _dwItemSID,
		const DWORD _nParam,
		const float _fParam )
	{
		return true;
	}


	const bool CALLBACK CCertificationMachineClubMode::OnResurrect (
		const DWORD _emResurrectType,
		const DWORD _emDieActorType,
		const DWORD _dwDieActorID,
		const DWORD _emKillActorType,
		const DWORD _dwKillActorID )
	{
		return true;
	}


	const bool CALLBACK CCertificationMachineClubMode::OnSkill (
		const DWORD _dwActorType,
		const DWORD _dwActorID )
	{
		return true;
	}


	void CALLBACK CCertificationMachineClubMode::OnMotionFinish (
		const DWORD _dwActorType,
		const DWORD _dwActorID,
		const DWORD _dwMotionMID,
		const DWORD _dwMotionSID,
		const float _fRemainSec )
	{
		m_sCertificationMachineDungoen.OnEndCertification( _dwActorID, _fRemainSec );
	}


	void CALLBACK CCertificationMachineClubMode::OnCustomMessage (
		const DBNUM _dwDBNum,
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3,
		const DWORD _dwParam4 )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnCustomMessageOuter (
		const DBNUM _dwDBNum,
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3,
		const DWORD _dwParam4 )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnCustomMessageFromAgent (
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3,
		const DWORD _dwParam4 )
	{

	}


	void CALLBACK CCertificationMachineClubMode::OnCustomMessage (
		const EM_CUSTOM_MESSAGE_TYPE _emSrcType,
		const DWORD _dwSrcID,
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3,
		const DWORD _dwParam4 )
	{
		m_sCertificationMachineDungoen.OnCustomMessage(
			_dwSrcID,
			_dwParam1,
			_dwParam2,
			_dwParam3,
			_dwParam4 );
	}


	const bool CALLBACK CCertificationMachineClubMode::OnUpdateVictoriousCountry (
		DWORD _dwContentID,
		DWORD _dwPreCountryID,
		DWORD _dwCurCountryID )
	{
		return true;
	}


	const std::string CCertificationMachineClubMode::GetFactionName ( const DWORD _dwFactionID )
	{
		return GetClubName( _dwFactionID );
	}


	void CCertificationMachineClubMode::UI_CertificationMachineMode_Progress (
		const DWORD _dwNameIndex,
		const DWORD _dwProgressIndex,
		const DWORD _dwPoint,
		const DWORD _dwTime )
	{
		if ( NULL == m_pLandManager )
			return;

		// 진행 UI를 표시한다;
		GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC sNetMsg; 
		sNetMsg.iNameIndex = static_cast< int >( _dwNameIndex );
		sNetMsg.iProgressIndex = static_cast< int >( _dwProgressIndex );
		sNetMsg.iPoint = static_cast< int >( _dwPoint );
		sNetMsg.fRemainTime = static_cast< float >( _dwTime );

		m_pLandManager->sendMsgPC( &sNetMsg );
	}


	const bool CCertificationMachineClubMode::UI_CertificationMachineMode_Minimap (
		const DWORD _dwChaDBNum,
		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* pChar = GetChar( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		msgpack::sbuffer buf;
		msgpack::pack( buf, *_pNetMsg );
		m_pServer->SENDTOCLIENT(
			pChar->ClientSlot(),
			NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK,
			buf );

		return true;
	}


	const bool CCertificationMachineClubMode::UI_CertificationMachineMode_Minimap_All (
		const SNATIVEID& _sMapID,
		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg )
	{
		if ( NULL == m_pLandManager )
			return false;

		msgpack::sbuffer buf;
		msgpack::pack( buf, *_pNetMsg );
		m_pLandManager->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return true;
	}


	void CCertificationMachineClubMode::UI_CertificationMachineMode_Result_Begin ( void )
	{
		if ( NULL == m_pLandManager )
			return;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC sNetMsg;
		sNetMsg.typeCompetition = COMPETITION::COMPETITION_GUIDANCE;

		sNetMsg.sTabInfo.strTabName = ID2SERVERTEXT( "RN_COMPETITION_RESULT_TAB", 0 ); // "전투 결과";
		sNetMsg.sTabInfo.emSortType = RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC;
		sNetMsg.sTabInfo.wSortTitleNum = 0;
		sNetMsg.sTabInfo.emMyResultType = RnCompetitionResultTabInfo::EM_MY_RESULT_CLUB;

		sNetMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 34, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 0 ) );
		sNetMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK, 34, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 15 ) );
		sNetMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_STRING, 110, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 4 ) );
		sNetMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_STRING, 110, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 14 ) );
		sNetMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 16 ) );
		sNetMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 17 ) );
		sNetMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 18 ) );

		msgpack::sbuffer buf;
		msgpack::pack( buf, sNetMsg );

		m_pLandManager->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );
	}


	void CCertificationMachineClubMode::UI_CertificationMachineMode_Result_Info (
		const DWORD _dwFactionID,
		const DWORD _dwPoint,
		const DWORD _dwRanking,
		const DWORD _dwCertificationMachineCount,
		const DWORD _dwCertifyCount )
	{
		if ( NULL == m_pLandManager )
			return;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC sNetMsg;
		sNetMsg.typeCompetition = COMPETITION::COMPETITION_GUIDANCE;
		sNetMsg.nTabNumber = 0;
		sNetMsg.sContents.dwKeyID = _dwFactionID;
		sNetMsg.sContents.AddContent( static_cast< int >( _dwRanking ) );
		sNetMsg.sContents.AddContent( static_cast< int >(  _dwFactionID ) );
		sNetMsg.sContents.AddContent( GetClubName( _dwFactionID ).c_str() );
		sNetMsg.sContents.AddContent( GetClubMasterName( _dwFactionID ).c_str() );
		sNetMsg.sContents.AddContent( static_cast< int >( _dwPoint ) );
		sNetMsg.sContents.AddContent( static_cast< int >( _dwCertificationMachineCount ) );
		sNetMsg.sContents.AddContent( static_cast< int >( _dwCertifyCount ) );

		msgpack::sbuffer buf;
		msgpack::pack( buf, sNetMsg );

		m_pLandManager->sendMsgPC(NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf);
	}


	void CCertificationMachineClubMode::UI_CertificationMachineMode_Result_End ( void )
	{
		if ( NULL == m_pLandManager )
			return;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC sNetMsg;
		sNetMsg.typeCompetition = COMPETITION::COMPETITION_GUIDANCE;
		sNetMsg.fOutTime = m_sCertificationMachineDungoen.TIME_GAME_DESTROY_WAITTIME;
		sNetMsg.nCloseButtonParam = m_sCertificationMachineDungoen.EMCUSTOM_MSG_EXIT_BUTTON;

		msgpack::sbuffer buf;
		msgpack::pack( buf, sNetMsg );

		m_pLandManager->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );
	}


	const bool CCertificationMachineClubMode::UI_CertificationMachineMode_Rank_MyInfo (
		const DWORD _dwFactionID,
		const DWORD _dwPoint,
		const DWORD _dwRanking )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC sNetMsg;
		sNetMsg.iMyClubPoint = _dwPoint;
		sNetMsg.iMyClubRank = _dwRanking;

		m_pLandManager->sendMsgFaction(
			Faction::FactionID( Faction::EMFACTION_TEAM, _dwFactionID ),
			&sNetMsg );

		m_pLandManager->sendMsgFaction(
			Faction::FactionID( Faction::EMFACTION_SAFE_TEAM, _dwFactionID ),
			&sNetMsg );

		return true;
	}


	const bool CCertificationMachineClubMode::UI_CertificationMachineMode_Rank_Info (
		const CertificationMachineMode::VEC_FACTION_POINT& _vecFactionPoint )
	{
		if ( NULL == m_pLandManager )
			return false;

		int nShowCount = 0;
		GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_FC sNetMsg;
		for ( int i=0; i<_vecFactionPoint.size(); ++i )
		{
			CertificationMachineMode::SFACTION_POINT sFactionPoint = _vecFactionPoint.at( i );

			UI_CertificationMachineMode_Rank_MyInfo(
				sFactionPoint.dwFactionID,
				sFactionPoint.dwFactionPoint,
				sFactionPoint.dwRanking );

			if ( nShowCount < 5 )
			{
				GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_FC::RankInfo sRankinfo;
				sRankinfo.clubName = GetFactionName( sFactionPoint.dwFactionID );
				sRankinfo.iPoint = sFactionPoint.dwFactionPoint;
				sRankinfo.iRank = sFactionPoint.dwRanking;
				nShowCount++;

				sNetMsg.rankVector.push_back( sRankinfo );
			}
		}

		msgpack::sbuffer buf;
		msgpack::pack( buf, sNetMsg );
		m_pLandManager->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return true;
	}


	void CCertificationMachineClubMode::Log_CertificationMachineMode_In (
		const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pLandManager )
			return;

		if ( NULL == m_pServer )
			return;

		const DWORD dwClubID = GetClubID( _dwChaDBNum );
		if ( CLUB_NULL == dwClubID )
			return;

		db::LogInstantGuidance* pDbAction = new db::LogInstantGuidance(
			db::LogInstantGuidance::EM_IN,
			m_pLandManager->getGUID() );

		pDbAction->strGuidanceName = m_sCertificationMachineDungoen.GetInstanceDungeonName();
		pDbAction->dwCharDbNum = _dwChaDBNum;
		pDbAction->dwClubDbNum = dwClubID;
		pDbAction->nClubPoint = m_sCertificationMachineDungoen.GetPointFaction( dwClubID );

		m_pServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );
	}


	void CCertificationMachineClubMode::Log_CertificationMachineMode_Out (
		const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pLandManager )
			return;

		if ( NULL == m_pServer )
			return;

		const DWORD dwClubID = GetClubID( _dwChaDBNum );
		if ( CLUB_NULL == dwClubID )
			return;

		db::LogInstantGuidance* pDbAction = new db::LogInstantGuidance(
			db::LogInstantGuidance::EM_OUT,
			m_pLandManager->getGUID() );

		pDbAction->strGuidanceName = m_sCertificationMachineDungoen.GetInstanceDungeonName();
		pDbAction->dwCharDbNum = _dwChaDBNum;
		pDbAction->dwClubDbNum = dwClubID;
		pDbAction->nClubPoint = m_sCertificationMachineDungoen.GetPointFaction( dwClubID );

		m_pServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );
	}


	void CCertificationMachineClubMode::Log_CertificationMachineMode_Result (
		const DWORD _dw1stFactionID,
		const DWORD _dw1stPoint,
		const DWORD _dw1stCertificationMachineCount,
		const DWORD _dw1stCertifyCount,
		const DWORD _dw2stFactionID,
		const DWORD _dw2stPoint,
		const DWORD _dw2stCertificationMachineCount,
		const DWORD _dw2stCertifyCount,
		const DWORD _dw3stFactionID,
		const DWORD _dw3stPoint,
		const DWORD _dw3stCertificationMachineCount,
		const DWORD _dw3stCertifyCount )
	{
		if ( NULL == m_pLandManager )
			return;

		if ( NULL == m_pServer )
			return;

		db::LogInstantGuidance* pDbAction = new db::LogInstantGuidance(
			db::LogInstantGuidance::EM_RESULT,
			m_pLandManager->getGUID() );

		pDbAction->strGuidanceName = m_sCertificationMachineDungoen.GetInstanceDungeonName();

		pDbAction->dwClubDbNum = _dw1stFactionID;
		pDbAction->nClubPoint = _dw1stPoint;
		pDbAction->nClubHaveAuthNum = _dw1stCertificationMachineCount;
		pDbAction->nClubAuthCount = _dw1stCertifyCount;

		pDbAction->dwSecondClubDbNum = _dw2stFactionID;
		pDbAction->nSecondClubPoint = _dw2stPoint;
		pDbAction->nSecondClubHaveAuthNum = _dw2stCertificationMachineCount;
		pDbAction->nSecondClubAuthCount = _dw2stCertifyCount;

		pDbAction->dwThirdClubDbNum = _dw3stFactionID;
		pDbAction->nThirdClubPoint = _dw3stPoint;
		pDbAction->nThirdClubHaveAuthNum = _dw3stCertificationMachineCount;
		pDbAction->nThirdClubAuthCount = _dw3stCertifyCount;

		m_pServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );
	}

}