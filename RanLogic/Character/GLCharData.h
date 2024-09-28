#ifndef GLCHARDATA_H_
#define GLCHARDATA_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>


#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>

//#include "../../InternalCommonLib/Interface/PlayerInterface.h"
#include "../../EngineLib/Common/ByteStream.h"
#include "../../EngineLib/DxMeshs/DxAniKeys.h"

#include "../s_NetGlobal.h"
#include "../Quest/GLQuestPlay.h"
#include "../Inventory/GLInventoryBank.h"
#include "../Inventory/GLInventoryStorage.h"
#include "../Item/GLItem.h"
#include "../Skill/GLSkill.h"
#include "../Vehicle/GLVEHICLE.h"
#include "../Post/GLPostData.h"
#include "../Network/NetLogicDefine.h"


struct SSKILLFACT;

enum ENUM_TITLE_INFO
{
    EM_TITLE_MAXLENGTH		= 24,
    EM_TITLEID_MAXLENGTH	= 36	//max = 32 (ACTIVITY_REWARDTITLE_65535_65535)
};

enum EMREACTION
{
	REACT_NULL = 0,
	REACT_TRACE,
	REACT_MOB,
	REACT_NPC,
	REACT_P2P,
	REACT_PVP,
	REACT_ITEM,
	REACT_SKILL,
	REACT_GATHERING,

	REACT_TOGGLE_VEHICLE,
	REACT_SWAP_ARMS,	  
	REACT_SUMMON_NPC,

	REACT_SIZE,
};

enum EMACTIONTAR
{
	EMACTAR_NULL	= 0,

	EMACTAR_PC_PVP	= 1,
	EMACTAR_PC_P2P	= 2,

	EMACTAR_MOB		= 3,
	EMACTAR_NPC		= 4,

	EMACTAR_PC_OUR	= 5,
	EMACTAR_PC_ANY	= 6,

	EMACTAR_ITEM	= 7,

	EMACTAR_SUMMON_ATTACK	= 8,
	EMACTAR_MATERIAL	= 9, 

	EMACTAR_SUMMON_NPC = 10, 
};

namespace COMMENT
{
    extern std::string szEMREACTION[REACT_SIZE+1];
};


enum EMCHARDATA
{
    EMSKILLQUICK_VERSION	= 0x0102,
    EMSKILLQUICK_SIZE_OLD	= 30,	
    EMSKILLQUICK_SIZE		= 40,	
	EMSKILLQUICK_ROW		= 4,
	EMSKILLQUICK_COL		= 10,
    EMACTIONQUICK_SIZE		= 6,
	EMSKILLQUICKTAB_SLOTSIZE = 10,	

    SKILLFACT_SIZE_NORMAL				= 14,
	SKILLFACT_SIZE_ADDITIONAL			= 1,
    SKILLFACT_SIZE_LUNCHBOX		 	= 1,
	SKILLFACT_SIZE_EXP_LUNCHBOX	= 1,
    SKILLFACT_SIZE_SYSTEM	 			= 5,
    SKILLFACT_SIZE			 			= SKILLFACT_SIZE_NORMAL + SKILLFACT_SIZE_LUNCHBOX + SKILLFACT_SIZE_EXP_LUNCHBOX + SKILLFACT_SIZE_SYSTEM + SKILLFACT_SIZE_ADDITIONAL,
	SKILLFACT_SIZE_NORMAL_INC_ADDITIONAL= SKILLFACT_SIZE_NORMAL + SKILLFACT_SIZE_ADDITIONAL + 1,	

    SKILLFACT_INDEX_NORMAL_START				= 0,
    SKILLFACT_INDEX_NORMAL_END					= SKILLFACT_SIZE_NORMAL,
	SKILLFACT_INDEX_ADDITIONAL_START		= SKILLFACT_INDEX_NORMAL_END,
	SKILLFACT_INDEX_ADDITIONAL_END			= SKILLFACT_INDEX_NORMAL_END + SKILLFACT_SIZE_ADDITIONAL,
    SKILLFACT_INDEX_LUNCHBOX_START 			= SKILLFACT_INDEX_ADDITIONAL_END,	
    SKILLFACT_INDEX_LUNCHBOX_END   			= SKILLFACT_INDEX_ADDITIONAL_END + SKILLFACT_SIZE_LUNCHBOX,
	SKILLFACT_INDEX_EXP_LUNCHBOX_START	= SKILLFACT_INDEX_LUNCHBOX_END,
	SKILLFACT_INDEX_EXP_LUNCHBOX_END		= SKILLFACT_INDEX_LUNCHBOX_END + SKILLFACT_SIZE_EXP_LUNCHBOX,
    SKILLFACT_INDEX_SYSTEM_START   			= SKILLFACT_INDEX_EXP_LUNCHBOX_END,
    SKILLFACT_INDEX_SYSTEM_END     			= SKILLFACT_INDEX_EXP_LUNCHBOX_END + SKILLFACT_SIZE_SYSTEM,

    SKILLFACT_INDEX_CTF_NORMAL_REWARD0  = SKILLFACT_INDEX_SYSTEM_START + 0,
    SKILLFACT_INDEX_CTF_NORMAL_REWARD1  = SKILLFACT_INDEX_SYSTEM_START + 1,
    SKILLFACT_INDEX_CTF_NORMAL_REWARD2  = SKILLFACT_INDEX_SYSTEM_START + 2,

    CROW_SKILLFACT_SIZE            = SKILLFACT_SIZE_NORMAL,
    CROW_SKILLFACT_INDEX_START     = 0,
    CROW_SKILLFACT_INDEX_END       = CROW_SKILLFACT_SIZE,	

    EMMAX_CLUB_NUM			= 100,
    EMMAX_SUMMON_NUM		= 10,

    EMSTORAGE_CHANNEL				= 5,

    EMSTORAGE_CHANNEL_DEF			= 0,
    EMSTORAGE_CHANNEL_DEF_SIZE		= 1,

    EMSTORAGE_CHANNEL_SPAN			= 1,
    EMSTORAGE_CHANNEL_SPAN_SIZE		= 3,

    EMSTORAGE_CHANNEL_PREMIUM		= 4,
    EMSTORAGE_CHANNEL_PREMIUM_SIZE	= 1,

	EMCHAR_SLOT_DATA_SIZE			= 5,
};

#define SKILL_ADDITIONAL_BATTLE			1

enum EMSKILLFACT_TYPE
{
    EMSKILLFACT_TYPE_NONE				= -1,
    EMSKILLFACT_TYPE_NORMAL				= 0,
	EMSKILLFACT_TYPE_ADDITIONAL		    = 1,
    EMSKILLFACT_TYPE_LUNCHBOX			= 2,
    EMSKILLFACT_TYPE_SYSTEM				= 3,
	EMSKILLFACT_TYPE_EXP_LUNCHBOX	    = 4,
	EMSKILLFACT_TYPE_FORCED_NORMAL	    = 5,
};

#define EMSKILLFACTTYPE_BY_INDEX(i)	(										\
    (i < SKILLFACT_INDEX_NORMAL_END ) ? EMSKILLFACT_TYPE_NORMAL				\
	: (i < SKILLFACT_INDEX_ADDITIONAL_END) ? EMSKILLFACT_TYPE_ADDITIONAL	\
    : (i < SKILLFACT_INDEX_LUNCHBOX_END) ? EMSKILLFACT_TYPE_LUNCHBOX		\
	: (i < SKILLFACT_INDEX_EXP_LUNCHBOX_END) ? EMSKILLFACT_TYPE_EXP_LUNCHBOX \
    : (i < SKILLFACT_INDEX_SYSTEM_END) ? EMSKILLFACT_TYPE_SYSTEM			\
    : EMSKILLFACT_TYPE_NONE													\
    )															

enum EMGM_EVENT_TYPE
{
    EMGM_EVENT_NONE		= 0x00,	
    EMGM_EVENT_SPEED	= 0x01,	
    EMGM_EVENT_ASPEED	= 0x02,
    EMGM_EVENT_ATTACK	= 0x04,	
};

enum EMVEHICLE_WHETHER
{
    EMVEHICLE_OFF				= 0,
    EMVEHICLE_DRIVER			= 1,
    EMVEHICLE_PASSENGER			= 2,
};


enum EMACTION_LIMIT
{
    EMACTION_LIMIT_NONE	   = 0x0000,
    EMACTION_LIMIT_ITEM	   = 0x0001,	
    EMACTION_LIMIT_DRUG	   = 0x0002,	
    EMACTION_LIMIT_SKILL   = 0x0004,	
    EMACTION_LIMIT_PET	   = 0x0008,	
    EMACTION_LIMIT_DRUG_HP = 0x0010,	
    EMACTION_LIMIT_CARD    = 0x0020,
};

enum EMACTION_SLOT
{
    EMACT_SLOT_NONE	= 0,
    EMACT_SLOT_DRUG	= 1,
};

#define PVP_POINT_RATIO_HEAL_DAMAGE		10		//1 Heal = 10 Damage
#define PVP_POINT_RATIO_REBIRTH_KILL	2		//1 Rebirth = 2 Kill

//
//mjeon.CaptureTheField
//

//
struct PVP_PERFORMANCE
{
	UINT		nKill;			//(4 bytes)
	UINT		nDeath;			//(4 bytes)
	UINT		nRebirthTo;		//(4 bytes)	
	UINT		nDamageTo;		//(4 bytes)
	UINT		nHealTo;		//(4 bytes)
	UINT		nContributionPoint;	//(4 bytes)

public:
	UINT64	nnDamageFrom;
	UINT64	nnHealFrom;
	//
	//UINT		nPadding;		//(4 bytes)		
	//UINT64	nnDamageTo;		//(8 bytes)
	//UINT64	nnHealTo;		//(8 bytes)	
	
	/*
		-- reserved : not used yet --


	*/

    MSGPACK_DEFINE(nKill, nDeath, nRebirthTo, nDamageTo, nHealTo, nContributionPoint);

	PVP_PERFORMANCE()
		:nKill(0)
		,nDeath(0)
		,nRebirthTo(0)
		,nDamageTo(0)
		,nHealTo(0)
		,nContributionPoint(0)
	{
	}

	void Reset()
	{
		nKill		= 0;
		nDeath		= 0;
		nRebirthTo	= 0;
		nDamageTo	= 0;
		nHealTo		= 0;
		nContributionPoint = 0;
	}

	void Kill()
	{
		nKill++;
		
		GASSERT(nKill < UINT_MAX);
	}

	void Death()
	{
		nDeath++;

		GASSERT(nDeath < UINT_MAX);
	}

	void RebirthTo()
	{
		nRebirthTo++;

		GASSERT(nRebirthTo < UINT_MAX);
	}

	void DamageTo(int nDamage)
	{
		if (nDamage < 0)
			nDamageTo -= nDamage;
		else
			nDamageTo += nDamage;

		GASSERT(nDamageTo < _UI64_MAX);
	}

	void HealTo(int nHeal)
	{
		if (nHeal < 0)
			nHealTo -= nHeal;
		else
			nHealTo += nHeal;

		GASSERT(nHealTo < _UI64_MAX);
	}
	
	UINT CalculateContributionPoint(int nExtraPoint /* based on Win/Lose */, int nMax /* cannot exceed the maximum point */)
	{
        nContributionPoint = min((UINT)nMax, nExtraPoint + (nDamageTo/100000 + nHealTo/10000 + nKill/2 + nRebirthTo));

		return nContributionPoint;
	}

	bool operator< (const PVP_PERFORMANCE& rhs) const
	{
		//
		// 1.Contribution Point
		//
		if (nContributionPoint != rhs.nContributionPoint)
			return (nContributionPoint > rhs.nContributionPoint);
		
		//
		// 2.Damage / Heal
		//
		UINT lDH = (nDamageTo + nHealTo * 10);
		UINT rDH = (rhs.nDamageTo + rhs.nHealTo * 10);

		if (lDH != rDH)
			return (lDH > rDH);

		//
		// 3.Kill / Death / Rebirth
		//
		int lKDR = (nKill - nDeath + PVP_POINT_RATIO_REBIRTH_KILL * nRebirthTo);
		int rKDR = (rhs.nKill - rhs.nDeath + PVP_POINT_RATIO_REBIRTH_KILL * rhs.nRebirthTo);

		if (lKDR != rKDR)
			return (lKDR > rKDR);

		if (nHealTo != rhs.nHealTo)
			return (nHealTo > rhs.nHealTo);

		if (nRebirthTo != rhs.nRebirthTo)
			return (nRebirthTo > rhs.nRebirthTo);

		if (nDeath != rhs.nDeath)
			return (nDeath < rhs.nDeath);	//Death

		//GASSERT(!"PVP_PERFORMANCE: (WARNING) There's no more counter to compare!!");
		return false;
	}


	void DamageFrom(WORD wDamage)
	{
		nnDamageFrom += wDamage;

		GASSERT(nnDamageFrom < _UI64_MAX);
	}

	void HealFrom(WORD wHeal)
	{
		nnHealFrom += wHeal;

		GASSERT(nnHealFrom < _UI64_MAX);
	}


private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & nKill;
		ar & nDeath;
		ar & nRebirthTo;
		ar & nDamageTo;
		ar & nHealTo;
		ar & nContributionPoint;
	}
};


