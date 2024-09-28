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
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cFieldID;					//	�ʵ� ��ȣ.
    LONGLONG	lnGenNum;					//	���� ��ȣ.

    BYTE		cReModelReq;				//	���� �õ� Ƚ��.
    BYTE		cReModelSuc;				//	���� ���� Ƚ��.

    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

    short		nHitRate;					//	������. ( +/- ) (%)
    short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

    short		nDamage;					//	�����. ( +/- )
    short		nDefense;					//	���. ( +/- )

    SRESIST		sResist;					//	���װ�.
};

struct SITEMCUSTOM_104
{
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.

    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cFieldID;					//	�ʵ� ��ȣ.
    LONGLONG	lnGenNum;					//	���� ��ȣ.

    BYTE		cReModelReq;				//	���� �õ� Ƚ��.
    BYTE		cReModelSuc;				//	���� ���� Ƚ��.

    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

    short		nHitRate;					//	������. ( +/- ) (%)
    short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

    short		nDamage;					//	�����. ( +/- )
    short		nDefense;					//	���. ( +/- )

    SRESIST		sResist;					//	���װ�.
};

struct SITEMCUSTOM_105
{
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.

    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cFieldID;					//	�ʵ� ��ȣ.
    LONGLONG	lnGenNum;					//	���� ��ȣ.

    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.

    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_106
{
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.

    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cFieldID;					//	�ʵ� ��ȣ.
    LONGLONG	lnGenNum;					//	���� ��ȣ.

    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.

    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_107
{
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cFieldID;					//	�ʵ� ��ȣ.
    LONGLONG	lnGenNum;					//	���� ��ȣ.

    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.

    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_108
{
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cChnID;						//	ä�� ��ȣ.
    BYTE		cFieldID;					//	�ʵ� ��ȣ.
    LONGLONG	lnGenNum;					//	���� ��ȣ.

    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )

    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.

    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.
};

struct SITEMCUSTOM_109
{
public:
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    LONGLONG	lnGenNum;					//	���� ��ȣ.
    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cChnID;						//	ä�� ��ȣ.

    BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.
    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
    //	���� �ɼ� ���.
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
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    LONGLONG	lnGenNum;					//	���� ��ȣ.
    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cChnID;						//	ä�� ��ȣ.

    BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.
    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
    //	���� �ɼ� ���.
    BYTE		cOptTYPE1;
    BYTE		cOptTYPE2;
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1;
    short		nOptVALUE2;
    short		nOptVALUE3;
    short		nOptVALUE4;

    DWORD		dwPetID;	// ��Ƶ�
};

struct SITEMCUSTOM_111
{
public:
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    LONGLONG	lnGenNum;					//	���� ��ȣ.
    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cChnID;						//	ä�� ��ȣ.

    BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.
    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
    //	���� �ɼ� ���.
    BYTE		cOptTYPE1;
    BYTE		cOptTYPE2;
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1;
    short		nOptVALUE2;
    short		nOptVALUE3;
    short		nOptVALUE4;

    DWORD		dwPetID;	// ��Ƶ�
    DWORD		dwVehicleID; // Ż�;Ƶ�(DB)
};

struct SITEMCUSTOM_112
{
public:
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    LONGLONG	lnGenNum;					//	���� ��ȣ.
    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cChnID;						//	ä�� ��ȣ.

    BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.
    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
    //	���� �ɼ� ���.
    BYTE		cOptTYPE1; // ���ݷ�
    BYTE		cOptTYPE2; // ������
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1; // ���ݷ� %
    short		nOptVALUE2; // ������ %
    short		nOptVALUE3;
    short		nOptVALUE4;

public:
    DWORD		dwPetID;	 // ��Ƶ�
    DWORD		dwVehicleID; // Ż�;Ƶ�(DB)

    bool		bVietnamGainItem;	 //	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
};

struct SITEMCUSTOM_113
{
public:
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    LONGLONG	lnGenNum;					//	���� ��ȣ.
    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cChnID;						//	ä�� ��ȣ.

    BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.
    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
    //	���� �ɼ� ���.
    BYTE		cOptTYPE1; // ���ݷ�
    BYTE		cOptTYPE2; // ������
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1; // ���ݷ� %
    short		nOptVALUE2; // ������ %
    short		nOptVALUE3;
    short		nOptVALUE4;

public:
    DWORD		dwPetID;	 //! ��Ƶ� (DB ��ȣ)
    DWORD		dwVehicleID; //! Ż�� ID (DB��ȣ)

    bool		bVietnamGainItem;	 //	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )

    // ���� �ڽ�Ƭ ����. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

};

struct SITEMCUSTOM_114
{
public:
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//	���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//	���� ��ȣ.
	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.

	BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.
	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	BYTE		cOptTYPE1; // ���ݷ�
	BYTE		cOptTYPE2; // ������
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // ���ݷ� %
	short		nOptVALUE2; // ������ %
	short		nOptVALUE3;
	short		nOptVALUE4;

public:
	DWORD		dwPetID;	 //! ��Ƶ� (DB ��ȣ)
	DWORD		dwVehicleID; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;	 //	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
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

	// ����� ���� ���� ��(0�� ���� �˻�);
	const bool setOption(const BYTE nType, const float fValue);

	// ������ ���� ���� ���� ��;
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

		short		nOptValue; // ���� �ɼ� ��;
		BYTE		cOptType; // ���� �ɼ� ����;
		BYTE		cOptCount; // ���� ī��� ���� �� Ƚ��;
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



//	Note : SITEMCUSTOM - ������ �������� ����.
//		
//		���� ����� "struct SINVENITEM_SAVE" �� ������ ��������� �Ѵ�.
//		�Ʒ� �Լ��鵵 ���� ���濡 ���� ����� �κ��� �����ؾ� �Ѵ�.
//					"BOOL SCHARDATA2::GETPUTONITEMS_BYBUF ()"
//					"BOOL SETPUTONITEMS_BYBUF ()"
//					"BOOL GLInventory::SETITEM_BYBUFFER ()"
//					"BOOL GLQUEST::LOAD ()" - INVENQARRAY
//					"BOOL GLQUEST_STEP::LOAD ()" - INVENQARRAY



struct SITEMCUSTOM_115
{
public:
    SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//! ������ �����ð�, ���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    LONGLONG	lnGenNum;					//! ���� ��ȣ.
    WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//! ���� Ÿ��.
    BYTE		cChnID;						//! ä�� ��ȣ.
    BYTE		cFieldID;					//! �ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
    BYTE		cDEFENSE;					//! ���� ���� ���.
    BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
    BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
    //	���� �ɼ� ���.
    RandomOpt_0100 randomOption;

public:
    DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)
    DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

    bool		bVietnamGainItem;	 //	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )

    // ���� �ڽ�Ƭ ����. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

    //������
    WORD        wDurability;
    DWORD       dwDurabilityCount;

};


struct SITEMCUSTOM_116
{
public:
    SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//! ������ �����ð�, ���Ѻ� �������� ��� ��ȿ �Ⱓ.
    __time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

    LONGLONG	lnGenNum;					//! ���� ��ȣ.
    WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//! ���� Ÿ��.
    BYTE		cChnID;						//! ä�� ��ȣ.
    BYTE		cFieldID;					//! �ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
    BYTE		cDEFENSE;					//! ���� ���� ���.
    BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
    BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
    //	���� �ɼ� ���.
    RandomOpt_0100 randomOption;

public:
    DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)
    DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

    bool		bVietnamGainItem;	 //	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )

    // ���� �ڽ�Ƭ ����. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

    //������
    WORD        wDurability;
    DWORD       dwDurabilityCount;

};

struct SITEMCUSTOM_117
{	// by luxes.
public:
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�, ���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//! ���� ��ȣ.
	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	BYTE		cChnID;						//! ä�� ��ȣ.
	BYTE		cFieldID;					//! �ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
	WORD        wRanPointTradeCount;
};

struct SITEMCUSTOM_118
{	// by luxes.
public:
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�, ���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//! ���� ��ȣ.
	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	BYTE		cChnID;						//! ä�� ��ȣ.
	BYTE		cFieldID;					//! �ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
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
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�, ���Ѻ� �������� ��� ��ȿ �Ⱓ.
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ.

