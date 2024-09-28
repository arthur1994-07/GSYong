#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* ���ӱ� ������;                                                      */
	/************************************************************************/
	struct SACCELERATOR_INFO
	{
		SNATIVEID										ID_CROW;							// ���ӱ��� Crow ID;
		SNATIVEID										ID_EFFECT;							// ���ӱ� ���� Effect ID;
		SNATIVEID										ID_MAP;								// ���ӱⰡ ��ġ�� Map ID;

		D3DXVECTOR3										POSITION;							// ���ӱ��� ��ġ;
		float											ROTATE;								// ���ӱ��� Rotate;

		DWORD											NUM_POINT;							// ȹ�� ����Ʈ;

		std::string										STR_EFFECT1;						// ���ӱ� �ı� Effect1;
		std::string										STR_EFFECT2;						// ���ӱ� �ı� Effect2;

		SACCELERATOR_INFO ( const SNATIVEID& _sCrowID
			, const SNATIVEID& _sMapID
			, const SNATIVEID& _sChargingEffectID
			, const D3DXVECTOR3& _vPosition
			, const float _fRotate
			, const DWORD _dwPoint
			, const std::string& _strEffect1
			, const std::string& _strEffect2 )
			: ID_CROW ( _sCrowID )
			, ID_MAP ( _sMapID )
			, ID_EFFECT ( _sChargingEffectID )
			, POSITION ( _vPosition )
			, ROTATE ( _fRotate )
			, NUM_POINT ( _dwPoint )
			, STR_EFFECT1 ( _strEffect1 )
			, STR_EFFECT2 ( _strEffect2 )
		{

		}

		SACCELERATOR_INFO ( const SACCELERATOR_INFO& _sInfo )
			: ID_CROW ( _sInfo.ID_CROW )
			, ID_MAP ( _sInfo.ID_MAP )
			, ID_EFFECT ( _sInfo.ID_EFFECT )
			, POSITION ( _sInfo.POSITION )
			, ROTATE ( _sInfo.ROTATE )
			, NUM_POINT ( _sInfo.NUM_POINT )
			, STR_EFFECT1 ( _sInfo.STR_EFFECT1 )
			, STR_EFFECT2 ( _sInfo.STR_EFFECT2 )
		{

		}

		~SACCELERATOR_INFO ( void ) { }
	};

	typedef		std::vector< SACCELERATOR_INFO >		VEC_ACCELERATOR_INFO;
	typedef		VEC_ACCELERATOR_INFO::iterator			VEC_ACCELERATOR_INFO_ITER;




	/************************************************************************/
	/* ���ӱ�;                                                             */
	/************************************************************************/
	class CAccelerator
	{
	public:
		// ���ӱ⸦ �����Ѵ�;
		void											CreateMachine ( void );

		// ���ӱ⸦ Ȱ��ȭ�ϰų� ��Ȱ��ȭ�Ѵ�;
		void											Activate ( void );
		void											Inactivate ( void );


	public:
		inline const DWORD								GetID ( void ) const { return m_dwID; }
		inline const DWORD								GetGaeaID ( void ) const { return m_dwCrowGaeaID; }
		inline const bool								GetActivate ( void ) const { return m_bActivate; }
		inline const D3DXVECTOR3&						GetPosition ( void ) const { return m_sInfo.POSITION; }
		inline const DWORD								GetPoint ( void ) const { return m_sInfo.NUM_POINT; }
		inline const SNATIVEID&							GetMapID ( void ) const { return m_sInfo.ID_MAP; }


	private:
		DWORD											m_dwID;								// ���ӱ��� ���� ��ȣ;

		SACCELERATOR_INFO								m_sInfo;							// ���ӱ� ����;

		DWORD											m_dwCrowGaeaID;						// ���ӱ��� Gaea ID;

		bool											m_bActivate;						// ���ӱ� Ȱ��ȭ ����;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	public:
		CAccelerator ( IInstanceFieldMode*	_pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon
			, const DWORD _dwID
			, const SACCELERATOR_INFO& _sInfo )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon )
			, m_dwID ( _dwID )
			, m_sInfo ( _sInfo )
			, m_dwCrowGaeaID ( GAEAID_NULL )
			, m_bActivate ( false )
		{

		}

		CAccelerator ( const CAccelerator& _sInfo )
			: m_pInstanceField ( _sInfo.m_pInstanceField )
			, m_pDungeon ( _sInfo.m_pDungeon )
			, m_dwID ( _sInfo.m_dwID )
			, m_sInfo ( _sInfo.m_sInfo )
			, m_dwCrowGaeaID ( _sInfo.m_dwCrowGaeaID )
			, m_bActivate ( _sInfo.m_bActivate )
		{

		}

		~CAccelerator ( void ) { }
	};




	/************************************************************************/
	/* ���ӱ� ������;                                                      */
	/************************************************************************/
	class CAcceleratorManager
	{
	private:
		typedef		std::vector< CAccelerator >			VEC_ACCELERATOR;
		typedef		VEC_ACCELERATOR::iterator			VEC_ACCELERATOR_ITER;


	public:
		// ���ӱ⸦ �߰��Ѵ�;
		void											AddAccelerator ( const SACCELERATOR_INFO& _sMachineInfo );

		// �߰��� ���ӱ⸦ ���� �����Ѵ�;
		void											CreateAllAccelerator ( void );

		// ���ӱ⸦ Ȱ��ȭ���·� �����Ѵ�;
		void											ActivateAllAccelerator ( void );

		// ���ӱ⸦ ���ɽ�Ų��;
		const bool										CaptureTheAccelerator ( const DWORD _dwAcceleratorGaeaID );

		// ���ӱ��� ���� ����Ʈ���� �����´�;
		const DWORD										GetPointAccelerator ( const DWORD _dwAcceleratorGaeaID );

		// ���ӱ� �̴ϸ� ������ �����Ѵ�;
		void											SetMinimapInfo ( const SNATIVEID& _sMapID, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg );


	private:
		VEC_ACCELERATOR									m_vecAccelerator;					// ���ӱ� ���;

		bool											m_bActivate;						// ���ӱ���� Ȱ������;


	private:
		IInstanceFieldMode*								m_pInstanceField;

		CInstanceDungeon_CertificationMachine*			m_pDungeon;


	public:
		CAcceleratorManager ( IInstanceFieldMode* _pInstanceField
			, CInstanceDungeon_CertificationMachine* _pDungeon )
			: m_pInstanceField ( _pInstanceField )
			, m_pDungeon ( _pDungeon )
			, m_bActivate ( false ) { }

		~CAcceleratorManager ( void ) { }
	};

}