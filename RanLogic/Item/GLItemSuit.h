#ifndef _GL_ITEM_SUIT_H_
#define _GL_ITEM_SUIT_H_

#pragma once

#include "./GLItemDef.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

namespace ITEM
{
    GLITEM_ATT assign_att ( glold::GLITEM_ATT_102 emOld );
    GLITEM_ATT assign_att ( glold_103::GLITEM_ATT_103 emOld );
    GLITEM_ATT assign_att ( glold_104::GLITEM_ATT_104 emOld );
	GLITEM_ATT assign_att ( glold_105::GLITEM_ATT_105 emOld );
	GLITEM_ATT assign_att ( glold_108::GLITEM_ATT_108 emOld );

    struct SSATE_BLOW_100
    {
        EMSTATE_BLOW	emTYPE;		//	적용 타입.
        float			fRATE;		//	발생 확율.
        float			fLIFE;		//	지속 시간.
        float			fVALUE;		//	적용 값.
    };

    struct SSATE_BLOW
    {
        EMSTATE_BLOW	emTYPE;		//	적용 타입.
        float			fRATE;		//	발생 확율.
        float			fLIFE;		//	지속 시간.
        float			fVAR1;		//	적용 값 1.
        float			fVAR2;		//	적용 값 2.

        SSATE_BLOW ()
            : emTYPE(EMBLOW_NONE)
            , fRATE(0.0f)
            , fLIFE(0.0f)
            , fVAR1(0.0f)
            , fVAR2(0.0f)
        {
        }
    };

    enum	EMHAND
    {
        EMHAND_BOTHHAND		= 0x0001,
        EMHAND_BIG			= 0x0002,
        EMHAND_BROOM		= 0x0004,
    };

    struct SSUIT_100
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        BOOL		bBothHand;					//	양손 모두 장착가능.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        glold::GLITEM_ATT_102	emAttack;					//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        SRESIST		sResist;					//	저항값.

        WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