	LONGLONG	lnGenNum;					//! ���� ��ȣ.
	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	BYTE		cChnID;						//! ä�� ��ȣ.
	BYTE		cFieldID;					//! �ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
	WORD        wRanPointTradeCount;

	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;
};

struct SITEMCUSTOM_120
{
public:
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

public:
	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�;
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ

	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	BYTE		cPeriodExtensionCount;		//! �Ⱓ ���� ��밡�� Ƚ��.
	BYTE		cCostumePeriodExtensionCount;					//! �ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
	WORD        wRanPointTradeCount;


	// ���� �ڽ�Ƭ ��������
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;
};

struct SITEMCUSTOM_121
{
public:
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

public:
	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�;
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ

public:
	// �Ϲ� ������(�ڽ�Ƭ �ƴ�x) ��ȿ�� ���� �Ⱓ;
	// �Ϲ� �������� ��ȿ���� tBORNTIME(�����ð�) + ITEM::SDRUG::tTIME_LMT + tEXTENSIONT �� ���� �����ȴ�;
	// �׸��� �Ʒ��� �����͵��� DB�� ����ɶ� 1900-01-01�� �������� �����̵ȴ�;
	// 30�� �����̶�� 1900-01-30�� �����̵ȴ� �����ؾ���;
	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

public:
	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	BYTE		cPeriodExtensionCount;		//! �Ⱓ ���� ��밡�� Ƚ��.
	BYTE		cCostumePeriodExtensionCount;

public:
	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
	WORD        wRanPointTradeCount;


