#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* ������ ������;                                                      */
	/************************************************************************/
	struct SCERTIFICATION_MACHINE_INFO
	{
		SNATIVEID										ID_CROW;							// �������� Crow ID;
		SNATIVEID										ID_MAP;								// �����Ⱑ ��ġ�� Map ID;
		SNATIVEID										ID_CERTIFY_EFFECT_BUFF;				// ���� ���� ����Ʈ ���� ��ų;
		D3DXVECTOR3										POSITION;							// �������� ��ġ;
		float											ROTATE;								// �������� Rotate;
		DWORD											NUM_POINT;							// �������� ȹ�� Point;
		DWORD											NUM_POINT_COUNT;					// �������� Point ȹ�� Ƚ��;
		DWORD											NUM_ADD_POINT_COUNT;				// �������� �߰� Point ȹ�� Ƚ��;

		std::string										STR_CERTIFICATION_EFFECT;			// ���� ���� Effect;
		std::string										STR_UNCERTIFICATION_EFFECT;			// ���� ���� Effect;

		SCERTIFICATION_MACHINE_INFO ( const SNATIVEID& _sCrowID
			, const SNATIVEID& _sMapID
			, const SNATIVEID& _sCertifyEffectBuff
			, const D3DXVECTOR3& _vPosition
			, const float _fRotate
			, const DWORD _dwPoint
			, const DWORD _dwPointCount
			, const DWORD _dwAddPointCount
			, const std::string& _strCertificationEffect
			, const std::string& _strUncertificationEffect )
			: ID_CROW ( _sCrowID )
			, ID_MAP ( _sMapID )
			, ID_CERTIFY_EFFECT_BUFF ( _sCertifyEffectBuff )
			, POSITION ( _vPosition )
			, ROTATE ( _fRotate )
			, NUM_POINT ( _dwPoint )
			, NUM_POINT_COUNT ( _dwPointCount )
			, NUM_ADD_POINT_COUNT ( _dwAddPointCount )
			, STR_CERTIFICATION_EFFECT ( _strCertificationEffect )
			, STR_UNCERTIFICATION_EFFECT ( _strUncertificationEffect )
		{

		}

		SCERTIFICATION_MACHINE_INFO ( const SCERTIFICATION_MACHINE_INFO& _sInfo )
			: ID_CROW ( _sInfo.ID_CROW )
			, ID_MAP ( _sInfo.ID_MAP )
			, ID_CERTIFY_EFFECT_BUFF ( _sInfo.ID_CERTIFY_EFFECT_BUFF )
			, POSITION ( _sInfo.POSITION )
			, ROTATE ( _sInfo.ROTATE )
			, NUM_POINT ( _sInfo.NUM_POINT )
			, NUM_POINT_COUNT ( _sInfo.NUM_POINT_COUNT )
			, NUM_ADD_POINT_COUNT ( _sInfo.NUM_ADD_POINT_COUNT )
			, STR_CERTIFICATION_EFFECT ( _sInfo.STR_CERTIFICATION_EFFECT )
			, STR_UNCERTIFICATION_EFFECT ( _sInfo.STR_UNCERTIFICATION_EFFECT )
		{

		}

		~SCERTIFICATION_MACHINE_INFO ( void ) { }
	};

	typedef		std::vector< SCERTIFICATION_MACHINE_INFO >			VEC_CERTIFICATION_INFO;
	typedef		VEC_CERTIFICATION_INFO::iterator					VEC_CERTIFICATION_INFO_ITER;




	/************************************************************************/
	/* ������;                                                             */
	/************************************************************************/
	class CCertificationMachine
	{
	public:
		// �����⸦ �����Ѵ�;
		void											CreateMachine ( void );

		// �����⸦ �����Ѵ�;
		void											CertificationMachine ( const DWORD _dwFactionID, const DWORD _dwChaDBNum, const bool _bAddPoint );

		// �����⿡�� ����Ʈ�� �����Ų��;
		const DWORD										GeneratePoint ( void );

		// �����⸦ ����ȭ�Ѵ�;
		void											Syncronize ( const DWORD _dwFactionID, const DWORD _dwCurPointCount );

		// ������ ����ȭ �޽����� ������;
		void											SendSyncronizeMessage ( void ) const;


	private:
		// �����⸦ �߸�ȭ��Ų��;
		void											_Neutralization ( void );


	public:
		inline const DWORD								GetID ( void ) const { return m_dwID; }
		inline const DWORD								GetPoint ( void ) const { return m_sInfo.NUM_POINT; }
		inline const DWORD								GetGaeaID ( void ) const { return m_dwCrowGaeaID; }
		inline const DWORD								GetMaxHP ( void ) const { return m_dwMaxHP; }
		inline const DWORD								GetFactionID ( void ) const { return m_dwFactionID; }
		inline const D3DXVECTOR3&						GetPosition ( void ) const { return m_sInfo.POSITION; }
		inline const SNATIVEID&							GetMapID ( void ) const { return m_sInfo.ID_MAP; }

		inline const bool								IsValidation ( void ) const { return m_bValidation; }

		inline void										SetFactionID ( const DWORD _dwFactionID ) { m_dwFactionID = _dwFactionID; }
		
		inline void										ActivateMachine ( void ) { m_bValidation = true; }
		inline void										InactivateMachine ( void ) { m_bValidation = false; }

		inline void										ActivateAddPoint ( void ) { m_bAddPoint = true; }
		inline void										InactivateAddPoint ( void ) { m_bAddPoint = false; }


	private:
		DWORD											m_dwID;								// �������� ���� ��ȣ;

		SCERTIFICATION_MACHINE_INFO						m_sInfo;							// ������ ����;

		DWORD											m_dwCrowGaeaID;						// �������� Gaea ID;
		DWORD											m_dwMaxHP;							// �������� Max HP;

		DWORD											m_dwFactionID;						// ������ ���� ID;
		DWORD											m_dwCurPointCount;					// ���� ����Ʈ ȹ�� Ƚ��;
		DWORD											m_dwMaxPointCount;					// ������ �ִ� ����Ʈ ȹ�� Ƚ��;

		bool											m_bAddPoint;						// AddPoint �������� Ȯ��;

		bool											m_bValidation;						// ������ ����;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	public:
		CCertificationMachine ( IInstanceFieldMode*	_pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon
			, const DWORD _dwID
			, const SCERTIFICATION_MACHINE_INFO& _sInfo )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon )
			, m_dwID ( _dwID )
			, m_sInfo ( _sInfo )
			, m_dwCrowGaeaID ( GAEAID_NULL )
			, m_dwMaxHP ( 0 )
			, m_dwFactionID ( MAXDWORD )
			, m_dwCurPointCount ( 0 )
			, m_dwMaxPointCount ( 0 )
			, m_bAddPoint ( false )
			, m_bValidation ( true )
		{

		}

		CCertificationMachine ( const CCertificationMachine& _sInfo )
			: m_pInstanceField ( _sInfo.m_pInstanceField )
			, m_pDungeon ( _sInfo.m_pDungeon )
			, m_dwID ( _sInfo.m_dwID )
			, m_sInfo ( _sInfo.m_sInfo )
			, m_dwCrowGaeaID ( _sInfo.m_dwCrowGaeaID )
			, m_dwMaxHP ( _sInfo.m_dwMaxHP )
			, m_dwFactionID ( _sInfo.m_dwFactionID )
			, m_dwCurPointCount ( _sInfo.m_dwCurPointCount )
			, m_dwMaxPointCount ( _sInfo.m_dwMaxPointCount )
			, m_bAddPoint ( _sInfo.m_bAddPoint )
			, m_bValidation ( _sInfo.m_bValidation )
		{

		}

		~CCertificationMachine ( void ) { }
	};




	/************************************************************************/
	/* ������ ������;                                                      */
	/************************************************************************/
	class CCertificationMachineManager
	{
	private:
		typedef		std::vector< CCertificationMachine >		VEC_CERTIFICATION_MACHINE;
		typedef		VEC_CERTIFICATION_MACHINE::iterator			VEC_CERTIFICATION_MACHINE_ITER;


	public:
		// �����⸦ �߰��Ѵ�;
		void											AddCertificationMachine ( const SCERTIFICATION_MACHINE_INFO& _sMachineInfo );

		// �߰��� �����⸦ ���� �����Ѵ�;
		void											CreateAllCertificationMachine ();

		// �������� GaeaID �� �̿��� �������� ID �� ���Ѵ�;
		const DWORD										GetCertificationMachineID ( const DWORD _dwGaeaID );

		// �������� ID �� �̿��� �������� GaeaID �� ���Ѵ�;
		const DWORD										GetCertificationMachineGaeaID ( const DWORD _dwMachineID );

		// �����⸦ �����ϰ� �ִ� ������ ���Ѵ�;
		const DWORD										GetCapturedFaction ( const DWORD _dwMachineID );

		// �����⸦ �������·� �����Ѵ�;
		void											CertificationMachine ( const DWORD _dwMachineID, const DWORD _dwFactionID, const DWORD _dwChaDBNum, const bool _bAddPoint );

		// Ư�������� ������ ��������� ����Ʈ�� ����Ѵ�;
		void											GenerateFactionPoint ( CertificationMachineMode::MAP_FACTION_POINT& _refMapFactionPoint );

		// �����⸦ ����ȭ�Ѵ�;
		void											Syncronize ( const DWORD _dwMachineID, const DWORD _dwFactionID, const DWORD _dwCurPointCount );

		// ������ ����ȭ �޽����� ������;
		void											SendSyncronizeMessage ( void );

		// ������ �̴ϸ� ������ �����Ѵ�;
		void											SetMinimapInfo ( const SNATIVEID& _sMapID, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg );

		// �����Ⱑ ���� ������ �������� Ȯ���Ѵ�;
		const bool										IsValidation ( const DWORD _dwMachineID );

		// �����⸦ ���� �����ϰų� �Ұ����� ���·� �ٲ۴�;
		void											ActivateMachine ( const DWORD _dwMachineID );
		void											InctivateMachine ( const DWORD _dwMachineID );

		// �������� AddPoint ���¸� Ȱ��ȭ�Ѵ�;
		void											ActivateAddPoint ( const DWORD _dwMachineID );
		void											InactivateAddPoint ( const DWORD _dwMachineID );

		// ������ ���� ���¸� Ȯ���Ѵ�;
		void											ConfirmCertification ( void );


	private:
		VEC_CERTIFICATION_MACHINE						m_vecCertificationMachine;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	private:
		const DWORD										NUM_ONCE_LIMIT_POINT;


	public:
		CCertificationMachineManager ( IInstanceFieldMode* _pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon
			, const DWORD _dwOnceLimitPoint )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon )
			, NUM_ONCE_LIMIT_POINT ( _dwOnceLimitPoint ) { }

		~CCertificationMachineManager ( void ) { }
	};

}