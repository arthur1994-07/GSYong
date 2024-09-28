#ifndef GLDEFINE_H_
#define GLDEFINE_H_

#pragma once

#include <vector>
#include <set>
#include <cmath>
#include <cfloat>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "../../../RanThirdParty/DirectX/Include/d3dx9math.h"
#include "TypeDefine.h"

#define FLOAT_SET	0xe021f

#define CHR_ID_LENGTH 33
#define CHAR_SZNAME CHR_ID_LENGTH //	케릭터 이름 제한.
#define CLUB_NAME_LENGTH CHR_ID_LENGTH

FORCEINLINE bool cast_bool(BOOL b) { return b ? true : false; }

//
//mjeon.instance.transfer.serialization
//
namespace boost
{
	namespace serialization
	{
		//
		//MFC CString support
		//
		template <typename Archive>
		inline void save(Archive & ar, const CString& str, const unsigned int file_version)
		{
			std::wstring wstr(str.GetString());
			ar & BOOST_SERIALIZATION_NVP(wstr);
		}

		template <typename Archive>
		inline void load(Archive &ar, CString& str, const unsigned int file_version)
		{
			std::wstring wstr;
			ar & BOOST_SERIALIZATION_NVP(wstr);
			str = wstr.data();
		}

		template <typename Archive>
		inline void serialize(Archive &ar, CString &str, const unsigned int file_version)
		{
			boost::serialization::split_free(ar, str.GetString(), file_version);
		}

		//
		//MFC CTime support
		//
		template <typename Archive>
		inline void save(Archive & ar, const CTime& time, const unsigned int file_version)
		{
			__time64_t t = time.GetTime();
			ar & t;
		}

		template <typename Archive>
		inline void load(Archive &ar, CTime& time, const unsigned int file_version)
		{
			__time64_t t;
			ar & t;
			time = t;
		}

		template <typename Archive>
		inline void serialize(Archive &ar, CTime &time, const unsigned int file_version)
		{
			boost::serialization::split_free(ar, time, file_version);
		}
	}
}


inline unsigned int SetControlfp(bool bSet)
{
    unsigned int control_word = 0;
	if (bSet) 
	{
		_controlfp_s(&control_word, _PC_24, _MCW_PC);
		_controlfp_s(&control_word, _RC_UP, _MCW_RC);
		_controlfp_s(&control_word, _IC_AFFINE, _MCW_IC);
	}
	else
	{
		_controlfp_s(&control_word, _CW_DEFAULT, 0xfffff);
	}
	
	return _controlfp_s(&control_word, 0, 0);
}

inline unsigned int GetControlfp()
{
    unsigned int control_word = 0;
	_controlfp_s(&control_word, 0, 0);
    return control_word;
}

inline unsigned int CheckControlfp()
{
	unsigned int nFloat = GetControlfp();
	if( nFloat != FLOAT_SET ) 
	{
		return SetControlfp( true );	
	}

	return nFloat;
}

template < typename TYPE >
TYPE DECREASE ( TYPE &Value, const TYPE Dec )
{
	if ( Value >= Dec )
		return Value -= Dec;
	return Value = 0;
}

template < typename TYPE >
TYPE LIMIT ( TYPE &Value, const TYPE Max )
{
	if ( Value >= Max ) return Value = Max;
	return Value;
}

template < typename TYPE >
TYPE LIMIT ( TYPE &Value, const TYPE Max, const TYPE Min )
{
	GASSERT(Min<=Max);

	if ( Value >= Max ) return Value = Max;
	if ( Value <= Min ) return Value = Min;

	return Value;
}

enum EMGLSTEP
{
	GL_VERYHIGH		= 1,		//	매우 높음.
	GL_HIGH			= 2,		//	높음.
	GL_NORMAL		= 3,		//	보통.
	GL_LOW			= 4,		//	낮음.
	GL_VERYLOW		= 5,		//	매우 낮음.
};

