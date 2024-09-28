#pragma once

#include "../IInstanceDungeonMode.h"

#include "./CertificationMachine.h"
#include "./Accelerator.h"
#include "./CertificationCharacter.h"

namespace InstanceSystem
{
	/************************************************************************/
	/* 보상 아이템;                                                        */
	/************************************************************************/
	struct SREWARD_ITEM
	{
		SNATIVEID											ITEM_REWARD;
		DWORD												NUM_ITEM_REWARD;
		DWORD												INDEX_POST_CONTENTS_STRING;
		DWORD												NUM_MINIMUM_POINT;
		DWORD												NUM_MAXIMUM_POINT;

		SREWARD_ITEM ( void )
			: ITEM_REWARD ( false )
			, NUM_ITEM_REWARD ( 0 )
			, INDEX_POST_CONTENTS_STRING ( 0 )
			, NUM_MINIMUM_POINT ( 0 )
			, NUM_MAXIMUM_POINT ( 0 )
		{

		}

		SREWARD_ITEM ( const SREWARD_ITEM& _sInfo )
			: ITEM_REWARD ( _sInfo.ITEM_REWARD )
			, NUM_ITEM_REWARD ( _sInfo.NUM_ITEM_REWARD )
			, INDEX_POST_CONTENTS_STRING ( _sInfo.INDEX_POST_CONTENTS_STRING )
			, NUM_MINIMUM_POINT ( _sInfo.NUM_MINIMUM_POINT )
			, NUM_MAXIMUM_POINT ( _sInfo.NUM_MAXIMUM_POINT )
		{

		}
	};

	typedef		std::vector< SREWARD_ITEM >					VEC_REWARD_ITEM;
	typedef		VEC_REWARD_ITEM::iterator					VEC_REWARD_ITEM_ITER;




	/************************************************************************/
	/* 인증기 던전 정보;                                                   */
	/************************************************************************/
	struct SCERTIFICATION_MACHINE_DUNGEON_INFO
	{
		int												TIME_ACCELERATOR_COOLTIME;
		int												TIME_ACCELERATOR_NOTIFY_COOLTIME;
		int												TIME_GAME_START_HELP_NOTIFY_TIME;
		int												TIME_CERTIFY_POINT_TIME;
		int												TIME_SAFE_TIME;
		int												TIME_ACCELERATOR_BUFFTIME;
		int												TIME_CERTIFY_WAITTIME;
		int												TIME_CERTIFICATION_MACHINE_LOCKTIME;
		int												TIME_CERTIFY_TIME;
		int												TIME_GAME_DESTROY_WAITTIME;
		int												TIME_RESURRECTION_WAITTIME;
		int												TIME_JOIN_BUFFTIME;
		int												TIME_JOIN_DEBUFFTIME;
		int												TIME_CONFIRM_CERTIFICATION_TIME;

		DWORD											NUM_TARGET_POINT;
		DWORD											NUM_APPROACH_POINT;
		DWORD											NUM_ONCE_LIMIT_POINT;
		DWORD											NUM_MINIMUM_REWARD_POINT;

		DWORD											NUM_PROGRESS_UI_INDEX;
		DWORD											NUM_MINIMAP_ICON_TYPE;
		std::string										STR_INSTANCE_DUNGEON_NAME;

		SNATIVEID										MAP_LOBBY;
		SNATIVEID										MAP_BATTLE;

		SNATIVEID										MOTION_CERTIFY;

		SNATIVEID										SKILL_CERTIFY_ADD_POINT_BUFF;
		SNATIVEID										SKILL_RESURRECTION_TIME_REDUCE_BUFF;
		SNATIVEID										SKILL_CERTIFY_TIME_REDUCE_BUFF;

		VEC_DWORD										VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING;

		VEC_NATIVEID									VEC_REMOVE_BUFF_BY_CERTIFYING;
		VEC_NATIVEID									VEC_ACCELERATOR_BUFF;
		VEC_NATIVEID									VEC_JOIN_BUFF;
		VEC_NATIVEID									VEC_JOIN_DEBUFF;

		VEC_REWARD_ITEM									VEC_REWARD_WIN;
		VEC_REWARD_ITEM									VEC_REWARD_PARTICIPANT;

		VEC_CERTIFICATION_INFO							VEC_CERTIFICATION_MACHINE_LIST;
		VEC_ACCELERATOR_INFO							VEC_ACCELERATOR_LIST;