//
//mjeon.CaptureTheField
//
struct CTF_STATISTICS_4_ONEPLAYER
{

	PVP_PERFORMANCE	pvpperf;				//(24 bytes)

	DWORD			wClass;					//(4 bytes)
	WORD			wSchool;				//(2 bytes)
	//UINT			nContributionPoint;		//(4 bytes)

	char			szName[CHAR_SZNAME];	//(36 bytes)	3 bytes padding 
											//Note: __int64

											//Total: 64 bytes

	CTF_STATISTICS_4_ONEPLAYER()
		:wClass(0)
		,wSchool(0)		
	{
		ZeroMemory(&pvpperf, sizeof(pvpperf));
		ZeroMemory(szName, sizeof(szName));
	}

	CTF_STATISTICS_4_ONEPLAYER(DWORD _wClass, WORD _wSchool, char *_szName, PVP_PERFORMANCE &_pvpperf)
		:wClass(_wClass)
		,wSchool(_wSchool)		
		,pvpperf(_pvpperf)
	{
		GASSERT(_szName);

		StringCchCopy(szName, CHAR_SZNAME, _szName);
	}
};



const int MAX_CP = 8000;

struct SDURATION_DAMAGE
{
    float m_fTickTime;	//	
    float m_fDamage;	//	
    float m_fTime;		//	

    STARGETID m_sID;	//
	SNATIVEID m_sSkillID;	//ID

    //MSGPACK_DEFINE(m_fTickTime, m_fDamage, m_fTime, m_sID);

    SDURATION_DAMAGE()
        : m_fTickTime ( 0.0f )
        , m_fDamage ( 0.0f )
        , m_fTime ( 0.0f )
    {
    }

    bool VAILD() 
    {
        if ( m_fTickTime > 0.0f && m_fDamage != 0.0f )
            return true;

        return false;
    }

    bool IsEqual( const SDURATION_DAMAGE& sDurDamage )
    {
        if ( m_fTickTime == sDurDamage.m_fTickTime &&
            m_fDamage == sDurDamage.m_fDamage )
            return true;

        return false;
    }

    void RESET ()
    {
        m_fTickTime = 0.0f;
        m_fDamage = 0.0f;
        m_sID.RESET();
		m_sSkillID.Reset();
        //		m_fTime = 0.0f;	// 
    }
};

struct SACTION_SLOT
{
    enum { VERSION = 0x0100, };

    WORD		wACT;
    SNATIVEID	sNID;

    MSGPACK_DEFINE(wACT, sNID);

    SACTION_SLOT()
        : wACT(EMACT_SLOT_NONE)
        , sNID(false)
    {
    }

    bool VALID () const
    {
        return wACT!=EMACT_SLOT_NONE && sNID!=SNATIVEID(false);
    }

    void RESET ()
    {
        wACT = EMACT_SLOT_NONE;
        sNID = SNATIVEID(false);
    }

    inline SNATIVEID GetNativeID() const { return sNID; }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & wACT;
		ar & sNID;
	}
};

struct DAMAGE_SPEC
{
	enum 
	{
		FLAG_IGNORED_DAMAGE		= 0x00000001,
		FLAG_ILLUSION					= 0X00000002,
	}; //enum

    float		m_fPsyDamageReduce;					
    float		m_fPsyDamageReduceEFFRate;			
	float		m_fPsyDamageReflection;					
	float		m_fPsyDamageReflectionEffRate;			
	float		m_fPsyDamageReflectionRate;			
	float		m_fPsyDamageReflectionRateEffRate;	

    float		m_fMagicDamageReduce;						
    float		m_fMagicDamageReduceEffRate;				   
    float		m_fMagicDamageReflection;					
    float		m_fMagicDamageReflectionEffRate;			
    float		m_fMagicDamageReflectionRate;				
    float		m_fMagicDamageReflectionRateEffRate;	
    float		m_fDamageCurse;				
    float		m_fDamageCurseEffRate;	
	float		m_fIllusionJumpBound;
	float		m_fIllusionLastTime;
	WORD		m_wIllusionRemain;
	STARGETID	m_sCurseTargetID;		
    DWORD		m_dwFlags;					
	DWORD		m_emImmuneApplyType;	
	DWORD		m_emImmuneActionType;	

    DAMAGE_SPEC()				 { AllReset(); }	

    void RESET()
    {
		m_fPsyDamageReduce					= 0.0f;
        m_fPsyDamageReduceEFFRate			= 1.0f;
        m_fMagicDamageReduce				= 0.0f;
        m_fMagicDamageReduceEffRate			= 1.0f;
        m_fPsyDamageReflection				= 0.0f;
        m_fPsyDamageReflectionEffRate		= 1.0f;
        m_fPsyDamageReflectionRate			= 0.0f;
        m_fPsyDamageReflectionRateEffRate	= 1.0f;
        m_fMagicDamageReflection			= 0.0f;
        m_fMagicDamageReflectionEffRate		= 1.0f;
        m_fMagicDamageReflectionRate		= 0.0f;
        m_fMagicDamageReflectionRateEffRate	= 1.0f;
        m_fDamageCurse						= 0.0f;
        m_fDamageCurseEffRate				= 1.0f;
        m_dwFlags							= 0;
		m_emImmuneApplyType					= 0;
		m_emImmuneActionType				= 0;
    }

	/**
	 * reset all of data include saved data.
	 */
	void AllReset()
	{
		RESET();
		m_wIllusionRemain		= 0;
		m_fIllusionJumpBound	= 0.0f;
		m_fIllusionLastTime		= 0.0f;
	} //AllReset

	DAMAGE_SPEC& operator = ( const DAMAGE_SPEC& rh )
	{
		m_fPsyDamageReduce					= rh.m_fPsyDamageReduce;
        m_fPsyDamageReduceEFFRate			= rh.m_fPsyDamageReduceEFFRate;
        m_fMagicDamageReduce				= rh.m_fMagicDamageReduce;
        m_fMagicDamageReduceEffRate			= rh.m_fMagicDamageReduceEffRate;
        m_fPsyDamageReflection				= rh.m_fPsyDamageReflection;
        m_fPsyDamageReflectionEffRate		= rh.m_fPsyDamageReflectionEffRate;
        m_fPsyDamageReflectionRate			= rh.m_fPsyDamageReflectionRate;
        m_fPsyDamageReflectionRateEffRate	= rh.m_fPsyDamageReflectionRateEffRate;
        m_fMagicDamageReflection			= rh.m_fMagicDamageReflection;
        m_fMagicDamageReflectionEffRate		= rh.m_fMagicDamageReflectionEffRate;
        m_fMagicDamageReflectionRate		= rh.m_fMagicDamageReflectionRate;
        m_fMagicDamageReflectionRateEffRate	= rh.m_fMagicDamageReflectionRateEffRate;
        m_fDamageCurse						= rh.m_fDamageCurse;
        m_fDamageCurseEffRate				= rh.m_fDamageCurseEffRate;
        m_dwFlags							= rh.m_dwFlags;							
		m_emImmuneApplyType					= rh.m_emImmuneApplyType;
		m_emImmuneActionType				= rh.m_emImmuneActionType;

		return *this;
	} 
};

struct SVIEWRANGERATE
{
	void RESET(void)
	{
		fRate = 1.0f;
	}
	void LIMIT(void)
	{
		const float _fRate(fRate);
		if ( _fRate < 0.0f )
			fRate = 0.0f;
		else if ( _fRate > 1.0f )
			fRate = 1.0f;
	}
	const float operator *(const float _fValue) const
	{
		return _fValue * fRate;
	}
	const float operator *(const int _nValue) const
	{
		return float(_nValue) * fRate;
	}
	const SVIEWRANGERATE& operator =(const float _fRate)
	{
		fRate = _fRate;
		LIMIT();
		return *this;
	}
	SVIEWRANGERATE(void)
		: fRate(1.0f)
	{
	}
	float fRate;
};

struct SIGNORED_DAMAGE
{
	const bool isAvailable(void) const
	{
		return (this->nIgnoreCount > 0) || (this->nIgnoreDamage > 0);
	}

	int nIgnoreCount; 
	int nIgnoreDamage; 

	SIGNORED_DAMAGE(void)
		: nIgnoreCount(0)
		, nIgnoreDamage(0)
	{
	}
	void RESET(void)
	{
		nIgnoreCount = 0;
		nIgnoreDamage = 0;
	}
	const bool AbsorbDamage(int& nDamage)
	{
		if ( nDamage <= 0 )
			return isAvailable();		

		if ( this->nIgnoreDamage > 0 )
		{
			const int _nLeftDamage(this->nIgnoreDamage - nDamage);
			if ( _nLeftDamage > 0 )
			{
				this->nIgnoreDamage = _nLeftDamage;
				nDamage = 0;
			}
			else
			{
				this->nIgnoreDamage = 0;
				nDamage = -_nLeftDamage;
			}
		}

		if ( this->nIgnoreCount > 0 )
		{
			nDamage = 0;
			--this->nIgnoreCount;
		}

		return isAvailable();
	}
};

struct SINVISIBLE
{
	DWORD dwLevelINVISIBLE;		
	BOOL bPrevStateRun;	

	SINVISIBLE(void)
	{
	}
	const bool SET(const SKILL::SSPEC& sSPEC) //
	{
		const DWORD dwLevel = DWORD(sSPEC.fVAR1);
		this->dwLevelINVISIBLE = dwLevel;

		return (dwLevel ? true : false);
	}
	void RESET(void)
	{		
	}
};

struct SRECVISIBLE
{
	DWORD dwLevelRECVISIBLE;
	float fRadiusRECVISIBLE;

	BOOL bFLAG; 
	float fLatestCheckTime; 

	SRECVISIBLE(void) : 	
	bFLAG(FALSE), fLatestCheckTime(0.0f), dwLevelRECVISIBLE(0), fRadiusRECVISIBLE(0.0f)
	{
	}	
	void SET(const SKILL::SSPEC& sSPEC)
	{
		dwLevelRECVISIBLE = DWORD(sSPEC.fVAR1);
		fRadiusRECVISIBLE= sSPEC.fVAR2;
	}
	void RESET(void)
	{
		dwLevelRECVISIBLE = 0;
		fRadiusRECVISIBLE = 0.0f;
	}
};

struct SSTIGMA
{	
	struct SSTIGMA_PACK
	{		
		STARGETID sUserID;		
		STARGETID sTakerID;	
		SNATIVEID sSkillID;		
		DWORD dwFLAG;
		float fRate;
		DWORD dwAsignDistance;		
		bool bUse;
		SSTIGMA_PACK(void) : bUse(false){}
	};	

	DWORD dwStigmaIndex;

	SSTIGMA_PACK sSTIGMA_PACK[SKILLFACT_SIZE];
	SSTIGMA(void) : dwStigmaIndex(0) 
	{
		for ( unsigned int _i = SKILLFACT_SIZE; _i; --_i )
			this->sSTIGMA_PACK[_i - 1].bUse = false;
	}
	void RESET(void)
	{
		*this = SSTIGMA();
	}
	const DWORD PushStigmaPack(const STARGETID& sUserID, const STARGETID& sTakerID, const SNATIVEID sSkillID, const SKILL::SSPEC& sSPEC);	
	const DWORD IsExistStigmaPack(const STARGETID& sUserID, const STARGETID& sTakerID, const SNATIVEID sSkillID);	
	const DWORD IsExistStigmaPack(const STARGETID& sUserID, const SNATIVEID sSkillID);	
	const SSTIGMA_PACK* GetStigmaPack(const DWORD dwIndex);
	const SSTIGMA_PACK* GetStigmaPack(void);
	void DeleteStigmaPack(const DWORD dwIndex);
};

struct SRELEASE_ENDURE
{	
	DWORD dwActivePercentage[SKILLFACT_SIZE];
	DWORD dwEffectFlag[SKILLFACT_SIZE];	
	DWORD dwSkillFactIndex[SKILLFACT_SIZE];
	DWORD nRELEASE_ENDURE;

	SRELEASE_ENDURE(void) : 
	nRELEASE_ENDURE(0)
	{
	}
	void SET(const SKILL::SSPEC& sSPEC, const DWORD dwCount)
	{
		this->dwActivePercentage[this->nRELEASE_ENDURE] = DWORD(sSPEC.fVAR1 * 100.0f);		
		this->dwEffectFlag[this->nRELEASE_ENDURE] = sSPEC.dwFLAG;
		this->dwSkillFactIndex[this->nRELEASE_ENDURE] = dwCount;
		++this->nRELEASE_ENDURE;
	}
	void RESET(void)
	{
		*this = SRELEASE_ENDURE();
	}
};

struct SDOMINATE	
{	
	STARGETID sControlTargetID;	
	EMSPEC_DOMINATE_TYPE	emController;		
	BOOL		bDominate;		
	DWORD	dwTimer;

	float	fRadiusMoveable;		
	bool	bDomination;

	SDOMINATE(void) :
	emController(EMSPEC_DOMINATE_TYPE_SELF),
		bDominate(false),
		dwTimer(0),
		bDomination(false)
	{
	}
	void SET(const SKILL::SSPEC& sSPEC)
	{
		fRadiusMoveable = sSPEC.fVAR2;
		emController = EMSPEC_DOMINATE_TYPE(sSPEC.dwFLAG);		
		bDomination = true;
	}
	void RESET(void)
	{		
		emController = EMSPEC_DOMINATE_TYPE_SELF;
		bDomination = false;
	}
};

