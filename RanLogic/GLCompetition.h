#pragma once

#include "./Character/GLCharDefine.h"
#include "./Character/GLCharData.h"
#include "./GLogicData.h"

#include <vector>

namespace PVP
{
    enum ENUM_PVP_TYPE
    {
        ENUM_PVP_TYPE_CDM           = 0, // (unused)
        ENUM_PVP_TYPE_ACADEMYCONFT  = 1, // (unused)
        ENUM_PVP_TYPE_CTF           = 2
    };

    enum ENUM_CTF_TYPE
    {
        ENUM_CTF_TYPE_NULL         = 0,
        ENUM_CTF_TYPE_POWERSTATION = 1,
    };

	enum EMTEAM
	{
		EMTEAM_SM = 0x0001,
		EMTEAM_HA = 0x0002,
		EMTEAM_BH = 0x0004,
	};

	enum ENUM_REWARD_TYPE
	{
		ENUM_REWARD_TYPE_CONTRIBUTION_PLAYER = 0x0001,
		ENUM_REWARD_TYPE_CONTRIBUTION_SCHOOL = 0x0002,
		ENUM_REWARD_TYPE_BUFF_PLAYER			= 0x0004,
		ENUM_REWARD_TYPE_BUFF_SCHOOL			= 0x0008,
	};

	struct CTF_RANKER_BASIC
	{	
		WORD		wSchool;
		DWORD		wClass;
		char		szName[CHAR_SZNAME];

		CTF_RANKER_BASIC()
			:wSchool(SCHOOL_NONE)
			,wClass(GLCI_NUM_ACTOR)
		{
			ZeroMemory(szName, sizeof(szName));
		}

		void SetName(char *_szName)
		{
			GASSERT(_szName);

			StringCchCopy(szName, CHAR_SZNAME, _szName);
		}
	};


	struct CTF_RANKER: public CTF_RANKER_BASIC
	{
		PVP_PERFORMANCE pvpperf;

		CTF_RANKER(const PVP_PERFORMANCE &_pvpperf)
			:pvpperf(_pvpperf)
		{
		}

		bool operator < (const CTF_RANKER& rhs) const
		{
			return (pvpperf < rhs.pvpperf);
		}
	};


	typedef std::vector<CTF_RANKER_BASIC>	VEC_CTF_RANKER;
	typedef VEC_CTF_RANKER::iterator		VEC_CTF_RANKER_ITER;

    struct CTF_RANKER_CLUB_LOG
    {        
        DWORD m_ClubDbNum;
        char  m_szName[CHAR_SZNAME];
        PVP_PERFORMANCE m_KillDeath;

        CTF_RANKER_CLUB_LOG(const PVP_PERFORMANCE& KillDeath, DWORD ClubDbNum, const char* szName)
            : m_KillDeath(KillDeath)
            , m_ClubDbNum(ClubDbNum)
        {
            if (szName)
                StringCchCopy(m_szName, CHAR_SZNAME, szName);
        }

        bool operator < (const CTF_RANKER_CLUB_LOG& rhs) const
        {
            return (m_KillDeath < rhs.m_KillDeath);
        }
    };

	//
	// history of previous CTF
	//
	struct CTF_HISTORY
	{
		__time32_t				tStartTime;
		__time32_t				tEndTime;					//실제 CTF가 종료한 시간

		WORD					wSchoolAuthed;				//Winner
		WORD					wReserved;

		std::vector<PVP::CTF_RANKER_BASIC>	vecRanking;	//index is the ranking
		std::vector<CTF_REWARD_BUFF>		vecRewardBuffs;

		CTF_HISTORY()
			:tStartTime(0)
			,tEndTime(0)
			,wSchoolAuthed(SCHOOL_NONE)
			,wReserved(0)
		{			
		}

		void Reset()
		{
			tStartTime = 0;
			tEndTime = 0;
			wSchoolAuthed = SCHOOL_NONE;
			wReserved = 0;

			vecRanking.clear();
			vecRewardBuffs.clear();
		}
	};


	struct SCTFRESULTDATA
	{
		SCTFRESULTDATA()
			: emSCHOOL(SCHOOL_NONE)
			, emCLASS(GLCI_NUM_ACTOR)
			, nDAMAGE(0)
			, nHEAL(0)
			, nKILL(0)
			, nDEATH(0)
			, nREBIRTH(0)
			, nCONTRIBUTION(0)
		{
		}

		ENUM_SCHOOL emSCHOOL;
		EMCHARINDEX emCLASS;
		std::string strNAME;
		UINT		nDAMAGE;
		UINT		nHEAL;
		UINT		nKILL;
		UINT		nDEATH;
		UINT		nREBIRTH;
		UINT		nCONTRIBUTION;

		const bool operator < ( const SCTFRESULTDATA& rhs ) const
		{
			PVP_PERFORMANCE lhsPF;
			lhsPF.nContributionPoint	= this->nCONTRIBUTION;
			lhsPF.nDamageTo				= this->nDAMAGE;
			lhsPF.nHealTo				= this->nHEAL;
			lhsPF.nKill					= this->nKILL;
			lhsPF.nRebirthTo			= this->nREBIRTH;

			PVP_PERFORMANCE rhsPF;
			rhsPF.nContributionPoint = rhs.nCONTRIBUTION;
			rhsPF.nDamageTo			 = rhs.nDAMAGE;
			rhsPF.nHealTo			 = rhs.nHEAL;
			rhsPF.nKill				 = rhs.nKILL;
			rhsPF.nRebirthTo		 = rhs.nREBIRTH;

			return (lhsPF < rhsPF);
		}

		const bool operator == ( const SCTFRESULTDATA& rhs ) const
		{
			return strNAME == rhs.strNAME;
		}

		void	   Reset() { strNAME.clear(); }
		const bool IsNull() const { return strNAME.empty(); }
	};

	typedef std::vector<SCTFRESULTDATA*>	VEC_RESULTDATA;
	typedef VEC_RESULTDATA::iterator		VEC_RESULTDATA_ITER;

} // namespace PVP

enum ENUM_CTF_STATE
{
    CTF_STATE_STOP		= 0,	//	시작하지 않음
    CTF_STATE_READY		= 1,	//	준비
    CTF_STATE_START		= 2,	//	시작
    CTF_STATE_DONE  	= 3,	//	점령완료
    CTF_STATE_REWARD	= 4,	//	보상연산
    //CTF_END			= 4,	//	종료
};