		const bool										LoadConfig ( sc::LuaInstance& _refLuaInstance );

		SCERTIFICATION_MACHINE_DUNGEON_INFO ( void ) { }

		SCERTIFICATION_MACHINE_DUNGEON_INFO ( const SCERTIFICATION_MACHINE_DUNGEON_INFO& _sInfo )
			: TIME_ACCELERATOR_COOLTIME ( _sInfo.TIME_ACCELERATOR_COOLTIME )
			, TIME_ACCELERATOR_NOTIFY_COOLTIME ( _sInfo.TIME_ACCELERATOR_NOTIFY_COOLTIME )
			, TIME_GAME_START_HELP_NOTIFY_TIME ( _sInfo.TIME_GAME_START_HELP_NOTIFY_TIME )
			, TIME_CERTIFY_POINT_TIME ( _sInfo.TIME_CERTIFY_POINT_TIME )
			, TIME_SAFE_TIME ( _sInfo.TIME_SAFE_TIME )
			, TIME_ACCELERATOR_BUFFTIME ( _sInfo.TIME_ACCELERATOR_BUFFTIME )
			, TIME_CERTIFY_WAITTIME ( _sInfo.TIME_CERTIFY_WAITTIME )
			, TIME_CERTIFICATION_MACHINE_LOCKTIME ( _sInfo.TIME_CERTIFICATION_MACHINE_LOCKTIME )
			, TIME_CERTIFY_TIME ( _sInfo.TIME_CERTIFY_TIME )
			, TIME_GAME_DESTROY_WAITTIME ( _sInfo.TIME_GAME_DESTROY_WAITTIME )
			, TIME_RESURRECTION_WAITTIME ( _sInfo.TIME_RESURRECTION_WAITTIME )
			, TIME_JOIN_BUFFTIME ( _sInfo.TIME_JOIN_BUFFTIME )
			, TIME_JOIN_DEBUFFTIME ( _sInfo.TIME_JOIN_DEBUFFTIME )
			, TIME_CONFIRM_CERTIFICATION_TIME ( _sInfo.TIME_CONFIRM_CERTIFICATION_TIME )
			, NUM_TARGET_POINT ( _sInfo.NUM_TARGET_POINT )
			, NUM_APPROACH_POINT ( _sInfo.NUM_APPROACH_POINT )
			, NUM_ONCE_LIMIT_POINT ( _sInfo.NUM_ONCE_LIMIT_POINT )
			, NUM_MINIMUM_REWARD_POINT ( _sInfo.NUM_MINIMUM_REWARD_POINT )
			, STR_INSTANCE_DUNGEON_NAME ( _sInfo.STR_INSTANCE_DUNGEON_NAME )
			, NUM_PROGRESS_UI_INDEX ( _sInfo.NUM_PROGRESS_UI_INDEX )
			, NUM_MINIMAP_ICON_TYPE ( _sInfo.NUM_MINIMAP_ICON_TYPE )
			, MAP_LOBBY ( _sInfo.MAP_LOBBY )
			, MAP_BATTLE ( _sInfo.MAP_BATTLE )
			, MOTION_CERTIFY ( _sInfo.MOTION_CERTIFY )
			, SKILL_CERTIFY_ADD_POINT_BUFF ( _sInfo.SKILL_CERTIFY_ADD_POINT_BUFF )
			, SKILL_RESURRECTION_TIME_REDUCE_BUFF ( _sInfo.SKILL_RESURRECTION_TIME_REDUCE_BUFF )
			, SKILL_CERTIFY_TIME_REDUCE_BUFF ( _sInfo.SKILL_CERTIFY_TIME_REDUCE_BUFF )
		{
			/// 스킬효과 목록 세팅;
			for ( int i=0; i<_sInfo.VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.size(); ++i )
			{
				const DWORD dwSkillEffect = _sInfo.VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.at( i );

				VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.push_back( dwSkillEffect );
			}
			

			/// 버프 목록 세팅;
			for ( int i=0; i<_sInfo.VEC_ACCELERATOR_BUFF.size(); ++i )
			{
				const SNATIVEID& sBuff = _sInfo.VEC_ACCELERATOR_BUFF.at( i );

				VEC_ACCELERATOR_BUFF.push_back( sBuff );
			}

			for ( int i=0; i<_sInfo.VEC_REMOVE_BUFF_BY_CERTIFYING.size(); ++i )
			{
				const SNATIVEID& sBuff = _sInfo.VEC_REMOVE_BUFF_BY_CERTIFYING.at( i );

				VEC_REMOVE_BUFF_BY_CERTIFYING.push_back( sBuff );
			}

			for ( int i=0; i<_sInfo.VEC_JOIN_BUFF.size(); ++i )
			{
				const SNATIVEID& sBuff = _sInfo.VEC_JOIN_BUFF.at( i );

				VEC_JOIN_BUFF.push_back( sBuff );
			}

			for ( int i=0; i<_sInfo.VEC_JOIN_DEBUFF.size(); ++i )
			{
				const SNATIVEID& sBuff = _sInfo.VEC_JOIN_DEBUFF.at( i );

				VEC_JOIN_DEBUFF.push_back( sBuff );
			}


			/// 보상 아이템 목록 세팅;
			for ( int i=0; i<_sInfo.VEC_REWARD_WIN.size(); ++i )
			{
				const SREWARD_ITEM& sBuff = _sInfo.VEC_REWARD_WIN.at( i );

				VEC_REWARD_WIN.push_back( sBuff );
			}

			for ( int i=0; i<_sInfo.VEC_REWARD_PARTICIPANT.size(); ++i )
			{
				const SREWARD_ITEM& sBuff = _sInfo.VEC_REWARD_PARTICIPANT.at( i );

				VEC_REWARD_PARTICIPANT.push_back( sBuff );
			}


			/// 인증기, 가속기 세팅;
			for ( int i=0; i<_sInfo.VEC_CERTIFICATION_MACHINE_LIST.size(); ++i )
			{
				const SCERTIFICATION_MACHINE_INFO& sInfo = _sInfo.VEC_CERTIFICATION_MACHINE_LIST.at( i );

				VEC_CERTIFICATION_MACHINE_LIST.push_back( sInfo );
			}

			for ( int i=0; i<_sInfo.VEC_ACCELERATOR_LIST.size(); ++i )
			{
				const SACCELERATOR_INFO& sInfo = _sInfo.VEC_ACCELERATOR_LIST.at( i );

				VEC_ACCELERATOR_LIST.push_back( sInfo );
			}
		}

