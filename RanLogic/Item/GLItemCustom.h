#pragma once

#include "../../SigmaCore/String/MinGuid.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../Character/GLCharDefine.h"
#include "./GLItemDef.h"
#include "./GLItemCostumeAttach.h"
#include "./GLItem_RandomGenOp_Utility.h"
#include "./GLItemCustom_Addon.h"

namespace sc 
{
    class BaseStream;
    class SerialFile;
}

namespace se
{
    class ByteStream;
}


struct SITEM;

struct SITEMCUSTOM_100
{
    SNATIVEID	sNativeID;

    WORD		wReModel;
    WORD		wTurnNum;

    short		nHitRate;
    short		nAvoidRate;

    short		nDamage;
    short		nDefense;

    SRESIST_101	sResist;
};

struct SITEMCUSTOM_101
{
    SNATIVEID	sNativeID;

    WORD		wReModel;
    WORD		wTurnNum;

    short		nHitRate;
    short		nAvoidRate;

    short		nDamage;
    short		nDefense;

    SRESIST		sResist;
};

struct SITEMCUSTOM_102
{
    SNATIVEID	sNativeID;

    BYTE		cGenType;
    BYTE		cFieldID;
    DWORD		dwGenNum;

    WORD		wReModel;
    WORD		wTurnNum;

    short		nHitRate;
    short		nAvoidRate;

    short		nDamage;
    short		nDefense;

    SRESIST		sResist;
};

struct SITEMCUSTOM_103
{
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

    BYTE		cGenType;					//	생성 타입.
    BYTE		cFieldID;					//	필드 번호.
    LONGLONG	lnGenNum;					//	생성 번호.

    BYTE		cReModelReq;				//	개조 시도 횟수.
    BYTE		cReModelSuc;				//	개조 성공 횟수.

    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

    short		nHitRate;					//	명중율. ( +/- ) (%)
    short		nAvoidRate;					//	회피율. ( +/- ) (%)

    short		nDamage;					//	대미지. ( +/- )
    short		nDefense;					//	방어. ( +/- )

    SRESIST		sResist;					//	저항값.
};

struct SITEMCUSTOM_104
{
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.

    BYTE		cGenType;					//	생성 타입.
    BYTE		cFieldID;					//	필드 번호.
    LONGLONG	lnGenNum;					//	생성 번호.

    BYTE		cReModelReq;				//	개조 시도 횟수.
    BYTE		cReModelSuc;				//	개조 성공 횟수.

    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

    short		nHitRate;					//	명중율. ( +/- ) (%)
    short		nAvoidRate;					//	회피율. ( +/- ) (%)

    short		nDamage;					//	대미지. ( +/- )
    short		nDefense;					//	방어. ( +/- )

    SRESIST		sResist;					//	저항값.
};

struct SITEMCUSTOM_105
{
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.

