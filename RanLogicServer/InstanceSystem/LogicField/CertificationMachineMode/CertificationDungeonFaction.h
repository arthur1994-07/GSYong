#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* ������ ���� ����;                                                   */
	/************************************************************************/
	class CCertificationDungeonFaction
	{
	private:
		typedef		std::vector< DWORD >					VEC_CERTIFICATION_MACHINE;
		typedef		VEC_CERTIFICATION_MACHINE::iterator		VEC_CERTIFICATION_MACHINE_ITER;


	public:
		// ������ �ʱ�ȭ�Ѵ�;
		void											Initialize ( void );

		// ������ �����ϰų� ������ ��� ó���Ѵ�;
		const bool										Join ( void );
		void											Out ( void );

		// �������� ������ �����ϰų� ������ �� ó���Ѵ�;
		const bool										StartCertification ( const DWORD _dwMachineID );
		void											SuccessCertification ( const DWORD _dwMachineID );

		// ����ȭ�� �ϰų� ����ȭ �޽����� ������;
		void											Syncronize ( const DWORD _dwPoint, const DWORD _dwCertifyCount );
		void											SyncronizeCertificationMachine ( const DWORD _dwMachineID );
		void											SendSyncronizeMessage ( void ) const;

		// �����Ⱑ �߸�ȭ �Ǿ���;
		void											CertificationMachineNeutralization ( const DWORD _dwMachineID );

		// ������ ���� �������� AddPoint ���¸� Ȱ��ȭ��Ų��;
		void											ActivateAddPoint_CertificationMachine ( void );
		void											InactivateAddPoint_CertificationMachine ( void );


	public:
		// ������ AddPoint ���¸� Ȱ��ȭ�ϰų� ��Ȱ��ȭ�Ѵ�;
		inline void										ActivateAddPoint ( void ) { m_bAddPoint = true; }
		inline void										InactivateAddPoint ( void ) { m_bAddPoint = false; }

		// ������ AddPointBuff �ο� ���¸� Ȱ��ȭ�ϰų� ��Ȱ��ȭ�Ѵ�;
		inline void										ActivateAddPointBuff ( void ) { m_bAddPointBuff = true; }
		inline void										InactivateAddPointBuff ( void ) { m_bAddPointBuff = false; }

		// ������ ����Ʈ�� �����Ѵ�;
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
		DWORD											m_dwFactionID;						// ���� ID;

		DWORD											m_dwMemberCount;					// �����;
		DWORD											m_dwPoint;							// ����Ʈ;
		DWORD											m_dwCertifyCount;					// ����Ƚ��;

		bool											m_bAddPoint;						// �߰�����Ʈ ����;
		bool											m_bAddPointBuff;					// �߰�����Ʈ ���� �ο� ����;

		bool											m_bValidation;						// ������ ��ȿ��;

		VEC_CERTIFICATION_MACHINE						m_vecCertificationMachine;			// ������ ������ ������ ���;


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
	/* ������ ���� ���� ������;                                           */
	/************************************************************************/
	class CCertificationDungeonFactionManager
	{
	private:
		typedef		std::map< DWORD, CCertificationDungeonFaction >			MAP_CERTIFICATION_DUNGEON_FACTION;
		typedef		MAP_CERTIFICATION_DUNGEON_FACTION::iterator				MAP_CERTIFICATION_DUNGEON_FACTION_ITER;
		typedef		MAP_CERTIFICATION_DUNGEON_FACTION::value_type			MAP_CERTIFICATION_DUNGEON_FACTION_VALUE;


	public:
		// ������ �����ϰų� ������ ��� ó���Ѵ�;
		void											Join ( const DWORD _dwFactionID, const bool _bClub );
		void											Out ( const DWORD _dwFactionID );

		// ������ �����ϰų� ������ ��� ó���Ѵ�;
		const bool										StartCertification ( const DWORD _dwFactionID, const DWORD _dwMachineID, const DWORD _dwCapturedFactionID );
		void											SuccessCertification ( const DWORD _dwFactionID, const DWORD _dwMachineID );

		// AddPoint ���¸� Ȱ��ȭ�ϰų� ��Ȱ��ȭ�� ��� ó���Ѵ�;
		void											ActivateAddPoint ( const DWORD _dwFactionID );
		void											InactivateAddPoint ( const DWORD _dwFactionID );

		// AddPointBuff �ο� ���¸� Ȱ��ȭ�ϰų� ��Ȱ��ȭ�� ��� ó���Ѵ�;
		void											ActivateAddPointBuff ( const DWORD _dwFactionID );
		void											InactivateAddPointBuff ( const DWORD _dwFactionID );

		// ������ ���� �������� AddPoint ���¸� Ȱ��ȭ��Ų��;
		void											ActivateAddPoint_CertificationMachine ( const DWORD _dwFactionID );

		// Point �� �����Ѵ�;
		const bool										UpdatePoint ( const CertificationMachineMode::MAP_FACTION_POINT& _mapFactionPoint );
		const bool										UpdatePointDirect ( const DWORD _dwFactionID, const DWORD _dwPoint );

		// ������ �����ϰų� ��������� �����Ѵ�;
		void											GameEnd ( void );
		const bool										GameReward ( void );

		// ����ȭ�ϰų� ����ȭ �޽����� ������;
		void											Syncronize ( const DWORD _dwFactionID, const DWORD _dwPoint, const DWORD _dwCertifyCount );
		void											SyncronizeCertificationMachine ( const DWORD _dwMachineID, const DWORD _dwFactionID );
		void											SendSyncronizeMessage ( void );

		// �����Ⱑ �߸�ȭ �Ǿ���;
		void											CertificationMachineNeutralization ( const DWORD _dwFactionID, const DWORD _dwMachineID );


	public:
		const bool										IsAddPoint ( const DWORD _dwFactionID );
		const bool										IsAddPointBuff ( const DWORD _dwFactionID );
		const DWORD										GetPoint ( const DWORD _dwFactionID );
		const DWORD										GetCertifyTime ( const DWORD _dwFactionID );


	public:
		// ���� ���������� ��� �������踦 �ʱ�ȭ�Ѵ�;
		void											_InitailizeRelation ( const DWORD _dwFactionID, const bool _bClub );

		// ��� �������� ���� �߸�ȭ��Ų��;
		void											_NeutralizationAllFaction ( void );

		// ����Ʈ�� ���ŵǸ� ��ŷ�� �����Ѵ�;
		const bool										_SortFactionRanking ( void );


	private:
		MAP_CERTIFICATION_DUNGEON_FACTION				m_mapCertificationDungeonFaction;		// �������� ��;

		CertificationMachineMode::VEC_FACTION_POINT		m_vecFactionPoint;						// ���� ��ŷ���� ����Ʈ;


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