		~SCERTIFICATION_MACHINE_DUNGEON_INFO ( void ) { }
	};




	/************************************************************************/
	/* 인증기 던전;                                                        */
	/************************************************************************/
	class CInstanceDungeon_CertificationMachine : public IInstanceDungeonMode
	{
	public:
		// 인증기 던전의 Custom Message Flag;
		enum
		{
			EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_POINT = IInstanceDungeonMode::EMCUSTOM_MSG_NEXT,
			EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_CERTIFYMACHINE,
			EMCUSTOM_MSG_CERTIFICATION_MODE_UPDATE_PROGRESS_TIME,

			EMCUSTOM_MSG_CERTIFICATION_MODE_CREATEMAP_FA,
			EMCUSTOM_MSG_CERTIFICATION_MODE_JOIN_FA,
			EMCUSTOM_MSG_CERTIFICATION_MODE_OUT_FA,
			EMCUSTOM_MSG_CERTIFICATION_MODE_BATTLE_CLOSE_FA,
			EMCUSTOM_MSG_CERTIFICATION_MODE_BATTLE_CLOSE_WIN_FA,
			EMCUSTOM_MSG_CERTIFICATION_MODE_NEXT,
		};


	public:
		const bool										CALLBACK OnInitialize ( const bool _bClubFaction );
		const bool										CALLBACK OnJoin ( const DWORD _dwChaDBNum, const DWORD _dwID );
		const bool										CALLBACK OnOut ( const DWORD _dwChaDBNum );
		const bool										CALLBACK OnEnterMap ( const DWORD _dwChaDBNum, const SNATIVEID _sEnterMapID );

		const bool										CALLBACK OnStartCertification ( const DWORD _dwMachineGaeaID, const DWORD _dwChaDBNum );
		const bool										CALLBACK OnEndCertification ( const DWORD _dwChaDBNum, const DWORD _dwRemainMotionSec );

		const bool										CALLBACK OnTimer ( DWORD _dwHandle, DWORD _dwParam0, DWORD _dwParam1, DWORD _dwParam2, DWORD _dwParam3 );
		const bool										CALLBACK OnDie ( const Faction::GLActorID _sDieActorID, const Faction::GLActorID _sKillActorID );