    BYTE		cGenType;					//	생성 타입.
    BYTE		cFieldID;					//	필드 번호.
    LONGLONG	lnGenNum;					//	생성 번호.

    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.

    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_106
{
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.

    BYTE		cGenType;					//	생성 타입.
    BYTE		cFieldID;					//	필드 번호.
    LONGLONG	lnGenNum;					//	생성 번호.

    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.

    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_107
{
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

    BYTE		cGenType;					//	생성 타입.
    BYTE		cFieldID;					//	필드 번호.
    LONGLONG	lnGenNum;					//	생성 번호.

    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.

    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_108
{
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

    BYTE		cGenType;					//	생성 타입.
    BYTE		cChnID;						//	채널 번호.
    BYTE		cFieldID;					//	필드 번호.
    LONGLONG	lnGenNum;					//	생성 번호.

    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )

    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.

    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.
};

struct SITEMCUSTOM_109
{
public:
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

    LONGLONG	lnGenNum;					//	생성 번호.
    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
    BYTE		cGenType;					//	생성 타입.
    BYTE		cChnID;						//	채널 번호.

    BYTE		cFieldID;					//	필드 번호.

public:
    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    BYTE		cOptTYPE1;
    BYTE		cOptTYPE2;
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1;
    short		nOptVALUE2;
    short		nOptVALUE3;
    short		nOptVALUE4;
};

struct SITEMCUSTOM_110
{
public:
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

    LONGLONG	lnGenNum;					//	생성 번호.
    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
    BYTE		cGenType;					//	생성 타입.
    BYTE		cChnID;						//	채널 번호.

    BYTE		cFieldID;					//	필드 번호.

public:
    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    BYTE		cOptTYPE1;
    BYTE		cOptTYPE2;
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1;
    short		nOptVALUE2;
    short		nOptVALUE3;
    short		nOptVALUE4;

    DWORD		dwPetID;	// 펫아디
};

struct SITEMCUSTOM_111
{
public:
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

    LONGLONG	lnGenNum;					//	생성 번호.
    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
    BYTE		cGenType;					//	생성 타입.
    BYTE		cChnID;						//	채널 번호.

    BYTE		cFieldID;					//	필드 번호.

public:
    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    BYTE		cOptTYPE1;
    BYTE		cOptTYPE2;
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1;
    short		nOptVALUE2;
    short		nOptVALUE3;
    short		nOptVALUE4;

    DWORD		dwPetID;	// 펫아디
    DWORD		dwVehicleID; // 탈것아뒤(DB)
};

struct SITEMCUSTOM_112
{
public:
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

    LONGLONG	lnGenNum;					//	생성 번호.
    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
    BYTE		cGenType;					//	생성 타입.
    BYTE		cChnID;						//	채널 번호.

    BYTE		cFieldID;					//	필드 번호.

public:
    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    BYTE		cOptTYPE1; // 공격력
    BYTE		cOptTYPE2; // 기저항
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1; // 공격력 %
    short		nOptVALUE2; // 기저항 %
    short		nOptVALUE3;
    short		nOptVALUE4;

public:
    DWORD		dwPetID;	 // 펫아디
    DWORD		dwVehicleID; // 탈것아뒤(DB)

    bool		bVietnamGainItem;	 //	베트남 탐닉 인벤 아이템( 베트남 전용 )
};

struct SITEMCUSTOM_113
{
public:
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

    LONGLONG	lnGenNum;					//	생성 번호.
    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
    BYTE		cGenType;					//	생성 타입.
    BYTE		cChnID;						//	채널 번호.

    BYTE		cFieldID;					//	필드 번호.

public:
    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    BYTE		cOptTYPE1; // 공격력
    BYTE		cOptTYPE2; // 기저항
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1; // 공격력 %
    short		nOptVALUE2; // 기저항 %
    short		nOptVALUE3;
    short		nOptVALUE4;

public:
    DWORD		dwPetID;	 //! 펫아디 (DB 번호)
    DWORD		dwVehicleID; //! 탈것 ID (DB번호)

    bool		bVietnamGainItem;	 //	베트남 탐닉 인벤 아이템( 베트남 전용 )

    // 염색 코스튬 정보. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

};

struct SITEMCUSTOM_114
{
public:
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//	시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//	시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//	생성 번호.
	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.

	BYTE		cFieldID;					//	필드 번호.

public:
	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	BYTE		cOptTYPE1; // 공격력
	BYTE		cOptTYPE2; // 기저항
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // 공격력 %
	short		nOptVALUE2; // 기저항 %
	short		nOptVALUE3;
	short		nOptVALUE4;

public:
	DWORD		dwPetID;	 //! 펫아디 (DB 번호)
	DWORD		dwVehicleID; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;	 //	베트남 탐닉 인벤 아이템( 베트남 전용 )

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

};

struct RandomOpt_0100;

struct RandomOpt
{
public:
	struct Option;
	class FixOption;
	enum { VERSION = 0x0101, NSIZE = 5, EMNULL = 0xffffffff };

public:	
	const bool assign(const DWORD nIndex, const BYTE nType, const short nValue);
	const bool assign(const DWORD nIndex, const RandomOpt::Option& option);

	// 빈곳에 값을 설정 함(0번 부터 검색);
	const bool setOption(const BYTE nType, const float fValue);

	// 지정된 곳에 값을 설정 함;
	const bool setOption(const DWORD nIndex, const BYTE nType, const float fValue);

	void remove(const DWORD nIndex);
	const BYTE getType(const DWORD nIndex) const;
	const short getValue(const DWORD nIndex) const;
	const BYTE getCount(const DWORD nIndex) const;

	const int reset(const BYTE cDAMAGE, const BYTE cDEFENSE, const ITEM_RANDOMOP_UTILL::FixOption& fixOption, const DWORD nFixMaximum);	
	bool Load_Ver100(const RandomOpt_0100& sRandomOpt);

private:
	const DWORD _findEmptySlot(void) const;
	const DWORD _size(void) const;

public:
	const bool operator ==(const RandomOpt& _rhs);
	const RandomOpt& operator =(const RandomOpt& _rhs);
	RandomOpt(void);
	RandomOpt(const RandomOpt& _rhs);

public:
	struct Option
	{
		void reset(void);
		const bool operator ==(const RandomOpt::Option& _rhs) const;
		const bool operator !=(const RandomOpt::Option& _rhs) const;
		const RandomOpt::Option& operator =(const RandomOpt::Option& rhs);
		Option(const BYTE _cOptType, const short _nOptValue, const BYTE _cOptCount);
		Option(const BYTE _cOptType, const short _nOptValue);
		Option(void);

		short		nOptValue; // 랜덤 옵션 값;
		BYTE		cOptType; // 랜덤 옵션 종류;
		BYTE		cOptCount; // 인장 카드로 고정 한 횟수;
		MSGPACK_DEFINE(nOptValue, cOptType, cOptCount);		

		friend class boost::serialization::access;
		template <typename Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & nOptValue;
			ar & cOptType;
			ar & cOptCount;
		}
	};

	Option option[RandomOpt::NSIZE];		

	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & option[0];
		ar & option[1];
		ar & option[2];
		ar & option[3];
		ar & option[4];
	}

	MSGPACK_DEFINE(option[0], option[1], option[2], option[3], option[4]);
};

struct RandomOpt_0100
{
	enum { VERSION = 0x0100, NSIZE = 4, EMNULL = 0xffffffff };
	RandomOpt::Option option[RandomOpt::NSIZE];	
};



//	Note : SITEMCUSTOM - 생성된 아이템의 정보.
//		
//		버전 변경시 "struct SINVENITEM_SAVE" 도 버전을 변경해줘야 한다.
//		아래 함수들도 버전 변경에 따라 입출력 부분을 수정해야 한다.
//					"BOOL SCHARDATA2::GETPUTONITEMS_BYBUF ()"
//					"BOOL SETPUTONITEMS_BYBUF ()"
//					"BOOL GLInventory::SETITEM_BYBUFFER ()"
//					"BOOL GLQUEST::LOAD ()" - INVENQARRAY
//					"BOOL GLQUEST_STEP::LOAD ()" - INVENQARRAY



struct SITEMCUSTOM_115
{
public:
    SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//! 아이템 생성시간, 시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간.

    LONGLONG	lnGenNum;					//! 생성 번호.
    WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
    BYTE		cGenType;					//! 생성 타입.
    BYTE		cChnID;						//! 채널 번호.
    BYTE		cFieldID;					//! 필드 번호.

public:
    BYTE		cDAMAGE;					//! 공격력 개조 등급.
    BYTE		cDEFENSE;					//! 방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    RandomOpt_0100 randomOption;

public:
    DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)
    DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

    bool		bVietnamGainItem;	 //	베트남 탐닉 인벤 아이템( 베트남 전용 )

    // 염색 코스튬 정보. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

    //내구도
    WORD        wDurability;
    DWORD       dwDurabilityCount;

};


struct SITEMCUSTOM_116
{
public:
    SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//! 아이템 생성시간, 시한부 아이템일 경우 유효 기간.
    __time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간.

    LONGLONG	lnGenNum;					//! 생성 번호.
    WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
    BYTE		cGenType;					//! 생성 타입.
    BYTE		cChnID;						//! 채널 번호.
    BYTE		cFieldID;					//! 필드 번호.

public:
    BYTE		cDAMAGE;					//! 공격력 개조 등급.
    BYTE		cDEFENSE;					//! 방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    RandomOpt_0100 randomOption;

public:
    DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)
    DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

    bool		bVietnamGainItem;	 //	베트남 탐닉 인벤 아이템( 베트남 전용 )

    // 염색 코스튬 정보. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

    //내구도
    WORD        wDurability;
    DWORD       dwDurabilityCount;

};