	// ���� �ڽ�Ƭ ��������
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;
};

struct SITEMCUSTOM_122
{
public:
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

public:
	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�;
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ

public:
	// �Ϲ� ������(�ڽ�Ƭ �ƴ�x) ��ȿ�� ���� �Ⱓ;
	// �Ϲ� �������� ��ȿ���� tBORNTIME(�����ð�) + ITEM::SDRUG::tTIME_LMT + tEXTENSIONT �� ���� �����ȴ�;
	// �׸��� �Ʒ��� �����͵��� DB�� ����ɶ� 1900-01-01�� �������� �����̵ȴ�;
	// 30�� �����̶�� 1900-01-30�� �����̵ȴ� �����ؾ���;
	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

public:
	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	BYTE		cPeriodExtensionCount;		//! �Ⱓ ���� ��밡�� Ƚ��.
	BYTE		cCostumePeriodExtensionCount;

public:
	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

public:
	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
	WORD        wRanPointTradeCount;


	// ���� �ڽ�Ƭ ��������
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;

	// �⺻ �ɷ�ġ;
	GLPADATA	sBasicAttackDamage;			// �⺻ ���ݷ�;
	WORD		wBasicDefence;				// �⺻ ����;
};

// SITEMCUSTOM �� ������ �⺻ ����;
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
	// �⺻ �ɷ�ġ;
	GLPADATA			m_sBasicAttackDamage;				// �⺻ ���ݷ�;
	WORD				m_wBasicDefence;					// �⺻ ����;

	// ��ũ ��ų;
	EMITEM_SKILL_LINK	m_emLinkSkillTarget;				// ��ũ ��ų Ÿ��;
	SNATIVEID			m_sLinkSkillID;						// ��ũ ��ų ID;
	int					m_wLinkSkillLevel;					// ��ũ ��ų Level;
	float				m_fLinkSkillOccurRate;				// ��ũ ��ų �߻� Ȯ��;


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
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�;
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ

	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

	//LONGLONG	lnGenNum;					//! ���� ��ȣ.
	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	//  [4/14/2016 gbgim]
	// ���� ������ ���� ��� �ش� ä�� ������ ����ϴ�(cChnID) �����Ͱ�;
	// ���Ǵ� ���� �����Ƿ� �Ⱓ���� ������ Ƚ�� ó�� ������ ����;
	// ���� �̸� Ŀ���ϱ� ���� �ϰ������� �����߿�����;
	//BYTE		cChnID;						//! ä�� ��ȣ.
	BYTE		cPeriodExtensionCount;		//! �Ⱓ ���� ��밡�� Ƚ��.
	//BYTE		cFieldID;					//! �ʵ� ��ȣ.
	BYTE		cCostumePeriodExtensionCount;					//! �ʵ� ��ȣ.

	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
	WORD        wRanPointTradeCount;