struct SChinaTime
{
	int	userAge;
	// 저장된 오프라인 누적 시간
	int offlineTime;
	// 저장된 게임 누적 시간
	LONGLONG gameTime;
	// 현재 게임 누적 시간
	LONGLONG currentGameTime;
	// 로그인한 시간
	__time64_t loginTime;
	// 마지막 로그오프한 시간
	__time64_t lastOffLineTime;

	SChinaTime()
	{
		Init();
	}
	void Init()
	{
		userAge			= 0;
		offlineTime		= 0;
		gameTime		= 0;
		loginTime		= 0;
		lastOffLineTime = 0;
		currentGameTime = 0;
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & userAge;
		ar & offlineTime;
		ar & gameTime;
		ar & currentGameTime;
		ar & loginTime;
		ar & lastOffLineTime;
	}
};

struct SVietnamGainSystem
{	
	// 저장된 게임 누적 시간
	LONGLONG gameTime;
	// 현재 게임 누적 시간
	LONGLONG currentGameTime;
	// 로그인한 시간
	__time64_t loginTime;
	// 마지막 로그오프한 날짜, 시간
	__time64_t lastOffLineTime;


	SVietnamGainSystem()
	{
		Init();
	}
	void Init()
	{
		gameTime		= 0;
		loginTime		= 0;
		lastOffLineTime = 0;
		currentGameTime = 0;
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & gameTime;
		ar & currentGameTime;
		ar & loginTime;
		ar & lastOffLineTime;
	}
};


struct SPETSKINPACKDATA 
{
	SNATIVEID sMobID;			// 바뀔 몹 ID
	float	  fScale;			// 바뀐 몹의 크기
	DWORD	  dwPetSkinTime;		// 적용 되는 시간
	bool	  bUsePetSkinPack;  // 적용되었는지 아닌지
	CTime	  startTime;

	SPETSKINPACKDATA()
		: sMobID(false)
		, fScale(1.0f)
		, dwPetSkinTime(0)
		, bUsePetSkinPack(false)
	{
	}

	void Init()
	{
		sMobID			= false;
		fScale			= 1.0f;
		dwPetSkinTime   = 0;
		bUsePetSkinPack = false;
	}

};

struct STracingData 
{
	char   m_szAccount[CHAR_SZNAME];
	DWORD  dwUserNum;
	DWORD	dwTracingTime;
	BYTE   dwOnLine;
	int	   nConnectServer;

	STracingData()
	{
		dwUserNum     = 0;
        memset(m_szAccount, 0, CHAR_SZNAME);
		dwOnLine = 255;
		nConnectServer = -1;
	}

	STracingData(const std::string& str)
	{
		dwUserNum      = 0;
		StringCchCopy(m_szAccount, CHAR_SZNAME, str.c_str());
		dwOnLine = 255;
		nConnectServer = -1;
	}

	STracingData(DWORD dwUserNum)
	{
		dwUserNum = dwUserNum;
		memset(m_szAccount, 0, CHAR_SZNAME);
		dwOnLine = 255;
		nConnectServer = -1;
	}

    const STracingData& STracingData::operator = (const STracingData& rhs)
    {
        if (this == &rhs)
            return *this;

        SetAccount(rhs.m_szAccount);
        dwUserNum      = rhs.dwUserNum;
        dwOnLine       = rhs.dwOnLine;
        nConnectServer = rhs.nConnectServer;
		dwTracingTime = rhs.dwTracingTime;

        return *this;
    }

    void SetAccount(const char* szAccount)
    {
        if (szAccount)
            StringCchCopy(m_szAccount, CHAR_SZNAME, szAccount);
    }

    void ResetAccount()
    {
        memset(m_szAccount, 0, CHAR_SZNAME);
    }    
};


struct SEventTime
{
	// 현재 게임 누적 시간
	LONGLONG currentGameTime;
	// 로그인한 시간
	__time64_t loginTime;