struct SDOMINATOR
{
	STARGETID sControlTargetID;	
	BOOL bDominate;		
	DWORD dwTimer;		

	SDOMINATOR(void) :	
	bDominate(false),
		dwTimer(0)
	{
	}
	
	void RESET(void){
		*this = SDOMINATOR();
	}
};

struct STAUNT  
{
    STARGETID sTauntTargetID;  
    float fTauntDistance;  
    
    STAUNT() :
        fTauntDistance(0)
    {
        sTauntTargetID = TARGETID_NULL;
    }
    
    void RESET()
    {
        *this = STAUNT();
    }
};

struct SHALLUCINATE	
{	
	EMANI_SUBTYPE	emAni_SubType;	
	DWORD dwTransformID;
	
	bool bTransform; 
	bool bEmoticon;
	bool bNonTargetable; 	

	SHALLUCINATE(void) :
	bTransform(false),
		bEmoticon(false),
		bNonTargetable(false)
	{
	}
	void SET(const SKILL::SSPEC& sSPEC)
	{
		this->bEmoticon = (sSPEC.dwFLAG & EMSPEC_HALLUCINATE_DWORD_EMOTICON) ? true : false;
		this->bTransform = (sSPEC.dwFLAG & EMSPEC_HALLUCINATE_DWORD_TRANSFORM) ? true : false;
		this->bNonTargetable = (sSPEC.dwFLAG & EMSPEC_HALLUCINATE_DWORD_NONTARGETABLE) ? true : false;		

		this->dwTransformID = DWORD(sSPEC.fVAR1);
		this->emAni_SubType = EMANI_SUBTYPE(DWORD(sSPEC.fVAR2));
	}
	void RESET(void)
	{
		*this = SHALLUCINATE();
	}
};

struct SINCREASEEFF 
{
	DWORD dwApplyFlag; 
	int nIncreaseCrushingBlowRate[SKILL::EMAPPLY_NSIZE]; // VAR1;
	int nIncreaseDamageRate[SKILL::EMAPPLY_NSIZE]; // VAR2;

	SINCREASEEFF() : dwApplyFlag(SKILL::EMAPPLY_DWORD_NULL)
	{
		for ( unsigned int _i = SKILL::EMAPPLY_NSIZE; _i; --_i )
		{
			this->nIncreaseCrushingBlowRate[_i - 1] = 0;
			this->nIncreaseDamageRate[_i - 1] = 0;
		}
	}

	const int GetIncreaseCrushingBlowRate(const DWORD dwApplyType)
	{
		switch ( dwApplyType )
		{
		case SKILL::EMAPPLY_DWORD_MELEE:
			return nIncreaseCrushingBlowRate[SKILL::EMAPPLY_MELEE];
			break;
		case SKILL::EMAPPLY_DWORD_RANGE:
			return nIncreaseCrushingBlowRate[SKILL::EMAPPLY_RANGE];
			break;
		case SKILL::EMAPPLY_DWORD_MAGIC:
			return nIncreaseCrushingBlowRate[SKILL::EMAPPLY_MAGIC];
			break;
		}
		return 0;
	}
	const int GetIncreaseDamageRate(const DWORD dwApplyType)
	{
		switch ( dwApplyType )
		{
		case SKILL::EMAPPLY_DWORD_MELEE:
			return nIncreaseDamageRate[SKILL::EMAPPLY_MELEE];
			break;
		case SKILL::EMAPPLY_DWORD_RANGE:
			return nIncreaseDamageRate[SKILL::EMAPPLY_RANGE];
			break;
		case SKILL::EMAPPLY_DWORD_MAGIC:
			return nIncreaseDamageRate[SKILL::EMAPPLY_MAGIC];
			break;
		}
		return 0;
	}
	void SET(const SKILL::SSPEC& sSPEC)
	{
		this->dwApplyFlag |=  sSPEC.dwFLAG;

		if ( sSPEC.dwFLAG & SKILL::EMAPPLY_DWORD_MELEE )
		{
			const int _nIncreseCrushingBlowRate = int(sSPEC.fVAR1 * 100.0f);
			const int _nIncreaseRate = int(sSPEC.fVAR2 * 100.0f);

			if ( _nIncreseCrushingBlowRate > this->nIncreaseCrushingBlowRate[SKILL::EMAPPLY_MELEE] )
				this->nIncreaseCrushingBlowRate[SKILL::EMAPPLY_MELEE] = _nIncreseCrushingBlowRate;
			if ( _nIncreaseRate > this->nIncreaseDamageRate[SKILL::EMAPPLY_MELEE] )
				this->nIncreaseDamageRate[SKILL::EMAPPLY_MELEE] = _nIncreaseRate;
		}

		if ( sSPEC.dwFLAG & SKILL::EMAPPLY_DWORD_RANGE )
		{
			const int _nIncreseCrushingBlowRate = int(sSPEC.fVAR1 * 100.0f);
			const int _nIncreaseRate = int(sSPEC.fVAR2 * 100.0f);

			if ( _nIncreseCrushingBlowRate > this->nIncreaseCrushingBlowRate[SKILL::EMAPPLY_RANGE] )
				this->nIncreaseCrushingBlowRate[SKILL::EMAPPLY_RANGE] = _nIncreseCrushingBlowRate;
			if ( _nIncreaseRate > this->nIncreaseDamageRate[SKILL::EMAPPLY_RANGE] )
				this->nIncreaseDamageRate[SKILL::EMAPPLY_RANGE] = _nIncreaseRate;
		}

		if ( sSPEC.dwFLAG & SKILL::EMAPPLY_DWORD_MAGIC )
		{
			const int _nIncreseCrushingBlowRate = int(sSPEC.fVAR1 * 100.0f);
			const int _nIncreaseRate = int(sSPEC.fVAR2 * 100.0f);

			if ( _nIncreseCrushingBlowRate > this->nIncreaseCrushingBlowRate[SKILL::EMAPPLY_MAGIC] )
				this->nIncreaseCrushingBlowRate[SKILL::EMAPPLY_MAGIC] = _nIncreseCrushingBlowRate;
			if ( _nIncreaseRate > this->nIncreaseDamageRate[SKILL::EMAPPLY_MAGIC] )
				this->nIncreaseDamageRate[SKILL::EMAPPLY_MAGIC] = _nIncreaseRate;
		}
	}
	void RESET(void)
	{
		*this = SINCREASEEFF();
	}
};

struct SLINKHP
{
	float fRef;
	float fTake;

	SLINKHP(void) :
	fRef(0.0f) ,fTake(1.0f)
	{
	}

	void SET(const float fVAR1, const float fVAR2)
	{
		if ( static_cast<int>(fVAR2) == TRUE )
		{
			if ( fVAR1 < this->fTake)
				this->fTake = fVAR1;
		}
	}
	void RESET(void)
	{
		*this = SLINKHP();
	}
};

struct SEXPERIENCERATE
{
	struct ExperienceElement
	{
		ExperienceElement(const float _fLifeTime, const float _fIncreaseRate);
		~ExperienceElement(void);

		float fLifeTime;
		float fIncreaseRate;
	};
	typedef std::vector<ExperienceElement> LifeTimeVector;
	typedef LifeTimeVector::iterator LifeTimeVectorIter;
	
	const SNATIVEID& getSkillID(void) const { return skillID; }
	const float getTotalRate(void) const { return fTotalIncreaseRate; }
	const float getRateLatest(void) const;
	const float getRate(const unsigned int _index) const;
	const float getTime(const unsigned int _index) const;
	const unsigned int size(void) const { return lifeTimeVector.size(); }
	const unsigned int getMaxSize(void) const { return sizeMax; }	
	const bool push_back(const SNATIVEID& _skillID, const DWORD _sizeMax, const float _fLifeTime, const float _fIncreaseRate);
	const float updateTime(const float fElapsedTime);
	void setLock(const bool _bLock) { bLock = _bLock; }	
	void clear(void);

private:
	void _refresh(void);

public:
	SEXPERIENCERATE(void);	

private:
	unsigned int sizeMax;
	float fTotalIncreaseRate;
	bool bLock;
	SNATIVEID skillID;
	LifeTimeVector lifeTimeVector;
};

struct SAIRBORNE
{
public:
	const float getHeight(void) const
	{
		return this->fHeightCurrent;
	}
	void push_back(const float _fTimeLife, const float _fTimeLevitate, const float _fHeightMax)
	{
		this->listElement.push_back(Element(_fTimeLife, _fTimeLevitate, _fHeightMax));
	}
	void update(const float _fTimeElapsed);

public:
	SAIRBORNE(void);
	
private:
	struct Element {
		const float update(const float _fTimeElapsed); //test
		Element(const float _fTimeLife, const float _fTimeLevitate, const float _fHeightMax);
		Element(const Element& _rhs);

		float fTimeElapsed;
		const float fUTimeLife;			
		const float fLevitateT;			
		const float fSlopeRiseFall;		
		const float fSlopeLevitate;		
		const float fInterceptY;		
		const float fHeightMax;			
	};

	typedef std::list<Element> ElementList;
	typedef ElementList::const_iterator ElementListConstIterator;
	typedef ElementList::iterator ElementListIterator;

	float fHeightCurrent;
	ElementList listElement;	
};

struct SCLONE
{
	int nCloneNum;

public:
	void SetClonNum( int nNUM )
	{
		if ( nCloneNum < nNUM)
			nCloneNum = nNUM;
	}

	int GetClonNum()
	{
		return nCloneNum;
	}

	void Reset()
	{
		nCloneNum = 0;
	}

	SCLONE():
	nCloneNum(0)
	{
	}
};

struct SVEHICLE
{
	EMVEHICLE_WHETHER	emVehicle;		
	float	fVehicleSpeedRate;
	float	fVehicleSpeedVol;

	SVEHICLE(void) :
	emVehicle(EMVEHICLE_OFF), fVehicleSpeedRate(0.0f), fVehicleSpeedVol(0.0f)
	{
	}
	void RESET(void)
	{
		*this = SVEHICLE();
	}
};

enum EMACTION_RATE{
	EMACTION_RATE_HP_RECOVER,	//	HP
	EMACTION_RATE_MP_RECOVER,	// MP
	EMACTION_RATE_SP_RECOVER,	// SP

	EMACTION_RATE_HP_INCREASE, // HP // m_fIncHP
	EMACTION_RATE_MP_INCREASE, // MP  // m_fIncMP
	EMACTION_RATE_SP_INCREASE, // SP // m_fIncSP
	EMACTION_RATE_CP_INCREASE, // CP // m_fIncHP

	EMACTION_RATE_DAMAGE,		
	EMACTION_RATE_DEFENSE,		

	EMACTION_RATE_SKILL_COOLDOWN,	

	EMACTION_RATE_MOVE_STATE,			
	EMACTION_RATE_MOVE_VELOCITY,		

	EMACTION_RATE_ATTACK_VELOCITY,	
	EMACTION_RATE_ATTACK_RANGE,		
	EMACTION_RATE_APPLY_RANGE,			

	EMACTION_RATE_EXP_GAIN,		
	EMACTION_RATE_ITEM_DROP,	
	EMACTION_RATE_MONEY_DROP,	

	EMACTION_RATE_SUMMON_TIME,	
	EMACTION_RATE_NSIZE,
};
struct SACTIONRATE
{
	float	m_fActionRate[EMACTION_RATE_NSIZE];

	void AddRate(const EMACTION_RATE emType, const float fAddValue)
	{
		this->m_fActionRate[emType] += fAddValue;
	}
	void SetRate(const EMACTION_RATE emType, const float fValue)
	{
		this->m_fActionRate[emType] = fValue;
	}

	SACTIONRATE(void)
	{
		this->m_fActionRate[EMACTION_RATE_HP_RECOVER]			= 0.0f;
		this->m_fActionRate[EMACTION_RATE_MP_RECOVER]			= 0.0f;
		this->m_fActionRate[EMACTION_RATE_SP_RECOVER]			= 0.0f;

		this->m_fActionRate[EMACTION_RATE_HP_INCREASE]			= 0.0f;
		this->m_fActionRate[EMACTION_RATE_MP_INCREASE]		= 0.0f;
		this->m_fActionRate[EMACTION_RATE_SP_INCREASE]			= 0.0f;
		this->m_fActionRate[EMACTION_RATE_CP_INCREASE]			= 0.0f;		

		this->m_fActionRate[EMACTION_RATE_DAMAGE]				= 1.0f;
		this->m_fActionRate[EMACTION_RATE_DEFENSE]				= 1.0f;
		
		this->m_fActionRate[EMACTION_RATE_SKILL_COOLDOWN]	= 1.0f;

		this->m_fActionRate[EMACTION_RATE_MOVE_STATE]			= 1.0f;
		this->m_fActionRate[EMACTION_RATE_MOVE_VELOCITY]	= 0.0f;

		this->m_fActionRate[EMACTION_RATE_ATTACK_VELOCITY]	= 0.0f;
		this->m_fActionRate[EMACTION_RATE_ATTACK_RANGE]		= 0.0f;
		this->m_fActionRate[EMACTION_RATE_APPLY_RANGE]		= 0.0f;

		this->m_fActionRate[EMACTION_RATE_EXP_GAIN]				= 1.0f;
		this->m_fActionRate[EMACTION_RATE_ITEM_DROP]			= 1.0f;
		this->m_fActionRate[EMACTION_RATE_MONEY_DROP]			= 1.0f;
		this->m_fActionRate[EMACTION_RATE_SUMMON_TIME]		= 0.0f;
	}