	// ���� �ڽ�Ƭ ��������
	COSTUMEUSERVALUE::sCostumeUser costumeUser;

	STLGrindInfo sTLGrind;

	sc::MGUID	guid;
	db::State   itemState;

	SITEMCUSTOM_BASIC sBasicInfo;
};

// �����ɼ� ������ �����;
struct SITEMCUSTOM_124
{
public:
	SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

	SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
	__time64_t	tBORNTIME;					//! ������ �����ð�;
	__time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ

	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

	//LONGLONG	lnGenNum;					//! ���� ��ȣ.
	WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//! ���� Ÿ��.
	//  [4/14/2016 gbgim]
	// ���� ������ ���� ��� �ش� ä�� ������ ����ϴ�(cChnID) �����Ͱ�;
	// ���Ǵ� ���� �����Ƿ� �Ⱓ���� ������ Ƚ�� ó�� ������ ����;
	// ���� �̸� Ŀ���ϱ� ���� �ϰ������� �����߿�����;
	//BYTE		cChnID;						//! ä�� ��ȣ.
	BYTE		cPeriodExtensionCount;		//! �Ⱓ ���� ��밡�� Ƚ��.
	//BYTE		cFieldID;					//! �ʵ� ��ȣ.
	BYTE		cCostumePeriodExtensionCount;					//! �ʵ� ��ȣ.

	BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
	BYTE		cDEFENSE;					//! ���� ���� ���.
	BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
	BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

	//	���� �ɼ� ���.
	RandomOpt_0100 randomOption;

	DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
	// ��� �ڽ����� ���; ���(true), ����(false)

	// ���� �ڽ�Ƭ ����. by luxes.
	DWORD		dwMainColor;
	DWORD		dwSubColor;

	//������
	WORD        wDurability;
	DWORD       dwDurabilityCount;

	// �� ����Ʈ �ŷ�
	WORD        wRanPointTradeCount;


	// ���� �ڽ�Ƭ ��������
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
		CHECKFLAGOP_NORMAL = 1,		// �Ϲ� �����۸� üũ;
		CHECKFLAGOP_COSTUME = 2,	// �ڽ�Ƭ�� üũ;
		CHECKFLAGOP_ALL = 3,		// ��� üũ;
	};

private:
    SNATIVEID	sNativeID;					//! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

public:
    SNATIVEID	nidDISGUISE;				//! �ڽ��� ���� ��Ų.
    __time64_t	tBORNTIME;					//! ������ �����ð�;
    __time64_t	tDISGUISE;					//! ���Ѻ� �ڽ����� ��� ��ȿ �Ⱓ
	
private:
	// �Ϲ� ������(�ڽ�Ƭ �ƴ�x) ��ȿ�� ���� �Ⱓ;
	// �Ϲ� �������� ��ȿ���� tBORNTIME(�����ð�) + ITEM::SDRUG::tTIME_LMT + tEXTENSIONT �� ���� �����ȴ�;
	// �׸��� �Ʒ��� �����͵��� DB�� ����ɶ� 1900-01-01�� �������� �����̵ȴ�;
	// 30�� �����̶�� 1900-01-30�� �����̵ȴ� �����ؾ���;
	__time64_t	tPERIODEX_BASIC;
	__time64_t	tPERIODEX_DISGUISE;

