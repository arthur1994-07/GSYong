#pragma once

#include "./InstanceDungeon_CertificationMachine_Define.h"

namespace InstanceSystem
{

	class IInstanceFieldMode;
	class CInstanceDungeon_CertificationMachine;

	/************************************************************************/
	/* 인증기 데이터;                                                      */
	/************************************************************************/
	struct SCERTIFICATION_MACHINE_INFO
	{
		SNATIVEID										ID_CROW;							// 인증기의 Crow ID;
		SNATIVEID										ID_MAP;								// 인증기가 위치한 Map ID;
		SNATIVEID										ID_CERTIFY_EFFECT_BUFF;				// 인증 성공 이펙트 버프 스킬;
		D3DXVECTOR3										POSITION;							// 인증기의 위치;
		float											ROTATE;								// 인증기의 Rotate;
		DWORD											NUM_POINT;							// 인증기의 획득 Point;
		DWORD											NUM_POINT_COUNT;					// 인증기의 Point 획득 횟수;
		DWORD											NUM_ADD_POINT_COUNT;				// 인증기의 추가 Point 획득 횟수;

		std::string										STR_CERTIFICATION_EFFECT;			// 인증 성공 Effect;
		std::string										STR_UNCERTIFICATION_EFFECT;			// 인증 해제 Effect;

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
	/* 인증기;                                                             */
	/************************************************************************/
	class CCertificationMachine
	{
	public:
		// 인증기를 생성한다;
		void											CreateMachine ( void );

		// 인증기를 인증한다;
		void											CertificationMachine ( const DWORD _dwFactionID, const DWORD _dwChaDBNum, const bool _bAddPoint );

		// 인증기에서 포인트를 생산시킨다;
		const DWORD										GeneratePoint ( void );

		// 인증기를 동기화한다;
		void											Syncronize ( const DWORD _dwFactionID, const DWORD _dwCurPointCount );

		// 인증기 동기화 메시지를 보낸다;
		void											SendSyncronizeMessage ( void ) const;


	private:
		// 인증기를 중립화시킨다;
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
		DWORD											m_dwID;								// 인증기의 고유 번호;

		SCERTIFICATION_MACHINE_INFO						m_sInfo;							// 인증기 정보;

		DWORD											m_dwCrowGaeaID;						// 인증기의 Gaea ID;
		DWORD											m_dwMaxHP;							// 인증기의 Max HP;

		DWORD											m_dwFactionID;						// 인증한 진영 ID;
		DWORD											m_dwCurPointCount;					// 현재 포인트 획득 횟수;
		DWORD											m_dwMaxPointCount;					// 현재의 최대 포인트 획득 횟수;

		bool											m_bAddPoint;						// AddPoint 상태인지 확인;

		bool											m_bValidation;						// 인증기 상태;


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
	/* 인증기 관리자;                                                      */
	/************************************************************************/
	class CCertificationMachineManager
	{
	private:
		typedef		std::vector< CCertificationMachine >		VEC_CERTIFICATION_MACHINE;
		typedef		VEC_CERTIFICATION_MACHINE::iterator			VEC_CERTIFICATION_MACHINE_ITER;


	public:
		// 인증기를 추가한다;
		void											AddCertificationMachine ( const SCERTIFICATION_MACHINE_INFO& _sMachineInfo );

		// 추가된 인증기를 전부 생성한다;
		void											CreateAllCertificationMachine ();

		// 인증기의 GaeaID 를 이용해 인증기의 ID 를 구한다;
		const DWORD										GetCertificationMachineID ( const DWORD _dwGaeaID );

		// 인증기의 ID 를 이용해 인증기의 GaeaID 를 구한다;
		const DWORD										GetCertificationMachineGaeaID ( const DWORD _dwMachineID );

		// 인증기를 인증하고 있는 진영을 구한다;
		const DWORD										GetCapturedFaction ( const DWORD _dwMachineID );

		// 인증기를 인증상태로 변경한다;
		void											CertificationMachine ( const DWORD _dwMachineID, const DWORD _dwFactionID, const DWORD _dwChaDBNum, const bool _bAddPoint );

		// 특정진영이 인증한 인증기들의 포인트를 계산한다;
		void											GenerateFactionPoint ( CertificationMachineMode::MAP_FACTION_POINT& _refMapFactionPoint );

		// 인증기를 동기화한다;
		void											Syncronize ( const DWORD _dwMachineID, const DWORD _dwFactionID, const DWORD _dwCurPointCount );

		// 인증기 동기화 메시지를 보낸다;
		void											SendSyncronizeMessage ( void );

		// 인증기 미니맵 정보를 세팅한다;
		void											SetMinimapInfo ( const SNATIVEID& _sMapID, GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC* _pNetMsg );

		// 인증기가 인증 가능한 상태인지 확인한다;
		const bool										IsValidation ( const DWORD _dwMachineID );

		// 인증기를 인증 가능하거나 불가능한 상태로 바꾼다;
		void											ActivateMachine ( const DWORD _dwMachineID );
		void											InctivateMachine ( const DWORD _dwMachineID );

		// 인증기의 AddPoint 상태를 활성화한다;
		void											ActivateAddPoint ( const DWORD _dwMachineID );
		void											InactivateAddPoint ( const DWORD _dwMachineID );

		// 인증기 인증 상태를 확인한다;
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