	void RESET(void)
	{
		*this = SACTIONRATE();
	}
};

/*

}
*/
enum EMACTION_VARIATION{
	EMACTION_VARIATION_POWER,			
	EMACTION_VARIATION_STRENGTH,		
	EMACTION_VARIATION_SPIRIT,			
	EMACTION_VARIATION_DEXTERITY,		
	EMACTION_VARIATION_INTELLIGENCE,	
	EMACTION_VARIATION_STAMINA,		

	
	EMACTION_VARIATION_FIRE_RESIST,			
	EMACTION_VARIATION_ICE_RESIST,			
	EMACTION_VARIATION_ELECTRIC_RESIST,	
	EMACTION_VARIATION_POISON_RESIST,		
	EMACTION_VARIATION_SPIRIT_RESIST,		

	EMACTION_VARIATION_HP_ADD,		// HP // m_nHP_ADD
	EMACTION_VARIATION_MP_ADD,		// MP // m_nMP_ADD
	EMACTION_VARIATION_SP_ADD,		// SP // m_nSP_ADD
	EMACTION_VARIATION_CP_ADD,		// CP // m_nCP_ADD

	EMACTION_VARIATION_SUM_HIT,			// m_nSUM_HIT
	EMACTION_VARIATION_SUM_AVOID,		// m_sSUM_AVOID
	EMACTION_VARIATION_DEFENSE_SKILL,	// m_nDEFENSE_SKILL

	EMACTION_VARIATION_NSIZE,
};

#define EMACTION_NRESIST (EMACTION_VARIATION_SPIRIT_RESIST - EMACTION_VARIATION_FIRE_RESIST + 1)

struct SACTIONVARIATION{
	int		m_iActionVariation[EMACTION_VARIATION_NSIZE];

	void AddVariation_AllResist(const int iAddValue){
		for ( unsigned int _i = EMACTION_NRESIST; _i; --_i )
		{
			this->m_iActionVariation[_i + EMACTION_VARIATION_FIRE_RESIST - 1] += iAddValue;
		}
	}
	const int GetElementResist(const EMELEMENT emElement) const
	{
		switch ( emElement )
		{
		case EMELEMENT_SPIRIT:
			return this->m_iActionVariation[EMACTION_VARIATION_SPIRIT_RESIST];
		case EMELEMENT_FIRE:	
			return this->m_iActionVariation[EMACTION_VARIATION_FIRE_RESIST];
		case EMELEMENT_ICE:
			return this->m_iActionVariation[EMACTION_VARIATION_ICE_RESIST];
		case EMELEMENT_ELECTRIC:
			return this->m_iActionVariation[EMACTION_VARIATION_ELECTRIC_RESIST];
		case EMELEMENT_POISON:
			return this->m_iActionVariation[EMACTION_VARIATION_POISON_RESIST];
		case EMELEMENT_STONE:
			return this->m_iActionVariation[EMACTION_VARIATION_FIRE_RESIST];
		case EMELEMENT_MAD:
			return this->m_iActionVariation[EMACTION_VARIATION_ICE_RESIST];
		case EMELEMENT_STUN:
			return this->m_iActionVariation[EMACTION_VARIATION_ELECTRIC_RESIST];
		case EMELEMENT_CURSE:
			return this->m_iActionVariation[EMACTION_VARIATION_POISON_RESIST];
		}
		return 0;
	}
	void AddVariation(const EMACTION_VARIATION emType, const int iAddValue)
	{
		this->m_iActionVariation[emType] += iAddValue;
	}
	void SetVariation(const EMACTION_VARIATION emType, const int iValue)
	{
		this->m_iActionVariation[emType] = iValue;
	}	

	SACTIONVARIATION(void)
	{
		for ( unsigned int _i = EMACTION_VARIATION_NSIZE; _i; --_i )
			this->m_iActionVariation[_i - 1] = 0;
	}
	void RESET(void)
	{
		*this = SACTIONVARIATION();
	}
};

struct SSTATEBLOW
{
    EMSTATE_BLOW	emBLOW;			
    float			fAGE;			
    float			fSTATE_VAR1;	
    float			fSTATE_VAR2;	
    WORD            wSchool;        

    //MSGPACK_DEFINE(emBLOW, fAGE, fSTATE_VAR1, fSTATE_VAR2, wSchool);

    SSTATEBLOW()
        : emBLOW(EMBLOW_NONE)
        , fAGE(0)
        , fSTATE_VAR1(0)
        , fSTATE_VAR2(0)
        , wSchool(SCHOOL_NONE)
    {
    }

    SSTATEBLOW(
        EMSTATE_BLOW _emBLOW,
        float _fAGE,
        float _fSTATE_VAR1,
        float _fSTATE_VAR2,
        WORD  _wSchool )
        : emBLOW(_emBLOW)
        , fAGE(_fAGE)
        , fSTATE_VAR1(_fSTATE_VAR1)
        , fSTATE_VAR2(_fSTATE_VAR2)
        , wSchool(_wSchool)
    {
    }

    SSTATEBLOW& operator= (const SSTATEBLOW &sblow )
    {
        emBLOW      = sblow.emBLOW;
        fAGE        = sblow.fAGE;
        fSTATE_VAR1 = sblow.fSTATE_VAR1;
        fSTATE_VAR2 = sblow.fSTATE_VAR2;
        wSchool     = sblow.wSchool;

        return *this;
    }

    void SetData(const SSTATEBLOW& StateBlow)
    {
        emBLOW      = StateBlow.emBLOW;
        fAGE        = StateBlow.fAGE;
        fSTATE_VAR1 = StateBlow.fSTATE_VAR1;
        fSTATE_VAR2 = StateBlow.fSTATE_VAR2;
        wSchool     = StateBlow.wSchool;
    }
};

struct SDROP_SKILLFACT;
struct SCHARSAVESKILLFACT;

struct SSKILLFACT
{
    enum ENSKILLFACT
    {
        FLAG_SKILLFACT_ALREADY_APPLY_INCREASE_TIME   =           0x00000001
    };

    SNATIVEID		sNATIVEID;		
    WORD			wLEVEL;			
    float			fAGE;			
    float           fLIFE;          
	bool			bIsExpiredTime;
    UINT            nEXPIREDTIME;   
    DWORD           dwFLAG;                    //< enum ENSKILLFACT

    SKILL::EMTYPES	emTYPE;			
    float			fMVAR;			

    STARGETID		sID;			

    std::vector<SKILL::SIMPACT_ADDON_LEVEL>		vecADDON;
    std::vector<SKILL::SSPEC_ADDON_LEVEL>		vecSPEC;

	SKILL::SSPEC_CA		pcCAData;

    DWORD			dwSpecialSkill;		 
    bool			bRanderSpecialEffect;     

    BOOL            bEnable;			  
	BOOL			bSkillUserEnermy;
		
    SSKILLFACT () :
    sNATIVEID(NATIVEID_NULL()),
        wLEVEL(0),
        fAGE(0),
        fLIFE(0), // 봉주
		bIsExpiredTime( false ),
        nEXPIREDTIME(0),
        dwFLAG(0),

        emTYPE(SKILL::EMFOR_VARHP),
        fMVAR(0),

        dwSpecialSkill(0),
        bRanderSpecialEffect(FALSE),
        bEnable(FALSE), bSkillUserEnermy(FALSE)
    {
    }

    BOOL IsSpecialSkill ( DWORD dwState )		 		{ return dwSpecialSkill == dwState ? TRUE : FALSE ; }
    void SetSpecialSkill ( DWORD dwState )				{ dwSpecialSkill = dwState; }

    BOOL IsAlive ();
    BOOL IsHaveExpiredTime () { return (nEXPIREDTIME) ? TRUE : FALSE; }
	BOOL IsHaveSPECADDON(EMSPEC_ADDON emSPECADDON);
    BOOL IsAgeInfinite  () { return (fAGE == FLT_MAX) ? TRUE : FALSE; }
    void SetAgeInfinite () { fAGE = FLT_MAX; }
    void SetdwFLAG( DWORD FLAG ) { dwFLAG = FLAG; };

    void RESET ();

 
    struct STIME
    {
        bool IsEXPIREDTIME;
		float fLIFE;

        union
        {
            float fAGE;
            UINT  nEXPIREDTIME;
        };

        STIME( const float _fAGE )
            : IsEXPIREDTIME(false)
            , fAGE(_fAGE)
			, fLIFE(_fAGE)
        {
        }

        STIME( const UINT _nEXPIREDTIME )
            : IsEXPIREDTIME(true)
            , nEXPIREDTIME(_nEXPIREDTIME)
			, fLIFE(_SKILLFACT_DEFAULTTIME)
        {
        }

        STIME( const SSKILLFACT& sSkillFact );
        STIME( const SDROP_SKILLFACT& sDropSkill );
        STIME( const SCHARSAVESKILLFACT& sSaveSkill );

        BOOL IsHaveExpiredTime()	const { return (IsEXPIREDTIME==true); }
        BOOL IsAgeInfinite()		const { return (fAGE == FLT_MAX); }
    };

    enum EMTIMESTRINGFORMAT
    {
        EMTIMESTRINGFORMAT_DETAIL = 0,
        EMTIMESTRINGFORMAT_SIMPLE = 1,
        EMTIMESTRINGFORMAT_SMART  = 2,
    };

    static void GetTimeByStringFormat ( CString& strDest, __time64_t tCurrentTime, const SSKILLFACT::STIME sTime, const EMTIMESTRINGFORMAT emTimeType=EMTIMESTRINGFORMAT_DETAIL );
	static bool GetTime( int& nDay, int& nHour, int& nMin, int& nSec, __time64_t tCurrentTime,
		const SSKILLFACT::STIME sTime, const EMTIMESTRINGFORMAT emTimeType = EMTIMESTRINGFORMAT_DETAIL );
};

struct SLANDEFFECT
{
    D3DXVECTOR3			vMinPos;
    D3DXVECTOR3			vMaxPos;
    EMLANDEFFECT_TYPE	emLandEffectType;
    BOOL				bCircleCheck;
    float				fLength;
    float				fValue;

    SLANDEFFECT() :
    vMinPos( 0.0f, 0.0f, 0.0f ),
        vMaxPos( 0.0f, 0.0f, 0.0f ),
        emLandEffectType(EMLANDEFFECT_ATK_SPEED),
        fValue( 0.0f ),
        fLength( 0.0f ),
        bCircleCheck(FALSE)
    {
    }

    void Init()
    {
        vMinPos			 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
        vMaxPos			 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
        emLandEffectType = EMLANDEFFECT_ATK_SPEED;
        fValue			 = 0.0f;
        fLength			 = 0.0f;
        bCircleCheck	 = FALSE;
    }

    bool IsUse()
    {
        if ( vMinPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ) return true;
        if ( vMaxPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ) return true;
        if ( emLandEffectType != EMLANDEFFECT_ATK_SPEED ) return true;
        if ( fValue	!= 0.0f ) return true;
        if ( bCircleCheck && fLength != 0.0f ) return true;

        return false;
    }

    bool IsApplyEffect( D3DXVECTOR3 vPos )
    {
        if( !IsUse() ) return false;

        if( bCircleCheck )
        {
            D3DXVECTOR2 vTemp = D3DXVECTOR2(vPos.x,vPos.z) - D3DXVECTOR2(vMinPos.x, vMinPos.z);
            if( fLength > fabs(D3DXVec2Length(&vTemp)) )
                return true;

        }else{
            if( vPos.x > vMinPos.x && vPos.z > vMinPos.z &&
                vPos.x < vMaxPos.x && vPos.z < vMaxPos.z )
                return true;
        }

        return false;
    }

    bool operator == ( const SLANDEFFECT &value )
    {
        if( vMaxPos			 != value.vMaxPos )			 return false;
        if( vMinPos			 != value.vMinPos )			 return false;
        if( emLandEffectType != value.emLandEffectType ) return false;
        if( fValue		     != value.fValue )			 return false;
        if( bCircleCheck	 != value.bCircleCheck )	 return false;
        if( fLength			 != value.fLength )			 return false;

        return true;
    }

};

typedef std::vector<SLANDEFFECT> VEC_LANDEFF;
typedef VEC_LANDEFF::iterator	 VEC_LANDEFF_ITER;


struct SLANDEFFECT_VALUE
{
    EMLANDEFFECT_TYPE	emLandEffectType;
    float				fValue;
    int                 nIndex;

    SLANDEFFECT_VALUE()
        : emLandEffectType ( EMLANDEFFECT_ATK_SPEED )
        , fValue ( 0.0f ) 
        , nIndex ( UINT_MAX ) 
    {
    }

    SLANDEFFECT_VALUE( const SLANDEFFECT& value, const int _nIndex )
    {
        ASSIGN ( value, _nIndex );
    }

    bool IsUse()
    {
        if ( emLandEffectType != EMLANDEFFECT_ATK_SPEED ) return true;
        if ( fValue != 0.0f ) return true;
        if ( nIndex != UINT_MAX ) return true;

        return false;
    }

