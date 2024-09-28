#pragma once

#include "./InstanceFieldModeDefine.h"

class GLLandManager;
class GLGaeaServer;

namespace InstanceSystem
{
	class ManagerField;
	class InstanceField;

	class IInstanceFieldMode
	{
	public:
		/// Callback Function;
		virtual const bool						CALLBACK OnInitialize ( ) = 0;
		virtual const bool						CALLBACK OnUpdate ( const float _fElapsedTime ) = 0;
		virtual void							CALLBACK OnTimer ( const DWORD _dwTimerHandle, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 ) = 0;
		virtual void							CALLBACK OnActorTriggerIn ( const InstanceActorTrigger& _sActorTrigger, const ACTOR_VECTOR& _sInnerActorVector ) = 0;
		virtual void							CALLBACK OnActorTriggerOut ( const InstanceActorTrigger& _sActorTrigger, const ACTOR_VECTOR& _sOuterActorVector ) = 0;
		virtual const bool						CALLBACK OnRequestJoin ( const DBNUM _dwDBNum, const EMAUTHORITY _emAuthority ) = 0;
		virtual const bool						CALLBACK OnRequestMoveGate ( const DBNUM _dwDBNum, const SNATIVEID& _sMapID, const SNATIVEID& _sMapTarget, const DWORD _dwGateID, const DWORD _dwGateIDTarget ) = 0;
		virtual void							CALLBACK OnOnline ( const DBNUM _dwDBNum ) = 0;
		virtual void							CALLBACK OnOffline ( const DBNUM _dwDBNum, const bool _bReserveMember = false ) = 0;
		virtual const bool						CALLBACK OnJoin ( const DBNUM _dwDBNum, GLLandMan* _pLand, IN OUT D3DXVECTOR3& _vAdjustPos ) = 0;
		virtual void							CALLBACK OnOut ( const DBNUM _dwDBNum ) = 0;
		virtual void							CALLBACK OnMapEnter ( const DBNUM _dwDBNum, const SNATIVEID& _sBaseMapID ) = 0;
		virtual void							CALLBACK OnPartyChangeMaster ( const GLPartyID& _sPartyID, const bool _bExpedition, const DBNUM _dwNewMasterDBNum, const bool _bInSameInstanceNewMaster, const DBNUM _dwOldMasterDBNum, const bool _bInSameInstanceOldMaster ) = 0;
		virtual void							CALLBACK OnPartyJoin ( const GLPartyID& _sPartyID, const bool _bExpedition, const DBNUM _dwJoinerDBNum, const bool _bInSameInstance ) = 0;
		virtual void							CALLBACK OnPartyOut ( const GLPartyID& _sPartyID, const bool _bExpedition, const DBNUM _dwOuterDBNum, const bool _bInSameInstance ) = 0;
		virtual void							CALLBACK OnClickTrigger ( const DWORD _emNpcType, const DWORD _dwNpcID, const DWORD _emClickActorType, const DWORD _dwClickActorID ) = 0;
		virtual void							CALLBACK OnRetryProc ( const DWORD _dwChaNum, bool _bAccept, const DWORD _dwResultFlag ) = 0;
		virtual const bool						CALLBACK OnCompleteRetryProc ( const DWORD _emFactionType, const DWORD _dwFactionID, bool _bSuccess, const DWORD _dwResultFlag ) = 0;
		virtual const bool						CALLBACK OnAllDie ( const DWORD _emFactionType, const DWORD _dwFactionID ) = 0;
		virtual const bool						CALLBACK OnDie ( const DWORD _emDieActorType, const DWORD _dwDieActorID, const DWORD _emKillActorType, const DWORD _dwKillActorID ) = 0;
		virtual const bool						CALLBACK OnReceiveDamage ( const DWORD _emDamagedActorType, const DWORD _dwDamagedActorID, const DWORD _emAttackActorType, const DWORD _dwAttackActorID, DWORD& _dwDamage, const DWORD _dwDamageFlag ) = 0;
		virtual const bool						CALLBACK OnReceiveHeal ( const DWORD _emActorType, const DWORD _dwActorID, const DWORD _emReActorType, const DWORD _dwReActorID, DWORD& _dwHeal, const DWORD _dwHealFlag ) = 0;
		virtual const bool						CALLBACK OnUseItem ( const DWORD _dwActorID, const DWORD _dwItemMID, const DWORD _dwItemSID, const DWORD _nParam = 0, const float _fParam = 0.0f ) = 0;
		virtual const bool						CALLBACK OnResurrect ( const DWORD _emResurrectType, const DWORD _emDieActorType, const DWORD _dwDieActorID, const DWORD _emKillActorType, const DWORD _dwKillActorID ) = 0;
		virtual const bool						CALLBACK OnSkill ( const DWORD _dwActorType, const DWORD _dwActorID ) = 0;
		virtual void							CALLBACK OnMotionFinish ( const DWORD _dwActorType, const DWORD _dwActorID, const DWORD _dwMotionMID, const DWORD _dwMotionSID, const float _fRemainSec ) = 0;
		virtual void							CALLBACK OnCustomMessage ( const DBNUM _dwDBNum, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 ) = 0;
		virtual void							CALLBACK OnCustomMessageOuter ( const DBNUM _dwDBNum, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 ) = 0;
		virtual void							CALLBACK OnCustomMessageFromAgent ( const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 ) = 0;
		virtual void							CALLBACK OnCustomMessage ( const EM_CUSTOM_MESSAGE_TYPE _emSrcType, const DWORD _dwSrcID, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 ) = 0;
		virtual const bool						CALLBACK OnUpdateVictoriousCountry ( DWORD _dwContentID, DWORD _dwPreCountryID, DWORD _dwCurCountryID ) = 0;