	SEventTime()
	{
		Init();
	}
	void Init()
	{
		loginTime		 = 0;
		currentGameTime  = 0;
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & currentGameTime;
		ar & loginTime;
	}
};

struct GLPADATA
{
	union
	{
		struct
		{
			DWORD	dwData;
		};
		struct
		{
			WORD	wNow;
			WORD	wMax;
		};
		struct
		{
			WORD	wLow;
			WORD	wHigh;
		};
	};

    MSGPACK_DEFINE(dwData);

	GLPADATA () 
		: dwData(0)
	{
	}

	GLPADATA ( WORD wL, WORD wH )
	{
		wLow = wL;
		wHigh = wH;
	}

	const DWORD GetData ( void ) const { return dwData; }
	const WORD GetLow ( void ) const { return wLow; }
	const WORD GetHigh ( void ) const { return wHigh; }

	void LIMIT ()					{ if ( wNow > wMax )	wNow = wMax; }
	void CHECKMIN ( WORD wMin )		{ if ( wNow < wMin )	wNow = wMin; }
	void TO_FULL ()					{ wNow = wMax; }

	void SetData( const WORD now, const WORD max ) { wNow = now; wMax = max; }
	
	void DECREASE ( const WORD wValue )
	{
		if ( wNow >= wValue )	wNow -= wValue;
		else					wNow = 0;
	}

	//void INCREASE ( const WORD wValue, const BOOL bRate=FALSE )
	//{
	//	if ( bRate )	wNow += ( (wMax*wValue) / 100 );
	//	else			wNow += wValue;

	//	LIMIT ();
	//}

    void INCREASE ( const WORD wValue, const BOOL bRate=FALSE, const int nAddValue = 0 )
    {
        int nNow = static_cast<int>(wNow);

        if ( bRate )
            nNow += static_cast<int>( (wMax*wValue) / 100 ) + nAddValue;
        else
            nNow += static_cast<int>(wValue) + nAddValue;

        if ( nNow < 0 )
            nNow = 0;
        
        if ( nNow > USHRT_MAX )    //WORD 최댓값을 넘어가면
            nNow = USHRT_MAX;

        wNow = static_cast<WORD>(nNow);
        LIMIT ();
    }

	WORD VARIATION ( const int nValue )
	{
		const int nOld = int(wLow);
		const int nLow = nOld + nValue;

		if ( nLow > USHRT_MAX )
			wLow = USHRT_MAX;
		else if ( nLow < 0 )
			wLow = 0;
		else
			wLow = nLow;


		LIMIT();

		const WORD wVariation(WORD(::abs(nOld - int(wLow))));
		return wVariation;
	}

	void VAR_PARAM ( const int nValue )
	{ // wLow + nValue = 1~USHRT_MAX;
		const int nLow = int(wLow) + nValue;
		if ( nLow > USHRT_MAX )
			wLow = USHRT_MAX;
		else if ( nLow < 1 )
			wLow = 1;
		else
			wLow = static_cast<WORD>(nLow);	//개조 수치 미적용 되던 부분 수정

		const int nHigh = int(wHigh) + nValue;
		if ( nHigh > USHRT_MAX )
			wHigh = USHRT_MAX;
		else if ( nHigh < 1 )
			wHigh = 1;
		else
			wHigh = static_cast<WORD>(nHigh); //개조 수치 미적용 되던 부분 수정
	}

	BOOL operator== ( const GLPADATA& nID ) const
	{
		return (wNow==nID.wNow) && (wMax==nID.wMax);
	}

	BOOL operator!= ( const GLPADATA& nID ) const
	{
		return !( (wNow==nID.wNow) && (wMax==nID.wMax) );
	}