    bool operator == ( const SLANDEFFECT_VALUE &value )
    {
        if ( emLandEffectType != value.emLandEffectType ) return false;
        if ( fValue != value.fValue ) return false;
        if ( nIndex != value.nIndex ) return false;

        return true;
    }

    void ASSIGN( const SLANDEFFECT& value, const int _nIndex )
    {
        emLandEffectType = value.emLandEffectType;
        fValue = value.fValue;
        nIndex = _nIndex;
    }
};

typedef std::vector<SLANDEFFECT_VALUE>	VEC_LANDEFF_VALUE;
typedef VEC_LANDEFF_VALUE::iterator		VEC_LANDEFF_VALUE_ITER;


struct SPASSIVE_SKILL_DATA
{
    int		m_nHP;
    int		m_nMP;
    int		m_nSP;

    int		m_nDAMAGE;
    int		m_nDEFENSE;

    int		m_nHIT;
    int		m_nAVOID;

    short	m_nPIERCE;
    float	m_fRANGE_PSY_LONG;	
    float	m_fTARRANGE_ALL;	
    float	m_fAPPLYRANGE_ALL;

    float	m_fMOVEVELO;
    float	m_fATTVELO;
    float	m_fSKILLDELAY;

    float	m_fINCR_HP;
    float	m_fINCR_MP;
    float	m_fINCR_SP;

    float	m_fDAMAGE_RATE;
    float	m_fDEFENSE_RATE;

    DAMAGE_SPEC m_sDamageSpec;

    int	m_nPA;
    int	m_nSA;
    int	m_nMA;

    float	m_fHP_RATE;
    float	m_fMP_RATE;
    float	m_fSP_RATE;

    SRESIST	m_sSUMRESIST;					

    float   m_fSUMMONACTIVETIME;            

	int		m_nCP_ADD;
	float	m_fDurationCPAdd;

    SPASSIVE_SKILL_DATA ()
		: m_nHP(0)
		, m_nMP(0)
		, m_nSP(0)
		
		, m_fINCR_HP(0)
		, m_fINCR_MP(0)
		, m_fINCR_SP(0)
		
		, m_nDAMAGE(0)
		, m_nDEFENSE(0)
		
		, m_nHIT(0)
		, m_nAVOID(0)
		
		, m_nPIERCE(0)
		, m_fRANGE_PSY_LONG(0.0f)
		
		, m_fTARRANGE_ALL(0.0f)
		, m_fAPPLYRANGE_ALL(0.0f)
		
		, m_fMOVEVELO(0.0f)
		, m_fATTVELO(0.0f)
		, m_fSKILLDELAY(0.0f)
		
		, m_fDAMAGE_RATE(0.0f)
		, m_fDEFENSE_RATE(0.0f)
		
		, m_nPA(0)
		, m_nSA(0)
		, m_nMA(0)
		
		, m_fHP_RATE(0.0f)
		, m_fMP_RATE(0.0f)
		, m_fSP_RATE(0.0f)
		
		, m_fSUMMONACTIVETIME(0.0f)
		, m_nCP_ADD(0)
		, m_fDurationCPAdd(0.0f)
    {
    }
};

//	CharClient
struct SPASSIVE_SKILL_DATA_CLIENT
{
    float	m_fMOVEVELO;
    float	m_fATTVELO;

    SPASSIVE_SKILL_DATA_CLIENT()
        : m_fMOVEVELO ( 0.0f ) 
        , m_fATTVELO ( 0.0f )
    {
    }

    SPASSIVE_SKILL_DATA_CLIENT& operator = ( const SPASSIVE_SKILL_DATA& sPassiveSkill )
    {
        m_fMOVEVELO = sPassiveSkill.m_fMOVEVELO;
        m_fATTVELO = sPassiveSkill.m_fATTVELO;

        return *this;
    }
};

struct SQITEM_FACT
{
    EMITEM_QUESTION	emType;
    float			fTime;
    WORD			wParam1;
    WORD			wParam2;

    SQITEM_FACT () :
    emType(QUESTION_NONE),
        fTime(0),
        wParam1(0),
        wParam2(0)
    {
    }

    bool IsACTIVE ()
    {
        return emType!=QUESTION_NONE;
    }

    void RESET ()
    {
        emType = QUESTION_NONE;
        fTime = 0;
        wParam1 = 0;
        wParam2 = 0;
    };
};

struct SEventState
{
    float				fItemGainRate;		
    float				fExpGainRate;		
    int					MinEventLevel;		
    int					MaxEventLevel;		
    bool				bEventStart;		
    int					EventPlayTime;		
    int					EventBusterTime;	
    CTime				EventStartTime;		
    DWORD				dwEventEndMinute;	


    SEventState()
    {
        Init();
    }
    void Init()
    {
        fItemGainRate    = 1.0f;
        fExpGainRate     = 1.0f;
        MinEventLevel    = 0;		
        MaxEventLevel    = 0;		
        bEventStart      = FALSE;			
        EventPlayTime    = 0;		
        EventBusterTime  = 0;		
        EventStartTime   = 0;
        dwEventEndMinute = 0;

    }

	void operator=(G_AUTH_EVENT_INFO& rhs)
    {
		rhs.itemGainRate = fItemGainRate;		
		rhs.expGainRate = fExpGainRate;		
		rhs.minEventLevel = MinEventLevel;		
		rhs.maxEventLevel = MaxEventLevel;		
		rhs.eventStart = bEventStart;		
		rhs.eventPlayTime = EventPlayTime;		
		rhs.eventBusterTime = EventBusterTime;	
		rhs.eventStartTime = EventStartTime;		
		rhs.eventEndMinute = dwEventEndMinute;	    
    }
};

struct SEVENT_FACT
{
    INT				nType;
    WORD			wSpeed;
    WORD			wASpeed;
    WORD			wAttack;
    bool            bIPEvent;

    SEVENT_FACT () 
        : nType(EMGM_EVENT_NONE)
        , wSpeed(0)
        , wASpeed(0)
        , wAttack(0)
        , bIPEvent(false)
    {
    }

    bool IsACTIVE( EMGM_EVENT_TYPE emType )
    {
        return (nType&emType)!=EMGM_EVENT_NONE;
    }

    void SetEVENT( EMGM_EVENT_TYPE emType, WORD wValue )
    {
        nType |= emType;

        switch( emType )
        {
        case EMGM_EVENT_SPEED:	wSpeed=wValue;	break;
        case EMGM_EVENT_ASPEED:	wASpeed=wValue;	break;
        case EMGM_EVENT_ATTACK:	wAttack=wValue;	break;
        }
    }

    void ResetEVENT( EMGM_EVENT_TYPE emType )
    {
        nType &= ~emType;
    }

    bool IsIPEvent() { return bIPEvent; }
};

struct SDROP_SKILLFACT
{
    SNATIVEID		sNATIVEID;		//	스킬 ID.
    WORD			wSLOT;			//	슬롯 위치.
    BYTE			cLEVEL;			//	스킬 LEVEL.
    bool		    IsEXPIREDTIME;  //	시간을 만료 시간으로 사용할 것인가.
    STARGETID       sTargetID;      //  스킬 걸어준 사람 ID
    union
    {
        float fAGE;			// 생성후 남은 시간.
        UINT  nEXPIREDTIME; // 만료 시간.
   
    };

    SDROP_SKILLFACT () 
        : sNATIVEID(NATIVEID_NULL())
        , wSLOT(0)
        , cLEVEL(0)
        , IsEXPIREDTIME(false)
        , fAGE(0)
    {
    }

    void Assign ( SSKILLFACT &sfact, WORD _wSLOT )
    {
        wSLOT = _wSLOT;
        sNATIVEID = sfact.sNATIVEID;
        cLEVEL = static_cast<BYTE>(sfact.wLEVEL);
        sTargetID = sfact.sID;

        if ( sfact.nEXPIREDTIME )
        {
            IsEXPIREDTIME = true;
            nEXPIREDTIME  = sfact.nEXPIREDTIME;
        }
        else
        {
            fAGE = sfact.fAGE;
        }
    }
};

struct SDROP_CHAR_BASE
{
    enum
    {
        CHAR_GEN	= 0x001,
        CLUB_CD		= 0x002
    };

    // 36
    char			szName[CHAR_SZNAME];				//(33 bytes)
    BYTE			m_bItemShopOpen;			     	//(Padding)
    WORD			wSchool;					    	//(Padding)	


    // 36
    char			szNick[CHAR_SZNAME];		//(33 bytes)
    BYTE			m_bUseArmSub;				//(Padding)
    WORD			wHair;						//Padding)

	// 36
    //char			szClubName[CHAR_SZNAME];		//(33 bytes)	
    //
    //mjeon.activity.title
    //
    BYTE			byActivityClass;			//  (Padding)
    WORD			wHairColor;					// (Padding)

	// 36
    char			szTitle[EM_TITLEID_MAXLENGTH];//XML-ID of the Activity title

	// 32
    int				nBright;					
    DWORD			dwCharID;					
    DWORD			dwGuild;					
    DWORD			dwAlliance;					
    DWORD			dwGuildMarkVer;				
    DWORD			dwGuildMaster;				
    DWORD			dwPMasterID;				
    DWORD			dwGaeaID;					

    // 32
    DWORD			dwCeID;						
    DWORD			dwActState;					
    DWORD			dwFLAGS;					
    float			m_fCDMSafeTime;             
    SNATIVEID		sMapID;						//	ID  (size: 4)
    GLPartyID		PartyID;					//  (size: 4)
    GLDWDATA		sHP;						//	(size: 8)

    // 12
    DWORD           m_dwANIMAINTYPE;            
    DWORD			m_dwANISUBTYPE;				
    float           m_fAniRemainTime;           

	// 36
    D3DXVECTOR3		vPos;						
    D3DXVECTOR3		vDir;						
    D3DXVECTOR3		vTarPos;					

    // 12
    SQITEM_FACT		sQITEMFACT;

    // 40
    SEventState		sEventState;
    
    // 8
    SPASSIVE_SKILL_DATA_CLIENT	sPASSIVE_SKILL;			//	passive skill data.    

	// 14
    WORD			Action;						
    WORD			emTribe;					
    DWORD			emClass;					
    WORD			wFace;						
    WORD			wSex;						
    WORD			wLevel;						

	// 12
    SEVENT_FACT		sEVENTFACT;
    //
    //mjeon.sns
    //
    BYTE			byFB;					// Authed for Facebook ?	(Padding)
    BYTE			byTW;					// Authed for Twitter ?		(Padding)

    //BOOL				m_bVehicle;
    //CLIENT_VEHICLE	m_sVehicle;
	BOOL	m_bGM;

	DWORD			dwCountryID;

	SDROP_CHAR_BASE () 
        : emTribe(TRIBE_HUMAN)
        , emClass(GLCC_FIGHTER_M)
        , wSchool(0)
        , wHairColor(0)
        , wSex(0)
        , wHair(0)
        , wFace(0)
        , nBright(0)
        , dwCharID(0)
        , wLevel(1)
        , dwGuild(CLUB_NULL)
        , dwGuildMarkVer(0)
        , dwGuildMaster(0)
        , dwAlliance(0)        
        , dwPMasterID(GAEAID_NULL)
        , dwGaeaID(0)
        , dwCeID(0)
        , vPos(0,0,0)
        , vDir(0,0,-1)
        , Action(GLAT_IDLE)
        , dwActState(NULL)
        , vTarPos(0,0,0)
        , dwFLAGS(NULL)
        , m_bUseArmSub(FALSE)
        , m_bItemShopOpen( false )
        , m_dwANIMAINTYPE( 0 )
        , m_dwANISUBTYPE( 0 )
        , m_fAniRemainTime( 0.0f )
        , m_fCDMSafeTime(0.0f)
        , byFB(0)
        , byTW(0)
        ,byActivityClass(0)
		, m_bGM(FALSE)
		, dwCountryID(0)
    {
        //mjeon
        //MIN_STATIC_ASSERT(sizeof(SDROP_CHAR)<=NET_DATA_BUFSIZE);
        //struct SNETDROP_PC
        memset(szName, 0, sizeof(char) * CHAR_SZNAME);
        memset(szNick, 0, sizeof(char) * CHAR_SZNAME);
        //memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
        memset(szTitle, 0, sizeof(char) * EM_TITLEID_MAXLENGTH);
    }

