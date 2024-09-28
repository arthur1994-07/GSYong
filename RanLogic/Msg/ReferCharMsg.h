#pragma once

#include "../../SigmaCore/gassert.h"

#include "../Character/GLCharData.h"
#include "../s_NetGlobal.h"

namespace ReferChar
{
	enum EReferResult
	{
		EReferSuccess,	// 성공
		EReferLogout,	// 접속중이 아니다
		EReferPrivate,	// 비공개이다
	};

	struct SCharInfo
	{
		char m_szTitle[ EM_TITLEID_MAXLENGTH ];

		EMCHARCLASS m_emClass;
		WORD m_wSchool;
		WORD m_wSex;
		WORD m_wHair;
		WORD m_wHairColor;
		WORD m_wFace;

		WORD m_wLevel;

		SCHARSTATS m_sStats;

		int m_powerAttack;	// 기본 공격력
		int m_powerDefnece;	// 기본 방어력
		int m_powerAttribute[SKILL::EMAPPLY_NSIZE];

		int m_nHP;
		int m_nMP;
		int m_nSP;

		SCharInfo()
			: m_emClass( GLCC_NONE )
			, m_wSchool( 0 )
			, m_wSex( 0 )
			, m_wHair( 0 )
			, m_wHairColor( 0 )
			, m_wFace( 0 )

			, m_wLevel( 1 )

			, m_powerAttack( 0 )
			, m_powerDefnece( 0 )

			, m_nHP( 0 )
			, m_nMP( 0 )
			, m_nSP( 0 )
		{			
			memset( m_szTitle, 0, sizeof( char ) * EM_TITLEID_MAXLENGTH );
			memset(m_powerAttribute, 0x00L, sizeof(int) * SKILL::EMAPPLY_NSIZE);
		}

		void SetTitle( const char* szTitle )
		{
			StringCchCopy( m_szTitle, EM_TITLEID_MAXLENGTH, szTitle );
		}
	};

	struct SCharInfoAddition
	{
		float m_fCritical;
		float m_fCrushingBlow;

		float m_fMoveSpeed;
		float m_fAtkSpeed;

		float m_fExpRate;

		float m_fINCR_HP_TOTAL;
		float m_fINCR_MP_TOTAL;
		float m_fINCR_SP_TOTAL;
		int m_nReceive_CP_Inc;

		int m_nPotion_HP_Inc;
		int m_nPotion_MP_Inc;
		int m_nPotion_SP_Inc;

		SCharInfoAddition()
			: m_fCritical( 0 )
			, m_fCrushingBlow( 0 )
			, m_fMoveSpeed( 0 )
			, m_fAtkSpeed( 0 )
			, m_fExpRate( 0 )
			, m_fINCR_HP_TOTAL( 0 )
			, m_fINCR_MP_TOTAL( 0 )
			, m_fINCR_SP_TOTAL( 0 )
			, m_nReceive_CP_Inc( 0 )
			, m_nPotion_HP_Inc( 0 )
			, m_nPotion_MP_Inc( 0 )
			, m_nPotion_SP_Inc( 0 )
		{}
	};

	struct SNET_MSG_REFER_CHAR_PUBLIC : public NET_MSG_GENERIC
	{
		bool m_bPublic;