		const bool										CALLBACK OnCustomMessage ( const DWORD _dwChaDBNum, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3 );


	public:
		const DWORD										GetPointFaction ( const DWORD _dwFactionID );
		const DWORD										GetDurationTime ( void );


	public:
		inline const DWORD								GetAllTime ( void ) const { return m_dwAllTime; }
		inline const bool								IsPlay ( void ) const { return m_bPlay; }
		inline const bool								IsInitialize ( void ) const { return m_bInitialize; }
		inline const bool								IsInitializeBattleMap ( void ) const { return m_bInitializeBattleMap; }
		inline const bool								IsClubFaction ( void ) const { return m_bClubFaction; }
		inline const bool								IsJoinBuff ( void ) const { return m_bJoinBuff; }
		inline const bool								IsJoinDebuff ( void ) const { return m_bJoinDebuff; }
		inline const std::string&						GetInstanceDungeonName ( void ) const { return STR_INSTANCE_DUNGEON_NAME; }


	private:
		void											_Initialize ( const int _nProgressTime, const int _nAlltime, const int _nOpenCount );
		void											_InitializeBattleMap ( void );

		void											_GameEnd ( void );
		void											_DestroyInstanceDungeon ( void );
		void											_ReleaseSafeFaction ( const DWORD _dwChaDBNum );
		void											_UnlockCertification ( const DWORD _dwChaDBNum );
		void											_UnlockCertificationMachine ( const DWORD _dwMachineID );
		void											_UpdateCertificationMachine ( void );
		void											_RechargeAccelerator ( void );
		void											_ExitAcceleratorBuff ( const DWORD _dwFactionID );
		void											_CertificationMachineNeutralization ( const DWORD _dwFactionID, const DWORD _dwMachineID );
		void											_CertificationMachineActivateAddPoint ( const DWORD _dwMachineID );
		void											_CertificationMachineInactivateAddPoint ( const DWORD _dwMachineID );
		void											_RewardWin ( const Faction::FactionID& _sFactionID );
		void											_RewardParticipant ( const Faction::FactionID& _sFactionID, const DWORD _dwPoint );
		void											_ReleaseJoinBuff ( void );
		void											_ReleaseJoinDebuff ( void );
		void											_ConfirmCertification ( void );


	private:
		DWORD											m_dwAllTime;						// 인던의 전체 진행시간;

		bool											m_bInitialize;						// 초기화상태;
		bool											m_bInitializeBattleMap;				// 전장맵 초기화상태;
		bool											m_bPlay;							// 던전 진행상태;
		bool											m_bClubFaction;						// 진영이 클럽인지 확인;
		bool											m_bJoinBuff;						// 입장버프 유효상태;
		bool											m_bJoinDebuff;						// 입장디버프 유효상태;

		// 타이머 핸들;
		int												m_hTimerAcceleratorRecharge;
		int												m_hTimerCertifyPoint;
		int												m_hTimerGameDuration;

		// 매니저;
		CCertificationCharacterManager					m_sCharacterManager;
		CCertificationMachineManager					m_sCertificationMachineManager;
		CAcceleratorManager								m_sAcceleratorManager;


	public:
		// 설정 데이터;
		const int										TIME_ACCELERATOR_COOLTIME;
		const int										TIME_ACCELERATOR_NOTIFY_COOLTIME;
		const int										TIME_GAME_START_HELP_NOTIFY_TIME;
		const int										TIME_CERTIFY_POINT_TIME;
		const int										TIME_SAFE_TIME;
		const int										TIME_ACCELERATOR_BUFFTIME;
		const int										TIME_CERTIFY_WAITTIME;
		const int										TIME_CERTIFICATION_MACHINE_LOCKTIME;
		const int										TIME_CERTIFY_TIME;
		const int										TIME_GAME_DESTROY_WAITTIME;
		const int										TIME_RESURRECTION_WAITTIME;
		const int										TIME_JOIN_BUFFTIME;
		const int										TIME_JOIN_DEBUFFTIME;
		const int										TIME_CONFIRM_CERTIFICATION_TIME;

		const DWORD										NUM_TARGET_POINT;
		const DWORD										NUM_APPROACH_POINT;
		const DWORD										NUM_MINIMUM_REWARD_POINT;