    SDROP_CHAR_BASE& operator = (const SDROP_CHAR_BASE& rhs)
    {
        StringCchCopy(szName, CHAR_SZNAME, rhs.szName);
        m_bItemShopOpen = rhs.m_bItemShopOpen;
        wSchool = rhs.wSchool;
        StringCchCopy(szNick, CHAR_SZNAME, rhs.szNick);
        m_bUseArmSub = rhs.m_bUseArmSub;
        wHair = rhs.wHair;
        //StringCchCopy(szClubName, CHAR_SZNAME, rhs.szClubName);
        byActivityClass = rhs.byActivityClass;
        wHairColor = rhs.wHairColor;
        StringCchCopy(szTitle, EM_TITLEID_MAXLENGTH, rhs.szTitle);
        nBright = rhs.nBright;
        dwCharID = rhs.dwCharID;
        dwGuild = rhs.dwGuild;
        dwAlliance = rhs.dwAlliance;
        dwGuildMarkVer = rhs.dwGuildMarkVer;
        dwGuildMaster = rhs.dwGuildMaster;
        PartyID = rhs.PartyID;
        dwPMasterID = rhs.dwPMasterID;
        dwGaeaID = rhs.dwGaeaID;
        dwCeID = rhs.dwCeID;
        dwActState = rhs.dwActState;
        dwFLAGS = rhs.dwFLAGS;
        m_dwANIMAINTYPE = rhs.m_dwANIMAINTYPE;
        m_dwANISUBTYPE = rhs.m_dwANISUBTYPE;
        m_fAniRemainTime = rhs.m_fAniRemainTime;
        m_fCDMSafeTime = rhs.m_fCDMSafeTime;
        sHP = rhs.sHP;
        sMapID = rhs.sMapID;
        vPos = rhs.vPos;
        vDir = rhs.vDir;
        vTarPos = rhs.vTarPos;
        sQITEMFACT = rhs.sQITEMFACT;
        sEventState = rhs.sEventState;
        sPASSIVE_SKILL = rhs.sPASSIVE_SKILL;
        Action = rhs.Action;
        emTribe = rhs.emTribe;
        emClass = rhs.emClass;
        wFace = rhs.wFace;
        wSex = rhs.wSex;
        wLevel = rhs.wLevel;
        sEVENTFACT = rhs.sEVENTFACT;
        byFB = rhs.byFB;
        byTW = rhs.byTW;
        //m_bVehicle = rhs.m_bVehicle;
        //m_sVehicle = rhs.m_sVehicle;
		m_bGM = rhs.m_bGM;
		dwCountryID = rhs.dwCountryID;
        return *this;
    }

    void SetName(const char* ChaName)
    {
        if (ChaName)
            StringCchCopy(szName, CHAR_SZNAME, ChaName);
    }

    void SetActivityTitle(const std::string& Title)
    {        
        StringCchCopy(szTitle, EM_TITLEID_MAXLENGTH, Title.c_str());
    }

//     void SetClubName(const char* ClubName)
//     {
//         if (ClubName)
//             StringCchCopy(szClubName, CHAR_SZNAME, ClubName);
//     }

    void SetNickName(const char* Nick)
    {
        if (Nick)
            StringCchCopy(szNick, CHAR_SZNAME, Nick);
    }
};

struct SDROP_CHAR
{
    SDROP_CHAR_BASE m_Base;

    SSTATEBLOW		sSTATEBLOWS[EMBLOW_MULTI];
    SDROP_SKILLFACT	sSKILLFACT[SKILLFACT_SIZE];
    SITEMCLIENT		m_PutOnItems[SLOT_NSIZE_S_2];
    
    SDROP_CHAR()
    {
    }

    SDROP_CHAR(const SDROP_CHAR_BASE& BaseData)
        : m_Base(BaseData)
    {
    }

    void SetStateBlows( const SSTATEBLOW* const pStateBlows )
    {
        for (int i=0; i<EMBLOW_MULTI; ++i)
            sSTATEBLOWS[i] = pStateBlows[i];
    }

    void SetSkillFact( const SDROP_SKILLFACT* const SkillFactData )
    {
        for (int i=0; i<SKILLFACT_SIZE; ++i)
            sSKILLFACT[i] = SkillFactData[i];
    }

    void SetPutOnItems( const SITEMCLIENT* const ItemData )
    {
        for (int i=0; i<SLOT_NSIZE_S_2; ++i)
            m_PutOnItems[i] = ItemData[i];
    }

    SDROP_CHAR& operator = (const SDROP_CHAR& rhs)
    {
        m_Base = rhs.m_Base;
        SetStateBlows( rhs.sSTATEBLOWS );
        SetSkillFact( rhs.sSKILLFACT );
        SetPutOnItems( rhs.m_PutOnItems );
        
        return *this;
    }
};

struct SCHARSAVESKILLFACT_100
{
    WORD        wSLOT;              

    SNATIVEID	sNATIVEID;			
    WORD		wLEVEL;				
    float		fAGE;				
    STARGETID   sID;				
    DWORD       dwCount;			

    SCHARSAVESKILLFACT_100(void)
		: wSLOT(0)
		, sNATIVEID(NATIVEID_NULL())
		, wLEVEL(0)
		, fAGE(0.0f)
		, sID(CROW_PC, EMTARGET_NULL)
		, dwCount(0)
    {
    }
};

struct SCHARSAVESKILLFACT_101
{
	WORD        wSLOT;             

	SNATIVEID	sNATIVEID;			
	WORD		wLEVEL;				
	bool		IsEXPIREDTIME;  

	union
	{
		float fAGE;			
		UINT  nEXPIREDTIME; 
	};
	
	STARGETID   sID;				
	DWORD       dwCount;			

	SCHARSAVESKILLFACT_101(void)
		: wSLOT(0)
		, sNATIVEID(NATIVEID_NULL())
		, IsEXPIREDTIME(false)
		, wLEVEL(0)
		, fAGE(0.0f)
		, sID(CROW_PC, EMTARGET_NULL)
		, dwCount(0)
	{
	}
};

struct SCHARSAVESKILLFACT_102
{
	WORD        wSLOT;          

	SNATIVEID	sNATIVEID;		
	BYTE		cLEVEL;			
	bool		IsEXPIREDTIME;  

	union
	{
		float fAGE;			
		UINT  nEXPIREDTIME; 
	};

	STARGETID   sID;				
	DWORD       dwCount;			
	float fVAR1;
	float fVAR2;

	SCHARSAVESKILLFACT_102(void)
		: wSLOT(0)
		, sNATIVEID(NATIVEID_NULL())
		, IsEXPIREDTIME(false)
		, cLEVEL(0)
		, fAGE(0.0f)
		, sID(CROW_PC, EMTARGET_NULL)
		, dwCount(0)
		, fVAR1(0.0f)
		, fVAR2(0.0f)
	{
	}
};

const UINT MAX_103_SIZE = 4;

struct SCHARSAVESKILLFACT_103
{
	static const DWORD VERSION;
	static const DWORD SIZE;

	WORD        wSLOT;          

	SNATIVEID	sNATIVEID;		
	BYTE		cLEVEL;			
	bool		IsEXPIREDTIME;  

	union
	{
		float fAGE;			
		UINT  nEXPIREDTIME; 
	};

	STARGETID   sID;				
	DWORD       dwCount;			

	float fLaunchTime[MAX_103_SIZE];
	float fLaunchValue[MAX_103_SIZE];

	SCHARSAVESKILLFACT_103(void)
		: wSLOT(0)
		, sNATIVEID(NATIVEID_NULL())
		, IsEXPIREDTIME(false)
		, cLEVEL(0)
		, fAGE(0.0f)
		, sID(CROW_PC, EMTARGET_NULL)
		, dwCount(0)
	{
		for ( unsigned int _n(MAX_103_SIZE); _n--; )
		{
			fLaunchTime[_n] = 0.0f;
			fLaunchValue[_n] = 0.0f;
		}
	}
};

const UINT MAX_SIZE = 10;

struct SCHARSAVESKILLFACT
{
    static const DWORD VERSION;
    static const DWORD SIZE;

    WORD        wSLOT;          

    SNATIVEID	sNATIVEID;		
    BYTE		cLEVEL;			
    bool		IsEXPIREDTIME;  

    union
    {
        float fAGE;			
        UINT  nEXPIREDTIME; 
    };

    STARGETID   sID;				
    DWORD       dwCount;			

	float fLaunchTime[MAX_SIZE];
	float fLaunchValue[MAX_SIZE];

    SCHARSAVESKILLFACT(void)
		: wSLOT(0)
		, sNATIVEID(NATIVEID_NULL())
		, IsEXPIREDTIME(false)
		, cLEVEL(0)
		, fAGE(0.0f)
		, sID(CROW_PC, EMTARGET_NULL)
		, dwCount(0)
    {
		for ( unsigned int _n(MAX_SIZE); _n--; )
		{
			fLaunchTime[_n] = 0.0f;
			fLaunchValue[_n] = 0.0f;
		}
    }

	void Reset ()
	{
		wSLOT = 0;
		sNATIVEID = NATIVEID_NULL();
		IsEXPIREDTIME = false;
		cLEVEL = 0;
		fAGE = 0.f;
		sID = STARGETID( CROW_PC, EMTARGET_NULL );
		dwCount = 0;

		for ( unsigned int _n(MAX_SIZE); _n--; )
		{
			fLaunchTime[_n] = 0.0f;
			fLaunchValue[_n] = 0.0f;
		}
	}

    void Assign ( SCHARSAVESKILLFACT_100& sSAVESKILLFACT )
    {
        wSLOT = sSAVESKILLFACT.wSLOT;

        sNATIVEID    = sSAVESKILLFACT.sNATIVEID;
        cLEVEL       = static_cast<BYTE>(sSAVESKILLFACT.wLEVEL);
        fAGE         = sSAVESKILLFACT.fAGE;
        sID          = sSAVESKILLFACT.sID;
        dwCount      = sSAVESKILLFACT.dwCount;

        IsEXPIREDTIME = false;
    }
	void Assign ( SCHARSAVESKILLFACT_101& sSAVESKILLFACT )
	{
		wSLOT = sSAVESKILLFACT.wSLOT;

		sNATIVEID    = sSAVESKILLFACT.sNATIVEID;
		cLEVEL       = static_cast<BYTE>(sSAVESKILLFACT.wLEVEL);
		fAGE         = sSAVESKILLFACT.fAGE;
		sID          = sSAVESKILLFACT.sID;
		dwCount      = sSAVESKILLFACT.dwCount;

		for ( unsigned int _n(MAX_SIZE); _n--; )
		{
			fLaunchTime[_n] = 0.0f;
			fLaunchValue[_n] = 0.0f;
		}

		IsEXPIREDTIME = false;
	}
	void Assign ( SCHARSAVESKILLFACT_102& sSAVESKILLFACT )
	{
		wSLOT = sSAVESKILLFACT.wSLOT;

		sNATIVEID    = sSAVESKILLFACT.sNATIVEID;
		cLEVEL       = static_cast<BYTE>(sSAVESKILLFACT.cLEVEL);
		fAGE         = sSAVESKILLFACT.fAGE;
		sID          = sSAVESKILLFACT.sID;
		dwCount      = sSAVESKILLFACT.dwCount;

		for ( unsigned int _n(MAX_SIZE); _n--; )
		{
			fLaunchTime[_n] = 0.0f;
			fLaunchValue[_n] = 0.0f;
		}

		fLaunchTime[0] = sSAVESKILLFACT.fVAR1;
		fLaunchValue[0] = sSAVESKILLFACT.fVAR2;

		IsEXPIREDTIME = false;
	}

	void Assign ( SCHARSAVESKILLFACT_103& sSAVESKILLFACT )
	{
		wSLOT = sSAVESKILLFACT.wSLOT;

		sNATIVEID    = sSAVESKILLFACT.sNATIVEID;
		cLEVEL       = static_cast<BYTE>(sSAVESKILLFACT.cLEVEL);
		fAGE         = sSAVESKILLFACT.fAGE;
		sID          = sSAVESKILLFACT.sID;
		dwCount      = sSAVESKILLFACT.dwCount;

		for ( unsigned int _n(MAX_SIZE); _n--; )
		{
			fLaunchTime[_n] = 0.0f;
			fLaunchValue[_n] = 0.0f;
		}

		for ( unsigned int _n(MAX_103_SIZE); _n--; )
		{
			fLaunchTime[0] = sSAVESKILLFACT.fLaunchTime[_n];
			fLaunchValue[0] = sSAVESKILLFACT.fLaunchValue[_n];
		}		

		IsEXPIREDTIME = false;
	}

    void Assign ( const WORD _wSlot, SSKILLFACT& sSKILLFACT )
    {
        wSLOT = _wSlot;

        sNATIVEID    = sSKILLFACT.sNATIVEID;
        cLEVEL       = static_cast<BYTE>(sSKILLFACT.wLEVEL);
        sID          = sSKILLFACT.sID;        

        if ( sSKILLFACT.IsHaveExpiredTime() )
        {
            IsEXPIREDTIME = true;
            nEXPIREDTIME = sSKILLFACT.nEXPIREDTIME;
        }
        else
        {
            IsEXPIREDTIME = false;
            fAGE = sSKILLFACT.fAGE;
        }		
    }

	void Assign ( const WORD _wSlot, SSKILLFACT& sSKILLFACT, const SEXPERIENCERATE& _experienceRate)
	{
		Assign(_wSlot, sSKILLFACT);		

		for ( unsigned int _n(MAX_SIZE); _n--; )
		{
			fLaunchTime[_n] = _experienceRate.getTime(_n);
			fLaunchValue[_n] = _experienceRate.getRate(_n);
		}
	}

	private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & wSLOT;
		ar & sNATIVEID;
		ar & cLEVEL;

		ar & IsEXPIREDTIME;
		ar & nEXPIREDTIME;

		ar & sID;
		ar & dwCount;

		ar & fLaunchTime[0];
		ar & fLaunchTime[1];
		ar & fLaunchTime[2];
		ar & fLaunchTime[3];
		ar & fLaunchTime[4];
		ar & fLaunchTime[5];
		ar & fLaunchTime[6];
		ar & fLaunchTime[7];
		ar & fLaunchTime[8];
		ar & fLaunchTime[9];
		