struct SITEMCUSTOM_117
{	// by luxes.
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간, 시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//! 생성 번호.
	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	BYTE		cChnID;						//! 채널 번호.
	BYTE		cFieldID;					//! 필드 번호.

public:
	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;
};

struct SITEMCUSTOM_118
{	// by luxes.
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간, 시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//! 생성 번호.
	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	BYTE		cChnID;						//! 채널 번호.
	BYTE		cFieldID;					//! 필드 번호.

public:
	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;

	COSTUMEUSERVALUE::sCostumeUser costumeUser;
};

struct SITEM_LOBY;
struct SITEMCLIENT;

struct STLGrindInfo
{
public :

	enum { VERSION = 0x0100 };

	STLGrindInfo();
	STLGrindInfo(const STLGrindInfo& _rhs);

	void resetInfo();

	const STLGrindInfo& operator=(const STLGrindInfo& _rhs);
	const bool operator==(const STLGrindInfo& _rhs);

public :
	BYTE       cGradeValue;
	__time64_t tFireTime;

	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & cGradeValue;
		ar & tFireTime;

	}

	MSGPACK_DEFINE(cGradeValue, tFireTime);
};

struct SITEMCUSTOM_119
{	// by luxes.
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간, 시한부 아이템일 경우 유효 기간.
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간.

	LONGLONG	lnGenNum;					//! 생성 번호.
	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	BYTE		cChnID;						//! 채널 번호.
	BYTE		cFieldID;					//! 필드 번호.

public:
	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;

	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;
};

struct SITEMCUSTOM_120
{
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

public:
	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간;
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간

	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	BYTE		cPeriodExtensionCount;		//! 기간 연장 사용가능 횟수.
	BYTE		cCostumePeriodExtensionCount;					//! 필드 번호.

public:
	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;


	// 유저 코스튬 스탯정보
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;
};

struct SITEMCUSTOM_121
{
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

public:
	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간;
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간

public:
	// 일반 아이템(코스튬 아님x) 시효성 연장 기간;
	// 일반 아이템의 시효성은 tBORNTIME(생성시간) + ITEM::SDRUG::tTIME_LMT + tEXTENSIONT 에 의해 결정된다;
	// 그리고 아래의 데이터들이 DB에 저장될때 1900-01-01을 기준으로 저장이된다;
	// 30일 연장이라면 1900-01-30로 저장이된다 유의해야함;
	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

public:
	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	BYTE		cPeriodExtensionCount;		//! 기간 연장 사용가능 횟수.
	BYTE		cCostumePeriodExtensionCount;

public:
	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;


	// 유저 코스튬 스탯정보
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;
};

struct SITEMCUSTOM_122
{
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

public:
	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간;
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간

public:
	// 일반 아이템(코스튬 아님x) 시효성 연장 기간;
	// 일반 아이템의 시효성은 tBORNTIME(생성시간) + ITEM::SDRUG::tTIME_LMT + tEXTENSIONT 에 의해 결정된다;
	// 그리고 아래의 데이터들이 DB에 저장될때 1900-01-01을 기준으로 저장이된다;
	// 30일 연장이라면 1900-01-30로 저장이된다 유의해야함;
	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

public:
	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	BYTE		cPeriodExtensionCount;		//! 기간 연장 사용가능 횟수.
	BYTE		cCostumePeriodExtensionCount;

public:
	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;


	// 유저 코스튬 스탯정보
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;

	// 기본 능력치;
	GLPADATA	sBasicAttackDamage;			// 기본 공격력;
	WORD		wBasicDefence;				// 기본 방어력;
};

// SITEMCUSTOM 의 아이템 기본 정보;
struct SITEMCUSTOM_BASIC
{
public:
	void Initialize ( void );

	void DefaultInitialize_BasicAttackDamage ( const SITEM* _pItem );
	void DefaultInitialize_BasicDefence ( const SITEM* _pItem );
	void DefaultInitialize_LinkSkill ( const SITEM* _pItem );
	void DefaultInitialize ( const SITEM* _pItem );

	void GenerateBasicAttackDamage_Make ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex );
	void GenerateBasicAttackDamage_Drop ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex );
	void GenerateBasicDefence_Make ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex );
	void GenerateBasicDefence_Drop ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex );
	void GenerateLinkSkill ( const int _nLinkSkillIndex );


public:
	inline const GLPADATA& GetBasicAttackDamage ( void ) const { return m_sBasicAttackDamage; }
	inline const WORD GetBasicDefence ( void ) const { return m_wBasicDefence; }
	
	inline const EMITEM_SKILL_LINK GetLinkSkillTarget ( void ) const { return m_emLinkSkillTarget; }
	inline const SNATIVEID& GetLinkSkillID ( void ) const { return m_sLinkSkillID; }
	inline const WORD GetLinkSkillLevel ( void ) const { return m_wLinkSkillLevel; }
	inline const float GetLinkSkillOccurRate ( void ) const { return m_fLinkSkillOccurRate; }

	inline void SetBasicAttackDamage ( const DWORD _dwBasicAttackDamageData ) { m_sBasicAttackDamage.dwData = _dwBasicAttackDamageData; }
	inline void SetBasicAttackDamage ( const GLPADATA& _sBasicAttackDamage ) { m_sBasicAttackDamage = _sBasicAttackDamage; }
	inline void SetBasicDefence ( const WORD _wBasicDefence ) { m_wBasicDefence = _wBasicDefence; }
	
	inline void SetLinkSkillTarget ( const EMITEM_SKILL_LINK _emLinkSkillTarget ) { m_emLinkSkillTarget = _emLinkSkillTarget; }
	inline void SetLinkSkillID ( const SNATIVEID& _sLinkSkillID ) { m_sLinkSkillID = _sLinkSkillID; }
	inline void SetLinkSkillLevel ( const WORD _wLinkSkillLevel ) { m_wLinkSkillLevel = _wLinkSkillLevel; }
	inline void SetLinkSkillOccurRate ( const float _fLinkSkillOccurRate ) { m_fLinkSkillOccurRate = _fLinkSkillOccurRate; }