	GLPADATA& operator += ( const GLPADATA& nID )
	{
		const DWORD dwNow(DWORD(wNow) + DWORD(nID.wNow));
		const DWORD dwMax(DWORD(wMax) + DWORD(nID.wMax));

		wNow = dwNow > USHRT_MAX ? USHRT_MAX : WORD(dwNow);
		wMax = dwMax > USHRT_MAX ? USHRT_MAX : WORD(dwMax);		

		return *this;
	}

	// 이거 이래 하면 안된다;;
	// 고쳤다간 무슨 일이 벌어질지 모르니 못고침;
	GLPADATA& operator + ( const GLPADATA& nID )
	{
		const DWORD dwNow(DWORD(wNow) + DWORD(nID.wNow));
		const DWORD dwMax(DWORD(wMax) + DWORD(nID.wMax));

		wNow = dwNow > USHRT_MAX ? USHRT_MAX : WORD(dwNow);
		wMax = dwMax > USHRT_MAX ? USHRT_MAX : WORD(dwMax);		

		return *this;
	}

    GLPADATA& operator *= ( const float fValue )
    {
		const DWORD dwNow(static_cast<WORD>(float(wNow) * fValue));
		const DWORD dwMax(static_cast<WORD>(float(wMax) * fValue));

		wNow = dwNow > USHRT_MAX ? USHRT_MAX : WORD(dwNow);
		wMax = dwMax > USHRT_MAX ? USHRT_MAX : WORD(dwMax);		

        return *this;
    }

	// 이거 이래 하면 안된다;;
	// 고쳤다간 무슨 일이 벌어질지 모르니 못고침;
    GLPADATA& operator * ( const float fValue )
    {
		const DWORD dwNow(static_cast<WORD>(float(wNow) * fValue));
		const DWORD dwMax(static_cast<WORD>(float(wMax) * fValue));

		wNow = dwNow > USHRT_MAX ? USHRT_MAX : WORD(dwNow);
		wMax = dwMax > USHRT_MAX ? USHRT_MAX : WORD(dwMax);		

        return *this;
    }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & dwData;
	}
};

struct GLDWDATA
{
	union
	{
		struct
		{
			DWORD	dwData1;
			DWORD	dwData2;
		};
		struct
		{
			DWORD	dwNow;
			DWORD	dwMax;
		};
		struct
		{
			DWORD	dwLow;
			DWORD	dwHigh;
		};
		struct
		{
			int nNow;
			int nMax;
		};
	};

    MSGPACK_DEFINE(dwData1, dwData2);

	GLDWDATA(void)
		: dwData1(0)
		, dwData2(0)
	{
	}

	GLDWDATA(const DWORD dwL, const DWORD dwH)
		: dwLow(dwL)
		, dwHigh(dwH)
	{
	}
	GLDWDATA(const int _nNow, const int _nMax)
		: nNow(_nNow)
		, nMax(_nMax)
	{
	}

	GLDWDATA(const GLDWDATA& _rhs)
		: dwNow(_rhs.dwNow)
		, dwMax(_rhs.dwMax)
	{
	}

	GLDWDATA ( const GLPADATA& nID )
		: dwNow(DWORD(nID.wNow))
		, dwMax(DWORD(nID.wMax))
	{
	}

	const bool operator == ( const GLDWDATA& nID ) const
	{
		return (dwNow==nID.dwNow) && (dwMax==nID.dwMax);
	}

	const bool operator != ( const GLDWDATA& nID ) const
	{
		return !( (dwNow==nID.dwNow) && (dwMax==nID.dwMax) );
	}

	const GLDWDATA& operator += ( const GLDWDATA& nID )
	{
		dwNow+=nID.dwNow;
		dwMax+=nID.dwMax;

		return *this;
	}
	const GLDWDATA& operator += ( const GLPADATA& nID )
	{
		const int nNowNew(nNow + int(nID.wNow));
		const int nMaxNew(nMax + int(nID.wMax));

		nNow = nNowNew < 0 ? INT_MAX : nNowNew;
		nMax = nMaxNew < 0 ? INT_MAX : nMaxNew;

		return *this;
	}

// 	const GLDWDATA operator + ( const GLDWDATA& nID )
// 	{
// 		const int nNowNew(nNow + nID.nNow);
// 		const int nMaxNew(nMax + nID.nMax);
// 
// 		return GLDWDATA(nNowNew, nMaxNew);
// 	}