		ar & fLaunchValue[0];
		ar & fLaunchValue[1];
		ar & fLaunchValue[2];
		ar & fLaunchValue[3];
		ar & fLaunchValue[4];
		ar & fLaunchValue[5];
		ar & fLaunchValue[6];
		ar & fLaunchValue[7];
		ar & fLaunchValue[8];
		ar & fLaunchValue[9];
	}
};

struct DEFENSE_SKILL
{

    SNATIVEID	m_dwSkillID;					//	MID/SID
    WORD		m_wLevel;						
    float		m_fRate;						//	 MID/SID 
    float		m_fRateEffRate;					
    bool		m_bActive;						

    DEFENSE_SKILL() 
        : m_dwSkillID ( NATIVEID_NULL() )
        , m_wLevel ( 0 )
        , m_fRate ( 0.0f )
        , m_fRateEffRate(1.0f)
        , m_bActive(false)
    {
    }

    void RESET()
    {
        m_dwSkillID = NATIVEID_NULL();
        m_wLevel = 0;
        m_fRate = 0.0f;
        m_fRateEffRate = 1.0f;
    }
};

struct SCHARDATA
    //struct SCHARDATA : public ic::CRttiObject< SCHARDATA, ic::IPlayer, 1, ic::EInternal >
{
protected:
    DWORD				m_dwUserID; //! User Db Num

public:
    DWORD				m_dwUserLvl;				//  Level. EMUSERTYPE
    __time64_t			m_tPREMIUM;					
    bool				m_bPREMIUM;					
    __time64_t			m_tCHATBLOCK;				

    __time64_t			m_tSTORAGE[EMSTORAGE_CHANNEL_SPAN_SIZE];	
    bool				m_bSTORAGE[EMSTORAGE_CHANNEL_SPAN_SIZE];	

    WORD				m_wINVENLINE;				

    DWORD				m_dwServerID;				//ServerGroupNum
    DWORD				m_CharDbNum;				

    char				m_szName[CHAR_SZNAME];		

    EMTRIBE				m_emTribe;					
    EMCHARCLASS			m_emClass;					
    WORD				m_wSchool;					
	WORD				m_wRebornNum;				
    WORD				m_wSex;						
    WORD				m_wHair;					
    WORD				m_wHairColor;				
    WORD				m_wFace;					

    int					m_nBright;					
    int					m_nLiving;					

    WORD				m_wLevel;					
    LONGLONG			m_lnMoney;					

    bool				m_bMoneyUpdate;				
    bool				m_bStorageMoneyUpdate;		
    WORD				m_wTempLevel;				
    LONGLONG			m_lnTempMoney;				
    LONGLONG			m_lnTempStorageMoney;		

    LONGLONG			m_lVNGainSysMoney;			

    DWORD				m_ClubDbNum;				
    char				m_szNick[CHAR_SZNAME];		
    __time64_t			m_tSECEDE;					

    SCHARSTATS			m_sStats;					//	Stats.
    WORD				m_wStatsPoint;				
	
    int						m_powerAttack;				
    int						m_powerDefence;				

	int						m_powerAttribute[SKILL::EMAPPLY_NSIZE]; 

    GLLLDATA			m_sExperience;				
    LONGLONG			m_lnRestorableExp;					

    DWORD				m_dwSkillPoint;				//	Skill Point.

    LONGLONG			m_lVNGainSysExp;			

    GLDWDATA			m_sHP;						
    GLDWDATA			m_sMP;						
    GLDWDATA			m_sSP;						
    GLPADATA			m_sCP;						//  Combat Point. 

    WORD				m_wPK;						

    bool				m_bEventBuster;				

    __time64_t			m_tLoginTime;				

    int					m_EventStartLv;				
    int					m_EventEndLv;				

    int					m_RemainEventTime;			
    int					m_RemainBusterTime;			

    bool				m_bEventApply;				
    bool				m_bEventStart;				

    int					m_EventStartTime;			
    int					m_EventBusterTime;			
	float				m_EventBusterExpGain;		
	float				m_EventBusterItemGain;		

    
    LONGLONG			m_VietnamGameTime;			
    BYTE				m_dwVietnamGainType;		
    DWORD				m_dwVietnamInvenCount;	

    TCHAR				m_szPhoneNumber[SMS_RECEIVER]; 

    float				m_fCDMSafeTime;				// CDM 
	float				m_fRemainMacroTime;			

public:
	SCHARDATA ();
	virtual ~SCHARDATA() {}

    void SetUserID( DWORD dwUserID) { m_dwUserID = dwUserID; }
    DWORD GetUserID() { return m_dwUserID; }


    DWORD GetClubDbNum() const { return m_ClubDbNum; }

    VOID RegistProperty();

    SCHARDATA &GETCHARDATA ()			{ return *this; }
    EMCHARINDEX GETCHARINDEX () const	{ return CharClassToIndex ( m_emClass ); }

    bool IsKEEP_STORAGE ( DWORD dwCHANNEL );	
    CTime GetStorageTime (  DWORD dwCHANNEL );
    WORD GetOnINVENLINE ();
    void CalcPREMIUM ();

	
	inline const DWORD GetSumStats () { return m_sStats.GetSumStats() + m_wStatsPoint; }

    
    inline void SetUserLevel(DWORD UserLevel) { m_dwUserLvl=UserLevel; }
    inline DWORD UserLevel() const { return m_dwUserLvl; }
    
    inline DWORD CharDbNum() const { return m_CharDbNum; }

	inline DWORD SvrGroupNum() const { return m_dwServerID; }	//ServerGroupNum

    
    inline LONGLONG GetInvenMoney() const { return m_lnMoney; }
    void SetInvenMoney( LONGLONG llMoney );
	/*inline void InvenMoneySubtract(LONGLONG SubtractMoney)
	{
		m_lnMoney = m_lnMoney - SubtractMoney;
		if (m_lnMoney < 0)
			m_lnMoney = 0;
	}*/

    /*inline void InvenMoneyAdd(LONGLONG AddMoney)
    {
        m_lnMoney = m_lnMoney + AddMoney;
        if (m_lnMoney < 0)
            m_lnMoney = 0;
    }*/

	LONGLONG AddInvenMoney( LONGLONG llMoney );
	LONGLONG SubtractInvenMoney( LONGLONG llMoney );

	inline LONGLONG GetTempInvenMoney() const { return m_lnTempMoney; }
	void SetTempInvenMoney( LONGLONG llMoney );

    
    const char* GetName() const { return m_szName; }

    void SetName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szName, CHAR_SZNAME, szName);
    }

    void SetName(const std::string& Name)
    {
        StringCchCopy(m_szName, CHAR_SZNAME, Name.c_str());
    }

    
    void SetNickName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szNick, CHAR_SZNAME, szName);
    }

    void SetNickName(const std::string& Name)
    {        
        StringCchCopy(m_szNick, CHAR_SZNAME, Name.c_str());
    }

    const char* GetNickName() const { return m_szNick; }

    
    void SetClubSecedeTime(const __time64_t& sTime) { m_tSECEDE = sTime; }
    __time64_t GetClubSecedeTime() const { return m_tSECEDE; }


	
	void SetChaStats( SCHARSTATS &sStats ) { m_sStats = sStats; }
	void SetChaAttack( int nAttack ) { m_powerAttack = nAttack; }
	void SetChaDefence( int nDefence ) { m_powerDefence = nDefence; }	
	float GetRemainMacroTime(void){return m_fRemainMacroTime;}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_dwUserID;
		
		ar & m_dwUserLvl;
		
		ar & m_tPREMIUM;
		ar & m_bPREMIUM;
		ar & m_tCHATBLOCK;

		ar & m_tSTORAGE;
		ar & m_bSTORAGE;

		ar & m_wINVENLINE;

		ar & m_dwServerID;
		ar & m_CharDbNum;

		ar & m_szName;

		ar & m_emTribe;
		ar & m_emClass;
		ar & m_wSchool;
		ar & m_wSex;
		ar & m_wHair;
		ar & m_wHairColor;
		ar & m_wFace;

		ar & m_nBright;
		ar & m_nLiving;

		ar & m_wLevel;
		ar & m_lnMoney;

		ar & m_bMoneyUpdate;
		ar & m_bStorageMoneyUpdate;
		ar & m_wTempLevel;
		ar & m_lnTempMoney;
		ar & m_lnTempStorageMoney;

		ar & m_lVNGainSysMoney;

		ar & m_ClubDbNum;					
		ar & m_szNick;
		ar & m_tSECEDE;

		ar & m_sStats;
		ar & m_wStatsPoint;

		ar & m_powerAttack;
		ar & m_powerDefence;

		ar & m_powerAttribute;

		ar & m_sExperience;
		ar & m_lnRestorableExp;

		ar & m_dwSkillPoint;

		ar & m_lVNGainSysExp;

		ar & m_sHP;
		ar & m_sMP;
		ar & m_sSP;
		ar & m_sCP;

		ar & m_wPK;

		ar & m_bEventBuster;

		ar & m_tLoginTime;

		ar & m_EventStartLv;
		ar & m_EventEndLv;

		ar & m_RemainEventTime;
		ar & m_RemainBusterTime;

		ar & m_bEventApply;
		ar & m_bEventStart;

		ar & m_EventStartTime;
		ar & m_EventBusterTime;

		ar & m_VietnamGameTime;
		ar & m_dwVietnamGainType;
		ar & m_dwVietnamInvenCount;

		ar & m_szPhoneNumber;

		ar & m_fCDMSafeTime;
	}
};

typedef SCHARDATA* PCHARDATA;

struct SCHARSKILL
{
    static const DWORD VERSION;
    static const DWORD SIZE;

    SNATIVEID	sNativeID;
    WORD		wLevel;

    SCHARSKILL ()
		:sNativeID(SNATIVEID::ID_NULL,SNATIVEID::ID_NULL)
		,wLevel(0)
    {
    }

    SCHARSKILL ( const SNATIVEID &_sNID, const WORD _wLvl )
		:sNativeID(_sNID)
        ,wLevel(_wLvl)
    {
    }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & sNativeID;
		ar & wLevel;
	}
};

enum EMSHOP
{
    PURKEY_LENGTH = 30,
};


struct SHOPPURCHASE
{    
    WORD wItemMain;
    WORD wItemSub;
    int  m_Price;
    std::string strPurKey;

    SHOPPURCHASE()
    {
        wItemMain = 0;
        wItemSub  = 0;
        m_Price = 0;
    }

    SHOPPURCHASE(const SHOPPURCHASE& rhs)
        : strPurKey(rhs.strPurKey)
        , wItemMain(rhs.wItemMain)
        , wItemSub(rhs.wItemSub)
        , m_Price(rhs.m_Price)
    {
    }

    SHOPPURCHASE& operator= (const SHOPPURCHASE& rhs)
    {        
        wItemMain = rhs.wItemMain;
        wItemSub = rhs.wItemSub;
        m_Price = rhs.m_Price;
        strPurKey = rhs.strPurKey;
        return *this;
    }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & wItemMain;
		ar & wItemSub;
		ar & m_Price;
		ar & strPurKey;
	}
};

typedef struct _sItemSkillInfo{
	SNATIVEID	sIDSkill;

	union {
		WORD	    wLVSkill;
		WORD		bRun;
	};

	_sItemSkillInfo() : sIDSkill( NATIVEID_NULL() ), wLVSkill(0) {}
	_sItemSkillInfo( SNATIVEID id, WORD wLv ) : sIDSkill(id), wLVSkill(wLv) {}
	_sItemSkillInfo( const _sItemSkillInfo& info) { sIDSkill = info.sIDSkill; wLVSkill = info.wLVSkill; }

	void reset() { sIDSkill = NATIVEID_NULL(); wLVSkill = 0; }
}sItemSkill;

typedef std::vector<SHOPPURCHASE> VECSHOP;

typedef std::map<std::string, SHOPPURCHASE>	MAPSHOP;
typedef MAPSHOP::iterator					MAPSHOP_ITER;
typedef MAPSHOP::value_type                 MAPSHOP_VALUE;

typedef std::map<DWORD,std::string>			MAPSHOP_KEY;
typedef MAPSHOP_KEY::iterator				MAPSHOP_KEY_ITER;

//Serialization 

typedef std::map<DWORD,SCHARSKILL>			SKILL_MAP;
typedef SKILL_MAP::iterator					SKILL_MAP_ITER;
typedef SKILL_MAP::const_iterator			SKILL_MAP_CITER;

struct SCHARDATA2 : public SCHARDATA
{
    char					m_szUID[USR_ID_LENGTH+1];

	SKILL_MAP				m_ExpChangeSkills;					
	SKILL_MAP				m_ExpSkills;						
    SITEMCUSTOM				m_PutOnItems[SLOT_TSIZE];			

    WORD					m_wSKILLQUICK_ACT;					
    SNATIVEID				m_sSKILLQUICK[EMSKILLQUICK_SIZE];	
    SACTION_SLOT			m_sACTIONQUICK[EMACTIONQUICK_SIZE];	

    GLInventory				m_cInventory;						
	SINVEN_MOVE_ITEM_SORT	m_sSwapInventory[ EInvenTotalSlot ]; 

    BOOL					m_bServerStorage;					
    LONGLONG				m_lnStorageMoney;					
    BOOL					m_bStorage[EMSTORAGE_CHANNEL];		
    GLInventoryStorage		m_cStorage[EMSTORAGE_CHANNEL];		