private:
	// 기본 능력치;
	GLPADATA			m_sBasicAttackDamage;				// 기본 공격력;
	WORD				m_wBasicDefence;					// 기본 방어력;

	// 링크 스킬;
	EMITEM_SKILL_LINK	m_emLinkSkillTarget;				// 링크 스킬 타겟;
	SNATIVEID			m_sLinkSkillID;						// 링크 스킬 ID;
	int					m_wLinkSkillLevel;					// 링크 스킬 Level;
	float				m_fLinkSkillOccurRate;				// 링크 스킬 발생 확률;


public:
	MSGPACK_DEFINE(
		m_sBasicAttackDamage,
		m_wBasicDefence,
		(int&)m_emLinkSkillTarget,
		m_sLinkSkillID,
		m_wLinkSkillLevel,
		m_fLinkSkillOccurRate );


public:
	SITEMCUSTOM_BASIC ()
		: m_wBasicDefence ( 0 )
		, m_emLinkSkillTarget ( EMITEM_SKILL_ALL )
		, m_sLinkSkillID ( false )
		, m_wLinkSkillLevel ( 0 )
		, m_fLinkSkillOccurRate ( 0.f )
	{

	}


private:
	friend class boost::serialization::access;
	template < typename Archive >
	void serialize ( Archive& ar, const unsigned int nVersion )
	{
		ar & m_sBasicAttackDamage;
		ar & m_wBasicDefence;
		ar & m_emLinkSkillTarget;
		ar & m_sLinkSkillID;
		ar & m_wLinkSkillLevel;
		ar & m_fLinkSkillOccurRate;
	}
};

struct SITEMCUSTOM_123
{
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간;
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간

	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

	//LONGLONG	lnGenNum;					//! 생성 번호.
	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	//  [4/14/2016 gbgim]
	// 기존 아이템 생성 당시 해당 채널 정보를 기억하던(cChnID) 데이터가;
	// 사용되는 곳이 없으므로 기간연장 아이템 횟수 처리 변수로 변경;
	// 현재 미리 커밋하기 위해 일괄적으로 변경중에있음;
	//BYTE		cChnID;						//! 채널 번호.
	BYTE		cPeriodExtensionCount;		//! 기간 연장 사용가능 횟수.
	//BYTE		cFieldID;					//! 필드 번호.
	BYTE		cCostumePeriodExtensionCount;					//! 필드 번호.

	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;


	// 유저 코스튬 스탯정보
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;

	SITEMCUSTOM_BASIC sBasicInfo;
};

// 랜덤옵션 사이즈 변경됨;
struct SITEMCUSTOM_124
{
public:
	SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

	SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
	__time64_t	tBORNTIME;					//! 아이템 생성시간;
	__time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간

	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

	//LONGLONG	lnGenNum;					//! 생성 번호.
	WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
	BYTE		cGenType;					//! 생성 타입.
	//  [4/14/2016 gbgim]
	// 기존 아이템 생성 당시 해당 채널 정보를 기억하던(cChnID) 데이터가;
	// 사용되는 곳이 없으므로 기간연장 아이템 횟수 처리 변수로 변경;
	// 현재 미리 커밋하기 위해 일괄적으로 변경중에있음;
	//BYTE		cChnID;						//! 채널 번호.
	BYTE		cPeriodExtensionCount;		//! 기간 연장 사용가능 횟수.
	//BYTE		cFieldID;					//! 필드 번호.
	BYTE		cCostumePeriodExtensionCount;					//! 필드 번호.

	BYTE		cDAMAGE;					//! 공격력 개조 등급.
	BYTE		cDEFENSE;					//! 방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

	//	랜덤 옵션 사용.
	RandomOpt_0100 randomOption;

	DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
	// 잠금 박스에서 사용; 잠금(true), 열림(false)

	// 염색 코스튬 정보. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//내구도
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// 란 포인트 거래
	WORD        wRanPointTradeCount;


	// 유저 코스튬 스탯정보
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;

	SITEMCUSTOM_BASIC sBasicInfo;
	SITEMCUSTOM_ADDON sAddonInfo;
};



struct SITEMCUSTOM
{	// by luxes.
public:
    enum { VERSION = 0x0125 };
	enum EMCHECKFLAG_OPTION
	{
		CHECKFLAGOP_NORMAL = 1,		// 일반 아이템만 체크;
		CHECKFLAGOP_COSTUME = 2,	// 코스튬만 체크;
		CHECKFLAGOP_ALL = 3,		// 모두 체크;
	};

private:
    SNATIVEID	sNativeID;					//! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

public:
    SNATIVEID	nidDISGUISE;				//! 코스툼 복장 스킨.
    __time64_t	tBORNTIME;					//! 아이템 생성시간;
    __time64_t	tDISGUISE;					//! 시한부 코스툼일 경우 유효 기간
	
private:
	// 일반 아이템(코스튬 아님x) 시효성 연장 기간;
	// 일반 아이템의 시효성은 tBORNTIME(생성시간) + ITEM::SDRUG::tTIME_LMT + tEXTENSIONT 에 의해 결정된다;
	// 그리고 아래의 데이터들이 DB에 저장될때 1900-01-01을 기준으로 저장이된다;
	// 30일 연장이라면 1900-01-30로 저장이된다 유의해야함;
	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

public:

    //LONGLONG	lnGenNum;					//! 생성 번호.
    WORD		wTurnNum;					//! 사용가능량. ( wUsedNum )
    BYTE		cGenType;					//! 생성 타입.
	//  [4/14/2016 gbgim]
	// 기존 아이템 생성 당시 해당 채널 정보를 기억하던(cChnID) 데이터가;
	// 사용되는 곳이 없으므로 기간연장 아이템 횟수 처리 변수로 변경;
	// 현재 미리 커밋하기 위해 일괄적으로 변경중에있음;
    //BYTE		cChnID;						//! 채널 번호.
	BYTE		cPeriodExtensionCount;		//! 기간 연장 사용가능 횟수.
	//BYTE		cFieldID;					//! 필드 번호.
    BYTE		cCostumePeriodExtensionCount;					//! 필드 번호.

public:
    BYTE		cDAMAGE;					//! 공격력 개조 등급.
    BYTE		cDEFENSE;					//! 방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//! 저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//! 저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//! 저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//! 저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//! 저항(정) 개조 등급.

public:
    //	랜덤 옵션 사용.
    RandomOpt randomOption;
	
public:
    DWORD		m_PetDbNum;	 //! 펫아디 (DB 번호)