public:

    //LONGLONG	lnGenNum;					//! ���� ��ȣ.
    WORD		wTurnNum;					//! ��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//! ���� Ÿ��.
	//  [4/14/2016 gbgim]
	// ���� ������ ���� ��� �ش� ä�� ������ ����ϴ�(cChnID) �����Ͱ�;
	// ���Ǵ� ���� �����Ƿ� �Ⱓ���� ������ Ƚ�� ó�� ������ ����;
	// ���� �̸� Ŀ���ϱ� ���� �ϰ������� �����߿�����;
    //BYTE		cChnID;						//! ä�� ��ȣ.
	BYTE		cPeriodExtensionCount;		//! �Ⱓ ���� ��밡�� Ƚ��.
	//BYTE		cFieldID;					//! �ʵ� ��ȣ.
    BYTE		cCostumePeriodExtensionCount;					//! �ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//! ���ݷ� ���� ���.
    BYTE		cDEFENSE;					//! ���� ���� ���.
    BYTE		cRESIST_FIRE;				//! ����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//! ����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//! ����(��) ���� ���.
    BYTE		cRESIST_POISON;				//! ����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//! ����(��) ���� ���.

public:
    //	���� �ɼ� ���.
    RandomOpt randomOption;
	
public:
    DWORD		m_PetDbNum;	 //! ��Ƶ� (DB ��ȣ)

	DWORD		m_VehicleDbNum; //! Ż�� ID (DB��ȣ)

	bool		bVietnamGainItem;		//	��Ʈ�� Ž�� �κ� ������( ��Ʈ�� ���� )
										// ��� �ڽ����� ���; ���(true), ����(false)

    // ���� �ڽ�Ƭ ����. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

    //������
    WORD        wDurability;
    DWORD       dwDurabilityCount;

    // �� ����Ʈ �ŷ�
    WORD        wRanPointTradeCount;


	// ���� �ڽ�Ƭ ��������
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

    //! ���� ID (������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID)
    SNATIVEID GetNativeID() const;
	inline SNATIVEID GetRealNativeID() const { return sNativeID; }
    inline void SetNativeID( const SNATIVEID& sNativeID_ ){ sNativeID = sNativeID_; }
    

    inline WORD Mid() const { return sNativeID.wMainID; }
    inline WORD Sid() const { return sNativeID.wSubID; }
    inline DWORD Id() const { return sNativeID.dwID; }

    //! ������ �����ð�, ���Ѻ� �������� ��� ��ȿ �Ⱓ.
    inline void       BornTimeSet(const __time64_t& BornTime) { tBORNTIME=BornTime; }
    inline __time64_t BornTime() const { return tBORNTIME; }

    //! ��밡�ɷ�
    inline void TurnNumSet(WORD TurnNum) { wTurnNum=TurnNum; }
    inline WORD TurnNum() const { return wTurnNum; }

    //! ���� Ÿ��
    inline EMITEMGEN GenType() const { return EMITEMGEN(cGenType); }
    inline void GenTypeSet(EMITEMGEN Type) { cGenType=static_cast<BYTE>(Type); }

    //! ä�� ��ȣ
    inline BYTE PeriodExtentionCount() const { return cPeriodExtensionCount; }
    inline void PeriodExtentionCountSet(BYTE periodExtentionCount) { cPeriodExtensionCount=periodExtentionCount; }

    //! �ʵ� ��ȣ
    inline BYTE CostumePeriodExtensionCount() const { return cCostumePeriodExtensionCount; }
    inline void CostumePeriodExtensionCountSet(BYTE costumePeriodExtensionCount) { cCostumePeriodExtensionCount=costumePeriodExtensionCount; }

    //! ���� ��ȣ
    //inline LONGLONG GenNum() const { return lnGenNum; }
    //inline void     GenNumSet(LONGLONG GenNum) { lnGenNum=GenNum; }    

	void Initialize( const SNATIVEID& sNID );

    //! ������
    inline void SetDurability( WORD wValue ) { wDurability=wValue; }

    void SetGM_GEN ( DWORD dwUserNum )
    {		
        // �� �κ��� ���������μ� GMCharEdit ���� ���� �������� �ŷ�/��ŷ� ������ �����ϴ�.
        // �븸 �䱸�� �ŷ� ���� ��ġ�� ���ش�.
        // 2005-12-16 Jgkim
        // cGenType = EMGEN_MOB; /// �Ϲ� ������ ������ �ɷ� ó���ϸ� �ŷ�����
        cGenType = EMGEN_GMEDIT; /// GMCharEdit �� �־��� ��� �ŷ��Ұ�
        //lnGenNum = dwUserNum;
    }

    void SetGM_GEN2 ( DWORD dwUserNum )
    {		
        // �� �κ��� ���������μ� GMCharEdit ���� ���� �������� �ŷ�/��ŷ� ������ �����ϴ�.
        // �븸 �䱸�� �ŷ� ���� ��ġ�� ���ش�.
        // 2005-12-16 Jgkim
        // cGenType = EMGEN_MOB; /// �Ϲ� ������ ������ �ɷ� ó���ϸ� �ŷ�����
        cGenType = EMGEN_GMEDIT2; /// GMCharEdit �� �־��� ��� �ŷ��Ұ�
        //lnGenNum = dwUserNum;
    }

	void SetGM_GEN_RANDOMGENNUM( const bool bEnableTrade, const sc::MGUID& id );

    //! GMCharEdit �� ������ ���������� �˻��Ѵ�.
    bool IsGM_GENITEM () const
    {
        return cGenType == EMGEN_GMEDIT;
    }

    //! Ż�� ID (DB��ȣ)
    inline DWORD VehicleDbNum() const { return m_VehicleDbNum; }
    inline void  VehicleDbNumSet(DWORD DbNum) { m_VehicleDbNum=DbNum; }

    //! ��Ƶ� (DB ��ȣ)
    inline DWORD PetDbNum() const { return m_PetDbNum; }
    inline void  PetDbNumSet(DWORD DbNum) { m_PetDbNum=DbNum; }

	// �⺻ �ɷ�ġ ���ݷ�, ���� ����;
	inline void SetBasicAttackDamage ( const DWORD _dwBasicAttackDamageData ) { sBasicInfo.SetBasicAttackDamage( _dwBasicAttackDamageData ); }
	inline void SetBasicAttackDamage ( const GLPADATA& _sBasicAttackDamage ) { sBasicInfo.SetBasicAttackDamage( _sBasicAttackDamage ); }
	inline void SetBasicDefence ( const WORD _wBasicDefence ) { sBasicInfo.SetBasicDefence( _wBasicDefence ); }

	// ��ũ ��ų ����;
	inline void SetLinkSkillTarget ( const EMITEM_SKILL_LINK _emLinkSkillTarget ) { sBasicInfo.SetLinkSkillTarget( _emLinkSkillTarget ); }
	inline void SetLinkSkillID ( const SNATIVEID& _sLinkSkillID ) { sBasicInfo.SetLinkSkillID( _sLinkSkillID ); }
	inline void SetLinkSkillLevel ( const WORD _wLinkSkillLevel ) { sBasicInfo.SetLinkSkillLevel( _wLinkSkillLevel ); }
	inline void SetLinkSkillOccurRate ( const float _fLinkSkillOccurRate ) { sBasicInfo.SetLinkSkillOccurRate( _fLinkSkillOccurRate ); }

