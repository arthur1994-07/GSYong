#pragma once

#include "./IInstanceFieldMode.h"

#include "./CertificationMachineMode/InstanceDungeon_CertificationMachine.h"

namespace InstanceSystem
{

	class CCertificationMachineClubMode : public IInstanceFieldMode
	{
	public:
		virtual const bool							CALLBACK OnInitialize ( );
		virtual const bool							CALLBACK OnUpdate ( const float _fElapsedTime );
		virtual void								CALLBACK OnTimer ( const DWORD _dwTimerHandle, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 );
		virtual void								CALLBACK OnActorTriggerIn ( const InstanceActorTrigger& _sActorTrigger, const ACTOR_VECTOR& _sInnerActorVector );
		virtual void								CALLBACK OnActorTriggerOut ( const InstanceActorTrigger& _sActorTrigger, const ACTOR_VECTOR& _sOuterActorVector );
		virtual const bool							CALLBACK OnRequestJoin ( const DBNUM _dwDBNum, const EMAUTHORITY _emAuthority );
		virtual const bool							CALLBACK OnRequestMoveGate ( const DBNUM _dwDBNum, const SNATIVEID& _sMapID, const SNATIVEID& _sMapTarget, const DWORD _dwGateID, const DWORD _dwGateIDTarget );
		virtual void								CALLBACK OnOnline ( const DBNUM _dwDBNum );
		virtual void								CALLBACK OnOffline ( const DBNUM _dwDBNum, const bool _bReserveMember = false );
		virtual const bool							CALLBACK OnJoin ( const DBNUM _dwDBNum, GLLandMan* _pLand, IN OUT D3DXVECTOR3& _vAdjustPos );
		virtual void								CALLBACK OnOut ( const DBNUM _dwDBNum );
		virtual void								CALLBACK OnMapEnter ( const DBNUM _dwDBNum, const SNATIVEID& _sBaseMapID );
		virtual void								CALLBACK OnPartyChangeMaster ( const GLPartyID& _sPartyID, const bool _bExpedition, const DBNUM _dwNewMasterDBNum, const bool _bInSameInstanceNewMaster, const DBNUM _dwOldMasterDBNum, const bool _bInSameInstanceOldMaster );
		virtual void								CALLBACK OnPartyJoin ( const GLPartyID& _sPartyID, const bool _bExpedition, const DBNUM _dwJoinerDBNum, const bool _bInSameInstance );
		virtual void								CALLBACK OnPartyOut ( const GLPartyID& _sPartyID, const bool _bExpedition, const DBNUM _dwOuterDBNum, const bool _bInSameInstance );
		virtual void								CALLBACK OnClickTrigger ( const DWORD _emNpcType, const DWORD _dwNpcID, const DWORD _emClickActorType, const DWORD _dwClickActorID );
		virtual void								CALLBACK OnRetryProc ( const DWORD _dwChaNum, bool _bAccept, const DWORD _dwResultFlag );
		virtual const bool							CALLBACK OnCompleteRetryProc ( const DWORD _emFactionType, const DWORD _dwFactionID, bool _bSuccess, const DWORD _dwResultFlag );
		virtual const bool							CALLBACK OnAllDie ( const DWORD _emFactionType, const DWORD _dwFactionID );
		virtual const bool							CALLBACK OnDie ( const DWORD _emDieActorType, const DWORD _dwDieActorID, const DWORD _emKillActorType, const DWORD _dwKillActorID );
		virtual const bool							CALLBACK OnReceiveDamage ( const DWORD _emDamagedActorType, const DWORD _dwDamagedActorID, const DWORD _emAttackActorType, const DWORD _dwAttackActorID, DWORD& _dwDamage, const DWORD _dwDamageFlag );
		virtual const bool							CALLBACK OnReceiveHeal ( const DWORD _emActorType, const DWORD _dwActorID, const DWORD _emReActorType, const DWORD _dwReActorID, DWORD& _dwHeal, const DWORD _dwHealFlag );
		virtual const bool							CALLBACK OnUseItem ( const DWORD _dwActorID, const DWORD _dwItemMID, const DWORD _dwItemSID, const DWORD _nParam = 0, const float _fParam = 0.0f );
		virtual const bool							CALLBACK OnResurrect ( const DWORD _emResurrectType, const DWORD _emDieActorType, const DWORD _dwDieActorID, const DWORD _emKillActorType, const DWORD _dwKillActorID );
		virtual const bool							CALLBACK OnSkill ( const DWORD _dwActorType, const DWORD _dwActorID );
		virtual void								CALLBACK OnMotionFinish ( const DWORD _dwActorType, const DWORD _dwActorID, const DWORD _dwMotionMID, const DWORD _dwMotionSID, const float _fRemainSec );
		virtual void								CALLBACK OnCustomMessage ( const DBNUM _dwDBNum, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 );
		virtual void								CALLBACK OnCustomMessageOuter ( const DBNUM _dwDBNum, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 );
		virtual void								CALLBACK OnCustomMessageFromAgent ( const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 );
		virtual void								CALLBACK OnCustomMessage ( const EM_CUSTOM_MESSAGE_TYPE _emSrcType, const DWORD _dwSrcID, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 );
		virtual const bool							CALLBACK OnUpdateVictoriousCountry ( DWORD _dwContentID, DWORD _dwPreCountryID, DWORD _dwCurCountryID );