	DWORD		m_VehicleDbNum; //! 탈것 ID (DB번호)

	bool		bVietnamGainItem;		//	베트남 탐닉 인벤 아이템( 베트남 전용 )
										// 잠금 박스에서 사용; 잠금(true), 열림(false)

    // 염색 코스튬 정보. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

    //내구도
    WORD        wDurability;
    DWORD       dwDurabilityCount;

    // 란 포인트 거래
    WORD        wRanPointTradeCount;


	// 유저 코스튬 스탯정보
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;

private:
	SITEMCUSTOM_BASIC sBasicInfo;

public:
	SITEMCUSTOM_ADDON sAddonInfo;

public:
    MSGPACK_DEFINE(
        sNativeID,
        nidDISGUISE,
        tBORNTIME,
        tDISGUISE,
		tPERIODEX_BASIC,
		tPERIODEX_DISGUISE,
        //lnGenNum,
        wTurnNum,
        cGenType,
        // cChnID,
		cPeriodExtensionCount,
		cCostumePeriodExtensionCount,
        cDAMAGE,
        cDEFENSE,
        cRESIST_FIRE,
        cRESIST_ICE,
        cRESIST_ELEC,
        cRESIST_POISON,
        cRESIST_SPIRIT,
        randomOption,
        m_PetDbNum,
        m_VehicleDbNum,
        bVietnamGainItem,    
        dwMainColor,
        dwSubColor, 
        wDurability,
		dwDurabilityCount,
        wRanPointTradeCount,
		costumeUser, 
		sTLGrind, 
		guid, 
		itemState,
		sBasicInfo,
		sAddonInfo
		);

public:
    SITEMCUSTOM();
    SITEMCUSTOM( const SNATIVEID& sNID );
	SITEMCUSTOM( const SITEMCUSTOM& sItemcustom );

    void ResetGrind    ( EMGRINDING_TYPE emTYPE );
    void DecreaseGrind ( EMGRINDING_TYPE emTYPE, const INT nValue );

    bool operator == ( const SITEMCUSTOM &value );
    bool operator != ( const SITEMCUSTOM &value );

    void Assign ( const SITEMCUSTOM_100 &sOld );
    void Assign ( const SITEMCUSTOM_101 &sOld );
    void Assign ( const SITEMCUSTOM_102 &sOld );
    void Assign ( const SITEMCUSTOM_103 &sOld );
    void Assign ( const SITEMCUSTOM_104 &sOld );
    void Assign ( const SITEMCUSTOM_105 &sOld );
    void Assign ( const SITEMCUSTOM_106 &sOld );
    void Assign ( const SITEMCUSTOM_107 &sOld );
    void Assign ( const SITEMCUSTOM_108 &sOld );
    void Assign ( const SITEMCUSTOM_109 &sOld );
    void Assign ( const SITEMCUSTOM_110 &sOld );
    void Assign ( const SITEMCUSTOM_111 &sOld );
    void Assign ( const SITEMCUSTOM_112 &sOld );
    void Assign ( const SITEMCUSTOM_113 &sOld );
	void Assign ( const SITEMCUSTOM_114 &sOld );
    void Assign ( const SITEMCUSTOM_115 &sOld );
    void Assign ( const SITEMCUSTOM_116 &sOld );
	void Assign ( const SITEMCUSTOM_117 &sOld );
	void Assign ( const SITEMCUSTOM_118 &sOld );
	void Assign ( const SITEMCUSTOM_119 &sOld );
	void Assign ( const SITEMCUSTOM_120 &sOld );
	void Assign ( const SITEMCUSTOM_121& sOld );
	void Assign ( const SITEMCUSTOM_122& sOld );
	void Assign ( const SITEMCUSTOM_123& sOld );
	void Assign ( const SITEMCUSTOM_124& sOld );
    void Assign ( const SITEM_LOBY &sOldLoby );
    void Assign ( const SITEMCLIENT &sClient );

    void AutoAssign( se::ByteStream& ByteStream );
	void AutoAssignForMigration( se::ByteStream& ByteStream );

	bool IS_GUID_NULL();

    //! 고유 ID (아이템 설정 최초 생성시에 부여되는 고유 ID)
    SNATIVEID GetNativeID() const;
	inline SNATIVEID GetRealNativeID() const { return sNativeID; }
    inline void SetNativeID( const SNATIVEID& sNativeID_ ){ sNativeID = sNativeID_; }
    

    inline WORD Mid() const { return sNativeID.wMainID; }
    inline WORD Sid() const { return sNativeID.wSubID; }
    inline DWORD Id() const { return sNativeID.dwID; }

    //! 아이템 생성시간, 시한부 아이템일 경우 유효 기간.
    inline void       BornTimeSet(const __time64_t& BornTime) { tBORNTIME=BornTime; }
    inline __time64_t BornTime() const { return tBORNTIME; }

    //! 사용가능량
    inline void TurnNumSet(WORD TurnNum) { wTurnNum=TurnNum; }
    inline WORD TurnNum() const { return wTurnNum; }

    //! 생성 타입
    inline EMITEMGEN GenType() const { return EMITEMGEN(cGenType); }
    inline void GenTypeSet(EMITEMGEN Type) { cGenType=static_cast<BYTE>(Type); }

    //! 채널 번호
    inline BYTE PeriodExtentionCount() const { return cPeriodExtensionCount; }
    inline void PeriodExtentionCountSet(BYTE periodExtentionCount) { cPeriodExtensionCount=periodExtentionCount; }