        //	특수 부가 효과들.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
        SVAR		sVARIATE;					//	변화율 효과.
        SSATE_BLOW_100	sBLOW;					//	상태 이상 효과.
    };

    struct SSUIT_101
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        BOOL		bBothHand;					//	양손 모두 장착가능.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        glold::GLITEM_ATT_102	emAttack;					//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        SRESIST		sResist;					//	저항값.

        WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

        //	특수 부가 효과들.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
        SVAR		sVARIATE;					//	변화율 효과.
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
    };

    struct SSUIT_102
    {
        enum { ADDON_SIZE = 4 };
        EMSUIT		emSuit;						//	아이템 착용 타입.
        bool		bBothHand;					//	양손 모두 장착가능.
        bool		bBig;						//	대형.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        glold::GLITEM_ATT_102	emAttack;		//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        SRESIST		sResist;					//	저항값.

        WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

        //	특수 부가 효과들.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
        SVAR		sVARIATE;					//	변화율 효과.
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
    };

    struct SSUIT_103
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        bool		bBothHand;					//	양손 모두 장착가능.
        bool		bBig;						//	대형.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        glold_103::GLITEM_ATT_103	emAttack;	//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        SRESIST		sResist;					//	저항값.

        WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

        //	특수 부가 효과들.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
        SVAR		sVARIATE;					//	변화율 효과.
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
    };

    struct SSUIT_104
    {
        enum { ADDON_SIZE = 4 };
        EMSUIT		emSuit;
        bool		bBothHand;
        bool		bBig;
        EMITEM_HAND	emHand;

        glold_104::GLITEM_ATT_104	emAttack;
        WORD		wAttRange;
        WORD		wReqSP;

        short		nHitRate;
        short		nAvoidRate;

        GLPADATA	gdDamage;
        short		nDefense;

        SRESIST_101	sResist;

        WORD		wReModelNum;

        SADDON		sADDON[ADDON_SIZE];
        SVAR		sVARIATE;
        SSATE_BLOW	sBLOW;
    };

    struct SSUIT_105
    {
        enum { ADDON_SIZE = 4 };
        EMSUIT		emSuit;
        bool		bBothHand;
        bool		bBig;
        EMITEM_HAND	emHand;

        glold_104::GLITEM_ATT_104	emAttack;
        WORD		wAttRange;
        WORD		wReqSP;

        short		nHitRate;
        short		nAvoidRate;

        GLPADATA	gdDamage;
        short		nDefense;

        SRESIST		sResist;

        WORD		wReModelNum;				//	개조 가능 횟수.		( '0' 개조 불가 )

        //	특수 부가 효과들.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
        SVAR		sVARIATE;					//	변화율 효과.
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
    };

    struct SSUIT_106
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        DWORD		dwHAND;						//	손 도구 속성.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        glold_104::GLITEM_ATT_104	emAttack;	//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        SRESIST		sResist;					//	저항값.

        //	개조 가능 횟수.		( '0' 개조 불가 ),
        //	헤어, 얼굴 아이템에서는 디자인 번호.
        WORD		wReModelNum;

        //	특수 부가 효과들.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
        SVAR		sVARIATE;					//	변화율 효과.
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
    };

    struct SSUIT_107	//	데이터 포맷 변경 진행
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        DWORD		dwHAND;						//	손 도구 속성.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        glold_104::GLITEM_ATT_104	emAttack;	//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        SRESIST		sResist;					//	저항값.

        //	개조 가능 횟수.		( '0' 개조 불가 ),
        //	헤어, 얼굴 아이템에서는 디자인 번호.
        WORD		wReModelNum;

        //	특수 부가 효과들.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.
        SVAR		sVARIATE;					//	변화율 효과.
        SVOL		sVOLUME;					//  변화량 효과. By 경대
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
    };

    struct SSUIT_108	//	무기타입 변경했음
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        DWORD		dwHAND;						//	손 도구 속성.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        SRESIST		sResist;					//	저항값.

        glold_104::GLITEM_ATT_104	emAttack;	//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        //	특수 부가 효과들.
        SVAR		sVARIATE;					//	변화율 효과.
        SVOL		sVOLUME;					//  변화량 효과. By 경대
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.

        //	개조 가능 횟수.		( '0' 개조 불가 ),
        //	헤어, 얼굴 아이템에서는 디자인 번호.
        WORD		wReModelNum;
    };

    //	착용 아이템, 관련 데이터.
    struct SSUIT_109 // By 경대
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        DWORD		dwHAND;						//	손 도구 속성.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        SRESIST		sResist;					//	저항값.

        GLITEM_ATT	emAttack;					//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        //	특수 부가 효과들.
        SVAR		sVARIATE;					//	변화율 효과.
        SVOL		sVOLUME;					//  변화량 효과. By 경대
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.

        //	개조 가능 횟수.		( '0' 개조 불가 ),
        //	헤어, 얼굴 아이템에서는 디자인 번호.
        WORD		wReModelNum;
    };

    struct SSUIT_110 // 염색 가능 기능 추가. by luxes.
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        DWORD		dwHAND;						//	손 도구 속성.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        SRESIST		sResist;					//	저항값.

        GLITEM_ATT	emAttack;	//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        //	특수 부가 효과들.
        SVAR		sVARIATE;					//	변화율 효과.
        SVOL		sVOLUME;					//  변화량 효과. By 경대
        SSATE_BLOW	sBLOW;						//	상태 이상 효과.
        SADDON		sADDON[ADDON_SIZE];			//	가산 효과.

        //	개조 가능 횟수.		( '0' 개조 불가 ),
        //	헤어, 얼굴 아이템에서는 디자인 번호.
        WORD		wReModelNum;

        bool		bChangeColor;				// 코스튬 염색 가능. by luxes.

    };

	struct SSUIT_111 // 내구도 추가
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		DWORD		dwHAND;						//	손 도구 속성.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		SRESIST		sResist;					//	저항값.

		GLITEM_ATT	emAttack;	//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		//	특수 부가 효과들.
		SVAR		sVARIATE;					//	변화율 효과.
		SVOL		sVOLUME;					//  변화량 효과. By 경대
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.

		//	개조 가능 횟수.		( '0' 개조 불가 ),
		//	헤어, 얼굴 아이템에서는 디자인 번호.
		WORD		wReModelNum;

		bool		bChangeColor;				// 코스튬 염색 가능. by luxes.

		// 내구도
		WORD        wDurabilityMax;             //! 최대 내구도 (max) [ 0 == 내구도 무제한 ]
		DWORD       dwRepairPrice;              //! 수리가격 (1p)
	};

	struct SSUIT_112 // 내구도 추가
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		DWORD		dwHAND;						//	손 도구 속성.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		SRESIST		sResist;					//	저항값.

		glold_108::GLITEM_ATT_108	emAttack;	//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		//	특수 부가 효과들.
		SVAR		sVARIATE;					//	변화율 효과.
		SVOL		sVOLUME;					//  변화량 효과. By 경대
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.

		//	개조 가능 횟수.		( '0' 개조 불가 ),
		// 고정 가능 수량;
		//	헤어, 얼굴 아이템에서는 디자인 번호.
		WORD		wReModelNum;

		unsignedState nItemSuitState; // 아이템 상태(GLItemDef.h/EMITEM_SUIT_STATE);

		// 내구도
		WORD        wDurabilityMax;             //! 최대 내구도 (max) [ 0 == 내구도 무제한 ]
		DWORD       dwRepairPrice;              //! 수리가격 (1p)
	};

	struct SSUIT_113
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		DWORD		dwHAND;						//	손 도구 속성.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		SRESIST		sResist;					//	저항값.

		GLITEM_ATT	emAttack;	//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		//	특수 부가 효과들.
		SVAR		sVARIATE;					//	변화율 효과.
		SVOL		sVOLUME;					//  변화량 효과. By 경대
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.

		//	개조 가능 횟수.		( '0' 개조 불가 ),
		// 고정 가능 수량;
		//	헤어, 얼굴 아이템에서는 디자인 번호.
		WORD		wReModelNum;

		unsignedState nItemSuitState; // 아이템 상태(GLItemDef.h/EMITEM_SUIT_STATE);

		// 내구도
		WORD        wDurabilityMax;             //! 최대 내구도 (max) [ 0 == 내구도 무제한 ]
		DWORD       dwRepairPrice;              //! 수리가격 (1p)
	};

	struct SSUIT_114
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	아이템 착용 타입.
		DWORD		dwHAND;						//	손 도구 속성.
		EMITEM_HAND	emHand;						//	장착 손 위치.

		GLPADATA	gdDamage;					//	대미지. ( +/- )
		short		nDefense;					//	방어. ( +/- )

		short		nHitRate;					//	명중율. ( +/- ) (%)
		short		nAvoidRate;					//	회피율. ( +/- ) (%)

		SRESIST		sResist;					//	저항값.

		GLITEM_ATT	emAttack;	//	공격속성.
		WORD		wAttRange;					//	공격 가능 거리.
		WORD		wReqSP;						//	공격시 필요한 SP.

		//	특수 부가 효과들.
		SVAR		sVARIATE;					//	변화율 효과.
		SVOL		sVOLUME;					//  변화량 효과. By 경대
		SSATE_BLOW	sBLOW;						//	상태 이상 효과.
		SADDON		sADDON[ADDON_SIZE];			//	가산 효과.

		//	개조 가능 횟수.		( '0' 개조 불가 ),
		// 고정 가능 수량;
		//	헤어, 얼굴 아이템에서는 디자인 번호.
		WORD		wReModelNum;

		unsignedState nItemSuitState; // 아이템 상태(GLItemDef.h/EMITEM_SUIT_STATE);

		// 내구도
		WORD        wDurabilityMax;             //! 최대 내구도 (max) [ 0 == 내구도 무제한 ]
		DWORD       dwRepairPrice;              //! 수리가격 (1p)

		// 기본 능력치 랜덤 세트 매니저 Index;
		int			nBasicAttackDamageIndex;
		int			nBasicDefenceIndex;

		// 기본 능력치 개조가능 여부;
		bool		bEditableBasicAttackDamage;
		bool		bEditableBasicDefence;
		SSUIT_114 ()
			: emSuit(SUIT_HEADGEAR)
			, dwHAND(NULL)
			, emHand(HAND_RIGHT)

			, emAttack(ITEMATT_NOTHING)
			, wAttRange(4)
			, wReqSP(0)

			, nHitRate(0)
			, nAvoidRate(0)

			, gdDamage(0,0)
			, nDefense(0)

			, wReModelNum(0)

			, wDurabilityMax(0)
			, dwRepairPrice(0)

			, nBasicAttackDamageIndex(-1)
			, nBasicDefenceIndex(-1)

			, bEditableBasicAttackDamage(false)
			, bEditableBasicDefence(false)
		{
		}

		// 115 버전 이후로는 SADDON, sVARIATE, sVOLUME을 SADDON하나로 합친다;
		// 목표버전에는 sVARIATE, sVOLUME이 사라질것이기에 114버전을 기준으로;
		// 하위 버전들을 컨버팅시킨후 다시 최신버전으로 컨버팅 시킨다;
		void Assign ( SSUIT_100 &Suit100 );
		void Assign ( SSUIT_101 &Suit101 );
		void Assign ( SSUIT_102 &Suit102 );
		void Assign ( SSUIT_103 &Suit103 );
		void Assign ( SSUIT_104 &Suit104 );
		void Assign ( SSUIT_105 &Suit105 );
		void Assign ( SSUIT_106 &Suit106 );
		void Assign ( SSUIT_107 &Suit107 );
		void Assign ( SSUIT_108 &Suit108 );
		void Assign ( SSUIT_109 &Suit109 );
		void Assign ( SSUIT_110 &sSuitOld );
		void Assign ( SSUIT_111 &sSuitOld );
		void Assign ( SSUIT_112 &sSuitOld );
		void Assign ( SSUIT_113& sSuitOld );

		void SETBOTHHAND ( bool bBOTH )		{ (bBOTH) ? (dwHAND|=EMHAND_BOTHHAND) : (dwHAND&=~EMHAND_BOTHHAND); }
		void SETBIG ( bool bBIG )			{ (bBIG) ? (dwHAND|=EMHAND_BIG) : (dwHAND&=~EMHAND_BIG); }
	};

	struct SSUIT_115
	{
		enum { ADDON_SIZE = 6, STRING_LENGTH = 64, };

		EMSUIT			emSuit;
		DWORD			dwHAND;
		EMITEM_HAND		emHand;

		GLPADATA		gdDamage;
		short			nDefense;

		short			nHitRate;
		short			nAvoidRate;

		SRESIST			sResist;

		GLITEM_ATT		emAttack;
		WORD			wAttRange;
		WORD			wReqSP;

		SSATE_BLOW		sBLOW;
		SADDON			sADDON[ ADDON_SIZE ];
		char			szRandomAddonScript[ STRING_LENGTH ];

		WORD			wReModelNum;

		unsignedState	nItemSuitState;

		WORD			wDurabilityMax;
		DWORD			dwRepairPrice;

		int				nBasicAttackDamageIndex;
		int				nBasicDefenceIndex;

		bool			bEditableBasicAttackDamage;
		bool			bEditableBasicDefence;
	};

    struct SSUIT // 내구도 추가
    {
        enum { ADDON_SIZE = 6, VERSION = 0x0116, STRING_LENGTH = 64, };

        EMSUIT		emSuit;						//	아이템 착용 타입.
        DWORD		dwHAND;						//	손 도구 속성.
        EMITEM_HAND	emHand;						//	장착 손 위치.

        GLPADATA	gdDamage;					//	대미지. ( +/- )
        short		nDefense;					//	방어. ( +/- )

        short		nHitRate;					//	명중율. ( +/- ) (%)
        short		nAvoidRate;					//	회피율. ( +/- ) (%)

        SRESIST		sResist;					//	저항값.

        GLITEM_ATT	emAttack;	//	공격속성.
        WORD		wAttRange;					//	공격 가능 거리.
        WORD		wReqSP;						//	공격시 필요한 SP.

        //	특수 부가 효과들.
//		SVAR		sVARIATE;						//	변화율 효과.
//		SVOL		sVOLUME;						//  변화량 효과. By 경대
        SSATE_BLOW	sBLOW;							//	상태 이상 효과.
        SADDON		sADDON[ADDON_SIZE];				//	가산 효과.
		char		szRandomAddonScript[STRING_LENGTH];	//	랜덤 가산효과 설정파일;

        //	개조 가능 횟수.		( '0' 개조 불가 ),
		// 고정 가능 수량;
        //	헤어, 얼굴 아이템에서는 디자인 번호.
        WORD		wReModelNum;

		unsignedState nItemSuitState; // 아이템 상태(GLItemDef.h/EMITEM_SUIT_STATE);

        // 내구도
        WORD        wDurabilityMax;             //! 최대 내구도 (max) [ 0 == 내구도 무제한 ]
        DWORD       dwRepairPrice;              //! 수리가격 (1p)

		// 기본 능력치 랜덤 세트 매니저 Index;
		int			nBasicAttackDamageType;
		int			nBasicAttackDamageIndex;

		int			nBasicDefenceType;
		int			nBasicDefenceIndex;

		// 기본 능력치 개조가능 여부;
		bool		bEditableBasicAttackDamage;
		bool		bEditableBasicDefence;

    SSUIT ()
            : emSuit(SUIT_HEADGEAR)
            , dwHAND(NULL)
            , emHand(HAND_RIGHT)

            , emAttack(ITEMATT_NOTHING)
            , wAttRange(4)
            , wReqSP(0)

            , nHitRate(0)
            , nAvoidRate(0)

            , gdDamage(0,0)
            , nDefense(0)

            , wReModelNum(0)
            
            , wDurabilityMax(0)
            , dwRepairPrice(0)

			, nBasicAttackDamageType(-1)
			, nBasicAttackDamageIndex(-1)

			, nBasicDefenceType(-1)
			, nBasicDefenceIndex(-1)

			, bEditableBasicAttackDamage(false)
			, bEditableBasicDefence(false)
        {
			memset(szRandomAddonScript, 0, sizeof(char) * STRING_LENGTH);
        }

		void Assign ( SSUIT_100 &Suit100 );
		void Assign ( SSUIT_101 &Suit101 );
		void Assign ( SSUIT_102 &Suit102 );
		void Assign ( SSUIT_103 &Suit103 );
		void Assign ( SSUIT_104 &Suit104 );
		void Assign ( SSUIT_105 &Suit105 );
		void Assign ( SSUIT_106 &Suit106 );
		void Assign ( SSUIT_107 &Suit107 );
		void Assign ( SSUIT_108 &Suit108 );
		void Assign ( SSUIT_109 &Suit109 );
		void Assign ( SSUIT_110 &sSuitOld );
		void Assign ( SSUIT_111 &sSuitOld );
		void Assign ( SSUIT_112 &sSuitOld );
		void Assign ( SSUIT_113& sSuitOld );
		void Assign ( SSUIT_114& sSuitOld );
		// 0x115버전 이전 데이터들의 SADDON, SVAR, SVOL을 SADDON 하나로 합친다;
		// 일반적 로드(위의 Assign) 이후에 이루어져야한다;
		void Assign ( SSUIT_115& sSuitOld );



        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );

        void SETBOTHHAND ( bool bBOTH )		{ (bBOTH) ? (dwHAND|=EMHAND_BOTHHAND) : (dwHAND&=~EMHAND_BOTHHAND); }
        void SETBIG ( bool bBIG )			{ (bBIG) ? (dwHAND|=EMHAND_BIG) : (dwHAND&=~EMHAND_BIG); }
        void SETBROOM ( bool bBROOM )		{ (bBROOM) ? (dwHAND|=EMHAND_BROOM) : (dwHAND&=~EMHAND_BROOM); }
		void SETSTATE(const DWORD nState, const bool bOn) { bOn ? nItemSuitState += nState : nItemSuitState -= nState; }		

        const bool IsBOTHHAND () const		{ return NULL!=(dwHAND&EMHAND_BOTHHAND); }
        const bool IsBIG () const				{ return NULL!=(dwHAND&EMHAND_BIG); }
        const bool IsBROOM () const			{ return NULL!=(dwHAND&EMHAND_BROOM); }
		const bool IsSTATE(const DWORD nState)	const { return nItemSuitState[nState]; }
		const bool IsStatEnable() const;
		const bool IsNone() const;

        //! 아이템 착용 타입
        inline EMSUIT Suit() const { return emSuit; }

		// 기본 공격력, 방어력;
		inline const GLPADATA GetAttackDamage ( void ) const { return gdDamage; }
		inline const WORD GetDefence ( void ) const { return nDefense; }

		// 기본 공격력, 방어력 랜덤 세트 매니저 Index;
		inline const int GetBasicAttackDamageType ( void ) const { return nBasicAttackDamageType; }
		inline const int GetBasicAttackDamageIndex ( void ) const { return nBasicAttackDamageIndex; }

		inline const int GetBasicDefenceType ( void ) const { return nBasicDefenceType; }
		inline const int GetBasicDefenceIndex ( void ) const { return nBasicDefenceIndex; }

		// 기본 공격력, 방어력 개조가능 여부;
		inline const bool IsEditableBasicAttackDamage ( void ) const { return bEditableBasicAttackDamage && ( -1 != GetBasicAttackDamageIndex() ) && ( -1 != GetBasicAttackDamageType() ); }
		inline const bool IsEditableBasicDefence ( void ) const { return bEditableBasicDefence && ( -1 != GetBasicDefenceIndex() ) && ( -1 != GetBasicDefenceType() ); }

		// 해당 Addon을 가지고있는가?;
		const bool IsHaveAddon( IN const EMITEM_ADDON emAddon, OUT int& nIdx ) const;
		// res:존재여부, 해당 Addon 값 모두 추출;
		const bool GetHaveAddonValue_float( IN const EMITEM_ADDON emAddon, OUT float& fResult ) const;
		const bool GetHaveAddonValue_int( IN const EMITEM_ADDON emAddon, OUT INT& nResult ) const;

		/// Datatyoe 이 바뀔 경우를 대비해 보정해준다;
		/// 보정이 필요없도록 저장 관리가 된는것이 더 좋은 방법이다;
		/// 이미 이렇게 구현되었으며 Datatype 이 변환될 일이 버그수정이후에는 거의 없으므로;
		/// 그대로 사용하자;

		/// 아래 함수들은 GLItemCustom_Addon 에 있는 GetValue Function 과 같이 수정되어야 한다;
		/// 중복코드가 되지만 수정할 일이 거의 없는 부분이라 그냥 중복시켰다
		template < typename T >
		const T GetAddonValue_CompareExchange ( const int _nIndex, const bool _bInteger )
		{
			if ( _nIndex < 0 || ADDON_SIZE <= _nIndex )
				return static_cast< const T >( 0 );

			int nValue = 0;
			float fValue = 0.f;
			if ( _bInteger )
			{
				nValue = sADDON[ _nIndex ].nVALUE;
				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = static_cast< int >( sADDON[ _nIndex ].fVALUE );

				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = 0;

				if ( sADDON[ _nIndex ].nVALUE != nValue )
					sADDON[ _nIndex ].nVALUE = nValue;
			}
			else
			{
				fValue = sADDON[ _nIndex ].fVALUE;
				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = static_cast< float >( sADDON[ _nIndex ].nVALUE );

				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = 0.f;

				if ( sADDON[ _nIndex ].fVALUE != fValue )
					sADDON[ _nIndex ].fVALUE = fValue;
			}

			const T tReturnValue = static_cast< const T >( ( _bInteger ) ? nValue : fValue );

			return tReturnValue;
		}

		// const version;
		template < typename T >
		const T GetAddonValue ( const int _nIndex, const bool _bInteger ) const
		{
			if ( _nIndex < 0 || ADDON_SIZE <= _nIndex )
				return static_cast< const T >( 0 );

			int nValue = 0;
			float fValue = 0.f;
			if ( _bInteger )
			{
				nValue = sADDON[ _nIndex ].nVALUE;
				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = static_cast< int >( sADDON[ _nIndex ].fVALUE );

				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = 0;
			}
			else
			{
				fValue = sADDON[ _nIndex ].fVALUE;
				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = static_cast< float >( sADDON[ _nIndex ].nVALUE );

				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = 0.f;
			}

			const T tReturnValue = static_cast< const T >( ( _bInteger ) ? nValue : fValue );

			return tReturnValue;
		}
    };

} // namespace ITEM
#endif // _GL_ITEM_SUIT_H_