		const std::string								STR_INSTANCE_DUNGEON_NAME;
		const DWORD										NUM_PROGRESS_UI_INDEX;
		const DWORD										NUM_MINIMAP_ICON_TYPE;

		const SNATIVEID									MAP_LOBBY;
		const SNATIVEID									MAP_BATTLE;

		const SNATIVEID									MOTION_CERTIFY;

		const SNATIVEID									SKILL_CERTIFY_ADD_POINT_BUFF;
		const SNATIVEID									SKILL_RESURRECTION_TIME_REDUCE_BUFF;
		const SNATIVEID									SKILL_CERTIFY_TIME_REDUCE_BUFF;

		VEC_DWORD										VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING;

		VEC_NATIVEID									VEC_REMOVE_BUFF_BY_CERTIFYING;
		VEC_NATIVEID									VEC_ACCELERATOR_BUFF;
		VEC_NATIVEID									VEC_JOIN_BUFF;
		VEC_NATIVEID									VEC_JOIN_DEBUFF;

		VEC_REWARD_ITEM									VEC_REWARD_WIN;
		VEC_REWARD_ITEM									VEC_REWARD_PARTICIPANT;


	public:
		CInstanceDungeon_CertificationMachine ( IInstanceFieldMode* _pInstanceField
			, const SCERTIFICATION_MACHINE_DUNGEON_INFO& _nDungeonInfo )
			: IInstanceDungeonMode ( _pInstanceField )
			, m_dwAllTime ( 0 )
			, m_bInitialize ( false )
			, m_bInitializeBattleMap ( false )
			, m_bPlay ( false )
			, m_bClubFaction ( false )
			, m_bJoinBuff ( false )
			, m_bJoinDebuff ( false )

			, m_hTimerAcceleratorRecharge ( 0 )
			, m_hTimerCertifyPoint ( 0 )
			, m_hTimerGameDuration ( 0 )

			, m_sCharacterManager ( _pInstanceField, this )
			, m_sCertificationMachineManager ( _pInstanceField, this, _nDungeonInfo.NUM_ONCE_LIMIT_POINT )
			, m_sAcceleratorManager ( _pInstanceField, this )

			, TIME_ACCELERATOR_COOLTIME ( _nDungeonInfo.TIME_ACCELERATOR_COOLTIME )
			, TIME_ACCELERATOR_NOTIFY_COOLTIME ( _nDungeonInfo.TIME_ACCELERATOR_NOTIFY_COOLTIME )
			, TIME_GAME_START_HELP_NOTIFY_TIME ( _nDungeonInfo.TIME_GAME_START_HELP_NOTIFY_TIME )
			, TIME_CERTIFY_POINT_TIME ( _nDungeonInfo.TIME_CERTIFY_POINT_TIME )
			, TIME_SAFE_TIME ( _nDungeonInfo.TIME_SAFE_TIME )
			, TIME_ACCELERATOR_BUFFTIME ( _nDungeonInfo.TIME_ACCELERATOR_BUFFTIME )
			, TIME_CERTIFY_WAITTIME ( _nDungeonInfo.TIME_CERTIFY_WAITTIME )
			, TIME_CERTIFICATION_MACHINE_LOCKTIME ( _nDungeonInfo.TIME_CERTIFICATION_MACHINE_LOCKTIME )
			, TIME_CERTIFY_TIME ( _nDungeonInfo.TIME_CERTIFY_TIME )
			, TIME_GAME_DESTROY_WAITTIME ( _nDungeonInfo.TIME_GAME_DESTROY_WAITTIME )
			, TIME_RESURRECTION_WAITTIME ( _nDungeonInfo.TIME_RESURRECTION_WAITTIME )
			, TIME_JOIN_BUFFTIME ( _nDungeonInfo.TIME_JOIN_BUFFTIME )
			, TIME_JOIN_DEBUFFTIME ( _nDungeonInfo.TIME_JOIN_DEBUFFTIME )
			, TIME_CONFIRM_CERTIFICATION_TIME ( _nDungeonInfo.TIME_CONFIRM_CERTIFICATION_TIME )

			, NUM_TARGET_POINT ( _nDungeonInfo.NUM_TARGET_POINT )
			, NUM_APPROACH_POINT ( _nDungeonInfo.NUM_APPROACH_POINT )
			, NUM_MINIMUM_REWARD_POINT ( _nDungeonInfo.NUM_MINIMUM_REWARD_POINT )