public:
	// �⺻�ɷ�ġ ���ݷ�, ���� ��������;
	inline const GLPADATA& GetBasicAttackDamage ( void ) const { return sBasicInfo.GetBasicAttackDamage(); }
	inline const WORD GetBasicDefence ( void ) const { return sBasicInfo.GetBasicDefence(); }

	// ��ũ ��ų ��������;
	inline const EMITEM_SKILL_LINK GetLinkSkillTarget ( void ) const { return sBasicInfo.GetLinkSkillTarget(); }
	inline const SNATIVEID& GetLinkSkillID ( void ) const { return sBasicInfo.GetLinkSkillID(); }
	inline const WORD GetLinkSkillLevel ( void ) const { return sBasicInfo.GetLinkSkillLevel(); }
	inline const float GetLinkSkillOccurRate ( void ) const { return sBasicInfo.GetLinkSkillOccurRate(); }

public:
	EMITEMLEVEL GetItemLevel () const;

    WORD GETGRADE_DAMAGE ( float fDurability = 0.0f) const;	
	//	���� ��޿� ���� ���ݷ� ���귮.
    //  �ν�Ʈ ���� ����Ǿ� ����� �ʿ� �� �� ����
	WORD GETGRADE_DEFENSE () const;						//	���� ��޿� ���� ���� ���귮.
    WORD GETGRADE_RESIST_FIRE () const;					//	���� ��޿� ���� ������ ���귮.
    WORD GETGRADE_RESIST_ICE () const;					//	���� ��޿� ���� ������ ���귮.
    WORD GETGRADE_RESIST_ELEC () const;					//	���� ��޿� ���� ������ ���귮.
    WORD GETGRADE_RESIST_POISON () const;				//	���� ��޿� ���� ������ ���귮.
    WORD GETGRADE_RESIST_SPIRIT () const;				//	���� ��޿� ���� ������ ���귮.

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
    GLDWDATA GETDAMAGE (EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;			//	�⺻ ����� + ������ + ����.
    WORD GETMaDAMAGE () const;				//	�⺻ ����ġ + ������ + ����.
    short GETDEFENSE (EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;				//	�⺻ ���� + ������ + ����.
    short GETHITRATE () const;				//	�⺻ ������ + ������.
    short GETAVOIDRATE () const;			//	�⺻ ������ + ������.

    WORD GETATTRANGE () const;				//	�⺻ ���ݰŸ� + ������.
    WORD GETREQ_SP () const;				//	�⺻ sp�Ҹ� + ������.

    WORD GETRESIST_FIRE ( float fDurability = 0.0f ) const;			//	�⺻ ����(ȭ) * ( �ν�Ʈ ������ ) + ������.
    WORD GETRESIST_ICE ( float fDurability = 0.0f ) const;			//	�⺻ ����(��) * ( �ν�Ʈ ������ ) + ������.
    WORD GETRESIST_ELEC ( float fDurability = 0.0f ) const;			//	�⺻ ����(��) * ( �ν�Ʈ ������ ) + ������.
    WORD GETRESIST_POISON ( float fDurability = 0.0f ) const;			//	�⺻ ����(��) * ( �ν�Ʈ ������ ) + ������.
    WORD GETRESIST_SPIRIT ( float fDurability = 0.0f ) const;			//	�⺻ ����(��) * ( �ν�Ʈ ������ ) + ������.

    int GETADDHP () const;					//	�ΰ� �ɼ� hp ���� + ������.
    int GETADDMP () const;					//	�ΰ� �ɼ� hp ���� + ������.
    int GETADDSP () const;					//	�ΰ� �ɼ� hp ���� + ������.
    int GETADDMA () const;					//	�ΰ� �ɼ� hp ���� + ������. 

    float GETINCHP () const;				//	Ư�� �ɼ� hp ������ + ������.
    float GETINCMP () const;				//	Ư�� �ɼ� mp ������ + ������.
    float GETINCSP () const;				//	Ư�� �ɼ� sp ������ + ������.
    float GETINCAP () const;				//	Ư�� �ɼ� ap ������ + ������.

    float GETMOVESPEED() const;				//  Ư�� �ɼ� �̵��ӵ� ���� + ������
    float GETMOVESPEEDR() const;			//  Ư�� �ɼ� �̵��ӵ� ������ 

	// [shhan][2015.01.27] rm #763
	//						[MY]�ڽ�Ƭ �̵��ӵ� ȿ�� ������û
	//						SUM_ITEM_BASIC(), GETMOVESPEED() �� �ڽ�Ƭ ��ġ�� �ջ�ǰ�
	//						SUM_ITEM_COSTUM(), ���� sVOLUME ���� �� ���ؼ� ������ �ȴ�.
	//						SUM_ITEM_BASIC() �۾��� GETMOVESPEED_EXCEPTDISGUISE() ���� ����ϵ��� �Ѵ�.
	float GETMOVESPEED_EXCEPTDISGUISE() const;	// GETMOVESPEED ���� �ڽ�Ƭ�� sVOLUME ���� ������.

    std::string GETNAME() const;            //  ������ �̸�;
public:
    GLPADATA getdamage (float fDurability = 0.0f, EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;	//	����� + ������ ��.
    WORD getmadamage (float fDurability = 0.0f) const;		//	���� + ������ ����.
    short getdefense (EMCHARINDEX charIdx = GLCI_NUM_ACTOR, int nLevel = -1) const;		//	���� + ������ ���.

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
	// �⺻ �ɷ�ġ ���� �ο�;
	const bool GenerateBasicStat ( const bool _bDropItem = false, const WORD _wFixedIndex = 0 );

	// ��ũ ��ų ���� �ο�;
	const bool GenerateLinkSkill ( void );

	// ���� �ɼ� ���� �ο�;
	const bool GenerateAddOption ( const bool _bDropItem = false );

	// �����ɼ� �ο�;
	const bool GENERATE_RANDOM_OPT(const bool bGenItem = true, const ITEM_RANDOMOP_UTILL::FixOption& fixOption = ITEM_RANDOMOP_UTILL::FixOption(), SITEM* pItemStamp = 0);

	// ���� �������� �����ɼ� ���� ��ȿ���� Ȯ���Ѵ�;
	const bool ConfirmValidRandomOpt ();

//    int GENERATE_RANDOM_OPT_SE( const SRANDOM_OPT_SET* pOptSet );
    
    void SetDefaultValue ( const SITEM* pItem );

public:
    //������
    WORD GetDurability(){ return wDurability; };

    bool IsBoxWrapped() const;     // ����Ǿ��ִ°�;
    bool BoxWrapping(const SNATIVEID& _wrapperID); // �ڽ� ������ ����;
    bool BoxUnwrapping(); // �ڽ� ������ ����;
    SNATIVEID GetBoxWrappedItemID() const; // �������� ����� �������� ID;

	void ResetInvest();
	void TempEnchantRecovery();

	void SetGuid( const sc::MGUID& _guid ) { guid = _guid; }
	sc::MGUID GetGuid() const { return guid; }
	void SetDbState( int emDbState, bool bForce = false )
	{
		itemState.setState( emDbState, bForce );
	}
	const int GetDbState() const { return itemState.getState(); }

	// ��� ����;
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
	/*����������������������������������������������������������
	
������������	�� �������ϸ� �� �Ǵ� ������
	   ������[ GLItemBasic - SItemBasic - DWORD dwFlags ]
	  ��������������������� ���ϰ� ���Ƴ���
	��������������[ GLItemCustom - SITEMCUSTOM ]
	   ��������������������ϰ� �ϴ� �� ��.
		   
		��  SITEM �� �⺻ ������ ������ ���� �ϸ� �� ��.
	����SITEMCUSTOM �� ����ڰ� ������ �� �ִ� ������ ���� ����.
	��������	   ��� SITEMCUSTOM ���� �ҷ��;� ��.

	����������������������- 140424 sjkim
	
	����������������������������������������������������������*/

	//! Trade Flag
	bool IsSale()		 const;		// ������ �Ǹ� ����
	bool IsExchange()	 const;		// ���ΰ� ��ȯ ����
	bool IsNotStorage()	 const;		// â�� ��� ���� ����
	bool IsThrow()		 const;		// �ʵ忡 ������ ����
	bool IsEvent()		 const;		// ���� �̺�Ʈ �������� �Ѱ� �̻� �� �ݰ� �ϱ� ����
	bool IsGarbage()	 const;		// �ļ� ����

	//! Item Flag
	bool IsDisguise()	 const;		// �ڽ���
	bool IsTimeLimit( EMCHECKFLAG_OPTION emFlag = CHECKFLAGOP_ALL )	 const;		// ��ȿ�� - ���Ѻ� �������� ��� ��ȿ �Ⱓ
	bool IsChannel_All() const;		// ��� ä��
	bool IsRestrict()	 const;		// ��� ����
    bool IsBoxWrappable() const;    // �������� ���� ���ɿ���;
	bool IsDecompose() const;			// ���ذ��� ����;
	bool IsCSApplied()	const;	// �ڽ�Ƭ �ɷ�ġ ����
	bool IsTempEnchant() const;
	bool IsOpItem(void) const;	// �����ɼ� ��ġ�� min,max ������ ����°�?;

private:
    
    bool CheckItemFlag( EMITEM_FLAG emItemFlag, EMCHECKFLAG_OPTION emFlag = CHECKFLAGOP_ALL ) const;       // �������� �÷��׸� �˻��Ѵ�.

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