	const GLDWDATA operator *(const float _fRate)
	{
		const int nNowNew(int(float(nNow) * _fRate));
		const int nMaxNew(int(float(nMax) * _fRate));

		return GLDWDATA(nNowNew, nMaxNew);
	}

	void TO_FULL ()					{ dwNow = dwMax; }
	void LIMIT ();//					{ if ( nNow > nMax ) nNow = nMax; }
	void CHECKMIN ( const int nMin )		{ nNow = nNow < nMin ? nMin : nNow; }

	void DECREASE ( const int nValue )
	{
		const int nValueNew(nNow - nValue);
		nNow = nValueNew < 0 ? 0 : nValueNew;
	}

	void INCREASE(const int nValue, const bool bRate = false, const int nAddValue = 0)
	{
		int nNew(nNow);

		if ( bRate )
			nNew += ((nMax * nValue) / 100 + nAddValue);
		else
			nNew += (nValue + nAddValue);

		if ( nNew < 0 )
			nNew = 0;

		if ( nNew > nMax )
			nNew = nMax;

		nNow = nNew;
	}

	const int VARIATION ( const int nValue )
	{
		const int nOld(nNow);
		const int nNew(nNow + nValue);

		if ( nNew < 0 )
			nNow = 0;
		else
			nNow = nNew;

		LIMIT();

		const int nVariation(::abs(nOld - nNow));
		return nVariation;
	}

	void VAR_PARAM ( const int nValue )
	{
		const int _nLow(nNow + nValue);
		if ( _nLow < 0 )
			nNow = 0;
		else
			nNow = _nLow;	//개조 수치 미적용 되던 부분 수정

		const int _nHigh(nMax + nValue);
		if ( _nHigh < 0 )
			nMax = INT_MAX;
		else
			nMax = _nHigh; //개조 수치 미적용 되던 부분 수정
	}
private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & dwData1;
		ar & dwData2;
	}
};

struct GLLLDATA
{
	union
	{
		struct
		{
			LONGLONG	lnData1;
			LONGLONG	lnData2;
		};
		struct
		{
			LONGLONG	lnNow;
			LONGLONG	lnMax;
		};
		struct
		{
			LONGLONG	lnLow;
			LONGLONG	lnHigh;
		};
	};

    MSGPACK_DEFINE(lnData1, lnData2);

	GLLLDATA () :
		lnData1(0),
		lnData2(0)
	{
	}

	GLLLDATA ( DWORD lnwL, DWORD lnH )
	{
		lnLow = lnwL;
		lnHigh = lnH;
	}

	void LIMIT ()
	{
		if ( lnNow > lnMax )	lnNow = lnMax;
	}

	BOOL ISOVER  ()
	{
		if ( lnNow > lnMax )	return TRUE;
		return FALSE;
	}

	void DECREASE ( const WORD wValue )
	{
		if ( lnNow >= wValue )	lnNow -= wValue;
		else					lnNow = 0;
	}

	BOOL operator== ( const GLLLDATA& nID ) const
	{
		return (lnNow==nID.lnNow) && (lnMax==nID.lnMax);
	}

	BOOL operator!= ( const GLLLDATA& nID ) const
	{
		return !( (lnNow==nID.lnNow) && (lnMax==nID.lnMax) );
	}

	GLLLDATA& operator += ( const GLLLDATA& nID )
	{
		lnNow+=nID.lnNow;
		lnMax+=nID.lnMax;

		return *this;
	}

