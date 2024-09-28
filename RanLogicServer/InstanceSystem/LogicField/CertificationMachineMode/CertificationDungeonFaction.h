#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* 인증기 던전 진영;                                                   */
	/************************************************************************/
	class CCertificationDungeonFaction
	{
	private:
		typedef		std::vector< DWORD >					VEC_CERTIFICATION_MACHINE;
		typedef		VEC_CERTIFICATION_MACHINE::iterator		VEC_CERTIFICATION_MACHINE_ITER;


	public:
		// 진영을 초기화한다;
		void											Initialize ( void );

		// 진영에 입장하거나 퇴장할 경우 처리한다;
		const bool										Join ( void );
		void											Out ( void );

		// 인증기의 인증을 시작하거나 성공할 때 처리한다;
		const bool										StartCertification ( const DWORD _dwMachineID );
		void											SuccessCertification ( const DWORD _dwMachineID );

		// 동기화를 하거나 동기화 메시지를 보낸다;
		void											Syncronize ( const DWORD _dwPoint, const DWORD _dwCertifyCount );
		void											SyncronizeCertificationMachine ( const DWORD _dwMachineID );
		void											SendSyncronizeMessage ( void ) const;

		// 인증기가 중립화 되었다;
		void											CertificationMachineNeutralization ( const DWORD _dwMachineID );

		// 진영에 속한 인증기의 AddPoint 상태를 활성화시킨다;
		void											ActivateAddPoint_CertificationMachine ( void );
		void											InactivateAddPoint_CertificationMachine ( void );


	public:
		// 진영의 AddPoint 상태를 활성화하거나 비활성화한다;
		inline void										ActivateAddPoint ( void ) { m_bAddPoint = true; }
		inline void										InactivateAddPoint ( void ) { m_bAddPoint = false; }

		// 진영의 AddPointBuff 부여 상태를 활성화하거나 비활성화한다;
		inline void										ActivateAddPointBuff ( void ) { m_bAddPointBuff = true; }
		inline void										InactivateAddPointBuff ( void ) { m_bAddPointBuff = false; }

		// 진영의 포인트를 갱신한다;
		inline void										UpdatePoint ( const DWORD _dwPoint ) { m_dwPoint += _dwPoint; }

		inline const DWORD								GetFactionID ( void ) const { return m_dwFactionID; }
		inline const DWORD								GetPoint ( void ) const { return m_dwPoint; }
		inline const DWORD								GetRealPoint ( void ) const { return m_dwPoint + m_dwCertifyCount + m_vecCertificationMachine.size(); }
		inline const DWORD								GetMemberCount ( void ) const { return m_dwMemberCount; }
		inline const DWORD								GetCertifyCount ( void ) const { return m_dwCertifyCount; }
		inline const DWORD								GetCertificationMachineCount ( void ) const { return m_vecCertificationMachine.size(); }
		inline const bool								IsValidation ( void ) const { return m_bValidation; }
		inline const bool								IsAddPoint ( void ) const { return m_bAddPoint; }
		inline const bool								IsAddPointBuff ( void ) const { return m_bAddPointBuff; }


	private:
		DWORD											m_dwFactionID;						// 진영 ID;

		DWORD											m_dwMemberCount;					// 멤버수;
		DWORD											m_dwPoint;							// 포인트;
		DWORD											m_dwCertifyCount;					// 인증횟수;

		bool											m_bAddPoint;						// 추가포인트 상태;
		bool											m_bAddPointBuff;					// 추가포인트 버프 부여 상태;

		bool											m_bValidation;						// 진영의 유효성;

		VEC_CERTIFICATION_MACHINE						m_vecCertificationMachine;			// 진영이 인증한 인증기 목록;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	public:
		CCertificationDungeonFaction ( IInstanceFieldMode* _pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon
			, const DWORD _dwFactionID )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon )
			, m_dwFactionID ( _dwFactionID )
			, m_dwMemberCount ( 0 )
			, m_dwPoint ( 0 )
			, m_dwCertifyCount ( 0 )
			, m_bAddPoint ( false )
			, m_bAddPointBuff ( false )
			, m_bValidation ( false )
		{

		}

		CCertificationDungeonFaction ( const CCertificationDungeonFaction& _sInfo )
			: m_pInstanceField ( _sInfo.m_pInstanceField )
			, m_pDungeon ( _sInfo.m_pDungeon )
			, m_dwFactionID ( _sInfo.m_dwFactionID )
			, m_dwMemberCount ( _sInfo.m_dwMemberCount )
			, m_dwPoint ( _sInfo.m_dwPoint )
			, m_dwCertifyCount ( _sInfo.m_dwCertifyCount )
			, m_bAddPoint ( _sInfo.m_bAddPoint )
			, m_bAddPointBuff ( _sInfo.m_bAddPointBuff )
			, m_bValidation ( _sInfo.m_bValidation )
		{

		}

		~CCertificationDungeonFaction ( void ) { }
	};




	/************************************************************************/
	/* 인증기 던전 진영 관리자;                                           */
	/************************************************************************/
	class CCertificationDungeonFactionManager
	{
	private:
		typedef		std::map< DWORD, CCertificationDungeonFaction >			MAP_CERTIFICATION_DUNGEON_FACTION;
		typedef		MAP_CERTIFICATION_DUNGEON_FACTION::iterator				MAP_CERTIFICATION_DUNGEON_FACTION_ITER;
		typedef		MAP_CERTIFICATION_DUNGEON_FACTION::value_type			MAP_CERTIFICATION_DUNGEON_FACTION_VALUE;


	public:
		// 진영에 입장하거나 퇴장할 경우 처리한다;
		void											Join ( const DWORD _dwFactionID, const bool _bClub );
		void											Out ( const DWORD _dwFactionID );

		// 인증을 시작하거나 성공할 경우 처리한다;
		const bool										StartCertification ( const DWORD _dwFactionID, const DWORD _dwMachineID, const DWORD _dwCapturedFactionID );
		void											SuccessCertification ( const DWORD _dwFactionID, const DWORD _dwMachineID );

		// AddPoint 상태를 활성화하거나 비활성화할 경우 처리한다;
		void											ActivateAddPoint ( const DWORD _dwFactionID );
		void											InactivateAddPoint ( const DWORD _dwFactionID );

		// AddPointBuff 부여 상태를 활성화하거나 비활성화할 경우 처리한다;
		void											ActivateAddPointBuff ( const DWORD _dwFactionID );
		void											InactivateAddPointBuff ( const DWORD _dwFactionID );

		// 진영에 속한 인증기의 AddPoint 상태를 활성화시킨다;
		void											ActivateAddPoint_CertificationMachine ( const DWORD _dwFactionID );

		// Point 를 갱신한다;
		const bool										UpdatePoint ( const CertificationMachineMode::MAP_FACTION_POINT& _mapFactionPoint );
		const bool										UpdatePointDirect ( const DWORD _dwFactionID, const DWORD _dwPoint );

		// 게임을 종료하거나 보상로직을 진행한다;
		void											GameEnd ( void );
		const bool										GameReward ( void );

		// 동기화하거나 동기화 메시지를 보낸다;
		void											Syncronize ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwCertifyCount );
		void											SyncronizeCertificationMachine ( const DWORD _dwMachineID, const DWORD _dwFactionID );
		void											SendSyncronizeMessage ( void );

		// 인증기가 중립화 되었다;
		void											CertificationMachineNeutralization ( const DWORD _dwFactionID, const DWORD _dwMachineID );


	public:
		const bool										IsAddPoint ( const DWORD _dwFactionID );
		const bool										IsAddPointBuff ( const DWORD _dwFactionID );
		const DWORD										GetPoint ( const DWORD _dwFactionID );
		const DWORD										GetCertifyTime ( const DWORD _dwFactionID );


	public:
		// 진영 최초입장일 경우 진영관계를 초기화한다;
		void											_InitailizeRelation ( const DWORD _dwFactionID, const bool _bClub );

		// 모든 진영들을 서로 중립화시킨다;
		void											_NeutralizationAllFaction ( void );

		// 포인트가 갱신되면 랭킹을 정렬한다;
		const bool										_SortFactionRanking ( void );


	private:
		MAP_CERTIFICATION_DUNGEON_FACTION				m_mapCertificationDungeonFaction;		// 진영정보 맵;

		CertificationMachineMode::VEC_FACTION_POINT		m_vecFactionPoint;						// 진영 랭킹정보 리스트;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	public:
		CCertificationDungeonFactionManager ( IInstanceFieldMode* _pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon ) { }

		~CCertificationDungeonFactionManager ( void ) { }
	};

}