			, STR_INSTANCE_DUNGEON_NAME ( _nDungeonInfo.STR_INSTANCE_DUNGEON_NAME )
			, NUM_PROGRESS_UI_INDEX ( _nDungeonInfo.NUM_PROGRESS_UI_INDEX )
			, NUM_MINIMAP_ICON_TYPE ( _nDungeonInfo.NUM_MINIMAP_ICON_TYPE )

			, MAP_LOBBY ( _nDungeonInfo.MAP_LOBBY )
			, MAP_BATTLE ( _nDungeonInfo.MAP_BATTLE )
			, MOTION_CERTIFY ( _nDungeonInfo.MOTION_CERTIFY )
			, SKILL_CERTIFY_ADD_POINT_BUFF ( _nDungeonInfo.SKILL_CERTIFY_ADD_POINT_BUFF )
			, SKILL_RESURRECTION_TIME_REDUCE_BUFF ( _nDungeonInfo.SKILL_RESURRECTION_TIME_REDUCE_BUFF )
			, SKILL_CERTIFY_TIME_REDUCE_BUFF ( _nDungeonInfo.SKILL_CERTIFY_TIME_REDUCE_BUFF )
		{
			/// 스킬효과 목록 세팅;
			for ( int i=0; i<_nDungeonInfo.VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.size(); ++i )
			{
				const DWORD dwSkillEffect = _nDungeonInfo.VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.at( i );

				VEC_REMOVE_SKILLEFFECT_BY_CERTIFYING.push_back( dwSkillEffect );
			}


			/// 버프 목록 세팅;
			for ( int i=0; i<_nDungeonInfo.VEC_ACCELERATOR_BUFF.size(); ++i )
			{
				const SNATIVEID& sBuff = _nDungeonInfo.VEC_ACCELERATOR_BUFF.at( i );

				VEC_ACCELERATOR_BUFF.push_back( sBuff );
			}

			for ( int i=0; i<_nDungeonInfo.VEC_REMOVE_BUFF_BY_CERTIFYING.size(); ++i )
			{
				const SNATIVEID& sBuff = _nDungeonInfo.VEC_REMOVE_BUFF_BY_CERTIFYING.at( i );

				VEC_REMOVE_BUFF_BY_CERTIFYING.push_back( sBuff );
			}

			for ( int i=0; i<_nDungeonInfo.VEC_JOIN_BUFF.size(); ++i )
			{
				const SNATIVEID& sBuff = _nDungeonInfo.VEC_JOIN_BUFF.at( i );

				VEC_JOIN_BUFF.push_back( sBuff );
			}

			for ( int i=0; i<_nDungeonInfo.VEC_JOIN_DEBUFF.size(); ++i )
			{
				const SNATIVEID& sBuff = _nDungeonInfo.VEC_JOIN_DEBUFF.at( i );

				VEC_JOIN_DEBUFF.push_back( sBuff );
			}


			/// 보상 아이템 목록 세팅;
			for ( int i=0; i<_nDungeonInfo.VEC_REWARD_WIN.size(); ++i )
			{
				const SREWARD_ITEM& sBuff = _nDungeonInfo.VEC_REWARD_WIN.at( i );

				VEC_REWARD_WIN.push_back( sBuff );
			}

			for ( int i=0; i<_nDungeonInfo.VEC_REWARD_PARTICIPANT.size(); ++i )
			{
				const SREWARD_ITEM& sBuff = _nDungeonInfo.VEC_REWARD_PARTICIPANT.at( i );

				VEC_REWARD_PARTICIPANT.push_back( sBuff );
			}


			/// 인증기, 가속기 세팅;
			for ( int i=0; i<_nDungeonInfo.VEC_CERTIFICATION_MACHINE_LIST.size(); ++i )
			{
				const SCERTIFICATION_MACHINE_INFO& sInfo = _nDungeonInfo.VEC_CERTIFICATION_MACHINE_LIST.at( i );

				m_sCertificationMachineManager.AddCertificationMachine( sInfo );
			}

			for ( int i=0; i<_nDungeonInfo.VEC_ACCELERATOR_LIST.size(); ++i )
			{
				const SACCELERATOR_INFO& sInfo = _nDungeonInfo.VEC_ACCELERATOR_LIST.at( i );

				m_sAcceleratorManager.AddAccelerator( sInfo );
			}
		}

		~CInstanceDungeon_CertificationMachine ( void ) { }
	};

}