	GLLLDATA& operator + ( const GLLLDATA& nID )
	{
		lnNow+=nID.lnNow;
		lnMax+=nID.lnMax;

		return *this;
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & lnLow;
		ar & lnHigh;
	}
};

// WidgetDef.lua
enum EMCROW
{
	CROW_PC			= 0,
	CROW_NPC		= 1,
	CROW_MOB		= 2,
	CROW_ITEM		= 3,
	CROW_MONEY		= 4,
	CROW_PET		= 5,	// PetData
	CROW_SUMMON		= 6,	// PetData
	CROW_MATERIAL	= 7,	// 재료NPC
    CROW_ZONENAME   = 8,    // 지명 Name - LevelEdit에서 셋팅되는 것
    CROW_GATENAME   = 9,    // 게이트 이름 표시( Name ) - LevelEdit에서 셋팅되는 것

    // 게이트, 신규 인던용으로 임시 추가, 신규 인던 쪽에서만 사용;
    // 실제 게이트는 EMCROW 형태의 ID를 갖지 않는다;
    // EMCROW를 갖는 형태로 바꿔야 함;
    CROW_GATE	= 10, 

    CROW_NUM		= 11,
    CROW_NULL	= 0xffffffff,
};

enum EMCROW_EX
{
	CROW_EX_PC			= 0x0001,
	CROW_EX_NPC			= 0x0002,
	CROW_EX_MOB			= 0x0004, 
	CROW_EX_ITEM		= 0x0008,
	CROW_EX_MONEY		= 0x0010,
	CROW_EX_PET			= 0x0020,
	CROW_EX_SUMMON		= 0x0040,
	CROW_EX_MATERIAL	= 0x0080,

	CROW_EX_ALL		= ( CROW_EX_PC | CROW_EX_NPC | CROW_EX_MOB | CROW_EX_ITEM | CROW_EX_MONEY | CROW_EX_PET | 
						CROW_EX_SUMMON | CROW_EX_MATERIAL )
};

enum EM_TARGETCONST
{
	EMTARGET_NULL	= -1,
	EMTARGET_NET	= 24,
};

enum EMSKILLCHECK
{
	EMSKILL_OK			= 0,
	EMSKILL_NOTLEARN	= 1,
	EMSKILL_DELAYTIME	= 2,

	EMSKILL_NOTITEM		= 3,
	EMSKILL_NOTARROW	= 4,
	EMSKILL_NOTCHARM	= 5,

	EMSKILL_NOTHP		= 6,
	EMSKILL_NOTMP		= 7,
	EMSKILL_NOTSP		= 8,
	EMSKILL_NOTEXP		= 9,
	EMSKILL_NOTCP		= 10,

	EMSKILL_NOTREBIRTH  = 11,

	EMSKILL_NOTBULLET	= 12,
	EMSKILL_NOTBATTERY	= 13,
	EMSKILL_NOTDRIVER	= 14,

	EMSKILL_REBIRTHBLOCK = 15,	// 부활 스킬이 금지된 맵이다;

	EMSKILL_FAILED,
};

enum EMSKILLCHECKOPTION
{
	EMSKILLCHECKOPTION_EXCEPT_NOTLEARN  = 0x0001, // 배우지 않음 제외
	EMSKILLCHECKOPTION_DEFENSESKILL		= 0x0002, // 디펜스 스킬
	EMSKILLCHECKOPTION_SUBARMS		    = 0x0004, // 서브 무기도 체크
};

enum EMSKILLREACTION
{
	EMSKILLREACTION_FAILED = 0,
	EMSKILLREACTION_SUCESS = 1,
};

enum EMFACTEFFPROCESSING
{
    EMFACTEFFECTPROCESSING_DESTRUCTOR = 0,
    EMFACTEFFECTPROCESSING_DEAD       = 1,
    EMFACTEFFECTPROCESSING_REBIRTH    = 2,
    EMFACTEFFECTPROCESSING_MOVEMAP    = 3,
    EMFACTEFFECTPROCESSING_AUTH       = 4,
};