    GLQuestPlay				m_cQuestPlay;						
	
	
	VSSETITEMINFO			m_vInfoPutOnSetItem;				

    
    MAPSHOP					m_mapCharged;						

    
    GLInventoryBank			m_cInvenCharged;					
    MAPSHOP_KEY				m_mapChargedKey;					

    
    //
    SNATIVEID			m_sStartMapID;				
    DWORD				m_dwStartGate;				
    D3DXVECTOR3			m_vStartPos;				

    SNATIVEID			m_sSaveMapID;				
    D3DXVECTOR3			m_vSavePos;					

    SNATIVEID			m_sLastCallMapID;			
    D3DXVECTOR3			m_vLastCallPos;				

    EMIP_BONUS_CLASS	m_emIPBonus;				
    SChinaTime			m_sChinaTime;				
    SEventTime			m_sEventTime;				

    SVietnamGainSystem  m_sVietnamSystem;			
    GLInventory			m_cVietnamInventory;		
    bool				m_bVietnamLevelUp;			

    
    BOOL			m_bRebuildOpen;					
	WORD			m_wRebuildType;
    SINVEN_POS		m_sRebuildItem;				
	SINVEN_POS		m_sStampItem;				
	SINVEN_POS		m_sPreInventoryItem;		
	ITEM_RANDOMOP_UTILL::FixOption	m_fixOption;			
    LONGLONG						m_i64RebuildCost;			
    LONGLONG						m_i64RebuildInput;			

    bool				m_bTracingUser;		

	
	SINVEN_POS			m_sPostItemPos;
	SITEMCUSTOM			m_sPostItem;

    
    SCHARSAVESKILLFACT m_sSAVESKILLFACT[SKILLFACT_SIZE]; 

    typedef std::map<DWORD, ITEM_COOLTIME>	COOLTIME_MAP;
    typedef COOLTIME_MAP::iterator			COOLTIME_MAP_ITER;
    typedef COOLTIME_MAP::const_iterator	COOLTIME_MAP_CITER;
    typedef COOLTIME_MAP::value_type        COOLTIME_MAP_VALUE;

    COOLTIME_MAP m_mapCoolTimeType; 
    COOLTIME_MAP m_mapCoolTimeID;   

	//
	//mjeon.CaptureTheField
	//
	int					m_nContributionPoint;		
	int					m_nRewardContribution;		

	PVP_PERFORMANCE		m_PvPPerformance;

	LONGLONG m_TexasHoldemChip;
    __time64_t m_ConfignmentSaleSlotExpansionDate;

	// Hire Summon 
	
	SNATIVEID m_sBasicSummonID;		
	SSUMMONABLE	m_sSummonable;		

	
	WORD m_wSlotIndex;
	bool m_arrLockSlotState[ EMCHAR_SLOT_DATA_SIZE ];

	
	SINVENITEM m_sBackUpRebuildInvenItem;

    SCHARDATA2();
	virtual ~SCHARDATA2() {}
    void Assign ( SCHARDATA2 &CharData );	

    EMCHARINDEX GETCHARINDEX () const { return CharClassToIndex ( m_emClass ); }

	
	BOOL ValidationCharData();
	BOOL ValidationCharStateMin(WORD *value);
	BOOL ValidationCharValueMin(int *value);
	BOOL ValidationCharStateMax(WORD *value);
	BOOL ValidationCharValueMax(int *value);

	BOOL ValidatePutOnTurnNum();
	BOOL ValidateVehicleRandomOption();
	BOOL ValidateRandomOption( std::vector< SITEMCUSTOM >& vecDeleteItems );

	DWORD		GetExpSkillsDataSize();
	BOOL		GetExpSkillsData( BYTE* pDest, const DWORD dwDestSize );

    BOOL SETEXPSKILLS_BYBUF ( se::ByteStream &ByteStream );
    BOOL GETEXPSKILLS_BYBUF ( se::ByteStream &ByteStream ) const;

	DWORD		GetPutOnItemsDataSize();
	BOOL		GetPutOnItemsData( BYTE* pDest, const DWORD dwDestSize );
    BOOL GETPUTONITEMS_BYBUF ( se::ByteStream &ByteStream ) const;

	DWORD		GetSkillQuickSlotDataSize();
	BOOL		GetSkillQuickSlotData( BYTE* pDest, const DWORD dwDestSize );

    BOOL SETSKILL_QUICKSLOT ( se::ByteStream &ByteStream );
    BOOL GETSKILL_QUICKSLOT ( se::ByteStream &ByteStream ) const;

	DWORD		GetActionQuickSlotDataSize();
	BOOL		GetActionQuickSlotData( BYTE* pDest, const DWORD dwDestSize );
    BOOL SETACTION_QUICKSLOT ( se::ByteStream &ByteStream );
    BOOL GETACTION_QUICKSLOT ( se::ByteStream &ByteStream ) const;

	DWORD		GetInventoryDataSize();
	BOOL		GetInventoryData( BYTE* pDest, const DWORD dwDestSize );
    BOOL SETINVENTORY_BYBUF ( se::ByteStream &ByteStream );
    BOOL GETINVENTORYE_BYBUF ( se::ByteStream &ByteStream ) const;

	BOOL SETINVENTORY_BYBUF_FOR_MIGRATION( se::ByteStream &ByteStream );

	DWORD		GetStorageDataSize();
	BOOL		GetStorageData( BYTE* pDest, const DWORD dwDestSize );
    BOOL SETSTORAGE_BYBUF ( se::ByteStream &ByteStream );
    BOOL GETSTORAGE_BYBUF ( se::ByteStream &ByteStream ) const;

    
	DWORD		GetQuestPlayDataSize();
	BOOL		GetQuestPlayData( BYTE* pDest, const DWORD dwDestSize );
    BOOL		SETQUESTPLAY(se::ByteStream& ByteStream, bool bCheck);
    BOOL		GETQUESTPLAY(se::ByteStream& ByteStream) const;

	BOOL SETQUESTPLAY_FOR_MIGRATION( se::ByteStream& ByteStream, bool bCheck );

	BOOL SETQUESTPLAY_NEW( 
		se::ByteStream& ByteStream, 
		bool bCheck, 
		std::multimap< DWORD, SINVENITEM_SAVE >& mapProcItems, 
		std::multimap< DWORD, SINVENITEM_SAVE >& mapEndItems, 
		std::vector< sc::MGUID >& vecDeleteItems );

    BOOL SETSHOPPURCHASE ( VECSHOP &vecSHOP );

    
    DWORD		GetVTAddInventoryDataSize();
	BOOL		GetVTAddInventoryData( BYTE* pDest, const DWORD dwDestSize );
	BOOL SETVTADDINVENTORY_BYBUF ( se::ByteStream &ByteStream );
    BOOL GETVTADDINVENTORYE_BYBUF ( se::ByteStream &ByteStream ) const;

	DWORD		GetItemCoolTimeDataSize();
	BOOL		GetItemCoolTimeData( BYTE* pDest, const DWORD dwDestSize );
    BOOL SETITEMCOOLTIME_BYBUF( se::ByteStream &ByteStream ); 
    BOOL GETITEMCOOLTIME_BYBUF( se::ByteStream &ByteStream ) const; 

	DWORD		GetSkillFactNum();
	DWORD		GetSkillFactDataSize();
	BOOL		GetSkillFactData( BYTE* pDest, const DWORD dwDestSize );
    BOOL SETSKILLFACT_BYBUF( se::ByteStream &ByteStream ); 
    BOOL GETSKILLFACT_BYBUF( se::ByteStream &ByteStream ) const; 

	
	inline LONGLONG GetStorageMoney() const { return m_lnStorageMoney; }
	void SetStorageMoney( LONGLONG llMoney );
	void AddStorageMoney( LONGLONG llMoney );
	void SubtractStorageMoney( LONGLONG llMoney );

	inline LONGLONG GetTempStorageMoney() const { return m_lnTempStorageMoney; }
	void SetTempStorageMoney( LONGLONG llMoney );
	void ClearSwapInventory();

	void SETPUTONITEM( EMSLOT emSlot, const SITEMCUSTOM& sItem );
	BOOL SETINVENTORY_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );
	BOOL SETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );
	BOOL GETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );
	BOOL GETPUTONITEMS_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );

public:
    BOOL LOADFILE(const std::string& FileName);

public:
    
    BOOL ADDSHOPPURCHASE ( const char* szPurKey, SNATIVEID nidITEM );
    BOOL DELSHOPPURCHASE ( const DWORD dwID );

public:	// ITEMREBUILD_MARK
    SITEMCUSTOM GET_REBUILD_ITEM() const;
	SITEMCUSTOM GET_REBUILD_ITEM_BACKUP() const;
	SITEMCUSTOM GET_STAMP_ITEM() const;
    SITEMCUSTOM GET_PREHOLD_ITEM() const;	
	void RESET_FIX_OPTION();
	void PUSH_FIX_OPTION(const BYTE cOptType, const DWORD nFixMaximum);
	const BYTE GET_FIX_OPTION_TYPE ( const UINT _nIndex ) const;
	const DWORD GetNFixOption(void) const;
	const SINVEN_POS& GetPreHoldItemPos() const { return m_sPreInventoryItem; }

public:	// ITEMREBUILD_MARK
    VOID InitRebuildData();
    VOID OpenRebuild() { m_bRebuildOpen = TRUE; }
    VOID CloseRebuild() { m_bRebuildOpen = FALSE; }
    const BOOL ValidRebuildOpen() { return m_bRebuildOpen; }
    const LONGLONG GetRebuildCost() { return m_i64RebuildCost; }
    const LONGLONG GetRebuildInput() { return m_i64RebuildInput; }

    void SetUserName(const char* szUserId)
    {
        if (szUserId)
            StringCchCopy(m_szUID, USR_ID_LENGTH+1, szUserId);
    }

public:
	void InitCostumStatData();

public:
	// TexasHoldem
	inline LONGLONG GetTexasHoldemChip() const { return m_TexasHoldemChip; }
	void SetTexasHoldemChip( LONGLONG chip );
	void AddTexasHoldemChip( LONGLONG chip );
	void SubtractTexasHoldemChip( LONGLONG chip );

public:
    // ConsignmentSale
    inline __time64_t GetConsignmentSaleSlotExpansionDate() const { return m_ConfignmentSaleSlotExpansionDate; }
    void SetConsignmentSaleSlotExpansionDate( __time64_t tTime );

//private:
public:
	VOID SetPostItemPos( const SINVEN_POS& sPos ) { m_sPostItemPos = sPos; }
	VOID SetPostItem( const SITEMCUSTOM& sItem ) { m_sPostItem = sItem; }
	const SINVEN_POS& GetPostItemPos() { return m_sPostItemPos; }
	const SITEMCUSTOM& GetPostItem() { return m_sPostItem; }

public:
	SCHARDATA2( SCHARDATA2& data )		{ Assign(data); }//GASSERT
	SCHARDATA2& operator= ( SCHARDATA2& data )	{ Assign(data); return *this; }//GASSERT; return *this; }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SCHARDATA>(*this);

		ar & m_szUID;

		ar & m_ExpSkills;
		ar & m_PutOnItems;

		ar & m_wSKILLQUICK_ACT;
		ar & m_sSKILLQUICK;
		ar & m_sACTIONQUICK;

		ar & m_cInventory;

		ar & m_bServerStorage;
		ar & m_lnStorageMoney;
		ar & m_bStorage;
		ar & m_cStorage;

		ar & m_cQuestPlay;

		ar & m_mapCharged;

		ar & m_cInvenCharged;
		ar & m_mapChargedKey;

		ar & m_sStartMapID;		
		ar & m_dwStartGate;		
		ar & m_vStartPos.x;
		ar & m_vStartPos.y;
		ar & m_vStartPos.z;

		ar & m_sSaveMapID;		
		ar & m_vSavePos.x;
		ar & m_vSavePos.y;
		ar & m_vSavePos.z;

		ar & m_sLastCallMapID;
		ar & m_vLastCallPos.x;
		ar & m_vLastCallPos.y;
		ar & m_vLastCallPos.z;

		ar & m_emIPBonus;
		ar & m_sChinaTime;
		ar & m_sEventTime;

		ar & m_sVietnamSystem;
		ar & m_cVietnamInventory;
		ar & m_bVietnamLevelUp;

		ar & m_bRebuildOpen;
		ar & m_sRebuildItem;
		ar & m_sStampItem;
		ar & m_sPreInventoryItem;
		ar & m_i64RebuildCost;
		ar & m_i64RebuildInput;

		ar & m_bTracingUser;
		
		ar & m_sSAVESKILLFACT;

		ar & m_mapCoolTimeType;
		ar & m_mapCoolTimeID;

		ar & m_nContributionPoint;
		ar & m_nRewardContribution;

		ar & m_PvPPerformance;

		ar & m_TexasHoldemChip;
        ar & m_ConfignmentSaleSlotExpansionDate;

		ar & m_sBasicSummonID;
		ar & m_sSummonable;

		ar & m_wSlotIndex;
		ar & m_arrLockSlotState;

		ar & m_sBackUpRebuildInvenItem;
	}
};
typedef SCHARDATA2* PCHARDATA2;


#endif // GLCHARDATA_H_
