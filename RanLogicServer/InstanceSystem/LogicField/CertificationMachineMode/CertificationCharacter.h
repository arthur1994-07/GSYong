#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

#include "./CertificationDungeonFaction.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* ĳ����;                                                              */
	/************************************************************************/
	class CCertificationCharacter
	{
	public:
		// ĳ���Ͱ� �ʿ� �����Ѵ�;
		void											EnterMap ( const SNATIVEID _sEnterMapID, const bool _bAddPoint );

		// ĳ������ �����������¸� �����Ѵ�;
		void											ReleaseSafeFaction ( void );

		// ĳ���Ͱ� ������ �����ϰų� �Ϸ��߰ų� �������� ��츦 ó���Ѵ�;
		const bool										StartCertification ( const DWORD _dwMachineID, const DWORD _dwCertifyTime );
		void											EndCertification ( void );
		void											SuccessCertification ( const DWORD _dwMachineID );

		// ĳ���Ϳ��� ���ӱ� ������ �ɾ��ְų� �����Ѵ�;
		void											ActivateAcceleratorBuff ( void );
		void											InactivateAceeleratorBuff ( void );

		// ĳ���Ϳ��� ���� ������ �ɾ��ش�;
		void											ActivateJoinBuff ( const DWORD _dwTime );
		void											InactivateJoinBuff ( void );

		// ĳ���Ϳ��� ���� ������� �ɾ��ش�;
		void											ActivateJoinDebuff ( const DWORD _dwTime );
		void											InactivateJoinDebuff ( void );

		// ������ ����Ǿ���;
		void											GameEnd ( void );

		// ���������� Ȯ���Ѵ�;
		const bool										IsCertifying ( void );

		// ĳ���Ͱ� ��Ҵ��� Ȯ���Ѵ�;
		const bool										IsDie ( void );


	public:
		// ĳ������ �����̳� ������¸� �����Ѵ�;
		inline void										Join ( const DWORD _dwFactionID ) { m_bValidation = true; m_bCanCertification = true; m_dwCertifyingMachine = MAXDWORD; m_dwFactionID = _dwFactionID; }
		inline void										Out ( void ) { m_bValidation = false; m_bCanCertification = false; m_dwCertifyingMachine = MAXDWORD; }

		// ĳ���Ͱ� ������ �� �� �ִ� ���¸� �����ϰų� �����Ѵ�;
		inline void										LockCertification ( void ) { m_bCanCertification = false; }
		inline void										UnlockCertification ( void ) { m_bCanCertification = true; }


	public:
		inline const DWORD								GetChaDBNum ( void ) const { return m_dwChaDBNum; }
		inline const DWORD								GetFactionID ( void ) const { return m_dwFactionID; }
		inline const DWORD								GetCertifyingMachine ( void ) const { return m_dwCertifyingMachine; }
		inline const bool								CanCertification ( void ) const { return m_bCanCertification; }
		inline const bool								IsValidation ( void ) const { return m_bValidation; }


	private:
		DWORD											m_dwCertifyingMachine;				// ���� ĳ���Ͱ� �������� ������ ID;
		bool											m_bCanCertification;				// ĳ������ �������� ����;

		bool											m_bValidation;						// ĳ������ ��ȿ������;
		bool											m_bAlive;							// ĳ������ ��������;

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
	/* ĳ���� ������;                                                      */
	/************************************************************************/
	class CCertificationCharacterManager
	{
	private:
		typedef		std::map< DWORD, CCertificationCharacter >			MAP_CERTIFICATION_CHARACTER;
		typedef		MAP_CERTIFICATION_CHARACTER::iterator				MAP_CERTIFICATION_CHARACTER_ITER;
		typedef		MAP_CERTIFICATION_CHARACTER::value_type				MAP_CERTIFICATION_CHARACTER_VALUE;


	public:
		// ĳ������ �����̳� ������¸� �����Ѵ�;
		void											Join ( const DWORD _dwChaDBNum, const DWORD _dwFactionID, const bool _bClub );
		void											Out ( const DWORD _dwChaDBNum );

		// ĳ���Ͱ� �ʿ� �����Ѵ�;
		void											EnterMap ( const DWORD _dwChaDBNum, const SNATIVEID _sEnterMapID );

		// ĳ���Ͱ� ������ �����ϰų� �Ϸ��߰ų� �������� ��츦 ó���Ѵ�;
		const bool										StartCertification ( const DWORD _dwChaDBNum, const DWORD _dwMachineID, const DWORD _dwCapturedFactionID );
		void											EndCertification ( const DWORD _dwChaDBNum );
		void											SuccessCertification ( const DWORD _dwChaDBNum );

		// ĳ������ �����������¸� �����Ѵ�;
		void											ReleaseSafeFaction ( const DWORD _dwChaDBNum );
		void											UnlockCertification ( const DWORD _dwChaDBNum );

		// ���ӱ� ������ ��ų� �����Ѵ�;
		void											CaptureAcceleratorBuff ( const DWORD _dwChaDBNum );
		void											ExitAcceleratorBuff ( const DWORD _dwFactionID );

		// ������ ����Ʈ�� ���ŵǴ� ������ ȣ��ȴ�;
		const bool										UpdatePoint ( const CertificationMachineMode::MAP_FACTION_POINT& _mapFactionPoint );
		const bool										UpdatePointDirect ( const DWORD _dwFactionID, const DWORD _dwPoint );

		// ������ ����ǰų� ��������� ����� �� ȣ��ȴ�;
		void											GameEnd ( void );
		const bool										GameReward ( void );

		// ����ȭ�� �ϰų� ����ȭ �޽����� ������;
		void											SyncronizeFactionInfo ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwCertifyCount );
		void											SyncronizeCertificationMachine ( const DWORD _dwMachineID, const DWORD _dwFactionID );
		void											SendSyncronizeMessage ( void );

		// �����Ⱑ �߸�ȭ �Ǿ���;
		void											CertificationMachineNeutralization ( const DWORD _dwFactionID, const DWORD _dwMachineID );


	public:
		const DWORD										GetCertifyingMachine ( const DWORD _dwChaDBNum );
		const DWORD										GetCertifyingFactionID ( const DWORD _dwChaDBNum );
		const bool										IsAddPointFaction ( const DWORD _dwChaDBNum );

		const DWORD										GetPointFaction ( const DWORD _dwFactionID );

		const bool										IsEmpty ( void );


	private:
		MAP_CERTIFICATION_CHARACTER						m_mapCertificationCharacter;		// ĳ���� ����Ʈ;

		CCertificationDungeonFactionManager				m_sFactionManager;					// ����������;

		int												m_hTimerAcceleratorBuff;			// ���ӱ� ���� ���� Ÿ�̸�;


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