    //! 필드 번호
    inline BYTE CostumePeriodExtensionCount() const { return cCostumePeriodExtensionCount; }
    inline void CostumePeriodExtensionCountSet(BYTE costumePeriodExtensionCount) { cCostumePeriodExtensionCount=costumePeriodExtensionCount; }

    //! 생성 번호
    //inline LONGLONG GenNum() const { return lnGenNum; }
    //inline void     GenNumSet(LONGLONG GenNum) { lnGenNum=GenNum; }    

	void Initialize( const SNATIVEID& sNID );

    //! 내구도
    inline void SetDurability( WORD wValue ) { wDurability=wValue; }

    void SetGM_GEN ( DWORD dwUserNum )
    {		
        // 이 부분을 수정함으로서 GMCharEdit 에서 넣은 아이템의 거래/비거래 설정이 가능하다.
        // 대만 요구로 거래 제한 장치를 없앤다.
        // 2005-12-16 Jgkim
        // cGenType = EMGEN_MOB; /// 일반 몹에서 떨어진 걸로 처리하면 거래가능
        cGenType = EMGEN_GMEDIT; /// GMCharEdit 로 넣었을 경우 거래불가
        //lnGenNum = dwUserNum;
    }

    void SetGM_GEN2 ( DWORD dwUserNum )
    {		
        // 이 부분을 수정함으로서 GMCharEdit 에서 넣은 아이템의 거래/비거래 설정이 가능하다.
        // 대만 요구로 거래 제한 장치를 없앤다.
        // 2005-12-16 Jgkim
        // cGenType = EMGEN_MOB; /// 일반 몹에서 떨어진 걸로 처리하면 거래가능
        cGenType = EMGEN_GMEDIT2; /// GMCharEdit 로 넣었을 경우 거래불가
        //lnGenNum = dwUserNum;
    }

	void SetGM_GEN_RANDOMGENNUM( const bool bEnableTrade, const sc::MGUID& id );

    //! GMCharEdit 로 생성된 아이템인지 검사한다.
    bool IsGM_GENITEM () const
    {
        return cGenType == EMGEN_GMEDIT;
    }

    //! 탈것 ID (DB번호)
    inline DWORD VehicleDbNum() const { return m_VehicleDbNum; }
    inline void  VehicleDbNumSet(DWORD DbNum) { m_VehicleDbNum=DbNum; }

    //! 펫아디 (DB 번호)
    inline DWORD PetDbNum() const { return m_PetDbNum; }
    inline void  PetDbNumSet(DWORD DbNum) { m_PetDbNum=DbNum; }

	// 기본 능력치 공격력, 방어력 세팅;
	inline void SetBasicAttackDamage ( const DWORD _dwBasicAttackDamageData ) { sBasicInfo.SetBasicAttackDamage( _dwBasicAttackDamageData ); }
	inline void SetBasicAttackDamage ( const GLPADATA& _sBasicAttackDamage ) { sBasicInfo.SetBasicAttackDamage( _sBasicAttackDamage ); }
	inline void SetBasicDefence ( const WORD _wBasicDefence ) { sBasicInfo.SetBasicDefence( _wBasicDefence ); }

	// 링크 스킬 세팅;
	inline void SetLinkSkillTarget ( const EMITEM_SKILL_LINK _emLinkSkillTarget ) { sBasicInfo.SetLinkSkillTarget( _emLinkSkillTarget ); }
	inline void SetLinkSkillID ( const SNATIVEID& _sLinkSkillID ) { sBasicInfo.SetLinkSkillID( _sLinkSkillID ); }
	inline void SetLinkSkillLevel ( const WORD _wLinkSkillLevel ) { sBasicInfo.SetLinkSkillLevel( _wLinkSkillLevel ); }
	inline void SetLinkSkillOccurRate ( const float _fLinkSkillOccurRate ) { sBasicInfo.SetLinkSkillOccurRate( _fLinkSkillOccurRate ); }

public:
	// 기본능력치 공격력, 방어력 가져가기;
	inline const GLPADATA& GetBasicAttackDamage ( void ) const { return sBasicInfo.GetBasicAttackDamage(); }
	inline const WORD GetBasicDefence ( void ) const { return sBasicInfo.GetBasicDefence(); }

	// 링크 스킬 가져가기;
	inline const EMITEM_SKILL_LINK GetLinkSkillTarget ( void ) const { return sBasicInfo.GetLinkSkillTarget(); }
	inline const SNATIVEID& GetLinkSkillID ( void ) const { return sBasicInfo.GetLinkSkillID(); }
	inline const WORD GetLinkSkillLevel ( void ) const { return sBasicInfo.GetLinkSkillLevel(); }
	inline const float GetLinkSkillOccurRate ( void ) const { return sBasicInfo.GetLinkSkillOccurRate(); }

public:
	EMITEMLEVEL GetItemLevel () const;

    WORD GETGRADE_DAMAGE ( float fDurability = 0.0f) const;	
	//	연마 등급에 따른 공격력 가산량.
    //  부스트 값이 적용되어 계산이 필요 할 수 있음
	WORD GETGRADE_DEFENSE () const;						//	연마 등급에 따른 방어력 가산량.
    WORD GETGRADE_RESIST_FIRE () const;					//	연마 등급에 따른 불저항 가산량.
    WORD GETGRADE_RESIST_ICE () const;					//	연마 등급에 따른 빙저항 가산량.
    WORD GETGRADE_RESIST_ELEC () const;					//	연마 등급에 따른 전저항 가산량.
    WORD GETGRADE_RESIST_POISON () const;				//	연마 등급에 따른 독저항 가산량.
    WORD GETGRADE_RESIST_SPIRIT () const;				//	연마 등급에 따른 기저항 가산량.

