#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

#include "./CertificationDungeonFaction.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* 캐릭터;                                                              */
	/************************************************************************/
	class CCertificationCharacter
	{
	public:
		// 캐릭터가 맵에 입장한다;
		void											EnterMap ( const SNATIVEID _sEnterMapID, const bool _bAddPoint );

		// 캐릭터의 안전진영상태를 해제한다;
		void											ReleaseSafeFaction ( void );

		// 캐릭터가 인증을 시작하거나 완료했거나 성공했을 경우를 처리한다;
		const bool										StartCertification ( const DWORD _dwMachineID, const DWORD _dwCertifyTime );
		void											EndCertification ( void );
		void											SuccessCertification ( const DWORD _dwMachineID );

		// 캐릭터에게 가속기 버프를 걸어주거나 해제한다;
		void											ActivateAcceleratorBuff ( void );
		void											InactivateAceeleratorBuff ( void );

		// 캐릭터에게 입장 버프를 걸어준다;
		void											ActivateJoinBuff ( const DWORD _dwTime );
		void											InactivateJoinBuff ( void );

		// 캐릭터에게 입장 디버프를 걸어준다;
		void											ActivateJoinDebuff ( const DWORD _dwTime );
		void											InactivateJoinDebuff ( void );

		// 게임이 종료되었다;
		void											GameEnd ( void );

		// 인증중인지 확인한다;
		const bool										IsCertifying ( void );

		// 캐릭터가 살았는지 확인한다;
		const bool										IsDie ( void );


	public:
		// 캐릭터의 입장이나 퇴장상태를 관리한다;
		inline void										Join ( const DWORD _dwFactionID ) { m_bValidation = true; m_bCanCertification = true; m_dwCertifyingMachine = MAXDWORD; m_dwFactionID = _dwFactionID; }
		inline void										Out ( void ) { m_bValidation = false; m_bCanCertification = false; m_dwCertifyingMachine = MAXDWORD; }

		// 캐릭터가 인증을 할 수 있는 상태를 제한하거나 해제한다;
		inline void										LockCertification ( void ) { m_bCanCertification = false; }
		inline void										UnlockCertification ( void ) { m_bCanCertification = true; }


	public:
		inline const DWORD								GetChaDBNum ( void ) const { return m_dwChaDBNum; }
		inline const DWORD								GetFactionID ( void ) const { return m_dwFactionID; }
		inline const DWORD								GetCertifyingMachine ( void ) const { return m_dwCertifyingMachine; }
		inline const bool								CanCertification ( void ) const { return m_bCanCertification; }
		inline const bool								IsValidation ( void ) const { return m_bValidation; }


	private:
		DWORD											m_dwCertifyingMachine;				// 현재 캐릭터가 인증중인 인증기 ID;
		bool											m_bCanCertification;				// 캐릭터의 인증가능 여부;

		bool											m_bValidation;						// 캐릭터의 유효성상태;
		bool											m_bAlive;							// 캐릭터의 생존여부;

		DWORD											m_dwFactionID;


	private:
		const DWORD										m_dwChaDBNum;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	public:
		CCertificationCharacter ( IInstanceFieldMode* _pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon
			, const DWORD _dwChaDBNum
			, const DWORD _dwFactionID )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon )
			, m_dwChaDBNum ( _dwChaDBNum )
			, m_dwFactionID ( _dwFactionID )
			, m_dwCertifyingMachine ( MAXDWORD )
			, m_bCanCertification ( false )
			, m_bValidation ( false )
			, m_bAlive ( true )
		{

		}

		CCertificationCharacter ( const CCertificationCharacter& _sInfo )
			: m_pInstanceField ( _sInfo.m_pInstanceField )
			, m_pDungeon ( _sInfo.m_pDungeon )
			, m_dwChaDBNum ( _sInfo.m_dwChaDBNum )
			, m_dwFactionID ( _sInfo.m_dwFactionID )
			, m_dwCertifyingMachine ( _sInfo.m_dwCertifyingMachine )
			, m_bCanCertification ( _sInfo.m_bCanCertification )
			, m_bValidation ( _sInfo.m_bValidation )
			, m_bAlive ( _sInfo.m_bAlive )
		{

		}

		~CCertificationCharacter ( void ) { }
	};




	/************************************************************************/
	/* 캐릭터 관리자;                                                      */
	/************************************************************************/
	class CCertificationCharacterManager
	{
	private:
		typedef		std::map< DWORD, CCertificationCharacter >			MAP_CERTIFICATION_CHARACTER;
		typedef		MAP_CERTIFICATION_CHARACTER::iterator				MAP_CERTIFICATION_CHARACTER_ITER;
		typedef		MAP_CERTIFICATION_CHARACTER::value_type				MAP_CERTIFICATION_CHARACTER_VALUE;


	public:
		// 캐릭터의 입장이나 퇴장상태를 관리한다;
		void											Join ( const DWORD _dwChaDBNum, const DWORD _dwFactionID, const bool _bClub );
		void											Out ( const DWORD _dwChaDBNum );

		// 캐릭터가 맵에 입장한다;
		void											EnterMap ( const DWORD _dwChaDBNum, const SNATIVEID _sEnterMapID );

		// 캐릭터가 인증을 시작하거나 완료했거나 성공했을 경우를 처리한다;
		const bool										StartCertification ( const DWORD _dwChaDBNum, const DWORD _dwMachineID, const DWORD _dwCapturedFactionID );
		void											EndCertification ( const DWORD _dwChaDBNum );
		void											SuccessCertification ( const DWORD _dwChaDBNum );

		// 캐릭터의 안전진영상태를 해제한다;
		void											ReleaseSafeFaction ( const DWORD _dwChaDBNum );
		void											UnlockCertification ( const DWORD _dwChaDBNum );

		// 가속기 버프를 얻거나 종료한다;
		void											CaptureAcceleratorBuff ( const DWORD _dwChaDBNum );
		void											ExitAcceleratorBuff ( const DWORD _dwFactionID );

		// 게임의 포인트가 갱신되는 시점에 호출된다;
		const bool										UpdatePoint ( const CertificationMachineMode::MAP_FACTION_POINT& _mapFactionPoint );
		const bool										UpdatePointDirect ( const DWORD _dwFactionID, const DWORD _dwPoint );

		// 게임이 종료되거나 보상로직이 진행될 때 호출된다;
		void											GameEnd ( void );
		const bool										GameReward ( void );

		// 동기화를 하거나 동기화 메시지를 보낸다;
		void											SyncronizeFactionInfo ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwCertifyCount );
		void											SyncronizeCertificationMachine ( const DWORD _dwMachineID, const DWORD _dwFactionID );
		void											SendSyncronizeMessage ( void );

		// 인증기가 중립화 되었다;
		void											CertificationMachineNeutralization ( const DWORD _dwFactionID, const DWORD _dwMachineID );


	public:
		const DWORD										GetCertifyingMachine ( const DWORD _dwChaDBNum );
		const DWORD										GetCertifyingFactionID ( const DWORD _dwChaDBNum );
		const bool										IsAddPointFaction ( const DWORD _dwChaDBNum );

		const DWORD										GetPointFaction ( const DWORD _dwFactionID );

		const bool										IsEmpty ( void );


	private:
		MAP_CERTIFICATION_CHARACTER						m_mapCertificationCharacter;		// 캐릭터 리스트;

		CCertificationDungeonFactionManager				m_sFactionManager;					// 진영관리자;

		int												m_hTimerAcceleratorBuff;			// 가속기 버프 해제 타이머;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	public:
		CCertificationCharacterManager ( IInstanceFieldMode* _pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon )
			, m_sFactionManager ( _pInstanceField, _pDungeon )
			, m_hTimerAcceleratorBuff ( 0 ) { }

		~CCertificationCharacterManager ( void ) { }
	};

}