	public:
		virtual const std::string					GetFactionName ( const DWORD _dwFactionID );

		virtual void								UI_CertificationMachineMode_Progress ( const DWORD _dwNameIndex, const DWORD _dwProgressIndex, const DWORD _dwPoint, const DWORD _dwTime );

		virtual const bool							UI_CertificationMachineMode_Minimap ( const DWORD _dwChaDBNum, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg );
		virtual const bool							UI_CertificationMachineMode_Minimap_All ( const SNATIVEID& _sMapID, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg );

		virtual void								UI_CertificationMachineMode_Result_Begin ( void );
		virtual void								UI_CertificationMachineMode_Result_Info ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwRanking, const DWORD _dwCertificationMachineCount, const DWORD _dwCertifyCount );
		virtual void								UI_CertificationMachineMode_Result_End ( void );

		virtual const bool							UI_CertificationMachineMode_Rank_MyInfo ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwRanking );
		virtual const bool							UI_CertificationMachineMode_Rank_Info ( const CertificationMachineMode::VEC_FACTION_POINT& _vecFactionPoint );

		virtual void								Log_CertificationMachineMode_In ( const DWORD _dwChaDBNum );
		virtual void								Log_CertificationMachineMode_Out ( const DWORD _dwChaDBNum );
		virtual void								Log_CertificationMachineMode_Result ( const DWORD _dw1stFactionID, const DWORD _dw1stPoint, const DWORD _dw1stCertificationMachineCount, const DWORD _dw1stCertifyCount, const DWORD _dw2stFactionID, const DWORD _dw2stPoint, const DWORD _dw2stCertificationMachineCount, const DWORD _dw2stCertifyCount, const DWORD _dw3stFactionID, const DWORD _dw3stPoint, const DWORD _dw3stCertificationMachineCount, const DWORD _dw3stCertifyCount );


	private:
		CInstanceDungeon_CertificationMachine		m_sCertificationMachineDungoen;


	public:
		CCertificationMachineClubMode ( GLGaeaServer* _pServer
			, GLLandManager* _pLandManager
			, MapID _sMapID
			, ManagerField* _pManagerField
			, InstanceField* _pInstanceField
			, sc::LuaInstance* _pLua
			, const SCERTIFICATION_MACHINE_DUNGEON_INFO& _sDungeonInfo )
			: IInstanceFieldMode ( _pServer
			, _pLandManager
			, _sMapID
			, _pManagerField
			, _pInstanceField
			, _pLua )
			, m_sCertificationMachineDungoen ( this, _sDungeonInfo )
		{

		}

		virtual ~CCertificationMachineClubMode ( void ) { }
	};

}