    BYTE GETGRADE ( EMGRINDING_TYPE emTYPE ) const;
    BYTE GETGRADE_EFFECT () const;

protected:
    BYTE grade_damage () const;
    BYTE grade_defense () const;
    BYTE grade_fire () const				{ return cRESIST_FIRE; }
    BYTE grade_ice () const					{ return cRESIST_ICE; }
    BYTE grade_elec () const				{ return cRESIST_ELEC; }
    BYTE grade_poison () const				{ return cRESIST_POISON; }
    BYTE grade_spirit () const				{ return cRESIST_SPIRIT; }

public:
    GLDWDATA GETDAMAGE (EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;			//	기본 대미지 + 랜덤옵 + 연마.
    WORD GETMaDAMAGE () const;				//	기본 마력치 + 랜덤옵 + 연마.
    short GETDEFENSE (EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;				//	기본 방어력 + 랜덤옵 + 연마.
    short GETHITRATE () const;				//	기본 명중율 + 랜덤옵.
    short GETAVOIDRATE () const;			//	기본 명중율 + 랜덤옵.

    WORD GETATTRANGE () const;				//	기본 공격거리 + 랜덤옵.
    WORD GETREQ_SP () const;				//	기본 sp소모 + 랜덤옵.

    WORD GETRESIST_FIRE ( float fDurability = 0.0f ) const;			//	기본 저항(화) * ( 부스트 증가값 ) + 랜덤옵.
    WORD GETRESIST_ICE ( float fDurability = 0.0f ) const;			//	기본 저항(빙) * ( 부스트 증가값 ) + 랜덤옵.
    WORD GETRESIST_ELEC ( float fDurability = 0.0f ) const;			//	기본 저항(전) * ( 부스트 증가값 ) + 랜덤옵.
    WORD GETRESIST_POISON ( float fDurability = 0.0f ) const;			//	기본 저항(독) * ( 부스트 증가값 ) + 랜덤옵.
    WORD GETRESIST_SPIRIT ( float fDurability = 0.0f ) const;			//	기본 저항(기) * ( 부스트 증가값 ) + 랜덤옵.

    int GETADDHP () const;					//	부가 옵션 hp 증가 + 랜덤옵.
    int GETADDMP () const;					//	부가 옵션 hp 증가 + 랜덤옵.
    int GETADDSP () const;					//	부가 옵션 hp 증가 + 랜덤옵.
    int GETADDMA () const;					//	부가 옵션 hp 증가 + 랜덤옵. 

    float GETINCHP () const;				//	특수 옵션 hp 증가율 + 랜덤옵.
    float GETINCMP () const;				//	특수 옵션 mp 증가율 + 랜덤옵.
    float GETINCSP () const;				//	특수 옵션 sp 증가율 + 랜덤옵.
    float GETINCAP () const;				//	특수 옵션 ap 증가율 + 랜덤옵.

    float GETMOVESPEED() const;				//  특수 옵션 이동속도 증가 + 랜덤옵
    float GETMOVESPEEDR() const;			//  특수 옵션 이동속도 증가율 

	// [shhan][2015.01.27] rm #763
	//						[MY]코스튬 이동속도 효과 수정요청
	//						SUM_ITEM_BASIC(), GETMOVESPEED() 에 코스튬 수치가 합산되고
	//						SUM_ITEM_COSTUM(), 에서 sVOLUME 값을 또 더해서 문제가 된다.
	//						SUM_ITEM_BASIC() 작업시 GETMOVESPEED_EXCEPTDISGUISE() 것을 사용하도록 한다.
	float GETMOVESPEED_EXCEPTDISGUISE() const;	// GETMOVESPEED 에서 코스튬의 sVOLUME 값이 빠진것.

    std::string GETNAME() const;            //  아이템 이름;
public:
    GLPADATA getdamage (float fDurability = 0.0f, EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;	//	대미지 + 랜덤옵 힘.
    WORD getmadamage (float fDurability = 0.0f) const;		//	마력 + 랜덤옵 마력.
    short getdefense (EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;		//	방어력 + 랜덤옵 방어.

public:
	const int GetOptIndex ( const RandomOption::RANDOM_OPTION _typeRandomOption ) const;
	float GETOptVALUE ( const RandomOption::RANDOM_OPTION _typeRandomOption ) const;
    short GETOptVALUE_SHORT ( const RandomOption::RANDOM_OPTION _typeRandomOption ) const;

public:
    const RandomOption::RANDOM_OPTION GETOptTYPE1 () const	{	return (RandomOption::RANDOM_OPTION)randomOption.getType(0); }
    const RandomOption::RANDOM_OPTION GETOptTYPE2 () const	{	return (RandomOption::RANDOM_OPTION)randomOption.getType(1); }
    const RandomOption::RANDOM_OPTION GETOptTYPE3 () const	{	return (RandomOption::RANDOM_OPTION)randomOption.getType(2); }
    const RandomOption::RANDOM_OPTION GETOptTYPE4 () const	{	return (RandomOption::RANDOM_OPTION)randomOption.getType(3); }
	const RandomOption::RANDOM_OPTION GETOptTYPE5 () const	{	return (RandomOption::RANDOM_OPTION)randomOption.getType(4); }
	const RandomOption::RANDOM_OPTION getOptType(const DWORD nIndex) const { return (RandomOption::RANDOM_OPTION)randomOption.getType(nIndex); }

    float GETOptVALUE1 () const;
    float GETOptVALUE2 () const;
    float GETOptVALUE3 () const;
    float GETOptVALUE4 () const;
	float GETOptVALUE5 () const;
	float getOptValue(const DWORD nIndex) const;

	const DWORD getOptCount(const DWORD nIndex) const;

    BOOL IsPerRandOpt( INT nRandOPT ) const;
    BOOL IsSetRandOpt() const;

protected:
    float GETOptVALUE ( const RandomOption::RANDOM_OPTION _typeRandomOption, short nVALUE ) const;
    const bool SETOptVALUE ( int nIndex, const RandomOption::RANDOM_OPTION _typeRandomOption, float fVALUE );
    const bool SETOptVALUEEmpty( const RandomOption::RANDOM_OPTION _typeRandomOption, float fVALUE );	// ITEMREBUILD_MARK
	int RESETOptVALUE(const ITEM_RANDOMOP_UTILL::FixOption& fixOption, const DWORD nFixMaximum);   

	//float GetCostumeUserVALUE();

public:
	// 기본 능력치 랜덤 부여;
	const bool GenerateBasicStat ( const bool _bDropItem = false, const WORD _wFixedIndex = 0 );

	// 링크 스킬 랜덤 부여;
	const bool GenerateLinkSkill ( void );

	// 가산 옵션 랜덤 부여;
	const bool GenerateAddOption ( const bool _bDropItem = false );

	// 랜덤옵션 부여;
	const bool GENERATE_RANDOM_OPT(const bool bGenItem = true, const ITEM_RANDOMOP_UTILL::FixOption& fixOption = ITEM_RANDOMOP_UTILL::FixOption(), SITEM* pItemStamp = 0);

	// 현재 아이템의 랜덤옵션 값이 유효한지 확인한다;
	const bool ConfirmValidRandomOpt ();

//    int GENERATE_RANDOM_OPT_SE( const SRANDOM_OPT_SET* pOptSet );
    
    void SetDefaultValue ( const SITEM* pItem );

public:
    //내구도
    WORD GetDurability(){ return wDurability; };

    bool IsBoxWrapped() const;     // 포장되어있는가;
    bool BoxWrapping(const SNATIVEID& _wrapperID); // 박스 포장지 설정;
    bool BoxUnwrapping(); // 박스 포장지 해제;
    SNATIVEID GetBoxWrappedItemID() const; // 포장지에 포장된 아이템의 ID;

	void ResetInvest();
	void TempEnchantRecovery();

	void SetGuid( const sc::MGUID& _guid ) { guid = _guid; }
	sc::MGUID GetGuid() const { return guid; }
	void SetDbState( int emDbState, bool bForce = false )
	{
		itemState.setState( emDbState, bForce );
	}
	const int GetDbState() const { return itemState.getState(); }

	// 기능 연장;
	__time64_t GetPeriodExBasic(void)	const { return tPERIODEX_BASIC; }
	__time64_t GetPeriodExDisguise(void) const { return tPERIODEX_DISGUISE; }
	__time64_t GetPeriodExBasic_ConvertDBData(void) const;
	__time64_t GetPeriodExDisguise_ConvertDBData(void) const;
	void SetPeriodExBasic(__time64_t tBasic)		 { tPERIODEX_BASIC = tBasic; }
	void SetPeriodExDisguise(__time64_t tDisguise)  { tPERIODEX_DISGUISE = tDisguise; }
	void SetPeriodExBasic_FromDB(__time64_t tBasic);
	void SetPeriodExDisguise_FromDB(__time64_t tDisguise);
	void ResetPeriodExBasic(void) { tPERIODEX_BASIC = 0; cPeriodExtensionCount = 0; }
	void ResetPeriodExDisguise(void) { tPERIODEX_DISGUISE = 0; cCostumePeriodExtensionCount = 0; }
    
public :
	/*────────────────────────────┐
	
　　　　　　	　 　변경하면 안 되는 정보라
	   　　　[ GLItemBasic - SItemBasic - DWORD dwFlags ]
	  　　　　　　　사용하지 못하게 막아놓고
	　　　　　　　[ GLItemCustom - SITEMCUSTOM ]
	   　　　　　　　　사용하게 하는 것 임.
		   
		　  SITEM 은 기본 아이템 정보라서 변경 하면 안 됨.
	　　SITEMCUSTOM 은 사용자가 변경할 수 있는 정보라 변경 가능.
	　　　　	   고로 SITEMCUSTOM 에서 불러와야 함.

	　　　　　　　　　　　- 140424 sjkim
	
	└────────────────────────────*/

	//! Trade Flag
	bool IsSale()		 const;		// 상점에 판매 가능
	bool IsExchange()	 const;		// 개인간 교환 가능
	bool IsNotStorage()	 const;		// 창고 사용 가능 여부
	bool IsThrow()		 const;		// 필드에 버리기 가능
	bool IsEvent()		 const;		// 여러 이벤트 아이템중 한개 이상 못 줍게 하기 위한
	bool IsGarbage()	 const;		// 파쇄 가능

	//! Item Flag
	bool IsDisguise()	 const;		// 코스툼
	bool IsTimeLimit( EMCHECKFLAG_OPTION emFlag = CHECKFLAGOP_ALL )	 const;		// 시효성 - 시한부 아이템일 경우 유효 기간
	bool IsChannel_All() const;		// 모든 채널
	bool IsRestrict()	 const;		// 기능 제한
    bool IsBoxWrappable() const;    // 포장지로 포장 가능여부;
	bool IsDecompose() const;			// 분해가능 여부;
	bool IsCSApplied()	const;	// 코스튬 능력치 적용
	bool IsTempEnchant() const;
	bool IsOpItem(void) const;	// 랜덤옵션 수치가 min,max 범위를 벗어났는가?;

private:
    
    bool CheckItemFlag( EMITEM_FLAG emItemFlag, EMCHECKFLAG_OPTION emFlag = CHECKFLAGOP_ALL ) const;       // 아이템의 플레그를 검사한다.

public:
    friend struct SITEMCLIENT;
    friend class GLItemLimit;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & sNativeID;
		ar & nidDISGUISE;
		ar & tBORNTIME;
		ar & tDISGUISE;
		ar & tPERIODEX_BASIC;
		ar & tPERIODEX_DISGUISE;

		//ar & lnGenNum;
		ar & wTurnNum;
		ar & cGenType;
		ar & cPeriodExtensionCount;
		ar & cCostumePeriodExtensionCount;


		ar & cDAMAGE;
		ar & cDEFENSE;
		ar & cRESIST_FIRE;
		ar & cRESIST_ICE;
		ar & cRESIST_ELEC;
		ar & cRESIST_POISON;
		ar & cRESIST_SPIRIT;

		ar & randomOption;


		ar & m_PetDbNum;
		ar & m_VehicleDbNum;

		ar & bVietnamGainItem;

		ar & dwMainColor;
		ar & dwSubColor;

        ar & wDurability;
        ar & dwDurabilityCount;

        ar & wRanPointTradeCount;

		ar & costumeUser;

		ar & sTLGrind;

		ar & guid;
		ar & itemState;

		ar & sBasicInfo;
		ar & sAddonInfo;
	}
};

BOOL SETPUTONITEMS_BYBUF ( SITEMCUSTOM *pPutOnItems, se::ByteStream &ByteStream );
BOOL SETPUTONITEMS_BYBUF_FOR_MIGRATION( SITEMCUSTOM *pPutOnItems, se::ByteStream &ByteStream );