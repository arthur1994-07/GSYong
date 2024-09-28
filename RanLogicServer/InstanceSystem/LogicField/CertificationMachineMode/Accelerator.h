#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* 가속기 데이터;                                                      */
	/************************************************************************/
	struct SACCELERATOR_INFO
	{
		SNATIVEID										ID_CROW;							// 가속기의 Crow ID;
		SNATIVEID										ID_EFFECT;							// 가속기 충전 Effect ID;
		SNATIVEID										ID_MAP;								// 가속기가 위치한 Map ID;

		D3DXVECTOR3										POSITION;							// 가속기의 위치;
		float											ROTATE;								// 가속기의 Rotate;

		DWORD											NUM_POINT;							// 획득 포인트;

		std::string										STR_EFFECT1;						// 가속기 파괴 Effect1;
		std::string										STR_EFFECT2;						// 가속기 파괴 Effect2;

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
	/* 가속기;                                                             */
	/************************************************************************/
	class CAccelerator
	{
	public:
		// 가속기를 생성한다;
		void											CreateMachine ( void );

		// 가속기를 활성화하거나 비활성화한다;
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
		DWORD											m_dwID;								// 가속기의 고유 번호;

		SACCELERATOR_INFO								m_sInfo;							// 가속기 정보;

		DWORD											m_dwCrowGaeaID;						// 가속기의 Gaea ID;

		bool											m_bActivate;						// 가속기 활성화 상태;


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
	/* 가속기 관리자;                                                      */
	/************************************************************************/
	class CAcceleratorManager
	{
	private:
		typedef		std::vector< CAccelerator >			VEC_ACCELERATOR;
		typedef		VEC_ACCELERATOR::iterator			VEC_ACCELERATOR_ITER;


	public:
		// 가속기를 추가한다;
		void											AddAccelerator ( const SACCELERATOR_INFO& _sMachineInfo );

		// 추가된 가속기를 전부 생성한다;
		void											CreateAllAccelerator ( void );

		// 가속기를 활성화상태로 변경한다;
		void											ActivateAllAccelerator ( void );

		// 가속기를 점령시킨다;
		const bool										CaptureTheAccelerator ( const DWORD _dwAcceleratorGaeaID );

		// 가속기의 증가 포인트값을 가져온다;
		const DWORD										GetPointAccelerator ( const DWORD _dwAcceleratorGaeaID );

		// 가속기 미니맵 정보를 세팅한다;
		void											SetMinimapInfo ( const SNATIVEID& _sMapID, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg );


	private:
		VEC_ACCELERATOR									m_vecAccelerator;					// 가속기 목록;

		bool											m_bActivate;						// 가속기들의 활성상태;


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