enum EMREACTIONABLE
{
    EMREACTIONABLE_FALSE        = 0,  // 반응 불가;
    EMREACTIONABLE_TRUE         = 1,  // 반응 가능;
    EMREACTIONABLE_FORCED_TRUE  = 2,  // 강제적 행위에 의해서 반응 가능;
};

struct STARID;
struct STARGETID
{
	EMCROW		emCrow;
	DWORD		GaeaId;
	D3DXVECTOR3	vPos;

//	MSGPACK_DEFINE(emCrow, GaeaId, vPos.x, vPos.y, vPos.z);

	STARGETID(void);
	STARGETID(const STARID& rhs);
	STARGETID(const EMCROW _emCrow, const DWORD _dwID);
	STARGETID(const DWORD _emCrow, const DWORD _dwID);
	STARGETID(const EMCROW _emCrow, const DWORD _dwID, const D3DXVECTOR3& _vPos);
	STARGETID(const DWORD _emCrow, const DWORD _dwID, const D3DXVECTOR3& _vPos);

	void RESET(void);
	const bool ISNULL() const;

	const STARGETID& operator = (const STARID& rhs);
	const bool operator < (const STARGETID& TargetID) const;	
	const bool operator == (const STARGETID& TargetID ) const;
	const bool operator != (const STARGETID&  TargetID ) const;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & emCrow;
		ar & GaeaId;
		ar & vPos.x;
		ar & vPos.y;
		ar & vPos.z;
	}
};

struct STARID
{
	DWORD		emCrow; // -> EMCROW emCrow;
	DWORD		dwID; // -> DWORD dwID;

	MSGPACK_DEFINE(emCrow, dwID);

	STARID(void);
	STARID(const EMCROW _emCrow, const DWORD _dwID);
	STARID(const DWORD _emCrow, const DWORD _dwID);

	void RESET(void);
	void SET(const STARGETID &sTARID);

	const EMCROW GETCROW(void) const;
	const DWORD GETID(void) const;

	const STARID& operator = ( const STARID& sTemp )
	{
		if( this == &sTemp ) return *this;

		emCrow = sTemp.emCrow;
		dwID   = sTemp.dwID;

		return *this;
	}
};


//! 타겟 선택용 참조카운터
struct STARGETIDREFCOUNTER : public STARGETID
{
	UINT nRefCount;

	STARGETIDREFCOUNTER()
		: nRefCount(1)
	{
	}

	STARGETIDREFCOUNTER( const STARGETID& sTargetID )
		: STARGETID(sTargetID)
		, nRefCount(1)
	{
	}

	bool operator < ( const STARGETIDREFCOUNTER &sTarget ) const
	{
		return STARGETID::operator<(sTarget);
	}

	bool operator == ( const STARGETID &sTarget ) const
	{
		return STARGETID::operator==(sTarget);
	}

	bool operator != ( const STARGETID &sTarget ) const
	{
		return STARGETID::operator!=(sTarget);
	}
};

typedef std::vector<STARGETID>	TARID_VEC;
typedef TARID_VEC::iterator		TARID_VEC_ITER;

typedef std::set<STARGETIDREFCOUNTER>	TARIDRC_SET;
typedef TARIDRC_SET::iterator			TARIDRC_SET_ITER;

#define TARGETID_NULL STARGETID()

struct SKILLTARGETDATA
{
	WORD			nTarget;
	STARID			TargetIDs[EMTARGET_NET];
	STARGETID		TargetID;

	SKILLTARGETDATA(void)
	{
		Reset();
	}

	void Reset()
	{
		nTarget = 0;
		TargetID.RESET();
		for ( int i=0; i<EMTARGET_NET; ++i )
			TargetIDs[i].RESET();
	}