	public:
		// MapID;
		inline const DWORD						GetInstanceMapID ( void ) const { return m_sMapID.getGaeaMapID().Mid(); }
		inline const SNATIVEID&					GetKeyMapID ( void ) const { return m_sMapID.getBaseMapID(); }


	public:
		virtual const std::string				GetFactionName ( const DWORD _dwFactionID ) = 0;

		virtual void							UI_CertificationMachineMode_Progress ( const DWORD _dwNameIndex, const DWORD _dwProgressIndex, const DWORD _dwPoint, const DWORD _dwTime ) { }

		virtual const bool						UI_CertificationMachineMode_Minimap ( const DWORD _dwChaDBNum, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg ) { return true; }
		virtual const bool						UI_CertificationMachineMode_Minimap_All ( const SNATIVEID& _sMapID, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg ) { return true; }

		virtual void							UI_CertificationMachineMode_Result_Begin ( void ) { }
		virtual void							UI_CertificationMachineMode_Result_Info ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwRanking, const DWORD _dwCertificationMachineCount, const DWORD _dwCertifyCount ) { }
		virtual void							UI_CertificationMachineMode_Result_End ( void ) { }

		virtual const bool						UI_CertificationMachineMode_Rank_MyInfo ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwRanking ) { return true; }
		virtual const bool						UI_CertificationMachineMode_Rank_Info ( const CertificationMachineMode::VEC_FACTION_POINT& _vecFactionPoint ) { return true; }

		virtual void							Log_CertificationMachineMode_In ( const DWORD _dwChaDBNum ) { }
		virtual void							Log_CertificationMachineMode_Out ( const DWORD _dwChaDBNum ) { }
		virtual void							Log_CertificationMachineMode_Result ( const DWORD _dw1stFactionID, const DWORD _dw1stPoint, const DWORD _dw1stCertificationMachineCount, const DWORD _dw1stCertifyCount, const DWORD _dw2stFactionID, const DWORD _dw2stPoint, const DWORD _dw2stCertificationMachineCount, const DWORD _dw2stCertifyCount, const DWORD _dw3stFactionID, const DWORD _dw3stPoint, const DWORD _dw3stCertificationMachineCount, const DWORD _dw3stCertifyCount ) { }


	public:
		/// Invoke Function;

		// Character;
		GLChar*									GetChar ( const DWORD _dwChaDBNum );
		ServerActor*							GetActor ( const Faction::GLActorID& _sActorID );

		const DWORD								GetHP ( const Faction::GLActorID& _sActorID );
		const DWORD								GetMP ( const Faction::GLActorID& _sActorID );
		const DWORD								GetSP ( const Faction::GLActorID& _sActorID );
		const DWORD								GetMaxHP ( const Faction::GLActorID& _sActorID );
		const DWORD								GetMaxMP ( const Faction::GLActorID& _sActorID );
		const DWORD								GetMaxSP ( const Faction::GLActorID& _sActorID );

		const bool								SetHP ( const Faction::GLActorID& _sActorID, const DWORD _dwHP );
		const bool								SetMP ( const Faction::GLActorID& _sActorID, const DWORD _dwMP );
		const bool								SetSP ( const Faction::GLActorID& _sActorID, const DWORD _dwSP );

		const bool								SetCTICountry ( const SNATIVEID& _sMapID, const DWORD _dwCountryID, bool _bUpdate );

		const DWORD								GetClubID ( const DWORD _dwChaDBNum );
		const DWORD								GetCountry ( const DWORD _dwChaDBNum );
		const DWORD								GetSchool ( const DWORD _dwChaDBNum );

		const std::string						GetClubName ( const DWORD _dwClubID );
		const std::string						GetCountryName ( const DWORD _dwCountryID );
		const std::string						GetActorName ( const Faction::GLActorID& _sActorID );
		const std::string						GetClubMasterName ( const DWORD _dwClubID );

		const D3DXVECTOR3						GetActorPosition ( const Faction::GLActorID& _sActorID );

		// Club;
		const bool								IsAllianceClub ( const DWORD _dwClubID, const DWORD _dwTargetClubID );

		// party;
		const bool								JoinParty ( const DWORD _dwMasterChaDBNum, const DWORD _dwChaDBNum );
		const bool								OutParty ( const DWORD _dwChaDBNum );
		const bool								ClearParty ( const DWORD _dwPartyID );

		// ViewRange;
		const bool								SetViewRange ( const DWORD _dwChaDBNum, const DWORD _dwViewRange, const float _fDuration );

		// Item;
		const DWORD								GetItemCount ( const DWORD _dwChaDBNum, const SNATIVEID& _sItemID );
		const bool								AddItem ( const DWORD _dwChaDBNum, const SNATIVEID& _sItemID, const DWORD _dwItemCount, const DWORD _dwGradeAttack, const DWORD _dwGradeDefense, const bool _bRandomOption );
		const bool								RemoveItem ( const DWORD _dwChaDBNum, const SNATIVEID& _sItemID, const DWORD _dwItemCount );

		// Move;
		const bool								SetPositionInstance ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMapID, const D3DXVECTOR3& _vPosition );
		const bool								SetObjectPositionInstance ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMapID, const Faction::GLActorID& _sTargetActorID );
		const bool								SetPositionField ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMapID, const D3DXVECTOR3& _vPosition );
		const bool								SetGatePositionField ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMapID, const DWORD _dwGateID );

		// Map 에서 Out 될 경우의 반환 위치를 설정한다;
		const bool								SetReturnPosition ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMapID, const DWORD _dwGateID );

		// Resurrection;
		const bool								SetResurrectionForced ( const DWORD _dwChaDBNum, const SNATIVEID& _sMapID, const DWORD _dwGateID, const float _fTime, const float _fRestore );
		const bool								SetResurrectionForcedPosition ( const DWORD _dwChaDBNum, const SNATIVEID& _sMapID, const D3DXVECTOR3& _vPosition, const float _fTime, const float _fRestore );
		const bool								SetResurrectionForcedInstance ( const DWORD _dwChaDBNum, const SNATIVEID& _sMapID, const DWORD _dwGateID, const float _fTime, const float _fRestore );
		const bool								SetResurrectionForcedInstancePosition ( const DWORD _dwChaDBNum, const SNATIVEID& _sMapID, const D3DXVECTOR3& _vPosition, const float _fTime, const float _fRestore );
		const bool								SetResurrectionDisable ( const DWORD _dwChaDBNum, const bool _bDisableSkill );
		const bool								SetResurrectionDisableSkill ( const DWORD _dwChaDBNum, const bool _bDisableSkill );
		const bool								SetDeathPenalty ( const DWORD _dwChaDBNum, const DWORD _dwDropExp, const DWORD _dwDropItemNum, const float _fDropItemRate );

		// Lock;
		const bool								SetJoinSuspendInstance ( const bool _bLock );
		const bool								SetJoinLockInstance ( const bool _bLock );
		const bool								SetJoinLockType ( const bool _bLock );

		// Setting;
		const bool								ActiveFactionChatting ( void );
		const bool								InactiveFactionChatting ( void );
		const bool								SetHPVisible ( const bool _bHPVisible );
		const bool								SetCountryMark ( const DWORD _dwChaDBNum, const bool _bCountryMark );
		const bool								SetVisible ( const DWORD _dwChaDBNum, const bool _bVisible );
		const bool								SetMoveLock ( const DWORD _dwChaDBNum, const bool _bMoveLock );

		// Timer;
		const DWORD								RegistTimer ( const DWORD _nTimeSec, const DWORD _dwParam0 = 0, const DWORD _dwParam1 = 0, const DWORD _dwParam2 = 0, const DWORD _dwParam3 = 0, const bool _bLoop = false );
		const bool								RemoveTimer ( const DWORD _hTimer );
		const bool								RemoveAllTimer ( void );
		const bool								IsRegistTimer ( const DWORD _hTimer );
		const bool								SuspendTimer ( const DWORD _hTimer );
		const bool								ResumeTimer ( const DWORD _hTimer );
		const int								GetLeftTime ( const DWORD _hTimer );

		// Faction;
		const Faction::FactionMemberVector*		GetFactionList ( const Faction::FactionID& _sFactionID );
		const bool								SetFaction ( const Faction::GLActorID& _sActorID, const Faction::FactionID& _sFactionID );
		const bool								SetRelation ( const Faction::FactionID& _sMyFactionID, const Faction::FactionID& _sTargetFactionID, const Faction::EMRELATION_TYPE _sRelationType );
		const bool								SetRelationToAll ( const Faction::FactionID& _sFactionID, const Faction::EMRELATION_TYPE _sRelationType );
		const bool								SetRelationFromAll ( const Faction::FactionID& _sFactionID, const Faction::EMRELATION_TYPE _sRelationType );

		// MobLayer;
		const bool								AddMobLayer ( const SNATIVEID& _sMapID, const DWORD _dwLayerIndex );
		const bool								DelMobLayer ( const SNATIVEID& _sMapID, const DWORD _dwLayerIndex );
		const bool								DelAllMobLayer ( const SNATIVEID& _sMapID );

		// Trigger;
		const bool								RegistActorTrigger ( const Faction::GLActorID& _sActorID, const DWORD _dwTriggerType, const float _dwTriggerRange );
		const bool								RegistActorZoneTrigger ( const Faction::GLActorID& _sActorID, const DWORD _dwTriggerType, const D3DXVECTOR3& _vPosition );
		const bool								RemoveActorTrigger ( const Faction::GLActorID& _sActorID );

		// Drop;
		const DWORD								DropCrow ( const SNATIVEID& _sMapID, const SNATIVEID& _sCrowID, const D3DXVECTOR3& _vPosition, const float _fRotate, const DWORD _dwLayerIndex = 0 );

		// Skill;
		const bool								ApplySkill ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sSkillID, const DWORD _dwSkillLevel, const float _fSkillTime = 10000.f, const float _fSkillLifeTime = _SKILLFACT_DEFAULTTIME, const DWORD _dwSkillFactType = 0 );
		const bool								ApplySkill_Faction ( const Faction::FactionID& _sFactionID, const SNATIVEID& _sSkillID, const DWORD _dwSkillLevel, const float _fSkillTime, const DWORD _dwSkillFactType = 0 );
		const bool								IsHaveSkillFact ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sSkillID );
		const bool								RemoveSkillFact ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sSkillID );
		const bool								RemoveSkillFact ( const Faction::GLActorID& _sActorID, const EMSPEC_ADDON _emSkillEffect );

		// Motion;
		const bool								SetMotion ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMotionID, const float _fMotionTime );
		const bool								IsMotion ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMotionID );
		const bool								ResetMotion ( const Faction::GLActorID& _sActorID );

		// Exp;
		const bool								AddExp ( const DWORD _dwChaDBNum, const float _fExp, const bool _bNotify );

		// Mail;
		const bool								SendSystemMail ( const DWORD _dwChaDBNum, const std::string& _strTitle, const std::string& _strContents, const LONGLONG _llAttatchedMoney, const SNATIVEID& _sAttatchedItem, const DWORD _dwAttatchedItemCount, const DWORD _dwContentsTextIndex = 0 );
		const bool								SendSystemMailFaction ( const Faction::FactionID& _sFactionID, const std::string& _strTitle, const std::string& _strContents, const LONGLONG _llAttatchedMoney, const SNATIVEID& _sAttatchedItem, const DWORD _dwAttatchedItemCount, const DWORD _dwContentsTextIndex = 0 );

		// Do;
		const bool								DoEffectPosition ( const DWORD _dwChaDBNum, const std::string& _strEffect, const SNATIVEID& _sMapID, const D3DXVECTOR3& _vPosition );
		const bool								DoEffectPositionAll ( const std::string& _strEffect, const SNATIVEID& _sMapID, const D3DXVECTOR3& _vPosition );
		const bool								DoEffectActor ( const DWORD _dwChaDBNum, const std::string& _strEffect, const Faction::GLActorID& _sActorID );
		const bool								DoEffectActorAll ( const std::string& _strEffect, const Faction::GLActorID& _sActorID );

		const bool								DoOut ( const DWORD _dwChaDBNum );
		const bool								DoJoin ( const DWORD _dwChaDBNum );
		const bool								DoMoveToPosition ( const Faction::GLActorID& _sActorID, const D3DXVECTOR3& _vPosition );
		const bool								DoMoveToObjectPosition ( const Faction::GLActorID& _sActorID, const Faction::GLActorID& _sTargetActorID );
		const bool								DoDestroy ();

		// SendMessage;
		const bool								SendCustomMessage ( const DWORD _dwDestType, const DWORD _DwDestID, const DWORD _dwParam1 = 0, const DWORD _dwParam2 = 0, const DWORD _dwParam3 = 0, const DWORD _dwParam4 = 0 );

		// Callback;
		const bool								CallbackFunction ( const std::string& _strFunctionName, const VEC_CALLBACK_PARAMETER& _vecParameter );

		// 채팅창에 System 메시지를 출력;
		const bool								PrintChatMsg ( const DWORD _dwChaDBNum, const std::string& _strText, const DWORD _dwTextColor );
		const bool								PrintChatXMLMsg ( const DWORD _dwChaDBNum, const InstanceSystem::InstanceXmlString& _sXMLString, const DWORD _dwTextColor );
		const bool								PrintChatMsgFaction ( const Faction::FactionID& _sFactionID, const std::string& _strText, const DWORD _dwTextColor, const bool _bAlly = false );
		const bool								PrintChatXMLMsgFaction ( const Faction::FactionID& _sFactionID, const InstanceSystem::InstanceXmlString& _sXMLString, const DWORD _dwTextColor, const bool _bAlly = false );
		const bool								PrintChatMsgAll ( const std::string& _strText, const DWORD _dwTextColor );
		const bool								PrintChatXMLMsgAll ( const InstanceSystem::InstanceXmlString& _sXMLString, const DWORD _dwTextColor );

		// 화면 중상단에 텍스트 출력;
		const bool								PrintSimpleMsg ( const DWORD _dwChaDBNum, const float _fTime, const std::string& _strText, const DWORD _dwTextColor, const DWORD _dwFontSize = 0 );
		const bool								PrintSimpleMsgFaction ( const Faction::FactionID& _sFactionID, const float _fTime, const std::string& _strText, const DWORD _dwTextColor, const DWORD _dwFontSize = 0, const bool _bAlly = false );
		const bool								PrintSimpleMsgAll ( const float _fTime, const std::string& _strText, const DWORD _dwTextColor, const DWORD _dwFontSize = 0 );
		const bool								PrintSimpleXMLMsgAll ( const float _fTime, const InstanceSystem::InstanceXmlString& _sXMLString, const DWORD _dwTextColor, const DWORD _dwFontSize = 0 );

		// 화면 상단 텍스트 다이얼로그 박스 출력;
		const bool								PrintTextDlgMsg ( const DWORD _dwChaDBNum, const std::string& _strText, const DWORD _dwTextColor );
		const bool								PrintTextDlgMsgFaction ( const Faction::FactionID& _sFactionID, const std::string& _strText, const DWORD _dwTextColor, const bool _bAlly = false );
		const bool								PrintTextDlgMsgAll ( const std::string& _strText, const DWORD _dwTextColor );

		// UI;
		const bool								UI_Auth ( const DWORD _dwChaDBNum, const bool _bVisible, const float _fTime );


	protected:
		ManagerField* const						m_pManagerField;
		InstanceField* const					m_pInstanceField;


	protected:
		const MapID								m_sMapID;
		GLLandManager* const					m_pLandManager;
		GLGaeaServer* const						m_pServer;


	protected:
		sc::LuaInstance* const					m_pLua;


	public:
		IInstanceFieldMode ( GLGaeaServer* _pServer
			, GLLandManager* _pLandManager
			, MapID _sMapID
			, ManagerField* _pManagerField
			, InstanceField* _pInstanceField
			, sc::LuaInstance* _pLua )
			: m_pServer ( _pServer )
			, m_pLandManager ( _pLandManager )
			, m_sMapID ( _sMapID )
			, m_pManagerField ( _pManagerField )
			, m_pInstanceField ( _pInstanceField )
			, m_pLua ( _pLua )
		{

		}

		virtual ~IInstanceFieldMode ( void ) { }
	};

}