		SNET_MSG_REFER_CHAR_PUBLIC()
			: m_bPublic( true )
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_PUBLIC );
            nType = NET_MSG_REFER_CHAR_PUBLIC;
            MIN_STATIC_ASSERT( sizeof( SNET_MSG_REFER_CHAR_PUBLIC ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_MSG_REFER_CHAR_BASE : public NET_MSG_GENERIC
	{
		char m_szTargetName[ CHAR_SZNAME ];
		DWORD m_nRequestCharDbNum;
		DWORD m_dwTargetGaeaId;

		SNET_MSG_REFER_CHAR_BASE()
			: m_nRequestCharDbNum( 0 )
			, m_dwTargetGaeaId(0)
		{
			memset( m_szTargetName, 0, sizeof( char ) * CHAR_SZNAME );
		}

		void SetTargetName( const char* szTargetName )
		{
			StringCchCopy( m_szTargetName, CHAR_SZNAME, szTargetName );
		}
	};

	struct SNET_MSG_REFER_CHAR_REQ : public SNET_MSG_REFER_CHAR_BASE
	{
        bool m_bForce;

		SNET_MSG_REFER_CHAR_REQ()
            : m_bForce(false)
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_REQ );
            nType = NET_MSG_REFER_CHAR_REQ;
            MIN_STATIC_ASSERT( sizeof( SNET_MSG_REFER_CHAR_REQ ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_MSG_REFER_CHAR_FB : public SNET_MSG_REFER_CHAR_BASE
	{
		EReferResult m_eResult;

		SNET_MSG_REFER_CHAR_FB()
			: m_eResult( EReferSuccess )
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_FB );
            nType = NET_MSG_REFER_CHAR_FB;
            MIN_STATIC_ASSERT( sizeof( SNET_MSG_REFER_CHAR_FB ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SPutonItem
	{
		BYTE m_nSlot;
		SITEMCUSTOM m_sItem;

		SPutonItem()
			: m_nSlot( SLOT_TSIZE )
		{
		}

		SPutonItem( BYTE nSlot, SITEMCUSTOM& sItem )
			: m_nSlot( nSlot )
			, m_sItem( sItem )
		{
		}
	};

	struct SNET_MSG_REFER_CHAR_PUTON : public SNET_MSG_REFER_CHAR_BASE
	{
		// 패킷이 버퍼사이즈 초과로 9개 -> 3개로 바꿤..쪼개서 보냄
		enum { EMaxCount = 3 };
		//enum { EMaxCount = 9 }; 

		WORD m_nNum;
		SPutonItem m_sPuton[ EMaxCount ];

		SNET_MSG_REFER_CHAR_PUTON()
			: m_nNum( 0 )
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_PUTON );
            nType = NET_MSG_REFER_CHAR_PUTON;
            MIN_STATIC_ASSERT( sizeof( SNET_MSG_REFER_CHAR_PUTON ) <= NET_DATA_BUFSIZE );
		}

		void Push( const SPutonItem& sData )
		{
			if( m_nNum < EMaxCount )
				m_sPuton[ m_nNum++ ] = sData;
		}

		void Resize()
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_PUTON );
			dwSize -= sizeof( SPutonItem ) * ( EMaxCount - m_nNum );
		}
	};

	struct SNET_MSG_REFER_CHAR_SKILL : public SNET_MSG_REFER_CHAR_BASE
	{
		enum { EMaxCount = 100 };

		WORD m_nNum;
		SCHARSKILL m_sSkill[ EMaxCount ];

		SNET_MSG_REFER_CHAR_SKILL()
			: m_nNum( 0 )
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_SKILL );
            nType = NET_MSG_REFER_CHAR_SKILL;
            MIN_STATIC_ASSERT( sizeof( SNET_MSG_REFER_CHAR_SKILL ) <= NET_DATA_BUFSIZE );
		}

		void Push( const SCHARSKILL& sData )
		{
			if( m_nNum < EMaxCount )
				m_sSkill[ m_nNum++ ] = sData;
		}

		void Resize()
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_SKILL );
			dwSize -= sizeof( SCHARSKILL ) * ( EMaxCount - m_nNum );
		}
	};

	struct SNET_MSG_REFER_CHAR_INFO : public SNET_MSG_REFER_CHAR_BASE
	{
		SCharInfo m_sInfo;
		SCharInfoAddition m_sInfoAddition;

		SNET_MSG_REFER_CHAR_INFO()
		{
			dwSize = sizeof( SNET_MSG_REFER_CHAR_INFO );
            nType = NET_MSG_REFER_CHAR_INFO;
            MIN_STATIC_ASSERT( sizeof( SNET_MSG_REFER_CHAR_INFO ) <= NET_DATA_BUFSIZE );
		}
	};
}