	const SKILLTARGETDATA& operator = ( const SKILLTARGETDATA& sTemp )
	{
		if( this == &sTemp ) return *this;

		nTarget = sTemp.nTarget;

		for( int i = 0; i < EMTARGET_NET; ++i ) 
			TargetIDs[i] = sTemp.TargetIDs[i];

		TargetID = sTemp.TargetID;

		return *this;
	}
};

//	Note : 공격 거리 유형.
enum EMATT_RGTYPE
{
	EMATT_SHORT	= 0,
	EMATT_LONG	= 1,
	
	EMATT_SIZE	= 2,
};

enum EMDETECT_ENEMY
{
	EMD_PC	= 0x01,
	EMD_MOB	= 0x02,
	EMD_NPC	= 0x04,
};

enum DAMAGE_TYPE
{
	DAMAGE_TYPE_NONE				= 0x0000,
	DAMAGE_TYPE_SHOCK				= 0x0001,	// 쇼크
	DAMAGE_TYPE_CRITICAL			= 0x0002,	// 크리티컬
	DAMAGE_TYPE_CRUSHING_BLOW		= 0x0004,	// 강한타격
	DAMAGE_TYPE_PSY_REDUCE			= 0x0008,	// 물리 흡수
	DAMAGE_TYPE_MAGIC_REDUCE		= 0x0010,	// 마법 흡수
	DAMAGE_TYPE_PSY_REFLECTION		= 0x0020,	// 물리 반사
	DAMAGE_TYPE_MAGIC_REFLECTION	= 0x0040,	// 마법 반사
	DAMAGE_TYPE_CURSE				= 0x0080,	// 저주
	DAMAGE_TYPE_ILLUSION			= 0x0100,	// 환술
	DAMAGE_TYPE_DOT					= 0x0200,	// 지속 데미지;
	DAMAGE_TYPE_STIGMA				= 0x0400,	// 낙인 데미지;
    DAMAGE_TYPE_GATHER              = 0x0800,   // 흡혈로 인한 자기자신에게의 데미지 (흡혈값이 -일때 발생); (반드시 HEAL_TYPE::HEAL_TYPE_GATHER 와 같은 값이여야 한다);
	DAMAGE_TYPE_CA					= 0x1000,	// 반격 데미지
	DAMAGE_TYPE_IMMUNE				= 0x0000,	// 면역 // 면역에 대한 추가 처리 필요할 시 0x0100으로;
};

enum HEAL_TYPE
{
	HEAL_TYPE_NONE			= 0x0000,
    HEAL_TYPE_CURSE			= 0x0080,	// 저주에 의한 회복;
    HEAL_TYPE_DOT			= 0x0200,	// 지속효과에 의한 회복;
    HEAL_TYPE_GATHER        = 0x0800,   // 흡혈로 인한 자기자신의 회복; (반드시 DAMAGE_TYPE::DAMAGE_TYPE_GATHER 와 같은 값이여야 한다);
	HEAL_TYPE_IMMUNE		= 0x0000,	// 면역 // 면역에 대한 추가 처리 필요할 시 0x0001로;
};

enum EMCROWCOLORTYPE
{
	EMCROWCOLORTYPE_ENEMY	 = 0,
	EMCROWCOLORTYPE_NPC		 = 1,
	EMCROWCOLORTYPE_ALLIANCE = 2,

	EMCROWCOLORTYPE_SIZE,
	EMCROWCOLORTYPE_NULL = EMCROWCOLORTYPE_ENEMY,
};

namespace COMMENT
{	
	extern std::string CROW[CROW_NUM];
	extern std::string ATTACK_RGTYPE[EMATT_SIZE];
}

enum EMIP_BONUS_CLASS
{
	EMIP_BONUS_NONE,
	EMIP_BONUS_A,
	EMIP_BONUS_B,
	EMIP_BONUS_C,
	EMIP_BONUS_D,
};

#endif // GLDEFINE_H_
